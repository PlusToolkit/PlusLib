/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PhantomRegistrationToolbox.h"

#include "fCalMainWindow.h"
#include "vtkToolVisualizer.h"

#include "StylusCalibrationToolbox.h"
#include "ConfigFileSaverDialog.h"

#include "vtkPhantomRegistrationAlgo.h"
#include "vtkPivotCalibrationAlgo.h"
#include "vtkFakeTracker.h"

#include <QFileDialog>

#include "vtkXMLUtilities.h"
#include "vtkRenderWindow.h"
#include "vtkAccurateTimer.h"
#include "vtkPolyDataMapper.h"
#include "vtkGlyph3D.h"
#include "vtkSphereSource.h"
#include "vtkProperty.h"
#include "vtkSTLReader.h"

//-----------------------------------------------------------------------------

PhantomRegistrationToolbox::PhantomRegistrationToolbox(fCalMainWindow* aParentMainWindow, Qt::WFlags aFlags)
	: AbstractToolbox(aParentMainWindow)
	, QWidget(aParentMainWindow, aFlags)
  , m_PhantomActor(NULL)
  , m_RequestedLandmarkActor(NULL)
  , m_RequestedLandmarkPolyData(NULL)
  , m_PhantomRenderer(NULL)
  , m_CurrentLandmarkIndex(0)
{
	ui.setupUi(this);

  // Create algorithm class
	m_PhantomRegistration = vtkPhantomRegistrationAlgo::New();
	if (m_PhantomRegistration == NULL) {
		LOG_ERROR("Unable to instantiate phantom registration algorithm class!");
		return;
	}

  // Create and add renderer to phantom canvas
	m_PhantomRenderer = vtkRenderer::New();
  m_PhantomRenderer->SetBackground(0.1, 0.1, 0.1);
  m_PhantomRenderer->SetBackground2(0.4, 0.4, 0.4);
  m_PhantomRenderer->SetGradientBackground(true);

	ui.canvasPhantom->GetRenderWindow()->AddRenderer(m_PhantomRenderer);

  // Connect events
	connect( ui.pushButton_OpenPhantomDefinition, SIGNAL( clicked() ), this, SLOT( OpenPhantomDefinition() ) );
	connect( ui.pushButton_OpenStylusCalibration, SIGNAL( clicked() ), this, SLOT( OpenStylusCalibration() ) );
	connect( ui.pushButton_RecordPoint, SIGNAL( clicked() ), this, SLOT( RecordPoint() ) );
	connect( ui.pushButton_Undo, SIGNAL( clicked() ), this, SLOT( Undo() ) );
	connect( ui.pushButton_Reset, SIGNAL( clicked() ), this, SLOT( Reset() ) );
	connect( ui.pushButton_Save, SIGNAL( clicked() ), this, SLOT( Save() ) );
}

//-----------------------------------------------------------------------------

