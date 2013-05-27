/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "SingleWallCalibrationToolbox.h"
#include "TrackedFrame.h"
#include "fCalMainWindow.h"
#include "vtkLineSegmentationAlgo.h"
#include "vtkSingleWallCalibrationAlgo.h"
#include "vtkTrackedFrameList.h"
#include "vtkVisualizationController.h"
#include <QFileDialog>
#include <QTimer>
#include <vtkXMLUtilities.h>

//-----------------------------------------------------------------------------

SingleWallCalibrationToolbox::SingleWallCalibrationToolbox(fCalMainWindow* aParentMainWindow, Qt::WFlags aFlags)
  : AbstractToolbox(aParentMainWindow)
  , QWidget(aParentMainWindow, aFlags)
  , m_Calibration(NULL)
  , m_Segmentation(NULL)
  , m_SpatialCalibrationData(NULL)
  , m_SpatialValidationData(NULL)
  , m_RecordingBuffer(NULL)
  , m_SingleFrameCount(18)
  , m_LastRecordedFrameTimestamp(0.0)
  , m_CancelRequest(false)
  , m_NumberOfCalibrationImagesToAcquire(200)
  , m_NumberOfValidationImagesToAcquire(100)
  , m_NumberOfSegmentedCalibrationImages(0)
  , m_NumberOfSegmentedValidationImages(0)
  , m_RecordingIntervalMs(200)
  , m_MaxTimeSpentWithProcessingMs(150)
  , m_LastProcessingTimePerFrameMs(-1)
{
  ui.setupUi(this);

  // Create algorithms
  m_Calibration = vtkSingleWallCalibrationAlgo::New();

  m_Segmentation = vtkLineSegmentationAlgo::New();

  // Create tracked frame lists
  m_SpatialCalibrationData = vtkTrackedFrameList::New();
  m_SpatialCalibrationData->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP | REQUIRE_TRACKING_OK); 

  m_SpatialValidationData = vtkTrackedFrameList::New();
  m_SpatialValidationData->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP | REQUIRE_TRACKING_OK); 

  m_RecordingBuffer = vtkTrackedFrameList::New();
  m_RecordingBuffer->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP | REQUIRE_TRACKING_OK); 

  // Change result display properties
  ui.label_Results->setFont(QFont("Courier", 8));

  // Connect events
  connect( ui.pushButton_StartSpatial, SIGNAL( clicked() ), this, SLOT( StartCalibration() ) );
  connect( ui.pushButton_CancelSpatial, SIGNAL( clicked() ), this, SLOT( CancelCalibration() ) );
  connect( ui.pushButton_SingleFrame, SIGNAL( clicked() ), this, SLOT( SingleFrame() ) );
}

//-----------------------------------------------------------------------------

SingleWallCalibrationToolbox::~SingleWallCalibrationToolbox()
{
  if (m_Calibration != NULL)
  {
    m_Calibration->Delete();
    m_Calibration = NULL;
  }

  m_Segmentation->Delete();
  m_Segmentation = NULL;

  if (m_SpatialCalibrationData != NULL)
  {
    m_SpatialCalibrationData->Delete();
    m_SpatialCalibrationData = NULL;
  } 

  if (m_SpatialValidationData != NULL)
  {
    m_SpatialValidationData->Delete();
    m_SpatialValidationData = NULL;
  }

  m_RecordingBuffer->Delete();
  m_RecordingBuffer = NULL;
}

//-----------------------------------------------------------------------------

