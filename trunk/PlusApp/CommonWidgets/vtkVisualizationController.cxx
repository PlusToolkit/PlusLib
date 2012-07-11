/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "TrackedFrame.h"

#include "vtkDataCollectorHardwareDevice.h" // Only for dumping buffers
#include "vtkDirectory.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkMath.h"
#include "vtkVisualizationController.h"
#include "vtkPlusVideoSource.h" // Only for dumping buffers
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkTrackedFrameList.h"
#include "vtkTracker.h" // Only for dumping buffers
#include "vtkTransform.h"
#include "vtkVideoBuffer.h" // Only for dumping buffers
#include "vtkXMLUtilities.h"
#include "vtksys/SystemTools.hxx"

#include <QApplication>
#include <QEvent>
#include <QTimer>
#include <QVTKWidget.h>

//-----------------------------------------------------------------------------

vtkStandardNewMacro(vtkVisualizationController);

//-----------------------------------------------------------------------------

vtkVisualizationController::vtkVisualizationController()
: ImageVisualizer(NULL)
, PerspectiveVisualizer(NULL)
, Canvas(NULL)
, DataCollector(NULL)
, AcquisitionTimer(NULL)
, ResultPolyData(NULL)
, InputPolyData(NULL)
, CurrentMode(DISPLAY_MODE_NONE)
, AcquisitionFrameRate(20)
, TransformRepository(NULL)
{
  // Create transform repository
  this->ClearTransformRepository();

  // Input points poly data
  vtkSmartPointer<vtkPolyData> inputPolyData = vtkSmartPointer<vtkPolyData>::New();
  inputPolyData->Initialize();
  vtkSmartPointer<vtkPoints> input = vtkSmartPointer<vtkPoints>::New();
  inputPolyData->SetPoints(input);

  this->SetInputPolyData(inputPolyData);

  // Result points poly data
  vtkSmartPointer<vtkPolyData> resultPolyData = vtkSmartPointer<vtkPolyData>::New();
  resultPolyData = vtkPolyData::New();
  resultPolyData->Initialize();
  vtkSmartPointer<vtkPoints> resultPoint = vtkSmartPointer<vtkPoints>::New();
  resultPolyData->SetPoints(resultPoint);
  this->SetResultPolyData(resultPolyData);  

  // Initialize timer
  this->AcquisitionTimer = new QTimer();
  this->AcquisitionTimer->start(1000.0 / this->AcquisitionFrameRate);

  // Create 2D visualizer
  vtkSmartPointer<vtkImageVisualizer> imageVisualizer = vtkSmartPointer<vtkImageVisualizer>::New();
  imageVisualizer->InitializeResultPolyData(this->ResultPolyData);
  this->SetImageVisualizer(imageVisualizer);

  // Create 3D visualizer
  vtkSmartPointer<vtk3DObjectVisualizer> perspectiveVisualizer = vtkSmartPointer<vtk3DObjectVisualizer>::New();
  perspectiveVisualizer->InitializeResultPolyData(this->ResultPolyData);
  perspectiveVisualizer->InitializeInputPolyData(this->InputPolyData);
  this->SetPerspectiveVisualizer(perspectiveVisualizer);

  connect( this->AcquisitionTimer, SIGNAL( timeout() ), this, SLOT( Update() ) );
}

//-----------------------------------------------------------------------------

