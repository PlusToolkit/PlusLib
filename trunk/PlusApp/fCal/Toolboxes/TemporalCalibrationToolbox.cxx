/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "TemporalCalibrationToolbox.h"
#include "TrackedFrame.h"
#include "fCalMainWindow.h"
#include "vtkPlusBuffer.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkTrackedFrameList.h"
#include "vtkVisualizationController.h"
#include <QFileDialog>
#include <QTimer>
#include <vtkChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkPlot.h>
#include <vtkRenderWindow.h>
#include <vtkTable.h>
#include <vtkXMLUtilities.h>

//-----------------------------------------------------------------------------
namespace
{
  const double INVALID_OFFSET = std::numeric_limits<double>::infinity();
}

//-----------------------------------------------------------------------------

TemporalCalibrationToolbox::TemporalCalibrationToolbox(fCalMainWindow* aParentMainWindow, Qt::WFlags aFlags)
: AbstractToolbox(aParentMainWindow)
, QWidget(aParentMainWindow, aFlags)
, CancelRequest(false)
, LastRecordedFixedItemTimestamp(0.0)
, LastRecordedMovingItemTimestamp(0.0)
, RecordingIntervalMs(200)
, TemporalCalibrationDurationSec(10)
, StartTimeSec(0.0)
, PreviousFixedOffset(INVALID_OFFSET)
, PreviousMovingOffset(INVALID_OFFSET)
, TemporalCalibrationPlotsWindow(NULL)
, UncalibratedPlotContextView(NULL)
, CalibratedPlotContextView(NULL)
, FixedChannel(NULL)
, FixedType(TemporalCalibration::FRAME_TYPE_NONE)
, MovingChannel(NULL)
, MovingType(TemporalCalibration::FRAME_TYPE_NONE)
{
  ui.setupUi(this);

  LineSegmentationClipRectangleOrigin[0]=0;
  LineSegmentationClipRectangleOrigin[1]=0;
  LineSegmentationClipRectangleSize[0]=0;
  LineSegmentationClipRectangleSize[1]=0;

  // Create tracked frame lists
  TemporalCalibrationFixedData = vtkTrackedFrameList::New();
  TemporalCalibrationMovingData = vtkTrackedFrameList::New();

  // Create temporal calibration metric tables
  VideoPositionMetric = vtkTable::New();
  UncalibratedTrackerPositionMetric = vtkTable::New();
  CalibratedTrackerPositionMetric = vtkTable::New();

  // Connect events
  connect( ui.pushButton_StartTemporal, SIGNAL( clicked() ), this, SLOT( StartCalibration() ) );
  connect( ui.pushButton_CancelTemporal, SIGNAL( clicked() ), this, SLOT( CancelCalibration() ) );
  connect( ui.pushButton_ShowPlots, SIGNAL( toggled(bool) ), this, SLOT( ShowPlotsToggled(bool) ) );

  connect( ui.comboBox_FixedChannelValue, SIGNAL( currentIndexChanged(int) ), this, SLOT( FixedSignalChanged(int) ) );
  connect( ui.comboBox_MovingChannelValue, SIGNAL( currentIndexChanged(int) ), this, SLOT( MovingSignalChanged(int) ) );

  connect( ui.comboBox_FixedSourceValue, SIGNAL( currentIndexChanged(int) ), this, SLOT( FixedSourceChanged(int) ) );
  connect( ui.comboBox_MovingSourceValue, SIGNAL( currentIndexChanged(int) ), this, SLOT( MovingSourceChanged(int) ) );
}

//-----------------------------------------------------------------------------

