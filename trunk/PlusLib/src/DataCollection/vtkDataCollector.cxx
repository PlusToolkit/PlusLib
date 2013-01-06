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
#include "vtkPlusStream.h"
#include "vtkPlusStreamBuffer.h"
#include "vtkPlusStreamTool.h"
#include "vtkSavedDataTracker.h"
#include "vtkSavedDataVideoSource.h"
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
, Connected(false)
, Started(false)
{

}

//----------------------------------------------------------------------------

vtkDataCollector::~vtkDataCollector()
{
  if( Started )
  {
    this->Stop();
  }
  if( Connected )
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
        continue;
      }
      if( factory->CreateInstance(deviceElement->GetAttribute("Type"), device, deviceElement->GetAttribute("Id")) == PLUS_FAIL )
      {    
        LOG_ERROR("Unable to create device: " << deviceElement->GetAttribute("Type"));
      }
      else
      {
        device->ReadConfiguration(aConfig);
        Devices.push_back(device);
      }
    }
  }

  if( Devices.size() == 0 )
  {
    LOG_ERROR("No devices created. Please verify configuration file and any error produced.");
    return PLUS_FAIL;
  }

  for( DeviceCollectionIterator it = Devices.begin(); it != Devices.end(); ++it )
  {
    if( (*it)->GetSelectable() )
    {
      this->SetSelectedDevice((*it)->GetDeviceId());
      break;
    }
  }

  vtkPlusDevice* aDevice;
  if( this->GetSelectedDevice(aDevice) != PLUS_SUCCESS )
  {
    LOG_ERROR("No selectable devices defined. Unable to locate any for data collection.");
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
        continue;
      }

      for ( int i = 0; i < deviceElement->GetNumberOfNestedElements(); ++i )
      {
        vtkXMLDataElement* streamElement = deviceElement->GetNestedElement(i); 
        if( STRCASECMP(streamElement->GetName(), "InputStream") == 0 )
        {
          // We have an input stream, lets find it
          for( DeviceCollectionIterator it = Devices.begin(); it != Devices.end(); ++it )
          {
            vtkPlusDevice* device = (*it);
            vtkPlusStream* aStream = NULL;
            if( device->GetStreamByName(aStream, streamElement->GetAttribute("Id")) == PLUS_SUCCESS )
            {
              // Found it!
              thisDevice->AddInputStream(aStream);
              continue;
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

  Started = true;

  return status;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::Stop()
{
  LOG_TRACE("vtkDataCollector::Stop()");

  Started = false;

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

PlusStatus vtkDataCollector::SetSelectedDevice( const std::string &aDeviceId )
{
  LOG_TRACE("vtkDataCollector::SetSelectedDevice(" << aDeviceId << ")");

  for( DeviceCollectionConstIterator it = Devices.begin(); it != Devices.end(); ++it )
  {
    vtkPlusDevice* device = (*it);
    if( aDeviceId.compare(device->GetDeviceId()) == 0 )
    {
      SelectedDevice = device;
      return PLUS_SUCCESS;
    }
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::GetSelectableDevices( DeviceCollection &OutVector ) const
{
  LOG_TRACE("vtkDataCollector::GetSelectableDevices()");

  OutVector.clear();

  for( DeviceCollectionConstIterator it = Devices.begin(); it != Devices.end(); ++it )
  {
    vtkPlusDevice* device = (*it);
    if( device->GetSelectable() )
    {
      OutVector.push_back(device);
    }
  }

  return OutVector.size() > 0 ? PLUS_SUCCESS : PLUS_FAIL;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::GetSelectedDevice( vtkPlusDevice* &aDevice )
{
  LOG_TRACE("vtkDataCollector::GetSelectedDevice()");

  if( SelectedDevice != NULL )
  {
    aDevice = SelectedDevice;
    return PLUS_SUCCESS;
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

  if( SelectedDevice != NULL )
  {
    return SelectedDevice->GetTrackingDataAvailable();
  }

  return false;
}

//----------------------------------------------------------------------------

bool vtkDataCollector::GetVideoDataAvailable() const
{
  LOG_TRACE("vtkDataCollector::GetVideoDataAvailable()");

  if( SelectedDevice != NULL )
  {
    return SelectedDevice->GetVideoDataAvailable();
  }

  return false;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollector::DumpBuffersToDirectory( const char * aDirectory )
{
  LOG_TRACE("vtkDataCollector::DumpBuffersToDirectory(" << aDirectory << ")");

  // Assemble file names
  std::string dateAndTime = vtksys::SystemTools::GetCurrentDateTime("%Y%m%d_%H%M%S");

  for( DeviceCollectionIterator it = Devices.begin(); it != Devices.end(); ++it )
  {
    vtkPlusDevice* device = *it;

    std::string outputDeviceBufferSequenceFileName = "BufferDump_";
    outputDeviceBufferSequenceFileName.append(device->GetDeviceId());
    outputDeviceBufferSequenceFileName.append("_");
    outputDeviceBufferSequenceFileName.append(dateAndTime);

    LOG_INFO("Write device buffer to " << outputDeviceBufferSequenceFileName);
    device->GetBuffer()->WriteToMetafile( aDirectory, outputDeviceBufferSequenceFileName.c_str(), false); 
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

  if( SelectedDevice != NULL )
  {
    return SelectedDevice->GetTrackedFrameByTime(time, trackedFrame);
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
PlusStatus vtkDataCollector::SetLocalTimeOffsetSec( double trackerLagSec, double videoLagSec )
{
  if( SelectedDevice == NULL )
  {
    LOG_ERROR("No selected stream mixer. Unable to set local time offset.");
    return PLUS_FAIL;
  }

  SelectedDevice->SetLocalTimeOffsetSec(videoLagSec);
  SelectedDevice->SetToolLocalTimeOffsetSec(trackerLagSec);
  return PLUS_SUCCESS;
}

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
  vtkPlusDevice* aDevice = NULL;
  vtkPlusStreamTool* firstActiveTool = NULL; 
  if ( this->GetSelectedDevice(aDevice) != PLUS_SUCCESS )
  {
    LOG_ERROR("No selected stream mixer. Unable to search for tools.");
    return PLUS_FAIL;
  }
  if( aDevice->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
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
    if ( aDevice->GetTrackedFrame(itemTimestamp, trackedFrame) != PLUS_SUCCESS )
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
  vtkPlusDevice* aDevice = NULL;
  if ( this->GetSelectedDevice(aDevice) != PLUS_SUCCESS )
  {
    LOG_ERROR("Selected stream mixer is invalid. No video source present."); 
    return PLUS_FAIL; 
  }
  if ( aDevice->GetBuffer()->GetNumberOfItems()==0 )
  {
    LOG_DEBUG("vtkDataCollector::GetVideoData: the video buffer is empty, no items will be returned"); 
    return PLUS_SUCCESS;
  }

  PlusStatus status = PLUS_SUCCESS;
  BufferItemUidType oldestItemUid=aDevice->GetBuffer()->GetOldestItemUidInBuffer();
  BufferItemUidType latestItemUid=aDevice->GetBuffer()->GetLatestItemUidInBuffer();
  for (BufferItemUidType itemUid=oldestItemUid; itemUid<=latestItemUid; ++itemUid)
  {
    double itemTimestamp=0;
    if (aDevice->GetBuffer()->GetTimeStamp(itemUid, itemTimestamp)!=ITEM_OK)
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
    if ( aDevice->GetTrackedFrame(itemTimestamp, trackedFrame) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to get video frame by time: " << std::fixed << itemTimestamp ); 
      status=PLUS_FAIL;
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

  double oldestTrackerTimeStamp(0);
  double latestTrackerTimeStamp(0);
  vtkSavedDataTracker* savedDataTracker = NULL;

  for( DeviceCollectionIterator it = this->Devices.begin(); it != this->Devices.end(); ++it )
  {
    savedDataTracker = dynamic_cast<vtkSavedDataTracker*>(*it); 

    if ( savedDataTracker != NULL )
    {
      if ( savedDataTracker->GetLocalTrackerBuffer() == NULL ) 
      {
        LOG_ERROR("Failed to get local tracker buffer!"); 
        return PLUS_FAIL;
      }

      if ( savedDataTracker->GetLocalTrackerBuffer()->GetOldestTimeStamp(oldestTrackerTimeStamp) !=  ITEM_OK ) 
      {
        LOG_WARNING("Failed to get oldest timestamp from local tracker buffer!"); 
        return PLUS_FAIL;
      }

      if ( savedDataTracker->GetLocalTrackerBuffer()->GetLatestTimeStamp(latestTrackerTimeStamp) !=  ITEM_OK ) 
      {
        LOG_WARNING("Failed to get latest timestamp from local tracker buffer!"); 
        return PLUS_FAIL;
      }
      break;
    }
  }

  double oldestVideoTimeStamp(0);
  double latestVideoTimeStamp(0);
  vtkSavedDataVideoSource* savedDataVideoSource = NULL;

  for( DeviceCollectionIterator it = this->Devices.begin(); it != this->Devices.end(); ++it )
  {
    savedDataVideoSource = dynamic_cast<vtkSavedDataVideoSource*>(*it);
    if( savedDataVideoSource != NULL)
    {
      if ( savedDataVideoSource->GetLocalVideoBuffer() == NULL ) 
      {
        LOG_ERROR("Failed to get local video buffer!"); 
        return PLUS_FAIL;
      }

      if ( savedDataVideoSource->GetLocalVideoBuffer()->GetOldestTimeStamp(oldestVideoTimeStamp) !=  ITEM_OK ) 
      {
        LOG_WARNING("Failed to get oldest timestamp from local video buffer!"); 
        return PLUS_FAIL;
      }

      if ( savedDataVideoSource->GetLocalVideoBuffer()->GetLatestTimeStamp(latestVideoTimeStamp) !=  ITEM_OK ) 
      {
        LOG_WARNING("Failed to get latest timestamp from local video buffer!"); 
        return PLUS_FAIL;
      }
      break;
    }
  }

  // Item timestamps should computed in the following way for saved datasets (time intersection of the two buffers)
  // itemTimestamp = loopStartTime + (actualTimestamp - startTimestamp) % loopTime 

  // Compute the loop start time 
  double loopStartTime(0);
  if ( oldestVideoTimeStamp > oldestTrackerTimeStamp )
  {
    loopStartTime = oldestVideoTimeStamp; 
  }
  else
  {
    loopStartTime = oldestTrackerTimeStamp; 
  }

  // Compute the loop time 
  double loopTime(0); 
  if ( savedDataVideoSource == NULL )
  {
    loopTime = latestTrackerTimeStamp - loopStartTime;
  }
  else if ( savedDataTracker == NULL )
  {
    loopTime = latestVideoTimeStamp - loopStartTime; 
  }
  else if ( latestVideoTimeStamp > latestTrackerTimeStamp )
  {
    loopTime = latestTrackerTimeStamp - loopStartTime; 
  }
  else
  {
    loopTime = latestVideoTimeStamp - loopStartTime; 
  }

  if ( loopTime < 0 )
  {
    LOG_ERROR("The two saved datasets don't intersect each other!"); 
    return PLUS_FAIL; 
  }

  if ( savedDataVideoSource != NULL )
  {
    savedDataVideoSource->SetLoopStartTime( loopStartTime );
    savedDataVideoSource->SetLoopTime( loopTime );
  }
  if ( savedDataTracker != NULL )
  {
    savedDataTracker->SetLoopStartTime( loopStartTime ); 
    savedDataTracker->SetLoopTime( loopTime );
  }

  return PLUS_SUCCESS; 
}
