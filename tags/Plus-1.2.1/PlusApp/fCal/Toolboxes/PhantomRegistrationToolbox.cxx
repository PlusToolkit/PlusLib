#include "PhantomRegistrationToolbox.h"

#include "PhantomRegistrationController.h"
#include "vtkFreehandController.h"
#include "StylusCalibrationController.h"

#include <QFileDialog>
#include <QTimer>

#include "vtkRenderWindow.h"
#include "vtkFileFinder.h"

//-----------------------------------------------------------------------------

PhantomRegistrationToolbox::PhantomRegistrationToolbox(QWidget* aParent, Qt::WFlags aFlags)
	: AbstractToolbox()
	, QWidget(aParent, aFlags)
{
	ui.setupUi(this);

	// Create timer
	m_AcquisitionTimer = new QTimer(this);

	// Initialize toolbox controller
	PhantomRegistrationController* toolboxController = PhantomRegistrationController::GetInstance();
	if (toolboxController == NULL) {
		LOG_ERROR("Record phantom points toolbox controller is not initialized!");
		return;
	}

	toolboxController->SetToolbox(this);

	// Add renderer to phantom canvas
	ui.canvasPhantom->GetRenderWindow()->AddRenderer(toolboxController->GetPhantomRenderer());

	// Connect events
	connect( ui.pushButton_OpenPhantomDefinition, SIGNAL( clicked() ), this, SLOT( OpenPhantomDefinitionClicked() ) );
	connect( ui.pushButton_OpenStylusCalibration, SIGNAL( clicked() ), this, SLOT( OpenStylusCalibrationClicked() ) );
	connect( ui.pushButton_RecordPoint, SIGNAL( clicked() ), this, SLOT( RecordPointClicked() ) );
	connect( ui.pushButton_Undo, SIGNAL( clicked() ), this, SLOT( UndoClicked() ) );
	connect( ui.pushButton_Reset, SIGNAL( clicked() ), this, SLOT( ResetClicked() ) );
	connect( ui.pushButton_Register, SIGNAL( clicked() ), this, SLOT( RegisterClicked() ) );
	connect( ui.pushButton_Save, SIGNAL( clicked() ), this, SLOT( SaveClicked() ) );

	connect( m_AcquisitionTimer, SIGNAL( timeout() ), this, SLOT( RequestDoAcquisition() ) );
}

//-----------------------------------------------------------------------------

