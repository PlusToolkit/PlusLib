#include "StylusCalibrationToolbox.h"

#include "fCalMainWindow.h"
#include "vtkToolVisualizer.h"

#include "vtkPivotCalibrationAlgo.h"
#include "ConfigFileSaverDialog.h"

#include <QFileDialog>
#include <QTimer>

#include "vtkMath.h"

//-----------------------------------------------------------------------------

StylusCalibrationToolbox::StylusCalibrationToolbox(fCalMainWindow* aParentMainWindow, Qt::WFlags aFlags)
	: AbstractToolbox(aParentMainWindow)
	, QWidget(aParentMainWindow, aFlags)
{
	ui.setupUi(this);

  m_NumberOfPoints = 200;
  m_CurrentPointNumber = 0;
  m_StylusPositionString = "";

  // Create algorithm class
	m_PivotCalibration = vtkPivotCalibrationAlgo::New();
	if (m_PivotCalibration == NULL) {
		LOG_ERROR("Unable to instantiate pivot calibration algorithm class!");
		return;
	}

	// Feed number of points from controller
	ui.spinBox_NumberOfStylusCalibrationPoints->setValue(m_NumberOfPoints);

	// Connect events
	connect( ui.pushButton_Start, SIGNAL( clicked() ), this, SLOT( Start() ) );
	connect( ui.pushButton_Stop, SIGNAL( clicked() ), this, SLOT( Stop() ) );
	connect( ui.pushButton_Save, SIGNAL( clicked() ), this, SLOT( Save() ) );
	connect( ui.spinBox_NumberOfStylusCalibrationPoints, SIGNAL( valueChanged(int) ), this, SLOT( NumberOfStylusCalibrationPointsChanged(int) ) );
}

//-----------------------------------------------------------------------------

StylusCalibrationToolbox::~StylusCalibrationToolbox()
{
	if (m_PivotCalibration != NULL) {
		m_PivotCalibration->Delete();
		m_PivotCalibration = NULL;
	} 
}

//-----------------------------------------------------------------------------

void StylusCalibrationToolbox::Initialize()
{
  LOG_TRACE("StylusCalibrationToolbox::Initialize");

  if ((m_ParentMainWindow->GetToolVisualizer()->GetDataCollector() != NULL) && (m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetConnected())) {

    m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->SetTrackingOnly(true);

    if (m_State == ToolboxState_Uninitialized) {
	    SetState(ToolboxState_Idle);
    }

    if (m_State != ToolboxState_Done) {
      m_ParentMainWindow->GetToolVisualizer()->GetResultPolyData()->Initialize();
    }

  }
}

//-----------------------------------------------------------------------------

void StylusCalibrationToolbox::RefreshContent()
{
	//LOG_TRACE("StylusCalibrationToolbox: Refresh stylus calibration toolbox content"); 

	// If initialized
	if (m_State == ToolboxState_Idle) {
		ui.label_NumberOfPoints->setText(QString("%1 / %2").arg(0).arg(m_NumberOfPoints));
		ui.label_CurrentPosition->setText(QString::fromStdString(m_StylusPositionString));

	} else
	// If in progress
	if (m_State == ToolboxState_InProgress) {
		ui.label_NumberOfPoints->setText(QString("%1 / %2").arg(m_CurrentPointNumber).arg(m_NumberOfPoints));
		ui.label_CurrentPosition->setText(QString::fromStdString(m_StylusPositionString));
		m_ParentMainWindow->SetStatusBarProgress((int)(100.0 * (m_CurrentPointNumber / (double)m_NumberOfPoints) + 0.5));

	} else
	// If done
	if (m_State == ToolboxState_Done) {
		ui.label_CurrentPosition->setText(QString::fromStdString(m_ParentMainWindow->GetToolVisualizer()->GetToolPositionString(TRACKER_TOOL_STYLUS, true)));
	}
}

//-----------------------------------------------------------------------------

