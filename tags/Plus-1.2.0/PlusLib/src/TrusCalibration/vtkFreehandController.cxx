#include "PlusConfigure.h"

#include "vtkFreehandController.h"

#include "vtkObjectFactory.h"
#include "vtkDirectory.h"

//-----------------------------------------------------------------------------

vtkFreehandController *vtkFreehandController::Instance = NULL;

//-----------------------------------------------------------------------------

vtkFreehandController* vtkFreehandController::New()
{
	return vtkFreehandController::GetInstance();
}

//-----------------------------------------------------------------------------

vtkFreehandController* vtkFreehandController::GetInstance()
{
	if(!vtkFreehandController::Instance) {
		// Try the factory first
		vtkFreehandController::Instance = (vtkFreehandController*)vtkObjectFactory::CreateInstance("vtkFreehandController");    

		if(!vtkFreehandController::Instance) {
			vtkFreehandController::Instance = new vtkFreehandController();	   
		}
	}
	// return the instance
	return vtkFreehandController::Instance;
}

//-----------------------------------------------------------------------------

vtkFreehandController::vtkFreehandController()
{
	this->DataCollector = NULL;
	this->RecordingFrameRate = 20;
	this->InputConfigFileName = NULL;
	this->OutputFolder = NULL;
	this->ProgramPath = NULL;
	this->InitializedOff();
	this->TrackingOnlyOn();
	this->Canvas = NULL;
	this->CanvasRenderer = NULL;

	VTK_LOG_TO_CONSOLE_ON
}

//-----------------------------------------------------------------------------

vtkFreehandController::~vtkFreehandController()
{
	if (this->DataCollector != NULL) {
		this->DataCollector->Stop();
	}

	this->SetDataCollector(NULL);
	this->SetCanvasRenderer(NULL);
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandController::Initialize()
{
	if (this->Initialized) {
		return PLUS_SUCCESS;
	}

	LOG_TRACE("Initialize vtkFreehandController"); 

	// Set up canvas renderer
	vtkSmartPointer<vtkRenderer> canvasRenderer = vtkSmartPointer<vtkRenderer>::New(); 
	canvasRenderer->SetBackground(0.6, 0.6, 0.6);
	this->SetCanvasRenderer(canvasRenderer);

	// Create directory for the output
	vtkSmartPointer<vtkDirectory> dir = vtkSmartPointer<vtkDirectory>::New(); 
	if ((this->OutputFolder != NULL) && (dir->Open(this->OutputFolder) == 0)) {	
		dir->MakeDirectory(this->OutputFolder);
	}

	this->SetInitialized(true);

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void vtkFreehandController::SetTrackingOnly(bool aOn)
{
	LOG_DEBUG("Set tracking only to " << (aOn ? "true" : "false"));

	this->TrackingOnly = aOn;

	if (this->DataCollector != NULL) {
		this->DataCollector->SetTrackingOnly(aOn);
	}
}

//-----------------------------------------------------------------------------

PlusStatus vtkFreehandController::StartDataCollection()
{
	// Stop data collection if already started
	if (this->GetDataCollector() != NULL) {
		this->GetDataCollector()->Stop();
	}

	// Set up data collector
	vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New(); 
	this->SetDataCollector(dataCollector);

	if (this->DataCollector->ReadConfigurationFromFile(this->InputConfigFileName) != PLUS_SUCCESS) {
		return PLUS_FAIL;
	}
	if (this->DataCollector->Initialize() != PLUS_SUCCESS) {
		return PLUS_FAIL;
	}
	if (this->DataCollector->Start() != PLUS_SUCCESS) {
		return PLUS_FAIL;
	}

	if ((this->DataCollector->GetTracker() == NULL) || (this->DataCollector->GetTracker()->GetNumberOfTools() < 1)) {
		LOG_WARNING("Unable to initialize Tracker!"); 
	}

	if (! this->DataCollector->GetInitialized()) {
		LOG_ERROR("Unable to initialize DataCollector!"); 
		return PLUS_FAIL;
	}

	return PLUS_SUCCESS;
}
