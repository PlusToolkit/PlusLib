/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "TrackedFrame.h"

#include "vtkAxesActor.h"
#include "vtkConeSource.h"
#include "vtkDataCollectorHardwareDevice.h" // Only for dumping buffers
#include "vtkDirectory.h"
#include "vtkGlyph3D.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkLineSource.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkObjectVisualizer.h"
#include "vtkPlusVideoSource.h" // Only for dumping buffers
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSphereSource.h"
#include "vtkTextProperty.h"
#include "vtkTrackedFrameList.h"
#include "vtkTracker.h" // Only for dumping buffers
#include "vtkTransform.h"
#include "vtkVideoBuffer.h" // Only for dumping buffers
#include "vtkXMLUtilities.h"
#include "vtksys/SystemTools.hxx"

#include <QTimer>
#include <QApplication>

//-----------------------------------------------------------------------------

vtkStandardNewMacro(vtkObjectVisualizer);

//-----------------------------------------------------------------------------

vtkObjectVisualizer::vtkObjectVisualizer()
: DataCollector(NULL)
, TransformRepository(NULL)
, AcquisitionFrameRate(20)
, WorldCoordinateFrame(NULL)
, CanvasRenderer(NULL)
, InputPolyData(NULL)
, InputActor(NULL)
, ResultPolyData(NULL)
, ResultActor(NULL)
, ImageActor(NULL)
, ImageCamera(NULL)
, VolumeActor(NULL)
, AcquisitionTimer(NULL)
, HorizontalOrientationTextActor(NULL)
, VerticalOrientationTextActor(NULL)
, OrientationMarkerAssembly(NULL)
, OrientationMarkerCurrentXRotation(0.0)
, OrientationMarkerCurrentYRotation(0.0)
{
  this->InitializedOff();
  this->ImageModeOff();
}

//-----------------------------------------------------------------------------

vtkObjectVisualizer::~vtkObjectVisualizer()
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

  this->SetOrientationMarkerAssembly(NULL);
  this->SetHorizontalOrientationTextActor(NULL);
  this->SetVerticalOrientationTextActor(NULL);
}

//-----------------------------------------------------------------------------

