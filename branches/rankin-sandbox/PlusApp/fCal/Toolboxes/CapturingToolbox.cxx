/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "CaptureControlWidget.h"
#include "CapturingToolbox.h"
#include "TrackedFrame.h"
#include "VolumeReconstructionToolbox.h"
#include "fCalMainWindow.h"
#include "vtkPlusDevice.h" // Only to get maximum frame rate in device mode
#include "vtkTrackedFrameList.h"
#include "vtkVisualizationController.h"
#include "vtksys/SystemTools.hxx"
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>

static const int MAX_ALLOWED_RECORDING_LAG_SEC = 3.0; // if the recording lags more than this then it'll skip frames to catch up

//-----------------------------------------------------------------------------
CapturingToolbox::CapturingToolbox(fCalMainWindow* aParentMainWindow, Qt::WFlags aFlags)
: AbstractToolbox(aParentMainWindow)
, QWidget(aParentMainWindow, aFlags)
, m_RecordedFrames(NULL)
, m_RecordingTimer(NULL)
, m_RecordingLastAlreadyRecordedFrameTimestamp(UNDEFINED_TIMESTAMP)
, m_RecordingNextFrameToBeRecordedTimestamp(0.0)
, m_SamplingFrameRate(8)
, m_RequestedFrameRate(0.0)
, m_ActualFrameRate(0.0)
{
  ui.setupUi(this);

  // Create tracked frame list
  m_RecordedFrames = vtkTrackedFrameList::New();
  m_RecordedFrames->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP); 

  // Connect events
  connect( ui.pushButton_Snapshot, SIGNAL( clicked() ), this, SLOT( TakeSnapshot() ) );
  connect( ui.pushButton_Record, SIGNAL( clicked() ), this, SLOT( Record() ) );
  connect( ui.pushButton_ClearRecordedFrames, SIGNAL( clicked() ), this, SLOT( ClearRecordedFrames() ) );
  connect( ui.pushButton_Save, SIGNAL( clicked() ), this, SLOT( Save() ) );
  connect( ui.pushButton_SaveAs, SIGNAL( clicked() ), this, SLOT( SaveAs() ) );
  connect( ui.pushButton_SaveAll, SIGNAL( clicked() ), this, SLOT( SaveAll() ) );
  connect( ui.pushButton_ClearAll, SIGNAL( clicked() ), this, SLOT( ClearAll() ) );
  connect( ui.pushButton_StartStopAll, SIGNAL( clicked() ), this, SLOT( StartStopAll() ) );
  connect( ui.horizontalSlider_SamplingRate, SIGNAL( valueChanged(int) ), this, SLOT( SamplingRateChanged(int) ) );

  // Create and connect recording timer
  m_RecordingTimer = new QTimer(this); 
  connect(m_RecordingTimer, SIGNAL(timeout()), this, SLOT(Capture()) );

  ui.pushButton_Save->setEnabled(m_RecordedFrames->GetNumberOfTrackedFrames() > 0);
  ui.pushButton_SaveAs->setEnabled(m_RecordedFrames->GetNumberOfTrackedFrames() > 0);

  ui.pushButton_StartStopAll->setEnabled(false);
  ui.pushButton_SaveAll->setEnabled(false);
  ui.pushButton_ClearAll->setEnabled(false);

  m_LastSaveLocation = vtkPlusConfig::GetInstance()->GetOutputDirectory();
}

//-----------------------------------------------------------------------------
CapturingToolbox::~CapturingToolbox()
{
  if (m_RecordedFrames != NULL) {
    m_RecordedFrames->Delete();
    m_RecordedFrames = NULL;
  }
}

