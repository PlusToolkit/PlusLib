#include "PlusConfigure.h"

#include "vtkToolVisualizer.h"

#include "vtkObjectFactory.h"
#include "vtkDirectory.h"
#include "vtkXMLUtilities.h"

#include "vtkMath.h"
#include "vtkPolyDataMapper.h"
#include "vtkGlyph3D.h"
#include "vtkSphereSource.h"
#include "vtkCylinderSource.h"
#include "vtkConeSource.h"
#include "vtkProperty.h"
#include "vtkAppendPolyData.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkSTLReader.h"
#include "vtkAxesActor.h"
#include "vtkRenderWindow.h"

#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleTrackballCamera.h"

#include <QTimer>

//-----------------------------------------------------------------------------

vtkStandardNewMacro(vtkDisplayableTool);
vtkStandardNewMacro(vtkToolVisualizer);

//-----------------------------------------------------------------------------

vtkToolVisualizer::vtkToolVisualizer()
{
	this->DataCollector = NULL;
	this->AcquisitionFrameRate = 20;
	this->InitializedOff();
	this->ImageModeOff();

	this->CanvasRenderer = NULL;
	this->InputPointsPolyData = NULL;
	this->InputPointsActor = NULL;
	this->ResultPointsPolyData = NULL;
	this->ResultPointsActor = NULL;
  this->ImageActor = NULL;
	this->ImageCamera = NULL;

  this->DisplayableToolVector.resize(TRACKER_TOOL_GENERAL + 1, NULL);

  this->ImageToProbeTransform = NULL;

	// Create timer
  this->AcquisitionTimer = NULL;

}

//-----------------------------------------------------------------------------

