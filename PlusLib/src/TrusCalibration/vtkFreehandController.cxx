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

vtkFreehandController* vtkFreehandController::GetInstance() {
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
	//this->TrackedFrameContainer = NULL; TODO
	this->RecordingFrameRate = 20;
	this->InputConfigFileName = NULL;
	this->OutputFolder = NULL;
	this->ProgramPath = NULL;
	this->ConfigDirectory = NULL;
	this->InitializedOff();
	this->TrackingOnlyOn();
	this->Canvas = NULL;
	this->CanvasRenderer = NULL;
	//this->CanvasImageActor = NULL; TODO

	VTK_LOG_TO_CONSOLE_ON
}

//-----------------------------------------------------------------------------

vtkFreehandController::~vtkFreehandController()
{
	this->SetDataCollector(NULL);
	this->SetCanvasRenderer(NULL);
	//this->SetCanvasImageActor(NULL); TODO

	/* TODO
	if ( this->TrackedFrameContainer != NULL ) {
		this->TrackedFrameContainer->Delete(); 
		this->TrackedFrameContainer = NULL; 
	}*/
}

//-----------------------------------------------------------------------------

void vtkFreehandController::Initialize()
{
	if (this->Initialized) {
		return;
	}

	LOG_DEBUG("Initialize vtkFreehandController"); 

	// Set up data collector and related containers
	vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New(); 
	this->SetDataCollector(dataCollector);

	this->DataCollector->ReadConfiguration(this->InputConfigFileName);
	this->DataCollector->Initialize();
	this->DataCollector->Start();

	if (this->DataCollector->GetTracker()->GetNumberOfTools() < 1) {
		LOG_ERROR("Unable to initialize Tracker!"); 
		exit(EXIT_FAILURE); // TODO ezek helyett logbol GUIra kiiratas
	}

	if (! this->DataCollector->GetInitialized()) {
		LOG_ERROR("Unable to initialize DataCollector!"); 
		exit(EXIT_FAILURE); 
	}

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
