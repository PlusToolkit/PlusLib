#include "FreehandCalibrationController.h"

#include "vtkFreehandController.h"
#include "PlusConfigure.h"

//-----------------------------------------------------------------------------

FreehandCalibrationController *FreehandCalibrationController::m_Instance = NULL;

//-----------------------------------------------------------------------------

FreehandCalibrationController* FreehandCalibrationController::GetInstance() {
	if (! m_Instance) {
		m_Instance = new FreehandCalibrationController();
	}
	return m_Instance;
}

//-----------------------------------------------------------------------------

FreehandCalibrationController::FreehandCalibrationController()
	:AbstractToolboxController()
	,m_ProbeCalibrationController(NULL)
	,m_TemporalCalibrationDone(false)
{
}

//-----------------------------------------------------------------------------

FreehandCalibrationController::~FreehandCalibrationController()
{
	if (m_ProbeCalibrationController != NULL) {
		m_ProbeCalibrationController->Delete();
		m_ProbeCalibrationController = NULL;
	}

}

//-----------------------------------------------------------------------------

void FreehandCalibrationController::Initialize()
{
	LOG_DEBUG("Initialize FreehandCalibrationController");

	vtkFreehandController* controller = vtkFreehandController::GetInstance();
	if ((controller == NULL) || (controller->GetInitialized() == false)) {
		LOG_ERROR("vtkFreehandController is not initialized!");
		return;
	}

	if (m_Toolbox) {
		m_Toolbox->Initialize();
	}

	if (m_State == ToolboxState_Uninitialized) {
		// Initialize visualization
		if (controller->GetCanvas() != NULL) {
			// Add image actor to the realtime renderer, and add renderer to Canvas
			controller->GetCanvasRenderer()->AddActor(controller->GetCanvasImageActor());
		}
	} else if (vtkFreehandController::GetInstance()->GetCanvas() != NULL) {  // If already initialized (it can occur if tab change - and so clear - happened)
		// Add all actors to the renderer again - state must be "Done", because tab cannot be changed if "In progress"
		controller->GetCanvasRenderer()->AddActor(controller->GetCanvasImageActor());
		controller->GetCanvasRenderer()->Modified();
	}

	// Set state to idle
	if (m_State == ToolboxState_Uninitialized) {
		m_State = ToolboxState_Idle;
	}
}

//-----------------------------------------------------------------------------

void FreehandCalibrationController::Clear()
{
	LOG_DEBUG("Clear FreehandCalibrationController");
}

//-----------------------------------------------------------------------------

void FreehandCalibrationController::DoAcquisition()
{
}

//-----------------------------------------------------------------------------

void FreehandCalibrationController::Start()
{
	//TODO
	m_State = ToolboxState_InProgress;
}

//-----------------------------------------------------------------------------

void FreehandCalibrationController::Stop()
{
	//TODO
	m_State = ToolboxState_Done;
}

//-----------------------------------------------------------------------------

bool FreehandCalibrationController::IsTemporalCalibrationDone()
{
	return m_TemporalCalibrationDone;
}

//-----------------------------------------------------------------------------

