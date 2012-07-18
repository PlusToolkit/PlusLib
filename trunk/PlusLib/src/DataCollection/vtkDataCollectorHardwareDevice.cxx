/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtkDataCollectorHardwareDevice.h"

#include "vtkXMLUtilities.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkXMLDataElement.h"
#include "vtkImageData.h"

#include "vtkTrackedFrameList.h"
#include "TrackedFrame.h"
#include "vtkRfProcessor.h"

#include "vtkTrackerFactory.h"
#include "vtkTracker.h"
#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"
#include "vtkSavedDataTracker.h"

#include "vtkPlusVideoSource.h"
#include "vtkPlusVideoSourceFactory.h"
#include "vtkVideoBuffer.h"
#include "vtkSavedDataVideoSource.h"
#include "vtkUsSimulatorVideoSource.h"

//----------------------------------------------------------------------------
// Signal boxes
#include "vtkSignalBox.h"
//#ifdef PLUS_USE_HEARTSIGNALBOX
//#include "vtkHeartSignalBox.h"
//#endif
//#ifdef PLUS_USE_USBECGBOX
//#include "vtkUSBECGBox.h"
//#endif

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkDataCollectorHardwareDevice, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkDataCollectorHardwareDevice);

vtkCxxSetObjectMacro(vtkDataCollectorHardwareDevice, Tracker, vtkTracker);
vtkCxxSetObjectMacro(vtkDataCollectorHardwareDevice, VideoSource, vtkPlusVideoSource);

//----------------------------------------------------------------------------
vtkDataCollectorHardwareDevice::vtkDataCollectorHardwareDevice()
  : vtkDataCollector()
{	
  this->VideoSource = NULL;
  this->Tracker = NULL; 
}

//----------------------------------------------------------------------------
vtkDataCollectorHardwareDevice::~vtkDataCollectorHardwareDevice()
{
  this->Disconnect();
  this->SetTracker(NULL); 
  this->SetVideoSource(NULL);
}

//----------------------------------------------------------------------------
void vtkDataCollectorHardwareDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  if ( this->Tracker != NULL )
  {
    os << indent << "Tracker: " << std::endl; 
    this->Tracker->PrintSelf(os, indent); 
  }

  if ( this->VideoSource != NULL )
  {
    os << indent << "Video source: " << std::endl; 
    this->VideoSource->PrintSelf(os, indent); 
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorHardwareDevice::Connect()
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::Connect"); 

  if ( this->GetConnected() )
  {
    // Devices already connected
    return PLUS_SUCCESS; 
  }

  PlusStatus status = PLUS_SUCCESS;

  // VideoSource can be null if video source type is None
  if ( this->GetVideoSource() != NULL ) 
  {
    this->GetVideoSource()->Connect();

    if (this->GetVideoSource()->GetConnected() != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to connect to video source!"); 
      status = PLUS_FAIL;
    }
    else
    {
      this->SetInputConnection(this->GetVideoSource()->GetOutputPort());
    }
  }

  // Tracker can be null if the TRACKER_TYPE == TRACKER_NONE 
  if ( this->GetTracker() != NULL ) 
  {
    if ( this->GetTracker()->Connect() != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to initialize tracker!"); 
      status = PLUS_FAIL;
    }
  }

  // Set saved datasets loop time
  if ( this->SetLoopTimes() != PLUS_SUCCESS )
  {
    LOG_WARNING("Failed to set loop times!"); 
    status = PLUS_FAIL;
  }

  if (status == PLUS_SUCCESS)
  {
    this->ConnectedOn(); 
  }
  else
  {
    this->Disconnect();
  }

  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorHardwareDevice::SetLoopTimes()
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::SetLoopTimes"); 

  vtkSavedDataTracker* savedDataTracker = dynamic_cast<vtkSavedDataTracker*>(this->GetTracker()); 

  double oldestTrackerTimeStamp(0);
  double latestTrackerTimeStamp(0);
  double oldestVideoTimeStamp(0);
  double latestVideoTimeStamp(0);

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

  }

  vtkSavedDataVideoSource* savedDataVideoSource = dynamic_cast<vtkSavedDataVideoSource*>(this->GetVideoSource());
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


