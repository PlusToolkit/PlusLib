/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "TemporalCalibrationToolbox.h"

#include "TemporalCalibrationAlgo.h"
#include "TrackedFrame.h"
#include "fCalMainWindow.h"

#include "vtkTrackedFrameList.h"
#include "vtkVisualizationController.h"
#include "vtkPlusVideoSource.h"
#include "vtkTracker.h"
#include "vtkTrackerTool.h"
#include "vtkPlusDataBuffer.h"

#include <QFileDialog>
#include <QTimer>

#include <vtkRenderWindow.h>
#include <vtkChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkPlot.h>
#include <vtkTable.h>
#include <vtkXMLUtilities.h>

//-----------------------------------------------------------------------------

TemporalCalibrationToolbox::TemporalCalibrationToolbox(fCalMainWindow* aParentMainWindow, Qt::WFlags aFlags)
: AbstractToolbox(aParentMainWindow)
, QWidget(aParentMainWindow, aFlags)
, m_CancelRequest(false)
, m_LastRecordedTrackingItemTimestamp(0.0)
, m_LastRecordedVideoItemTimestamp(0.0)
, m_RecordingIntervalMs(200)
, m_TemporalCalibrationDurationSec(10)
, m_StartTimeSec(0.0)
, m_PreviousTrackerOffset(-1.0)
, m_PreviousVideoOffset(-1.0)
, m_TemporalCalibrationPlotsWindow(NULL)
, m_UncalibratedPlotContextView(NULL)
, m_CalibratedPlotContextView(NULL)
{
  ui.setupUi(this);

  // Create tracked frame lists
  m_TemporalCalibrationTrackingData = vtkTrackedFrameList::New();
  m_TemporalCalibrationTrackingData->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP | REQUIRE_TRACKING_OK); 

  m_TemporalCalibrationVideoData = vtkTrackedFrameList::New();
  m_TemporalCalibrationVideoData->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP); 

  // Create temporal calibration metric tables
  m_VideoPositionMetric = vtkTable::New();
  m_UncalibratedTrackerPositionMetric = vtkTable::New();
  m_CalibratedTrackerPositionMetric = vtkTable::New();

  // Connect events
  connect( ui.pushButton_StartTemporal, SIGNAL( clicked() ), this, SLOT( StartCalibration() ) );
  connect( ui.pushButton_CancelTemporal, SIGNAL( clicked() ), this, SLOT( CancelCalibration() ) );
  connect( ui.pushButton_ShowPlots, SIGNAL( toggled(bool) ), this, SLOT( ShowPlotsToggled(bool) ) );
}

//-----------------------------------------------------------------------------

TemporalCalibrationToolbox::~TemporalCalibrationToolbox()
{
  if (m_TemporalCalibrationTrackingData != NULL)
  {
    m_TemporalCalibrationTrackingData->Delete();
    m_TemporalCalibrationTrackingData = NULL;
  } 

  if (m_TemporalCalibrationVideoData != NULL)
  {
    m_TemporalCalibrationVideoData->Delete();
    m_TemporalCalibrationVideoData = NULL;
  } 

  if (m_VideoPositionMetric != NULL)
  {
    m_VideoPositionMetric->Delete();
    m_VideoPositionMetric = NULL;
  } 

  if (m_UncalibratedTrackerPositionMetric != NULL)
  {
    m_UncalibratedTrackerPositionMetric->Delete();
    m_UncalibratedTrackerPositionMetric = NULL;
  } 

  if (m_CalibratedTrackerPositionMetric != NULL)
  {
    m_CalibratedTrackerPositionMetric->Delete();
    m_CalibratedTrackerPositionMetric = NULL;
  } 

  if (m_TemporalCalibrationPlotsWindow != NULL)
  {
    delete m_TemporalCalibrationPlotsWindow;
    m_TemporalCalibrationPlotsWindow = NULL;
  } 

  if (m_UncalibratedPlotContextView != NULL)
  {
    m_UncalibratedPlotContextView->Delete();
    m_UncalibratedPlotContextView = NULL;
  } 

  if (m_CalibratedPlotContextView != NULL)
  {
    m_CalibratedPlotContextView->Delete();
    m_CalibratedPlotContextView = NULL;
  } 
}

//-----------------------------------------------------------------------------

