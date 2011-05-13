#include "PlusConfigure.h"
#include "vtkDataCollector.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkXMLUtilities.h"
#include "vtkBMPWriter.h"

#include "vtkVideoFrame2.h"
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
#include "vtkSonixVideoSource2.h"
#endif
#ifdef PLUS_USE_ICCAPTURING_VIDEO
#include "vtkICCapturingSource2.h"
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

//----------------------------------------------------------------------------
// Constant defines 
// Signal boxes
#define SYNCHRO_SIGNAL_NOGATING 1
#define SYNCHRO_SIGNAL_FAKE 2
#define SYNCHRO_SIGNAL_HEART 3
#define SYNCHRO_SIGNAL_USB 4

// Video modes
#define SYNCHRO_VIDEO_NTSC 1
#define SYNCHRO_VIDEO_PAL 2

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
	this->ToolTransMatrices.reserve(0); 
	this->ToolFlags.reserve(0); 

	this->MainToolNumber = -1; 
	this->DumpBufferSize = 500; 

	this->InitializedOff(); 
	this->SetTrackingOnly(false);
	this->SetVideoOnly(false);
	this->CancelSyncRequestOff(); 

	// Most recent frame buffer index 
	this->MostRecentFrameBufferIndex = 0; 
}


//----------------------------------------------------------------------------
vtkDataCollector::~vtkDataCollector()
{
	this->SetTracker(NULL); 
	this->SetVideoSource(NULL);
	this->SetSynchronizer(NULL); 

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
}

//----------------------------------------------------------------------------
void vtkDataCollector::Initialize()
{
	LOG_TRACE("vtkDataCollector::Initialize"); 
	this->InitializedOff(); 

	// Connect to devices
	this->Connect(); 

	this->InitializedOn(); 
}

//----------------------------------------------------------------------------
void vtkDataCollector::Connect()
{
	LOG_TRACE("vtkDataCollector::Connect"); 

	if ( this->GetVideoSource() == NULL ) 
	{
		LOG_WARNING("Unable to find video source! Please read configuration file first." ); 
	}
	else
	{
		this->GetVideoSource()->Initialize();

		if (!this->GetVideoSource()->GetInitialized())
		{
			LOG_ERROR("Unable to initialize video source!"); 
			exit(EXIT_FAILURE);  
		}

		this->SetInputConnection(this->GetVideoSource()->GetOutputPort());
	}

	if ( this->GetTracker() == NULL ) 
	{
		LOG_WARNING("Unable to find tracker! Please read configuration file first." ); 
	}
	else
	{
		if ( !this->GetTracker()->Probe() )
		{
			LOG_ERROR("Unable to initialize tracker!"); 
			exit(EXIT_FAILURE);  
		}

		// If we hadn't set the main tool number, set to the first active
		if ( this->GetMainToolNumber() == -1)
		{
			this->SetMainToolNumber( this->GetNextActiveToolNumber() ); 
		}
	}
}

//----------------------------------------------------------------------------
void vtkDataCollector::Disconnect()
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
}

//----------------------------------------------------------------------------
void vtkDataCollector::Start()
{
	LOG_TRACE("vtkDataCollector::Start"); 

	if ( ! this->GetTrackingOnly() )
	{
		if ( this->GetVideoSource() != NULL )
		{
			this->GetVideoSource()->Record();
		}
	}
	else
	{
		LOG_DEBUG("Start data collection in tracking only mode."); 
	}

	if ( ! this->GetVideoOnly() )
	{
		if ( this->GetTracker() != NULL )
		{
			this->GetTracker()->StartTracking(); 

			// reserve enough transformation capacity for each tool
			this->ToolTransMatrices.clear(); 
			this->ToolFlags.clear(); 
			for ( int i = 0; i < this->GetTracker()->GetNumberOfTools(); i++ )
			{
				vtkMatrix4x4* matrix = vtkMatrix4x4::New(); 
				this->ToolTransMatrices.push_back(matrix); 
				this->ToolFlags.push_back(0); 
			}
		}
	}
	else
	{
		LOG_DEBUG("Start data collection in video only mode."); 
	}
}

//----------------------------------------------------------------------------
void vtkDataCollector::Stop()
{
	LOG_TRACE("vtkDataCollector::Stop"); 
	if ( this->GetVideoSource() != NULL )
	{
		this->GetVideoSource()->Stop();
	}

	if ( this->GetTracker() != NULL )
	{
		this->GetTracker()->StopTracking(); 
	}
}

//----------------------------------------------------------------------------
void vtkDataCollector::CopyTrackerBuffer( vtkTrackerBuffer* trackerBuffer, int toolNumber )
{
	LOG_TRACE("vtkDataCollector::CopyTrackerBuffer"); 

	if ( this->GetTracker() == NULL ) 
	{	
		LOG_ERROR("Unable to copy tracker buffer - there is no tracker selected!"); 
		return; 
	}

	if ( trackerBuffer == NULL )
	{
		trackerBuffer = vtkTrackerBuffer::New();
	}

	this->GetTracker()->GetTool(toolNumber)->GetBuffer()->Lock(); 
	trackerBuffer->Lock(); 

	trackerBuffer->DeepCopy(this->GetTracker()->GetTool(toolNumber)->GetBuffer()); 

	trackerBuffer->Unlock();
	this->GetTracker()->GetTool(toolNumber)->GetBuffer()->Unlock(); 

}

//----------------------------------------------------------------------------
void vtkDataCollector::CopyTracker( vtkTracker* tracker)
{
	LOG_TRACE("vtkDataCollector::CopyTracker"); 
	if ( this->GetTracker() == NULL ) 
	{	
		LOG_ERROR("Unable to copy tracker buffer - there is no tracker selected!"); 
		return; 
	}

	if ( tracker == NULL )
	{
		tracker = vtkTracker::New();
	}

	tracker->DeepCopy(this->GetTracker()); 
}

//----------------------------------------------------------------------------
void vtkDataCollector::DumpTrackerToMetafile( vtkTracker* tracker, const char* outputFolder, const char* metaFileName, bool useCompression /*= false*/ )
{
	LOG_TRACE("vtkDataCollector::DumpTrackerToMetafile: " << outputFolder << "/" << metaFileName); 

	if ( tracker == NULL ) 
	{
		LOG_WARNING("Unable to copy tracker to sequence metafile: tracker is NULL"); 
		return; 
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
	
	for ( int i = 0 ; i < numberOfItems; i++ ) 
	{
		const int percent = floor( (100.0*i) / (1.0*numberOfItems)); 
		if ( this->ProgressBarUpdateCallbackFunction != NULL )
		{
			(*ProgressBarUpdateCallbackFunction)(percent); 
		}

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
			LOG_ERROR("Unable to allocate memory for image: " << err);
			continue; 
		}	

		TrackedFrame trackedFrame; 
		trackedFrame.ImageData = frame;
		trackedFrame.ImageData->Register(); 

		const double frameTimestamp = tracker->GetTool(this->GetMainToolNumber())->GetBuffer()->GetFilteredTimeStamp(( numberOfItems - 1 ) - i); 

		// Add main tool timestamp
		std::ostringstream timestampFieldValue; 
		timestampFieldValue << std::fixed << frameTimestamp; 
		trackedFrame.SetCustomFrameField("Timestamp", timestampFieldValue.str()); 

		// Add main tool unfiltered timestamp
		std::ostringstream unfilteredtimestampFieldValue; 
		unfilteredtimestampFieldValue << std::fixed << tracker->GetTool(this->GetMainToolNumber())->GetBuffer()->GetUnfilteredTimeStamp(( numberOfItems - 1 ) - i); 
		trackedFrame.SetCustomFrameField("UnfilteredTimestamp", unfilteredtimestampFieldValue.str()); 

		// Add main tool frameNumber
		std::ostringstream frameNumberFieldValue; 
		frameNumberFieldValue << std::fixed << tracker->GetTool(this->GetMainToolNumber())->GetBuffer()->GetFrameNumber(( numberOfItems - 1 ) - i); 
		trackedFrame.SetCustomFrameField("FrameNumber", frameNumberFieldValue.str()); 

		// Add main tool status
		long flag = tracker->GetTool(this->GetMainToolNumber())->GetBuffer()->GetFlags(( numberOfItems - 1 ) - i); 
		std::string status = tracker->ConvertFlagToString(flag); 
		trackedFrame.SetCustomFrameField("Status", status); 

		// Set default transform name
		trackedFrame.DefaultFrameTransformName = tracker->GetTool(this->GetMainToolNumber())->GetToolName(); 

		// Add transforms
		for ( int tool = 0; tool < tracker->GetNumberOfTools(); tool++ )
		{
			if ( tracker->GetTool(tool)->GetEnabled() )
			{
				vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
				const int index = tracker->GetTool(tool)->GetBuffer()->GetIndexFromTime(frameTimestamp); 
				tracker->GetTool(tool)->GetBuffer()->GetMatrix(matrix, index); 
				trackedFrame.SetCustomFrameTransform(tracker->GetTool(tool)->GetToolName(), matrix); 
			}
		}

		// Add tracked frame to the list
		trackedFrameList->AddTrackedFrame(&trackedFrame); 
	}

	// Save tracked frames to metafile
	trackedFrameList->SaveToSequenceMetafile(outputFolder, metaFileName, vtkTrackedFrameList::SEQ_METAFILE_MHA, useCompression); 

	tracker->Unlock();
}

