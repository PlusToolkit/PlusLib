#include "PlusConfigure.h"
#include "vtkDataCollector.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkXMLUtilities.h"
#include "vtkBMPWriter.h"

#include "vtkTrackerTool.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkPointData.h"
#include "vtkImageFlip.h"
#include "vtkImageExport.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMetaImageSequenceIO.h"


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
#ifdef PLUS_USE_AMS_TRACKER
#include "vtkAMSTracker.h"
#endif
#ifdef PLUS_USE_Ascension3DG
#include "vtkAscension3DGTracker.h"
#endif
#include "vtkBufferedTracker.h"
#include "vtkFakeTracker.h"
#include "vtkSavedDataTracker.h"

//----------------------------------------------------------------------------
// Video source
#include "vtkVideoSource2.h"
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
#ifdef PLUS_USE_SONIX_VIDEO
#include "vtkSonixVideoSource.h"
#endif
#ifdef PLUS_USE_ICCAPTURING_VIDEO
#include "vtkICCapturingSource.h"
#endif
#include "vtkBufferedVideoSource.h"
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

//----------------------------------------------------------------------------
vtkDataCollector::vtkDataCollector()
{	
    this->VideoSource = NULL;
    this->Tracker = NULL; 
    this->Synchronizer = NULL; 
    this->ProgressBarUpdateCallbackFunction = NULL; 

    this->ConfigurationData = NULL; 
    this->ConfigFileName = NULL;
    this->ConfigFileVersion = 2.0; 

    this->DeviceSetName = NULL; 
    this->DeviceSetDescription = NULL; 
    this->ToolTransMatrices.reserve(0); 
    this->ToolFlags.reserve(0); 

    this->InitializedOff(); 
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
    this->SetConfigFileName(NULL); 
    this->SetDeviceSetName(NULL); 
    this->SetDeviceSetDescription(NULL); 

    for ( unsigned int i = 0; i < this->ToolTransMatrices.size(); i++ ) 
    {
        this->ToolTransMatrices[i]->Delete(); 
        this->ToolTransMatrices[i]=NULL; 
    }

    if ( this->ConfigurationData != NULL ) 
    {
        this->ConfigurationData->Delete(); 
        this->ConfigurationData = NULL; 
    }
}


