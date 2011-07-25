#include "PlusConfigure.h"

#include "VolumeReconstructionController.h"
#include "vtkFreehandController.h"

#include "vtkVolumeReconstructor.h"
#include "vtkImageExtractComponents.h"
#include "vtkDataSetWriter.h"
#include "vtkImageData.h"
#include "vtkActor.h"
#include "vtkMarchingContourFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"

//-----------------------------------------------------------------------------

VolumeReconstructionController *VolumeReconstructionController::m_Instance = NULL;

//-----------------------------------------------------------------------------

VolumeReconstructionController* VolumeReconstructionController::GetInstance() {
	if (! m_Instance) {
		m_Instance = new VolumeReconstructionController();
	}
	return m_Instance;
}

//-----------------------------------------------------------------------------

VolumeReconstructionController::VolumeReconstructionController()
	: AbstractToolboxController()
	, m_VolumeReconstructor(NULL)
	, m_VolumeReconstructionConfigFileLoaded(false)
	, m_ProgressPercent(0)
	, m_ProgressMessage("")
	, m_ReconstructedVolume(NULL)
	, m_ContourActor(NULL)
	, m_ContouringThreshold(64.0)
{
	m_VolumeReconstructor = vtkVolumeReconstructor::New();

	m_State = ToolboxState_Uninitialized;
}

//-----------------------------------------------------------------------------

VolumeReconstructionController::~VolumeReconstructionController()
{
	if (m_VolumeReconstructor != NULL) {
		m_VolumeReconstructor->Delete();
		m_VolumeReconstructor = NULL;
	}

	if (m_ReconstructedVolume != NULL) {
		m_ReconstructedVolume->Delete();
		m_ReconstructedVolume = NULL;
	}

	if (m_ContourActor != NULL) {
		m_ContourActor->Delete();
		m_ContourActor = NULL;
	}
}

//-----------------------------------------------------------------------------

