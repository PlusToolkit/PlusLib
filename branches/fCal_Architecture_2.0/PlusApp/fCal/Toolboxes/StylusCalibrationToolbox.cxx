#include "StylusCalibrationToolbox.h"

#include "StylusCalibrationAlgo.h"
#include "vtkFileFinder.h"
#include "vtkFreehandController.h"
#include "vtkFCalVisualizer.h"

#include <QVTKWidget.h>
#include <QFileDialog>
#include <QTimer>

//-----------------------------------------------------------------------------

StylusCalibrationToolbox::StylusCalibrationToolbox(vtkFCalVisualizer* aVisualizer, QWidget* aParent, Qt::WFlags aFlags)
	: AbstractToolbox()
	, QWidget(aParent, aFlags)
  , m_Visualizer(aVisualizer)
{
	ui.setupUi(this);

	ui.label_CurrentPositionText->setToolTip(tr("In reference tool coordinate system"));
	ui.label_CurrentPosition->setToolTip(tr("In reference tool coordinate system"));

  if (m_Visualizer == NULL) {
    LOG_ERROR("Stylus calibration toolbox cannot be created without valid visualizer!");
    return;
  }

  m_Visualizer->GetFCalController()->GetStylusCalibrationAlgo()->SetNumberOfPoints(100);

	// Feed number of points from controller
	ui.spinBox_NumberOfStylusCalibrationPoints->setValue(m_Visualizer->GetFCalController()->GetStylusCalibrationAlgo()->GetNumberOfPoints());

	// Connect events
	connect( ui.pushButton_Start, SIGNAL( clicked() ), this, SLOT( StartClicked() ) );
	connect( ui.pushButton_Stop, SIGNAL( clicked() ), this, SLOT( StopClicked() ) );
	connect( ui.pushButton_Save, SIGNAL( clicked() ), this, SLOT( SaveResultClicked() ) );
	connect( ui.spinBox_NumberOfStylusCalibrationPoints, SIGNAL( valueChanged(int) ), this, SLOT( NumberOfStylusCalibrationPointsChanged(int) ) );
}

//-----------------------------------------------------------------------------

StylusCalibrationToolbox::~StylusCalibrationToolbox()
{
}

//-----------------------------------------------------------------------------

