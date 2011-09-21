#include "FreehandCalibrationToolbox.h"

#include "vtkFreehandCalibrationController.h"
#include "vtkPhantomRegistrationAlgo.h"

#include "PhantomRegistrationToolbox.h"
#include "ConfigFileSaverDialog.h"
#include "SegmentationParameterDialog.h"

#include "fCalMainWindow.h"
#include "vtkToolVisualizer.h"

#include <QFileDialog>

//-----------------------------------------------------------------------------

FreehandCalibrationToolbox::FreehandCalibrationToolbox(fCalMainWindow* aParentMainWindow, Qt::WFlags aFlags)
	: AbstractToolbox(aParentMainWindow)
	, QWidget(aParentMainWindow, aFlags)
{
	ui.setupUi(this);

	//TODO tooltips

	m_TemporalCalibrationDone = false;
	m_SpatialCalibrationDone = false;

	// Create algorithm
  m_FreehandCalibration = vtkFreehandCalibrationController::New();

	ui.label_SpatialCalibration->setFont(QFont("SansSerif", 9, QFont::Bold));
	ui.label_TemporalCalibration->setFont(QFont("SansSerif", 9, QFont::Bold));

	//ui.label_Results->setFont(QFont("Courier", 7));

	// Connect events
	connect( ui.pushButton_OpenPhantomRegistration, SIGNAL( clicked() ), this, SLOT( OpenPhantomRegistration() ) );
	connect( ui.pushButton_OpenCalibrationConfiguration, SIGNAL( clicked() ), this, SLOT( OpenCalibrationConfiguration() ) );
	connect( ui.pushButton_EditCalibrationConfiguration, SIGNAL( clicked() ), this, SLOT( EditCalibrationConfiguration() ) );
	connect( ui.pushButton_StartTemporal, SIGNAL( clicked() ), this, SLOT( StartTemporal() ) );
	connect( ui.pushButton_ResetTemporal, SIGNAL( clicked() ), this, SLOT( ResetTemporal() ) );
	connect( ui.pushButton_SkipTemporal, SIGNAL( clicked() ), this, SLOT( SkipTemporal() ) );
	connect( ui.pushButton_StartSpatial, SIGNAL( clicked() ), this, SLOT( StartSpatial() ) );
	connect( ui.pushButton_ResetSpatial, SIGNAL( clicked() ), this, SLOT( ResetSpatial() ) );
	connect( ui.pushButton_Save, SIGNAL( clicked() ), this, SLOT( Save() ) );
	connect( ui.checkBox_ShowDevices, SIGNAL( toggled(bool) ), this, SLOT( ShowDevicesToggled(bool) ) );
}

//-----------------------------------------------------------------------------

