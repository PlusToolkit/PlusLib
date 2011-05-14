#include "vtkFreehandCalibrationController.h"

#include "vtkFreehandController.h"
#include "PhantomRegistrationController.h"
#include "PlusConfigure.h"

#include "BrachyTRUSCalibrator.h"

#include "vtkObjectFactory.h"
#include "vtkAccurateTimer.h"

//-----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkFreehandCalibrationController, "$Revision: 1.0 $");

vtkFreehandCalibrationController *vtkFreehandCalibrationController::Instance = NULL;

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
} 

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

	this->USImageFrameOriginXInPixels = 0; 
	this->USImageFrameOriginYInPixels = 0; 

	// Initializing vtkCalibrationController members
	this->InitializedOff(); 
	this->EnableSystemLogOff();
	this->SetCalibrationMode(REALTIME); 
	this->VisualizationComponent = NULL;

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

	// Initialize vtkCalibrationController
	if ( this->GetSegParameters() == NULL ) {
		this->SegParameters = new SegmentationParameters(); 
	}

	// Initialize the segmenation component
	this->mptrAutomatedSegmentation = new KPhantomSeg( 
		this->GetImageWidthInPixels(), this->GetImageHeightInPixels(), 
		this->GetSearchStartAtX(), this->GetSearchStartAtY(), 
		this->GetSearchDimensionX(), this->GetSearchDimensionY(), this->GetEnableSegmentationAnalysis(), "frame.jpg");

	// Initialize the calibration component
	if ( CalibrationPhantom == NULL ) {
		CalibrationPhantom = new BrachyTRUSCalibrator( this->GetEnableSystemLog() );
	}

	// Set the ultrasound image frame in pixels
	// This defines the US image frame origin in pixels W.R.T. the left-upper corner of the original image, with X pointing to the right (column) and Y pointing down to the bottom (row).
	this->SetUSImageFrameOriginInPixels( this->GetUSImageFrameOriginXInPixels(), this->GetUSImageFrameOriginYInPixels() ); 

	// STEP-OPTIONAL. Apply the US 3D beamwidth data to calibration if desired
	// This will pass the US 3D beamwidth data and their predefined weights to the calibration component.
	/*
	if( this->GetUS3DBeamwidthDataReady() )
	{
		this->GetCalibrator()->setUltrasoundBeamwidthAndWeightFactorsTable(
			this->GetIncorporatingUS3DBeamProfile(),
			*this->GetInterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM(),
			*this->GetSortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN(),
			*this->GetMinElevationBeamwidthAndFocalZoneInUSImageFrame() );
	}
	*/

	// Set state to idle (initialized)
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

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::SetUSImageFrameOriginInPixels(int* origin)
{
	this->SetUSImageFrameOriginInPixels(origin[0], origin[1]); 
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::SetUSImageFrameOriginInPixels(int originX, int originY)
{
	LOG_TRACE("vtkFreehandCalibrationController::SetUSImageFrameOriginInPixels: " << originX << "  " << originY); //TODO Copy these traces to the other functions as well

	this->SetUSImageFrameOriginXInPixels(originX);
	this->SetUSImageFrameOriginYInPixels(originY);

	if (this->GetCalibrator() != NULL) {
		this->GetCalibrator()->setUltrasoundImageFrameOriginInPixels(originX, originY); 
	}
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::ReadConfiguration(const char* configFileNameWithPath)
{
	LOG_TRACE("vtkProbeCalibrationController::ReadConfiguration - " << configFileNameWithPath);

	this->SetConfigurationFileName(configFileNameWithPath);
	
	vtkXMLDataElement *calibrationController = vtkXMLUtilities::ReadElementFromFile(this->GetConfigurationFileName());
	ReadConfiguration(calibrationController);
	calibrationController->Delete();
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::ReadConfiguration(vtkXMLDataElement* configData)
{
	LOG_TRACE("vtkProbeCalibrationController::ReadConfiguration");

	if ( configData == NULL ) {
		LOG_ERROR("Unable to read the main configration file"); 
		exit(EXIT_FAILURE); 
	}

	// Calibration controller specifications
	vtkSmartPointer<vtkXMLDataElement> calibrationController = configData->FindNestedElementWithName("CalibrationController"); 
	this->ReadCalibrationControllerConfiguration(calibrationController); 

	// ProbeCalibration specifications
	//vtkSmartPointer<vtkXMLDataElement> probeCalibration = calibrationController->FindNestedElementWithName("ProbeCalibration");
	//this->CalibrationControllerIO->ReadProbeCalibrationConfiguration(probeCalibration);
//TODO a number of needed image-t egyelore beegetem a kodba, a tobbi cucc meg egyelore nem kell
}

//-----------------------------------------------------------------------------

void vtkFreehandCalibrationController::RegisterPhantomGeometry()
{
	LOG_TRACE("vtkFreehandCalibrationController::RegisterPhantomGeometry"); 

	// Register the phantom geometry to the DRB frame in the "Emulator" mode.
	vnl_matrix<double> transformMatrixPhantom2DRB4x4InEmulatorMode(4,4);
	this->ConvertVtkMatrixToVnlMatrixInMeter( PhantomRegistrationController::GetInstance()->GetPhantomReferenceToPhantomTransform()->GetMatrix(), transformMatrixPhantom2DRB4x4InEmulatorMode ); 

	this->GetCalibrator()->registerPhantomGeometryInEmulatorMode( transformMatrixPhantom2DRB4x4InEmulatorMode );
}
