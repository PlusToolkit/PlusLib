#include "PhantomRegistrationToolbox.h"

#include "FreehandMainWindow.h"
#include "vtkFreehandController.h"
#include "StylusCalibrationController.h"

#include <QFileDialog>
#include <QTimer>

#include "vtkRenderWindow.h"

//-----------------------------------------------------------------------------

PhantomRegistrationToolbox::PhantomRegistrationToolbox(QWidget* aParent, Qt::WFlags aFlags)
	:AbstractToolbox()
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
	connect( ui.pushButton_OpenPhantomDefinition, SIGNAL( pressed() ), this, SLOT( OpenPhantomDefinitionClicked() ) );
	connect( ui.pushButton_OpenStylusCalibration, SIGNAL( pressed() ), this, SLOT( OpenStylusCalibrationClicked() ) );
	connect( ui.pushButton_RecordPoint, SIGNAL( pressed() ), this, SLOT( RecordPointClicked() ) );
	connect( ui.pushButton_Undo, SIGNAL( pressed() ), this, SLOT( UndoClicked() ) );
	connect( ui.pushButton_Reset, SIGNAL( pressed() ), this, SLOT( ResetClicked() ) );
	connect( ui.pushButton_Register, SIGNAL( pressed() ), this, SLOT( RegisterClicked() ) );
	connect( ui.pushButton_Save, SIGNAL( pressed() ), this, SLOT( SaveClicked() ) );

	connect( m_AcquisitionTimer, SIGNAL( timeout() ), this, SLOT( RequestDoAcquisition() ) );

	// Start timer for acquisition
	m_AcquisitionTimer->start(1000 / vtkFreehandController::GetInstance()->GetRecordingFrameRate());
}

//-----------------------------------------------------------------------------

PhantomRegistrationToolbox::~PhantomRegistrationToolbox()
{
	ui.canvasPhantom->GetRenderWindow()->RemoveRenderer(PhantomRegistrationController::GetInstance()->GetPhantomRenderer());

	PhantomRegistrationController* phantomRegistrationController = PhantomRegistrationController::GetInstance();
	if (phantomRegistrationController != NULL) {
		delete phantomRegistrationController;
	}

	if (m_AcquisitionTimer != NULL) {
		delete m_AcquisitionTimer;
		m_AcquisitionTimer = NULL;
	}
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::Initialize()
{
	// If stylus calibration has just been done, then indicate it
	if (StylusCalibrationController::GetInstance()->GetStylusToStylustipTransform() != NULL) {
		ui.lineEdit_StylusCalibration->setText(tr("Using session calibration data"));
	}
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::RefreshToolboxContent()
{
	//LOG_DEBUG("PhantomRegistrationToolbox: Refresh phantom registration toolbox content"); 

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
	PhantomRegistrationController::GetInstance()->Stop();
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::Clear()
{
	LOG_INFO("Clear phantom registration"); 

	// Stop the acquisition timer
	m_AcquisitionTimer->stop();
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::RequestDoAcquisition()
{
	PhantomRegistrationController::GetInstance()->DoAcquisition();
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::OpenPhantomDefinitionClicked()
{
	LOG_DEBUG("PhantomRegistrationToolbox: Open phantom definition button clicked"); 

	// File open dialog for selecting phantom definition xml
	QString filter = QString( tr( "XML files ( *.xml );;" ) );
	QString fileName = QFileDialog::getOpenFileName(NULL, QString( tr( "Open phantom descriptor XML" ) ), vtkFreehandController::GetInstance()->GetConfigDirectory(), filter);
	if (fileName.isNull()) {
		return;
	}

	// Load phantom definition xml
	if (PhantomRegistrationController::GetInstance()->LoadPhantomDefinitionFromFile(fileName.toStdString())) {
		// Refresh phantom canvas
		ui.canvasPhantom->update();

		ui.lineEdit_PhantomDefinition->setText(fileName);
		ui.lineEdit_PhantomDefinition->setToolTip(fileName);

		// Set to InProgress if both stylus calibration and phantom definition are available
		PhantomRegistrationController::GetInstance()->Start();
	}
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::OpenStylusCalibrationClicked()
{
	LOG_DEBUG("PhantomRegistrationToolbox: Open stylus calibration XML button clicked");

	// File open dialog for selecting phantom definition xml
	QString filter = QString( tr( "XML files ( *.xml );;" ) );
	QString fileName = QFileDialog::getOpenFileName(NULL, QString( tr( "Open stylus calibration XML" ) ), vtkFreehandController::GetInstance()->GetConfigDirectory(), filter);
	if (fileName.isNull()) {
		return;
	}

	// Load phantom definition xml
	if (StylusCalibrationController::GetInstance()->LoadStylusCalibrationFromFile(fileName.toStdString())) {
		ui.lineEdit_StylusCalibration->setText(fileName);
		ui.lineEdit_StylusCalibration->setToolTip(fileName);

		// Set to InProgress if both stylus calibration and phantom definition are available
		PhantomRegistrationController::GetInstance()->Start();
	}
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::RecordPointClicked()
{
	LOG_DEBUG("PhantomRegistrationToolbox: Record button clicked"); 

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
	LOG_DEBUG("PhantomRegistrationToolbox: Undo button clicked"); 

	PhantomRegistrationController::GetInstance()->Undo();
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::ResetClicked()
{
	LOG_DEBUG("PhantomRegistrationToolbox: Reset button clicked"); 

	PhantomRegistrationController::GetInstance()->Reset();

	emit SetTabsEnabled(true);
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::RegisterClicked()
{
	LOG_DEBUG("PhantomRegistrationToolbox: Register button clicked"); 

	PhantomRegistrationController::GetInstance()->Register();

	emit SetTabsEnabled(true);
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::SaveClicked()
{
	LOG_DEBUG("PhantomRegistrationToolbox: Save button clicked"); 

	QString filter = QString( tr( "XML files ( *.xml );;" ) );
	QString fileName = QFileDialog::getSaveFileName(NULL, tr("Save phantom registration result"), vtkFreehandController::GetInstance()->GetConfigDirectory(), filter);

	if (! fileName.isNull() ) {
		PhantomRegistrationController::GetInstance()->SavePhantomRegistrationToFile(fileName.toStdString());
	}	
}
