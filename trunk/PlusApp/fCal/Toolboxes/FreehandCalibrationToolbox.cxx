/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "FreehandCalibrationToolbox.h"

#include "vtkProbeCalibrationAlgo.h"
#include "TemporalCalibrationAlgo.h"
#include "vtkDataCollectorHardwareDevice.h"
#include "vtkTrackedFrameList.h"
#include "TrackedFrame.h"

#include "vtkPlusVideoSource.h" // Only for getting the local time offset in device mode
#include "vtkVideoBuffer.h" // Only for getting the local time offset in device mode
#include "vtkTracker.h" // Only for getting the local time offset in device mode
#include "vtkTrackerTool.h" // Only for getting the local time offset in device mode
#include "vtkTrackerBuffer.h" // Only for getting the local time offset in device mode

#include "ConfigFileSaverDialog.h"
#include "SegmentationParameterDialog.h"

#include "fCalMainWindow.h"
#include "vtkObjectVisualizer.h"

#include "FidPatternRecognition.h"

#include <vtkXMLUtilities.h>
#include <vtkContextView.h>
#include <vtkChartXY.h>
#include <vtkTable.h>
#include <vtkPlot.h>

#include <QFileDialog>
#include <QTimer>

//-----------------------------------------------------------------------------

FreehandCalibrationToolbox::FreehandCalibrationToolbox(fCalMainWindow* aParentMainWindow, Qt::WFlags aFlags)
  : AbstractToolbox(aParentMainWindow)
  , QWidget(aParentMainWindow, aFlags)
  , m_CancelRequest(false)
  , m_LastRecordedFrameTimestamp(0.0)
  , m_NumberOfCalibrationImagesToAcquire(200)
  , m_NumberOfValidationImagesToAcquire(100)
  , m_NumberOfSegmentedCalibrationImages(0)
  , m_NumberOfSegmentedValidationImages(0)
  , m_RecordingIntervalMs(200)
  , m_MaxTimeSpentWithProcessingMs(150)
  , m_TemporalCalibrationDurationSec(10)
  , m_LastProcessingTimePerFrameMs(-1)
  , m_StartTimeSec(0.0)
  , m_PreviousTrackerOffset(-1.0)
  , m_PreviousVideoOffset(-1.0)
  , m_SpatialCalibrationInProgress(false)
  , m_TemporalCalibrationInProgress(false)
  , m_TemporalCalibrationPlotsWindow(NULL)
  , m_UncalibratedPlotContextView(NULL)
  , m_CalibratedPlotContextView(NULL)
{
  ui.setupUi(this);

  // Create algorithms
  m_Calibration = vtkProbeCalibrationAlgo::New();

  m_PatternRecognition = new FidPatternRecognition();

  // Create tracked frame lists
  m_SpatialCalibrationData = vtkTrackedFrameList::New();
  m_SpatialCalibrationData->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP | REQUIRE_TRACKING_OK); 

  m_SpatialValidationData = vtkTrackedFrameList::New();
  m_SpatialValidationData->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP | REQUIRE_TRACKING_OK); 

  m_TemporalCalibrationTrackingData = vtkTrackedFrameList::New();
  m_TemporalCalibrationTrackingData->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP | REQUIRE_TRACKING_OK); 

  m_TemporalCalibrationVideoData = vtkTrackedFrameList::New();
  m_TemporalCalibrationVideoData->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP | REQUIRE_TRACKING_OK); 

  // Create temporal calibration metric tables
  m_VideoPositionMetric = vtkTable::New();
  m_UncalibratedTrackerPositionMetric = vtkTable::New();
  m_CalibratedTrackerPositionMetric = vtkTable::New();

  // Change result display properties
  ui.label_Results->setFont(QFont("Courier", 8));

  // Connect events
  connect( ui.pushButton_OpenPhantomRegistration, SIGNAL( clicked() ), this, SLOT( OpenPhantomRegistration() ) );
  connect( ui.pushButton_OpenSegmentationParameters, SIGNAL( clicked() ), this, SLOT( OpenSegmentationParameters() ) );
  connect( ui.pushButton_EditSegmentationParameters, SIGNAL( clicked() ), this, SLOT( EditSegmentationParameters() ) );
  connect( ui.pushButton_StartTemporal, SIGNAL( clicked() ), this, SLOT( StartTemporal() ) );
  connect( ui.pushButton_CancelTemporal, SIGNAL( clicked() ), this, SLOT( CancelCalibration() ) );
  connect( ui.pushButton_StartSpatial, SIGNAL( clicked() ), this, SLOT( StartSpatial() ) );
  connect( ui.pushButton_CancelSpatial, SIGNAL( clicked() ), this, SLOT( CancelCalibration() ) );
  connect( ui.pushButton_ShowPlots, SIGNAL( toggled(bool) ), this, SLOT( ShowPlotsToggled(bool) ) );
}

//-----------------------------------------------------------------------------