FreehandCalibrationToolbox::~FreehandCalibrationToolbox()
{
	if (m_FreehandCalibration != NULL) {
		m_FreehandCalibration->Delete();
		m_FreehandCalibration = NULL;
	} 
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::Initialize()
{
	LOG_TRACE("FreehandCalibrationToolbox::Initialize"); 

  if ((m_ParentMainWindow->GetToolVisualizer()->GetDataCollector() != NULL) && (m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetConnected())) {

    // Determine if there is already a phantom registration present
    PhantomRegistrationToolbox* phantomRegistrationToolbox = dynamic_cast<PhantomRegistrationToolbox*>(m_ParentMainWindow->GetToolbox(ToolboxType_PhantomRegistration));
    if ((phantomRegistrationToolbox != NULL) && (phantomRegistrationToolbox->GetPhantomRegistrationAlgo() != NULL)) {

      if (phantomRegistrationToolbox->GetState() == ToolboxState_Done) {
		    ui.lineEdit_PhantomRegistration->setText(tr("Using session registration data"));

      } else if (phantomRegistrationToolbox->GetPhantomRegistrationAlgo()->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) == PLUS_SUCCESS) {
        m_ParentMainWindow->GetToolVisualizer()->SetPhantomToPhantomReferenceTransform(phantomRegistrationToolbox->GetPhantomRegistrationAlgo()->GetPhantomToPhantomReferenceTransform());

		    ui.lineEdit_PhantomRegistration->setText(tr("Using session registration data"));
      }
    } else {
      LOG_ERROR("Phantom registration toolbox not found!");
      return;
    }

    // Try to load calibration configuration from the device set configuration
    if (m_FreehandCalibration->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) == PLUS_SUCCESS) {
      ui.lineEdit_CalibrationConfiguration->setText(tr("Using session calibration configuration"));
    }

	  if (m_State == ToolboxState_Uninitialized) {
		  SetState(ToolboxState_Idle);
      m_FreehandCalibration->InitializedOn();
	  }

	  if (m_State != ToolboxState_Done) {
      m_ParentMainWindow->GetToolVisualizer()->GetResultPointsPolyData()->GetPoints()->Reset();
    }

  } else {
	  SetState(ToolboxState_Error);
    LOG_ERROR("Frehand calibration cannot be initialized because data collection is not started!");
  }
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::RefreshContent()
{
	//LOG_TRACE("StylusCalibrationToolbox::RefreshToolboxContent"); 

	// If in progress
	if (m_State == ToolboxState_InProgress) {
    m_ParentMainWindow->SetStatusBarProgress(m_FreehandCalibration->GetProgressPercent());

		// Needed for forced refreshing the UI (without this, no progress is shown)
		QApplication::processEvents();
  }
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::SetDisplayAccordingToState()
{
  LOG_TRACE("FreehandCalibrationToolbox::SetDisplayAccordingToState");

  m_ParentMainWindow->GetToolVisualizer()->HideAll();
  m_ParentMainWindow->GetToolVisualizer()->EnableImageMode(! ui.checkBox_ShowDevices->isChecked());

  m_FreehandCalibration->SetDataCollector(m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()); // TODO: TEMPORARY UNTIL TRACKED FRAME ACQUISITION GOES INTO APPLICATIONS

  double videoTimeOffset = 0.0;
	if ((m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetVideoSource() != NULL) && (m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetVideoSource()->GetBuffer() != NULL)) {
    videoTimeOffset = m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetVideoSource()->GetBuffer()->GetLocalTimeOffset();
  }

	// If initialization failed
	if (m_State == ToolboxState_Uninitialized) {
		ui.label_InstructionsTemporal->setText(tr(""));
		ui.pushButton_StartTemporal->setEnabled(false);
		ui.pushButton_ResetTemporal->setEnabled(false);
		ui.pushButton_SkipTemporal->setEnabled(false);

		ui.label_InstructionsSpatial->setText(tr(""));
		ui.frame_SpatialCalibration->setEnabled(false);

		ui.checkBox_ShowDevices->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);

		m_ParentMainWindow->SetStatusBarText(QString(""));
		m_ParentMainWindow->SetStatusBarProgress(-1);

	} else
	// If initialized
	if (m_State == ToolboxState_Idle) {
		if (m_TemporalCalibrationDone == false) { // If temporal calibration has not been done yet
			ui.label_InstructionsTemporal->setText(tr("Press Start and to perform temporal calibration or Skip\n\nCurrent video time offset: %1 ms").arg(videoTimeOffset));

			ui.pushButton_StartTemporal->setEnabled(true);
			ui.pushButton_ResetTemporal->setEnabled(false);
			ui.pushButton_SkipTemporal->setEnabled(true);

			ui.label_InstructionsSpatial->setText(tr(""));
			ui.frame_SpatialCalibration->setEnabled(false);

			ui.checkBox_ShowDevices->setEnabled(false);
			ui.pushButton_Save->setEnabled(false);
			ui.label_Results->setText(tr(""));

			ui.pushButton_StartTemporal->setFocus();

    } else { // If temporal calibration is finished
			ui.label_InstructionsTemporal->setText(tr("Current video time offset: %1 ms").arg(videoTimeOffset));
			ui.pushButton_StartTemporal->setEnabled(false);
			ui.pushButton_ResetTemporal->setEnabled(true);
			ui.pushButton_SkipTemporal->setEnabled(false);

      if (IsReadyToStartSpatialCalibration()) {
			  ui.label_InstructionsSpatial->setText(tr("Press Start and start scanning the phantom"));
      } else {
			  ui.label_InstructionsSpatial->setText(tr("Configuration files need to be loaded"));
      }
			ui.frame_SpatialCalibration->setEnabled(true);
			ui.pushButton_ResetSpatial->setEnabled(false);

			ui.checkBox_ShowDevices->setEnabled(false);
			ui.pushButton_Save->setEnabled(false);
			ui.label_Results->setText(tr(""));

			ui.pushButton_StartSpatial->setFocus();
		}

		ui.pushButton_StartSpatial->setEnabled(IsReadyToStartSpatialCalibration());

		m_ParentMainWindow->SetStatusBarText(QString(""));
		m_ParentMainWindow->SetStatusBarProgress(-1);

    QApplication::restoreOverrideCursor();

	} else
	// If in progress
	if (m_State == ToolboxState_InProgress) {
		if (m_TemporalCalibrationDone == false) { // If temporal calibration has not been done yet
			ui.label_InstructionsTemporal->setText(tr("Make abrupt movements with the probe every 2 seconds"));
			ui.pushButton_StartTemporal->setEnabled(false);
			ui.pushButton_ResetTemporal->setEnabled(true);
		  ui.pushButton_SkipTemporal->setEnabled(false);

			ui.label_InstructionsSpatial->setText(tr(""));
			ui.frame_SpatialCalibration->setEnabled(false);

			ui.checkBox_ShowDevices->setEnabled(false);
			ui.pushButton_Save->setEnabled(false);
			ui.label_Results->setText(tr(""));

			ui.pushButton_ResetTemporal->setFocus();

		} else { // If temporal calibration is finished
			ui.label_InstructionsTemporal->setText(tr("Temporal calibration is ready to save\n(video time offset: %1 ms)").arg(videoTimeOffset));
			ui.pushButton_StartTemporal->setEnabled(false);
			ui.pushButton_ResetTemporal->setEnabled(false);
		  ui.pushButton_SkipTemporal->setEnabled(false);

			ui.label_InstructionsSpatial->setText(tr("Scan the phantom in the most degrees of freedom possible"));
			ui.frame_SpatialCalibration->setEnabled(true);
			ui.pushButton_StartSpatial->setEnabled(false);
			ui.pushButton_ResetSpatial->setEnabled(true);

			ui.checkBox_ShowDevices->setEnabled(false);
			ui.pushButton_Save->setEnabled(false);
			ui.label_Results->setText(tr(""));

			ui.pushButton_ResetSpatial->setFocus();
		}

		m_ParentMainWindow->SetStatusBarText(QString(" Acquiring and adding images to calibrator"));
    m_ParentMainWindow->SetStatusBarProgress(m_FreehandCalibration->GetProgressPercent());

  } else
	// If done
	if (m_State == ToolboxState_Done) {
		ui.label_InstructionsTemporal->setText(tr("Temporal calibration is ready to save\n(video time offset: %1 ms)").arg(videoTimeOffset));
		ui.pushButton_StartTemporal->setEnabled(false);
		ui.pushButton_ResetTemporal->setEnabled(false);
		ui.pushButton_SkipTemporal->setEnabled(false);

		ui.label_InstructionsSpatial->setText(tr("Spatial calibration is ready to save"));
		ui.frame_SpatialCalibration->setEnabled(true);
		ui.pushButton_StartSpatial->setEnabled(false);
		ui.pushButton_ResetSpatial->setEnabled(true);

		ui.checkBox_ShowDevices->setEnabled(true);
		ui.pushButton_Save->setEnabled(true);
		ui.label_Results->setText(QString::fromStdString(m_FreehandCalibration->GetResultString()));

		ui.pushButton_Save->setFocus();

    m_ParentMainWindow->GetToolVisualizer()->ShowTool(TRACKER_TOOL_PROBE, true);
    m_ParentMainWindow->GetToolVisualizer()->ShowTool(TRACKER_TOOL_REFERENCE, true);
    m_ParentMainWindow->GetToolVisualizer()->ShowTool(TRACKER_TOOL_STYLUS, true);
    m_ParentMainWindow->GetToolVisualizer()->ShowTool(TRACKER_TOOL_NEEDLE, true);

		m_ParentMainWindow->SetStatusBarText(QString(" Calibration done"));
		m_ParentMainWindow->SetStatusBarProgress(-1);

    QApplication::restoreOverrideCursor();

	} else
	// If error occured
	if (m_State == ToolboxState_Error) {
		ui.label_InstructionsTemporal->setText(tr("Error occured!"));
		ui.label_InstructionsTemporal->setFont(QFont("SansSerif", 8, QFont::Bold));
		ui.pushButton_StartTemporal->setEnabled(false);
		ui.pushButton_ResetTemporal->setEnabled(false);
		ui.pushButton_SkipTemporal->setEnabled(false);

		ui.label_InstructionsSpatial->setText(tr(""));
		ui.frame_SpatialCalibration->setEnabled(false);
		ui.pushButton_StartSpatial->setEnabled(false);
		ui.pushButton_ResetSpatial->setEnabled(false);

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
  vtkSmartPointer<vtkXMLDataElement> rootElement = vtkXMLUtilities::ReadElementFromFile(fileName.toStdString().c_str());
	if (rootElement == NULL) {	
		LOG_ERROR("Unable to read the configuration file: " << fileName.toStdString()); 
		return;
	}

	// Load phantom registration
  PhantomRegistrationToolbox* phantomRegistrationToolbox = dynamic_cast<PhantomRegistrationToolbox*>(m_ParentMainWindow->GetToolbox(ToolboxType_PhantomRegistration));
  if ((phantomRegistrationToolbox != NULL) && (phantomRegistrationToolbox->GetPhantomRegistrationAlgo() != NULL)) {

    if (phantomRegistrationToolbox->GetPhantomRegistrationAlgo()->ReadConfiguration(rootElement) == PLUS_SUCCESS) {
		  ui.lineEdit_PhantomRegistration->setText(fileName);
		  ui.lineEdit_PhantomRegistration->setToolTip(fileName);
	  } else {
		  ui.lineEdit_PhantomRegistration->setText(tr("Invalid file!"));
		  ui.lineEdit_PhantomRegistration->setToolTip("");
	  }
  }
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::OpenCalibrationConfiguration()
{
  LOG_TRACE("FreehandCalibrationToolbox::OpenCalibrationConfigurationClicked"); 

	// File open dialog for selecting calibration configuration xml
	QString filter = QString( tr( "XML files ( *.xml );;" ) );
	QString fileName = QFileDialog::getOpenFileName(NULL, QString( tr( "Open calibration configuration XML" ) ), vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory(), filter);
	if (fileName.isNull()) {
		return;
	}

  // Parse XML file
  vtkSmartPointer<vtkXMLDataElement> rootElement = vtkXMLUtilities::ReadElementFromFile(fileName.toStdString().c_str());
	if (rootElement == NULL) {	
		LOG_ERROR("Unable to read the configuration file: " << fileName.toStdString()); 
		return;
	}

	// Load calibration configuration xml
	if (m_FreehandCalibration->ReadConfiguration(rootElement) != PLUS_SUCCESS) {
		ui.lineEdit_CalibrationConfiguration->setText(tr("Invalid file!"));
		ui.lineEdit_CalibrationConfiguration->setToolTip("");

		LOG_ERROR("Calibration configuration file " << fileName.toStdString().c_str() << " cannot be loaded!");
		return;
	}

	// Re-calculate camera parameters
  m_ParentMainWindow->GetToolVisualizer()->CalculateImageCameraParameters();

	ui.lineEdit_CalibrationConfiguration->setText(fileName);
	ui.lineEdit_CalibrationConfiguration->setToolTip(fileName);
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::EditCalibrationConfiguration()
{
  LOG_TRACE("FreehandCalibrationToolbox::EditCalibrationConfigurationClicked");

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
	LOG_TRACE("FreehandCalibrationToolbox::StartTemporalClicked"); 

	m_ParentMainWindow->SetTabsEnabled(false);

  SetState(ToolboxState_InProgress);

	m_FreehandCalibration->DoTemporalCalibration();

	m_TemporalCalibrationDone = true;

	m_ParentMainWindow->SetTabsEnabled(true);

  SetState(ToolboxState_Idle);

  SetDisplayAccordingToState();
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::ResetTemporal()
{
	LOG_TRACE("FreehandCalibrationToolbox::ResetTemporalClicked"); 

	// Cancel synchronization (temporal calibration) in data collector
  m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->CancelSyncRequestOn();

	m_ParentMainWindow->SetTabsEnabled(true);

  SetState(ToolboxState_Idle);

	m_TemporalCalibrationDone = false;

  SetDisplayAccordingToState();
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::SkipTemporal()
{
	LOG_TRACE("FreehandCalibrationToolbox::SkipTemporalClicked"); 

	m_TemporalCalibrationDone = true;

  SetDisplayAccordingToState();
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::StartSpatial()
{
	LOG_TRACE("FreehandCalibrationToolbox::StartSpatialClicked"); 

	if (m_FreehandCalibration->GetCalibrationMode() != REALTIME) {
		LOG_ERROR("Unable to start calibration in offline mode!");
		return; 
	}

	m_ParentMainWindow->SetTabsEnabled(false);

	QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

  PhantomRegistrationToolbox* phantomRegistrationToolbox = dynamic_cast<PhantomRegistrationToolbox*>(m_ParentMainWindow->GetToolbox(ToolboxType_PhantomRegistration));
  if ((phantomRegistrationToolbox != NULL) && (phantomRegistrationToolbox->GetPhantomRegistrationAlgo() != NULL)) {
    m_FreehandCalibration->InitializeCalibration(phantomRegistrationToolbox->GetPhantomRegistrationAlgo()->GetPhantomToPhantomReferenceTransform());
  } else {
    LOG_ERROR("Phantom registration toolbox or algorithm is not initialized!");
    return;
  }

  SetState(ToolboxState_InProgress);

  // Start calibration and compute results on success
	if ((m_FreehandCalibration->DoSpatialCalibration() == PLUS_SUCCESS) && (m_FreehandCalibration->ComputeCalibrationResults() == PLUS_SUCCESS)) {

    // Set result for visualization
    m_ParentMainWindow->GetToolVisualizer()->SetImageToProbeTransform(m_FreehandCalibration->GetTransformImageToProbe());

    m_SpatialCalibrationDone = true;

    SetState(ToolboxState_Done);
	}

	m_ParentMainWindow->SetTabsEnabled(true);

  SetDisplayAccordingToState();
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::ResetSpatial()
{
	LOG_TRACE("FreehandCalibrationToolbox::ResetSpatialClicked"); 

  // Turn off device visualization if it was on
  if (ui.checkBox_ShowDevices->isChecked() == true) {
    ui.checkBox_ShowDevices->setChecked(false);
  }

  // Reset calibration
  m_FreehandCalibration->ResetCalibration();

	m_ParentMainWindow->SetTabsEnabled(true);

  SetState(ToolboxState_Idle);

  m_SpatialCalibrationDone = false;

  SetDisplayAccordingToState();
}

//-----------------------------------------------------------------------------

bool FreehandCalibrationToolbox::IsReadyToStartSpatialCalibration()
{
	LOG_TRACE("FreehandCalibrationToolbox::IsReadyToStartSpatialCalibration");

  PhantomRegistrationToolbox* phantomRegistrationToolbox = dynamic_cast<PhantomRegistrationToolbox*>(m_ParentMainWindow->GetToolbox(ToolboxType_PhantomRegistration));

  if ((m_State == ToolboxState_Uninitialized)
		|| (! m_TemporalCalibrationDone)
		|| (phantomRegistrationToolbox == NULL)
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

  m_ParentMainWindow->GetToolVisualizer()->EnableImageMode(!aOn);
}

//-----------------------------------------------------------------------------
/*
PlusStatus vtkFreehandCalibrationController::DisplaySegmentedPoints(bool aSuccess)
{
	LOG_TRACE("vtkFreehandCalibrationController::DisplaySegmentedPoints(" << (aSuccess?"true":"false") << ")");

	if (! aSuccess) {
		this->SegmentedPointsActor->VisibilityOff();
		//this->SegmentedPointsActor->GetProperty()->SetColor(0.5, 0.5, 0.5);

		return PLUS_SUCCESS;
	}

	// Get last results and feed the points into vtkPolyData for displaying
	SegmentedFrame lastSegmentedFrame = this->SegmentedFrameContainer.at(this->SegmentedFrameContainer.size() - 1);
	int height = lastSegmentedFrame.TrackedFrameInfo->GetFrameSize()[1];

	vtkSmartPointer<vtkPoints> inputPoints = vtkSmartPointer<vtkPoints>::New();
  inputPoints->SetNumberOfPoints(this->GetPatternRecognition()->GetFidLabeling()->GetFoundDotsCoordinateValue().size());

	std::vector<std::vector<double>> dots = this->GetPatternRecognition()->GetFidLabeling()->GetFoundDotsCoordinateValue();
	for (int i=0; i<dots.size(); ++i) {
		inputPoints->InsertPoint(i, dots[i][0], height - dots[i][1], 0.0);
	}
	inputPoints->Modified();

	this->SegmentedPointsPolyData->Initialize();
	this->SegmentedPointsPolyData->SetPoints(inputPoints);

	this->SegmentedPointsActor->VisibilityOn();
	//this->SegmentedPointsActor->GetProperty()->SetColor(0.0, 0.8, 0.0);

	return PLUS_SUCCESS;
}
*/
