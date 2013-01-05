/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkGnuplotExecuter.h"
#include "vtkHTMLGenerator.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMetaImageReader.h"
#include "vtkMetaImageWriter.h"
#include "vtkMultiThreader.h" 
#include "vtkObjectFactory.h"
#include "vtkPlusDevice.h"
#include "vtkPlusStream.h"
#include "vtkPlusStreamBuffer.h"
#include "vtkPlusStreamTool.h"
#include "vtkWindows.h"
#include "vtkRecursiveCriticalSection.h"
#include "vtkRfProcessor.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTrackedFrameList.h"
#include "vtksys/SystemTools.hxx"
#include <ctype.h>
#include <time.h>

#if ( _MSC_VER >= 1300 ) // Visual studio .NET
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4312 )
#endif

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkPlusDevice, "$Revision: 1.1$");
vtkStandardNewMacro(vtkPlusDevice);

// If a frame cannot be retrieved from the device buffers (because it was overwritten by new frames)
// then we skip a SAMPLING_SKIPPING_MARGIN_SEC long period to allow the application to catch up
static const double SAMPLING_SKIPPING_MARGIN_SEC=0.1; 
const int vtkPlusDevice::VIRTUAL_DEVICE_FRAME_RATE = 50;
static const int FRAME_RATE_AVERAGING = 10;

//----------------------------------------------------------------------------
vtkPlusDevice::vtkPlusDevice()
: ThreadAlive(false)
, Connected(0)
, ThreadId(-1)
, CurrentStream(NULL)
, CurrentStreamBufferItem(new StreamBufferItem())
, ToolReferenceFrameName(NULL)
, DeviceId(NULL)
, DeviceImageOrientation(US_IMG_ORIENT_XX)
, AcquisitionRate(30)
, Recording(0)
, SaveRfProcessingParameters(false)
, RfProcessor(vtkRfProcessor::New())
, BlankImage(vtkImageData::New())
, DesiredTimestamp(-1)
, UpdateWithDesiredTimestamp(0)
, TimestampClosestToDesired(-1)
, FrameNumber(0)
, FrameTimeStamp(0)
, NumberOfOutputFrames(1)
, OutputNeedsInitialization(1)
, Selectable(false)
, RequireDeviceImageOrientationInDeviceSetConfiguration(false)
, RequireFrameBufferSizeInDeviceSetConfiguration(false)
, RequireAcquisitionRateInDeviceSetConfiguration(false)
, RequireAveragedItemsForFilteringInDeviceSetConfiguration(false)
, RequireToolAveragedItemsForFilteringInDeviceSetConfiguration(false)
, RequireLocalTimeOffsetSecInDeviceSetConfiguration(false)
, RequireUsImageOrientationInDeviceSetConfiguration(false)
, RequireRfElementInDeviceSetConfiguration(false)
{
  this->SetNumberOfInputPorts(0);

  this->SetToolReferenceFrameName("Tracker");

  // Default size for brightness frame
  this->BrightnessFrameSize[0]=640;
  this->BrightnessFrameSize[1]=480;

  // Create a blank image, it will be used as output if frames are not available
  this->BlankImage->SetExtent( 0, this->BrightnessFrameSize[0] -1, 0, this->BrightnessFrameSize[1] - 1, 0, 0);
  this->BlankImage->SetScalarTypeToUnsignedChar();
  this->BlankImage->SetNumberOfScalarComponents(1); 
  this->BlankImage->AllocateScalars(); 
  unsigned long memorysize = this->BrightnessFrameSize[0]*this->BrightnessFrameSize[1]*this->BlankImage->GetScalarSize(); 
  memset(this->BlankImage->GetScalarPointer(), 0, memorysize);

  this->Threader = vtkMultiThreader::New();

  // For threaded capture of transformations
  this->UpdateMutex = vtkRecursiveCriticalSection::New();
}

//----------------------------------------------------------------------------
vtkPlusDevice::~vtkPlusDevice()
{
  // we certainly don't want to access a virtual
  // function after the subclass has destructed!!
  if (this->Connected)
  {
    Disconnect();
  }

  this->OutputStreams.clear();
  this->Tools.clear();

  delete CurrentStreamBufferItem; CurrentStreamBufferItem = NULL;

  DELETE_IF_NOT_NULL(this->BlankImage);

  DELETE_IF_NOT_NULL(this->RfProcessor);

  DELETE_IF_NOT_NULL(this->Threader);

  DELETE_IF_NOT_NULL(this->UpdateMutex);
}

//----------------------------------------------------------------------------
void vtkPlusDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Connected: " << (this->Connected ? "Yes\n" : "No\n");
  os << indent << "SDK version: " << this->GetSdkVersion() << "\n";
  os << indent << "AcquisitionRate: " << this->AcquisitionRate << "\n";
  os << indent << "Recording: " << (this->Recording ? "On\n" : "Off\n");
  os << indent << "NumberOfOutputFrames: " << this->NumberOfOutputFrames << "\n";
  
  for( StreamContainerConstIterator it = this->OutputStreams.begin(); it != this->OutputStreams.end(); ++it )
  {
    vtkPlusStream* str = *it;
    str->PrintSelf(os, indent);
  }

  if (this->ToolReferenceFrameName)
  {
    os << indent << "ToolReferenceFrameName: " << this->ToolReferenceFrameName << "\n";
  }

  for( ToolContainerConstIterator it = this->Tools.begin(); it != this->Tools.end(); ++it )
  {
    vtkPlusStreamTool* tool = it->second;
    tool->PrintSelf(os, indent);
  }
}

//----------------------------------------------------------------------------
std::string vtkPlusDevice::GetSdkVersion()
{
  // Base class version is the same as the Plus library version
  std::string ver = std::string("Plus-") + std::string(PLUSLIB_VERSION); 
  return ver; 
}

