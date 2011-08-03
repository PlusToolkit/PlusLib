#include "FreehandCalibrationToolbox.h"

#include "vtkFreehandCalibrationController.h"
#include "vtkFreehandController.h"
#include "PhantomRegistrationController.h"
#include "vtkFileFinder.h"

#include <QVTKWidget.h>
#include <QFileDialog>
#include <QTimer>
#include <QMessageBox>

//-----------------------------------------------------------------------------

FreehandCalibrationToolbox::FreehandCalibrationToolbox(QWidget* aParent, Qt::WFlags aFlags)
	: AbstractToolbox()
	, QWidget(aParent, aFlags)
{
	ui.setupUi(this);

	//TODO tooltips

	// Create timer
	m_AcquisitionTimer = new QTimer(this);

	// Initialize toolbox controller
	vtkFreehandCalibrationController* toolboxController = vtkFreehandCalibrationController::GetInstance();
	if (toolboxController == NULL) {
		LOG_ERROR("Freehand calibration toolbox controller is not initialized!");
		return;
	}

	toolboxController->SetToolbox(this);

	ui.label_SpatialCalibration->setFont(QFont("SansSerif", 9, QFont::Bold));
	ui.label_TemporalCalibration->setFont(QFont("SansSerif", 9, QFont::Bold));

	//ui.label_Results->setFont(QFont("Courier", 7));

	// Connect events
	connect( ui.pushButton_OpenPhantomRegistration, SIGNAL( clicked() ), this, SLOT( OpenPhantomRegistrationClicked() ) );
	connect( ui.pushButton_OpenCalibrationConfiguration, SIGNAL( clicked() ), this, SLOT( OpenCalibrationConfigurationClicked() ) );
	connect( ui.pushButton_StartTemporal, SIGNAL( clicked() ), this, SLOT( StartTemporalClicked() ) );
	connect( ui.pushButton_ResetTemporal, SIGNAL( clicked() ), this, SLOT( ResetTemporalClicked() ) );
	connect( ui.pushButton_SkipTemporal, SIGNAL( clicked() ), this, SLOT( SkipTemporalClicked() ) );
	connect( ui.pushButton_StartSpatial, SIGNAL( clicked() ), this, SLOT( StartSpatialClicked() ) );
	connect( ui.pushButton_ResetSpatial, SIGNAL( clicked() ), this, SLOT( ResetSpatialClicked() ) );
	connect( ui.pushButton_Save, SIGNAL( clicked() ), this, SLOT( SaveClicked() ) );
	connect( ui.checkBox_ShowDevices, SIGNAL( toggled(bool) ), this, SLOT( ShowDevicesToggled(bool) ) );

	connect( m_AcquisitionTimer, SIGNAL( timeout() ), this, SLOT( RequestDoAcquisition() ) );
}

//-----------------------------------------------------------------------------

