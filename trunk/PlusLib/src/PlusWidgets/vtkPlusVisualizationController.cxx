/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "vtkPlusVisualizationController.h"
#include "vtkPlus3DObjectVisualizer.h"
#include "vtkPlusImageVisualizer.h"

// PlusLib includes
#include <PlusConfigure.h>
#include <PlusTrackedFrame.h>
#include <vtkPlusDevice.h>
#include <vtkPlusTrackedFrameList.h>

// VTK includes
#include <QVTKWidget.h>
#include <vtkDirectory.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMath.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTransform.h>
#include <vtkXMLUtilities.h>
#include <vtksys/SystemTools.hxx>

// Qt includes
#include <QApplication>
#include <QEvent>
#include <QTimer>

//-----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusVisualizationController);

//-----------------------------------------------------------------------------
vtkPlusVisualizationController::vtkPlusVisualizationController()
  : ImageVisualizer(vtkSmartPointer<vtkPlusImageVisualizer>::New())
  , PerspectiveVisualizer(vtkSmartPointer < vtkPlus3DObjectVisualizer >::New())
  , BlankRenderer(vtkSmartPointer<vtkRenderer>::New())
  , ResultPolyData(vtkSmartPointer<vtkPolyData>::New())
  , InputPolyData(vtkSmartPointer<vtkPolyData>::New())
  , CurrentMode(DISPLAY_MODE_NONE)
  , AcquisitionFrameRate(20)
  , TransformRepository(NULL)
  , SelectedChannel(NULL)
  , DataCollector(NULL)
{
  // Create transform repository
  this->ClearTransformRepository();

  // Input points poly data
  vtkSmartPointer<vtkPoints> input = vtkSmartPointer<vtkPoints>::New();
  this->InputPolyData->SetPoints(input);

  // Result points poly data
  vtkSmartPointer<vtkPoints> resultPoint = vtkSmartPointer<vtkPoints>::New();
  this->ResultPolyData->SetPoints(resultPoint);

  // Initialize timer
  connect(&this->AcquisitionTimer, &QTimer::timeout, this, &vtkPlusVisualizationController::Update);
  this->AcquisitionTimer.start(1000.0 / this->AcquisitionFrameRate);

  // Create 2D visualizer
  this->ImageVisualizer->SetResultPolyData(this->ResultPolyData);
  this->ImageVisualizer->EnableROI(false);

  // Create 3D visualizer
  this->PerspectiveVisualizer->SetResultPolyData(this->ResultPolyData);
  this->PerspectiveVisualizer->SetInputPolyData(this->InputPolyData);

  // Set up blank renderer
  this->BlankRenderer->SetBackground(0.1, 0.1, 0.1);
  this->BlankRenderer->SetBackground2(0.4, 0.4, 0.4);
  this->BlankRenderer->SetGradientBackground(true);
}

//-----------------------------------------------------------------------------
vtkPlusVisualizationController::~vtkPlusVisualizationController()
{
  disconnect(&this->AcquisitionTimer, &QTimer::timeout, this, &vtkPlusVisualizationController::Update);
  this->AcquisitionTimer.stop();

  if (this->GetDataCollector() != NULL)
  {
    this->GetDataCollector()->Stop();
    this->GetDataCollector()->Disconnect();
  }
  this->SetDataCollector(NULL);
  this->SetTransformRepository(NULL);
}

//-----------------------------------------------------------------------------

void vtkPlusVisualizationController::SetCanvas(QVTKWidget* aCanvas)
{
  this->Canvas = aCanvas;
  this->Canvas->setFocusPolicy(Qt::ClickFocus);
}


//----------------------------------------------------------------------------
void vtkPlusVisualizationController::ClearResultPolyData()
{
  this->ResultPolyData->Initialize();
  this->ResultPolyData->Modified();
}

