/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "FreehandCalibrationToolbox.h"

#include "vtkCalibrationController.h"
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
{
	ui.setupUi(this);

  m_CancelRequest = false;

	// Create algorithm
  m_Calibration = vtkCalibrationController::New();

	ui.label_SpatialCalibration->setFont(QFont("SansSerif", 9, QFont::Bold));
	ui.label_TemporalCalibration->setFont(QFont("SansSerif", 9, QFont::Bold));

	//ui.label_Results->setFont(QFont("Courier", 7));

	// Connect events
	connect( ui.pushButton_OpenPhantomRegistration, SIGNAL( clicked() ), this, SLOT( OpenPhantomRegistration() ) );
	connect( ui.pushButton_OpenCalibrationConfiguration, SIGNAL( clicked() ), this, SLOT( OpenCalibrationConfiguration() ) );
	connect( ui.pushButton_EditCalibrationConfiguration, SIGNAL( clicked() ), this, SLOT( EditCalibrationConfiguration() ) );
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

  if ((m_ParentMainWindow->GetToolVisualizer()->GetDataCollector() != NULL) && (m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetConnected())) {

		m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->SetTrackingOnly(false);

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
    if ( (m_Calibration->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) == PLUS_SUCCESS)
       && (m_Calibration->ReadFreehandCalibrationConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) == PLUS_SUCCESS) )
    {
      ui.lineEdit_CalibrationConfiguration->setText(tr("Using session calibration configuration"));
    }

    // Load calibration matrix into tool visualizer if it exists
    if ((IsReadyToStartSpatialCalibration()) && (m_Calibration->GetCalibrationDate() != NULL)) {
      m_ParentMainWindow->GetToolVisualizer()->GetDisplayableTool(TRACKER_TOOL_PROBE)->DisplayableOn();
      m_ParentMainWindow->GetToolVisualizer()->SetImageToProbeTransform(m_Calibration->GetTransformUserImageToProbe());
    }

    // Set initialized if it was uninitialized
	  if (m_State == ToolboxState_Uninitialized) {
		  SetState(ToolboxState_Idle);
	  }

	  if (m_State != ToolboxState_Done) {
      m_ParentMainWindow->GetToolVisualizer()->GetResultPolyData()->Initialize();
    }
  }
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

	// If initialization failed
	if (m_State == ToolboxState_Uninitialized) {
		ui.label_InstructionsTemporal->setText(tr(""));
		ui.pushButton_StartTemporal->setEnabled(false);
    ui.pushButton_CancelTemporal->setEnabled(false);

		ui.label_InstructionsSpatial->setText(tr(""));

		ui.checkBox_ShowDevices->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);

		m_ParentMainWindow->SetStatusBarText(QString(""));
		m_ParentMainWindow->SetStatusBarProgress(-1);

	} else
	// If initialized
	if (m_State == ToolboxState_Idle) {
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

	} else
	// If in progress
	if (m_State == ToolboxState_InProgress) {
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

  } else
	// If done
	if (m_State == ToolboxState_Done) {
		ui.label_InstructionsTemporal->setText(tr("Temporal calibration is ready to save\n(video time offset: %1 ms)").arg(videoTimeOffset));
		ui.pushButton_StartTemporal->setEnabled(true);
		ui.pushButton_CancelSpatial->setEnabled(false);

		ui.label_InstructionsSpatial->setText(tr("Spatial calibration is ready to save"));
		ui.pushButton_StartSpatial->setEnabled(true);
		ui.pushButton_CancelSpatial->setEnabled(false);

		ui.checkBox_ShowDevices->setEnabled(true);
		ui.pushButton_Save->setEnabled(true);
		ui.label_Results->setText(QString::fromStdString(m_Calibration->GetResultString()));

		ui.pushButton_Save->setFocus();

		m_ParentMainWindow->SetStatusBarText(QString(" Calibration done"));
		m_ParentMainWindow->SetStatusBarProgress(-1);

    QApplication::restoreOverrideCursor();

	} else
	// If error occured
	if (m_State == ToolboxState_Error) {
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
  vtkSmartPointer<vtkXMLDataElement> rootElement = vtkXMLUtilities::ReadElementFromFile(fileName.toStdString().c_str());
	if (rootElement == NULL) {	
		LOG_ERROR("Unable to read the configuration file: " << fileName.toStdString()); 
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
	if ( (m_Calibration->ReadConfiguration(rootElement) != PLUS_SUCCESS)
    || (m_Calibration->ReadFreehandCalibrationConfiguration(rootElement) != PLUS_SUCCESS) )
  {
		ui.lineEdit_CalibrationConfiguration->setText(tr("Invalid file!"));
		ui.lineEdit_CalibrationConfiguration->setToolTip("");

		LOG_ERROR("Calibration configuration file " << fileName.toStdString().c_str() << " cannot be loaded!");
		return;
	}

  // Replace USCalibration element with the one in the just read file
  vtkPlusConfig::ReplaceElementInDeviceSetConfiguration("USCalibration", rootElement);

	// Re-calculate camera parameters
  m_ParentMainWindow->GetToolVisualizer()->CalculateImageCameraParameters();

	ui.lineEdit_CalibrationConfiguration->setText(fileName);
	ui.lineEdit_CalibrationConfiguration->setToolTip(fileName);

  SetDisplayAccordingToState();
}

//-----------------------------------------------------------------------------

void FreehandCalibrationToolbox::EditCalibrationConfiguration()
{
  LOG_TRACE("FreehandCalibrationToolbox::EditCalibrationConfiguration");

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

  // Reset calibration
  m_Calibration->ResetFreehandCalibration();

  // Create tracked frame lists
	vtkTrackedFrameList* validationTrackedFrameList = vtkTrackedFrameList::New();
  validationTrackedFrameList->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData());

	vtkTrackedFrameList* calibrationTrackedFrameList = vtkTrackedFrameList::New();
  calibrationTrackedFrameList->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData());

  const int maxNumberOfValidationImages = m_Calibration->GetImageDataInfo(FREEHAND_MOTION_2).NumberOfImagesToAcquire; //TODO change these when image data info variables are deleted
	const int maxNumberOfCalibrationImages = m_Calibration->GetImageDataInfo(FREEHAND_MOTION_1).NumberOfImagesToAcquire; 
	int numberOfSegmentedImages = 0;
	int numberOfFailedSegmentations = 0;

  // Create segmenter
  FidPatternRecognition* patternRecognition = new FidPatternRecognition();
	patternRecognition->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData());

  // Reset cancel request flag
	m_CancelRequest = false;

	// Acquire and add validation and calibration data
	while (numberOfSegmentedImages < maxNumberOfValidationImages + maxNumberOfCalibrationImages)
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
      int progressPercent = (int)((numberOfSegmentedImages / (double)(maxNumberOfValidationImages + maxNumberOfCalibrationImages)) * 100.0);
	    m_ParentMainWindow->SetStatusBarProgress(progressPercent);

  		// Display segmented points (or hide them if unsuccessful)
      DisplaySegmentedPoints(&segmentationResults, trackedFrame.GetFrameSize()[1]);

      // Check if segmentation was unsuccessful
      if (trackedFrame.GetFiducialPointsCoordinatePx() == NULL || trackedFrame.GetFiducialPointsCoordinatePx()->GetNumberOfPoints() == 0)
      {
        ++numberOfFailedSegmentations;
        QApplication::processEvents();
        continue;
      }

      // Add tracked frame to the proper list
			if (numberOfSegmentedImages < maxNumberOfValidationImages)
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

  delete patternRecognition;

  if (m_Calibration->Calibrate( validationTrackedFrameList, calibrationTrackedFrameList,
                                m_Calibration->GetTrackedFrameList(FREEHAND_MOTION_1)->GetDefaultFrameTransformName().c_str() ) != PLUS_SUCCESS)  //TODO change these when image data info variables are deleted
  {
    LOG_ERROR("Calibration failed!");
    return PLUS_FAIL;
  }

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

