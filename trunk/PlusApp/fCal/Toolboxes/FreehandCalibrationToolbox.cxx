#include "FreehandCalibrationToolbox.h"

#include "vtkCalibrationController.h"
#include "vtkPhantomRegistrationAlgo.h"

#include "PhantomRegistrationToolbox.h"
#include "ConfigFileSaverDialog.h"
#include "SegmentationParameterDialog.h"

#include "fCalMainWindow.h"
#include "vtkToolVisualizer.h"

#include "vtkXMLUtilities.h"

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

	  if (m_State == ToolboxState_Uninitialized) {
		  SetState(ToolboxState_Idle);
	  }

	  if (m_State != ToolboxState_Done) {
      m_ParentMainWindow->GetToolVisualizer()->GetResultPointsPolyData()->GetPoints()->Reset();
    }
  }
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
  m_ParentMainWindow->GetToolVisualizer()->EnableImageMode(! ui.checkBox_ShowDevices->isChecked());

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
		ui.label_Results->setText(QString::fromStdString(m_Calibration->GetResultString()));

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
	if (m_Calibration->ReadConfiguration(rootElement) != PLUS_SUCCESS) {
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

  // Do the calibration
  //m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->SetProgressBarUpdateCallbackFunction(UpdateProgress);
  m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->Synchronize(vtkPlusConfig::GetInstance()->GetOutputDirectory(), true );

	//this->ProgressPercent = 0;

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

	m_ParentMainWindow->SetTabsEnabled(false);

	QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

  PhantomRegistrationToolbox* phantomRegistrationToolbox = dynamic_cast<PhantomRegistrationToolbox*>(m_ParentMainWindow->GetToolbox(ToolboxType_PhantomRegistration));
  if ((phantomRegistrationToolbox != NULL) && (phantomRegistrationToolbox->GetPhantomRegistrationAlgo() != NULL)) {
    m_Calibration->Initialize();
    m_Calibration->RegisterPhantomGeometry(phantomRegistrationToolbox->GetPhantomRegistrationAlgo()->GetPhantomToPhantomReferenceTransform());

  } else {
    LOG_ERROR("Phantom registration toolbox or algorithm is not initialized!");
    return;
  }

  SetState(ToolboxState_InProgress);

  // Start calibration and compute results on success
	if ((DoSpatialCalibration() == PLUS_SUCCESS) && (m_Calibration->ComputeCalibrationResults() == PLUS_SUCCESS)) {

    // Set result for visualization
    m_ParentMainWindow->GetToolVisualizer()->SetImageToProbeTransform(m_Calibration->GetTransformUserImageToProbe());

    m_SpatialCalibrationDone = true;

    SetState(ToolboxState_Done);
	}

	m_ParentMainWindow->SetTabsEnabled(true);

  SetDisplayAccordingToState();
}

//-----------------------------------------------------------------------------