//-----------------------------------------------------------------------------
void CapturingToolbox::OnActivated()
{
  LOG_TRACE("CapturingToolbox::OnActivated"); 

  if ((m_ParentMainWindow->GetVisualizationController()->GetDataCollector() != NULL)
    && (m_ParentMainWindow->GetVisualizationController()->GetDataCollector()->GetConnected()))
  {
    // Set initialized if it was uninitialized
    if (m_State == ToolboxState_Uninitialized || m_State == ToolboxState_Error)
    {
      SetState(ToolboxState_Idle);
    }
    else
    {
      SetDisplayAccordingToState();
    }

    DeviceCollection aCollection;
    if( m_ParentMainWindow->GetVisualizationController()->GetDataCollector()->GetDevices(aCollection) == PLUS_SUCCESS )
    {
      for( DeviceCollectionConstIterator it = aCollection.begin(); it != aCollection.end(); ++it)
      {
        vtkPlusDevice* aDevice = *it;
        if( dynamic_cast<vtkVirtualDiscCapture*>(aDevice) != NULL )
        {
          vtkVirtualDiscCapture* capDevice = dynamic_cast<vtkVirtualDiscCapture*>(aDevice);
          CaptureControlWidget* aWidget = new CaptureControlWidget(NULL);
          aWidget->SetCaptureDevice(*capDevice);
          ui.captureWidgetLayout->addWidget(aWidget);
          m_CaptureWidgets.push_back(aWidget);
          connect(aWidget, SIGNAL(EmitStatusMessage(const std::string&)), this, SLOT(HandleStatusMessage(const std::string&)) );
        }
      }
    }
  }
  else
  {
    SetState(ToolboxState_Uninitialized);
  }
}

//-----------------------------------------------------------------------------
void CapturingToolbox::RefreshContent()
{
  //LOG_TRACE("CapturingToolbox::RefreshContent");

  if (m_State == ToolboxState_InProgress)
  {
    ui.label_ActualRecordingFrameRate->setText(QString::number(m_ActualFrameRate, 'f', 2));
    ui.label_NumberOfRecordedFrames->setText(QString::number(m_RecordedFrames->GetNumberOfTrackedFrames()));
  }

  ui.pushButton_SaveAll->setEnabled(false);
  for( std::vector<CaptureControlWidget*>::iterator it = m_CaptureWidgets.begin(); it != m_CaptureWidgets.end(); ++it )
  {
    CaptureControlWidget* widget = *it;
    if( widget->CanSave() )
    {
      ui.pushButton_SaveAll->setEnabled(true);
      break;
    }
  }

  ui.pushButton_ClearAll->setEnabled( m_CaptureWidgets.size() > 0 );
  ui.pushButton_StartStopAll->setEnabled( m_CaptureWidgets.size() > 0 );

  for( std::vector<CaptureControlWidget*>::iterator it = m_CaptureWidgets.begin(); it != m_CaptureWidgets.end(); ++it )
  {
    CaptureControlWidget* widget = *it;
    widget->UpdateBasedOnState();
  }
}