PhantomRegistrationToolbox::~PhantomRegistrationToolbox()
{
	if (m_PhantomRegistration != NULL) {
		m_PhantomRegistration->Delete();
		m_PhantomRegistration = NULL;
	} 

 	if (m_PhantomActor != NULL) {
    m_PhantomRenderer->RemoveActor(m_PhantomActor);
		m_PhantomActor->Delete();
		m_PhantomActor = NULL;
	}

	if (m_RequestedLandmarkActor != NULL) {
    m_PhantomRenderer->RemoveActor(m_RequestedLandmarkActor);
		m_RequestedLandmarkActor->Delete();
		m_RequestedLandmarkActor = NULL;
	}

	if (m_RequestedLandmarkPolyData != NULL) {
		m_RequestedLandmarkPolyData->Delete();
		m_RequestedLandmarkPolyData = NULL;
	}

  if (m_PhantomRenderer != NULL) {
	  ui.canvasPhantom->GetRenderWindow()->RemoveRenderer(m_PhantomRenderer);
		m_PhantomRenderer->Delete();
		m_PhantomRenderer = NULL;
	}
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::Initialize()
{
	LOG_TRACE("PhantomRegistrationToolbox::Initialize"); 

  if ( (m_ParentMainWindow->GetToolVisualizer()->GetDataCollector() != NULL)
    && (m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetConnected())
    && (m_State != ToolboxState_Done) )
  {
    m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->SetTrackingOnly(true);

    bool readyToStart = true;

    // Determine if there is already a stylus calibration present (either in the tool if it was just performed or in the configuration data)
    StylusCalibrationToolbox* stylusCalibrationToolbox = dynamic_cast<StylusCalibrationToolbox*>(m_ParentMainWindow->GetToolbox(ToolboxType_StylusCalibration));
    if ((stylusCalibrationToolbox != NULL) && (stylusCalibrationToolbox->GetPivotCalibrationAlgo() != NULL)) {
      vtkPivotCalibrationAlgo* pivotCalibration = stylusCalibrationToolbox->GetPivotCalibrationAlgo();

      if ( (stylusCalibrationToolbox->GetState() == ToolboxState_Done)
        || (pivotCalibration->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData(), TRACKER_TOOL_STYLUS) == PLUS_SUCCESS) )
      {
  		  ui.lineEdit_StylusCalibration->setText(tr("Using session calibration data"));

        // Set calibration matrix to stylus tool for later use
        vtkDisplayableTool* stylusDisplayable = m_ParentMainWindow->GetToolVisualizer()->GetDisplayableTool(TRACKER_TOOL_STYLUS);
        if (stylusDisplayable == NULL) {
          LOG_ERROR("Stylus tool not found!");
          return;
        }

        vtkSmartPointer<vtkMatrix4x4> calibrationMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
        calibrationMatrix->DeepCopy(pivotCalibration->GetTooltipToToolTransform()->GetMatrix());
        stylusDisplayable->GetTool()->SetCalibrationMatrix(calibrationMatrix);
        stylusDisplayable->GetTool()->GetBuffer()->SetToolCalibrationMatrix(calibrationMatrix); // TODO This is not good that we have to set a matrix to two classes. It should be stored in one member variable only

      } else {
        readyToStart = false;
      }
    } else {
      LOG_ERROR("Stylus calibration toolbox not found!");
      return;
    }

    // Try to load phantom definition from the device set configuration
    if (m_PhantomRegistration->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) == PLUS_SUCCESS)
    {
      ui.lineEdit_PhantomDefinition->setText(tr("Using session phantom definition"));
    }
    else
    {
      readyToStart = false;
    }

    // Initialize toolbox canvas
    if (InitializeVisualization() != PLUS_SUCCESS)
    {
      LOG_ERROR("Initializing phantom registration visualization failed!");
      return;
    }

    // Set state to idle
	  if (m_State == ToolboxState_Uninitialized)
    {
	    SetState(ToolboxState_Idle);
	  }

    // Set to InProgress if both stylus calibration and phantom definition are available
    if (readyToStart)
    {
      Start();
    }

  }
  else
  {
    SetDisplayAccordingToState();
  }
}

//-----------------------------------------------------------------------------

