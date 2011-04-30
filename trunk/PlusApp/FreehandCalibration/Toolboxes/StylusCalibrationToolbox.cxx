#include "StylusCalibrationToolbox.h"

#include "FreehandMainWindow.h"
#include "vtkFreehandController.h"

#include <QFileDialog>
#include <QTimer>

//-----------------------------------------------------------------------------

StylusCalibrationToolbox::StylusCalibrationToolbox(QWidget* aParent, Qt::WFlags aFlags)
	:AbstractToolbox()
{
	ui.setupUi(this);

	ui.label_CurrentPositionText->setToolTip(tr("In reference tool coordinate system"));
	ui.label_CurrentPosition->setToolTip(tr("In reference tool coordinate system"));

	// Create timer
	m_AcquisitionTimer = new QTimer(this);

	// Initialize toolbox controller
	StylusCalibrationController* toolboxController = StylusCalibrationController::GetInstance();
	if (toolboxController == NULL) {
		LOG_ERROR("Stylus calibration toolbox controller is not initialized!");
		return;
	}

	toolboxController->SetToolbox(this);
	toolboxController->SetNumberOfPoints(100); //TODO configurable

	// Connect events
	connect( ui.pushButton_Start, SIGNAL( clicked() ), this, SLOT( StartClicked() ) );
	connect( ui.pushButton_Stop, SIGNAL( clicked() ), this, SLOT( StopClicked() ) );
	connect( ui.pushButton_Save, SIGNAL( clicked() ), this, SLOT( SaveResultClicked() ) );

	connect( m_AcquisitionTimer, SIGNAL( timeout() ), this, SLOT( RequestDoAcquisition() ) );
}

//-----------------------------------------------------------------------------

StylusCalibrationToolbox::~StylusCalibrationToolbox()
{
	StylusCalibrationController* stylusCalibrationController = StylusCalibrationController::GetInstance();
	if (stylusCalibrationController != NULL) {
		delete stylusCalibrationController;
	}

	if (m_AcquisitionTimer != NULL) {
		delete m_AcquisitionTimer;
		m_AcquisitionTimer = NULL;
	}
}

//-----------------------------------------------------------------------------