PlusStatus FreehandCalibrationToolbox::DoSpatialCalibration()
{
	LOG_TRACE("FreehandCalibrationToolbox::DoSpatialCalibration");

	const int maxNumberOfValidationImages = m_Calibration->GetImageDataInfo(FREEHAND_MOTION_2).NumberOfImagesToAcquire; 
	const int maxNumberOfCalibrationImages = m_Calibration->GetImageDataInfo(FREEHAND_MOTION_1).NumberOfImagesToAcquire; 
	int numberOfAcquiredImages = 0;
	int numberOfFailedSegmentations = 0;

	m_CancelRequest = false;

	// Acquire and add validation and calibration data
	while ((m_Calibration->GetImageDataInfo(FREEHAND_MOTION_2).NumberOfSegmentedImages < maxNumberOfValidationImages)
		|| (m_Calibration->GetImageDataInfo(FREEHAND_MOTION_1).NumberOfSegmentedImages < maxNumberOfCalibrationImages)) {

		bool segmentationSuccessful = false;

		if (m_CancelRequest) {
			// Cancel the job
			return PLUS_FAIL;
		}

		// Get latest tracked frame from data collector
		TrackedFrame trackedFrame; 
		m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetTrackedFrame(&trackedFrame); 

		if (trackedFrame.GetStatus() & (TR_MISSING | TR_OUT_OF_VIEW)) {
			LOG_DEBUG("Tracker out of view"); 
		} else if (trackedFrame.GetStatus() & (TR_REQ_TIMEOUT)) {
			LOG_DEBUG("Tracker request timeout"); 
		} else { // TR_OK
			if (numberOfAcquiredImages < maxNumberOfValidationImages) {
				// Validation data
				if ( m_Calibration->GetTrackedFrameList(FREEHAND_MOTION_2)->ValidateData(&trackedFrame) ) {
					if (m_Calibration->AddTrackedFrameData(&trackedFrame, FREEHAND_MOTION_2)) {
						segmentationSuccessful = true;
					} else {
						++numberOfFailedSegmentations;
						LOG_DEBUG("Adding tracked frame " << m_Calibration->GetImageDataInfo(FREEHAND_MOTION_2).NumberOfSegmentedImages << " (for validation) failed!");
					}
				}
			} else {
				// Calibration data
				if ( m_Calibration->GetTrackedFrameList(FREEHAND_MOTION_1)->ValidateData(&trackedFrame) ) {
					if (m_Calibration->AddTrackedFrameData(&trackedFrame, FREEHAND_MOTION_1)) {
						segmentationSuccessful = true;
					} else {
						++numberOfFailedSegmentations;
						LOG_DEBUG("Adding tracked frame " << m_Calibration->GetImageDataInfo(FREEHAND_MOTION_1).NumberOfSegmentedImages << " (for calibration) failed!");
					}
				}
			}
		}

		// Update progress if tracked frame has been successfully added
		if (segmentationSuccessful) {
			++numberOfAcquiredImages;

      int progressPercent = (int)((numberOfAcquiredImages / (double)(maxNumberOfValidationImages + maxNumberOfCalibrationImages)) * 100.0);
			m_ParentMainWindow->SetStatusBarProgress(progressPercent);
		}

		// Display segmented points (or hide them if unsuccessful)
		DisplaySegmentedPoints(segmentationSuccessful);

    QApplication::processEvents();
	}

	LOG_INFO("Segmentation success rate: " << numberOfAcquiredImages << " out of " << numberOfAcquiredImages + numberOfFailedSegmentations << " (" << (int)(((double)numberOfAcquiredImages / (double)(numberOfAcquiredImages + numberOfFailedSegmentations)) * 100.0 + 0.49) << " percent)");

	return PLUS_SUCCESS;
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
  m_Calibration->ResetFreehandCalibration();

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

PlusStatus FreehandCalibrationToolbox::DisplaySegmentedPoints(bool aSuccess)
{
	LOG_TRACE("vtkFreehandCalibrationController::DisplaySegmentedPoints(" << (aSuccess?"true":"false") << ")");

  if (! aSuccess) {
    m_ParentMainWindow->GetToolVisualizer()->ShowResultPoints(false);
    return PLUS_SUCCESS;
  }

  vtkPoints* points = m_ParentMainWindow->GetToolVisualizer()->GetResultPointsPolyData()->GetPoints();
  points->Reset();

	// Get last results and feed the points into vtkPolyData for displaying
  SegmentedFrame lastSegmentedFrame = this->m_Calibration->GetSegmentedFrameContainer().at(this->m_Calibration->GetSegmentedFrameContainer().size() - 1);
	int height = lastSegmentedFrame.TrackedFrameInfo->GetFrameSize()[1];

	vtkSmartPointer<vtkPoints> segmentedPoints = vtkSmartPointer<vtkPoints>::New();
  segmentedPoints->SetNumberOfPoints(this->m_Calibration->GetPatternRecognition()->GetFidLabeling()->GetFoundDotsCoordinateValue().size());

	std::vector<std::vector<double>> dots = this->m_Calibration->GetPatternRecognition()->GetFidLabeling()->GetFoundDotsCoordinateValue();
	for (int i=0; i<dots.size(); ++i) {
		segmentedPoints->InsertPoint(i, dots[i][0], height - dots[i][1], 0.0);
	}
	segmentedPoints->Modified();

  m_ParentMainWindow->GetToolVisualizer()->GetResultPointsPolyData()->SetPoints(segmentedPoints);

  m_ParentMainWindow->GetToolVisualizer()->ShowResultPoints(true);

	return PLUS_SUCCESS;
}