FreehandCalibrationToolbox::~FreehandCalibrationToolbox()
{
	vtkFreehandCalibrationController* freehandCalibrationController = vtkFreehandCalibrationController::GetInstance();
	if (freehandCalibrationController != NULL) {
		delete freehandCalibrationController;
	}

	if (m_AcquisitionTimer != NULL) {
    m_AcquisitionTimer->stop();
		delete m_AcquisitionTimer;
		m_AcquisitionTimer = NULL;
	}
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::Initialize()
{
	LOG_TRACE("FreehandCalibrationToolbox::Initialize"); 

	// If phantom registration has just been done, then indicate it
	PhantomRegistrationController* phantomRegistrationController = PhantomRegistrationController::GetInstance();
	if (phantomRegistrationController == NULL) {
		LOG_ERROR("Phantom registration controller not initialized!");
		return;
	}
  vtkFreehandController* controller = vtkFreehandController::GetInstance();
  if (controller == NULL) {
    LOG_ERROR("Freehand controller is not initialized!");
    return;
  }

  // If phantom registration controller does not have the calibration transform then try to load it from the device set configuration
  if ((controller->GetConfigurationData()) && (phantomRegistrationController->GetPhantomToPhantomReferenceTransform() == NULL)) {
    phantomRegistrationController->LoadPhantomRegistration(controller->GetConfigurationData());
  }

	if (phantomRegistrationController->GetPhantomToPhantomReferenceTransform() != NULL) {
		ui.lineEdit_PhantomRegistration->setText(tr("Using session registration data"));
	}

  // Try to load calibration configuration from the device set configuration
  if ((controller->GetConfigurationData()) && (vtkFreehandCalibrationController::GetInstance()->ReadConfiguration(controller->GetConfigurationData()) == PLUS_SUCCESS)) {
    vtkFreehandCalibrationController::GetInstance()->SetConfigurationFileName(controller->GetConfigurationFileName());
    ui.lineEdit_CalibrationConfiguration->setText(tr("Using session calibration configuration"));
  }
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::RefreshToolboxContent()
{
	//LOG_TRACE("StylusCalibrationToolbox: Refresh stylus calibration toolbox content"); 

	vtkFreehandCalibrationController* toolboxController = vtkFreehandCalibrationController::GetInstance();

	// If initialization failed
	if (toolboxController->State() == ToolboxState_Uninitialized) {
		ui.label_InstructionsTemporal->setText(tr(""));
		ui.pushButton_StartTemporal->setEnabled(false);
		ui.pushButton_ResetTemporal->setEnabled(false);
		ui.pushButton_SkipTemporal->setEnabled(false);

		ui.label_InstructionsSpatial->setText(tr(""));
		ui.frame_SpatialCalibration->setEnabled(false);

		ui.checkBox_ShowDevices->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);
	} else
	// If initialized
	if (toolboxController->State() == ToolboxState_Idle) {
		if (toolboxController->GetTemporalCalibrationDone() == false) { // If temporal calibration has not been done yet
			ui.label_InstructionsTemporal->setText(tr("Press Start and to perform temporal calibration or Skip\n\nCurrent video time offset: %1 ms").arg(toolboxController->GetVideoTimeOffset()));

			ui.pushButton_StartTemporal->setEnabled(true);
			ui.pushButton_ResetTemporal->setEnabled(false);
			ui.pushButton_SkipTemporal->setEnabled(true);

			ui.label_InstructionsSpatial->setText(tr(""));
			ui.frame_SpatialCalibration->setEnabled(false);

			ui.checkBox_ShowDevices->setEnabled(false);
			ui.pushButton_Save->setEnabled(false);
			ui.label_Results->setText(tr(""));

			if (!(ui.pushButton_StartTemporal->hasFocus() || vtkFreehandController::GetInstance()->GetCanvas()->hasFocus())) {
				ui.pushButton_StartTemporal->setFocus();
			}
		} else { // If temporal calibration is finished
			ui.label_InstructionsTemporal->setText(tr("Current video time offset: %1 ms").arg(toolboxController->GetVideoTimeOffset()));
			ui.pushButton_StartTemporal->setEnabled(false);
			ui.pushButton_ResetTemporal->setEnabled(true);
			ui.pushButton_SkipTemporal->setEnabled(false);

      if (toolboxController->IsReadyToStartSpatialCalibration()) {
			  ui.label_InstructionsSpatial->setText(tr("Press Start and start scanning the phantom"));
      } else {
			  ui.label_InstructionsSpatial->setText(tr("Configuration files need to be loaded"));
      }
			ui.frame_SpatialCalibration->setEnabled(true);
			ui.pushButton_ResetSpatial->setEnabled(false);

			ui.checkBox_ShowDevices->setEnabled(false);
			ui.pushButton_Save->setEnabled(false);
			ui.label_Results->setText(tr(""));

			if (!(ui.pushButton_StartSpatial->hasFocus() || vtkFreehandController::GetInstance()->GetCanvas()->hasFocus())) {
				ui.pushButton_StartSpatial->setFocus();
			}
		}

		ui.pushButton_StartSpatial->setEnabled(toolboxController->IsReadyToStartSpatialCalibration());

		QApplication::restoreOverrideCursor();

	} else
	// If in progress
	if (toolboxController->State() == ToolboxState_InProgress) {
		if (toolboxController->GetTemporalCalibrationDone() == false) { // If temporal calibration has not been done yet
			ui.label_InstructionsTemporal->setText(tr("Make abrupt movements with the probe every 2 seconds"));
			ui.pushButton_StartTemporal->setEnabled(false);
			ui.pushButton_ResetTemporal->setEnabled(true);

			ui.label_InstructionsSpatial->setText(tr(""));
			ui.frame_SpatialCalibration->setEnabled(false);

			ui.checkBox_ShowDevices->setEnabled(false);
			ui.pushButton_Save->setEnabled(false);
			ui.label_Results->setText(tr(""));

			if (!(ui.pushButton_ResetTemporal->hasFocus() || vtkFreehandController::GetInstance()->GetCanvas()->hasFocus())) {
				ui.pushButton_ResetTemporal->setFocus();
			}

		} else { // If temporal calibration is finished
			ui.label_InstructionsTemporal->setText(tr("Temporal calibration is ready to save\n(video time offset: %1 ms)").arg(toolboxController->GetVideoTimeOffset()));
			ui.pushButton_StartTemporal->setEnabled(false);
			ui.pushButton_ResetTemporal->setEnabled(false);

			ui.label_InstructionsSpatial->setText(tr("Scan the phantom in the most degrees of freedom possible"));
			ui.frame_SpatialCalibration->setEnabled(true);
			ui.pushButton_StartSpatial->setEnabled(false);
			ui.pushButton_ResetSpatial->setEnabled(true);

			ui.checkBox_ShowDevices->setEnabled(false);
			ui.pushButton_Save->setEnabled(false);
			ui.label_Results->setText(tr(""));

			if (!(ui.pushButton_ResetSpatial->hasFocus() || vtkFreehandController::GetInstance()->GetCanvas()->hasFocus())) {
				ui.pushButton_ResetSpatial->setFocus();
			}
		}

		// Needed for forced refreshing the UI (without this, no progress is shown)
		QApplication::processEvents();

	} else
	// If done
	if (toolboxController->State() == ToolboxState_Done) {
		ui.label_InstructionsTemporal->setText(tr("Temporal calibration is ready to save\n(video time offset: %1 ms)").arg(toolboxController->GetVideoTimeOffset()));
		ui.pushButton_StartTemporal->setEnabled(false);
		ui.pushButton_ResetTemporal->setEnabled(false);

		ui.label_InstructionsSpatial->setText(tr("Spatial calibration is ready to save"));
		ui.frame_SpatialCalibration->setEnabled(true);
		ui.pushButton_StartSpatial->setEnabled(false);
		ui.pushButton_ResetSpatial->setEnabled(true);

		ui.checkBox_ShowDevices->setEnabled(true);
		ui.pushButton_Save->setEnabled(true);
		ui.label_Results->setText(QString::fromStdString(toolboxController->GetResultString()));

		if (!(ui.pushButton_Save->hasFocus() || vtkFreehandController::GetInstance()->GetCanvas()->hasFocus())) {
			ui.pushButton_Save->setFocus();
		}

		QApplication::restoreOverrideCursor();
	}
	// If error occured
	if (toolboxController->State() == ToolboxState_Error) {
		ui.label_InstructionsTemporal->setText(tr("Error occured!"));
		ui.label_InstructionsTemporal->setFont(QFont("SansSerif", 8, QFont::Bold));
		ui.pushButton_StartTemporal->setEnabled(false);
		ui.pushButton_ResetTemporal->setEnabled(false);

		ui.label_InstructionsSpatial->setText(tr(""));
		ui.frame_SpatialCalibration->setEnabled(false);
		ui.pushButton_StartSpatial->setEnabled(false);
		ui.pushButton_ResetSpatial->setEnabled(false);

		ui.checkBox_ShowDevices->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);

		QApplication::restoreOverrideCursor();

		// If tab changed, then restart timer (clearing stops the timer)
		if ((! m_AcquisitionTimer->isActive()) && (toolboxController->GetShowDevices())) {
			m_AcquisitionTimer->start();
		}
	}
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::Stop()
{
	LOG_TRACE("FreehandCalibrationToolbox::Stop"); 

	emit SetTabsEnabled(true);

	vtkFreehandCalibrationController::GetInstance()->Stop();
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::Clear()
{
	LOG_TRACE("FreehandCalibrationToolbox::Clear"); 

	// Stop the acquisition timer
	if (vtkFreehandCalibrationController::GetInstance()->GetShowDevices()) {
		m_AcquisitionTimer->stop();
	}
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::OpenPhantomRegistrationClicked()
{
	LOG_TRACE("FreehandCalibrationToolbox: Open phantom registration button clicked"); 

	// File open dialog for selecting phantom registration xml
	QString filter = QString( tr( "XML files ( *.xml );;" ) );
	QString fileName = QFileDialog::getOpenFileName(NULL, QString( tr( "Open phantom registration XML" ) ), vtkFileFinder::GetInstance()->GetConfigurationDirectory(), filter);
	if (fileName.isNull()) {
		return;
	}

	// Load phantom registration xml
	if (PhantomRegistrationController::GetInstance()->LoadPhantomRegistrationFromFile(fileName.toStdString()) == PLUS_SUCCESS) {
		ui.lineEdit_PhantomRegistration->setText(fileName);
		ui.lineEdit_PhantomRegistration->setToolTip(fileName);
	} else {
		ui.lineEdit_PhantomRegistration->setText(tr("Invalid file!"));
		ui.lineEdit_PhantomRegistration->setToolTip("");
	}
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::OpenCalibrationConfigurationClicked()
{
	LOG_TRACE("FreehandCalibrationToolbox: Open calibration configuration button clicked"); 

	// File open dialog for selecting calibration configuration xml
	QString filter = QString( tr( "XML files ( *.xml );;" ) );
	QString fileName = QFileDialog::getOpenFileName(NULL, QString( tr( "Open calibration configuration XML" ) ), vtkFileFinder::GetInstance()->GetConfigurationDirectory(), filter);
	if (fileName.isNull()) {
		return;
	}

	// Load calibration configuration xml
	if (vtkFreehandCalibrationController::GetInstance()->ReadConfiguration(fileName.toStdString().c_str()) != PLUS_SUCCESS) {
		ui.lineEdit_CalibrationConfiguration->setText(tr("Invalid file!"));
		ui.lineEdit_CalibrationConfiguration->setToolTip("");

		LOG_ERROR("Calibration configuration file " << fileName.toStdString().c_str() << " cannot be loaded!");
		return;
	}

	// Re-calculate camera parameters
	vtkFreehandCalibrationController::GetInstance()->CalculateImageCameraParameters();

	ui.lineEdit_CalibrationConfiguration->setText(fileName);
	ui.lineEdit_CalibrationConfiguration->setToolTip(fileName);
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::StartTemporalClicked()
{
	LOG_TRACE("FreehandCalibrationToolbox::StartTemporalClicked"); 

	emit SetTabsEnabled(false);

	vtkFreehandCalibrationController::GetInstance()->Start();

	vtkFreehandCalibrationController::GetInstance()->DoTemporalCalibration();

	// Re-enable tab changing and stop controller
	Stop();
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::ResetTemporalClicked()
{
	LOG_TRACE("FreehandCalibrationToolbox::ResetTemporalClicked"); 

  // Reset controller
	vtkFreehandCalibrationController::GetInstance()->Reset();

  // Re-enable tab changing and stop controller
	Stop();
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::SkipTemporalClicked()
{
	LOG_TRACE("FreehandCalibrationToolbox::SkipTemporalClicked"); 

	vtkFreehandCalibrationController::GetInstance()->TemporalCalibrationDoneOn();
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::StartSpatialClicked()
{
	LOG_TRACE("FreehandCalibrationToolbox::StartSpatialClicked"); 

	vtkFreehandCalibrationController* toolboxController = vtkFreehandCalibrationController::GetInstance();
	if (toolboxController == NULL) {
		LOG_ERROR("Freehand calibration toolbox controller is not initialized!");
		return;
	}

	if (toolboxController->GetCalibrationMode() != REALTIME) {
		LOG_ERROR("Unable to start calibration in offline mode!");
		return; 
	}

	emit SetTabsEnabled(false);

	QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

	toolboxController->Start();

	// Start calibration and compute results on success
	if (toolboxController->DoSpatialCalibration() == PLUS_SUCCESS) {

		toolboxController->ComputeCalibrationResults();
	}

	// Re-enable tab changing and stop controller
	Stop();
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::ResetSpatialClicked()
{
	LOG_TRACE("FreehandCalibrationToolbox::ResetSpatialClicked"); 

  // Turn off device visualization if it was on
  if (vtkFreehandCalibrationController::GetInstance()->GetShowDevices() == true) {
    ui.checkBox_ShowDevices->setChecked(false);
  }

  // Reset controller
  vtkFreehandCalibrationController::GetInstance()->Reset();

  // Re-enable tab changing and stop controller
	Stop();
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::SaveClicked()
{
	LOG_TRACE("FreehandCalibrationToolbox::SaveClicked"); 

  if (vtkFreehandController::GetInstance()->SaveConfigurationToFile() == PLUS_SUCCESS) {
    QMessageBox::message(tr("fCal"), tr("Configuration saved to %1").arg(vtkFreehandController::GetInstance()->GetConfigurationFileName()));
  }
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::ShowDevicesToggled(bool aOn)
{
	LOG_TRACE("FreehandCalibrationToolbox::ShowDevicesToggled(" << (aOn?"true":"false") << ")"); 

	if (aOn) {
		// Start timer for acquisition
		m_AcquisitionTimer->start(1000 / vtkFreehandController::GetInstance()->GetRecordingFrameRate());
	} else {
		// Stop acquisition timer
		m_AcquisitionTimer->stop();
	}

	vtkFreehandCalibrationController::GetInstance()->ToggleDeviceVisualization(aOn);
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::RequestDoAcquisition()
{
	LOG_TRACE("FreehandCalibrationToolbox::RequestDoAcquisition"); 

	vtkFreehandCalibrationController::GetInstance()->DoAcquisition();
}
