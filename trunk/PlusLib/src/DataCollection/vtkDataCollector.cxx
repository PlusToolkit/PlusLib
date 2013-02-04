/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "PlusXmlUtils.h"
#include "TrackedFrame.h"
#include "vtkDataCollector.h"
#include "vtkObjectFactory.h"
#include "vtkPlusDevice.h"
#include "vtkPlusDevice.h"
#include "vtkPlusDeviceFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusStreamBuffer.h"
#include "vtkPlusDataSource.h"
#include "vtkSavedDataSource.h"
#include "vtkTrackedFrameList.h"
#include "vtkXMLDataElement.h"
#include "vtksys/SystemTools.hxx"

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkDataCollector, "$Revision: 2.0$");
vtkStandardNewMacro(vtkDataCollector);

//----------------------------------------------------------------------------

vtkDataCollector::vtkDataCollector()
: vtkObject()
, StartupDelaySec(0.0)
, SelectedDevice(NULL)
, DefaultSelectedDevice(NULL)
, Connected(false)
, Started(false)
{

}

//----------------------------------------------------------------------------

vtkDataCollector::~vtkDataCollector()
{
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

  for ( int i = 0; i < dataCollectionElement->GetNumberOfNestedElements(); ++i )
  {
    vtkXMLDataElement* deviceElement = dataCollectionElement->GetNestedElement(i); 

    if (STRCASECMP(deviceElement->GetName(), "Device") == 0 )
    {
      vtkPlusDevice* device = NULL;
      if( deviceElement->GetAttribute("Id") == NULL )
      {
        LOG_ERROR("Device of type " << deviceElement->GetAttribute("Type") << " with no ID. Unable to continue operating with an incomplete device configuration.");
        return PLUS_FAIL;
      }
      if( factory->CreateInstance(deviceElement->GetAttribute("Type"), device, deviceElement->GetAttribute("Id")) == PLUS_FAIL )
      {    
        LOG_ERROR("Unable to create device: " << deviceElement->GetAttribute("Type"));
        return PLUS_FAIL;
      }
      device->ReadConfiguration(aConfig);
      Devices.push_back(device);
    }
  }

  if( Devices.size() == 0 )
  {
    LOG_ERROR("No devices created. Please verify configuration file and any error produced.");
    return PLUS_FAIL;
  }

  // default device to request data from on connect
  const char* defaultSelectedDeviceId = dataCollectionElement->GetAttribute("DefaultSelectedDeviceId");
  if (defaultSelectedDeviceId != NULL)
  {
    std::string deviceId(defaultSelectedDeviceId);
    vtkPlusDevice* aDevice = NULL;
    if( this->GetDevice(aDevice, deviceId) == PLUS_SUCCESS )
    {
      this->SelectedDevice = aDevice;
    }
  }
  else
  {
    // select the last device by default (usually we are interested in the output of mixer devices,
    // which are mostly defined as the last device)
    this->SelectedDevice = Devices.back();
  }

  vtkPlusChannel* aChannel(NULL);
  if( this->SelectedDevice != NULL && this->GetSelectedChannel(aChannel) != PLUS_SUCCESS )
  {
    LOG_ERROR("No selectable channels defined. Unable to locate any for data collection.");
    return PLUS_FAIL;
  }

  // Connect any and all input streams to their corresponding output streams
  for ( int i = 0; i < dataCollectionElement->GetNumberOfNestedElements(); ++i )
  {
    vtkXMLDataElement* deviceElement = dataCollectionElement->GetNestedElement(i); 

    if (STRCASECMP(deviceElement->GetName(), "Device") == 0 )
    {
      vtkPlusDevice* thisDevice = NULL;
      if( this->GetDevice(thisDevice, deviceElement->GetAttribute("Id")) != PLUS_SUCCESS )
      {
        LOG_ERROR("Device " << deviceElement->GetAttribute("Id") << " doesn't exist.");
        return PLUS_FAIL;
      }

      vtkXMLDataElement* inputChannelsElement = deviceElement->FindNestedElementWithName("InputChannels");

      if (inputChannelsElement != NULL)
      {
        for ( int i = 0; i < inputChannelsElement->GetNumberOfNestedElements(); ++i )
        {
          vtkXMLDataElement* inputChannelElement = inputChannelsElement->GetNestedElement(i); 
          if( STRCASECMP(inputChannelElement->GetName(), "InputChannel") == 0 )
          {
            // We have an input stream, lets find it
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
    }
  }

  for( DeviceCollectionIterator it = this->Devices.begin(); it != this->Devices.end(); ++it )
  {
    (*it)->NotifyConfigured();
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
  vtkAccurateTimer::Delay(this->StartupDelaySec);

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
  this->SelectedDevice = NULL;

  return status;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::GetMostRecentTimestamp( double &ts ) const
{
  LOG_TRACE("vtkDataCollector::GetMostRecentTimestamp()");

  if( SelectedDevice != NULL )
  {
    return SelectedDevice->GetMostRecentTimestamp(ts);
  }

  LOG_ERROR("No selected tracked frame producer. No one to ask for most recent timestamp!");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::GetTrackedFrameList( double& aTimestamp, vtkTrackedFrameList* aTrackedFrameList, int aMaxNumberOfFramesToAdd /*= -1*/ ) const
{
  LOG_TRACE("vtkDataCollector::GetTrackedFrameList(" << aTimestamp << ", aTrackedFrameList, " << aMaxNumberOfFramesToAdd << ")");

  if( this->SelectedDevice != NULL )
  {
    return SelectedDevice->GetTrackedFrameList(aTimestamp, aTrackedFrameList, aMaxNumberOfFramesToAdd);
  }

  LOG_ERROR("No selected tracked frame producer. No one to ask for a tracked frame list!");
  return PLUS_FAIL;
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

bool vtkDataCollector::GetTrackingEnabled() const
{
  LOG_TRACE("vtkDataCollector::GetTrackingEnabled()");

  if( SelectedDevice != NULL )
  {
    return SelectedDevice->GetTrackingDataAvailable();
  }

  return false;
}

//----------------------------------------------------------------------------
bool vtkDataCollector::GetVideoEnabled() const
{
  if( SelectedDevice != NULL )
  {
    return SelectedDevice->GetVideoDataAvailable();
  }

  return false;
}


//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::GetTrackedFrame( TrackedFrame* trackedFrame )
{
  LOG_TRACE("vtkDataCollector::GetTrackedFrame()");

  if( SelectedDevice != NULL )
  {
    return SelectedDevice->GetTrackedFrame(trackedFrame);
  }

  LOG_ERROR("No selected tracked frame producer. No one to ask for a tracked frame!");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::SetSelectedChannel( const std::string &aDeviceId, const std::string& aChannelId )
{
  LOG_TRACE("vtkDataCollector::SetSelectedChannel(" << aDeviceId << ", " << aChannelId << ")");

  vtkPlusChannel* aChannel(NULL);
  if( this->SelectedDevice != NULL && STRCASECMP(aDeviceId.c_str(), this->SelectedDevice->GetDeviceId()) == 0 
    && this->SelectedDevice->GetCurrentChannel(aChannel) == PLUS_SUCCESS && STRCASECMP(aChannel->GetChannelId(), aChannelId.c_str()) == 0 )
  {
    // Channel is already selected
    return PLUS_SUCCESS;
  }

  for( DeviceCollectionConstIterator it = Devices.begin(); it != Devices.end(); ++it )
  {
    vtkPlusDevice* device = (*it);
    if( aDeviceId.compare(device->GetDeviceId()) == 0 )
    {
      this->SelectedDevice = device;
      return this->SelectedDevice->SetCurrentChannel(aChannelId);
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

PlusStatus vtkDataCollector::GetSelectedChannel( vtkPlusChannel* &aChannel )
{
  LOG_TRACE("vtkDataCollector::GetSelectedChannel()");

  if( SelectedDevice != NULL )
  {
    return SelectedDevice->GetCurrentChannel(aChannel);
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------

bool vtkDataCollector::GetConnected() const
{
  return Connected;
}

//----------------------------------------------------------------------------

bool vtkDataCollector::GetTrackingDataAvailable() const
{
  LOG_TRACE("vtkDataCollector::GetTrackingDataAvailable()");

  if( this->SelectedDevice != NULL )
  {
    return this->SelectedDevice->GetTrackingDataAvailable();
  }

  return false;
}

//----------------------------------------------------------------------------

bool vtkDataCollector::GetVideoDataAvailable() const
{
  LOG_TRACE("vtkDataCollector::GetVideoDataAvailable()");

  if( this->SelectedDevice != NULL )
  {
    return this->SelectedDevice->GetVideoDataAvailable();
  }

  return false;
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

    std::string outputDeviceBufferSequenceFileName = "BufferDump_";
    outputDeviceBufferSequenceFileName.append(device->GetDeviceId());
    outputDeviceBufferSequenceFileName.append("_");
    outputDeviceBufferSequenceFileName.append(dateAndTime);


    LOG_INFO("Write device buffer to " << outputDeviceBufferSequenceFileName);
    vtkPlusDataSource* aSource(NULL);
    vtkPlusChannel* aChannel(NULL);
    if( device->GetCurrentChannel(aChannel) && aChannel->GetVideoSource(aSource) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to retrieve the video source in the SavedDataSource device.");
      return PLUS_FAIL;
    }
    aSource->GetBuffer()->WriteToMetafile( aDirectory, outputDeviceBufferSequenceFileName.c_str(), false); 
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
/*
void vtkDataCollector::SetTrackingOnly( bool aValue)
{
VideoEnabled = aValue == true ? false : true;
TrackingEnabled = aValue;
}

//----------------------------------------------------------------------------

void vtkDataCollector::SetVideoOnly( bool aValue )
{
TrackingEnabled = aValue == true ? false : true;
VideoEnabled = aValue;
}
*/

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetTrackedFrameByTime(double time, TrackedFrame* trackedFrame)
{
  LOG_TRACE("vtkDataCollector::GetTrackedFrameByTime()");

  if( this->SelectedDevice != NULL )
  {
    return this->SelectedDevice->GetTrackedFrameByTime(time, trackedFrame);
  }

  LOG_ERROR("No selected device. Unable to get a tracked frame by time when no device available/selected.");
  return PLUS_FAIL;
}

//------------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetFrameSize(int aDim[2])
{
  LOG_TRACE("vtkDataCollector::GetFrameSize");

  if( this->SelectedDevice != NULL )
  {
    return this->SelectedDevice->GetFrameSize(aDim);
  }

  LOG_ERROR("No selected device. Unable to GetFrameSize() when no device available/selected.");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetBrightnessFrameSize(int aDim[2])
{
  LOG_TRACE("vtkDataCollector::GetFrameSize");

  if( this->SelectedDevice != NULL )
  {
    return this->SelectedDevice->GetBrightnessFrameSize(aDim);
  }

  LOG_ERROR("No selected device. Unable to GetBrightnessFrameSize() when no device available/selected.");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
vtkImageData* vtkDataCollector::GetBrightnessOutput()
{
  LOG_TRACE("vtkDataCollector::GetBrightnessOutput");

  if( this->SelectedDevice != NULL )
  {
    return this->SelectedDevice->GetBrightnessOutput();
  }

  LOG_ERROR("No selected device. Unable to GetBrightnessOutput() when no device available/selected.");
  return NULL;
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

//------------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetTrackerToolReferenceFrame(std::string &aToolReferenceFrameName)
{
  LOG_TRACE("vtkDataCollector::GetTrackerToolReferenceFrame");

  // If there is a physical tracker device then get the info from there
  if (this->SelectedDevice != NULL)
  {
    aToolReferenceFrameName = std::string(this->SelectedDevice->GetToolReferenceFrameName());
    return PLUS_SUCCESS;
  }

  // Try to find it out from the custom transforms that are stored in the tracked frame
  return GetTrackerToolReferenceFrameFromTrackedFrame(aToolReferenceFrameName);
}

//------------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetTrackerToolReferenceFrameFromTrackedFrame(std::string &aToolReferenceFrameName)
{
  LOG_TRACE("vtkDataCollectorFile::GetTrackerToolReferenceFrame");

  TrackedFrame trackedFrame;
  if (this->GetTrackedFrame(&trackedFrame) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to get tracked frame!");
    return PLUS_FAIL;
  }

  std::vector<PlusTransformName> transformNames;
  trackedFrame.GetCustomFrameTransformNameList(transformNames);

  if (transformNames.size() == 0)
  {
    LOG_ERROR("No transforms found in tracked frame!");
    return PLUS_FAIL;
  }

  std::string frameName = "";
  for (std::vector<PlusTransformName>::iterator it = transformNames.begin(); it != transformNames.end(); ++it)
  {
    if (frameName == "")
    {
      frameName = it->To();
    }
    else if (frameName != it->To())
    {
      LOG_ERROR("Destination coordinate frame names are not the same!");
      return PLUS_FAIL;
    }
  }

  aToolReferenceFrameName = frameName;

  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
/*
PlusStatus vtkDataCollector::SetLocalTimeOffsetSec( double trackerLagSec, double videoLagSec )
{
if( this->SelectedDevice == NULL )
{
LOG_ERROR("No selected stream mixer. Unable to set local time offset.");
return PLUS_FAIL;
}

this->SelectedDevice->SetImageLocalTimeOffsetSec(videoLagSec);
this->SelectedDevice->SetToolLocalTimeOffsetSec(trackerLagSec);
return PLUS_SUCCESS;
}
*/

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetTrackingData(double& aTimestampFrom, vtkTrackedFrameList* aTrackedFrameList)
{
  LOG_TRACE("vtkDataCollector::GetTrackingData(" << aTimestampFrom); 

  if ( aTrackedFrameList == NULL )
  {
    LOG_ERROR("Unable to get tracked frame list - output tracked frame list is NULL"); 
    return PLUS_FAIL; 
  }

  // If the buffer is empty then don't display an error just return without adding any items to the output tracked frame list
  if ( !this->GetTrackingEnabled() )
  {
    LOG_ERROR("Unable to get tracked frame list - Tracking is not enabled"); 
    return PLUS_FAIL; 
  }
  if ( this->SelectedDevice == NULL )
  {
    LOG_ERROR("Unable to get tracked frame list - stream mixer is invalid"); 
    return PLUS_FAIL; 
  }

  // Get the first tool
  vtkPlusChannel* aChannel(NULL);
  vtkPlusDataSource* firstActiveTool = NULL; 
  if ( this->GetSelectedChannel(aChannel) != PLUS_SUCCESS )
  {
    LOG_ERROR("No selected channel. Unable to search for tools.");
    return PLUS_FAIL;
  }
  if( aChannel->GetOwnerDevice()->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to get tracked frame list - there is no active tool!"); 
    return PLUS_FAIL; 
  }

  vtkPlusStreamBuffer* trackerBuffer = firstActiveTool->GetBuffer(); 
  if ( trackerBuffer == NULL )
  {
    LOG_ERROR("Unable to get tracked frame list - Failed to get first active tool!"); 
    return PLUS_FAIL; 
  }

  if ( trackerBuffer->GetNumberOfItems()==0 )
  {
    LOG_DEBUG("vtkDataCollector::GetTrackingData: the tracking buffer is empty, no items will be returned"); 
    return PLUS_SUCCESS;
  }

  PlusStatus status = PLUS_SUCCESS;
  BufferItemUidType oldestItemUid = trackerBuffer->GetOldestItemUidInBuffer();
  BufferItemUidType latestItemUid = trackerBuffer->GetLatestItemUidInBuffer();
  for (BufferItemUidType itemUid = oldestItemUid; itemUid <= latestItemUid; ++itemUid)
  {
    double itemTimestamp=0;
    if (trackerBuffer->GetTimeStamp(itemUid, itemTimestamp)!=ITEM_OK)
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
    TrackedFrame trackedFrame; 
    // TODO : move get tracked frame logic to channel
    if ( aChannel->GetOwnerDevice()->GetTrackedFrame(itemTimestamp, trackedFrame, false /* get tracking data only */ ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to get tracking data by time: " << std::fixed << itemTimestamp ); 
      status=PLUS_FAIL;
    }
    // Add tracked frame to the list 
    if ( aTrackedFrameList->AddTrackedFrame(&trackedFrame, vtkTrackedFrameList::SKIP_INVALID_FRAME) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to add tracking data to the list!" ); 
      status=PLUS_FAIL; 
    }
  }

  return status; 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetVideoData(double& aTimestampFrom, vtkTrackedFrameList* aTrackedFrameList)
{
  LOG_TRACE("vtkDataCollector::GetVideoData(" << aTimestampFrom); 

  if ( aTrackedFrameList == NULL )
  {
    LOG_ERROR("Unable to get tracked frame list - output tracked frmae list is NULL"); 
    return PLUS_FAIL; 
  }

  // If the buffer is empty then don't display an error just return without adding any items to the output tracked frame list
  if ( !this->GetVideoEnabled() )
  {
    LOG_ERROR("Unable to get tracked frame list - video is not enabled"); 
    return PLUS_FAIL; 
  }
  vtkPlusChannel* aChannel(NULL);
  if ( this->GetSelectedChannel(aChannel) != PLUS_SUCCESS )
  {
    LOG_ERROR("Selected channel is invalid. No video source present."); 
    return PLUS_FAIL; 
  }
  vtkPlusDataSource* aSource(NULL);
  if ( aChannel->GetVideoSource(aSource) == PLUS_SUCCESS && aSource->GetBuffer()->GetNumberOfItems()==0 )
  {
    LOG_DEBUG("vtkDataCollector::GetVideoData: the video buffer is empty, no items will be returned"); 
    return PLUS_SUCCESS;
  }

  PlusStatus status = PLUS_SUCCESS;
  BufferItemUidType oldestItemUid=aSource->GetBuffer()->GetOldestItemUidInBuffer();
  BufferItemUidType latestItemUid=aSource->GetBuffer()->GetLatestItemUidInBuffer();
  for (BufferItemUidType itemUid=oldestItemUid; itemUid<=latestItemUid; ++itemUid)
  {
    double itemTimestamp=0;
    if (aSource->GetBuffer()->GetTimeStamp(itemUid, itemTimestamp)!=ITEM_OK)
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
    TrackedFrame trackedFrame; 

    StreamBufferItem currentStreamBufferItem; 
    if ( aSource->GetBuffer()->GetStreamBufferItem(itemUid, &currentStreamBufferItem) != ITEM_OK )
    {
      LOG_ERROR("Couldn't get video buffer item by frame UID: " << itemUid); 
      return PLUS_FAIL; 
    }

    // Copy frame 
    PlusVideoFrame frame = currentStreamBufferItem.GetFrame(); 
    trackedFrame.SetImageData(frame);
    trackedFrame.SetTimestamp(itemTimestamp);

    // Copy all custom fields
    StreamBufferItem::FieldMapType fieldMap = currentStreamBufferItem.GetCustomFrameFieldMap();
    StreamBufferItem::FieldMapType::iterator fieldIterator;
    for (fieldIterator = fieldMap.begin(); fieldIterator != fieldMap.end(); fieldIterator++)
    {
      trackedFrame.SetCustomFrameField((*fieldIterator).first, (*fieldIterator).second);
    }

    // Add tracked frame to the list 
    if ( aTrackedFrameList->AddTrackedFrame(&trackedFrame, vtkTrackedFrameList::SKIP_INVALID_FRAME) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to add video data to the list!" ); 
      status=PLUS_FAIL; 
    }
  }

  return status; 
}

//------------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetFrameRate( double& frameRate ) const
{
  if( this->SelectedDevice != NULL )
  {
    frameRate = this->SelectedDevice->GetAcquisitionRate();
    return PLUS_SUCCESS;
  }

  LOG_ERROR("No selected mixer when requesting the framerate. Nothing to poll!");
  return PLUS_FAIL;
}

//------------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetTrackedFrameListSampled( double& aTimestamp, vtkTrackedFrameList* aTrackedFrameList, double aSamplingRateSec, double maxTimeLimitSec/*=-1*/ )
{
  if( this->SelectedDevice != NULL )
  {
    return this->SelectedDevice->GetTrackedFrameListSampled(aTimestamp, aTrackedFrameList, aSamplingRateSec, maxTimeLimitSec);
  }

  LOG_ERROR("No selected mixer when GetTrackedFrameListSampled(). Nothing to poll!");
  return PLUS_FAIL;
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