FreehandCalibrationToolbox::~FreehandCalibrationToolbox()
{
  if (m_Calibration != NULL) {
    m_Calibration->Delete();
    m_Calibration = NULL;
  } 

  if (m_PatternRecognition != NULL) {
    delete m_PatternRecognition;
    m_PatternRecognition = NULL;
  } 

  if (m_SpatialCalibrationData != NULL) {
    m_SpatialCalibrationData->Delete();
    m_SpatialCalibrationData = NULL;
  } 

  if (m_SpatialValidationData != NULL) {
    m_SpatialValidationData->Delete();
    m_SpatialValidationData = NULL;
  } 

  if (m_TemporalCalibrationTrackingData != NULL) {
    m_TemporalCalibrationTrackingData->Delete();
    m_TemporalCalibrationTrackingData = NULL;
  } 

  if (m_TemporalCalibrationVideoData != NULL) {
    m_TemporalCalibrationVideoData->Delete();
    m_TemporalCalibrationVideoData = NULL;
  } 

  if (m_VideoPositionMetric != NULL) {
    m_VideoPositionMetric->Delete();
    m_VideoPositionMetric = NULL;
  } 

  if (m_UncalibratedTrackerPositionMetric != NULL) {
    m_UncalibratedTrackerPositionMetric->Delete();
    m_UncalibratedTrackerPositionMetric = NULL;
  } 

  if (m_CalibratedTrackerPositionMetric != NULL) {
    m_CalibratedTrackerPositionMetric->Delete();
    m_CalibratedTrackerPositionMetric = NULL;
  } 

  if (m_TemporalCalibrationPlotsWindow != NULL) {
    delete m_TemporalCalibrationPlotsWindow;
    m_TemporalCalibrationPlotsWindow = NULL;
  } 

  if (m_UncalibratedPlotContextView != NULL) {
    m_UncalibratedPlotContextView->Delete();
    m_UncalibratedPlotContextView = NULL;
  } 

  if (m_CalibratedPlotContextView != NULL) {
    m_CalibratedPlotContextView->Delete();
    m_CalibratedPlotContextView = NULL;
  } 
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::Initialize()
{
  LOG_TRACE("FreehandCalibrationToolbox::Initialize"); 

  if (m_State == ToolboxState_Done)
  {
    SetDisplayAccordingToState();
    return;
  }

  // Clear results poly data
  m_ParentMainWindow->GetObjectVisualizer()->GetResultPolyData()->Initialize();

  if ( (m_ParentMainWindow->GetObjectVisualizer()->GetDataCollector() != NULL)
    && (m_ParentMainWindow->GetObjectVisualizer()->GetDataCollector()->GetConnected()))
  {
    //m_ParentMainWindow->GetObjectVisualizer()->GetDataCollector()->SetTrackingOnly(false);

    if (m_Calibration->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Reading probe calibration algorithm configuration failed!");
      return;
    }

    // Read freehand calibration configuration
    if (ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Reading calibration configuration failed!");
      return;
    }

    // Check if probe to reference transform is available
    if (m_ParentMainWindow->GetObjectVisualizer()->IsExistingTransform(m_Calibration->GetProbeCoordinateFrame(), m_Calibration->GetReferenceCoordinateFrame()) != PLUS_SUCCESS)
    {
      LOG_ERROR("No transform found between probe and reference!");
      return;
    }

    // Set initialized if it was uninitialized
    if (m_State == ToolboxState_Uninitialized)
    {
      SetState(ToolboxState_Idle);
    }
    else
    {
      SetDisplayAccordingToState();
    }
  }
  else
  {
    SetState(ToolboxState_Uninitialized);
  }
}

//-----------------------------------------------------------------------------

PlusStatus FreehandCalibrationToolbox::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  LOG_TRACE("FreehandCalibrationToolbox::ReadConfiguration");

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

  bool success = true;

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

  // Duration of temporal calibration
  int temporalCalibrationDurationSec = 0; 
  if ( fCalElement->GetScalarAttribute("TemporalCalibrationDurationSec", temporalCalibrationDurationSec ) )
  {
    m_TemporalCalibrationDurationSec = temporalCalibrationDurationSec;
  }
  else
  {
    LOG_WARNING("Unable to read TemporalCalibrationDurationSec attribute from fCal element of the device set configuration, default value '" << m_TemporalCalibrationDurationSec << "' will be used");
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::RefreshContent()
{
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::SetDisplayAccordingToState()
{
  LOG_TRACE("FreehandCalibrationToolbox::SetDisplayAccordingToState");

  if (m_ParentMainWindow->AreDevicesShown() == false)
  {
    m_ParentMainWindow->GetObjectVisualizer()->HideAll();
    m_ParentMainWindow->GetObjectVisualizer()->EnableImageMode(true);
  }

  // Enable or disable the image manipulation menu
  m_ParentMainWindow->SetImageManipulationEnabled(m_ParentMainWindow->GetObjectVisualizer()->GetImageMode() == true);
  // Hide or show the orientation markers based on the value of the checkbox
  m_ParentMainWindow->GetObjectVisualizer()->EnableOrientationMarkers(m_ParentMainWindow->IsOrientationMarkersEnabled());

  double videoTimeOffset = 0.0;
  if (m_ParentMainWindow->GetObjectVisualizer()->GetDataCollector() != NULL)
  {
    vtkDataCollectorHardwareDevice* dataCollectorHardwareDevice = dynamic_cast<vtkDataCollectorHardwareDevice*>(m_ParentMainWindow->GetObjectVisualizer()->GetDataCollector());
    if ( dataCollectorHardwareDevice )
    {
      if ( (dataCollectorHardwareDevice->GetVideoSource() != NULL)
        && (dataCollectorHardwareDevice->GetVideoSource()->GetBuffer() != NULL))
      {
        videoTimeOffset = dataCollectorHardwareDevice->GetVideoSource()->GetBuffer()->GetLocalTimeOffsetSec();
      }
    }
  }

  if (m_State == ToolboxState_Uninitialized)
  {
    ui.pushButton_OpenPhantomRegistration->setEnabled(false);
    ui.pushButton_OpenSegmentationParameters->setEnabled(false);
    ui.pushButton_EditSegmentationParameters->setEnabled(false);

    ui.label_Warning->setVisible(false);
    ui.label_Results->setText(QString(""));

    ui.label_InstructionsSpatial->setText(QString(""));
    ui.pushButton_StartSpatial->setEnabled(false);
    ui.pushButton_CancelSpatial->setEnabled(false);

    ui.label_InstructionsTemporal->setText(QString(""));
    ui.pushButton_StartTemporal->setEnabled(false);
    ui.pushButton_CancelTemporal->setEnabled(false);
    ui.pushButton_ShowPlots->setEnabled(false);

    m_ParentMainWindow->SetStatusBarText(QString(""));
    m_ParentMainWindow->SetStatusBarProgress(-1);
  }
  else if (m_State == ToolboxState_Idle)
  {
    bool isReadyToStartSpatialCalibration = false;
    if ( (m_ParentMainWindow->GetObjectVisualizer()->GetDataCollector() != NULL)
      && (m_ParentMainWindow->GetObjectVisualizer()->GetDataCollector()->GetConnected()))
    {
      isReadyToStartSpatialCalibration = IsReadyToStartSpatialCalibration();
    }

    ui.pushButton_OpenPhantomRegistration->setEnabled(true);
    ui.pushButton_OpenSegmentationParameters->setEnabled(true);
    ui.pushButton_EditSegmentationParameters->setEnabled(true);

    ui.label_Warning->setVisible(false);
    ui.label_Results->setText(QString(""));

    ui.frame_SpatialCalibration->setEnabled(true);
    ui.label_InstructionsSpatial->setText(QString(""));
    ui.pushButton_CancelSpatial->setEnabled(false);
    ui.pushButton_StartSpatial->setEnabled(isReadyToStartSpatialCalibration);
    ui.pushButton_StartSpatial->setFocus();

    ui.frame_TemporalCalibration->setEnabled(true);
    ui.label_InstructionsTemporal->setText(tr("Current video time offset: %1 s\nMove probe to vertical position in the water tank so that the bottom is visible and press Start").arg(videoTimeOffset));
    ui.pushButton_StartTemporal->setEnabled(true);
    ui.pushButton_CancelTemporal->setEnabled(false);
    ui.pushButton_ShowPlots->setEnabled(false);

    m_ParentMainWindow->SetStatusBarText(QString(""));
    m_ParentMainWindow->SetStatusBarProgress(-1);

    QApplication::restoreOverrideCursor();
  }
  else if (m_State == ToolboxState_InProgress)
  {
    ui.pushButton_OpenPhantomRegistration->setEnabled(false);
    ui.pushButton_OpenSegmentationParameters->setEnabled(false);
    ui.pushButton_EditSegmentationParameters->setEnabled(false);

    ui.label_Warning->setVisible(false);
    ui.label_Results->setText(QString(""));

    m_ParentMainWindow->SetStatusBarText(QString(" Acquiring and adding images to calibrator"));
    m_ParentMainWindow->SetStatusBarProgress(0);

    if (m_SpatialCalibrationInProgress)
    {
      ui.label_InstructionsSpatial->setText(tr("Scan the phantom in the most degrees of freedom possible until the progress bar is filled.\nIf the segmentation does not work (green dots on wires do not appear) then cancel and edit segmentation parameters"));
      ui.frame_SpatialCalibration->setEnabled(true);
      ui.pushButton_StartSpatial->setEnabled(false);
      ui.pushButton_CancelSpatial->setEnabled(true);
      ui.pushButton_CancelSpatial->setFocus();
    }
    else
    {
      ui.label_InstructionsSpatial->setText(QString(""));
      ui.frame_SpatialCalibration->setEnabled(false);
    }

    if (m_TemporalCalibrationInProgress)
    {
      ui.label_InstructionsTemporal->setText(tr("Move probe up and down so that the tank bottom is visible with 2s period until the progress bar is filled"));
      ui.frame_TemporalCalibration->setEnabled(true);
      ui.pushButton_StartTemporal->setEnabled(false);
      ui.pushButton_CancelTemporal->setEnabled(true);
      ui.pushButton_CancelTemporal->setFocus();
    }
    else
    {
      ui.label_InstructionsTemporal->setText(tr("Current video time offset: %1 s").arg(videoTimeOffset));
      ui.frame_TemporalCalibration->setEnabled(false);
    }
  }
  else if (m_State == ToolboxState_Done)
  {
    ui.pushButton_OpenPhantomRegistration->setEnabled(true);
    ui.pushButton_OpenSegmentationParameters->setEnabled(true);
    ui.pushButton_EditSegmentationParameters->setEnabled(true);

    if (m_SpatialCalibrationInProgress)
    {
      ui.label_InstructionsSpatial->setText(tr("Spatial calibration is ready to save"));

      if (m_Calibration->GetCalibrationReprojectionError3DMean() >= 2.0)
      {
        QPalette palette;
        palette.setBrush(QPalette::WindowText, QBrush(QColor::fromRgb(223, 0, 0)));
        ui.label_Warning->setPalette(palette);
        ui.label_Warning->setVisible(true);
        ui.label_Warning->setText(tr("Calibration error is too high!\n  Please re-calibrate"));
      }
      else if (m_Calibration->GetCalibrationReprojectionError3DMean() >= 1.0)
      {
        QPalette palette;
        palette.setBrush(QPalette::WindowText, QBrush(QColor::fromRgb(255, 128, 0)));
        ui.label_Warning->setPalette(palette);
        ui.label_Warning->setVisible(true);
        ui.label_Warning->setText(tr("Calibration error is relatively high!\n  Consider re-calibrating"));
      }
      else
      {
        ui.label_Warning->setVisible(false);
      }
      ui.label_Results->setText(m_Calibration->GetResultString().c_str());
    }
    else
    {
      ui.label_InstructionsSpatial->setText(QString(""));
      ui.label_Warning->setVisible(false);
      ui.label_Results->setText(QString(""));
    }
    ui.frame_SpatialCalibration->setEnabled(true);
    ui.pushButton_StartSpatial->setEnabled(true);
    ui.pushButton_CancelSpatial->setEnabled(false);

    if (m_TemporalCalibrationInProgress)
    {
      ui.label_InstructionsTemporal->setText(tr("Temporal calibration is ready to save and its result plots can be viewed\n(Video time offset: %1 s)").arg(videoTimeOffset));
      ui.pushButton_ShowPlots->setEnabled(true);
    }
    else
    {
      ui.label_InstructionsTemporal->setText(tr("Current video time offset: %1 s").arg(videoTimeOffset));
    }
    ui.frame_TemporalCalibration->setEnabled(true);
    ui.pushButton_StartTemporal->setEnabled(true);
    ui.pushButton_CancelTemporal->setEnabled(false);

    m_ParentMainWindow->SetStatusBarText(QString(" Calibration done"));
    m_ParentMainWindow->SetStatusBarProgress(-1);

    QApplication::restoreOverrideCursor();
  }
  else if (m_State == ToolboxState_Error)
  {
    ui.pushButton_OpenPhantomRegistration->setEnabled(false);
    ui.pushButton_OpenSegmentationParameters->setEnabled(false);

    ui.label_Warning->setVisible(false);
    ui.label_Results->setText(QString(""));

    ui.label_InstructionsSpatial->setText(QString(""));
    ui.pushButton_StartSpatial->setEnabled(false);
    ui.pushButton_CancelSpatial->setEnabled(false);

    ui.label_InstructionsTemporal->setText(tr("Error occured!"));
    ui.pushButton_StartTemporal->setEnabled(false);
    ui.pushButton_CancelTemporal->setEnabled(false);
    ui.pushButton_ShowPlots->setEnabled(false);

    m_ParentMainWindow->SetStatusBarText(QString(""));
    m_ParentMainWindow->SetStatusBarProgress(-1);

    QApplication::restoreOverrideCursor();
  }
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::OpenPhantomRegistration()
{
  LOG_TRACE("FreehandCalibrationToolbox::OpenPhantomRegistrationClicked"); 

  // File open dialog for selecting phantom registration xml
  QString filter = QString( tr( "XML files ( *.xml );;" ) );
  QString fileName = QFileDialog::getOpenFileName(NULL, QString( tr( "Open phantom registration XML" ) ), vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory(), filter);
  if (fileName.isNull())
  {
    return;
  }

  // Parse XML file
  vtkSmartPointer<vtkXMLDataElement> rootElement = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(fileName.toAscii().data()));
  if (rootElement == NULL)
  {	
    LOG_ERROR("Unable to read the configuration file: " << fileName.toAscii().data()); 
    return;
  }

  // Read phantom registration transform
  PlusTransformName phantomToReferenceTransformName(m_Calibration->GetPhantomCoordinateFrame(), m_Calibration->GetReferenceCoordinateFrame());
  vtkSmartPointer<vtkMatrix4x4> phantomToReferenceTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  std::string transformDate;
  double transformError = 0.0;
  bool valid = false;
  vtkTransformRepository* tempTransformRepo = vtkTransformRepository::New();
  if ( tempTransformRepo->ReadConfiguration( vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData() ) != PLUS_SUCCESS
    || tempTransformRepo->GetTransform(phantomToReferenceTransformName, phantomToReferenceTransformMatrix, &valid) != PLUS_SUCCESS
    || tempTransformRepo->GetTransformDate(phantomToReferenceTransformName, transformDate) != PLUS_SUCCESS
    || tempTransformRepo->GetTransformError(phantomToReferenceTransformName, transformError) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read transform from opened file!");
    tempTransformRepo->Delete();
    return;
  }

  tempTransformRepo->Delete();

  if (valid)
  {
    if (m_ParentMainWindow->GetObjectVisualizer()->GetTransformRepository()->SetTransform(phantomToReferenceTransformName, phantomToReferenceTransformMatrix) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set phantom registration transform to transform repository!");
      return;
    }

    m_ParentMainWindow->GetObjectVisualizer()->GetTransformRepository()->SetTransformDate(phantomToReferenceTransformName, transformDate.c_str());
    m_ParentMainWindow->GetObjectVisualizer()->GetTransformRepository()->SetTransformError(phantomToReferenceTransformName, transformError);
    m_ParentMainWindow->GetObjectVisualizer()->GetTransformRepository()->SetTransformPersistent(phantomToReferenceTransformName, true);
  }
  else
  {
    LOG_ERROR("Invalid phantom registration transform found, it was not set!");
  }

  SetDisplayAccordingToState();

  LOG_INFO("Phantom registration imported in freehand calibration toolbox from file '" << fileName.toAscii().data() << "'");
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::OpenSegmentationParameters()
{
  LOG_TRACE("FreehandCalibrationToolbox::OpenSegmentationParameters"); 

  // File open dialog for selecting calibration configuration xml
  QString filter = QString( tr( "XML files ( *.xml );;" ) );
  QString fileName = QFileDialog::getOpenFileName(NULL, QString( tr( "Open calibration configuration XML" ) ), vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory(), filter);
  if (fileName.isNull())
  {
    return;
  }

  // Parse XML file
  vtkSmartPointer<vtkXMLDataElement> rootElement = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(fileName.toAscii().data()));
  if (rootElement == NULL)
  {
    LOG_ERROR("Unable to read the configuration file: " << fileName.toAscii().data()); 
    return;
  }

  // Load calibration configuration xml
  if (m_PatternRecognition->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to import segmentation parameters!");
    return;
  }

  // Replace USCalibration element with the one in the just read file
  vtkPlusConfig::ReplaceElementInDeviceSetConfiguration("Segmentation", rootElement);

  // Re-calculate camera parameters
  m_ParentMainWindow->GetObjectVisualizer()->CalculateImageCameraParameters();

  SetDisplayAccordingToState();

  LOG_INFO("Segmentation parameters imported in freehand calibration toolbox from file '" << fileName.toAscii().data() << "'");
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::EditSegmentationParameters()
{
  LOG_INFO("Edit segmentation parameters started");

  // Disconnect realtime image from main canvas
  m_ParentMainWindow->GetObjectVisualizer()->GetImageActor()->SetInput(NULL);

  // Show segmentation parameter dialog
  SegmentationParameterDialog* segmentationParamDialog = new SegmentationParameterDialog(this, m_ParentMainWindow->GetObjectVisualizer()->GetDataCollector());
  segmentationParamDialog->exec();

  delete segmentationParamDialog;

  // Re-connect realtime image to canvas
  m_ParentMainWindow->GetObjectVisualizer()->GetImageActor()->SetInput(m_ParentMainWindow->GetObjectVisualizer()->GetDataCollector()->GetOutput());

  // Update segmentation parameters
  if (m_PatternRecognition->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to update segmentation parameters!");
    return;
  }

  LOG_INFO("Edit segmentation parameters ended");
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::StartTemporal()
{
  LOG_INFO("Temporal calibration started");

  m_ParentMainWindow->SetTabsEnabled(false);

  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

  // Set validation transform names for tracked frame list
  std::string toolReferenceFrame;
  if ( (m_ParentMainWindow->GetObjectVisualizer()->GetDataCollector() == NULL)
    || (m_ParentMainWindow->GetObjectVisualizer()->GetDataCollector()->GetTrackerToolReferenceFrame(toolReferenceFrame) != PLUS_SUCCESS) )
  {
    LOG_ERROR("Failed to get tool reference frame name!");
    return;
  }
  PlusTransformName transformNameForValidation(m_ParentMainWindow->GetProbeCoordinateFrame(), toolReferenceFrame.c_str());
  m_TemporalCalibrationTrackingData->SetFrameTransformNameForValidation(transformNameForValidation);
  m_TemporalCalibrationVideoData->SetFrameTransformNameForValidation(transformNameForValidation);

  // Set the local timeoffset to 0 before synchronization
  bool offsetsSuccessfullyRetrieved = false;
  if (m_ParentMainWindow->GetObjectVisualizer()->GetDataCollector() != NULL)
  {
    vtkDataCollectorHardwareDevice* dataCollectorHardwareDevice = dynamic_cast<vtkDataCollectorHardwareDevice*>(m_ParentMainWindow->GetObjectVisualizer()->GetDataCollector());
    if (dataCollectorHardwareDevice)
    {
      if ( (dataCollectorHardwareDevice->GetVideoSource() != NULL)
        && (dataCollectorHardwareDevice->GetVideoSource()->GetBuffer() != NULL))
      {
        m_PreviousTrackerOffset = dataCollectorHardwareDevice->GetTracker()->GetToolIteratorBegin()->second->GetBuffer()->GetLocalTimeOffsetSec(); 
        m_PreviousVideoOffset = dataCollectorHardwareDevice->GetVideoSource()->GetBuffer()->GetLocalTimeOffsetSec(); 
        dataCollectorHardwareDevice->SetLocalTimeOffsetSec(0.0, 0.0); 
        offsetsSuccessfullyRetrieved = true;
      }
    }
  }
  if (!offsetsSuccessfullyRetrieved)
  {
    LOG_ERROR("Tracker and video offset retrieval failed due to problems with data collector or the buffers!");
    CancelCalibration();
    return;
  }

  m_TemporalCalibrationTrackingData->Clear();
  m_TemporalCalibrationVideoData->Clear();

  m_LastRecordedFrameTimestamp = 0.0;

  m_StartTimeSec = vtkAccurateTimer::GetSystemTime();
  m_CancelRequest = false;

  m_TemporalCalibrationInProgress = true;
  SetState(ToolboxState_InProgress);

  // Start calibration and compute results on success
  DoTemporalCalibration();
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::DoTemporalCalibration()
{
  //LOG_TRACE("FreehandCalibrationToolbox::DoTemporalCalibration");

  // Get current time
  double currentTimeSec = vtkAccurateTimer::GetSystemTime();

  if (currentTimeSec - m_StartTimeSec >= m_TemporalCalibrationDurationSec)
  {
	  // Create dialog
	  QDialog* temporalCalibrationDialog = new QDialog(this, Qt::Dialog);
	  temporalCalibrationDialog->setMinimumSize(QSize(360,80));
	  temporalCalibrationDialog->setCaption(tr("fCal"));
	  temporalCalibrationDialog->setBackgroundColor(QColor(224, 224, 224));

	  QLabel* temporalCalibrationLabel = new QLabel(QString("Computing temporal calibration, please wait..."), temporalCalibrationDialog);
	  temporalCalibrationLabel->setFont(QFont("SansSerif", 16));

	  QHBoxLayout* layout = new QHBoxLayout();
	  layout->addWidget(temporalCalibrationLabel);

	  temporalCalibrationDialog->setLayout(layout);
	  temporalCalibrationDialog->show();

    ui.frame_TemporalCalibration->setEnabled(false);
    ui.label_InstructionsTemporal->setText(tr("Please wait until computing temporal calibration is finished"));
    m_ParentMainWindow->SetStatusBarText(QString(" Computing temporal calibration"));

	  QApplication::processEvents();

    // Do the calibration
    TemporalCalibration temporalCalibrationObject;
    temporalCalibrationObject.SetTrackerFrames(m_TemporalCalibrationTrackingData);
    temporalCalibrationObject.SetVideoFrames(m_TemporalCalibrationVideoData);
    temporalCalibrationObject.SetSamplingResolutionSec(0.001);
    temporalCalibrationObject.SetSaveIntermediateImagesToOn(false);

    PlusTransformName probeToReferenceTransformName(m_ParentMainWindow->GetProbeCoordinateFrame(), m_ParentMainWindow->GetReferenceCoordinateFrame());
    std::string probeToReferenceTransformNameString;
    probeToReferenceTransformName.GetTransformName(probeToReferenceTransformNameString);
    temporalCalibrationObject.SetProbeToReferenceTransformName(probeToReferenceTransformNameString);

    //  Calculate the time-offset
    if (temporalCalibrationObject.Update() != PLUS_SUCCESS)
    {
      LOG_ERROR("Cannot determine tracker lag, temporal calibration failed!");
      CancelCalibration();

		  temporalCalibrationDialog->done(0);
      temporalCalibrationDialog->hide();
      delete temporalCalibrationDialog;

      return;
    }

    // Get result
    double trackerLagSec = 0;
    if (temporalCalibrationObject.GetTrackerLagSec(trackerLagSec)!=PLUS_SUCCESS)
    {
      LOG_ERROR("Cannot determine tracker lag, temporal calibration failed");
      CancelCalibration();

		  temporalCalibrationDialog->done(0);
      temporalCalibrationDialog->hide();
      delete temporalCalibrationDialog;

      return;
    }

    LOG_INFO("Video offset: " << trackerLagSec << " s ( > 0 if the video data lags )");

    // Set the result local timeoffset
    bool offsetsSuccessfullySet = false;
    if (m_ParentMainWindow->GetObjectVisualizer()->GetDataCollector() != NULL)
    {
      vtkDataCollectorHardwareDevice* dataCollectorHardwareDevice = dynamic_cast<vtkDataCollectorHardwareDevice*>(m_ParentMainWindow->GetObjectVisualizer()->GetDataCollector());
      if (dataCollectorHardwareDevice)
      {
        if ( (dataCollectorHardwareDevice->GetVideoSource() != NULL)
          && (dataCollectorHardwareDevice->GetVideoSource()->GetBuffer() != NULL))
        {
          dataCollectorHardwareDevice->SetLocalTimeOffsetSec(trackerLagSec, 0.0); 
          offsetsSuccessfullySet = true;
        }
      }
    }
    if (!offsetsSuccessfullySet)
    {
      LOG_ERROR("Tracker and video offset setting failed due to problems with data collector or the buffers!");
      CancelCalibration();

      temporalCalibrationDialog->done(0);
      temporalCalibrationDialog->hide();
      delete temporalCalibrationDialog;

      return;
    }
    // Save metric tables
    temporalCalibrationObject.GetVideoPositionSignal(m_VideoPositionMetric);
    temporalCalibrationObject.GetUncalibratedTrackerPositionSignal(m_UncalibratedTrackerPositionMetric);
    temporalCalibrationObject.GetCalibratedTrackerPositionSignal(m_CalibratedTrackerPositionMetric);

    m_TemporalCalibrationTrackingData->Clear();
    m_TemporalCalibrationVideoData->Clear();

    SetState(ToolboxState_Done);

    m_TemporalCalibrationInProgress = false;

    m_ParentMainWindow->SetTabsEnabled(true);

	  // Close dialog
	  temporalCalibrationDialog->done(0);
    temporalCalibrationDialog->hide();
    delete temporalCalibrationDialog;

    return;
  }


  // Cancel if requested
  if (m_CancelRequest)
  {
    LOG_INFO("Calibration process cancelled by the user");
    CancelCalibration();
    return;
  }

  int numberOfTrackingFramesBeforeRecording = m_TemporalCalibrationTrackingData->GetNumberOfTrackedFrames();
  int numberOfVideoFramesBeforeRecording = m_TemporalCalibrationVideoData->GetNumberOfTrackedFrames();

  // Acquire tracking frames
  double lastRecordedTrackingFrameTimestamp = m_LastRecordedFrameTimestamp;
  if ( m_ParentMainWindow->GetObjectVisualizer()->GetDataCollector()->GetTrackedFrameList(lastRecordedTrackingFrameTimestamp, m_TemporalCalibrationTrackingData, -1, false, true) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get tracked frame list from data collector (last recorded timestamp: " << std::fixed << m_LastRecordedFrameTimestamp ); 
    CancelCalibration();
    return; 
  }

  // Acquire video frames
  double lastRecordedVideoFrameTimestamp = m_LastRecordedFrameTimestamp;
  if ( m_ParentMainWindow->GetObjectVisualizer()->GetDataCollector()->GetTrackedFrameList(lastRecordedVideoFrameTimestamp, m_TemporalCalibrationVideoData, -1, true, false) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get tracked frame list from data collector (last recorded timestamp: " << std::fixed << m_LastRecordedFrameTimestamp ); 
    CancelCalibration();
    return; 
  }

  m_LastRecordedFrameTimestamp = std::min( lastRecordedTrackingFrameTimestamp, lastRecordedVideoFrameTimestamp );

  // Update progress
  int progressPercent = (int)((currentTimeSec - m_StartTimeSec) / m_TemporalCalibrationDurationSec * 100.0);
  m_ParentMainWindow->SetStatusBarProgress(progressPercent);

  LOG_DEBUG("Number of tracked frames in the calibration dataset: Tracking: " << std::setw(3) << numberOfTrackingFramesBeforeRecording << " => " << m_TemporalCalibrationTrackingData->GetNumberOfTrackedFrames() << "; Video: " << numberOfVideoFramesBeforeRecording << " => " << m_TemporalCalibrationVideoData->GetNumberOfTrackedFrames());

  QTimer::singleShot(m_RecordingIntervalMs, this, SLOT(DoTemporalCalibration())); 
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::StartSpatial()
{
  LOG_INFO("Spatial calibration started");

  m_ParentMainWindow->SetTabsEnabled(false);

  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

  // Set validation transform names for tracked frame lists
  std::string toolReferenceFrame;
  if ( (m_ParentMainWindow->GetObjectVisualizer()->GetDataCollector() == NULL)
    || (m_ParentMainWindow->GetObjectVisualizer()->GetDataCollector()->GetTrackerToolReferenceFrame(toolReferenceFrame) != PLUS_SUCCESS) )
  {
    LOG_ERROR("Failed to get tool reference frame name!");
    return;
  }
  PlusTransformName transformNameForValidation(m_ParentMainWindow->GetProbeCoordinateFrame(), toolReferenceFrame.c_str());
  m_SpatialCalibrationData->SetFrameTransformNameForValidation(transformNameForValidation);
  m_SpatialValidationData->SetFrameTransformNameForValidation(transformNameForValidation);

  // Initialize algorithms and containers
  if ( (this->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS)
    || (m_PatternRecognition->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS) )
  {
    LOG_ERROR("Reading configuration failed!");
    return;
  }

  m_SpatialCalibrationData->Clear();
  m_SpatialValidationData->Clear();

  m_NumberOfSegmentedCalibrationImages = 0;
  m_NumberOfSegmentedValidationImages = 0;
  m_LastRecordedFrameTimestamp = 0.0;

  m_CancelRequest = false;

  m_SpatialCalibrationInProgress = true;
  SetState(ToolboxState_InProgress);

  // Start calibration and compute results on success
  DoSpatialCalibration();
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::DoSpatialCalibration()
{
  //LOG_TRACE("FreehandCalibrationToolbox::DoSpatialCalibration");

  // Get current time
  double startTimeSec = vtkAccurateTimer::GetSystemTime();

  // Calibrate if acquisition is ready
  if ( m_NumberOfSegmentedCalibrationImages >= m_NumberOfCalibrationImagesToAcquire
    && m_NumberOfSegmentedValidationImages >= m_NumberOfValidationImagesToAcquire)
  {
    LOG_INFO("Segmentation success rate: " << m_NumberOfSegmentedCalibrationImages + m_NumberOfSegmentedValidationImages << " out of " << m_SpatialCalibrationData->GetNumberOfTrackedFrames() + m_SpatialValidationData->GetNumberOfTrackedFrames() << " (" << (int)(((double)(m_NumberOfSegmentedCalibrationImages + m_NumberOfSegmentedValidationImages) / (double)(m_SpatialCalibrationData->GetNumberOfTrackedFrames() + m_SpatialValidationData->GetNumberOfTrackedFrames())) * 100.0 + 0.49) << " percent)");

    if (m_Calibration->Calibrate( m_SpatialValidationData, m_SpatialCalibrationData, m_ParentMainWindow->GetObjectVisualizer()->GetTransformRepository(), m_PatternRecognition->GetFidLineFinder()->GetNWires() ) != PLUS_SUCCESS)
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
    m_SpatialCalibrationInProgress = false;

    m_ParentMainWindow->SetTabsEnabled(true);

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

  int numberOfFramesBeforeRecording = trackedFrameListToUse->GetNumberOfTrackedFrames();

  // Acquire tracked frames since last acquisition (minimum 1 frame)
  if (m_LastProcessingTimePerFrameMs<1)
  {
    // if processing was less than 1ms/frame then assume it was 1ms (1000FPS processing speed) to avoid division by zero
    m_LastProcessingTimePerFrameMs=1;
  }
  int numberOfFramesToGet = std::max(m_MaxTimeSpentWithProcessingMs / m_LastProcessingTimePerFrameMs, 1);

  if ( m_ParentMainWindow->GetObjectVisualizer()->GetDataCollector()->GetTrackedFrameList(
    m_LastRecordedFrameTimestamp, trackedFrameListToUse, numberOfFramesToGet) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get tracked frame list from data collector (last recorded timestamp: " << std::fixed << m_LastRecordedFrameTimestamp ); 
    CancelCalibration();
    return; 
  }

  // Segment last recorded images
  int numberOfNewlySegmentedImages = 0;
  if ( m_PatternRecognition->RecognizePattern(trackedFrameListToUse, &numberOfNewlySegmentedImages) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to segment tracked frame list!"); 
    CancelCalibration();
    return; 
  }

  if (m_NumberOfSegmentedValidationImages < m_NumberOfValidationImagesToAcquire)
  {
    m_NumberOfSegmentedValidationImages += numberOfNewlySegmentedImages;
  }
  else
  {
    m_NumberOfSegmentedCalibrationImages += numberOfNewlySegmentedImages;
  }

  LOG_DEBUG("Number of segmented images in this round: " << numberOfNewlySegmentedImages << " out of " << trackedFrameListToUse->GetNumberOfTrackedFrames() - numberOfFramesBeforeRecording);

  // Update progress if tracked frame has been successfully added
  int progressPercent = (int)(((m_NumberOfSegmentedCalibrationImages + m_NumberOfSegmentedValidationImages) / (double)(std::max(m_NumberOfValidationImagesToAcquire, m_NumberOfSegmentedValidationImages) + m_NumberOfCalibrationImagesToAcquire)) * 100.0);
  m_ParentMainWindow->SetStatusBarProgress(progressPercent);

  // Display segmented points (or hide them if unsuccessful)
  if (numberOfNewlySegmentedImages > 0)
  {
    DisplaySegmentedPoints();
  }
  else
  {
    m_ParentMainWindow->GetObjectVisualizer()->ShowResult(false);
  }

  // Compute time spent with processing one frame in this round
  double computationTimeMs = (vtkAccurateTimer::GetSystemTime() - startTimeSec) * 1000.0;

  // Update last processing time if new tracked frames have been aquired
  if (trackedFrameListToUse->GetNumberOfTrackedFrames() > numberOfFramesBeforeRecording)
  {
    m_LastProcessingTimePerFrameMs = computationTimeMs / (trackedFrameListToUse->GetNumberOfTrackedFrames() - numberOfFramesBeforeRecording);
  }

  // Launch timer to run acquisition again
  int waitTimeMs = std::max((int)(m_RecordingIntervalMs - computationTimeMs), 0);

  if (waitTimeMs == 0)
  {
    LOG_WARNING("Processing cannot keep up with aquisition! Try to decrease MaxTimeSpentWithProcessingMs parameter in device set configuration (it should be more than the processing time (the last one was " << m_LastProcessingTimePerFrameMs << "), so if it is already small, try to increase RecordingIntervalMs too)");
  }

  LOG_DEBUG("Number of requested frames: " << numberOfFramesToGet);
  LOG_DEBUG("Number of tracked frames in the list: " << std::setw(3) << numberOfFramesBeforeRecording << " => " << trackedFrameListToUse->GetNumberOfTrackedFrames());
  LOG_DEBUG("Last processing time: " << m_LastProcessingTimePerFrameMs);
  LOG_DEBUG("Computation time: " << computationTimeMs);
  LOG_DEBUG("Waiting time: " << waitTimeMs);

  QTimer::singleShot(waitTimeMs , this, SLOT(DoSpatialCalibration())); 
}

//-----------------------------------------------------------------------------

PlusStatus FreehandCalibrationToolbox::SetAndSaveResults()
{
  LOG_TRACE("FreehandCalibrationToolbox::SetAndSaveResults");

  // Set transducer origin related transforms
  vtkSmartPointer<vtkTransform> imageToProbeTransform = vtkSmartPointer<vtkTransform>::New();
  imageToProbeTransform->SetMatrix(m_Calibration->GetImageToProbeTransformMatrix());
  double* imageToProbeScale = imageToProbeTransform->GetScale();
  vtkSmartPointer<vtkTransform> transducerOriginPixelToTransducerOriginTransform = vtkSmartPointer<vtkTransform>::New();
  transducerOriginPixelToTransducerOriginTransform->Identity();
  transducerOriginPixelToTransducerOriginTransform->Scale(imageToProbeScale);

  PlusTransformName transducerOriginPixelToTransducerOriginTransformName(m_ParentMainWindow->GetTransducerOriginPixelCoordinateFrame(), m_ParentMainWindow->GetTransducerOriginCoordinateFrame());
  m_ParentMainWindow->GetObjectVisualizer()->GetTransformRepository()->SetTransform(transducerOriginPixelToTransducerOriginTransformName, transducerOriginPixelToTransducerOriginTransform->GetMatrix());
  m_ParentMainWindow->GetObjectVisualizer()->GetTransformRepository()->SetTransformPersistent(transducerOriginPixelToTransducerOriginTransformName, true);
  m_ParentMainWindow->GetObjectVisualizer()->GetTransformRepository()->SetTransformDate(transducerOriginPixelToTransducerOriginTransformName, vtkAccurateTimer::GetInstance()->GetDateAndTimeString().c_str());

  // Set result for visualization
  vtkDisplayableObject* transducerOriginDisplayable = NULL;
  if (m_ParentMainWindow->GetObjectVisualizer()->GetDisplayableObject(m_ParentMainWindow->GetTransducerOriginCoordinateFrame().c_str(), transducerOriginDisplayable) == PLUS_SUCCESS)
  {
    transducerOriginDisplayable->DisplayableOn();
  }
  vtkDisplayableObject* imageDisplayable = NULL;
  if (m_ParentMainWindow->GetObjectVisualizer()->GetDisplayableObject(m_ParentMainWindow->GetImageCoordinateFrame().c_str(), imageDisplayable) == PLUS_SUCCESS)
  {
    imageDisplayable->DisplayableOn();
  }

  // Save result in configuration
  if ( m_ParentMainWindow->GetObjectVisualizer()->GetTransformRepository()->WriteConfiguration( vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData() ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to save freehand calibration result in configuration XML tree!");
    SetState(ToolboxState_Error);
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::CancelCalibration()
{
  LOG_INFO("Calibration cancelled");

  m_CancelRequest = true;

  if (m_TemporalCalibrationInProgress)
  {
    // Reset the local timeoffset to the previous values
    bool offsetsSuccessfullySet = false;
    if (m_ParentMainWindow->GetObjectVisualizer()->GetDataCollector() != NULL)
    {
      vtkDataCollectorHardwareDevice* dataCollectorHardwareDevice = dynamic_cast<vtkDataCollectorHardwareDevice*>(m_ParentMainWindow->GetObjectVisualizer()->GetDataCollector());
      if (dataCollectorHardwareDevice)
      {
        if ( (dataCollectorHardwareDevice->GetVideoSource() != NULL)
          && (dataCollectorHardwareDevice->GetVideoSource()->GetBuffer() != NULL))
        {
          dataCollectorHardwareDevice->SetLocalTimeOffsetSec(m_PreviousTrackerOffset, m_PreviousVideoOffset); 
          offsetsSuccessfullySet = true;
        }
      }
    }
    if (!offsetsSuccessfullySet)
    {
      LOG_ERROR("Tracker and video offset setting failed due to problems with data collector or the buffers!");
    }
  }

  m_SpatialCalibrationInProgress = false;
  m_TemporalCalibrationInProgress = false;

  m_ParentMainWindow->SetTabsEnabled(true);

  SetState(ToolboxState_Idle);
}

//-----------------------------------------------------------------------------

bool FreehandCalibrationToolbox::IsReadyToStartSpatialCalibration()
{
  LOG_TRACE("FreehandCalibrationToolbox::IsReadyToStartSpatialCalibration");

  // Try to load segmentation parameters from the device set configuration (see if it is there and correct)
  FidPatternRecognition* patternRecognition = new FidPatternRecognition();
  if (patternRecognition->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS)
  {
    ui.label_InstructionsSpatial->setText(tr("Pattern recognition configuration needs to be imported"));
    return false;
  }
  delete patternRecognition;

  // Determine if there is already a phantom registration present
  if (m_ParentMainWindow->GetObjectVisualizer()->IsExistingTransform(m_Calibration->GetPhantomCoordinateFrame(), m_Calibration->GetReferenceCoordinateFrame()) != PLUS_SUCCESS)
  {
    ui.label_InstructionsSpatial->setText(tr("Phantom registration needs to be imported"));
    return false;
  }

  // Everything is fine, ready for spatial calibration
  ui.label_InstructionsSpatial->setText(tr("Press Start and start scanning the phantom"));

  return true;
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::DisplaySegmentedPoints()
{
  LOG_TRACE("FreehandCalibrationToolbox::DisplaySegmentedPoints");

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

  // Look for last segmented image and display the points
  for (int i=trackedFrameListToUse->GetNumberOfTrackedFrames() - 1; i>=0; --i)
  {
    vtkPoints* segmentedPoints = trackedFrameListToUse->GetTrackedFrame(i)->GetFiducialPointsCoordinatePx();
    if (segmentedPoints)
    {
      m_ParentMainWindow->GetObjectVisualizer()->GetResultPolyData()->SetPoints(segmentedPoints);
      m_ParentMainWindow->GetObjectVisualizer()->ShowResult(true);
      break;
    }
  }
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::ShowPlotsToggled(bool aOn)
{
  // Delete objects if toggled off, make sure they are deleted when toggled on
  if (m_TemporalCalibrationPlotsWindow != NULL)
  {
    delete m_TemporalCalibrationPlotsWindow;
    m_TemporalCalibrationPlotsWindow = NULL;
  } 

  if (m_UncalibratedPlotContextView != NULL)
  {
    m_UncalibratedPlotContextView->Delete();
    m_UncalibratedPlotContextView = NULL;
  } 

  if (m_CalibratedPlotContextView != NULL)
  {
    m_CalibratedPlotContextView->Delete();
    m_CalibratedPlotContextView = NULL;
  } 

  if (aOn)
  {
	  // Create window and layout
	  m_TemporalCalibrationPlotsWindow = new QWidget(this, Qt::Tool);
	  m_TemporalCalibrationPlotsWindow->setMinimumSize(QSize(800, 600));
	  m_TemporalCalibrationPlotsWindow->setCaption(tr("Temporal calibration report"));
	  m_TemporalCalibrationPlotsWindow->setBackgroundColor(QColor::fromRgb(255, 255, 255));

		// Install event filter that is called on closing the window
		m_TemporalCalibrationPlotsWindow->installEventFilter(this);

	  QGridLayout* gridPlotWindow = new QGridLayout(m_TemporalCalibrationPlotsWindow, 2, 1, 0, 4, "");

    // Uncalibrted chart view
    QVTKWidget* uncalibratedPlotVtkWidget = new QVTKWidget(m_TemporalCalibrationPlotsWindow);

    m_UncalibratedPlotContextView = vtkContextView::New();
    m_UncalibratedPlotContextView->GetRenderer()->SetBackground(1.0, 1.0, 1.0);

    vtkSmartPointer<vtkChartXY> uncalibratedChart = vtkSmartPointer<vtkChartXY>::New();
    uncalibratedChart->Register(m_UncalibratedPlotContextView);

    vtkPlot *uncalibratedTrackerMetricLine = uncalibratedChart->AddPlot(vtkChart::LINE);
    uncalibratedTrackerMetricLine->SetInput(m_UncalibratedTrackerPositionMetric, 0, 1);
    uncalibratedTrackerMetricLine->SetColor(1,0,0);
    uncalibratedTrackerMetricLine->SetWidth(1.0);

    vtkPlot *videoPositionMetricLineU = uncalibratedChart->AddPlot(vtkChart::LINE);
    videoPositionMetricLineU->SetInput(m_VideoPositionMetric, 0, 1);
    videoPositionMetricLineU->SetColor(0,0,1);
    videoPositionMetricLineU->SetWidth(1.0);

    uncalibratedChart->SetShowLegend(true);
    m_UncalibratedPlotContextView->GetScene()->AddItem(uncalibratedChart);

    uncalibratedPlotVtkWidget->GetRenderWindow()->AddRenderer(m_UncalibratedPlotContextView->GetRenderer());
    uncalibratedPlotVtkWidget->GetRenderWindow()->SetSize(800,600);

    gridPlotWindow->addWidget(uncalibratedPlotVtkWidget, 0, 0);

    // Calibrted chart view
    QVTKWidget* calibratedPlotVtkWidget = new QVTKWidget(m_TemporalCalibrationPlotsWindow);

    m_CalibratedPlotContextView = vtkContextView::New();
    m_CalibratedPlotContextView->GetRenderer()->SetBackground(1.0, 1.0, 1.0);

    vtkSmartPointer<vtkChartXY> calibratedChart = vtkSmartPointer<vtkChartXY>::New();
    calibratedChart->Register(m_CalibratedPlotContextView);

    vtkPlot *calibratedTrackerMetricLine = calibratedChart->AddPlot(vtkChart::LINE);
    calibratedTrackerMetricLine->SetInput(m_CalibratedTrackerPositionMetric, 0, 1);
    calibratedTrackerMetricLine->SetColor(0,1,0);
    calibratedTrackerMetricLine->SetWidth(1.0);

    vtkPlot *videoPositionMetricLineC = calibratedChart->AddPlot(vtkChart::LINE);
    videoPositionMetricLineC->SetInput(m_VideoPositionMetric, 0, 1);
    videoPositionMetricLineC->SetColor(0,0,1);
    videoPositionMetricLineC->SetWidth(1.0);

    calibratedChart->SetShowLegend(true);
    m_CalibratedPlotContextView->GetScene()->AddItem(calibratedChart);

    calibratedPlotVtkWidget->GetRenderWindow()->AddRenderer(m_CalibratedPlotContextView->GetRenderer());
    calibratedPlotVtkWidget->GetRenderWindow()->SetSize(800,600);

    gridPlotWindow->addWidget(calibratedPlotVtkWidget, 1, 0);

    // Position and show window
    m_TemporalCalibrationPlotsWindow->setLayout(gridPlotWindow);
    m_TemporalCalibrationPlotsWindow->move( mapToGlobal( QPoint( ui.pushButton_ShowPlots->x() + ui.pushButton_ShowPlots->width(), 20 ) ) );
    m_TemporalCalibrationPlotsWindow->show();
  }
}

//-----------------------------------------------------------------------------

bool FreehandCalibrationToolbox::eventFilter(QObject *obj, QEvent *ev)
{
	if ( obj == m_TemporalCalibrationPlotsWindow )
  {
		if ( ev->type() == QEvent::Close )
    {
			ui.pushButton_ShowPlots->setChecked(false);
		}
    else
    {
			// Pass the event on to the parent class
			return QWidget::eventFilter( obj, ev );
		}
	}

	return true;
}