//----------------------------------------------------------------------------
void vtkDataCollector::CopyVideoBuffer( vtkVideoBuffer2* videoBuffer )
{
	if ( this->GetVideoSource() == NULL ) 
	{	
		vtkErrorMacro(<< "Unable to copy video buffer - there is no video source selected!"); 
		return; 
	}

	if ( videoBuffer == NULL ) 
	{
		videoBuffer = vtkVideoBuffer2::New();
	}


	this->GetVideoSource()->GetBuffer()->Lock();
	videoBuffer->Lock(); 

	videoBuffer->DeepCopy(this->GetVideoSource()->GetBuffer()); 

	videoBuffer->Unlock(); 
	this->GetVideoSource()->GetBuffer()->Unlock();
}

//----------------------------------------------------------------------------
void vtkDataCollector::DumpVideoBufferToMetafile( vtkVideoBuffer2* videoBuffer, const char* outputFolder, const char* metaFileName, bool useCompression /*=false*/ )
{
	if ( videoBuffer == NULL )
	{
		return; 
	}

	videoBuffer->Lock(); 

	const int numberOfFrames = videoBuffer->GetNumberOfItems(); 
	vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 

	for ( int i = 0 ; i < numberOfFrames; i++ ) 
	{
		const int percent = floor( (100.0*i) / (1.0*numberOfFrames)); 
		if ( this->ProgressBarUpdateCallbackFunction != NULL )
		{
			(*ProgressBarUpdateCallbackFunction)(percent); 
		}

		const int bufferItem = ( numberOfFrames - 1 ) - i; 

		// Allocate memory for new frame
		vtkSmartPointer<vtkImageData> vtkimage = vtkSmartPointer<vtkImageData>::New(); 
		vtkimage->CopyStructure( this->GetOutput() ); 
		vtkimage->SetScalarTypeToUnsignedChar(); 
		vtkimage->AllocateScalars(); 

		videoBuffer->GetFrame(bufferItem)->CopyData(vtkimage->GetScalarPointer(), vtkimage->GetExtent(), vtkimage->GetExtent(), videoBuffer->GetFrame(bufferItem)->GetPixelFormat() ); 

		// Convert vtkImage to itkimage
		TrackedFrame::ImageType::Pointer itkimage = TrackedFrame::ImageType::New(); 
		this->ConvertVtkImageToItkImage(vtkimage, itkimage); 

		TrackedFrame trackedFrame; 
		trackedFrame.ImageData = itkimage;
		trackedFrame.ImageData->Register(); 

		// Set default transform name
		trackedFrame.DefaultFrameTransformName = "IdentityTransform"; 

		// Add transform 
		vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
		matrix->Identity(); 
		trackedFrame.SetCustomFrameTransform(trackedFrame.DefaultFrameTransformName, matrix); 

		// Add filtered timestamp
		std::ostringstream timestampFieldValue; 
		timestampFieldValue << std::fixed << videoBuffer->GetFilteredTimeStamp(bufferItem); 
		trackedFrame.SetCustomFrameField("Timestamp", timestampFieldValue.str()); 

		// Add unfiltered timestamp
		std::ostringstream unfilteredtimestampFieldValue; 
		unfilteredtimestampFieldValue << std::fixed << videoBuffer->GetUnfilteredTimeStamp(bufferItem); 
		trackedFrame.SetCustomFrameField("UnfilteredTimestamp", unfilteredtimestampFieldValue.str()); 

		// Add frame number
		std::ostringstream frameNumberFieldValue; 
		frameNumberFieldValue << std::fixed << videoBuffer->GetFrameNumber(bufferItem); 
		trackedFrame.SetCustomFrameField("FrameNumber", frameNumberFieldValue.str()); 

		// Add tracked frame to the list
		trackedFrameList->AddTrackedFrame(&trackedFrame); 
	}

	// Save tracked frames to metafile
	trackedFrameList->SaveToSequenceMetafile(outputFolder, metaFileName, vtkTrackedFrameList::SEQ_METAFILE_MHA, useCompression); 


	videoBuffer->Unlock();
}