void SingleWallCalibrationToolbox::OnActivated()
{
  LOG_TRACE("SingleWallCalibrationToolbox::OnActivated"); 

  if (m_State == ToolboxState_Done)
  {
    SetDisplayAccordingToState();
    return;
  }

  if ( (m_ParentMainWindow->GetVisualizationController()->GetDataCollector() != NULL)
    && (m_ParentMainWindow->GetVisualizationController()->GetDataCollector()->GetConnected()))
  {
    if (m_Calibration->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Reading single wall calibration configuration failed!");
      return;
    }

    // Read spatial calibration configuration
    if (ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Reading spatial calibration configuration failed!");
      return;
    }

    // Set initialized if it was uninitialized
    if (m_State == ToolboxState_Uninitialized || m_State == ToolboxState_Error)
    {
      SetState(ToolboxState_Idle);
    }
    else
    {
      SetDisplayAccordingToState();
    }

    // Set validation transform names for tracked frame lists
    std::string toolReferenceFrame;
    if ( (m_ParentMainWindow->GetSelectedChannel() == NULL)
      || (m_ParentMainWindow->GetSelectedChannel()->GetOwnerDevice()->GetToolReferenceFrameName() == NULL) )
    {
      LOG_ERROR("Failed to get tool reference frame name!");
      return;
    }

    toolReferenceFrame = m_ParentMainWindow->GetSelectedChannel()->GetOwnerDevice()->GetToolReferenceFrameName();
    PlusTransformName transformNameForValidation(m_ParentMainWindow->GetProbeCoordinateFrame(), toolReferenceFrame.c_str());
    m_SpatialCalibrationData->SetFrameTransformNameForValidation(transformNameForValidation);
    m_SpatialValidationData->SetFrameTransformNameForValidation(transformNameForValidation);
    m_RecordingBuffer->SetFrameTransformNameForValidation(transformNameForValidation);

    // Initialize algorithms and containers
    if ( m_Calibration->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS 
      || m_Segmentation->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS )
    {
      LOG_ERROR("Reading configuration failed!");
      return;
    }

    m_SpatialCalibrationData->Clear();
    m_SpatialValidationData->Clear();
    m_RecordingBuffer->Clear();

    m_NumberOfSegmentedCalibrationImages = 0;
    m_NumberOfSegmentedValidationImages = 0;
    m_LastRecordedFrameTimestamp = 0.0;

    m_Segmentation->SetTrackedFrameList(m_RecordingBuffer);

    m_CancelRequest = false;
  }
  else
  {
    SetState(ToolboxState_Uninitialized);
  }
}

//-----------------------------------------------------------------------------

