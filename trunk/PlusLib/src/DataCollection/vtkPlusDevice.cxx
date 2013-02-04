/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
//#include "itkImageIOBase.h"
#include "vtkGnuplotExecuter.h"
#include "vtkHTMLGenerator.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMetaImageReader.h"
#include "vtkMetaImageWriter.h"
#include "vtkMultiThreader.h" 
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusDevice.h"
#include "vtkPlusStreamBuffer.h"
#include "vtkRecursiveCriticalSection.h"
#include "vtkRfProcessor.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTrackedFrameList.h"
#include "vtkWindows.h"
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

const int vtkPlusDevice::VIRTUAL_DEVICE_FRAME_RATE = 50;
static const int FRAME_RATE_AVERAGING = 10;

//----------------------------------------------------------------------------
vtkPlusDevice::vtkPlusDevice()
: ThreadAlive(false)
, Connected(0)
, ThreadId(-1)
, CurrentChannel(NULL)
, CurrentStreamBufferItem(new StreamBufferItem())
, ToolReferenceFrameName(NULL)
, DeviceId(NULL)
, DefaultOutputChannel(NULL)
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
  if (this->Connected)
  {
    Disconnect();
  }

  this->InputChannels.clear();
  this->OutputChannels.clear();
  this->Tools.clear();
  this->VideoSources.clear();

  delete this->CurrentStreamBufferItem; this->CurrentStreamBufferItem = NULL;

  delete this->DeviceId; this->DeviceId = NULL;

  if( this->DefaultOutputChannel != NULL )
  {
    delete this->DefaultOutputChannel;
    this->DefaultOutputChannel = NULL;
  }

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

  for( ChannelContainerConstIterator it = this->OutputChannels.begin(); it != this->OutputChannels.end(); ++it )
  {
    vtkPlusChannel* str = *it;
    str->PrintSelf(os, indent);
  }

  if (this->ToolReferenceFrameName)
  {
    os << indent << "ToolReferenceFrameName: " << this->ToolReferenceFrameName << "\n";
  }

  for( DataSourceContainerConstIterator it = this->VideoSources.begin(); it != this->VideoSources.end(); ++it )
  {
    vtkPlusDataSource* dataSource = it->second;
    dataSource->PrintSelf(os, indent);
  }

  for( DataSourceContainerConstIterator it = this->Tools.begin(); it != this->Tools.end(); ++it )
  {
    vtkPlusDataSource* dataSource = it->second;
    dataSource->PrintSelf(os, indent);
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
DataSourceContainerConstIterator vtkPlusDevice::GetToolIteratorBegin() const
{
  return this->Tools.begin(); 
}

//----------------------------------------------------------------------------
DataSourceContainerConstIterator vtkPlusDevice::GetToolIteratorEnd() const
{
  return this->Tools.end();
}

//----------------------------------------------------------------------------
int vtkPlusDevice::GetNumberOfTools() const
{
  return this->Tools.size(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::AddTool( vtkPlusDataSource* tool )
{
  if ( tool == NULL )
  {
    LOG_ERROR("Failed to add tool to tracker, tool is NULL!"); 
    return PLUS_FAIL; 
  }

  if ( tool->GetSourceId() == NULL || tool->GetPortName() == NULL )
  {
    LOG_ERROR("Failed to add tool to tracker, tool Name and PortName must be defined!"); 
    return PLUS_FAIL; 
  }

  if ( this->Tools.find( tool->GetSourceId() ) == this->GetToolIteratorEnd() )
  {
    // Check tool port names, it should be unique too
    for ( DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
    {
      if ( STRCASECMP( tool->GetPortName(), it->second->GetPortName() ) == 0 )
      {
        LOG_ERROR("Failed to add '" << tool->GetSourceId() << "' tool to container: tool with name '" << it->second->GetSourceId() 
          << "' is already defined on port '" << tool->GetPortName() << "'!"); 
        return PLUS_FAIL; 
      }
    }

    tool->Register(this); 
    tool->SetDevice(this); 
    this->Tools[tool->GetSourceId()] = tool; 
  }
  else
  {
    LOG_ERROR("Tool with name '" << tool->GetSourceId() << "' is already in the tool conatainer!"); 
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetFirstActiveTool(vtkPlusDataSource*& aTool) const
{
  if( this->CurrentChannel == NULL )
  {
    LOG_ERROR("Current channel is null. Unable to get first active tool.");
    return PLUS_FAIL;
  }

  if ( this->CurrentChannel->GetToolsStartConstIterator() == this->CurrentChannel->GetToolsEndConstIterator() )
  {
    LOG_ERROR("Failed to get first active tool - there is no active tool!"); 
    return PLUS_FAIL; 
  }

  // Get the first tool
  aTool = this->CurrentChannel->GetToolsStartIterator()->second; 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetTool(const char* aToolName, vtkPlusDataSource*& aTool)
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

  if( CurrentChannel == NULL )
  {
    LOG_ERROR("Failed to get tool, CurrentChannel is NULL"); 
    return PLUS_FAIL;
  }

  if( this->CurrentChannel->GetTool(aTool, aToolName) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to find tool '"<< aToolName <<"' in the list, please check the configuration file first." ); 
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetToolByPortName( const char* portName, vtkPlusDataSource*& aTool)
{
  if ( portName == NULL )
  {
    LOG_ERROR("Failed to get tool - port name is NULL!"); 
    return PLUS_FAIL; 
  }

  for ( DataSourceContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    if ( STRCASECMP( portName, it->second->GetPortName() ) == 0 )
    {
      aTool = it->second; 
      return PLUS_SUCCESS; 
    }
  }

  if( CurrentChannel == NULL )
  {
    LOG_ERROR("Failed to get tool, CurrentChannel is NULL"); 
    return PLUS_FAIL;
  }

  for ( DataSourceContainerConstIterator it = this->CurrentChannel->GetToolsStartConstIterator(); it != this->CurrentChannel->GetToolsEndConstIterator(); ++it)
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
  for ( DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    it->second->GetBuffer()->SetBufferSize( aBufferSize ); 
  }
}

//----------------------------------------------------------------------------
void vtkPlusDevice::SetVideoLocalTimeOffsetSec( double aTimeOffsetSec )
{
  for( DataSourceContainerIterator it = this->VideoSources.begin(); it != this->VideoSources.end(); ++it )
  {
    vtkPlusDataSource* image = it->second;
    image->GetBuffer()->SetLocalTimeOffsetSec(aTimeOffsetSec);
  }
}

//----------------------------------------------------------------------------
void vtkPlusDevice::SetToolLocalTimeOffsetSec( double aTimeOffsetSec )
{
  // local tools
  for( DataSourceContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it )
  {
    vtkPlusDataSource* tool = it->second;
    tool->GetBuffer()->SetLocalTimeOffsetSec(aTimeOffsetSec);
  }
}

//----------------------------------------------------------------------------
double vtkPlusDevice::GetToolLocalTimeOffsetSec()
{
  // local tools
  for( DataSourceContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it )
  {
    vtkPlusDataSource* tool = it->second;
    double aTimeOffsetSec=tool->GetBuffer()->GetLocalTimeOffsetSec();
    return aTimeOffsetSec;
  }
  LOG_ERROR("Failed to get tool local time offset");
  return 0.0;
}

//----------------------------------------------------------------------------
double vtkPlusDevice::GetVideoLocalTimeOffsetSec()
{
  // local images
  for( DataSourceContainerIterator it = this->VideoSources.begin(); it != this->VideoSources.end(); ++it )
  {
    vtkPlusDataSource* image = it->second;
    double aTimeOffsetSec = image->GetBuffer()->GetLocalTimeOffsetSec();
    return aTimeOffsetSec;
  }
  LOG_ERROR("Failed to get image local time offset");
  return 0.0;
}

//-----------------------------------------------------------------------------
void vtkPlusDevice::DeepCopy(vtkPlusDevice* device)
{
  LOG_TRACE("vtkPlusDevice::DeepCopy"); 
  for ( DataSourceContainerConstIterator it = device->Tools.begin(); it != device->Tools.end(); ++it )
  {
    LOG_DEBUG("Copy the buffer of tracker tool: " << it->first ); 
    if ( this->AddTool(it->second) != PLUS_SUCCESS )
    {
      LOG_ERROR("Copy of tool '" << it->first << "' failed - unabale to add tool to the container!"); 
      continue; 
    }

    vtkPlusDataSource* tool = NULL; 
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
  for ( DataSourceContainerConstIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    if ( numberOfItems < 0 || numberOfItems > it->second->GetBuffer()->GetNumberOfItems() )
    {
      numberOfItems = it->second->GetBuffer()->GetNumberOfItems(); 
    }
  }

  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 

  PlusStatus status=PLUS_SUCCESS;

  // Get the first source
  vtkPlusDataSource* firstActiveTool = this->Tools.begin()->second; 

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

    // Add main source timestamp
    std::ostringstream timestampFieldValue; 
    timestampFieldValue << std::fixed << frameTimestamp; 
    trackedFrame.SetCustomFrameField("Timestamp", timestampFieldValue.str()); 

    // Add main source unfiltered timestamp
    std::ostringstream unfilteredtimestampFieldValue; 
    unfilteredtimestampFieldValue << std::fixed << bufferItem.GetUnfilteredTimestamp(firstActiveTool->GetBuffer()->GetLocalTimeOffsetSec()); 
    trackedFrame.SetCustomFrameField("UnfilteredTimestamp", unfilteredtimestampFieldValue.str()); 

    // Add main source frameNumber
    std::ostringstream frameNumberFieldValue; 
    frameNumberFieldValue << std::fixed << bufferItem.GetIndex(); 
    trackedFrame.SetCustomFrameField("FrameNumber", frameNumberFieldValue.str()); 

    // Add transforms
    for ( DataSourceContainerConstIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
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

      PlusTransformName toolToTrackerTransform(it->second->GetSourceId(), this->ToolReferenceFrameName ); 
      trackedFrame.SetCustomFrameTransform(toolToTrackerTransform, toolMatrix ); 

      // Add source status
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
vtkXMLDataElement* vtkPlusDevice::FindOutputChannelElement( vtkXMLDataElement* rootXMLElement, const char* aChannelId )
{
  if( rootXMLElement == NULL || aChannelId == NULL)
  {
    LOG_ERROR("Unable to find XML element for this channel. Bad inputs.");
    return NULL;
  }

  vtkXMLDataElement* deviceXMLElement = this->FindThisDeviceElement(rootXMLElement);

  vtkXMLDataElement* outputChannelsElement = deviceXMLElement->FindNestedElementWithName("OutputChannels");
  if( outputChannelsElement != NULL )
  {
    for ( int i = 0; i < outputChannelsElement->GetNumberOfNestedElements(); ++i )
    {
      vtkXMLDataElement* anXMLElement = outputChannelsElement->GetNestedElement(i); 

      if (anXMLElement->GetName() != NULL && anXMLElement->GetAttribute("Id") != NULL && 
        STRCASECMP(anXMLElement->GetName(), "OutputChannel") == 0 && STRCASECMP(anXMLElement->GetAttribute("Id"), aChannelId) == 0)
      {
        return anXMLElement;
      }
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------
vtkXMLDataElement* vtkPlusDevice::FindInputChannelElement( vtkXMLDataElement* rootXMLElement, const char* aChannelId )
{
  if( rootXMLElement == NULL || aChannelId == NULL)
  {
    LOG_ERROR("Unable to find XML element for this channel. Bad inputs.");
    return NULL;
  }

  vtkXMLDataElement* deviceXMLElement = this->FindThisDeviceElement(rootXMLElement);

  vtkXMLDataElement* inputChannelsElement = deviceXMLElement->FindNestedElementWithName("InputChannels");
  if( inputChannelsElement != NULL )
  {
    for ( int i = 0; i < inputChannelsElement->GetNumberOfNestedElements(); ++i )
    {
      vtkXMLDataElement* anXMLElement = inputChannelsElement->GetNestedElement(i); 

      if (anXMLElement->GetName() != NULL && anXMLElement->GetAttribute("Id") != NULL && 
        STRCASECMP(anXMLElement->GetName(), "InputChannel") == 0 && STRCASECMP(anXMLElement->GetAttribute("Id"), aChannelId) == 0)
      {
        return anXMLElement;
      }
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

  vtkXMLDataElement* dataSourcesElement = deviceXMLElement->FindNestedElementWithName("DataSources");
  if( dataSourcesElement != NULL )
  {
    // Read source configurations 
    for ( int source = 0; source < dataSourcesElement->GetNumberOfNestedElements(); source++ )
    {
      vtkXMLDataElement* dataSourceElement = dataSourcesElement->GetNestedElement(source); 

      if ( STRCASECMP(dataSourceElement->GetName(), "DataSource") != 0 )
      {
        // if this is not a Tool element, skip it
        continue; 
      }

      if( dataSourceElement->GetAttribute("Type") != NULL && STRCASECMP(dataSourceElement->GetAttribute("Type"), "Tool") == 0 )
      {
        vtkSmartPointer<vtkPlusDataSource> aDataSource = vtkSmartPointer<vtkPlusDataSource>::New(); 
        if ( aDataSource->ReadConfiguration(dataSourceElement, RequireToolAveragedItemsForFilteringInDeviceSetConfiguration) != PLUS_SUCCESS )
        {
          LOG_ERROR("Unable to add tool to tracker - failed to read tool configuration"); 
          continue; 
        }

        aDataSource->SetReferenceName(this->ToolReferenceFrameName);

        if ( this->AddTool(aDataSource) != PLUS_SUCCESS )
        {
          LOG_ERROR("Failed to add tool '" << aDataSource->GetSourceId() << "' to device on port " << aDataSource->GetPortName() );
        }
      }
      else if( dataSourceElement->GetAttribute("Type") != NULL && STRCASECMP(dataSourceElement->GetAttribute("Type"), "Video") == 0 )
      {
        vtkSmartPointer<vtkPlusDataSource> streamImage = vtkSmartPointer<vtkPlusDataSource>::New();
        streamImage->ReadConfiguration(dataSourceElement, RequireAveragedItemsForFilteringInDeviceSetConfiguration );

        if ( this->AddVideo(streamImage) != PLUS_SUCCESS )
        {
          LOG_ERROR("Failed to add video source '" << streamImage->GetSourceId() << "' to device.");
        }
      }
    }
  }

  // Continue with device configuration
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

  vtkXMLDataElement* outputChannelsElement = deviceXMLElement->FindNestedElementWithName("OutputChannels");
  if( outputChannelsElement != NULL )
  {
    // Now that we have the tools and images, we can create the output streams and connect things as necessary
    for ( int channel = 0; channel < outputChannelsElement->GetNumberOfNestedElements(); channel++ )
    {
      vtkXMLDataElement* channelElement = outputChannelsElement->GetNestedElement(channel); 
      if ( STRCASECMP(channelElement->GetName(), "OutputChannel") != 0 )
      {
        // if this is not a channel element, skip it
        continue; 
      }

      vtkSmartPointer<vtkPlusChannel> aChannel = vtkSmartPointer<vtkPlusChannel>::New();
      aChannel->SetOwnerDevice(this);
      aChannel->ReadConfiguration(channelElement);


      this->OutputChannels.push_back(aChannel);
      aChannel->Register(this);
    }
  }

  double localTimeOffsetSec = 0;
  if ( deviceXMLElement->GetScalarAttribute("LocalTimeOffsetSec", localTimeOffsetSec) )
  {
    LOG_INFO("Device local time offset: " << 1000*localTimeOffsetSec << "ms" );
    this->SetVideoLocalTimeOffsetSec(localTimeOffsetSec);
    this->SetToolLocalTimeOffsetSec(localTimeOffsetSec);
  }
  else if ( RequireLocalTimeOffsetSecInDeviceSetConfiguration )
  {
    LOG_ERROR("Unable to find local time offset in device configuration when it is required.");
  }

  if( this->OutputChannels.size() == 0 )
  {
    LOG_INFO("No output channels defined for device: " << this->GetDeviceId() );
  }
  else
  {
    const char* defaultOutputChannel = deviceXMLElement->GetAttribute("DefaultOutputChannelId");
    if( defaultOutputChannel != NULL )
    {
      vtkPlusChannel* aChannel(NULL);
      if( this->GetOutputChannelByName(aChannel, defaultOutputChannel) == PLUS_SUCCESS )
      {
        this->CurrentChannel = aChannel;
      }
    }
    else
    {
      this->CurrentChannel = this->OutputChannels[0];
    }
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

  vtkXMLDataElement* dataSourcesElement = deviceDataElement->FindNestedElementWithName("DataSources");
  if( dataSourcesElement != NULL )
  {
    for ( int i = 0; i < dataSourcesElement->GetNumberOfNestedElements(); i++ )
    {
      vtkXMLDataElement* dataSourceElement = dataSourcesElement->GetNestedElement(i); 
      if ( STRCASECMP(dataSourceElement->GetName(), "DataSource") != 0 )
      {
        // if this is not a data source element, skip it
        continue; 
      }
      vtkPlusDataSource* aDataSource=NULL;
      if( dataSourceElement->GetAttribute("Type") != NULL && STRCASECMP(dataSourceElement->GetAttribute("Type"), "Tool") == 0)
      {
        if( dataSourceElement->GetAttribute("Id") == NULL || this->GetTool(dataSourceElement->GetAttribute("Id"), aDataSource) != PLUS_SUCCESS )
        {
          LOG_ERROR("Unable to retrieve tool when saving config.");
          return PLUS_FAIL;
        }
        aDataSource->WriteConfiguration(dataSourceElement);
      }
      else if( dataSourceElement->GetAttribute("Type") != NULL && STRCASECMP(dataSourceElement->GetAttribute("Type"), "Video") == 0)
      {
        if( dataSourceElement->GetAttribute("Id") == NULL || this->GetVideoSource(dataSourceElement->GetAttribute("Id"), aDataSource) != PLUS_SUCCESS )
        {
          LOG_ERROR("Unable to retrieve video source when saving config.");
          return PLUS_FAIL;
        }
        aDataSource->WriteConfiguration(dataSourceElement);
      }
    }
  }

  if (this->SaveRfProcessingParameters)
  {
    vtkXMLDataElement* rfElement = deviceDataElement->FindNestedElementWithName("RfProcessing");
    this->RfProcessor->WriteConfiguration(rfElement);
  }

  this->InternalWriteOutputChannels(config);

  this->InternalWriteInputChannels(config);

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
    vtkPlusDataSource* aSource(NULL);
    if( this->CurrentChannel != NULL && this->CurrentChannel->GetVideoSource(aSource) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to retrieve the video source.");
      return PLUS_FAIL;
    }

    outVal = aSource->GetBuffer()->GetBufferSize();
    return PLUS_SUCCESS;
  }

  for ( DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    if( STRCASECMP(it->second->GetSourceId(), toolName) == 0 )
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
    vtkPlusDataSource* aSource(NULL);
    if( this->CurrentChannel != NULL && this->CurrentChannel->GetVideoSource(aSource) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to retrieve the video source.");
      return PLUS_FAIL;
    }

    // update the buffer size
    if ( aSource->GetBuffer()->SetBufferSize(FrameBufferSize) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to set buffer size!");
      return PLUS_FAIL;
    }
    return PLUS_SUCCESS;
  }

  for ( DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    if( STRCASECMP(it->second->GetSourceId(), toolName) == 0 )
    {
      it->second->GetBuffer()->SetBufferSize(FrameBufferSize);
    }
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusDevice::SetStartTime( double startTime )
{
  for ( DataSourceContainerConstIterator it = this->GetVideoIteratorBegin(); it != this->GetVideoIteratorEnd(); ++it)
  {
    it->second->GetBuffer()->SetStartTime(startTime); 
  }

  for ( DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
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
  for ( DataSourceContainerConstIterator it = this->GetVideoIteratorBegin(); it != this->GetVideoIteratorEnd(); ++it)
  {
    sumStartTime += it->second->GetBuffer()->GetStartTime(); 
    numberOfBuffers++; 
  }

  for ( DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
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
  for ( DataSourceContainerConstIterator it = this->GetVideoIteratorBegin(); it != this->GetVideoIteratorEnd(); ++it)
  {
    it->second->GetBuffer()->Clear(); 
  }
  for ( DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
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
  vtkPlusDataSource* aSource(NULL);
  if( this->CurrentChannel != NULL && this->CurrentChannel->GetVideoSource(aSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve the video source.");
    return resultImage;
  }

  if ( aSource->GetBuffer() != NULL && aSource->GetBuffer()->GetLatestStreamBufferItem( &this->BrightnessOutputTrackedFrame ) != ITEM_OK )
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

  vtkPlusDataSource* tool = NULL; 
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

  vtkPlusDataSource* tool = NULL; 
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
    vtkPlusDataSource* tool = this->GetToolIteratorBegin()->second;  

    if ( tool->GetBuffer()->GetTimeStampReportTable(timestampReportTable) != PLUS_SUCCESS )
    { 
      LOG_ERROR("Failed to get timestamp report table from tool '"<< tool->GetSourceId() << "' buffer!"); 
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

  if( this->GetVideoEnabled() )
  {
    vtkPlusDataSource* aSource(NULL);
    if( this->CurrentChannel != NULL && this->CurrentChannel->GetVideoSource(aSource) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to retrieve the video source.");
      return PLUS_FAIL;
    }

    if ( aSource->GetBuffer()->GetTimeStampReportTable(timestampReportTable) != PLUS_SUCCESS )
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
  vtkPlusDataSource* aSource(NULL);
  if( this->CurrentChannel != NULL && this->CurrentChannel->GetVideoSource(aSource) != PLUS_SUCCESS )
  {
    return 0;
  }

  int extent[6] = {0, aSource->GetBuffer()->GetFrameSize()[0] - 1, 0, aSource->GetBuffer()->GetFrameSize()[1] - 1, 0, 0 };
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),extent,6);

  // Set the origin and spacing. The video source provides raw pixel output, therefore the spacing is (1,1,1) and the origin is (0,0)
  double spacing[3]={1,1,1};
  outInfo->Set(vtkDataObject::SPACING(),spacing,3);
  double origin[3]={0,0,0};
  outInfo->Set(vtkDataObject::ORIGIN(),origin,3);

  // set default data type - unsigned char and number of components 1
  vtkDataObject::SetPointDataActiveScalarInfo(outInfo, PlusVideoFrame::GetVTKScalarPixelType(aSource->GetBuffer()->GetPixelType()), 1);

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

  vtkPlusDataSource* aSource(NULL);
  if( this->CurrentChannel != NULL && this->CurrentChannel->GetVideoSource(aSource) != PLUS_SUCCESS )
  {
    return 1;
  }

  if ( aSource->GetBuffer() == NULL || aSource->GetBuffer()->GetNumberOfItems() < 1 )
  {
    // If the video buffer is empty, we can return immediately
    LOG_DEBUG("Cannot request data from video source, the video buffer is empty or does not exist!");
    return 1;
  }

  if (this->UpdateWithDesiredTimestamp && this->DesiredTimestamp != -1)
  {
    ItemStatus itemStatus = aSource->GetBuffer()->GetStreamBufferItemFromTime(this->DesiredTimestamp, this->CurrentStreamBufferItem, vtkPlusStreamBuffer::EXACT_TIME);
    if ( itemStatus != ITEM_OK )
    {
      LOG_ERROR("Unable to copy video data to the requested output!");
      return 1;
    } 
  }
  else
  {
    // get the most recent frame if we are not updating with the desired timestamp
    ItemStatus itemStatus = aSource->GetBuffer()->GetLatestStreamBufferItem(this->CurrentStreamBufferItem);
    if ( itemStatus != ITEM_OK )
    {
      LOG_ERROR("Unable to copy video data to the requested output!");
      return 1;
    }
  }

  this->FrameTimeStamp = this->CurrentStreamBufferItem->GetTimestamp( aSource->GetBuffer()->GetLocalTimeOffsetSec() );
  this->TimestampClosestToDesired = this->CurrentStreamBufferItem->GetTimestamp( aSource->GetBuffer()->GetLocalTimeOffsetSec() );

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

  vtkPlusDataSource* aSource(NULL);
  if( this->CurrentChannel != NULL && this->CurrentChannel->GetVideoSource(aSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve the video source.");
    return PLUS_FAIL;
  }

  int* frameSize = aSource->GetBuffer()->GetFrameSize();

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

  aSource->GetBuffer()->SetFrameSize(x,y); 

  aSource->Modified();
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

  vtkPlusDataSource* aSource(NULL);
  if( this->CurrentChannel != NULL && this->CurrentChannel->GetVideoSource(aSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve the video source.");
    return PLUS_FAIL;
  }

  return aSource->GetBuffer()->GetFrameSize(dim);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::SetPixelType(PlusCommon::ITKScalarPixelType pixelType)
{
  LOG_TRACE("vtkPlusDevice::SetPixelType");

  vtkPlusDataSource* aSource(NULL);
  if( this->CurrentChannel != NULL && this->CurrentChannel->GetVideoSource(aSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve the video source.");
    return PLUS_FAIL;
  }

  return aSource->GetBuffer()->SetPixelType(pixelType);
}

//----------------------------------------------------------------------------
PlusCommon::ITKScalarPixelType vtkPlusDevice::GetPixelType()
{
  LOG_TRACE("vtkPlusDevice::GetPixelType");

  vtkPlusDataSource* aSource(NULL);
  if( this->CurrentChannel != NULL && this->CurrentChannel->GetVideoSource(aSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve the video source.");
    return itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  }

  return aSource->GetBuffer()->GetPixelType();
}

//----------------------------------------------------------------------------
US_IMAGE_TYPE vtkPlusDevice::GetImageType()
{
  vtkPlusDataSource* aSource(NULL);
  if( this->CurrentChannel != NULL && this->CurrentChannel->GetVideoSource(aSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve the video source.");
    return US_IMG_TYPE_XX;
  }

  return aSource->GetBuffer()->GetImageType();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::SetImageType(US_IMAGE_TYPE imageType)
{
  vtkPlusDataSource* aSource(NULL);
  if( this->CurrentChannel != NULL && this->CurrentChannel->GetVideoSource(aSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve the video source.");
    return PLUS_FAIL;
  }
  return aSource->GetBuffer()->SetImageType(imageType);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetOutputChannelByName(vtkPlusChannel*& aChannel, const char * aChannelId )
{
  if( aChannelId == NULL )
  {
    LOG_ERROR("Null channel name sent to GetOutputChannelByName.");
    return PLUS_FAIL;
  }

  for( ChannelContainerIterator it = this->OutputChannels.begin(); it != this->OutputChannels.end(); ++it)
  {
    vtkPlusChannel* channel = (*it);
    if( STRCASECMP(channel->GetChannelId(), aChannelId) == 0 )
    {
      aChannel = channel;
      return PLUS_SUCCESS;
    }
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::AddInputChannel(vtkPlusChannel* aChannel )
{
  for( ChannelContainerIterator it = InputChannels.begin(); it != InputChannels.end(); ++it)
  {
    if( STRCASECMP((*it)->GetChannelId(), aChannel->GetChannelId()) == 0 )
    {
      LOG_WARNING("Duplicate addition of an input aChannel.");
      return PLUS_SUCCESS;
    }
  }

  // aChannel remains valid, as it is owned by another device (which provides it as an output)
  // TODO: it could be safer to increase the reference counter of aChannel here (and decrease the ref counter when aChannel is removed from InputChannels)
  this->InputChannels.push_back(aChannel);
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
  if( this->CurrentChannel != NULL )
  {
    return this->CurrentChannel->GetOldestTimestamp(ts);
  }

  LOG_ERROR("No current channel.");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetMostRecentTimestamp(double &ts)
{
  if( this->CurrentChannel != NULL )
  {
    return this->CurrentChannel->GetMostRecentTimestamp(ts);
  }

  LOG_ERROR("No current channel.");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
double vtkPlusDevice::GetClosestTrackedFrameTimestampByTime(double time)
{
  if( this->CurrentChannel != NULL )
  {
    return this->CurrentChannel->GetClosestTrackedFrameTimestampByTime(time);
  }

  LOG_ERROR("Current aChannel not defined. Unable to answer GetClosestTrackedFrameTimestampByTime().");
  return false;
}

//----------------------------------------------------------------------------
bool vtkPlusDevice::GetTrackingDataAvailable()
{
  if( this->CurrentChannel != NULL )
  {
    return this->CurrentChannel->GetTrackingDataAvailable();
  }

  LOG_ERROR("Current aChannel not defined. Unable to answer GetTrackingDataAvailable().");
  return false;
}

//----------------------------------------------------------------------------
bool vtkPlusDevice::GetVideoDataAvailable()
{
  if( this->CurrentChannel != NULL )
  {
    return this->CurrentChannel->GetVideoDataAvailable();
  }

  LOG_ERROR("Current aChannel not defined. Unable to answer GetVideoDataAvailable().");
  return false;
}

//----------------------------------------------------------------------------
/*vtkPlusStreamBuffer* vtkPlusDevice::GetBuffer()
{
  if( this->CurrentChannel != NULL )
  {
    return this->CurrentChannel->GetBuffer();
  }

  LOG_ERROR("Current aChannel not defined. Unable to answer GetTrackingEnabled().");
  return false;
}*/

//----------------------------------------------------------------------------
bool vtkPlusDevice::GetTrackingEnabled() const
{
  if( this->CurrentChannel != NULL )
  {
    return this->CurrentChannel->GetTrackingEnabled();
  }

  LOG_ERROR("Current aChannel not defined. Unable to answer GetTrackingEnabled().");
  return false;
}

//----------------------------------------------------------------------------
bool vtkPlusDevice::GetVideoEnabled() const
{
  if( this->CurrentChannel != NULL )
  {
    return this->CurrentChannel->GetVideoEnabled();
  }

  LOG_ERROR("Current aChannel not defined. Unable to answer GetVideoEnabled().");
  return false;
}

//----------------------------------------------------------------------------
void vtkPlusDevice::InternalWriteOutputChannels( vtkXMLDataElement* rootXMLElement )
{
  LOG_TRACE("vtkPlusDevice::InternalWriteOutputChannels( " << rootXMLElement->GetName() << ")");

  for( ChannelContainerConstIterator it = this->OutputChannels.begin(); it != this->OutputChannels.end(); ++it)
  {
    vtkPlusChannel* aStream = *it;
    vtkXMLDataElement* streamElement = this->FindOutputChannelElement(rootXMLElement, aStream->GetChannelId());
    aStream->WriteConfiguration(streamElement);
  }
}

//----------------------------------------------------------------------------
void vtkPlusDevice::InternalWriteInputChannels( vtkXMLDataElement* rootXMLElement )
{
  LOG_TRACE("vtkPlusDevice::InternalWriteInputChannels( " << rootXMLElement->GetName() << ")");

  for( ChannelContainerConstIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it)
  {
    vtkPlusChannel* aStream = *it;
    vtkXMLDataElement* streamElement = this->FindInputChannelElement(rootXMLElement, aStream->GetChannelId());
    aStream->WriteConfiguration(streamElement);
  }
}

//----------------------------------------------------------------------------
DataSourceContainerConstIterator vtkPlusDevice::GetVideoIteratorBegin() const
{
  return this->VideoSources.begin(); 
}

//----------------------------------------------------------------------------
DataSourceContainerConstIterator vtkPlusDevice::GetVideoIteratorEnd() const
{
  return this->VideoSources.end();
}

//----------------------------------------------------------------------------
int vtkPlusDevice::GetNumberOfVideoSources() const
{
  return this->VideoSources.size(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::AddVideo( vtkPlusDataSource* aVideo )
{
  if ( aVideo == NULL )
  {
    LOG_ERROR("Failed to add video to device, video is NULL!"); 
    return PLUS_FAIL; 
  }

  if ( aVideo->GetSourceId() == NULL )
  {
    LOG_ERROR("Failed to add video to device, image Id must be defined!"); 
    return PLUS_FAIL; 
  }

  if ( this->VideoSources.find( aVideo->GetSourceId() ) == this->GetVideoIteratorEnd() )
  {
    // Check image port names, it should be unique too
    for ( DataSourceContainerConstIterator it = this->GetVideoIteratorBegin(); it != this->GetVideoIteratorEnd(); ++it)
    {
      if ( STRCASECMP( aVideo->GetSourceId(), it->second->GetSourceId() ) == 0 )
      {
        LOG_ERROR("Failed to add '" << aVideo->GetSourceId() << "' video to container: video with Id '" << it->second->GetSourceId() 
          << "' is already defined'!"); 
        return PLUS_FAIL; 
      }
    }

    aVideo->Register(this); 
    aVideo->SetDevice(this); 
    this->VideoSources[aVideo->GetSourceId()] = aVideo; 
  }
  else
  {
    LOG_ERROR("Image with Id '" << aVideo->GetSourceId() << "' is already in the image container!"); 
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetFirstActiveVideoSource(vtkPlusDataSource*& aVideoSource)
{
  if( this->CurrentChannel == NULL )
  {
    LOG_ERROR("Current aChannel is null. Unable to get first active source.");
    return PLUS_FAIL;
  }

  if ( !this->CurrentChannel->HasVideoSource() )
  {
    LOG_ERROR("Failed to get first active video source - there is no video source!"); 
    return PLUS_FAIL; 
  }

  // Get the first source
  if( this->CurrentChannel->GetVideoSource(aVideoSource) == PLUS_SUCCESS )
  {
    return PLUS_SUCCESS;
  }

  return PLUS_FAIL; 
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetVideoSource(const char* aSourceId, vtkPlusDataSource*& aVideoSource)
{
  if ( aSourceId == NULL )
  {
    LOG_ERROR("Failed to get image, image name is NULL"); 
    return PLUS_FAIL; 
  }

  if( this->VideoSources.find(aSourceId) != this->VideoSources.end() )
  {
    aVideoSource = this->VideoSources.find(aSourceId)->second;
    return PLUS_SUCCESS;
  }

  if( CurrentChannel == NULL )
  {
    return PLUS_FAIL;
  }

  if( this->CurrentChannel->GetVideoSource(aVideoSource) == PLUS_SUCCESS && STRCASECMP(aVideoSource->GetSourceId(), aSourceId) != 0)
  {
    aVideoSource = NULL;
    LOG_ERROR("Unable to find video source with Id '" << aSourceId <<"' in the list, please check the configuration file first." ); 
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetDataSource( const char* aSourceId, vtkPlusDataSource*& aSource )
{
  if( this->GetVideoSource(aSourceId, aSource) == PLUS_SUCCESS )
  {
    return PLUS_SUCCESS;
  }
  if( this->GetTool(aSourceId, aSource) == PLUS_SUCCESS )
  {
    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
ChannelContainerConstIterator vtkPlusDevice::GetOutputChannelsStart() const
{
  return this->OutputChannels.begin();
}

//----------------------------------------------------------------------------
ChannelContainerIterator vtkPlusDevice::GetOutputChannelsStart()
{
  return this->OutputChannels.begin();
}

//----------------------------------------------------------------------------
ChannelContainerConstIterator vtkPlusDevice::GetOutputChannelsEnd() const
{
  return this->OutputChannels.end();
}

//----------------------------------------------------------------------------
ChannelContainerIterator vtkPlusDevice::GetOutputChannelsEnd()
{
  return this->OutputChannels.end();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetCurrentChannel( vtkPlusChannel*& aChannel )
{
  if( this->CurrentChannel != NULL )
  {
    aChannel = this->CurrentChannel;
    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::SetCurrentChannel( const std::string& aChannelId )
{
  vtkPlusChannel* aChannel(NULL);
  if( this->GetOutputChannelByName(aChannel, aChannelId.c_str()) == PLUS_SUCCESS )
  {
    this->CurrentChannel = aChannel;
    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetTrackedFrame( double timestamp, TrackedFrame& trackedFrame, bool enableImageData/*=true*/ )
{
  PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->UpdateMutex);

  if( this->CurrentChannel == NULL )
  {
    LOG_ERROR("No current channel.");
    return PLUS_FAIL;    
  }

  return this->CurrentChannel->GetTrackedFrame(timestamp, trackedFrame, enableImageData);  
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetTrackedFrame( TrackedFrame *trackedFrame )
{
  LOG_TRACE("vtkPlusDevice::GetTrackedFrame(TrackedFrame)"); 

  double mostRecentFrameTimestamp(0);
  if (this->GetMostRecentTimestamp(mostRecentFrameTimestamp) != PLUS_SUCCESS) 
  {
    LOG_ERROR("Failed to get most recent timestamp from the buffer!"); 
    return PLUS_FAIL;
  }

  return this->GetTrackedFrameByTime(mostRecentFrameTimestamp, trackedFrame); 
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetTrackedFrameListSampled( double& aTimestamp, vtkTrackedFrameList* aTrackedFrameList, double aSamplingRateSec, double maxTimeLimitSec/*=-1*/ )
{
  PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->UpdateMutex);

  if( this->CurrentChannel != NULL )
  {
    return this->CurrentChannel->GetTrackedFrameListSampled(aTimestamp, aTrackedFrameList, aSamplingRateSec, maxTimeLimitSec);
  }

  LOG_ERROR("No current channel.");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetTrackedFrameList( double& aTimestampFrom, vtkTrackedFrameList* aTrackedFrameList, int aMaxNumberOfFramesToAdd )
{
  if( this->CurrentChannel != NULL )
  {
    return this->CurrentChannel->GetTrackedFrameList(aTimestampFrom, aTrackedFrameList, aMaxNumberOfFramesToAdd);
  }

  LOG_ERROR("No current channel.");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetTrackedFrameByTime( double time, TrackedFrame* trackedFrame )
{
  if( this->CurrentChannel != NULL )
  {
    return this->CurrentChannel->GetTrackedFrameByTime(time, trackedFrame);
  }

  LOG_ERROR("No current channel.");
  return PLUS_FAIL;
}
