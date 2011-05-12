#include "FreehandCalibrationToolbox.h"

#include "FreehandMainWindow.h"
#include "vtkFreehandController.h"
#include "PhantomRegistrationController.h"

#include <QFileDialog>
#include <QTimer>

//-----------------------------------------------------------------------------

FreehandCalibrationToolbox::FreehandCalibrationToolbox(QWidget* aParent, Qt::WFlags aFlags)
	:AbstractToolbox()
{
	ui.setupUi(this);

	//TODO tooltips

	// Create timer
	m_AcquisitionTimer = new QTimer(this);

	// Initialize toolbox controller
	FreehandCalibrationController* toolboxController = FreehandCalibrationController::GetInstance();
	if (toolboxController == NULL) {
		LOG_ERROR("Freehand calibration toolbox controller is not initialized!");
		return;
	}

	toolboxController->SetToolbox(this);

	ui.label_SpatialCalibration->setFont(QFont("SansSerif", 9, QFont::Bold));
	ui.label_TemporalCalibration->setFont(QFont("SansSerif", 9, QFont::Bold));

	// Connect events
	connect( ui.pushButton_OpenPhantomDefinition, SIGNAL( pressed() ), this, SLOT( OpenPhantomDefinitionClicked() ) );
	connect( ui.pushButton_OpenPhantomRegistration, SIGNAL( pressed() ), this, SLOT( OpenPhantomRegistrationClicked() ) );
	connect( ui.pushButton_OpenCalibrationConfiguration, SIGNAL( pressed() ), this, SLOT( OpenCalibrationConfigurationClicked() ) );
	connect( ui.pushButton_StartTemporal, SIGNAL( pressed() ), this, SLOT( StartTemporalClicked() ) );
	connect( ui.pushButton_ResetTemporal, SIGNAL( pressed() ), this, SLOT( ResetTemporalClicked() ) );
	connect( ui.pushButton_StartSpatial, SIGNAL( pressed() ), this, SLOT( StartSpatialClicked() ) );
	connect( ui.pushButton_ResetSpatial, SIGNAL( pressed() ), this, SLOT( ResetSpatialClicked() ) );
	connect( ui.pushButton_Save, SIGNAL( pressed() ), this, SLOT( SaveClicked() ) );

}

//-----------------------------------------------------------------------------

