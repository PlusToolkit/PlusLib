/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"

#include "vtkToolVisualizer.h"

#include "vtkTrackedFrameList.h"
#include "TrackedFrame.h"

#include "vtkDataCollectorHardwareDevice.h" // Only for dumping buffers
#include "vtkTracker.h"

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
#include "vtkAppendPolyData.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkSTLReader.h"
#include "vtkAxesActor.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkPlusVideoSource.h"
#include "vtkVideoBuffer.h"
#include "vtksys/SystemTools.hxx"

#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleTrackballCamera.h"

#include <QTimer>

//-----------------------------------------------------------------------------

vtkStandardNewMacro(vtkToolVisualizer);

//-----------------------------------------------------------------------------

vtkToolVisualizer::vtkToolVisualizer()
{
  this->DataCollector = NULL;
  this->TransformRepository = NULL;
  this->AcquisitionFrameRate = 20;
  this->InitializedOff();
  this->ImageModeOff();
  this->WorldCoordinateFrame = NULL;
  this->ImageCoordinateFrame = NULL; //TODO needed?

  this->CanvasRenderer = NULL;
  this->InputPolyData = NULL;
  this->InputActor = NULL;
  this->ResultPolyData = NULL;
  this->ResultActor = NULL;
  this->ImageActor = NULL;
  this->ImageCamera = NULL;
  this->VolumeActor = NULL;

  // Create timer
  this->AcquisitionTimer = NULL;
}

//-----------------------------------------------------------------------------