//----------------------------------------------------------------------------
void vtkPlusVisualizationController::ClearInputPolyData()
{
  this->InputPolyData->Initialize();
  this->InputPolyData->Modified();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::ResetCamera()
{
  if (this->GetCanvasRenderer() != NULL)
  {
    this->GetCanvasRenderer()->ResetCamera();
    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}


//----------------------------------------------------------------------------
void vtkPlusVisualizationController::SetResultPolyDataPoints(vtkPoints* points)
{
  this->ResultPolyData->SetPoints(points);
}

//----------------------------------------------------------------------------
void vtkPlusVisualizationController::SetInputPolyDataPoints(vtkPoints* points)
{
  this->InputPolyData->SetPoints(points);
}

//----------------------------------------------------------------------------
vtkPoints* vtkPlusVisualizationController::GetResultPolyDataPoints()
{
  return ResultPolyData->GetPoints();
}

//----------------------------------------------------------------------------
vtkPoints* vtkPlusVisualizationController::GetInputPolyDataPoints()
{
  return this->InputPolyData->GetPoints();
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::SetAcquisitionFrameRate(int aFrameRate)
{
  LOG_TRACE("vtkPlusVisualizationController::SetAcquisitionFrameRate(" << aFrameRate << ")");

  this->AcquisitionFrameRate = aFrameRate;

  if (this->AcquisitionTimer.isActive())
  {
    this->AcquisitionTimer.stop();
    this->AcquisitionTimer.start(1000.0 / this->AcquisitionFrameRate);
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::HideAll()
{
  LOG_TRACE("vtkPlusVisualizationController::HideAll");

  // Hide all actors from the renderer
  if (this->PerspectiveVisualizer != NULL)
  {
    this->PerspectiveVisualizer->HideAll();
  }
  if (this->ImageVisualizer != NULL)
  {
    this->ImageVisualizer->HideAll();
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::ShowInput(bool aOn)
{
  LOG_TRACE("vtkPlusVisualizationController::ShowInput(" << (aOn ? "true" : "false") << ")");

  if (this->CurrentMode == DISPLAY_MODE_3D && this->PerspectiveVisualizer != NULL)
  {
    this->PerspectiveVisualizer->ShowInput(aOn);
    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::ShowResult(bool aOn)
{
  LOG_TRACE("vtkPlusVisualizationController::ShowResult(" << (aOn ? "true" : "false") << ")");

  if (this->CurrentMode == DISPLAY_MODE_3D && this->PerspectiveVisualizer != NULL)
  {
    this->PerspectiveVisualizer->ShowResult(aOn);
    return PLUS_SUCCESS;
  }
  else if (this->CurrentMode == DISPLAY_MODE_2D && this->ImageVisualizer != NULL)
  {
    this->ImageVisualizer->ShowResult(aOn);
    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::SetVisualizationMode(DISPLAY_MODE aMode)
{
  LOG_TRACE("vtkPlusVisualizationController::SetVisualizationMode( DISPLAY_MODE " << (aMode ? "true" : "false") << ")");

  if (this->GetDataCollector() == NULL)
  {
    LOG_DEBUG("Data collector has not been initialized when visualization mode was changed.");
    return PLUS_FAIL;
  }

  if (this->Canvas == NULL)
  {
    LOG_ERROR("Trying to change visualization mode but no canvas has been assigned for display.");
    return PLUS_FAIL;
  }

  this->DisconnectInput();

  if (aMode == DISPLAY_MODE_2D)
  {
    if (this->SelectedChannel == NULL || this->SelectedChannel->GetVideoDataAvailable() == false)
    {
      LOG_WARNING("Cannot switch to image mode without enabled video in data collector!");
      return PLUS_FAIL;
    }
    if (this->SelectedChannel->GetBrightnessOutput() == NULL)
    {
      LOG_WARNING("No B-mode data available to visualize. Disabling 2D visualization.");
      return PLUS_FAIL;
    }

    if (this->BlankRenderer != NULL && Canvas->GetRenderWindow()->HasRenderer(this->BlankRenderer))
    {
      Canvas->GetRenderWindow()->RemoveRenderer(this->BlankRenderer);
    }
    if (this->ImageVisualizer != NULL && Canvas->GetRenderWindow()->HasRenderer(this->ImageVisualizer->GetCanvasRenderer()))
    {
      Canvas->GetRenderWindow()->RemoveRenderer(this->ImageVisualizer->GetCanvasRenderer());
    }
    if (this->PerspectiveVisualizer != NULL && Canvas->GetRenderWindow()->HasRenderer(this->PerspectiveVisualizer->GetCanvasRenderer()))
    {
      // If there's already been a renderer added, remove it
      Canvas->GetRenderWindow()->RemoveRenderer(this->PerspectiveVisualizer->GetCanvasRenderer());
    }
    // Add the 2D renderer
    if (!Canvas->GetRenderWindow()->HasRenderer(this->ImageVisualizer->GetCanvasRenderer()))
    {
      Canvas->GetRenderWindow()->AddRenderer(this->ImageVisualizer->GetCanvasRenderer());
      this->ImageVisualizer->GetImageActor()->VisibilityOn();
      this->ImageVisualizer->UpdateCameraPose();
    }

    // Disable camera movements
    Canvas->GetRenderWindow()->GetInteractor()->RemoveAllObservers();
  }
  else if (aMode == DISPLAY_MODE_3D)
  {
    if (this->BlankRenderer != NULL && Canvas->GetRenderWindow()->HasRenderer(this->BlankRenderer))
    {
      Canvas->GetRenderWindow()->RemoveRenderer(this->BlankRenderer);
    }
    if (this->ImageVisualizer != NULL && Canvas->GetRenderWindow()->HasRenderer(this->ImageVisualizer->GetCanvasRenderer()))
    {
      Canvas->GetRenderWindow()->RemoveRenderer(this->ImageVisualizer->GetCanvasRenderer());
    }
    if (this->PerspectiveVisualizer != NULL && Canvas->GetRenderWindow()->HasRenderer(this->PerspectiveVisualizer->GetCanvasRenderer()))
    {
      Canvas->GetRenderWindow()->RemoveRenderer(this->PerspectiveVisualizer->GetCanvasRenderer());
    }

    // Add the 3D renderer
    if (!Canvas->GetRenderWindow()->HasRenderer(this->PerspectiveVisualizer->GetCanvasRenderer()))
    {
      Canvas->GetRenderWindow()->AddRenderer(this->PerspectiveVisualizer->GetCanvasRenderer());
    }

    // Enable camera movements
    Canvas->GetRenderWindow()->GetInteractor()->SetInteractorStyle(vtkInteractorStyleTrackballCamera::New());
  }
  else
  {
    this->HideRenderer();
  }

  CurrentMode = aMode;

  this->ConnectInput();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::ShowOrientationMarkers(bool aShow)
{
  LOG_TRACE("vtkPlusVisualizationController::ShowOrientationMarkers(" << (aShow ? "true" : "false") << ")");

  if (this->ImageVisualizer != NULL && this->Is2DMode())
  {
    this->ImageVisualizer->ShowOrientationMarkers(aShow);
  }
  else
  {
    LOG_ERROR("Image visualizer not created or controller is not in 2D mode.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusVisualizationController::SetLineSegmentationPoints(double startPoint_Image[2], double endPoint_Image[2])
{
  this->ImageVisualizer->SetLineSegmentationPoints(startPoint_Image, endPoint_Image);
}

//----------------------------------------------------------------------------
void vtkPlusVisualizationController::SetLineSegmentationVisible(bool _arg)
{
  this->ImageVisualizer->SetLineSegmentationVisible(_arg);
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::StartDataCollection()
{
  LOG_TRACE("vtkPlusVisualizationController::StartDataCollection");

  // Delete data collection if already exists
  vtkPlusDataCollector* dataCollector = this->GetDataCollector();
  if (dataCollector != NULL)
  {
    dataCollector->Stop();
    dataCollector->Disconnect();
    this->SetDataCollector(NULL);
  }

  // Create the proper data collector variant
  dataCollector = vtkPlusDataCollector::New();
  this->SetDataCollector(dataCollector);
  dataCollector->Delete();

  // Read configuration
  if (dataCollector->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  if (dataCollector->Connect() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  if (dataCollector->Start() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  if (!dataCollector->GetConnected())
  {
    LOG_ERROR("Unable to initialize DataCollector!");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::DumpBuffersToDirectory(const char* aDirectory)
{
  LOG_TRACE("vtkPlusVisualizationController::DumpBuffersToDirectory");

  vtkPlusDataCollector* dataCollector = this->GetDataCollector();
  if (dataCollector == NULL || !dataCollector->GetConnected())
  {
    LOG_INFO("Data collector is not connected, buffers cannot be saved");
    return PLUS_FAIL;
  }

  if (dataCollector->DumpBuffersToDirectory(aDirectory) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to dump data buffers to file.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkPlusVisualizationController::resizeEvent(QResizeEvent* aEvent)
{
  LOG_TRACE("vtkPlusVisualizationController::resizeEvent( ... )");
  if (this->ImageVisualizer != NULL)
  {
    this->ImageVisualizer->UpdateCameraPose();
  }
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::SetScreenRightDownAxesOrientation(US_IMAGE_ORIENTATION aOrientation /*= US_IMG_ORIENT_MF*/)
{
  LOG_TRACE("vtkPlusVisualizationController::ShowOrientationMarkers(" << aOrientation << ")");

  if (this->ImageVisualizer != NULL)
  {
    this->ImageVisualizer->SetScreenRightDownAxesOrientation(aOrientation);
    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::Update()
{
  if (this->PerspectiveVisualizer != NULL && CurrentMode == DISPLAY_MODE_3D)
  {
    this->PerspectiveVisualizer->Update();
  }

  // Force update of the brightness image in the DataCollector,
  // because it is the image that the image actors show
  if (this->SelectedChannel != NULL && this->GetImageActor() != NULL)
  {
    this->GetImageActor()->SetInputData(this->SelectedChannel->GetBrightnessOutput());
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
vtkRenderer* vtkPlusVisualizationController::GetCanvasRenderer()
{
  if (this->CurrentMode == DISPLAY_MODE_3D && this->PerspectiveVisualizer != NULL)
  {
    return PerspectiveVisualizer->GetCanvasRenderer();
  }
  else if (this->ImageVisualizer != NULL)
  {
    return ImageVisualizer->GetCanvasRenderer();
  }

  return NULL;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::EnableVolumeActor(bool aEnable)
{
  if (aEnable)
  {
    if (this->PerspectiveVisualizer != NULL && this->PerspectiveVisualizer->GetVolumeActor() != NULL)
    {
      this->PerspectiveVisualizer->GetVolumeActor()->VisibilityOn();
    }
  }
  else
  {
    if (this->PerspectiveVisualizer != NULL && this->PerspectiveVisualizer->GetVolumeActor() != NULL)
    {
      this->PerspectiveVisualizer->GetVolumeActor()->VisibilityOff();
    }
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::GetTransformTranslationString(const char* aTransformFrom, const char* aTransformTo, std::string& aTransformTranslationString, bool* aValid/* = NULL*/)
{
  PlusTransformName transformName(aTransformFrom, aTransformTo);

  return GetTransformTranslationString(transformName, aTransformTranslationString, aValid);
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::GetTransformTranslationString(PlusTransformName aTransform, std::string& aTransformTranslationString, bool* aValid/* = NULL*/)
{
  vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  if (GetTransformMatrix(aTransform, transformMatrix, aValid) != PLUS_SUCCESS)
  {
    aTransformTranslationString = "N/A";
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ss << std::fixed << std::setprecision(1) << transformMatrix->GetElement(0, 3) << " " << transformMatrix->GetElement(1, 3) << " " << transformMatrix->GetElement(2, 3);

  aTransformTranslationString = ss.str();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::GetTransformMatrix(const char* aTransformFrom, const char* aTransformTo, vtkMatrix4x4* aOutputMatrix, bool* aValid/* = NULL*/)
{
  PlusTransformName transformName(aTransformFrom, aTransformTo);

  return GetTransformMatrix(transformName, aOutputMatrix, aValid);
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::GetTransformMatrix(PlusTransformName aTransform, vtkMatrix4x4* aOutputMatrix, bool* aValid/* = NULL*/)
{
  PlusTrackedFrame trackedFrame;
  if (this->SelectedChannel == NULL || this->SelectedChannel->GetTrackedFrame(trackedFrame) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to get tracked frame from selected channel!");
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
PlusStatus vtkPlusVisualizationController::SetVolumeMapper(vtkPolyDataMapper* aContourMapper)
{
  if (this->PerspectiveVisualizer != NULL)
  {
    this->PerspectiveVisualizer->SetVolumeMapper(aContourMapper);
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::SetVolumeColor(double r, double g, double b)
{
  if (this->PerspectiveVisualizer != NULL)
  {
    this->PerspectiveVisualizer->SetVolumeColor(r, g, b);
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::SetInputColor(double r, double g, double b)
{
  if (this->PerspectiveVisualizer != NULL)
  {
    this->PerspectiveVisualizer->SetInputColor(r, g, b);
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::IsExistingTransform(const char* aTransformFrom, const char* aTransformTo, bool aUseLatestTrackedFrame/* = true */)
{
  PlusTransformName transformName(aTransformFrom, aTransformTo);

  if (aUseLatestTrackedFrame)
  {
    if (this->SelectedChannel == NULL || this->SelectedChannel->GetTrackingDataAvailable() == false)
    {
      LOG_WARNING("SelectedChannel object is invalid or the selected channel does not contain tracking data!");
      return PLUS_FAIL;
    }

    PlusTrackedFrame trackedFrame;
    if (this->SelectedChannel->GetTrackedFrame(trackedFrame) != PLUS_SUCCESS)
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
PlusStatus vtkPlusVisualizationController::DisconnectInput()
{
  if (this->GetImageActor() != NULL)
  {
    this->GetImageActor()->SetInputData(NULL);
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::ConnectInput()
{
  vtkPlusChannel* aChannel(NULL);
  if (this->GetImageActor() != NULL && this->SelectedChannel != NULL)
  {
    this->GetImageActor()->SetInputData(this->SelectedChannel->GetBrightnessOutput());
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
vtkImageActor* vtkPlusVisualizationController::GetImageActor()
{
  if (this->CurrentMode == DISPLAY_MODE_2D && this->ImageVisualizer != NULL)
  {
    return this->ImageVisualizer->GetImageActor();
  }
  else if (this->CurrentMode == DISPLAY_MODE_3D && this->PerspectiveVisualizer != NULL)
  {
    return this->PerspectiveVisualizer->GetImageActor();
  }

  return NULL;
}

//-----------------------------------------------------------------------------
bool vtkPlusVisualizationController::Is2DMode()
{
  return CurrentMode == DISPLAY_MODE_2D;
}

//-----------------------------------------------------------------------------
bool vtkPlusVisualizationController::Is3DMode()
{
  return CurrentMode == DISPLAY_MODE_3D;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::HideRenderer()
{
  LOG_TRACE("vtkPlusVisualizationController::HideRenderer");

  if (this->Canvas == NULL)
  {
    LOG_ERROR("Trying to hide a visualization controller that hasn't been assigned a canvas.");
    return PLUS_FAIL;
  }

  if (this->PerspectiveVisualizer != NULL && Canvas->GetRenderWindow()->HasRenderer(PerspectiveVisualizer->GetCanvasRenderer()))
  {
    // If there's already been a renderer added, remove it
    Canvas->GetRenderWindow()->RemoveRenderer(PerspectiveVisualizer->GetCanvasRenderer());
  }
  if (this->ImageVisualizer != NULL && Canvas->GetRenderWindow()->HasRenderer(ImageVisualizer->GetCanvasRenderer()))
  {
    // If there's already been a renderer added, remove it
    Canvas->GetRenderWindow()->RemoveRenderer(ImageVisualizer->GetCanvasRenderer());
  }

  Canvas->GetRenderWindow()->AddRenderer(this->BlankRenderer);

  this->CurrentMode = DISPLAY_MODE_NONE;

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::ShowAllObjects(bool aShow)
{
  if (this->PerspectiveVisualizer != NULL)
  {
    return this->PerspectiveVisualizer->ShowAllObjects(aShow);
  }

  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::ReadRoiConfiguration(vtkXMLDataElement* aXMLElement)
{
  if (this->ImageVisualizer == NULL)
  {
    LOG_ERROR("Failed to read ROI configuration, ImageVisualizer is invalid");
    return PLUS_FAIL;
  }
  if (this->ImageVisualizer->ReadRoiConfiguration(aXMLElement) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to configure image visualizer ROI.");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::ReadConfiguration(vtkXMLDataElement* aXMLElement)
{
  // Fill up transform repository
  if (this->TransformRepository->ReadConfiguration(aXMLElement) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to initialize transform repository!");
  }

  // Pass on any configuration steps to children
  if (this->PerspectiveVisualizer != NULL)
  {
    this->PerspectiveVisualizer->SetTransformRepository(this->TransformRepository);
    if (this->PerspectiveVisualizer->ReadConfiguration(aXMLElement) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to configure perspective visualizer.");
      return PLUS_FAIL;
    }
  }

  if (this->ImageVisualizer != NULL)
  {
    if (this->ImageVisualizer->ReadConfiguration(aXMLElement) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to configure image visualizer.");
      return PLUS_FAIL;
    }
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::WriteConfiguration(vtkXMLDataElement* aXMLElement)
{
  PlusStatus status = PLUS_SUCCESS;

  if (this->GetDataCollector()->WriteConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to save configuration of data collector");
    status = PLUS_FAIL;
  }

  if (this->TransformRepository->WriteConfiguration(aXMLElement) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to save configuration of transform repository");
    status = PLUS_FAIL;
  }

  // Here we could give a chance to PerspectiveVisualizer and ImageVisualizer
  // to save configuration parameters, but currently they don't need to save anything.

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::StopAndDisconnectDataCollector()
{
  vtkSmartPointer<vtkPlusDataCollector> dataCollector = this->GetDataCollector();
  if (dataCollector == NULL)
  {
    LOG_WARNING("Trying to disconnect from non-connected data collector.");
    return PLUS_FAIL;
  }

  this->DisconnectInput();
  SetDataCollector(NULL);   // the local smart pointer still keeps a reference

  dataCollector->Stop();
  dataCollector->Disconnect();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::ClearTransformRepository()
{
  vtkSmartPointer<vtkPlusTransformRepository> transformRepository = vtkSmartPointer<vtkPlusTransformRepository>::New();
  this->SetTransformRepository(transformRepository);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::SetROIBounds(int xMin, int xMax, int yMin, int yMax)
{
  if (this->ImageVisualizer == NULL)
  {
    LOG_ERROR("Image visualizer not created when attempting to set ROI bounds.");
    return PLUS_FAIL;
  }
  this->ImageVisualizer->SetROIBounds(xMin, xMax, yMin, yMax);
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::EnableROI(bool aEnable)
{
  if (this->ImageVisualizer != NULL)
  {
    this->ImageVisualizer->EnableROI(aEnable);
    return PLUS_SUCCESS;
  }

  LOG_ERROR("Image visualizer not created when attempting to enable the ROI.");
  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::EnableWireLabels(bool aEnable)
{
  LOG_TRACE("vtkPlusVisualizationController::EnableWireLabels");

  if (this->ImageVisualizer != NULL)
  {
    this->ImageVisualizer->EnableWireLabels(aEnable);
    return PLUS_SUCCESS;
  }

  LOG_ERROR("Image visualizer not created when attempting to enable the wire visualization.");
  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::SetWireLabelPositions(vtkPoints* aPointList)
{
  LOG_TRACE("vtkPlusVisualizationController::SetWireLabelPositions");

  if (this->ImageVisualizer != NULL)
  {
    this->ImageVisualizer->SetWireLabelPositions(aPointList);
    return PLUS_SUCCESS;
  }

  LOG_ERROR("Image visualizer not created when attempting to set wire label positions.");
  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::ShowObjectById(const char* aModelId, bool aOn)
{
  LOG_TRACE("vtkPlusVisualizationController::ShowObjectById");

  if (aModelId == NULL)
  {
    return PLUS_FAIL;
  }

  if (this->PerspectiveVisualizer != NULL)
  {
    this->PerspectiveVisualizer->ShowObjectById(aModelId, aOn);
    return PLUS_SUCCESS;
  }

  LOG_ERROR("3D visualizer not created when attempting to show an object by ID.");
  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::AddObject(vtkPlusDisplayableObject* aObject)
{
  LOG_TRACE("vtkPlusVisualizationController::AddObject");

  if (aObject == NULL)
  {
    return PLUS_FAIL;
  }

  if (this->PerspectiveVisualizer != NULL)
  {
    this->PerspectiveVisualizer->AddObject(aObject);
    return PLUS_SUCCESS;
  }

  LOG_ERROR("3D visualizer not created when attempting to add an object");
  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------
vtkPlusDisplayableObject* vtkPlusVisualizationController::GetObjectById(const char* aId)
{
  LOG_TRACE("vtkPlusVisualizationController::ShowObjectById");

  if (aId == NULL)
  {
    return NULL;
  }

  if (this->PerspectiveVisualizer != NULL)
  {
    return this->PerspectiveVisualizer->GetObjectById(aId);
  }

  LOG_ERROR("3D visualizer not created when attempting to retrieve an object by ID.");
  return NULL;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVisualizationController::Reset()
{
  PlusStatus perspective, image;
  if (this->PerspectiveVisualizer != NULL)
  {
    perspective = this->PerspectiveVisualizer->ClearDisplayableObjects();
  }
  if (this->ImageVisualizer != NULL)
  {
    image = this->ImageVisualizer->Reset();
  }

  return (perspective == PLUS_SUCCESS && image == PLUS_SUCCESS) ? PLUS_SUCCESS : PLUS_FAIL;
}

//-----------------------------------------------------------------------------
void vtkPlusVisualizationController::SetInputData(vtkImageData* input)
{
  if (this->ImageVisualizer != NULL)
  {
    this->ImageVisualizer->SetInputData(input);
  }
}

//-----------------------------------------------------------------------------
void vtkPlusVisualizationController::SetSelectedChannel(vtkPlusChannel* aChannel)
{
  this->SelectedChannel = aChannel;

  if (this->ImageVisualizer != NULL)
  {
    this->ImageVisualizer->SetChannel(aChannel);
  }

  if (this->PerspectiveVisualizer != NULL)
  {
    this->PerspectiveVisualizer->SetChannel(aChannel);
  }
}

//----------------------------------------------------------------------------
vtkPlusChannel* vtkPlusVisualizationController::GetSelectedChannel()
{
  return this->SelectedChannel;
}

//-----------------------------------------------------------------------------
void vtkPlusVisualizationController::SetSliceNumber(int number)
{
  if (this->ImageVisualizer != NULL)
  {
    this->ImageVisualizer->SetSliceNumber(number);
  }
  if (this->PerspectiveVisualizer != NULL)
  {
    this->PerspectiveVisualizer->SetSliceNumber(number);
  }
}