PlusStatus PhantomRegistrationToolbox::InitializeVisualization()
{
	LOG_TRACE("PhantomRegistrationToolbox::InitializeVisualization"); 

  if (m_State == ToolboxState_Uninitialized) {
    if (m_ParentMainWindow->GetToolVisualizer()->GetDisplayableTool(TRACKER_TOOL_REFERENCE) == NULL) {
      LOG_ERROR("Invalid reference tool actor! Probable device set is not connected.");
      return PLUS_FAIL;
    }

    // Initialize requested landmarks visualization in toolbox canvas
		m_RequestedLandmarkPolyData = vtkPolyData::New();
		m_RequestedLandmarkPolyData->Initialize();
		vtkSmartPointer<vtkPoints> requestedLandmarkPoints = vtkSmartPointer<vtkPoints>::New();
		m_RequestedLandmarkPolyData->SetPoints(requestedLandmarkPoints);

		m_RequestedLandmarkActor = vtkActor::New();
		vtkSmartPointer<vtkPolyDataMapper> requestedLandmarksMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		vtkSmartPointer<vtkGlyph3D> requestedLandmarksGlyph = vtkSmartPointer<vtkGlyph3D>::New();
		vtkSmartPointer<vtkSphereSource> requestedLandmarksSphereSource = vtkSmartPointer<vtkSphereSource>::New();
		requestedLandmarksSphereSource->SetRadius(1.5); // mm

		requestedLandmarksGlyph->SetInputConnection(m_RequestedLandmarkPolyData->GetProducerPort());
		requestedLandmarksGlyph->SetSourceConnection(requestedLandmarksSphereSource->GetOutputPort());
		requestedLandmarksMapper->SetInputConnection(requestedLandmarksGlyph->GetOutputPort());
		m_RequestedLandmarkActor->SetMapper(requestedLandmarksMapper);
		m_RequestedLandmarkActor->GetProperty()->SetColor(1.0, 0.0, 0.0);

    // Initialize phantom visualization in toolbox canvas
    vtkSmartPointer<vtkSTLReader> stlReader = vtkSmartPointer<vtkSTLReader>::New();
    if (m_ParentMainWindow->GetToolVisualizer()->LoadPhantomModel(stlReader) == PLUS_SUCCESS) {
      m_PhantomActor = vtkActor::New();
		  vtkSmartPointer<vtkPolyDataMapper> stlMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		  stlMapper->SetInputConnection(stlReader->GetOutputPort());
		  m_PhantomActor->SetMapper(stlMapper);
      m_PhantomActor->GetProperty()->SetOpacity(0.6);
      m_PhantomActor->SetUserTransform(m_ParentMainWindow->GetToolVisualizer()->GetDisplayableTool(TRACKER_TOOL_REFERENCE)->GetTool()->GetModelToToolTransform());

    } else {
      LOG_ERROR("Phantom cannot be visualized in toolbox canvas because model cannot be loaded!");
      return PLUS_FAIL;
    }

		// Add actors
    m_PhantomRenderer->AddActor(m_PhantomActor);
		m_PhantomRenderer->AddActor(m_RequestedLandmarkActor);
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::RefreshContent()
{
	//LOG_TRACE("PhantomRegistrationToolbox::RefreshContent"); 

	// If in progress
	if (m_State == ToolboxState_InProgress) {
		ui.label_StylusPosition->setText(m_ParentMainWindow->GetToolVisualizer()->GetToolPositionString(TRACKER_TOOL_STYLUS, true).c_str());
    ui.label_Instructions->setText(QString("Touch landmark named %1 and press Record point button").arg(m_PhantomRegistration->GetDefinedLandmarkName(m_CurrentLandmarkIndex).c_str()));

		if (m_CurrentLandmarkIndex < 1) {
			ui.pushButton_Undo->setEnabled(false);
			ui.pushButton_Reset->setEnabled(false);
		} else {
			ui.pushButton_Undo->setEnabled(true);
			ui.pushButton_Reset->setEnabled(true);
		}

    m_ParentMainWindow->SetStatusBarProgress((int)(100.0 * (m_CurrentLandmarkIndex / m_PhantomRegistration->GetDefinedLandmarks()->GetNumberOfPoints()) + 0.5));

  } else
	// If done
	if (m_State == ToolboxState_Done) {
    ui.label_StylusPosition->setText(m_ParentMainWindow->GetToolVisualizer()->GetToolPositionString(TRACKER_TOOL_STYLUS, true).c_str());
  }

	ui.canvasPhantom->update();
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::SetDisplayAccordingToState()
{
  LOG_TRACE("PhantomRegistrationToolbox::SetDisplayAccordingToState");

  m_ParentMainWindow->GetToolVisualizer()->HideAll();
  m_ParentMainWindow->GetToolVisualizer()->EnableImageMode(false);

	if (m_State == ToolboxState_Uninitialized)
  {
		ui.label_StylusPosition->setText(tr("N/A"));
		ui.label_Instructions->setText("");

		ui.pushButton_OpenPhantomDefinition->setEnabled(false);
		ui.pushButton_OpenStylusCalibration->setEnabled(false);
		ui.pushButton_RecordPoint->setEnabled(false);
		ui.pushButton_Reset->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);
		ui.pushButton_Undo->setEnabled(false);

		m_ParentMainWindow->SetStatusBarText(QString(""));
		m_ParentMainWindow->SetStatusBarProgress(-1);
  }
  else if (m_State == ToolboxState_Idle)
  {
		ui.label_StylusPosition->setText(tr("N/A"));
		if (ui.lineEdit_StylusCalibration->text().length() == 0) {
			ui.label_Instructions->setText(tr("Stylus calibration XML needs to be loaded"));
		} else if (ui.lineEdit_PhantomDefinition->text().length() == 0) {
			ui.label_Instructions->setText(tr("Phantom definition XML needs to be loaded"));
		}
		ui.label_Instructions->setFont(QFont("SansSerif", 8, QFont::Bold));

		ui.pushButton_OpenPhantomDefinition->setEnabled(true);
		ui.pushButton_OpenStylusCalibration->setEnabled(true);
		ui.pushButton_RecordPoint->setEnabled(false);
		ui.pushButton_Reset->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);
		ui.pushButton_Undo->setEnabled(false);

		m_ParentMainWindow->SetStatusBarText(QString(""));
		m_ParentMainWindow->SetStatusBarProgress(-1);
  }
  else if (m_State == ToolboxState_InProgress)
  {
		ui.label_Instructions->setFont(QFont("SansSerif", 8, QFont::Bold));

		ui.pushButton_OpenPhantomDefinition->setEnabled(true);
		ui.pushButton_OpenStylusCalibration->setEnabled(true);
		ui.pushButton_RecordPoint->setEnabled(true);
		ui.pushButton_Save->setEnabled(false);

		if (m_CurrentLandmarkIndex < 1) {
			ui.pushButton_Undo->setEnabled(false);
			ui.pushButton_Reset->setEnabled(false);
		} else {
			ui.pushButton_Undo->setEnabled(true);
			ui.pushButton_Reset->setEnabled(true);
		}

    m_ParentMainWindow->SetStatusBarText(QString(" Recording phantom landmarks"));
		m_ParentMainWindow->SetStatusBarProgress(0);

    m_ParentMainWindow->GetToolVisualizer()->ShowInput(true);
    m_ParentMainWindow->GetToolVisualizer()->ShowTool(TRACKER_TOOL_STYLUS, true);
    if (m_CurrentLandmarkIndex >= 3) {
      m_ParentMainWindow->GetToolVisualizer()->ShowTool(TRACKER_TOOL_REFERENCE, true);
    }

    ui.pushButton_RecordPoint->setFocus();
  }
  else if (m_State == ToolboxState_Done)
  {
		ui.label_Instructions->setText(tr("Transform is ready to save"));

		ui.pushButton_OpenPhantomDefinition->setEnabled(true);
		ui.pushButton_OpenStylusCalibration->setEnabled(true);
		ui.pushButton_Save->setEnabled(true);
		ui.pushButton_RecordPoint->setEnabled(false);
		ui.pushButton_Reset->setEnabled(true);
		ui.pushButton_Undo->setEnabled(true);

		ui.label_Instructions->setFont(QFont("SansSerif", 8, QFont::Bold));

		ui.pushButton_OpenPhantomDefinition->setEnabled(true);
		ui.pushButton_OpenStylusCalibration->setEnabled(true);

		m_ParentMainWindow->SetStatusBarText(QString(" Phantom registration done"));
		m_ParentMainWindow->SetStatusBarProgress(-1);

    m_ParentMainWindow->GetToolVisualizer()->ShowInput(true);
    m_ParentMainWindow->GetToolVisualizer()->ShowTool(TRACKER_TOOL_REFERENCE, true);
    m_ParentMainWindow->GetToolVisualizer()->ShowTool(TRACKER_TOOL_STYLUS, true);

  }
  else if (m_State == ToolboxState_Error)
  {
		ui.label_StylusPosition->setText(tr("N/A"));
		ui.label_Instructions->setText("Error occured!");
		ui.label_Instructions->setFont(QFont("SansSerif", 8, QFont::Bold));

		ui.pushButton_OpenPhantomDefinition->setEnabled(true);
		ui.pushButton_OpenStylusCalibration->setEnabled(true);
		ui.pushButton_RecordPoint->setEnabled(false);
		ui.pushButton_Reset->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);
		ui.pushButton_Undo->setEnabled(false);

    m_ParentMainWindow->SetStatusBarText(QString(""));
		m_ParentMainWindow->SetStatusBarProgress(-1);
	}
}