TemporalCalibrationToolbox::~TemporalCalibrationToolbox()
{
  if (TemporalCalibrationFixedData != NULL)
  {
    TemporalCalibrationFixedData->Delete();
    TemporalCalibrationFixedData = NULL;
  } 

  if (TemporalCalibrationMovingData != NULL)
  {
    TemporalCalibrationMovingData->Delete();
    TemporalCalibrationMovingData = NULL;
  } 

  if (VideoPositionMetric != NULL)
  {
    VideoPositionMetric->Delete();
    VideoPositionMetric = NULL;
  } 

  if (UncalibratedTrackerPositionMetric != NULL)
  {
    UncalibratedTrackerPositionMetric->Delete();
    UncalibratedTrackerPositionMetric = NULL;
  } 

  if (CalibratedTrackerPositionMetric != NULL)
  {
    CalibratedTrackerPositionMetric->Delete();
    CalibratedTrackerPositionMetric = NULL;
  } 

  if (TemporalCalibrationPlotsWindow != NULL)
  {
    delete TemporalCalibrationPlotsWindow;
    TemporalCalibrationPlotsWindow = NULL;
  } 

  if (UncalibratedPlotContextView != NULL)
  {
    UncalibratedPlotContextView->Delete();
    UncalibratedPlotContextView = NULL;
  } 

  if (CalibratedPlotContextView != NULL)
  {
    CalibratedPlotContextView->Delete();
    CalibratedPlotContextView = NULL;
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

    // Retrieve channels from devices
    DeviceCollection aCollection;
    if( m_ParentMainWindow->GetVisualizationController()->GetDataCollector()->GetDevices(aCollection) != PLUS_SUCCESS )
    {
      ui.label_InstructionsTemporal->setText(tr("Unable to retrieve devices! See log."));
      SetState(ToolboxState_Error);
      return;
    }
    ui.comboBox_FixedChannelValue->clear();
    ui.comboBox_MovingChannelValue->clear();
    ui.comboBox_FixedSourceValue->clear();
    ui.comboBox_MovingSourceValue->clear();
    for( DeviceCollectionIterator it = aCollection.begin(); it != aCollection.end(); ++it )
    {
      if( !(*it)->IsVirtual() )
      {
        for( ChannelContainerIterator chanIt = (*it)->GetOutputChannelsStart(); chanIt != (*it)->GetOutputChannelsEnd(); ++chanIt )
        {
          ui.comboBox_FixedChannelValue->addItem(QString((*chanIt)->GetChannelId()));
          ui.comboBox_MovingChannelValue->addItem(QString((*chanIt)->GetChannelId()));
        }
      }
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

    this->FixedSignalChanged(0);
    this->MovingSignalChanged(0);
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
    RecordingIntervalMs = recordingIntervalMs;
  }
  else
  {
    LOG_WARNING("Unable to read RecordingIntervalMs attribute from fCal element of the device set configuration, default value '" << RecordingIntervalMs << "' will be used");
  }

  // Duration of temporal calibration
  int temporalCalibrationDurationSec = 0; 
  if ( fCalElement->GetScalarAttribute("TemporalCalibrationDurationSec", temporalCalibrationDurationSec ) )
  {
    TemporalCalibrationDurationSec = temporalCalibrationDurationSec;
  }
  else
  {
    LOG_WARNING("Unable to read TemporalCalibrationDurationSec attribute from fCal element of the device set configuration, default value '" << TemporalCalibrationDurationSec << "' will be used");
  }

  // TODO: move all temporal calibration attributes into one vtkTemporalCalibrationAlgo element
  // Now we get the clipping region from the fiducial line segmentation algo element
  vtkXMLDataElement* segmentationParameters = aConfig->FindNestedElementWithName("Segmentation");
  if (segmentationParameters != NULL)
  {
    int clipOrigin[2] = {0};
    int clipSize[2] = {0};
    if ( segmentationParameters->GetVectorAttribute("ClipRectangleOrigin", 2, clipOrigin) && 
      segmentationParameters->GetVectorAttribute("ClipRectangleSize", 2, clipSize) )
    {
      LineSegmentationClipRectangleOrigin[0] = clipOrigin[0];
      LineSegmentationClipRectangleOrigin[1] = clipOrigin[1];
      LineSegmentationClipRectangleSize[0] = clipSize[0];
      LineSegmentationClipRectangleSize[1] = clipSize[1];
    }
    else
    {
      LOG_WARNING("Cannot find ClipRectangleOrigin or ClipRectangleSize attributes in the segmentation configuration.");
    }
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

  double timeOffset = 0.0;

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
      // TODO : replace this with.... the value between two devices?
      // possibly do this on combobox callback?
      if ( m_ParentMainWindow->GetSelectedChannel() != NULL )
      {
        timeOffset = m_ParentMainWindow->GetSelectedChannel()->GetOwnerDevice()->GetLocalTimeOffsetSec();
      }
    }

    // Update state message
    ui.label_State->setText(tr("Current video time offset: %1 s").arg(timeOffset));
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

    ui.comboBox_FixedChannelValue->setEnabled(false);
    ui.comboBox_MovingChannelValue->setEnabled(false);
    ui.comboBox_FixedSourceValue->setEnabled(false);
    ui.comboBox_MovingSourceValue->setEnabled(false);
  }
  else if (m_State == ToolboxState_Idle)
  {
    bool result = QString::compare(ui.comboBox_FixedChannelValue->currentText(), ui.comboBox_MovingChannelValue->currentText(), Qt::CaseInsensitive) == 0;
    result = result && QString::compare(ui.comboBox_FixedSourceValue->currentText(), ui.comboBox_MovingSourceValue->currentText(), Qt::CaseInsensitive) == 0;
    if( result )
    {
      ui.label_InstructionsTemporal->setText(tr("Please select different fixed and moving signal sources."));
    }
    else
    {
      ui.label_InstructionsTemporal->setText(tr("Move probe to vertical position in the water tank so that the bottom is visible and press Start"));
    }
    ui.pushButton_StartTemporal->setEnabled(!result);
    ui.pushButton_CancelTemporal->setEnabled(false);
    ui.pushButton_ShowPlots->setEnabled(false);

    m_ParentMainWindow->SetStatusBarText(QString(""));
    m_ParentMainWindow->SetStatusBarProgress(-1);

    ui.comboBox_FixedChannelValue->setEnabled(true);
    ui.comboBox_MovingChannelValue->setEnabled(true);
    ui.comboBox_FixedSourceValue->setEnabled(ui.comboBox_FixedSourceValue->count() > 0 );
    ui.comboBox_MovingSourceValue->setEnabled(ui.comboBox_MovingSourceValue->count() > 0 );

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

    ui.comboBox_FixedChannelValue->setEnabled(false);
    ui.comboBox_MovingChannelValue->setEnabled(false);
    ui.comboBox_FixedSourceValue->setEnabled(false);
    ui.comboBox_MovingSourceValue->setEnabled(false);
  }
  else if (m_State == ToolboxState_Done)
  {
    bool result = QString::compare(ui.comboBox_FixedChannelValue->currentText(), ui.comboBox_MovingChannelValue->currentText(), Qt::CaseInsensitive) == 0;
    result = result && QString::compare(ui.comboBox_FixedSourceValue->currentText(), ui.comboBox_MovingSourceValue->currentText(), Qt::CaseInsensitive) == 0;
    if( result )
    {
      ui.label_InstructionsTemporal->setText(tr("Please select different fixed and moving signal sources."));
    }
    else
    {
      ui.label_InstructionsTemporal->setText(tr("Temporal calibration is ready to save and its result plots can be viewed\n(Video time offset: %1 s)").arg(timeOffset));
    }
    ui.pushButton_ShowPlots->setEnabled(true);
    ui.pushButton_StartTemporal->setEnabled(!result);
    ui.pushButton_CancelTemporal->setEnabled(false);

    m_ParentMainWindow->SetStatusBarText(QString(" Calibration done"));
    m_ParentMainWindow->SetStatusBarProgress(-1);

    ui.comboBox_FixedChannelValue->setEnabled(true);
    ui.comboBox_MovingChannelValue->setEnabled(true);
    ui.comboBox_FixedSourceValue->setEnabled(ui.comboBox_FixedSourceValue->count() > 0 );
    ui.comboBox_MovingSourceValue->setEnabled(ui.comboBox_MovingSourceValue->count() > 0 );

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

    ui.comboBox_FixedChannelValue->setEnabled(false);
    ui.comboBox_MovingChannelValue->setEnabled(false);
    ui.comboBox_FixedSourceValue->setEnabled(false);
    ui.comboBox_MovingSourceValue->setEnabled(false);

    QApplication::restoreOverrideCursor();
  }
}