void StylusCalibrationToolbox::RefreshToolboxContent()
{
	//LOG_DEBUG("StylusCalibrationToolbox: Refresh stylus calibration toolbox content"); 

	StylusCalibrationController* toolboxController = StylusCalibrationController::GetInstance();

	// If initialization failed
	if (toolboxController->State() == ToolboxState_Uninitialized) {
		ui.label_NumberOfPoints->setText(QString("%1 / %2").arg(0).arg(toolboxController->GetNumberOfPoints()));
		ui.label_BoundingBox->setText(tr("N/A"));
		ui.label_Precision->setText(tr("N/A"));
		ui.label_CurrentPosition->setText(tr("N/A"));
		ui.label_StylusTipTransform->setText(tr("N/A"));
		ui.label_Instructions->setText(tr(""));

		ui.pushButton_Start->setEnabled(false);
		ui.pushButton_Stop->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);
	} else
	// If initialized
	if (toolboxController->State() == ToolboxState_Idle) {
		ui.label_NumberOfPoints->setText(QString("%1 / %2").arg(0).arg(toolboxController->GetNumberOfPoints()));
		ui.label_BoundingBox->setText(tr("N/A"));
		ui.label_Precision->setText(tr("N/A"));
		ui.label_CurrentPosition->setText(tr("N/A"));
		ui.label_StylusTipTransform->setText(tr("N/A"));
		ui.label_Instructions->setText(tr("Put stylus so that its tip is in steady position, and press Start"));
		ui.label_Instructions->setFont(QFont("SansSerif", 8, QFont::Bold));

		ui.pushButton_Start->setEnabled(true);
		ui.pushButton_Stop->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);

		if (!(ui.pushButton_Start->hasFocus() || vtkFreehandController::GetInstance()->GetCanvas()->hasFocus())) {
			ui.pushButton_Start->setFocus();
		}
	} else
	// If in progress
	if (toolboxController->State() == ToolboxState_InProgress) {
		ui.label_NumberOfPoints->setText(QString("%1 / %2").arg(toolboxController->GetCurrentPointNumber()).arg(toolboxController->GetNumberOfPoints()));
		ui.label_BoundingBox->setText(QString::fromStdString(toolboxController->GetBoundingBoxString()));
		ui.label_Precision->setText(tr("N/A"));
		ui.label_CurrentPosition->setText(QString::fromStdString(toolboxController->GetPositionString()));
		ui.label_StylusTipTransform->setText(tr("N/A"));
		ui.label_Instructions->setText(tr("Move around stylus with its tip fixed until the required amount of points are aquired"));
		ui.label_Instructions->setFont(QFont("SansSerif", 8, QFont::Bold));

		ui.pushButton_Start->setEnabled(false);
		ui.pushButton_Stop->setEnabled(true);
		ui.pushButton_Save->setEnabled(false);

		if (!(ui.pushButton_Stop->hasFocus() || vtkFreehandController::GetInstance()->GetCanvas()->hasFocus())) {
			ui.pushButton_Stop->setFocus();
		}
	} else
	// If done
	if (toolboxController->State() == ToolboxState_Done) {
		ui.pushButton_Start->setEnabled(true);
		ui.pushButton_Stop->setEnabled(false);
		ui.pushButton_Save->setEnabled(true);
		ui.label_Instructions->setText(tr("Calibration transform is ready to save"));
		ui.label_Instructions->setFont(QFont("SansSerif", 8, QFont::Bold));

		QApplication::restoreOverrideCursor();

		ui.label_NumberOfPoints->setText(QString("%1 / %2").arg(toolboxController->GetCurrentPointNumber()).arg(toolboxController->GetNumberOfPoints()));
		ui.label_BoundingBox->setText(QString::fromStdString(toolboxController->GetBoundingBoxString()));
		ui.label_Precision->setText(QString("%1 mm").arg(toolboxController->GetPrecision(), 2));
		ui.label_CurrentPosition->setText(QString::fromStdString(toolboxController->GetPositionString()));
		ui.label_StylusTipTransform->setText(QString::fromStdString(toolboxController->GetStylusToStylustipTransformString()));

		//if (!(ui.pushButton_Start->hasFocus() || vtkFreehandController::GetInstance()->GetCanvas()->hasFocus())) {
		//	ui.pushButton_Start->setFocus();
		//}
	}
	// If error occured
	if (toolboxController->State() == ToolboxState_Error) {
		ui.pushButton_Start->setEnabled(true);
		ui.pushButton_Stop->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);
		ui.label_Instructions->setText(tr("Error occured!"));
		ui.label_Instructions->setFont(QFont("SansSerif", 8, QFont::Bold));

		QApplication::restoreOverrideCursor();

		ui.label_NumberOfPoints->setText(tr("N/A"));
		ui.label_BoundingBox->setText(tr("N/A"));
		ui.label_Precision->setText(tr("N/A"));
		ui.label_CurrentPosition->setText(tr("N/A"));
		ui.label_StylusTipTransform->setText(tr("N/A"));
	}
}

//-----------------------------------------------------------------------------

void StylusCalibrationToolbox::StartClicked()
{
	LOG_DEBUG("StylusCalibrationToolbox: Start button clicked"); 

	emit SetTabsEnabled(false);
	QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

	StylusCalibrationController::GetInstance()->Start();

	// Start timer for acquisition
	m_AcquisitionTimer->start(1000 / vtkFreehandController::GetInstance()->GetRecordingFrameRate());
}

//-----------------------------------------------------------------------------

void StylusCalibrationToolbox::StopClicked()
{
	LOG_DEBUG("StylusCalibrationToolbox: Stop button clicked"); 

	Stop();
}

//-----------------------------------------------------------------------------

void StylusCalibrationToolbox::Stop()
{
	LOG_INFO("Stop stylus calibration"); 

	emit SetTabsEnabled(true);
	QApplication::restoreOverrideCursor();

	StylusCalibrationController::GetInstance()->Stop();
}

//-----------------------------------------------------------------------------

void StylusCalibrationToolbox::Clear()
{
	LOG_INFO("Clear stylus calibration"); 

	// Stop the acquisition timer
	m_AcquisitionTimer->stop();
}

//-----------------------------------------------------------------------------

void StylusCalibrationToolbox::SaveResultClicked()
{
	LOG_DEBUG("StylusCalibrationToolbox: Save button clicked"); 

	QString filter = QString( tr( "XML files ( *.xml );;" ) );
	QString fileName = QFileDialog::getSaveFileName(NULL, tr("Save stylus calibration result"), vtkFreehandController::GetInstance()->GetConfigDirectory(), filter);

	if (! fileName.isNull() ) {
		StylusCalibrationController::GetInstance()->SaveStylusCalibrationToFile(fileName.toStdString());
	}	
}

//-----------------------------------------------------------------------------

void StylusCalibrationToolbox::RequestDoAcquisition()
{
	StylusCalibrationController::GetInstance()->DoAcquisition(); //TODO Singleton AbstractToolboxController? (ugy, hogy meghivja ennek a konstruktorat is! akkor mehetne ez a fgv az abstract-ba)
}