void FreehandCalibrationController::StartTemporalCalibration()
{
	if ( m_ProbeCalibrationController->GetCalibrationMode() != REALTIME ) {
		LOG_ERROR( "Unable to start temporal calibration in offline mode!" );  
		return; 
	}
/*
	if ( this->ProgressBarUpdateCallbackFunction != NULL ) {
		this->GetDataCollector()->GetSynchronizer()->SetProgressBarUpdateCallbackFunction(this->ProgressBarUpdateCallbackFunction); 
	}

	this->CancelRequestOff(); 

	// Set the local timeoffset to 0 before synchronization 
	if ( this->DataCollector->GetSynchronizer()->GetSynchronized() )
	{
		this->DataCollector->GetVideoSource()->GetBuffer()->SetLocalTimeOffset( 0 ); 

		for ( int i = 0; i < this->DataCollector->GetTracker()->GetNumberOfTools(); i++ )
		{
			this->DataCollector->GetTracker()->GetTool(i)->GetBuffer()->SetLocalTimeOffset( 0 ); 
		}
	}

	// Set the length of the acquisition 
	const double syncTimeLength = this->GetDataCollector()->GetSynchronizer()->GetSynchronizationTimeLength(); 

	// Get the realtime tracking frequency
	this->DataCollector->GetTracker()->GetTool(this->DataCollector->GetMainToolNumber())->GetBuffer()->Lock(); 
	double trackerFrameRate = this->DataCollector->GetTracker()->GetTool(this->DataCollector->GetMainToolNumber())->GetBuffer()->GetFrameRate(); 
	this->DataCollector->GetTracker()->GetTool(this->DataCollector->GetMainToolNumber())->GetBuffer()->Unlock(); 

	// Get the realtime video frame rate
	this->DataCollector->GetVideoSource()->GetBuffer()->Lock();
	double videoFrameRate = this->DataCollector->GetVideoSource()->GetBuffer()->GetFrameRate(); 
	this->DataCollector->GetVideoSource()->GetBuffer()->Unlock();

	const int trackerBufferSize = this->DataCollector->GetTracker()->GetTool(this->DataCollector->GetMainToolNumber())->GetBuffer()->GetBufferSize(); 
	const int videoBufferSize = this->DataCollector->GetVideoSource()->GetBuffer()->GetBufferSize(); 
	int syncTrackerBufferSize = trackerFrameRate * syncTimeLength + 100; 
	int syncVideoBufferSize = videoFrameRate * syncTimeLength + 100; 

	//************************************************************************************
	// Change buffer size to fit the whole acquisition 
	if ( syncVideoBufferSize > videoBufferSize )
	{
		LOG_DEBUG("Change video buffer size to: " << syncVideoBufferSize); 
		this->DataCollector->GetVideoSource()->GetBuffer()->Lock(); 
		this->DataCollector->GetVideoSource()->GetBuffer()->Clear(); 
		this->DataCollector->GetVideoSource()->GetBuffer()->Unlock();

		this->DataCollector->GetVideoSource()->SetFrameBufferSize(syncVideoBufferSize);
	}
	else
	{
		this->DataCollector->GetVideoSource()->GetBuffer()->Lock(); 
		this->DataCollector->GetVideoSource()->GetBuffer()->Clear(); 
		this->DataCollector->GetVideoSource()->GetBuffer()->Unlock(); 
	}

	if ( syncTrackerBufferSize > trackerBufferSize )
	{
		LOG_DEBUG("Change tracker buffer size to: " << syncTrackerBufferSize); 
		for ( int i = 0; i < this->DataCollector->GetTracker()->GetNumberOfTools(); i++ )
		{
			this->DataCollector->GetTracker()->GetTool(i)->GetBuffer()->Lock(); 
			this->DataCollector->GetTracker()->GetTool(i)->GetBuffer()->Clear(); 
			this->DataCollector->GetTracker()->GetTool(i)->GetBuffer()->SetBufferSize(syncTrackerBufferSize); 
			this->DataCollector->GetTracker()->GetTool(i)->GetBuffer()->Unlock(); 
		}
	}
	else
	{
		for ( int i = 0; i < this->DataCollector->GetTracker()->GetNumberOfTools(); i++ )
		{
			this->DataCollector->GetTracker()->GetTool(i)->GetBuffer()->Lock(); 
			this->DataCollector->GetTracker()->GetTool(i)->GetBuffer()->Clear(); 
			this->DataCollector->GetTracker()->GetTool(i)->GetBuffer()->Unlock(); 
		}
	}

	//************************************************************************************
	// Acquire data 
	const double syncStartTime = vtkAccurateTimer::GetSystemTime(); 
	while ( syncStartTime + syncTimeLength > vtkAccurateTimer::GetSystemTime() )
	{
		if ( this->CancelRequest ) 
		{
			// we should cancel the job...
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
	if ( this->DataCollector->GetVideoSource() != NULL ) 
	{
		LOG_DEBUG("Copy video buffer ..."); 
		this->DataCollector->CopyVideoBuffer(videobuffer); 
	}

	vtkSmartPointer<vtkTracker> tracker = vtkSmartPointer<vtkTracker>::New(); 
	if ( this->DataCollector->GetTracker() != NULL )
	{
		LOG_DEBUG("Copy tracker ..."); 
		this->DataCollector->CopyTracker(tracker); 
	}
	vtkTrackerBuffer* trackerbuffer = tracker->GetTool(this->DataCollector->GetMainToolNumber())->GetBuffer(); 

	if ( this->ProbeCalibrationController->GetEnableTrackedSequenceDataSaving() )
	{
		LOG_INFO(">>>>>>>>>>>> Save temporal calibration buffers to file ... "); 
		this->DataCollector->DumpTrackerBufferToMetafile(trackerbuffer, "./", "DataCollectorSyncTrackerBuffer", false );
		this->DataCollector->DumpVideoBufferToMetafile(videobuffer, "./", "DataCollectorSyncVideoBuffer", false ); 
	}

	//************************************************************************************
	// Start synchronization 
	this->DataCollector->GetSynchronizer()->SetSyncStartTime(syncStartTime); 

	LOG_DEBUG("Tracker buffer size: " << trackerbuffer->GetBufferSize()); 
	LOG_DEBUG("Tracker buffer elements: " << trackerbuffer->GetNumberOfItems()); 
	LOG_DEBUG("Video buffer size: " << videobuffer->GetBufferSize()); 
	LOG_DEBUG("Video buffer elements: " << videobuffer->GetNumberOfItems()); 
	this->DataCollector->GetSynchronizer()->SetTrackerBuffer(trackerbuffer); 
	this->DataCollector->GetSynchronizer()->SetVideoBuffer(videobuffer); 

	this->DataCollector->GetSynchronizer()->Synchronize(); 

	//************************************************************************************
	// Set the local time for buffers if the calibration was done
	if ( this->DataCollector->GetSynchronizer()->GetSynchronized() )
	{
		this->DataCollector->GetVideoSource()->GetBuffer()->SetLocalTimeOffset( this->DataCollector->GetSynchronizer()->GetVideoOffset() ); 

		for ( int i = 0; i < this->DataCollector->GetTracker()->GetNumberOfTools(); i++ )
		{
			this->DataCollector->GetTracker()->GetTool(i)->GetBuffer()->SetLocalTimeOffset(this->DataCollector->GetSynchronizer()->GetTrackerOffset() ); 
		}

		this->TemporalCalibrationDoneOn(); 
	}
	*/
}

//-----------------------------------------------------------------------------

void FreehandCalibrationController::StartSpatialCalibration()
{
	if ( m_ProbeCalibrationController->GetCalibrationMode() != REALTIME ) {
		LOG_ERROR( "Unable to start spatial calibration in offline mode!" );  
		return; 
	}
	/*
	this->CancelRequestOff(); 
	const int maxNumOfProbeTranslationImages = this->StepperCalibrationController->GetRealtimeImageDataInfo(PROBE_TRANSLATION).NumberOfImagesToAcquire; 
	
	while ( this->StepperCalibrationController->GetRealtimeImageDataInfo(PROBE_TRANSLATION).NumberOfSegmentedImages < maxNumOfProbeTranslationImages )
	{
		if ( this->CancelRequest ) 
		{
			// we should cancel the job...
			return; 
		}

		this->AddTrackedFrame(PROBE_TRANSLATION); 
	}

	this->StepperCalibrationController->CalibrateProbeTranslationAxis(); 
	this->ProbeTranslationAxisCalibrationDoneOn();
	*/
}

//-----------------------------------------------------------------------------

void FreehandCalibrationController::SaveCalibrationResults(std::string aFile)
{
	//TODO temporal+spatial (check if they are available)
}