//-----------------------------------------------------------------------------

void TemporalCalibrationToolbox::StartCalibration()
{
  LOG_INFO("Temporal calibration started");

  m_ParentMainWindow->SetToolboxesEnabled(false);

  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

  if( this->FixedChannel == NULL || this->MovingChannel == NULL )
  {
    if( m_ParentMainWindow->GetVisualizationController()->GetDataCollector()->GetChannel(this->MovingChannel, 
      std::string(ui.comboBox_MovingChannelValue->currentText().toLatin1())) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to retrieve device " << std::string(ui.comboBox_MovingChannelValue->currentText().toLatin1()));
      return;
    }

    if( m_ParentMainWindow->GetVisualizationController()->GetDataCollector()->GetChannel(this->FixedChannel, 
      std::string(ui.comboBox_FixedChannelValue->currentText().toLatin1())) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to retrieve device " << std::string(ui.comboBox_FixedChannelValue->currentText().toLatin1()));
      return;
    }
  }

  // Set validation transform names for tracked frame list
  // Set the local time offset to 0 before synchronization
  QString curFixedType = ui.comboBox_FixedSourceValue->itemData(ui.comboBox_FixedSourceValue->currentIndex()).asString();
  this->FixedType = TemporalCalibration::FRAME_TYPE_VIDEO;
  this->TemporalCalibrationFixedData->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP);
  if( QString::compare(curFixedType, QString("Video")) != 0 )
  {
    this->FixedType = TemporalCalibration::FRAME_TYPE_TRACKER;
    this->TemporalCalibrationFixedData->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP | REQUIRE_TRACKING_OK );
    this->FixedValidationTransformName.SetTransformName(std::string(ui.comboBox_FixedSourceValue->currentText().toLatin1()).c_str());
    this->TemporalCalibrationFixedData->SetFrameTransformNameForValidation(this->FixedValidationTransformName);
  }    
  double offset(INVALID_OFFSET);
  offset = this->FixedChannel->GetOwnerDevice()->GetLocalTimeOffsetSec();
  this->PreviousFixedOffset = offset;
  this->FixedChannel->GetOwnerDevice()->SetLocalTimeOffsetSec(0.0);

  QString curMovingType = ui.comboBox_MovingSourceValue->itemData(ui.comboBox_MovingSourceValue->currentIndex()).asString();
  this->MovingType = TemporalCalibration::FRAME_TYPE_VIDEO;
  this->TemporalCalibrationMovingData->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP);
  if( QString::compare(curMovingType, QString("Video")) != 0 )
  {
    this->MovingType = TemporalCalibration::FRAME_TYPE_TRACKER;
    this->TemporalCalibrationMovingData->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP | REQUIRE_TRACKING_OK );
    this->MovingValidationTransformName.SetTransformName(std::string(ui.comboBox_MovingSourceValue->currentText().toLatin1()).c_str());
    this->TemporalCalibrationMovingData->SetFrameTransformNameForValidation(this->MovingValidationTransformName);
  }
  offset = this->MovingChannel->GetOwnerDevice()->GetLocalTimeOffsetSec();
  this->PreviousMovingOffset = offset;
  this->MovingChannel->GetOwnerDevice()->SetLocalTimeOffsetSec(0.0);

  TemporalCalibrationFixedData->Clear();
  TemporalCalibrationMovingData->Clear();

  double currentTimeSec = vtkAccurateTimer::GetSystemTime();
  LastRecordedFixedItemTimestamp = currentTimeSec;
  LastRecordedMovingItemTimestamp = currentTimeSec;

  StartTimeSec = vtkAccurateTimer::GetSystemTime();
  CancelRequest = false;

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
  TemporalCalibration temporalCalibrationObject;
  temporalCalibrationObject.SetFixedFrames(TemporalCalibrationFixedData, this->FixedType);
  if( this->FixedType == TemporalCalibration::FRAME_TYPE_TRACKER )
  {
    temporalCalibrationObject.SetFixedProbeToReferenceTransformName(std::string(ui.comboBox_FixedSourceValue->currentText().toLatin1()));
  }
  temporalCalibrationObject.SetMovingFrames(TemporalCalibrationMovingData, this->MovingType);
  if( this->MovingType == TemporalCalibration::FRAME_TYPE_TRACKER )
  {
    temporalCalibrationObject.SetMovingProbeToReferenceTransformName(std::string(ui.comboBox_MovingSourceValue->currentText().toLatin1()));
  }

  temporalCalibrationObject.SetVideoClipRectangle( LineSegmentationClipRectangleOrigin,  LineSegmentationClipRectangleSize);
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
    case TemporalCalibration::TEMPORAL_CALIBRATION_ERROR_NOT_ENOUGH_FIXED_FRAMES:
      errorStr = "Not enough frames in fixed signal.";
      break;
    case TemporalCalibration::TEMPORAL_CALIBRATION_ERROR_NO_FRAMES_IN_ULTRASOUND_DATA:
      errorStr = "No frames in ultrasound data.";
      break;
    case TemporalCalibration::TEMPORAL_CALIBRATION_ERROR_SAMPLING_RESOLUTION_TOO_SMALL:
      errorStr = "Sampling resolution too small.";
      break;
    case TemporalCalibration::TEMPORAL_CALIBRATION_ERROR_NONE:
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

  if( m_ParentMainWindow->GetSelectedChannel() != NULL )
  {
    m_ParentMainWindow->GetSelectedChannel()->GetOwnerDevice()->SetLocalTimeOffsetSec(trackerLagSec);
    offsetsSuccessfullySet = true;
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
  temporalCalibrationObject.GetVideoPositionSignal(VideoPositionMetric);
  temporalCalibrationObject.GetUncalibratedTrackerPositionSignal(UncalibratedTrackerPositionMetric);
  temporalCalibrationObject.GetCalibratedTrackerPositionSignal(CalibratedTrackerPositionMetric);

  TemporalCalibrationFixedData->Clear();
  TemporalCalibrationMovingData->Clear();

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

  if (currentTimeSec - StartTimeSec >= TemporalCalibrationDurationSec)
  {
    // Update progress
    m_ParentMainWindow->SetStatusBarProgress(100);

    // The prescribed data collection time is up
    ComputeCalibrationResults();
    return;
  }

  // Cancel if requested
  if (CancelRequest)
  {
    LOG_INFO("Calibration process cancelled by the user");
    CancelCalibration();
    return;
  }

  int numberOfTrackingFramesBeforeRecording = TemporalCalibrationFixedData->GetNumberOfTrackedFrames();
  int numberOfVideoFramesBeforeRecording = TemporalCalibrationMovingData->GetNumberOfTrackedFrames();

  if( this->FixedChannel != NULL )
  {
    if( this->FixedChannel->GetTrackedFrameList(LastRecordedFixedItemTimestamp, TemporalCalibrationFixedData, 50) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to add data to fixed frame list.");
    }
  }
  else
  {
    LOG_ERROR("Unable to retrieve data for fixed channel. Aborting.");
    CancelCalibration();
    return;
  }
  if( this->MovingChannel != NULL )
  {
    if( this->MovingChannel->GetTrackedFrameList(LastRecordedMovingItemTimestamp, TemporalCalibrationMovingData, 50) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to add data to moving frame list.");
    }
  }
  else
  {
    LOG_ERROR("Unable to retrieve data for moving channel. Aborting.");
    CancelCalibration();
    return;
  }

  // Update progress
  int progressPercent = (int)((currentTimeSec - StartTimeSec) / TemporalCalibrationDurationSec * 100.0);
  m_ParentMainWindow->SetStatusBarProgress(progressPercent);
  LOG_DEBUG("Number of tracked frames in the calibration dataset: Tracking: " << std::setw(3) << numberOfTrackingFramesBeforeRecording << " => " << TemporalCalibrationFixedData->GetNumberOfTrackedFrames() << "; Video: " << numberOfVideoFramesBeforeRecording << " => " << TemporalCalibrationMovingData->GetNumberOfTrackedFrames());

  QTimer::singleShot(RecordingIntervalMs, this, SLOT(DoCalibration())); 
}