vtkVisualizationController::~vtkVisualizationController()
{
  if (this->AcquisitionTimer != NULL)
  {
    disconnect( this->AcquisitionTimer, SIGNAL( timeout() ), this, SLOT( Update() ) );
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

  this->SetInputPolyData(NULL);
  this->SetResultPolyData(NULL);

  this->SetTransformRepository(NULL);
  this->SetImageVisualizer(NULL);
  this->SetPerspectiveVisualizer(NULL);

  this->SetImageVisualizer(NULL);
  this->SetPerspectiveVisualizer(NULL);
}

//-----------------------------------------------------------------------------

PlusStatus vtkVisualizationController::SetAcquisitionFrameRate(int aFrameRate)
{
  LOG_TRACE("vtkVisualizationController::SetAcquisitionFrameRate(" << aFrameRate << ")");

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

PlusStatus vtkVisualizationController::HideAll()
{
  LOG_TRACE("vtkVisualizationController::HideAll");

  // Hide all actors from the renderer
  if( this->PerspectiveVisualizer != NULL )
  {
    this->PerspectiveVisualizer->HideAll();
  }
  if( this->ImageVisualizer != NULL )
  {
    this->ImageVisualizer->HideAll();
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkVisualizationController::ShowInput(bool aOn)
{
  LOG_TRACE("vtkVisualizationController::ShowInput(" << (aOn?"true":"false") << ")");

  if( this->CurrentMode == DISPLAY_MODE_3D && this->PerspectiveVisualizer != NULL )
  {
    this->PerspectiveVisualizer->ShowInput(aOn);
    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------

PlusStatus vtkVisualizationController::ShowResult(bool aOn)
{
  LOG_TRACE("vtkVisualizationController::ShowResult(" << (aOn?"true":"false") << ")");

  if( this->CurrentMode == DISPLAY_MODE_3D && this->PerspectiveVisualizer != NULL )
  {
    this->PerspectiveVisualizer->ShowResult(aOn);
    return PLUS_SUCCESS;
  }
  else if( this->CurrentMode == DISPLAY_MODE_2D && this->ImageVisualizer != NULL )
  {
    this->ImageVisualizer->ShowResult(aOn);
    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------

PlusStatus vtkVisualizationController::SetVisualizationMode( DISPLAY_MODE aMode )
{
  LOG_TRACE("vtkVisualizationController::SetVisualizationMode( DISPLAY_MODE " << (aMode?"true":"false") << ")");

  if (this->DataCollector == NULL)
  {
    LOG_ERROR("Data collector has not been initialized when visualization was requested.");
    return PLUS_FAIL;
  }

  if( this->Canvas == NULL )
  {
    LOG_ERROR("Trying to change visualization mode but no canvas has been assigned for display.");
    return PLUS_FAIL;
  }

  this->DisconnectInput();

  if (aMode == DISPLAY_MODE_2D)
  {
    if (this->DataCollector->GetVideoEnabled() == false)
    {
      LOG_DEBUG("Cannot switch to image mode without enabled video in data collector!");
      return PLUS_FAIL;
    }

    if( this->ImageVisualizer != NULL && this->GetCanvas()->GetRenderWindow()->HasRenderer(this->ImageVisualizer->GetCanvasRenderer()) )
    {
      this->GetCanvas()->GetRenderWindow()->RemoveRenderer(this->ImageVisualizer->GetCanvasRenderer());
    }
    if( this->PerspectiveVisualizer != NULL && this->GetCanvas()->GetRenderWindow()->HasRenderer(this->PerspectiveVisualizer->GetCanvasRenderer()) )
    {
      // If there's already been a renderer added, remove it
      this->GetCanvas()->GetRenderWindow()->RemoveRenderer(this->PerspectiveVisualizer->GetCanvasRenderer());
    }
    // Add the 2D renderer
    if( !GetCanvas()->GetRenderWindow()->HasRenderer(this->ImageVisualizer->GetCanvasRenderer()) )
    {
      this->GetCanvas()->GetRenderWindow()->AddRenderer(this->ImageVisualizer->GetCanvasRenderer());
      this->ImageVisualizer->GetImageActor()->VisibilityOn();
      this->ImageVisualizer->UpdateCameraPose();
      this->ImageVisualizer->SetScreenRightDownAxesOrientation(US_IMG_ORIENT_MF);
    }

    // Disable camera movements
    this->GetCanvas()->GetRenderWindow()->GetInteractor()->RemoveAllObservers();
    // Show the canvas
    this->GetCanvas()->setVisible(true);
  }
  else if ( aMode == DISPLAY_MODE_3D )
  {
    if( this->ImageVisualizer != NULL && this->GetCanvas()->GetRenderWindow()->HasRenderer(this->ImageVisualizer->GetCanvasRenderer()) )
    {
      this->GetCanvas()->GetRenderWindow()->RemoveRenderer(this->ImageVisualizer->GetCanvasRenderer());
    }
    if( this->PerspectiveVisualizer != NULL && this->GetCanvas()->GetRenderWindow()->HasRenderer(this->PerspectiveVisualizer->GetCanvasRenderer()) )
    {
      this->GetCanvas()->GetRenderWindow()->RemoveRenderer(this->PerspectiveVisualizer->GetCanvasRenderer());
    }

    // Add the 3D renderer
    if( !GetCanvas()->GetRenderWindow()->HasRenderer(this->PerspectiveVisualizer->GetCanvasRenderer()) )
    {
      this->GetCanvas()->GetRenderWindow()->AddRenderer(this->PerspectiveVisualizer->GetCanvasRenderer());
    }

    // Enable camera movements
    this->GetCanvas()->GetRenderWindow()->GetInteractor()->SetInteractorStyle(vtkInteractorStyleTrackballCamera::New());
    // Show the canvas
    this->GetCanvas()->setVisible(true);
  }
  else
  {
    this->HideRenderer();
  }

  this->ConnectInput();

  CurrentMode = aMode;

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkVisualizationController::ShowOrientationMarkers( bool aShow )
{
  LOG_TRACE("vtkVisualizationController::ShowOrientationMarkers(" << (aShow?"true":"false") << ")");
  // TODO : if in 2D mode, fire this toggle

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkVisualizationController::StartDataCollection()
{
  LOG_TRACE("vtkVisualizationController::StartDataCollection"); 

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

  this->ImageVisualizer->InitializeDataCollector(this->DataCollector);
  this->PerspectiveVisualizer->InitializeDataCollector(this->DataCollector);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkVisualizationController::DumpBuffersToDirectory(const char* aDirectory)
{
  LOG_TRACE("vtkVisualizationController::DumpBuffersToDirectory");

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

void vtkVisualizationController::resizeEvent( QResizeEvent* aEvent )
{
  LOG_TRACE("vtkVisualizationController::resizeEvent( ... )");
  if( this->ImageVisualizer != NULL)
  {
    this->ImageVisualizer->UpdateCameraPose();
  }
}

//-----------------------------------------------------------------------------

PlusStatus vtkVisualizationController::SetScreenRightDownAxesOrientation( US_IMAGE_ORIENTATION aOrientation /*= US_IMG_ORIENT_MF*/ )
{
  LOG_TRACE("vtkVisualizationController::ShowOrientationMarkers(" << aOrientation << ")");

  if( this->ImageVisualizer != NULL)
  {
    this->ImageVisualizer->SetScreenRightDownAxesOrientation(aOrientation);
    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------

PlusStatus vtkVisualizationController::Update()
{
  if( this->PerspectiveVisualizer != NULL && CurrentMode == DISPLAY_MODE_3D)
  {
    this->PerspectiveVisualizer->Update();
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

vtkRenderer* vtkVisualizationController::GetCanvasRenderer()
{
  if( this->CurrentMode == DISPLAY_MODE_3D && this->PerspectiveVisualizer != NULL)
  {
    return PerspectiveVisualizer->GetCanvasRenderer();
  }
  else if ( this->ImageVisualizer != NULL )
  {
    return ImageVisualizer->GetCanvasRenderer();
  }

  return NULL;
}

//-----------------------------------------------------------------------------

PlusStatus vtkVisualizationController::EnableVolumeActor( bool aEnable )
{
  if( aEnable )
  {
    if( this->PerspectiveVisualizer != NULL )
    {
      this->PerspectiveVisualizer->GetVolumeActor()->VisibilityOn();
    }
  }
  else
  {
    if( this->PerspectiveVisualizer != NULL )
    {
      this->PerspectiveVisualizer->GetVolumeActor()->VisibilityOff();
    }
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkVisualizationController::GetTransformTranslationString(const char* aTransformFrom, const char* aTransformTo, std::string &aTransformTranslationString, bool* aValid/* = NULL*/)
{
  PlusTransformName transformName(aTransformFrom, aTransformTo);

  return GetTransformTranslationString(transformName, aTransformTranslationString, aValid);
}

//-----------------------------------------------------------------------------

PlusStatus vtkVisualizationController::GetTransformTranslationString(PlusTransformName aTransform, std::string &aTransformTranslationString, bool* aValid/* = NULL*/)
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

PlusStatus vtkVisualizationController::GetTransformMatrix(const char* aTransformFrom, const char* aTransformTo, vtkSmartPointer<vtkMatrix4x4> aOutputMatrix, bool* aValid/* = NULL*/)
{
  PlusTransformName transformName(aTransformFrom, aTransformTo);

  return GetTransformMatrix(transformName, aOutputMatrix, aValid);
}

//-----------------------------------------------------------------------------

PlusStatus vtkVisualizationController::GetTransformMatrix(PlusTransformName aTransform, vtkSmartPointer<vtkMatrix4x4> aOutputMatrix, bool* aValid/* = NULL*/)
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

PlusStatus vtkVisualizationController::SetVolumeMapper( vtkSmartPointer<vtkPolyDataMapper> aContourMapper )
{
  if( this->PerspectiveVisualizer != NULL )
  {
    this->PerspectiveVisualizer->GetVolumeActor()->SetMapper(aContourMapper);
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkVisualizationController::SetVolumeColor( double r, double g, double b )
{
  if( this->PerspectiveVisualizer != NULL )
  {
    this->PerspectiveVisualizer->GetVolumeActor()->GetProperty()->SetColor(r, g, b);
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkVisualizationController::IsExistingTransform(const char* aTransformFrom, const char* aTransformTo, bool aUseLatestTrackedFrame/* = true */)
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

PlusStatus vtkVisualizationController::ShowObject(const char* aObjectCoordinateFrame, bool aOn)
{
  LOG_TRACE("vtkPerspectiveVisualizer::ShowObject(" << aObjectCoordinateFrame << ", " << (aOn?"true":"false") << ")");

  if( this->PerspectiveVisualizer != NULL )
  {
    return this->PerspectiveVisualizer->ShowObject(aObjectCoordinateFrame, aOn);
  }

  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------

PlusStatus vtkVisualizationController::DisconnectInput()
{
  if( this->GetImageActor() != NULL )
  {
    this->GetImageActor()->SetInput(NULL);
    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------

PlusStatus vtkVisualizationController::ConnectInput()
{
  if( this->GetImageActor() != NULL )
  {
    this->GetImageActor()->SetInput(this->DataCollector->GetBrightnessOutput());
    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------

vtkImageActor* vtkVisualizationController::GetImageActor()
{
  if( this->CurrentMode == DISPLAY_MODE_2D && this->ImageVisualizer != NULL )
  {
    return this->ImageVisualizer->GetImageActor();
  }
  else if( this->PerspectiveVisualizer != NULL )
  {
    return this->PerspectiveVisualizer->GetImageActor();
  }

  return NULL;
}

//-----------------------------------------------------------------------------

bool vtkVisualizationController::Is2DMode()
{
  return CurrentMode == DISPLAY_MODE_2D;
}

//-----------------------------------------------------------------------------

bool vtkVisualizationController::Is3DMode()
{
  return CurrentMode == DISPLAY_MODE_3D;
}

//-----------------------------------------------------------------------------

PlusStatus vtkVisualizationController::HideRenderer()
{
  LOG_TRACE("vtkVisualizationController::HideRenderer");

  if( this->Canvas == NULL )
  {
    LOG_ERROR("Trying to hide a visualization controller that hasn't been assigned a canvas.");
    return PLUS_FAIL;
  }

  if( this->PerspectiveVisualizer != NULL && Canvas->GetRenderWindow()->HasRenderer(PerspectiveVisualizer->GetCanvasRenderer()) )
  {
    // If there's already been a renderer added, remove it
    this->GetCanvas()->GetRenderWindow()->RemoveRenderer(PerspectiveVisualizer->GetCanvasRenderer());
  }
  if( this->ImageVisualizer != NULL && Canvas->GetRenderWindow()->HasRenderer(ImageVisualizer->GetCanvasRenderer()) )
  {
    // If there's already been a renderer added, remove it
    this->GetCanvas()->GetRenderWindow()->RemoveRenderer(ImageVisualizer->GetCanvasRenderer());
  }

  if( this->GetCanvas()->isVisible() )
  {
    this->GetCanvas()->setVisible(false);
  }

  this->CurrentMode = DISPLAY_MODE_NONE;

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkVisualizationController::ShowAllObjects( bool aShow )
{
  if( this->PerspectiveVisualizer != NULL )
  {
    return this->PerspectiveVisualizer->ShowAllObjects(aShow);
  }

  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------

PlusStatus vtkVisualizationController::ReadConfiguration(vtkSmartPointer<vtkXMLDataElement> aXMLElement)
{
  // Fill up transform repository
  if( this->TransformRepository->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to initialize transform repository!"); 
  }

  // Pass on any configuration steps to children
  if( this->PerspectiveVisualizer != NULL )
  {
    this->PerspectiveVisualizer->InitializeTransformRepository(this->TransformRepository);
    return this->PerspectiveVisualizer->ReadConfiguration(aXMLElement);
  }

  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------

PlusStatus vtkVisualizationController::StopAndDisconnectDataCollector()
{
  if( this->DataCollector == NULL )
  {
    LOG_WARNING("Trying to disconnect from non-connected data collector.");
    return PLUS_FAIL;
  }

  this->DisconnectInput();
  this->PerspectiveVisualizer->ClearDisplayableObjects();

  this->DataCollector->Stop();
  this->DataCollector->Disconnect();
  this->SetDataCollector(NULL);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkVisualizationController::ClearTransformRepository()
{
  vtkSmartPointer<vtkTransformRepository> transformRepository = vtkSmartPointer<vtkTransformRepository>::New();
  this->SetTransformRepository(transformRepository);

  return PLUS_SUCCESS;
}