//----------------------------------------------------------------------------
bool vtkPlusDevice::IsResettable()
{
  // By default, devices cannot be reset.
  return false;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::Reset()
{
  // By default, do nothing
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusDevice::SetToolLED(const char* portName, int led, int state)
{
  PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->UpdateMutex);
  this->InternalSetToolLED(portName, led, state);
}

//----------------------------------------------------------------------------
ToolContainerConstIterator vtkPlusDevice::GetToolIteratorBegin() const
{
  return this->Tools.begin(); 
}

//----------------------------------------------------------------------------
ToolContainerConstIterator vtkPlusDevice::GetToolIteratorEnd() const
{
  return this->Tools.end();
}

//----------------------------------------------------------------------------
int vtkPlusDevice::GetNumberOfTools() const
{
  return this->Tools.size(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::AddTool( vtkPlusStreamTool* tool )
{
  if ( tool == NULL )
  {
    LOG_ERROR("Failed to add tool to tracker, tool is NULL!"); 
    return PLUS_FAIL; 
  }

  if ( tool->GetToolName() == NULL || tool->GetPortName() == NULL )
  {
    LOG_ERROR("Failed to add tool to tracker, tool Name and PortName must be defined!"); 
    return PLUS_FAIL; 
  }

  if ( this->Tools.find( tool->GetToolName() ) == this->GetToolIteratorEnd() )
  {
    // Check tool port names, it should be unique too
    for ( ToolContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
    {
      if ( STRCASECMP( tool->GetPortName(), it->second->GetPortName() ) == 0 )
      {
        LOG_ERROR("Failed to add '" << tool->GetToolName() << "' tool to container: tool with name '" << it->second->GetToolName() 
          << "' is already defined on port '" << tool->GetPortName() << "'!"); 
        return PLUS_FAIL; 
      }
    }

    tool->Register(this); 
    tool->SetDevice(this); 
    this->Tools[tool->GetToolName()] = tool; 
  }
  else
  {
    LOG_ERROR("Tool with name '" << tool->GetToolName() << "' is already in the tool conatainer!"); 
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetFirstActiveTool(vtkPlusStreamTool*& aTool)
{
  if( this->CurrentStream == NULL )
  {
    LOG_ERROR("Current stream is null. Unable to get first active tool.");
    return PLUS_FAIL;
  }

  if ( this->CurrentStream->GetToolBuffersStartConstIterator() == this->CurrentStream->GetToolBuffersEndConstIterator() )
  {
    LOG_ERROR("Failed to get first active tool - there is no active tool!"); 
    return PLUS_FAIL; 
  }

  // Get the first tool
  aTool = this->CurrentStream->GetToolBuffersStartIterator()->second; 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetTool(const char* aToolName, vtkPlusStreamTool*& aTool)
{
  if ( aToolName == NULL )
  {
    LOG_ERROR("Failed to get tool, tool name is NULL"); 
    return PLUS_FAIL; 
  }

  if( this->Tools.find(aToolName) != this->Tools.end() )
  {
    aTool = this->Tools.find(aToolName)->second;
    return PLUS_SUCCESS;
  }

  if( CurrentStream == NULL )
  {
    LOG_ERROR("Failed to get tool, CurrentStream is NULL"); 
    return PLUS_FAIL;
  }

  if( this->CurrentStream->GetTool(aTool, aToolName) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to find tool '"<< aToolName <<"' in the list, please check the configuration file first." ); 
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetToolByPortName( const char* portName, vtkPlusStreamTool*& aTool)
{
  if ( portName == NULL )
  {
    LOG_ERROR("Failed to get tool - port name is NULL!"); 
    return PLUS_FAIL; 
  }

  for ( ToolContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    if ( STRCASECMP( portName, it->second->GetPortName() ) == 0 )
    {
      aTool = it->second; 
      return PLUS_SUCCESS; 
    }
  }

  if( CurrentStream == NULL )
  {
    LOG_ERROR("Failed to get tool, CurrentStream is NULL"); 
    return PLUS_FAIL;
  }

  for ( ToolContainerConstIterator it = this->CurrentStream->GetToolBuffersStartConstIterator(); it != this->CurrentStream->GetToolBuffersEndConstIterator(); ++it)
  {
    if ( STRCASECMP( portName, it->second->GetPortName() ) == 0 )
    {
      aTool = it->second; 
      return PLUS_SUCCESS; 
    }
  }

  return PLUS_FAIL; 
}

//-----------------------------------------------------------------------------
void vtkPlusDevice::SetToolsBufferSize( int aBufferSize )
{
  LOG_TRACE("vtkPlusDevice::SetToolsBufferSize(" << aBufferSize << ")" ); 
  for ( ToolContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    it->second->GetBuffer()->SetBufferSize( aBufferSize ); 
  }
}

//----------------------------------------------------------------------------
void vtkPlusDevice::SetLocalTimeOffsetSec( double aTimeOffsetSec )
{
  for( StreamContainerIterator it = this->OutputStreams.begin(); it != this->OutputStreams.end(); ++it)
  {
    vtkPlusStream* aStream = *it;
    for( StreamBufferMapContainerConstIterator bufIter = aStream->GetBuffersStartConstIterator(); bufIter != aStream->GetBuffersEndConstIterator(); ++bufIter)
    {
      vtkPlusStreamBuffer* aBuff = bufIter->second;
      aBuff->SetLocalTimeOffsetSec(aTimeOffsetSec);
    }
  }
}

//----------------------------------------------------------------------------
void vtkPlusDevice::SetToolLocalTimeOffsetSec( double aTimeOffsetSec )
{
  for( ToolContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it )
  {
    vtkPlusStreamTool* tool = it->second;
    tool->GetBuffer()->SetLocalTimeOffsetSec(aTimeOffsetSec);
  }
}

//-----------------------------------------------------------------------------
void vtkPlusDevice::DeepCopy(vtkPlusDevice* device)
{
  LOG_TRACE("vtkPlusDevice::DeepCopy"); 
  for ( ToolContainerConstIterator it = device->Tools.begin(); it != device->Tools.end(); ++it )
  {
    LOG_DEBUG("Copy the buffer of tracker tool: " << it->first ); 
    if ( this->AddTool(it->second) != PLUS_SUCCESS )
    {
      LOG_ERROR("Copy of tool '" << it->first << "' failed - unabale to add tool to the container!"); 
      continue; 
    }

    vtkPlusStreamTool* tool = NULL; 
    if ( this->GetTool(it->first.c_str(), tool ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Copy of tool '" << it->first << "' failed - unabale to get tool from container!"); 
      continue;   
    }

    tool->DeepCopy( it->second ); 
  }

  this->InternalUpdateRate = device->GetInternalUpdateRate();
  this->SetAcquisitionRate(device->GetAcquisitionRate()); 
}

//-----------------------------------------------------------------------------
double vtkPlusDevice::GetInternalUpdateRate() const
{
  return this->InternalUpdateRate;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::SetAcquisitionRate( double aRate )
{
  LOG_TRACE("vtkPlusDevice::SetAcquisitionRate(" << aRate << ")");

  if (this->AcquisitionRate == aRate)
  {
    return PLUS_SUCCESS;
  }

  this->AcquisitionRate = aRate;
  this->Modified();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusDevice::Beep(int n)
{
  PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->UpdateMutex);
  this->InternalBeep(n);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::WriteToMetafile( const char* outputFolder, const char* metaFileName, bool useCompression /*= false*/ )
{
  LOG_TRACE("vtkPlusDevice::WriteToMetafile: " << outputFolder << "/" << metaFileName); 

  if ( this->GetNumberOfTools() == 0 )
  {
    LOG_ERROR("Failed to write tracker to metafile - there are no active tools!"); 
    return PLUS_FAIL; 
  }

  // Get the number of items from buffers and use the lowest
  int numberOfItems(-1); 
  for ( ToolContainerConstIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    if ( numberOfItems < 0 || numberOfItems > it->second->GetBuffer()->GetNumberOfItems() )
    {
      numberOfItems = it->second->GetBuffer()->GetNumberOfItems(); 
    }
  }

  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 

  PlusStatus status=PLUS_SUCCESS;

  // Get the first tool
  vtkPlusStreamTool* firstActiveTool = this->Tools.begin()->second; 

  for ( int i = 0 ; i < numberOfItems; i++ ) 
  {
    //Create fake image 
    typedef itk::Image<unsigned char, 2> ImageType;
    ImageType::Pointer frame = ImageType::New(); 
    ImageType::SizeType size={{1,1}};
    ImageType::IndexType start={{0,0}};
    ImageType::RegionType region;
    region.SetSize(size);
    region.SetIndex(start);
    frame->SetRegions(region);

    try
    {
      frame->Allocate();
    }
    catch (itk::ExceptionObject & err) 
    {		
      LOG_ERROR("Unable to allocate memory for image: " << err.GetDescription() );
      status=PLUS_FAIL;
      continue; 
    }	

    TrackedFrame trackedFrame; 
    trackedFrame.GetImageData()->SetITKImageBase(frame);

    StreamBufferItem bufferItem; 
    BufferItemUidType uid = firstActiveTool->GetBuffer()->GetOldestItemUidInBuffer() + i; 

    if ( firstActiveTool->GetBuffer()->GetStreamBufferItem(uid, &bufferItem) != ITEM_OK )
    {
      LOG_ERROR("Failed to get tracker buffer item with UID: " << uid ); 
      continue; 
    }

    const double frameTimestamp = bufferItem.GetFilteredTimestamp(firstActiveTool->GetBuffer()->GetLocalTimeOffsetSec()); 

    // Add main tool timestamp
    std::ostringstream timestampFieldValue; 
    timestampFieldValue << std::fixed << frameTimestamp; 
    trackedFrame.SetCustomFrameField("Timestamp", timestampFieldValue.str()); 

    // Add main tool unfiltered timestamp
    std::ostringstream unfilteredtimestampFieldValue; 
    unfilteredtimestampFieldValue << std::fixed << bufferItem.GetUnfilteredTimestamp(firstActiveTool->GetBuffer()->GetLocalTimeOffsetSec()); 
    trackedFrame.SetCustomFrameField("UnfilteredTimestamp", unfilteredtimestampFieldValue.str()); 

    // Add main tool frameNumber
    std::ostringstream frameNumberFieldValue; 
    frameNumberFieldValue << std::fixed << bufferItem.GetIndex(); 
    trackedFrame.SetCustomFrameField("FrameNumber", frameNumberFieldValue.str()); 

    // Add transforms
    for ( ToolContainerConstIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
    {
      StreamBufferItem toolBufferItem; 
      if ( it->second->GetBuffer()->GetStreamBufferItemFromTime( frameTimestamp, &toolBufferItem, vtkPlusStreamBuffer::EXACT_TIME ) != ITEM_OK )
      {
        LOG_ERROR("Failed to get tracker buffer item from time: " << std::fixed << frameTimestamp ); 
        continue; 
      }

      vtkSmartPointer<vtkMatrix4x4> toolMatrix=vtkSmartPointer<vtkMatrix4x4>::New();
      if (toolBufferItem.GetMatrix(toolMatrix)!=PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to get toolMatrix"); 
        return PLUS_FAIL; 
      }

      PlusTransformName toolToTrackerTransform(it->second->GetToolName(), this->ToolReferenceFrameName ); 
      trackedFrame.SetCustomFrameTransform(toolToTrackerTransform, toolMatrix ); 

      // Add tool status
      trackedFrame.SetCustomFrameTransformStatus(toolToTrackerTransform, vtkPlusDevice::ConvertToolStatusToTrackedFrameFieldStatus(toolBufferItem.GetStatus()) ); 
    }

    // Add tracked frame to the list
    trackedFrameList->AddTrackedFrame(&trackedFrame); 
  }

  // Save tracked frames to metafile
  if ( trackedFrameList->SaveToSequenceMetafile(outputFolder, metaFileName, vtkTrackedFrameList::SEQ_METAFILE_MHA, useCompression) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to save tracked frames to sequence metafile!"); 
    return PLUS_FAIL;
  }

  return status;
}

//----------------------------------------------------------------------------
TrackedFrameFieldStatus vtkPlusDevice::ConvertToolStatusToTrackedFrameFieldStatus(ToolStatus status)
{
  TrackedFrameFieldStatus fieldStatus = FIELD_INVALID; 
  if ( status == TOOL_OK )
  {
    fieldStatus = FIELD_OK; 
  }

  return fieldStatus; 
}

//----------------------------------------------------------------------------
ToolStatus vtkPlusDevice::ConvertTrackedFrameFieldStatusToToolStatus(TrackedFrameFieldStatus fieldStatus)
{
  ToolStatus status = TOOL_MISSING; 
  if ( fieldStatus == FIELD_OK)
  {
    status = TOOL_OK ; 
  }

  return status; 
}

//----------------------------------------------------------------------------
std::string vtkPlusDevice::ConvertToolStatusToString(ToolStatus status)
{
  std::string flagFieldValue; 
  if ( status == TOOL_OK )
  {
    flagFieldValue = "OK"; 
  }
  else if ( status == TOOL_MISSING )
  {
    flagFieldValue = "TOOL_MISSING"; 
  }
  else if ( status == TOOL_OUT_OF_VIEW )
  {
    flagFieldValue = "TOOL_OUT_OF_VIEW"; 
  }
  else if ( status == TOOL_OUT_OF_VOLUME )
  {
    flagFieldValue = "TOOL_OUT_OF_VOLUME"; 
  }
  else if ( status == TOOL_REQ_TIMEOUT )
  {
    flagFieldValue = "TOOL_REQ_TIMEOUT"; 
  }
  else
  { 
    LOG_ERROR("Unknown tracker status received - set TOOL_INVALID by default!"); 
    flagFieldValue = "TOOL_INVALID"; 
  }

  return flagFieldValue; 
}


//----------------------------------------------------------------------------
vtkXMLDataElement* vtkPlusDevice::FindThisDeviceElement( vtkXMLDataElement* rootXMLElement )
{
  if( rootXMLElement == NULL )
  {
    LOG_ERROR("Unable to find device XML element for this device.");
    return NULL;
  }

  vtkXMLDataElement* dataCollectionElement = rootXMLElement->FindNestedElementWithName("DataCollection");
  if (dataCollectionElement == NULL)
  {
    LOG_ERROR("Unable to find data collection element in XML tree!"); 
    return NULL;     
  }

  vtkXMLDataElement* deviceXMLElement = NULL;
  for ( int i = 0; i < dataCollectionElement->GetNumberOfNestedElements(); ++i )
  {
    deviceXMLElement = dataCollectionElement->GetNestedElement(i); 

    if (deviceXMLElement->GetName() != NULL && deviceXMLElement->GetAttribute("Id") != NULL && 
      STRCASECMP(deviceXMLElement->GetName(), "Device") == 0 && STRCASECMP(deviceXMLElement->GetAttribute("Id"), this->GetDeviceId()) == 0)
    {
      return deviceXMLElement;
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------
vtkXMLDataElement* vtkPlusDevice::FindOutputStreamElement( vtkXMLDataElement* rootXMLElement, const char* aStreamId )
{
  if( rootXMLElement == NULL || aStreamId == NULL)
  {
    LOG_ERROR("Unable to find XML element for this stream. Bad inputs.");
    return NULL;
  }

  vtkXMLDataElement* deviceXMLElement = this->FindThisDeviceElement(rootXMLElement);

  for ( int i = 0; i < deviceXMLElement->GetNumberOfNestedElements(); ++i )
  {
    vtkXMLDataElement* streamXMLElement = deviceXMLElement->GetNestedElement(i); 

    if (streamXMLElement->GetName() != NULL && streamXMLElement->GetAttribute("Id") != NULL && 
      STRCASECMP(streamXMLElement->GetName(), "OutputStream") == 0 && STRCASECMP(streamXMLElement->GetAttribute("Id"), aStreamId) == 0)
    {
      return streamXMLElement;
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------
vtkXMLDataElement* vtkPlusDevice::FindInputStreamElement( vtkXMLDataElement* rootXMLElement, const char* aStreamId )
{
  if( rootXMLElement == NULL || aStreamId == NULL)
  {
    LOG_ERROR("Unable to find XML element for this stream. Bad inputs.");
    return NULL;
  }

  vtkXMLDataElement* deviceXMLElement = this->FindThisDeviceElement(rootXMLElement);

  for ( int i = 0; i < deviceXMLElement->GetNumberOfNestedElements(); ++i )
  {
    vtkXMLDataElement* streamXMLElement = deviceXMLElement->GetNestedElement(i); 

    if (streamXMLElement->GetName() != NULL && streamXMLElement->GetAttribute("Id") != NULL && 
      STRCASECMP(streamXMLElement->GetName(), "InputStream") == 0 && STRCASECMP(streamXMLElement->GetAttribute("Id"), aStreamId) == 0)
    {
      return streamXMLElement;
    }
  }

  return NULL;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::ReadConfiguration(vtkXMLDataElement* rootXMLElement)
{
  LOG_TRACE("vtkPlusDevice::ReadConfiguration");

  if( rootXMLElement == NULL )
  {
    LOG_ERROR("Unable to find device XML element for this device.");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* deviceXMLElement = this->FindThisDeviceElement(rootXMLElement);

  if( deviceXMLElement == NULL )
  {
    LOG_ERROR("Unable to find device XML element for device " << this->GetDeviceId() );
    return PLUS_FAIL;
  }

  const char* selectable = deviceXMLElement->GetAttribute("Selectable");
  if( selectable == NULL )
  {
    selectable = "false";
  }
  SetSelectable(STRCASECMP(selectable, "true") == 0);

  // Read tool configurations 
  for ( int tool = 0; tool < deviceXMLElement->GetNumberOfNestedElements(); tool++ )
  {
    vtkXMLDataElement* toolDataElement = deviceXMLElement->GetNestedElement(tool); 
    if ( STRCASECMP(toolDataElement->GetName(), "Tool") != 0 )
    {
      // if this is not a Tool element, skip it
      continue; 
    }

    vtkSmartPointer<vtkPlusStreamTool> streamTool = vtkSmartPointer<vtkPlusStreamTool>::New(); 
    if ( streamTool->ReadConfiguration(toolDataElement, RequireToolAveragedItemsForFilteringInDeviceSetConfiguration) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to add tool to tracker - failed to read tool configuration"); 
      continue; 
    }

    if ( this->AddTool(streamTool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to add tool '" << streamTool->GetToolName() << "' to tracker on port " << streamTool->GetPortName() );
    }
  }

  int acquisitionRate = 0;
  if ( deviceXMLElement->GetScalarAttribute("AcquisitionRate", acquisitionRate) )
  {
    this->SetAcquisitionRate(acquisitionRate);
  }
  else if( RequireAcquisitionRateInDeviceSetConfiguration )
  {
    LOG_ERROR("Unable to find acquisition rate in device element when it is required.");
  }

  const char* usImageOrientation = deviceXMLElement->GetAttribute("UsImageOrientation");
  if ( usImageOrientation != NULL )
  {
    LOG_INFO("Selected US image orientation: " << usImageOrientation );
    this->SetDeviceImageOrientation( PlusVideoFrame::GetUsImageOrientationFromString(usImageOrientation) );
    if ( this->GetDeviceImageOrientation() == US_IMG_ORIENT_XX )
    {
      LOG_ERROR("Ultrasound image orientation is undefined - please set UsImageOrientation in the video source configuration");
    }
  }
  else if (this->RequireDeviceImageOrientationInDeviceSetConfiguration)
  {
    LOG_ERROR("Ultrasound image orientation is not defined in the device element - please set UsImageOrientation in the device configuration");
  }

  vtkXMLDataElement* rfElement = deviceXMLElement->FindNestedElementWithName("RfProcessing");
  if (rfElement != NULL)
  {
    this->RfProcessor->ReadConfiguration(rfElement);
    this->SaveRfProcessingParameters=true;
  }
  else if( RequireRfElementInDeviceSetConfiguration )
  {
    LOG_ERROR("Unable to find rf processing sub-element in device configuration when it is required.");
  }

  // Now that we have the tools, we can create the output streams and connect things as necessary
  for ( int stream = 0; stream < deviceXMLElement->GetNumberOfNestedElements(); stream++ )
  {
    vtkXMLDataElement* streamElement = deviceXMLElement->GetNestedElement(stream); 
    if ( STRCASECMP(streamElement->GetName(), "OutputStream") != 0 )
    {
      // if this is not a stream element, skip it
      continue; 
    }

    vtkSmartPointer<vtkPlusStream> aStream = vtkSmartPointer<vtkPlusStream>::New();
    aStream->SetOwnerDevice(this);
    aStream->ReadConfiguration(streamElement, RequireFrameBufferSizeInDeviceSetConfiguration, RequireAveragedItemsForFilteringInDeviceSetConfiguration);

    this->OutputStreams.push_back(aStream);
    aStream->Register(this);
  }

  double localTimeOffsetSec = 0;
  if ( deviceXMLElement->GetScalarAttribute("LocalTimeOffsetSec", localTimeOffsetSec) )
  {
    LOG_INFO("Device local time offset: " << 1000*localTimeOffsetSec << "ms" );
    this->SetLocalTimeOffsetSec(localTimeOffsetSec);
    this->SetToolLocalTimeOffsetSec(localTimeOffsetSec);
  }
  else if ( RequireLocalTimeOffsetSecInDeviceSetConfiguration )
  {
    LOG_ERROR("Unable to find local time offset in device configuration when it is required.");
  }

  if( this->OutputStreams.size() == 0 )
  {
    LOG_INFO("No output streams defined for device: " << this->GetDeviceId() );
  }

  // For each output stream, add the tools to the output stream
  // Now that we have the tools, we can create the output streams and connect things as necessary
  for ( int stream = 0; stream < deviceXMLElement->GetNumberOfNestedElements(); stream++ )
  {
    vtkXMLDataElement* streamElement = deviceXMLElement->GetNestedElement(stream); 
    if ( STRCASECMP(streamElement->GetName(), "OutputStream") != 0 )
    {
      // if this is not a stream element, skip it
      continue; 
    }
    
    const char * id = streamElement->GetAttribute("Id");  // Can't make it to this point if an ID is incorrectly defined
    vtkPlusStream* aStream=NULL;
    if( this->GetStreamByName(aStream, id) != PLUS_SUCCESS )
    {
      LOG_ERROR("Error while trying to find stream by name.");
      return PLUS_FAIL;
    }

    for ( int tool = 0; tool < streamElement->GetNumberOfNestedElements(); tool++ )
    {
      vtkXMLDataElement* toolDataElement = streamElement->GetNestedElement(tool); 
      if ( STRCASECMP(toolDataElement->GetName(), "Tool") != 0 )
      {
        continue;
      }
      
      const char* toolName = toolDataElement->GetAttribute("Name");
      vtkPlusStreamTool* aTool=NULL;
      if( this->GetTool(toolName, aTool) != PLUS_SUCCESS )
      {
        LOG_ERROR("Unable to retrieve tool.");
        return PLUS_FAIL;
      }

      aStream->AddTool(aTool);
    }
  }

  if( this->OutputStreams.size() > 0 )
  {
    this->CurrentStream = this->OutputStreams[0];
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::WriteConfiguration( vtkXMLDataElement* config )
{
  if ( config == NULL )
  {
    LOG_ERROR("Unable to write configuration: xml data element is NULL!"); 
    return PLUS_FAIL;
  }

  vtkXMLDataElement* dataCollectionConfig = config->FindNestedElementWithName("DataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* deviceDataElement = NULL;
  for ( int device = 0; device < dataCollectionConfig->GetNumberOfNestedElements(); device++ )
  {
    vtkXMLDataElement* anElement = dataCollectionConfig->GetNestedElement(device);
    if( STRCASECMP(anElement->GetName(), "Device") == 0 && anElement->GetAttribute("Id") != NULL && STRCASECMP(anElement->GetAttribute("Id"), this->GetDeviceId()) == 0 )
    {
      deviceDataElement = anElement;
      break;
    }
  }

  if ( deviceDataElement == NULL )
  {
    LOG_ERROR("Unable to write configuration: xml data element is NULL!"); 
    return PLUS_FAIL;
  }

  deviceDataElement->SetAttribute("Selectable", this->GetSelectable() ? "true" : "false");

  for ( int i = 0; i < deviceDataElement->GetNumberOfNestedElements(); i++ )
  {
    vtkXMLDataElement* toolElement = deviceDataElement->GetNestedElement(i); 
    if ( STRCASECMP(toolElement->GetName(), "Tool") != 0 )
    {
      // if this is not a tool element, skip it
      continue; 
    }
    vtkPlusStreamTool* aTool=NULL;    
    if( toolElement->GetAttribute("Name") == NULL || this->GetTool(toolElement->GetAttribute("Name"), aTool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to retrieve tool when saving config.");
      return PLUS_FAIL;
    }
    aTool->WriteConfiguration(toolElement);
  }

  if (this->SaveRfProcessingParameters)
  {
    vtkXMLDataElement* rfElement = deviceDataElement->FindNestedElementWithName("RfProcessing");
    this->RfProcessor->WriteConfiguration(rfElement);
  }

  this->InternalWriteOutputStreams(config);

  this->InternalWriteInputStreams(config);

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::Connect()
{
  LOG_TRACE("vtkPlusDevice::Connect");

  if (this->Connected)
  {
    LOG_DEBUG("Already connected to the data source");
    return PLUS_SUCCESS;
  }

  if (this->InternalConnect()!=PLUS_SUCCESS)
  {
    LOG_ERROR("Cannot connect to data source, ConnectInternal failed");
    return PLUS_FAIL;
  }

  this->Connected = 1;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::Disconnect()
{
  LOG_TRACE("vtkPlusDevice::Disconnect");

  if (!this->Connected)
  {
    LOG_DEBUG("Data source is already disconnected");
    return PLUS_SUCCESS;
  }
  if (this->Recording)
  {
    this->StopRecording();
  }

  this->Connected = 0;

  InternalDisconnect();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetTrackedFrame( double timestamp, TrackedFrame& aTrackedFrame )
{
  int numberOfErrors(0);
  double synchronizedTimestamp(0);

  if( this->CurrentStream == NULL )
  {
    LOG_ERROR("Current stream is null. There is no stream to request a tracked frame from.");
    return PLUS_FAIL;
  }

  PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->UpdateMutex);

  // Get frame UID
  if( this->CurrentStream->BufferCount() > 0 )
  {
    BufferItemUidType frameUID = 0; 
    ItemStatus status = this->GetBuffer()->GetItemUidFromTime(timestamp, frameUID); 
    if ( status != ITEM_OK )
    {
      if ( status == ITEM_NOT_AVAILABLE_ANYMORE )
      {
        LOG_ERROR("Couldn't get frame UID from time (" << std::fixed << timestamp <<
          ") - item not available anymore!"); 
      }
      else if ( status == ITEM_NOT_AVAILABLE_YET )
      {
        LOG_ERROR("Couldn't get frame UID from time (" << std::fixed << timestamp <<
          ") - item not available yet!");
      }
      else
      {
        LOG_ERROR("Couldn't get frame UID from time (" << std::fixed << timestamp << ")!");
      }

      return PLUS_FAIL; 
    }

    StreamBufferItem CurrentStreamBufferItem; 
    if ( this->GetBuffer()->GetStreamBufferItem(frameUID, &CurrentStreamBufferItem) != ITEM_OK )
    {
      LOG_ERROR("Couldn't get video buffer item by frame UID: " << frameUID); 
      return PLUS_FAIL; 
    }

    // Copy frame 
    PlusVideoFrame frame = CurrentStreamBufferItem.GetFrame(); 
    aTrackedFrame.SetImageData(frame);

    // Copy all custom fields
    StreamBufferItem::FieldMapType fieldMap = CurrentStreamBufferItem.GetCustomFrameFieldMap();
    StreamBufferItem::FieldMapType::iterator fieldIterator;
    for (fieldIterator = fieldMap.begin(); fieldIterator != fieldMap.end(); fieldIterator++)
    {
      aTrackedFrame.SetCustomFrameField((*fieldIterator).first, (*fieldIterator).second);
    }

    synchronizedTimestamp = CurrentStreamBufferItem.GetTimestamp(this->GetBuffer()->GetLocalTimeOffsetSec());
  }

  if( synchronizedTimestamp == 0 )
  {
    synchronizedTimestamp = timestamp;
  }

  // Add main tool timestamp
  std::ostringstream timestampFieldValue; 
  timestampFieldValue << std::fixed << synchronizedTimestamp; 
  aTrackedFrame.SetCustomFrameField("Timestamp", timestampFieldValue.str()); 

  for (ToolContainerConstIterator it = this->CurrentStream->GetToolBuffersStartIterator(); it != this->CurrentStream->GetToolBuffersEndIterator(); ++it)
  {
    vtkPlusStreamTool* aTool = it->second;
    PlusTransformName toolTransformName(aTool->GetToolName(), this->ToolReferenceFrameName ); 
    if ( !toolTransformName.IsValid() )
    {
      LOG_ERROR("Tool transform name is invalid!"); 
      numberOfErrors++; 
      continue; 
    }

    StreamBufferItem bufferItem;
    ItemStatus result = aTool->GetBuffer()->GetStreamBufferItemFromTime(synchronizedTimestamp, &bufferItem, vtkPlusStreamBuffer::INTERPOLATED );
    if ( result != ITEM_OK )
    {
      double latestTimestamp(0); 
      if ( aTool->GetBuffer()->GetLatestTimeStamp(latestTimestamp) != ITEM_OK )
      {
        LOG_ERROR("Failed to get latest timestamp!");
        numberOfErrors++;
      }

      double oldestTimestamp(0); 
      if ( aTool->GetBuffer()->GetOldestTimeStamp(oldestTimestamp) != ITEM_OK )
      {
        LOG_ERROR("Failed to get oldest timestamp!");
        numberOfErrors++; 
      }

      LOG_ERROR(aTool->GetToolName() << ": Failed to get tracker item from buffer by time: " << std::fixed << synchronizedTimestamp << " (Latest timestamp: " << latestTimestamp << "   Oldest timestamp: " << oldestTimestamp << ")."); 
      numberOfErrors++; 
      continue; 
    }

    vtkSmartPointer<vtkMatrix4x4> dMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    if (bufferItem.GetMatrix(dMatrix) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to get matrix from buffer item for tool " << aTool->GetToolName() ); 
      numberOfErrors++; 
      continue; 
    }

    if ( aTrackedFrame.SetCustomFrameTransform(toolTransformName, dMatrix) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to set transform for tool " << aTool->GetToolName() ); 
      numberOfErrors++; 
      continue; 
    }

    if ( aTrackedFrame.SetCustomFrameTransformStatus(toolTransformName, vtkPlusDevice::ConvertToolStatusToTrackedFrameFieldStatus(bufferItem.GetStatus()) ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to set transform status for tool " << aTool->GetToolName() ); 
      numberOfErrors++; 
      continue; 
    }

    // Copy all custom fields
    StreamBufferItem::FieldMapType fieldMap = bufferItem.GetCustomFrameFieldMap();
    StreamBufferItem::FieldMapType::iterator fieldIterator;
    for (fieldIterator = fieldMap.begin(); fieldIterator != fieldMap.end(); fieldIterator++)
    {
      aTrackedFrame.SetCustomFrameField((*fieldIterator).first, (*fieldIterator).second);
    }

    synchronizedTimestamp = bufferItem.GetTimestamp(0);
  }

  // Copy frame timestamp   
  aTrackedFrame.SetTimestamp(synchronizedTimestamp);

  return (numberOfErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL ); 
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetTrackedFrame(TrackedFrame* trackedFrame)
{
  LOG_TRACE("vtkPlusDevice::GetTrackedFrame - TrackedFrame"); 

  double mostRecentFrameTimestamp(0);
  if (this->GetMostRecentTimestamp(mostRecentFrameTimestamp) != PLUS_SUCCESS) 
  {
    LOG_ERROR("Failed to get most recent timestamp from the buffer!"); 
    return PLUS_FAIL;
  }

  return this->GetTrackedFrameByTime(mostRecentFrameTimestamp, trackedFrame); 
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetTrackedFrameList( double& aTimestampFrom, vtkTrackedFrameList* aTrackedFrameList, int aMaxNumberOfFramesToAdd )
{
  LOG_TRACE("vtkPlusDevice::GetTrackedFrameList(" << aTimestampFrom << ", " << aMaxNumberOfFramesToAdd << ")"); 

  if ( aTrackedFrameList == NULL )
  {
    LOG_ERROR("Unable to get tracked frame list - output tracked frame list is NULL!"); 
    return PLUS_FAIL; 
  }

  // If the buffer is empty then don't display an error just return without adding any items to the output tracked frame list
  if ( this->GetVideoDataAvailable() )
  {
    if ( this->GetBuffer()->GetNumberOfItems() == 0 )
    {
      LOG_DEBUG("vtkDataCollector::GetTrackedFrameList: the video buffer is empty, no items will be returned"); 
      return PLUS_SUCCESS;
    }
  }

  if ( this->GetTrackingEnabled() )
  {
    // Get the first tool
    vtkPlusStreamTool* firstActiveTool = NULL; 
    if ( this->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get first active tool"); 
      return PLUS_FAIL; 
    }
    if ( firstActiveTool->GetBuffer()->GetNumberOfItems() == 0 )
    {
      LOG_DEBUG("vtkDataCollector::GetTrackedFrameList: the tracker buffer is empty, no items will be returned"); 
      return PLUS_SUCCESS;
    }
  }

  // Get latest and oldest timestamp
  double mostRecentTimestamp(0); 
  if ( this->GetMostRecentTimestamp(mostRecentTimestamp) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to get most recent timestamp!"); 
    return PLUS_FAIL; 
  }

  PlusStatus status = PLUS_SUCCESS; 
  double oldestTimestamp(0); 
  if ( this->GetOldestTimestamp(oldestTimestamp) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get oldest timestamp from buffer!"); 
    return PLUS_FAIL; 
  }

  // If zero or negative starting timestamp is specified we assume that the acquisition starts from the most recent frame
  if ( aTimestampFrom <= 0.0)
  {
    aTimestampFrom = mostRecentTimestamp; 
  }

  if (aTimestampFrom<oldestTimestamp)
  {
    LOG_ERROR("Items are requested from "<<aTimestampFrom<<", but the oldest available data is acquired at "<<oldestTimestamp);
    return PLUS_FAIL;
  }

  if ( aMaxNumberOfFramesToAdd > 0 ) 
  {
    if ( this->GetVideoDataAvailable() )
    {
      BufferItemUidType mostRecentVideoUid = 0; 
      if ( this->GetBuffer()->GetItemUidFromTime(mostRecentTimestamp, mostRecentVideoUid) != ITEM_OK )
      {
        LOG_ERROR("Failed to get video buffer item by timestamp " << mostRecentTimestamp);
        return PLUS_FAIL;
      }
      BufferItemUidType videoUidFrom = 0; 
      if ( this->GetBuffer()->GetItemUidFromTime(aTimestampFrom, videoUidFrom) != ITEM_OK )
      {
        LOG_ERROR("Failed to get video buffer item by timestamp " << aTimestampFrom);
        return PLUS_FAIL;
      }

      BufferItemUidType firstVideoUidToAdd = videoUidFrom;
      if ( mostRecentVideoUid - videoUidFrom + 1 > aMaxNumberOfFramesToAdd )
      {
        // More frames are requested than the maximum allowed frames to add        
        firstVideoUidToAdd = mostRecentVideoUid - aMaxNumberOfFramesToAdd + 1; // +1: because most recent is needed too
      }
      else
      {
        LOG_TRACE("Number of frames in the video buffer is less than maxNumberOfFramesToAdd (more data is allowed to be recorded than it was provided by the data sources)"); 
      }

      if ( this->GetBuffer()->GetTimeStamp(firstVideoUidToAdd, aTimestampFrom ) != ITEM_OK )
      {
        LOG_ERROR("Failed to get video buffer timestamp from UID: " << firstVideoUidToAdd ); 
        return PLUS_FAIL; 
      }
    }
    else if ( this->GetTrackingEnabled() )
    {
      // Get the first tool
      vtkPlusStreamTool* firstActiveTool = NULL; 
      if ( this->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get tracked frame list - there is no active tool!"); 
        return PLUS_FAIL; 
      }
      vtkPlusStreamBuffer* trackerBuffer = firstActiveTool->GetBuffer(); 
      if ( trackerBuffer == NULL )
      {
        LOG_ERROR("Failed to get first active tool!"); 
        return PLUS_FAIL; 
      }

      BufferItemUidType mostRecentTrackerUid = 0; 
      if ( trackerBuffer->GetItemUidFromTime(mostRecentTimestamp, mostRecentTrackerUid) != ITEM_OK )
      {
        LOG_ERROR("Failed to get tracked buffer item by timestamp " << mostRecentTimestamp);
        return PLUS_FAIL;
      }
      BufferItemUidType trackerUidFrom = 0; 
      if ( trackerBuffer->GetItemUidFromTime(aTimestampFrom, trackerUidFrom) != ITEM_OK )
      {
        LOG_ERROR("Failed to get tracker buffer item by timestamp " << aTimestampFrom);
        return PLUS_FAIL;
      }

      BufferItemUidType firstTrackerUidToAdd = trackerUidFrom;
      if ( mostRecentTrackerUid - trackerUidFrom + 1 > aMaxNumberOfFramesToAdd )
      {
        // More frames are requested than the maximum allowed frames to add        
        firstTrackerUidToAdd = mostRecentTrackerUid - aMaxNumberOfFramesToAdd + 1; // +1: because most recent is needed too
      }
      else
      {
        LOG_TRACE("Number of frames in the tracker buffer is less than maxNumberOfFramesToAdd (more data is allowed to be recorded than it was provided by the data sources)"); 
      }

      if ( trackerBuffer->GetTimeStamp(firstTrackerUidToAdd, aTimestampFrom ) != ITEM_OK )
      {
        LOG_ERROR("Failed to get tracker buffer timestamp from UID: " << firstTrackerUidToAdd ); 
        return PLUS_FAIL; 
      }
    }
  }

  // Check input frameTimestamp to be in a valid range 
  if (aTimestampFrom > mostRecentTimestamp )
  {
    aTimestampFrom = mostRecentTimestamp; 
  }
  else if ( aTimestampFrom < oldestTimestamp )
  {
    aTimestampFrom = oldestTimestamp; 
  }

  // Determine how many frames to add
  int numberOfFramesSinceTimestamp = GetNumberOfFramesBetweenTimestamps(aTimestampFrom, mostRecentTimestamp);

  int numberOfFramesToAdd = 0;
  if (aMaxNumberOfFramesToAdd > 0)
  {
    numberOfFramesToAdd = std::min( aMaxNumberOfFramesToAdd, numberOfFramesSinceTimestamp );
  }
  else
  {
    numberOfFramesToAdd = numberOfFramesSinceTimestamp;
  }

  LOG_DEBUG("Number of added frames: " << numberOfFramesToAdd << " out of " << numberOfFramesSinceTimestamp);

  // If we couldn't find any frames (or one of the items were invalid) 
  // set the timestamp to the most recent one
  if ( numberOfFramesToAdd == 0 )
  {
    aTimestampFrom = mostRecentTimestamp; 
  }

  for (int i=0; i<numberOfFramesToAdd; ++i)
  {
    // Get tracked frame from buffer
    TrackedFrame trackedFrame; 

    if ( this->GetTrackedFrameByTime(aTimestampFrom, &trackedFrame) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to get tracked frame by time: " << std::fixed << aTimestampFrom ); 
      return PLUS_FAIL;
    }

    // Add tracked frame to the list 
    if ( aTrackedFrameList->AddTrackedFrame(&trackedFrame, vtkTrackedFrameList::SKIP_INVALID_FRAME) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to add tracked frame to the list!" ); 
      return PLUS_FAIL; 
    }

    // Get next timestamp
    if ( this->GetVideoDataAvailable() && i < numberOfFramesToAdd - 1 )
    {
      BufferItemUidType videoUid(0); 
      if ( this->GetBuffer()->GetItemUidFromTime(aTimestampFrom, videoUid) != ITEM_OK )
      {
        LOG_ERROR("Failed to get video buffer item UID from time: " << std::fixed << aTimestampFrom ); 
        return PLUS_FAIL; 
      }

      if ( videoUid >= this->GetBuffer()->GetLatestItemUidInBuffer() )
      {
        LOG_WARNING("Requested video uid (" << videoUid+1 << ") is not in the buffer yet!");
        break;
      }

      // Get the timestamp of the next item in the buffer
      if ( this->GetBuffer()->GetTimeStamp(++videoUid, aTimestampFrom) != ITEM_OK )
      {
        LOG_ERROR("Unable to get timestamp from video buffer by UID: " << videoUid); 
        return PLUS_FAIL;
      }
    }
    else if ( this->GetTrackingEnabled() && i < numberOfFramesToAdd - 1 )
    {
      // Get the first tool
      vtkPlusStreamTool* firstActiveTool = NULL; 
      if ( this->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get tracked frame list - there is no active tool!"); 
        return PLUS_FAIL; 
      }

      vtkPlusStreamBuffer* trackerBuffer = firstActiveTool->GetBuffer(); 
      if ( trackerBuffer == NULL )
      {
        LOG_ERROR("Failed to get first active tool!"); 
        return PLUS_FAIL;
      }

      BufferItemUidType trackerUid(0); 
      if ( trackerBuffer->GetItemUidFromTime(aTimestampFrom, trackerUid) != ITEM_OK )
      {
        LOG_ERROR("Failed to get tracker buffer item UID from time: " << std::fixed << aTimestampFrom ); 
        return PLUS_FAIL; 
      }

      if ( trackerUid >= trackerBuffer->GetLatestItemUidInBuffer() )
      {
        LOG_ERROR("Requested tracker uid (" << trackerUid+1 << ") is not in the buffer yet!");
        break;
      }

      // Get the timestamp of the next item in the buffer
      if ( trackerBuffer->GetTimeStamp(++trackerUid, aTimestampFrom) != ITEM_OK )
      {
        LOG_WARNING("Unable to get timestamp from tracker buffer by UID: " << trackerUid); 
        return PLUS_FAIL;
      }
    }
  }

  return status; 
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetTrackedFrameListSampled(double& aTimestamp, vtkTrackedFrameList* aTrackedFrameList, double aSamplingRateSec, double maxTimeLimitSec/*=-1*/)
{
  LOG_TRACE("vtkDataCollector::GetTrackedFrameListSampled(" << aTimestamp << ", " << aSamplingRateSec << ")"); 

  if ( aTrackedFrameList == NULL )
  {
    LOG_ERROR("Unable to get tracked frame list - output tracked frame list is NULL!"); 
    return PLUS_FAIL; 
  }

  double startTimeSec = vtkAccurateTimer::GetSystemTime();

  double mostRecentTimestamp(0); 
  if ( this->GetMostRecentTimestamp(mostRecentTimestamp) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to get most recent timestamp!"); 
    return PLUS_FAIL; 
  }
  // If the user provided a 0 timestamp then we just set the timestamp and not yet collect any data
  if ( aTimestamp < 0.0001 )
  {
    aTimestamp = mostRecentTimestamp; 
  }

  // Check if there are less frames than it would be needed according to the sampling rate
  int numberOfFramesSinceTimestamp = GetNumberOfFramesBetweenTimestamps(aTimestamp, mostRecentTimestamp);
  int numberOfSampledFrames = (int)((mostRecentTimestamp - aTimestamp) / aSamplingRateSec);

  if (numberOfFramesSinceTimestamp < numberOfSampledFrames)
  {
    LOG_WARNING("Unable to add frames at the requested sampling rate because the acquisition frame rate (" << numberOfFramesSinceTimestamp / (mostRecentTimestamp - aTimestamp) << ") is lower than the requested sampling rate ("<<1.0/aSamplingRateSec<<"fps). Reduce the sampling rate or increase the acquisition rate to resolve the issue.");
  }

  PlusStatus status=PLUS_SUCCESS;
  // Add frames to input trackedFrameList
  double latestAddedTimestamp=UNDEFINED_TIMESTAMP;
  for (;aTimestamp + aSamplingRateSec <= mostRecentTimestamp && (vtkAccurateTimer::GetSystemTime() - startTimeSec) < maxTimeLimitSec; aTimestamp += aSamplingRateSec)
  {
    double oldestTimestamp=0;
    if ( this->GetOldestTimestamp(oldestTimestamp) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get oldest timestamp from buffer!"); 
      return PLUS_FAIL; 
    }
    const double skippingMargin = ceil(SAMPLING_SKIPPING_MARGIN_SEC / aSamplingRateSec) * aSamplingRateSec;
    if (aTimestamp < oldestTimestamp+skippingMargin)
    {
      // the frame will be removed from the buffer really soon, so instead of trying to retrieve from the buffer and failing skip some frames
      double skipTime=ceil( (oldestTimestamp + skippingMargin + aSamplingRateSec - aTimestamp) / aSamplingRateSec) * aSamplingRateSec;
      aTimestamp += skipTime;
      LOG_WARNING("Frames are not available any more at time: " << std::fixed << aTimestamp <<". Skipping " << skipTime << " seconds."); 
      continue;
    }
    double closestTimestamp = GetClosestTrackedFrameTimestampByTime(aTimestamp);
    if ( latestAddedTimestamp != UNDEFINED_TIMESTAMP && closestTimestamp != UNDEFINED_TIMESTAMP && closestTimestamp <= latestAddedTimestamp )
    {
      // This frame has been already added.
      // Continue to avoid running GetTrackedFrameByTime (that copies the frame pixels from the device buffer).
      continue;
    }
    // Get tracked frame from buffer
    TrackedFrame trackedFrame; 
    if ( GetTrackedFrameByTime(closestTimestamp, &trackedFrame) != PLUS_SUCCESS )
    {
      LOG_WARNING("Unable retrieve frame from the devices for time: " << std::fixed << aTimestamp <<", probably the item is not available in the buffers anymore. Frames may be lost."); 
      continue;
    }
    latestAddedTimestamp=trackedFrame.GetTimestamp();
    // Add tracked frame to the list 
    if ( aTrackedFrameList->AddTrackedFrame(&trackedFrame, vtkTrackedFrameList::SKIP_INVALID_FRAME) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to add tracked frame to the list!" ); 
      status=PLUS_FAIL; 
    }
  }

  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetTrackedFrameByTime(double time, TrackedFrame* trackedFrame)
{
  LOG_TRACE("vtkDataCollector::GetTrackedFrameByTime");

  if ( this->GetTrackedFrame(time, *trackedFrame) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get tracked frame from device " << this->GetDeviceId() << " for timestamp time: " << std::fixed << time );
    return PLUS_FAIL;
  }

  // Save frame timestamp
  std::ostringstream strTimestamp; 
  strTimestamp << std::fixed << trackedFrame->GetTimestamp(); 
  trackedFrame->SetCustomFrameField("Timestamp", strTimestamp.str()); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
// Set the source to acquire data continuously.
// You should override this as appropriate for your device. 
PlusStatus vtkPlusDevice::StartRecording()
{
  LOG_TRACE("vtkPlusDevice::StartRecording");

  if (this->Recording)
  {
    LOG_DEBUG("Recording is already active");
    return PLUS_SUCCESS;
  }

  if (!this->Connected)
  {
    if (this->Connect() != PLUS_SUCCESS)
    {
      LOG_ERROR("Cannot start recording, connection failed");
      return PLUS_FAIL;
    }
  }

  if (this->InternalStartRecording() != PLUS_SUCCESS)
  {
    LOG_ERROR("Cannot start recording, internal StartRecording failed");
    return PLUS_FAIL;
  }

  this->Recording = 1;

  this->ThreadId =
    this->Threader->SpawnThread((vtkThreadFunctionType)\
    &vtkDataCaptureThread,this);

  this->Modified();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// Stop continuous acquisition.  You will have to override this
// if your class overrides Record()
PlusStatus vtkPlusDevice::StopRecording()
{
  LOG_TRACE("vtkPlusDevice::StopRecording");

  if (!this->Recording)
  {
    LOG_DEBUG("Recording is already inactive");
    return PLUS_SUCCESS;
  }

  this->ThreadId = -1;
  this->Recording = 0;

  // Let's give a chance to the thread to stop before we kill the connection
  while ( this->ThreadAlive )
  {
    vtkAccurateTimer::Delay(0.1);
  }
  this->ThreadId = -1; 

  if ( this->InternalStopRecording() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to stop tracking thread!"); 
    return PLUS_FAIL; 
  }

  this->Modified();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// this function runs in an alternate thread to asynchronously acquire data
void* vtkPlusDevice::vtkDataCaptureThread(vtkMultiThreader::ThreadInfo *data)
{
  vtkPlusDevice *self = (vtkPlusDevice *)(data->UserData);

  double rate = self->GetAcquisitionRate();
  unsigned long frame = 0;
  double currtime[FRAME_RATE_AVERAGING]={0};
  unsigned long updatecount = 0;
  self->ThreadAlive = true; 

  while ( self->IsRecording() )
  {
    double newtime = vtkAccurateTimer::GetSystemTime();
    // get current tracking rate over last few updates
    double difftime = newtime - currtime[updatecount%FRAME_RATE_AVERAGING];
    currtime[updatecount%FRAME_RATE_AVERAGING] = newtime;
    if (updatecount > FRAME_RATE_AVERAGING && difftime != 0)
    {
      self->InternalUpdateRate = (FRAME_RATE_AVERAGING/difftime);
    }

    {
      // Lock before update 
      PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(self->UpdateMutex);
      if (!self->Recording)
      {
        // recording has been stopped
        break;
      }
      self->InternalUpdate();
      self->UpdateTime.Modified();
    }

    double delay = ( newtime + 1.0 / rate - vtkAccurateTimer::GetSystemTime() );
    if ( delay > 0 )
    {
      vtkAccurateTimer::Delay(delay); 
    }

    updatecount++;
  }

  self->ThreadAlive = false; 
  return NULL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetBufferSize( int& outVal, const char * toolName /*= NULL*/ )
{
  LOG_TRACE("vtkPlusDeviceg::GetBufferSize");

  if( toolName == NULL )
  {
    outVal = this->GetBuffer()->GetBufferSize();
    return PLUS_SUCCESS;
  }

  for ( ToolContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    if( STRCASECMP(it->second->GetToolName(), toolName) == 0 )
    {
      outVal = it->second->GetBuffer()->GetBufferSize();
      return PLUS_SUCCESS;
    }
  }
  
  LOG_ERROR("Unable to find tool " << toolName << " when looking its buffer size");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
// set or change the circular buffer size
// you will have to override this if you want the buffers
// to be device-specific (i.e. something other than vtkDataArray)
PlusStatus vtkPlusDevice::SetBufferSize( int FrameBufferSize, const char* toolName /*= NULL*/ )
{
  LOG_TRACE("vtkPlusDevice::SetBufferSize(" << FrameBufferSize << ")");

  if (FrameBufferSize < 0)
  {
    LOG_ERROR("vtkPlusDevice::SetBufferSize: There must be at least one framebuffer");
    return PLUS_FAIL;
  }

  if( toolName == NULL )
  {
    // update the buffer size
    if ( this->GetBuffer()->SetBufferSize(FrameBufferSize) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to set buffer size!");
      return PLUS_FAIL;
    }
    return PLUS_SUCCESS;
  }
  
  for ( ToolContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    if( STRCASECMP(it->second->GetToolName(), toolName) == 0 )
    {
      it->second->GetBuffer()->SetBufferSize(FrameBufferSize);
    }
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusDevice::SetStartTime( double startTime )
{
  for( StreamContainerIterator it = this->OutputStreams.begin(); it != this->OutputStreams.end(); ++it )
  {
    vtkPlusStream* aStream = *it;
    for( StreamBufferMapContainerConstIterator buffIter = aStream->GetBuffersStartConstIterator(); buffIter != aStream->GetBuffersEndConstIterator(); ++buffIter)
    {
      buffIter->second->SetStartTime(startTime);
    }
  }

  for ( ToolContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    it->second->GetBuffer()->SetStartTime(startTime); 
  }
}

//----------------------------------------------------------------------------
double vtkPlusDevice::GetStartTime()
{
  // TODO : should this return the buffer start time or should the buffer start time 
  //        simply be included in the average
  double sumStartTime = 0.0;
  double numberOfBuffers(0); 
  for( StreamContainerConstIterator it = this->OutputStreams.begin(); it != this->OutputStreams.end(); ++it )
  {
    vtkPlusStream* aStream = *it;
    for( StreamBufferMapContainerConstIterator buffIter = aStream->GetBuffersStartConstIterator(); buffIter != aStream->GetBuffersEndConstIterator(); ++buffIter )
    {
      sumStartTime += buffIter->second->GetStartTime();
      numberOfBuffers++;
    }
  }
  
  for ( ToolContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    sumStartTime += it->second->GetBuffer()->GetStartTime(); 
    numberOfBuffers++; 
  }

  return sumStartTime / numberOfBuffers;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::Probe()
{
  PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->UpdateMutex);

  if (this->InternalStartRecording() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  if (this->InternalStopRecording() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkPlusDevice::ClearAllBuffers()
{
  for( StreamContainerIterator it = this->OutputStreams.begin(); it != this->OutputStreams.end(); ++it )
  {
    vtkPlusStream* aStream = *it;
    for( StreamBufferMapContainerConstIterator buffIter = aStream->GetBuffersStartConstIterator(); buffIter != aStream->GetBuffersEndConstIterator(); ++buffIter )
    {
      vtkPlusStreamBuffer* aBuff = buffIter->second;
      aBuff->Clear();
    }
  }

  for ( ToolContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    it->second->GetBuffer()->Clear(); 
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetBrightnessFrameSize(int aDim[2])
{
  aDim[0]=this->BrightnessFrameSize[0];
  aDim[1]=this->BrightnessFrameSize[1];

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
vtkImageData* vtkPlusDevice::GetBrightnessOutput()
{  
  vtkImageData* resultImage=this->BlankImage;
  if ( GetBuffer()->GetLatestStreamBufferItem( &this->BrightnessOutputTrackedFrame ) != ITEM_OK )
  {
    LOG_DEBUG("No video data available yet, return blank frame");
  }
  else if (this->BrightnessOutputTrackedFrame.GetFrame().GetImageType()==US_IMG_BRIGHTNESS)
  {
    // B-mode image already, just return as is
    resultImage=this->BrightnessOutputTrackedFrame.GetFrame().GetVtkImage();
  }
  else
  {
    // RF frame, convert to B-mode frame
    this->RfProcessor->SetRfFrame(this->BrightnessOutputTrackedFrame.GetFrame().GetVtkImage(), this->BrightnessOutputTrackedFrame.GetFrame().GetImageType());
    resultImage=this->RfProcessor->GetBrightessScanConvertedImage();

    // RF processing parameters were used, so save them into the config file
    this->SaveRfProcessingParameters=true;
  }

  int *resultExtent=resultImage->GetExtent();
  this->BrightnessFrameSize[0]=resultExtent[1]-resultExtent[0]+1;
  this->BrightnessFrameSize[1]=resultExtent[3]-resultExtent[2]+1;

  return resultImage;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::ForceUpdate()
{
  LOG_TRACE("vtkPlusDevice::ForceUpdate");

  if (!this->Connected)
  {
    if (this->Connect() != PLUS_SUCCESS)
    {
      LOG_ERROR("Cannot acquire a single frame, connection failed");
      return PLUS_FAIL;
    }
  }

  {
    PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->UpdateMutex);
    this->InternalUpdate();
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::ToolTimeStampedUpdateWithoutFiltering(const char* aToolName, vtkMatrix4x4 *matrix, ToolStatus status, double unfilteredtimestamp, double filteredtimestamp) 
{
  if ( aToolName == NULL )
  {
    LOG_ERROR("Failed to update tool - tool name is NULL!"); 
    return PLUS_FAIL; 
  }

  vtkPlusStreamTool* tool = NULL; 
  if ( this->GetTool(aToolName, tool) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to update tool - unable to find tool!" << aToolName ); 
    return PLUS_FAIL; 
  }

  // This function is for devices has no frame numbering, just auto increment tool frame number if new frame received
  unsigned long frameNumber = tool->GetFrameNumber() + 1 ; 
  vtkPlusStreamBuffer* buffer = tool->GetBuffer();
  PlusStatus bufferStatus = buffer->AddTimeStampedItem(matrix, status, frameNumber, unfilteredtimestamp, filteredtimestamp);
  tool->SetFrameNumber(frameNumber); 

  return bufferStatus; 
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::ToolTimeStampedUpdate(const char* aToolName, vtkMatrix4x4 *matrix, ToolStatus status, unsigned long frameNumber, double unfilteredtimestamp) 
{
  if ( aToolName == NULL )
  {
    LOG_ERROR("Failed to update tool - tool name is NULL!"); 
    return PLUS_FAIL; 
  }

  vtkPlusStreamTool* tool = NULL; 
  if ( this->GetTool(aToolName, tool) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to update tool - unable to find tool!" << aToolName ); 
    return PLUS_FAIL; 
  }

  vtkPlusStreamBuffer* buffer = tool->GetBuffer();
  PlusStatus bufferStatus = buffer->AddTimeStampedItem(matrix, status, frameNumber, unfilteredtimestamp);
  tool->SetFrameNumber(frameNumber); 

  return bufferStatus; 
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GenerateDataAcquisitionReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter)
{
  if ( htmlReport == NULL || plotter == NULL )
  {
    LOG_ERROR("Caller should define HTML report generator and gnuplot plotter before report generation!"); 
    return PLUS_FAIL; 
  }

  vtkSmartPointer<vtkTable> timestampReportTable = vtkSmartPointer<vtkTable>::New(); 

  if( this->GetTrackingEnabled() )
  {
    // Use the first tool in the container to generate the report
    vtkPlusStreamTool* tool = this->GetToolIteratorBegin()->second;  

    if ( tool->GetBuffer()->GetTimeStampReportTable(timestampReportTable) != PLUS_SUCCESS )
    { 
      LOG_ERROR("Failed to get timestamp report table from tool '"<< tool->GetToolName() << "' buffer!"); 
      return PLUS_FAIL; 
    }
  }

  std::string reportFile = vtkPlusConfig::GetInstance()->GetOutputDirectory() + std::string("/")
    + std::string(vtkPlusConfig::GetInstance()->GetApplicationStartTimestamp()) 
    + std::string(".DataBufferTimestamps.txt"); 

  const char* scriptsFolder = vtkPlusConfig::GetInstance()->GetScriptsDirectory();
  std::string plotBufferTimestampScript = scriptsFolder + std::string("/gnuplot/PlotBufferTimestamp.gnu"); 
  if ( !vtksys::SystemTools::FileExists( plotBufferTimestampScript.c_str(), true) )
  {
    LOG_ERROR("Unable to find gnuplot script at: " << plotBufferTimestampScript); 
    return PLUS_FAIL; 
  }

  htmlReport->AddText("Tracking Data Acquisition Analysis", vtkHTMLGenerator::H1); 
  plotter->ClearArguments(); 
  plotter->AddArgument("-e");
  std::ostringstream trackerBufferAnalysis; 
  trackerBufferAnalysis << "f='" << reportFile << "'; o='TrackerBufferTimestamps';" << std::ends; 
  plotter->AddArgument(trackerBufferAnalysis.str().c_str()); 
  plotter->AddArgument(plotBufferTimestampScript.c_str());  

  htmlReport->AddImage("TrackerBufferTimestamps.jpg", "Tracking Data Acquisition Analysis"); 

  htmlReport->AddHorizontalLine(); 

  if ( this->GetBuffer()->GetTimeStampReportTable(timestampReportTable) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get timestamp report table from video buffer!");
    return PLUS_FAIL;
  }

  htmlReport->AddText("Video Data Acquisition Analysis", vtkHTMLGenerator::H1);
  plotter->ClearArguments();
  plotter->AddArgument("-e");
  std::ostringstream videoBufferAnalysis;
  videoBufferAnalysis << "f='" << reportFile << "'; o='VideoBufferTimestamps';" << std::ends;
  plotter->AddArgument(videoBufferAnalysis.str().c_str());

  plotter->AddArgument(plotBufferTimestampScript.c_str()); 
  if ( plotter->Execute() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to run gnuplot executer!");
    return PLUS_FAIL;
  }
  plotter->ClearArguments();

  htmlReport->AddImage("VideoBufferTimestamps.jpg", "Video Data Acquisition Analysis");

  htmlReport->AddHorizontalLine();

  if ( vtkGnuplotExecuter::DumpTableToFileInGnuplotFormat( timestampReportTable, reportFile.c_str() ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to write table to file in gnuplot format!");
    return PLUS_FAIL;
  }

  if ( !vtksys::SystemTools::FileExists( reportFile.c_str(), true) )
  {
    LOG_ERROR("Unable to find video data acquisition report file at: " << reportFile);
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
// This method returns the largest data that can be generated.
int vtkPlusDevice::RequestInformation(vtkInformation * vtkNotUsed(request),
                                           vtkInformationVector **vtkNotUsed(inputVector),
                                           vtkInformationVector *outputVector)
{
  //LOG_TRACE("vtkPlusDevice::RequestInformation");

  if (!this->Connected)
  {
    Connect();
  }

  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // Set extent
  int extent[6] = {0, this->GetBuffer()->GetFrameSize()[0] - 1, 0, this->GetBuffer()->GetFrameSize()[1] - 1, 0, 0 };
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),extent,6);

  // Set the origin and spacing. The video source provides raw pixel output, therefore the spacing is (1,1,1) and the origin is (0,0)
  double spacing[3]={1,1,1};
  outInfo->Set(vtkDataObject::SPACING(),spacing,3);
  double origin[3]={0,0,0};
  outInfo->Set(vtkDataObject::ORIGIN(),origin,3);

  // set default data type - unsigned char and number of components 1
  vtkDataObject::SetPointDataActiveScalarInfo(outInfo, PlusVideoFrame::GetVTKScalarPixelType(this->GetBuffer()->GetPixelType()), 1);

  return 1;
}

//----------------------------------------------------------------------------
// The Execute method is fairly complex, so I would not recommend overriding
// it unless you have to.  Override the UnpackRasterLine() method instead.
int vtkPlusDevice::RequestData(vtkInformation *vtkNotUsed(request),
                                    vtkInformationVector **vtkNotUsed(inputVector),
                                    vtkInformationVector *vtkNotUsed(outputVector))
{
  LOG_TRACE("vtkPlusDevice::RequestData");

  vtkImageData *data = vtkImageData::SafeDownCast(this->AllocateOutputData(this->GetOutputDataObject(0)));
  unsigned char *outPtr = (unsigned char *)data->GetScalarPointer();

  if ( this->GetBuffer() == NULL || this->GetBuffer()->GetNumberOfItems() < 1 )
  {
    // If the video buffer is empty, we can return immediately
    LOG_DEBUG("Cannot request data from video source, the video buffer is empty or does not exist!");
    return 1;
  }

  if (this->UpdateWithDesiredTimestamp && this->DesiredTimestamp != -1)
  {
    ItemStatus itemStatus = this->GetBuffer()->GetStreamBufferItemFromTime(this->DesiredTimestamp, this->CurrentStreamBufferItem, vtkPlusStreamBuffer::EXACT_TIME);
    if ( itemStatus != ITEM_OK )
    {
      LOG_ERROR("Unable to copy video data to the requested output!");
      return 1;
    } 
  }
  else
  {
    // get the most recent frame if we are not updating with the desired timestamp
    ItemStatus itemStatus = this->GetBuffer()->GetLatestStreamBufferItem(this->CurrentStreamBufferItem);
    if ( itemStatus != ITEM_OK )
    {
      LOG_ERROR("Unable to copy video data to the requested output!");
      return 1;
    }
  }

  this->FrameTimeStamp = this->CurrentStreamBufferItem->GetTimestamp( this->GetBuffer()->GetLocalTimeOffsetSec() );
  this->TimestampClosestToDesired = this->CurrentStreamBufferItem->GetTimestamp( this->GetBuffer()->GetLocalTimeOffsetSec() );

  void* sourcePtr=this->CurrentStreamBufferItem->GetFrame().GetBufferPointer();
  int bytesToCopy=this->CurrentStreamBufferItem->GetFrame().GetFrameSizeInBytes();

  int dimensions[3]={0,0,0};
  data->GetDimensions(dimensions);
  int outputSizeInBytes=dimensions[0]*dimensions[1]*dimensions[2]*data->GetScalarSize()*data->GetNumberOfScalarComponents();

  // the actual output size may be smaller than the output available
  // (e.g., when the rendering window is resized)
  if (bytesToCopy>outputSizeInBytes)
  {
    bytesToCopy=outputSizeInBytes;
  }

  memcpy( outPtr, sourcePtr, bytesToCopy);

  return 1;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::SetFrameSize(int x, int y)
{
  LOG_TRACE("vtkPlusDevice::SetFrameSize(" << x << ", " << y << ")");

  int* frameSize = this->GetBuffer()->GetFrameSize();

  if (x == frameSize[0] &&
    y == frameSize[1] )
  {
    return PLUS_SUCCESS;
  }

  if (x < 1 || y < 1)
  {
    LOG_ERROR("SetFrameSize: Illegal frame size");
    return PLUS_FAIL;
  }

  this->GetBuffer()->SetFrameSize(x,y); 

  this->Modified();
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetFrameSize(int &x, int &y)
{
  LOG_TRACE("vtkPlusDevice::GetFrameSize");

  int dim[2];
  if( this->GetFrameSize(dim) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to get frame size from the device.");
    return PLUS_FAIL;
  }
  x = dim[0];
  y = dim[1];

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetFrameSize(int dim[2])
{
  LOG_TRACE("vtkPlusDevice::GetFrameSize");

  return this->GetBuffer()->GetFrameSize(dim);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::SetPixelType(PlusCommon::ITKScalarPixelType pixelType)
{
  LOG_TRACE("vtkPlusDevice::SetPixelType");

  return this->GetBuffer()->SetPixelType(pixelType);
}

//----------------------------------------------------------------------------
PlusCommon::ITKScalarPixelType vtkPlusDevice::GetPixelType()
{
  LOG_TRACE("vtkPlusDevice::GetPixelType");

  return this->GetBuffer()->GetPixelType();
}

//----------------------------------------------------------------------------
US_IMAGE_TYPE vtkPlusDevice::GetImageType()
{
  return this->GetBuffer()->GetImageType();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::SetImageType(US_IMAGE_TYPE imageType)
{
  return this->GetBuffer()->SetImageType(imageType);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetStreamByName(vtkPlusStream*& aStream, const char * aStreamName )
{
  if( aStreamName == NULL )
  {
    LOG_ERROR("Null stream name sent to GetStreamByName.");
    return PLUS_FAIL;
  }

  for( StreamContainerIterator it = this->OutputStreams.begin(); it != this->OutputStreams.end(); ++it)
  {
    vtkPlusStream* stream = (*it);
    if( STRCASECMP(stream->GetStreamId(), aStreamName) == 0 )
    {
      aStream = stream;
      return PLUS_SUCCESS;
    }
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::AddInputStream(vtkPlusStream* aStream )
{
  for( StreamContainerIterator it = InputStreams.begin(); it != InputStreams.end(); ++it)
  {
    if( STRCASECMP((*it)->GetStreamId(), aStream->GetStreamId()) == 0 )
    {
      LOG_WARNING("Duplicate addition of an input stream.");
      return PLUS_SUCCESS;
    }
  }

  // aStream remains valid, as it is owned by another device (which provides it as an output)
  // TODO: it could be safer to increase the reference counter of aStream here (and decrease the ref counter when aStream is removed from InputStreams)
  this->InputStreams.push_back(aStream);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
double vtkPlusDevice::GetAcquisitionRate() const
{
  return this->AcquisitionRate;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetOldestTimestamp(double &ts)
{
  LOG_TRACE("vtkDataCollector::GetOldestTimestamp"); 
  ts=0;

  // ********************* video timestamp **********************
  double oldestVideoTimestamp(0); 
  if ( this->GetVideoDataAvailable() )
  {
    if ( this->GetBuffer()->GetOldestTimeStamp(oldestVideoTimestamp) != ITEM_OK )
    {
      LOG_WARNING("Failed to get oldest timestamp from video buffer!"); 
      return PLUS_FAIL; 
    }
  }

  // ********************* tracker timestamp **********************
  double oldestTrackerTimestamp(0); 
  if ( this->GetTrackingEnabled() )
  {
    // Get the first tool
    vtkPlusStreamTool* firstActiveTool = NULL; 
    if ( this->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get oldest timestamp from tracker buffer - there is no active tool!"); 
      return PLUS_FAIL; 
    }

    vtkPlusStreamBuffer* trackerBuffer = firstActiveTool->GetBuffer(); 

    if ( trackerBuffer == NULL )
    {
      LOG_ERROR("Failed to get first active tool!"); 
      return PLUS_FAIL; 
    }

    BufferItemUidType uid = trackerBuffer->GetOldestItemUidInBuffer(); 
    if ( uid + 1 < trackerBuffer->GetLatestItemUidInBuffer() )
    {
      // Always use the oldestItemUid + 1 to be able to interpolate transforms
      uid = uid + 1; 
    }

    // Get the oldest valid timestamp from the tracker buffer
    if ( trackerBuffer->GetTimeStamp(uid, oldestTrackerTimestamp ) != ITEM_OK )
    {
      LOG_WARNING("Unable to get timestamp from default tool tracker buffer with UID: " << uid); 
      return PLUS_FAIL;
    }
  }

  if ( !this->GetVideoDataAvailable() )
  {
    oldestVideoTimestamp = oldestTrackerTimestamp; 
  }

  if ( !this->GetTrackingEnabled() )
  {
    oldestTrackerTimestamp = oldestVideoTimestamp; 
  }

  // If the video timestamp is older than the tracker timestamp, adopt to the tracker timestamp
  while ( oldestVideoTimestamp < oldestTrackerTimestamp )
  {
    // Start from the oldest tracker timestamp 
    oldestVideoTimestamp = oldestTrackerTimestamp; 

    BufferItemUidType videoUid(0); 
    if ( this->GetBuffer()->GetItemUidFromTime(oldestVideoTimestamp, videoUid) != ITEM_OK )
    {
      LOG_ERROR("Failed to get video buffer item UID from time: " << std::fixed << oldestVideoTimestamp ); 
      return PLUS_FAIL; 
    }

    if ( videoUid + 1 <= this->GetBuffer()->GetLatestItemUidInBuffer() ) 
    {
      // Always use the next video UID to have an overlap between the two buffers 
      videoUid = videoUid + 1; 
    }

    if ( this->GetBuffer()->GetTimeStamp(videoUid, oldestVideoTimestamp) != ITEM_OK )
    {
      LOG_ERROR("Failed to get video buffer timestamp from UID: " << videoUid); 
      return PLUS_FAIL; 
    }
  }

  ts = oldestVideoTimestamp; 

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetMostRecentTimestamp(double &ts)
{
  LOG_TRACE("vtkDataCollector::GetMostRecentTimestamp"); 
  ts=0;

  double latestVideoTimestamp(0); 
  // This can't check for data, only if there is a video source device...
  if ( this->GetVideoDataAvailable() )
  {
    // Get the most recent timestamp from the buffer
    if ( this->GetBuffer()->GetLatestTimeStamp(latestVideoTimestamp) != ITEM_OK )
    {
      LOG_WARNING("Unable to get latest timestamp from video buffer!"); 
      return PLUS_FAIL;
    }
  }

  double latestTrackerTimestamp(0); 
  if ( this->GetTrackingEnabled() )
  {
    // Get the first tool
    vtkPlusStreamTool* firstActiveTool = NULL; 
    if ( this->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get most recent timestamp from tracker buffer - there is no active tool!"); 
      return PLUS_FAIL; 
    }

    vtkPlusStreamBuffer* trackerBuffer = firstActiveTool->GetBuffer(); 
    BufferItemUidType uid = trackerBuffer->GetLatestItemUidInBuffer(); 
    if ( uid > 1 )
    {
      // Always use the latestItemUid - 1 to be able to interpolate transforms
      uid = uid - 1; 
    }

    // Get the most recent valid timestamp from the tracker buffer
    if ( trackerBuffer->GetTimeStamp(uid, latestTrackerTimestamp ) != ITEM_OK )
    {
      LOG_WARNING("Unable to get timestamp from default tool tracker buffer with UID: " << uid); 
      return PLUS_FAIL;
    }
  }

  if ( !this->GetVideoDataAvailable() )
  {
    latestVideoTimestamp = latestTrackerTimestamp; 
  }

  if ( !this->GetTrackingEnabled() )
  {
    latestTrackerTimestamp = latestVideoTimestamp; 
  }

  // If the video timestamp is newer than the tracker timestamp, adopt to the tracker timestamp 
  while ( latestVideoTimestamp > latestTrackerTimestamp )
  {
    // Start from the latest tracker timestamp 
    latestVideoTimestamp = latestTrackerTimestamp; 

    BufferItemUidType videoUid(0); 
    if ( this->GetBuffer()->GetItemUidFromTime(latestVideoTimestamp, videoUid) != ITEM_OK )
    {
      LOG_ERROR("Failed to get video buffer item UID from time: " << std::fixed << latestVideoTimestamp ); 
      return PLUS_FAIL; 
    }

    if ( videoUid > 1 ) 
    {
      // Always use the preceding video UID to have time for transform interpolation 
      videoUid = videoUid - 1; 
    }

    if ( this->GetBuffer()->GetTimeStamp(videoUid, latestVideoTimestamp) != ITEM_OK )
    {
      LOG_ERROR("Failed to get video buffer timestamp from UID: " << videoUid); 
      return PLUS_FAIL; 
    }
  }

  ts = latestVideoTimestamp; 

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
double vtkPlusDevice::GetClosestTrackedFrameTimestampByTime(double time)
{

  if ( this->GetVideoDataAvailable() )
  {
    BufferItemUidType uid=0;
    if (this->GetBuffer()->GetItemUidFromTime(time, uid)!=ITEM_OK)
    {
      return UNDEFINED_TIMESTAMP;
    }    
    double closestTimestamp = UNDEFINED_TIMESTAMP; 
    if ( this->GetBuffer()->GetTimeStamp(uid, closestTimestamp)!=ITEM_OK)
    {
      return UNDEFINED_TIMESTAMP;
    }
    return closestTimestamp;
  }

  if ( this->GetTrackingEnabled() )
  {
    // Get the first tool
    vtkPlusStreamTool* firstActiveTool = NULL; 
    if ( this->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
    {
      // there is no active tool
      return UNDEFINED_TIMESTAMP; 
    }
    vtkPlusStreamBuffer* trackerBuffer = firstActiveTool->GetBuffer(); 
    if ( trackerBuffer == NULL )
    {
      // there is no buffer
      return UNDEFINED_TIMESTAMP;
    }
    BufferItemUidType uid=0;
    if (trackerBuffer->GetItemUidFromTime(time, uid)!=ITEM_OK)
    {
      return UNDEFINED_TIMESTAMP;
    }    
    double closestTimestamp = UNDEFINED_TIMESTAMP; 
    if (trackerBuffer->GetTimeStamp(uid, closestTimestamp)!=ITEM_OK)
    {
      return UNDEFINED_TIMESTAMP;
    }
    return closestTimestamp;
  }

  // neither tracker, nor video data available
  return UNDEFINED_TIMESTAMP;
}

//----------------------------------------------------------------------------
int vtkPlusDevice::GetNumberOfFramesBetweenTimestamps(double aTimestampFrom, double aTimestampTo)
{
  LOG_TRACE("vtkDataCollector::GetNumberOfFramesBetweenTimestamps(" << aTimestampFrom << ", " << aTimestampTo << ")");

  int numberOfFrames = 0;

  if ( this->GetVideoDataAvailable() )
  {
    StreamBufferItem vFromItem; 
    if (this->GetBuffer()->GetStreamBufferItemFromTime(aTimestampFrom, &vFromItem, vtkPlusStreamBuffer::EXACT_TIME) != ITEM_OK )
    {
      return 0;
    }

    StreamBufferItem vToItem; 
    if (this->GetBuffer()->GetStreamBufferItemFromTime(aTimestampTo, &vToItem, vtkPlusStreamBuffer::EXACT_TIME) != ITEM_OK )
    {
      return 0;
    }

    numberOfFrames = abs((int)(vToItem.GetUid() - vFromItem.GetUid()));
  }
  else if ( this->GetTrackingEnabled())
  {
    // Get the first tool
    vtkPlusStreamTool* firstActiveTool = NULL; 
    if ( this->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get number of frames between timestamps - there is no active tool!"); 
      return PLUS_FAIL; 
    }

    vtkPlusStreamBuffer* trackerBuffer = firstActiveTool->GetBuffer(); 
    if ( trackerBuffer == NULL )
    {
      LOG_ERROR("Failed to get first active tool!"); 
      return 0; 
    }

    // vtkPlusStreamBuffer::INTERPOLATED will give the closest item UID  
    StreamBufferItem tFromItem; 
    if (trackerBuffer->GetStreamBufferItemFromTime(aTimestampFrom, &tFromItem, vtkPlusStreamBuffer::INTERPOLATED) != ITEM_OK )
    {
      return 0;
    } 

    // vtkPlusStreamBuffer::INTERPOLATED will give the closest item UID 
    StreamBufferItem tToItem; 
    if (trackerBuffer->GetStreamBufferItemFromTime(aTimestampTo, &tToItem, vtkPlusStreamBuffer::INTERPOLATED) != ITEM_OK )
    {
      return 0;
    }

    numberOfFrames = abs((int)(tToItem.GetUid() - tFromItem.GetUid())); 
  }

  return numberOfFrames + 1;
}

//----------------------------------------------------------------------------
bool vtkPlusDevice::GetTrackingDataAvailable()
{
  TrackedFrame trackedFrame;
  if (this->GetTrackedFrame(&trackedFrame) != PLUS_SUCCESS)
  {
    LOG_ERROR("Cannot determine if tracking data is available, because failed to get tracked frame");
    return false;
  }

  std::vector<PlusTransformName> transformNames;
  trackedFrame.GetCustomFrameTransformNameList(transformNames);
  if (transformNames.size() == 0)
  {
    LOG_DEBUG("No transforms found in tracked frame");
    return false;
  }

  // there are transforms in the tracked frame
  return true;
}

//----------------------------------------------------------------------------
bool vtkPlusDevice::GetVideoDataAvailable()
{
  for( StreamContainerConstIterator it = this->OutputStreams.begin(); it != this->OutputStreams.end(); ++it)
  {
    vtkPlusStream* stream = *it;
    for( StreamBufferMapContainerConstIterator bufIt = stream->GetBuffersStartConstIterator(); bufIt != stream->GetBuffersEndConstIterator(); ++bufIt)
    {
      vtkPlusStreamBuffer* buffer = bufIt->second;
      StreamBufferItem item;
      if( buffer->GetLatestStreamBufferItem(&item) != ITEM_OK )
      {
        continue;
      }
      if( item.HasValidVideoData() )
      {
        return true;
      }
    }

    // Now check any and all tool buffers
    for( ToolContainerConstIterator it = stream->GetOwnerDevice()->GetToolIteratorBegin(); it != stream->GetOwnerDevice()->GetToolIteratorEnd(); ++it)
    {
      vtkPlusStreamTool* tool = it->second;
      StreamBufferItem item;
      if( tool->GetBuffer()->GetLatestStreamBufferItem(&item) != ITEM_OK )
      {
        continue;
      }
      if( item.HasValidVideoData() )
      {
        return true;
      }
    }
  }

  return false;
}

//----------------------------------------------------------------------------
vtkPlusStreamBuffer* vtkPlusDevice::GetBuffer()
{
  return this->GetBuffer(0);
}

//----------------------------------------------------------------------------
vtkPlusStreamBuffer* vtkPlusDevice::GetBuffer( int port )
{
  if( this->CurrentStream == NULL )
  {
    return NULL;
  }

  vtkPlusStreamBuffer* aBuff=NULL;
  if( this->CurrentStream->GetBuffer(aBuff, port) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to retrieve selected non-tool buffer " << port << " from device " << this->GetDeviceId());
    return NULL;
  }
  return aBuff;
}

//----------------------------------------------------------------------------
bool vtkPlusDevice::GetTrackingEnabled() const
{
  if( this->CurrentStream != NULL )
  {
    return this->CurrentStream->ToolCount() > 0;
  }
   
  LOG_ERROR("Current stream not defined. Unable to answer GetTrackingEnabled().");
  return false;
}

//----------------------------------------------------------------------------
void vtkPlusDevice::InternalWriteOutputStreams( vtkXMLDataElement* rootXMLElement )
{
  LOG_TRACE("vtkPlusDevice::InternalWriteOutputStreams( " << rootXMLElement->GetName() << ")");

  for( StreamContainerConstIterator it = this->OutputStreams.begin(); it != this->OutputStreams.end(); ++it)
  {
    vtkPlusStream* aStream = *it;
    vtkXMLDataElement* streamElement = this->FindOutputStreamElement(rootXMLElement, aStream->GetStreamId());
    aStream->WriteConfiguration(streamElement);
  }
}

//----------------------------------------------------------------------------
void vtkPlusDevice::InternalWriteInputStreams( vtkXMLDataElement* rootXMLElement )
{
  LOG_TRACE("vtkPlusDevice::InternalWriteInputStreams( " << rootXMLElement->GetName() << ")");

  for( StreamContainerConstIterator it = this->InputStreams.begin(); it != this->InputStreams.end(); ++it)
  {
    vtkPlusStream* aStream = *it;
    vtkXMLDataElement* streamElement = this->FindInputStreamElement(rootXMLElement, aStream->GetStreamId());
    aStream->WriteConfiguration(streamElement);
  }
}