//-----------------------------------------------------------------------------
void CapturingToolbox::SetDisplayAccordingToState()
{
  LOG_TRACE("CapturingToolbox::SetDisplayAccordingToState");

  // If connected
  if ( (m_ParentMainWindow->GetVisualizationController()->GetDataCollector() != NULL)
    && (m_ParentMainWindow->GetVisualizationController()->GetDataCollector()->GetConnected()) )
  {
    // If the force show devices isn't enabled, set it to 2D
    if( !m_ParentMainWindow->IsForceShowDevicesEnabled() )
    {
      if( m_ParentMainWindow->GetVisualizationController()->SetVisualizationMode(vtkVisualizationController::DISPLAY_MODE_2D) != PLUS_SUCCESS )
      {
        LOG_WARNING("Unable to switch to 2D visualization. No video feed to capture.");
        m_ParentMainWindow->GetVisualizationController()->HideRenderer();
      }
      else
      {
        // Enable or disable the image manipulation menu
        m_ParentMainWindow->SetImageManipulationMenuEnabled( m_ParentMainWindow->GetVisualizationController()->Is2DMode() );

        // Hide or show the orientation markers based on the value of the checkbox
        m_ParentMainWindow->GetVisualizationController()->ShowOrientationMarkers(m_ParentMainWindow->IsOrientationMarkersEnabled());
      }
    }

    // If tracking
    if (m_ParentMainWindow->GetSelectedChannel()->GetTrackingDataAvailable())
    {
      // Update state message according to available transforms
      if (!m_ParentMainWindow->GetImageCoordinateFrame().empty() && !m_ParentMainWindow->GetProbeCoordinateFrame().empty())
      {
        std::string imageToProbeTransformNameStr;
        PlusTransformName imageToProbeTransformName(
          m_ParentMainWindow->GetImageCoordinateFrame(), m_ParentMainWindow->GetProbeCoordinateFrame());
        imageToProbeTransformName.GetTransformName(imageToProbeTransformNameStr);

        if (m_ParentMainWindow->GetVisualizationController()->IsExistingTransform(
          m_ParentMainWindow->GetImageCoordinateFrame().c_str(), m_ParentMainWindow->GetProbeCoordinateFrame().c_str(), false) == PLUS_SUCCESS)
        {
          std::string date, errorStr;
          double error;
          if (m_ParentMainWindow->GetVisualizationController()->GetTransformRepository()->GetTransformDate(imageToProbeTransformName, date) != PLUS_SUCCESS)
          {
            date = "N/A";
          }
          if (m_ParentMainWindow->GetVisualizationController()->GetTransformRepository()->GetTransformError(imageToProbeTransformName, error) == PLUS_SUCCESS)
          {
            std::stringstream ss;
            ss << std::fixed << error;
            errorStr = ss.str();
          }
          else
          {
            errorStr = "N/A";
          }

          QPalette palette;
          palette.setColor(ui.label_State->foregroundRole(), Qt::black);
          ui.label_State->setPalette(palette);
          ui.label_State->setText( QString("%1 transform present, ready for capturing. \nDate: %2, Error: %3").arg(imageToProbeTransformNameStr.c_str()).arg(date.c_str()).arg(errorStr.c_str()) );
        }
        else
        {
          QPalette palette;
          palette.setColor(ui.label_State->foregroundRole(), QColor::fromRgb(255, 128, 0));
          ui.label_State->setPalette(palette);
          ui.label_State->setText( QString("%1 transform is absent, spatial calibration needs to be performed or imported.").arg(imageToProbeTransformNameStr.c_str()) );
          LOG_INFO(imageToProbeTransformNameStr << " transform is absent, spatial calibration needs to be performed or imported.");
        }
      }
      else
      {
        QPalette palette;
        palette.setColor(ui.label_State->foregroundRole(), QColor::fromRgb(255, 128, 0));
        ui.label_State->setPalette(palette);
        ui.label_State->setText( QString("fCal configuration element does not contain both ImageCoordinateFrame and ProbeCoordinateFrame attributes!") );
        LOG_INFO("fCal configuration element does not contain both ImageCoordinateFrame and ProbeCoordinateFrame attributes");
      }
    }
    else
    {
      QPalette palette;
      palette.setColor(ui.label_State->foregroundRole(), Qt::black);
      ui.label_State->setPalette(palette);
      ui.label_State->setText( QString("Tracking is not enabled.") );
      LOG_INFO("Tracking is not enabled.");
    }
  }
  else
  {
    QPalette palette;
    palette.setColor(ui.label_State->foregroundRole(), QColor::fromRgb(255, 128, 0));
    ui.label_State->setPalette(palette);
    ui.label_State->setText(tr("fCal is not connected to devices. Switch to Configuration toolbox to connect."));
    LOG_INFO("fCal is not connected to devices");
    m_State = ToolboxState_Error;
  }

  // Set widget states according to state
  if (m_State == ToolboxState_Uninitialized)
  {
    ui.pushButton_Snapshot->setEnabled(false);
    ui.pushButton_Record->setEnabled(false);
    ui.pushButton_ClearRecordedFrames->setEnabled(false);
    ui.pushButton_Save->setEnabled(false);
    ui.pushButton_SaveAs->setEnabled(false);
    ui.horizontalSlider_SamplingRate->setEnabled(false);
  }
  else if (m_State == ToolboxState_Idle)
  {
    ui.pushButton_Record->setText("Record");
    ui.pushButton_Record->setIcon( QPixmap( ":/icons/Resources/icon_Record.png" ) );
    ui.pushButton_Record->setFocus();

    ui.pushButton_Snapshot->setEnabled(true);
    ui.pushButton_Record->setEnabled(true);
    ui.pushButton_ClearRecordedFrames->setEnabled(false);
    ui.pushButton_Save->setEnabled(false);
    ui.pushButton_SaveAs->setEnabled(false);
    ui.horizontalSlider_SamplingRate->setEnabled(true);

    SamplingRateChanged(ui.horizontalSlider_SamplingRate->value());

    ui.label_ActualRecordingFrameRate->setText(tr("0.00"));
    ui.label_MaximumRecordingFrameRate->setText(QString::number( GetMaximumFrameRate() ));

    ui.label_NumberOfRecordedFrames->setText("0");
  }
  else if (m_State == ToolboxState_InProgress)
  {
    ui.pushButton_Record->setText(tr("Stop"));
    ui.pushButton_Record->setIcon( QPixmap( ":/icons/Resources/icon_Stop.png" ) );

    ui.pushButton_Snapshot->setEnabled(false);
    ui.pushButton_Record->setEnabled(true);
    ui.pushButton_ClearRecordedFrames->setEnabled(false);
    ui.pushButton_Save->setEnabled(false);
    ui.pushButton_SaveAs->setEnabled(false);
    ui.horizontalSlider_SamplingRate->setEnabled(false);

    // Change the function to be invoked on clicking on the now Stop button
    disconnect( ui.pushButton_Record, SIGNAL( clicked() ), this, SLOT( Record() ) );
    connect( ui.pushButton_Record, SIGNAL( clicked() ), this, SLOT( Stop() ) );
  }
  else if (m_State == ToolboxState_Done)
  {
    ui.pushButton_Record->setText(tr("Record"));
    ui.pushButton_Record->setIcon( QIcon( ":/icons/Resources/icon_Record.png" ) );

    ui.pushButton_Snapshot->setEnabled(true);
    ui.pushButton_Record->setEnabled(true);
    ui.pushButton_ClearRecordedFrames->setEnabled(true);
    ui.pushButton_Save->setEnabled(true);
    ui.pushButton_SaveAs->setEnabled(true);
    ui.horizontalSlider_SamplingRate->setEnabled(true);

    ui.label_ActualRecordingFrameRate->setText("0.00");
    ui.label_MaximumRecordingFrameRate->setText(QString::number( GetMaximumFrameRate() ));

    ui.label_NumberOfRecordedFrames->setText(QString::number(m_RecordedFrames->GetNumberOfTrackedFrames()));

    // Change the function to be invoked on clicking on the now Record button
    disconnect( ui.pushButton_Record, SIGNAL( clicked() ), this, SLOT( Stop() ) );
    connect( ui.pushButton_Record, SIGNAL( clicked() ), this, SLOT( Record() ) );
  }
  else if (m_State == ToolboxState_Error)
  {
    ui.pushButton_Snapshot->setEnabled(false);
    ui.pushButton_Record->setEnabled(false);
    ui.pushButton_ClearRecordedFrames->setEnabled(false);
    ui.pushButton_Save->setEnabled(false);
    ui.pushButton_SaveAs->setEnabled(false);
    ui.horizontalSlider_SamplingRate->setEnabled(false);
  }
}

