/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "vtkObjectFactory.h"
#include "vtkPlusStream.h"
#include "vtkVirtualStreamMixer.h"

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkVirtualStreamMixer, "$Revision: 1.0$");
vtkStandardNewMacro(vtkVirtualStreamMixer);

//----------------------------------------------------------------------------
vtkVirtualStreamMixer::vtkVirtualStreamMixer()
: vtkPlusDevice()
{
}

//----------------------------------------------------------------------------
vtkVirtualStreamMixer::~vtkVirtualStreamMixer()
{
}

//----------------------------------------------------------------------------
void vtkVirtualStreamMixer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
bool vtkVirtualStreamMixer::GetTrackingDataAvailable() const
{
  // TODO : implement this
  // check for transforms within the input stream
  return true;
}

//----------------------------------------------------------------------------
bool vtkVirtualStreamMixer::GetVideoDataAvailable() const
{
  // Check if input stream buffers contains plusvideoframe with extents != -1, -1
  // if so, we have video data
  // TODO : implement this
  return true;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamMixer::GetMostRecentTimestamp( double & ts ) const
{
  // TODO : implement this
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamMixer::GetTrackedFrameList( double& aTimestamp, vtkTrackedFrameList* aTrackedFrameList, int aMaxNumberOfFramesToAdd )
{
  // TODO : implement this
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamMixer::GetTrackedFrame( double timestamp, TrackedFrame *trackedFrame )
{
  // TODO : implement this
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamMixer::GetTrackedFrame( TrackedFrame *trackedFrame )
{
  // TODO : implement this
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamMixer::GetTrackedFrameByTime(double time, TrackedFrame* trackedFrame)
{
  LOG_TRACE("vtkDataCollector::GetTrackedFrameByTime");
  double synchronizedTime = 0; 

  // For each in stream, get the timestamp
  // If all streams can return a tracked frame for the timestamp, success
  /*
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
*/
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
vtkPlusStream* vtkVirtualStreamMixer::GetStream() const
{
  // Virtual stream mixers always have exactly one output stream
  return this->OutputStreams[0];
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamMixer::InternalUpdate()
{
  // TODO : take input streams, merge data, put in output stream
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamMixer::Reset()
{
  Superclass::Reset();

  for( StreamContainerIterator it = this->InputStreams.begin(); it != this->InputStreams.end(); ++it )
  {
    vtkPlusStream* str = (*it);
    if( str->GetOwnerDevice()->IsTracker() )
    {
      str->GetOwnerDevice()->Reset();
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
double vtkVirtualStreamMixer::GetAcquisitionRate() const
{
  // TODO : Which device to choose from...
}

//----------------------------------------------------------------------------

PlusStatus vtkVirtualStreamMixer::GetTrackedFrameListSampled(double& aTimestamp, vtkTrackedFrameList* aTrackedFrameList, double aSamplingRateSec, double maxTimeLimitSec/*=-1*/)
{
  LOG_TRACE("vtkDataCollector::GetTrackedFrameListSampled(" << aTimestamp << ", " << aSamplingRateSec << ")"); 

  if ( aTrackedFrameList == NULL )
  {
    LOG_ERROR("Unable to get tracked frame list - output tracked frmae list is NULL!"); 
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
    LOG_WARNING("Unable to add frames at the requested sampling rate because the acquisition frame rate ("<<numberOfFramesSinceTimestamp/(mostRecentTimestamp - aTimestamp)<<") is lower than the requested sampling rate ("<<1.0/aSamplingRateSec<<"fps). Reduce the sampling rate or increase the acquisition rate to resolve the issue.");
  }

  PlusStatus status=PLUS_SUCCESS;
  // Add frames to input trackedFrameList
  double latestAddedTimestamp=UNDEFINED_TIMESTAMP;
  for (;aTimestamp + aSamplingRateSec <= mostRecentTimestamp && (vtkAccurateTimer::GetSystemTime() - startTimeSec)<maxTimeLimitSec; aTimestamp += aSamplingRateSec)
  {
    double oldestTimestamp=0;
    if ( this->GetOldestTimestamp(oldestTimestamp) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get oldest timestamp from buffer!"); 
      return PLUS_FAIL; 
    }
    const double skippingMargin=ceil(SAMPLING_SKIPPING_MARGIN_SEC/aSamplingRateSec)*aSamplingRateSec;
    if (aTimestamp<oldestTimestamp+skippingMargin)
    {
      // the frame will be removed from the buffer really soon, so instead of trying to retrieve from the buffer and failing skip some frames
      double skipTime=ceil((oldestTimestamp+skippingMargin+aSamplingRateSec-aTimestamp)/aSamplingRateSec)*aSamplingRateSec;
      aTimestamp+=skipTime;
      LOG_WARNING("Frames are not available any more at time: " << std::fixed << aTimestamp <<". Skipping "<<skipTime<<" seconds."); 
      continue;
    }
    double closestTimestamp=GetClosestTrackedFrameTimestampByTime(aTimestamp);
    if ( latestAddedTimestamp!=UNDEFINED_TIMESTAMP && closestTimestamp!=UNDEFINED_TIMESTAMP && closestTimestamp <= latestAddedTimestamp )
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
int vtkVirtualStreamMixer::GetNumberOfFramesBetweenTimestamps(double aTimestampFrom, double aTimestampTo)
{
  LOG_TRACE("vtkDataCollector::GetNumberOfFramesBetweenTimestamps(" << aTimestampFrom << ", " << aTimestampTo << ")");

  int numberOfFrames = 0;

  if ( this->GetVideoEnabled() )
  {
    StreamBufferItem vFromItem; 
    if (this->VideoSource->GetBuffer()->GetDataBufferItemFromTime(aTimestampFrom, &vFromItem, vtkPlusDataBuffer::EXACT_TIME) != ITEM_OK )
    {
      return 0;
    }

    StreamBufferItem vToItem; 
    if (this->VideoSource->GetBuffer()->GetDataBufferItemFromTime(aTimestampTo, &vToItem, vtkPlusDataBuffer::EXACT_TIME) != ITEM_OK )
    {
      return 0;
    }

    numberOfFrames = abs((int)(vToItem.GetUid() - vFromItem.GetUid()));
  }
  else if ( this->GetTrackingDataAvailable() )
  {
    // Get the first tool
    vtkTrackerTool* firstActiveTool = NULL; 
    if ( this->GetTracker()->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
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
    if (trackerBuffer->GetDataBufferItemFromTime(aTimestampFrom, &tFromItem, vtkPlusStreamBuffer::INTERPOLATED) != ITEM_OK )
    {
      return 0;
    } 

    // vtkPlusStreamBuffer::INTERPOLATED will give the closest item UID 
    StreamBufferItem tToItem; 
    if (trackerBuffer->GetDataBufferItemFromTime(aTimestampTo, &tToItem, vtkPlusStreamBuffer::INTERPOLATED) != ITEM_OK )
    {
      return 0;
    }

    numberOfFrames = abs((int)(tToItem.GetUid() - tFromItem.GetUid())); 
  }

  return numberOfFrames + 1;
}

//----------------------------------------------------------------------------
double vtkVirtualStreamMixer::GetClosestTrackedFrameTimestampByTime(double time)
{

  if ( this->GetVideoEnabled() && this->GetVideoSource() )
  {
    BufferItemUidType uid=0;
    if (this->GetVideoSource()->GetBuffer()->GetItemUidFromTime(time, uid)!=ITEM_OK)
    {
      return UNDEFINED_TIMESTAMP;
    }    
    double closestTimestamp = UNDEFINED_TIMESTAMP; 
    if ( this->GetVideoSource()->GetBuffer()->GetTimeStamp(uid, closestTimestamp)!=ITEM_OK)
    {
      return UNDEFINED_TIMESTAMP;
    }
    return closestTimestamp;
  }

  if ( this->GetTrackingEnabled() && this->GetTracker() != NULL )
  {
    // Get the first tool
    vtkTrackerTool* firstActiveTool = NULL; 
    if ( this->GetTracker()->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
    {
      // there is no active tool
      return UNDEFINED_TIMESTAMP; 
    }
    vtkPlusDataBuffer* trackerBuffer = firstActiveTool->GetBuffer(); 
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
PlusStatus vtkVirtualStreamMixer::GetOldestTimestamp(double &ts)
{
  LOG_TRACE("vtkDataCollector::GetOldestTimestamp"); 
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

    vtkPlusDataBuffer* trackerBuffer = firstActiveTool->GetBuffer(); 

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
PlusStatus vtkVirtualStreamMixer::GetMostRecentTimestamp(double &ts)
{
  LOG_TRACE("vtkDataCollector::GetMostRecentTimestamp"); 
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

    vtkPlusDataBuffer* trackerBuffer = firstActiveTool->GetBuffer(); 
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