//----------------------------------------------------------------------------
void vtkDataCollector::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);

    if ( this->ConfigFileName != NULL )
    {
        os << indent << "Config file name: " << this->ConfigFileName << std::endl; 
    }

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

    // VideoSource can be null if the ACQUISITION_TYPE == SYNCHRO_VIDEO_NONE 
    if ( this->GetVideoSource() != NULL ) 
    {
        this->GetVideoSource()->Initialize();

        if (!this->GetVideoSource()->GetInitialized())
        {
            LOG_ERROR("Unable to initialize video source!"); 
            return PLUS_FAIL;  
        }

        this->SetInputConnection(this->GetVideoSource()->GetOutputPort());
    }

    // Tracker can be null if the TRACKER_TYPE == TRACKER_NONE 
    if ( this->GetTracker() != NULL ) 
    {
        if ( !this->GetTracker()->Connect() )
        {
            LOG_ERROR("Unable to initialize tracker!"); 
            return PLUS_FAIL;
        }
    }

    return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::Disconnect()
{
    LOG_TRACE("vtkDataCollector::Disconnect"); 

    if ( this->GetVideoSource() != NULL ) 
    {
        this->GetVideoSource()->Disconnect(); 
    }

    if ( this->GetTracker() != NULL ) 
    {
        this->GetTracker()->Disconnect(); 
    }

    return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::Start()
{
    LOG_TRACE("vtkDataCollector::Start"); 

    if ( this->GetVideoEnabled() )
    {
        if ( this->GetVideoSource() != NULL && !this->GetVideoSource()->GetRecording())
        {
             this->GetVideoSource()->StartRecording();
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
            this->ToolFlags.clear(); 
            for ( int i = 0; i < this->GetTracker()->GetNumberOfTools(); i++ )
            {
                vtkMatrix4x4* matrix = vtkMatrix4x4::New(); 
                this->ToolTransMatrices.push_back(matrix); 
                this->ToolFlags.push_back(0); 
            }

            this->GetTracker()->StartTracking(); 
        }
    }
    else
    {
        LOG_DEBUG("Start data collection in video only mode."); 
    }

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

    this->GetTracker()->GetTool(toolNumber)->GetBuffer()->Lock(); 
    trackerBuffer->Lock(); 

    trackerBuffer->DeepCopy(this->GetTracker()->GetTool(toolNumber)->GetBuffer()); 

    trackerBuffer->Unlock();
    this->GetTracker()->GetTool(toolNumber)->GetBuffer()->Unlock(); 

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

    tracker->Lock(); 

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

    for ( int i = 0 ; i < numberOfItems; i++ ) 
    {
        //Create fake image 
        TrackedFrame::ImageType::Pointer frame = TrackedFrame::ImageType::New(); 
        TrackedFrame::ImageType::SizeType size = {1, 1};
        TrackedFrame::ImageType::IndexType start = {0,0};
        TrackedFrame::ImageType::RegionType region;
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
        trackedFrame.ImageData = frame;

        const int defaultToolPortNumber = tracker->GetDefaultTool(); 
        const double frameTimestamp = tracker->GetTool(defaultToolPortNumber)->GetBuffer()->GetFilteredTimeStamp(( numberOfItems - 1 ) - i); 
        
        // Add main tool timestamp
        std::ostringstream timestampFieldValue; 
        timestampFieldValue << std::fixed << frameTimestamp; 
        trackedFrame.SetCustomFrameField("Timestamp", timestampFieldValue.str()); 

        // Add main tool unfiltered timestamp
        std::ostringstream unfilteredtimestampFieldValue; 
        unfilteredtimestampFieldValue << std::fixed << tracker->GetTool(defaultToolPortNumber)->GetBuffer()->GetUnfilteredTimeStamp(( numberOfItems - 1 ) - i); 
        trackedFrame.SetCustomFrameField("UnfilteredTimestamp", unfilteredtimestampFieldValue.str()); 

        // Add main tool frameNumber
        std::ostringstream frameNumberFieldValue; 
        frameNumberFieldValue << std::fixed << tracker->GetTool(defaultToolPortNumber)->GetBuffer()->GetFrameNumber(( numberOfItems - 1 ) - i); 
        trackedFrame.SetCustomFrameField("FrameNumber", frameNumberFieldValue.str()); 

        // Add main tool status
        long flag = tracker->GetTool(defaultToolPortNumber)->GetBuffer()->GetFlags(( numberOfItems - 1 ) - i); 
        std::string status = tracker->ConvertFlagToString(flag); 
        trackedFrame.SetCustomFrameField("Status", status); 

        // Set default transform name
        trackedFrame.DefaultFrameTransformName = tracker->GetTool(defaultToolPortNumber)->GetToolName(); 

        // Add transforms
        for ( int tool = 0; tool < tracker->GetNumberOfTools(); tool++ )
        {
            if ( tracker->GetTool(tool)->GetEnabled() )
            {
                const int index = tracker->GetTool(tool)->GetBuffer()->GetIndexFromTime(frameTimestamp); 

                vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
                tracker->GetTool(tool)->GetBuffer()->GetMatrix(matrix, index); 
                trackedFrame.SetCustomFrameTransform(tracker->GetTool(tool)->GetToolName(), matrix); 

                const char* calibMatrixName = tracker->GetTool(tool)->GetCalibrationMatrixName(); 
                vtkMatrix4x4* calibmatrix = tracker->GetTool(tool)->GetCalibrationMatrix(); 

                if ( calibMatrixName != NULL && calibmatrix != NULL )
                {
                    std::ostringstream strcalibmatrix; 
                    for ( int r = 0; r < 4; r++ )
                    {
                        for ( int c = 0; c < 4; c++ )
                        {
                            strcalibmatrix << std::fixed << calibmatrix->GetElement(r,c) << " "; 
                        }
                    }

                    trackedFrame.SetCustomField(calibMatrixName, strcalibmatrix.str()); 
                }

            }
        }

        // Add tracked frame to the list
        trackedFrameList->AddTrackedFrame(&trackedFrame); 
    }

    // Save tracked frames to metafile
    trackedFrameList->SaveToSequenceMetafile(outputFolder, metaFileName, vtkTrackedFrameList::SEQ_METAFILE_MHA, useCompression); 

    tracker->Unlock();

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
    VideoBufferItem videoItem; 

    for ( BufferItemUidType frameUid = videoBuffer->GetOldestItemUidInBuffer(); frameUid <= videoBuffer->GetLatestItemUidInBuffer(); ++frameUid ) 
    {

        if ( videoBuffer->GetVideoBufferItem(frameUid, &videoItem) != ITEM_OK )
        {
            LOG_ERROR("Unable to get frame from buffer with UID: " << frameUid); 
            status=PLUS_FAIL;
            continue; 
        }

        TrackedFrame trackedFrame; 
        trackedFrame.ImageData = videoItem.GetFrame();

        // Set default transform name
        trackedFrame.DefaultFrameTransformName = "IdentityTransform"; 

        // Add transform 
        vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
        matrix->Identity(); 
        trackedFrame.SetCustomFrameTransform(trackedFrame.DefaultFrameTransformName, matrix); 

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
    trackedFrameList->SaveToSequenceMetafile(outputFolder, metaFileName, vtkTrackedFrameList::SEQ_METAFILE_MHA, useCompression); 

    return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::Synchronize()
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

    //************************************************************************************
    // Save local time offsets before sync
    const double prevVideoOffset = this->GetVideoSource()->GetBuffer()->GetLocalTimeOffset(); 
    const double prevTrackerOffset = this->GetTracker()->GetTool(this->GetDefaultToolPortNumber())->GetBuffer()->GetLocalTimeOffset(); 

    //************************************************************************************
    // Set the local timeoffset to 0 before synchronization 
    this->SetLocalTimeOffset(0, 0); 

    //************************************************************************************
    // Set the length of the acquisition 
    const double syncTimeLength = this->GetSynchronizer()->GetSynchronizationTimeLength(); 

    // Get the realtime tracking frequency
    this->GetTracker()->GetTool(this->GetDefaultToolPortNumber())->GetBuffer()->Lock(); 
    double trackerFrameRate = this->GetTracker()->GetTool(this->GetDefaultToolPortNumber())->GetBuffer()->GetFrameRate(); 
    this->GetTracker()->GetTool(this->GetDefaultToolPortNumber())->GetBuffer()->Unlock(); 

    // Get the realtime video frame rate
    double videoFrameRate = this->GetVideoSource()->GetBuffer()->GetFrameRate(); 

    const int trackerBufferSize = this->GetTracker()->GetTool(this->GetDefaultToolPortNumber())->GetBuffer()->GetBufferSize(); 
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
            this->GetTracker()->GetTool(i)->GetBuffer()->Lock(); 
            this->GetTracker()->GetTool(i)->GetBuffer()->SetBufferSize(syncTrackerBufferSize); 
            this->GetTracker()->GetTool(i)->GetBuffer()->Clear(); 
            this->GetTracker()->GetTool(i)->GetBuffer()->Unlock(); 
        }
    }
    else
    {
        for ( int i = 0; i < this->GetTracker()->GetNumberOfTools(); i++ )
        {
            this->GetTracker()->GetTool(i)->GetBuffer()->Lock(); 
            this->GetTracker()->GetTool(i)->GetBuffer()->Clear(); 
            this->GetTracker()->GetTool(i)->GetBuffer()->Unlock(); 
        }
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

    if ( PlusLogger::Instance()->GetLogLevel() <=  PlusLogger::LOG_LEVEL_DEBUG )
    {
        LOG_INFO(">>>>>>>>>>>> Save temporal calibration buffers to file ... "); 
        this->WriteTrackerToMetafile(tracker, "./", "DataCollectorSyncTrackerBuffer", false );
        this->WriteVideoBufferToMetafile(videobuffer, "./", "DataCollectorSyncVideoBuffer", false ); 
    }


    //************************************************************************************
    // Start synchronization 
    this->GetSynchronizer()->SetProgressBarUpdateCallbackFunction(ProgressBarUpdateCallbackFunction); 
    this->GetSynchronizer()->SetSyncStartTime(syncStartTime); 

    vtkTrackerBuffer* trackerbuffer = tracker->GetTool(this->GetDefaultToolPortNumber())->GetBuffer(); 

    LOG_DEBUG("Tracker buffer size: " << trackerbuffer->GetBufferSize()); 
    LOG_DEBUG("Tracker buffer elements: " << trackerbuffer->GetNumberOfItems()); 
    LOG_DEBUG("Video buffer size: " << videobuffer->GetBufferSize()); 
    LOG_DEBUG("Video buffer elements: " << videobuffer->GetNumberOfItems()); 
    this->GetSynchronizer()->SetTrackerBuffer(trackerbuffer); 
    this->GetSynchronizer()->SetVideoBuffer(videobuffer); 

    this->GetSynchronizer()->Synchronize(); 

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
        this->GetTracker()->GetTool(i)->GetBuffer()->Lock(); 
        this->GetTracker()->GetTool(i)->GetBuffer()->SetBufferSize(trackerBufferSize); 
        this->GetTracker()->GetTool(i)->GetBuffer()->Clear(); 
        this->GetTracker()->GetTool(i)->GetBuffer()->Unlock(); 
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
    }

    if ( this->GetTracker() != NULL ) 
    {
        for ( int i = 0; i < this->GetTracker()->GetNumberOfTools(); i++ )
        {
            this->GetTracker()->GetTool(i)->GetBuffer()->SetLocalTimeOffset(trackerOffset); 
        }
    }
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetMostRecentTimestamp(double &ts)
{
    LOG_TRACE("vtkDataCollector::GetMostRecentTimestamp"); 
    ts=0;

    if ( this->GetVideoSource() == NULL )
    {
        LOG_ERROR("Unable to get most recent timestamp without video source!"); 
        return PLUS_FAIL; 
    }

    // Get the most recent timestamp from the buffer
    if ( this->GetVideoSource()->GetBuffer()->GetLatestTimeStamp(ts) != ITEM_OK )
    {
        LOG_WARNING("Unable to get most recent video item from video buffer!"); 
        return PLUS_FAIL;
    }

    return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
std::string vtkDataCollector::GetDefaultToolName()
{
    LOG_TRACE("vtkDataCollector::GetDefaultToolName"); 

    std::string defToolName; 
    if ( this->GetTracker() != NULL )
    {
        defToolName = this->GetTracker()->GetTool(this->GetDefaultToolPortNumber())->GetToolName(); 
    }
    else
    {
        LOG_ERROR("Unable to get default tool name without tracking device!"); 
    }

    return defToolName; 
}

//----------------------------------------------------------------------------
long vtkDataCollector::GetDefaultToolStatus( double time )
{
    LOG_TRACE("vtkDataCollector::GetDefaultToolStatus"); 
    if ( this->GetTracker() == NULL )
    {
        LOG_ERROR("Unable to get default tool status without tracking device!"); 
        return TR_MISSING; 
    }

    vtkTrackerBuffer* buffer = this->GetTracker()->GetTool(this->GetDefaultToolPortNumber())->GetBuffer(); 

    buffer->Lock(); 
    int index = buffer->GetIndexFromTime(time); 
    long status = buffer->GetFlags(index); 
    buffer->Unlock(); 

    return status; 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetFrameByTime(const double time, vtkImageData* frame, double& frameTimestamp)
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

    if ( UsImageConverterCommon::ConvertItkImageToVtkImage(currentVideoBufferItem.GetFrame(), frame) != PLUS_SUCCESS )
    {
        LOG_ERROR("Failed to copy image data for UID: " << frameUID);
        return PLUS_FAIL;
    }

    // Get frame timestamp 
    frameTimestamp = currentVideoBufferItem.GetTimestamp( this->GetVideoSource()->GetBuffer()->GetLocalTimeOffset() ); 
    return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetTransformWithTimestamp(vtkMatrix4x4* toolTransMatrix, double& transformTimestamp, long& flags, int toolNumber/* = 0*/, bool calibratedTransform /*= false*/ )
{
    //LOG_TRACE("vtkDataCollector::GetTransformWithTimestamp"); 
    if ( this->GetTracker() == NULL ) 
    {	
        LOG_ERROR("Unable to get transform - there is no tracker selected!"); 
        return PLUS_FAIL; 
    }

    this->GetTracker()->GetTool(toolNumber)->GetBuffer()->Lock(); 
    {
        flags = this->GetTracker()->GetTool(toolNumber)->GetBuffer()->GetFlags(vtkDataCollector::MostRecentFrameBufferIndex); 

        // Get the most recent transform from the buffer (if the video slower we have a chance to get the right transform)
        if (calibratedTransform)
        {
            this->GetTracker()->GetTool(toolNumber)->GetBuffer()->GetCalibratedMatrix(toolTransMatrix, vtkDataCollector::MostRecentFrameBufferIndex); 
        }
        else
        {
            this->GetTracker()->GetTool(toolNumber)->GetBuffer()->GetMatrix(toolTransMatrix, vtkDataCollector::MostRecentFrameBufferIndex); 
        }

        transformTimestamp = this->GetTracker()->GetTool(toolNumber)->GetBuffer()->GetTimeStamp(vtkDataCollector::MostRecentFrameBufferIndex); 
    }
    this->GetTracker()->GetTool(toolNumber)->GetBuffer()->Unlock(); 

    return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetTransformByTimestamp(vtkMatrix4x4* toolTransMatrix, long& flags, const double synchronizedTime, int toolNumber/* = 0*/, bool calibratedTransform /*= false*/)
{
    //LOG_TRACE("vtkDataCollector::GetTransformByTimestamp"); 
    if ( this->GetTracker() == NULL ) 
    {	
        LOG_ERROR("Unable to get transform - there is no tracker selected!"); 
        return PLUS_FAIL; 
    }

    this->GetTracker()->GetTool(toolNumber)->GetBuffer()->Lock(); 
    {
        int frameBufferIndex = this->GetTracker()->GetTool(toolNumber)->GetBuffer()->GetIndexFromTime(synchronizedTime); 

        if (calibratedTransform)
        {
            this->GetTracker()->GetTool(toolNumber)->GetBuffer()->GetCalibratedMatrix(toolTransMatrix, frameBufferIndex); 
        }
        else
        {
            this->GetTracker()->GetTool(toolNumber)->GetBuffer()->GetMatrix(toolTransMatrix, frameBufferIndex); 
        }

        flags = this->GetTracker()->GetTool(toolNumber)->GetBuffer()->GetFlags(frameBufferIndex); 
    }
    this->GetTracker()->GetTool(toolNumber)->GetBuffer()->Unlock(); 

    return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
double vtkDataCollector::GetTransformsByTimeInterval(std::vector<vtkMatrix4x4*> &toolTransMatrixVector, std::vector<long> &flagsVector, const double startTime, const double endTime, int toolNumber/* = 0*/, bool calibratedTransform /*= false*/)
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
    flagsVector.clear();

    vtkTrackerBuffer* buffer = this->GetTracker()->GetTool(toolNumber)->GetBuffer();

    buffer->Lock();


    int frameBufferStartIndex = 1 - buffer->GetBufferSize(); // Start index is initialized with the oldest one
    if (startTime > 0) { // If specific startTime was given then get start index for the item after the one with the specified timestamp
        frameBufferStartIndex = buffer->GetIndexFromTime(startTime) + 1; // The transform at start time is skipped
    } else {
        if (buffer->GetBufferSize() > buffer->GetNumberOfItems()) {
            frameBufferStartIndex = 1 - buffer->GetNumberOfItems();
        }
    }

    double returnTime;
    int frameBufferEndIndex = 0; // End index is initialized as the most recent data (it remains so if endTime is -1)
    if (endTime > 0) { // If specific endTime was given then get corresponding end index
        frameBufferEndIndex = buffer->GetIndexFromTime(endTime);
        returnTime = endTime;
    } else {
        returnTime = buffer->GetTimeStamp(0);
    }

    if (frameBufferStartIndex > frameBufferEndIndex) {
        frameBufferStartIndex -= buffer->GetBufferSize();
    }

    for (int i=frameBufferStartIndex; i<=frameBufferEndIndex; ++i) {
        vtkMatrix4x4* currentTransform = vtkMatrix4x4::New();

        if (calibratedTransform)
        {
            buffer->GetCalibratedMatrix(currentTransform, i);
        }
        else
        {
            buffer->GetMatrix(currentTransform, i);
        }

        long currentFlags = buffer->GetFlags(i);

        toolTransMatrixVector.push_back(currentTransform);
        flagsVector.push_back(currentFlags);
    }

    buffer->Unlock();

    return returnTime;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetTrackedFrame(vtkImageData* frame, vtkMatrix4x4* toolTransMatrix, long& flags, double& synchronizedTime, int toolNumber/* = 0*/, bool calibratedTransform /*= false*/)
{
    //LOG_TRACE("vtkDataCollector::GetTrackedFrame - vtkImageData"); 
    PlusStatus frameStatus = PLUS_SUCCESS; 
    if ( this->GetVideoEnabled() )
    {
        double mostRecentFrameTimestamp(0);
        if (this->GetMostRecentTimestamp(mostRecentFrameTimestamp)!=PLUS_SUCCESS) 
        {
            frameStatus = PLUS_FAIL;
        }
        else
        {
            frameStatus = this->GetFrameByTime(mostRecentFrameTimestamp, frame, synchronizedTime); 
        }

    }

    PlusStatus transformStatus=PLUS_SUCCESS; 
    if ( this->GetTrackingEnabled() )
    {
        transformStatus	= this->GetTransformByTimestamp( toolTransMatrix, flags, synchronizedTime, toolNumber, calibratedTransform); 
    }

    if (frameStatus==PLUS_FAIL || transformStatus==PLUS_FAIL)
    {
        return PLUS_FAIL;
    }

    return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetTrackedFrame(TrackedFrame* trackedFrame, bool calibratedTransform /*= false*/)
{
    //LOG_TRACE("vtkDataCollector::GetTrackedFrame - TrackedFrame"); 
    double mostRecentFrameTimestamp(0);
    if (this->GetMostRecentTimestamp(mostRecentFrameTimestamp)!=PLUS_SUCCESS) 
    {
        return PLUS_FAIL;
    }
    return this->GetTrackedFrameByTime(mostRecentFrameTimestamp, trackedFrame, calibratedTransform); 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetTrackedFrameByTime(const double time, TrackedFrame* trackedFrame, bool calibratedTransform /*= false*/)
{
    //LOG_TRACE("vtkDataCollector::GetTrackedFrameByTime - TrackedFrame");
    double synchronizedTime(0); 

    if ( this->GetVideoEnabled() )
    {
        // Allocate memory for new frame
        vtkSmartPointer<vtkImageData> vtkimage = vtkSmartPointer<vtkImageData>::New(); 
        vtkimage->CopyStructure( this->GetOutput() ); 
        vtkimage->SetScalarTypeToUnsignedChar(); 
        vtkimage->AllocateScalars(); 

        // Get the frame by time
        if ( ! this->GetFrameByTime(time, vtkimage, synchronizedTime) )
        {
            LOG_ERROR("Failed to get tracked frame by time: " << std::fixed << time ); 
            return PLUS_FAIL; 
        }

        // Convert vtkImage to itkimage
        TrackedFrame::ImageType::Pointer itkimage = TrackedFrame::ImageType::New(); 
        UsImageConverterCommon::ConvertVtkImageToItkImage(vtkimage, itkimage); 

        //Add all information to the tracked frame
        trackedFrame->ImageData = itkimage; 
    }

    if ( this->GetTrackingEnabled() && this->GetTracker() != NULL )
    {
        // Get tracker buffer values 
        std::map<std::string, std::string> toolsBufferMatrices; 
        std::map<std::string, std::string> toolsCalibrationMatrices; 
        std::map<std::string, std::string> toolsStatuses; 

        if ( !this->GetVideoEnabled() )
        {
            this->GetTracker()->GetTool( this->GetTracker()->GetDefaultTool() )->GetBuffer()->Lock(); 
            int idx = this->GetTracker()->GetTool( this->GetTracker()->GetDefaultTool() )->GetBuffer()->GetIndexFromTime(time); 
            synchronizedTime = this->GetTracker()->GetTool( this->GetTracker()->GetDefaultTool() )->GetBuffer()->GetTimeStamp(idx); 
            this->GetTracker()->GetTool( this->GetTracker()->GetDefaultTool() )->GetBuffer()->Unlock(); 
        }

        this->GetTracker()->GetTrackerToolBufferStringList(synchronizedTime, toolsBufferMatrices, toolsCalibrationMatrices, toolsStatuses, calibratedTransform); 

        trackedFrame->Status = this->GetDefaultToolStatus(synchronizedTime); 
        trackedFrame->DefaultFrameTransformName = this->GetDefaultToolName(); 

        for ( std::map<std::string, std::string>::iterator it = toolsBufferMatrices.begin(); it != toolsBufferMatrices.end(); it++ )
        {
            // Set tool buffer values 
            trackedFrame->SetCustomFrameField(it->first, it->second); 
        }

        for ( std::map<std::string, std::string>::iterator it = toolsCalibrationMatrices.begin(); it != toolsCalibrationMatrices.end(); it++ )
        {
            // Set tool calibration values 
            trackedFrame->SetCustomField(it->first, it->second); 
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
    trackedFrame->Timestamp = synchronizedTime; 

    // Save frame timestamp
    std::ostringstream strTimestamp; 
    strTimestamp << std::fixed << trackedFrame->Timestamp; 
    trackedFrame->SetCustomFrameField("Timestamp", strTimestamp.str()); 

    return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetTrackedFrameByTime(const double time, vtkImageData* frame, std::vector<vtkMatrix4x4*> &toolTransforms, std::vector<std::string> &toolTransformNames, std::vector<long> &flags, double& synchronizedTime, bool calibratedTransform /*= false*/)
{
    //LOG_TRACE("vtkDataCollector::GetTrackedFrameByTime - vtkImageData");
    toolTransforms.clear(); 
    toolTransformNames.clear(); 
    flags.clear(); 

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
                long flag(0); 

                vtkSmartPointer<vtkMatrix4x4> toolTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
                this->GetTransformByTimestamp( toolTransMatrix, flag, synchronizedTime, tool, calibratedTransform); 

                toolTransMatrix->Register(NULL); 
                toolTransforms.push_back(toolTransMatrix); 

                std::string transformName( this->GetTracker()->GetTool(tool)->GetToolName() ); 
                toolTransformNames.push_back(transformName); 

                flags.push_back(flag); 
            }
        }
    }

    return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::GetTrackedFrame(vtkImageData* frame, std::vector<vtkMatrix4x4*> &toolTransforms, std::vector<std::string> &toolTransformNames, std::vector<long> &flags, double& synchronizedTime, bool calibratedTransform /*= false*/)
{
    //LOG_TRACE("vtkDataCollector::GetTrackedFrame - vtkImageData");
    // Get the most recent frame timestamp 
    double mostRecentFrameTimestamp(0);
    if (this->GetMostRecentTimestamp(mostRecentFrameTimestamp)!=PLUS_SUCCESS) 
    {
        return PLUS_FAIL;
    }
    return this->GetTrackedFrameByTime(mostRecentFrameTimestamp, frame, toolTransforms, toolTransformNames, flags, synchronizedTime, calibratedTransform); 
}

//----------------------------------------------------------------------------
int vtkDataCollector::RequestData( vtkInformation* vtkNotUsed( request ), vtkInformationVector**  inputVector, vtkInformationVector* outputVector )
{
    //LOG_TRACE("vtkDataCollector::RequestData");
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    vtkImageData *inData = vtkImageData::SafeDownCast(
        inInfo->Get(vtkDataObject::DATA_OBJECT()));
    vtkImageData *outData = vtkImageData::SafeDownCast(
        outInfo->Get(vtkDataObject::DATA_OBJECT()));

    int extent[6];
    inData->GetExtent(extent);
    outData->SetExtent(extent);
    outData->GetPointData()->PassData(inData->GetPointData()); 

    VideoBufferItem currentVideoBufferItem; 
    if ( this->GetVideoSource()->GetBuffer()->GetLatestVideoBufferItem( &currentVideoBufferItem ) != ITEM_OK )
    {
        LOG_WARNING("Failed to get latest video buffer item!"); 
        return 0; 
    }

    // TODO: Do we need to copy the latest image data? 
    //this->CurrentVideoBufferItem->GetFrame()->CopyData(

    const double globalTime = currentVideoBufferItem.GetTimestamp( this->GetVideoSource()->GetBuffer()->GetLocalTimeOffset() ); 

    if( this->GetTracker() != NULL && this->GetTracker()->IsTracking() )
    {
        for ( int i = 0; i < this->GetTracker()->GetNumberOfTools(); i++)
        {
            this->GetTracker()->GetTool(i)->GetBuffer()->Lock(); 
            {
                vtkSmartPointer<vtkMatrix4x4> toolTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 

                int frameBufferIndex = this->GetTracker()->GetTool(i)->GetBuffer()->GetIndexFromTime(globalTime); 
                this->GetTracker()->GetTool(i)->GetBuffer()->GetMatrix(toolTransMatrix, frameBufferIndex); 
                int flags = this->GetTracker()->GetTool(i)->GetBuffer()->GetFlags(frameBufferIndex);
                this->ToolTransMatrices[i]->DeepCopy(toolTransMatrix); 
                this->ToolTransMatrices[i]->Modified(); 
                this->ToolFlags[i] = flags; 
            }
            this->GetTracker()->GetTool(i)->GetBuffer()->Unlock(); 
        }
    }

    return 1;
} 

//------------------------------------------------------------------------------
PlusStatus vtkDataCollector::ReadConfigurationFromFile( const char* configFileName)
{	
    LOG_TRACE("vtkDataCollector::ReadConfiguration");
    this->SetConfigFileName(configFileName); 
    return this->ReadConfigurationFromFile(); 
}

//------------------------------------------------------------------------------
PlusStatus vtkDataCollector::ReadConfigurationFromFile()
{
    LOG_TRACE("vtkDataCollector::ReadConfiguration");

    const char* configFn=this->GetConfigFileName();
    if (configFn==NULL)
    {
        LOG_ERROR("Unable to read configuration file: no filename is specified"); 
        return PLUS_FAIL;
    }

    if ( this->ConfigurationData != NULL ) 
    {
        this->ConfigurationData->Delete(); 
        this->ConfigurationData = NULL; 
    }

    this->ConfigurationData = vtkXMLUtilities::ReadElementFromFile(configFn); 
    if ( this->ConfigurationData == NULL) 
    {	
        LOG_ERROR("Unable to read configuration from file " << configFn); 
        return PLUS_FAIL;
    } 

    double version(0); 
    if ( this->ConfigurationData->GetScalarAttribute("version", version) )
    {
        if ( version < this->ConfigFileVersion )
        {
            LOG_ERROR("This version of configuration file is no longer supported! Please update to version " << std::fixed << this->ConfigFileVersion ); 
            return PLUS_FAIL;
        }
    }

    vtkXMLDataElement* imageAcqusitionConfig = this->ConfigurationData->FindNestedElementWithName("ImageAcqusition"); 
    if ( imageAcqusitionConfig != NULL) 
    {
        this->ReadImageAcqusitionProperties(imageAcqusitionConfig); 
    }

    vtkXMLDataElement* trackerConfig = this->ConfigurationData->FindNestedElementWithName("Tracker"); 
    if ( trackerConfig != NULL) 
    {
        this->ReadTrackerProperties(trackerConfig); 
    }

    vtkXMLDataElement* synchronizationConfig = this->ConfigurationData->FindNestedElementWithName("Synchronization"); 
    if ( synchronizationConfig != NULL) 
    {
        this->ReadSynchronizationProperties(synchronizationConfig); 
    }
    return PLUS_SUCCESS;
}


//------------------------------------------------------------------------------
PlusStatus vtkDataCollector::ReadTrackerProperties(vtkXMLDataElement* trackerConfig)
{
    if (trackerConfig==NULL)
    {
        LOG_ERROR("TrackerConfig element is invalid");
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
    }
    //******************* AMS Tracker ***************************
    else if ( STRCASECMP("AMSTracker", type)==0) 
    {
#ifdef PLUS_USE_AMS_TRACKER
        LOG_DEBUG("Tracker type: AMS tracker"); 
        this->SetTrackerType(TRACKER_AMS); 
        vtkSmartPointer<vtkAMSTracker> tracker = vtkSmartPointer<vtkAMSTracker>::New();
        this->SetTracker(tracker); 
        tracker->ReadConfiguration(trackerConfig); 
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
        tracker->ReadConfiguration(trackerConfig); 

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
        tracker->ReadConfiguration(trackerConfig); 
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
        tracker->ReadConfiguration(trackerConfig); 
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
        tracker->ReadConfiguration(trackerConfig); 
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
        tracker->ReadConfiguration(trackerConfig); 
#endif
    }
    //******************* Saved dataset ***************************
    else if ( STRCASECMP("SavedDataset", type)==0) 
    {
        LOG_DEBUG("Tracker type: Saved Dataset");
        this->SetTrackerType(TRACKER_SAVEDDATASET); 
        vtkSmartPointer<vtkSavedDataTracker> tracker = vtkSmartPointer<vtkSavedDataTracker>::New();
        this->SetTracker(tracker); 
        tracker->ReadConfiguration(trackerConfig); 
    }
    //******************* Ascension 3DG ***************************
    else if ( STRCASECMP( "Ascension3DG", type ) == 0 )
    {
#ifdef PLUS_USE_Ascension3DG
        LOG_DEBUG( "Tracker type: Ascension 3DG" );
        this->SetTrackerType( TRACKER_ASCENSION3DG );
        vtkSmartPointer< vtkAscension3DGTracker > tracker = vtkSmartPointer< vtkAscension3DGTracker >::New();
        this->SetTracker( tracker );
        tracker->ReadConfiguration(trackerConfig); 
#endif
    }
    //******************* Fake Tracker ***************************
    else if ( STRCASECMP("FakeTracker", type)==0) 
    {
        LOG_DEBUG("Tracker type: Fake Tracker");
        this->SetTrackerType(TRACKER_FAKE); 
        vtkSmartPointer<vtkFakeTracker> tracker = vtkSmartPointer<vtkFakeTracker>::New();
        this->SetTracker(tracker);
        tracker->ReadConfiguration(trackerConfig);
    }
    else
    {
        this->SetTrackerType(TRACKER_NONE); 
        LOG_DEBUG("Tracker type: None");
        this->SetTracker(NULL); 
    }
    return PLUS_SUCCESS;
}


//------------------------------------------------------------------------------
PlusStatus vtkDataCollector::ReadImageAcqusitionProperties(vtkXMLDataElement* imageAcqusitionConfig)
{
    LOG_TRACE("vtkDataCollector::ReadImageAcqusitionProperties");

    if (imageAcqusitionConfig==NULL)
    {
        LOG_ERROR("ImageAcqusitionConfig element is invalid");
        return PLUS_FAIL;
    }

    const char* type = imageAcqusitionConfig->GetAttribute("Type"); 

    if ( type == NULL ) 
    {
        LOG_WARNING("Unable to find image acquisition type, set to default: None"); 
        this->SetAcquisitionType(SYNCHRO_VIDEO_NONE); 
        LOG_DEBUG("Image acquisition type: None");
        this->SetVideoSource(NULL); 
    }
    //******************* Sonix Video ***************************
    else if ( STRCASECMP("SonixVideo", type)==0) 
    {
#ifdef PLUS_USE_SONIX_VIDEO
        LOG_DEBUG("Image acquisition type: Sonix Video"); 
        this->SetAcquisitionType(SYNCHRO_VIDEO_SONIX); 
        vtkSmartPointer<vtkSonixVideoSource> videoSource = vtkSmartPointer<vtkSonixVideoSource>::New();
        this->SetVideoSource(videoSource); 
        videoSource->ReadConfiguration(imageAcqusitionConfig); 
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
        videoSource->ReadConfiguration(imageAcqusitionConfig); 
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
        videoSource->ReadConfiguration(imageAcqusitionConfig); 
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
        videoSource->ReadConfiguration(imageAcqusitionConfig); 
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
        videoSource->ReadConfiguration(imageAcqusitionConfig); 
#endif
    }
    //******************* Noise Video ***************************
    else if ( STRCASECMP("NoiseVideo", type)==0) 
    {
        LOG_DEBUG("Image acquisition type: Noise Video"); 
        this->SetAcquisitionType(SYNCHRO_VIDEO_NOISE); 
        vtkSmartPointer<vtkVideoSource2> videoSource = vtkSmartPointer<vtkVideoSource2>::New();
        this->SetVideoSource(videoSource); 
        videoSource->ReadConfiguration(imageAcqusitionConfig); 
    }
    //******************* Saved dataset ***************************
    else if ( STRCASECMP("SavedDataset", type)==0 ) 
    {
        LOG_DEBUG("Image acquisition type: Saved Dataset");
        this->SetAcquisitionType(SYNCHRO_VIDEO_SAVEDDATASET); 
        vtkSmartPointer<vtkSavedDataVideoSource> videoSource = vtkSmartPointer<vtkSavedDataVideoSource>::New();
        this->SetVideoSource(videoSource); 
        videoSource->ReadConfiguration(imageAcqusitionConfig); 
    }
    else
    {
        this->SetAcquisitionType(SYNCHRO_VIDEO_NONE); 
        LOG_DEBUG("Image acquisition type: None");
        this->SetVideoSource(NULL); 
    }
    return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
PlusStatus vtkDataCollector::ReadSynchronizationProperties(vtkXMLDataElement* synchronizationConfig)
{
    LOG_TRACE("vtkDataCollector::ReadSynchronizationProperties");

    if (synchronizationConfig==NULL)
    {
        LOG_ERROR("SynchronizationConfig element is invalid");
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
        synchronizer->ReadConfiguration(synchronizationConfig); 
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
int vtkDataCollector::GetDefaultToolPortNumber()
{
    int defaultToolPortNumber(0); 
    if ( this->GetTracker() != NULL )
    {
        defaultToolPortNumber = this->GetTracker()->GetDefaultTool(); 
    }

    return defaultToolPortNumber; 
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

