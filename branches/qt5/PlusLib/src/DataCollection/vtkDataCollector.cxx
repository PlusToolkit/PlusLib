/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "TrackedFrame.h"
#include "vtkDataCollector.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusDevice.h"
#include "vtkPlusDeviceFactory.h"
#include "vtkPlusBuffer.h"
#include "vtkSavedDataSource.h"
#include "vtkTrackedFrameList.h"
#include "vtkXMLDataElement.h"
#include "vtksys/SystemTools.hxx"

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkDataCollector);

//----------------------------------------------------------------------------

vtkDataCollector::vtkDataCollector()
: vtkObject()
, StartupDelaySec(0.0)
, Connected(false)
, Started(false)
{
}

//----------------------------------------------------------------------------

vtkDataCollector::~vtkDataCollector()
{
  LOG_TRACE("vtkDataCollector::~vtkDataCollector()");
  if( this->Started )
  {
    this->Stop();
  }
  if( this->Connected )
  {
    this->Disconnect();
  }

  for( DeviceCollectionIterator it = this->Devices.begin(); it != this->Devices.end(); ++it)
  {
    (*it)->Delete();
  }
  Devices.clear();
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::ReadConfiguration( vtkXMLDataElement* aConfig )
{
  LOG_TRACE("vtkDataCollector::ReadConfiguration()");

  if (aConfig == NULL)
  {
    LOG_ERROR("Unable to read configuration"); 
    return PLUS_FAIL; 
  }

  if( this->Devices.size() > 0 )
  {
    // ReadConfiguration is being called for the n-th time
    LOG_ERROR("Repeated calls of vtkDataCollector::ReadConfiguration are not permitted. Delete the data collector and re-create to connect to a different config file.");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* dataCollectionElement = aConfig->FindNestedElementWithName("DataCollection");

  if (dataCollectionElement == NULL)
  {
    LOG_ERROR("Unable to find data collection element in XML tree!"); 
    return PLUS_FAIL;     
  }

  // Read StartupDelaySec
  double startupDelaySec(0.0); 
  if ( dataCollectionElement->GetScalarAttribute("StartupDelaySec", startupDelaySec) )
  {
    this->SetStartupDelaySec(startupDelaySec); 
    LOG_DEBUG("StartupDelaySec: " << std::fixed << startupDelaySec ); 
  }

  vtkSmartPointer<vtkPlusDeviceFactory> factory = vtkSmartPointer<vtkPlusDeviceFactory>::New();

  std::set<std::string> existingDeviceIds;

  for ( int i = 0; i < dataCollectionElement->GetNumberOfNestedElements(); ++i )
  {
    vtkXMLDataElement* deviceElement = dataCollectionElement->GetNestedElement(i); 
    if (deviceElement==NULL || STRCASECMP(deviceElement->GetName(), "Device") != 0 )
    {
      // only process valid Device elements
      continue;
    }

    vtkPlusDevice* device = NULL;
    const char* deviceId = deviceElement->GetAttribute("Id");
    if (deviceId==NULL)
    {
      LOG_ERROR("Device of type " << ( deviceElement->GetAttribute("Type") == NULL ? "UNDEFINED" : deviceElement->GetAttribute("Type")) << " has no Id attribute");
      return PLUS_FAIL;
    }

    if (existingDeviceIds.count(deviceId)>0)
    {
      LOG_ERROR("Multiple devices exist with the same Id: \'" << deviceId << "\'");
      return PLUS_FAIL;
    }
    existingDeviceIds.insert(deviceId);

    if( factory->CreateInstance(deviceElement->GetAttribute("Type"), device, deviceElement->GetAttribute("Id")) == PLUS_FAIL )
    {    
      LOG_ERROR("Unable to create device: " << deviceElement->GetAttribute("Type"));
      continue;
    }
    device->SetDataCollector(this);
    if (device->ReadConfiguration(aConfig)!=PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to read parameters of device: " << deviceElement->GetAttribute("Id") << " (type: "<<deviceElement->GetAttribute("Type")<<")");
      return PLUS_FAIL;
    }
    Devices.push_back(device);
  }

  if( Devices.size() == 0 )
  {
    LOG_ERROR("No devices created. Please verify configuration file and any error produced.");
    return PLUS_FAIL;
  }

  // Check output channels (at least one should exist and each id must be unique)
  std::set<std::string> existingOutputChannelNames;
  bool outputChannelFound = false;
  for( DeviceCollectionIterator deviceIt = this->Devices.begin(); deviceIt != this->Devices.end(); ++deviceIt)
  {
    for( ChannelContainerConstIterator outputChannelIt = (*deviceIt)->GetOutputChannelsStart(); outputChannelIt != (*deviceIt)->GetOutputChannelsEnd(); ++outputChannelIt )
    {
      outputChannelFound = true;
      const char* outputChannelId=(*outputChannelIt)->GetChannelId();
      if (outputChannelId)
      {
        if (existingOutputChannelNames.count(outputChannelId)>0)
        {
          LOG_ERROR("Same output channel Id is defined at multiple locations: "<<outputChannelId);
          return PLUS_FAIL;
        }
        existingOutputChannelNames.insert(outputChannelId);
      }
    }
  }
  
  if( !outputChannelFound )
  {
    LOG_WARNING("No output channels defined. Unable to locate any for data collection.");
  }

  // Connect any and all input streams to their corresponding output streams
  for ( int i = 0; i < dataCollectionElement->GetNumberOfNestedElements(); ++i )
  {
    vtkXMLDataElement* deviceElement = dataCollectionElement->GetNestedElement(i); 
    if (deviceElement==NULL || STRCASECMP(deviceElement->GetName(), "Device") != 0 )
    {
      // not a valid Device element
      continue;
    }
    vtkPlusDevice* thisDevice = NULL;
    if( this->GetDevice(thisDevice, deviceElement->GetAttribute("Id")) != PLUS_SUCCESS )
    {
      LOG_ERROR("Device " << deviceElement->GetAttribute("Id") << " doesn't exist.");
      return PLUS_FAIL;
    }
    vtkXMLDataElement* inputChannelsElement = deviceElement->FindNestedElementWithName("InputChannels");
    if (inputChannelsElement == NULL)
    {
      // no input channels, nothing to connect
      continue;
    }
    for ( int i = 0; i < inputChannelsElement->GetNumberOfNestedElements(); ++i )
    {
      vtkXMLDataElement* inputChannelElement = inputChannelsElement->GetNestedElement(i); 
      if( STRCASECMP(inputChannelElement->GetName(), "InputChannel") == 0 )
      {
        // We have an input channel, lets find it
        for( DeviceCollectionIterator it = Devices.begin(); it != Devices.end(); ++it )
        {
          vtkPlusDevice* device = (*it);
          vtkPlusChannel* aChannel = NULL;
          if( device->GetOutputChannelByName(aChannel, inputChannelElement->GetAttribute("Id")) == PLUS_SUCCESS )
          {
            // Found it!
            thisDevice->AddInputChannel(aChannel);
            break;
          }
        }
      }
    }
  }

  for( DeviceCollectionIterator it = this->Devices.begin(); it != this->Devices.end(); ++it )
  {
    if( (*it)->NotifyConfigured() != PLUS_SUCCESS )
    {
      LOG_ERROR("Device: " << (*it)->GetDeviceId() << " reports incorrect configuration. Please verify configuration.");
      return PLUS_FAIL;
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::WriteConfiguration( vtkXMLDataElement* aConfig )
{
  LOG_TRACE("vtkDataCollector::WriteConfiguration()");

  vtkXMLDataElement* dataCollectionConfig = PlusXmlUtils::GetNestedElementWithName(aConfig,"DataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  dataCollectionConfig->SetDoubleAttribute("StartupDelaySec", GetStartupDelaySec());

  PlusStatus status = PLUS_SUCCESS;

  for( DeviceCollectionConstIterator it = Devices.begin(); it != Devices.end(); ++it )
  {
    vtkPlusDevice* device = (*it);

    if( device->WriteConfiguration(aConfig) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to save device configuration " << device->GetDeviceId() );
      status = PLUS_FAIL;
    }
  }

  return status;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::Start()
{
  LOG_TRACE("vtkDataCollector::Start()");

  PlusStatus status = PLUS_SUCCESS;

  const double startTime = vtkAccurateTimer::GetSystemTime(); 

  for( DeviceCollectionIterator it = Devices.begin(); it != Devices.end(); ++ it )
  {
    vtkPlusDevice* device = *it;

    if( device->StartRecording() != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to start data acquisition for device " << device->GetDeviceId() << ".");
      status = PLUS_FAIL;
    }
    device->SetStartTime(startTime);
  }

  LOG_DEBUG("vtkDataCollector::Start -- wait " << std::fixed << this->StartupDelaySec << " sec for buffer init..."); 

  vtkAccurateTimer::DelayWithEventProcessing(this->StartupDelaySec);

  this->Started = true;

  return status;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::Stop()
{
  LOG_TRACE("vtkDataCollector::Stop()");

  this->Started = false;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::Connect()
{
  LOG_TRACE("vtkDataCollector::Connect()");

  PlusStatus status = PLUS_SUCCESS;

  for( DeviceCollectionIterator it = Devices.begin(); it != Devices.end(); ++ it )
  {
    vtkPlusDevice* device = *it;

    if( device->Connect() != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to connect device: " << device->GetDeviceId() <<".");
      status = PLUS_FAIL;
    }
  }

  if( status != PLUS_SUCCESS )
  {
    this->Disconnect();
    status = PLUS_FAIL;
  }

  if ( this->SetLoopTimes() != PLUS_SUCCESS )
  {
    LOG_WARNING("Failed to set loop times!"); 
    status = PLUS_FAIL;
  }

  this->Connected = (status == PLUS_SUCCESS);
  return status;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::Disconnect()
{
  LOG_TRACE("vtkDataCollector::Disconnect()");

  PlusStatus status = PLUS_SUCCESS;

  for( DeviceCollectionIterator it = Devices.begin(); it != Devices.end(); ++ it )
  {
    vtkPlusDevice* device = *it;

    if( device->Disconnect() != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to disconnect device: " << device->GetDeviceId() <<".");
      status = PLUS_FAIL;
    }
  }

  Connected = false;
  LOG_DEBUG("vtkDataCollector::Disconnect: All devices have been disconnected");

  return status;
}

//----------------------------------------------------------------------------

void vtkDataCollector::PrintSelf( ostream& os, vtkIndent indent )
{
  LOG_TRACE("vtkDataCollector::PrintSelf()");

  this->Superclass::PrintSelf(os,indent);

  for( DeviceCollectionIterator it = Devices.begin(); it != Devices.end(); ++ it )
  {
    os << indent << "Device: " << std::endl; 
    (*it)->PrintSelf(os, indent); 
  }
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::GetDevice( vtkPlusDevice* &aDevice, const std::string &aDeviceId ) const
{
  LOG_TRACE("vtkDataCollector::GetDevice( aDevice, " << aDeviceId << ")");

  for( DeviceCollectionConstIterator it = Devices.begin(); it != Devices.end(); ++it )
  {
    vtkPlusDevice* device = (*it);

    if( STRCASECMP(device->GetDeviceId(), aDeviceId.c_str()) == 0 )
    {
      aDevice = device;
      return PLUS_SUCCESS;
    }
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::GetDevices( DeviceCollection &OutVector ) const
{
  LOG_TRACE("vtkDataCollector::GetDevices()");

  OutVector.clear();

  for( DeviceCollectionConstIterator it = Devices.begin(); it != Devices.end(); ++it )
  {
    OutVector.push_back(*it);
  }

  return OutVector.size() > 0 ? PLUS_SUCCESS : PLUS_FAIL;
}

//----------------------------------------------------------------------------
bool vtkDataCollector::GetStarted() const
{
  return this->Started;
}

//----------------------------------------------------------------------------

bool vtkDataCollector::GetConnected() const
{
  return this->Connected;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::DumpBuffersToDirectory( const char * aDirectory )
{
  LOG_TRACE("vtkDataCollector::DumpBuffersToDirectory(" << aDirectory << ")");

  // Assemble file names
  std::string dateAndTime = vtksys::SystemTools::GetCurrentDateTime("%Y%m%d_%H%M%S");

  for( DeviceCollectionIterator it = this->Devices.begin(); it != this->Devices.end(); ++it )
  {
    vtkPlusDevice* device = *it;

    std::string outputDeviceBufferSequenceFileName = vtkPlusConfig::GetInstance()->GetOutputPath( std::string("BufferDump_")+device->GetDeviceId()+"_"+dateAndTime+".mha" );

    LOG_INFO("Write device buffer to " << outputDeviceBufferSequenceFileName);
    vtkPlusDataSource* aSource(NULL);
    for( ChannelContainerIterator chanIt = device->GetOutputChannelsStart(); chanIt != device->GetOutputChannelsEnd(); ++chanIt )
    {
      if( (*chanIt)->GetVideoSource(aSource) != PLUS_SUCCESS )
      {
        LOG_ERROR("Unable to retrieve the video source in the device.");
        return PLUS_FAIL;
      }
      aSource->WriteToMetafile( outputDeviceBufferSequenceFileName.c_str(), false); 
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
DeviceCollectionConstIterator vtkDataCollector::GetDeviceConstIteratorBegin() const
{
  return this->Devices.begin();
}

//----------------------------------------------------------------------------
DeviceCollectionConstIterator vtkDataCollector::GetDeviceConstIteratorEnd() const
{
  return this->Devices.end();
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetTrackingData(vtkPlusChannel* aRequestedChannel, double& aTimestampFrom, vtkTrackedFrameList* aTrackedFrameList)
{
  LOG_TRACE("vtkDataCollector::GetTrackingData(" << aRequestedChannel->GetChannelId() << ", " << aTimestampFrom << ")"); 

  if ( aTrackedFrameList == NULL )
  {
    LOG_ERROR("Unable to get tracked frame list - output tracked frame list is NULL"); 
    return PLUS_FAIL; 
  }

  // If the buffer is empty then don't display an error just return without adding any items to the output tracked frame list
  if ( !aRequestedChannel->GetTrackingEnabled() )
  {
    LOG_ERROR("Unable to get tracked frame list - Tracking is not enabled"); 
    return PLUS_FAIL; 
  }

  // Get the first tool, transforms will be returned at the timestamps of this first tool
  vtkPlusDataSource* firstActiveTool = NULL; 
  if( aRequestedChannel->GetOwnerDevice()->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to get tracked frame list - there is no active tool!"); 
    return PLUS_FAIL; 
  }

  if ( firstActiveTool->GetNumberOfItems()==0 )
  {
    LOG_DEBUG("vtkDataCollector::GetTrackingData: the tracking buffer is empty, no items will be returned"); 
    return PLUS_SUCCESS;
  }

  PlusStatus status = PLUS_SUCCESS;
  BufferItemUidType oldestItemUid = firstActiveTool->GetOldestItemUidInBuffer();
  BufferItemUidType latestItemUid = firstActiveTool->GetLatestItemUidInBuffer();
  for (BufferItemUidType itemUid = oldestItemUid; itemUid <= latestItemUid; ++itemUid)
  {
    double itemTimestamp=0;
    if (firstActiveTool->GetTimeStamp(itemUid, itemTimestamp)!=ITEM_OK)
    {
      // probably the buffer item is not available anymore
      continue;
    }
    if (itemTimestamp <= aTimestampFrom)
    {
      // this item has been acquired before the requested start time
      continue;
    }
    aTimestampFrom = itemTimestamp;
    // Get tracked frame from buffer
    TrackedFrame* trackedFrame = new TrackedFrame;
    if ( aRequestedChannel->GetTrackedFrame(itemTimestamp, *trackedFrame, false /* get tracking data only */ ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to get tracking data by time: " << std::fixed << itemTimestamp ); 
      status=PLUS_FAIL;
    }
    // Add tracked frame to the list 
    if ( aTrackedFrameList->TakeTrackedFrame(trackedFrame, vtkTrackedFrameList::SKIP_INVALID_FRAME) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to add tracking data to the list!" ); 
      status=PLUS_FAIL; 
    }
  }

  return status; 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetVideoData(vtkPlusChannel* aRequestedChannel, double& aTimestampFrom, vtkTrackedFrameList* aTrackedFrameList)
{
  LOG_TRACE("vtkDataCollector::GetVideoData(" << aRequestedChannel->GetChannelId() << ", " << aTimestampFrom << ")"); 

  if ( aTrackedFrameList == NULL )
  {
    LOG_ERROR("Unable to get tracked frame list - output tracked frmae list is NULL"); 
    return PLUS_FAIL; 
  }

  // If the buffer is empty then don't display an error just return without adding any items to the output tracked frame list
  vtkPlusDataSource* aSource(NULL);
  if ( aRequestedChannel->GetVideoSource(aSource) == PLUS_SUCCESS && aSource->GetNumberOfItems()==0 )
  {
    LOG_DEBUG("vtkDataCollector::GetVideoData: the video buffer is empty, no items will be returned"); 
    return PLUS_SUCCESS;
  }

  PlusStatus status = PLUS_SUCCESS;
  BufferItemUidType oldestItemUid = aSource->GetOldestItemUidInBuffer();
  BufferItemUidType latestItemUid = aSource->GetLatestItemUidInBuffer();
  for (BufferItemUidType itemUid = oldestItemUid; itemUid <= latestItemUid; ++itemUid)
  {
    double itemTimestamp=0;
    if (aSource->GetTimeStamp(itemUid, itemTimestamp)!=ITEM_OK)
    {
      // probably the buffer item is not available anymore
      continue;
    }
    if (itemTimestamp<=aTimestampFrom)
    {
      // this item has been acquired before the requested start time
      continue;
    }
    aTimestampFrom=itemTimestamp;
    // Get tracked frame from buffer
    TrackedFrame* trackedFrame = new TrackedFrame;
    StreamBufferItem currentStreamBufferItem; 
    if ( aSource->GetStreamBufferItem(itemUid, &currentStreamBufferItem) != ITEM_OK )
    {
      LOG_ERROR("Couldn't get video buffer item by frame UID: " << itemUid);
      delete trackedFrame;
      return PLUS_FAIL; 
    }

    // Copy frame 
    trackedFrame->SetImageData(currentStreamBufferItem.GetFrame());
    trackedFrame->SetTimestamp(itemTimestamp);

    // Copy all custom fields
    StreamBufferItem::FieldMapType fieldMap = currentStreamBufferItem.GetCustomFrameFieldMap();
    StreamBufferItem::FieldMapType::iterator fieldIterator;
    for (fieldIterator = fieldMap.begin(); fieldIterator != fieldMap.end(); fieldIterator++)
    {
      trackedFrame->SetCustomFrameField((*fieldIterator).first, (*fieldIterator).second);
    }

    // Add tracked frame to the list 
    if ( aTrackedFrameList->TakeTrackedFrame(trackedFrame, vtkTrackedFrameList::SKIP_INVALID_FRAME) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to add video data to the list!" ); 
      status=PLUS_FAIL; 
    }
  }

  return status; 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::SetLoopTimes()
{
  LOG_TRACE("vtkDataCollector::SetLoopTimes"); 

  double latestLoopStartTime(0);
  double earliestLoopStopTime(0);
  bool isLoopStartStopTimeInitialized=false;

  for( DeviceCollectionIterator it = this->Devices.begin(); it != this->Devices.end(); ++it )
  {
    vtkSavedDataSource* savedDataSource = dynamic_cast<vtkSavedDataSource*>(*it);
    if( savedDataSource == NULL)
    {
      // loops are only set for saved data sources
      continue;
    }
    if(!savedDataSource->GetUseOriginalTimestamps())
    {
      LOG_DEBUG("The device "<<savedDataSource->GetDeviceId()<<" does not use original timestamps, therefore synchronization of loop time is not applicable");
      continue;
    }
    double loopStartTime=0;
    double loopStopTime=0;
    savedDataSource->GetLoopTimeRange(loopStartTime, loopStopTime);
    if (loopStartTime>latestLoopStartTime || !isLoopStartStopTimeInitialized)
    {
      latestLoopStartTime=loopStartTime;
    }
    if (loopStopTime<earliestLoopStopTime || !isLoopStartStopTimeInitialized)
    {
      earliestLoopStopTime=loopStopTime;
    }
    isLoopStartStopTimeInitialized=true;
  }

  if (!isLoopStartStopTimeInitialized)
  {
    LOG_DEBUG("No saved data source devices were found that use original timestamps, so synchronization of loop times is not performed");
    return PLUS_SUCCESS;
  }

  if (latestLoopStartTime>=earliestLoopStopTime)
  {
    LOG_ERROR("Data sets in saved data source devices do not have a common time range. Synchronization of loop times is not possible.");
    return PLUS_FAIL;
  }

  // Set the common loop range for all saved data source devices
  for( DeviceCollectionIterator it = this->Devices.begin(); it != this->Devices.end(); ++it )
  {
    vtkSavedDataSource* savedDataSource = dynamic_cast<vtkSavedDataSource*>(*it);
    if( savedDataSource == NULL)
    {
      // loops are only set for saved data sources
      continue;
    }
    savedDataSource->SetLoopTimeRange(latestLoopStartTime, earliestLoopStopTime);
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetChannel( vtkPlusChannel* &aChannel, const std::string &aChannelId ) const
{
  for( DeviceCollectionConstIterator it = this->Devices.begin(); it != this->Devices.end(); ++it )
  {
    if( (*it)->GetOutputChannelByName(aChannel, aChannelId.c_str()) == PLUS_SUCCESS )
    {
      return PLUS_SUCCESS;
    }
  }

  aChannel = NULL;
  return PLUS_FAIL;
}