//-----------------------------------------------------------------------------
void CapturingToolbox::TakeSnapshot()
{
  LOG_TRACE("CapturingToolbox::TakeSnapshot"); 

  TrackedFrame trackedFrame;

  if (m_ParentMainWindow->GetSelectedChannel() == NULL || m_ParentMainWindow->GetSelectedChannel()->GetTrackedFrame(&trackedFrame) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to get tracked frame for the snapshot!");
    return;
  }

  // Check if there are any valid transforms
  std::vector<PlusTransformName> transformNames;
  trackedFrame.GetCustomFrameTransformNameList(transformNames);
  bool validFrame = false;

  if (transformNames.size() == 0)
  {
    validFrame = true;
  }
  else
  {
    for (std::vector<PlusTransformName>::iterator it = transformNames.begin(); it != transformNames.end(); ++it)
    {
      TrackedFrameFieldStatus status = FIELD_INVALID;
      trackedFrame.GetCustomFrameTransformStatus(*it, status);

      if ( status == FIELD_OK )
      {
        validFrame = true;
        break;
      }
    }
  }

  if ( !validFrame )
  {
    LOG_WARNING("Unable to record tracked frame: All the tool transforms are invalid!"); 
    return;
  }

  // Add tracked frame to the list
  if (m_RecordedFrames->AddTrackedFrame(&trackedFrame, vtkTrackedFrameList::SKIP_INVALID_FRAME) != PLUS_SUCCESS)
  {
    LOG_WARNING("Frame could not be added because validation failed!");
    return;
  }

  SetState(ToolboxState_Done);

  LOG_INFO("Snapshot taken");
}

//-----------------------------------------------------------------------------
void CapturingToolbox::Record()
{
  LOG_INFO("Capturing started");

  m_ParentMainWindow->SetToolboxesEnabled(false);
  m_ActualFrameRate=0.0; // display 0.00 until a real estimation is available

  // Reset accessory members
  m_RecordingFirstFrameIndexInThisSegment=m_RecordedFrames->GetNumberOfTrackedFrames();

  ui.plainTextEdit_saveResult->clear();

  m_RecordingNextFrameToBeRecordedTimestamp = vtkAccurateTimer::GetSystemTime();
  m_RecordingLastAlreadyRecordedFrameTimestamp=UNDEFINED_TIMESTAMP; // none yet

  // Start capturing
  SetState(ToolboxState_InProgress);
  double samplingPeriodMsec = GetSamplingPeriodSec()*1000.0;
  m_RecordingTimer->start(samplingPeriodMsec);
}