PhantomRegistrationToolbox::~PhantomRegistrationToolbox()
{
	PhantomRegistrationController* phantomRegistrationController = PhantomRegistrationController::GetInstance();
	if (phantomRegistrationController != NULL) {
		ui.canvasPhantom->GetRenderWindow()->RemoveRenderer(PhantomRegistrationController::GetInstance()->GetPhantomRenderer());

		delete phantomRegistrationController;
	}

	if (m_AcquisitionTimer != NULL) {
    m_AcquisitionTimer->stop();
		delete m_AcquisitionTimer;
		m_AcquisitionTimer = NULL;
	}
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::Initialize()
{
	LOG_TRACE("PhantomRegistrationToolbox::Initialize"); 

	// If stylus calibration has just been done, then indicate it
  StylusCalibrationController* stylusCalibrationController = StylusCalibrationController::GetInstance();
  if (stylusCalibrationController == NULL) {
    LOG_ERROR("Stylus calibration controller is not initialized!");
    return;
  }
  vtkFreehandController* controller = vtkFreehandController::GetInstance();
  if (controller == NULL) {
    LOG_ERROR("Freehand controller is not initialized!");
    return;
  }

  // If stylus calibration controller does not have the calibration transform then try to load it from the device set configuration
  if ((controller->GetConfigurationData()) && (stylusCalibrationController->GetStylustipToStylusTransform() == NULL)) {
    stylusCalibrationController->LoadStylusCalibration(controller->GetConfigurationData());
  }

  bool readyToStart = true;

  if (stylusCalibrationController->GetStylustipToStylusTransform() != NULL) {

		// In case the user changed device set since calibration, set the calibration to the tool
		stylusCalibrationController->FeedStylusCalibrationMatrixToTool();

		ui.lineEdit_StylusCalibration->setText(tr("Using session calibration data"));
  } else {
    readyToStart = false;
  }

  // Try to load phantom definition from the device set configuration
  if ((controller->GetConfigurationData()) && (PhantomRegistrationController::GetInstance()->LoadPhantomDefinition(controller->GetConfigurationData()) == PLUS_SUCCESS)) {
    ui.lineEdit_PhantomDefinition->setText(tr("Using session phantom definition"));
  } else {
    readyToStart = false;
  }

  // Start timer for acquisition
	m_AcquisitionTimer->start(1000 / vtkFreehandController::GetInstance()->GetRecordingFrameRate());

	// Set to InProgress if both stylus calibration and phantom definition are available
  if (readyToStart) {
		PhantomRegistrationController::GetInstance()->Start();
  }
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::RefreshToolboxContent()
{
	//LOG_TRACE("PhantomRegistrationToolbox: Refresh phantom registration toolbox content"); 

	PhantomRegistrationController* toolboxController = PhantomRegistrationController::GetInstance();

	// If initialization failed
	if (toolboxController->State() == ToolboxState_Uninitialized) {
		ui.label_StylusPosition->setText(tr("N/A"));
		ui.label_Instructions->setText("");

		ui.pushButton_OpenPhantomDefinition->setEnabled(false);
		ui.pushButton_OpenStylusCalibration->setEnabled(false);
		ui.pushButton_RecordPoint->setEnabled(false);
		ui.pushButton_Register->setEnabled(false);
		ui.pushButton_Reset->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);
		ui.pushButton_Undo->setEnabled(false);
	} else
	// If initialized
	if (toolboxController->State() == ToolboxState_Idle) {
		ui.label_StylusPosition->setText(tr("N/A"));
		if (ui.lineEdit_StylusCalibration->text().length() == 0) {
			ui.label_Instructions->setText(tr("Stylus calibration XML has to be loaded"));
		} else if (ui.lineEdit_PhantomDefinition->text().length() == 0) {
			ui.label_Instructions->setText(tr("Phantom definition XML has to be loaded"));
		}
		ui.label_Instructions->setFont(QFont("SansSerif", 8, QFont::Bold));

		ui.pushButton_OpenPhantomDefinition->setEnabled(true);
		ui.pushButton_OpenStylusCalibration->setEnabled(true);
		ui.pushButton_RecordPoint->setEnabled(false);
		ui.pushButton_Register->setEnabled(false);
		ui.pushButton_Reset->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);
		ui.pushButton_Undo->setEnabled(false);

		// If tab changed, then restart timer (clearing stops the timer)
		if (! m_AcquisitionTimer->isActive()) {
			m_AcquisitionTimer->start();
		}
	} else
	// If in progress
	if (toolboxController->State() == ToolboxState_InProgress) {
		ui.label_StylusPosition->setText(QString::fromStdString(toolboxController->GetPositionString()));
		ui.label_Instructions->setText(QString("Touch landmark named %1 and press Record point button").arg(QString::fromStdString(toolboxController->GetCurrentLandmarkName())));
		ui.label_Instructions->setFont(QFont("SansSerif", 8, QFont::Bold));

		ui.pushButton_OpenPhantomDefinition->setEnabled(true);
		ui.pushButton_OpenStylusCalibration->setEnabled(true);
		ui.pushButton_RecordPoint->setEnabled(true);
		ui.pushButton_Register->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);

		if (toolboxController->GetCurrentLandmarkIndex() < 1) {
			ui.pushButton_Undo->setEnabled(false);
			ui.pushButton_Reset->setEnabled(false);
		} else {
			ui.pushButton_Undo->setEnabled(true);
			ui.pushButton_Reset->setEnabled(true);
		}

		if (!(ui.pushButton_RecordPoint->hasFocus() || vtkFreehandController::GetInstance()->GetCanvas()->hasFocus())) {
			ui.pushButton_RecordPoint->setFocus();
		}
	} else
	// If done
	if (toolboxController->State() == ToolboxState_Done) {
		ui.label_StylusPosition->setText(QString::fromStdString(toolboxController->GetPositionString()));

		if (! toolboxController->GetPhantomToPhantomReferenceTransform()) {
			ui.label_Instructions->setText(tr("Press Register button to compute the transform"));

			ui.pushButton_OpenPhantomDefinition->setEnabled(true);
			ui.pushButton_OpenStylusCalibration->setEnabled(true);
			ui.pushButton_Register->setEnabled(true);
			ui.pushButton_Save->setEnabled(false);
			ui.pushButton_RecordPoint->setEnabled(false);
			ui.pushButton_Reset->setEnabled(true);
			ui.pushButton_Undo->setEnabled(true);
		} else {
			ui.label_Instructions->setText(tr("Transform is ready to save"));

			ui.pushButton_OpenPhantomDefinition->setEnabled(true);
			ui.pushButton_OpenStylusCalibration->setEnabled(true);
			ui.pushButton_Register->setEnabled(false);
			ui.pushButton_Save->setEnabled(true);
			ui.pushButton_RecordPoint->setEnabled(false);
			ui.pushButton_Reset->setEnabled(true);
			ui.pushButton_Undo->setEnabled(true);
		}

		ui.label_Instructions->setFont(QFont("SansSerif", 8, QFont::Bold));

		ui.pushButton_OpenPhantomDefinition->setEnabled(true);
		ui.pushButton_OpenStylusCalibration->setEnabled(true);

		// If tab changed, then restart timer (clearing stops the timer)
		if (! m_AcquisitionTimer->isActive()) {
			m_AcquisitionTimer->start();
		}
	} else
	// If error occured
	if (toolboxController->State() == ToolboxState_Error) {
		ui.label_StylusPosition->setText(tr("N/A"));
		ui.label_Instructions->setText("Error occured!");
		ui.label_Instructions->setFont(QFont("SansSerif", 8, QFont::Bold));

		ui.pushButton_OpenPhantomDefinition->setEnabled(true);
		ui.pushButton_OpenStylusCalibration->setEnabled(true);
		ui.pushButton_RecordPoint->setEnabled(false);
		ui.pushButton_Register->setEnabled(false);
		ui.pushButton_Reset->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);
		ui.pushButton_Undo->setEnabled(false);
	}

	ui.canvasPhantom->update();
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::Stop()
{
	LOG_TRACE("PhantomRegistrationToolbox::Stop"); 

	PhantomRegistrationController::GetInstance()->Stop();
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::Clear()
{
	LOG_TRACE("PhantomRegistrationToolbox::Clear"); 

	// Stop the acquisition timer
	m_AcquisitionTimer->stop();
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::RequestDoAcquisition()
{
	LOG_TRACE("PhantomRegistrationToolbox::RequestDoAcquisition"); 

	PhantomRegistrationController::GetInstance()->DoAcquisition();
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::OpenPhantomDefinitionClicked()
{
	LOG_TRACE("PhantomRegistrationToolbox: Open phantom definition button clicked"); 

	// File open dialog for selecting phantom definition xml
	QString filter = QString( tr( "XML files ( *.xml );;" ) );
	QString fileName = QFileDialog::getOpenFileName(NULL, QString( tr( "Open phantom descriptor XML" ) ), vtkFileFinder::GetInstance()->GetConfigurationDirectory(), filter);
	if (fileName.isNull()) {
		return;
	}

	// Load phantom definition xml
	if (PhantomRegistrationController::GetInstance()->LoadPhantomDefinitionFromFile(fileName.toStdString()) == PLUS_SUCCESS) {
		// Refresh phantom canvas
		ui.canvasPhantom->update();

		ui.lineEdit_PhantomDefinition->setText(fileName);
		ui.lineEdit_PhantomDefinition->setToolTip(fileName);

		// Set to InProgress if both stylus calibration and phantom definition are available
		PhantomRegistrationController::GetInstance()->Start();
	} else {
		ui.lineEdit_PhantomDefinition->setText(tr("Invalid file!"));
		ui.lineEdit_PhantomDefinition->setToolTip("");
	}
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::OpenStylusCalibrationClicked()
{
	LOG_TRACE("PhantomRegistrationToolbox: Open stylus calibration XML button clicked");

	// File open dialog for selecting phantom definition xml
	QString filter = QString( tr( "XML files ( *.xml );;" ) );
	QString fileName = QFileDialog::getOpenFileName(NULL, QString( tr( "Open stylus calibration XML" ) ), vtkFileFinder::GetInstance()->GetConfigurationDirectory(), filter);
	if (fileName.isNull()) {
		return;
	}

	// Load stylus calibration xml
	if (StylusCalibrationController::GetInstance()->LoadStylusCalibrationFromFile(fileName.toStdString()) == PLUS_SUCCESS) {
		ui.lineEdit_StylusCalibration->setText(fileName);
		ui.lineEdit_StylusCalibration->setToolTip(fileName);

		// Set to InProgress if both stylus calibration and phantom definition are available
		PhantomRegistrationController::GetInstance()->Start();
	} else {
		ui.lineEdit_StylusCalibration->setText(tr("Invalid file!"));
		ui.lineEdit_StylusCalibration->setToolTip("");
	}
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::RecordPointClicked()
{
	LOG_TRACE("PhantomRegistrationToolbox: Record button clicked"); 

	PhantomRegistrationController* toolboxController = PhantomRegistrationController::GetInstance();

	// Disable tabs if this is the first landmark to record
	if (toolboxController->GetCurrentLandmarkIndex() == 0) {
		emit SetTabsEnabled(false);
	}

	toolboxController->RequestRecording();
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::UndoClicked()
{
	LOG_TRACE("PhantomRegistrationToolbox: Undo button clicked"); 

	PhantomRegistrationController::GetInstance()->Undo();
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::ResetClicked()
{
	LOG_TRACE("PhantomRegistrationToolbox: Reset button clicked"); 

	PhantomRegistrationController::GetInstance()->Reset();

	emit SetTabsEnabled(true);
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::RegisterClicked()
{
	LOG_TRACE("PhantomRegistrationToolbox: Register button clicked"); 

	PhantomRegistrationController::GetInstance()->Register();

	emit SetTabsEnabled(true);
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::SaveClicked()
{
	LOG_TRACE("PhantomRegistrationToolbox: Save button clicked"); 

	QString filter = QString( tr( "XML files ( *.xml );;" ) );
	QString fileName = QFileDialog::getSaveFileName(NULL, tr("Save phantom registration result"), vtkFileFinder::GetInstance()->GetConfigurationDirectory(), filter);

	if (! fileName.isNull() ) {
		PhantomRegistrationController::GetInstance()->SavePhantomRegistrationToFile(fileName.toStdString());
	}	
}
