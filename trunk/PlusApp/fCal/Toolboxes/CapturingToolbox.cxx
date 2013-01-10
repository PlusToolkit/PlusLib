/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

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

//-----------------------------------------------------------------------------

CapturingToolbox::CapturingToolbox(fCalMainWindow* aParentMainWindow, Qt::WFlags aFlags)
: AbstractToolbox(aParentMainWindow)
, QWidget(aParentMainWindow, aFlags)
, m_RecordedFrames(NULL)
, m_RecordingTimer(NULL)
, m_LastRecordedFrameTimestamp(0.0)
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

  connect( ui.horizontalSlider_SamplingRate, SIGNAL( valueChanged(int) ), this, SLOT( SamplingRateChanged(int) ) );

  // Create and connect recording timer
  m_RecordingTimer = new QTimer(this); 
  connect(m_RecordingTimer, SIGNAL(timeout()), this, SLOT(Capture()) );

  ui.pushButton_Save->setEnabled(m_RecordedFrames->GetNumberOfTrackedFrames() > 0);
  ui.pushButton_SaveAs->setEnabled(m_RecordedFrames->GetNumberOfTrackedFrames() > 0);

  m_LastSaveLocation = vtkPlusConfig::GetInstance()->GetImageDirectory();
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
        LOG_WARNING("Unable to switch to 2D visualization. Unable to use capturing toolbox.");
        m_ParentMainWindow->GetVisualizationController()->HideRenderer();
        this->m_State = ToolboxState_Error;
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
    if (m_ParentMainWindow->GetVisualizationController()->GetDataCollector()->GetTrackingDataAvailable())
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
            char imageToProbeTransformErrorChars[32];
            SNPRINTF(imageToProbeTransformErrorChars, 32, "%.3lf", error);
            errorStr = imageToProbeTransformErrorChars;
          }
          else
          {
            errorStr = "N/A";
          }

          ui.label_State->setPaletteForegroundColor(Qt::black);
          ui.label_State->setText( QString("%1 transform present, ready for capturing. \nDate: %2, Error: %3").arg(imageToProbeTransformNameStr.c_str()).arg(date.c_str()).arg(errorStr.c_str()) );
        }
        else
        {
          ui.label_State->setPaletteForegroundColor(QColor::fromRgb(255, 128, 0));
          ui.label_State->setText( QString("%1 transform is absent, spatial calibration needs to be performed or imported.").arg(imageToProbeTransformNameStr.c_str()) );
          LOG_INFO(imageToProbeTransformNameStr << " transform is absent, spatial calibration needs to be performed or imported.");
        }
      }
      else
      {
        ui.label_State->setPaletteForegroundColor(QColor::fromRgb(255, 128, 0));
        ui.label_State->setText( QString("fCal configuration element does not contain both ImageCoordinateFrame and ProbeCoordinateFrame attributes!") );
        LOG_INFO("fCal configuration element does not contain both ImageCoordinateFrame and ProbeCoordinateFrame attributes");
      }
    }
    else
    {
      ui.label_State->setPaletteForegroundColor(Qt::black);
      ui.label_State->setText( QString("Tracking is not enabled.") );
      LOG_INFO("Tracking is not enabled.");
    }
  }
  else
  {
    ui.label_State->setPaletteForegroundColor(QColor::fromRgb(255, 128, 0));
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

    ui.label_ActualRecordingFrameRate->setText(tr("0.0"));
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

    ui.label_ActualRecordingFrameRate->setText(tr("0.0"));
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

  if (m_ParentMainWindow->GetVisualizationController()->GetDataCollector()->GetTrackedFrame(&trackedFrame) != PLUS_SUCCESS)
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

  // Reset accessory members
  m_FirstRecordedFrameIndexInThisSegment=m_RecordedFrames->GetNumberOfTrackedFrames();

  vtkDataCollector* dataCollector = NULL;
  if ( (m_ParentMainWindow == NULL) || (m_ParentMainWindow->GetVisualizationController() == NULL) || ((dataCollector = m_ParentMainWindow->GetVisualizationController()->GetDataCollector()) == NULL) )
  {
    LOG_ERROR("Unable to reach valid data collector object!");
    return;
  }

  ui.plainTextEdit_saveResult->clear();

  dataCollector->GetMostRecentTimestamp(m_LastRecordedFrameTimestamp);

  // Start capturing
  SetState(ToolboxState_InProgress);
  m_RecordingTimer->start(GetSamplingPeriodMsec()); 
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
  double maxProcessingTimeSec = (GetSamplingPeriodMsec()*1000.0) * 2; // put a hard limit on the max processing time to make sure the application remains responsive during recording (as a maximum of 2x the update timer period)
  double requestedFramePeriodSec=0.1;
  if (m_RequestedFrameRate>0)
  {
    requestedFramePeriodSec=1.0 / m_RequestedFrameRate;
  }
  else
  {
    LOG_WARNING("RequestedFrameRate is invalid");
  }
  if ( dataCollector->GetTrackedFrameListSampled(m_LastRecordedFrameTimestamp, m_RecordedFrames, requestedFramePeriodSec, maxProcessingTimeSec) != PLUS_SUCCESS )
  {
    LOG_ERROR("Error while gettig tracked frame list from data collector during capturing. Last recorded timestamp: " << std::fixed << m_LastRecordedFrameTimestamp ); 
  }

  // Compute the average frame rate from the ratio of recently acquired frames
  int frame1Index=m_RecordedFrames->GetNumberOfTrackedFrames()-1; // index of the latest frame
  int frame2Index=frame1Index-m_RequestedFrameRate*2.0-1; // index of an earlier acquired frame (go back by approximately 2 seconds + one frame)
  if (frame2Index<m_FirstRecordedFrameIndexInThisSegment)
  {
    // make sure we stay in the current recording segment
    frame2Index=m_FirstRecordedFrameIndexInThisSegment;
  }
  if (frame1Index>frame2Index)
  {   
    TrackedFrame *frame1=m_RecordedFrames->GetTrackedFrame(frame1Index);
    TrackedFrame *frame2=m_RecordedFrames->GetTrackedFrame(frame2Index);
    if (frame1!=NULL && frame2!=NULL)
    {
      double frameTimeDiff=frame1->GetTimestamp()-frame2->GetTimestamp();
      if (frameTimeDiff>0)
      {
        m_ActualFrameRate = (frame1Index-frame2Index)/frameTimeDiff;
      }
      else
      {
        m_ActualFrameRate=0;
      }
    }    
  }

  // Check whether the recording needed more time than the sampling interval
  double recordingTimeMs = (vtkAccurateTimer::GetSystemTime() - startTimeSec) * 1000.0;
  if (recordingTimeMs > GetSamplingPeriodMsec())
  {
    LOG_WARNING("Recording of frames takes too long time ("<<recordingTimeMs<<"ms instead of the allocated "<<GetSamplingPeriodMsec()<<"ms). This can cause slow-down of the application and non-uniform sampling. Reduce the acquisition rate or sampling rate to resolve the problem.");
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

  QString fileName = QString("%1/TrackedImageSequence_%2").arg(m_LastSaveLocation).arg(vtksys::SystemTools::GetCurrentDateTime("%Y%m%d_%H%M%S").c_str());

  m_LastSaveLocation = fileName.mid(0, fileName.lastIndexOf('/'));

  WriteToFile(fileName);

  LOG_INFO("Captured tracked frame list saved into '" << fileName.toAscii().data() << "'");
}

//-----------------------------------------------------------------------------

void CapturingToolbox::SaveAs()
{
  LOG_TRACE("CapturingToolbox::SaveAs"); 

  QString filter = QString( tr( "SequenceMetaFiles (*.mha *.mhd);;" ) );
  QString fileName;

  fileName = QFileDialog::getSaveFileName(NULL, tr("Save captured tracked frames"),
    QString("%1/TrackedImageSequence_%2").arg(m_LastSaveLocation).arg(vtksys::SystemTools::GetCurrentDateTime("%Y%m%d_%H%M%S").c_str()), filter);
  m_LastSaveLocation = fileName.mid(0, fileName.lastIndexOf('/'));

  WriteToFile(fileName);

  LOG_INFO("Captured tracked frame list saved into '" << fileName.toAscii().data() << "'");
}

//-----------------------------------------------------------------------------

void CapturingToolbox::WriteToFile( QString& aFilename )
{
  if (! aFilename.isNull() )
  {
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

    // Actual saving
    std::string filePath = aFilename.toAscii();
    std::string path = vtksys::SystemTools::GetFilenamePath(filePath); 
    std::string filename = vtksys::SystemTools::GetFilenameWithoutExtension(filePath); 
    std::string extension = vtksys::SystemTools::GetFilenameExtension(filePath); 

    vtkTrackedFrameList::SEQ_METAFILE_EXTENSION ext(vtkTrackedFrameList::SEQ_METAFILE_MHA); 
    if ( STRCASECMP(".mhd", extension.c_str() ) == 0 )
    {
      ext = vtkTrackedFrameList::SEQ_METAFILE_MHD; 
    }
    else
    {
      ext = vtkTrackedFrameList::SEQ_METAFILE_MHA; 
    }

    if ( m_RecordedFrames->SaveToSequenceMetafile(path.c_str(), filename.c_str(), ext, false) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to save tracked frames to sequence metafile!"); 
      return;
    }

    QString result = "File saved to\n";
    result += aFilename;
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
    std::string configFileName = path + "/" + filename + "_config.xml";
    PlusCommon::PrintXML(configFileName.c_str(), vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData());

    m_RecordedFrames->Clear(); 
    SetState(ToolboxState_Idle);

    QApplication::restoreOverrideCursor();
  }	
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
  int samplingRate = (int)(pow(2.0, ui.horizontalSlider_SamplingRate->maxValue() - aValue));

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

  if (m_ParentMainWindow == NULL || m_ParentMainWindow->GetVisualizationController() == NULL || m_ParentMainWindow->GetVisualizationController()->GetDataCollector() == NULL)
  {
    LOG_ERROR("Unable to reach valid data collector object!");
    return 0.0;
  }

  double frameRate = 0.0;
  if (m_ParentMainWindow->GetVisualizationController()->GetDataCollector()->GetFrameRate(frameRate) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to get frame rate from data collector!");
  }

  return frameRate;
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

double CapturingToolbox::GetSamplingPeriodMsec()
{
  double samplingPeriodMsec=100;
  if (m_SamplingFrameRate>0)
  {
    samplingPeriodMsec=1000.0/m_SamplingFrameRate;
  }
  else
  {
    LOG_WARNING("m_SamplingFrameRate value is invalid "<<m_SamplingFrameRate);
  }
  return samplingPeriodMsec;
}
