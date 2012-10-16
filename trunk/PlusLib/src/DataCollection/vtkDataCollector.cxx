/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "PlusXmlUtils.h"
#include "TrackedFrame.h"
#include "vtkDataCollector.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkPlusDataBuffer.h"
#include "vtkPlusVideoSource.h"
#include "vtkPlusVideoSourceFactory.h"
#include "vtkSavedDataTracker.h"
#include "vtkSavedDataVideoSource.h"
#include "vtkTrackedFrameList.h"
#include "vtkTracker.h"
#include "vtkTrackerFactory.h"
#include "vtkTrackerTool.h"
#include "vtkUsSimulatorVideoSource.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLUtilities.h"

// If a frame cannot be retrieved from the device buffers (because it was overwritten by new frames)
// then we skip a SAMPLING_SKIPPING_MARGIN_SEC long period to allow the application to catch up
static const double SAMPLING_SKIPPING_MARGIN_SEC=0.1; 

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

vtkCxxRevisionMacro(vtkDataCollector, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkDataCollector);

vtkCxxSetObjectMacro(vtkDataCollector, Tracker, vtkTracker);
vtkCxxSetObjectMacro(vtkDataCollector, VideoSource, vtkPlusVideoSource);

//----------------------------------------------------------------------------
vtkDataCollector::vtkDataCollector()
  : vtkObject()
{	
  this->StartupDelaySec = 0.0; 

  this->ConnectedOff(); 

  this->TrackingEnabled = true;
  this->VideoEnabled = true;
  
  this->VideoSource = NULL;
  this->Tracker = NULL; 

  // Create a blank image, it will be used as output if frames are not available
  int blankImageSize[2]={10,10};
  this->BlankImage=vtkImageData::New();
  this->BlankImage->SetExtent( 0, blankImageSize[0]-1, 0, blankImageSize[1]-1, 0, 0);
  this->BlankImage->SetScalarTypeToUnsignedChar();
  this->BlankImage->SetNumberOfScalarComponents(1); 
  this->BlankImage->AllocateScalars(); 
  unsigned long memorysize = blankImageSize[0]*blankImageSize[1]*this->BlankImage->GetScalarSize(); 
  memset(this->BlankImage->GetScalarPointer(), 0, memorysize);   
}

//----------------------------------------------------------------------------
vtkDataCollector::~vtkDataCollector()
{
  this->Disconnect();
  this->SetTracker(NULL); 
  this->SetVideoSource(NULL);
  this->BlankImage->Delete();
  this->BlankImage=NULL;
}

