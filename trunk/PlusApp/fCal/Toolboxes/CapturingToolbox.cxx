/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "CapturingToolbox.h"

#include "fCalMainWindow.h"
#include "vtkToolVisualizer.h"
#include "VolumeReconstructionToolbox.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>

#include "vtkTrackedFrameList.h"
#include "vtkPlusVideoSource.h"

#include "vtksys/SystemTools.hxx"

//-----------------------------------------------------------------------------

CapturingToolbox::CapturingToolbox(fCalMainWindow* aParentMainWindow, Qt::WFlags aFlags)
	: AbstractToolbox(aParentMainWindow)
	, QWidget(aParentMainWindow, aFlags)
	, m_RecordedFrames(NULL)
  , m_RecordingTimer(NULL)
  , m_LastRecordedFrameTimestamp(0.0)
  , m_NumberOfIncomingFramesSinceStart(0)
  , m_SamplingRate(1)
  , m_SamplingFrameRate(8)
  , m_ActualFrameRate(0)
  , m_DefaultFrameTransformName("Probe")
{
	ui.setupUi(this);

  // Create tracked frame list
  m_RecordedFrames = vtkTrackedFrameList::New();
  m_RecordedFrames->SetDefaultFrameTransformName(m_DefaultFrameTransformName.c_str());
  m_RecordedFrames->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP | REQUIRE_TRACKING_OK); 

  // Connect events
	connect( ui.pushButton_Snapshot, SIGNAL( clicked() ), this, SLOT( TakeSnapshot() ) );
	connect( ui.pushButton_Record, SIGNAL( clicked() ), this, SLOT( Record() ) );
	connect( ui.pushButton_ClearRecordedFrames, SIGNAL( clicked() ), this, SLOT( ClearRecordedFrames() ) );
	connect( ui.pushButton_Save, SIGNAL( clicked() ), this, SLOT( Save() ) );

	connect( ui.horizontalSlider_SamplingRate, SIGNAL( valueChanged(int) ), this, SLOT( SamplingRateChanged(int) ) );

  // Create and connect recording timer
  m_RecordingTimer = new QTimer(this); 
  connect(m_RecordingTimer, SIGNAL(timeout()), this, SLOT(Capture()) );
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

void CapturingToolbox::Initialize()
{
	LOG_TRACE("CapturingToolbox::Initialize"); 

  if ((m_ParentMainWindow->GetToolVisualizer()->GetDataCollector() != NULL) && (m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetConnected()))
  {
    m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->SetTrackingOnly(false);

    // Set initialized if it was uninitialized
    if (m_State == ToolboxState_Uninitialized)
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

  m_ParentMainWindow->GetToolVisualizer()->HideAll();
  m_ParentMainWindow->GetToolVisualizer()->EnableImageMode(true);

	if (m_State == ToolboxState_Uninitialized)
  {
		ui.pushButton_Snapshot->setEnabled(false);
    ui.pushButton_Record->setEnabled(false);
    ui.pushButton_ClearRecordedFrames->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);
    ui.horizontalSlider_SamplingRate->setEnabled(false);
	}
  else if (m_State == ToolboxState_Idle)
  {
    ui.pushButton_Record->setText("Record");
    ui.pushButton_Record->setIcon( QPixmap( ":/icons/Resources/icon_Record.png" ) );

		ui.pushButton_Snapshot->setEnabled(true);
    ui.pushButton_Record->setEnabled(true);
    ui.pushButton_ClearRecordedFrames->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);
    ui.horizontalSlider_SamplingRate->setEnabled(true);

    SamplingRateChanged(ui.horizontalSlider_SamplingRate->value());

    int maxFrameRate = GetMaximumFrameRate();
    ui.label_ActualRecordingFrameRate->setText(tr("0.0"));
    ui.label_MaximumRecordingFrameRate->setText(QString::number(maxFrameRate));

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
    ui.horizontalSlider_SamplingRate->setEnabled(true);

    int maxFrameRate = m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetVideoSource()->GetFrameRate();
    ui.label_ActualRecordingFrameRate->setText(tr("0.0"));
    ui.label_MaximumRecordingFrameRate->setText(QString::number(maxFrameRate));

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
    ui.horizontalSlider_SamplingRate->setEnabled(false);
	}
}