void StylusCalibrationToolbox::SetDisplayAccordingToState()
{
  LOG_TRACE("StylusCalibrationToolbox::SetDisplayAccordingToState"); 

  m_ParentMainWindow->GetToolVisualizer()->HideAll();
  m_ParentMainWindow->GetToolVisualizer()->EnableImageMode(false);

	if (m_State == ToolboxState_Uninitialized) {
		ui.label_NumberOfPoints->setText(QString("%1 / %2").arg(0).arg(m_NumberOfPoints));
		ui.label_CalibrationError->setText(tr("N/A"));
		ui.label_CurrentPosition->setText(tr("N/A"));
		ui.label_StylusTipTransform->setText(tr("N/A"));
		ui.label_Instructions->setText(tr(""));

		ui.pushButton_Start->setEnabled(false);
		ui.pushButton_Stop->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);

 		m_ParentMainWindow->SetStatusBarText(QString(""));
		m_ParentMainWindow->SetStatusBarProgress(-1);

} else
	// If initialized
  if (m_State == ToolboxState_Idle) {
		ui.label_CalibrationError->setText(tr("N/A"));
		ui.label_CurrentPosition->setText(tr("N/A"));
		ui.label_StylusTipTransform->setText(tr("N/A"));
		ui.label_Instructions->setText(tr("Put stylus so that its tip is in steady position, and press Start"));
		ui.label_Instructions->setFont(QFont("SansSerif", 8, QFont::Bold));

		ui.pushButton_Start->setEnabled(true);
		ui.pushButton_Stop->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);

		ui.pushButton_Start->setFocus();

 		m_ParentMainWindow->SetStatusBarText(QString(""));
		m_ParentMainWindow->SetStatusBarProgress(-1);

	} else
	// If in progress
	if (m_State == ToolboxState_InProgress) {
		ui.label_NumberOfPoints->setText(QString("%1 / %2").arg(m_CurrentPointNumber).arg(m_NumberOfPoints));
		ui.label_CalibrationError->setText(tr("N/A"));
		ui.label_CurrentPosition->setText(QString::fromStdString(m_StylusPositionString));
		ui.label_StylusTipTransform->setText(tr("N/A"));
		ui.label_Instructions->setText(tr("Move around stylus with its tip fixed until the required amount of points are aquired"));
		ui.label_Instructions->setFont(QFont("SansSerif", 8, QFont::Bold));

		ui.pushButton_Start->setEnabled(false);
		ui.pushButton_Stop->setEnabled(true);
		ui.pushButton_Save->setEnabled(false);

		m_ParentMainWindow->SetStatusBarText(QString(" Recording stylus positions"));
		m_ParentMainWindow->SetStatusBarProgress(0);

    m_ParentMainWindow->GetToolVisualizer()->ShowInput(true);

		ui.pushButton_Stop->setFocus();

  } else
	// If done
	if (m_State == ToolboxState_Done) {
		ui.pushButton_Start->setEnabled(true);
		ui.pushButton_Stop->setEnabled(false);
		ui.pushButton_Save->setEnabled(true);
		ui.label_Instructions->setText(tr("Calibration transform is ready to save"));
		ui.label_Instructions->setFont(QFont("SansSerif", 8, QFont::Bold));

		ui.label_NumberOfPoints->setText(QString("%1 / %2").arg(m_CurrentPointNumber).arg(m_NumberOfPoints));
		ui.label_CalibrationError->setText(QString("%1 mm").arg(m_PivotCalibration->GetCalibrationError(), 2));
		ui.label_CurrentPosition->setText(QString::fromStdString(m_StylusPositionString));
		ui.label_StylusTipTransform->setText(QString::fromStdString(m_PivotCalibration->GetTooltipToToolTranslationString()));

		m_ParentMainWindow->SetStatusBarText(QString(" Stylus calibration done"));
		m_ParentMainWindow->SetStatusBarProgress(-1);

    m_ParentMainWindow->GetToolVisualizer()->ShowInput(true);
    m_ParentMainWindow->GetToolVisualizer()->ShowTool(TRACKER_TOOL_STYLUS, true);
    m_ParentMainWindow->GetToolVisualizer()->ShowResult(true);

		QApplication::restoreOverrideCursor();

  } else
	// If error occured
	if (m_State == ToolboxState_Error) {
		ui.pushButton_Start->setEnabled(true);
		ui.pushButton_Stop->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);
		ui.label_Instructions->setText(tr(""));
		ui.label_Instructions->setFont(QFont("SansSerif", 8, QFont::Bold));

		ui.label_NumberOfPoints->setText(tr("N/A"));
		ui.label_CalibrationError->setText(tr("N/A"));
		ui.label_CurrentPosition->setText(tr("N/A"));
		ui.label_StylusTipTransform->setText(tr("N/A"));

		m_ParentMainWindow->SetStatusBarText(QString(""));
		m_ParentMainWindow->SetStatusBarProgress(-1);

		QApplication::restoreOverrideCursor();
  }
}

