/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtkDataCollector.h"

#include "PlusConfigure.h"
#include "vtkObjectFactory.h"
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

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

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
#ifdef PLUS_USE_FLOCK
#include "vtkFlockTracker.h"
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
/*#else
#ifdef USE_LINUX_VIDEO
#include "vtkV4L2VideoSource2.h"
#endif*/
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

static void *vtkFrameUpdaterThread(vtkMultiThreader::ThreadInfo *data); 

vtkCxxRevisionMacro(vtkDataCollector, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkDataCollector);

vtkCxxSetObjectMacro(vtkDataCollector, Synchronizer, vtkDataCollectorSynchronizer);
vtkCxxSetObjectMacro(vtkDataCollector, Tracker, vtkTracker);
vtkCxxSetObjectMacro(vtkDataCollector, VideoSource, vtkPlusVideoSource);

//----------------------------------------------------------------------------
vtkDataCollector::vtkDataCollector()
{	
  this->VideoSource = NULL;
  this->Tracker = NULL; 
  this->Synchronizer = NULL; 
  this->ProgressBarUpdateCallbackFunction = NULL; 

  this->DataCollectionConfigVersion = 2.0; 

  this->StartupDelaySec = 0.0; 

  this->DeviceSetName = NULL; 
  this->DeviceSetDescription = NULL; 
  this->ToolTransMatrices.reserve(0); 
  this->ToolStatus.reserve(0); 

  this->InitializedOff(); 
  this->ConnectedOff(); 
  this->TrackingEnabled = true;
  this->VideoEnabled = true;
  this->CancelSyncRequestOff(); 
}


//----------------------------------------------------------------------------
vtkDataCollector::~vtkDataCollector()
{
  this->SetTracker(NULL); 
  this->SetVideoSource(NULL);
  this->SetSynchronizer(NULL); 
  this->SetDeviceSetName(NULL); 
  this->SetDeviceSetDescription(NULL); 

  for ( unsigned int i = 0; i < this->ToolTransMatrices.size(); i++ ) 
  {
    this->ToolTransMatrices[i]->Delete(); 
    this->ToolTransMatrices[i]=NULL; 
  }
}


