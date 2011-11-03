/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "FreehandCalibrationToolbox.h"

#include "vtkProbeCalibrationAlgo.h"
#include "vtkPhantomRegistrationAlgo.h"
#include "vtkPlusVideoSource.h"
#include "vtkVideoBuffer.h"

#include "PhantomRegistrationToolbox.h"
#include "ConfigFileSaverDialog.h"
#include "SegmentationParameterDialog.h"

#include "fCalMainWindow.h"
#include "vtkToolVisualizer.h"

#include "FidPatternRecognition.h"

#include "vtkXMLUtilities.h"

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
  , m_LastProcessingTimePerFrameMs(-1)
{
  ui.setupUi(this);

  // Create algorithms
  m_Calibration = vtkProbeCalibrationAlgo::New();

  m_PatternRecognition = new FidPatternRecognition();

  // Create tracked frame lists
  m_CalibrationData = vtkTrackedFrameList::New();
  m_CalibrationData->SetDefaultFrameTransformName("Probe");
  m_CalibrationData->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP | REQUIRE_TRACKING_OK); 

  m_ValidationData = vtkTrackedFrameList::New();
  m_ValidationData->SetDefaultFrameTransformName("Probe");
  m_ValidationData->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP | REQUIRE_TRACKING_OK); 

  // Change result display properties
  ui.label_Results->setFont(QFont("Courier", 8));

  // Connect events
  connect( ui.pushButton_OpenPhantomRegistration, SIGNAL( clicked() ), this, SLOT( OpenPhantomRegistration() ) );
  connect( ui.pushButton_OpenSegmentationParameters, SIGNAL( clicked() ), this, SLOT( OpenSegmentationParameters() ) );
  connect( ui.pushButton_EditSegmentationParameters, SIGNAL( clicked() ), this, SLOT( EditSegmentationParameters() ) );
  connect( ui.pushButton_StartTemporal, SIGNAL( clicked() ), this, SLOT( StartTemporal() ) );
  connect( ui.pushButton_CancelTemporal, SIGNAL( clicked() ), this, SLOT( CancelTemporal() ) );
  connect( ui.pushButton_StartSpatial, SIGNAL( clicked() ), this, SLOT( StartSpatial() ) );
  connect( ui.pushButton_CancelSpatial, SIGNAL( clicked() ), this, SLOT( CancelSpatial() ) );
  connect( ui.pushButton_Save, SIGNAL( clicked() ), this, SLOT( Save() ) );
  connect( ui.checkBox_ShowDevices, SIGNAL( toggled(bool) ), this, SLOT( ShowDevicesToggled(bool) ) );
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

  if (m_CalibrationData != NULL) {
    m_CalibrationData->Delete();
    m_CalibrationData = NULL;
  } 

  if (m_ValidationData != NULL) {
    m_ValidationData->Delete();
    m_ValidationData = NULL;
  } 
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::Initialize()
{
  LOG_TRACE("FreehandCalibrationToolbox::Initialize"); 

  if ((m_ParentMainWindow->GetToolVisualizer()->GetDataCollector() != NULL) && (m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetConnected()))
  {
    m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->SetTrackingOnly(false);

    // Determine if there is already a phantom registration present
    PhantomRegistrationToolbox* phantomRegistrationToolbox = dynamic_cast<PhantomRegistrationToolbox*>(m_ParentMainWindow->GetToolbox(ToolboxType_PhantomRegistration));
    if ((phantomRegistrationToolbox != NULL) && (phantomRegistrationToolbox->GetPhantomRegistrationAlgo() != NULL)) {

      if (phantomRegistrationToolbox->GetState() == ToolboxState_Done)
      {
        ui.lineEdit_PhantomRegistration->setText(tr("Using session registration data"));
      }
      else if (phantomRegistrationToolbox->GetPhantomRegistrationAlgo()->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) == PLUS_SUCCESS)
      {
        m_ParentMainWindow->GetToolVisualizer()->SetPhantomToPhantomReferenceTransform(phantomRegistrationToolbox->GetPhantomRegistrationAlgo()->GetPhantomToPhantomReferenceTransform());

        ui.lineEdit_PhantomRegistration->setText(tr("Using session registration data"));
      }
    } else {
      LOG_ERROR("Phantom registration toolbox not found!");
      return;
    }

    // Try to load calibration configuration from the device set configuration
    FidPatternRecognition* patternRecognition = new FidPatternRecognition();
    if (patternRecognition->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) == PLUS_SUCCESS)
    {
      ui.lineEdit_SegmentationParameters->setText(tr("Using session segmentation parameters"));
    }
    delete patternRecognition;

    // Load calibration matrix into tool visualizer if it exists
    if ((IsReadyToStartSpatialCalibration()) && (m_Calibration->GetCalibrationDate() != NULL))
    {
      m_ParentMainWindow->GetToolVisualizer()->GetDisplayableTool(TRACKER_TOOL_PROBE)->DisplayableOn();
      m_ParentMainWindow->GetToolVisualizer()->SetImageToProbeTransform(m_Calibration->GetTransformUserImageToProbe());
    }

    // Set initialized if it was uninitialized
    if (m_State == ToolboxState_Uninitialized)
    {
      if (ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS)
      {
        LOG_ERROR("Reading stylus calibration configuraiton failed!");
        return;
      }

      SetState(ToolboxState_Idle);
    }

    if (m_State != ToolboxState_Done) {
      m_ParentMainWindow->GetToolVisualizer()->GetResultPolyData()->Initialize();
    }
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

  // Read number of needed images
  int numberOfCalibrationImagesToAcquire = 0; 
  if ( fCalElement->GetScalarAttribute("NumberOfCalibrationImagesToAcquire", numberOfCalibrationImagesToAcquire ) )
  {
    m_NumberOfCalibrationImagesToAcquire = numberOfCalibrationImagesToAcquire;
  }

  int numberOfValidationImagesToAcquire = 0; 
  if ( fCalElement->GetScalarAttribute("NumberOfValidationImagesToAcquire", numberOfValidationImagesToAcquire ) )
  {
    m_NumberOfValidationImagesToAcquire = numberOfValidationImagesToAcquire;
  }

  // Recording interval and processing time
  int recordingIntervalMs = 0; 
  if ( fCalElement->GetScalarAttribute("RecordingIntervalMs", recordingIntervalMs ) )
  {
    m_RecordingIntervalMs = recordingIntervalMs;
  }

  int maxTimeSpentWithProcessingMs = 0; 
  if ( fCalElement->GetScalarAttribute("MaxTimeSpentWithProcessingMs", maxTimeSpentWithProcessingMs ) )
  {
    m_MaxTimeSpentWithProcessingMs = maxTimeSpentWithProcessingMs;
  }

  // Read probe calibration
  std::string toolType;
	vtkTracker::ConvertToolTypeToString(TRACKER_TOOL_PROBE, toolType);

  vtkXMLDataElement* probeDefinition = vtkPlusConfig::LookupElementWithNameContainingChildWithNameAndAttribute(aConfig, "Tracker", "Tool", "Type", toolType.c_str());
	if (probeDefinition == NULL) {
		LOG_ERROR("No probe definition is found in the XML tree!");
		return PLUS_FAIL;
	}

	vtkXMLDataElement* calibration = probeDefinition->FindNestedElementWithName("Calibration");
	if (calibration == NULL) {
		LOG_ERROR("No calibration section is found in probe definition!");
		return PLUS_FAIL;
	}

  // Read calibration matrix
	double* userImageToProbeTransformVector = new double[16]; 
	if (calibration->GetVectorAttribute("MatrixValue", 16, userImageToProbeTransformVector)) {
    vtkSmartPointer<vtkTransform> userImageToProbeTransform = vtkSmartPointer<vtkTransform>::New();
    userImageToProbeTransform->Identity();
    userImageToProbeTransform->SetMatrix(userImageToProbeTransformVector);
    m_Calibration->SetTransformUserImageToProbe(userImageToProbeTransform);
	}
	delete[] userImageToProbeTransformVector;

  // Read calibration date
  const char* date = calibration->GetAttribute("Date");
  if ((date != NULL) && (STRCASECMP(date, "") != 0)) {
    m_Calibration->SetCalibrationDate(date);
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::Reset()
{
  LOG_TRACE("FreehandCalibrationToolbox::Reset"); 

  // Turn off show devices (this function is called when disconnecting, there is no valid result anymore to show)
  ui.checkBox_ShowDevices->setChecked(false);

  if (m_ParentMainWindow->GetToolVisualizer()->GetDisplayableTool(TRACKER_TOOL_PROBE) != NULL)
  {
    m_ParentMainWindow->GetToolVisualizer()->GetDisplayableTool(TRACKER_TOOL_PROBE)->DisplayableOff();
  }

  SetState(ToolboxState_Idle);
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::RefreshContent()
{
  //LOG_TRACE("StylusCalibrationToolbox::RefreshToolboxContent"); 
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::SetDisplayAccordingToState()
{
  LOG_TRACE("FreehandCalibrationToolbox::SetDisplayAccordingToState");

  m_ParentMainWindow->GetToolVisualizer()->HideAll();

  ShowDevicesToggled(ui.checkBox_ShowDevices->isChecked());

  double videoTimeOffset = 0.0;
  if ((m_ParentMainWindow->GetToolVisualizer()->GetDataCollector() != NULL)
    && (m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetVideoSource() != NULL)
    && (m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetVideoSource()->GetBuffer() != NULL))
  {
    videoTimeOffset = m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetVideoSource()->GetBuffer()->GetLocalTimeOffset();
  }

  if (m_State == ToolboxState_Uninitialized)
  {
    ui.label_InstructionsTemporal->setText(tr(""));
    ui.pushButton_StartTemporal->setEnabled(false);
    ui.pushButton_CancelTemporal->setEnabled(false);

    ui.label_InstructionsSpatial->setText(tr(""));

    ui.checkBox_ShowDevices->setEnabled(false);
    ui.pushButton_Save->setEnabled(false);
    ui.pushButton_EditSegmentationParameters->setEnabled(false);

    m_ParentMainWindow->SetStatusBarText(QString(""));
    m_ParentMainWindow->SetStatusBarProgress(-1);
  }
  else if (m_State == ToolboxState_Idle)
  {
    ui.label_InstructionsTemporal->setText(tr("Current video time offset: %1 ms").arg(videoTimeOffset));
    ui.pushButton_StartTemporal->setEnabled(false); ui.pushButton_StartTemporal->setToolTip(tr("Temporal calibration is disabled until fixing the algorithm, sorry!")); //TODO this is temporarily disabled
    ui.pushButton_CancelTemporal->setEnabled(false);

    if (IsReadyToStartSpatialCalibration()) {
      ui.label_InstructionsSpatial->setText(tr("Press Start and start scanning the phantom"));
    } else {
      ui.label_InstructionsSpatial->setText(tr("Configuration files need to be loaded"));
    }
    ui.pushButton_CancelSpatial->setEnabled(false);

    if ((IsReadyToStartSpatialCalibration()) && (m_Calibration->GetCalibrationDate() != NULL)) {
      ui.checkBox_ShowDevices->setEnabled(true);
    } else {
      ui.checkBox_ShowDevices->setEnabled(false);
    }

    ui.pushButton_EditSegmentationParameters->setEnabled(true);
    ui.pushButton_Save->setEnabled(false);
    ui.label_Results->setText(tr(""));

    ui.pushButton_StartSpatial->setFocus();

    ui.pushButton_StartSpatial->setEnabled(IsReadyToStartSpatialCalibration());

    m_ParentMainWindow->SetStatusBarText(QString(""));
    m_ParentMainWindow->SetStatusBarProgress(-1);

    QApplication::restoreOverrideCursor();
  }
  else if (m_State == ToolboxState_InProgress)
  {
    ui.label_InstructionsTemporal->setText(tr("Current video time offset: %1 ms").arg(videoTimeOffset));
    ui.pushButton_StartTemporal->setEnabled(false);

    ui.label_InstructionsSpatial->setText(tr("Scan the phantom in the most degrees of freedom possible"));
    ui.frame_SpatialCalibration->setEnabled(true);
    ui.pushButton_StartSpatial->setEnabled(false);

    ui.pushButton_EditSegmentationParameters->setEnabled(false);
    ui.checkBox_ShowDevices->setEnabled(false);
    ui.pushButton_Save->setEnabled(false);
    ui.label_Results->setText(tr(""));

    m_ParentMainWindow->SetStatusBarText(QString(" Acquiring and adding images to calibrator"));
    m_ParentMainWindow->SetStatusBarProgress(0);
  }
  else if (m_State == ToolboxState_Done)
  {
    ui.label_InstructionsTemporal->setText(tr("Temporal calibration is ready to save\n(video time offset: %1 ms)").arg(videoTimeOffset));
    ui.pushButton_StartTemporal->setEnabled(true);
    ui.pushButton_CancelSpatial->setEnabled(false);

    ui.label_InstructionsSpatial->setText(tr("Spatial calibration is ready to save"));
    ui.pushButton_StartSpatial->setEnabled(true);
    ui.pushButton_CancelSpatial->setEnabled(false);

    ui.pushButton_EditSegmentationParameters->setEnabled(true);
    ui.checkBox_ShowDevices->setEnabled(true);
    ui.pushButton_Save->setEnabled(true);
    ui.label_Results->setText(m_Calibration->GetResultString().c_str());

    ui.pushButton_Save->setFocus();

    m_ParentMainWindow->SetStatusBarText(QString(" Calibration done"));
    m_ParentMainWindow->SetStatusBarProgress(-1);

    QApplication::restoreOverrideCursor();
  }
  else if (m_State == ToolboxState_Error)
  {
    ui.label_InstructionsTemporal->setText(tr("Error occured!"));
    ui.label_InstructionsTemporal->setFont(QFont("SansSerif", 8, QFont::Bold));
    ui.pushButton_StartTemporal->setEnabled(false);
    ui.pushButton_CancelSpatial->setEnabled(false);

    ui.label_InstructionsSpatial->setText(tr(""));
    ui.pushButton_StartSpatial->setEnabled(false);
    ui.pushButton_CancelSpatial->setEnabled(false);

    ui.checkBox_ShowDevices->setEnabled(false);
    ui.pushButton_Save->setEnabled(false);

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
  if (fileName.isNull()) {
    return;
  }

  // Parse XML file
  vtkSmartPointer<vtkXMLDataElement> rootElement = vtkSmartPointer<vtkXMLDataElement>::Take(
    vtkXMLUtilities::ReadElementFromFile(fileName.toAscii().data()));
  if (rootElement == NULL) {	
    LOG_ERROR("Unable to read the configuration file: " << fileName.toAscii().data()); 
    return;
  }

  // Load phantom registration
  PhantomRegistrationToolbox* phantomRegistrationToolbox = dynamic_cast<PhantomRegistrationToolbox*>(m_ParentMainWindow->GetToolbox(ToolboxType_PhantomRegistration));
  if ((phantomRegistrationToolbox == NULL) || (phantomRegistrationToolbox->GetPhantomRegistrationAlgo() == NULL)) {
    LOG_ERROR("Phantom registration toolbox not found!");
    return;
  }

  if (phantomRegistrationToolbox->GetPhantomRegistrationAlgo()->ReadConfiguration(rootElement) != PLUS_SUCCESS) {
    ui.lineEdit_PhantomRegistration->setText(tr("Invalid file!"));
    ui.lineEdit_PhantomRegistration->setToolTip("");
  }

  // Replace PhantomDefinition element with the one in the just read file
  vtkPlusConfig::ReplaceElementInDeviceSetConfiguration("PhantomDefinition", rootElement);

  // Set file name in text field
  ui.lineEdit_PhantomRegistration->setText(fileName);
  ui.lineEdit_PhantomRegistration->setToolTip(fileName);

  SetDisplayAccordingToState();
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
  vtkSmartPointer<vtkXMLDataElement> rootElement = vtkSmartPointer<vtkXMLDataElement>::Take(
    vtkXMLUtilities::ReadElementFromFile(fileName.toAscii().data()));
  if (rootElement == NULL)
  {
    LOG_ERROR("Unable to read the configuration file: " << fileName.toAscii().data()); 
    return;
  }

  // Load calibration configuration xml
  if (m_PatternRecognition->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS)
  {
    ui.lineEdit_SegmentationParameters->setText(tr("Invalid file!"));
    ui.lineEdit_SegmentationParameters->setToolTip("");

    LOG_ERROR("Configuration file " << fileName.toAscii().data() << " cannot be loaded!");
    return;
  }

  // Replace USCalibration element with the one in the just read file
  vtkPlusConfig::ReplaceElementInDeviceSetConfiguration("USCalibration", rootElement);

  // Re-calculate camera parameters
  m_ParentMainWindow->GetToolVisualizer()->CalculateImageCameraParameters();

  ui.lineEdit_SegmentationParameters->setText(fileName);
  ui.lineEdit_SegmentationParameters->setToolTip(fileName);

  SetDisplayAccordingToState();
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::EditSegmentationParameters()
{
  LOG_TRACE("FreehandCalibrationToolbox::EditSegmentationParameters");

  // Disconnect realtime image from main canvas
  m_ParentMainWindow->GetToolVisualizer()->GetImageActor()->SetInput(NULL);

  // Show segmentation parameter dialog
  SegmentationParameterDialog* segmentationParamDialog = new SegmentationParameterDialog(this, m_ParentMainWindow->GetToolVisualizer()->GetDataCollector());
  segmentationParamDialog->exec();

  delete segmentationParamDialog;

  // Re-connect realtime image to canvas
  m_ParentMainWindow->GetToolVisualizer()->GetImageActor()->SetInput(m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetOutput());
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::StartTemporal()
{
  LOG_TRACE("FreehandCalibrationToolbox::StartTemporal"); 

  m_ParentMainWindow->SetTabsEnabled(false);

  SetState(ToolboxState_InProgress);

  ui.pushButton_CancelTemporal->setEnabled(true);
  ui.pushButton_CancelTemporal->setFocus();

  QApplication::processEvents();

  // Do the calibration
  //m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->SetProgressBarUpdateCallbackFunction(UpdateProgress);
  m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->Synchronize(vtkPlusConfig::GetInstance()->GetOutputDirectory(), true );

  //this->ProgressPercent = 0;

  m_ParentMainWindow->SetTabsEnabled(true);

  SetState(ToolboxState_Idle);
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::CancelTemporal()
{
  LOG_TRACE("FreehandCalibrationToolbox::CancelTemporal"); 

  // Cancel synchronization (temporal calibration) in data collector
  m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->CancelSyncRequestOn();

  m_ParentMainWindow->SetTabsEnabled(true);

  SetState(ToolboxState_Idle);
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::StartSpatial()
{
  LOG_TRACE("FreehandCalibrationToolbox::StartSpatial"); 

  m_ParentMainWindow->SetTabsEnabled(false);

  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

  PhantomRegistrationToolbox* phantomRegistrationToolbox = dynamic_cast<PhantomRegistrationToolbox*>(m_ParentMainWindow->GetToolbox(ToolboxType_PhantomRegistration));
  if ((phantomRegistrationToolbox != NULL) && (phantomRegistrationToolbox->GetPhantomRegistrationAlgo() != NULL))
  {
    m_Calibration->Initialize();
    m_Calibration->SetPhantomToReferenceTransform(phantomRegistrationToolbox->GetPhantomRegistrationAlgo()->GetPhantomToPhantomReferenceTransform());
  }
  else
  {
    LOG_ERROR("Phantom registration toolbox or algorithm is not initialized!");
    return;
  }

  SetState(ToolboxState_InProgress);

  ui.pushButton_CancelSpatial->setEnabled(true);
  ui.pushButton_CancelSpatial->setFocus();

  // Initialize algorithms and containers
  m_PatternRecognition->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData());

  m_CalibrationData->Clear();
  m_ValidationData->Clear();

  m_NumberOfSegmentedCalibrationImages = 0;
  m_NumberOfSegmentedValidationImages = 0;

  m_LastRecordedFrameTimestamp = 0.0;

  m_CancelRequest = false;

  // Start calibration and compute results on success
  DoSpatialCalibration();
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::DoSpatialCalibration()
{
  LOG_TRACE("FreehandCalibrationToolbox::DoSpatialCalibration");

  // Get current time
  double startTimeSec = vtkAccurateTimer::GetSystemTime();

  // Calibrate if acquisition is ready
  if ( m_NumberOfSegmentedCalibrationImages >= m_NumberOfCalibrationImagesToAcquire
    && m_NumberOfSegmentedValidationImages >= m_NumberOfValidationImagesToAcquire)
  {
    LOG_INFO("Segmentation success rate: " << m_NumberOfSegmentedCalibrationImages + m_NumberOfSegmentedValidationImages << " out of " << m_CalibrationData->GetNumberOfTrackedFrames() + m_ValidationData->GetNumberOfTrackedFrames() << " (" << (int)(((double)(m_NumberOfSegmentedCalibrationImages + m_NumberOfSegmentedValidationImages) / (double)(m_CalibrationData->GetNumberOfTrackedFrames() + m_ValidationData->GetNumberOfTrackedFrames())) * 100.0 + 0.49) << " percent)");

    if (m_Calibration->Calibrate( m_ValidationData, m_CalibrationData, "Probe", m_PatternRecognition->GetFidLineFinder()->GetNWires() ) != PLUS_SUCCESS)
    {
      LOG_ERROR("Calibration failed!");
      CancelSpatial();
      return;
    }

    // Set result for visualization
    m_ParentMainWindow->GetToolVisualizer()->GetDisplayableTool(TRACKER_TOOL_PROBE)->DisplayableOn();
    m_ParentMainWindow->GetToolVisualizer()->SetImageToProbeTransform(m_Calibration->GetTransformUserImageToProbe());

    SetState(ToolboxState_Done);

    m_ParentMainWindow->SetTabsEnabled(true);

    return;
  }


  // Cancel if requested
  if (m_CancelRequest)
  {
    LOG_INFO("Calibration process cancelled by the user");
    CancelSpatial();
    return;
  }

  // Determine which data container to use
  vtkTrackedFrameList* trackedFrameListToUse = NULL;
  if (m_NumberOfSegmentedValidationImages < m_NumberOfValidationImagesToAcquire)
  {
    trackedFrameListToUse = m_ValidationData;
  }
  else
  {
    trackedFrameListToUse = m_CalibrationData;
  }

  int numberOfFramesBeforeRecording = trackedFrameListToUse->GetNumberOfTrackedFrames();

  // Acquire tracked frames since last acquisition (minimum 1 frame)
  int numberOfFramesToGet = std::max(m_MaxTimeSpentWithProcessingMs / m_LastProcessingTimePerFrameMs, 1);

  if ( m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetTrackedFrameList(
    m_LastRecordedFrameTimestamp, trackedFrameListToUse, numberOfFramesToGet) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get tracked frame list from data collector (last recorded timestamp: " << std::fixed << m_LastRecordedFrameTimestamp ); 
    CancelSpatial();
    return; 
  }

  // Segment last recorded images
  int numberOfNewlySegmentedImages = 0;
  if ( m_PatternRecognition->RecognizePattern(trackedFrameListToUse, &numberOfNewlySegmentedImages) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to segment tracked frame list!"); 
    CancelSpatial();
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

  LOG_DEBUG("Number of segmented images in this recording round: " << numberOfNewlySegmentedImages << " out of " << trackedFrameListToUse->GetNumberOfTrackedFrames() - numberOfFramesBeforeRecording);

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
    m_ParentMainWindow->GetToolVisualizer()->ShowResult(false);
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
    LOG_WARNING("Processing cannot keep up with aquisition! Try to decrease MaxTimeSpentWithProcessingMs parameter in device set configuration");
  }

  LOG_DEBUG("Number of requested frames: " << std::setw(3) << numberOfFramesToGet << ", number of tracked frames in the list: " << numberOfFramesBeforeRecording << " => " << trackedFrameListToUse->GetNumberOfTrackedFrames());
  LOG_DEBUG("Computation time: " << std::setw(4) << computationTimeMs << ", Waiting time: " << waitTimeMs);
  LOG_DEBUG("Last processing time: " << m_LastProcessingTimePerFrameMs);

  QTimer::singleShot(waitTimeMs , this, SLOT(DoSpatialCalibration())); 
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::CancelSpatial()
{
  LOG_TRACE("FreehandCalibrationToolbox::CancelSpatial"); 

  // Turn off device visualization if it was on
  if (ui.checkBox_ShowDevices->isChecked() == true) {
    ui.checkBox_ShowDevices->setChecked(false);
  }

  m_CancelRequest = true;

  m_ParentMainWindow->SetTabsEnabled(true);

  SetState(ToolboxState_Idle);
}

//-----------------------------------------------------------------------------

bool FreehandCalibrationToolbox::IsReadyToStartSpatialCalibration()
{
  LOG_TRACE("FreehandCalibrationToolbox::IsReadyToStartSpatialCalibration");

  PhantomRegistrationToolbox* phantomRegistrationToolbox = dynamic_cast<PhantomRegistrationToolbox*>(m_ParentMainWindow->GetToolbox(ToolboxType_PhantomRegistration));

  if ( (phantomRegistrationToolbox == NULL)
    || (phantomRegistrationToolbox->GetPhantomRegistrationAlgo() == NULL)
    || (phantomRegistrationToolbox->GetPhantomRegistrationAlgo()->GetPhantomToPhantomReferenceTransform() == NULL))
  {
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::Save()
{
  LOG_TRACE("FreehandCalibrationToolbox::Save"); 

  ConfigFileSaverDialog* configSaverDialog = new ConfigFileSaverDialog(this);
  configSaverDialog->exec();

  delete configSaverDialog;
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::ShowDevicesToggled(bool aOn)
{
  LOG_TRACE("FreehandCalibrationToolbox::ShowDevicesToggled(" << (aOn?"true":"false") << ")"); 

  m_ParentMainWindow->GetToolVisualizer()->ShowTool(TRACKER_TOOL_PROBE, aOn);
  m_ParentMainWindow->GetToolVisualizer()->ShowTool(TRACKER_TOOL_REFERENCE, aOn);
  m_ParentMainWindow->GetToolVisualizer()->ShowTool(TRACKER_TOOL_STYLUS, aOn);
  m_ParentMainWindow->GetToolVisualizer()->ShowTool(TRACKER_TOOL_NEEDLE, aOn);

  m_ParentMainWindow->GetToolVisualizer()->EnableImageMode(!aOn);
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::DisplaySegmentedPoints()
{
  LOG_TRACE("FreehandCalibrationToolbox::DisplaySegmentedPoints");

  // Determine which data container to use
  vtkTrackedFrameList* trackedFrameListToUse = NULL;
  if (m_NumberOfSegmentedValidationImages < m_NumberOfValidationImagesToAcquire)
  {
    trackedFrameListToUse = m_ValidationData;
  }
  else
  {
    trackedFrameListToUse = m_CalibrationData;
  }

  // Look for last segmented image and display the points
  for (int i=trackedFrameListToUse->GetNumberOfTrackedFrames() - 1; i>=0; --i)
  {
    vtkPoints* segmentedPoints = trackedFrameListToUse->GetTrackedFrame(i)->GetFiducialPointsCoordinatePx();
    if (segmentedPoints)
    {
      m_ParentMainWindow->GetToolVisualizer()->GetResultPolyData()->SetPoints(segmentedPoints);
      m_ParentMainWindow->GetToolVisualizer()->ShowResult(true);
      break;
    }
  }
}
