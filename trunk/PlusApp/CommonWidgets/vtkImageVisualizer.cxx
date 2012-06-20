/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "vtkAssembly.h"
#include "vtkCamera.h"
#include "vtkConeSource.h"
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

static const double MAX_WIDGET_THICKNESS = 10.0;  // maximum thickness of any object in the scene (camera is positioned at -MAX_WIDGET_THICKNESS - 1
static double HORIZONTAL_TEXT_ORIENTATION_MARKER_OFFSET[3] = {30.0, 17.0, -1.0};
static double VERTICAL_TEXT_ORIENTATION_MARKER_OFFSET[3] = {4.0, 40.0, -1.0};
static double ORIENTATION_MARKER_COLOUR[3] = {0.0, 1.0, 0.0};
static double ORIENTATION_MARKER_SIZE = 51.0;
static double ORIENTATION_MARKER_ASSEMBLY_POSITION[3] = {5.0, 5.0, -1.0};
static const double ORIENTATION_MARKER_CONE_RADIUS = 5.0;
static const double ORIENTATION_MARKER_CONE_HEIGHT = 15.0;

//-----------------------------------------------------------------------------

vtkImageVisualizer::vtkImageVisualizer()
: DataCollector(NULL)
, CanvasRenderer(NULL)
, ImageActor(NULL)
, ResultPolyData(NULL)
, ResultActor(NULL)
, ResultGlyph(NULL)
, ImageCamera(NULL)
, OrientationMarkerAssembly(NULL)
, HorizontalOrientationTextActor(NULL)
, VerticalOrientationTextActor(NULL)
, OrientationMarkerCurrentXRotation(0.0)
, OrientationMarkerCurrentYRotation(0.0)
, CurrentMarkerOrientation(US_IMG_ORIENT_MF)
{
  // Set up canvas renderer
  vtkSmartPointer<vtkRenderer> canvasRenderer = vtkSmartPointer<vtkRenderer>::New(); 
  canvasRenderer->SetBackground(0.1, 0.1, 0.1);
  canvasRenderer->SetBackground2(0.4, 0.4, 0.4);
  canvasRenderer->SetGradientBackground(false);
  this->SetCanvasRenderer(canvasRenderer);

  // Create Glyph and Actor
  vtkSmartPointer<vtkActor> resultActor = vtkSmartPointer<vtkActor>::New();
  this->SetResultActor(resultActor);
  vtkSmartPointer<vtkGlyph3D> resultGlyph = vtkSmartPointer<vtkGlyph3D>::New();
  this->SetResultGlyph(resultGlyph);
  vtkSmartPointer<vtkPolyDataMapper> resultMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkSphereSource> resultSphereSource = vtkSmartPointer<vtkSphereSource>::New();
  resultSphereSource->SetRadius(3.0); // mm

  this->ResultGlyph->SetSourceConnection(resultSphereSource->GetOutputPort());
  resultMapper->SetInputConnection(this->ResultGlyph->GetOutputPort());
  this->ResultActor->SetMapper(resultMapper);
  this->ResultActor->GetProperty()->SetColor(0.0, 0.8, 0.0);

  // Create image actor
  vtkSmartPointer<vtkImageActor> imageActor = vtkSmartPointer<vtkImageActor>::New();
  this->SetImageActor(imageActor);

  // Add actors to the renderer
  this->CanvasRenderer->AddActor(this->ResultActor);
  this->CanvasRenderer->AddActor(this->ImageActor);

  this->InitializeOrientationMarkers();
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

PlusStatus vtkImageVisualizer::InitializeOrientationMarkers()
{
  this->SetOrientationMarkerAssembly(vtkSmartPointer<vtkAssembly>::New());

  // Since the internal orientation is always MF, display the indicators for MF in all cases
  vtkSmartPointer<vtkTextActor3D> horizontalOrientationTextActor = vtkSmartPointer<vtkTextActor3D>::New();
  horizontalOrientationTextActor->GetTextProperty()->SetColor(ORIENTATION_MARKER_COLOUR);
  horizontalOrientationTextActor->GetTextProperty()->SetFontFamilyToArial();
  horizontalOrientationTextActor->GetTextProperty()->SetFontSize(16);
  horizontalOrientationTextActor->GetTextProperty()->SetJustificationToLeft();
  horizontalOrientationTextActor->GetTextProperty()->SetVerticalJustificationToTop();
  horizontalOrientationTextActor->GetTextProperty()->BoldOn(); 
  horizontalOrientationTextActor->SetInput("M");
  horizontalOrientationTextActor->RotateWXYZ(180.0, 1.0, 0.0, 0.0);
  horizontalOrientationTextActor->SetPosition(HORIZONTAL_TEXT_ORIENTATION_MARKER_OFFSET);
  this->SetHorizontalOrientationTextActor(horizontalOrientationTextActor);
  this->OrientationMarkerAssembly->AddPart(horizontalOrientationTextActor);

  vtkSmartPointer<vtkTextActor3D> verticalOrientationTextActor = vtkSmartPointer<vtkTextActor3D>::New();
  verticalOrientationTextActor->GetTextProperty()->SetColor(ORIENTATION_MARKER_COLOUR);
  verticalOrientationTextActor->GetTextProperty()->SetFontFamilyToArial();
  verticalOrientationTextActor->GetTextProperty()->SetFontSize(16);
  verticalOrientationTextActor->GetTextProperty()->SetJustificationToLeft();
  verticalOrientationTextActor->GetTextProperty()->SetVerticalJustificationToTop();
  verticalOrientationTextActor->GetTextProperty()->BoldOn(); 
  verticalOrientationTextActor->SetInput("F");
  verticalOrientationTextActor->RotateWXYZ(180.0, 1.0, 0.0, 0.0); 
  verticalOrientationTextActor->SetPosition(VERTICAL_TEXT_ORIENTATION_MARKER_OFFSET);
  this->SetVerticalOrientationTextActor(verticalOrientationTextActor);
  this->OrientationMarkerAssembly->AddPart(verticalOrientationTextActor);

  vtkSmartPointer<vtkActor> horizontalLineActor = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkPolyDataMapper> horizontalLineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkLineSource> horizontalLineSource = vtkSmartPointer<vtkLineSource>::New();
  horizontalLineSource->SetPoint1(0.0, 0.0, -0.5);
  double horizontal[3] = {1.0, 0.0, -0.5};
  horizontal[0] *= (ORIENTATION_MARKER_SIZE * 0.9);
  horizontalLineSource->SetPoint2(horizontal);
  horizontalLineMapper->SetInputConnection(horizontalLineSource->GetOutputPort());
  horizontalLineActor->GetProperty()->SetColor(ORIENTATION_MARKER_COLOUR);
  horizontalLineActor->SetMapper(horizontalLineMapper);
  this->OrientationMarkerAssembly->AddPart(horizontalLineActor);

  vtkSmartPointer<vtkActor> verticalLineActor = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkPolyDataMapper> verticalLineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkLineSource> verticalLineSource = vtkSmartPointer<vtkLineSource>::New();
  verticalLineSource->SetPoint1(0.0, 0.0, -0.5);
  double vertical[3] = {0.0, 1.0, -0.5};
  vertical[1] *= (ORIENTATION_MARKER_SIZE * 0.9);
  verticalLineSource->SetPoint2(vertical);
  verticalLineMapper->SetInputConnection(verticalLineSource->GetOutputPort());
  verticalLineActor->GetProperty()->SetColor(ORIENTATION_MARKER_COLOUR);
  verticalLineActor->SetMapper(verticalLineMapper);
  this->OrientationMarkerAssembly->AddPart(verticalLineActor);

  vtkSmartPointer<vtkActor> horizontalConeActor = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkPolyDataMapper> horizontalConeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkConeSource> horizontalConeSource = vtkSmartPointer<vtkConeSource>::New();
  horizontalConeSource->SetHeight(ORIENTATION_MARKER_CONE_HEIGHT);
  horizontalConeSource->SetRadius(ORIENTATION_MARKER_CONE_RADIUS);
  horizontalConeMapper->SetInputConnection(horizontalConeSource->GetOutputPort());
  horizontalConeActor->GetProperty()->SetColor(ORIENTATION_MARKER_COLOUR);
  horizontalConeActor->RotateWXYZ(90.0, 1.0, 0.0, 0.0); 
  horizontal[0] = ORIENTATION_MARKER_SIZE;
  horizontalConeActor->SetPosition(horizontal);
  horizontalConeActor->SetMapper(horizontalConeMapper);
  this->OrientationMarkerAssembly->AddPart(horizontalConeActor);

  vtkSmartPointer<vtkActor> verticalConeActor = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkPolyDataMapper> verticalConeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkConeSource> verticalConeSource = vtkSmartPointer<vtkConeSource>::New();
  verticalConeSource->SetHeight(ORIENTATION_MARKER_CONE_HEIGHT);
  verticalConeSource->SetRadius(ORIENTATION_MARKER_CONE_RADIUS);
  verticalConeMapper->SetInputConnection(verticalConeSource->GetOutputPort());
  verticalConeActor->GetProperty()->SetColor(ORIENTATION_MARKER_COLOUR);
  verticalConeActor->RotateWXYZ(90.0, 1.0, 0.0, 0.0); 
  verticalConeActor->RotateWXYZ(90.0, 0.0, 0.0, 1.0); 
  vertical[1] = ORIENTATION_MARKER_SIZE;
  verticalConeActor->SetPosition(vertical);
  verticalConeActor->SetMapper(verticalConeMapper);
  this->OrientationMarkerAssembly->AddPart(verticalConeActor);

  this->GetCanvasRenderer()->AddActor(this->OrientationMarkerAssembly);

  this->OrientationMarkerAssembly->SetPosition(ORIENTATION_MARKER_ASSEMBLY_POSITION);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkImageVisualizer::UpdateOrientationMarkerTransformPosition()
{
  int dimensions[2];
  if( this->DataCollector == NULL )
  {
    LOG_WARNING("Trying to modify vtkImageVisualizer orientation while not connected.");
    return PLUS_FAIL;
  }

  this->DataCollector->GetFrameSize(dimensions);
  double newPosition[3];

  // Undo any existing rotations to the markers, so that all subsequent rotations are from a base orientation
  // Base orientation is MF_SCREEN_RIGHT_DOWN
  this->GetOrientationMarkerAssembly()->RotateX(-OrientationMarkerCurrentXRotation);
  this->GetOrientationMarkerAssembly()->RotateY(-OrientationMarkerCurrentYRotation);

  // Apply any necessary rotations and repositioning
  // Also change the letters on the display to indicate the new orientation
  switch(CurrentMarkerOrientation)
  {
  case US_IMG_ORIENT_MF:
    this->GetHorizontalOrientationTextActor()->SetInput("M");
    this->GetVerticalOrientationTextActor()->SetInput("F");
    OrientationMarkerCurrentXRotation = 0;
    OrientationMarkerCurrentYRotation = 0;
    newPosition[0] = ORIENTATION_MARKER_ASSEMBLY_POSITION[0];
    newPosition[1] = ORIENTATION_MARKER_ASSEMBLY_POSITION[1];
    newPosition[2] = ORIENTATION_MARKER_ASSEMBLY_POSITION[2];
    break;
  case US_IMG_ORIENT_MN:
    this->GetHorizontalOrientationTextActor()->SetInput("M");
    this->GetVerticalOrientationTextActor()->SetInput("N");
    OrientationMarkerCurrentYRotation = 0;
    this->GetOrientationMarkerAssembly()->RotateX(180);
    OrientationMarkerCurrentXRotation = 180;
    newPosition[0] = ORIENTATION_MARKER_ASSEMBLY_POSITION[0];
    newPosition[1] = dimensions[1] - ORIENTATION_MARKER_ASSEMBLY_POSITION[1];
    newPosition[2] = -ORIENTATION_MARKER_ASSEMBLY_POSITION[2];
    break;
  case US_IMG_ORIENT_UN:
    this->GetHorizontalOrientationTextActor()->SetInput("U");
    this->GetVerticalOrientationTextActor()->SetInput("N");
    this->GetOrientationMarkerAssembly()->RotateX(180);
    OrientationMarkerCurrentXRotation = 180.0;
    this->GetOrientationMarkerAssembly()->RotateY(180);
    OrientationMarkerCurrentYRotation = 180.0;
    newPosition[0] = dimensions[0] - ORIENTATION_MARKER_ASSEMBLY_POSITION[0];
    newPosition[1] = dimensions[1] - ORIENTATION_MARKER_ASSEMBLY_POSITION[1];
    newPosition[2] = ORIENTATION_MARKER_ASSEMBLY_POSITION[2];
    break;
  case US_IMG_ORIENT_UF:
    this->GetHorizontalOrientationTextActor()->SetInput("U");
    this->GetVerticalOrientationTextActor()->SetInput("F");
    this->GetOrientationMarkerAssembly()->RotateY(180);
    OrientationMarkerCurrentYRotation = 180.0;
    OrientationMarkerCurrentXRotation = 0;
    newPosition[0] = dimensions[0] - ORIENTATION_MARKER_ASSEMBLY_POSITION[0];
    newPosition[1] = ORIENTATION_MARKER_ASSEMBLY_POSITION[1];
    newPosition[2] = -ORIENTATION_MARKER_ASSEMBLY_POSITION[2];
    break;
  }
  this->GetOrientationMarkerAssembly()->SetPosition(newPosition[0], newPosition[1], newPosition[2]);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkImageVisualizer::UpdateCameraPose()
{
  LOG_TRACE("vtkImageVisualizer::UpdateCameraPose");

  if( this->DataCollector == NULL )
  {
    LOG_WARNING("Trying to update vtkImageVisualizer camera pose while not connected.");
    return PLUS_FAIL;
  }

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
    // If canvas aspect ratio is more elongated in the X position then compute the distance according to the Y axis
    imageCamera->SetParallelScale(imageCenterY);
  }
  else
  {
    imageCamera->SetParallelScale(imageCenterX * (double)renderWindowSize[1] / (double)renderWindowSize[0]);
  }

  imageCamera->SetPosition(imageCenterX, imageCenterY, -MAX_WIDGET_THICKNESS - 1);

  // Set camera
  this->SetImageCamera(imageCamera);

  this->CanvasRenderer->SetActiveCamera(this->ImageCamera);

  // Depending on the desired orientation either roll the camera about its view vector 
  // and/or position the camera on the +z or -z side of the image plane.
  // This depends on having back face culling disabled.
  double cameraPos[3] = {imageCenterX, imageCenterY, -MAX_WIDGET_THICKNESS - 1};
  switch(CurrentMarkerOrientation)
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

  return UpdateOrientationMarkerTransformPosition();
}

//-----------------------------------------------------------------------------

PlusStatus vtkImageVisualizer::SetScreenRightDownAxesOrientation( US_IMAGE_ORIENTATION aOrientation /*= US_IMG_ORIENT_MF*/ )
{
  LOG_TRACE("vtkImageVisualizer::SetScreenRightDownAxesOrientation(" << aOrientation << ")");

  CurrentMarkerOrientation = aOrientation;

  return UpdateCameraPose();
}

//-----------------------------------------------------------------------------

PlusStatus vtkImageVisualizer::ShowOrientationMarkers( bool aShow )
{
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

//-----------------------------------------------------------------------------

PlusStatus vtkImageVisualizer::InitializeDataCollector( vtkSmartPointer<vtkDataCollector> aCollector )
{
  if( aCollector != NULL )
  {
    // Store a reference to the data collector
    this->SetDataCollector(aCollector);

    if (this->DataCollector->GetConnected() == false)
    {
      LOG_ERROR("Data collection not initialized or device visualization cannot be initialized unless they are connected");
      return PLUS_FAIL;
    }

    this->ImageActor->SetInput(this->DataCollector->GetOutput());
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkImageVisualizer::InitializeResultPolyData( vtkSmartPointer<vtkPolyData> aResultPolyData )
{
  if( aResultPolyData != NULL )
  {
    // Result points poly data
    this->SetResultPolyData(aResultPolyData);

    this->ResultGlyph->SetInputConnection(this->ResultPolyData->GetProducerPort());
  }

  return PLUS_SUCCESS;
}
