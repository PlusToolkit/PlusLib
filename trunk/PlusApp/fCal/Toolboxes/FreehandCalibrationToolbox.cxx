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

//-----------------------------------------------------------------------------

FreehandCalibrationToolbox::FreehandCalibrationToolbox(fCalMainWindow* aParentMainWindow, Qt::WFlags aFlags)
: AbstractToolbox(aParentMainWindow)
, QWidget(aParentMainWindow, aFlags)
, m_CancelRequest(false)
, m_NumberOfCalibrationImagesToAcquire(200)
, m_NumberOfValidationImagesToAcquire(100)
{
  ui.setupUi(this);

  // Create algorithm
  m_Calibration = vtkProbeCalibrationAlgo::New();

  ui.label_SpatialCalibration->setFont(QFont("SansSerif", 9, QFont::Bold));
  ui.label_TemporalCalibration->setFont(QFont("SansSerif", 9, QFont::Bold));

  //ui.label_Results->setFont(QFont("Courier", 7));

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
  FidPatternRecognition* patternRecognition = new FidPatternRecognition();
  if (patternRecognition->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS)
  {
    ui.lineEdit_SegmentationParameters->setText(tr("Invalid file!"));
    ui.lineEdit_SegmentationParameters->setToolTip("");

    LOG_ERROR("Configuration file " << fileName.toAscii().data() << " cannot be loaded!");
    return;
  }
  delete patternRecognition;

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

  // Start calibration and compute results on success
  if (DoSpatialCalibration() == PLUS_SUCCESS)
  {
    // Set result for visualization
    m_ParentMainWindow->GetToolVisualizer()->GetDisplayableTool(TRACKER_TOOL_PROBE)->DisplayableOn();
    m_ParentMainWindow->GetToolVisualizer()->SetImageToProbeTransform(m_Calibration->GetTransformUserImageToProbe());

    SetState(ToolboxState_Done);
  }

  m_ParentMainWindow->SetTabsEnabled(true);
}

//-----------------------------------------------------------------------------

PlusStatus FreehandCalibrationToolbox::DoSpatialCalibration()
{
  LOG_TRACE("FreehandCalibrationToolbox::DoSpatialCalibration");

  // Create tracked frame lists
  vtkTrackedFrameList* validationTrackedFrameList = vtkTrackedFrameList::New();
  validationTrackedFrameList->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData());

  vtkTrackedFrameList* calibrationTrackedFrameList = vtkTrackedFrameList::New();
  calibrationTrackedFrameList->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData());

  int numberOfSegmentedImages = 0;
  int numberOfFailedSegmentations = 0;

  // Create segmenter
  FidPatternRecognition* patternRecognition = new FidPatternRecognition();
  patternRecognition->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData());

  // Reset cancel request flag
  m_CancelRequest = false;

  // Acquire and add validation and calibration data
  while (numberOfSegmentedImages < m_NumberOfValidationImagesToAcquire + m_NumberOfCalibrationImagesToAcquire)
  {
    if (m_CancelRequest) {
      // Cancel the job
      delete patternRecognition;
      return PLUS_FAIL;
    }

    // Get latest tracked frame from data collector
    TrackedFrame trackedFrame; 
    m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetTrackedFrame(&trackedFrame);     

    if (trackedFrame.GetStatus() & (TR_MISSING | TR_OUT_OF_VIEW))
    {
      LOG_DEBUG("Tracker out of view"); 
      m_ParentMainWindow->GetToolVisualizer()->ShowResult(false);
    }
    else if (trackedFrame.GetStatus() & (TR_REQ_TIMEOUT))
    {
      LOG_DEBUG("Tracker request timeout"); 
      m_ParentMainWindow->GetToolVisualizer()->ShowResult(false);
    }
    else // TR_OK
    {
      // Segment image
      PatternRecognitionResult segmentationResults;
      if (patternRecognition->RecognizePattern(&trackedFrame, segmentationResults) != PLUS_SUCCESS)
      {
        LOG_ERROR("Segmentation encountered errors!");
        delete patternRecognition;
        return PLUS_FAIL;
      }

      // Update progress if tracked frame has been successfully added
      int progressPercent = (int)((numberOfSegmentedImages / (double)(m_NumberOfValidationImagesToAcquire + m_NumberOfCalibrationImagesToAcquire)) * 100.0);
      m_ParentMainWindow->SetStatusBarProgress(progressPercent);

      // Display segmented points (or hide them if unsuccessful)
      DisplaySegmentedPoints(&segmentationResults);

      // Check if segmentation was unsuccessful
      if (trackedFrame.GetFiducialPointsCoordinatePx() == NULL || trackedFrame.GetFiducialPointsCoordinatePx()->GetNumberOfPoints() == 0)
      {
        ++numberOfFailedSegmentations;
        QApplication::processEvents();
        continue;
      }

      // Add tracked frame to the proper list
      if (numberOfSegmentedImages < m_NumberOfValidationImagesToAcquire)
      { // Validation data
        validationTrackedFrameList->AddTrackedFrame(&trackedFrame);
      }
      else
      { // Calibration data
        calibrationTrackedFrameList->AddTrackedFrame(&trackedFrame);
      }

      ++numberOfSegmentedImages;
    }

    QApplication::processEvents();
  }

  LOG_INFO("Segmentation success rate: " << numberOfSegmentedImages << " out of " << numberOfSegmentedImages + numberOfFailedSegmentations << " (" << (int)(((double)numberOfSegmentedImages / (double)(numberOfSegmentedImages + numberOfFailedSegmentations)) * 100.0 + 0.49) << " percent)");

  if (m_Calibration->Calibrate( validationTrackedFrameList, calibrationTrackedFrameList, "Probe", patternRecognition->GetFidLineFinder()->GetNWires() ) != PLUS_SUCCESS)
  {
    LOG_ERROR("Calibration failed!");
    delete patternRecognition;
    return PLUS_FAIL;
  }

  delete patternRecognition;

  return PLUS_SUCCESS;
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

PlusStatus FreehandCalibrationToolbox::DisplaySegmentedPoints(PatternRecognitionResult* aSegmentationResult)
{
  LOG_TRACE("vtkFreehandCalibrationController::DisplaySegmentedPoints");

  if (aSegmentationResult->GetDotsFound() == false)
  {
    m_ParentMainWindow->GetToolVisualizer()->ShowResult(false);
    return PLUS_SUCCESS;
  }

  // Get last results and feed the points into vtkPolyData for displaying
  vtkSmartPointer<vtkPoints> segmentedPoints = vtkSmartPointer<vtkPoints>::New();
  segmentedPoints->SetNumberOfPoints(aSegmentationResult->GetFoundDotsCoordinateValue().size());

  std::vector<std::vector<double>> dots = aSegmentationResult->GetFoundDotsCoordinateValue();
  for (int i=0; i<dots.size(); ++i)
  {
    segmentedPoints->InsertPoint(i, dots[i][0], dots[i][1], 0.0);
  }
  segmentedPoints->Modified();

  m_ParentMainWindow->GetToolVisualizer()->GetResultPolyData()->SetPoints(segmentedPoints);

  m_ParentMainWindow->GetToolVisualizer()->ShowResult(true);

  return PLUS_SUCCESS;
}
