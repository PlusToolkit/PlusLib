/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"

#include "vtkToolVisualizer.h"

#include "vtkDataCollectorHardwareDevice.h" //TODO
#include "vtkTracker.h"

#include "vtkTrackedFrameList.h"
#include "TrackedFrame.h"

#include "vtkObjectFactory.h"
#include "vtkDirectory.h"
#include "vtkXMLUtilities.h"

#include "vtkTransform.h"
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
#include "vtkPlusVideoSource.h"
#include "vtkVideoBuffer.h"
#include "vtksys/SystemTools.hxx"

#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleTrackballCamera.h"

#include <QTimer>

//-----------------------------------------------------------------------------

vtkStandardNewMacro(vtkToolVisualizer);

vtkCxxSetObjectMacro(vtkToolVisualizer, ImageToProbeTransform, vtkTransform);

//-----------------------------------------------------------------------------

vtkToolVisualizer::vtkToolVisualizer()
{
  this->DataCollector = NULL;
  this->TransformRepository = NULL;
  this->AcquisitionFrameRate = 20;
  this->InitializedOff();
  this->ImageModeOff();
  this->ProbeToolName = NULL; //TODO delete
  this->ReferenceToolName = NULL;
  this->StylusToolName = NULL;

  this->CanvasRenderer = NULL;
  this->InputPolyData = NULL;
  this->InputActor = NULL;
  this->ResultPolyData = NULL;
  this->ResultActor = NULL;
  this->ImageActor = NULL;
  this->ImageCamera = NULL;
  this->VolumeActor = NULL;

  this->ImageToProbeTransform = NULL; // TODO delete
  // TODO Add ImageRenderingTransformName instead

  this->SetReferenceToolName("Reference"); // TODO needed?

  // Create timer
  this->AcquisitionTimer = NULL;
}

//-----------------------------------------------------------------------------