FreehandCalibrationToolbox::~FreehandCalibrationToolbox()
{
	FreehandCalibrationController* freehandCalibrationController = FreehandCalibrationController::GetInstance();
	if (freehandCalibrationController != NULL) {
		delete freehandCalibrationController;
	}

	if (m_AcquisitionTimer != NULL) {
		delete m_AcquisitionTimer;
		m_AcquisitionTimer = NULL;
	}
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::Initialize()
{
	// If phantom registration has just been done, then indicate it
	if (PhantomRegistrationController::GetInstance()->GetPhantomReferenceToPhantomTransform() != NULL) {
		ui.lineEdit_PhantomRegistration->setText(tr("Using session calibration data"));
	}
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::RefreshToolboxContent()
{
	//LOG_DEBUG("StylusCalibrationToolbox: Refresh stylus calibration toolbox content"); 

	FreehandCalibrationController* toolboxController = FreehandCalibrationController::GetInstance();

	// If initialization failed
	if (toolboxController->State() == ToolboxState_Uninitialized) {
		ui.label_InstructionsTemporal->setText(tr(""));
		ui.pushButton_StartTemporal->setEnabled(false);
		ui.pushButton_ResetTemporal->setEnabled(false);

		ui.label_InstructionsSpatial->setText(tr(""));
		ui.frame_SpatialCalibration->setEnabled(false);

		ui.pushButton_Save->setEnabled(false);
	} else
	// If initialized
	if (toolboxController->State() == ToolboxState_Idle) {
		if (FreehandCalibrationController::GetInstance()->IsTemporalCalibrationDone() == false) { // If temporal calibration has not been done yet
			ui.label_InstructionsTemporal->setText(tr("Press Start and make abrupt movements with the probe every 2 seconds"));
			ui.pushButton_StartTemporal->setEnabled(true);
			ui.pushButton_ResetTemporal->setEnabled(false);

			ui.label_InstructionsSpatial->setText(tr(""));
			ui.frame_SpatialCalibration->setEnabled(false);

			ui.pushButton_Save->setEnabled(false);

			if (!(ui.pushButton_StartTemporal->hasFocus() || vtkFreehandController::GetInstance()->GetCanvas()->hasFocus())) {
				ui.pushButton_StartTemporal->setFocus();
			}
		} else { // If temporal calibration is finished
			ui.label_InstructionsTemporal->setText(tr("Temporal calibration is ready to save"));
			ui.pushButton_StartTemporal->setEnabled(false);
			ui.pushButton_ResetTemporal->setEnabled(true);

			ui.label_InstructionsSpatial->setText(tr("Press Start and start scanning the phantom"));
			ui.frame_SpatialCalibration->setEnabled(true);
			ui.pushButton_StartSpatial->setEnabled(true);
			ui.pushButton_ResetSpatial->setEnabled(false);

			ui.pushButton_Save->setEnabled(false);

			if (!(ui.pushButton_StartSpatial->hasFocus() || vtkFreehandController::GetInstance()->GetCanvas()->hasFocus())) {
				ui.pushButton_StartSpatial->setFocus();
			}
		}
	} else
	// If in progress
	if (toolboxController->State() == ToolboxState_InProgress) {
		if (FreehandCalibrationController::GetInstance()->IsTemporalCalibrationDone() == false) { // If temporal calibration has not been done yet
			ui.label_InstructionsTemporal->setText(tr("Make abrupt movements with the probe every 2 seconds"));
			ui.pushButton_StartTemporal->setEnabled(false);
			ui.pushButton_ResetTemporal->setEnabled(true);

			ui.label_InstructionsSpatial->setText(tr(""));
			ui.frame_SpatialCalibration->setEnabled(false);

			ui.pushButton_Save->setEnabled(false);
		} else { // If temporal calibration is finished
			ui.label_InstructionsTemporal->setText(tr("Temporal calibration is ready to save"));
			ui.pushButton_StartTemporal->setEnabled(false);
			ui.pushButton_ResetTemporal->setEnabled(true);

			ui.label_InstructionsSpatial->setText(tr("Scan the phantom in the most degrees of freedom possible"));
			ui.frame_SpatialCalibration->setEnabled(true);
			ui.pushButton_StartSpatial->setEnabled(false);
			ui.pushButton_ResetSpatial->setEnabled(true);

			ui.pushButton_Save->setEnabled(false);
		}
	} else
	// If done
	if (toolboxController->State() == ToolboxState_Done) {
		ui.label_InstructionsTemporal->setText(tr("Temporal calibration is ready to save"));
		ui.pushButton_StartTemporal->setEnabled(false);
		ui.pushButton_ResetTemporal->setEnabled(true);

		ui.label_InstructionsSpatial->setText(tr("Spatial calibration is ready to save"));
		ui.frame_SpatialCalibration->setEnabled(true);
		ui.pushButton_StartSpatial->setEnabled(false);
		ui.pushButton_ResetSpatial->setEnabled(true);

		ui.pushButton_Save->setEnabled(true);

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

		ui.pushButton_Save->setEnabled(false);

		QApplication::restoreOverrideCursor();
	}
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::Stop()
{
	FreehandCalibrationController::GetInstance()->Stop();
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::Clear()
{
	LOG_INFO("Clear freehand calibration"); 
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::RequestDoAcquisition()
{
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::OpenPhantomDefinitionClicked()
{
	LOG_DEBUG("FreehandCalibrationToolbox: Open phantom definition button clicked"); 

	// File open dialog for selecting phantom definition xml
	QString filter = QString( tr( "XML files ( *.xml );;" ) );
	QString fileName = QFileDialog::getOpenFileName(NULL, QString( tr( "Open phantom descriptor XML" ) ), vtkFreehandController::GetInstance()->GetConfigDirectory(), filter);
	if (fileName.isNull()) {
		return;
	}

	// Load phantom definition xml
	if (PhantomRegistrationController::GetInstance()->LoadPhantomDefinitionFromFile(fileName.toStdString())) {
		ui.lineEdit_PhantomDefinition->setText(fileName);
		ui.lineEdit_PhantomDefinition->setToolTip(fileName);
	}
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::OpenPhantomRegistrationClicked()
{
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::OpenCalibrationConfigurationClicked()
{
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::StartTemporalClicked()
{
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::ResetTemporalClicked()
{
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::StartSpatialClicked()
{
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::ResetSpatialClicked()
{
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::SaveClicked()
{
}