void StylusCalibrationToolbox::RefreshToolboxContent()
{
	//LOG_TRACE("StylusCalibrationToolbox: Refresh stylus calibration toolbox content"); 

	StylusCalibrationAlgo* algo = m_Visualizer->GetFCalController()->GetStylusCalibrationAlgo();

	// If initialization failed
	if (m_State == ToolboxState_Uninitialized) {
		ui.label_NumberOfPoints->setText(QString("%1 / %2").arg(0).arg(algo->GetNumberOfPoints()));
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
	if (m_State == ToolboxState_Idle) {
		ui.label_NumberOfPoints->setText(QString("%1 / %2").arg(0).arg(algo->GetNumberOfPoints()));
		ui.label_BoundingBox->setText(tr("N/A"));
		ui.label_Precision->setText(tr("N/A"));
		ui.label_CurrentPosition->setText(tr("N/A"));
		ui.label_StylusTipTransform->setText(tr("N/A"));
		ui.label_Instructions->setText(tr("Put stylus so that its tip is in steady position, and press Start"));
		ui.label_Instructions->setFont(QFont("SansSerif", 8, QFont::Bold));

		ui.pushButton_Start->setEnabled(true);
		ui.pushButton_Stop->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);

		if (!(ui.pushButton_Start->hasFocus() || m_Visualizer->GetCanvas()->hasFocus())) {
			ui.pushButton_Start->setFocus();
		}

		// If tab changed, then restart timer (clearing stops the timer)
		if (! m_AcquisitionTimer->isActive()) {
			m_AcquisitionTimer->start();
		}
	} else
	// If in progress
	if (m_State == ToolboxState_InProgress) {
		ui.label_NumberOfPoints->setText(QString("%1 / %2").arg(algo->GetCurrentPointNumber()).arg(toolboxController->GetNumberOfPoints()));
		ui.label_BoundingBox->setText(QString::fromStdString(algo->GetBoundingBoxString()));
		ui.label_Precision->setText(tr("N/A"));
		ui.label_CurrentPosition->setText(QString::fromStdString(algo->GetPositionString()));
		ui.label_StylusTipTransform->setText(tr("N/A"));
		ui.label_Instructions->setText(tr("Move around stylus with its tip fixed until the required amount of points are aquired"));
		ui.label_Instructions->setFont(QFont("SansSerif", 8, QFont::Bold));

		ui.pushButton_Start->setEnabled(false);
		ui.pushButton_Stop->setEnabled(true);
		ui.pushButton_Save->setEnabled(false);

		if (!(ui.pushButton_Stop->hasFocus() || m_Visualizer->GetCanvas()->hasFocus())) {
			ui.pushButton_Stop->setFocus();
		}
	} else
	// If done
	if (m_State == ToolboxState_Done) {
		ui.pushButton_Start->setEnabled(true);
		ui.pushButton_Stop->setEnabled(false);
		ui.pushButton_Save->setEnabled(true);
		ui.label_Instructions->setText(tr("Calibration transform is ready to save"));
		ui.label_Instructions->setFont(QFont("SansSerif", 8, QFont::Bold));

		QApplication::restoreOverrideCursor();

		ui.label_NumberOfPoints->setText(QString("%1 / %2").arg(algo->GetCurrentPointNumber()).arg(toolboxController->GetNumberOfPoints()));
		ui.label_BoundingBox->setText(QString::fromStdString(algo->GetBoundingBoxString()));
		ui.label_Precision->setText(QString("%1 mm").arg(algo->GetPrecision(), 2));
		ui.label_CurrentPosition->setText(QString::fromStdString(algo->GetPositionString()));
		ui.label_StylusTipTransform->setText(QString::fromStdString(algo->GetStylustipToStylusTransformString()));

		//if (!(ui.pushButton_Start->hasFocus() || vtkFreehandController::GetInstance()->GetCanvas()->hasFocus())) {
		//	ui.pushButton_Start->setFocus();
		//}
	}
	// If error occured
	if (m_State == ToolboxState_Error) {
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
  LOG_TRACE("StylusCalibrationToolbox::StartClicked"); 

	emit SetTabsEnabled(false);
	QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

  // Start stylus calibration in controller
  if (m_Visualizer == NULL) {
    LOG_ERROR("Stylus calibration cannot be started without valid visualizer!");
    return;
  }

  if (m_Visualizer->Start(TOOLBOX_TYPE_STYLUS_CALIBRATION) != PLUS_SUCCESS) {
    LOG_ERROR("Starting stylus calibration failed!");
    return;
  }

	// Set state to in progress
	m_State = ToolboxState_InProgress;
}

//-----------------------------------------------------------------------------

void StylusCalibrationToolbox::StopClicked()
{
	LOG_TRACE("StylusCalibrationToolbox::StopClicked"); 

  if (this->Stop() != PLUS_SUCCESS) {
    LOG_ERROR("TODO");
    return;
  }
}

//-----------------------------------------------------------------------------

PlusStatus StylusCalibrationToolbox::Initialize()
{
	LOG_TRACE("StylusCalibrationToolbox::Initialize"); 

	// Set state to idle
	if (m_State == ToolboxState_Uninitialized) {
		m_State = ToolboxState_Idle;
	}
}

//-----------------------------------------------------------------------------

void StylusCalibrationToolbox::Stop()
{
	LOG_TRACE("StylusCalibrationToolbox::Stop"); 

	emit SetTabsEnabled(true);
	QApplication::restoreOverrideCursor();

	bool success = CalibrateStylus();

	if (success) {
		LOG_INFO("Stylus calibration successful");
		m_State = ToolboxState_Done;
	} else {
		LOG_ERROR("Stylus calibration failed!");
		m_CurrentPointNumber = 0;
		m_State = ToolboxState_Error;
	}
  
	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void StylusCalibrationToolbox::Clear()
{
	LOG_TRACE("StylusCalibrationToolbox::Clear"); 

	// Remove all actors from the renderer
	vtkRenderer* renderer = vtkFreehandController::GetInstance()->GetCanvasRenderer();
	renderer->RemoveActor(m_InputActor);
	renderer->RemoveActor(m_StylusActor);
	renderer->RemoveActor(m_StylusTipActor);
	renderer->Modified();
}

//-----------------------------------------------------------------------------

void StylusCalibrationToolbox::SaveResultClicked()
{
	LOG_TRACE("StylusCalibrationToolbox: Save button clicked"); 

	QString filter = QString( tr( "XML files ( *.xml );;" ) );
  QString fileName = QFileDialog::getSaveFileName(NULL, tr("Save stylus calibration result"), QString::fromStdString(vtkFreehandController::GetInstance()->GetNewConfigurationFileName()), filter);

	if (! fileName.isNull() ) {
		if (StylusCalibrationController::GetInstance()->SaveStylusCalibrationToFile(fileName.toStdString()) != PLUS_SUCCESS) {
      LOG_ERROR("Saving configuration file to '" << fileName.toStdString() << "' failed!");
      return;
    }
	}	
}

//-----------------------------------------------------------------------------

void StylusCalibrationToolbox::RequestDoAcquisition()
{
	LOG_TRACE("StylusCalibrationToolbox::RequestDoAcquisition"); 

	StylusCalibrationController::GetInstance()->DoAcquisition(); //TODO Singleton AbstractToolboxController? (so that it calls the constructor of this class too! then this function could go to abstract)
}

//-----------------------------------------------------------------------------

void StylusCalibrationToolbox::NumberOfStylusCalibrationPointsChanged(int aNumberOfPoints)
{
	LOG_TRACE("StylusCalibrationToolbox::NumberOfStylusCalibrationPointsChanged");

	StylusCalibrationController::GetInstance()->SetNumberOfPoints(aNumberOfPoints);
}