//-----------------------------------------------------------------------------
void CapturingToolbox::Capture()
{
  //LOG_TRACE("CapturingToolbox::Capture");

  double startTimeSec = vtkAccurateTimer::GetSystemTime();

  vtkDataCollector* dataCollector = NULL;
  if ( (m_ParentMainWindow == NULL) || (m_ParentMainWindow->GetVisualizationController() == NULL) || ((dataCollector = m_ParentMainWindow->GetVisualizationController()->GetDataCollector()) == NULL) )
  {
    LOG_ERROR("Unable to reach valid data collector object!");
    return;
  }

  // Record
  double maxProcessingTimeSec = GetSamplingPeriodSec() * 2.0; // put a hard limit on the max processing time to make sure the application remains responsive during recording
  double requestedFramePeriodSec = 0.1;
  if (m_RequestedFrameRate > 0)
  {
    requestedFramePeriodSec = 1.0 / m_RequestedFrameRate;
  }
  else
  {
    LOG_WARNING("RequestedFrameRate is invalid");
  }
  int nbFramesBefore = m_RecordedFrames->GetNumberOfTrackedFrames();
  if ( m_ParentMainWindow->GetSelectedChannel()->GetTrackedFrameListSampled(m_RecordingLastAlreadyRecordedFrameTimestamp, m_RecordingNextFrameToBeRecordedTimestamp, m_RecordedFrames, requestedFramePeriodSec, maxProcessingTimeSec) != PLUS_SUCCESS )
  {
    LOG_ERROR("Error while getting tracked frame list from data collector during capturing. Last recorded timestamp: " << std::fixed << m_RecordingNextFrameToBeRecordedTimestamp ); 
  }
  int nbFramesAfter = m_RecordedFrames->GetNumberOfTrackedFrames();

  // Compute the average frame rate from the ratio of recently acquired frames
  int frame1Index = m_RecordedFrames->GetNumberOfTrackedFrames() - 1; // index of the latest frame
  int frame2Index = frame1Index - m_RequestedFrameRate * 5.0 - 1; // index of an earlier acquired frame (go back by approximately 5 seconds + one frame)
  if (frame2Index < m_RecordingFirstFrameIndexInThisSegment)
  {
    // make sure we stay in the current recording segment
    frame2Index = m_RecordingFirstFrameIndexInThisSegment;
  }
  if (frame1Index > frame2Index)
  {   
    TrackedFrame* frame1 = m_RecordedFrames->GetTrackedFrame(frame1Index);
    TrackedFrame* frame2 = m_RecordedFrames->GetTrackedFrame(frame2Index);
    if (frame1 != NULL && frame2 != NULL)
    {
      double frameTimeDiff = frame1->GetTimestamp() - frame2->GetTimestamp();
      if (frameTimeDiff > 0)
      {
        m_ActualFrameRate = (frame1Index - frame2Index) / frameTimeDiff;
      }
      else
      {
        m_ActualFrameRate = 0;
      }
    }    
  }

  // Check whether the recording needed more time than the sampling interval
  double recordingTimeSec = vtkAccurateTimer::GetSystemTime() - startTimeSec;
  if (recordingTimeSec > GetSamplingPeriodSec())
  {
    LOG_WARNING("Recording of frames takes too long time (" << recordingTimeSec << "sec instead of the allocated " << GetSamplingPeriodSec() << "sec). This can cause slow-down of the application and non-uniform sampling. Reduce the acquisition rate or sampling rate to resolve the problem.");
  }
  double recordingLagSec = vtkAccurateTimer::GetSystemTime() - m_RecordingNextFrameToBeRecordedTimestamp;
  if (recordingLagSec > MAX_ALLOWED_RECORDING_LAG_SEC)
  {
    LOG_ERROR("Recording cannot keep up with the acquisition. Skip " << recordingLagSec << " seconds of the data stream to catch up.");
    m_RecordingNextFrameToBeRecordedTimestamp = vtkAccurateTimer::GetSystemTime();
  }
}