//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorHardwareDevice::Disconnect()
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::Disconnect"); 

  if ( !this->GetConnected() )
  {
    // Devices already disconnected 
    return PLUS_SUCCESS; 
  }

  if ( this->GetVideoSource() != NULL ) 
  {
    this->GetVideoSource()->Disconnect(); 
  }

  if ( this->GetTracker() != NULL ) 
  {
    this->GetTracker()->Disconnect(); 
  }

  this->ConnectedOff(); 

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorHardwareDevice::Start()
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::Start"); 

  if (this->Tracker == NULL && this->VideoSource == NULL)
  {
    LOG_ERROR("Data collection cannot be started because neither tracker nor imaging device is available!");
    return PLUS_FAIL;
  }

  const double startTime = vtkAccurateTimer::GetSystemTime(); 

  if ( this->GetVideoEnabled() )
  {
    if ( this->GetVideoSource() != NULL && !this->GetVideoSource()->GetRecording())
    {
      if ( this->GetVideoSource()->StartRecording() != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to start video recording!"); 
        return PLUS_FAIL; 
      }
      this->GetVideoSource()->SetStartTime(startTime); 
    }
  }
  else
  {
    LOG_DEBUG("Start data collection in tracking only mode."); 
  }

  if ( this->GetTrackingEnabled() )
  {
    if ( this->GetTracker() != NULL && !this->GetTracker()->IsTracking() )
    {
      if ( this->GetTracker()->StartTracking() != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to start tracking!"); 
        return PLUS_FAIL; 
      }
      this->GetTracker()->SetStartTime(startTime);
    }
  }
  else
  {
    LOG_DEBUG("Start data collection in video only mode."); 
  }

  LOG_DEBUG("vtkDataCollectorHardwareDevice::Start: wait " << std::fixed << this->StartupDelaySec << " sec for buffer init..."); 
  vtkAccurateTimer::Delay(this->StartupDelaySec); 

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorHardwareDevice::Stop()
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::Stop"); 
  if ( this->GetVideoSource() != NULL )
  {
    this->GetVideoSource()->StopRecording();
  }

  if ( this->GetTracker() != NULL )
  {
    this->GetTracker()->StopTracking(); 
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkDataCollectorHardwareDevice::SetLocalTimeOffsetSec(double videoOffsetSec, double trackerOffsetSec)
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::SetLocalTimeOffsetSec");

  if ( this->GetVideoSource() != NULL ) 
  {	
    this->GetVideoSource()->GetBuffer()->SetLocalTimeOffsetSec( videoOffsetSec ); 
    this->GetVideoSource()->WriteConfiguration( vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData() ); 
  }

  if ( this->GetTracker() != NULL ) 
  {
    for ( ToolIteratorType it = this->GetTracker()->GetToolIteratorBegin(); it != this->GetTracker()->GetToolIteratorEnd(); ++it)
    {
      it->second->GetBuffer()->SetLocalTimeOffsetSec( trackerOffsetSec ); 
    }
    this->GetTracker()->WriteConfiguration( vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData() ); 
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorHardwareDevice::GetOldestTimestamp(double &ts)
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::GetOldestTimestamp"); 
  ts=0;

  // ********************* video timestamp **********************
  double oldestVideoTimestamp(0); 
  if ( this->GetVideoEnabled() )
  {
    if ( this->GetVideoSource() == NULL )
    {
      LOG_ERROR("Unable to get most recent timestamp without video source!"); 
      return PLUS_FAIL; 
    }

    if ( this->VideoSource->GetBuffer()->GetOldestTimeStamp(oldestVideoTimestamp) != ITEM_OK )
    {
      LOG_WARNING("Failed to get oldest timestamp from video buffer!"); 
      return PLUS_FAIL; 
    }
  }

  // ********************* tracker timestamp **********************
  double oldestTrackerTimestamp(0); 
  if ( this->GetTrackingEnabled() )
  {
    if ( this->GetTracker() == NULL )
    {
      LOG_ERROR("Unable to get most recent timestamp without tracker!"); 
      return PLUS_FAIL; 
    }

    // Get the first tool
    vtkTrackerTool* firstActiveTool = NULL; 
    if ( this->GetTracker()->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get oldest timestamp from tracker buffer - there is no active tool!"); 
      return PLUS_FAIL; 
    }

    vtkTrackerBuffer* trackerBuffer = firstActiveTool->GetBuffer(); 

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

  if ( !this->GetVideoEnabled() )
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
    if ( this->GetVideoSource()->GetBuffer()->GetItemUidFromTime(oldestVideoTimestamp, videoUid) != ITEM_OK )
    {
      LOG_ERROR("Failed to get video buffer item UID from time: " << std::fixed << oldestVideoTimestamp ); 
      return PLUS_FAIL; 
    }

    if ( videoUid + 1 <= this->GetVideoSource()->GetBuffer()->GetLatestItemUidInBuffer() ) 
    {
      // Always use the next video UID to have an overlap between the two buffers 
      videoUid = videoUid + 1; 
    }

    if ( this->GetVideoSource()->GetBuffer()->GetTimeStamp(videoUid, oldestVideoTimestamp) != ITEM_OK )
    {
      LOG_ERROR("Failed to get video buffer timestamp from UID: " << videoUid); 
      return PLUS_FAIL; 
    }
  }

  ts = oldestVideoTimestamp; 

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorHardwareDevice::GetMostRecentTimestamp(double &ts)
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::GetMostRecentTimestamp"); 
  ts=0;

  double latestVideoTimestamp(0); 
  if ( this->GetVideoEnabled() )
  {
    if ( this->GetVideoSource() == NULL )
    {
      LOG_ERROR("Unable to get most recent timestamp without video source!"); 
      return PLUS_FAIL; 
    }

    // Get the most recent timestamp from the buffer
    if ( this->GetVideoSource()->GetBuffer()->GetLatestTimeStamp(latestVideoTimestamp) != ITEM_OK )
    {
      LOG_WARNING("Unable to get latest timestamp from video buffer!"); 
      return PLUS_FAIL;
    }
  }

  double latestTrackerTimestamp(0); 
  if ( this->GetTrackingEnabled() )
  {
    if ( this->GetTracker() == NULL )
    {
      LOG_ERROR("Unable to get most recent timestamp without tracker!"); 
      return PLUS_FAIL; 
    }

    // Get the first tool
    vtkTrackerTool* firstActiveTool = NULL; 
    if ( this->GetTracker()->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get most recent timestamp from tracker buffer - there is no active tool!"); 
      return PLUS_FAIL; 
    }

    vtkTrackerBuffer* trackerBuffer = firstActiveTool->GetBuffer(); 
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

  if ( !this->GetVideoEnabled() )
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
    if ( this->GetVideoSource()->GetBuffer()->GetItemUidFromTime(latestVideoTimestamp, videoUid) != ITEM_OK )
    {
      LOG_ERROR("Failed to get video buffer item UID from time: " << std::fixed << latestVideoTimestamp ); 
      return PLUS_FAIL; 
    }

    if ( videoUid > 1 ) 
    {
      // Always use the preceding video UID to have time for transform interpolation 
      videoUid = videoUid - 1; 
    }

    if ( this->GetVideoSource()->GetBuffer()->GetTimeStamp(videoUid, latestVideoTimestamp) != ITEM_OK )
    {
      LOG_ERROR("Failed to get video buffer timestamp from UID: " << videoUid); 
      return PLUS_FAIL; 
    }
  }

  ts = latestVideoTimestamp; 

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
int vtkDataCollectorHardwareDevice::GetNumberOfFramesBetweenTimestamps(double aTimestampFrom, double aTimestampTo)
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::GetNumberOfFramesBetweenTimestamps(" << aTimestampFrom << ", " << aTimestampTo << ")");

  int numberOfFrames = 0;

  if ( this->GetVideoEnabled() )
  {
    VideoBufferItem vFromItem; 
    if (this->VideoSource->GetBuffer()->GetVideoBufferItemFromTime(aTimestampFrom, &vFromItem) != ITEM_OK )
    {
      return 0;
    }

    VideoBufferItem vToItem; 
    if (this->VideoSource->GetBuffer()->GetVideoBufferItemFromTime(aTimestampTo, &vToItem) != ITEM_OK )
    {
      return 0;
    }

    numberOfFrames = abs((int)(vToItem.GetUid() - vFromItem.GetUid()));
  }
  else if ( this->TrackingEnabled )
  {
    // Get the first tool
    vtkTrackerTool* firstActiveTool = NULL; 
    if ( this->GetTracker()->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get number of frames between timestamps - there is no active tool!"); 
      return PLUS_FAIL; 
    }
    
    vtkTrackerBuffer* trackerBuffer = firstActiveTool->GetBuffer(); 
    if ( trackerBuffer == NULL )
    {
      LOG_ERROR("Failed to get first active tool!"); 
      return 0; 
    }

    // vtkTrackerBuffer::INTERPOLATED will give the closest item UID  
    TrackerBufferItem tFromItem; 
    if (trackerBuffer->GetTrackerBufferItemFromTime(aTimestampFrom, &tFromItem, vtkTrackerBuffer::INTERPOLATED) != ITEM_OK )
    {
      return 0;
    } 

    // vtkTrackerBuffer::INTERPOLATED will give the closest item UID 
    TrackerBufferItem tToItem; 
    if (trackerBuffer->GetTrackerBufferItemFromTime(aTimestampTo, &tToItem, vtkTrackerBuffer::INTERPOLATED) != ITEM_OK )
    {
      return 0;
    }

    numberOfFrames = abs((int)(tToItem.GetUid() - tFromItem.GetUid())); 
  }

  return numberOfFrames + 1;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorHardwareDevice::GetTrackedFrameList(double& aTimestamp, vtkTrackedFrameList* aTrackedFrameList, int aMaxNumberOfFramesToAdd/*=-1*/, bool aVideoEnabled/*=true*/, bool aTrackingEnabled/*=true*/)
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::GetTrackedFrameList(" << aTimestamp << ", " << aMaxNumberOfFramesToAdd << ", " << (aVideoEnabled?"true":"false") << ", " << (aTrackingEnabled?"true":"false") << ")"); 

  if ( aTrackedFrameList == NULL )
  {
    LOG_ERROR("Unable to get tracked frame list - output tracked frmae list is NULL!"); 
    return PLUS_FAIL; 
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
    LOG_WARNING("Failed to get oldest timestamp from buffer!"); 
    return PLUS_FAIL; 
  }

  if ( aMaxNumberOfFramesToAdd > 0 ) 
  {
    if ( this->VideoEnabled && aVideoEnabled )
    {
      BufferItemUidType mostRecentVideoUid; 
      if ( this->VideoSource->GetBuffer()->GetItemUidFromTime(mostRecentTimestamp, mostRecentVideoUid) != ITEM_OK )
      {
        LOG_ERROR("Failed to get video buffer item by timestamp " << mostRecentTimestamp);
        return PLUS_FAIL;
      }
      if ( mostRecentVideoUid - this->VideoSource->GetBuffer()->GetOldestItemUidInBuffer() > aMaxNumberOfFramesToAdd )
      {
        // Most recent is needed too
        mostRecentVideoUid = mostRecentVideoUid - aMaxNumberOfFramesToAdd + 1; 
      }
      else
      {
        LOG_WARNING("Number of frames in the buffer is less than maxNumberOfFramesToAdd!"); 
      }

      if ( this->VideoSource->GetBuffer()->GetTimeStamp(mostRecentVideoUid, aTimestamp ) != ITEM_OK )
      {
        LOG_ERROR("Failed to get video buffer timestamp from UID: " << mostRecentVideoUid ); 
        return PLUS_FAIL; 
      }
    }
    else if ( this->TrackingEnabled && aTrackingEnabled )
    {
      // Get the first tool
      vtkTrackerTool* firstActiveTool = NULL; 
      if ( this->GetTracker()->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get tracked frame list - there is no active tool!"); 
        return PLUS_FAIL; 
      }

      vtkTrackerBuffer* trackerBuffer = firstActiveTool->GetBuffer(); 
      if ( trackerBuffer == NULL )
      {
        LOG_ERROR("Failed to get first active tool!"); 
        return PLUS_FAIL; 
      }

      BufferItemUidType mostRecentTrackerUid;
      if ( trackerBuffer->GetItemUidFromTime(mostRecentTimestamp, mostRecentTrackerUid ) != ITEM_OK )
      {
        LOG_ERROR("Failed to get tracked buffer item by timestamp " << mostRecentTimestamp);
        return PLUS_FAIL;
      }
      if ( mostRecentTrackerUid - trackerBuffer->GetOldestItemUidInBuffer() > aMaxNumberOfFramesToAdd  )
      {
        // Most recent is needed too
        mostRecentTrackerUid = mostRecentTrackerUid - aMaxNumberOfFramesToAdd + 1; 
      }
      else
      {
        LOG_DEBUG("Number of frames in the buffer is less than maxNumberOfFramesToAdd ("<<aMaxNumberOfFramesToAdd<<")" ); 
      }

      if ( trackerBuffer->GetTimeStamp(mostRecentTrackerUid, aTimestamp ) != ITEM_OK )
      {
        LOG_ERROR("Failed to get tracker buffer timestamp from UID: " << mostRecentTrackerUid ); 
        return PLUS_FAIL; 
      }
    }
  }

  // Check input frameTimestamp to be in a valid range 
  if ( aTimestamp < 0.0001 || aTimestamp > mostRecentTimestamp )
  {
    aTimestamp = mostRecentTimestamp; 
  }
  else if ( oldestTimestamp > aTimestamp )
  {
    aTimestamp = oldestTimestamp; 
  }

  // Determine how many frames to add
  int numberOfFramesSinceTimestamp = GetNumberOfFramesBetweenTimestamps(aTimestamp, mostRecentTimestamp);

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
    aTimestamp = mostRecentTimestamp; 
  }

  for (int i=0; i<numberOfFramesToAdd; ++i)
  {
    // Get tracked frame from buffer
    TrackedFrame trackedFrame; 

    if ( this->GetTrackedFrameByTime(aTimestamp, &trackedFrame) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to get tracked frame by time: " << std::fixed << aTimestamp ); 
      return PLUS_FAIL;
    }

    // Add tracked frame to the list 
    if ( aTrackedFrameList->AddTrackedFrame(&trackedFrame, vtkTrackedFrameList::SKIP_INVALID_FRAME) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to add tracked frame to the list!" ); 
      return PLUS_FAIL; 
    }

    // Get next timestamp
    if ( this->GetVideoEnabled() && i < numberOfFramesToAdd - 1 )
    {
      BufferItemUidType videoUid(0); 
      if ( this->VideoSource->GetBuffer()->GetItemUidFromTime(aTimestamp, videoUid) != ITEM_OK )
      {
        LOG_ERROR("Failed to get video buffer item UID from time: " << std::fixed << aTimestamp ); 
        return PLUS_FAIL; 
      }

      if ( videoUid >= this->VideoSource->GetBuffer()->GetLatestItemUidInBuffer() )
      {
        LOG_WARNING("Requested video uid (" << videoUid+1 << ") is not in the buffer yet!");
        break;
      }

      // Get the timestamp of the next item in the buffer
      if ( this->VideoSource->GetBuffer()->GetTimeStamp(++videoUid, aTimestamp) != ITEM_OK )
      {
        LOG_ERROR("Unable to get timestamp from video buffer by UID: " << videoUid); 
        return PLUS_FAIL;
      }
    }
    else if ( this->GetTrackingEnabled() && i < numberOfFramesToAdd - 1 )
    {
      // Get the first tool
      vtkTrackerTool* firstActiveTool = NULL; 
      if ( this->GetTracker()->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get tracked frame list - there is no active tool!"); 
        return PLUS_FAIL; 
      }

      vtkTrackerBuffer* trackerBuffer = firstActiveTool->GetBuffer(); 
      if ( trackerBuffer == NULL )
      {
        LOG_ERROR("Failed to get first active tool!"); 
        return PLUS_FAIL;
      }

      BufferItemUidType trackerUid(0); 
      if ( trackerBuffer->GetItemUidFromTime(aTimestamp, trackerUid) != ITEM_OK )
      {
        LOG_ERROR("Failed to get tracker buffer item UID from time: " << std::fixed << aTimestamp ); 
        return PLUS_FAIL; 
      }

      if ( trackerUid >= trackerBuffer->GetLatestItemUidInBuffer() )
      {
        LOG_ERROR("Requested tracker uid (" << trackerUid+1 << ") is not in the buffer yet!");
        break;
      }

      // Get the timestamp of the next item in the buffer
      if ( trackerBuffer->GetTimeStamp(++trackerUid, aTimestamp) != ITEM_OK )
      {
        LOG_WARNING("Unable to get timestamp from tracker buffer by UID: " << trackerUid); 
        return PLUS_FAIL;
      }
    }
  }

  return status; 
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollectorHardwareDevice::GetTrackedFrameListSampled(double& aTimestamp, vtkTrackedFrameList* aTrackedFrameList, double aSamplingRateSec)
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::GetTrackedFrameListSampled(" << aTimestamp << ", " << aSamplingRateSec << ")"); 

  if ( aTrackedFrameList == NULL )
  {
    LOG_ERROR("Unable to get tracked frame list - output tracked frmae list is NULL!"); 
    return PLUS_FAIL; 
  }

  // Get latest and oldest timestamp
  double mostRecentTimestamp(0); 
  if ( this->GetMostRecentTimestamp(mostRecentTimestamp) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to get most recent timestamp!"); 
    return PLUS_FAIL; 
  }

  double oldestTimestamp(0); 
  if ( this->GetOldestTimestamp(oldestTimestamp) != PLUS_SUCCESS )
  {
    LOG_WARNING("Failed to get oldest timestamp from buffer!"); 
    return PLUS_FAIL; 
  }

  // Check input frameTimestamp to be in a valid range 
  if ( aTimestamp < 0.0001 || aTimestamp > mostRecentTimestamp )
  {
    aTimestamp = mostRecentTimestamp; 
  }
  else if ( oldestTimestamp > aTimestamp )
  {
    aTimestamp = oldestTimestamp; 
  }

  // Check if there are less frames than it would be needed according to the sampling rate
  int numberOfFramesSinceTimestamp = GetNumberOfFramesBetweenTimestamps(aTimestamp, mostRecentTimestamp);
  int numberOfSampledFrames = (int)((mostRecentTimestamp - aTimestamp) / aSamplingRateSec);

  if (numberOfFramesSinceTimestamp < numberOfSampledFrames)
  {
    LOG_WARNING("The requested sampling rate is faster than the acquisition itself - fewer unique frames will be added to the list!");
  }

  // Add frames to input trackedFrameList
  while (aTimestamp + aSamplingRateSec <= mostRecentTimestamp)
  {
    // Get tracked frame from buffer
    TrackedFrame trackedFrame; 

    if ( this->GetTrackedFrameByTime(aTimestamp, &trackedFrame) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to get tracked frame by time: " << std::fixed << aTimestamp ); 
      return PLUS_FAIL;
    }

    // Add tracked frame to the list 
    if ( aTrackedFrameList->AddTrackedFrame(&trackedFrame, vtkTrackedFrameList::SKIP_INVALID_FRAME) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to add tracked frame to the list!" ); 
      return PLUS_FAIL; 
    }

    // Set timestamp to the next sampled one
    aTimestamp += aSamplingRateSec;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorHardwareDevice::GetTrackedFrame(TrackedFrame* trackedFrame)
{
  //LOG_TRACE("vtkDataCollectorHardwareDevice::GetTrackedFrame - TrackedFrame"); 

  double mostRecentFrameTimestamp(0);
  if (this->GetMostRecentTimestamp(mostRecentFrameTimestamp) != PLUS_SUCCESS) 
  {
    LOG_ERROR("Failed to get most recent timestamp from the buffer!"); 
    return PLUS_FAIL;
  }

  return this->GetTrackedFrameByTime(mostRecentFrameTimestamp, trackedFrame); 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorHardwareDevice::GetTrackedFrameByTime(double time, TrackedFrame* trackedFrame)
{
  //LOG_TRACE("vtkDataCollectorHardwareDevice::GetTrackedFrameByTime - TrackedFrame");
  double synchronizedTime = 0; 

  if ( this->GetVideoEnabled() && this->GetVideoSource() )
  {
    if ( this->GetVideoSource()->GetTrackedFrame(time, trackedFrame) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get tracked frame from video source by time: " << std::fixed << time );
      return PLUS_FAIL;
    }

    synchronizedTime = trackedFrame->GetTimestamp();
  }

  if ( this->GetTrackingEnabled() && this->GetTracker() != NULL )
  {
    if ( !this->GetVideoEnabled() )
    {
      synchronizedTime = time;
    }

    if ( this->GetTracker()->GetTrackedFrame(synchronizedTime, trackedFrame) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get all transforms from tracker at: " << std::fixed << synchronizedTime ); 
      return PLUS_FAIL; 
    }
  }

  // Save tracked frame timestamp 
  trackedFrame->SetTimestamp(synchronizedTime); 

  // Save frame timestamp
  std::ostringstream strTimestamp; 
  strTimestamp << std::fixed << trackedFrame->GetTimestamp(); 
  trackedFrame->SetCustomFrameField("Timestamp", strTimestamp.str()); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
int vtkDataCollectorHardwareDevice::RequestData( vtkInformation* vtkNotUsed( request ), vtkInformationVector**  inputVector, vtkInformationVector* outputVector )
{
  //LOG_TRACE("vtkDataCollectorHardwareDevice::RequestData");

  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkImageData *outData = vtkImageData::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT()) );

  if ( this->GetVideoSource()->GetBuffer()->GetNumberOfItems() < 1 ) 
  {
    int* size = this->GetVideoSource()->GetFrameSize();
    outData->SetExtent( 0, size[0] -1, 0, size[1] - 1, 0, 0);
    outData->SetScalarTypeToUnsignedChar();
    outData->SetNumberOfScalarComponents(1); 
    outData->AllocateScalars(); 
    unsigned long memorysize = size[0]*size[1]*outData->GetScalarSize();
    memset(outData->GetScalarPointer(), 0, memorysize);

    // If the video buffer is empty, we can return immediately 
    LOG_DEBUG("Cannot request data from video source, the video buffer is empty!"); 
    return 1;
  }

  VideoBufferItem currentVideoBufferItem; 
  if ( this->GetVideoSource()->GetBuffer()->GetLatestVideoBufferItem( &currentVideoBufferItem ) != ITEM_OK )
  {
    LOG_WARNING("Failed to get latest video buffer item!"); 
    return 1; 
  }

  outData->DeepCopy(currentVideoBufferItem.GetFrame().GetVtkImage());

  const double globalTime = currentVideoBufferItem.GetTimestamp( this->GetVideoSource()->GetBuffer()->GetLocalTimeOffsetSec() ); 

  if( this->GetTracker() != NULL && this->TrackingEnabled )
  {
    for ( ToolIteratorType it = this->GetTracker()->GetToolIteratorBegin(); it != this->GetTracker()->GetToolIteratorEnd(); ++it)
    {
      if ( it->second->GetBuffer()->GetNumberOfItems() < 1 )
      {
        // If the tracker tool buffer is empty, we can return immediately 
        LOG_DEBUG("Cannot request data from tracker, the tracker tool buffer is empty!"); 
        return 1;
      }

      TrackerBufferItem bufferItem; 
      if ( it->second->GetBuffer()->GetLatestTrackerBufferItem(&bufferItem) != ITEM_OK )
      {
        LOG_ERROR("Failed to get latest tracker buffer item!"); 
        return 1; 
      }

      vtkSmartPointer<vtkMatrix4x4> toolTransMatrix=vtkSmartPointer<vtkMatrix4x4>::New();
      if (bufferItem.GetMatrix(toolTransMatrix)!=PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to get toolTransMatrix"); 
        return 1; 
      }
    }
  }

  return 1;
} 

//------------------------------------------------------------------------------
PlusStatus vtkDataCollectorHardwareDevice::ReadConfiguration(vtkXMLDataElement* aConfigurationData)
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::ReadConfiguration");

  if (aConfigurationData == NULL)
  {
    LOG_ERROR("Input configuration element is invalid");
    return PLUS_FAIL;
  }
  
  if (VerifyDeviceSetConfigurationData(aConfigurationData)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Data collector cannot be instantiated because the device set configuration data is invalid");
    return PLUS_FAIL;
  }

  // Get data collection configuration element
  vtkXMLDataElement* dataCollectionConfig = aConfigurationData->FindNestedElementWithName("DataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  // Read StartupDelaySec
  double startupDelaySec(0.0); 
  if ( dataCollectionConfig->GetScalarAttribute("StartupDelaySec", startupDelaySec) )
  {
    this->SetStartupDelaySec(startupDelaySec); 
    LOG_DEBUG("StartupDelaySec: " << std::fixed << startupDelaySec ); 
  }

  // Read Tracker
  if (this->ReadTrackerProperties(aConfigurationData) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to read tracker configuration!");
    return PLUS_FAIL;
  }

  // Read ImageAcquisition
  //   Note: now it's important to read the tracker configuration first because of the UsSimulatorVideoSource
  //   (later when dealing with the streams - #461 - we hopefully will not need this)
  if (this->ReadImageAcquisitionProperties(aConfigurationData) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to read image acquisition configuration!");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
PlusStatus vtkDataCollectorHardwareDevice::ReadTrackerProperties(vtkXMLDataElement* aConfigurationData)
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::ReadTrackerProperties");

  this->TrackingEnabled = false; 

  vtkXMLDataElement* dataCollectionConfig = aConfigurationData->FindNestedElementWithName("DataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* trackerConfig = dataCollectionConfig->FindNestedElementWithName("Tracker"); 
  if (trackerConfig == NULL) 
  {
    LOG_DEBUG("Cannot find Tracker element in XML tree!");
    return PLUS_SUCCESS;
  }

  LOG_TRACE("vtkDataCollectorHardwareDevice::ReadTrackerProperties");
  const char* type = trackerConfig->GetAttribute("Type"); 
  
  vtkSmartPointer<vtkTrackerFactory> trackerFactory = vtkSmartPointer<vtkTrackerFactory>::New(); 
  if ( trackerFactory->CreateInstance(type, this->Tracker) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to create tracker instance!"); 
    return PLUS_FAIL; 
  }

  if ( this->Tracker )
  {
    this->TrackingEnabled = true; 
    return this->Tracker->ReadConfiguration(aConfigurationData); 
  }
  
  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
PlusStatus vtkDataCollectorHardwareDevice::ReadImageAcquisitionProperties(vtkXMLDataElement* aConfigurationData)
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::ReadImageAcquisitionProperties");

  this->VideoEnabled = false; 

  vtkXMLDataElement* dataCollectionConfig = aConfigurationData->FindNestedElementWithName("DataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* imageAcquisitionConfig = dataCollectionConfig->FindNestedElementWithName("ImageAcquisition"); 
  if (imageAcquisitionConfig == NULL) 
  {
    LOG_DEBUG("Unable to find ImageAcquisition element in configuration XML structure!");
    return PLUS_SUCCESS;
  }

  const char* type = imageAcquisitionConfig->GetAttribute("Type"); 

  vtkSmartPointer<vtkPlusVideoSourceFactory> videoSourceFactory = vtkSmartPointer<vtkPlusVideoSourceFactory>::New(); 
  if ( videoSourceFactory->CreateInstance(type, this->VideoSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to create video source instance!"); 
    return PLUS_FAIL; 
  }

  // Set tracker to the UsSimulatorVideoSource
  //   TODO: Change this when dealing with the streams - #461
  vtkUsSimulatorVideoSource* usSimulatorVideoSource = dynamic_cast<vtkUsSimulatorVideoSource*>(this->VideoSource);
  if (usSimulatorVideoSource)
  {
    if (this->Tracker)
    {
      usSimulatorVideoSource->SetTracker(this->Tracker);
    }
    else
    {
      LOG_ERROR("There is no tracker to set to US Simulator Video Source!");
      return PLUS_FAIL; 
    }
  }

  this->RfProcessor->ReadConfiguration(aConfigurationData);

  if ( this->VideoSource )
  {
    this->VideoEnabled = true; 
    return this->VideoSource->ReadConfiguration(aConfigurationData); 
  }
 
  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
void vtkDataCollectorHardwareDevice::SetTrackingOnly(bool trackingOnly)
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::SetTrackingOnly");
  if ( trackingOnly )
  {
    this->VideoEnabled = false;
  }
  else
  {
    if (this->VideoSource != NULL)
    {
      this->VideoEnabled = true;
    }
  }

  if ( this->GetConnected() && this->VideoSource != NULL )
  {
    if ( this->VideoEnabled )
    {
      this->GetVideoSource()->StartRecording();
    }
    else
    {
      this->GetVideoSource()->StopRecording();
    }
  }
}

//------------------------------------------------------------------------------
void vtkDataCollectorHardwareDevice::SetVideoOnly(bool videoOnly)
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::SetVideoOnly");
  if ( videoOnly )
  {
    this->TrackingEnabled = false;
  }
  else
  {
    if (this->Tracker != NULL)
    {
      this->TrackingEnabled = true;
    }
  }

  if ( this->GetConnected() && this->Tracker != NULL )
  {
    if ( this->TrackingEnabled )
    {
      this->GetTracker()->StartTracking();
    }
    else
    {
      this->GetTracker()->StopTracking();
    }
  }
}

//------------------------------------------------------------------------------
void vtkDataCollectorHardwareDevice::GetFrameSize(int aDim[2])
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::GetFrameSize");

  if (this->VideoSource == NULL)
  {
    LOG_ERROR("Video source is invalid!");
    return;
  }

  this->VideoSource->GetFrameSize(aDim);
}

//------------------------------------------------------------------------------
PlusStatus vtkDataCollectorHardwareDevice::GetTrackerToolReferenceFrame(std::string &aToolReferenceFrameName)
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::GetTrackerToolReferenceFrame");

  // If there is a physical tracker device then get the info from there
  if (this->Tracker != NULL)
  {
    aToolReferenceFrameName = std::string(this->Tracker->GetToolReferenceFrameName());
    return PLUS_SUCCESS;
  }
  
  // Try to find it out from the custom transforms that are stored in the tracked frame
  return GetTrackerToolReferenceFrameFromTrackedFrame(aToolReferenceFrameName);
}

//------------------------------------------------------------------------------
PlusStatus vtkDataCollectorHardwareDevice::GetTrackerToolReferenceFrameFromTrackedFrame(std::string &aToolReferenceFrameName)
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
PlusStatus vtkDataCollectorHardwareDevice::GetFrameRate(double &aFrameRate)
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::GetFrameRate");

  if ( this->VideoEnabled && this->VideoSource )
  {
    aFrameRate = this->VideoSource->GetAcquisitionRate();
  }
  else if ( this->Tracker )
  {
    aFrameRate = this->Tracker->GetAcquisitionRate();
  }
  else
  {
    LOG_ERROR("Neither video nor tracking is enabled!");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}