vtkToolVisualizer::~vtkToolVisualizer()
{
  ClearDisplayableObjects();

  if (this->AcquisitionTimer != NULL)
  {
    disconnect( this->AcquisitionTimer, SIGNAL( timeout() ), this, SLOT( UpdateObjectVisualization() ) );
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

  // Initialize basic visualization
  if (InitializeBasicVisualization() != PLUS_SUCCESS)
  {
    LOG_ERROR("Initializing basic visualization failed!");
    return PLUS_FAIL;
  }

  // Create transform repository
  vtkSmartPointer<vtkTransformRepository> transformRepository = vtkSmartPointer<vtkTransformRepository>::New();
  this->SetTransformRepository(transformRepository);

  // Initialize timer
  this->AcquisitionTimer = new QTimer();
  this->AcquisitionTimer->start(1000.0 / this->AcquisitionFrameRate);

  connect( this->AcquisitionTimer, SIGNAL( timeout() ), this, SLOT( UpdateObjectVisualization() ) );

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

  // World coordinate frame name
  const char* worldCoordinateFrame = renderingElement->GetAttribute("WorldCoordinateFrame");
  if (worldCoordinateFrame == NULL)
  {
	  LOG_ERROR("WorldCoordinateFrame is not specified in DisplayableTool element of the configuration!");
    return PLUS_FAIL;     
  }

  this->SetWorldCoordinateFrame(worldCoordinateFrame);

  // Read displayable tool configurations
  bool imageFound = false;
  for ( int i = 0; i < renderingElement->GetNumberOfNestedElements(); ++i )
  {
    vtkXMLDataElement* displayableObjectElement = renderingElement->GetNestedElement(i); 
    if ( STRCASECMP(displayableObjectElement->GetName(), "DisplayableObject") != 0 )
    {
      // if this is not a DisplayableObject element, skip it
      continue; 
    }

    // Create displayable tool
    vtkDisplayableObject* displayableObject = vtkDisplayableObject::New();

    // Check if image
    const char* type = displayableObjectElement->GetAttribute("Type");
    if (STRCASECMP(type, "Image") == 0)
    {
      const char* objectCoordinateFrame = displayableObjectElement->GetAttribute("ObjectCoordinateFrame");
      if (objectCoordinateFrame == NULL)
      {
        LOG_ERROR("No ObjectCoordinateFrame defined for image!");
      }

      displayableObject->SetObjectCoordinateFrame(objectCoordinateFrame);
      displayableObject->SetActor(this->ImageActor);
      displayableObject->SetDisplayable(false);
      this->SetImageCoordinateFrame(objectCoordinateFrame);

      imageFound = true;
    }
    else
    {
      // Read configuration if not image
      if (displayableObject->ReadConfiguration(displayableObjectElement) != PLUS_SUCCESS)
      {
        LOG_ERROR("Unable to read displayable tool configuration!");
        continue;
      }
    }

    this->DisplayableObjects[displayableObject->GetObjectCoordinateFrame()] = displayableObject;
  }

  if (this->DisplayableObjects.size() == 0)
  {
    LOG_ERROR("No displayable objects found!");
    return PLUS_FAIL;
  }

  if (!imageFound)
  {
    LOG_INFO("No image found in the displayable object list, it will not be displayed!");
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::InitializeBasicVisualization()
{
  LOG_TRACE("vtkToolVisualizer::InitializeBasicVisualization");

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

PlusStatus vtkToolVisualizer::ClearDisplayableObjects()
{
  LOG_TRACE("vtkToolVisualizer::ClearDisplayableObjects");

  for (std::map<std::string, vtkDisplayableObject*>::iterator it = this->DisplayableObjects.begin(); it != this->DisplayableObjects.end(); ++it)
  {
    vtkDisplayableObject* tool = it->second;
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

  this->DisplayableObjects.clear();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::InitializeObjectVisualization()
{
  LOG_TRACE("vtkToolVisualizer::InitializeObjectVisualization");

  if (this->DataCollector->GetConnected() == false)
  {
    LOG_ERROR("Device visualization cannot be initialized unless they are connected");
    return PLUS_FAIL;
  }

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

  if (this->DataCollector->GetTrackingEnabled() == false)
  {
    LOG_ERROR("No tracking data is available");
    return PLUS_FAIL;
  }

  // Load displayable object models
  for (std::map<std::string, vtkDisplayableObject*>::iterator it = this->DisplayableObjects.begin(); it != this->DisplayableObjects.end(); ++it)
  {
    vtkDisplayableObject* displayableObject = it->second;
    if (displayableObject == NULL)
    {
      LOG_ERROR("Invalid displayable object!");
      continue;
    }

    std::string objectName(displayableObject->GetObjectCoordinateFrame());

    // Check if file name exists and file can be found
    std::string stlFileName = "";
    if ( (displayableObject->GetSTLModelFileName() == NULL) || (STRCASECMP(displayableObject->GetSTLModelFileName(), "") == 0) )
    {
      LOG_DEBUG("No STL file name found for displayable object (" << objectName << ")");
      continue;
    }

    std::string searchResult = vtkPlusConfig::GetFirstFileFoundInConfigurationDirectory(displayableObject->GetSTLModelFileName());
    if (STRCASECMP("", searchResult.c_str()) == 0)
    {
      LOG_WARNING("Displayable object (" << displayableObject->GetObjectCoordinateFrame() << ") model file is not found with name: " << displayableObject->GetSTLModelFileName());
      // Don't continue cycle here because for some tools default model can be assigned
    }
    else
    {
      stlFileName = searchResult;
    }

    // Handle missing object models if possible
    if (stlFileName.empty())
    {
      if (objectName.find("Stylus") != std::string::npos) // If the tool name contains stylus but there is no model for it
      {
        LOG_INFO("No stylus model file found - default model will be displayed");

        SetDefaultStylusModel(displayableObject->GetActor());
        this->CanvasRenderer->AddActor(displayableObject->GetActor());
      }
    }
    else
    // Load STL model as usual
    {
      vtkSmartPointer<vtkSTLReader> stlReader = vtkSmartPointer<vtkSTLReader>::New();
      stlReader->SetFileName(stlFileName.c_str());

      vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
      mapper->SetInputConnection(stlReader->GetOutputPort());

      vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();

      displayableObject->SetActor(actor);
      displayableObject->SetMapper(mapper);
      displayableObject->SetOpacity( displayableObject->GetLastOpacity() );

      if (objectName.find("Stylus") != std::string::npos) // Stylus is always black
      {
        displayableObject->SetColor(0.0, 0.0, 0.0);
      }

      this->CanvasRenderer->AddActor(displayableObject->GetActor());
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
    if (this->AcquisitionTimer->isActive())
    {
      this->AcquisitionTimer->stop();
      this->AcquisitionTimer->start(1000.0 / this->AcquisitionFrameRate);
    }
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

  ShowAllObjects(false);

  this->CanvasRenderer->Modified();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::ShowAllObjects(bool aOn)
{
  LOG_TRACE("vtkToolVisualizer::ShowAllObjects(" << (aOn?"true":"false") << ")");

  for (std::map<std::string, vtkDisplayableObject*>::iterator it = this->DisplayableObjects.begin(); it != this->DisplayableObjects.end(); ++it)
  {
    vtkDisplayableObject* displayableObject = it->second;
    if ((displayableObject != NULL) && (displayableObject->GetActor() != NULL))
    {
      displayableObject->GetActor()->SetVisibility(aOn);
    }
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::ShowObject(const char* aObjectCoordinateFrame, bool aOn)
{
  LOG_TRACE("vtkToolVisualizer::ShowObject(" << aObjectCoordinateFrame << ", " << (aOn?"true":"false") << ")");

  if (this->DisplayableObjects.find(aObjectCoordinateFrame) != this->DisplayableObjects.end())
  {
    this->DisplayableObjects[aObjectCoordinateFrame]->GetActor()->SetVisibility(aOn);
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

PlusStatus vtkToolVisualizer::GetDisplayableObject(const char* aObjectCoordinateFrame, vtkDisplayableObject* &aDisplayableObject)
{
  LOG_TRACE("vtkToolVisualizer::GetDisplayableObject");

  if (aObjectCoordinateFrame == NULL)
  {
    LOG_ERROR("Invalid object coordinate frame name!");
    return PLUS_FAIL;
  }

  if (this->DisplayableObjects.find(aObjectCoordinateFrame) != this->DisplayableObjects.end())
  {
    aDisplayableObject = this->DisplayableObjects[aObjectCoordinateFrame];
    return PLUS_SUCCESS;
  }
  else
  {
    LOG_ERROR("Requested displayable object '" << aObjectCoordinateFrame << "' is missing!");
    return PLUS_FAIL;
  }
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
    if (this->DataCollector->GetVideoEnabled() == false)
    {
      LOG_DEBUG("Cannot switch to image mode without enabled video in data collector!");
      return PLUS_FAIL;
    }

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

    // Set opacity of all displayable objects to zero (if image mode is turned off, display devices function will set it back)
    for (std::map<std::string, vtkDisplayableObject*>::iterator it = this->DisplayableObjects.begin(); it != this->DisplayableObjects.end(); ++it)
    {
      vtkDisplayableObject* displayableObject = it->second;
      if ((displayableObject != NULL) && (displayableObject->GetActor() != NULL) && (this->ImageActor != displayableObject->GetActor()))
      {
        if (fabs(displayableObject->GetOpacity()) > 0.001 && fabs(displayableObject->GetOpacity() - 0.3) > 0.001)
        {
          displayableObject->SetLastOpacity(displayableObject->GetOpacity());
        }
        displayableObject->SetOpacity(0.0);
      }
    }
  }
  else if (this->ImageMode == true) // If just changed from image mode to show devices mode
  {
    // Reset opacities
    for (std::map<std::string, vtkDisplayableObject*>::iterator it = this->DisplayableObjects.begin(); it != this->DisplayableObjects.end(); ++it)
    {
      vtkDisplayableObject* displayableObject = it->second;
      displayableObject->SetOpacity( displayableObject->GetLastOpacity() );
    }

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

PlusStatus vtkToolVisualizer::UpdateObjectVisualization()
{
  // In image mode there is no need for update objects
  if (this->ImageMode)
  {
    return PLUS_FAIL;
  }

  // If none of the objects are displayable then return with fail
  bool noObjectsToDisplay = true;
  for (std::map<std::string, vtkDisplayableObject*>::iterator it = this->DisplayableObjects.begin(); it != this->DisplayableObjects.end(); ++it)
  {
    vtkDisplayableObject* displayableObject = it->second;
    if ( displayableObject->IsDisplayable() && displayableObject->GetActor() && displayableObject->GetActor()->GetVisibility() > 0 )
    {
      noObjectsToDisplay = false;
    }
  }

  if (noObjectsToDisplay)
  {
    return PLUS_FAIL;
  }

  // Get tracked frame and set the transforms
  TrackedFrame trackedFrame; 
  if ( this->DataCollector->GetTrackedFrame(&trackedFrame) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get tracked frame!"); 
    return PLUS_FAIL; 
  }

  if ( this->TransformRepository->SetTransforms(trackedFrame) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set current transforms to transform repository!"); 
    return PLUS_FAIL; 
  }

  bool resetCameraNeeded = false;

  // Update actors of displayable objects
  for (std::map<std::string, vtkDisplayableObject*>::iterator it = this->DisplayableObjects.begin(); it != this->DisplayableObjects.end(); ++it)
  {
    vtkDisplayableObject* displayableObject = it->second;

    // If not displayable then hide
    if (displayableObject->IsDisplayable() == false)
    {
      displayableObject->GetActor()->VisibilityOff();
      continue;
    }

    // Get object to world transform
    PlusTransformName objectCoordinateFrameToWorldTransformName(displayableObject->GetObjectCoordinateFrame(), this->WorldCoordinateFrame);
    vtkSmartPointer<vtkMatrix4x4> objectCoordinateFrameToWorldTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();

bool sajt=false;
if (sajt) {this->TransformRepository->PrintSelf(std::cout, vtkIndent());} //TODO
    bool valid = false;
    if ( this->TransformRepository->GetTransform(objectCoordinateFrameToWorldTransformName, objectCoordinateFrameToWorldTransformMatrix, &valid) != PLUS_SUCCESS )
    {
      LOG_ERROR("No transform found from object (" << displayableObject->GetObjectCoordinateFrame() << ") to world! (" << this->WorldCoordinateFrame << ")");
      continue;
    }

    // If the transform is valid then display it normally
    if (valid)
    {
      // If opacity was 0.0, then this is the first visualization iteration after switching back from image mode - reset opacity and camera is needed
      // In case of 0.3 it was previously out of view, same opacity and camera reset is needed
      if (fabs(displayableObject->GetOpacity()) < 0.001 || fabs(displayableObject->GetOpacity() - 0.3) < 0.001)
      {
        displayableObject->SetOpacity( displayableObject->GetLastOpacity() );
        resetCameraNeeded = true;
      }

      // Assemble and set transform for visualization
      vtkSmartPointer<vtkTransform> objectModelToWorldTransform = vtkSmartPointer<vtkTransform>::New();
      objectModelToWorldTransform->Identity();
      objectModelToWorldTransform->Concatenate(objectCoordinateFrameToWorldTransformMatrix);
      objectModelToWorldTransform->Concatenate(displayableObject->GetModelToObjectTransform());
      objectModelToWorldTransform->Modified();

      displayableObject->GetActor()->SetUserTransform(objectModelToWorldTransform);
    }
    // If invalid then make it partially transparent and leave in place
    else
    {
      if (fabs(displayableObject->GetOpacity()) > 0.001 && fabs(displayableObject->GetOpacity() - 0.3) > 0.001)
      {
        displayableObject->SetLastOpacity( displayableObject->GetOpacity() );
      }
      displayableObject->SetOpacity( 0.3 );
    }
  } // for all displayable objects

  if (resetCameraNeeded)
  {
    this->CanvasRenderer->ResetCamera();
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
  if (this->DataCollector->GetTrackedFrame(&trackedFrame) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to get tracked frame from data collector!");
    return PLUS_FAIL;
  }

  this->TransformRepository->SetTransforms(trackedFrame);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::SetDefaultStylusModel(vtkProp3D* aActor)
{
  LOG_TRACE("vtkToolVisualizer::SetDefaultStylusModel");

  vtkActor* actor = dynamic_cast<vtkActor*>(aActor);
  if (actor == NULL)
  {
    LOG_ERROR("Unable to load stylus model to an uninitialized or invalid actor!");
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
  actor->SetMapper(stylusMapper);
  actor->GetProperty()->SetColor(0.0, 0.0, 0.0);
  actor->SetVisibility(false);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::DumpBuffersToDirectory(const char* aDirectory)
{
  LOG_TRACE("vtkToolVisualizer::DumpBuffersToDirectory");

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

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::IsExistingTransform(const char* aTransformFrom, const char* aTransformTo, bool aUseLatestTrackedFrame/* = true */)
{
  PlusTransformName transformName(aTransformFrom, aTransformTo);

  if (aUseLatestTrackedFrame)
  {
    if (this->DataCollector == NULL || this->DataCollector->GetTrackingEnabled() == false)
    {
      LOG_ERROR("Data collector object is invalid or not tracking!");
      return PLUS_FAIL;
    }

    TrackedFrame trackedFrame;
    if (this->DataCollector->GetTrackedFrame(&trackedFrame) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to get tracked frame from data collector!");
      return PLUS_FAIL;
    }
    if (this->TransformRepository->SetTransforms(trackedFrame) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to set transforms from tracked frame!");
      return PLUS_FAIL;
    }
  }

  return this->TransformRepository->IsExistingTransform(transformName);
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::GetTransformTranslationString(const char* aTransformFrom, const char* aTransformTo, std::string &aTransformTranslationString, bool* aValid/* = NULL*/)
{
  PlusTransformName transformName(aTransformFrom, aTransformTo);

  return GetTransformTranslationString(transformName, aTransformTranslationString, aValid);
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::GetTransformTranslationString(PlusTransformName aTransform, std::string &aTransformTranslationString, bool* aValid/* = NULL*/)
{
  vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  if (GetTransformMatrix(aTransform, transformMatrix, aValid) != PLUS_SUCCESS)
  {
    aTransformTranslationString = "N/A";
    return PLUS_FAIL;
  }

  char positionChars[32];
  sprintf_s(positionChars, 32, "%.1lf X %.1lf X %.1lf", transformMatrix->GetElement(0,3), transformMatrix->GetElement(1,3), transformMatrix->GetElement(2,3));

  aTransformTranslationString = std::string(positionChars);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::GetTransformMatrix(const char* aTransformFrom, const char* aTransformTo, vtkSmartPointer<vtkMatrix4x4> aOutputMatrix, bool* aValid/* = NULL*/)
{
  PlusTransformName transformName(aTransformFrom, aTransformTo);

  return GetTransformMatrix(transformName, aOutputMatrix, aValid);
}

//-----------------------------------------------------------------------------

PlusStatus vtkToolVisualizer::GetTransformMatrix(PlusTransformName aTransform, vtkSmartPointer<vtkMatrix4x4> aOutputMatrix, bool* aValid/* = NULL*/)
{
  TrackedFrame trackedFrame;
  if (this->DataCollector->GetTrackedFrame(&trackedFrame) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to get tracked frame from data collector!");
    return PLUS_FAIL;
  }
  if (this->TransformRepository->SetTransforms(trackedFrame) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to set transforms from tracked frame!");
    return PLUS_FAIL;
  }

  if (this->TransformRepository->GetTransform(aTransform, aOutputMatrix, aValid) != PLUS_SUCCESS)
  {
    std::string transformName; 
    aTransform.GetTransformName(transformName); 
    LOG_ERROR("Cannot get frame transform '" << transformName << "' from tracked frame!");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