void TemporalCalibrationToolbox::OnActivated()
{
  LOG_TRACE("TemporalCalibrationToolbox::OnActivated"); 

  if (m_State == ToolboxState_Done)
  {
    SetDisplayAccordingToState();
    return;
  }

  if ( (m_ParentMainWindow->GetVisualizationController()->GetDataCollector() != NULL)
    && (m_ParentMainWindow->GetVisualizationController()->GetDataCollector()->GetConnected()))
  {
    // Read temporal calibration configuration
    if (ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Reading temporal calibration configuration failed!");
      return;
    }

    // Set initialized if it was uninitialized
    if (m_State == ToolboxState_Uninitialized || m_State == ToolboxState_Error)
    {
      SetState(ToolboxState_Idle);
    }
    else
    {
      SetDisplayAccordingToState();
    }
  }
  else
  {
    SetState(ToolboxState_Uninitialized);
  }
}

//-----------------------------------------------------------------------------

PlusStatus TemporalCalibrationToolbox::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  LOG_TRACE("TemporalCalibrationToolbox::ReadConfiguration");

  if (aConfig == NULL)
  {
    LOG_ERROR("Unable to read configuration"); 
    return PLUS_FAIL; 
  }

  vtkXMLDataElement* fCalElement = aConfig->FindNestedElementWithName("fCal");

  if (fCalElement == NULL)
  {
    LOG_ERROR("Unable to find fCal element in XML tree!"); 
    return PLUS_FAIL;     
  }

  // Recording interval and processing time
  int recordingIntervalMs = 0; 
  if ( fCalElement->GetScalarAttribute("RecordingIntervalMs", recordingIntervalMs ) )
  {
    m_RecordingIntervalMs = recordingIntervalMs;
  }
  else
  {
    LOG_WARNING("Unable to read RecordingIntervalMs attribute from fCal element of the device set configuration, default value '" << m_RecordingIntervalMs << "' will be used");
  }

  // Duration of temporal calibration
  int temporalCalibrationDurationSec = 0; 
  if ( fCalElement->GetScalarAttribute("TemporalCalibrationDurationSec", temporalCalibrationDurationSec ) )
  {
    m_TemporalCalibrationDurationSec = temporalCalibrationDurationSec;
  }
  else
  {
    LOG_WARNING("Unable to read TemporalCalibrationDurationSec attribute from fCal element of the device set configuration, default value '" << m_TemporalCalibrationDurationSec << "' will be used");
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void TemporalCalibrationToolbox::RefreshContent()
{
}

//-----------------------------------------------------------------------------

void TemporalCalibrationToolbox::SetDisplayAccordingToState()
{
  LOG_TRACE("TemporalCalibrationToolbox::SetDisplayAccordingToState");

  double videoTimeOffset = 0.0;

  // If connected
  if ( (m_ParentMainWindow->GetVisualizationController()->GetDataCollector() != NULL)
    && (m_ParentMainWindow->GetVisualizationController()->GetDataCollector()->GetConnected()) )
  {
    // If the force show devices isn't enabled, set it to 2D
    if( !m_ParentMainWindow->IsForceShowDevicesEnabled() )
    {
      m_ParentMainWindow->GetVisualizationController()->HideAll();
      // 2D mode auto-turns back on the image
      m_ParentMainWindow->GetVisualizationController()->SetVisualizationMode(vtkVisualizationController::DISPLAY_MODE_2D);
    }

    // Enable or disable the image manipulation menu
    m_ParentMainWindow->SetImageManipulationMenuEnabled( m_ParentMainWindow->GetVisualizationController()->Is2DMode() );

    if (m_ParentMainWindow->GetVisualizationController()->GetDataCollector() != NULL)
    {
      vtkDataCollector* dataCollector = m_ParentMainWindow->GetVisualizationController()->GetDataCollector();
      if ( dataCollector )
      {
        if ( (dataCollector->GetVideoSource() != NULL)
          && (dataCollector->GetVideoSource()->GetBuffer() != NULL))
        {
          videoTimeOffset = dataCollector->GetVideoSource()->GetBuffer()->GetLocalTimeOffsetSec();
        }
      }
    }

    // Update state message
    ui.label_State->setText(tr("Current video time offset: %1 s").arg(videoTimeOffset));
  }

  // Set widget states according to state
  if (m_State == ToolboxState_Uninitialized)
  {
    ui.label_InstructionsTemporal->setText(QString(""));
    ui.pushButton_StartTemporal->setEnabled(false);
    ui.pushButton_CancelTemporal->setEnabled(false);
    ui.pushButton_ShowPlots->setEnabled(false);

    m_ParentMainWindow->SetStatusBarText(QString(""));
    m_ParentMainWindow->SetStatusBarProgress(-1);
  }
  else if (m_State == ToolboxState_Idle)
  {
    ui.label_InstructionsTemporal->setText(tr("Move probe to vertical position in the water tank so that the bottom is visible and press Start"));
    ui.pushButton_StartTemporal->setEnabled(true);
    ui.pushButton_CancelTemporal->setEnabled(false);
    ui.pushButton_ShowPlots->setEnabled(false);

    m_ParentMainWindow->SetStatusBarText(QString(""));
    m_ParentMainWindow->SetStatusBarProgress(-1);

    QApplication::restoreOverrideCursor();
  }
  else if (m_State == ToolboxState_InProgress)
  {
    m_ParentMainWindow->SetStatusBarText(QString(" Acquiring and adding images to calibrator"));
    m_ParentMainWindow->SetStatusBarProgress(0);

    ui.label_InstructionsTemporal->setText(tr("Move probe up and down so that the tank bottom is visible with 2s period until the progress bar is filled"));
    ui.pushButton_StartTemporal->setEnabled(false);
    ui.pushButton_CancelTemporal->setEnabled(true);
    ui.pushButton_CancelTemporal->setFocus();
  }
  else if (m_State == ToolboxState_Done)
  {
    ui.label_InstructionsTemporal->setText(tr("Temporal calibration is ready to save and its result plots can be viewed\n(Video time offset: %1 s)").arg(videoTimeOffset));
    ui.pushButton_ShowPlots->setEnabled(true);

    ui.pushButton_StartTemporal->setEnabled(true);
    ui.pushButton_CancelTemporal->setEnabled(false);

    m_ParentMainWindow->SetStatusBarText(QString(" Calibration done"));
    m_ParentMainWindow->SetStatusBarProgress(-1);

    QApplication::restoreOverrideCursor();
  }
  else if (m_State == ToolboxState_Error)
  {
    ui.label_InstructionsTemporal->setText(tr("Error occurred!"));
    ui.pushButton_StartTemporal->setEnabled(false);
    ui.pushButton_CancelTemporal->setEnabled(false);
    ui.pushButton_ShowPlots->setEnabled(false);

    m_ParentMainWindow->SetStatusBarText(QString(""));
    m_ParentMainWindow->SetStatusBarProgress(-1);

    QApplication::restoreOverrideCursor();
  }
}

//-----------------------------------------------------------------------------

void TemporalCalibrationToolbox::StartCalibration()
{
  LOG_INFO("Temporal calibration started");

  m_ParentMainWindow->SetToolboxesEnabled(false);

  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

  // Set validation transform names for tracked frame list
  std::string toolReferenceFrame;
  if ( (m_ParentMainWindow->GetVisualizationController()->GetDataCollector() == NULL)
    || (m_ParentMainWindow->GetVisualizationController()->GetDataCollector()->GetTrackerToolReferenceFrame(toolReferenceFrame) != PLUS_SUCCESS) )
  {
    LOG_ERROR("Failed to get tool reference frame name!");
    return;
  }
  PlusTransformName transformNameForValidation(m_ParentMainWindow->GetProbeCoordinateFrame(), toolReferenceFrame.c_str());
  m_TemporalCalibrationTrackingData->SetFrameTransformNameForValidation(transformNameForValidation);
  m_TemporalCalibrationVideoData->SetFrameTransformNameForValidation(transformNameForValidation);

  // Set the local time offset to 0 before synchronization
  bool offsetsSuccessfullyRetrieved = false;
  if (m_ParentMainWindow->GetVisualizationController()->GetDataCollector() != NULL)
  {
    vtkDataCollector* dataCollector = m_ParentMainWindow->GetVisualizationController()->GetDataCollector();
    if (dataCollector)
    {
      if (dataCollector->GetTracker() == NULL)
      {
        LOG_ERROR("No tracker device is available");
      }
      else if ( (dataCollector->GetVideoSource() != NULL)
        && (dataCollector->GetVideoSource()->GetBuffer() != NULL))
      {
        m_PreviousTrackerOffset = dataCollector->GetTracker()->GetToolIteratorBegin()->second->GetBuffer()->GetLocalTimeOffsetSec(); 
        m_PreviousVideoOffset = dataCollector->GetVideoSource()->GetBuffer()->GetLocalTimeOffsetSec(); 
        dataCollector->SetLocalTimeOffsetSec(0.0, 0.0); 
        offsetsSuccessfullyRetrieved = true;
      }
    }
  }
  if (!offsetsSuccessfullyRetrieved)
  {
    LOG_ERROR("Tracker and video offset retrieval failed due to problems with data collector or the buffers!");
    CancelCalibration();
    return;
  }

  m_TemporalCalibrationTrackingData->Clear();
  m_TemporalCalibrationVideoData->Clear();

  double currentTimeSec = vtkAccurateTimer::GetSystemTime();
  m_LastRecordedTrackingItemTimestamp = currentTimeSec;
  m_LastRecordedVideoItemTimestamp = currentTimeSec;

  m_StartTimeSec = vtkAccurateTimer::GetSystemTime();
  m_CancelRequest = false;

  SetState(ToolboxState_InProgress);

  // Start calibration and compute results on success
  DoCalibration();
}

//-----------------------------------------------------------------------------
void TemporalCalibrationToolbox::ComputeCalibrationResults()
{
  // Create dialog
  QDialog* temporalCalibrationDialog = new QDialog(this, Qt::Dialog);
  temporalCalibrationDialog->setMinimumSize(QSize(360,80));
  temporalCalibrationDialog->setCaption(tr("fCal"));
  temporalCalibrationDialog->setBackgroundColor(QColor(224, 224, 224));

  QLabel* temporalCalibrationLabel = new QLabel(QString("Computing temporal calibration, please wait..."), temporalCalibrationDialog);
  temporalCalibrationLabel->setFont(QFont("SansSerif", 16));

  QHBoxLayout* layout = new QHBoxLayout();
  layout->addWidget(temporalCalibrationLabel);

  temporalCalibrationDialog->setLayout(layout);
  temporalCalibrationDialog->show();

  ui.label_InstructionsTemporal->setText(tr("Please wait until computing temporal calibration is finished"));
  m_ParentMainWindow->SetStatusBarText(QString(" Computing temporal calibration"));

  QApplication::processEvents();

  // Do the calibration  
  TemporalCalibration temporalCalibrationObject;
  PlusTransformName probeToReferenceTransformName(m_ParentMainWindow->GetProbeCoordinateFrame(), m_ParentMainWindow->GetReferenceCoordinateFrame());
  std::string probeToReferenceTransformNameString;
  probeToReferenceTransformName.GetTransformName(probeToReferenceTransformNameString);
  temporalCalibrationObject.SetTrackerFrames(m_TemporalCalibrationTrackingData, probeToReferenceTransformNameString);
  temporalCalibrationObject.SetVideoFrames(m_TemporalCalibrationVideoData);
  temporalCalibrationObject.SetSamplingResolutionSec(0.001);
  temporalCalibrationObject.SetSaveIntermediateImages(false);

  //  Calculate the time-offset
  TemporalCalibration::TEMPORAL_CALIBRATION_ERROR error = TemporalCalibration::TEMPORAL_CALIBRATION_ERROR_NONE;
  std::string errorStr;
  std::ostringstream strs;
  if (temporalCalibrationObject.Update(error) != PLUS_SUCCESS)
  {
    switch (error)
    {
    case TemporalCalibration::TEMPORAL_CALIBRATION_ERROR_RESULT_ABOVE_THRESHOLD: 
      double correlation;
      temporalCalibrationObject.GetBestCorrelation(correlation);

      strs << "Result above threshold. " << correlation;
      errorStr = strs.str();
      break;
    case TemporalCalibration::TEMPORAL_CALIBRATION_ERROR_INVALID_TRANSFORM_NAME:
      errorStr = "Invalid transform name.";
      break;
    case TemporalCalibration::TEMPORAL_CALIBRATION_ERROR_NO_TIMESTAMPS:
      errorStr = "No timestamps on data.";
      break;
    case TemporalCalibration::TEMPORAL_CALIBRATION_ERROR_UNABLE_NORMALIZE_METRIC:
      errorStr = "Unable to normalize the data.";
      break;
    case TemporalCalibration::TEMPORAL_CALIBRATION_ERROR_CORRELATION_RESULT_EMPTY:
      errorStr = "Correlation list empty. Unable to perform analysis on data.";
      break;
    case TemporalCalibration::TEMPORAL_CALIBRATION_ERROR_NO_VIDEO_DATA:
      errorStr = "Missing video data.";
      break;
    case TemporalCalibration::TEMPORAL_CALIBRATION_ERROR_NOT_MF_ORIENTATION:
      errorStr = "Data not in MF orientation.";
      break;
    case TemporalCalibration::TEMPORAL_CALIBRATION_ERROR_NO_FRAMES_IN_VIDEO_DATA:
      errorStr = "No frames in video data.";
      break;
    case TemporalCalibration::TEMPORAL_CALIBRATION_ERROR_NO_FRAMES_IN_ULTRASOUND_DATA:
      errorStr = "No frames in ultrasound data.";
      break;
    case TemporalCalibration::TEMPORAL_CALIBRATION_ERROR_SAMPLING_RESOLUTION_TOO_SMALL:
      errorStr = "Sampling resolution too small.";
      break;
    }

    LOG_ERROR("Cannot determine tracker lag, temporal calibration failed! Error: " << errorStr);
    CancelCalibration();

    ui.label_State->setPaletteForegroundColor(QColor::fromRgb(255, 0, 0));
    ui.label_State->setText( QString(errorStr.c_str()) );

    temporalCalibrationDialog->done(0);
    temporalCalibrationDialog->hide();
    delete temporalCalibrationDialog;

    return;
  }

  // Get result
  double trackerLagSec = 0;
  if (temporalCalibrationObject.GetTrackerLagSec(trackerLagSec)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Cannot determine tracker lag, temporal calibration failed");
    CancelCalibration();

    temporalCalibrationDialog->done(0);
    temporalCalibrationDialog->hide();
    delete temporalCalibrationDialog;

    return;
  }

  LOG_INFO("Video offset: " << trackerLagSec << " s ( > 0 if the video data lags )");

  // Set the result local time offset
  bool offsetsSuccessfullySet = false;
  if (m_ParentMainWindow->GetVisualizationController()->GetDataCollector() != NULL)
  {
    vtkDataCollector* dataCollector = m_ParentMainWindow->GetVisualizationController()->GetDataCollector();
    if (dataCollector)
    {
      if ( (dataCollector->GetVideoSource() != NULL)
        && (dataCollector->GetVideoSource()->GetBuffer() != NULL))
      {
        dataCollector->SetLocalTimeOffsetSec(trackerLagSec, 0.0); 
        offsetsSuccessfullySet = true;
      }
    }
  }
  if (!offsetsSuccessfullySet)
  {
    LOG_ERROR("Tracker and video offset setting failed due to problems with data collector or the buffers!");
    CancelCalibration();

    temporalCalibrationDialog->done(0);
    temporalCalibrationDialog->hide();
    delete temporalCalibrationDialog;

    return;
  }
  // Save metric tables
  temporalCalibrationObject.GetVideoPositionSignal(m_VideoPositionMetric);
  temporalCalibrationObject.GetUncalibratedTrackerPositionSignal(m_UncalibratedTrackerPositionMetric);
  temporalCalibrationObject.GetCalibratedTrackerPositionSignal(m_CalibratedTrackerPositionMetric);

  m_TemporalCalibrationTrackingData->Clear();
  m_TemporalCalibrationVideoData->Clear();

  SetState(ToolboxState_Done);

  m_ParentMainWindow->SetToolboxesEnabled(true);

  // Close dialog
  temporalCalibrationDialog->done(0);
  temporalCalibrationDialog->hide();
  delete temporalCalibrationDialog;
}


//-----------------------------------------------------------------------------

void TemporalCalibrationToolbox::DoCalibration()
{
  //LOG_TRACE("TemporalCalibrationToolbox::DoTemporalCalibration");

  // Get current time
  double currentTimeSec = vtkAccurateTimer::GetSystemTime();

  if (currentTimeSec - m_StartTimeSec >= m_TemporalCalibrationDurationSec)
  {
    // The prescribed data collection time is up
    ComputeCalibrationResults();
    return;
  }

  // Cancel if requested
  if (m_CancelRequest)
  {
    LOG_INFO("Calibration process cancelled by the user");
    CancelCalibration();
    return;
  }

  int numberOfTrackingFramesBeforeRecording = m_TemporalCalibrationTrackingData->GetNumberOfTrackedFrames();
  int numberOfVideoFramesBeforeRecording = m_TemporalCalibrationVideoData->GetNumberOfTrackedFrames();

  // Acquire tracking frames
  if ( m_ParentMainWindow->GetVisualizationController()->GetDataCollector()->GetTrackingData(m_LastRecordedTrackingItemTimestamp, m_TemporalCalibrationTrackingData) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to tracking data from data collector (last recorded timestamp: " << std::fixed << m_LastRecordedTrackingItemTimestamp << ")"); 
  }
  else
  {
    // Acquire video frames
    if ( m_ParentMainWindow->GetVisualizationController()->GetDataCollector()->GetVideoData(m_LastRecordedVideoItemTimestamp, m_TemporalCalibrationVideoData) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get video data from data collector (last recorded timestamp: " << std::fixed << m_LastRecordedVideoItemTimestamp << ")"); 
    }
    else
    {
      // Update progress
      int progressPercent = (int)((currentTimeSec - m_StartTimeSec) / m_TemporalCalibrationDurationSec * 100.0);
      m_ParentMainWindow->SetStatusBarProgress(progressPercent);
      LOG_DEBUG("Number of tracked frames in the calibration dataset: Tracking: " << std::setw(3) << numberOfTrackingFramesBeforeRecording << " => " << m_TemporalCalibrationTrackingData->GetNumberOfTrackedFrames() << "; Video: " << numberOfVideoFramesBeforeRecording << " => " << m_TemporalCalibrationVideoData->GetNumberOfTrackedFrames());
    }
  }

  QTimer::singleShot(m_RecordingIntervalMs, this, SLOT(DoCalibration())); 
}

//-----------------------------------------------------------------------------

void TemporalCalibrationToolbox::CancelCalibration()
{
  LOG_INFO("Calibration cancelled");

  m_CancelRequest = true;

  // Reset the local time offset to the previous values
  bool offsetsSuccessfullySet = false;
  if (m_ParentMainWindow->GetVisualizationController()->GetDataCollector() != NULL)
  {
    vtkDataCollector* dataCollector = m_ParentMainWindow->GetVisualizationController()->GetDataCollector();
    if (dataCollector)
    {
      if ( (dataCollector->GetVideoSource() != NULL)
        && (dataCollector->GetVideoSource()->GetBuffer() != NULL))
      {
        dataCollector->SetLocalTimeOffsetSec(m_PreviousTrackerOffset, m_PreviousVideoOffset); 
        offsetsSuccessfullySet = true;
      }
    }
  }
  if (!offsetsSuccessfullySet)
  {
    LOG_ERROR("Tracker and video offset setting failed due to problems with data collector or the buffers!");
  }

  m_ParentMainWindow->SetToolboxesEnabled(true);

  SetState(ToolboxState_Idle);
}

//-----------------------------------------------------------------------------

void TemporalCalibrationToolbox::ShowPlotsToggled(bool aOn)
{
  // Delete objects if toggled off, make sure they are deleted when toggled on
  if (m_TemporalCalibrationPlotsWindow != NULL)
  {
    delete m_TemporalCalibrationPlotsWindow;
    m_TemporalCalibrationPlotsWindow = NULL;
  } 

  if (m_UncalibratedPlotContextView != NULL)
  {
    m_UncalibratedPlotContextView->Delete();
    m_UncalibratedPlotContextView = NULL;
  } 

  if (m_CalibratedPlotContextView != NULL)
  {
    m_CalibratedPlotContextView->Delete();
    m_CalibratedPlotContextView = NULL;
  } 

  if (aOn)
  {
    // Create window and layout
    m_TemporalCalibrationPlotsWindow = new QWidget(this, Qt::Tool);
    m_TemporalCalibrationPlotsWindow->setMinimumSize(QSize(800, 600));
    m_TemporalCalibrationPlotsWindow->setCaption(tr("Temporal calibration report"));
    m_TemporalCalibrationPlotsWindow->setBackgroundColor(QColor::fromRgb(255, 255, 255));

    // Install event filter that is called on closing the window
    m_TemporalCalibrationPlotsWindow->installEventFilter(this);

    QGridLayout* gridPlotWindow = new QGridLayout(m_TemporalCalibrationPlotsWindow, 2, 1, 0, 4, "");

    // Uncalibrted chart view
    QVTKWidget* uncalibratedPlotVtkWidget = new QVTKWidget(m_TemporalCalibrationPlotsWindow);

    m_UncalibratedPlotContextView = vtkContextView::New();
    m_UncalibratedPlotContextView->GetRenderer()->SetBackground(1.0, 1.0, 1.0);

    vtkSmartPointer<vtkChartXY> uncalibratedChart = vtkSmartPointer<vtkChartXY>::New();
    uncalibratedChart->Register(m_UncalibratedPlotContextView);

    vtkPlot *uncalibratedTrackerMetricLine = uncalibratedChart->AddPlot(vtkChart::LINE);
    uncalibratedTrackerMetricLine->SetInput(m_UncalibratedTrackerPositionMetric, 0, 1);
    uncalibratedTrackerMetricLine->SetColor(1,0,0);
    uncalibratedTrackerMetricLine->SetWidth(1.0);

    vtkPlot *videoPositionMetricLineU = uncalibratedChart->AddPlot(vtkChart::LINE);
    videoPositionMetricLineU->SetInput(m_VideoPositionMetric, 0, 1);
    videoPositionMetricLineU->SetColor(0,0,1);
    videoPositionMetricLineU->SetWidth(1.0);

    uncalibratedChart->SetShowLegend(true);
    m_UncalibratedPlotContextView->GetScene()->AddItem(uncalibratedChart);

    uncalibratedPlotVtkWidget->GetRenderWindow()->AddRenderer(m_UncalibratedPlotContextView->GetRenderer());
    uncalibratedPlotVtkWidget->GetRenderWindow()->SetSize(800,600);

    gridPlotWindow->addWidget(uncalibratedPlotVtkWidget, 0, 0);

    // Calibrted chart view
    QVTKWidget* calibratedPlotVtkWidget = new QVTKWidget(m_TemporalCalibrationPlotsWindow);

    m_CalibratedPlotContextView = vtkContextView::New();
    m_CalibratedPlotContextView->GetRenderer()->SetBackground(1.0, 1.0, 1.0);

    vtkSmartPointer<vtkChartXY> calibratedChart = vtkSmartPointer<vtkChartXY>::New();
    calibratedChart->Register(m_CalibratedPlotContextView);

    vtkPlot *calibratedTrackerMetricLine = calibratedChart->AddPlot(vtkChart::LINE);
    calibratedTrackerMetricLine->SetInput(m_CalibratedTrackerPositionMetric, 0, 1);
    calibratedTrackerMetricLine->SetColor(0,1,0);
    calibratedTrackerMetricLine->SetWidth(1.0);

    vtkPlot *videoPositionMetricLineC = calibratedChart->AddPlot(vtkChart::LINE);
    videoPositionMetricLineC->SetInput(m_VideoPositionMetric, 0, 1);
    videoPositionMetricLineC->SetColor(0,0,1);
    videoPositionMetricLineC->SetWidth(1.0);

    calibratedChart->SetShowLegend(true);
    m_CalibratedPlotContextView->GetScene()->AddItem(calibratedChart);

    calibratedPlotVtkWidget->GetRenderWindow()->AddRenderer(m_CalibratedPlotContextView->GetRenderer());
    calibratedPlotVtkWidget->GetRenderWindow()->SetSize(800,600);

    gridPlotWindow->addWidget(calibratedPlotVtkWidget, 1, 0);

    // Position and show window
    m_TemporalCalibrationPlotsWindow->setLayout(gridPlotWindow);
    m_TemporalCalibrationPlotsWindow->move( mapToGlobal( QPoint( ui.pushButton_ShowPlots->x() + ui.pushButton_ShowPlots->width(), 20 ) ) );
    m_TemporalCalibrationPlotsWindow->show();
  }
}

//-----------------------------------------------------------------------------

bool TemporalCalibrationToolbox::eventFilter(QObject *obj, QEvent *ev)
{
  if ( obj == m_TemporalCalibrationPlotsWindow )
  {
    if ( ev->type() == QEvent::Close )
    {
      ui.pushButton_ShowPlots->setChecked(false);
    }
    else
    {
      // Pass the event on to the parent class
      return QWidget::eventFilter( obj, ev );
    }
  }

  return true;
}