//----------------------------------------------------------------------------
void vtkDataCollector::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  if ( this->GetTracker() != NULL )
  {
    os << indent << "Tracker: " << std::endl; 
    this->GetTracker()->PrintSelf(os, indent); 
  }

  if ( this->GetVideoSource() != NULL )
  {
    os << indent << "Video source: " << std::endl; 
    this->GetVideoSource()->PrintSelf(os, indent); 
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::Initialize()
{
  LOG_TRACE("vtkDataCollector::Initialize"); 
  this->InitializedOff(); 

  // Connect to devices
  if (this->Connect()!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  this->InitializedOn(); 
  return PLUS_SUCCESS;
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
PlusStatus vtkDataCollector::SetLoopTimes()
{
  LOG_TRACE("vtkDataCollector::SetLoopTimes"); 

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
      // reserve enough transformation capacity for each tool
      this->ToolTransMatrices.clear(); 
      this->ToolStatus.clear(); 
      for ( int i = 0; i < this->GetTracker()->GetNumberOfTools(); i++ )
      {
        vtkMatrix4x4* matrix = vtkMatrix4x4::New(); 
        this->ToolTransMatrices.push_back(matrix); 
        this->ToolStatus.push_back(TR_OK); 
      }

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

  LOG_DEBUG("vtkDataCollector::Start: wait " << std::fixed << this->GetStartupDelaySec() << " sec for buffer init..."); 
  vtkAccurateTimer::Delay(this->GetStartupDelaySec()); 

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
PlusStatus vtkDataCollector::CopyTrackerBuffer( vtkTrackerBuffer* trackerBuffer, int toolNumber )
{
  LOG_TRACE("vtkDataCollector::CopyTrackerBuffer"); 

  if ( this->GetTracker() == NULL ) 
  {	
    LOG_ERROR("Unable to copy tracker buffer - there is no tracker selected!"); 
    return PLUS_FAIL; 
  }

  if ( trackerBuffer == NULL )
  {
    LOG_ERROR("Unable to copy tracker buffer to a NULL buffer!"); 
    return PLUS_FAIL; 
  }

  trackerBuffer->DeepCopy(this->GetTracker()->GetTool(toolNumber)->GetBuffer()); 

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::CopyTracker( vtkTracker* tracker)
{
  LOG_TRACE("vtkDataCollector::CopyTracker"); 
  if ( this->GetTracker() == NULL ) 
  {	
    LOG_ERROR("Unable to copy tracker buffer - there is no tracker selected!"); 
    return PLUS_FAIL; 
  }

  if ( tracker == NULL )
  {
    LOG_ERROR("Unable to copy tracker to a NULL tracker!"); 
    return PLUS_FAIL; 
  }

  tracker->DeepCopy(this->GetTracker()); 

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::WriteTrackerToMetafile( vtkTracker* tracker, const char* outputFolder, const char* metaFileName, bool useCompression /*= false*/ )
{
  LOG_TRACE("vtkDataCollector::WriteTrackerToMetafile: " << outputFolder << "/" << metaFileName); 

  if ( tracker == NULL ) 
  {
    LOG_ERROR("Unable to copy tracker to sequence metafile: tracker is NULL"); 
    return PLUS_FAIL; 
  }

  // Get the number of items from buffers and use the lowest
  int numberOfItems(-1); 
  for ( int i = 0; i < tracker->GetNumberOfTools(); i++ )
  {
    if ( tracker->GetTool(i)->GetEnabled() )
    {
      if ( numberOfItems < 0 || numberOfItems > tracker->GetTool(i)->GetBuffer()->GetNumberOfItems() )
      {
        numberOfItems = tracker->GetTool(i)->GetBuffer()->GetNumberOfItems(); 
      }
    }
  }

  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 

  PlusStatus status=PLUS_SUCCESS;

  int firstActiveToolNumber = -1; 
  if ( tracker->GetFirstActiveTool(firstActiveToolNumber) != PLUS_SUCCESS )
  {
	  LOG_ERROR("Failed to get first active tool number!"); 
	  return PLUS_FAIL; 
  }

  // Write calibration matrices into the trackedframelist
  std::map<std::string, std::string> toolsCalibrationMatrices; 
  if ( tracker->GetTrackerToolCalibrationMatrixStringList(toolsCalibrationMatrices) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get tracker tool calibration matrix stringlist"); 
    return PLUS_FAIL; 
  }
  for ( std::map<std::string, std::string>::iterator it = toolsCalibrationMatrices.begin(); it != toolsCalibrationMatrices.end(); it++ )
  {
    // Set tool calibration values 
    trackedFrameList->SetCustomString(it->first.c_str(), it->second.c_str()); 
  }

  // Set default transform name
  trackedFrameList->SetDefaultFrameTransformName(tracker->GetTool(firstActiveToolNumber)->GetToolName()); 
  for ( int i = 0 ; i < numberOfItems; i++ ) 
  {
    //Create fake image 
    typedef itk::Image<unsigned char, 2> ImageType;
    ImageType::Pointer frame = ImageType::New(); 
    ImageType::SizeType size = {1, 1};
    ImageType::IndexType start = {0,0};
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

    TrackerBufferItem bufferItem; 
    BufferItemUidType uid = tracker->GetTool(firstActiveToolNumber)->GetBuffer()->GetOldestItemUidInBuffer() + i; 

    if ( tracker->GetTool(firstActiveToolNumber)->GetBuffer()->GetTrackerBufferItem(uid, &bufferItem, false) != ITEM_OK )
    {
      LOG_ERROR("Failed to get tracker buffer item with UID: " << uid ); 
      continue; 
    }

    const double frameTimestamp = bufferItem.GetFilteredTimestamp(tracker->GetTool(firstActiveToolNumber)->GetBuffer()->GetLocalTimeOffset()); 

    // Add main tool timestamp
    std::ostringstream timestampFieldValue; 
    timestampFieldValue << std::fixed << frameTimestamp; 
    trackedFrame.SetCustomFrameField("Timestamp", timestampFieldValue.str()); 

    // Add main tool unfiltered timestamp
    std::ostringstream unfilteredtimestampFieldValue; 
    unfilteredtimestampFieldValue << std::fixed << bufferItem.GetUnfilteredTimestamp(tracker->GetTool(firstActiveToolNumber)->GetBuffer()->GetLocalTimeOffset()); 
    trackedFrame.SetCustomFrameField("UnfilteredTimestamp", unfilteredtimestampFieldValue.str()); 

    // Add main tool frameNumber
    std::ostringstream frameNumberFieldValue; 
    frameNumberFieldValue << std::fixed << bufferItem.GetIndex(); 
    trackedFrame.SetCustomFrameField("FrameNumber", frameNumberFieldValue.str()); 

    // Add main tool status
    trackedFrame.SetCustomFrameField("Status", vtkTracker::ConvertTrackerStatusToString(bufferItem.GetStatus()) ); 

    // Add transforms
    for ( int tool = 0; tool < tracker->GetNumberOfTools(); tool++ )
    {
      if ( tracker->GetTool(tool)->GetEnabled() )
      {
        TrackerBufferItem toolBufferItem; 
        if ( tracker->GetTool(tool)->GetBuffer()->GetTrackerBufferItemFromTime( frameTimestamp, &toolBufferItem, vtkTrackerBuffer::EXACT_TIME, false ) != ITEM_OK )
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

        trackedFrame.SetCustomFrameTransform(tracker->GetTool(tool)->GetToolName(), toolMatrix ); 

      }
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
PlusStatus vtkDataCollector::CopyVideoBuffer( vtkVideoBuffer* videoBuffer )
{
  if ( this->GetVideoSource() == NULL ) 
  {	
    LOG_ERROR("Unable to copy video buffer - there is no video source selected!"); 
    return PLUS_FAIL; 
  }

  if ( videoBuffer == NULL ) 
  {
    LOG_ERROR("Unable to copy video buffer to a NULL buffer!"); 
    return PLUS_FAIL; 
  }

  videoBuffer->DeepCopy(this->GetVideoSource()->GetBuffer()); 

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::WriteVideoBufferToMetafile( vtkVideoBuffer* videoBuffer, const char* outputFolder, const char* metaFileName, bool useCompression /*=false*/ )
{
  if ( videoBuffer == NULL )
  {
    LOG_ERROR("Unable to dump video buffer if it's NULL!"); 
    return PLUS_FAIL; 
  }

  const int numberOfFrames = videoBuffer->GetNumberOfItems(); 
  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 

  PlusStatus status=PLUS_SUCCESS;

  // Set default transform name
  std::string defaulTransformName="IdentityTransform";
  trackedFrameList->SetDefaultFrameTransformName(defaulTransformName.c_str()); 
  for ( BufferItemUidType frameUid = videoBuffer->GetOldestItemUidInBuffer(); frameUid <= videoBuffer->GetLatestItemUidInBuffer(); ++frameUid ) 
  {

    VideoBufferItem videoItem; 
    if ( videoBuffer->GetVideoBufferItem(frameUid, &videoItem) != ITEM_OK )
    {
      LOG_ERROR("Unable to get frame from buffer with UID: " << frameUid); 
      status=PLUS_FAIL;
      continue; 
    }

    TrackedFrame trackedFrame;
    trackedFrame.SetImageData(videoItem.GetFrame());

    // Add transform 
    vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
    matrix->Identity(); 
    trackedFrame.SetCustomFrameTransform(defaulTransformName, matrix); 

    // Add filtered timestamp
    double filteredTimestamp = videoItem.GetFilteredTimestamp( videoBuffer->GetLocalTimeOffset() ); 
    std::ostringstream timestampFieldValue; 
    timestampFieldValue << std::fixed << filteredTimestamp; 
    trackedFrame.SetCustomFrameField("Timestamp", timestampFieldValue.str()); 

    // Add unfiltered timestamp
    double unfilteredTimestamp = videoItem.GetUnfilteredTimestamp( videoBuffer->GetLocalTimeOffset() ); 
    std::ostringstream unfilteredtimestampFieldValue; 
    unfilteredtimestampFieldValue << std::fixed << unfilteredTimestamp; 
    trackedFrame.SetCustomFrameField("UnfilteredTimestamp", unfilteredtimestampFieldValue.str()); 

    // Add frame number
    unsigned long frameNumber = videoItem.GetIndex();  
    std::ostringstream frameNumberFieldValue; 
    frameNumberFieldValue << std::fixed << frameNumber; 
    trackedFrame.SetCustomFrameField("FrameNumber", frameNumberFieldValue.str()); 

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
PlusStatus vtkDataCollector::Synchronize( const char* bufferOutputFolder /*= NULL*/ , bool acquireDataOnly /*= false*/ )
{
  LOG_TRACE("vtkDataCollector::Synchronize"); 

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

  int firstActiveTool = -1;
  if (this->GetTracker()->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS) {
	  LOG_ERROR("Unable to get first active tool!");
	  return PLUS_FAIL;
  }

  //************************************************************************************
  // Save local time offsets before sync
  const double prevVideoOffset = this->GetVideoSource()->GetBuffer()->GetLocalTimeOffset(); 
  const double prevTrackerOffset = this->GetTracker()->GetTool(firstActiveTool)->GetBuffer()->GetLocalTimeOffset(); 

  //************************************************************************************
  // Set the local timeoffset to 0 before synchronization 
  this->SetLocalTimeOffset(0, 0); 

  //************************************************************************************
  // Set the length of the acquisition 
  const double syncTimeLength = this->GetSynchronizer()->GetSynchronizationTimeLength(); 

  // Get the realtime tracking frequency
  double trackerFrameRate = this->GetTracker()->GetTool(firstActiveTool)->GetBuffer()->GetFrameRate(); 

  // Get the realtime video frame rate
  double videoFrameRate = this->GetVideoSource()->GetBuffer()->GetFrameRate(); 

  const int trackerBufferSize = this->GetTracker()->GetTool(firstActiveTool)->GetBuffer()->GetBufferSize(); 
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
    for ( int i = 0; i < this->GetTracker()->GetNumberOfTools(); i++ )
    {
      this->GetTracker()->GetTool(i)->GetBuffer()->SetBufferSize(syncTrackerBufferSize); 
      this->GetTracker()->GetTool(i)->GetBuffer()->Clear(); 
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
  if ( this->GetVideoSource() != NULL ) 
  {
    LOG_DEBUG("Copy video buffer ..."); 
    this->CopyVideoBuffer(videobuffer); 
  }

  vtkSmartPointer<vtkTracker> tracker = vtkSmartPointer<vtkTracker>::New(); 
  if ( this->GetTracker() != NULL )
  {
    LOG_DEBUG("Copy tracker ..."); 
    this->CopyTracker(tracker); 
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

    this->WriteTrackerToMetafile(tracker, bufferOutputFolder, trackerBufferFileName.str().c_str(), false );
    this->WriteVideoBufferToMetafile(videobuffer, bufferOutputFolder, videoBufferFileName.str().c_str() , false ); 
  }


  //************************************************************************************
  // Start synchronization 

  vtkTrackerBuffer* trackerbuffer = tracker->GetTool(firstActiveTool)->GetBuffer(); 
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
  for ( int i = 0; i < this->GetTracker()->GetNumberOfTools(); i++ )
  {
    this->GetTracker()->GetTool(i)->GetBuffer()->SetBufferSize(trackerBufferSize); 
    this->GetTracker()->GetTool(i)->GetBuffer()->Clear(); 
  }

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
void vtkDataCollector::SetLocalTimeOffset(double videoOffset, double trackerOffset)
{
  LOG_TRACE("vtkDataCollector::SetLocalTimeOffset");

  if ( this->GetVideoSource() != NULL ) 
  {	
    this->GetVideoSource()->GetBuffer()->SetLocalTimeOffset( videoOffset ); 
    this->GetVideoSource()->WriteConfiguration( vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData() ); 
  }

  if ( this->GetTracker() != NULL ) 
  {
    for ( int i = 0; i < this->GetTracker()->GetNumberOfTools(); i++ )
    {
      this->GetTracker()->GetTool(i)->GetBuffer()->SetLocalTimeOffset(trackerOffset); 
    }
    this->GetTracker()->WriteConfiguration( vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData() ); 
  }
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

    
	int firstActiveToolNumber = -1; 
	if ( this->GetTracker()->GetFirstActiveTool(firstActiveToolNumber) != PLUS_SUCCESS )
	{
		LOG_ERROR("There are no active tracker tools!"); 
		return PLUS_FAIL; 
	}

	vtkTrackerBuffer* trackerBuffer = this->GetTracker()->GetTool(firstActiveToolNumber)->GetBuffer(); 
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

  while ( latestVideoTimestamp > latestTrackerTimestamp )
  {
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
PlusStatus vtkDataCollector::GetToolStatus( double time, int toolNumber, TrackerStatus &status )
{
  LOG_TRACE("vtkDataCollector::GetToolStatus"); 
  if ( this->GetTracker() == NULL )
  {
    LOG_ERROR("Unable to get tool status without tracking device!"); 
    status = TR_MISSING; 
    return PLUS_FAIL; 
  }

  vtkTrackerBuffer* buffer = this->GetTracker()->GetTool(toolNumber)->GetBuffer(); 

  TrackerBufferItem bufferItem; 
  if ( buffer->GetTrackerBufferItemFromTime(time, &bufferItem, vtkTrackerBuffer::INTERPOLATED, false) != ITEM_OK )
  {
    LOG_ERROR("Failed to get tracker item from buffer by timestamp: " << std::fixed << time ); 
    return PLUS_FAIL; 
  } 

  status = bufferItem.GetStatus(); 
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetFrameByTime(double time, vtkImageData* vtkFrame, double& frameTimestamp)
{
  PlusVideoFrame frame;
  if ( this->GetFrameByTime(time, frame, frameTimestamp) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get frame by time: " << std::fixed << time ); 
    return PLUS_FAIL; 
  }
  return frame.CopyToVtkImage(vtkFrame);
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetFrameByTime(double time, PlusVideoFrame& frame, double& frameTimestamp)
{
  //LOG_TRACE("vtkDataCollector::GetFrameByTime"); 
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
PlusStatus vtkDataCollector::GetTransformWithTimestamp(vtkMatrix4x4* toolTransMatrix, double& transformTimestamp, TrackerStatus& status, int toolNumber/* = 0*/, bool calibratedTransform /*= false*/ )
{
  LOG_TRACE("vtkDataCollector::GetTransformWithTimestamp"); 
  if ( this->GetTracker() == NULL ) 
  {	
    LOG_ERROR("Unable to get transform - there is no tracker selected!"); 
    return PLUS_FAIL; 
  }

  TrackerBufferItem bufferItem; 
  if ( this->GetTracker()->GetTool(toolNumber)->GetBuffer()->GetLatestTrackerBufferItem(&bufferItem, calibratedTransform) != ITEM_OK )
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

  transformTimestamp = bufferItem.GetTimestamp( this->GetTracker()->GetTool(toolNumber)->GetBuffer()->GetLocalTimeOffset() ); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetTransformByTimestamp(vtkMatrix4x4* toolTransMatrix, TrackerStatus& status, double synchronizedTime, int toolNumber/* = 0*/, bool calibratedTransform /*= false*/)
{
  LOG_TRACE("vtkDataCollector::GetTransformByTimestamp"); 
  if ( this->GetTracker() == NULL ) 
  {	
    LOG_ERROR("Unable to get transform - there is no tracker selected!"); 
    return PLUS_FAIL; 
  }

  TrackerBufferItem bufferItem; 
  if ( this->GetTracker()->GetTool(toolNumber)->GetBuffer()->GetTrackerBufferItemFromTime(synchronizedTime, &bufferItem, vtkTrackerBuffer::INTERPOLATED, calibratedTransform) != ITEM_OK )
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
double vtkDataCollector::GetTransformsByTimeInterval(std::vector<vtkMatrix4x4*> &toolTransMatrixVector, std::vector<TrackerStatus> &statusVector, double startTime, double endTime, int toolNumber/* = 0*/, bool calibratedTransform /*= false*/)
{
  //LOG_TRACE("vtkDataCollector::GetTransformsByTimeInterval"); 
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

  vtkTrackerBuffer* buffer = this->GetTracker()->GetTool(toolNumber)->GetBuffer();


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
PlusStatus vtkDataCollector::GetTrackedFrame(vtkImageData* frame, vtkMatrix4x4* toolTransMatrix, TrackerStatus &status, double& synchronizedTime, int toolNumber/* = 0*/, bool calibratedTransform /*= false*/)
{
  LOG_TRACE("vtkDataCollector::GetTrackedFrame - vtkImageData"); 

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
    if ( this->GetTransformByTimestamp( toolTransMatrix, status, synchronizedTime, toolNumber, calibratedTransform) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get transform bt timestamp: " << std::fixed << synchronizedTime ); 
      return PLUS_FAIL; 
    }
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetTrackedFrameList(double& frameTimestamp, vtkTrackedFrameList* trackedFrameList, 
                                                 long validationRequirements, 
                                                 const char* frameTransformNameForPositionValidation/*=NULL*/ )
{
  if ( trackedFrameList == NULL )
  {
    LOG_ERROR("Unable to get tracked frame list - output tracked frmae list is NULL!"); 
    return PLUS_FAIL; 
  }

  PlusStatus status = PLUS_SUCCESS; 
  double oldestTimestamp(0); 
  if ( this->VideoSource->GetBuffer()->GetOldestTimeStamp(oldestTimestamp) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get oldest timestamp from buffer!"); 
    return PLUS_FAIL; 
  }

  double mostRecentTimestamp(0); 
  if ( this->GetMostRecentTimestamp(mostRecentTimestamp) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to get most recent timestamp!"); 
    return PLUS_FAIL; 
  }

  // Check input frameTimestamp to be in a valid range 
  if ( frameTimestamp < 0.0001 || frameTimestamp > mostRecentTimestamp )
  {
    frameTimestamp = mostRecentTimestamp; 
  }
  else if ( frameTimestamp > frameTimestamp )
  {
    frameTimestamp = oldestTimestamp; 
  }

  
  BufferItemUidType videoUid(0); 
  if ( this->VideoSource->GetBuffer()->GetItemUidFromTime(frameTimestamp, videoUid) != ITEM_OK )
  {
    LOG_ERROR("Failed to get video buffer item UID from time: " << std::fixed << frameTimestamp ); 
    return PLUS_FAIL; 
  }

  do 
  {
    // Get tracked frame from buffer
    TrackedFrame trackedFrame; 
    if ( this->GetTrackedFrameByTime(frameTimestamp, &trackedFrame) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to get tracked frame by time: " << std::fixed << frameTimestamp ); 
      status = PLUS_FAIL; 
    }
    else 
    {
      // Add tracked frame to the list 
      if ( trackedFrameList->ValidateData(&trackedFrame, validationRequirements, frameTransformNameForPositionValidation )
        && trackedFrameList->AddTrackedFrame(&trackedFrame) != PLUS_SUCCESS )
      {
        LOG_ERROR("Unable to add tracked frame to the list!" ); 
        status = PLUS_FAIL; 
      }

      // Get the timestamp of the next item in the buffer
      if ( this->VideoSource->GetBuffer()->GetTimeStamp(++videoUid, frameTimestamp) != PLUS_SUCCESS )
      {
          LOG_ERROR("Unable to get timestamp from video buffer by UID: " << videoUid); 
          status = PLUS_FAIL; 
      }
    }

  } while(status == PLUS_SUCCESS && frameTimestamp <= mostRecentTimestamp); 

  return status; 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetTrackedFrame(TrackedFrame* trackedFrame, bool calibratedTransform /*= false*/)
{
  LOG_TRACE("vtkDataCollector::GetTrackedFrame - TrackedFrame"); 

  double mostRecentFrameTimestamp(0);
  if (this->GetMostRecentTimestamp(mostRecentFrameTimestamp) != PLUS_SUCCESS) 
  {
    LOG_ERROR("Failed to get most recent timestamp from the buffer!"); 
    return PLUS_FAIL;
  }

  return this->GetTrackedFrameByTime(mostRecentFrameTimestamp, trackedFrame, calibratedTransform); 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetTrackedFrameByTime(double time, TrackedFrame* trackedFrame, bool calibratedTransform /*= false*/)
{
  //LOG_TRACE("vtkDataCollector::GetTrackedFrameByTime - TrackedFrame");
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

	  int toolNumber = this->GetTracker()->GetFirstPortNumberByType(TRACKER_TOOL_PROBE);
	  if (toolNumber < 0)
	  {
		  if (this->GetTracker()->GetFirstActiveTool(toolNumber) != PLUS_SUCCESS)
		  {
			  LOG_ERROR("There are no active tools!"); 
			  return PLUS_FAIL; 
		  }
	  }

    TrackerStatus status;
	  if ( this->GetToolStatus(synchronizedTime, toolNumber, status ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get tool status by time: " << std::fixed << synchronizedTime ); 
      return PLUS_FAIL; 
    }
    trackedFrame->SetStatus(status);

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
const char* vtkDataCollector::GetDefaultFrameTransformName(int toolNumber)
{
	return this->GetTracker()->GetTool(toolNumber)->GetToolName(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetTrackedFrameByTime(double time, vtkImageData* frame, std::vector<vtkMatrix4x4*> &toolTransforms, std::vector<std::string> &toolTransformNames, std::vector<TrackerStatus> &status, double& synchronizedTime, bool calibratedTransform /*= false*/)
{
  //LOG_TRACE("vtkDataCollector::GetTrackedFrameByTime - vtkImageData");
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
    for ( int tool = 0; tool < this->GetTracker()->GetNumberOfTools(); tool++ )
    {
      if ( this->GetTracker()->GetTool(tool)->GetEnabled() )
      {
        TrackerStatus trackerStatus = TR_OK; 

        vtkSmartPointer<vtkMatrix4x4> toolTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
        this->GetTransformByTimestamp( toolTransMatrix, trackerStatus, synchronizedTime, tool, calibratedTransform); 

        toolTransMatrix->Register(NULL); 
        toolTransforms.push_back(toolTransMatrix); 

        std::string transformName( this->GetTracker()->GetTool(tool)->GetToolName() ); 
        toolTransformNames.push_back(transformName); 

        status.push_back(trackerStatus); 
      }
    }
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
int vtkDataCollector::RequestData( vtkInformation* vtkNotUsed( request ), vtkInformationVector**  inputVector, vtkInformationVector* outputVector )
{
  LOG_TRACE("vtkDataCollector::RequestData");

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

  currentVideoBufferItem.GetFrame().CopyToVtkImage(outData);

  const double globalTime = currentVideoBufferItem.GetTimestamp( this->GetVideoSource()->GetBuffer()->GetLocalTimeOffset() ); 

  if( this->GetTracker() != NULL && this->TrackingEnabled )
  {
    for ( int i = 0; i < this->GetTracker()->GetNumberOfTools(); i++)
    {
      if ( this->GetTracker()->GetTool(i)->GetBuffer()->GetNumberOfItems() < 1 )
      {
        // If the tracker tool buffer is empty, we can return immediately 
        LOG_DEBUG("Cannot request data from tracker, the tracker tool buffer is empty!"); 
        return 1;
      }

      TrackerBufferItem bufferItem; 
      if ( this->GetTracker()->GetTool(i)->GetBuffer()->GetLatestTrackerBufferItem(&bufferItem, false) != ITEM_OK )
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

      this->ToolTransMatrices[i]->DeepCopy(toolTransMatrix); 
      this->ToolTransMatrices[i]->Modified(); 
      this->ToolStatus[i] = bufferItem.GetStatus(); 
    }
  }

  return 1;
} 

//------------------------------------------------------------------------------
PlusStatus vtkDataCollector::ReadConfiguration(vtkXMLDataElement* aConfigurationData)
{
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
	vtkSmartPointer<vtkXMLDataElement> dataCollectionConfig = aConfigurationData->FindNestedElementWithName("USDataCollection");
	if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find USDataCollection element in XML tree!");
		return PLUS_FAIL;
	}

  // Check data collection configuration version
  double usDataCollectionVersion = 0; 
  if ( dataCollectionConfig->GetScalarAttribute("version", usDataCollectionVersion) )
  {
    if ( usDataCollectionVersion < this->DataCollectionConfigVersion )
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
PlusStatus vtkDataCollector::ReadTrackerProperties(vtkXMLDataElement* aConfigurationData)
{
  LOG_TRACE("vtkDataCollector::ReadTrackerProperties");

	vtkSmartPointer<vtkXMLDataElement> dataCollectionConfig = aConfigurationData->FindNestedElementWithName("USDataCollection");
	if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find USDataCollection element in XML tree!");
		return PLUS_FAIL;
	}

  vtkSmartPointer<vtkXMLDataElement> trackerConfig = dataCollectionConfig->FindNestedElementWithName("Tracker"); 
  if (trackerConfig == NULL) 
  {
    LOG_ERROR("Cannot find Tracker element in XML tree!");
		return PLUS_FAIL;
  }

  LOG_TRACE("vtkDataCollector::ReadTrackerProperties");
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
  //******************* Flock Tracker ***************************
  else if ( STRCASECMP("FlockTracker", type)==0) 
  {
#ifdef PLUS_USE_FLOCK
    LOG_DEBUG("Tracker type: Flock tracker"); 
    this->SetTrackerType(TRACKER_FLOCK); 
    vtkSmartPointer<vtkFlockTracker> tracker = vtkSmartPointer<vtkFlockTracker>::New();
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
PlusStatus vtkDataCollector::ReadImageAcquisitionProperties(vtkXMLDataElement* aConfigurationData)
{
  LOG_TRACE("vtkDataCollector::ReadImageAcquisitionProperties");

	vtkSmartPointer<vtkXMLDataElement> dataCollectionConfig = aConfigurationData->FindNestedElementWithName("USDataCollection");
	if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find USDataCollection element in XML tree!");
		return PLUS_FAIL;
	}

  vtkSmartPointer<vtkXMLDataElement> imageAcquisitionConfig = dataCollectionConfig->FindNestedElementWithName("ImageAcquisition"); 
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
PlusStatus vtkDataCollector::ReadSynchronizationProperties(vtkXMLDataElement* aConfigurationData)
{
  LOG_TRACE("vtkDataCollector::ReadSynchronizationProperties");

	vtkSmartPointer<vtkXMLDataElement> dataCollectionConfig = aConfigurationData->FindNestedElementWithName("USDataCollection");
	if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find USDataCollection element in XML tree!");
		return PLUS_FAIL;
	}

  vtkSmartPointer<vtkXMLDataElement> synchronizationConfig = dataCollectionConfig->FindNestedElementWithName("Synchronization"); 
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
void vtkDataCollector::SetTrackingOnly(bool trackingOnly)
{
  LOG_TRACE("vtkDataCollector::SetTrackingOnly");
  if ( trackingOnly )
  {
    this->VideoEnabled = false;
  }
  else
  {
    this->VideoEnabled = true;
  }

  if ( this->GetInitialized() && this->GetVideoSource() != NULL )
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
    this->TrackingEnabled = true;
  }

  if ( this->GetInitialized() && this->GetTracker() != NULL )
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
int vtkDataCollector::GetNumberOfTools()
{
  LOG_TRACE("vtkDataCollector::GetNumberOfTools");
  int ret = 0;
  if ( this->GetTracker() != NULL )
  {
    ret = this->GetTracker()->GetNumberOfTools();
  }
  return ret;
}