//-----------------------------------------------------------------------------

void TemporalCalibrationToolbox::CancelCalibration()
{
  LOG_INFO("Calibration cancelled");

  CancelRequest = true;

  // Reset the local time offset to the previous values
  if (this->FixedChannel != NULL && this->PreviousFixedOffset != INVALID_OFFSET )
  {
    this->FixedChannel->GetOwnerDevice()->SetLocalTimeOffsetSec(this->PreviousFixedOffset);
  }

  if( this->MovingChannel != NULL && this->PreviousMovingOffset != INVALID_OFFSET )
  {
    this->MovingChannel->GetOwnerDevice()->SetLocalTimeOffsetSec(this->PreviousMovingOffset);
  }

  this->FixedChannel = NULL;
  this->MovingChannel = NULL;
  this->FixedType = TemporalCalibration::FRAME_TYPE_NONE;
  this->MovingType = TemporalCalibration::FRAME_TYPE_NONE;
  this->FixedValidationTransformName.Clear();
  this->MovingValidationTransformName.Clear();

  m_ParentMainWindow->SetToolboxesEnabled(true);

  SetState(ToolboxState_Idle);
}

//-----------------------------------------------------------------------------

void TemporalCalibrationToolbox::ShowPlotsToggled(bool aOn)
{
  // Delete objects if toggled off, make sure they are deleted when toggled on
  if (TemporalCalibrationPlotsWindow != NULL)
  {
    delete TemporalCalibrationPlotsWindow;
    TemporalCalibrationPlotsWindow = NULL;
  } 

  if (UncalibratedPlotContextView != NULL)
  {
    UncalibratedPlotContextView->Delete();
    UncalibratedPlotContextView = NULL;
  } 

  if (CalibratedPlotContextView != NULL)
  {
    CalibratedPlotContextView->Delete();
    CalibratedPlotContextView = NULL;
  } 

  if (aOn)
  {
    // Create window and layout
    TemporalCalibrationPlotsWindow = new QWidget(this, Qt::Tool);
    TemporalCalibrationPlotsWindow->setMinimumSize(QSize(800, 600));
    TemporalCalibrationPlotsWindow->setCaption(tr("Temporal calibration report"));
    TemporalCalibrationPlotsWindow->setBackgroundColor(QColor::fromRgb(255, 255, 255));

    // Install event filter that is called on closing the window
    TemporalCalibrationPlotsWindow->installEventFilter(this);

    QGridLayout* gridPlotWindow = new QGridLayout(TemporalCalibrationPlotsWindow);
    gridPlotWindow->setMargin(0);
    gridPlotWindow->setSpacing(4);

    // Uncalibrated chart view
    QVTKWidget* uncalibratedPlotVtkWidget = new QVTKWidget(TemporalCalibrationPlotsWindow);

    UncalibratedPlotContextView = vtkContextView::New();
    UncalibratedPlotContextView->GetRenderer()->SetBackground(1.0, 1.0, 1.0);

    vtkSmartPointer<vtkChartXY> uncalibratedChart = vtkSmartPointer<vtkChartXY>::New();
    uncalibratedChart->Register(UncalibratedPlotContextView);

    vtkPlot *uncalibratedTrackerMetricLine = uncalibratedChart->AddPlot(vtkChart::LINE);
    uncalibratedTrackerMetricLine->SetInput(UncalibratedTrackerPositionMetric, 0, 1);
    //vtkVariantArray* array = vtkVariantArray::New();
    //m_UncalibratedTrackerPositionMetric->GetRow(13, array);
    //array->PrintSelf(std::cout, *(vtkIndent::New()));
    //std::cout << array->GetValue(0) << "::" << array->GetValue(1) << std::endl;
    uncalibratedTrackerMetricLine->SetColor(1,0,0);
    uncalibratedTrackerMetricLine->SetWidth(1.0);

    vtkPlot *videoPositionMetricLineU = uncalibratedChart->AddPlot(vtkChart::LINE);
    videoPositionMetricLineU->SetInput(VideoPositionMetric, 0, 1);
    videoPositionMetricLineU->SetColor(0,0,1);
    videoPositionMetricLineU->SetWidth(1.0);

    uncalibratedChart->SetShowLegend(true);
    UncalibratedPlotContextView->GetScene()->AddItem(uncalibratedChart);

    uncalibratedPlotVtkWidget->GetRenderWindow()->AddRenderer(UncalibratedPlotContextView->GetRenderer());
    uncalibratedPlotVtkWidget->GetRenderWindow()->SetSize(800,600);

    gridPlotWindow->addWidget(uncalibratedPlotVtkWidget, 0, 0);

    // Calibrated chart view
    QVTKWidget* calibratedPlotVtkWidget = new QVTKWidget(TemporalCalibrationPlotsWindow);

    CalibratedPlotContextView = vtkContextView::New();
    CalibratedPlotContextView->GetRenderer()->SetBackground(1.0, 1.0, 1.0);

    vtkSmartPointer<vtkChartXY> calibratedChart = vtkSmartPointer<vtkChartXY>::New();
    calibratedChart->Register(CalibratedPlotContextView);

    vtkPlot *calibratedTrackerMetricLine = calibratedChart->AddPlot(vtkChart::LINE);
    calibratedTrackerMetricLine->SetInput(CalibratedTrackerPositionMetric, 0, 1);
    calibratedTrackerMetricLine->SetColor(0,1,0);
    calibratedTrackerMetricLine->SetWidth(1.0);

    vtkPlot *videoPositionMetricLineC = calibratedChart->AddPlot(vtkChart::LINE);
    videoPositionMetricLineC->SetInput(VideoPositionMetric, 0, 1);
    videoPositionMetricLineC->SetColor(0,0,1);
    videoPositionMetricLineC->SetWidth(1.0);

    calibratedChart->SetShowLegend(true);
    CalibratedPlotContextView->GetScene()->AddItem(calibratedChart);

    calibratedPlotVtkWidget->GetRenderWindow()->AddRenderer(CalibratedPlotContextView->GetRenderer());
    calibratedPlotVtkWidget->GetRenderWindow()->SetSize(800,600);

    gridPlotWindow->addWidget(calibratedPlotVtkWidget, 1, 0);

    // Position and show window
    TemporalCalibrationPlotsWindow->setLayout(gridPlotWindow);
    TemporalCalibrationPlotsWindow->move( mapToGlobal( QPoint( ui.pushButton_ShowPlots->x() + ui.pushButton_ShowPlots->width(), 20 ) ) );
    TemporalCalibrationPlotsWindow->show();
  }
}