//-----------------------------------------------------------------------------
void CapturingToolbox::Stop()
{
  LOG_INFO("Capturing stopped");

  m_RecordingTimer->stop();
  SetState(ToolboxState_Done);

  m_ParentMainWindow->SetToolboxesEnabled(true);
}

//-----------------------------------------------------------------------------
void CapturingToolbox::Save()
{
  LOG_TRACE("CapturingToolbox::Save"); 

  // TODO: just for testing
  std::string defaultFileName = m_LastSaveLocation + "/TrackedImageSequence_" + vtksys::SystemTools::GetCurrentDateTime("%Y%m%d_%H%M%S")+".mha";  
  WriteToFile(QString(defaultFileName.c_str()));

  LOG_INFO("Captured tracked frame list saved into '" << defaultFileName << "'");
}

//-----------------------------------------------------------------------------
void CapturingToolbox::SaveAs()
{
  LOG_TRACE("CapturingToolbox::SaveAs"); 

  std::string defaultFileName = m_LastSaveLocation+"/TrackedImageSequence_"+vtksys::SystemTools::GetCurrentDateTime("%Y%m%d_%H%M%S")+".mha";
  QString filter = QString( tr( "SequenceMetaFiles (*.mha *.mhd);;" ) );
  QString fileNameQt = QFileDialog::getSaveFileName(NULL, tr("Save captured tracked frames"), QString(defaultFileName.c_str()), filter);
  std::string fileName = fileNameQt.toLatin1().constData();
  m_LastSaveLocation = vtksys::SystemTools::GetFilenamePath(fileName.c_str());

  WriteToFile(QString(fileName.c_str()));

  LOG_INFO("Captured tracked frame list saved into '" << fileName << "'");
}

//-----------------------------------------------------------------------------
void CapturingToolbox::WriteToFile( const QString& aFilename )
{
  if (aFilename.isEmpty())
  {
    LOG_ERROR("Writing sequence to metafile failed: output file name is empty");
    return;
  }

  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

  // Actual saving
  if ( m_RecordedFrames->SaveToSequenceMetafile(aFilename.toLatin1().constData(), false) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to save tracked frames to sequence metafile!"); 
    return;
  }

  QString result = "File saved to\n"+aFilename;
  ui.plainTextEdit_saveResult->clear();
  ui.plainTextEdit_saveResult->insertPlainText(result);

  // Add file name to image list in Volume reconstruction toolbox
  VolumeReconstructionToolbox* volumeReconstructionToolbox = dynamic_cast<VolumeReconstructionToolbox*>(m_ParentMainWindow->GetToolbox(ToolboxType_VolumeReconstruction));
  if (volumeReconstructionToolbox != NULL)
  {
    volumeReconstructionToolbox->AddImageFileName(aFilename);
  }

  // Write the current state into the device set configuration XML
  m_ParentMainWindow->GetVisualizationController()->WriteConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData());

  // Save config file next to the tracked frame list

  std::string path = vtksys::SystemTools::GetFilenamePath(aFilename.toLatin1().constData()); 
  std::string filename = vtksys::SystemTools::GetFilenameWithoutExtension(aFilename.toLatin1().constData());
  std::string configFileName = path + "/" + filename + "_config.xml";
  PlusCommon::PrintXML(configFileName.c_str(), vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData());

  m_RecordedFrames->Clear(); 
  SetState(ToolboxState_Idle);

  QApplication::restoreOverrideCursor();
  	
}