PlusStatus SingleWallCalibrationToolbox::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  LOG_TRACE("SingleWallCalibrationToolbox::ReadConfiguration");

  if (aConfig == NULL)
  {
    LOG_ERROR("Unable to read configuration"); 
    return PLUS_FAIL; 
  }

  vtkXMLDataElement* fCalElement = aConfig->FindNestedElementWithName("fCal");

  if (fCalElement == NULL)
  {
    LOG_ERROR("Unable to find fCal element in XML tree!"); 
    return PLUS_FAIL;     
  }

  // Read number of needed images
  int numberOfCalibrationImagesToAcquire = 0; 
  if ( fCalElement->GetScalarAttribute("NumberOfCalibrationImagesToAcquire", numberOfCalibrationImagesToAcquire ) )
  {
    m_NumberOfCalibrationImagesToAcquire = numberOfCalibrationImagesToAcquire;
  }
  else
  {
    LOG_WARNING("Unable to read NumberOfCalibrationImagesToAcquire attribute from fCal element of the device set configuration, default value '" << m_NumberOfCalibrationImagesToAcquire << "' will be used");
  }

  int numberOfValidationImagesToAcquire = 0; 
  if ( fCalElement->GetScalarAttribute("NumberOfValidationImagesToAcquire", numberOfValidationImagesToAcquire ) )
  {
    m_NumberOfValidationImagesToAcquire = numberOfValidationImagesToAcquire;
  }
  else
  {
    LOG_WARNING("Unable to read NumberOfValidationImagesToAcquire attribute from fCal element of the device set configuration, default value '" << m_NumberOfValidationImagesToAcquire << "' will be used");
  }

  // Recording interval and processing time
  int recordingIntervalMs = 0; 
  if ( fCalElement->GetScalarAttribute("RecordingIntervalMs", recordingIntervalMs ) )
  {
    m_RecordingIntervalMs = recordingIntervalMs;
  }
  else
  {
    LOG_WARNING("Unable to read RecordingIntervalMs attribute from fCal element of the device set configuration, default value '" << m_RecordingIntervalMs << "' will be used");
  }

  int maxTimeSpentWithProcessingMs = 0; 
  if ( fCalElement->GetScalarAttribute("MaxTimeSpentWithProcessingMs", maxTimeSpentWithProcessingMs ) )
  {
    m_MaxTimeSpentWithProcessingMs = maxTimeSpentWithProcessingMs;
  }
  else
  {
    LOG_WARNING("Unable to read MaxTimeSpentWithProcessingMs attribute from fCal element of the device set configuration, default value '" << m_MaxTimeSpentWithProcessingMs << "' will be used");
  }

  int singleFrameCount(18);
  if( fCalElement->GetScalarAttribute("SingleFrameCount", singleFrameCount) )
  {
    m_SingleFrameCount = singleFrameCount;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void SingleWallCalibrationToolbox::RefreshContent()
{
}

//-----------------------------------------------------------------------------

void SingleWallCalibrationToolbox::SetDisplayAccordingToState()
{
  LOG_TRACE("SpatialCalibrationToolbox::SetDisplayAccordingToState");

  // If connected
  if ( (m_ParentMainWindow->GetVisualizationController()->GetDataCollector() != NULL)
    && (m_ParentMainWindow->GetVisualizationController()->GetDataCollector()->GetConnected()) )
  {
    // If the force show devices isn't enabled, set it to 2D
    if( !m_ParentMainWindow->IsForceShowDevicesEnabled() )
    {
      // 2D mode auto-turns back on the image
      if( m_ParentMainWindow->GetVisualizationController()->SetVisualizationMode(vtkVisualizationController::DISPLAY_MODE_2D) != PLUS_SUCCESS )
      {
        LOG_WARNING("Unable to switch to 2D visualization. Unable to use freehand calibration toolbox.");
        m_ParentMainWindow->GetVisualizationController()->HideRenderer();
        this->m_State = ToolboxState_Error;
      }
      else
      {
        // Enable or disable the image manipulation menu
        m_ParentMainWindow->SetImageManipulationMenuEnabled( m_ParentMainWindow->GetVisualizationController()->Is2DMode() );

        // Hide or show the orientation markers based on the value of the checkbox
        m_ParentMainWindow->GetVisualizationController()->ShowOrientationMarkers(m_ParentMainWindow->IsOrientationMarkersEnabled());
      }
    }

    // Update state message according to available transforms
    if (m_Calibration->GetImageCoordinateFrame() && m_Calibration->GetProbeCoordinateFrame())
    {
      if (m_ParentMainWindow->GetVisualizationController()->IsExistingTransform(m_Calibration->GetProbeCoordinateFrame(), m_Calibration->GetReferenceCoordinateFrame()) == PLUS_SUCCESS)
      {
        std::string imageToProbeTransformNameStr;
        PlusTransformName imageToProbeTransformName(
          m_Calibration->GetImageCoordinateFrame(), m_Calibration->GetProbeCoordinateFrame());
        imageToProbeTransformName.GetTransformName(imageToProbeTransformNameStr);

        if (m_ParentMainWindow->GetVisualizationController()->IsExistingTransform(
          m_Calibration->GetImageCoordinateFrame(), m_Calibration->GetProbeCoordinateFrame(), false) == PLUS_SUCCESS)
        {
          std::string date, errorStr;
          double error;
          if (m_ParentMainWindow->GetVisualizationController()->GetTransformRepository()->GetTransformDate(imageToProbeTransformName, date) != PLUS_SUCCESS)
          {
            date = "N/A";
          }
          if (m_ParentMainWindow->GetVisualizationController()->GetTransformRepository()->GetTransformError(imageToProbeTransformName, error) == PLUS_SUCCESS)
          {
            std::stringstream ss;
            ss << std::fixed << error;
            errorStr = ss.str();
          }
          else
          {
            errorStr = "N/A";
          }

          ui.label_State->setPaletteForegroundColor(Qt::black);
          ui.label_State->setText( QString("%1 transform present.\nDate: %2, Error: %3").arg(imageToProbeTransformNameStr.c_str()).arg(date.c_str()).arg(errorStr.c_str()) );
        }
        else
        {
          ui.label_State->setPaletteForegroundColor(QColor::fromRgb(255, 128, 0));
          ui.label_State->setText( QString("%1 transform is absent, spatial calibration needs to be performed.").arg(imageToProbeTransformNameStr.c_str()) );
          LOG_INFO(imageToProbeTransformNameStr << " transform is absent, spatial calibration needs to be performed");
        }
      }
      else
      {
        ui.label_State->setPaletteForegroundColor(QColor::fromRgb(255, 128, 0));
        ui.label_State->setText( tr("Phantom registration is missing. It needs to be performed or imported") );
        LOG_INFO("Phantom registration is missing. It needs to be performed or imported");
        m_State = ToolboxState_Error;
      }
    }
    else
    {
      ui.label_State->setPaletteForegroundColor(QColor::fromRgb(255, 128, 0));
      ui.label_State->setText( QString("Probe calibration configuration is missing!") );
      LOG_INFO("Probe calibration configuration is missing");
      m_State = ToolboxState_Error;
    }
  }
  else
  {
    ui.label_State->setPaletteForegroundColor(QColor::fromRgb(255, 128, 0));
    ui.label_State->setText(tr("fCal is not connected to devices. Switch to Configuration toolbox to connect."));
    LOG_INFO("fCal is not connected to devices");
  }

  // Set widget states according to state
  if (m_State == ToolboxState_Uninitialized)
  {
    ui.pushButton_OpenPhantomRegistration->setEnabled(false);

    ui.label_Warning->setVisible(false);
    ui.label_Results->setText(QString(""));

    ui.label_InstructionsSpatial->setText(QString(""));
    ui.pushButton_StartSpatial->setEnabled(false);
    ui.pushButton_CancelSpatial->setEnabled(false);
    ui.pushButton_SingleFrame->setEnabled(false);

    m_ParentMainWindow->SetStatusBarText(QString(""));
    m_ParentMainWindow->SetStatusBarProgress(-1);
  }
  else if (m_State == ToolboxState_Idle)
  {
    bool isReadyToStartSpatialCalibration = false;
    if ( (m_ParentMainWindow->GetVisualizationController()->GetDataCollector() != NULL)
      && (m_ParentMainWindow->GetVisualizationController()->GetDataCollector()->GetConnected()))
    {
      isReadyToStartSpatialCalibration = IsReadyToStartSpatialCalibration();
    }

    ui.pushButton_OpenPhantomRegistration->setEnabled(true);

    ui.label_Warning->setVisible(false);
    ui.label_Results->setText(QString(""));

    ui.label_InstructionsSpatial->setText(QString(""));
    ui.pushButton_CancelSpatial->setEnabled(false);
    ui.pushButton_StartSpatial->setEnabled(isReadyToStartSpatialCalibration);
    ui.pushButton_SingleFrame->setEnabled(isReadyToStartSpatialCalibration);
    ui.pushButton_StartSpatial->setFocus();

    m_ParentMainWindow->SetStatusBarText(QString(""));
    m_ParentMainWindow->SetStatusBarProgress(-1);

    QApplication::restoreOverrideCursor();
  }
  else if (m_State == ToolboxState_InProgress)
  {
    ui.pushButton_OpenPhantomRegistration->setEnabled(false);

    ui.label_Warning->setVisible(false);
    ui.label_Results->setText(QString(""));

    m_ParentMainWindow->SetStatusBarText(QString(" Acquiring and adding images to calibrator"));
    m_ParentMainWindow->SetStatusBarProgress(0);

    ui.label_InstructionsSpatial->setText(tr("Scan the bottom of your tank until the progress bar is filled."));
    ui.pushButton_StartSpatial->setEnabled(false);
    ui.pushButton_CancelSpatial->setEnabled(true);
    ui.pushButton_SingleFrame->setEnabled(true);
    ui.pushButton_CancelSpatial->setFocus();
  }
  else if (m_State == ToolboxState_Done)
  {
    ui.pushButton_OpenPhantomRegistration->setEnabled(true);

    ui.label_InstructionsSpatial->setText(tr("Spatial calibration is ready to save"));
    ui.label_Warning->setVisible(false);
    ui.label_Results->setText(m_Calibration->GetResultString().c_str());

    ui.pushButton_StartSpatial->setEnabled(true);
    ui.pushButton_SingleFrame->setEnabled(true);
    ui.pushButton_CancelSpatial->setEnabled(false);

    m_ParentMainWindow->SetStatusBarText(QString(" Calibration done"));
    m_ParentMainWindow->SetStatusBarProgress(-1);

    QApplication::restoreOverrideCursor();
  }
  else if (m_State == ToolboxState_Error)
  {
    ui.pushButton_OpenPhantomRegistration->setEnabled(false);

    ui.label_Warning->setVisible(false);
    ui.label_Results->setText(QString(""));

    ui.label_InstructionsSpatial->setText(QString(""));
    ui.pushButton_StartSpatial->setEnabled(false);
    ui.pushButton_CancelSpatial->setEnabled(false);
    ui.pushButton_SingleFrame->setEnabled(false);

    m_ParentMainWindow->SetStatusBarText(QString(""));
    m_ParentMainWindow->SetStatusBarProgress(-1);

    QApplication::restoreOverrideCursor();
  }
}

//-----------------------------------------------------------------------------

void SingleWallCalibrationToolbox::StartCalibration()
{
  LOG_INFO("Spatial calibration started");

  m_ParentMainWindow->SetToolboxesEnabled(false);

  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

  SetState(ToolboxState_InProgress);

  // Start calibration and compute results on success
  DoCalibration();
}

//-----------------------------------------------------------------------------

void SingleWallCalibrationToolbox::DoCalibration()
{
  LOG_TRACE("SingleWallCalibrationToolbox::DoSpatialCalibration");

  // Get current time
  double startTimeSec = vtkAccurateTimer::GetSystemTime();

  // Calibrate if acquisition is ready
  if ( m_NumberOfSegmentedCalibrationImages >= m_NumberOfCalibrationImagesToAcquire
    && m_NumberOfSegmentedValidationImages >= m_NumberOfValidationImagesToAcquire)
  {
    LOG_INFO("Segmentation success rate: " << m_NumberOfSegmentedCalibrationImages + m_NumberOfSegmentedValidationImages << " out of " << m_SpatialCalibrationData->GetNumberOfTrackedFrames() + m_SpatialValidationData->GetNumberOfTrackedFrames() << " (" << (int)(((double)(m_NumberOfSegmentedCalibrationImages + m_NumberOfSegmentedValidationImages) / (double)(m_SpatialCalibrationData->GetNumberOfTrackedFrames() + m_SpatialValidationData->GetNumberOfTrackedFrames())) * 100.0 + 0.49) << " percent)");

    m_Calibration->SetTrackedFrameList(m_SpatialCalibrationData);

    if (m_Calibration->Calibrate() != PLUS_SUCCESS)
    {
      LOG_ERROR("Calibration failed!");
      CancelCalibration();
      return;
    }

    if (SetAndSaveResults() != PLUS_SUCCESS)
    {
      LOG_ERROR("Setting and saving results failed!");
      CancelCalibration();
      return;
    }

    m_SpatialCalibrationData->Clear();
    m_SpatialValidationData->Clear();

    SetState(ToolboxState_Done);

    m_ParentMainWindow->SetToolboxesEnabled(true);
    m_ParentMainWindow->GetVisualizationController()->EnableWireLabels(false);
    m_ParentMainWindow->GetVisualizationController()->ShowResult(false);

    return;
  }


  // Cancel if requested
  if (m_CancelRequest)
  {
    LOG_INFO("Calibration process cancelled by the user");
    CancelCalibration();
    return;
  }

  // Determine which data container to use
  vtkTrackedFrameList* trackedFrameListToUse = NULL;
  if (m_NumberOfSegmentedValidationImages < m_NumberOfValidationImagesToAcquire)
  {
    trackedFrameListToUse = m_SpatialValidationData;
  }
  else
  {
    trackedFrameListToUse = m_SpatialCalibrationData;
  }

  // Acquire tracked frames since last acquisition (minimum 1 frame)
  if (m_LastProcessingTimePerFrameMs < 1)
  {
    // if processing was less than 1ms/frame then assume it was 1ms (1000FPS processing speed) to avoid division by zero
    m_LastProcessingTimePerFrameMs = 1;
  }
  int numberOfFramesToGet = std::max(m_MaxTimeSpentWithProcessingMs / m_LastProcessingTimePerFrameMs, 1);

  this->m_RecordingBuffer->Clear();
  if ( m_ParentMainWindow->GetSelectedChannel() != NULL && m_ParentMainWindow->GetSelectedChannel()->GetTrackedFrameList(
    m_LastRecordedFrameTimestamp, this->m_RecordingBuffer, numberOfFramesToGet) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get tracked frame list from data collector (last recorded timestamp: " << std::fixed << m_LastRecordedFrameTimestamp ); 
    CancelCalibration();
    return; 
  }

  m_Segmentation->Reset();
  if( m_Segmentation->Update() != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to segment image. Adjust position of line in image.");
    QTimer::singleShot(100, this, SLOT(DoCalibration())); 
    return;
  }
  // TODO: also, show it on the overlay so the user can see if the segmentation is good

  int numberOfNewlySegmentedImages = this->m_RecordingBuffer->GetNumberOfTrackedFrames();
  trackedFrameListToUse->AddTrackedFrameList(m_RecordingBuffer, vtkTrackedFrameList::SKIP_INVALID_FRAME);

  if (m_NumberOfSegmentedValidationImages < m_NumberOfValidationImagesToAcquire)
  {
    m_NumberOfSegmentedValidationImages += numberOfNewlySegmentedImages;
  }
  else
  {
    m_NumberOfSegmentedCalibrationImages += numberOfNewlySegmentedImages;
  }

  LOG_DEBUG("Number of segmented images in this round: " << numberOfNewlySegmentedImages << " out of " << m_RecordingBuffer->GetNumberOfTrackedFrames());

  // Update progress if tracked frame has been successfully added
  int progressPercent = (int)(((m_NumberOfSegmentedCalibrationImages + m_NumberOfSegmentedValidationImages) / (double)(std::max(m_NumberOfValidationImagesToAcquire, m_NumberOfSegmentedValidationImages) + m_NumberOfCalibrationImagesToAcquire)) * 100.0);
  m_ParentMainWindow->SetStatusBarProgress(progressPercent);

  // Compute time spent with processing one frame in this round
  double computationTimeMs = (vtkAccurateTimer::GetSystemTime() - startTimeSec) * 1000.0;

  // Update last processing time if new tracked frames have been acquired
  if (trackedFrameListToUse->GetNumberOfTrackedFrames() > 0)
  {
    m_LastProcessingTimePerFrameMs = computationTimeMs / (m_RecordingBuffer->GetNumberOfTrackedFrames());
  }

  // Launch timer to run acquisition again
  int waitTimeMs = std::max((int)(m_RecordingIntervalMs - computationTimeMs), 0);

  if (waitTimeMs == 0)
  {
    LOG_WARNING("Processing cannot keep up with aquisition! Try to decrease MaxTimeSpentWithProcessingMs parameter in device set configuration (it should be more than the processing time (the last one was " << m_LastProcessingTimePerFrameMs << "), so if it is already small, try to increase RecordingIntervalMs too)");
  }

  LOG_DEBUG("Number of requested frames: " << numberOfFramesToGet);
  LOG_DEBUG("Number of tracked frames in the list: " << std::setw(3) << m_RecordingBuffer->GetNumberOfTrackedFrames());
  LOG_DEBUG("Last processing time: " << m_LastProcessingTimePerFrameMs);
  LOG_DEBUG("Computation time: " << computationTimeMs);
  LOG_DEBUG("Waiting time: " << waitTimeMs);

  QTimer::singleShot(waitTimeMs, this, SLOT(DoCalibration())); 
}

//-----------------------------------------------------------------------------

PlusStatus SingleWallCalibrationToolbox::SetAndSaveResults()
{
  LOG_TRACE("SingleWallCalibrationToolbox::SetAndSaveResults");

  // Set transducer origin related transforms
  vtkSmartPointer<vtkTransform> imageToProbeTransform = vtkSmartPointer<vtkTransform>::New();
  vtkSmartPointer<vtkMatrix4x4> imageToProbeTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  imageToProbeTransformMatrix->DeepCopy(&(m_Calibration->GetImageToProbeTransformation()));
  imageToProbeTransform->SetMatrix(imageToProbeTransformMatrix);
  double* imageToProbeScale = imageToProbeTransform->GetScale();
  vtkSmartPointer<vtkTransform> transducerOriginPixelToTransducerOriginTransform = vtkSmartPointer<vtkTransform>::New();
  transducerOriginPixelToTransducerOriginTransform->Identity();
  transducerOriginPixelToTransducerOriginTransform->Scale(imageToProbeScale);

  PlusTransformName transducerOriginPixelToTransducerOriginTransformName(m_ParentMainWindow->GetTransducerOriginPixelCoordinateFrame(), m_ParentMainWindow->GetTransducerOriginCoordinateFrame());
  m_ParentMainWindow->GetVisualizationController()->GetTransformRepository()->SetTransform(transducerOriginPixelToTransducerOriginTransformName, transducerOriginPixelToTransducerOriginTransform->GetMatrix());
  m_ParentMainWindow->GetVisualizationController()->GetTransformRepository()->SetTransformPersistent(transducerOriginPixelToTransducerOriginTransformName, true);
  m_ParentMainWindow->GetVisualizationController()->GetTransformRepository()->SetTransformDate(transducerOriginPixelToTransducerOriginTransformName, vtkAccurateTimer::GetInstance()->GetDateAndTimeString().c_str());

  // Set result for visualization
  vtkDisplayableObject* object = m_ParentMainWindow->GetVisualizationController()->GetObjectById(m_ParentMainWindow->GetTransducerModelId());
  if (object != NULL)
  {
    object->DisplayableOn();
  }
  else
  {
    LOG_WARNING("Missing probe displayable object.");
  }
  object = m_ParentMainWindow->GetVisualizationController()->GetObjectById(m_ParentMainWindow->GetImageObjectId());
  if (object != NULL)
  {
    object->DisplayableOn();
  }
  else
  {
    LOG_WARNING("Missing image displayable object.");
  }

  // Save result in configuration
  if ( m_ParentMainWindow->GetVisualizationController()->GetTransformRepository()->WriteConfiguration( vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData() ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to save freehand calibration result in configuration XML tree!");
    SetState(ToolboxState_Error);
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void SingleWallCalibrationToolbox::CancelCalibration()
{
  LOG_INFO("Calibration cancelled");

  m_CancelRequest = true;

  m_RecordingBuffer->Clear();
  m_ParentMainWindow->SetToolboxesEnabled(true);
  m_ParentMainWindow->GetVisualizationController()->EnableWireLabels(false);
  m_ParentMainWindow->GetVisualizationController()->ShowResult(false);

  SetState(ToolboxState_Idle);
}

//-----------------------------------------------------------------------------

void SingleWallCalibrationToolbox::SingleFrame()
{
  TrackedFrame frame;
  if( m_ParentMainWindow->GetSelectedChannel()->GetTrackedFrame(&frame) != PLUS_SUCCESS )
  {
    LOG_WARNING("Unable to acquire a frame from selected channel. Try again.");
    return;
  }

  if( m_Segmentation->Update() != PLUS_SUCCESS )
  {
    LOG_WARNING("Unable to segment line. Try again.");
    return;
  }

  if( m_ParentMainWindow->GetVisualizationController()->Is2DMode() )
  {
    // We're in 2d mode, let's try to visualize the segmentation

  }

  if( m_RecordingBuffer->AddTrackedFrame(&frame, vtkTrackedFrameList::SKIP_INVALID_FRAME) != PLUS_SUCCESS )
  {
    LOG_WARNING("Unable to add frame to list. Try again.");
    return;
  }

  this->SetState(ToolboxState_InProgress);

  std::stringstream ss;
  ss << "Frame: " << m_RecordingBuffer->GetNumberOfTrackedFrames()<< " of " << m_SingleFrameCount << " recorded.";
  ui.label_Results->setText(ss.str().c_str());

  if( m_RecordingBuffer->GetNumberOfTrackedFrames() >= m_SingleFrameCount )
  {
    m_Calibration->SetTrackedFrameList(m_RecordingBuffer);

    if (m_Calibration->Calibrate() != PLUS_SUCCESS)
    {
      LOG_ERROR("Calibration failed!");
      return;
    }

    if (SetAndSaveResults() != PLUS_SUCCESS)
    {
      LOG_ERROR("Setting and saving results failed!");
      return;
    }

    m_RecordingBuffer->Clear();

    this->SetState(ToolboxState_Done);
    this->SetDisplayAccordingToState();
  }
}

//-----------------------------------------------------------------------------

bool SingleWallCalibrationToolbox::IsReadyToStartSpatialCalibration()
{
  LOG_TRACE("SpatialCalibrationToolbox::IsReadyToStartSpatialCalibration");

  // Everything is fine, ready for spatial calibration
  ui.label_InstructionsSpatial->setText(tr("Press Start and start scanning the phantom"));

  return true;
}

//-----------------------------------------------------------------------------

void SingleWallCalibrationToolbox::Reset()
{
  AbstractToolbox::Reset();

  if (m_Calibration != NULL)
  {
    m_Calibration->Delete();
    m_Calibration = NULL;
  }

  if (m_SpatialCalibrationData != NULL)
  {
    m_SpatialCalibrationData->Delete();
    m_SpatialCalibrationData = NULL;
  } 

  if (m_SpatialValidationData != NULL)
  {
    m_SpatialValidationData->Delete();
    m_SpatialValidationData = NULL;
  }

  // Create algorithms
  m_Calibration = vtkSingleWallCalibrationAlgo::New();

  // Create tracked frame lists
  m_SpatialCalibrationData = vtkTrackedFrameList::New();
  m_SpatialCalibrationData->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP | REQUIRE_TRACKING_OK); 

  m_SpatialValidationData = vtkTrackedFrameList::New();
  m_SpatialValidationData->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP | REQUIRE_TRACKING_OK); 
}

//-----------------------------------------------------------------------------

void SingleWallCalibrationToolbox::OnDeactivated()
{

}
