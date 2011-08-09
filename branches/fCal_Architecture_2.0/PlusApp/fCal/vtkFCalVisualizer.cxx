#include "vtkFCalVisualizer.h"

#include "vtkTrackerTool.h"

#include "vtkSTLReader.h"
#include "vtkPolyDataMapper.h"
#include "vtkGlyph3D.h"
#include "vtkSphereSource.h"
#include "vtkProperty.h"
#include "vtkTransformPolyDataFilter.h"

#include <QTimer>

//-----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkFCalVisualizer, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkFCalVisualizer);

//----------------------------------------------------------------------------
void vtkFCalVisualizer::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
} 

//-----------------------------------------------------------------------------

vtkFCalVisualizer::vtkFCalVisualizer()
{
  this->Canvas = NULL;
	this->CanvasRenderer = NULL;
	this->CanvasImageActor = NULL;
	this->PhantomBodyActor = NULL;
	this->ProbeActor = NULL;
	this->StylusActor = NULL;
	this->NeedleActor = NULL;
	this->SegmentedPointsActor = NULL;
	this->SegmentedPointsPolyData = NULL;
	this->ImageCamera = NULL;

	this->SetActiveToolbox(FCAL_TOOLBOX_UNDEFINED);

	VTK_LOG_TO_CONSOLE_ON
}

//-----------------------------------------------------------------------------

