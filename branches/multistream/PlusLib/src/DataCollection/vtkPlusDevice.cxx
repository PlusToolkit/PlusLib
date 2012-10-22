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
#include "vtkPlusStreamBuffer.h"
#include "vtkRecursiveCriticalSection.h"
#include "vtkRfProcessor.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTrackedFrameList.h"
#include "vtkPlusStreamTool.h"
#include "vtkWindows.h"
#include "vtksys\SystemTools.hxx"
#include <ctype.h>
#include <time.h>

#if ( _MSC_VER >= 1300 ) // Visual studio .NET
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4312 )
#endif

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkPlusDevice, "$Revision: 1.1$");
vtkStandardNewMacro(vtkPlusDevice);

//----------------------------------------------------------------------------
vtkPlusDevice::vtkPlusDevice()
: Recording(0)
, Connected(0)
, AcquisitionRate(30)
, ThreadAlive(false)
, ThreadId(-1)
, RequireDeviceImageOrientationInDeviceSetConfiguration(false)
, RequireFrameBufferSizeInDeviceSetConfiguration(false)
, RequireAcquisitionRateInDeviceSetConfiguration(false)
, RequireAveragedItemsForFilteringInDeviceSetConfiguration(false)
, RequireLocalTimeOffsetSecInDeviceSetConfiguration(false)
, RequireUsImageOrientationInDeviceSetConfiguration(false)
, RequireRfElementInDeviceSetConfiguration(false)
, RequireToolBufferSizeInDeviceSetConfiguration(false)
, DeviceImageOrientation(US_IMG_ORIENT_XX)
, SaveRfProcessingParameters(false)
, RfProcessor(vtkRfProcessor::New())
, BlankImage(vtkImageData::New())
, Buffer(vtkPlusStreamBuffer::New())
, CurrentDataBufferItem(new StreamBufferItem())
, FrameTimeStamp(0)
, FrameNumber(0)
, OutputNeedsInitialization(1)
, NumberOfOutputFrames(1)
, UpdateWithDesiredTimestamp(0)
, DesiredTimestamp(-1)
, TimestampClosestToDesired(-1)
{
  SetBufferSize(50);

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

  this->SetBufferSize(0);

  if( this->CurrentDataBufferItem != NULL )
  {
    delete CurrentDataBufferItem; CurrentDataBufferItem = NULL;
  }

  for ( ToolContainerConstIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it )
  {
    it->second->SetDevice(NULL); 
    it->second->Delete(); 
  }

  DELETE_IF_NOT_NULL(this->Buffer);

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
  os << indent << "Buffer:\n";
  this->Buffer->PrintSelf(os, indent.GetNextIndent());

  if (this->ToolReferenceFrameName)
  {
    os << indent << "ToolReferenceFrameName: " << this->ToolReferenceFrameName << "\n";
  }

  for( ToolContainerConstIteratorType it = this->ToolContainer.begin(); it != this->ToolContainer.end(); ++it )
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
ToolContainerConstIteratorType vtkPlusDevice::GetToolIteratorBegin() const
{
  return this->ToolContainer.begin(); 
}

//----------------------------------------------------------------------------
ToolContainerConstIteratorType vtkPlusDevice::GetToolIteratorEnd() const
{
  return this->ToolContainer.end();
}

//----------------------------------------------------------------------------
int vtkPlusDevice::GetNumberOfTools() const
{
  return this->ToolContainer.size(); 
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

  if ( this->ToolContainer.find( tool->GetToolName() ) == this->GetToolIteratorEnd() )
  {
    // Check tool port names, it should be unique too
    for ( ToolContainerConstIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
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
    this->ToolContainer[tool->GetToolName()] = tool; 
  }
  else
  {
    LOG_ERROR("Tool with name '" << tool->GetToolName() << "' is already in the tool conatainer!"); 
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetFirstActiveTool(vtkPlusStreamTool* &aTool)
{
  if ( this->GetToolIteratorBegin() == this->GetToolIteratorEnd() )
  {
    LOG_ERROR("Failed to get first active tool - there is no active tool!"); 
    return PLUS_FAIL; 
  }

  // Get the first tool
  aTool = this->GetToolIteratorBegin()->second; 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetTool(const char* aToolName, vtkPlusStreamTool* &aTool)
{
  if ( aToolName == NULL )
  {
    LOG_ERROR("Failed to get tool, tool name is NULL!"); 
    return PLUS_FAIL; 
  }

  ToolContainerConstIteratorType tool = this->ToolContainer.find(aToolName); 
  if ( tool == this->GetToolIteratorEnd() )
  {
    std::ostringstream availableTools; 
    for ( ToolContainerConstIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it )
    {
      availableTools << it->first <<";"; 
    }
    LOG_ERROR("Unable to find tool '"<< aToolName <<"' in the list, please check the configuration file first (available tools: " << availableTools.str() << ")." ); 
    return PLUS_FAIL; 
  }

  aTool = tool->second; 

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetToolByPortName( const char* portName, vtkPlusStreamTool* &aTool)
{
  if ( portName == NULL )
  {
    LOG_ERROR("Failed to get tool - port name is NULL!"); 
    return PLUS_FAIL; 
  }

  for ( ToolContainerConstIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
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
  for ( ToolContainerConstIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    it->second->GetBuffer()->SetBufferSize( aBufferSize ); 
  }
}

//-----------------------------------------------------------------------------
void vtkPlusDevice::SetLocalTimeOffsetSec( double aLocalTimeOffsetSec )
{
  LOG_TRACE("vtkPlusDevice::SetToolsLocalTimeOffsetSec(" << aLocalTimeOffsetSec << ")" ); 

  this->Buffer->SetLocalTimeOffsetSec(aLocalTimeOffsetSec);

  for ( ToolContainerConstIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    it->second->GetBuffer()->SetLocalTimeOffsetSec(aLocalTimeOffsetSec); 
  }
}

//-----------------------------------------------------------------------------
void vtkPlusDevice::DeepCopy(vtkPlusDevice* device)
{
  LOG_TRACE("vtkPlusDevice::DeepCopy"); 
  for ( ToolContainerConstIteratorType it = device->ToolContainer.begin(); it != device->ToolContainer.end(); ++it )
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
  for ( ToolContainerConstIteratorType it = this->ToolContainer.begin(); it != this->ToolContainer.end(); ++it)
  {
    if ( numberOfItems < 0 || numberOfItems > it->second->GetBuffer()->GetNumberOfItems() )
    {
      numberOfItems = it->second->GetBuffer()->GetNumberOfItems(); 
    }
  }

  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 

  PlusStatus status=PLUS_SUCCESS;

  // Get the first tool
  vtkPlusStreamTool* firstActiveTool = this->ToolContainer.begin()->second; 

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

    if ( firstActiveTool->GetBuffer()->GetDataBufferItem(uid, &bufferItem) != ITEM_OK )
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
    for ( ToolContainerConstIteratorType it = this->ToolContainer.begin(); it != this->ToolContainer.end(); ++it)
    {
      StreamBufferItem toolBufferItem; 
      if ( it->second->GetBuffer()->GetDataBufferItemFromTime( frameTimestamp, &toolBufferItem, vtkPlusStreamBuffer::EXACT_TIME ) != ITEM_OK )
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

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::ReadConfiguration(vtkXMLDataElement* config)
{
  // TODO : Create output streams
  //        each device creates its own output streams
  //        data collector will setinput/setouput and connect the pipeline as necessary
  //        each devices requestdata function will pull a stream from the input(s) and use it

  LOG_TRACE("vtkPlusDevice::ReadConfiguration");

  if ( config == NULL )
  {
    LOG_ERROR("Unable to configure Sonix video source! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  vtkXMLDataElement* dataCollectionConfig = config->FindNestedElementWithName("DataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* deviceXMLElement = NULL;
  for ( int device = 0; device < dataCollectionConfig->GetNumberOfNestedElements(); device++ )
  {
    vtkXMLDataElement* deviceXMLElement = dataCollectionConfig->GetNestedElement(device);
    if( STRCASECMP(deviceXMLElement->GetName(), "Device") == 0 && STRCASECMP(deviceXMLElement->GetAttribute("Id"), this->GetDeviceId()) )
    {
      break;
    }
  }

  if( deviceXMLElement == NULL )
  {
    LOG_ERROR("Unable to find device XML element for this device.");
    return PLUS_FAIL;
  }

  // TODO : for each OutputStream element, create a vtkplusstream, call read configuration on it with its xml element

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
    if ( streamTool->ReadConfiguration(toolDataElement) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to add tool to tracker - failed to read tool configuration"); 
      continue; 
    }

    if ( this->AddTool(streamTool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to add tool '" << streamTool->GetToolName() << "' to tracker on port " << streamTool->GetPortName() );
      continue; 
    }

    int bufferSize = 0; 
    if ( toolDataElement->GetScalarAttribute("BufferSize", bufferSize) ) 
    {
      streamTool->GetBuffer()->SetBufferSize(bufferSize);
    }
    else if ( RequireToolBufferSizeInDeviceSetConfiguration )
    {
      LOG_ERROR("Unable to find tool \"" << streamTool->GetToolName() << "\" buffer size in device element when it is required.");
    }
  }

  int bufferSize = 0;
  if ( deviceXMLElement->GetScalarAttribute("BufferSize", bufferSize) )
  {
    if ( this->Buffer->SetBufferSize(bufferSize) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to set video buffer size!");
    }
  }
  else if( RequireFrameBufferSizeInDeviceSetConfiguration )
  {
    LOG_ERROR("Unable to find main buffer size in device element when it is required.");
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

  int averagedItemsForFiltering = 0;
  if ( deviceXMLElement->GetScalarAttribute("AveragedItemsForFiltering", averagedItemsForFiltering) )
  {
    this->Buffer->SetAveragedItemsForFiltering(averagedItemsForFiltering);

    for ( ToolContainerConstIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
    {
      it->second->GetBuffer()->SetAveragedItemsForFiltering(averagedItemsForFiltering); 
    }
  }
  else if ( RequireAveragedItemsForFilteringInDeviceSetConfiguration )
  {
    LOG_ERROR("Unable to find averaged items for filtering in device configuration when it is required.");
  }
  else
  {
    LOG_DEBUG("Unable to find AveragedItemsForFiltering attribute in device element. Using default value.");
  }

  double localTimeOffsetSec = 0;
  if ( deviceXMLElement->GetScalarAttribute("LocalTimeOffsetSec", localTimeOffsetSec) )
  {
    LOG_INFO("Device local time offset: " << 1000*localTimeOffsetSec << "ms" );
    this->SetLocalTimeOffsetSec(localTimeOffsetSec);
  }
  else if ( RequireLocalTimeOffsetSecInDeviceSetConfiguration )
  {
    LOG_ERROR("Unable to find local time offset in device configuration when it is required.");
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
    vtkXMLDataElement* deviceDataElement = dataCollectionConfig->GetNestedElement(device);
    if( STRCASECMP(deviceDataElement->GetName(), "Device") == 0 && STRCASECMP(deviceDataElement->GetAttribute("Id"), this->GetDeviceId()) )
    {
      break;
    }
  }

  if ( deviceDataElement == NULL )
  {
    LOG_ERROR("Unable to write configuration: xml data element is NULL!"); 
    return PLUS_FAIL;
  }

  for ( ToolContainerConstIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    vtkPlusStreamTool* tool = it->second; 
    deviceDataElement->SetIntAttribute("BufferSize", tool->GetBuffer()->GetBufferSize()); 
    deviceDataElement->SetDoubleAttribute("LocalTimeOffsetSec", tool->GetBuffer()->GetLocalTimeOffsetSec() ); 
  }

  deviceDataElement->SetIntAttribute("BufferSize", this->Buffer->GetBufferSize());
  deviceDataElement->SetDoubleAttribute("LocalTimeOffsetSec", this->Buffer->GetLocalTimeOffsetSec() );

  if (this->SaveRfProcessingParameters)
  {
    vtkXMLDataElement* rfElement = deviceDataElement->FindNestedElementWithName("RfProcessing");
    this->RfProcessor->WriteConfiguration(rfElement);
  }

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

  // Get frame UID
  BufferItemUidType frameUID = 0; 
  ItemStatus status = this->Buffer->GetItemUidFromTime(timestamp, frameUID); 
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

  StreamBufferItem currentDataBufferItem; 
  if ( this->Buffer->GetDataBufferItem(frameUID, &currentDataBufferItem) != ITEM_OK )
  {
    LOG_ERROR("Couldn't get video buffer item by frame UID: " << frameUID); 
    return PLUS_FAIL; 
  }

  // Copy frame 
  PlusVideoFrame frame = currentDataBufferItem.GetFrame(); 
  aTrackedFrame.SetImageData(frame);

  // Copy all custom fields
  StreamBufferItem::FieldMapType fieldMap = currentDataBufferItem.GetCustomFrameFieldMap();
  StreamBufferItem::FieldMapType::iterator fieldIterator;
  for (fieldIterator = fieldMap.begin(); fieldIterator != fieldMap.end(); fieldIterator++)
  {
    aTrackedFrame.SetCustomFrameField((*fieldIterator).first, (*fieldIterator).second);
  }

  // Add main tool timestamp
  std::ostringstream timestampFieldValue; 
  timestampFieldValue << std::fixed << timestamp; 
  aTrackedFrame.SetCustomFrameField("Timestamp", timestampFieldValue.str()); 

  for (ToolContainerConstIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    PlusTransformName toolTransformName(it->second->GetToolName(), this->ToolReferenceFrameName ); 
    if ( ! toolTransformName.IsValid() )
    {
      LOG_ERROR("Tool transform name is invalid!"); 
      numberOfErrors++; 
      continue; 
    }

    StreamBufferItem bufferItem; 
    if ( it->second->GetBuffer()->GetDataBufferItemFromTime(timestamp, &bufferItem, vtkPlusStreamBuffer::INTERPOLATED ) != ITEM_OK )
    {
      double latestTimestamp(0); 
      if ( it->second->GetBuffer()->GetLatestTimeStamp(latestTimestamp) != ITEM_OK )
      {
        LOG_ERROR("Failed to get latest timestamp!"); 
      }

      double oldestTimestamp(0); 
      if ( it->second->GetBuffer()->GetOldestTimeStamp(oldestTimestamp) != ITEM_OK )
      {
        LOG_ERROR("Failed to get oldest timestamp!"); 
      }

      LOG_ERROR("Failed to get tracker item from buffer by time: " << std::fixed << timestamp << " (Latest timestamp: " << latestTimestamp << "   Oldest timestamp: " << oldestTimestamp << ")."); 
      numberOfErrors++; 
      continue; 
    }

    vtkSmartPointer<vtkMatrix4x4> dMatrix=vtkSmartPointer<vtkMatrix4x4>::New();
    if (bufferItem.GetMatrix(dMatrix)!=PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to get matrix from buffer item for tool " << it->second->GetToolName() ); 
      numberOfErrors++; 
      continue; 
    }

    if ( aTrackedFrame.SetCustomFrameTransform(toolTransformName, dMatrix) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to set transform for tool " << it->second->GetToolName() ); 
      numberOfErrors++; 
      continue; 
    }

    if ( aTrackedFrame.SetCustomFrameTransformStatus(toolTransformName, vtkPlusDevice::ConvertToolStatusToTrackedFrameFieldStatus(bufferItem.GetStatus()) ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to set transform status for tool " << it->second->GetToolName() ); 
      numberOfErrors++; 
      continue; 
    }
  }

  // TODO : determine who gets to timestamp this frame
  // Copy frame timestamp 
  aTrackedFrame.SetTimestamp(
    currentDataBufferItem.GetTimestamp(this->Buffer->GetLocalTimeOffsetSec()));

  return (numberOfErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL ); 
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
    if (this->Connect()!=PLUS_SUCCESS)
    {
      LOG_ERROR("Cannot start recording, connection failed");
      return PLUS_FAIL;
    }
  }

  if (this->InternalStartRecording()!=PLUS_SUCCESS)
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
  double currtime[10]={0};
  unsigned long updatecount = 0;
  self->ThreadAlive = true; 

  while ( self->IsRecording() )
  {
    double newtime = vtkAccurateTimer::GetSystemTime();
    // get current tracking rate over last 10 updates
    double difftime = newtime - currtime[updatecount%10];
    currtime[updatecount%10] = newtime;
    if (updatecount > 10 && difftime != 0)
    {
      self->InternalUpdateRate = (10.0/difftime);
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
PlusStatus vtkPlusDevice::SetBuffer(vtkPlusStreamBuffer *newBuffer)
{
  if (newBuffer==this->Buffer)
  {
    // no action, the buffer has been set already
    return PLUS_SUCCESS;
  }
  if ( newBuffer == NULL )
  {
    // this->Buffer pointer assumed to be valid (non-NULL) pointer throughout the 
    // vtkPlusDevice class and its child classes, therefore setting it to NULL
    // is not allowed
    LOG_ERROR( "A valid buffer is required as input for vtkPlusDevice::SetBuffer" );
    return PLUS_FAIL;
  }
  this->Buffer->UnRegister(this);
  this->Buffer=newBuffer;
  this->Buffer->Register(this);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetBufferSize( int& outVal, const char * toolName /*= NULL*/ )
{
  LOG_TRACE("vtkPlusDeviceg::GetBufferSize");

  if( toolName == NULL )
  {
    outVal = this->Buffer->GetBufferSize();
    return PLUS_SUCCESS;
  }

  for ( ToolContainerConstIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
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
    if ( this->Buffer->SetBufferSize(FrameBufferSize) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to set buffer size!");
      return PLUS_FAIL;
    }
    return PLUS_SUCCESS;
  }
  
  for ( ToolContainerConstIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
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
  this->Buffer->SetStartTime(startTime);

  for ( ToolContainerConstIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
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
  double numberOfTools(0); 
  for ( ToolContainerConstIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    sumStartTime += it->second->GetBuffer()->GetStartTime(); 
    numberOfTools++; 
  }

  return sumStartTime / numberOfTools;
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

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::CopyBuffer( vtkPlusStreamBuffer* aStreamBuffer, const char* aToolName )
{
  LOG_TRACE("vtkTracker::CopyBuffer"); 

  if ( aStreamBuffer == NULL )
  {
    LOG_ERROR("Unable to copy stream buffer to a NULL buffer!"); 
    return PLUS_FAIL; 
  }

  if( aToolName == NULL )
  {
    // Copy the image buffer
    aStreamBuffer->DeepCopy(this->GetBuffer());
    return PLUS_SUCCESS;
  }

  vtkPlusStreamTool * tool = NULL; 
  if ( GetTool(aToolName, tool) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get tool with name: " << aToolName ); 
    return PLUS_FAIL; 
  }

  aStreamBuffer->DeepCopy(tool->GetBuffer()); 

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkPlusDevice::ClearAllBuffers()
{
  this->GetBuffer()->Clear();

  for ( ToolContainerConstIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    it->second->GetBuffer()->Clear(); 
  }
}

//----------------------------------------------------------------------------
void vtkPlusDevice::GetBrightnessFrameSize(int aDim[2])
{
  aDim[0]=this->BrightnessFrameSize[0];
  aDim[1]=this->BrightnessFrameSize[1];
}

//----------------------------------------------------------------------------
vtkImageData* vtkPlusDevice::GetBrightnessOutput()
{  
  vtkImageData* resultImage=this->BlankImage;
  if ( GetBuffer()->GetLatestDataBufferItem( &this->BrightnessOutputTrackedFrame ) != ITEM_OK )
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
  vtkPlusStreamBuffer *buffer = tool->GetBuffer();
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

  vtkPlusStreamBuffer *buffer = tool->GetBuffer();
  PlusStatus bufferStatus = buffer->AddTimeStampedItem(matrix, status, frameNumber, unfilteredtimestamp);
  tool->SetFrameNumber(frameNumber); 

  return bufferStatus; 
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GenerateDataAcquisitionReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter)
{
  // TODO : test this

  if ( htmlReport == NULL || plotter == NULL )
  {
    LOG_ERROR("Caller should define HTML report generator and gnuplot plotter before report generation!"); 
    return PLUS_FAIL; 
  }

  vtkSmartPointer<vtkTable> timestampReportTable = vtkSmartPointer<vtkTable>::New(); 

  if( this->ToolContainer.size() > 0 )
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

  if ( this->Buffer->GetTimeStampReportTable(timestampReportTable) != PLUS_SUCCESS )
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

//-----------------------------------------------------------------------------
bool vtkPlusDevice::IsTracker() const
{
  LOG_ERROR("IsTracker called on vtkPlusDevice directly. I can't answer that question! Notify a developer. DeviceId: " << this->GetDeviceId());
  return false;
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
  int extent[6] = {0, this->Buffer->GetFrameSize()[0] - 1, 0, this->Buffer->GetFrameSize()[1] - 1, 0, 0 };
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),extent,6);

  // Set the origin and spacing. The video source provides raw pixel output, therefore the spacing is (1,1,1) and the origin is (0,0)
  double spacing[3]={1,1,1};
  outInfo->Set(vtkDataObject::SPACING(),spacing,3);
  double origin[3]={0,0,0};
  outInfo->Set(vtkDataObject::ORIGIN(),origin,3);

  // set default data type - unsigned char and number of components 1
  vtkDataObject::SetPointDataActiveScalarInfo(outInfo, PlusVideoFrame::GetVTKScalarPixelType(this->Buffer->GetPixelType()), 1);

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

  // TODO : re-write this to output multiple buffers in a vtkPlusStream
  // the output data
  vtkImageData *data = vtkImageData::SafeDownCast(this->AllocateOutputData(this->GetOutputDataObject(0)));
  unsigned char *outPtr = (unsigned char *)data->GetScalarPointer();

  if ( this->Buffer->GetNumberOfItems() < 1 )
  {
    // If the video buffer is empty, we can return immediately
    LOG_DEBUG("Cannot request data from video source, the video buffer is empty!");
    return 1;
  }

  if (this->UpdateWithDesiredTimestamp && this->DesiredTimestamp != -1)
  {
    ItemStatus itemStatus = this->Buffer->GetDataBufferItemFromTime(this->DesiredTimestamp, this->CurrentDataBufferItem, vtkPlusStreamBuffer::EXACT_TIME);
    if ( itemStatus != ITEM_OK )
    {
      LOG_ERROR("Unable to copy video data to the requested output!");
      return 1;
    } 
  }
  else
  {
    // get the most recent frame if we are not updating with the desired timestamp
    ItemStatus itemStatus = this->Buffer->GetLatestDataBufferItem(this->CurrentDataBufferItem);
    if ( itemStatus != ITEM_OK )
    {
      LOG_ERROR("Unable to copy video data to the requested output!");
      return 1;
    }
  }

  this->FrameTimeStamp = this->CurrentDataBufferItem->GetTimestamp( this->Buffer->GetLocalTimeOffsetSec() );
  this->TimestampClosestToDesired = this->CurrentDataBufferItem->GetTimestamp( this->Buffer->GetLocalTimeOffsetSec() );

  void* sourcePtr=this->CurrentDataBufferItem->GetFrame().GetBufferPointer();
  int bytesToCopy=this->CurrentDataBufferItem->GetFrame().GetFrameSizeInBytes();

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

  int* frameSize = this->Buffer->GetFrameSize();

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

  this->Buffer->SetFrameSize(x,y); 

  this->Modified();
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
int* vtkPlusDevice::GetFrameSize()
{
  LOG_TRACE("vtkPlusDevice::GetFrameSize");

  return this->Buffer->GetFrameSize();
}

//----------------------------------------------------------------------------
void vtkPlusDevice::GetFrameSize(int &x, int &y)
{
  LOG_TRACE("vtkPlusDevice::GetFrameSize");

  int dim[2];
  this->GetFrameSize(dim);
  x = dim[0];
  y = dim[1];
}

//----------------------------------------------------------------------------
void vtkPlusDevice::GetFrameSize(int dim[2])
{
  LOG_TRACE("vtkPlusDevice::GetFrameSize");

  this->Buffer->GetFrameSize(dim);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::SetPixelType(PlusCommon::ITKScalarPixelType pixelType)
{
  LOG_TRACE("vtkPlusDevice::SetPixelType");

  return this->Buffer->SetPixelType(pixelType);
}

//----------------------------------------------------------------------------
PlusCommon::ITKScalarPixelType vtkPlusDevice::GetPixelType()
{
  LOG_TRACE("vtkPlusDevice::GetPixelType");

  return this->Buffer->GetPixelType();
}

//----------------------------------------------------------------------------
US_IMAGE_TYPE vtkPlusDevice::GetImageType()
{
  return this->Buffer->GetImageType();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::SetImageType(US_IMAGE_TYPE imageType)
{
  return this->Buffer->SetImageType(imageType);
}

//----------------------------------------------------------------------------
vtkImageData* vtkPlusDevice::AllocateOutputData(vtkDataObject *output)
{ 
  // set the extent to be the update extent
  vtkImageData *out = vtkImageData::SafeDownCast(output);
  if (out)
  {
    // this needs to be fixed -Ken
    vtkStreamingDemandDrivenPipeline *sddp = 
      vtkStreamingDemandDrivenPipeline::SafeDownCast(this->GetExecutive());
    int numInfoObj = sddp->GetNumberOfOutputPorts();
    if (sddp && numInfoObj == 1)
    {
      int extent[6];
      sddp->GetOutputInformation(0)->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),extent);
      out->SetExtent(extent);
    }
    else
    {
      vtkWarningMacro( "There are multiple output ports. You cannot use AllocateOutputData" );
      return NULL;
    }
    out->AllocateScalars();
  }
  return out;
}