//-----------------------------------------------------------------------------

void CapturingToolbox::TakeSnapshot()
{
	LOG_TRACE("CapturingToolbox::TakeSnapshot"); 

	TrackedFrame trackedFrame;

  if (m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetTrackedFrame(&trackedFrame) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to get tracked frame for the snapshot!");
    return;
  }

	if ( trackedFrame.GetStatus() != TR_OK )
	{
		LOG_WARNING("Unable to record tracked frame: Tracker out of view!"); 
    return;
	}

  // Add tracked frame to the list
  m_RecordedFrames->AddTrackedFrame(&trackedFrame, vtkTrackedFrameList::SKIP_INVALID_FRAME);

  ui.label_NumberOfRecordedFrames->setText(QString::number(m_RecordedFrames->GetNumberOfTrackedFrames()));
}

//-----------------------------------------------------------------------------

void CapturingToolbox::Record()
{
	LOG_TRACE("CapturingToolbox::Record"); 

  m_ParentMainWindow->SetTabsEnabled(false);

  // Reset accessory members
  m_NumberOfIncomingFramesSinceStart = 0;
  m_RecordedFrameNumberQueue.clear();

  vtkDataCollector* dataCollector = NULL;
  if ( (m_ParentMainWindow == NULL) || (m_ParentMainWindow->GetToolVisualizer() == NULL) || ((dataCollector = m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()) == NULL) )
  {
    LOG_ERROR("Unable to reach valid data collector object!");
    return;
  }

  dataCollector->GetMostRecentTimestamp(m_LastRecordedFrameTimestamp);

  // Start capturing
  SetState(ToolboxState_InProgress);
  m_RecordingTimer->start(1000.0 / m_SamplingFrameRate); 
}

//-----------------------------------------------------------------------------