PlusStatus vtkObjectVisualizer::Initialize()
{
  LOG_TRACE("vtkObjectVisualizer::Initialize"); 

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

PlusStatus vtkObjectVisualizer::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  LOG_TRACE("vtkObjectVisualizer::ReadConfiguration");

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

    // Get type
    const char* type = displayableObjectElement->GetAttribute("Type");
    if (type == NULL)
    {
      LOG_ERROR("No type found for displayable object!");
      continue;
    }

    // Create displayable tool
    vtkDisplayableObject* displayableObject = vtkDisplayableObject::New(type);

    // Check if image
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

PlusStatus vtkObjectVisualizer::InitializeBasicVisualization()
{
  LOG_TRACE("vtkObjectVisualizer::InitializeBasicVisualization");

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

  if (InitializeOrientationMarkers() != PLUS_SUCCESS)
  {
    LOG_ERROR("Error during creation of orientation marker actors.");
    return PLUS_FAIL;
  }

  // Hide all actors
  HideAll();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkObjectVisualizer::ClearDisplayableObjects()
{
  LOG_TRACE("vtkObjectVisualizer::ClearDisplayableObjects");

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

PlusStatus vtkObjectVisualizer::InitializeObjectVisualization()
{
  LOG_TRACE("vtkObjectVisualizer::InitializeObjectVisualization");

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

  // Add displayable object actors to renderer
  for (std::map<std::string, vtkDisplayableObject*>::iterator it = this->DisplayableObjects.begin(); it != this->DisplayableObjects.end(); ++it)
  {
    vtkDisplayableObject* displayableObject = it->second;
    if (displayableObject == NULL)
    {
      LOG_ERROR("Invalid displayable object!");
      continue;
    }

    this->CanvasRenderer->AddActor(displayableObject->GetActor());
  }

  // Hide all tools so that they don't appear by default
  HideAll();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkObjectVisualizer::SetAcquisitionFrameRate(int aFrameRate)
{
  LOG_TRACE("vtkObjectVisualizer::SetAcquisitionFrameRate(" << aFrameRate << ")");

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

PlusStatus vtkObjectVisualizer::HideAll()
{
  LOG_TRACE("vtkObjectVisualizer::HideAll");

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

PlusStatus vtkObjectVisualizer::ShowAllObjects(bool aOn)
{
  LOG_TRACE("vtkObjectVisualizer::ShowAllObjects(" << (aOn?"true":"false") << ")");

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

PlusStatus vtkObjectVisualizer::ShowObject(const char* aObjectCoordinateFrame, bool aOn)
{
  LOG_TRACE("vtkObjectVisualizer::ShowObject(" << aObjectCoordinateFrame << ", " << (aOn?"true":"false") << ")");

  if (this->DisplayableObjects.find(aObjectCoordinateFrame) != this->DisplayableObjects.end())
  {
    this->DisplayableObjects[aObjectCoordinateFrame]->GetActor()->SetVisibility(aOn);
  }

  this->CanvasRenderer->Modified();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkObjectVisualizer::ShowInput(bool aOn)
{
  LOG_TRACE("vtkObjectVisualizer::ShowInput(" << (aOn?"true":"false") << ")");

  this->InputActor->SetVisibility(aOn);
  this->CanvasRenderer->Modified();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkObjectVisualizer::ShowResult(bool aOn)
{
  LOG_TRACE("vtkObjectVisualizer::ShowResult(" << (aOn?"true":"false") << ")");

  this->ResultActor->SetVisibility(aOn);
  this->CanvasRenderer->Modified();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkObjectVisualizer::GetDisplayableObject(const char* aObjectCoordinateFrame, vtkDisplayableObject* &aDisplayableObject)
{
  LOG_TRACE("vtkObjectVisualizer::GetDisplayableObject");

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

PlusStatus vtkObjectVisualizer::EnableImageMode(bool aOn)
{
  LOG_TRACE("vtkObjectVisualizer::EnableImageMode(" << (aOn?"true":"false") << ")");

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

    this->SetImageMode(aOn);
    this->SetScreenRightDownAxesOrientation();
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
    this->SetImageMode(aOn);
  }

  // Disable camera movements in image mode and enable otherwise
  EnableCameraMovements(!aOn);

  this->CanvasRenderer->SetGradientBackground(!aOn);
  this->CanvasRenderer->Modified();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkObjectVisualizer::SetScreenRightDownAxesOrientation( US_IMAGE_ORIENTATION aOrientation /*= US_IMG_ORIENT_MF*/ )
{
  LOG_TRACE("vtkObjectVisualizer::SetScreenRightDownAxesOrientation(" << aOrientation << ")");

  if( this->ImageMode )
  {
    double cameraPos[3];
    this->ImageCamera->GetPosition(cameraPos);

    // Depending on the desired orientation either roll the camera about its view vector 
    // and/or position the camera on the +z or -z side of the image plane.
    // This depends on having back face culling disabled.
    switch(aOrientation)
    {
    case US_IMG_ORIENT_UN:
      {
        //Unmarked, near
        this->ImageCamera->SetRoll(0);
        if( cameraPos[2] > 0.0 )
        {
          this->ImageCamera->SetPosition(cameraPos[0], cameraPos[1], -cameraPos[2]);
        }

        // Move and Orient the Orientation Markers to face the camera
        CalculateOrientationMarkerTransform(US_IMG_ORIENT_UN);
      }
      break;
    case US_IMG_ORIENT_UF:
      {
        // Unmarked, far
        this->ImageCamera->SetRoll(-180);
        if( cameraPos[2] < 0.0 )
        {
          this->ImageCamera->SetPosition(cameraPos[0], cameraPos[1], -cameraPos[2]);
        }

        // Move and Orient the Orientation Markers to face the camera
        CalculateOrientationMarkerTransform(US_IMG_ORIENT_UF);
      }
      break;
    case US_IMG_ORIENT_MF:
      {
        // Marked, far
        this->ImageCamera->SetRoll(-180);
        if( cameraPos[2] > 0.0 )
        {
          this->ImageCamera->SetPosition(cameraPos[0], cameraPos[1], -cameraPos[2]);
        }

        // Move and Orient the Orientation Markers to face the camera
        CalculateOrientationMarkerTransform(US_IMG_ORIENT_MF);
      }
      break;
    case US_IMG_ORIENT_MN:
      {
        // Marked, near
        this->ImageCamera->SetRoll(0);
        if( cameraPos[2] < 0.0 )
        {
          this->ImageCamera->SetPosition(cameraPos[0], cameraPos[1], -cameraPos[2]);
        }

        // Move and Orient the Orientation Markers to face the camera
        CalculateOrientationMarkerTransform(US_IMG_ORIENT_MN);
      }
      break;
    }
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkObjectVisualizer::EnableOrientationMarkers( bool aEnable )
{
  vtkSmartPointer<vtkPropCollection> collection = vtkSmartPointer<vtkPropCollection>::New();
  this->OrientationMarkerAssembly->GetActors(collection);
  collection->InitTraversal();

  if( !aEnable )
  {
    this->OrientationMarkerAssembly->VisibilityOff();
  }
  else
  {
    this->OrientationMarkerAssembly->VisibilityOn();
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkObjectVisualizer::UpdateObjectVisualization()
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
    PlusTransformName objectCoordinateFrameToWorldTransformName(displayableObject->GetObjectCoordinateFrame(), this->WorldCoordinateFrame);
    vtkSmartPointer<vtkMatrix4x4> objectCoordinateFrameToWorldTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();

    // If not displayable or valid transform does not exist then hide
    if ( (displayableObject->IsDisplayable() == false)
      || (this->TransformRepository->IsExistingTransform(objectCoordinateFrameToWorldTransformName) != PLUS_SUCCESS) )
    {
      if (displayableObject->GetActor())
      {
        displayableObject->GetActor()->VisibilityOff();
      }
      continue;
    }

    // Get object to world transform
    bool valid = false;
    if ( this->TransformRepository->GetTransform(objectCoordinateFrameToWorldTransformName, objectCoordinateFrameToWorldTransformMatrix, &valid) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get transform from object (" << displayableObject->GetObjectCoordinateFrame() << ") to world! (" << this->WorldCoordinateFrame << ")");
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

      vtkDisplayableModel* displayableModel = dynamic_cast<vtkDisplayableModel*>(displayableObject);
      if (displayableModel)
      {
        objectModelToWorldTransform->Concatenate(displayableModel->GetModelToObjectTransform());
      }
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

PlusStatus vtkObjectVisualizer::CalculateImageCameraParameters()
{
  //LOG_TRACE("vtkObjectVisualizer::CalculateImageCameraParameters");

  // Only set new camera if image actor is visible and data collector is connected
  if ((this->ImageActor == NULL) || (this->ImageActor->GetVisibility() == 0) || (this->DataCollector->GetConnected() == false))
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

PlusStatus vtkObjectVisualizer::EnableCameraMovements(bool aEnabled)
{
  LOG_TRACE("vtkObjectVisualizer::EnableCameraMovements(" << (aEnabled?"true":"false") << ")");

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

PlusStatus vtkObjectVisualizer::StartDataCollection()
{
  LOG_TRACE("vtkObjectVisualizer::StartDataCollection"); 

  // Delete data collection if already exists
  if (this->DataCollector != NULL)
  {
    this->DataCollector->Stop();
    this->DataCollector->Disconnect();

    this->SetDataCollector(NULL);
  }

  // Create the proper data collector variant
  vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New(); 
  if (dataCollector.GetPointer()==NULL)
  {
    LOG_ERROR("Failed to create DataCollector");
    return PLUS_FAIL;
  }
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

  // Some video sources (such as the Video For Windows video source) require additional
  // initialization time, with a chance to process background events. Therefore,
  // wait and retry getting a trackedframe if it fails, for a few times.
  const double MAX_STARTUP_DELAY_SEC=5.0;
  const double RETRY_AFTER_SEC=0.5;
  double waitedForSec=0;
  PlusStatus trackedFrameRequestStatus=PLUS_FAIL;
  while (waitedForSec<MAX_STARTUP_DELAY_SEC)
  {
    if (this->DataCollector->GetTrackedFrame(&trackedFrame) == PLUS_SUCCESS)
    {
      trackedFrameRequestStatus=PLUS_SUCCESS;
      break;
    }
    QApplication::processEvents();
    vtkAccurateTimer::Delay(RETRY_AFTER_SEC);
    waitedForSec+=RETRY_AFTER_SEC;
  }
  if (trackedFrameRequestStatus!=PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to get tracked frame from data collector");
    return PLUS_FAIL;
  }

  this->TransformRepository->SetTransforms(trackedFrame);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkObjectVisualizer::DumpBuffersToDirectory(const char* aDirectory)
{
  LOG_TRACE("vtkObjectVisualizer::DumpBuffersToDirectory");

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

PlusStatus vtkObjectVisualizer::IsExistingTransform(const char* aTransformFrom, const char* aTransformTo, bool aUseLatestTrackedFrame/* = true */)
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

  // For debugging purposes
  bool printTransforms = false;
  if (printTransforms)
  {
    this->TransformRepository->PrintSelf(std::cout, vtkIndent());
  }

  return this->TransformRepository->IsExistingTransform(transformName);
}

//-----------------------------------------------------------------------------

PlusStatus vtkObjectVisualizer::GetTransformTranslationString(const char* aTransformFrom, const char* aTransformTo, std::string &aTransformTranslationString, bool* aValid/* = NULL*/)
{
  PlusTransformName transformName(aTransformFrom, aTransformTo);

  return GetTransformTranslationString(transformName, aTransformTranslationString, aValid);
}

//-----------------------------------------------------------------------------

PlusStatus vtkObjectVisualizer::GetTransformTranslationString(PlusTransformName aTransform, std::string &aTransformTranslationString, bool* aValid/* = NULL*/)
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

PlusStatus vtkObjectVisualizer::GetTransformMatrix(const char* aTransformFrom, const char* aTransformTo, vtkSmartPointer<vtkMatrix4x4> aOutputMatrix, bool* aValid/* = NULL*/)
{
  PlusTransformName transformName(aTransformFrom, aTransformTo);

  return GetTransformMatrix(transformName, aOutputMatrix, aValid);
}

//-----------------------------------------------------------------------------

PlusStatus vtkObjectVisualizer::GetTransformMatrix(PlusTransformName aTransform, vtkSmartPointer<vtkMatrix4x4> aOutputMatrix, bool* aValid/* = NULL*/)
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

//-----------------------------------------------------------------------------

PlusStatus vtkObjectVisualizer::InitializeOrientationMarkers()
{
  this->SetOrientationMarkerAssembly(vtkSmartPointer<vtkAssembly>::New());

  // Since the internal orientation is always MF, display the indicators for MF in all cases
  vtkSmartPointer<vtkTextActor3D> horizontalOrientationTextActor = vtkSmartPointer<vtkTextActor3D>::New();
  horizontalOrientationTextActor->GetTextProperty()->SetColor(0.0, 1.0, 0.0);
  horizontalOrientationTextActor->GetTextProperty()->SetFontFamilyToArial();
  horizontalOrientationTextActor->GetTextProperty()->SetFontSize(16);
  horizontalOrientationTextActor->GetTextProperty()->SetJustificationToLeft();
  horizontalOrientationTextActor->GetTextProperty()->SetVerticalJustificationToTop();
  horizontalOrientationTextActor->GetTextProperty()->BoldOn(); 
  horizontalOrientationTextActor->SetInput("M");
  horizontalOrientationTextActor->RotateWXYZ(180.0, 1.0, 0.0, 0.0); 
  horizontalOrientationTextActor->SetPosition(35.0, 22.0, -1.0);
  this->SetHorizontalOrientationTextActor(horizontalOrientationTextActor);
  this->OrientationMarkerAssembly->AddPart(horizontalOrientationTextActor);

  vtkSmartPointer<vtkTextActor3D> verticalOrientationTextActor = vtkSmartPointer<vtkTextActor3D>::New();
  verticalOrientationTextActor->GetTextProperty()->SetColor(0.0, 1.0, 0.0);
  verticalOrientationTextActor->GetTextProperty()->SetFontFamilyToArial();
  verticalOrientationTextActor->GetTextProperty()->SetFontSize(16);
  verticalOrientationTextActor->GetTextProperty()->SetJustificationToLeft();
  verticalOrientationTextActor->GetTextProperty()->SetVerticalJustificationToTop();
  verticalOrientationTextActor->GetTextProperty()->BoldOn(); 
  verticalOrientationTextActor->SetInput("F");
  verticalOrientationTextActor->RotateWXYZ(180.0, 1.0, 0.0, 0.0); 
  verticalOrientationTextActor->SetPosition(9.0, 45.0, -1.0);
  this->SetVerticalOrientationTextActor(verticalOrientationTextActor);
  this->OrientationMarkerAssembly->AddPart(verticalOrientationTextActor);

  vtkSmartPointer<vtkActor> horizontalLineActor = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkPolyDataMapper> horizontalLineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkLineSource> horizontalLineSource = vtkSmartPointer<vtkLineSource>::New();
  horizontalLineSource->SetPoint1(5.0, 5.0, -0.5);
  horizontalLineSource->SetPoint2(50.0, 5.0, -0.5);
  horizontalLineMapper->SetInputConnection(horizontalLineSource->GetOutputPort());
  horizontalLineActor->GetProperty()->SetColor(0.0, 1.0, 0.0);
  horizontalLineActor->SetMapper(horizontalLineMapper);
  this->OrientationMarkerAssembly->AddPart(horizontalLineActor);

  vtkSmartPointer<vtkActor> verticalLineActor = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkPolyDataMapper> verticalLineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkLineSource> verticalLineSource = vtkSmartPointer<vtkLineSource>::New();
  verticalLineSource->SetPoint1(5.0, 5.0, -0.5);
  verticalLineSource->SetPoint2(5.0, 50.0, -0.5);
  verticalLineMapper->SetInputConnection(verticalLineSource->GetOutputPort());
  verticalLineActor->GetProperty()->SetColor(0.0, 1.0, 0.0);
  verticalLineActor->SetMapper(verticalLineMapper);
  this->OrientationMarkerAssembly->AddPart(verticalLineActor);

  vtkSmartPointer<vtkActor> horizontalConeActor = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkPolyDataMapper> horizontalConeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkConeSource> horizontalConeSource = vtkSmartPointer<vtkConeSource>::New();
  horizontalConeSource->SetHeight(15.0);
  horizontalConeSource->SetRadius(5.0);
  horizontalConeMapper->SetInputConnection(horizontalConeSource->GetOutputPort());
  horizontalConeActor->GetProperty()->SetColor(0.0, 1.0, 0.0);
  horizontalConeActor->RotateWXYZ(90.0, 1.0, 0.0, 0.0); 
  horizontalConeActor->SetPosition(56.0, 5.0, -0.5);
  horizontalConeActor->SetMapper(horizontalConeMapper);
  this->OrientationMarkerAssembly->AddPart(horizontalConeActor);

  vtkSmartPointer<vtkActor> verticalConeActor = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkPolyDataMapper> verticalConeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkConeSource> verticalConeSource = vtkSmartPointer<vtkConeSource>::New();
  verticalConeSource->SetHeight(15.0);
  verticalConeSource->SetRadius(5.0);
  verticalConeMapper->SetInputConnection(verticalConeSource->GetOutputPort());
  verticalConeActor->GetProperty()->SetColor(0.0, 1.0, 0.0);
  verticalConeActor->RotateWXYZ(90.0, 1.0, 0.0, 0.0); 
  verticalConeActor->RotateWXYZ(90.0, 0.0, 0.0, 1.0); 
  verticalConeActor->SetPosition(5.0, 56.0, -0.5);
  verticalConeActor->SetMapper(verticalConeMapper);
  this->OrientationMarkerAssembly->AddPart(verticalConeActor);

  this->GetCanvasRenderer()->AddActor(this->OrientationMarkerAssembly);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkObjectVisualizer::CalculateOrientationMarkerTransform( US_IMAGE_ORIENTATION aOrientation /*= US_IMG_ORIENT_MF*/ )
{
  int dimensions[2];
  this->DataCollector->GetFrameSize(dimensions);
  double newPosition[3];

  // Undo any existing rotations to the markers, so that all subsequent rotations are from a base orientation
  // Base orientation is MF_SCREEN_RIGHT_DOWN
  this->GetOrientationMarkerAssembly()->RotateX(-OrientationMarkerCurrentXRotation);
  this->GetOrientationMarkerAssembly()->RotateY(-OrientationMarkerCurrentYRotation);

  // Apply any necessary rotations and repositionings
  // Also change the letters on the display to indicate the new orientation
  switch(aOrientation)
  {
  case US_IMG_ORIENT_MF:
    this->GetHorizontalOrientationTextActor()->SetInput("M");
    this->GetVerticalOrientationTextActor()->SetInput("F");
    OrientationMarkerCurrentXRotation = 0;
    OrientationMarkerCurrentYRotation = 0;
    newPosition[0] = 0.0;
    newPosition[1] = 0.0;
    newPosition[2] = -1.0;
    break;
  case US_IMG_ORIENT_MN:
    this->GetHorizontalOrientationTextActor()->SetInput("M");
    this->GetVerticalOrientationTextActor()->SetInput("N");
    OrientationMarkerCurrentYRotation = 0;
    this->GetOrientationMarkerAssembly()->RotateX(180);
    OrientationMarkerCurrentXRotation = 180;
    newPosition[0] = 0.0;
    newPosition[1] = dimensions[1];
    newPosition[2] = 1.0;
    break;
  case US_IMG_ORIENT_UN:
    this->GetHorizontalOrientationTextActor()->SetInput("U");
    this->GetVerticalOrientationTextActor()->SetInput("N");
    this->GetOrientationMarkerAssembly()->RotateX(180);
    OrientationMarkerCurrentXRotation = 180.0;
    this->GetOrientationMarkerAssembly()->RotateY(180);
    OrientationMarkerCurrentYRotation = 180.0;
    newPosition[0] = dimensions[0];
    newPosition[1] = dimensions[1];
    newPosition[2] = -1.0;
    break;
  case US_IMG_ORIENT_UF:
    this->GetHorizontalOrientationTextActor()->SetInput("U");
    this->GetVerticalOrientationTextActor()->SetInput("F");
    this->GetOrientationMarkerAssembly()->RotateY(180);
    OrientationMarkerCurrentYRotation = 180.0;
    OrientationMarkerCurrentXRotation = 0;
    newPosition[0] = dimensions[0];
    newPosition[1] = 0.0;
    newPosition[2] = 1.0;
    break;
  }
  this->GetOrientationMarkerAssembly()->SetPosition(newPosition[0], newPosition[1], newPosition[2]);

  return PLUS_SUCCESS;
}
