/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtkDataCollectorHardwareDevice.h"

#include "vtkXMLUtilities.h"
#include "vtkTrackerTool.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkXMLDataElement.h"
#include "vtkImageData.h"
#include "vtkVideoBuffer.h"
#include "vtkTrackerBuffer.h"
#include "vtkTrackedFrameList.h"
#include "vtkDataCollectorSynchronizer.h"
#include "vtkTracker.h"
#include "vtkPlusVideoSource.h"

//----------------------------------------------------------------------------
// Tracker devices
#include "vtkTracker.h"
#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"
#ifdef PLUS_USE_POLARIS
#include "vtkPOLARISTracker.h"
#include "vtkNDITracker.h"
#endif
#ifdef PLUS_USE_CERTUS
#include "vtkNDICertusTracker.h"
#endif
#ifdef PLUS_USE_MICRONTRACKER
#include "vtkMicronTracker.h"
#endif
#ifdef PLUS_USE_BRACHY_TRACKER
#include "vtkBrachyTracker.h"
#endif
#ifdef PLUS_USE_Ascension3DG
#include "vtkAscension3DGTracker.h"
#endif
#include "vtkFakeTracker.h"
#include "vtkSavedDataTracker.h"

//----------------------------------------------------------------------------
// Video source
#include "vtkPlusVideoSource.h"
//#ifdef PLUS_USE_MATROX_IMAGING
//#include "vtkMILVideoSource2.h"
//#endif
#ifdef WIN32
#ifdef VTK_VFW_SUPPORTS_CAPTURE
#include "vtkWin32VideoSource2.h"
#endif
//#else
//#ifdef USE_LINUX_VIDEO
//#include "vtkV4L2VideoSource2.h"
//#endif
#endif
#ifdef PLUS_USE_ULTRASONIX_VIDEO
#include "vtkSonixVideoSource.h"
#include "vtkSonixPortaVideoSource.h"
#endif
#ifdef PLUS_USE_ICCAPTURING_VIDEO
#include "vtkICCapturingSource.h"
#endif
#include "vtkSavedDataVideoSource.h"

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

vtkCxxSetObjectMacro(vtkDataCollectorHardwareDevice, Synchronizer, vtkDataCollectorSynchronizer);
vtkCxxSetObjectMacro(vtkDataCollectorHardwareDevice, Tracker, vtkTracker);
vtkCxxSetObjectMacro(vtkDataCollectorHardwareDevice, VideoSource, vtkPlusVideoSource);

//----------------------------------------------------------------------------
vtkDataCollectorHardwareDevice::vtkDataCollectorHardwareDevice()
  : vtkDataCollector()
{	
  this->VideoSource = NULL;
  this->Tracker = NULL; 
  this->Synchronizer = NULL; 
  this->CancelSyncRequestOff(); 
}