void CapturingToolbox::Capture()
{
	//LOG_TRACE("CapturingToolbox::Capture");

  double startTimeSec = vtkAccurateTimer::GetSystemTime();

  vtkTrackedFrameList* acquiredFrames = vtkTrackedFrameList::New();
  acquiredFrames->SetDefaultFrameTransformName(m_DefaultFrameTransformName.c_str());

  vtkDataCollector* dataCollector = NULL;
  if ( (m_ParentMainWindow == NULL) || (m_ParentMainWindow->GetToolVisualizer() == NULL) || ((dataCollector = m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()) == NULL) )
  {
    LOG_ERROR("Unable to reach valid data collector object!");
    return;
  }

  // Record
  if ( dataCollector->GetTrackedFrameList(m_LastRecordedFrameTimestamp, acquiredFrames) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get tracked frame list from data collector (last recorded timestamp: " << std::fixed << m_LastRecordedFrameTimestamp ); 
    return; 
  }

  int numberOfAcquiredFrames = acquiredFrames->GetNumberOfTrackedFrames();
  int numberOfRecordedFrames = 0;

  // Add tracked frames to the recorded list according to the sampling rate
  for (int i=0; i<numberOfAcquiredFrames; ++i)
  {
    if ( (m_NumberOfIncomingFramesSinceStart + i + 1) % m_SamplingRate == 0 )
    {
      m_RecordedFrames->AddTrackedFrame(acquiredFrames->GetTrackedFrame(i));
      numberOfRecordedFrames++;
    }
  }

  acquiredFrames->Delete();

  m_NumberOfIncomingFramesSinceStart += numberOfAcquiredFrames;

  // Compute actual frame rate
  m_RecordedFrameNumberQueue.push_back(numberOfRecordedFrames);

  while (m_RecordedFrameNumberQueue.size() > m_SamplingFrameRate * 2)
  {
    m_RecordedFrameNumberQueue.pop_front();
  }

  int numberOfRecordedFramesInTheLastTwoSeconds = 0;
  for (std::deque<int>::iterator it = m_RecordedFrameNumberQueue.begin(); it != m_RecordedFrameNumberQueue.end(); ++it)
  {
    numberOfRecordedFramesInTheLastTwoSeconds += (*it);
  }

  m_ActualFrameRate = (double)numberOfRecordedFramesInTheLastTwoSeconds / (m_RecordedFrameNumberQueue.size() * (1.0 / m_SamplingFrameRate));

  // Check whether the recording needed more time than the sampling interval
  double recordingTimeMs = (vtkAccurateTimer::GetSystemTime() - startTimeSec) * 1000.0;
  if (recordingTimeMs > 1000.0 / m_SamplingFrameRate)
  {
    LOG_WARNING("Recording cannot keep up with aquisition!");
  }
}

//-----------------------------------------------------------------------------

void CapturingToolbox::Stop()
{
	LOG_TRACE("CapturingToolbox::Stop"); 

  m_RecordingTimer->stop();
  SetState(ToolboxState_Done);

  m_ParentMainWindow->SetTabsEnabled(true);
}

//-----------------------------------------------------------------------------

void CapturingToolbox::Save()
{
	LOG_TRACE("CapturingToolbox::Save"); 

	QString filter = QString( tr( "SequenceMetaFiles (*.mha *.mhd);;" ) );
  QString fileName = QFileDialog::getSaveFileName(NULL, tr("Save captured tracked frames"), QString(vtkPlusConfig::GetInstance()->GetImageDirectory()), filter);

	if (! fileName.isNull() )
  {
		QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

    // Actual saving
    std::string filePath = fileName.ascii();
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

    // Add file name to image list in Volume reconstruction toolbox
    VolumeReconstructionToolbox* volumeReconstructionToolbox = dynamic_cast<VolumeReconstructionToolbox*>(m_ParentMainWindow->GetToolbox(ToolboxType_VolumeReconstruction));
    if (volumeReconstructionToolbox != NULL)
    {
      volumeReconstructionToolbox->AddImageFileName(fileName);
    }

	  m_RecordedFrames->Clear(); 
    SetState(ToolboxState_Idle);

		QApplication::restoreOverrideCursor();
	}	
}

//-----------------------------------------------------------------------------

void CapturingToolbox::ClearRecordedFrames()
{
	LOG_TRACE("CapturingToolbox::ClearRecordedFrames"); 

  if (QMessageBox::question(this, tr("fCal - Capturing"), tr("Dou you want to discard all the recorded frames?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No)
    == QMessageBox::No)
  {
    return; 
  }

  m_RecordedFrames->Clear();

  SetState(ToolboxState_Idle);
}

//-----------------------------------------------------------------------------

void CapturingToolbox::SamplingRateChanged(int aValue)
{
	LOG_TRACE("CapturingToolbox::RecordingFrameRateChanged(" << aValue << ")"); 

  double maxFrameRate = GetMaximumFrameRate();
  int samplingRate = (int)(pow(2.0, ui.horizontalSlider_SamplingRate->maxValue() - aValue));
  double requestedFrameRate = maxFrameRate / (double)samplingRate;

  m_SamplingRate = samplingRate;

  ui.horizontalSlider_SamplingRate->setToolTip(tr("1 / ").append(QString::number((int)samplingRate)));
  ui.label_RequestedRecordingFrameRate->setText(QString::number(requestedFrameRate, 'f', 2));
}

//-----------------------------------------------------------------------------

double CapturingToolbox::GetMaximumFrameRate()
{
	LOG_TRACE("CapturingToolbox::GetMaximumFrameRate");

  if (m_ParentMainWindow == NULL || m_ParentMainWindow->GetToolVisualizer() == NULL || m_ParentMainWindow->GetToolVisualizer()->GetDataCollector() == NULL)
  {
    LOG_ERROR("Unable to reach valid data collector object!");
    return 0.0;
  }
  
  if ( m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetVideoEnabled()
    && m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetVideoSource())
  {
    return m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetVideoSource()->GetFrameRate();
  }
  else if (m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetTracker())
  {
    return m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetTracker()->GetFrequency();
  }
  else
  {
    return 0.0;
  }
}