//-----------------------------------------------------------------------------

PlusStatus PhantomRegistrationToolbox::Start()
{
	LOG_TRACE("PhantomRegistrationToolbox::Start"); 

  if (m_PhantomRegistration->GetDefinedLandmarks()->GetNumberOfPoints() < 4) {
    LOG_ERROR("Not enough (" << m_PhantomRegistration->GetDefinedLandmarks()->GetNumberOfPoints() << ") defined landmarks (should be at least 4)!");
    return PLUS_FAIL;
  }

  StylusCalibrationToolbox* stylusCalibrationToolbox = dynamic_cast<StylusCalibrationToolbox*>(m_ParentMainWindow->GetToolbox(ToolboxType_StylusCalibration));
  if ((stylusCalibrationToolbox != NULL) && (stylusCalibrationToolbox->GetPivotCalibrationAlgo() != NULL))
  {
    vtkPivotCalibrationAlgo* pivotCalibration = stylusCalibrationToolbox->GetPivotCalibrationAlgo();
    if (pivotCalibration->GetTooltipToToolTransform() != NULL)
    {
		  m_CurrentLandmarkIndex = 0;

	    // Initialize input points poly data in visualizer
	    m_ParentMainWindow->GetToolVisualizer()->GetInputPolyData()->GetPoints()->Initialize();
	    m_ParentMainWindow->GetToolVisualizer()->GetInputPolyData()->Modified();

	    // Highlight first landmark
      m_RequestedLandmarkPolyData->GetPoints()->InsertPoint(0, m_PhantomRegistration->GetDefinedLandmarks()->GetPoint(0));
	    m_RequestedLandmarkPolyData->GetPoints()->Modified();

      SetState(ToolboxState_InProgress);
    }
  } else {
    LOG_ERROR("Stylus calibration toolbox not found!");
    return PLUS_FAIL;
  }

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::OpenPhantomDefinition()
{
  LOG_TRACE("PhantomRegistrationToolbox::OpenPhantomDefinition"); 

	// File open dialog for selecting phantom definition xml
	QString filter = QString( tr( "XML files ( *.xml );;" ) );
	QString fileName = QFileDialog::getOpenFileName(NULL, QString( tr( "Open phantom descriptor XML" ) ), vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory(), filter);
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

	// Load phantom definition xml
  if (m_PhantomRegistration->ReadConfiguration(rootElement) == PLUS_SUCCESS) {
		ui.lineEdit_PhantomDefinition->setText(fileName);
		ui.lineEdit_PhantomDefinition->setToolTip(fileName);

    // Replace PhantomDefinition element with the one in the just read file
    vtkPlusConfig::ReplaceElementInDeviceSetConfiguration("PhantomDefinition", rootElement);

		// Set to InProgress if both stylus calibration and phantom definition are available
		Start();

	} else {
		ui.lineEdit_PhantomDefinition->setText(tr("Invalid file!"));
		ui.lineEdit_PhantomDefinition->setToolTip("");
	}
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::OpenStylusCalibration()
{
  LOG_TRACE("PhantomRegistrationToolbox::OpenStylusCalibration");

	// File open dialog for selecting phantom definition xml
	QString filter = QString( tr( "XML files ( *.xml );;" ) );
	QString fileName = QFileDialog::getOpenFileName(NULL, QString( tr( "Open stylus calibration XML" ) ), vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory(), filter);
	if (fileName.isNull()) {
		return;
	}

  StylusCalibrationToolbox* stylusCalibrationToolbox = dynamic_cast<StylusCalibrationToolbox*>(m_ParentMainWindow->GetToolbox(ToolboxType_StylusCalibration));
  if ((stylusCalibrationToolbox != NULL) && (stylusCalibrationToolbox->GetPivotCalibrationAlgo() != NULL))
  {
    vtkPivotCalibrationAlgo* pivotCalibration = stylusCalibrationToolbox->GetPivotCalibrationAlgo();

    if (pivotCalibration->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData(), TRACKER_TOOL_STYLUS) == PLUS_SUCCESS)
    {
		  ui.lineEdit_StylusCalibration->setText(fileName);
		  ui.lineEdit_StylusCalibration->setToolTip(fileName);

 		  // Set to InProgress if both stylus calibration and phantom definition are available
      Start();
    }
    else
    {
		  ui.lineEdit_StylusCalibration->setText(tr("Invalid file!"));
		  ui.lineEdit_StylusCalibration->setToolTip("");
    }
  }
  else
  {
    LOG_ERROR("Stylus calibration toolbox not found!");
    return;
  }
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::RecordPoint()
{
  LOG_TRACE("PhantomRegistrationToolbox::RecordPoint"); 

	// If tracker is FakeTracker then set counter (trigger position change) and wait for it to apply the new position
	vtkFakeTracker *fakeTracker = dynamic_cast<vtkFakeTracker*>(m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetTracker());
	if (fakeTracker != NULL)
  {
		fakeTracker->SetCounter(m_CurrentLandmarkIndex);
		vtkAccurateTimer::Delay(1.1 / fakeTracker->GetFrequency());
	}

  // Acquire point and add to registration algorithm
  vtkSmartPointer<vtkMatrix4x4> stylusTipToReferenceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  if (m_ParentMainWindow->GetToolVisualizer()->AcquireTrackerPositionForToolByType(TRACKER_TOOL_STYLUS, stylusTipToReferenceMatrix, true) == TR_OK)
  {
		double elements[16]; //TODO find other way
		double stylusTipPosition[4];
		for (int i=0; i<4; ++i) for (int j=0; j<4; ++j) elements[4*j+i] = stylusTipToReferenceMatrix->GetElement(i,j);
		double origin[4] = {0.0, 0.0, 0.0, 1.0};
		vtkMatrix4x4::PointMultiply(elements, origin, stylusTipPosition);

		// Add recorded point to algorithm
    m_PhantomRegistration->GetRecordedLandmarks()->InsertPoint(m_CurrentLandmarkIndex, stylusTipPosition[0], stylusTipPosition[1], stylusTipPosition[2]);
		m_PhantomRegistration->GetRecordedLandmarks()->Modified();

		// Add recorded point to visualization
    m_ParentMainWindow->GetToolVisualizer()->GetInputPolyData()->GetPoints()->InsertPoint(m_CurrentLandmarkIndex, stylusTipPosition[0], stylusTipPosition[1], stylusTipPosition[2]);
    m_ParentMainWindow->GetToolVisualizer()->GetInputPolyData()->Modified();

		// Set new current landmark number and reset request flag
		++m_CurrentLandmarkIndex;

    // If there are at least 3 acuired points then register
    if (m_CurrentLandmarkIndex >= 3)
    {
      if (m_PhantomRegistration->Register() == PLUS_SUCCESS)
      {
        // Set result for visualization
        m_ParentMainWindow->GetToolVisualizer()->SetPhantomToPhantomReferenceTransform(m_PhantomRegistration->GetPhantomToPhantomReferenceTransform());
        m_ParentMainWindow->GetToolVisualizer()->ShowTool(TRACKER_TOOL_REFERENCE, true);
      } else {
        LOG_ERROR("Phantom registration failed!");
      }
    }

		// If it was the last landmark then write configuration, set status to done and reset landmark counter
    if (m_CurrentLandmarkIndex == m_PhantomRegistration->GetDefinedLandmarks()->GetNumberOfPoints())
    {
      if (m_PhantomRegistration->WriteConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS)
      {
        LOG_ERROR("Unable to save phantom registration result in configuration XML tree!");
        SetState(ToolboxState_Error);
        return;
      }
      else
      {
        SetState(ToolboxState_Done);
      }

	    m_RequestedLandmarkPolyData->GetPoints()->GetData()->RemoveTuple(0);
	    m_RequestedLandmarkPolyData->GetPoints()->Modified();
		}
    else
    {
			// Highlight next landmark
			m_RequestedLandmarkPolyData->GetPoints()->InsertPoint(0, m_PhantomRegistration->GetDefinedLandmarks()->GetPoint(m_CurrentLandmarkIndex));
			m_RequestedLandmarkPolyData->GetPoints()->Modified();
		}

		// Reset camera after each recording
		m_ParentMainWindow->GetToolVisualizer()->GetCanvasRenderer()->ResetCamera();
  }
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::Undo()
{
	LOG_TRACE("PhantomRegistrationToolbox::Undo"); 

	if (m_State == ToolboxState_Done) {
		SetState(ToolboxState_InProgress);
	}

	if (m_CurrentLandmarkIndex > 0)
  {
		// Decrease current landmark index
		--m_CurrentLandmarkIndex;

		// Reset result transform (in case Undo was pressed when the registration was ready)
		m_PhantomRegistration->SetPhantomToPhantomReferenceTransform(NULL);

		// Delete previously acquired landmark
		m_ParentMainWindow->GetToolVisualizer()->GetInputPolyData()->GetPoints()->GetData()->RemoveTuple(m_CurrentLandmarkIndex);
		m_ParentMainWindow->GetToolVisualizer()->GetInputPolyData()->Modified();

		// Highlight previous landmark
		m_RequestedLandmarkPolyData->GetPoints()->InsertPoint(0, m_PhantomRegistration->GetDefinedLandmarks()->GetPoint(m_CurrentLandmarkIndex));
		m_RequestedLandmarkPolyData->GetPoints()->Modified();

    // Hide phantom from main canvas
    m_ParentMainWindow->GetToolVisualizer()->ShowTool(TRACKER_TOOL_REFERENCE, false);
  }

	// If tracker is FakeTracker then set counter
	vtkFakeTracker *fakeTracker = dynamic_cast<vtkFakeTracker*>(m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetTracker());
	if (fakeTracker != NULL) {
		fakeTracker->SetCounter(m_CurrentLandmarkIndex);
	}
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::Reset()
{
	LOG_TRACE("PhantomRegistrationToolbox::Reset"); 

	if (m_State == ToolboxState_Done) {
		SetState(ToolboxState_InProgress);
	}

	// Delete acquired landmarks
	vtkSmartPointer<vtkPoints> landmarkPoints = vtkSmartPointer<vtkPoints>::New();
	m_ParentMainWindow->GetToolVisualizer()->GetInputPolyData()->SetPoints(landmarkPoints);
	m_ParentMainWindow->GetToolVisualizer()->GetInputPolyData()->Modified();

	// Reset current landmark index
	m_CurrentLandmarkIndex = 0;

	// Reset result transform (if Reset was pressed when the registration was ready we have to make it null)
	m_PhantomRegistration->SetPhantomToPhantomReferenceTransform(NULL);

	// Highlight first landmark
  if ((m_State != ToolboxState_Uninitialized) && (m_PhantomRegistration->GetDefinedLandmarks()->GetNumberOfPoints() > 0)) {
	  m_RequestedLandmarkPolyData->GetPoints()->InsertPoint(0, m_PhantomRegistration->GetDefinedLandmarks()->GetPoint(0));
	  m_RequestedLandmarkPolyData->GetPoints()->Modified();
  }

  // Hide phantom from main canvas
  m_ParentMainWindow->GetToolVisualizer()->ShowTool(TRACKER_TOOL_REFERENCE, false);

	// If tracker is FakeTracker then reset counter
	vtkFakeTracker *fakeTracker = dynamic_cast<vtkFakeTracker*>(m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetTracker());
	if (fakeTracker != NULL) {
		fakeTracker->SetCounter(m_CurrentLandmarkIndex);
	}
}

//-----------------------------------------------------------------------------

void PhantomRegistrationToolbox::Save()
{
  LOG_TRACE("PhantomRegistrationToolbox::Save"); 

  ConfigFileSaverDialog* configSaverDialog = new ConfigFileSaverDialog(this);
  configSaverDialog->exec();

  delete configSaverDialog;
}