vtkToolVisualizer::~vtkToolVisualizer()
{
  if (this->AcquisitionTimer != NULL)
  {
    disconnect( this->AcquisitionTimer, SIGNAL( timeout() ), this, SLOT( DisplayDevices() ) );
    this->AcquisitionTimer->stop();
    delete this->AcquisitionTimer;
    this->AcquisitionTimer = NULL;
  } 
  
  if (this->DataCollector != NULL)
  {
    this->DataCollector->Stop();
    this->DataCollector->Disconnect();
  }
  this->SetDataCollector(NULL);

  this->SetTransformRepository(NULL);

  this->SetInputActor(NULL);
  this->SetInputPolyData(NULL);
  this->SetResultActor(NULL);
  this->SetResultPolyData(NULL);
  this->SetVolumeActor(NULL);

  this->SetCanvasRenderer(NULL);
  this->SetImageActor(NULL);
  this->SetImageCamera(NULL);

  ClearDisplayableTools();

  this->SetImageToProbeTransform(NULL);
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::Initialize()
{
  LOG_TRACE("vtkToolVisualizer::Initialize"); 

  if (this->Initialized)
  {
    return PLUS_SUCCESS;
  }

  // Set up canvas renderer
  vtkSmartPointer<vtkRenderer> canvasRenderer = vtkSmartPointer<vtkRenderer>::New(); 
  canvasRenderer->SetBackground(0.1, 0.1, 0.1);
  canvasRenderer->SetBackground2(0.4, 0.4, 0.4);
  canvasRenderer->SetGradientBackground(true);
  this->SetCanvasRenderer(canvasRenderer);

  // Initialize visualization
  if (InitializeVisualization() != PLUS_SUCCESS)
  {
    LOG_ERROR("Initializing visualization failed!");
    return PLUS_FAIL;
  }

  // Create transform repository
  vtkSmartPointer<vtkTransformRepository> transformRepository = vtkSmartPointer<vtkTransformRepository>::New();
  this->SetTransformRepository(transformRepository);

  // Initialize timer
  this->AcquisitionTimer = new QTimer();
  this->AcquisitionTimer->start(1000.0 / this->AcquisitionFrameRate);

  connect( this->AcquisitionTimer, SIGNAL( timeout() ), this, SLOT( DisplayDevices() ) );

  this->SetInitialized(true);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  LOG_TRACE("vtkToolVisualizer::ReadConfiguration");

  if (aConfig == NULL)
  {
    LOG_ERROR("Unable to read configuration"); 
    return PLUS_FAIL; 
  }

  // Read coordinate definitions
  if ( this->TransformRepository->ReadConfiguration( vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData() ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read coordinate definitions from device set configuration!");
    return PLUS_FAIL;
  }

  // Rendering section
  vtkXMLDataElement* renderingElement = aConfig->FindNestedElementWithName("Rendering"); 

  if (renderingElement == NULL)
  {
    LOG_ERROR("Unable to find Rendering element in XML tree!"); 
    return PLUS_FAIL;     
  }

  // Read displayable tool configurations
  for ( int i = 0; i < renderingElement->GetNumberOfNestedElements(); ++i )
  {
    vtkXMLDataElement* displayableToolElement = renderingElement->GetNestedElement(i); 
    if ( STRCASECMP(displayableToolElement->GetName(), "DisplayableTool") != 0 )
    {
      // if this is not a DisplayableTool element, skip it
      continue; 
    }

    // Create displayable tool and read its configuration
    vtkDisplayableTool* displayableTool = vtkDisplayableTool::New();

    if (displayableTool->ReadConfiguration(displayableToolElement) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to read displayable tool configuration!");
      continue;
    }

    this->DisplayableTools[displayableTool->GetToolToWorldTransformName().From()] = displayableTool;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::InitializeVisualization()
{
  LOG_TRACE("vtkToolVisualizer::InitializeVisualization");

  // Input points poly data
  vtkSmartPointer<vtkPolyData> inputPolyData = vtkSmartPointer<vtkPolyData>::New();
  inputPolyData->Initialize();
  vtkSmartPointer<vtkPoints> input = vtkSmartPointer<vtkPoints>::New();
  inputPolyData->SetPoints(input);
  this->SetInputPolyData(inputPolyData);

  // Input points actor
  vtkSmartPointer<vtkActor> inputActor = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkPolyDataMapper> inputMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkGlyph3D> inputGlyph = vtkSmartPointer<vtkGlyph3D>::New();
  vtkSmartPointer<vtkSphereSource> inputSphereSource = vtkSmartPointer<vtkSphereSource>::New();
  inputSphereSource->SetRadius(1.5); // mm

  inputGlyph->SetInputConnection(this->InputPolyData->GetProducerPort());
  inputGlyph->SetSourceConnection(inputSphereSource->GetOutputPort());
  inputMapper->SetInputConnection(inputGlyph->GetOutputPort());
  inputActor->SetMapper(inputMapper);
  inputActor->GetProperty()->SetColor(0.0, 0.7, 1.0);
  this->SetInputActor(inputActor);


  // Result points poly data
  vtkSmartPointer<vtkPolyData> resultPolyData = vtkSmartPointer<vtkPolyData>::New();
  resultPolyData = vtkPolyData::New();
  resultPolyData->Initialize();
  vtkSmartPointer<vtkPoints> resultPoint = vtkSmartPointer<vtkPoints>::New();
  //resultPoint->SetNumberOfPoints(1); // Makes the input actor disappear!
  resultPolyData->SetPoints(resultPoint);
  this->SetResultPolyData(resultPolyData);

  // Result points actor
  vtkSmartPointer<vtkActor> resultActor = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkPolyDataMapper> resultMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkGlyph3D> resultGlyph = vtkSmartPointer<vtkGlyph3D>::New();
  vtkSmartPointer<vtkSphereSource> resultSphereSource = vtkSmartPointer<vtkSphereSource>::New();
  resultSphereSource->SetRadius(3.0); // mm

  resultGlyph->SetInputConnection(this->ResultPolyData->GetProducerPort());
  resultGlyph->SetSourceConnection(resultSphereSource->GetOutputPort());
  resultMapper->SetInputConnection(resultGlyph->GetOutputPort());
  resultActor->SetMapper(resultMapper);
  resultActor->GetProperty()->SetColor(0.0, 0.8, 0.0);
  this->SetResultActor(resultActor);

  // Volume actor
  vtkSmartPointer<vtkActor> volumeActor = vtkSmartPointer<vtkActor>::New();
  this->SetVolumeActor(volumeActor);

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
  this->CanvasRenderer->AddActor(this->InputActor);
  this->CanvasRenderer->AddActor(this->ResultActor);
  this->CanvasRenderer->AddActor(this->VolumeActor);
  this->CanvasRenderer->AddActor(this->ImageActor);

  // Hide all actors
  HideAll();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::ClearDisplayableTools()
{
  LOG_TRACE("vtkToolVisualizer::ClearDisplayableTools");

  for (std::map<std::string, vtkDisplayableTool*>::iterator it = this->DisplayableTools.begin(); it != this->DisplayableTools.end(); ++it)
  {
    vtkDisplayableTool* tool = it->second;
    if (tool != NULL)
    {
      if (tool->GetActor() != NULL)
      {
        this->CanvasRenderer->RemoveActor(tool->GetActor());
      }

      tool->Delete();
      tool = NULL;
    }
  }

  this->DisplayableTools.clear();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::InitializeDeviceVisualization()
{
  LOG_TRACE("vtkToolVisualizer::InitializeDeviceVisualization");

  if (this->ProbeToolName == NULL || STRCASECMP(this->ProbeToolName, "") == 0)
  {
    LOG_ERROR("Probe name is unspecified!");
    return PLUS_FAIL;
  }

  if (this->DataCollector->GetConnected() == false)
  {
    LOG_ERROR("Device visualization cannot be initialized unless they are connected");
    return PLUS_FAIL;
  }

  // Delete displayable tools
  ClearDisplayableTools();

  // Connect data collector to image actor
  if (this->DataCollector->GetVideoEnabled())
  {
    this->ImageActor->VisibilityOn();
    this->ImageActor->SetInput(this->DataCollector->GetOutput());
  }
  else
  {
    LOG_INFO("Data collector has no video output, cannot initialize image actor");
  }

  // Load phantom model and registration from configuration and set up visualization
  if (InitializePhantomVisualization() == PLUS_FAIL)
  {
    LOG_WARNING("Initializing phantom visualization failed!");
  }

  if (this->DataCollector->GetTrackingEnabled() == false)
  {
    LOG_ERROR("No tracking data is available");
    return PLUS_FAIL;
  }

  // Load tool models
  for (std::map<std::string, vtkDisplayableTool*>::iterator it = this->DisplayableTools.begin(); it != this->DisplayableTools.end(); ++it)
  {
    vtkDisplayableTool* displayableTool = it->second;
    if (displayableTool == NULL)
    {
      LOG_ERROR("Invalid displayable tool!");
      continue;
    }

    std::string toolName = displayableTool->GetToolToWorldTransformName().From();

    // Check if file name exists and file can be found
    std::string stlFileName = "";
    if (STRCASECMP(displayableTool->GetSTLModelFileName(), "") == 0)
    {
      LOG_WARNING("No STL file name found for displayable tool (" << toolName << ") - it cannot be displayed!");
      continue;
    }

    std::string searchResult = vtkPlusConfig::GetFirstFileFoundInConfigurationDirectory(displayableTool->GetSTLModelFileName());
    if (STRCASECMP("", searchResult.c_str()) == 0)
    {
      LOG_WARNING("Tool (" << toolName << ") model file is not found with name: " << displayableTool->GetSTLModelFileName());
      // Don't continue cycle here because for some tools default model can be assigned
    }
    else
    {
      stlFileName = searchResult;
    }

    // Handle missing tool models
    if (stlFileName.empty())
    {
      if (toolName.compare(this->ProbeToolName) == 0) // If the tool is the given probe and there is no model for it
      {
        LOG_WARNING("Unable to initialize probe visualization - no probe is displayed");
      }
      else if (toolName.find("Stylus") != std::string::npos) // If the tool name contains stylus but there is no model for it
      {
        LOG_INFO("No stylus model file found - default model will be displayed");

        SetDefaultStylusModel(displayableTool->GetActor());
        this->CanvasRenderer->AddActor(displayableTool->GetActor());
      }
    }
    else
    // Load STL model
    {
      vtkSmartPointer<vtkSTLReader> stlReader = vtkSmartPointer<vtkSTLReader>::New();
      stlReader->SetFileName(stlFileName.c_str());

      vtkSmartPointer<vtkPolyDataMapper> toolMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
      toolMapper->SetInputConnection(stlReader->GetOutputPort());

      displayableTool->GetActor()->SetMapper(toolMapper);

      if (toolName.find("Stylus") != std::string::npos) // Stylus is always black
      {
        displayableTool->GetActor()->GetProperty()->SetColor(0.0, 0.0, 0.0);
      }

      this->CanvasRenderer->AddActor(displayableTool->GetActor());
    }
  }

  // Hide all tools so that they don't appear next to the configuration toolbox
  HideAll();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::SetAcquisitionFrameRate(int aFrameRate)
{
  LOG_TRACE("vtkToolVisualizer::SetAcquisitionFrameRate(" << aFrameRate << ")");

  this->AcquisitionFrameRate = aFrameRate;

  if (this->AcquisitionTimer != NULL)
  {
    this->AcquisitionTimer->stop();
    this->AcquisitionTimer->start(1000.0 / this->AcquisitionFrameRate);
  }
  else
  {
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
  this->InputActor->VisibilityOff();
  this->ResultActor->VisibilityOff();
  this->VolumeActor->VisibilityOff();
  this->ImageActor->VisibilityOff();

  ShowAllTools(false);

  this->CanvasRenderer->Modified();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::ShowAllTools(bool aOn)
{
  LOG_TRACE("vtkToolVisualizer::ShowAllTools(" << (aOn?"true":"false") << ")");

  for (std::map<std::string, vtkDisplayableTool*>::iterator it = this->DisplayableTools.begin(); it != this->DisplayableTools.end(); ++it)
  {
    vtkDisplayableTool* tool = it->second;
    if ((tool != NULL) && (tool->GetActor() != NULL))
    {
      tool->GetActor()->SetVisibility(aOn);
    }
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::ShowTool(const char* aToolName, bool aOn)
{
  LOG_TRACE("vtkToolVisualizer::ShowTool(" << aToolName << ", " << (aOn?"true":"false") << ")");

  if (this->DisplayableTools.find(aToolName) != this->DisplayableTools.end())
  {
    this->DisplayableTools[aToolName]->GetActor()->SetVisibility(aOn);
  }

  this->CanvasRenderer->Modified();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::ShowInput(bool aOn)
{
  LOG_TRACE("vtkToolVisualizer::ShowInput(" << (aOn?"true":"false") << ")");

  this->InputActor->SetVisibility(aOn);
  this->CanvasRenderer->Modified();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::ShowResult(bool aOn)
{
  LOG_TRACE("vtkToolVisualizer::ShowResult(" << (aOn?"true":"false") << ")");

  this->ResultActor->SetVisibility(aOn);
  this->CanvasRenderer->Modified();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::SetPhantomToReferenceTransform(vtkTransform* aTransform)
{
  LOG_TRACE("vtkToolVisualizer::SetPhantomToReferenceTransform");

  if (this->DisplayableTools.find(this->ReferenceToolName) == this->DisplayableTools.end())
  {
    LOG_ERROR("Missing reference displayable tool!");
    return PLUS_FAIL;
  }

  vtkSmartPointer<vtkTransform> phantomModelToPhantomReferenceTransform = vtkSmartPointer<vtkTransform>::New();
  phantomModelToPhantomReferenceTransform->Identity();
  phantomModelToPhantomReferenceTransform->Concatenate(aTransform->GetMatrix());
  phantomModelToPhantomReferenceTransform->Concatenate(this->DisplayableTools[this->ReferenceToolName]->GetModelToToolTransform());
  phantomModelToPhantomReferenceTransform->Modified();

  this->DisplayableTools[this->ReferenceToolName]->GetActor()->SetUserTransform(phantomModelToPhantomReferenceTransform);

  this->DisplayableTools[this->ReferenceToolName]->DisplayableOn();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::GetDisplayableTool(const char* aToolName, vtkDisplayableTool* &aDisplayableTool)
{
  LOG_TRACE("vtkToolVisualizer::GetDisplayableTool(" << aToolName << ")");

  if (this->DisplayableTools.find(aToolName) != this->DisplayableTools.end())
  {
    aDisplayableTool = this->DisplayableTools[aToolName];
    return PLUS_SUCCESS;
  }
  else
  {
    LOG_ERROR("Requested displayable tool '" << aToolName << "' is missing!");
    return PLUS_FAIL;
  }
}

//-----------------------------------------------------------------------------

TrackedFrameFieldStatus vtkToolVisualizer::AcquireTrackerPositionForToolByName(const char* aName, vtkSmartPointer<vtkMatrix4x4> aOutputMatrix, bool aCalibrated/* = false*/)
{
  //LOG_TRACE("vtkToolVisualizer::AcquireTrackerPositionForToolByName");

  // TODO!!!!!!!
  LOG_ERROR("TEMPORARY ISSUE: Acquire position has to be changed to use tracked frame instead of tracker tools!");
  return FIELD_INVALID;
  /*
  if (this->DataCollector->GetTracker() == NULL)
  {
    LOG_ERROR("Tracker is not initialized!");
    return TR_MISSING;
  }

  vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  TrackerStatus status = TR_MISSING;
  double timestamp;

  vtkTrackerTool* tool = NULL;
  if (this->DataCollector->GetTracker()->GetTool(aName, tool) != PLUS_SUCCESS)
  {
    LOG_ERROR("No tool found with the specified name '" << aName << "'!");
    return TR_MISSING;
  }

  this->DataCollector->GetTransformWithTimestamp(transformMatrix, timestamp, status, aName, aCalibrated); 

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
  */
}

//-----------------------------------------------------------------------------

std::string vtkToolVisualizer::GetToolPositionString(const char* aToolName, bool aCalibrated)
{
  //LOG_TRACE("vtkToolVisualizer::GetToolPositionString");

  vtkSmartPointer<vtkMatrix4x4> toolToReferenceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  TrackedFrameFieldStatus status = AcquireTrackerPositionForToolByName(aToolName, toolToReferenceMatrix, aCalibrated);
  if (status == FIELD_OK)
  {
    // Assemble position string
    char toolPositionChars[32];
    sprintf_s(toolPositionChars, 32, "%.1lf X %.1lf X %.1lf", toolToReferenceMatrix->GetElement(0,3), toolToReferenceMatrix->GetElement(1,3), toolToReferenceMatrix->GetElement(2,3));
    return std::string(toolPositionChars);
  }

  return "Missing tool";
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::EnableImageMode(bool aOn)
{
  LOG_TRACE("vtkToolVisualizer::EnableImageMode(" << (aOn?"true":"false") << ")");

  if (this->DataCollector == NULL)
  {
    return PLUS_SUCCESS;
  }

  if (aOn)
  {
    this->ImageActor->VisibilityOn();
    this->ImageActor->SetOpacity(1.0);

    if (CalculateImageCameraParameters() != PLUS_SUCCESS)
    {
      LOG_ERROR("Calculating image camera parameters failed!");
      return PLUS_FAIL;
    }

    vtkSmartPointer<vtkTransform> identity = vtkSmartPointer<vtkTransform>::New();
    identity->Identity();
    this->ImageActor->SetUserTransform(identity);

    // Set opacity of all tools to zero (if image mode is turned off, display devices function will set it back)
    for (std::map<std::string, vtkDisplayableTool*>::iterator it = this->DisplayableTools.begin(); it != this->DisplayableTools.end(); ++it)
    {
      vtkDisplayableTool* tool = it->second;
      if ((tool != NULL) && (tool->GetActor() != NULL))
      {
        tool->GetActor()->GetProperty()->SetOpacity(0.0);
      }
    }
  }
  else if (this->ImageMode == true) // If just changed from image mode to show devices mode
  {
    // Reset opacity of phantom
    if (this->DisplayableTools.find(this->ReferenceToolName) == this->DisplayableTools.end())
    {
      LOG_ERROR("Missing reference displayable tool!");
      return PLUS_FAIL;
    }
    this->DisplayableTools[this->ReferenceToolName]->GetActor()->GetProperty()->SetOpacity(0.6);

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
  if (this->ImageMode)
  {
    return PLUS_FAIL;
  }

  // If none of the tools are displayable then return with fail
  bool noToolsToDisplay = true;
  for (std::map<std::string, vtkDisplayableTool*>::iterator it = this->DisplayableTools.begin(); it != this->DisplayableTools.end(); ++it)
  {
    vtkDisplayableTool* tool = it->second;
    if ( tool->IsDisplayable() && tool->GetActor()->GetVisibility() > 0 )
    {
      noToolsToDisplay = false;
    }
  }

  if (noToolsToDisplay)
  {
    return PLUS_FAIL;
  }

  bool resetCameraNeeded = false;

  TrackedFrame trackedFrame; 
  if ( this->DataCollector->GetTrackedFrame(&trackedFrame) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get tracked frame!"); 
    return PLUS_FAIL; 
  }

  LOG_ERROR("Temporary issue: update transform repo with tracked frame"); 
  /*
  // For all tools
  for (std::map<std::string, vtkDisplayableTool*>::iterator it = this->DisplayableTools.begin(); it != this->DisplayableTools.end(); ++it)
  {
    std::string toolName = it->first;
    vtkDisplayableTool* tool = it->second;

    // If reference then no need for setting transform (the phantom is fixed to the reference) - also skip is tool is missing
    if ((tool == NULL) || (tool->GetTool() == NULL) || (toolName.compare(this->ReferenceToolName) == 0))
    {
      continue;
    }
    else if (tool->GetDisplayable() == false)
    {
      tool->GetActor()->VisibilityOff();
    }

    // If opacity was 0.0, then this is the first visualization iteration after switvhing back from image mode - reset camera is needed
    if (tool->GetActor()->GetProperty()->GetOpacity() == 0.0)
    {
      resetCameraNeeded = true;
    }

    TrackedFrameFieldStatus status = FIELD_INVALID;

    vtkSmartPointer<vtkMatrix4x4> toolToReferenceTransformMatrix = NULL;

    // Acquire position from tracker
    PlusTransformName transformName(toolName, "Reference");
    toolToReferenceTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    
    // Compute and set transforms for actors
    if (status == FIELD_OK)
    {
      vtkSmartPointer<vtkTransform> toolModelToPhantomReferenceTransform = vtkSmartPointer<vtkTransform>::New();
      toolModelToPhantomReferenceTransform->Identity();
      toolModelToPhantomReferenceTransform->Concatenate(toolToReferenceTransformMatrix);

      // TODO: Get transforms from Transform repo
      LOG_ERROR("TEMPORARY ISSUE: get toolModelToPhantomReferenceTransform from TransformRepository");

      if (toolName.compare(this->ProbeToolName) != 0)
      {
        // TODO: Remove it!
        //toolModelToPhantomReferenceTransform->Concatenate(tool->GetTool()->GetCalibrationMatrix());
      }
      toolModelToPhantomReferenceTransform->Concatenate(tool->GetTool()->GetModelToToolTransform());
      toolModelToPhantomReferenceTransform->Modified();

      // If probe then take care of the image canvas
      if (toolName.compare(this->ProbeToolName) == 0)
      {
        if (tool->GetActor()->GetProperty()->GetOpacity() == 0.3)
        {
          resetCameraNeeded = true;
        }

        // If image to probe matrix has non-null element, then it is considered calibrated, so we show the image (the transform matrix was initialized to zero)
        bool calibrated = false;
        for (int i=0; i<16; ++i)
        {
          if (this->ImageToProbeTransform->GetMatrix()->GetElement(i/4, i%4) != 0)
          {
            calibrated = true;
            break;
          }
        }

        if (calibrated)
        {
          // Image canvas transform
          vtkSmartPointer<vtkTransform> imageToPhantomReferenceTransform = vtkSmartPointer<vtkTransform>::New();
          imageToPhantomReferenceTransform->Identity();
          imageToPhantomReferenceTransform->Concatenate(toolToReferenceTransformMatrix);
          imageToPhantomReferenceTransform->Concatenate(this->ImageToProbeTransform);
          
          this->ImageActor->VisibilityOn();
          this->ImageActor->SetOpacity(1.0);
          this->ImageActor->SetUserTransform(imageToPhantomReferenceTransform);
        }
        else
        {
          this->ImageActor->VisibilityOff();
        }
      }

      tool->GetActor()->GetProperty()->SetOpacity(1.0);
      tool->GetActor()->SetUserTransform(toolModelToPhantomReferenceTransform);
    }
    else // if status is not TR_OK
    {
      tool->GetActor()->GetProperty()->SetOpacity(0.3);

      if (toolName.compare(this->ProbeToolName) == 0)
      {
         this->ImageActor->SetOpacity(0.3);
      }
    }
  } // for all tools

  if (resetCameraNeeded)
  {
    this->CanvasRenderer->ResetCamera();
  }*/

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::InitializePhantomVisualization()
{
  LOG_TRACE("vtkToolVisualizer::InitializePhantomVisualization");

  if (this->ReferenceToolName == NULL || STRCASECMP(this->ReferenceToolName, "") == 0)
  {
    LOG_ERROR("Reference name is unspecified!");
    return PLUS_FAIL;
  }

  if (this->DataCollector->GetTrackingEnabled() == false)
  {
    LOG_ERROR("No tracker is available");
    return PLUS_FAIL;
  }

  // Create displayable tool object for phantom
  vtkDisplayableTool* displayableTool = vtkDisplayableTool::New();
  displayableTool->DisplayableOff(); // Until phantom registration is missing (phantom to phantom reference transform is set)
  this->DisplayableTools[this->ReferenceToolName] = displayableTool;

  // Get phantom definition xml data element
  vtkXMLDataElement* phantomDefinition = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()->FindNestedElementWithName("PhantomDefinition");
  if (phantomDefinition == NULL)
  {
    LOG_WARNING("No phantom definition is found in the XML tree - no phantom will be displayed!");
    return PLUS_FAIL;
  }

  // Load phantom registration transform
  vtkXMLDataElement* geometry = phantomDefinition->FindNestedElementWithName("Geometry"); 
  if (geometry == NULL)
  {
    LOG_ERROR("Phantom geometry information not found!");
    return PLUS_FAIL;
  }

  PlusTransformName phantomToReferenceTransformName("Phantom", this->ReferenceToolName); // TODO No constant strings
  vtkSmartPointer<vtkMatrix4x4> phantomToReferenceTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  bool valid = false;
  if ( (this->TransformRepository->GetTransform(phantomToReferenceTransformName, phantomToReferenceTransformMatrix, &valid) != PLUS_SUCCESS) || (!valid) )
  {
    LOG_ERROR("No valid phantom registration transform found!");
    return PLUS_FAIL;
  }

  vtkSmartPointer<vtkTransform> phantomToReferenceTransform = vtkSmartPointer<vtkTransform>::New();
  phantomToReferenceTransform->Identity();
  phantomToReferenceTransform->Concatenate(phantomToReferenceTransformMatrix);

  if ( SetPhantomToReferenceTransform(phantomToReferenceTransform) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to set phantom to reference transform to visualizer!");
    return PLUS_FAIL;
  }

  // Set up the visualization pipeline
  vtkSmartPointer<vtkSTLReader> stlReader = vtkSmartPointer<vtkSTLReader>::New();
  if ( LoadPhantomModel(stlReader) == PLUS_SUCCESS )
  {
    vtkSmartPointer<vtkPolyDataMapper> phantomMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    phantomMapper->SetInputConnection(stlReader->GetOutputPort());

    this->DisplayableTools[this->ReferenceToolName]->GetActor()->SetMapper(phantomMapper);
    this->DisplayableTools[this->ReferenceToolName]->GetActor()->GetProperty()->SetOpacity(0.6);
    this->CanvasRenderer->AddActor(this->DisplayableTools[this->ReferenceToolName]->GetActor());
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::CalculateImageCameraParameters()
{
  //LOG_TRACE("vtkToolVisualizer::CalculateImageCameraParameters");

  // Only set new camera if image actor is visible
  if ((this->ImageActor == NULL) || (this->ImageActor->GetVisibility() == 0))
  {
    return PLUS_SUCCESS;
  }

  // Calculate image center
  double imageCenterX = 0;
  double imageCenterY = 0;
  int dimensions[2];
  this->DataCollector->GetFrameSize(dimensions);
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
  if ((double)renderWindowSize[0] / (double)renderWindowSize[1] > imageCenterX / imageCenterY)
  {
    // If canvas aspect ratio is more elongenated in the X position then compute the distance according to the Y axis
    imageCamera->SetParallelScale(imageCenterY);
  }
  else
  {
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

  if (aEnabled)
  {
    this->CanvasRenderer->GetRenderWindow()->GetInteractor()->SetInteractorStyle(vtkInteractorStyleTrackballCamera::New());
  }
  else
  {
    this->CanvasRenderer->GetRenderWindow()->GetInteractor()->RemoveAllObservers();
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::StartDataCollection()
{
  LOG_TRACE("vtkToolVisualizer::StartDataCollection"); 

  // Delete data collection if already exists
  if (this->DataCollector != NULL)
  {
    this->DataCollector->Stop();
    this->DataCollector->Disconnect();

    this->SetDataCollector(NULL);
  }

  // Create the proper data collector variant
  vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New(); 
  this->SetDataCollector(dataCollector);

  // Reset transform repository
  this->TransformRepository->Clear();

  // Read configuration
  if (this->DataCollector->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  if (this->DataCollector->Connect() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  if (this->DataCollector->Start() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  if (! this->DataCollector->GetConnected())
  {
    LOG_ERROR("Unable to initialize DataCollector!"); 
    return PLUS_FAIL;
  }

  // Fill up transform repository
  this->TransformRepository->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData());

  TrackedFrame trackedFrame;
  this->DataCollector->GetTrackedFrame(&trackedFrame);
  this->TransformRepository->SetTransforms(trackedFrame);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::LoadPhantomModel(vtkSTLReader* aSTLReader) //TODO do it another way (somehow get the file name instead)
{
  LOG_TRACE("vtkToolVisualizer::LoadPhantomModel");

  vtkXMLDataElement* phantomDefinition = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()->FindNestedElementWithName("PhantomDefinition");
  if (phantomDefinition == NULL)
  {
    LOG_WARNING("No phantom definition is found in the XML tree - no phantom will be displayed!");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* model = phantomDefinition->FindNestedElementWithName("Model"); 
  if (model == NULL)
  {
    LOG_WARNING("Phantom model information not found - no model displayed");
    return PLUS_FAIL;
  }

  const char* file = model->GetAttribute("File");
  if (!file)
  {
    LOG_WARNING("Phantom model file name not found - no model displayed");
    return PLUS_FAIL;
  }

  // Initialize phantom model visualization
  std::string searchResult = vtkPlusConfig::GetFirstFileFoundInConfigurationDirectory(file);
  if (STRCASECMP("", searchResult.c_str()) == 0)
  {
    LOG_WARNING("Phantom model file is not found with name: " << file << " - no model is displayed");
    return PLUS_FAIL;
  }
  else
  {
    this->DisplayableTools[this->ReferenceToolName]->SetSTLModelFileName(searchResult.c_str());
    aSTLReader->SetFileName(searchResult.c_str());
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::SetDefaultStylusModel(vtkActor* aActor)
{
  LOG_TRACE("vtkToolVisualizer::SetDefaultStylusModel");

  if (aActor == NULL)
  {
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
  vtkSmartPointer<vtkConeSource> resultConeSource = vtkSmartPointer<vtkConeSource>::New();
  resultConeSource->SetRadius(1.5); // mm
  resultConeSource->SetHeight(10.0); //mm
  vtkSmartPointer<vtkTransform> coneTransform = vtkSmartPointer<vtkTransform>::New();
  coneTransform->Identity();
  coneTransform->RotateZ(-90.0);
  coneTransform->Translate(-5.0, 0.0, 0.0);
  vtkSmartPointer<vtkTransformPolyDataFilter> coneTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  coneTransformFilter->AddInputConnection(resultConeSource->GetOutputPort());
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

  if ((this->DataCollector == NULL) || (! this->DataCollector->GetConnected()))
  {
    LOG_INFO("Data collector is not connected, buffers cannot be saved");
    return PLUS_FAIL;
  }

  vtkDataCollectorHardwareDevice* dataCollectorHardwareDevice = dynamic_cast<vtkDataCollectorHardwareDevice*>(this->DataCollector);
  if ( dataCollectorHardwareDevice == NULL )
  {
    LOG_INFO("Data collector is not the type that uses hardware devices, there are no buffers to save");
    return PLUS_FAIL;
  }

  // Assemble file names
  std::string dateAndTime = vtksys::SystemTools::GetCurrentDateTime("%Y%m%d_%H%M%S");
  std::string outputVideoBufferSequenceFileName = "BufferDump_Video_";
  outputVideoBufferSequenceFileName.append(dateAndTime);
  std::string outputTrackerBufferSequenceFileName = "BufferDump_Tracker_";
  outputTrackerBufferSequenceFileName.append(dateAndTime);

  // Dump buffers to file 
  if ( dataCollectorHardwareDevice->GetVideoSource() != NULL )
  {
    LOG_INFO("Write video buffer to " << outputVideoBufferSequenceFileName);
    dataCollectorHardwareDevice->GetVideoSource()->GetBuffer()->WriteToMetafile( aDirectory, outputVideoBufferSequenceFileName.c_str(), false); 
  }

  if ( dataCollectorHardwareDevice->GetTracker() != NULL )
  {
    LOG_INFO("Write tracker buffer to " << outputTrackerBufferSequenceFileName);
    dataCollectorHardwareDevice->GetTracker()->WriteToMetafile( aDirectory, outputTrackerBufferSequenceFileName.c_str(), false);
  }

  return PLUS_SUCCESS;
}