//----------------------------------------------------------------------------
void vtkDataCollector::PrintSelf(ostream& os, vtkIndent indent)
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
PlusStatus vtkDataCollector::Connect()
{
  LOG_TRACE("vtkDataCollector::Connect"); 

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

    /*
    if (this->GetVideoSource()->GetConnected() != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to connect to video source!"); 
      status = PLUS_FAIL;
    }
    else
    {
      this->SetInputConnection(this->GetVideoSource()->GetOutputPort());
    }
    */
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
PlusStatus vtkDataCollector::SetLoopTimes()
{
  LOG_TRACE("vtkDataCollector::SetLoopTimes"); 

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
PlusStatus vtkDataCollector::Disconnect()
{
  LOG_TRACE("vtkDataCollector::Disconnect"); 

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
PlusStatus vtkDataCollector::Start()
{
  LOG_TRACE("vtkDataCollector::Start"); 

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

  LOG_DEBUG("vtkDataCollector::Start: wait " << std::fixed << this->StartupDelaySec << " sec for buffer init..."); 
  vtkAccurateTimer::Delay(this->StartupDelaySec); 

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::Stop()
{
  LOG_TRACE("vtkDataCollector::Stop"); 
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
void vtkDataCollector::SetLocalTimeOffsetSec(double videoOffsetSec, double trackerOffsetSec)
{
  LOG_TRACE("vtkDataCollector::SetLocalTimeOffsetSec");

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
PlusStatus vtkDataCollector::GetOldestTimestamp(double &ts)
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
PlusStatus vtkDataCollector::GetMostRecentTimestamp(double &ts)
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

//----------------------------------------------------------------------------
int vtkDataCollector::GetNumberOfFramesBetweenTimestamps(double aTimestampFrom, double aTimestampTo)
{
  LOG_TRACE("vtkDataCollector::GetNumberOfFramesBetweenTimestamps(" << aTimestampFrom << ", " << aTimestampTo << ")");

  int numberOfFrames = 0;

  if ( this->GetVideoEnabled() )
  {
    DataBufferItem vFromItem; 
    if (this->VideoSource->GetBuffer()->GetDataBufferItemFromTime(aTimestampFrom, &vFromItem, vtkPlusDataBuffer::EXACT_TIME) != ITEM_OK )
    {
      return 0;
    }

    DataBufferItem vToItem; 
    if (this->VideoSource->GetBuffer()->GetDataBufferItemFromTime(aTimestampTo, &vToItem, vtkPlusDataBuffer::EXACT_TIME) != ITEM_OK )
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
    
    vtkPlusDataBuffer* trackerBuffer = firstActiveTool->GetBuffer(); 
    if ( trackerBuffer == NULL )
    {
      LOG_ERROR("Failed to get first active tool!"); 
      return 0; 
    }

    // vtkPlusDataBuffer::INTERPOLATED will give the closest item UID  
    DataBufferItem tFromItem; 
    if (trackerBuffer->GetDataBufferItemFromTime(aTimestampFrom, &tFromItem, vtkPlusDataBuffer::INTERPOLATED) != ITEM_OK )
    {
      return 0;
    } 

    // vtkPlusDataBuffer::INTERPOLATED will give the closest item UID 
    DataBufferItem tToItem; 
    if (trackerBuffer->GetDataBufferItemFromTime(aTimestampTo, &tToItem, vtkPlusDataBuffer::INTERPOLATED) != ITEM_OK )
    {
      return 0;
    }

    numberOfFrames = abs((int)(tToItem.GetUid() - tFromItem.GetUid())); 
  }

  return numberOfFrames + 1;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetTrackedFrameList(double& aTimestamp, vtkTrackedFrameList* aTrackedFrameList, int aMaxNumberOfFramesToAdd/*=-1*/)
{
  LOG_TRACE("vtkDataCollector::GetTrackedFrameList(" << aTimestamp << ", " << aMaxNumberOfFramesToAdd); 

  if ( aTrackedFrameList == NULL )
  {
    LOG_ERROR("Unable to get tracked frame list - output tracked frmae list is NULL!"); 
    return PLUS_FAIL; 
  }

  // If the buffer is empty then don't display an error just return without adding any items to the output tracked frame list
  if ( this->GetVideoEnabled() )
  {
    if ( this->GetVideoSource() == NULL )
    {
      LOG_ERROR("Video source is invalid"); 
      return PLUS_FAIL; 
    }
    if ( this->GetVideoSource()->GetBuffer()->GetNumberOfItems()==0 )
    {
      LOG_DEBUG("vtkDataCollector::GetTrackedFrameList: the video buffer is empty, no items will be returned"); 
      return PLUS_SUCCESS;
    }
  }
  if ( this->GetTrackingEnabled() )
  {
    if ( this->GetTracker() == NULL )
    {
      LOG_ERROR("Tracker is invalid"); 
      return PLUS_FAIL; 
    }
    // Get the first tool
    vtkTrackerTool* firstActiveTool = NULL; 
    if ( this->GetTracker()->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get first active tool"); 
      return PLUS_FAIL; 
    }
    if ( firstActiveTool->GetBuffer()->GetNumberOfItems()==0 )
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
    LOG_WARNING("Failed to get oldest timestamp from buffer!"); 
    return PLUS_FAIL; 
  }

  if ( aMaxNumberOfFramesToAdd > 0 ) 
  {
    if ( this->GetVideoEnabled() )
    {
      BufferItemUidType mostRecentVideoUid = 0; 
      if ( this->VideoSource->GetBuffer()->GetItemUidFromTime(mostRecentTimestamp, mostRecentVideoUid) != ITEM_OK )
      {
        LOG_ERROR("Failed to get video buffer item by timestamp " << mostRecentTimestamp);
        return PLUS_FAIL;
      }
      BufferItemUidType firstVideoUidToAdd = mostRecentVideoUid;
      if ( mostRecentVideoUid - this->VideoSource->GetBuffer()->GetOldestItemUidInBuffer() > aMaxNumberOfFramesToAdd )
      {
        // Most recent is needed too
        firstVideoUidToAdd= mostRecentVideoUid - aMaxNumberOfFramesToAdd + 1; 
      }
      else
      {
        LOG_DEBUG("Number of frames in the buffer is less than maxNumberOfFramesToAdd (more data is allowed to be recorded than it was provided by the data sources)"); 
      }

      if ( this->VideoSource->GetBuffer()->GetTimeStamp(firstVideoUidToAdd, aTimestamp ) != ITEM_OK )
      {
        LOG_ERROR("Failed to get video buffer timestamp from UID: " << firstVideoUidToAdd ); 
        return PLUS_FAIL; 
      }
    }
    else if ( this->GetTrackingEnabled() )
    {
      // Get the first tool
      vtkTrackerTool* firstActiveTool = NULL; 
      if ( this->GetTracker()->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get tracked frame list - there is no active tool!"); 
        return PLUS_FAIL; 
      }

      vtkPlusDataBuffer* trackerBuffer = firstActiveTool->GetBuffer(); 
      if ( trackerBuffer == NULL )
      {
        LOG_ERROR("Failed to get first active tool!"); 
        return PLUS_FAIL; 
      }

      BufferItemUidType mostRecentTrackerUid = 0;
      if ( trackerBuffer->GetItemUidFromTime(mostRecentTimestamp, mostRecentTrackerUid ) != ITEM_OK )
      {
        LOG_ERROR("Failed to get tracked buffer item by timestamp " << mostRecentTimestamp);
        return PLUS_FAIL;
      }
      BufferItemUidType firstTrackerUidToAdd = mostRecentTrackerUid;
      if ( mostRecentTrackerUid - trackerBuffer->GetOldestItemUidInBuffer() > aMaxNumberOfFramesToAdd  )
      {
        // Most recent is needed too
        firstTrackerUidToAdd = mostRecentTrackerUid - aMaxNumberOfFramesToAdd + 1; 
      }
      else
      {
        LOG_DEBUG("Number of frames in the buffer is less than maxNumberOfFramesToAdd ("<<aMaxNumberOfFramesToAdd<<")" ); 
      }

      if ( trackerBuffer->GetTimeStamp(firstTrackerUidToAdd, aTimestamp ) != ITEM_OK )
      {
        LOG_ERROR("Failed to get tracker buffer timestamp from UID: " << firstTrackerUidToAdd ); 
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

      vtkPlusDataBuffer* trackerBuffer = firstActiveTool->GetBuffer(); 
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
  if ( this->GetVideoSource() == NULL )
  {
    LOG_ERROR("Video source is invalid"); 
    return PLUS_FAIL; 
  }
  if ( this->GetVideoSource()->GetBuffer()->GetNumberOfItems()==0 )
  {
    LOG_DEBUG("vtkDataCollector::GetVideoData: the video buffer is empty, no items will be returned"); 
    return PLUS_SUCCESS;
  }

  PlusStatus status = PLUS_SUCCESS;
  BufferItemUidType oldestItemUid=this->GetVideoSource()->GetBuffer()->GetOldestItemUidInBuffer();
  BufferItemUidType latestItemUid=this->GetVideoSource()->GetBuffer()->GetLatestItemUidInBuffer();
  for (BufferItemUidType itemUid=oldestItemUid; itemUid<=latestItemUid; ++itemUid)
  {
    double itemTimestamp=0;
    if (this->GetVideoSource()->GetBuffer()->GetTimeStamp(itemUid, itemTimestamp)!=ITEM_OK)
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
    if ( this->GetVideoSource()->GetTrackedFrame(itemTimestamp, &trackedFrame) != PLUS_SUCCESS )
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

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetTrackingData(double& aTimestampFrom, vtkTrackedFrameList* aTrackedFrameList)
{
  LOG_TRACE("vtkDataCollector::GetTrackingData(" << aTimestampFrom); 

  if ( aTrackedFrameList == NULL )
  {
    LOG_ERROR("Unable to get tracked frame list - output tracked frmae list is NULL"); 
    return PLUS_FAIL; 
  }

  // If the buffer is empty then don't display an error just return without adding any items to the output tracked frame list
  if ( !this->GetTrackingEnabled() )
  {
    LOG_ERROR("Unable to get tracked frame list - Tracking is not enabled"); 
    return PLUS_FAIL; 
  }
  if ( this->GetTracker() == NULL )
  {
    LOG_ERROR("Unable to get tracked frame list - Tracker is invalid"); 
    return PLUS_FAIL; 
  }

  // Get the first tool
  vtkTrackerTool* firstActiveTool = NULL; 
  if ( this->GetTracker()->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to get tracked frame list - there is no active tool!"); 
    return PLUS_FAIL; 
  }

  vtkPlusDataBuffer* trackerBuffer = firstActiveTool->GetBuffer(); 
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
  BufferItemUidType oldestItemUid=trackerBuffer->GetOldestItemUidInBuffer();
  BufferItemUidType latestItemUid=trackerBuffer->GetLatestItemUidInBuffer();
  for (BufferItemUidType itemUid=oldestItemUid; itemUid<=latestItemUid; ++itemUid)
  {
    double itemTimestamp=0;
    if (trackerBuffer->GetTimeStamp(itemUid, itemTimestamp)!=ITEM_OK)
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
    if ( this->GetTracker()->GetTrackedFrame(itemTimestamp, &trackedFrame) != PLUS_SUCCESS )
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

PlusStatus vtkDataCollector::GetTrackedFrameListSampled(double& aTimestamp, vtkTrackedFrameList* aTrackedFrameList, double aSamplingRateSec, double maxTimeLimitSec/*=-1*/)
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
PlusStatus vtkDataCollector::GetTrackedFrame(TrackedFrame* trackedFrame)
{
  //LOG_TRACE("vtkDataCollector::GetTrackedFrame - TrackedFrame"); 

  double mostRecentFrameTimestamp(0);
  if (this->GetMostRecentTimestamp(mostRecentFrameTimestamp) != PLUS_SUCCESS) 
  {
    LOG_ERROR("Failed to get most recent timestamp from the buffer!"); 
    return PLUS_FAIL;
  }

  return this->GetTrackedFrameByTime(mostRecentFrameTimestamp, trackedFrame); 
}

//----------------------------------------------------------------------------
double vtkDataCollector::GetClosestTrackedFrameTimestampByTime(double time)
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
PlusStatus vtkDataCollector::GetTrackedFrameByTime(double time, TrackedFrame* trackedFrame)
{
  //LOG_TRACE("vtkDataCollector::GetTrackedFrameByTime - TrackedFrame");
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
vtkImageData* vtkDataCollector::GetBrightnessOutput()
{
  if (this->VideoSource == NULL)
  {
    LOG_ERROR("Video source is invalid!");
    return this->BlankImage;
  }
  return this->VideoSource->GetBrightnessOutput();
}

//------------------------------------------------------------------------------
PlusStatus vtkDataCollector::ReadConfiguration(vtkXMLDataElement* aConfigurationData)
{
  LOG_TRACE("vtkDataCollector::ReadConfiguration");

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
PlusStatus vtkDataCollector::WriteConfiguration( vtkXMLDataElement* aConfigurationData )
{
  vtkXMLDataElement* dataCollectionConfig = PlusXmlUtils::GetNestedElementWithName(aConfigurationData,"DataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  dataCollectionConfig->SetDoubleAttribute("StartupDelaySec", GetStartupDelaySec());

  PlusStatus status=PLUS_SUCCESS;

  if (this->Tracker!=NULL)
  {
    if (this->Tracker->WriteConfiguration(aConfigurationData) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to save tracker configuration");
      status=PLUS_FAIL;
    }
  }

  if (this->VideoSource!=NULL)
  {
    if (this->VideoSource->WriteConfiguration(aConfigurationData) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to save tracker configuration");
      status=PLUS_FAIL;
    }
  }

  return status;
}

//------------------------------------------------------------------------------
PlusStatus vtkDataCollector::ReadTrackerProperties(vtkXMLDataElement* aConfigurationData)
{
  LOG_TRACE("vtkDataCollector::ReadTrackerProperties");

  this->TrackingEnabled = false; 
  if (this->Tracker!=NULL)
  {
    this->Tracker->Delete();
  }
  this->Tracker=NULL;

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

  LOG_TRACE("vtkDataCollector::ReadTrackerProperties");
  const char* type = trackerConfig->GetAttribute("Type"); 

  if (type==NULL)
  {
    LOG_ERROR("Failed to create video source instance, ImageAcquisition Type attribute is not defined"); 
    return PLUS_FAIL; 
  }
  if (STRCASECMP(type,"None")==NULL)
  {
    LOG_WARNING("Tracker Type=\"None\" is deprecated (since July 19, 2012). Simply skip the Tracker element if no tracking is needed");
    return PLUS_SUCCESS;
  }

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
PlusStatus vtkDataCollector::ReadImageAcquisitionProperties(vtkXMLDataElement* aConfigurationData)
{
  LOG_TRACE("vtkDataCollector::ReadImageAcquisitionProperties");

  this->VideoEnabled = false; 
  if (this->VideoSource!=NULL)
  {
    this->VideoSource->Delete();
  }
  this->VideoSource=NULL;

  vtkXMLDataElement* dataCollectionConfig = aConfigurationData->FindNestedElementWithName("DataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* imageAcquisitionConfig = dataCollectionConfig->FindNestedElementWithName("ImageAcquisition"); 
  if (imageAcquisitionConfig != NULL) 
  {
    const char* deviceType = imageAcquisitionConfig->GetAttribute("Type");
    if (deviceType==NULL)
    {
      LOG_ERROR("Failed to create video source instance, ImageAcquisition Type attribute is not defined"); 
      return PLUS_FAIL; 
    }
    if (STRCASECMP(deviceType,"None")==NULL)
    {
      LOG_WARNING("ImageAcquisition Type=\"None\" is deprecated (since July 19, 2012). Simply skip the ImageAcquisition element if no image acquisition is needed");
      return PLUS_SUCCESS;
    }
    vtkSmartPointer<vtkPlusVideoSourceFactory> videoSourceFactory = vtkSmartPointer<vtkPlusVideoSourceFactory>::New(); 
    if ( videoSourceFactory->CreateInstance(deviceType, this->VideoSource) != PLUS_SUCCESS || this->VideoSource==NULL)
    {
      LOG_ERROR("Failed to create video source instance"); 
      return PLUS_FAIL; 
    }
    if (this->VideoSource->ReadConfiguration(aConfigurationData)!=PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to initialize video source from device set configuration"); 
      return PLUS_FAIL;
    }
  }
  else
  {
    LOG_DEBUG("Unable to find DataCollection/ImageAcquisition element in configuration XML structure!");
    
    // Try if we can find a File element:
    // For compatibility reasons we allow definition of SavedDataVideoSource as 
    // <File SequenceMetafile="xyz.mha" ReplayEnabled="TRUE" />
    vtkXMLDataElement* fileAcquisitionConfig = dataCollectionConfig->FindNestedElementWithName("File"); 
    if (fileAcquisitionConfig==NULL)
    {
      LOG_DEBUG("Unable to find DataCollection/File element in configuration XML structure!");
      return PLUS_SUCCESS;
    }
    
    // There is a File element
    vtkSavedDataVideoSource* savedDataVideoSource=vtkSavedDataVideoSource::New();

    const char* sequenceMetafile = fileAcquisitionConfig->GetAttribute("SequenceMetafile"); 
    if ( sequenceMetafile != NULL ) 
    {
      std::string foundAbsoluteImagePath;
      if (vtkPlusConfig::GetAbsoluteImagePath(sequenceMetafile, foundAbsoluteImagePath) != PLUS_SUCCESS)
      {
        LOG_ERROR("Unable to find SequenceMetafile element in configuration XML structure!");
        return PLUS_FAIL;
      }      
      savedDataVideoSource->SetSequenceMetafile(foundAbsoluteImagePath.c_str());      
    }

    const char* repeatEnabled = fileAcquisitionConfig->GetAttribute("RepeatEnabled"); 
    // Backward compatibility: accept ReplayEnabled instead of RepeatEnabled
    if (repeatEnabled==NULL)
    {
      repeatEnabled = fileAcquisitionConfig->GetAttribute("ReplayEnabled"); 
      if (repeatEnabled!=NULL)
      {
        LOG_WARNING("Deprecated (since July 19, 2012) attribute name is detected: ReplayEnabled. Use RepeatEnabled instead.");
      }
    }
    if ( repeatEnabled != NULL ) 
    {
      if ( STRCASECMP("TRUE", repeatEnabled ) == 0 )
      {
        savedDataVideoSource->RepeatEnabledOn(); 
      }
      else if ( STRCASECMP("FALSE", repeatEnabled ) == 0 )
      {
        savedDataVideoSource->RepeatEnabledOff();
      }
      else
      {
        LOG_WARNING("Unable to recognize RepeatEnabled attribute: " << repeatEnabled << " - changed to false by default!"); 
        savedDataVideoSource->RepeatEnabledOff();
      }
    } 

    savedDataVideoSource->UseOriginalTimestampsOn();
    savedDataVideoSource->UseAllFrameFieldsOn();
        
    this->VideoSource=savedDataVideoSource;
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

  if ( this->VideoSource )
  {
    this->VideoEnabled = true; 
  }
 
  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
void vtkDataCollector::SetTrackingOnly(bool trackingOnly)
{
  LOG_TRACE("vtkDataCollector::SetTrackingOnly");
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
void vtkDataCollector::SetVideoOnly(bool videoOnly)
{
  LOG_TRACE("vtkDataCollector::SetVideoOnly");
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
PlusStatus vtkDataCollector::GetFrameSize(int aDim[2])
{
  LOG_TRACE("vtkDataCollector::GetFrameSize");

  if (this->VideoSource == NULL)
  {
    LOG_ERROR("Video source is invalid!");
    return PLUS_FAIL;
  }

  this->VideoSource->GetFrameSize(aDim);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetBrightnessFrameSize(int aDim[2])
{
  if (this->VideoSource == NULL)
  {
    LOG_ERROR("Video source is invalid!");
    aDim[0]=this->BlankImage->GetExtent()[1]-this->BlankImage->GetExtent()[0];
    aDim[1]=this->BlankImage->GetExtent()[3]-this->BlankImage->GetExtent()[2];
    return PLUS_FAIL;
  }
  
  this->VideoSource->GetBrightnessFrameSize(aDim);
  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetTrackerToolReferenceFrame(std::string &aToolReferenceFrameName)
{
  LOG_TRACE("vtkDataCollector::GetTrackerToolReferenceFrame");

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
PlusStatus vtkDataCollector::GetFrameRate(double &aFrameRate)
{
  LOG_TRACE("vtkDataCollector::GetFrameRate");

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

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::VerifyDeviceSetConfigurationData(vtkXMLDataElement* rootElement)
{
  if (rootElement == NULL)
  {
    LOG_ERROR("DeviceSetConfigurationData is invalid");
    return PLUS_FAIL;
  }
  // Check plus configuration version. There were significant changes in the configuration file structure,
  // so reject processing older ones.
  const double minimumRequiredPlusConfigurationVersion = 1.4;
  double plusConfigurationVersion = 0;
  if (!rootElement->GetScalarAttribute("version", plusConfigurationVersion))
  {
    LOG_ERROR("Version is not specified in the device set configuration. Minimum required version: " 
      << std::fixed << std::setprecision(1) << minimumRequiredPlusConfigurationVersion << ".");
    return PLUS_FAIL;
  }
  if (plusConfigurationVersion < minimumRequiredPlusConfigurationVersion)
  {
    LOG_ERROR("This version ("<< std::fixed << std::setprecision(1) << plusConfigurationVersion
      << ") of the device set configuration is no longer supported. Minimum required version: "
      << minimumRequiredPlusConfigurationVersion << ".");            
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
bool vtkDataCollector::GetTrackingDataAvailable()
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