//----------------------------------------------------------------------------
void vtkDataCollector::Synchronize( const bool saveSyncData /*=false*/ )
{
	LOG_TRACE("vtkDataCollector::Synchronize"); 
	if ( this->GetTracker() == NULL ) 
	{	
		LOG_ERROR("Unable to synchronize tracker - there is no tracker selected!"); 
		return; 
	}

	if ( this->GetVideoSource() == NULL ) 
	{	
		LOG_ERROR("Unable to synchronize video - there is no video source selected!"); 
		return; 
	}

	this->CancelSyncRequestOff(); 

	//************************************************************************************
	// Save local time offsets before sync
	const double prevVideoOffset = this->GetVideoSource()->GetBuffer()->GetLocalTimeOffset(); 
	const double prevTrackerOffset = this->GetTracker()->GetTool(this->GetMainToolNumber())->GetBuffer()->GetLocalTimeOffset(); 

	//************************************************************************************
	// Set the local timeoffset to 0 before synchronization 
	this->SetLocalTimeOffset(0, 0); 
	
	//************************************************************************************
	// Set the length of the acquisition 
	const double syncTimeLength = this->GetSynchronizer()->GetSynchronizationTimeLength(); 

	// Get the realtime tracking frequency
	this->GetTracker()->GetTool(this->GetMainToolNumber())->GetBuffer()->Lock(); 
	double trackerFrameRate = this->GetTracker()->GetTool(this->GetMainToolNumber())->GetBuffer()->GetFrameRate(); 
	this->GetTracker()->GetTool(this->GetMainToolNumber())->GetBuffer()->Unlock(); 

	// Get the realtime video frame rate
	this->GetVideoSource()->GetBuffer()->Lock();
	double videoFrameRate = this->GetVideoSource()->GetBuffer()->GetFrameRate(); 
	this->GetVideoSource()->GetBuffer()->Unlock();

	const int trackerBufferSize = this->GetTracker()->GetTool(this->GetMainToolNumber())->GetBuffer()->GetBufferSize(); 
	const int videoBufferSize = this->GetVideoSource()->GetBuffer()->GetBufferSize(); 
	int syncTrackerBufferSize = trackerFrameRate * syncTimeLength + 100; 
	int syncVideoBufferSize = videoFrameRate * syncTimeLength + 100; 

	//************************************************************************************
	// Change buffer size to fit the whole acquisition 
	if ( syncVideoBufferSize > videoBufferSize )
	{
		LOG_DEBUG("Change video buffer size to: " << syncVideoBufferSize); 
		this->GetVideoSource()->SetFrameBufferSize(syncVideoBufferSize);	
		this->GetVideoSource()->GetBuffer()->Lock(); 
		this->GetVideoSource()->GetBuffer()->Clear(); 
		this->GetVideoSource()->GetBuffer()->Unlock();
	}
	else
	{
		this->GetVideoSource()->GetBuffer()->Lock(); 
		this->GetVideoSource()->GetBuffer()->Clear(); 
		this->GetVideoSource()->GetBuffer()->Unlock(); 
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
			return; 
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
	vtkSmartPointer<vtkVideoBuffer2> videobuffer = vtkSmartPointer<vtkVideoBuffer2>::New(); 
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

	if ( saveSyncData )
	{
		LOG_INFO(">>>>>>>>>>>> Save temporal calibration buffers to file ... "); 
		this->DumpTrackerToMetafile(tracker, "./", "DataCollectorSyncTrackerBuffer", false );
		this->DumpVideoBufferToMetafile(videobuffer, "./", "DataCollectorSyncVideoBuffer", false ); 
	}
	

	//************************************************************************************
	// Start synchronization 
	this->GetSynchronizer()->SetProgressBarUpdateCallbackFunction(ProgressBarUpdateCallbackFunction); 
	this->GetSynchronizer()->SetSyncStartTime(syncStartTime); 

	vtkTrackerBuffer* trackerbuffer = tracker->GetTool(this->GetMainToolNumber())->GetBuffer(); 

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
	this->GetVideoSource()->SetFrameBufferSize(videoBufferSize);
	this->GetVideoSource()->GetBuffer()->Lock(); 
	this->GetVideoSource()->GetBuffer()->Clear(); 
	this->GetVideoSource()->GetBuffer()->Unlock();
	
	LOG_DEBUG("Change tracker buffer size to: " << trackerBufferSize); 
	for ( int i = 0; i < this->GetTracker()->GetNumberOfTools(); i++ )
	{
		this->GetTracker()->GetTool(i)->GetBuffer()->Lock(); 
		this->GetTracker()->GetTool(i)->GetBuffer()->SetBufferSize(trackerBufferSize); 
		this->GetTracker()->GetTool(i)->GetBuffer()->Clear(); 
		this->GetTracker()->GetTool(i)->GetBuffer()->Unlock(); 
	}

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
double vtkDataCollector::GetMostRecentTimestamp()
{
	LOG_TRACE("vtkDataCollector::GetMostRecentTimestamp"); 
	this->GetVideoSource()->GetBuffer()->Lock(); 
	// Get the most recent frame from the buffer
	double frameTimestamp = this->GetVideoSource()->GetBuffer()->GetTimeStamp(this->MostRecentFrameBufferIndex); 
	this->GetVideoSource()->GetBuffer()->Unlock();

	return frameTimestamp; 
}

//----------------------------------------------------------------------------
std::string vtkDataCollector::GetMainToolName()
{
	LOG_TRACE("vtkDataCollector::GetMainToolName"); 
	std::string mainToolName( this->GetTracker()->GetTool(this->GetMainToolNumber())->GetToolName() ); 

	return mainToolName; 
}

//----------------------------------------------------------------------------
long vtkDataCollector::GetMainToolStatus( double time )
{
	LOG_TRACE("vtkDataCollector::GetMainToolStatus"); 
	vtkTrackerBuffer* buffer = this->GetTracker()->GetTool(this->GetMainToolNumber())->GetBuffer(); 

	buffer->Lock(); 
	int index = buffer->GetIndexFromTime(time); 
	long status = buffer->GetFlags(index); 
	buffer->Unlock(); 

	return status; 
}

//----------------------------------------------------------------------------
void vtkDataCollector::GetFrameByTime(const double time, vtkImageData* frame, double& frameTimestamp)
{
	//LOG_TRACE("vtkDataCollector::GetFrameByTime"); 
	if ( this->GetVideoSource() == NULL ) 
	{	
		LOG_ERROR("Unable to get frame - there is no video source selected!"); 
		return; 
	}

	this->GetVideoSource()->GetBuffer()->Lock(); 
	const int frameIndex = this->GetVideoSource()->GetBuffer()->GetIndexFromTime(time); 
	int videoFormat = this->GetVideoSource()->GetOutputFormat(); 
	this->GetVideoSource()->GetBuffer()->GetFrame(frameIndex)->CopyData(frame->GetScalarPointer(), frame->GetExtent(), frame->GetExtent(), videoFormat); 
	frameTimestamp = this->GetVideoSource()->GetBuffer()->GetTimeStamp(frameIndex); 

	this->GetVideoSource()->GetBuffer()->Unlock();
}

//----------------------------------------------------------------------------
double vtkDataCollector::GetFrameTimestampByTime(double time)
{
	//LOG_TRACE("vtkDataCollector::GetFrameTimestampByTime"); 
	if ( this->GetVideoSource() == NULL ) 
	{	
		LOG_ERROR("Unable to get frame - there is no video source selected!"); 
		return 0; 
	}

	this->GetVideoSource()->GetBuffer()->Lock(); 
	const int frameIndex = this->GetVideoSource()->GetBuffer()->GetIndexFromTime(time); 
	double frameTimestamp = this->GetVideoSource()->GetBuffer()->GetTimeStamp(frameIndex); 
	this->GetVideoSource()->GetBuffer()->Unlock();

	return frameTimestamp; 
}

//----------------------------------------------------------------------------
void vtkDataCollector::GetFrameWithTimestamp(vtkImageData* frame, double& frameTimestamp)
{
	//LOG_TRACE("vtkDataCollector::GetFrameWithTimestamp"); 
	if ( this->GetVideoSource() == NULL ) 
	{	
		LOG_ERROR("Unable to get frame - there is no video source selected!"); 
		return; 
	}

	this->GetVideoSource()->GetBuffer()->Lock(); 
	// Get the most recent frame from the buffer (if the tracker slower we have a chance to get the right transform)
	int videoFormat = this->GetVideoSource()->GetOutputFormat(); 
	this->GetVideoSource()->GetBuffer()->GetFrame(this->MostRecentFrameBufferIndex)->CopyData(frame->GetScalarPointer(), frame->GetExtent(), frame->GetExtent(), videoFormat); 
	frameTimestamp = this->GetVideoSource()->GetBuffer()->GetTimeStamp(this->MostRecentFrameBufferIndex); 

	this->GetVideoSource()->GetBuffer()->Unlock();
}

//----------------------------------------------------------------------------
void vtkDataCollector::GetTransformWithTimestamp(vtkMatrix4x4* toolTransMatrix, double& transformTimestamp, long& flags, int toolNumber/* = 0*/, bool calibratedTransform /*= false*/ )
{
	//LOG_TRACE("vtkDataCollector::GetTransformWithTimestamp"); 
	if ( this->GetTracker() == NULL ) 
	{	
		LOG_ERROR("Unable to get transform - there is no tracker selected!"); 
		return; 
	}

	this->GetTracker()->GetTool(toolNumber)->GetBuffer()->Lock(); 

	
	flags = this->GetTracker()->GetTool(toolNumber)->GetBuffer()->GetFlags(this->MostRecentFrameBufferIndex); 
	
	// Get the most recent transform from the buffer (if the video slower we have a chance to get the right transform)
	if (calibratedTransform)
	{
		this->GetTracker()->GetTool(toolNumber)->GetBuffer()->GetCalibratedMatrix(toolTransMatrix, this->MostRecentFrameBufferIndex); 
	}
	else
	{
		this->GetTracker()->GetTool(toolNumber)->GetBuffer()->GetMatrix(toolTransMatrix, this->MostRecentFrameBufferIndex); 
	}
	
	transformTimestamp = this->GetTracker()->GetTool(toolNumber)->GetBuffer()->GetTimeStamp(this->MostRecentFrameBufferIndex); 

	this->GetTracker()->GetTool(toolNumber)->GetBuffer()->Unlock(); 
}

//----------------------------------------------------------------------------
void vtkDataCollector::GetTransformByTimestamp(vtkMatrix4x4* toolTransMatrix, long& flags, const double synchronizedTime, int toolNumber/* = 0*/, bool calibratedTransform /*= false*/)
{
	//LOG_TRACE("vtkDataCollector::GetTransformByTimestamp"); 
	if ( this->GetTracker() == NULL ) 
	{	
		LOG_ERROR("Unable to get transform - there is no tracker selected!"); 
		return; 
	}

	this->GetTracker()->GetTool(toolNumber)->GetBuffer()->Lock(); 
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
	this->GetTracker()->GetTool(toolNumber)->GetBuffer()->Unlock(); 
}

//----------------------------------------------------------------------------
double vtkDataCollector::GetTransformsByTimeInterval(std::vector<vtkMatrix4x4*> &toolTransMatrixVector, std::vector<long> &flagsVector, const double startTime, const double endTime, int toolNumber/* = 0*/, bool calibratedTransform /*= false*/)
{
	//LOG_TRACE("vtkDataCollector::GetTransformsByTimeInterval"); 
	if ( this->GetTracker() == NULL ) 
	{	
		vtkErrorMacro(<< "Unable to get transforms - there is no tracker selected!"); 
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
	if (startTime != -1.0) { // If specific startTime was given then get start index for the item after the one with the specified timestamp
		frameBufferStartIndex = buffer->GetIndexFromTime(startTime) + 1; // The transform at start time is skipped
	} else {
		if (buffer->GetBufferSize() > buffer->GetNumberOfItems()) {
			frameBufferStartIndex = 1 - buffer->GetNumberOfItems();
		}
	}

	double returnTime;
	int frameBufferEndIndex = 0; // End index is initialized as the most recent data (it remains so if endTime is -1)
	if (endTime != -1.0) { // If specific endTime was given then get corresponding end index
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
void vtkDataCollector::GetTrackedFrame(vtkImageData* frame, vtkMatrix4x4* toolTransMatrix, long& flags, double& synchronizedTime, int toolNumber/* = 0*/, bool calibratedTransform /*= false*/)
{
	//LOG_TRACE("vtkDataCollector::GetTrackedFrame - vtkImageData"); 
	this->GetFrameWithTimestamp(frame, synchronizedTime); 
	this->GetTransformByTimestamp( toolTransMatrix, flags, synchronizedTime, toolNumber, calibratedTransform); 
}

//----------------------------------------------------------------------------
void vtkDataCollector::ConvertVtkImageToItkImage(vtkImageData* inFrame, TrackedFrame::ImageType* outFrame)
{
	//LOG_TRACE("vtkDataCollector::ConvertVtkImageToItkImage"); 
	// convert vtkImageData to itkImage 
	vtkSmartPointer<vtkImageFlip> imageFlipy = vtkSmartPointer<vtkImageFlip>::New(); 
	imageFlipy->SetInput(inFrame); 
	imageFlipy->SetFilteredAxis(1); 
	imageFlipy->Update(); 

	vtkSmartPointer<vtkImageExport> imageExport = vtkSmartPointer<vtkImageExport>::New(); 
	imageExport->ImageLowerLeftOff();
	imageExport->SetInput(imageFlipy->GetOutput()); 
	imageExport->Update(); 

	double width = inFrame->GetExtent()[1] - inFrame->GetExtent()[0] + 1; 
	double height = inFrame->GetExtent()[3] - inFrame->GetExtent()[2] + 1; 
	TrackedFrame::ImageType::SizeType size = { width, height };
	TrackedFrame::ImageType::IndexType start = {0,0};
	TrackedFrame::ImageType::RegionType region;
	region.SetSize(size);
	region.SetIndex(start);
	outFrame->SetRegions(region);
	outFrame->Allocate();

	memcpy( outFrame->GetBufferPointer(), imageExport->GetPointerToData(), imageExport->GetDataMemorySize() ); 
}

//----------------------------------------------------------------------------
void vtkDataCollector::GetTrackedFrame(TrackedFrame* trackedFrame, bool calibratedTransform /*= false*/)
{
	//LOG_TRACE("vtkDataCollector::GetTrackedFrame - TrackedFrame"); 
	const double time = this->GetMostRecentTimestamp(); 
	this->GetTrackedFrameByTime(time, trackedFrame, calibratedTransform); 
}

//----------------------------------------------------------------------------
void vtkDataCollector::GetTrackedFrameByTime(const double time, TrackedFrame* trackedFrame, bool calibratedTransform /*= false*/)
{
	//LOG_TRACE("vtkDataCollector::GetTrackedFrameByTime - TrackedFrame");
	// Allocate memory for new frame
	vtkSmartPointer<vtkImageData> vtkimage = vtkSmartPointer<vtkImageData>::New(); 
	vtkimage->CopyStructure( this->GetOutput() ); 
	vtkimage->SetScalarTypeToUnsignedChar(); 
	vtkimage->AllocateScalars(); 

	// Get the frame by time
	double synchronizedTime(0); 
	this->GetFrameByTime(time, vtkimage, synchronizedTime); 

	// Convert vtkImage to itkimage
	TrackedFrame::ImageType::Pointer itkimage = TrackedFrame::ImageType::New(); 
	this->ConvertVtkImageToItkImage(vtkimage, itkimage); 

	// Get tracker buffer values 
	std::vector<std::string> toolNames; 
	std::vector<std::string> toolBufferValues; 
	std::vector<std::string> toolBufferStatuses; 

	this->GetTracker()->GetTrackerToolBufferStringList(synchronizedTime, toolNames, toolBufferValues, toolBufferStatuses, calibratedTransform); 

	//Add all information to the tracked frame
	trackedFrame->Timestamp = synchronizedTime; 
	trackedFrame->ImageData = itkimage; 
	trackedFrame->ImageData->Register(); 
	trackedFrame->Status = this->GetMainToolStatus(synchronizedTime); 
	trackedFrame->DefaultFrameTransformName = this->GetMainToolName(); 

	for ( int tool = 0; tool < toolNames.size(); ++tool)
	{
		// Set tool buffer values 
		trackedFrame->SetCustomFrameField(toolNames[tool], toolBufferValues[tool]); 

		// Set tool buffer statuses 
		std::ostringstream statusName; 
		statusName << toolNames[tool] << "Status"; 
		trackedFrame->SetCustomFrameField(statusName.str(), toolBufferStatuses[tool]); 
	}
}

//----------------------------------------------------------------------------
void vtkDataCollector::GetTrackedFrameByTime(const double time, vtkImageData* frame, std::vector<vtkMatrix4x4*> &toolTransforms, std::vector<std::string> &toolTransformNames, std::vector<long> &flags, double& synchronizedTime, bool calibratedTransform /*= false*/)
{
	//LOG_TRACE("vtkDataCollector::GetTrackedFrameByTime - vtkImageData");
	toolTransforms.clear(); 
	toolTransformNames.clear(); 
	flags.clear(); 

	this->GetFrameByTime(time, frame, synchronizedTime); 

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

//----------------------------------------------------------------------------
void vtkDataCollector::GetTrackedFrame(vtkImageData* frame, std::vector<vtkMatrix4x4*> &toolTransforms, std::vector<std::string> &toolTransformNames, std::vector<long> &flags, double& synchronizedTime, bool calibratedTransform /*= false*/)
{
	//LOG_TRACE("vtkDataCollector::GetTrackedFrame - vtkImageData");
	// Get the most recent frame timestamp 
	const double time = this->GetMostRecentTimestamp(); 
	this->GetTrackedFrameByTime(time, frame, toolTransforms, toolTransformNames, flags, synchronizedTime, calibratedTransform); 
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

	if( this->GetTracker() != NULL && this->GetTracker()->IsTracking() )
	{
		this->VideoSource->GetBuffer()->Lock(); 
		// Get the most recent frame from the buffer (if the video slower we have a chance to get the right transform)
		double globalTime = this->VideoSource->GetFrameTimeStamp(0);
		this->VideoSource->GetBuffer()->Unlock(); 

		for ( int i = 0; i < this->GetTracker()->GetNumberOfTools(); i++)
		{
			this->GetTracker()->GetTool(i)->GetBuffer()->Lock(); 
			vtkSmartPointer<vtkMatrix4x4> toolTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 

			int frameBufferIndex = this->GetTracker()->GetTool(i)->GetBuffer()->GetIndexFromTime(globalTime); 
			this->GetTracker()->GetTool(i)->GetBuffer()->GetMatrix(toolTransMatrix, frameBufferIndex); 
			int flags = this->GetTracker()->GetTool(i)->GetBuffer()->GetFlags(frameBufferIndex);
			this->ToolTransMatrices[i]->DeepCopy(toolTransMatrix); 
			this->ToolTransMatrices[i]->Modified(); 
			this->ToolFlags[i] = flags; 
			this->GetTracker()->GetTool(i)->GetBuffer()->Unlock(); 
		}
	}

	return 1;
} 

//------------------------------------------------------------------------------
vtkMatrix4x4* vtkDataCollector::GetToolTransMatrix( unsigned int toolNumber/* = 0 */) 
{	
	//LOG_TRACE("vtkDataCollector::GetToolTransMatrix");
	if ( toolNumber >= ToolTransMatrices.size() )
	{
		vtkErrorMacro( << "The tool number is larger than number of tools: " << this->GetTracker()->GetNumberOfTools() ); 
		return NULL;
	}
	return ToolTransMatrices[toolNumber]; 
}

//------------------------------------------------------------------------------
int  vtkDataCollector::GetToolFlags( unsigned int toolNumber/* = 0 */) 
{	
	//LOG_TRACE("vtkDataCollector::GetToolFlags");
	if ( toolNumber >= ToolFlags.size() )
	{
		vtkErrorMacro( << "The tool number is larger than number of tools: " << this->GetTracker()->GetNumberOfTools() ); 
		return NULL;
	}
	return ToolFlags[toolNumber]; 
}


//------------------------------------------------------------------------------
void vtkDataCollector::ReadConfiguration( const char* configFileName)
{	
	LOG_TRACE("vtkDataCollector::ReadConfiguration: " << configFileName);
	this->SetConfigFileName(configFileName); 
	this->ReadConfiguration(); 
}


//----------------------------------------------------------------------------
int vtkDataCollector::GetNextActiveToolNumber()
{
	LOG_TRACE("vtkDataCollector::GetNextActiveToolNumber");
	if ( this->GetTracker() == NULL ) 
	{	
		LOG_ERROR("Unable to get next active tool number - there is no tracker selected!"); 
		return -1; 
	}

	int nextToolNumber = this->GetMainToolNumber(); 
	const int numberOfTools = this->GetTracker()->GetNumberOfTools(); 
	bool toolActive(false);
	int loop(0); 

	while( !toolActive )
	{
		if ( ++nextToolNumber >= numberOfTools )
		{
			nextToolNumber = 0; 
			loop++; 
		}

		toolActive = this->GetTracker()->GetTool(nextToolNumber)->GetEnabled(); 

		// avoid endless loop
		if ( loop > 1 ) 
		{
			return this->GetMainToolNumber(); 
		}
	}

	return nextToolNumber; 
}

//----------------------------------------------------------------------------
int vtkDataCollector::GetPreviousActiveToolNumber()
{
	LOG_TRACE("vtkDataCollector::GetPreviousActiveToolNumber");
	if ( this->GetTracker() == NULL ) 
	{	
		LOG_ERROR("Unable to get previous active tool number - there is no tracker selected!"); 
		return -1; 
	}

	int prevToolNumber = this->GetMainToolNumber(); 
	const int numberOfTools = this->GetTracker()->GetNumberOfTools(); 
	bool toolActive(false);
	int loop(0); 

	while( !toolActive )
	{
		if ( --prevToolNumber < 0)
		{
			prevToolNumber = numberOfTools - 1; 
			loop++; 
		}

		toolActive = this->GetTracker()->GetTool(prevToolNumber)->GetEnabled(); 

		// avoid endless loop
		if ( loop > 1 ) 
		{
			return this->GetMainToolNumber(); 
		}
	}

	return prevToolNumber; 
}

//------------------------------------------------------------------------------
void vtkDataCollector::ReadConfiguration()
{
	LOG_TRACE("vtkDataCollector::ReadConfiguration");
	this->ConfigurationData = vtkXMLUtilities::ReadElementFromFile(this->GetConfigFileName()); 

	if ( this->ConfigurationData == NULL) 
	{	
		LOG_ERROR("Unable to read the main configration file: " << this->GetConfigFileName() ); 
		exit(EXIT_FAILURE); 
	} 

	vtkSmartPointer<vtkXMLDataElement> imageAcqusitionConfig = this->ConfigurationData->FindNestedElementWithName("ImageAcqusition"); 
	if ( imageAcqusitionConfig != NULL) 
	{
		this->ReadImageAcqusitionProperties(imageAcqusitionConfig); 
	}

	vtkSmartPointer<vtkXMLDataElement> trackerConfig = this->ConfigurationData->FindNestedElementWithName("Tracker"); 
	if ( trackerConfig != NULL) 
	{
		this->ReadTrackerProperties(trackerConfig); 
	}

	vtkSmartPointer<vtkXMLDataElement> synchronizationConfig = this->ConfigurationData->FindNestedElementWithName("Synchronization"); 
	if ( synchronizationConfig != NULL) 
	{
		this->ReadSynchronizationProperties(synchronizationConfig); 
	}
}


//------------------------------------------------------------------------------
void vtkDataCollector::ReadTrackerProperties(vtkXMLDataElement* trackerConfig)
{
	LOG_TRACE("vtkDataCollector::ReadTrackerProperties");
	const char* type = trackerConfig->GetAttribute("Type"); 
	if ( type == NULL ) 
	{
		vtkWarningMacro(<< "Unable to find tracker type, set to default None"); 

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

		vtkSmartPointer<vtkXMLDataElement> xmlconfig = trackerConfig->FindNestedElementWithName("AMSTracker"); 

		if ( xmlconfig != NULL )
		{
			vtkSmartPointer<vtkAMSTracker> tracker = vtkSmartPointer<vtkAMSTracker>::New();
			this->SetTracker(tracker); 
			tracker->ReadConfiguration(xmlconfig); 
			this->SetMainToolNumber(vtkAMSTracker::PROBEHOME_TO_PROBE_TRANSFORM); 
		}
#endif
	}
	//******************* Certus Tracker ***************************
	else if ( STRCASECMP("CertusTracker", type)==0) 
	{
		LOG_DEBUG("Tracker type: Certus tracker"); 
		this->SetTrackerType(TRACKER_CERTUS); 

		vtkSmartPointer<vtkXMLDataElement> trackerCertus = trackerConfig->FindNestedElementWithName("CertusTracker"); 

		if ( trackerCertus != NULL )
		{
#ifdef PLUS_USE_CERTUS
			vtkSmartPointer<vtkNDICertusTracker> tracker = vtkSmartPointer<vtkNDICertusTracker>::New();
			this->SetTracker(tracker); 

			int referenceToolNumber(-1);
			if ( trackerCertus->GetScalarAttribute("ReferenceToolNumber", referenceToolNumber) ) 
			{
				tracker->SetReferenceTool(referenceToolNumber); 
			}

			int mainToolNumber(-1);
			if ( trackerCertus->GetScalarAttribute("MainToolNumber", mainToolNumber) ) 
			{
				tracker->SetMainTool(mainToolNumber); 
				this->SetMainToolNumber(mainToolNumber); 
			}
#endif
		}
	}
	//******************* Polaris Tracker ***************************
	else if ( STRCASECMP("PolarisTracker", type)==0) 
	{
		LOG_DEBUG("Tracker type: Polaris tracker"); 
		this->SetTrackerType(TRACKER_POLARIS); 

		vtkSmartPointer<vtkXMLDataElement> trackerPolaris = trackerConfig->FindNestedElementWithName("PolarisTracker"); 

		if ( trackerPolaris != NULL )
		{
#ifdef PLUS_USE_POLARIS
			vtkSmartPointer<vtkPOLARISTracker> tracker = vtkSmartPointer<vtkPOLARISTracker>::New();
			this->SetTracker(tracker); 

			// TODO: set base parameters
#endif
		}
	}
	//******************* Aurora Tracker ***************************
	else if ( STRCASECMP("AuroraTracker", type)==0) 
	{
		LOG_DEBUG("Tracker type: Aurora tracker"); 
		this->SetTrackerType(TRACKER_AURORA); 

		vtkSmartPointer<vtkXMLDataElement> trackerAurora = trackerConfig->FindNestedElementWithName("AuroraTracker"); 

		if ( trackerAurora != NULL )
		{
#ifdef PLUS_USE_POLARIS
			vtkSmartPointer<vtkNDITracker> tracker = vtkSmartPointer<vtkNDITracker>::New();
			this->SetTracker(tracker); 

			// TODO: set base parameters
#endif
		}
	}
	//******************* Flock Tracker ***************************
	else if ( STRCASECMP("FlockTracker", type)==0) 
	{
		LOG_DEBUG("Tracker type: Flock tracker"); 
		this->SetTrackerType(TRACKER_FLOCK); 

		vtkSmartPointer<vtkXMLDataElement> trackerFlock = trackerConfig->FindNestedElementWithName("FlockTracker"); 

		if ( trackerFlock != NULL )
		{
#ifdef PLUS_USE_FLOCK
			vtkSmartPointer<vtkFlockTracker> tracker = vtkSmartPointer<vtkFlockTracker>::New();
			this->SetTracker(tracker); 

			// TODO: set base parameters
#endif
		}
	}
	//******************* Micron Tracker ***************************
	else if ( STRCASECMP("MicronTracker", type)==0) 
	{
		LOG_DEBUG("Tracker type: Micron tracker"); 
		this->SetTrackerType(TRACKER_MICRON); 

		vtkSmartPointer<vtkXMLDataElement> trackerMicron = trackerConfig->FindNestedElementWithName("MicronTracker"); 

		if ( trackerMicron != NULL )
		{
#ifdef PLUS_USE_MICRONTRACKER
			vtkSmartPointer<vtkMicronTracker> tracker = vtkSmartPointer<vtkMicronTracker>::New();
			this->SetTracker(tracker); 

			// TODO: set base parameters
#endif
		}
	}
	//******************* Saved dataset ***************************
	else if ( STRCASECMP("SavedDataset", type)==0) 
	{
		LOG_DEBUG("Tracker type: Saved Dataset");
		this->SetTrackerType(TRACKER_SAVEDDATASET); 

		vtkSmartPointer<vtkXMLDataElement> xmlconfig = trackerConfig->FindNestedElementWithName("SavedDataset"); 

		if ( xmlconfig != NULL )
		{
			vtkSmartPointer<vtkSavedDataTracker> tracker = vtkSmartPointer<vtkSavedDataTracker>::New();
			this->SetTracker(tracker); 
			tracker->ReadConfiguration(xmlconfig); 
			this->SetMainToolNumber(0); 
		}
	}
	//******************* Ascension 3DG ***************************
	else if ( STRCASECMP( "Ascension3DG", type ) == 0 )
	{
#ifdef PLUS_USE_Ascension3DG
		LOG_DEBUG( "Tracker type: Ascension 3DG" );
		this->SetTrackerType( TRACKER_ASCENSION3DG );

		vtkSmartPointer< vtkXMLDataElement > xmlconfig = trackerConfig->FindNestedElementWithName( "Ascension3DG" );

		if ( xmlconfig != NULL )
		{
			vtkSmartPointer< vtkAscension3DGTracker > tracker = vtkSmartPointer< vtkAscension3DGTracker >::New();
			this->SetTracker( tracker );
			this->SetMainToolNumber( 0 ); // TODO: Read this from config xml data.
		}
		else
		{
			LOG_WARNING("Unable to read Ascension3DG XML data element"); 
		}

#endif
	}
	//******************* Fake Tracker ***************************
	else if ( STRCASECMP("FakeTracker", type)==0) 
	{
		LOG_DEBUG("Tracker type: Fake Tracker");
		this->SetTrackerType(TRACKER_FAKE); 

		vtkSmartPointer<vtkXMLDataElement> xmlconfig = trackerConfig->FindNestedElementWithName("FakeTracker"); 

		if ( xmlconfig != NULL )
		{
			vtkSmartPointer<vtkFakeTracker> tracker = vtkSmartPointer<vtkFakeTracker>::New();
			this->SetTracker(tracker);
			tracker->ReadConfiguration(xmlconfig);
			this->SetMainToolNumber(tracker->GetMainTool());
		}
	}
	else
	{
		this->SetTrackerType(TRACKER_NONE); 
		LOG_DEBUG("Tracker type: None");
		this->SetTracker(NULL); 
	}

	if ( this->GetTracker() != NULL )
	{
		int bufferSize = 0; 
		if ( trackerConfig->GetScalarAttribute("BufferSize", bufferSize) ) 
		{
			for ( int i = 0; i < this->GetTracker()->GetNumberOfTools(); i++)
			{
				this->GetTracker()->GetTool(i)->GetBuffer()->SetBufferSize( bufferSize ); 
			}
		}

		double frequency = 0; 
		if ( trackerConfig->GetScalarAttribute("Frequency", frequency) ) 
		{
			this->GetTracker()->SetFrequency(frequency);  
		}

		double localTimeOffset = 0; 
		if ( trackerConfig->GetScalarAttribute("LocalTimeOffset", localTimeOffset) )
		{
			LOG_INFO("Tracker local time offset: " << std::fixed << 1000*localTimeOffset << "ms" ); 
			for ( int i = 0; i < this->GetTracker()->GetNumberOfTools(); i++)
			{
				this->GetTracker()->GetTool(i)->GetBuffer()->SetLocalTimeOffset(localTimeOffset);
			}
		}

	}
}


//------------------------------------------------------------------------------
void vtkDataCollector::ReadImageAcqusitionProperties(vtkXMLDataElement* imageAcqusitionConfig)
{
	LOG_TRACE("vtkDataCollector::ReadImageAcqusitionProperties");
	const char* type = imageAcqusitionConfig->GetAttribute("Type"); 

	if ( type == NULL ) 
	{
		vtkWarningMacro(<< "Unable to find image acquisition type, set to default None"); 

		this->SetAcquisitionType(SYNCHRO_VIDEO_NONE); 
		LOG_DEBUG("Image acquisition type: None");
		this->SetVideoSource(NULL); 
	}
	//******************* Sonix Video ***************************
	else if ( STRCASECMP("SonixVideo", type)==0) 
	{
		LOG_DEBUG("Image acquisition type: Sonix Video"); 
		this->SetAcquisitionType(SYNCHRO_VIDEO_SONIX); 

		vtkSmartPointer<vtkXMLDataElement> sonixVideo = imageAcqusitionConfig->FindNestedElementWithName("SonixVideo"); 

		if ( sonixVideo != NULL) 
		{
			//Set SonixRP parameters from config file
#ifdef PLUS_USE_SONIX_VIDEO
			vtkSmartPointer<vtkSonixVideoSource2> videoSource = vtkSmartPointer<vtkSonixVideoSource2>::New();
			this->SetVideoSource(videoSource); 

			const char* ipAddress = sonixVideo->GetAttribute("IP"); 
			if ( ipAddress != NULL) 
			{
				videoSource->SetSonixIP(ipAddress); 
			}
			LOG_DEBUG("Sonix Video IP: " << ipAddress); 

			int imagingMode = 0; 
			if ( sonixVideo->GetScalarAttribute("ImagingMode", imagingMode)) 
			{
				videoSource->SetImagingMode(imagingMode); 
			}

			int acquisitionDataType = 0; 
			if ( sonixVideo->GetScalarAttribute("AcquisitionDataType", acquisitionDataType)) 
			{
				videoSource->SetAcquisitionDataType(acquisitionDataType); 
			}

			int depth = -1; 
			if ( sonixVideo->GetScalarAttribute("Depth", depth)) 
			{
				videoSource->SetDepth(depth); 
			}

			int sector = -1; 
			if ( sonixVideo->GetScalarAttribute("Sector", sector)) 
			{
				videoSource->SetSector(sector); 
			}

			int gain = -1; 
			if ( sonixVideo->GetScalarAttribute("Gain", gain)) 
			{
				videoSource->SetGain(gain); 
			}

			int dynRange = -1; 
			if ( sonixVideo->GetScalarAttribute("DynRange", dynRange)) 
			{
				videoSource->SetDynRange(dynRange); 
			}

			int zoom = -1; 
			if ( sonixVideo->GetScalarAttribute("Zoom", zoom)) 
			{
				videoSource->SetZoom(zoom); 
			}

			int frequency = -1; 
			if ( sonixVideo->GetScalarAttribute("Frequency", frequency)) 
			{
				videoSource->SetFrequency(frequency); 
			}

			int compressionStatus = 0; 
			if ( sonixVideo->GetScalarAttribute("CompressionStatus", compressionStatus)) 
			{
				videoSource->SetCompressionStatus(compressionStatus); 
			}

			int timeout = 0; 
			if ( sonixVideo->GetScalarAttribute("Timeout", timeout)) 
			{
				videoSource->SetTimeout(timeout); 
			}
#endif
		}
	}
	//******************* Matrox Imaging ***************************
	else if ( STRCASECMP("MatroxImaging", type)==0) 
	{
		LOG_DEBUG("Image acquisition type: Matrox Imaging"); 
		this->SetAcquisitionType(SYNCHRO_VIDEO_MIL); 

		vtkSmartPointer<vtkXMLDataElement> matroxImaging = imageAcqusitionConfig->FindNestedElementWithName("MatroxImaging"); 

		if ( matroxImaging != NULL) 
		{
#ifdef PLUS_USE_MATROX_IMAGING
			vtkSmartPointer<vtkMILVideoSource2> videoSource = vtkSmartPointer<vtkMILVideoSource2>::New();
			this->SetVideoSource(videoSource); 

			// TODO: set base parameters
#endif
		}
	}
	//******************* Video For Windows ***************************
	else if ( STRCASECMP("VFWVideo", type)==0) 
	{
		LOG_DEBUG("Image acquisition type: Video For Windows"); 
		this->SetAcquisitionType(SYNCHRO_VIDEO_WIN32); 	
		vtkSmartPointer<vtkXMLDataElement> videoForWindows = imageAcqusitionConfig->FindNestedElementWithName("VFWVideo"); 

		if ( videoForWindows != NULL) 
		{
#ifdef VTK_VFW_SUPPORTS_CAPTURE
			vtkSmartPointer<vtkWin32VideoSource2> videoSource = vtkSmartPointer<vtkWin32VideoSource2>::New();
			this->SetVideoSource(videoSource); 
			// TODO: set base parameters
#endif
		}
	}
	//******************* IC Capturing frame grabber ***************************
	else if ( STRCASECMP("ICCapturing", type)==0) 
	{
		LOG_DEBUG("Image acquisition type: IC Capturing"); 
		this->SetAcquisitionType(SYNCHRO_VIDEO_ICCAPTURING); 
		vtkSmartPointer<vtkXMLDataElement> icCapturing = imageAcqusitionConfig->FindNestedElementWithName("ICCapturing"); 

		if ( icCapturing != NULL) 
		{
#ifdef PLUS_USE_ICCAPTURING_VIDEO
			vtkSmartPointer<vtkICCapturingSource2> videoSource = vtkSmartPointer<vtkICCapturingSource2>::New();
			this->SetVideoSource(videoSource); 

			const char* deviceName = icCapturing->GetAttribute("DeviceName"); 
			if ( deviceName != NULL) 
			{
				videoSource->SetDeviceName(deviceName); 
			}

			const char* videoNorm = icCapturing->GetAttribute("VideoNorm"); 
			if ( videoNorm != NULL) 
			{
				videoSource->SetVideoNorm(videoNorm); 
			}

			const char* videoFormat = icCapturing->GetAttribute("VideoFormat"); 
			if ( videoFormat != NULL) 
			{
				videoSource->SetVideoFormat(videoFormat); 
			}

			const char* inputChannel = icCapturing->GetAttribute("InputChannel"); 
			if ( inputChannel != NULL) 
			{
				videoSource->SetInputChannel(inputChannel); 
			}

			const char* licenseKey = icCapturing->GetAttribute("LicenseKey"); 
			if ( licenseKey != NULL) 
			{
				videoSource->SetLicenceKey(licenseKey); 
			}

			int icBufferSize = 0; 
			if ( icCapturing->GetScalarAttribute("ICBufferSize", icBufferSize) ) 
			{
				videoSource->SetICBufferSize(icBufferSize); 
			}
#endif
		}
	}
	//******************* Linux Video ***************************
	else if ( STRCASECMP("LinuxVideo", type)==0) 
	{
		LOG_DEBUG("Image acquisition type: Linux Video mode"); 
		this->SetAcquisitionType(SYNCHRO_VIDEO_LINUX); 
		vtkSmartPointer<vtkXMLDataElement> linuxVideo = imageAcqusitionConfig->FindNestedElementWithName("LinuxVideo"); 

		if ( linuxVideo != NULL) 
		{
#ifdef PLUS_USE_LINUX_VIDEO
			//vtkSmartPointer<vtkV4L2LinuxSource2> videoSource = vtkSmartPointer<vtkV4L2LinuxSource2>::New();
			//this->SetVideoSource(videoSource); 
			// TODO: set base parameters
#endif
		}
	}
	//******************* Noise Video ***************************
	else if ( STRCASECMP("NoiseVideo", type)==0) 
	{
		LOG_DEBUG("Image acquisition type: Noise Video"); 
		this->SetAcquisitionType(SYNCHRO_VIDEO_NOISE); 
		vtkSmartPointer<vtkXMLDataElement> noiseVideo = imageAcqusitionConfig->FindNestedElementWithName("NoiseVideo"); 

		if ( noiseVideo != NULL) 
		{
			vtkSmartPointer<vtkVideoSource2> videoSource = vtkSmartPointer<vtkVideoSource2>::New();
			//this->SetVideoSource(videoSource); 
		}
	}
	//******************* Saved dataset ***************************
	else if ( STRCASECMP("SavedDataset", type)==0 ) 
	{
		LOG_DEBUG("Image acquisition type: Saved Dataset");
		this->SetAcquisitionType(SYNCHRO_VIDEO_SAVEDDATASET); 

		vtkSmartPointer<vtkXMLDataElement> savedDataset = imageAcqusitionConfig->FindNestedElementWithName("SavedDataset"); 
		if ( savedDataset != NULL) 
		{
			vtkSmartPointer<vtkSavedDataVideoSource> videoSource = vtkSmartPointer<vtkSavedDataVideoSource>::New();
			this->SetVideoSource(videoSource); 

			const char* sequenceMetafile = savedDataset->GetAttribute("SequenceMetafile"); 
			if ( sequenceMetafile != NULL ) 
			{
				videoSource->SetSequenceMetafile(sequenceMetafile);
			}
		}
	}
	else
	{
		this->SetAcquisitionType(SYNCHRO_VIDEO_NONE); 
		LOG_DEBUG("Image acquisition type: None");
		this->SetVideoSource(NULL); 
	}

	if ( this->GetVideoSource() != NULL )
	{

		int frameSize[3] = {0, 0, 0}; 
		if ( imageAcqusitionConfig->GetVectorAttribute("FrameSize", 3, frameSize) )
		{
			this->GetVideoSource()->SetFrameSize(frameSize[0], frameSize[1], frameSize[2]); 
		}

		int bufferSize = 0; 
		if ( imageAcqusitionConfig->GetScalarAttribute("BufferSize", bufferSize) )
		{
			this->GetVideoSource()->GetBuffer()->SetBufferSize(bufferSize); 
		}

		int frameRate = 0; 
		if ( imageAcqusitionConfig->GetScalarAttribute("FrameRate", frameRate) )
		{
			this->GetVideoSource()->SetFrameRate(frameRate); 
		}

		double smoothingFactor = 0; 
		if ( imageAcqusitionConfig->GetScalarAttribute("SmoothingFactor", smoothingFactor) )
		{
			this->GetVideoSource()->SetSmoothingFactor(smoothingFactor); 
		}

		double localTimeOffset = 0; 
		if ( imageAcqusitionConfig->GetScalarAttribute("LocalTimeOffset", localTimeOffset) )
		{
			LOG_INFO("Image acqusition local time offset: " << std::fixed << 1000*localTimeOffset << "ms" ); 
			this->GetVideoSource()->GetBuffer()->SetLocalTimeOffset(localTimeOffset); 
		}
	}
}

//------------------------------------------------------------------------------
void vtkDataCollector::ReadSynchronizationProperties(vtkXMLDataElement* synchronizationConfig)
{
	LOG_TRACE("vtkDataCollector::ReadSynchronizationProperties");
	vtkSmartPointer<vtkDataCollectorSynchronizer> synchronizer = vtkSmartPointer<vtkDataCollectorSynchronizer>::New(); 
	this->SetSynchronizer(synchronizer); 

	vtkSmartPointer<vtkXMLDataElement> synchronizerConfig = synchronizationConfig->FindNestedElementWithName("Synchronizer"); 
	if ( synchronizerConfig == NULL) 
	{
		vtkWarningMacro(<< "Unable to find synchronizer settings, set to default."); 
	}
	else
	{
		int synchronizationTimeLength = 0; 
		if ( synchronizerConfig->GetScalarAttribute("SynchronizationTimeLength", synchronizationTimeLength) )
		{
			this->GetSynchronizer()->SetSynchronizationTimeLength(synchronizationTimeLength); 
		}

		int minNumOfSyncSteps = 0; 
		if ( synchronizerConfig->GetScalarAttribute("MinNumOfSyncSteps", minNumOfSyncSteps) )
		{
			this->GetSynchronizer()->SetMinNumOfSyncSteps(minNumOfSyncSteps); 
		}

		int numberOfAveragedFrames = 0; 
		if ( synchronizerConfig->GetScalarAttribute("NumberOfAveragedFrames", numberOfAveragedFrames) )
		{
			this->GetSynchronizer()->SetNumberOfAveragedFrames(numberOfAveragedFrames); 
		}

		int numberOfAveragedTransforms = 0; 
		if ( synchronizerConfig->GetScalarAttribute("NumberOfAveragedTransforms", numberOfAveragedTransforms) )
		{
			this->GetSynchronizer()->SetNumberOfAveragedTransforms(numberOfAveragedTransforms); 
		}

		int thresholdMultiplier = 0; 
		if ( synchronizerConfig->GetScalarAttribute("ThresholdMultiplier", thresholdMultiplier) )
		{
			this->GetSynchronizer()->SetThresholdMultiplier(thresholdMultiplier); 
		}

		double minTransformThreshold = 0; 
		if ( synchronizerConfig->GetScalarAttribute("MinTransformThreshold", minTransformThreshold) )
		{
			this->GetSynchronizer()->SetMinTransformThreshold(minTransformThreshold); 
		}

		double maxTransformDifference = 0; 
		if ( synchronizerConfig->GetScalarAttribute("MaxTransformDifference", maxTransformDifference) )
		{
			this->GetSynchronizer()->SetMaxTransformDifference(maxTransformDifference); 
		}

		double minFrameThreshold = 0; 
		if ( synchronizerConfig->GetScalarAttribute("MinFrameThreshold", minFrameThreshold) )
		{
			this->GetSynchronizer()->SetMinFrameThreshold(minFrameThreshold); 
		}

		double maxFrameDifference = 0; 
		if ( synchronizerConfig->GetScalarAttribute("MaxFrameDifference", maxFrameDifference) )
		{
			this->GetSynchronizer()->SetMaxFrameDifference(maxFrameDifference); 
		}
	}
}


//------------------------------------------------------------------------------
void vtkDataCollector::SetTrackingOnly(bool trackingOnly)
{
	LOG_TRACE("vtkDataCollector::SetTrackingOnly");
	if ( ! this->GetInitialized() )
	{
		this->TrackingOnly = trackingOnly;
	}
	else
	{
		if ( this->GetTrackingOnly() == trackingOnly )
		{
			return;
		}
		else if (this->GetVideoSource() != NULL)
		{
			if ( trackingOnly )
			{
				this->GetVideoSource()->Stop();
			}
			else
			{
				this->GetVideoSource()->Record();
			}
		}
	}
}

//------------------------------------------------------------------------------
void vtkDataCollector::SetVideoOnly(bool videoOnly)
{
	LOG_TRACE("vtkDataCollector::SetVideoOnly");
	if ( ! this->GetInitialized() )
	{
		this->VideoOnly = videoOnly;
	}
	else
	{
		if ( this->GetVideoOnly() == videoOnly )
		{
			return;
		}
		else if (this->GetTracker() != NULL)
		{
			if ( videoOnly )
			{
				this->GetTracker()->StopTracking();
			}
			else
			{
				this->GetTracker()->StartTracking();
			}
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