vtkFCalVisualizer::~vtkFCalVisualizer()
{
  this->SetCanvas(NULL);
  this->SetCanvasRenderer(NULL);
	this->SetCanvasImageActor(NULL);
	this->SetPhantomBodyActor(NULL);
	this->SetProbeActor(NULL);
	this->SetStylusActor(NULL);
	this->SetNeedleActor(NULL);
	this->SetSegmentedPointsActor(NULL);
	this->SetSegmentedPointsPolyData(NULL);
	this->SetImageCamera(NULL);

  this->AcquisitionTimer = NULL;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFCalVisualizer::Initialize()
{
  LOG_TRACE("vtkFCalVisualizer::Initialize"); 

	if (this->Initialized) {
		return PLUS_SUCCESS;
	}

	// Create timer
	this->AcquisitionTimer = new QTimer();

  vtkSmartPointer<vtkFCalController> fCalController = vtkSmartPointer<FCalController>::New();
  this->SetFCalController(fCalController);

	this->SetInitialized(true);

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFCalVisualizer::InitializeVisualization()
{
	LOG_TRACE("vtkFCalVisualizer::InitializeVisualization"); 

  if (this->Canvas == NULL) {
    LOG_ERROR("Canvas is not initialized!");
    return PLUS_FAIL;
  }

  // Set up canvas renderer
	vtkSmartPointer<vtkRenderer> canvasRenderer = vtkSmartPointer<vtkRenderer>::New(); 
	canvasRenderer->SetBackground(0.1, 0.1, 0.1);
  canvasRenderer->SetBackground2(255/255.0, 235/255.0, 158/255.0);
  canvasRenderer->SetGradientBackground(true);
	this->SetCanvasRenderer(canvasRenderer);

  // Initialize canvas image actor
	vtkSmartPointer<vtkImageActor> canvasImageActor = vtkSmartPointer<vtkImageActor>::New();
	this->SetCanvasImageActor(canvasImageActor);

  if (this->FCalController->DataCollector->GetAcquisitionType() != SYNCHRO_VIDEO_NONE) {
    this->CanvasImageActor->VisibilityOn();
	  this->CanvasImageActor->SetInput(dataCollector->GetOutput());
  } else {
	  LOG_WARNING("Data collector has no output port, canvas image actor initalization failed.");
  }

  // Create segmented points actor
	vtkSmartPointer<vtkActor> segmentedPointsActor = vtkSmartPointer<vtkActor>::New();

	vtkSmartPointer<vtkPolyData> segmentedPointsPolyData = vtkSmartPointer<vtkPolyData>::New();
	segmentedPointsPolyData->Initialize();
	this->SetSegmentedPointsPolyData(segmentedPointsPolyData);

	vtkSmartPointer<vtkPolyDataMapper> segmentedPointMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkGlyph3D> segmentedPointGlyph = vtkSmartPointer<vtkGlyph3D>::New();
	vtkSmartPointer<vtkSphereSource> segmentedPointSphereSource = vtkSmartPointer<vtkSphereSource>::New();
	segmentedPointSphereSource->SetRadius(4.0);

	segmentedPointGlyph->SetInputConnection(this->SegmentedPointsPolyData->GetProducerPort());
	segmentedPointGlyph->SetSourceConnection(segmentedPointSphereSource->GetOutputPort());
	segmentedPointMapper->SetInputConnection(segmentedPointGlyph->GetOutputPort());

	segmentedPointsActor->SetMapper(segmentedPointMapper);
	segmentedPointsActor->GetProperty()->SetColor(0.0, 0.8, 0.0);
	segmentedPointsActor->VisibilityOff();

	this->SetSegmentedPointsActor(segmentedPointsActor);
	
	// Add actors to the realtime renderer, and add renderer to Canvas
  this->CanvasRenderer->AddActor(this->CanvasImageActor);
	this->CanvasRenderer->AddActor(this->SegmentedPointsActor);
	this->CanvasRenderer->InteractiveOff(); // TODO it doesn't work - find a way to disable interactions (also re-enable on Clear)
	this->CanvasRenderer->Modified();

  if (this->InitializeToolVisualization() != PLUS_SUCCESS) {
    LOG_ERROR("Tool visualization failed!");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFCalVisualizer::InitializeToolVisualization()
{
	LOG_TRACE("vtkFCalVisualizer::InitializeToolVisualization");

	if ((this->DataCollector == NULL) || (this->DataCollector->GetTracker() == NULL)) {
		LOG_ERROR("Data collector is not initialized!");
		return PLUS_FAIL;
	}
  if (this->Canvas == NULL) {
    LOG_ERROR("Canvas is not initialized!");
    return PLUS_FAIL;
  }

  // Load phantom model and create phantom body actor
  if ((vtkFreehandCalibrationController::GetInstance()->GetModelToPhantomTransform() != NULL) && (PhantomRegistrationController::GetInstance()->GetPhantomToPhantomReferenceTransform() != NULL)) {
		// Initialize phantom model visualization
		if (! vtksys::SystemTools::FileExists(vtkFreehandCalibrationController::GetInstance()->GetPhantomModelFileName())) {
			LOG_WARNING("Phantom model file is not found with the specified name: " << vtkFreehandCalibrationController::GetInstance()->GetPhantomModelFileName());
		} else {
			vtkSmartPointer<vtkSTLReader> stlReader = vtkSmartPointer<vtkSTLReader>::New();
			stlReader->SetFileName(vtkFreehandCalibrationController::GetInstance()->GetPhantomModelFileName());
			
			vtkSmartPointer<vtkTransform> phantomModelToPhantomReferenceTransform = vtkSmartPointer<vtkTransform>::New();
			phantomModelToPhantomReferenceTransform->Identity();
			phantomModelToPhantomReferenceTransform->Concatenate(PhantomRegistrationController::GetInstance()->GetPhantomToPhantomReferenceTransform());
			phantomModelToPhantomReferenceTransform->Concatenate(vtkFreehandCalibrationController::GetInstance()->GetModelToPhantomTransform());
			phantomModelToPhantomReferenceTransform->Modified();

			vtkSmartPointer<vtkTransformPolyDataFilter> phantomModelToPhantomReferenceTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
			phantomModelToPhantomReferenceTransformFilter->AddInputConnection(stlReader->GetOutputPort());
			phantomModelToPhantomReferenceTransformFilter->SetTransform(phantomModelToPhantomReferenceTransform);

			vtkSmartPointer<vtkPolyDataMapper> phantomMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
			phantomMapper->SetInputConnection(phantomModelToPhantomReferenceTransformFilter->GetOutputPort());
			vtkSmartPointer<vtkActor> phantomBodyActor = vtkSmartPointer<vtkActor>::New();
			phantomBodyActor->SetMapper(phantomMapper);
      phantomBodyActor->GetProperty()->SetOpacity(0.6);
			this->SetPhantomBodyActor(phantomBodyActor);
		}
	} else {
		LOG_WARNING("Phantom definiton file or its related transforms are not specified, phantom will not be displayed");
	}

	// Load device models and create actors
	for (int i=0; i<this->DataCollector->GetTracker()->GetNumberOfTools(); ++i) {
		vtkTrackerTool *tool = this->DataCollector->GetTracker()->GetTool(i);
		if ((tool == NULL) || (!tool->GetEnabled())) {
			continue;
		}

		vtkSmartPointer<vtkActor> deviceActor = vtkSmartPointer<vtkActor>::New();

		// Set proper members
		if (tool->GetToolType() == TRACKER_TOOL_STYLUS) {
			StylusCalibrationController::GetInstance()->LoadStylusModel(deviceActor);

			this->SetStylusActor(deviceActor);

		} else {
			// Load model if file name exists and file can be found
			if (STRCASECMP(tool->GetTool3DModelFileName(), "") != 0) {
				std::string searchResult = vtkFileFinder::GetFirstFileFoundInConfigurationDirectory(tool->GetTool3DModelFileName());

				if (STRCASECMP("", searchResult.c_str()) == 0) {
					LOG_WARNING("Tool (" << tool->GetToolName() << ") model file is not found with name: " << tool->GetTool3DModelFileName());
				} else {
					vtkSmartPointer<vtkSTLReader> stlReader = vtkSmartPointer<vtkSTLReader>::New();
					stlReader->SetFileName(searchResult.c_str());

					// TODO Try to use this filter instead of always setting all these transforms on every acquisition
          /*
					vtkSmartPointer<vtkTransform> toolModelToToolReferenceTransform = vtkSmartPointer<vtkTransform>::New();
					toolModelToToolReferenceTransform->Identity();
					toolModelToToolReferenceTransform->Concatenate(tool->GetToolToToolReferenceTransform());
					toolModelToToolReferenceTransform->Concatenate(tool->GetModelToToolTransform());
					toolModelToToolReferenceTransform->Modified();

					vtkSmartPointer<vtkTransformPolyDataFilter> toolModelToToolReferenceTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
					toolModelToToolReferenceTransformFilter->AddInputConnection(stlReader->GetOutputPort());
					toolModelToToolReferenceTransformFilter->SetTransform(toolModelToToolReferenceTransform);
          */

					vtkSmartPointer<vtkPolyDataMapper> toolMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
					//toolMapper->SetInputConnection(toolModelToToolReferenceTransformFilter->GetOutputPort());
					toolMapper->SetInputConnection(stlReader->GetOutputPort());

					deviceActor->SetMapper(toolMapper);

					if (tool->GetToolType() == TRACKER_TOOL_PROBE) {
						this->SetProbeActor(deviceActor);

					} else if (tool->GetToolType() == TRACKER_TOOL_NEEDLE) {
						this->SetNeedleActor(deviceActor);
					}
				}
			}
		}
	} // for each tool

	if (this->ProbeActor == NULL) {
		LOG_ERROR("Initializing probe visualization failed!");
		return PLUS_FAIL;
	}

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFCalVisualizer::DisplayTools()
{
	LOG_TRACE("vtkFCalVisualizer::DisplayTools");

	if (this->DataCollector == NULL) {
		LOG_ERROR("Data collector is not initialized!");
		return PLUS_FAIL;
	}
	if (this->DataCollector->GetTracker() == NULL) {
		LOG_ERROR("Tracker is not initialized properly!");
		return PLUS_FAIL;
	}
	int firstActiveToolNumber = -1; 
	if (this->DataCollector->GetTracker()->GetFirstActiveTool(firstActiveToolNumber) != PLUS_SUCCESS)
	{
		LOG_ERROR("There are no active tracker tools!"); 
		return PLUS_FAIL; 
	}

	int referenceToolNumber = this->DataCollector->GetTracker()->GetReferenceToolNumber();
	bool resetCameraNeeded = false;

	for (int toolNumber=0; toolNumber < this->DataCollector->GetTracker()->GetNumberOfTools(); ++toolNumber) {
		// If reference then no need for setting transform (the phantom is fixed to the reference)
		if (toolNumber == referenceToolNumber) {
			continue;
		}

		vtkTrackerTool *tool = this->DataCollector->GetTracker()->GetTool(toolNumber);
		if ((tool == NULL) || (!tool->GetEnabled())) {
			continue;
		}

		TrackerStatus status = TR_MISSING;
		double timestamp;

		vtkSmartPointer<vtkMatrix4x4> toolToReferenceTransformMatrix = NULL;

		// Acquire position from tracker
		toolToReferenceTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
		this->DataCollector->GetTransformWithTimestamp(toolToReferenceTransformMatrix, timestamp, status, toolNumber); 

		// Compute and set transforms for actors
		if (status == TR_OK) {
			// If other tool, set the transform according to the tool name
      if ((this->ProbeActor != NULL) && (tool->GetToolType() == TRACKER_TOOL_PROBE)) {
				if (this->ProbeActor->GetProperty()->GetOpacity() == 0.9) {
					resetCameraNeeded = true;
				}

        // Probe transform
				vtkSmartPointer<vtkTransform> probeModelToPhantomReferenceTransform = vtkSmartPointer<vtkTransform>::New();
				probeModelToPhantomReferenceTransform->Identity();
				probeModelToPhantomReferenceTransform->Concatenate(toolToReferenceTransformMatrix);
        probeModelToPhantomReferenceTransform->Concatenate(tool->GetModelToToolTransform());

        this->ProbeActor->GetProperty()->SetOpacity(1.0);
				this->ProbeActor->SetUserTransform(probeModelToPhantomReferenceTransform);

				// Image canvas transform
				vtkSmartPointer<vtkTransform> imageToPhantomReferenceTransform = vtkSmartPointer<vtkTransform>::New();
				imageToPhantomReferenceTransform->Identity();
				imageToPhantomReferenceTransform->Concatenate(toolToReferenceTransformMatrix);
        imageToPhantomReferenceTransform->Concatenate(vtkFreehandCalibrationController::GetInstance()->GetTransformImageToProbe());
				
        this->CanvasImageActor->SetOpacity(1.0);
				this->CanvasImageActor->SetUserTransform(imageToPhantomReferenceTransform);

			} else {

				// Create and apply actor transform
				vtkSmartPointer<vtkTransform> toolModelToPhantomReferenceTransform = vtkSmartPointer<vtkTransform>::New();
				toolModelToPhantomReferenceTransform->Identity();
				toolModelToPhantomReferenceTransform->Concatenate(toolToReferenceTransformMatrix);
				toolModelToPhantomReferenceTransform->Concatenate(tool->GetCalibrationMatrix());
				toolModelToPhantomReferenceTransform->Concatenate(tool->GetModelToToolTransform());
				toolModelToPhantomReferenceTransform->Modified();

				if ((this->StylusActor != NULL) && (tool->GetToolType() == TRACKER_TOOL_STYLUS)) {
					this->StylusActor->GetProperty()->SetOpacity(1.0);
					this->StylusActor->SetUserTransform(toolModelToPhantomReferenceTransform);

        } else if ((this->NeedleActor != NULL) && (tool->GetToolType() == TRACKER_TOOL_NEEDLE)) {
					this->NeedleActor->GetProperty()->SetOpacity(1.0);
					this->NeedleActor->SetUserTransform(toolModelToPhantomReferenceTransform);
				}
			}
		} else {
			if ((this->ProbeActor != NULL) && (tool->GetToolType() == TRACKER_TOOL_PROBE)) {
				this->ProbeActor->GetProperty()->SetOpacity(0.3);
        this->CanvasImageActor->SetOpacity(0.3);

			} else if ((this->StylusActor != NULL) && (tool->GetToolType() == TRACKER_TOOL_STYLUS)) {
				this->StylusActor->GetProperty()->SetOpacity(0.3);

			} else if ((this->NeedleActor != NULL) && (tool->GetToolType() == TRACKER_TOOL_NEEDLE)) {
				this->NeedleActor->GetProperty()->SetOpacity(0.3);
			}
		}
	} // for each tool

	if (resetCameraNeeded) {
		this->CanvasRenderer->ResetCamera();
	}

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

AbstractToolboxController* vtkFCalVisualizer::GetActiveToolboxController()
{
	LOG_TRACE("vtkFCalVisualizer::GetActiveToolboxController");

  switch (this->ActiveToolbox) {

		case FCAL_TOOLBOX_CONFIGURATION:
			return ConfigurationController::GetInstance();

		case FCAL_TOOLBOX_STYLUS_CALIBRATION:
			return StylusCalibrationController::GetInstance();

		case FCAL_TOOLBOX_PHANTOM_REGISTRATION:
			return PhantomRegistrationController::GetInstance();

		case FCAL_TOOLBOX_FREEHAND_CALIBRATION:
			return vtkFreehandCalibrationController::GetInstance();

		case FCAL_TOOLBOX_VOLUME_RECONSTRUCTION:
			return VolumeReconstructionController::GetInstance();

		default:
			LOG_ERROR("No toolbox of this type found!");
			return NULL;
	}
}

//-----------------------------------------------------------------------------

PlusStatus vtkFCalVisualizer::SetActiveToolbox(FCAL_TOOLBOX_TYPE aToolboxType)
{
	LOG_TRACE("vtkFCalVisualizer::SetActiveToolbox(" << aToolboxType << ")");

  this->ActiveToolbox = aToolboxType;

  // Change visualization according to the active toolbox
	switch (this->ActiveToolbox) {

		case FCAL_TOOLBOX_CONFIGURATION:
  		this->TrackingOnlyOn();

			break;

		case FCAL_TOOLBOX_STYLUS_CALIBRATION:
  		this->TrackingOnlyOn();

			break;

		case FCAL_TOOLBOX_PHANTOM_REGISTRATION:
  		this->TrackingOnlyOn();

			break;

		case FCAL_TOOLBOX_FREEHAND_CALIBRATION:
  		this->TrackingOnlyOff();

			break;

		case FCAL_TOOLBOX_VOLUME_RECONSTRUCTION:
  		this->TrackingOnlyOff();

			break;

		default:
			LOG_ERROR("No toolbox of this type found!");
			return PLUS_FAIL;
	}

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkFCalVisualizer::Start(FCAL_TOOLBOX_TYPE aToolboxType)
{
	// Reset polydatas (make it look like empty)
	this->FCalController->GetStylusCalibrationAlgo()->GetInputPolyData->GetPoints()->Reset();
	this->FCalController->GetStylusCalibrationAlgo()->GetStylusTipPolyData->GetPoints()->Reset(); //TODO make this just a point and create the polydata here?

  // Show input points
  if (this->InputActor != NULL) {
    this->InputActor->VisibilityOn();
  }

	// Remove stylus tip actor
	this->CanvasRenderer->RemoveActor(m_StylusTipActor);
	this->CanvasRenderer->RemoveActor(m_StylusActor);
}

//-----------------------------------------------------------------------------

PlusStatus vtkFCalVisualizer::Stop(FCAL_TOOLBOX_TYPE aToolboxType)
{
}

//-----------------------------------------------------------------------------

PlusStatus vtkFCalVisualizer::Clear(FCAL_TOOLBOX_TYPE aToolboxType)
{
}

//-----------------------------------------------------------------------------

PlusStatus vtkFCalVisualizer::LoadStylusModel(vtkActor* aActor)
{
	LOG_TRACE("vtkFCalVisualizer::LoadStylusModel");

	vtkDataCollector* dataCollector = vtkFreehandController::GetInstance()->GetDataCollector();
	if (dataCollector == NULL) {
		LOG_ERROR("Data collector is not initialized!");
		return PLUS_FAIL;
	}
	if (dataCollector->GetTracker() == NULL) {
		LOG_ERROR("Tracker is not initialized properly!");
		return PLUS_FAIL;
	}
	if (m_StylusPortNumber < 0) {
    if (DetermineStylusPortNumber() != PLUS_SUCCESS) {
      LOG_ERROR("Unable to determine stylus port number!");
      return PLUS_FAIL;
    }
	}
	vtkTrackerTool *tool = dataCollector->GetTracker()->GetTool(m_StylusPortNumber);
	if ((tool == NULL) || (!tool->GetEnabled())) {
		return PLUS_FAIL;
	}

	if (aActor == NULL) {
		aActor = m_StylusActor;
	}

	std::string modelFileName = tool->GetTool3DModelFileName();

	std::string searchResult = "";
	if ((modelFileName != "") && (STRCASECMP(vtkFileFinder::GetInstance()->GetConfigurationDirectory(), "") != 0)) {
		searchResult = vtkFileFinder::GetFirstFileFoundInConfigurationDirectory(modelFileName.c_str());
	}

	if (searchResult != "") {
		// Load model and transform it to the tool reference (stylus) coordinate system
		vtkSmartPointer<vtkSTLReader> stlReader = vtkSmartPointer<vtkSTLReader>::New();
		stlReader->SetFileName(searchResult.c_str());

		// TODO Try to use this filter instead of always setting all these transforms on every acquisition
		//vtkSmartPointer<vtkTransform> stylusModelToStylusReferenceTransform = vtkSmartPointer<vtkTransform>::New();
		//stylusModelToStylusReferenceTransform->Identity();
		//stylusModelToStylusReferenceTransform->Concatenate(tool->GetToolToToolReferenceTransform());
		//stylusModelToStylusReferenceTransform->Concatenate(tool->GetModelToToolTransform());
		//stylusModelToStylusReferenceTransform->Modified();

		//vtkSmartPointer<vtkTransformPolyDataFilter> stylusModelToStylusReferenceTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		//stylusModelToStylusReferenceTransformFilter->AddInputConnection(stlReader->GetOutputPort());
		//stylusModelToStylusReferenceTransformFilter->SetTransform(stylusModelToStylusReferenceTransform);

		vtkSmartPointer<vtkPolyDataMapper> stylusMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		//stylusMapper->SetInputConnection(stylusModelToStylusReferenceTransformFilter->GetOutputPort());
		stylusMapper->SetInputConnection(stlReader->GetOutputPort());
		aActor->SetMapper(stylusMapper);
		aActor->GetProperty()->SetColor(0.0, 0.0, 0.0);

	} else {
		// Create default model
		vtkSmartPointer<vtkPolyDataMapper> stylusMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		vtkSmartPointer<vtkCylinderSource> stylusBigCylinderSource = vtkSmartPointer<vtkCylinderSource>::New();
		stylusBigCylinderSource->SetRadius(3.0); // mm
		stylusBigCylinderSource->SetHeight(120.0); // mm
		stylusBigCylinderSource->SetCenter(0.0, 150.0, 0.0);
		vtkSmartPointer<vtkCylinderSource> stylusSmallCylinderSource = vtkSmartPointer<vtkCylinderSource>::New();
		stylusSmallCylinderSource->SetRadius(1.5); // mm
		stylusSmallCylinderSource->SetHeight(80.0); // mm
		stylusSmallCylinderSource->SetCenter(0.0, 50.0, 0.0);
		vtkSmartPointer<vtkConeSource> stylusTipConeSource = vtkSmartPointer<vtkConeSource>::New();
		stylusTipConeSource->SetRadius(1.5); // mm
		stylusTipConeSource->SetHeight(10.0); //mm
		vtkSmartPointer<vtkTransform> coneTransform = vtkSmartPointer<vtkTransform>::New();
		coneTransform->Identity();
		coneTransform->RotateZ(-90.0);
		coneTransform->Translate(-5.0, 0.0, 0.0);
		vtkSmartPointer<vtkTransformPolyDataFilter> coneTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		coneTransformFilter->AddInputConnection(stylusTipConeSource->GetOutputPort());
		coneTransformFilter->SetTransform(coneTransform);

		vtkSmartPointer<vtkAppendPolyData> appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();
		appendFilter->AddInputConnection(stylusBigCylinderSource->GetOutputPort());
		appendFilter->AddInputConnection(stylusSmallCylinderSource->GetOutputPort());
		appendFilter->AddInputConnection(coneTransformFilter->GetOutputPort());
		vtkSmartPointer<vtkTransform> stylusTransform = vtkSmartPointer<vtkTransform>::New();
		stylusTransform->Identity();
		stylusTransform->RotateZ(90.0);
		vtkSmartPointer<vtkTransformPolyDataFilter> stylusTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		stylusTransformFilter->AddInputConnection(appendFilter->GetOutputPort());
		stylusTransformFilter->SetTransform(stylusTransform);

		stylusMapper->SetInputConnection(stylusTransformFilter->GetOutputPort());
		aActor->SetMapper(stylusMapper);
		aActor->GetProperty()->SetColor(0.0, 0.0, 0.0);
	}

	return PLUS_SUCCESS;
}