//-----------------------------------------------------------------------------
void CapturingToolbox::ClearRecordedFrames()
{
  LOG_TRACE("CapturingToolbox::ClearRecordedFrames"); 

  if ( QMessageBox::question(this, tr("fCal - Capturing"), tr("Do you want to discard all the recorded frames?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No )
  {
    return; 
  }

  ClearRecordedFramesInternal();

  LOG_INFO("Recorded frames cleared");
}

//-----------------------------------------------------------------------------
void CapturingToolbox::SamplingRateChanged(int aValue)
{
  LOG_TRACE("CapturingToolbox::RecordingFrameRateChanged(" << aValue << ")"); 

  double maxFrameRate = GetMaximumFrameRate();
  int samplingRate = (int)(pow(2.0, ui.horizontalSlider_SamplingRate->maximum() - aValue));

  if (samplingRate>0)
  {
    m_RequestedFrameRate = maxFrameRate / (double)samplingRate;
  }
  else
  {
    LOG_WARNING("samplingRate value is invalid");
    m_RequestedFrameRate = maxFrameRate;
  }

  ui.horizontalSlider_SamplingRate->setToolTip(tr("1 / ").append(QString::number((int)samplingRate)));
  ui.label_RequestedRecordingFrameRate->setText(QString::number(m_RequestedFrameRate, 'f', 2));

  LOG_INFO("Sampling rate changed to " << aValue << " (matching requested frame rate is " << m_RequestedFrameRate << ")");
}

//-----------------------------------------------------------------------------
double CapturingToolbox::GetMaximumFrameRate()
{
  LOG_TRACE("CapturingToolbox::GetMaximumFrameRate");

  if (m_ParentMainWindow == NULL || m_ParentMainWindow->GetSelectedChannel() == NULL )
  {
    LOG_ERROR("Unable to reach valid data collector object!");
    return 0.0;
  }

  return m_ParentMainWindow->GetSelectedChannel()->GetOwnerDevice()->GetAcquisitionRate();
}

//-----------------------------------------------------------------------------
void CapturingToolbox::Reset()
{
  AbstractToolbox::Reset();

  this->ClearRecordedFramesInternal();
}

//-----------------------------------------------------------------------------
void CapturingToolbox::ClearRecordedFramesInternal()
{
  m_RecordedFrames->Clear();

  SetState(ToolboxState_Idle);
}

//-----------------------------------------------------------------------------
double CapturingToolbox::GetSamplingPeriodSec()
{
  double samplingPeriodSec=0.1;
  if (m_SamplingFrameRate>0)
  {
    samplingPeriodSec=1.0/m_SamplingFrameRate;
  }
  else
  {
    LOG_WARNING("m_SamplingFrameRate value is invalid "<<m_SamplingFrameRate<<". Use default sampling period of "<<samplingPeriodSec<<" sec");
  }
  return samplingPeriodSec;
}

//-----------------------------------------------------------------------------
void CapturingToolbox::HandleStatusMessage( const std::string& aMessage )
{
  ui.plainTextEdit_saveResult->clear();
  QString message(aMessage.c_str());
  ui.plainTextEdit_saveResult->insertPlainText(message);
}

//-----------------------------------------------------------------------------
void CapturingToolbox::StartStopAll()
{
  QString text = ui.pushButton_StartStopAll->text();
  bool enable(true);
  if( QString::compare(text, QString("Record All")) == 0 )
  {
    ui.pushButton_StartStopAll->setText(QString("Stop All"));
    ui.pushButton_StartStopAll->setIcon( QPixmap( ":/icons/Resources/icon_Stop.png" ) );
  }
  else
  {
    ui.pushButton_StartStopAll->setText(QString("Record All"));
    ui.pushButton_StartStopAll->setIcon( QPixmap( ":/icons/Resources/icon_Record.png" ) );
    enable = false;
  }

  for( std::vector<CaptureControlWidget*>::iterator it = m_CaptureWidgets.begin(); it != m_CaptureWidgets.end(); ++it )
  {
    CaptureControlWidget* widget = *it;
    widget->SetEnableCapturing(enable);
  }
}

//-----------------------------------------------------------------------------
void CapturingToolbox::ClearAll()
{
  for( std::vector<CaptureControlWidget*>::iterator it = m_CaptureWidgets.begin(); it != m_CaptureWidgets.end(); ++it )
  {
    CaptureControlWidget* widget = *it;
    widget->Clear();
  }
}

//-----------------------------------------------------------------------------
void CapturingToolbox::SaveAll()
{
  for( std::vector<CaptureControlWidget*>::iterator it = m_CaptureWidgets.begin(); it != m_CaptureWidgets.end(); ++it )
  {
    CaptureControlWidget* widget = *it;
    widget->SaveFile();
  }
}

//-----------------------------------------------------------------------------
void CapturingToolbox::OnDeactivated()
{
  for( std::vector<CaptureControlWidget*>::iterator it = m_CaptureWidgets.begin(); it != m_CaptureWidgets.end(); ++it )
  {
    disconnect((*it), SIGNAL(EmitStatusMessage(const std::string&)), this, SLOT(HandleStatusMessage(const std::string&)) );
    ui.captureWidgetLayout->removeWidget(*it);
    delete *it;
  }

  this->m_CaptureWidgets.clear();
}