//-----------------------------------------------------------------------------

bool TemporalCalibrationToolbox::eventFilter(QObject *obj, QEvent *ev)
{
  if ( obj == TemporalCalibrationPlotsWindow )
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

//-----------------------------------------------------------------------------

void TemporalCalibrationToolbox::OnDeactivated()
{

}

//-----------------------------------------------------------------------------

void TemporalCalibrationToolbox::FixedSignalChanged( int newIndex )
{
  ui.comboBox_FixedSourceValue->clear();

  if( ui.comboBox_FixedChannelValue->currentIndex() != -1 )
  {
    if( m_ParentMainWindow->GetVisualizationController()->GetDataCollector()->GetChannel(this->FixedChannel, 
      std::string(ui.comboBox_FixedChannelValue->currentText().toLatin1())) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to retrieve device " << std::string(ui.comboBox_FixedChannelValue->currentText().toLatin1()));
      return;
    }

    // Determine data types, populate
    vtkPlusDataSource* aSource(NULL);
    if( this->FixedChannel->GetVideoSource(aSource) == PLUS_SUCCESS )
    {
      QVariant strVar = QVariant::fromValue(QString("Video"));
      ui.comboBox_FixedSourceValue->addItem(aSource->GetSourceId(), strVar);
    }
    TrackedFrame frame;
    if( this->FixedChannel->GetTrackedFrame(&frame) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to retrieve tracked frame from channel: " << this->FixedChannel->GetChannelId() );
      return;
    }
    vtkTransformRepository* repo = vtkTransformRepository::New();
    repo->SetTransforms(frame);
    
    std::vector<PlusTransformName> nameList;
    frame.GetCustomFrameTransformNameList(nameList);
    std::vector<std::string> fromList;
    std::vector<std::string> toList;
    for (std::vector<PlusTransformName>::iterator it = nameList.begin(); it != nameList.end(); ++it )
    {
      if( std::find(fromList.begin(), fromList.end(), it->From()) == fromList.end() )
      {
        fromList.push_back(it->From());
        toList.push_back(it->From());
      }
      if( std::find(toList.begin(), toList.end(), it->To()) == toList.end() )
      {
        fromList.push_back(it->To());
        toList.push_back(it->To());
      }
    }

    // build exhaustive list of all possible transforms
    for (std::vector<std::string>::iterator fromIt = fromList.begin(); fromIt != fromList.end(); ++fromIt )
    {
      for (std::vector<std::string>::iterator toIt = toList.begin(); toIt != toList.end(); ++toIt )
      {
        if( fromIt->compare(*toIt) == 0 )
        {
          continue;
        }

        PlusTransformName trans;
        std::stringstream ss;
        ss << (*fromIt) << "To" << (*toIt);
        trans.SetTransformName(ss.str().c_str());

        if( trans.IsValid() && repo->IsExistingTransform(trans) )
        {
          QVariant strVar = QVariant::fromValue(QString("Transform"));
          ui.comboBox_FixedSourceValue->addItem(QString(ss.str().c_str()), strVar);
        }
      }
    }

    repo->Delete();
  }
  
  this->SetDisplayAccordingToState();
}

//-----------------------------------------------------------------------------

void TemporalCalibrationToolbox::MovingSignalChanged( int newIndex )
{
  ui.comboBox_MovingSourceValue->clear();

  if( ui.comboBox_MovingChannelValue->currentIndex() != -1 )
  {
    if( m_ParentMainWindow->GetVisualizationController()->GetDataCollector()->GetChannel(this->MovingChannel, 
      std::string(ui.comboBox_MovingChannelValue->currentText().toLatin1())) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to retrieve device " << std::string(ui.comboBox_MovingChannelValue->currentText().toLatin1()));
      return;
    }

    // Determine data types, populate
    vtkPlusDataSource* aSource(NULL);
    if( this->MovingChannel->GetVideoSource(aSource) == PLUS_SUCCESS )
    {
      QVariant strVar = QVariant::fromValue(QString("Video"));
      ui.comboBox_MovingSourceValue->addItem(aSource->GetSourceId(), strVar);
    }
    TrackedFrame frame;
    if( this->MovingChannel->GetTrackedFrame(&frame) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to retrieve tracked frame from channel: " << this->MovingChannel->GetChannelId() );
      return;
    }
    vtkTransformRepository* repo = vtkTransformRepository::New();
    repo->SetTransforms(frame);

    std::vector<PlusTransformName> nameList;
    frame.GetCustomFrameTransformNameList(nameList);
    std::vector<std::string> fromList;
    std::vector<std::string> toList;
    for (std::vector<PlusTransformName>::iterator it = nameList.begin(); it != nameList.end(); ++it )
    {
      if( std::find(fromList.begin(), fromList.end(), it->From()) == fromList.end() )
      {
        fromList.push_back(it->From());
        toList.push_back(it->From());
      }
      if( std::find(toList.begin(), toList.end(), it->To()) == toList.end() )
      {
        fromList.push_back(it->To());
        toList.push_back(it->To());
      }
    }

    // build exhaustive list of all possible transforms
    for (std::vector<std::string>::iterator fromIt = fromList.begin(); fromIt != fromList.end(); ++fromIt )
    {
      for (std::vector<std::string>::iterator toIt = toList.begin(); toIt != toList.end(); ++toIt )
      {
        if( fromIt->compare(*toIt) == 0 )
        {
          continue;
        }

        PlusTransformName trans;
        std::stringstream ss;
        ss << (*fromIt) << "To" << (*toIt);
        trans.SetTransformName(ss.str().c_str());
        
        if( trans.IsValid() && repo->IsExistingTransform(trans) )
        {
          QVariant strVar = QVariant::fromValue(QString("Transform"));
          ui.comboBox_MovingSourceValue->addItem(QString(ss.str().c_str()), strVar);
        }
      }
    }

    repo->Delete();
  }

  this->SetDisplayAccordingToState();
}

//-----------------------------------------------------------------------------

void TemporalCalibrationToolbox::FixedSourceChanged( int newIndex )
{
  this->SetDisplayAccordingToState();
}

//-----------------------------------------------------------------------------

void TemporalCalibrationToolbox::MovingSourceChanged( int newIndex )
{
  this->SetDisplayAccordingToState();
}