//----------------------------------------------------------------------------
vtkDataCollectorHardwareDevice::~vtkDataCollectorHardwareDevice()
{
  this->SetTracker(NULL); 
  this->SetVideoSource(NULL);
  this->SetSynchronizer(NULL); 
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

  // VideoSource can be null if the ACQUISITION_TYPE == SYNCHRO_VIDEO_NONE 
  if ( this->GetVideoSource() != NULL ) 
  {
    this->GetVideoSource()->Connect();

    if (!this->GetVideoSource()->GetConnected())
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
    if ( !this->GetTracker()->Connect() )
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

  if ( (this->GetTrackerType() != TRACKER_SAVEDDATASET) && (this->GetAcquisitionType() != SYNCHRO_VIDEO_SAVEDDATASET) )
  {
    // No need to compute loop time
    return PLUS_SUCCESS;
  }

  vtkSavedDataTracker* savedDataTracker = NULL;
  vtkSavedDataVideoSource* savedDataVideoSource = NULL;

  double oldestTrackerTimeStamp(0);
  double latestTrackerTimeStamp(0);
  double oldestVideoTimeStamp(0);
  double latestVideoTimeStamp(0);

  if ( this->GetTrackerType() == TRACKER_SAVEDDATASET )
  {
    savedDataTracker = dynamic_cast<vtkSavedDataTracker*>(this->GetTracker()); 

    if ( savedDataTracker == NULL )
    {
      LOG_ERROR("Failed to dynamic cast saved data tracker!"); 
      return PLUS_FAIL; 
    }

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

  if( this->GetAcquisitionType() == SYNCHRO_VIDEO_SAVEDDATASET )
  {
    savedDataVideoSource = dynamic_cast<vtkSavedDataVideoSource*>(this->GetVideoSource()); 

    if ( savedDataVideoSource == NULL )
    {
      LOG_ERROR("Failed to dynamic cast saved data video source!"); 
      return PLUS_FAIL; 
    }

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
    LOG_ERROR("Data collection cannot be started because devices have not been initialized properly!");
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
      this->GetVideoSource()->GetBuffer()->SetStartTime(startTime); 
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

  LOG_DEBUG("vtkDataCollectorHardwareDevice::Start: wait " << std::fixed << this->GetStartupDelaySec() << " sec for buffer init..."); 
  vtkAccurateTimer::Delay(this->GetStartupDelaySec()); 

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
PlusStatus vtkDataCollectorHardwareDevice::Synchronize( const char* bufferOutputFolder /*= NULL*/ , bool acquireDataOnly /*= false*/ )
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::Synchronize"); 

  if ( this->GetSynchronizer() == NULL )
  {
    LOG_WARNING("Unable to synchronize without a synchronizer! Please set synchronizer type in the configuration file!"); 
    return PLUS_FAIL; 
  }

  if ( this->GetTracker() == NULL ) 
  {	
    LOG_ERROR("Unable to synchronize tracker - there is no tracker selected!"); 
    return PLUS_FAIL; 
  }

  if ( this->GetVideoSource() == NULL ) 
  {	
    LOG_ERROR("Unable to synchronize video - there is no video source selected!"); 
    return PLUS_FAIL; 
  }

  this->CancelSyncRequestOff(); 

  if ( this->GetTracker()->GetToolIteratorBegin() == this->GetTracker()->GetToolIteratorEnd() )
  {
    LOG_ERROR("Failed to synchronize - there are no active tools!"); 
    return PLUS_FAIL; 
  }

  // Get the first tool
  vtkTrackerTool* firstActiveTool = this->GetTracker()->GetToolIteratorBegin()->second; 

  //************************************************************************************
  // Save local time offsets before sync
  const double prevVideoOffset = this->GetVideoSource()->GetBuffer()->GetLocalTimeOffset(); 
  const double prevTrackerOffset = firstActiveTool->GetBuffer()->GetLocalTimeOffset(); 

  //************************************************************************************
  // Set the local timeoffset to 0 before synchronization 
  this->SetLocalTimeOffset(0, 0); 

  //************************************************************************************
  // Set the length of the acquisition 
  const double syncTimeLength = this->GetSynchronizer()->GetSynchronizationTimeLength(); 

  // Get the realtime tracking frequency
  double trackerFrameRate = firstActiveTool->GetBuffer()->GetFrameRate(); 

  // Get the realtime video frame rate
  double videoFrameRate = this->GetVideoSource()->GetBuffer()->GetFrameRate(); 

  const int trackerBufferSize = firstActiveTool->GetBuffer()->GetBufferSize(); 
  const int videoBufferSize = this->GetVideoSource()->GetBuffer()->GetBufferSize(); 
  int syncTrackerBufferSize = trackerFrameRate * syncTimeLength + 100; 
  int syncVideoBufferSize = videoFrameRate * syncTimeLength + 100; 

  //************************************************************************************
  // Change buffer size to fit the whole acquisition 
  if ( syncVideoBufferSize > videoBufferSize )
  {
    LOG_DEBUG("Change video buffer size to: " << syncVideoBufferSize); 
    if ( this->GetVideoSource()->SetFrameBufferSize(syncVideoBufferSize) != PLUS_SUCCESS )
    {
      LOG_WARNING("Failed to change video buffer size!"); 
    }
    this->GetVideoSource()->GetBuffer()->Clear(); 
  }
  else
  {
    this->GetVideoSource()->GetBuffer()->Clear(); 
  }

  if ( syncTrackerBufferSize > trackerBufferSize )
  {
    LOG_DEBUG("Change tracker buffer size to: " << syncTrackerBufferSize); 
    for ( ToolIteratorType it = this->GetTracker()->GetToolIteratorBegin(); it != this->GetTracker()->GetToolIteratorEnd(); ++it)
    {
      it->second->GetBuffer()->SetBufferSize(syncTrackerBufferSize); 
      it->second->GetBuffer()->Clear(); 
    }
  }
  else
  {
    this->GetTracker()->ClearAllBuffers();
  }

  //************************************************************************************
  // Acquire data 
  const double syncStartTime = vtkAccurateTimer::GetSystemTime(); 
  while ( syncStartTime + syncTimeLength > vtkAccurateTimer::GetSystemTime() )
  {
    if ( this->CancelSyncRequest ) 
    {
      // we should cancel the job...
      this->SetLocalTimeOffset(prevVideoOffset, prevTrackerOffset); 
      return PLUS_FAIL; 
    }

    const int percent = floor(100*(vtkAccurateTimer::GetSystemTime() - syncStartTime) / syncTimeLength); 

    if ( this->ProgressBarUpdateCallbackFunction != NULL )
    {
      (*ProgressBarUpdateCallbackFunction)(percent); 
    }

    vtkAccurateTimer::Delay(0.1); 
  }

  if ( this->ProgressBarUpdateCallbackFunction != NULL )
  {
    (*ProgressBarUpdateCallbackFunction)(100); 
  }

  //************************************************************************************
  // Copy buffers to local buffer
  vtkSmartPointer<vtkVideoBuffer> videobuffer = vtkSmartPointer<vtkVideoBuffer>::New(); 
  if ( this->VideoSource != NULL ) 
  {
    LOG_DEBUG("Copy video buffer ..."); 
    videobuffer->DeepCopy(this->VideoSource->GetBuffer());
  }

  vtkSmartPointer<vtkTracker> tracker = vtkSmartPointer<vtkTracker>::New(); 
  if ( this->Tracker != NULL )
  {
    LOG_DEBUG("Copy tracker ..."); 
    tracker->DeepCopy(this->Tracker); 
  }

  if ( acquireDataOnly || vtkPlusLogger::Instance()->GetLogLevel() >=  vtkPlusLogger::LOG_LEVEL_DEBUG )
  {
    if ( bufferOutputFolder == NULL )
    {
      bufferOutputFolder = "./"; 
    }

    std::string strDateAndTime = vtkAccurateTimer::GetDateAndTimeString(); 
    std::ostringstream trackerBufferFileName; 
    trackerBufferFileName << strDateAndTime << "_DataCollectorSyncTrackerBuffer"; 
    std::ostringstream videoBufferFileName; 
    videoBufferFileName << strDateAndTime << "_DataCollectorSyncVideoBuffer"; 

    LOG_INFO("Save temporal calibration buffers to file in " << bufferOutputFolder << ", tracker: " << trackerBufferFileName.str().c_str() << ", video: " << videoBufferFileName.str().c_str()); 

    tracker->WriteToMetafile( bufferOutputFolder, trackerBufferFileName.str().c_str(), false );
    videobuffer->WriteToMetafile( bufferOutputFolder, videoBufferFileName.str().c_str() , false ); 
  }


  //************************************************************************************
  // Start synchronization 

  vtkTrackerBuffer* trackerbuffer = firstActiveTool->GetBuffer(); 
  LOG_DEBUG("Tracker buffer size: " << trackerbuffer->GetBufferSize()); 
  LOG_DEBUG("Tracker buffer elements: " << trackerbuffer->GetNumberOfItems()); 
  LOG_DEBUG("Video buffer size: " << videobuffer->GetBufferSize()); 
  LOG_DEBUG("Video buffer elements: " << videobuffer->GetNumberOfItems()); 

  if ( !acquireDataOnly )
  {
    this->GetSynchronizer()->SetProgressBarUpdateCallbackFunction(ProgressBarUpdateCallbackFunction); 

    this->GetSynchronizer()->SetTrackerBuffer(trackerbuffer); 
    this->GetSynchronizer()->SetVideoBuffer(videobuffer); 
    this->GetSynchronizer()->SetStartupDelaySec( this->GetStartupDelaySec() ); 

    this->GetSynchronizer()->Synchronize(); 
  }

  //************************************************************************************
  // Set the local time for buffers if the calibration was done
  if ( this->GetSynchronizer()->GetSynchronized() )
  {
    this->SetLocalTimeOffset(this->GetSynchronizer()->GetVideoOffset(), this->GetSynchronizer()->GetTrackerOffset()); 
  }

  this->GetSynchronizer()->SetTrackerBuffer(NULL); 
  this->GetSynchronizer()->SetVideoBuffer(NULL); 

  //************************************************************************************
  // Change buffer size back to original 
  LOG_DEBUG("Change video buffer size to: " << videoBufferSize); 
  if ( this->GetVideoSource()->SetFrameBufferSize(videoBufferSize) != PLUS_SUCCESS )
  {
    LOG_WARNING("Failed to change video buffer size!"); 
  }
  this->GetVideoSource()->GetBuffer()->Clear(); 

  LOG_DEBUG("Change tracker buffer size to: " << trackerBufferSize); 
  for ( ToolIteratorType it = this->GetTracker()->GetToolIteratorBegin(); it != this->GetTracker()->GetToolIteratorEnd(); ++it)
  {
    it->second->GetBuffer()->SetBufferSize(trackerBufferSize); 
    it->second->GetBuffer()->Clear(); 
  }

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
void vtkDataCollectorHardwareDevice::SetLocalTimeOffset(double videoOffset, double trackerOffset)
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::SetLocalTimeOffset");

  if ( this->GetVideoSource() != NULL ) 
  {	
    this->GetVideoSource()->GetBuffer()->SetLocalTimeOffset( videoOffset ); 
    this->GetVideoSource()->WriteConfiguration( vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData() ); 
  }

  if ( this->GetTracker() != NULL ) 
  {
    for ( ToolIteratorType it = this->GetTracker()->GetToolIteratorBegin(); it != this->GetTracker()->GetToolIteratorEnd(); ++it)
    {
      it->second->GetBuffer()->SetLocalTimeOffset(trackerOffset); 
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
    if ( uid - 1 > 0 )
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

    if ( videoUid - 1 > 0 ) 
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
PlusStatus vtkDataCollectorHardwareDevice::GetFrameByTime(double time, vtkImageData* vtkFrame, double& frameTimestamp)
{
  //LOG_TRACE("vtkDataCollectorHardwareDevice::GetFrameByTime"); 
  PlusVideoFrame frame;
  if ( this->GetFrameByTime(time, frame, frameTimestamp) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get frame by time: " << std::fixed << time ); 
    return PLUS_FAIL; 
  }

  vtkImageData* imgData = frame.GetVtkImage(); 
  if ( imgData == NULL )
  {
    LOG_ERROR("Failed to get vtk image from PlusVideoFrame!"); 
    return PLUS_FAIL; 
  }

  vtkFrame->DeepCopy(imgData); 

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorHardwareDevice::GetFrameByTime(double time, PlusVideoFrame& frame, double& frameTimestamp)
{
  //LOG_TRACE("vtkDataCollectorHardwareDevice::GetFrameByTime"); 
  if ( this->GetVideoSource() == NULL ) 
  {	
    LOG_ERROR("Unable to get frame - there is no video source selected!"); 
    return PLUS_FAIL; 
  }

  // Get frame UID
  BufferItemUidType frameUID(0); 
  ItemStatus status = this->GetVideoSource()->GetBuffer()->GetItemUidFromTime(time, frameUID); 
  if ( status != ITEM_OK )
  {
    if ( status == ITEM_NOT_AVAILABLE_ANYMORE )
    {
      LOG_ERROR("Couldn't get frame UID from time (" << std::fixed << time << ") - item not available anymore!"); 
    }
    else if ( status == ITEM_NOT_AVAILABLE_YET )
    {
      LOG_ERROR("Couldn't get frame UID from time (" << std::fixed << time << ") - item not available yet!"); 
    }
    else
    {
      LOG_ERROR("Couldn't get frame UID from time (" << std::fixed << time << ")!"); 
    }

    return PLUS_FAIL; 
  }

  VideoBufferItem currentVideoBufferItem; 
  if ( this->GetVideoSource()->GetBuffer()->GetVideoBufferItem(frameUID, &currentVideoBufferItem) != ITEM_OK )
  {
    LOG_ERROR("Couldn't get video buffer item by frame UID: " << frameUID); 
    return PLUS_FAIL; 
  }

  // Copy frame 
  frame=currentVideoBufferItem.GetFrame(); 

  // Copy frame timestamp 
  frameTimestamp = currentVideoBufferItem.GetTimestamp( this->GetVideoSource()->GetBuffer()->GetLocalTimeOffset() ); 
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorHardwareDevice::GetTransformWithTimestamp(vtkMatrix4x4* toolTransMatrix, double& transformTimestamp, TrackerStatus& status, const char* aToolName, bool calibratedTransform /*= false*/ )
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::GetTransformWithTimestamp"); 
  if ( this->GetTracker() == NULL ) 
  {	
    LOG_ERROR("Unable to get transform - there is no tracker selected!"); 
    return PLUS_FAIL; 
  }

  if ( aToolName == NULL )
  {
    LOG_ERROR("Unable to get transform - tool name is NULL!"); 
    return PLUS_FAIL; 
  }

  vtkTrackerTool* tool = NULL; 
  if ( this->GetTracker()->GetTool(aToolName, tool) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to get transform for tool " << aToolName); 
    return PLUS_FAIL; 
  }  

  TrackerBufferItem bufferItem; 
  if ( tool->GetBuffer()->GetLatestTrackerBufferItem(&bufferItem, calibratedTransform) != ITEM_OK )
  { 
    LOG_ERROR("Failed to get latest tracker buffer item from buffer!"); 
    return PLUS_FAIL; 
  } 

  status = bufferItem.GetStatus(); 

  if (bufferItem.GetMatrix(toolTransMatrix)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to get currentMatrix"); 
    return PLUS_FAIL; 
  }

  transformTimestamp = bufferItem.GetTimestamp( tool->GetBuffer()->GetLocalTimeOffset() ); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorHardwareDevice::GetTransformByTimestamp(vtkMatrix4x4* toolTransMatrix, TrackerStatus& status, double synchronizedTime, const char* aToolName, bool calibratedTransform /*= false*/)
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::GetTransformByTimestamp"); 
  if ( this->GetTracker() == NULL ) 
  {	
    LOG_ERROR("Unable to get transform - there is no tracker selected!"); 
    return PLUS_FAIL; 
  }

  if ( aToolName == NULL )
  {
    LOG_ERROR("Unable to get transform - tool name is NULL!"); 
    return PLUS_FAIL; 
  }

  vtkTrackerTool* tool = NULL; 
  if ( this->GetTracker()->GetTool(aToolName, tool) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to get transform for tool " << aToolName); 
    return PLUS_FAIL; 
  }

  TrackerBufferItem bufferItem; 
  if ( tool->GetBuffer()->GetTrackerBufferItemFromTime(synchronizedTime, &bufferItem, vtkTrackerBuffer::INTERPOLATED, calibratedTransform) != ITEM_OK )
  { 
    LOG_ERROR("Failed to get tracker buffer item from time: " << std::fixed << synchronizedTime); 
    return PLUS_FAIL; 
  } 

  status = bufferItem.GetStatus(); 

  if (bufferItem.GetMatrix(toolTransMatrix)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to get toolTransMatrix"); 
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
double vtkDataCollectorHardwareDevice::GetTransformsByTimeInterval(std::vector<vtkMatrix4x4*> &toolTransMatrixVector, std::vector<TrackerStatus> &statusVector, double startTime, double endTime, const char* aToolName, bool calibratedTransform /*= false*/)
{
  //LOG_TRACE("vtkDataCollectorHardwareDevice::GetTransformsByTimeInterval"); 
  if ( this->GetTracker() == NULL ) 
  {	
    LOG_ERROR("Unable to get transforms - there is no tracker selected!"); 
    return -1.0; 
  }

  for (std::vector<vtkMatrix4x4*>::iterator it = toolTransMatrixVector.begin(); it != toolTransMatrixVector.end(); ++it) {
    if ((*it) != NULL) {
      (*it)->Delete();
    }
  }
  toolTransMatrixVector.clear();
  statusVector.clear();

  if ( aToolName == NULL )
  {
    LOG_ERROR("Unable to get transforms - tool name is NULL!"); 
    return PLUS_FAIL; 
  }

  vtkTrackerTool* tool = NULL; 
  if ( this->GetTracker()->GetTool(aToolName, tool) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to get transforms for tool " << aToolName); 
    return PLUS_FAIL; 
  }

  vtkTrackerBuffer* buffer = tool->GetBuffer();

  BufferItemUidType frameBufferStartIndex = buffer->GetOldestItemUidInBuffer(); // Start index is initialized with the oldest one
  if (startTime > 0) 
  { 
    // If specific startTime was given then get start index for the item after the one with the specified timestamp
    if ( buffer->GetItemUidFromTime(startTime, frameBufferStartIndex) != ITEM_OK )
    {
      LOG_ERROR("Failed to get item UID from time: " << std::fixed << startTime ); 
      return -1.0; 
    }
    frameBufferStartIndex = frameBufferStartIndex + 1; // The transform at start time is skipped
  }

  double returnTime;
  BufferItemUidType frameBufferEndIndex = buffer->GetLatestItemUidInBuffer(); // End index is initialized as the most recent data (it remains so if endTime is -1)
  if (endTime > 0) 
  { 
    // If specific endTime was given then get corresponding end index
    if ( buffer->GetItemUidFromTime(endTime, frameBufferEndIndex) != ITEM_OK )
    {
      LOG_ERROR("Failed to get item UID from time: " << std::fixed << startTime ); 
      return -1.0; 
    }
    returnTime = endTime;
  } 

  for (int i=frameBufferStartIndex; i<=frameBufferEndIndex; ++i) 
  {
    TrackerBufferItem bufferItem; 
    if ( buffer->GetTrackerBufferItem(i, &bufferItem, calibratedTransform) != ITEM_OK )
    {
      LOG_ERROR("Failed to get tracker buffer item with UID: " << i ); 
      continue; 
    }

    vtkSmartPointer<vtkMatrix4x4> toolTransMatrix=vtkSmartPointer<vtkMatrix4x4>::New();
    if (bufferItem.GetMatrix(toolTransMatrix)!=PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to get toolTransMatrix"); 
      continue; 
    }

    toolTransMatrixVector.push_back( toolTransMatrix );
    statusVector.push_back( bufferItem.GetStatus() );
  }

  return returnTime;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorHardwareDevice::GetTrackedFrame(vtkImageData* frame, vtkMatrix4x4* toolTransMatrix, TrackerStatus &status, double& synchronizedTime, const char* aToolName, bool calibratedTransform /*= false*/)
{
  //LOG_TRACE("vtkDataCollectorHardwareDevice::GetTrackedFrame - vtkImageData"); 

  double mostRecentFrameTimestamp(0);
  if (this->GetMostRecentTimestamp(mostRecentFrameTimestamp) != PLUS_SUCCESS) 
  {
    LOG_ERROR("Failed to get most recent timestamp from the buffer!"); 
    return PLUS_FAIL;
  }

  synchronizedTime = mostRecentFrameTimestamp; 

  if ( this->GetVideoEnabled() )
  {
    if ( this->GetFrameByTime(mostRecentFrameTimestamp, frame, synchronizedTime) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get video frame from the buffer by timestamp: " << std::fixed << mostRecentFrameTimestamp); 
      return PLUS_FAIL;
    }
  }

  if ( this->GetTrackingEnabled() )
  {
    if ( this->GetTransformByTimestamp( toolTransMatrix, status, synchronizedTime, aToolName, calibratedTransform) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get transform bt timestamp: " << std::fixed << synchronizedTime ); 
      return PLUS_FAIL; 
    }
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
int vtkDataCollectorHardwareDevice::GetNumberOfFramesBetweenTimestamps(double frameTimestampFrom, double frameTimestampTo)
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::GetNumberOfFramesBetweenTimestamps(" << frameTimestampFrom << ", " << frameTimestampTo << ")");

  int numberOfFrames = 0;

  if ( this->GetVideoEnabled() )
  {
    VideoBufferItem vFromItem; 
    if (this->GetVideoSource()->GetBuffer()->GetVideoBufferItemFromTime(frameTimestampFrom, &vFromItem) != ITEM_OK )
    {
      return 0;
    }

    VideoBufferItem vToItem; 
    if (this->GetVideoSource()->GetBuffer()->GetVideoBufferItemFromTime(frameTimestampTo, &vToItem) != ITEM_OK )
    {
      return 0;
    }

    numberOfFrames = abs((int)(vToItem.GetUid() - vFromItem.GetUid()));
  }
  else if ( this->GetTrackingEnabled() )
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

    TrackerBufferItem tFromItem; 
    if (trackerBuffer->GetTrackerBufferItem(frameTimestampFrom, &tFromItem) != ITEM_OK )
    {
      return 0;
    } 

    TrackerBufferItem tToItem; 
    if (trackerBuffer->GetTrackerBufferItem(frameTimestampTo, &tToItem) != ITEM_OK )
    {
      return 0;
    }

    numberOfFrames = abs((int)(tToItem.GetUid() - tFromItem.GetUid())); 
  }

  return numberOfFrames + 1;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorHardwareDevice::GetTrackedFrameList(double& frameTimestamp, vtkTrackedFrameList* trackedFrameList, int maxNumberOfFramesToAdd/*=-1*/)
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::GetTrackedFrameList(" << frameTimestamp << ", " << maxNumberOfFramesToAdd << ")"); 

  if ( trackedFrameList == NULL )
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

  if ( maxNumberOfFramesToAdd > 0 ) 
  {
    if ( this->GetVideoEnabled() )
    {
      BufferItemUidType mostRecentVideoUid; 
      if ( this->VideoSource->GetBuffer()->GetItemUidFromTime(mostRecentTimestamp, mostRecentVideoUid) != ITEM_OK )
      {
        LOG_ERROR("Failed to get video buffer item by timestamp " << mostRecentTimestamp);
        return PLUS_FAIL;
      }
      if ( mostRecentVideoUid - maxNumberOfFramesToAdd > this->VideoSource->GetBuffer()->GetOldestItemUidInBuffer() )
      {
        // Most recent is needed too
        mostRecentVideoUid = mostRecentVideoUid - maxNumberOfFramesToAdd + 1; 
      }
      else
      {
        LOG_WARNING("Number of frames in the buffer is less than maxNumberOfFramesToAdd!"); 
      }

      if ( this->VideoSource->GetBuffer()->GetTimeStamp(mostRecentVideoUid, frameTimestamp ) != ITEM_OK )
      {
        LOG_ERROR("Failed to get video buffer timestamp from UID: " << mostRecentVideoUid ); 
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
      if ( mostRecentTrackerUid - maxNumberOfFramesToAdd > trackerBuffer->GetOldestItemUidInBuffer() )
      {
        // Most recent is needed too
        mostRecentTrackerUid = mostRecentTrackerUid - maxNumberOfFramesToAdd + 1; 
      }
      else
      {
        LOG_WARNING("Number of frames in the buffer is less than maxNumberOfFramesToAdd!"); 
      }

      if ( trackerBuffer->GetTimeStamp(mostRecentTrackerUid, frameTimestamp ) != ITEM_OK )
      {
        LOG_ERROR("Failed to get tracker buffer timestamp from UID: " << mostRecentTrackerUid ); 
        return PLUS_FAIL; 
      }
    }
  }

  // Check input frameTimestamp to be in a valid range 
  if ( frameTimestamp < 0.0001 || frameTimestamp > mostRecentTimestamp )
  {
    frameTimestamp = mostRecentTimestamp; 
  }
  else if ( oldestTimestamp > frameTimestamp )
  {
    frameTimestamp = oldestTimestamp; 
  }

  // Determine how many frames to add
  int numberOfFramesSinceTimestamp = GetNumberOfFramesBetweenTimestamps(frameTimestamp, mostRecentTimestamp);

  int numberOfFramesToAdd = 0;
  if (maxNumberOfFramesToAdd > 0)
  {
    numberOfFramesToAdd = std::min( maxNumberOfFramesToAdd, numberOfFramesSinceTimestamp );
  }
  else
  {
    numberOfFramesToAdd = numberOfFramesSinceTimestamp;
  }

  LOG_DEBUG("Number of added frames: " << numberOfFramesToAdd << " out of " << numberOfFramesSinceTimestamp);

  for (int i=0; i<numberOfFramesToAdd; ++i)
  {
    // Get tracked frame from buffer
    TrackedFrame trackedFrame; 

    if ( this->GetTrackedFrameByTime(frameTimestamp, &trackedFrame) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to get tracked frame by time: " << std::fixed << frameTimestamp ); 
      return PLUS_FAIL;
    }

    // Add tracked frame to the list 
    if ( trackedFrameList->AddTrackedFrame(&trackedFrame, vtkTrackedFrameList::SKIP_INVALID_FRAME) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to add tracked frame to the list!" ); 
      return PLUS_FAIL; 
    }

    // Get next timestamp
    if ( this->GetVideoEnabled() && i < numberOfFramesToAdd - 1 )
    {
      BufferItemUidType videoUid(0); 
      if ( this->VideoSource->GetBuffer()->GetItemUidFromTime(frameTimestamp, videoUid) != ITEM_OK )
      {
        LOG_ERROR("Failed to get video buffer item UID from time: " << std::fixed << frameTimestamp ); 
        return PLUS_FAIL; 
      }

      if ( videoUid >= this->VideoSource->GetBuffer()->GetLatestItemUidInBuffer() )
      {
        LOG_WARNING("Requested video uid (" << videoUid+1 << ") is not in the buffer yet!");
        break;
      }

      // Get the timestamp of the next item in the buffer
      if ( this->VideoSource->GetBuffer()->GetTimeStamp(++videoUid, frameTimestamp) != ITEM_OK )
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
      if ( trackerBuffer->GetItemUidFromTime(frameTimestamp, trackerUid) != ITEM_OK )
      {
        LOG_ERROR("Failed to get tracker buffer item UID from time: " << std::fixed << frameTimestamp ); 
        return PLUS_FAIL; 
      }

      if ( trackerUid >= trackerBuffer->GetLatestItemUidInBuffer() )
      {
        LOG_ERROR("Requested tracker uid (" << trackerUid+1 << ") is not in the buffer yet!");
        break;
      }

      // Get the timestamp of the next item in the buffer
      if ( trackerBuffer->GetTimeStamp(++trackerUid, frameTimestamp) != ITEM_OK )
      {
        LOG_WARNING("Unable to get timestamp from tracker buffer by UID: " << trackerUid); 
        return PLUS_FAIL;
      }
    }
  }

  return status; 
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollectorHardwareDevice::GetTrackedFrameListSampled(double& frameTimestamp, vtkTrackedFrameList* trackedFrameList, double samplingRateSec)
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::GetTrackedFrameListSampled(" << frameTimestamp << ", " << samplingRateSec << ")"); 

  if ( trackedFrameList == NULL )
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
  if ( frameTimestamp < 0.0001 || frameTimestamp > mostRecentTimestamp )
  {
    frameTimestamp = mostRecentTimestamp; 
  }
  else if ( oldestTimestamp > frameTimestamp )
  {
    frameTimestamp = oldestTimestamp; 
  }

  // Check if there are less frames than it would be needed according to the sampling rate
  int numberOfFramesSinceTimestamp = GetNumberOfFramesBetweenTimestamps(frameTimestamp, mostRecentTimestamp);
  int numberOfSampledFrames = (int)((mostRecentTimestamp - frameTimestamp) / samplingRateSec);

  if (numberOfFramesSinceTimestamp < numberOfSampledFrames)
  {
    LOG_WARNING("The requested sampling rate is faster than the acquisition itself - fewer unique frames will be added to the list!");
  }

  // Add frames to input trackedFrameList
  while (frameTimestamp + samplingRateSec <= mostRecentTimestamp)
  {
    // Get tracked frame from buffer
    TrackedFrame trackedFrame; 

    if ( this->GetTrackedFrameByTime(frameTimestamp, &trackedFrame) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to get tracked frame by time: " << std::fixed << frameTimestamp ); 
      return PLUS_FAIL;
    }

    // Add tracked frame to the list 
    if ( trackedFrameList->AddTrackedFrame(&trackedFrame, vtkTrackedFrameList::SKIP_INVALID_FRAME) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to add tracked frame to the list!" ); 
      return PLUS_FAIL; 
    }

    // Set timestamp to the next sampled one
    frameTimestamp += samplingRateSec;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorHardwareDevice::GetTrackedFrame(TrackedFrame* trackedFrame, bool calibratedTransform /*= false*/)
{
  //LOG_TRACE("vtkDataCollectorHardwareDevice::GetTrackedFrame - TrackedFrame"); 

  double mostRecentFrameTimestamp(0);
  if (this->GetMostRecentTimestamp(mostRecentFrameTimestamp) != PLUS_SUCCESS) 
  {
    LOG_ERROR("Failed to get most recent timestamp from the buffer!"); 
    return PLUS_FAIL;
  }

  return this->GetTrackedFrameByTime(mostRecentFrameTimestamp, trackedFrame, calibratedTransform); 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorHardwareDevice::GetTrackedFrameByTime(double time, TrackedFrame* trackedFrame, bool calibratedTransform /*= false*/)
{
  //LOG_TRACE("vtkDataCollectorHardwareDevice::GetTrackedFrameByTime - TrackedFrame");
  double synchronizedTime(0); 

  if ( this->GetVideoEnabled() )
  {
    PlusVideoFrame frame; 

    // Get the frame by time
    if ( ! this->GetFrameByTime(time, frame, synchronizedTime) )
    {
      LOG_ERROR("Failed to get tracked frame by time: " << std::fixed << time ); 
      return PLUS_FAIL; 
    }

    //Add all information to the tracked frame
    trackedFrame->SetImageData(frame);
  }

  if ( this->GetTrackingEnabled() && this->GetTracker() != NULL )
  {
    // Get tracker buffer values 
    std::map<std::string, std::string> toolsBufferMatrices; 
    std::map<std::string, std::string> toolsStatuses; 

    if ( !this->GetVideoEnabled() )
    {
      synchronizedTime = time;  
    }

    if ( this->GetTracker()->GetTrackerToolBufferStringList(synchronizedTime, toolsBufferMatrices, toolsStatuses, calibratedTransform) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get tracker tool buffer stringlist: " << std::fixed << synchronizedTime ); 
      return PLUS_FAIL; 
    }

    for ( std::map<std::string, std::string>::iterator it = toolsBufferMatrices.begin(); it != toolsBufferMatrices.end(); it++ )
    {
      // Set tool buffer values 
      trackedFrame->SetCustomFrameField(it->first, it->second); 
    }

    for ( std::map<std::string, std::string>::iterator it = toolsStatuses.begin(); it != toolsStatuses.end(); it++ )
    {
      // Set tool buffer statuses 
      std::ostringstream statusName; 
      statusName << it->first << "Status"; 
      trackedFrame->SetCustomFrameField(statusName.str(), it->second); 
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
PlusStatus vtkDataCollectorHardwareDevice::GetTrackedFrameByTime(double time, vtkImageData* frame, std::vector<vtkMatrix4x4*> &toolTransforms, std::vector<std::string> &toolTransformNames, std::vector<TrackerStatus> &status, double& synchronizedTime, bool calibratedTransform /*= false*/)
{
  //LOG_TRACE("vtkDataCollectorHardwareDevice::GetTrackedFrameByTime - vtkImageData");
  toolTransforms.clear(); 
  toolTransformNames.clear(); 
  status.clear(); 

  if ( ! this->GetFrameByTime(time, frame, synchronizedTime) )
  {
    LOG_ERROR( "Failed to get tracked frame by time: " << time ); 
    return PLUS_FAIL; 
  }

  if ( this->GetTracker() != NULL )
  {
    for ( ToolIteratorType it = this->GetTracker()->GetToolIteratorBegin(); it != this->GetTracker()->GetToolIteratorEnd(); ++it)
    {
        TrackerStatus trackerStatus = TR_OK; 
        const char* toolName = it->second->GetToolName(); 

        vtkSmartPointer<vtkMatrix4x4> toolTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
        this->GetTransformByTimestamp( toolTransMatrix, trackerStatus, synchronizedTime, toolName, calibratedTransform); 

        toolTransMatrix->Register(NULL); 
        toolTransforms.push_back(toolTransMatrix); 

        std::string transformName( toolName ); 
        toolTransformNames.push_back(transformName); 

        status.push_back(trackerStatus); 
    }
  }

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

  const double globalTime = currentVideoBufferItem.GetTimestamp( this->GetVideoSource()->GetBuffer()->GetLocalTimeOffset() ); 

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
      if ( it->second->GetBuffer()->GetLatestTrackerBufferItem(&bufferItem, false) != ITEM_OK )
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

  // Check plus configuration version
  double plusConfigurationVersion = 0;
  if (aConfigurationData->GetScalarAttribute("version", plusConfigurationVersion))
  {
    double currentVersion = (double)PLUSLIB_VERSION_MAJOR + ((double)PLUSLIB_VERSION_MINOR / 10.0);

    if (plusConfigurationVersion < currentVersion)
    {
      LOG_ERROR("This version of configuration file is no longer supported! Please update to version " << std::fixed << currentVersion); 
      return PLUS_FAIL;
    }
  }

  // Get data collection configuration element
  vtkXMLDataElement* dataCollectionConfig = aConfigurationData->FindNestedElementWithName("DataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  // Check data collection configuration version
  double dataCollectionVersion = 0; 
  if ( dataCollectionConfig->GetScalarAttribute("version", dataCollectionVersion) )
  {
    if ( dataCollectionVersion < this->DataCollectionConfigVersion )
    {
      LOG_ERROR("This version of configuration file is no longer supported! Please update to version " << std::fixed << this->DataCollectionConfigVersion ); 
      return PLUS_FAIL;
    }
  }

  // Read StartupDelaySec
  double startupDelaySec(0.0); 
  if ( dataCollectionConfig->GetScalarAttribute("StartupDelaySec", startupDelaySec) )
  {
    this->SetStartupDelaySec(startupDelaySec); 
    LOG_DEBUG("StartupDelaySec: " << std::fixed << startupDelaySec ); 
  }

  // Read ImageAcquisition
  if (this->ReadImageAcquisitionProperties(aConfigurationData) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to read image acquisition configuration!");
    return PLUS_FAIL;
  }

  // Read Tracker
  if (this->ReadTrackerProperties(aConfigurationData) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to read tracker configuration!");
    return PLUS_FAIL;
  }

  // Read Synchronization
  if (this->ReadSynchronizationProperties(aConfigurationData) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to read synchronization configuration!");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
PlusStatus vtkDataCollectorHardwareDevice::ReadTrackerProperties(vtkXMLDataElement* aConfigurationData)
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::ReadTrackerProperties");

  vtkXMLDataElement* dataCollectionConfig = aConfigurationData->FindNestedElementWithName("DataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* trackerConfig = dataCollectionConfig->FindNestedElementWithName("Tracker"); 
  if (trackerConfig == NULL) 
  {
    LOG_ERROR("Cannot find Tracker element in XML tree!");
    return PLUS_FAIL;
  }

  LOG_TRACE("vtkDataCollectorHardwareDevice::ReadTrackerProperties");
  const char* type = trackerConfig->GetAttribute("Type"); 
  if ( type == NULL ) 
  {
    LOG_WARNING("Unable to find tracker type, set to default: None"); 

    this->SetTrackerType(TRACKER_NONE); 
    LOG_DEBUG("Tracker type: None");
    this->SetTracker(NULL); 
    this->TrackingEnabled = false; 
  }
  //******************* Brachy Tracker ***************************
  else if ( STRCASECMP("BrachyTracker", type)==0) 
  {
#ifdef PLUS_USE_BRACHY_TRACKER
    LOG_DEBUG("Tracker type: Brachy tracker"); 
    this->SetTrackerType(TRACKER_BRACHY); 
    vtkSmartPointer<vtkBrachyTracker> tracker = vtkSmartPointer<vtkBrachyTracker>::New();
    this->SetTracker(tracker); 
    tracker->ReadConfiguration(aConfigurationData); 
#endif
  }
  //******************* Certus Tracker ***************************
  else if ( STRCASECMP("CertusTracker", type)==0) 
  {
#ifdef PLUS_USE_CERTUS
    LOG_DEBUG("Tracker type: Certus tracker"); 
    this->SetTrackerType(TRACKER_CERTUS); 
    vtkSmartPointer<vtkNDICertusTracker> tracker = vtkSmartPointer<vtkNDICertusTracker>::New();
    this->SetTracker(tracker); 
    tracker->ReadConfiguration(aConfigurationData); 

    /*int referenceToolNumber(-1);
    if ( trackerCertus->GetScalarAttribute("ReferenceToolNumber", referenceToolNumber) ) 
    {
    tracker->SetReferenceTool(referenceToolNumber); 
    }

    int mainToolNumber(-1);
    if ( trackerCertus->GetScalarAttribute("MainToolNumber", mainToolNumber) ) 
    {
    tracker->SetMainTool(mainToolNumber); 
    this->SetMainToolNumber(mainToolNumber); 
    }*/
#endif
  }
  //******************* Polaris Tracker ***************************
  else if ( STRCASECMP("PolarisTracker", type)==0) 
  {
#ifdef PLUS_USE_POLARIS
    LOG_DEBUG("Tracker type: Polaris tracker"); 
    this->SetTrackerType(TRACKER_POLARIS); 
    vtkSmartPointer<vtkPOLARISTracker> tracker = vtkSmartPointer<vtkPOLARISTracker>::New();
    this->SetTracker(tracker); 
    tracker->ReadConfiguration(aConfigurationData); 
#endif
  }
  //******************* Aurora Tracker ***************************
  else if ( STRCASECMP("AuroraTracker", type)==0) 
  {
#ifdef PLUS_USE_POLARIS
    LOG_DEBUG("Tracker type: Aurora tracker"); 
    this->SetTrackerType(TRACKER_AURORA); 
    vtkSmartPointer<vtkNDITracker> tracker = vtkSmartPointer<vtkNDITracker>::New();
    this->SetTracker(tracker); 
    tracker->ReadConfiguration(aConfigurationData); 
#endif
  }
  //******************* Micron Tracker ***************************
  else if ( STRCASECMP("MicronTracker", type)==0) 
  {
#ifdef PLUS_USE_MICRONTRACKER
    LOG_DEBUG("Tracker type: Micron tracker"); 
    this->SetTrackerType(TRACKER_MICRON); 
    vtkSmartPointer<vtkMicronTracker> tracker = vtkSmartPointer<vtkMicronTracker>::New();
    this->SetTracker(tracker); 
    tracker->ReadConfiguration(aConfigurationData); 
#endif
  }
  //******************* Saved dataset ***************************
  else if ( STRCASECMP("SavedDataset", type)==0) 
  {
    LOG_DEBUG("Tracker type: Saved Dataset");
    this->SetTrackerType(TRACKER_SAVEDDATASET); 
    vtkSmartPointer<vtkSavedDataTracker> tracker = vtkSmartPointer<vtkSavedDataTracker>::New();
    this->SetTracker(tracker); 
    tracker->ReadConfiguration(aConfigurationData); 
  }
  //******************* Ascension 3DG ***************************
  else if ( STRCASECMP( "Ascension3DG", type ) == 0 )
  {
#ifdef PLUS_USE_Ascension3DG
    LOG_DEBUG( "Tracker type: Ascension 3DG" );
    this->SetTrackerType( TRACKER_ASCENSION3DG );
    vtkSmartPointer< vtkAscension3DGTracker > tracker = vtkSmartPointer< vtkAscension3DGTracker >::New();
    this->SetTracker( tracker );
    tracker->ReadConfiguration(aConfigurationData); 
#endif
  }
  //******************* Fake Tracker ***************************
  else if ( STRCASECMP("FakeTracker", type)==0) 
  {
    LOG_DEBUG("Tracker type: Fake Tracker");
    this->SetTrackerType(TRACKER_FAKE); 
    vtkSmartPointer<vtkFakeTracker> tracker = vtkSmartPointer<vtkFakeTracker>::New();
    this->SetTracker(tracker);
    tracker->ReadConfiguration(aConfigurationData);
  }
  else
  {
    this->SetTrackerType(TRACKER_NONE); 
    LOG_DEBUG("Tracker type: None");
    this->SetTracker(NULL); 
    this->TrackingEnabled = false; 
  }
  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
PlusStatus vtkDataCollectorHardwareDevice::ReadImageAcquisitionProperties(vtkXMLDataElement* aConfigurationData)
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::ReadImageAcquisitionProperties");

  vtkXMLDataElement* dataCollectionConfig = aConfigurationData->FindNestedElementWithName("DataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* imageAcquisitionConfig = dataCollectionConfig->FindNestedElementWithName("ImageAcquisition"); 
  if (imageAcquisitionConfig == NULL) 
  {
    LOG_ERROR("Unable to find ImageAcquisition element in configuration XML structure!");
    return PLUS_FAIL;
  }

  const char* type = imageAcquisitionConfig->GetAttribute("Type"); 

  if ( type == NULL ) 
  {
    LOG_WARNING("Unable to find image acquisition type, set to default: None"); 
    this->SetAcquisitionType(SYNCHRO_VIDEO_NONE); 
    LOG_DEBUG("Image acquisition type: None");
    this->SetVideoSource(NULL); 
    this->VideoEnabled = false; 
  }
  //******************* Sonix Video ***************************
  else if ( STRCASECMP("SonixVideo", type)==0) 
  {
#ifdef PLUS_USE_ULTRASONIX_VIDEO
    LOG_DEBUG("Image acquisition type: Sonix Video"); 
    this->SetAcquisitionType(SYNCHRO_VIDEO_SONIX); 
    vtkSmartPointer<vtkSonixVideoSource> videoSource = vtkSmartPointer<vtkSonixVideoSource>::New();
    this->SetVideoSource(videoSource); 
    videoSource->ReadConfiguration(aConfigurationData);
#else
    LOG_ERROR("Ultrasonix support was disabled when this Plus library was built");     
#endif
  }
  //***************** Sonix Porta Video ***********************
  else if ( STRCASECMP("SonixPortaVideo", type)==0) 
  {
#ifdef PLUS_USE_ULTRASONIX_VIDEO
    LOG_DEBUG("Image acquisition type: Ultrasonix Porta Video"); 
    this->SetAcquisitionType(SYNCHRO_VIDEO_SONIX); 
    vtkSmartPointer<vtkSonixPortaVideoSource> videoSource = vtkSmartPointer<vtkSonixPortaVideoSource>::New();
    this->SetVideoSource(videoSource); 
    videoSource->ReadConfiguration(aConfigurationData);
#else
    LOG_ERROR("Ultrasonix support was disabled when this Plus library was built");     
#endif
  }
  //******************* Matrox Imaging ***************************
  else if ( STRCASECMP("MatroxImaging", type)==0) 
  {
#ifdef PLUS_USE_MATROX_IMAGING
    LOG_DEBUG("Image acquisition type: Matrox Imaging"); 
    this->SetAcquisitionType(SYNCHRO_VIDEO_MIL); 
    vtkSmartPointer<vtkMILVideoSource2> videoSource = vtkSmartPointer<vtkMILVideoSource2>::New();
    this->SetVideoSource(videoSource); 
    videoSource->ReadConfiguration(aConfigurationData); 
#endif
  }
  //******************* Video For Windows ***************************
  else if ( STRCASECMP("VFWVideo", type)==0) 
  {
#ifdef VTK_VFW_SUPPORTS_CAPTURE
    LOG_DEBUG("Image acquisition type: Video For Windows"); 
    this->SetAcquisitionType(SYNCHRO_VIDEO_WIN32); 	
    vtkSmartPointer<vtkWin32VideoSource2> videoSource = vtkSmartPointer<vtkWin32VideoSource2>::New();
    this->SetVideoSource(videoSource); 
    videoSource->ReadConfiguration(aConfigurationData); 
#endif
  }
  //******************* IC Capturing frame grabber ***************************
  else if ( STRCASECMP("ICCapturing", type)==0) 
  {
#ifdef PLUS_USE_ICCAPTURING_VIDEO
    LOG_DEBUG("Image acquisition type: IC Capturing"); 
    this->SetAcquisitionType(SYNCHRO_VIDEO_ICCAPTURING); 
    vtkSmartPointer<vtkICCapturingSource> videoSource = vtkSmartPointer<vtkICCapturingSource>::New();
    this->SetVideoSource(videoSource); 
    videoSource->ReadConfiguration(aConfigurationData); 
#endif
  }
  //******************* Linux Video ***************************
  else if ( STRCASECMP("LinuxVideo", type)==0) 
  {
#ifdef PLUS_USE_LINUX_VIDEO
    LOG_DEBUG("Image acquisition type: Linux Video mode"); 
    this->SetAcquisitionType(SYNCHRO_VIDEO_LINUX); 
    vtkSmartPointer<vtkV4L2LinuxSource2> videoSource = vtkSmartPointer<vtkV4L2LinuxSource2>::New();
    this->SetVideoSource(videoSource); 
    videoSource->ReadConfiguration(aConfigurationData); 
#endif
  }
  //******************* Noise Video ***************************
  else if ( STRCASECMP("NoiseVideo", type)==0) 
  {
    LOG_DEBUG("Image acquisition type: Noise Video"); 
    this->SetAcquisitionType(SYNCHRO_VIDEO_NOISE); 
    vtkSmartPointer<vtkPlusVideoSource> videoSource = vtkSmartPointer<vtkPlusVideoSource>::New();
    this->SetVideoSource(videoSource); 
    videoSource->ReadConfiguration(aConfigurationData); 
  }
  //******************* Saved dataset ***************************
  else if ( STRCASECMP("SavedDataset", type)==0 ) 
  {
    LOG_DEBUG("Image acquisition type: Saved Dataset");
    this->SetAcquisitionType(SYNCHRO_VIDEO_SAVEDDATASET); 
    vtkSmartPointer<vtkSavedDataVideoSource> videoSource = vtkSmartPointer<vtkSavedDataVideoSource>::New();
    this->SetVideoSource(videoSource); 
    videoSource->ReadConfiguration(aConfigurationData); 
  }
  else
  {
    this->SetAcquisitionType(SYNCHRO_VIDEO_NONE); 
    LOG_DEBUG("Image acquisition type: None");
    this->SetVideoSource(NULL); 
    this->VideoEnabled = false; 
  }
  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
PlusStatus vtkDataCollectorHardwareDevice::ReadSynchronizationProperties(vtkXMLDataElement* aConfigurationData)
{
  LOG_TRACE("vtkDataCollectorHardwareDevice::ReadSynchronizationProperties");

  vtkXMLDataElement* dataCollectionConfig = aConfigurationData->FindNestedElementWithName("DataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* synchronizationConfig = dataCollectionConfig->FindNestedElementWithName("Synchronization"); 
  if (synchronizationConfig == NULL) 
  {
    LOG_ERROR("Cannot find Synchronization element in XML tree!");
    return PLUS_FAIL;
  }

  const char* type = synchronizationConfig->GetAttribute("Type"); 

  if ( type == NULL ) 
  {
    LOG_WARNING("Unable to find synchronization type, set to default: None"); 
    this->SetSyncType(SYNC_NONE); 
    LOG_DEBUG("Sync type: None");
    this->SetSynchronizer(NULL); 
  }
  //******************* Change Detection ***************************
  else if ( STRCASECMP("ChangeDetection", type)==0) 
  {
    LOG_DEBUG("Sync type: Change Detection");
    vtkSmartPointer<vtkDataCollectorSynchronizer> synchronizer = vtkSmartPointer<vtkDataCollectorSynchronizer>::New(); 
    this->SetSyncType(SYNC_CHANGE_DETECTION); 
    this->SetSynchronizer(synchronizer); 
    synchronizer->ReadConfiguration(aConfigurationData); 
  }
  else
  {
    this->SetSyncType(SYNC_NONE); 
    LOG_DEBUG("Sync type: None");
    this->SetSynchronizer(NULL);
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
    this->VideoEnabled = true;
  }

  if ( this->GetConnected() && this->GetVideoSource() != NULL )
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
    this->TrackingEnabled = true;
  }

  if ( this->GetConnected() && this->GetTracker() != NULL )
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
