#include "vtkFreehandController.h"

#include "vtkObjectFactory.h"
#include "vtkDirectory.h"
#include "vtkTrackerTool.h"
#include "vtkTrackedFrameList.h"

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
	this->TrackedFrameContainer = NULL;
	this->RecordingFrameRate = 20;
	this->InputConfigFileName = NULL;
	this->OutputFolder = NULL;
	this->ProgramPath = NULL;
	this->ConfigDirectory = NULL;
	this->InitializedOff();
	this->TrackingOnlyOn();
	this->Canvas = NULL;
	this->CanvasRenderer = NULL;
	this->CanvasImageActor = NULL;

	VTK_LOG_TO_CONSOLE_ON
}

//-----------------------------------------------------------------------------

vtkFreehandController::~vtkFreehandController()
{
	this->SetDataCollector(NULL);
	this->SetCanvasRenderer(NULL);
	//this->SetCanvasImageActor(NULL); TODO

	if ( this->TrackedFrameContainer != NULL ) {
		this->TrackedFrameContainer->Delete(); 
		this->TrackedFrameContainer = NULL; 
	}
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

	if ( this->TrackedFrameContainer == NULL ) {
		this->TrackedFrameContainer = vtkTrackedFrameList::New(); 
	}

	// If we hadn't set the main tool number, set to the first active
	if ( this->DataCollector->GetMainToolNumber() == -1) {
		this->DataCollector->SetMainToolNumber(this->GetNextActiveToolNumber());
	}

	// Set up canvas image actor
	vtkSmartPointer<vtkImageActor> canvasImageActor = vtkSmartPointer<vtkImageActor>::New();
	//canvasImageActor->VisibilityOn(); 
	this->SetCanvasImageActor(canvasImageActor); 
	//CanvasImageActor->SetInput( this->DataCollector->GetOutput() );

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

int vtkFreehandController::GetNextActiveToolNumber()
{
	int nextToolNumber = this->DataCollector->GetMainToolNumber();
	const int numberOfTools = this->DataCollector->GetTracker()->GetNumberOfTools(); 
	bool toolActive(false);
	int loop(0); 

	while (!toolActive) {
		if (++nextToolNumber >= numberOfTools) {
			nextToolNumber = 0; 
			loop++; 
		}

		toolActive = this->DataCollector->GetTracker()->GetTool(nextToolNumber)->GetEnabled(); 

		// avoid endless loop
		if (loop > 1) {
			return this->DataCollector->GetMainToolNumber();
		}
	}

	return nextToolNumber; 
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

int vtkFreehandController::GetNumberOfRecordedFrames()
{ 
	int numOfFrames = 0;
	if (this->TrackedFrameContainer) {
		numOfFrames = this->TrackedFrameContainer->GetNumberOfTrackedFrames(); 
	}

	return numOfFrames; 
}

//-----------------------------------------------------------------------------

void vtkFreehandController::AddTrackedFrame( vtkImageData* aImageData, std::vector<vtkMatrix4x4*> aToolTransforms, std::vector<std::string> aToolTransformNames, std::vector<long> aFlags, double aTimestamp)
{
	TrackedFrame trackedFrame;
	trackedFrame.ImageData = NULL; 
	trackedFrame.Timestamp = aTimestamp; 
	trackedFrame.DefaultFrameTransformName = aToolTransformNames[this->DataCollector->GetMainToolNumber()]; 

	if ( !this->TrackedFrameContainer->ValidateData(&trackedFrame) ) {
		// We've already inserted this frame into the sequence
		return; 
	}
	/*
	// convert vtkImageData to itkImage 
	vtkSmartPointer<vtkImageFlip> imageFlipy = vtkSmartPointer<vtkImageFlip>::New(); 
	imageFlipy->SetInput(imageData); 
	imageFlipy->SetFilteredAxis(1); 
	imageFlipy->Update(); 

	vtkSmartPointer<vtkImageExport> imageExport = vtkSmartPointer<vtkImageExport>::New(); 
	imageExport->ImageLowerLeftOff();
	imageExport->SetInput(imageFlipy->GetOutput()); 
	imageExport->Update(); 

	ImageType::Pointer frame = ImageType::New();
	double width = imageData->GetExtent()[1] - imageData->GetExtent()[0] + 1; 
	double height = imageData->GetExtent()[3] - imageData->GetExtent()[2] + 1; 
	ImageType::SizeType size = { width, height };
	ImageType::IndexType start = {0,0};
	ImageType::RegionType region;
	region.SetSize(size);
	region.SetIndex(start);
	frame->SetRegions(region);
	frame->Allocate();

	memcpy( frame->GetBufferPointer(), imageExport->GetPointerToData(), imageExport->GetDataMemorySize() ); 

	trackedFrame.ImageData = frame;
	*/
	// Save flags
	for (unsigned int i = 0; i < aFlags.size(); i++) {
		if (aToolTransformNames.size() <= i) {
			LOG_ERROR("Unable to find tool name for tool number: " << i); 
			continue; 
		}

		std::string flagName = aToolTransformNames[i] + std::string("Status"); 
		std::string flagFieldValue;

		if ( aFlags[i] == TR_OK ) {
			flagFieldValue = "OK "; 
		} else if ( (aFlags[i] & TR_MISSING) != 0 ) {
			flagFieldValue = "TR_MISSING "; 
		} else if ( (aFlags[i] & TR_OUT_OF_VIEW) != 0 ) {
			flagFieldValue = "TR_OUT_OF_VIEW "; 
		} else if ( (aFlags[i] & TR_OUT_OF_VOLUME) != 0 ) {
			flagFieldValue = "TR_OUT_OF_VOLUME "; 
		} else if ( (aFlags[i] & TR_REQ_TIMEOUT) != 0 ) {
			flagFieldValue = "TR_REQ_TIMEOUT "; 
		}

		trackedFrame.SetCustomFrameField(flagName, flagFieldValue); 
	}


	// Save transforms
	for (unsigned int i = 0; i < aToolTransforms.size(); i++) {
		if (aToolTransformNames.size() <= i) {
			LOG_ERROR("Unable to find tool transform name for tool number: " << i); 
			continue; 
		}

		trackedFrame.SetCustomFrameTransform(aToolTransformNames[i], aToolTransforms[i]); 
	}

	this->TrackedFrameContainer->AddTrackedFrame(&trackedFrame); 

	LOG_DEBUG("New tracked frame to container added"); 
}

//-----------------------------------------------------------------------------

void vtkFreehandController::ClearTrackedFrameContainer()
{
	this->TrackedFrameContainer->Clear();
}
