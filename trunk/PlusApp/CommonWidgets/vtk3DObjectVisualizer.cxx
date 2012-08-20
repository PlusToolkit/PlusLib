/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "TrackedFrame.h"

#include "vtkGlyph3D.h"
#include "vtkObjectFactory.h"
#include "vtk3DObjectVisualizer.h"
#include "vtkDisplayableObject.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkSmartPointer.h"
#include "vtkSphereSource.h"

//-----------------------------------------------------------------------------
vtkStandardNewMacro(vtk3DObjectVisualizer);
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

vtk3DObjectVisualizer::vtk3DObjectVisualizer()
: DataCollector(NULL)
, CanvasRenderer(NULL)
, ImageActor(NULL)
, InputPolyData(NULL)
, InputActor(NULL)
, InputGlyph(NULL)
, ResultPolyData(NULL)
, ResultActor(NULL)
, ResultGlyph(NULL)
, VolumeActor(NULL)
, WorldCoordinateFrame(NULL)
, TransformRepository(NULL)
{
  // Set up canvas renderer
  vtkSmartPointer<vtkRenderer> canvasRenderer = vtkSmartPointer<vtkRenderer>::New(); 
  canvasRenderer->SetBackground(0.1, 0.1, 0.1);
  canvasRenderer->SetBackground2(0.4, 0.4, 0.4);
  canvasRenderer->SetGradientBackground(true);
  this->SetCanvasRenderer(canvasRenderer);

  // Input points actor
  vtkSmartPointer<vtkActor> inputActor = vtkSmartPointer<vtkActor>::New();
  this->SetInputActor(inputActor);
  vtkSmartPointer<vtkPolyDataMapper> inputMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkGlyph3D> inputGlyph = vtkSmartPointer<vtkGlyph3D>::New();
  this->SetInputGlyph(inputGlyph);
  vtkSmartPointer<vtkSphereSource> inputSphereSource = vtkSmartPointer<vtkSphereSource>::New();
  inputSphereSource->SetRadius(1.5); // mm

  // Connect all input items (except poly data) in chain
  this->InputGlyph->SetSourceConnection(inputSphereSource->GetOutputPort());  
  inputMapper->SetInputConnection(this->InputGlyph->GetOutputPort());
  this->InputActor->SetMapper(inputMapper);
  this->InputActor->GetProperty()->SetColor(0.0, 0.7, 1.0);

  // Result points actor
  vtkSmartPointer<vtkActor> resultActor = vtkSmartPointer<vtkActor>::New();
  this->SetResultActor(resultActor);
  vtkSmartPointer<vtkPolyDataMapper> resultMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkGlyph3D> resultGlyph = vtkSmartPointer<vtkGlyph3D>::New();
  this->SetResultGlyph(resultGlyph);
  vtkSmartPointer<vtkSphereSource> resultSphereSource = vtkSmartPointer<vtkSphereSource>::New();
  resultSphereSource->SetRadius(3.0); // mm

  // Connect all result items (except poly data) in chain
  this->ResultGlyph->SetSourceConnection(resultSphereSource->GetOutputPort());
  resultMapper->SetInputConnection(resultGlyph->GetOutputPort());
  this->ResultActor->SetMapper(resultMapper);
  this->ResultActor->GetProperty()->SetColor(0.0, 0.8, 0.0);

  // Volume actor
  vtkSmartPointer<vtkActor> volumeActor = vtkSmartPointer<vtkActor>::New();
  this->SetVolumeActor(volumeActor);

  // Create image actor
  vtkSmartPointer<vtkImageActor> imageActor = vtkSmartPointer<vtkImageActor>::New();
  this->SetImageActor(imageActor);

  this->CanvasRenderer->AddActor(this->InputActor);
  this->CanvasRenderer->AddActor(this->ResultActor);
  this->CanvasRenderer->AddActor(this->VolumeActor);
}

//-----------------------------------------------------------------------------

vtk3DObjectVisualizer::~vtk3DObjectVisualizer()
{
  ClearDisplayableObjects();

  this->SetResultActor(NULL);
  this->SetResultPolyData(NULL);
  this->SetCanvasRenderer(NULL);
  this->SetImageActor(NULL);
  this->SetInputPolyData(NULL);
  this->SetInputActor(NULL);
  this->SetVolumeActor(NULL);
  this->SetWorldCoordinateFrame(NULL);
  this->SetTransformRepository(NULL);
}

