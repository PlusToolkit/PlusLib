#include "vtkFreehandCalibrationController.h"

#include "vtkFreehandController.h"
#include "PlusConfigure.h"

#include "vtkObjectFactory.h"
#include "vtkAccurateTimer.h"

//-----------------------------------------------------------------------------

vtkFreehandCalibrationController *vtkFreehandCalibrationController::Instance = NULL;

//-----------------------------------------------------------------------------

vtkFreehandCalibrationController* vtkFreehandCalibrationController::New()
{
	return vtkFreehandCalibrationController::GetInstance();
}

//-----------------------------------------------------------------------------

vtkFreehandCalibrationController* vtkFreehandCalibrationController::GetInstance() {
	if(!vtkFreehandCalibrationController::Instance) {
		// Try the factory first
		vtkFreehandCalibrationController::Instance = (vtkFreehandCalibrationController*)vtkObjectFactory::CreateInstance("vtkFreehandCalibrationController");    

		if(!vtkFreehandCalibrationController::Instance) {
			vtkFreehandCalibrationController::Instance = new vtkFreehandCalibrationController();	   
		}
	}
	// return the instance
	return vtkFreehandCalibrationController::Instance;
}

//-----------------------------------------------------------------------------

vtkFreehandCalibrationController::vtkFreehandCalibrationController()
	:AbstractToolboxController()
{
	this->TemporalCalibrationDone = false;
	this->ProgressPercent = 0;

	VTK_LOG_TO_CONSOLE_ON
}

//-----------------------------------------------------------------------------

vtkFreehandCalibrationController::~vtkFreehandCalibrationController()
{
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::Initialize()
{
	LOG_DEBUG("Initialize vtkFreehandCalibrationController");

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

void vtkFreehandCalibrationController::Clear()
{
	LOG_DEBUG("Clear vtkFreehandCalibrationController");
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::DoAcquisition()
{
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::Start()
{
	//TODO
	m_State = ToolboxState_InProgress;
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::Stop()
{
	//TODO
	m_State = ToolboxState_Done;
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::UpdateProgress(int aPercent) {
	vtkFreehandCalibrationController::GetInstance()->SetProgressPercent(aPercent);
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::StartTemporalCalibration()
{
	vtkFreehandController* controller = vtkFreehandController::GetInstance();
	if ((controller == NULL) || (controller->GetInitialized() == false)) {
		LOG_ERROR("vtkFreehandController is not initialized!");
		return;
	}

	controller->GetDataCollector()->SetProgressBarUpdateCallbackFunction(UpdateProgress);
	controller->GetDataCollector()->Synchronize();

	this->TemporalCalibrationDoneOn();
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::StartSpatialCalibration()
{
	if ( GetCalibrationMode() != REALTIME ) {
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

void vtkFreehandCalibrationController::SaveCalibrationResults(std::string aFile)
{
	//TODO temporal+spatial (check if they are available)
}

