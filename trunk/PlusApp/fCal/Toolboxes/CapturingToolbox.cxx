/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "CapturingToolbox.h"

#include "fCalMainWindow.h"
#include "vtkToolVisualizer.h"

#include <QFileDialog>

#include "vtkTrackedFrameList.h"
#include "vtkPlusVideoSource.h"

//-----------------------------------------------------------------------------

CapturingToolbox::CapturingToolbox(fCalMainWindow* aParentMainWindow, Qt::WFlags aFlags)
	: AbstractToolbox(aParentMainWindow)
	, QWidget(aParentMainWindow, aFlags)
	, m_RecordedFrames(NULL)
  , m_LastRecordedFrameTimestamp(0.0)
  , m_CancelRequest(false)
  , m_RequestedFrameRate(-1.0)
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
    ui.label_ActualRecordingFrameRate->setText(tr("sajt"));
  }
}

//-----------------------------------------------------------------------------

void CapturingToolbox::SetDisplayAccordingToState()
{
  LOG_TRACE("CapturingToolbox::SetDisplayAccordingToState");

  m_ParentMainWindow->GetToolVisualizer()->HideAll();

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
    ui.pushButton_Save->setIcon( QIcon( ":/icons/Resources/icon_Record.png" ) );

		ui.pushButton_Snapshot->setEnabled(true);
    ui.pushButton_Record->setEnabled(true);
    ui.pushButton_ClearRecordedFrames->setEnabled(false);
		ui.pushButton_Save->setEnabled(false);
    ui.horizontalSlider_SamplingRate->setEnabled(true);
    ui.horizontalSlider_SamplingRate->setValue(10);

    int maxFrameRate = GetMaximumFrameRate();
    m_RequestedFrameRate = maxFrameRate;
    ui.label_RequestedRecordingFrameRate->setText(QString::number(maxFrameRate));
    ui.label_ActualRecordingFrameRate->setText(tr("0.0"));
    ui.label_MaximumRecordingFrameRate->setText(QString::number(maxFrameRate));
	}
  else if (m_State == ToolboxState_InProgress)
  {
    ui.pushButton_Record->setText("Stop");
    ui.pushButton_Save->setIcon( QIcon( ":/icons/Resources/icon_Stop.png" ) );

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
    ui.pushButton_Record->setText("Record");
    ui.pushButton_Save->setIcon( QIcon( ":/icons/Resources/icon_Record.png" ) );

		ui.pushButton_Snapshot->setEnabled(true);
    ui.pushButton_Record->setEnabled(true);
    ui.pushButton_ClearRecordedFrames->setEnabled(true);
		ui.pushButton_Save->setEnabled(true);
    ui.horizontalSlider_SamplingRate->setEnabled(true);

    int maxFrameRate = m_ParentMainWindow->GetToolVisualizer()->GetDataCollector()->GetVideoSource()->GetFrameRate();
    ui.label_ActualRecordingFrameRate->setText(tr("0.0"));
    ui.label_MaximumRecordingFrameRate->setText(QString::number(maxFrameRate));

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

void CapturingToolbox::Save()
{
	LOG_TRACE("CapturingToolbox::Save"); 

	QString filter = QString( tr( "VTK files ( *.vtk );;" ) );
	QString fileName = QFileDialog::getSaveFileName(NULL, tr("Save reconstructed volume"), "", filter);

	if (! fileName.isNull() )
  {
		QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

    // Actual saving

		QApplication::restoreOverrideCursor();
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
}

//-----------------------------------------------------------------------------

void CapturingToolbox::Record()
{
	LOG_TRACE("CapturingToolbox::Record"); 

  m_ParentMainWindow->SetTabsEnabled(false);

  SetState(ToolboxState_InProgress);
}

//-----------------------------------------------------------------------------

void CapturingToolbox::Capture()
{
	//LOG_TRACE("CapturingToolbox::Capture");

  if (m_CancelRequest)
  {
    m_CancelRequest = false;

    SetState(ToolboxState_Done);
    return;
  }
}

//-----------------------------------------------------------------------------

void CapturingToolbox::Stop()
{
	LOG_TRACE("CapturingToolbox::Stop"); 

  m_ParentMainWindow->SetTabsEnabled(true);

  m_CancelRequest = true;
}

//-----------------------------------------------------------------------------

void CapturingToolbox::ClearRecordedFrames()
{
	LOG_TRACE("CapturingToolbox::ClearRecordedFrames"); 
}

//-----------------------------------------------------------------------------

void CapturingToolbox::SamplingRateChanged(int aValue)
{
	LOG_TRACE("CapturingToolbox::RecordingFrameRateChanged(" << aValue << ")"); 

  double maxFrameRate = GetMaximumFrameRate();
  double requestedFrameRate = maxFrameRate / (double)(pow(2.0, 10-aValue));

  m_RequestedFrameRate = requestedFrameRate;

  ui.label_RequestedRecordingFrameRate->setText(QString::number(maxFrameRate, 'f', 2));
}

//-----------------------------------------------------------------------------

double CapturingToolbox::GetMaximumFrameRate()
{
	LOG_TRACE("CapturingToolbox::GetMaximumFrameRate");

  if (m_ParentMainWindow == NULL || m_ParentMainWindow->GetToolVisualizer() == NULL || m_ParentMainWindow->GetToolVisualizer()->GetDataCollector() == NULL)
  {
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