vtkToolVisualizer::~vtkToolVisualizer()
{
	if (this->AcquisitionTimer != NULL) {
    disconnect( this->AcquisitionTimer, SIGNAL( timeout() ), this, SLOT( DisplayDevices() ) );
    this->AcquisitionTimer->stop();
		delete this->AcquisitionTimer;
		this->AcquisitionTimer = NULL;
	} 
  
  if (this->DataCollector != NULL) {
		this->DataCollector->Stop();
	}
	this->SetDataCollector(NULL);

	this->SetCanvasRenderer(NULL);
	this->SetImageActor(NULL);
	this->SetImageCamera(NULL);

  for (std::vector<vtkDisplayableTool*>::iterator it = this->DisplayableToolVector.begin(); it != this->DisplayableToolVector.end(); ++it) {
    if ((*it) != NULL) {
      (*it)->Delete();
      (*it) = NULL;
    }
  }
  this->DisplayableToolVector.clear();

  this->SetImageToProbeTransform(NULL);
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::Initialize()
{
	LOG_TRACE("vtkToolVisualizer::Initialize"); 

	if (this->Initialized) {
		return PLUS_SUCCESS;
	}

	// Set up canvas renderer
	vtkSmartPointer<vtkRenderer> canvasRenderer = vtkSmartPointer<vtkRenderer>::New(); 
	canvasRenderer->SetBackground(0.1, 0.1, 0.1);
  canvasRenderer->SetBackground2(0.4, 0.4, 0.4);
  canvasRenderer->SetGradientBackground(true);
	this->SetCanvasRenderer(canvasRenderer);

	// Initialize visualization
	if (InitializeVisualization() != PLUS_SUCCESS) {
    LOG_ERROR("Initializing visualization failed!");
    return PLUS_FAIL;
  }

  // Initialize timer
	this->AcquisitionTimer = new QTimer();
	this->AcquisitionTimer->start(1000.0 / this->AcquisitionFrameRate);

  connect( this->AcquisitionTimer, SIGNAL( timeout() ), this, SLOT( DisplayDevices() ) );

  this->SetInitialized(true);

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::InitializeVisualization()
{
	LOG_TRACE("vtkToolVisualizer::InitializeVisualization");

	// Input points poly data
  vtkSmartPointer<vtkPolyData> inputPointsPolyData = vtkSmartPointer<vtkPolyData>::New();
	inputPointsPolyData->Initialize();
	vtkSmartPointer<vtkPoints> inputPoints = vtkSmartPointer<vtkPoints>::New();
	inputPointsPolyData->SetPoints(inputPoints);
	this->SetInputPointsPolyData(inputPointsPolyData);

	// Input points actor
  vtkSmartPointer<vtkActor> inputPointsActor = vtkSmartPointer<vtkActor>::New();
	vtkSmartPointer<vtkPolyDataMapper> inputPointsMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkGlyph3D> inputPointsGlyph = vtkSmartPointer<vtkGlyph3D>::New();
	vtkSmartPointer<vtkSphereSource> inputPointsSphereSource = vtkSmartPointer<vtkSphereSource>::New();
	inputPointsSphereSource->SetRadius(1.5); // mm

	inputPointsGlyph->SetInputConnection(this->InputPointsPolyData->GetProducerPort());
	inputPointsGlyph->SetSourceConnection(inputPointsSphereSource->GetOutputPort());
	inputPointsMapper->SetInputConnection(inputPointsGlyph->GetOutputPort());
	inputPointsActor->SetMapper(inputPointsMapper);
	inputPointsActor->GetProperty()->SetColor(0.0, 0.7, 1.0);
	this->SetInputPointsActor(inputPointsActor);


	// Result points poly data
  vtkSmartPointer<vtkPolyData> resultPointsPolyData = vtkSmartPointer<vtkPolyData>::New();
	resultPointsPolyData = vtkPolyData::New();
	resultPointsPolyData->Initialize();
	vtkSmartPointer<vtkPoints> resultPointsPoint = vtkSmartPointer<vtkPoints>::New();
	//resultPointsPoint->SetNumberOfPoints(1); // Makes the input actor disappear!
	resultPointsPolyData->SetPoints(resultPointsPoint);
	this->SetResultPointsPolyData(resultPointsPolyData);

	// Result points actor
  vtkSmartPointer<vtkActor> resultPointsActor = vtkSmartPointer<vtkActor>::New();
	vtkSmartPointer<vtkPolyDataMapper> resultPointsMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkGlyph3D> resultPointsGlyph = vtkSmartPointer<vtkGlyph3D>::New();
	vtkSmartPointer<vtkSphereSource> resultPointsSphereSource = vtkSmartPointer<vtkSphereSource>::New();
	resultPointsSphereSource->SetRadius(3.0); // mm

	resultPointsGlyph->SetInputConnection(this->ResultPointsPolyData->GetProducerPort());
	resultPointsGlyph->SetSourceConnection(resultPointsSphereSource->GetOutputPort());
	resultPointsMapper->SetInputConnection(resultPointsGlyph->GetOutputPort());
	resultPointsActor->SetMapper(resultPointsMapper);
	resultPointsActor->GetProperty()->SetColor(0.0, 0.8, 0.0);
	this->SetResultPointsActor(resultPointsActor);

  // Create image actor
  vtkSmartPointer<vtkImageActor> imageActor = vtkSmartPointer<vtkImageActor>::New();
	this->SetImageActor(imageActor);

  // Create initial transform objects
	vtkSmartPointer<vtkTransform> imageToProbeTransform = vtkSmartPointer<vtkTransform>::New();
  imageToProbeTransform->GetMatrix()->Zero(); // Image is not displayed until it is a valid matrix
  this->SetImageToProbeTransform(imageToProbeTransform);

	// Axes actor (CODE SNIPPET FOR DEBUGGING)
	//vtkSmartPointer<vtkAxesActor> axesActor = vtkSmartPointer<vtkAxesActor>::New();
	//axesActor->SetShaftTypeToCylinder();
	//axesActor->SetXAxisLabelText("X");
	//axesActor->SetYAxisLabelText("Y");
	//axesActor->SetZAxisLabelText("Z");
	//axesActor->SetAxisLabels(0);
	//axesActor->SetTotalLength(50, 50, 50);
	//this->CanvasRenderer->AddActor(axesActor);

  // Add non-tool actors to the renderer
	this->CanvasRenderer->AddActor(this->InputPointsActor);
	this->CanvasRenderer->AddActor(this->ResultPointsActor);
	this->CanvasRenderer->AddActor(this->ImageActor);

  // Hide all actors
  HideAll();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::InitializeDeviceVisualization()
{
	LOG_TRACE("vtkToolVisualizer::InitializeDeviceVisualization");

  // Delete displayable tools
  for (std::vector<vtkDisplayableTool*>::iterator it = this->DisplayableToolVector.begin(); it != this->DisplayableToolVector.end(); ++it) {
    if ((*it) != NULL) {
      if ((*it)->GetActor() != NULL) {
        this->CanvasRenderer->RemoveActor((*it)->GetActor());
      }

      (*it)->Delete();
      (*it) = NULL;
    }
  }

  // Connect data collector to image actor
	if (this->DataCollector->GetAcquisitionType() != SYNCHRO_VIDEO_NONE) {
		this->ImageActor->VisibilityOn();
		this->ImageActor->SetInput(this->DataCollector->GetOutput());
	} else {
		LOG_WARNING("Data collector has no video output, cannot initialize image actor");
	}

  // Load phantom model and registration from configuration and set up visualization
  if (InitializePhantomVisualization() == PLUS_FAIL) {
    LOG_WARNING("Initializing phantom visualization failed!");
  }

	// Load tool models
	for (int i=0; i < this->DataCollector->GetTracker()->GetNumberOfTools(); ++i) {
		vtkTrackerTool *tool = this->DataCollector->GetTracker()->GetTool(i);
		if ((tool == NULL) || (!tool->GetEnabled())) {
			continue;
		}

    vtkDisplayableTool* displayableTool = NULL;

		// Load model if file name exists and file can be found
		if (STRCASECMP(tool->GetTool3DModelFileName(), "") != 0) {
			std::string searchResult = vtkPlusConfig::GetFirstFileFoundInConfigurationDirectory(tool->GetTool3DModelFileName());

			if (STRCASECMP("", searchResult.c_str()) == 0) {
				LOG_WARNING("Tool (" << tool->GetToolName() << ") model file is not found with name: " << tool->GetTool3DModelFileName());
			} else {
				vtkSmartPointer<vtkSTLReader> stlReader = vtkSmartPointer<vtkSTLReader>::New();
				stlReader->SetFileName(searchResult.c_str());

				vtkSmartPointer<vtkPolyDataMapper> toolMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
				toolMapper->SetInputConnection(stlReader->GetOutputPort());

        displayableTool = vtkDisplayableTool::New();
				displayableTool->GetActor()->SetMapper(toolMapper);
        displayableTool->SetTool(tool);

				if (tool->GetToolType() == TRACKER_TOOL_STYLUS) { // Stylus is always black
          displayableTool->GetActor()->GetProperty()->SetColor(0.0, 0.0, 0.0);
				}

        this->DisplayableToolVector[tool->GetToolType()] = displayableTool;
        this->CanvasRenderer->AddActor(displayableTool->GetActor());
      }
		}

    // Handle missing tool models
	  if ((tool->GetToolType() == TRACKER_TOOL_PROBE) && (this->DisplayableToolVector[TRACKER_TOOL_PROBE] == NULL)) {
		  LOG_WARNING("Unable to initialize probe visualization - no probe is displayed");
	  } else
    if ((tool->GetToolType() == TRACKER_TOOL_STYLUS) && (this->DisplayableToolVector[TRACKER_TOOL_STYLUS] == NULL)) {
		  LOG_INFO("No stylus model file found - default model will be displayed");

      displayableTool = vtkDisplayableTool::New();
      displayableTool->SetTool(tool);
      SetDefaultStylusModel(displayableTool->GetActor());
      this->DisplayableToolVector[TRACKER_TOOL_STYLUS] = displayableTool;
      this->CanvasRenderer->AddActor(displayableTool->GetActor());
	  } else
	  if ((tool->GetToolType() == TRACKER_TOOL_NEEDLE) && (this->DisplayableToolVector[TRACKER_TOOL_NEEDLE] == NULL)) {
		  LOG_INFO("Unable to initialize needle visualization - no needle is displayed");
	  }

	} // for each tool

  // Hide all tools so that they don't appear next to the configuration toolbox
  HideAll();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::SetAcquisitionFrameRate(int aFrameRate)
{
	LOG_TRACE("vtkToolVisualizer::SetAcquisitionFrameRate(" << aFrameRate << ")");

  this->AcquisitionFrameRate = aFrameRate;

  if (this->AcquisitionTimer != NULL) {
    this->AcquisitionTimer->stop();
    this->AcquisitionTimer->start(1000.0 / this->AcquisitionFrameRate);
  } else {
    LOG_ERROR("Acquisition timer is not initialized!");
    return PLUS_FAIL;
  }

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::HideAll()
{
	LOG_TRACE("vtkToolVisualizer::HideAll");

	// Hide all actors from the renderer
	this->InputPointsActor->VisibilityOff();
	this->ResultPointsActor->VisibilityOff();
  this->ImageActor->VisibilityOff();

  for (std::vector<vtkDisplayableTool*>::iterator it = this->DisplayableToolVector.begin(); it != this->DisplayableToolVector.end(); ++it) {
    if (((*it) != NULL) && ((*it)->GetActor() != NULL)) {
      (*it)->GetActor()->VisibilityOff();
    }
  }

	this->CanvasRenderer->Modified();

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::ShowTool(TRACKER_TOOL_TYPE aType, bool aOn)
{
  LOG_TRACE("vtkToolVisualizer::ShowTool(" << aType << ", " << (aOn?"true":"false") << ")");

  if (this->DisplayableToolVector[aType] != NULL) {
    this->DisplayableToolVector[aType]->GetActor()->SetVisibility(aOn);
  }

	this->CanvasRenderer->Modified();

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::ShowInputPoints(bool aOn)
{
	LOG_TRACE("vtkToolVisualizer::ShowInputPoints(" << (aOn?"true":"false") << ")");

  this->InputPointsActor->SetVisibility(aOn);
	this->CanvasRenderer->Modified();

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::ShowResultPoints(bool aOn)
{
	LOG_TRACE("vtkToolVisualizer::ShowResultPoints(" << (aOn?"true":"false") << ")");

  this->ResultPointsActor->SetVisibility(aOn);
	this->CanvasRenderer->Modified();

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void vtkToolVisualizer::SetPhantomToPhantomReferenceTransform(vtkTransform* aTransform)
{
	LOG_TRACE("vtkToolVisualizer::SetPhantomToPhantomReferenceTransform");

  vtkSmartPointer<vtkTransform> phantomModelToPhantomReferenceTransform = vtkSmartPointer<vtkTransform>::New();
  phantomModelToPhantomReferenceTransform->Identity();
  phantomModelToPhantomReferenceTransform->Concatenate(aTransform->GetMatrix());
  phantomModelToPhantomReferenceTransform->Concatenate(this->DisplayableToolVector[TRACKER_TOOL_REFERENCE]->GetTool()->GetModelToToolTransform());
  phantomModelToPhantomReferenceTransform->Modified();

  this->DisplayableToolVector[TRACKER_TOOL_REFERENCE]->GetActor()->SetUserTransform(phantomModelToPhantomReferenceTransform);

  this->DisplayableToolVector[TRACKER_TOOL_REFERENCE]->DisplayableOn();
}

//-----------------------------------------------------------------------------

vtkDisplayableTool* vtkToolVisualizer::GetDisplayableTool(TRACKER_TOOL_TYPE aType)
{
	LOG_TRACE("vtkToolVisualizer::GetDisplayableTool(" << aType << ")");

  if (this->DisplayableToolVector.size() > aType) {
    return this->DisplayableToolVector[aType];
  } else {
    return NULL;
  }
}

//-----------------------------------------------------------------------------

TrackerStatus vtkToolVisualizer::AcquireTrackerPositionForToolByType(TRACKER_TOOL_TYPE aType, vtkSmartPointer<vtkMatrix4x4> aOutputMatrix, bool aCalibrated/* = false*/)
{
	//LOG_TRACE("vtkToolVisualizer::AcquireTrackerPositionForToolByType");

	if (this->DataCollector->GetTracker() == NULL) {
		LOG_ERROR("Tracker is not initialized!");
		return TR_MISSING;
	}

	vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	TrackerStatus status = TR_MISSING;
	double timestamp;

  int toolNumber = this->DataCollector->GetTracker()->GetFirstPortNumberByType(aType);
  if (toolNumber == -1) {
    LOG_ERROR("Unable to find tool with type " << aType << " in tracker!");
    return TR_MISSING;
  }

	if (this->DataCollector->GetTracker()->GetTool(toolNumber)->GetEnabled()) {
		this->DataCollector->GetTransformWithTimestamp(transformMatrix, timestamp, status, toolNumber, aCalibrated); 
	}

  switch (status)
  {
    case TR_MISSING:
	    LOG_DEBUG("Missing tool");
      break;
    case TR_OUT_OF_VIEW:
	    LOG_DEBUG("Tool out of view");
      break;
    case TR_REQ_TIMEOUT:
	    LOG_WARNING("Tracker request timeout!");
      break;
  }

	aOutputMatrix->DeepCopy(transformMatrix);

  return status;
}

//-----------------------------------------------------------------------------

std::string vtkToolVisualizer::GetToolPositionString(TRACKER_TOOL_TYPE aType, bool aCalibrated)
{
	//LOG_TRACE("vtkToolVisualizer::GetToolPositionString");

  vtkSmartPointer<vtkMatrix4x4> toolToReferenceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  TrackerStatus status = AcquireTrackerPositionForToolByType(aType, toolToReferenceMatrix, aCalibrated);
  if (status == TR_OK) {
		// Compute the new position - TODO: find other way
    double toolPosition[4];
		double elements[16];
		double origin[4] = {0.0, 0.0, 0.0, 1.0};

    for (int i=0; i<4; ++i) {
      for (int j=0; j<4; ++j) {
        elements[4*j+i] = toolToReferenceMatrix->GetElement(i,j);
      }
    }

		vtkMatrix4x4::PointMultiply(elements, origin, toolPosition);

		// Assemble position string
		char toolPositionChars[32];

		sprintf_s(toolPositionChars, 32, "%.1lf X %.1lf X %.1lf", toolPosition[0], toolPosition[1], toolPosition[2]);
		return std::string(toolPositionChars);
  } else {
    switch (status)
    {
      case TR_MISSING:
	      return "Missing tool";
      case TR_OUT_OF_VIEW:
	      return "Tool out of view";
      case TR_REQ_TIMEOUT:
	      return "Tracker request timeout!";
      default:
        return "Other error!";
    }
  }
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::EnableImageMode(bool aOn)
{
	LOG_TRACE("vtkToolVisualizer::EnableImageMode(" << (aOn?"true":"false") << ")");

  if (this->DataCollector == NULL) {
    return PLUS_SUCCESS;
  }

  if (aOn) {
    this->ImageActor->VisibilityOn();
    this->ImageActor->SetOpacity(1.0);

    if (CalculateImageCameraParameters() != PLUS_SUCCESS) {
      LOG_ERROR("Calculating image camera parameters failed!");
      return PLUS_FAIL;
    }

    vtkSmartPointer<vtkTransform> identity = vtkSmartPointer<vtkTransform>::New();
    identity->Identity();
    this->ImageActor->SetUserTransform(identity);

    // Set opacity of all tools to zero (if image mode is turned off, display devices function will set it back)
    for (std::vector<vtkDisplayableTool*>::iterator it = this->DisplayableToolVector.begin(); it != this->DisplayableToolVector.end(); ++it) {
      if (((*it) != NULL) && ((*it)->GetActor() != NULL)) {
        (*it)->GetActor()->GetProperty()->SetOpacity(0.0);
      }
    }

  // If just changed from image mode to show devices mode
  } else if (this->ImageMode == true) {
    // Reset opacity of phantom
    this->DisplayableToolVector[TRACKER_TOOL_REFERENCE]->GetActor()->GetProperty()->SetOpacity(0.6);

		// Reset camera to show all devices and the image
		vtkSmartPointer<vtkCamera> imageCamera = vtkSmartPointer<vtkCamera>::New(); 
		imageCamera->SetViewUp(0, 1, 0);
		imageCamera->ParallelProjectionOff();
		this->SetImageCamera(imageCamera);

		this->CanvasRenderer->SetActiveCamera(this->ImageCamera);
  }

  // Disable camera movements in image mode and enable otherwise
  EnableCameraMovements(!aOn);

  this->CanvasRenderer->SetGradientBackground(!aOn);
  this->CanvasRenderer->Modified();

  this->SetImageMode(aOn);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::DisplayDevices()
{
	//LOG_TRACE("vtkToolVisualizer::DisplayDevices");

  // In image mode there is no need for device visualization
  if (this->ImageMode) {
    return PLUS_FAIL;
  }

  // If none of the tools are displayable then return with fail
  if ( ((this->DisplayableToolVector[TRACKER_TOOL_PROBE] == NULL) || (this->DisplayableToolVector[TRACKER_TOOL_PROBE]->IsDisplayable() == false) || (this->DisplayableToolVector[TRACKER_TOOL_PROBE]->GetActor()->GetVisibility() == 0))
    && ((this->DisplayableToolVector[TRACKER_TOOL_STYLUS] == NULL) || (this->DisplayableToolVector[TRACKER_TOOL_STYLUS]->IsDisplayable() == false) || (this->DisplayableToolVector[TRACKER_TOOL_STYLUS]->GetActor()->GetVisibility() == 0))
    && ((this->DisplayableToolVector[TRACKER_TOOL_NEEDLE] == NULL) || (this->DisplayableToolVector[TRACKER_TOOL_NEEDLE]->IsDisplayable() == false) || (this->DisplayableToolVector[TRACKER_TOOL_NEEDLE]->GetActor()->GetVisibility() == 0)))
  {
    return PLUS_FAIL;
  }

  bool resetCameraNeeded = false;

  // For all tools
  for (std::vector<vtkDisplayableTool*>::iterator it = this->DisplayableToolVector.begin(); it != this->DisplayableToolVector.end(); ++it) {
    vtkDisplayableTool* tool = (*it);

	  // If reference then no need for setting transform (the phantom is fixed to the reference) - also skip is tool is missing
    if ((tool == NULL) || (tool->GetTool() == NULL) || (tool->GetTool()->GetToolType() == TRACKER_TOOL_REFERENCE)) {
		  continue;
	  } else if ((tool->GetTool()->GetEnabled() == false) || (tool->GetDisplayable() == false)) {
      tool->GetActor()->VisibilityOff();
    }

    // If opacity was 0.0, then this is the first visualization iteration after switvhing back from image mode - reset camera is needed
    if (tool->GetActor()->GetProperty()->GetOpacity() == 0.0) {
      resetCameraNeeded = true;
    }

	  TrackerStatus status = TR_MISSING;
	  double timestamp;

	  vtkSmartPointer<vtkMatrix4x4> toolToReferenceTransformMatrix = NULL;

	  // Acquire position from tracker
	  toolToReferenceTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
    this->DataCollector->GetTransformWithTimestamp(toolToReferenceTransformMatrix, timestamp, status, tool->GetTool()->GetToolPort()); 

	  // Compute and set transforms for actors
	  if (status == TR_OK) {
		  vtkSmartPointer<vtkTransform> toolModelToPhantomReferenceTransform = vtkSmartPointer<vtkTransform>::New();
		  toolModelToPhantomReferenceTransform->Identity();
		  toolModelToPhantomReferenceTransform->Concatenate(toolToReferenceTransformMatrix);
      if (tool->GetTool()->GetToolType() != TRACKER_TOOL_PROBE) {
        toolModelToPhantomReferenceTransform->Concatenate(tool->GetTool()->GetCalibrationMatrix());
      }
      toolModelToPhantomReferenceTransform->Concatenate(tool->GetTool()->GetModelToToolTransform());
		  toolModelToPhantomReferenceTransform->Modified();

      // If probe then take care of the image canvas
      if (tool->GetTool()->GetToolType() == TRACKER_TOOL_PROBE) {
        if (tool->GetActor()->GetProperty()->GetOpacity() == 0.3) {
          resetCameraNeeded = true;
        }

        // If image to probe matrix has non-null element, then it is considered calibrated, so we show the image (the transform matrix was initialized to zero)
        bool calibrated = false;
        for (int i=0; i<16; ++i) {
          if (this->ImageToProbeTransform->GetMatrix()->GetElement(i/4, i%4) != 0) {
            calibrated = true;
            break;
          }
        }

        if (calibrated) {
          // Image canvas transform
			    vtkSmartPointer<vtkTransform> imageToPhantomReferenceTransform = vtkSmartPointer<vtkTransform>::New();
			    imageToPhantomReferenceTransform->Identity();
			    imageToPhantomReferenceTransform->Concatenate(toolToReferenceTransformMatrix);
			    imageToPhantomReferenceTransform->Concatenate(this->ImageToProbeTransform);
  				
          this->ImageActor->VisibilityOn();
          this->ImageActor->SetOpacity(1.0);
			    this->ImageActor->SetUserTransform(imageToPhantomReferenceTransform);

        } else {
          this->ImageActor->VisibilityOff();
        }
      }

      tool->GetActor()->GetProperty()->SetOpacity(1.0);
		  tool->GetActor()->SetUserTransform(toolModelToPhantomReferenceTransform);

    } else { // if status is not TR_OK
      tool->GetActor()->GetProperty()->SetOpacity(0.3);

      if (tool->GetTool()->GetToolType() == TRACKER_TOOL_PROBE) {
         this->ImageActor->SetOpacity(0.3);
      }
    }
  } // for all tools

  if (resetCameraNeeded) {
	  this->CanvasRenderer->ResetCamera();
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::InitializePhantomVisualization()
{
	LOG_TRACE("vtkToolVisualizer::InitializePhantomVisualization");

  vtkSmartPointer<vtkTrackerTool> referenceTool = this->DataCollector->GetTracker()->GetTool(this->DataCollector->GetTracker()->GetReferenceToolNumber());
  if (referenceTool == NULL) {
    LOG_WARNING("No reference tool is present in the tracker - one is created for visualization");

    referenceTool = vtkSmartPointer<vtkTrackerTool>::New();
    referenceTool->SetToolType(TRACKER_TOOL_REFERENCE);
  }

  // Create displayable tool object for phantom
  vtkDisplayableTool* displayableTool = vtkDisplayableTool::New();
  displayableTool->DisplayableOff(); // Until phantom registration is missing (phantom to phantom reference transform is set)
  displayableTool->SetTool(referenceTool);
  this->DisplayableToolVector[TRACKER_TOOL_REFERENCE] = displayableTool;

  // Get phantom definition xml data element
	vtkSmartPointer<vtkXMLDataElement> phantomDefinition = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()->FindNestedElementWithName("PhantomDefinition");
	if (phantomDefinition == NULL) {
		LOG_WARNING("No phantom definition is found in the XML tree - no phantom will be displayed!");
		return PLUS_FAIL;
	}

	// Load phantom registration transform
  vtkSmartPointer<vtkXMLDataElement> geometry = phantomDefinition->FindNestedElementWithName("Geometry"); 
  if (geometry == NULL) {
	  LOG_ERROR("Phantom geometry information not found!");
	  return PLUS_FAIL;
  }

  vtkSmartPointer<vtkXMLDataElement> registration = geometry->FindNestedElementWithName("Registration"); 
  if (registration == NULL) {
	  LOG_ERROR("Registration element not found!");
	  return PLUS_FAIL;
  } else {

    // Check date - if it is empty, the calibration is considered as invalid (as the calibration transforms in the installed config files are identity matrices with empty dates)
    const char* date = registration->GetAttribute("Date");
    if ((date == NULL) || (STRCASECMP(date, "") == 0)) {
      LOG_INFO("Transform cannot be loaded with no date entered - phantom model is not shown until registering it");
    } else {
      // Load transform
	    double* transform = new double[16]; 
	    if (registration->GetVectorAttribute("MatrixValue", 16, transform)) {
        vtkSmartPointer<vtkTransform> phantomToPhantomReferenceTransform = vtkSmartPointer<vtkTransform>::New();
        phantomToPhantomReferenceTransform->Identity();
		    phantomToPhantomReferenceTransform->SetMatrix(transform);
        this->SetPhantomToPhantomReferenceTransform(phantomToPhantomReferenceTransform);
      } else {
        LOG_ERROR("Unable to read MatrixValue element in phantom registration!");
        return PLUS_FAIL;
	    }
	    delete[] transform;
    }
  }

	// Load model to phantom transform
  vtkSmartPointer<vtkXMLDataElement> model = phantomDefinition->FindNestedElementWithName("Model"); 
	if (model == NULL) {
		LOG_WARNING("Phantom model information not found - no model displayed");
	} else {
		// PhantomModelToPhantomTransform - transform input model for proper visualization
		double* phantomModelToPhantomTransformVector = new double[16]; 
		if (model->GetVectorAttribute("ModelToPhantomTransform", 16, phantomModelToPhantomTransformVector)) {
			vtkSmartPointer<vtkTransform> phantomModelToPhantomTransform = vtkSmartPointer<vtkTransform>::New();
      phantomModelToPhantomTransform->Identity();
			phantomModelToPhantomTransform->SetMatrix(phantomModelToPhantomTransformVector);
      this->DisplayableToolVector[TRACKER_TOOL_REFERENCE]->GetTool()->SetModelToToolTransform(phantomModelToPhantomTransform);

    } else {
      LOG_ERROR("Unable to read ModelToPhantomTransform element!");
      return PLUS_FAIL;
    }

		delete[] phantomModelToPhantomTransformVector;
	}

  // If model and all transforms has been found, set up the visualization pipeline
  vtkSmartPointer<vtkSTLReader> stlReader = vtkSmartPointer<vtkSTLReader>::New();
  if (LoadPhantomModel(stlReader) == PLUS_SUCCESS) {
	  vtkSmartPointer<vtkPolyDataMapper> phantomMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	  phantomMapper->SetInputConnection(stlReader->GetOutputPort());

  	this->DisplayableToolVector[TRACKER_TOOL_REFERENCE]->GetActor()->SetMapper(phantomMapper);
    this->DisplayableToolVector[TRACKER_TOOL_REFERENCE]->GetActor()->GetProperty()->SetOpacity(0.6);
    this->CanvasRenderer->AddActor(this->DisplayableToolVector[TRACKER_TOOL_REFERENCE]->GetActor());
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::CalculateImageCameraParameters()
{
	//LOG_TRACE("vtkToolVisualizer::CalculateImageCameraParameters");

  // Only set new camera if image actor is visible
  if ((this->ImageActor == NULL) || (this->ImageActor->GetVisibility() == 0)) {
    return PLUS_SUCCESS;
  }

	if (this->DataCollector->GetVideoSource() == NULL) {
		LOG_WARNING("Data collector has no video source!");
		return PLUS_FAIL;
	}

	// Calculate image center
	double imageCenterX = 0;
	double imageCenterY = 0;
	int dimensions[3];
	this->DataCollector->GetVideoSource()->GetFrameSize(dimensions);
	imageCenterX = dimensions[0] / 2.0;
	imageCenterY = dimensions[1] / 2.0;

	// Set up camera
	vtkSmartPointer<vtkCamera> imageCamera = vtkSmartPointer<vtkCamera>::New(); 
	imageCamera->SetFocalPoint(imageCenterX, imageCenterY, 0);
	imageCamera->SetViewUp(0, -1, 0);
	imageCamera->SetClippingRange(0.1, 2000.0);
	imageCamera->ParallelProjectionOn();

	// Calculate distance of camera from the plane
  int *renderWindowSize = this->CanvasRenderer->GetRenderWindow()->GetSize();
	if ((double)renderWindowSize[0] / (double)renderWindowSize[1] > imageCenterX / imageCenterY) {
		// If canvas aspect ratio is more elongenated in the X position then compute the distance according to the Y axis
		imageCamera->SetParallelScale(imageCenterY);
	} else {
		imageCamera->SetParallelScale(imageCenterX * (double)renderWindowSize[1] / (double)renderWindowSize[0]);
	}

  imageCamera->SetPosition(imageCenterX, imageCenterY, -200.0);

	// Set camera
	this->SetImageCamera(imageCamera);

	this->CanvasRenderer->SetActiveCamera(this->ImageCamera);

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::EnableCameraMovements(bool aEnabled)
{
	LOG_TRACE("vtkToolVisualizer::EnableCameraMovements(" << (aEnabled?"true":"false") << ")");

  if (aEnabled) {
    this->CanvasRenderer->GetRenderWindow()->GetInteractor()->SetInteractorStyle(vtkInteractorStyleTrackballCamera::New());
  } else {
    this->CanvasRenderer->GetRenderWindow()->GetInteractor()->RemoveAllObservers();
  }

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::StartDataCollection()
{
	LOG_TRACE("vtkToolVisualizer::StartDataCollection"); 

	// Stop data collection if already started
	if (this->DataCollector != NULL) {
		this->DataCollector->Stop();
	}

	// Initialize data collector and read configuration
	vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New(); 
	this->SetDataCollector(dataCollector);

  if (this->DataCollector->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS) {
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

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::LoadPhantomModel(vtkSTLReader* aSTLReader)
{
	LOG_TRACE("vtkToolVisualizer::LoadPhantomModel");

	vtkSmartPointer<vtkXMLDataElement> phantomDefinition = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()->FindNestedElementWithName("PhantomDefinition");
	if (phantomDefinition == NULL) {
		LOG_WARNING("No phantom definition is found in the XML tree - no phantom will be displayed!");
		return PLUS_FAIL;
	}

  vtkSmartPointer<vtkXMLDataElement> model = phantomDefinition->FindNestedElementWithName("Model"); 
	if (model == NULL) {
		LOG_WARNING("Phantom model information not found - no model displayed");
		return PLUS_FAIL;
  }

	const char* file = model->GetAttribute("File");
	if (!file) {
		LOG_WARNING("Phantom model file name not found - no model displayed");
		return PLUS_FAIL;
  } else {
		// Initialize phantom model visualization
		std::string searchResult = vtkPlusConfig::GetFirstFileFoundInConfigurationDirectory(file);
		if (STRCASECMP("", searchResult.c_str()) == 0) {
			LOG_WARNING("Phantom model file is not found with name: " << file << " - no model is displayed");
  		return PLUS_FAIL;
		} else {
			aSTLReader->SetFileName(searchResult.c_str());
		}
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::SetDefaultStylusModel(vtkActor* aActor)
{
	LOG_TRACE("vtkToolVisualizer::SetDefaultStylusModel");

	if (aActor == NULL) {
		LOG_ERROR("Unable to load stylus model to an uninitialized actor!");
		return PLUS_FAIL;
	}

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
	vtkSmartPointer<vtkConeSource> resultPointsConeSource = vtkSmartPointer<vtkConeSource>::New();
	resultPointsConeSource->SetRadius(1.5); // mm
	resultPointsConeSource->SetHeight(10.0); //mm
	vtkSmartPointer<vtkTransform> coneTransform = vtkSmartPointer<vtkTransform>::New();
	coneTransform->Identity();
	coneTransform->RotateZ(-90.0);
	coneTransform->Translate(-5.0, 0.0, 0.0);
	vtkSmartPointer<vtkTransformPolyDataFilter> coneTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	coneTransformFilter->AddInputConnection(resultPointsConeSource->GetOutputPort());
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
  aActor->SetVisibility(false);

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::DumpBuffersToDirectory(const char* aDirectory)
{
  LOG_TRACE("vtkToolVisualizer::DumpBuffersToDirectory(" << aDirectory << ")");

  if ((this->DataCollector == NULL) || (! this->DataCollector->GetInitialized())) {
		LOG_ERROR("Data collector is not initialized!");
		return PLUS_FAIL;
	}

  // Assemble file names
  std::string dateAndTime = vtksys::SystemTools::GetCurrentDateTime("%Y%m%d_%H%M%S");
  std::string outputVideoBufferSequenceFileName = "BufferDump_Video_";
  outputVideoBufferSequenceFileName.append(dateAndTime);
  std::string outputTrackerBufferSequenceFileName = "BufferDump_Tracker_";
  outputTrackerBufferSequenceFileName.append(dateAndTime);

  // Dump buffers to file 
  if ( this->DataCollector->GetVideoSource() != NULL )  {
    LOG_INFO("Write video buffer to " << outputVideoBufferSequenceFileName);
    this->DataCollector->WriteVideoBufferToMetafile( this->DataCollector->GetVideoSource()->GetBuffer(), aDirectory, outputVideoBufferSequenceFileName.c_str(), false); 
  }

  if ( this->DataCollector->GetTracker() != NULL ) {
    LOG_INFO("Write tracker buffer to " << outputTrackerBufferSequenceFileName);
    this->DataCollector->WriteTrackerToMetafile( this->DataCollector->GetTracker(), aDirectory, outputTrackerBufferSequenceFileName.c_str(), false); 
  }

  return PLUS_SUCCESS;
}