//-----------------------------------------------------------------------------

PlusStatus vtk3DObjectVisualizer::Update()
{
  // If none of the objects are displayable then return with fail
  bool noObjectsToDisplay = true;
  for (std::vector<vtkDisplayableObject*>::iterator it = this->DisplayableObjects.begin(); it != this->DisplayableObjects.end(); ++it)
  {
    vtkDisplayableObject* displayableObject = *it;
    if ( displayableObject->IsDisplayable() && displayableObject->GetActor() && displayableObject->GetActor()->GetVisibility() > 0 )
    {
      noObjectsToDisplay = false;
      break;
    }
  }

  if (noObjectsToDisplay)
  {
    return PLUS_SUCCESS;
  }

  if( this->DataCollector == NULL )
  {
    // With new member initialization flexibility, this is an acceptable fail... return PLUS_SUCCESS?
    //LOG_WARNING("Update called with no data collector.");
    return PLUS_FAIL;
  }

  // Get tracked frame and set the transforms
  TrackedFrame trackedFrame; 
  if ( this->DataCollector->GetTrackedFrame(&trackedFrame) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get tracked frame!"); 
    return PLUS_FAIL; 
  }

  if( this->TransformRepository == NULL )
  {
    // With new member initialization flexibility, this is an acceptable fail... return PLUS_SUCCESS?
    //LOG_WARNING("Update called with no transform repository.");
    return PLUS_FAIL;
  }
  if ( this->TransformRepository->SetTransforms(trackedFrame) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set current transforms to transform repository!"); 
    return PLUS_FAIL; 
  }

  bool resetCameraNeeded = false;

  // Update actors of displayable objects
  for (std::vector<vtkDisplayableObject*>::iterator it = this->DisplayableObjects.begin(); it != this->DisplayableObjects.end(); ++it)
  {
    vtkDisplayableObject* displayableObject = *it;
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

PlusStatus vtk3DObjectVisualizer::ClearDisplayableObjects()
{
  LOG_TRACE("vtkPerspectiveVisualizer::ClearDisplayableObjects");

  for (std::vector<vtkDisplayableObject*>::iterator it = this->DisplayableObjects.begin(); it != this->DisplayableObjects.end(); ++it)
  {
    vtkDisplayableObject* tool = *it;
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

PlusStatus vtk3DObjectVisualizer::ShowAllObjects(bool aOn)
{
  LOG_TRACE("vtkPerspectiveVisualizer::ShowAllObjects(" << (aOn?"true":"false") << ")");

  for (std::vector<vtkDisplayableObject*>::iterator it = this->DisplayableObjects.begin(); it != this->DisplayableObjects.end(); ++it)
  {
    vtkDisplayableObject* displayableObject = *it;
    if ((displayableObject != NULL) && (displayableObject->GetActor() != NULL))
    {
      displayableObject->GetActor()->SetVisibility(aOn);
    }
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtk3DObjectVisualizer::ShowObjectsByCoordinateFrame(const char* aObjectCoordinateFrame, bool aOn)
{
  LOG_TRACE("vtkPerspectiveVisualizer::ShowObjectsByCoordinateFrame(" << aObjectCoordinateFrame << ", " << (aOn?"true":"false") << ")");

  std::vector<vtkDisplayableModel*> objects = this->GetDisplayableObjects<vtkDisplayableModel>(aObjectCoordinateFrame);
  for( std::vector<vtkDisplayableModel*>::iterator it = objects.begin(); it != objects.end(); ++it)
  {
    (*it)->GetActor()->SetVisibility(aOn);
  }

  this->CanvasRenderer->Modified();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtk3DObjectVisualizer::ShowInput(bool aOn)
{
  this->InputActor->SetVisibility(aOn);
  this->CanvasRenderer->Modified();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtk3DObjectVisualizer::ShowResult(bool aOn)
{
  this->ResultActor->SetVisibility(aOn);
  this->CanvasRenderer->Modified();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtk3DObjectVisualizer::HideAll()
{
  this->InputActor->VisibilityOff();
  this->ResultActor->VisibilityOff();
  this->VolumeActor->VisibilityOff();
  this->ImageActor->VisibilityOff();

  ShowAllObjects(false);

  this->CanvasRenderer->Modified();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtk3DObjectVisualizer::AssignInputPolyData(vtkPolyData* aInputPolyData )
{
  if( aInputPolyData != NULL )
  {
    this->SetInputPolyData(aInputPolyData);

    this->InputGlyph->SetInputConnection(this->InputPolyData->GetProducerPort());
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtk3DObjectVisualizer::AssignResultPolyData(vtkPolyData* aResultPolyData )
{
  if( aResultPolyData != NULL )
  {
    this->SetResultPolyData(aResultPolyData);

    this->ResultGlyph->SetInputConnection(this->ResultPolyData->GetProducerPort());
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtk3DObjectVisualizer::AssignTransformRepository(vtkTransformRepository* aTransformRepository )
{
  LOG_TRACE("vtk3DObjectVisualizer::SetTransformRepository");

  if( aTransformRepository != NULL )
  {
    if( this->DataCollector == NULL )
    {
      LOG_ERROR("Data collector not initialized. Data collection must be present before transform repository is set.");
      return PLUS_FAIL;
    }

    if (this->DataCollector->GetConnected() == false)
    {
      LOG_ERROR("Device visualization cannot be initialized unless they are connected");
      return PLUS_FAIL;
    }

    TrackedFrame trackedFrame;
    if (this->DataCollector->GetTrackedFrame(&trackedFrame) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to get tracked frame from data collector");
      return PLUS_FAIL;
    }

    if (this->DataCollector->GetTrackingDataAvailable() == false)
    {
      LOG_WARNING("No tracking data is available");
    }

    this->SetTransformRepository(aTransformRepository);
    this->TransformRepository->SetTransforms(trackedFrame);
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtk3DObjectVisualizer::AssignDataCollector(vtkDataCollector* aCollector )
{
  LOG_TRACE("vtk3DObjectVisualizer::AssignDataCollector");

  this->SetDataCollector(aCollector);

  if( aCollector != NULL )
  {
    if (this->DataCollector->GetConnected() == false)
    {
      LOG_ERROR("Data collection not initialized or device visualization cannot be initialized unless they are connected");
      return PLUS_FAIL;
    }

    // Connect data collector to image actor
    if (this->DataCollector->GetVideoEnabled())
    {
      this->ImageActor->VisibilityOn();
      this->ImageActor->SetInput(this->DataCollector->GetBrightnessOutput());
    }
    else
    {
      LOG_INFO("Data collector has no video output, cannot initialize image actor");
    }
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtk3DObjectVisualizer::ReadConfiguration(vtkXMLDataElement* aXMLElement)
{
  // Rendering section
  vtkXMLDataElement* renderingElement = aXMLElement->FindNestedElementWithName("Rendering"); 

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

    this->DisplayableObjects.push_back(displayableObject);
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

  // Add displayable object actors to renderer
  for (std::vector<vtkDisplayableObject*>::iterator it = this->DisplayableObjects.begin(); it != this->DisplayableObjects.end(); ++it)
  {
    vtkDisplayableObject* displayableObject = *it;
    if (displayableObject == NULL)
    {
      LOG_ERROR("Invalid displayable object!");
      continue;
    }

    this->CanvasRenderer->AddActor(displayableObject->GetActor());
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtk3DObjectVisualizer::ShowObjectById( const char* aModelId, bool aOn )
{
  LOG_TRACE("vtk3DObjectVisualizer::ShowObjectById(" << aModelId << ", " << (aOn?"true":"false") << ")");

  for (std::vector<vtkDisplayableObject*>::iterator it = this->DisplayableObjects.begin(); it != this->DisplayableObjects.end(); ++it)
  {
    vtkDisplayableObject* displayableObject = *it;

    if( displayableObject->GetObjectId() != NULL && STRCASECMP(displayableObject->GetObjectId(), aModelId) == 0 )
    {
      displayableObject->GetActor()->SetVisibility(aOn);
    }
  }

  return PLUS_SUCCESS;
}