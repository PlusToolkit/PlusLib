/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "vtkAssembly.h"
#include "vtkCamera.h"
#include "vtkConeSource.h"
#include "vtkGlyph3D.h"
#include "vtkImageActor.h"
#include "vtkImageVisualizer.h"
#include "vtkLineSource.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkSphereSource.h"
#include "vtkTextActor3D.h"
#include "vtkTextProperty.h"

//-----------------------------------------------------------------------------
vtkStandardNewMacro(vtkImageVisualizer);
//-----------------------------------------------------------------------------

const double vtkImageVisualizer::IMAGE_CAMERA_Z_POSITION = -10.0;  // value here must be greater in magnitude than any other Z coordinate that needs to be displayed
double vtkImageVisualizer::HORIZONTAL_TEXT_ORIENTATION_MARKER_OFFSET[3] = {30.0, 17.0, -1.0};
double vtkImageVisualizer::VERTICAL_TEXT_ORIENTATION_MARKER_OFFSET[3] = {4.0, 40.0, -1.0};
double vtkImageVisualizer::ORIENTATION_MARKER_COLOUR[3] = {0.0, 1.0, 0.0};
double vtkImageVisualizer::HORIZONTAL_LINE_ORIENTATION_MARKER_END_POINT[3] = {45.0, 0.0, -0.5};
double vtkImageVisualizer::VERTICAL_LINE_ORIENTATION_MARKER_END_POINT[3] = {0.0, 45.0, -0.5};
double vtkImageVisualizer::ORIENTATION_MARKER_ASSEMBLY_POSITION[3] = {5.0, 5.0, -1.0};
const double vtkImageVisualizer::ORIENTATION_MARKER_CONE_RADIUS = 5.0;
const double vtkImageVisualizer::ORIENTATION_MARKER_CONE_HEIGHT = 15.0;
double vtkImageVisualizer::HORIZONTAL_CONE_ORIENTATION_MARKER_OFFSET[3] = {51.0, 0.0, -0.5};
double vtkImageVisualizer::VERTICAL_CONE_ORIENTATION_MARKER_OFFSET[3] = {0.0, 51.0, -0.5};

//-----------------------------------------------------------------------------

vtkImageVisualizer::vtkImageVisualizer()
: CanvasRenderer(NULL)
, DataCollector(NULL)
, ResultPolyData(NULL)
, ResultActor(NULL)
, ImageActor(NULL)
, ImageCamera(NULL)
, HorizontalOrientationTextActor(NULL)
, VerticalOrientationTextActor(NULL)
, OrientationMarkerAssembly(NULL)
, OrientationMarkerCurrentXRotation(0.0)
, OrientationMarkerCurrentYRotation(0.0)
, Initialized(false)
{

}

//-----------------------------------------------------------------------------

vtkImageVisualizer::~vtkImageVisualizer()
{
  this->SetResultActor(NULL);
  this->SetResultPolyData(NULL);
  this->SetCanvasRenderer(NULL);
  this->SetImageActor(NULL);
  this->SetImageCamera(NULL);
  this->SetOrientationMarkerAssembly(NULL);
  this->SetHorizontalOrientationTextActor(NULL);
  this->SetVerticalOrientationTextActor(NULL);
}

//-----------------------------------------------------------------------------