PlusStatus FreehandCalibrationToolbox::DisplaySegmentedPoints(PatternRecognitionResult* aSegmentationResult, int aImageHeight)
{
	LOG_TRACE("vtkFreehandCalibrationController::DisplaySegmentedPoints");

  if (aSegmentationResult->GetDotsFound() == false) {
    m_ParentMainWindow->GetToolVisualizer()->ShowResult(false);
    return PLUS_SUCCESS;
  }

	// Get last results and feed the points into vtkPolyData for displaying
	vtkSmartPointer<vtkPoints> segmentedPoints = vtkSmartPointer<vtkPoints>::New();
  segmentedPoints->SetNumberOfPoints(aSegmentationResult->GetFoundDotsCoordinateValue().size());

	std::vector<std::vector<double>> dots = aSegmentationResult->GetFoundDotsCoordinateValue();
	for (int i=0; i<dots.size(); ++i) {
		segmentedPoints->InsertPoint(i, dots[i][0], aImageHeight - dots[i][1], 0.0);
	}
	segmentedPoints->Modified();

  m_ParentMainWindow->GetToolVisualizer()->GetResultPolyData()->SetPoints(segmentedPoints);

  m_ParentMainWindow->GetToolVisualizer()->ShowResult(true);

	return PLUS_SUCCESS;
}