PlusStatus VolumeReconstructionController::Initialize()
{
	LOG_TRACE("VolumeReconstructionController::Initialize");

	if (m_Toolbox) {
		m_Toolbox->Initialize();
	}

	InitializeVisualization();

	// Set state to idle
	if (m_State == ToolboxState_Uninitialized) {
		m_State = ToolboxState_Idle;
	}

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void VolumeReconstructionController::InitializeVisualization()
{
	LOG_TRACE("VolumeReconstructionController::InitializeVisualization"); 

	if (m_State == ToolboxState_Uninitialized) {
		if (vtkFreehandController::GetInstance()->GetCanvas() != NULL) {
			// Initialize contour actor
			m_ContourActor = vtkActor::New();
			vtkFreehandController::GetInstance()->GetCanvasRenderer()->AddActor(m_ContourActor);
		}
	} else if (vtkFreehandController::GetInstance()->GetCanvas() != NULL) { // If already initialized (it can occur if tab change - and so clear - happened)
		// Add all actors to the renderers again
		vtkRenderer* renderer = vtkFreehandController::GetInstance()->GetCanvasRenderer();
		renderer->AddActor(m_ContourActor);
		renderer->Modified();
	}
}

//-----------------------------------------------------------------------------

PlusStatus VolumeReconstructionController::Clear()
{
	LOG_TRACE("VolumeReconstructionController::Clear");

	// Remove all actors from the renderers
	vtkRenderer* renderer = vtkFreehandController::GetInstance()->GetCanvasRenderer();
	renderer->RemoveActor(m_ContourActor);
	renderer->Modified();

	m_Toolbox->Clear();

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus VolumeReconstructionController::DoAcquisition()
{
	LOG_TRACE("VolumeReconstructionController::DoAcquisition");

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus VolumeReconstructionController::Start()
{
	LOG_TRACE("VolumeReconstructionController::Start");

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus VolumeReconstructionController::Stop()
{
	LOG_TRACE("VolumeReconstructionController::Stop");

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

int VolumeReconstructionController::GetProgressPercent()
{
	return m_ProgressPercent;
}

//-----------------------------------------------------------------------------

std::string VolumeReconstructionController::GetProgressMessage()
{
	return m_ProgressMessage;
}

//-----------------------------------------------------------------------------

bool VolumeReconstructionController::GetVolumeReconstructionConfigFileLoaded()
{
	return m_VolumeReconstructionConfigFileLoaded;
}

//-----------------------------------------------------------------------------

vtkImageData* VolumeReconstructionController::GetReconstructedVolume()
{
	return m_ReconstructedVolume;
}

//-----------------------------------------------------------------------------

void VolumeReconstructionController::SetContouringThreshold(double aThreshold)
{
	LOG_TRACE("VolumeReconstructionController::SetContouringThreshold(" << aThreshold << ")"); 

	m_ContouringThreshold = aThreshold;
}

//-----------------------------------------------------------------------------

PlusStatus VolumeReconstructionController::LoadVolumeReconstructionConfigFromFile(std::string aFile)
{
	LOG_TRACE("VolumeReconstructionController::LoadVolumeReconstructionConfigFromFile(" << aFile << ")");

	bool success = m_VolumeReconstructor->ReadConfiguration(aFile.c_str());

	m_VolumeReconstructionConfigFileLoaded = success;

	return (success ? PLUS_SUCCESS : PLUS_FAIL);
}

//-----------------------------------------------------------------------------

PlusStatus VolumeReconstructionController::ReconstructVolumeFromInputImage(std::string aInputImage)
{
	LOG_TRACE("VolumeReconstructionController::ReconstructVolumeFromInputImage(" << aInputImage << ")");

	m_State = ToolboxState_InProgress;

	// Read image
	m_ProgressMessage = " Reading image sequence...";
	m_ProgressPercent = 0;
	vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
	trackedFrameList->ReadFromSequenceMetafile(aInputImage.c_str()); 
	
	// Set the input frame parameters for volume reconstruction
	m_VolumeReconstructor->SetNumberOfFrames( trackedFrameList->GetNumberOfTrackedFrames() ); 
	m_VolumeReconstructor->SetFrameSize( trackedFrameList->GetFrameSize() ); 
    m_VolumeReconstructor->SetNumberOfBitsPerPixel( trackedFrameList->GetNumberOfBitsPerPixel() ); 
	
	m_VolumeReconstructor->Initialize(); 
  
	// Feed images to the reconstructor	
	std::ostringstream osTransformImageToTool; 
	m_VolumeReconstructor->GetImageToToolTransform()->GetMatrix()->Print( osTransformImageToTool );
	LOG_DEBUG("Image to tool (probe calibration) transform: \n" << osTransformImageToTool.str());  
	
	const int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames(); 
	for ( int imgNumber = 0; imgNumber < numberOfFrames; ++imgNumber ) {
		// Set progress
		m_ProgressPercent = (int)((100.0 * imgNumber) / numberOfFrames + 0.49);
  		if (m_Toolbox) {
			m_Toolbox->RefreshToolboxContent();
		}

		vtkSmartPointer<vtkMatrix4x4> mToolToReference = vtkSmartPointer<vtkMatrix4x4>::New();
		double tToolToReference[16]; 
		if ( trackedFrameList->GetTrackedFrame(imgNumber)->GetDefaultFrameTransform(tToolToReference) ) {
			mToolToReference->DeepCopy(tToolToReference); 
		} else {
			LOG_ERROR("Unable to get default frame transform for frame #" << imgNumber); 
			continue; 
		}
    
		// Get Timestamp
		double timestamp(0); 
		const char* strTimestamp = trackedFrameList->GetTrackedFrame(imgNumber)->GetCustomFrameField("Timestamp"); 
		if ( strTimestamp == NULL )  {
			timestamp = imgNumber + 1;  // Just to make sure its increasing and not zero. This is not a normal case.
		} else {
			timestamp = atof(strTimestamp); 
		}
		
		// Add each tracked frame to reconstructor - US image orientation always MF in tracked frame list
		m_VolumeReconstructor->AddTrackedFrame(trackedFrameList->GetTrackedFrame(imgNumber)->ImageData, US_IMG_ORIENT_MF, mToolToReference, timestamp );
	}
	
	m_ProgressPercent = 0;
	if (m_Toolbox) {
		m_Toolbox->RefreshToolboxContent();
	}

	trackedFrameList->Clear(); 
  
	// Reconstruct
	m_ProgressMessage = " Reconstructing...";
	m_VolumeReconstructor->StartReconstruction(); 

	while ( !m_VolumeReconstructor->GetReconstructor()->GetReconstructionFinished() ) {
		m_ProgressPercent = (int)( ( 1 - ( 1.0 * m_VolumeReconstructor->GetReconstructor()->ReconstructionFrameCount / numberOfFrames )) * 100 + 0.49);
  		if (m_Toolbox) {
			m_Toolbox->RefreshToolboxContent();
		}

		vtksys::SystemTools::Delay(200); 
	}

	m_ProgressPercent = 0;
	m_ProgressMessage = " Filling holes in output volume...";
	if (m_Toolbox) {
		m_Toolbox->RefreshToolboxContent();
	}

	m_VolumeReconstructor->FillHoles(); 


	// Extract the 0th component
	vtkSmartPointer<vtkImageExtractComponents> extract = vtkSmartPointer<vtkImageExtractComponents>::New();
	extract->SetComponents(0);
	extract->SetInput(m_VolumeReconstructor->GetReconstructor()->GetOutputFromPort(0));

	m_ReconstructedVolume = vtkImageData::New();
	m_ReconstructedVolume = extract->GetOutput();

	m_ProgressPercent = 0;

	// Display result
	DisplayReconstructedVolume();

	m_ProgressPercent = 100;
	if (m_Toolbox) {
		m_Toolbox->RefreshToolboxContent();
	}

	m_State = ToolboxState_Done;

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void VolumeReconstructionController::DisplayReconstructedVolume()
{
	LOG_TRACE("VolumeReconstructionController::DisplayReconstructedVolume"); 

	if (vtkFreehandController::GetInstance()->GetCanvas() != NULL) {
		m_ProgressMessage = " Generating contour for displaying...";
		if (m_Toolbox) {
			m_Toolbox->RefreshToolboxContent();
		}

		vtkSmartPointer<vtkMarchingContourFilter> contourFilter = vtkSmartPointer<vtkMarchingContourFilter>::New();
		contourFilter->SetInput(m_ReconstructedVolume);
		contourFilter->SetValue(0, m_ContouringThreshold);

		vtkSmartPointer<vtkPolyDataMapper> contourMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		contourMapper->SetInputConnection(contourFilter->GetOutputPort());

		m_ContourActor->SetMapper(contourMapper);
		m_ContourActor->GetProperty()->SetColor(0.0, 0.0, 1.0);

		vtkFreehandController::GetInstance()->GetCanvasRenderer()->ResetCamera();
	}
}

//-----------------------------------------------------------------------------

PlusStatus VolumeReconstructionController::SaveVolumeToFile(std::string aOutput)
{
	LOG_TRACE("VolumeReconstructionController::SaveVolumeToFile(" << aOutput << ")"); 

	// Write out to file
	vtkSmartPointer<vtkDataSetWriter> writer = vtkSmartPointer<vtkDataSetWriter>::New();
	writer->SetFileTypeToBinary();
	writer->SetInput(m_ReconstructedVolume);
	writer->SetFileName(aOutput.c_str());
	writer->Update();

	return PLUS_SUCCESS;
}