PlusStatus vtkImageVisualizer::Initialize(vtkSmartPointer<vtkDataCollector> aCollector, vtkSmartPointer<vtkPolyData> aResultPolyData)
{
  LOG_TRACE("vtkImageVisualizer::Initialize");

  if( aCollector == NULL )
  {
    LOG_ERROR("Data collector not initialized. Data collection must be present.");
    return PLUS_FAIL;
  }

  // Store a reference to the data collector
  this->SetDataCollector(aCollector);

  if (this->DataCollector->GetConnected() == false)
  {
    LOG_ERROR("Data collection not initialized or device visualization cannot be initialized unless they are connected");
    return PLUS_FAIL;
  }

  // Set up canvas renderer
  vtkSmartPointer<vtkRenderer> canvasRenderer = vtkSmartPointer<vtkRenderer>::New(); 
  canvasRenderer->SetBackground(0.1, 0.1, 0.1);
  canvasRenderer->SetBackground2(0.4, 0.4, 0.4);
  canvasRenderer->SetGradientBackground(false);
  this->SetCanvasRenderer(canvasRenderer);

  // Result points poly data
  this->SetResultPolyData(aResultPolyData);

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

  // Create image actor
  vtkSmartPointer<vtkImageActor> imageActor = vtkSmartPointer<vtkImageActor>::New();
  this->SetImageActor(imageActor);

  // Add actors to the renderer
  this->CanvasRenderer->AddActor(this->ResultActor);
  this->CanvasRenderer->AddActor(this->ImageActor);

  if (InitializeOrientationMarkers() != PLUS_SUCCESS)
  {
    LOG_ERROR("Error during creation of orientation marker actors.");
    return PLUS_FAIL;
  }

  this->InitializedOn();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkImageVisualizer::InitializeOrientationMarkers()
{
  this->SetOrientationMarkerAssembly(vtkSmartPointer<vtkAssembly>::New());

  // Since the internal orientation is always MF, display the indicators for MF in all cases
  vtkSmartPointer<vtkTextActor3D> horizontalOrientationTextActor = vtkSmartPointer<vtkTextActor3D>::New();
  horizontalOrientationTextActor->GetTextProperty()->SetColor(vtkImageVisualizer::ORIENTATION_MARKER_COLOUR);
  horizontalOrientationTextActor->GetTextProperty()->SetFontFamilyToArial();
  horizontalOrientationTextActor->GetTextProperty()->SetFontSize(16);
  horizontalOrientationTextActor->GetTextProperty()->SetJustificationToLeft();
  horizontalOrientationTextActor->GetTextProperty()->SetVerticalJustificationToTop();
  horizontalOrientationTextActor->GetTextProperty()->BoldOn(); 
  horizontalOrientationTextActor->SetInput("M");
  horizontalOrientationTextActor->RotateWXYZ(180.0, 1.0, 0.0, 0.0);
  horizontalOrientationTextActor->SetPosition(vtkImageVisualizer::HORIZONTAL_TEXT_ORIENTATION_MARKER_OFFSET);
  this->SetHorizontalOrientationTextActor(horizontalOrientationTextActor);
  this->OrientationMarkerAssembly->AddPart(horizontalOrientationTextActor);

  vtkSmartPointer<vtkTextActor3D> verticalOrientationTextActor = vtkSmartPointer<vtkTextActor3D>::New();
  verticalOrientationTextActor->GetTextProperty()->SetColor(vtkImageVisualizer::ORIENTATION_MARKER_COLOUR);
  verticalOrientationTextActor->GetTextProperty()->SetFontFamilyToArial();
  verticalOrientationTextActor->GetTextProperty()->SetFontSize(16);
  verticalOrientationTextActor->GetTextProperty()->SetJustificationToLeft();
  verticalOrientationTextActor->GetTextProperty()->SetVerticalJustificationToTop();
  verticalOrientationTextActor->GetTextProperty()->BoldOn(); 
  verticalOrientationTextActor->SetInput("F");
  verticalOrientationTextActor->RotateWXYZ(180.0, 1.0, 0.0, 0.0); 
  verticalOrientationTextActor->SetPosition(vtkImageVisualizer::VERTICAL_TEXT_ORIENTATION_MARKER_OFFSET);
  this->SetVerticalOrientationTextActor(verticalOrientationTextActor);
  this->OrientationMarkerAssembly->AddPart(verticalOrientationTextActor);

  vtkSmartPointer<vtkActor> horizontalLineActor = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkPolyDataMapper> horizontalLineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkLineSource> horizontalLineSource = vtkSmartPointer<vtkLineSource>::New();
  horizontalLineSource->SetPoint1(0.0, 0.0, -0.5);
  horizontalLineSource->SetPoint2(vtkImageVisualizer::HORIZONTAL_LINE_ORIENTATION_MARKER_END_POINT);
  horizontalLineMapper->SetInputConnection(horizontalLineSource->GetOutputPort());
  horizontalLineActor->GetProperty()->SetColor(vtkImageVisualizer::ORIENTATION_MARKER_COLOUR);
  horizontalLineActor->SetMapper(horizontalLineMapper);
  this->OrientationMarkerAssembly->AddPart(horizontalLineActor);

  vtkSmartPointer<vtkActor> verticalLineActor = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkPolyDataMapper> verticalLineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkLineSource> verticalLineSource = vtkSmartPointer<vtkLineSource>::New();
  verticalLineSource->SetPoint1(0.0, 0.0, -0.5);
  verticalLineSource->SetPoint2(vtkImageVisualizer::VERTICAL_LINE_ORIENTATION_MARKER_END_POINT);
  verticalLineMapper->SetInputConnection(verticalLineSource->GetOutputPort());
  verticalLineActor->GetProperty()->SetColor(vtkImageVisualizer::ORIENTATION_MARKER_COLOUR);
  verticalLineActor->SetMapper(verticalLineMapper);
  this->OrientationMarkerAssembly->AddPart(verticalLineActor);

  vtkSmartPointer<vtkActor> horizontalConeActor = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkPolyDataMapper> horizontalConeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkConeSource> horizontalConeSource = vtkSmartPointer<vtkConeSource>::New();
  horizontalConeSource->SetHeight(vtkImageVisualizer::ORIENTATION_MARKER_CONE_HEIGHT);
  horizontalConeSource->SetRadius(vtkImageVisualizer::ORIENTATION_MARKER_CONE_RADIUS);
  horizontalConeMapper->SetInputConnection(horizontalConeSource->GetOutputPort());
  horizontalConeActor->GetProperty()->SetColor(vtkImageVisualizer::ORIENTATION_MARKER_COLOUR);
  horizontalConeActor->RotateWXYZ(90.0, 1.0, 0.0, 0.0); 
  horizontalConeActor->SetPosition(vtkImageVisualizer::HORIZONTAL_CONE_ORIENTATION_MARKER_OFFSET);
  horizontalConeActor->SetMapper(horizontalConeMapper);
  this->OrientationMarkerAssembly->AddPart(horizontalConeActor);

  vtkSmartPointer<vtkActor> verticalConeActor = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkPolyDataMapper> verticalConeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkConeSource> verticalConeSource = vtkSmartPointer<vtkConeSource>::New();
  verticalConeSource->SetHeight(vtkImageVisualizer::ORIENTATION_MARKER_CONE_HEIGHT);
  verticalConeSource->SetRadius(vtkImageVisualizer::ORIENTATION_MARKER_CONE_RADIUS);
  verticalConeMapper->SetInputConnection(verticalConeSource->GetOutputPort());
  verticalConeActor->GetProperty()->SetColor(vtkImageVisualizer::ORIENTATION_MARKER_COLOUR);
  verticalConeActor->RotateWXYZ(90.0, 1.0, 0.0, 0.0); 
  verticalConeActor->RotateWXYZ(90.0, 0.0, 0.0, 1.0); 
  verticalConeActor->SetPosition(vtkImageVisualizer::VERTICAL_CONE_ORIENTATION_MARKER_OFFSET);
  verticalConeActor->SetMapper(verticalConeMapper);
  this->OrientationMarkerAssembly->AddPart(verticalConeActor);

  this->GetCanvasRenderer()->AddActor(this->OrientationMarkerAssembly);

  this->OrientationMarkerAssembly->SetPosition(vtkImageVisualizer::ORIENTATION_MARKER_ASSEMBLY_POSITION);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkImageVisualizer::UpdateOrientationMarkerTransformPosition( US_IMAGE_ORIENTATION aOrientation /*= US_IMG_ORIENT_MF*/ )
{
  int dimensions[2];
  this->DataCollector->GetFrameSize(dimensions);
  double newPosition[3];

  // Undo any existing rotations to the markers, so that all subsequent rotations are from a base orientation
  // Base orientation is MF_SCREEN_RIGHT_DOWN
  this->GetOrientationMarkerAssembly()->RotateX(-OrientationMarkerCurrentXRotation);
  this->GetOrientationMarkerAssembly()->RotateY(-OrientationMarkerCurrentYRotation);

  // Apply any necessary rotations and repositioning
  // Also change the letters on the display to indicate the new orientation
  switch(aOrientation)
  {
  case US_IMG_ORIENT_MF:
    this->GetHorizontalOrientationTextActor()->SetInput("M");
    this->GetVerticalOrientationTextActor()->SetInput("F");
    OrientationMarkerCurrentXRotation = 0;
    OrientationMarkerCurrentYRotation = 0;
    newPosition[0] = vtkImageVisualizer::ORIENTATION_MARKER_ASSEMBLY_POSITION[0];
    newPosition[1] = vtkImageVisualizer::ORIENTATION_MARKER_ASSEMBLY_POSITION[1];
    newPosition[2] = vtkImageVisualizer::ORIENTATION_MARKER_ASSEMBLY_POSITION[2];
    break;
  case US_IMG_ORIENT_MN:
    this->GetHorizontalOrientationTextActor()->SetInput("M");
    this->GetVerticalOrientationTextActor()->SetInput("N");
    OrientationMarkerCurrentYRotation = 0;
    this->GetOrientationMarkerAssembly()->RotateX(180);
    OrientationMarkerCurrentXRotation = 180;
    newPosition[0] = vtkImageVisualizer::ORIENTATION_MARKER_ASSEMBLY_POSITION[0];
    newPosition[1] = dimensions[1] - vtkImageVisualizer::ORIENTATION_MARKER_ASSEMBLY_POSITION[1];
    newPosition[2] = -vtkImageVisualizer::ORIENTATION_MARKER_ASSEMBLY_POSITION[2];
    break;
  case US_IMG_ORIENT_UN:
    this->GetHorizontalOrientationTextActor()->SetInput("U");
    this->GetVerticalOrientationTextActor()->SetInput("N");
    this->GetOrientationMarkerAssembly()->RotateX(180);
    OrientationMarkerCurrentXRotation = 180.0;
    this->GetOrientationMarkerAssembly()->RotateY(180);
    OrientationMarkerCurrentYRotation = 180.0;
    newPosition[0] = dimensions[0] - vtkImageVisualizer::ORIENTATION_MARKER_ASSEMBLY_POSITION[0];
    newPosition[1] = dimensions[1] - vtkImageVisualizer::ORIENTATION_MARKER_ASSEMBLY_POSITION[1];
    newPosition[2] = vtkImageVisualizer::ORIENTATION_MARKER_ASSEMBLY_POSITION[2];
    break;
  case US_IMG_ORIENT_UF:
    this->GetHorizontalOrientationTextActor()->SetInput("U");
    this->GetVerticalOrientationTextActor()->SetInput("F");
    this->GetOrientationMarkerAssembly()->RotateY(180);
    OrientationMarkerCurrentYRotation = 180.0;
    OrientationMarkerCurrentXRotation = 0;
    newPosition[0] = dimensions[0] - vtkImageVisualizer::ORIENTATION_MARKER_ASSEMBLY_POSITION[0];
    newPosition[1] = vtkImageVisualizer::ORIENTATION_MARKER_ASSEMBLY_POSITION[1];
    newPosition[2] = -vtkImageVisualizer::ORIENTATION_MARKER_ASSEMBLY_POSITION[2];
    break;
  }
  this->GetOrientationMarkerAssembly()->SetPosition(newPosition[0], newPosition[1], newPosition[2]);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkImageVisualizer::CalculateCameraParameters()
{
  LOG_TRACE("vtkImageVisualizer::CalculateImageCameraParameters");

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
  if( this->CanvasRenderer->GetRenderWindow() == NULL)
  {
    LOG_ERROR("Render window unavailable when trying to render 2D image.");
    return PLUS_FAIL;
  }
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

  imageCamera->SetPosition(imageCenterX, imageCenterY, vtkImageVisualizer::IMAGE_CAMERA_Z_POSITION);

  // Set camera
  this->SetImageCamera(imageCamera);

  this->CanvasRenderer->SetActiveCamera(this->ImageCamera);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkImageVisualizer::SetScreenRightDownAxesOrientation( US_IMAGE_ORIENTATION aOrientation /*= US_IMG_ORIENT_MF*/ )
{
  LOG_TRACE("vtkImageVisualizer::SetScreenRightDownAxesOrientation(" << aOrientation << ")");

  double cameraPos[3] = {0.0, 0.0, 0.0};
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
    }
    break;
  }

  return UpdateOrientationMarkerTransformPosition(aOrientation);
}

//-----------------------------------------------------------------------------

PlusStatus vtkImageVisualizer::ShowOrientationMarkers( bool aShow )
{
  vtkSmartPointer<vtkPropCollection> collection = vtkSmartPointer<vtkPropCollection>::New();
  this->OrientationMarkerAssembly->GetActors(collection);
  collection->InitTraversal();

  if( !aShow )
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

PlusStatus vtkImageVisualizer::HideAll()
{
  this->ResultActor->VisibilityOff();
  this->ImageActor->VisibilityOff();

  this->CanvasRenderer->Modified();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkImageVisualizer::ShowResult(bool aOn)
{
  this->ResultActor->SetVisibility(aOn);
  this->CanvasRenderer->Modified();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkImageVisualizer::SetResultColor(double r, double g, double b)
{
  this->ResultActor->GetProperty()->SetColor(r, g, b);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkImageVisualizer::SetResultOpacity(double aOpacity)
{
  this->ResultActor->GetProperty()->SetOpacity(aOpacity);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void vtkImageVisualizer::SetInput( vtkSmartPointer<vtkImageData> aImage )
{
  this->GetImageActor()->SetInput(aImage);
}