//-----------------------------------------------------------------------------

void StylusCalibrationToolbox::Start()
{
  LOG_TRACE("StylusCalibrationToolbox::StartClicked"); 

	m_ParentMainWindow->SetTabsEnabled(false);
	QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

	m_CurrentPointNumber = 0;

  // Clear input points and result point
	vtkSmartPointer<vtkPoints> inputPoints = vtkSmartPointer<vtkPoints>::New();
	m_ParentMainWindow->GetToolVisualizer()->GetInputPolyData()->SetPoints(inputPoints);

  vtkSmartPointer<vtkPoints> resultPointsPoint = vtkSmartPointer<vtkPoints>::New();
	m_ParentMainWindow->GetToolVisualizer()->GetResultPolyData()->SetPoints(resultPointsPoint);

  // Initialize calibration
	m_PivotCalibration->Initialize();

	// Initialize stylus tool
  vtkDisplayableTool* stylusDisplayable = m_ParentMainWindow->GetToolVisualizer()->GetDisplayableTool(TRACKER_TOOL_STYLUS);
  if (stylusDisplayable == NULL) {
    LOG_ERROR("Stylus tool not found!");
    return;
  }

	vtkSmartPointer<vtkTransform> initialTransform = vtkSmartPointer<vtkTransform>::New();
	initialTransform->Identity();
	stylusDisplayable->GetTool()->SetCalibrationMatrix(initialTransform->GetMatrix());

	// Set state to in progress
	SetState(ToolboxState_InProgress);

  // Connect acquisition function to timer
  connect( m_ParentMainWindow->GetToolVisualizer()->GetAcquisitionTimer(), SIGNAL( timeout() ), this, SLOT( AddStylusPositionToCalibration() ) );
}

//-----------------------------------------------------------------------------

void StylusCalibrationToolbox::Stop()
{
  LOG_TRACE("StylusCalibrationToolbox::StopClicked"); 

  // Disonnect acquisition function to timer
  disconnect( m_ParentMainWindow->GetToolVisualizer()->GetAcquisitionTimer(), SIGNAL( timeout() ), this, SLOT( AddStylusPositionToCalibration() ) );

  // Calibrate
	PlusStatus success = m_PivotCalibration->DoTooltipCalibration();

	if (success == PLUS_SUCCESS) {
		LOG_INFO("Stylus calibration successful");

    // Feed result to tool
    vtkDisplayableTool* stylusDisplayable = m_ParentMainWindow->GetToolVisualizer()->GetDisplayableTool(TRACKER_TOOL_STYLUS);
    if (stylusDisplayable == NULL) {
      LOG_ERROR("Stylus tool not found!");
      SetState(ToolboxState_Error);
      return;
    }

    stylusDisplayable->GetTool()->SetCalibrationMatrix(m_PivotCalibration->GetTooltipToToolTransform()->GetMatrix());

    // Set result point
    double stylustipPosition[3];
    m_PivotCalibration->GetTooltipPosition(stylustipPosition);
		vtkPoints* points = m_ParentMainWindow->GetToolVisualizer()->GetResultPolyData()->GetPoints();
		points->InsertPoint(0, stylustipPosition[0], stylustipPosition[1], stylustipPosition[2]);
		points->Modified();

    // Write result in configuration
    if (m_PivotCalibration->WriteConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData(), TRACKER_TOOL_STYLUS) != PLUS_SUCCESS) {
      LOG_ERROR("Unable to save stylus calibration result in configuration XML tree!");
      SetState(ToolboxState_Error);
      return;
    }

		SetState(ToolboxState_Done);

  } else {
		LOG_ERROR("Stylus calibration failed!");

		m_CurrentPointNumber = 0;

		SetState(ToolboxState_Error);
	}

	m_ParentMainWindow->SetTabsEnabled(true);
}

//-----------------------------------------------------------------------------

void StylusCalibrationToolbox::Save()
{
  LOG_TRACE("StylusCalibrationToolbox::Save");

  ConfigFileSaverDialog* configSaverDialog = new ConfigFileSaverDialog(this);
  configSaverDialog->exec();

  delete configSaverDialog;
}

//-----------------------------------------------------------------------------

void StylusCalibrationToolbox::NumberOfStylusCalibrationPointsChanged(int aNumberOfPoints)
{
	LOG_TRACE("StylusCalibrationToolbox::NumberOfStylusCalibrationPointsChanged");

	m_NumberOfPoints = aNumberOfPoints;
}

//-----------------------------------------------------------------------------

void StylusCalibrationToolbox::AddStylusPositionToCalibration()
{
	LOG_TRACE("StylusCalibrationToolbox::AddStylusPositionToCalibration");

  vtkSmartPointer<vtkMatrix4x4> stylusToReferenceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  if (m_ParentMainWindow->GetToolVisualizer()->AcquireTrackerPositionForToolByType(TRACKER_TOOL_STYLUS, stylusToReferenceMatrix) == TR_OK) {
		// Compute the new position - TODO: find other way
    double stylusPosition[4];
		double elements[16];
		double origin[4] = {0.0, 0.0, 0.0, 1.0};

    for (int i=0; i<4; ++i) {
      for (int j=0; j<4; ++j) {
        elements[4*j+i] = stylusToReferenceMatrix->GetElement(i,j);
      }
    }

		vtkMatrix4x4::PointMultiply(elements, origin, stylusPosition);

		// Assemble position string for toolbox
		char stylusPositionChars[32];

		sprintf_s(stylusPositionChars, 32, "%.1lf X %.1lf X %.1lf", stylusPosition[0], stylusPosition[1], stylusPosition[2]);
		m_StylusPositionString = std::string(stylusPositionChars);

    // Add point to the input if fulfills the criteria
		vtkPoints* points = m_ParentMainWindow->GetToolVisualizer()->GetInputPolyData()->GetPoints();

		double distance_lowThreshold_mm = 3.0; // TODO: review these thresholds with the guys
		double distance_highThreshold_mm = 1000.0;
		double distance = -1.0;
		if (m_CurrentPointNumber < 1) {
      // Always allow
			distance = (distance_lowThreshold_mm + distance_highThreshold_mm) / 2.0;
		} else {
			double previousPosition[4];
			points->GetPoint(m_CurrentPointNumber-1, previousPosition);
			previousPosition[3] = 1.0;
			// Square distance
			distance = vtkMath::Distance2BetweenPoints(stylusPosition, previousPosition);
		}

		// If current point is close to the previous one, or too far (outlier), we do not insert it
		if (distance < distance_lowThreshold_mm * distance_lowThreshold_mm) {
			LOG_DEBUG("Acquired position is too close to the previous - it is skipped");
		} else if (distance > distance_highThreshold_mm * distance_highThreshold_mm) {
			LOG_DEBUG("Acquired position seems to be an outlier - it is skipped");
		} else {
			// Add the point into the calibration dataset
      m_PivotCalibration->InsertNextCalibrationPoint(stylusToReferenceMatrix);

      // Add to polydata for rendering
			points->InsertPoint(m_CurrentPointNumber, stylusPosition[0], stylusPosition[1], stylusPosition[2]);
			points->Modified();

			// Set new current point number
			++m_CurrentPointNumber;

			// Reset the camera once in a while
			if ((m_CurrentPointNumber > 0) && ((m_CurrentPointNumber % 10 == 0) || (m_CurrentPointNumber == 5) || (m_CurrentPointNumber >= m_NumberOfPoints))) {
				m_ParentMainWindow->GetToolVisualizer()->GetCanvasRenderer()->ResetCamera();
			}

      // If enough points have been acquired, stop
			if (m_CurrentPointNumber >= m_NumberOfPoints) {
				Stop();
			}
    }
  }
}
