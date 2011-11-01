/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "TrackedUltrasoundCapturingGUI.h"
#include "TrackedUltrasoundCapturing.h"
#include "vtksys/SystemTools.hxx"
#include "vtkTrackerTool.h"
#include "vtkCommand.h"
#include "vtkRenderWindow.h"
#include <string>

// QT includes
#include <qapplication.h>
#include <qstring.h>			
#include <qfont.h>
#include <qlabel.h>
#include <QFileDialog>
#include <QTimer>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QSettings>

#include "DeviceSetSelectorWidget.h"
#include "ToolStateDisplayWidget.h"
#include "StatusIcon.h"

const QString LABEL_RECORDING_FRAME_RATE("Recording Frame Rate:");
const QString LABEL_SYNC_VIDEO_OFFSET("Video offset:");



class vtkInteractorCallback : public vtkCommand
{
public:
  static vtkInteractorCallback *New()
  {
    vtkInteractorCallback *cb = new vtkInteractorCallback;
    return cb;
  }

  virtual void Execute(vtkObject *vtkNotUsed(caller), unsigned long eventId,
    void *vtkNotUsed(callData))
  {
    TrackedUltrasoundCapturingGUI* tucGUI = TrackedUltrasoundCapturingGUI::GetInstance(); 

    if (vtkCommand::CharEvent == eventId)
    {
      //char keycode = tucGUI->GetRenderer(tucGUI->currentPage())->GetRenderWindow()->GetInteractor()->GetKeyCode(); 
    }
  }
};

TrackedUltrasoundCapturingGUI* TrackedUltrasoundCapturingGUI::Instance = 0;

//----------------------------------------------------------------------
TrackedUltrasoundCapturingGUI* TrackedUltrasoundCapturingGUI::New()
{
  return TrackedUltrasoundCapturingGUI::GetInstance();
}

//----------------------------------------------------------------------
TrackedUltrasoundCapturingGUI* TrackedUltrasoundCapturingGUI::GetInstance()
{
  if(!TrackedUltrasoundCapturingGUI::Instance)
  {
    TrackedUltrasoundCapturingGUI::Instance = new TrackedUltrasoundCapturingGUI;	   
  }
  // return the instance
  return TrackedUltrasoundCapturingGUI::Instance;
}

//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::SetInstance(TrackedUltrasoundCapturingGUI* instance)
{
  if (TrackedUltrasoundCapturingGUI::Instance==instance)
  {
    return;
  }
  // preferably this will be NULL
  if (TrackedUltrasoundCapturingGUI::Instance)
  {
    delete TrackedUltrasoundCapturingGUI::Instance;
  }

  TrackedUltrasoundCapturingGUI::Instance = instance;
  if (!instance)
  {
    return;
  }
}

//----------------------------------------------------------------------
TrackedUltrasoundCapturingGUI::TrackedUltrasoundCapturingGUI( QWidget* parent ) 
: 
QWizard(parent)
{
  this->Initialized = false; 

  // Create the UI widgets 
  setupUi(this);

  // Create device set selector widget
  this->m_DeviceSetSelectorWidget = new DeviceSetSelectorWidget(this);

  // Make connections
  connect( m_DeviceSetSelectorWidget, SIGNAL( ConnectToDevicesByConfigFileInvoked(std::string) ), this, SLOT( ConnectToDevicesByConfigFile(std::string) ) );
  
  // Setup device set selector widget
  this->m_DeviceSetSelectorWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  this->m_DeviceSetSelectorWidget->SetComboBoxMinWidth(0); 
  this->m_DeviceSetSelectorWidget->resize(this->width(), this->height()); 

  // Create and setup tool display widget
  this->m_SyncToolStateDisplayWidget = new ToolStateDisplayWidget(this);
  this->m_SyncToolStateDisplayWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
  this->m_RecordingToolStateDisplayWidget = new ToolStateDisplayWidget(this);
  this->m_RecordingToolStateDisplayWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

  // Insert widgets into placeholders
  this->GridDeviceSetSelection->addWidget(m_DeviceSetSelectorWidget);
  this->RecordingGridToolStateDisplay->addWidget(m_RecordingToolStateDisplayWidget); 
  this->SyncGridToolStateDisplay->addWidget(m_SyncToolStateDisplayWidget); 
  
  // Hide buttons/labels
  this->StopButton->hide(); 
  this->CancelSyncButton->hide(); 
  this->CapturingProgressBar->hide(); 
  this->SyncProgressBar->hide(); 

  // Create timer event to update widgets
  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(UpdateWidgets()) );
  timer->start(50);

  // Timer event for recording
  this->m_RecordingTimer = new QTimer(this); 
  connect(this->m_RecordingTimer, SIGNAL(timeout()), this, SLOT(RecordTrackedFrame()) );

  // Connect signals and slots
  connect(this, SIGNAL(Update()), this, SLOT(UpdateWidgets()) );
  connect(this, SIGNAL(UpdateProgressBar(int)), this, SLOT(UpdateProgressBarRequest(int)) );

  this->m_USCapturing = TrackedUltrasoundCapturing::New(); 

  // Set output directory
  std::string outputPath = vtksys::SystemTools::CollapseFullPath("./Output", this->GetProgramPath().c_str()); 
  this->m_USCapturing->SetOutputFolder(outputPath.c_str()); 
  this->SettingsOutputFolderName->setText(outputPath.c_str()); 

  // Set image sequence file name
  this->m_USCapturing->SetImageSequenceFileName("TrackedUltrasound"); 
  this->SettingsOutputImageSequenceFileName->setText(this->m_USCapturing->GetImageSequenceFileName()); 

  // Set frame rate 
  this->m_USCapturing->SetFrameRate(10); 

  // Select log level 
  vtkPlusLogger::Instance()->SetLogLevel(vtkPlusLogger::LOG_LEVEL_INFO); 
  QString strLogLevel; 
  switch ( vtkPlusLogger::Instance()->GetLogLevel() )
  {
  case vtkPlusLogger::LOG_LEVEL_ERROR:
    strLogLevel = "Error"; 
    break;
  case vtkPlusLogger::LOG_LEVEL_WARNING:
    strLogLevel = "Warning"; 
    break;
  case vtkPlusLogger::LOG_LEVEL_INFO:
    strLogLevel = "Info"; 
    break;
  case vtkPlusLogger::LOG_LEVEL_DEBUG:
    strLogLevel = "Debug"; 
    break;
  case vtkPlusLogger::LOG_LEVEL_TRACE:
    strLogLevel = "Trace"; 
    break;
  }

  this->LogLevelComboBox->setCurrentIndex( this->LogLevelComboBox->findText( strLogLevel, Qt::MatchContains )); 

  this->UpdateWidgets();
}

//----------------------------------------------------------------------
std::string TrackedUltrasoundCapturingGUI::GetProgramPath()
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::GetProgramPath");
  std::string programPath("./"), errorMsg; 
  if ( !vtksys::SystemTools::FindProgramPath(qApp->argv()[0], programPath, errorMsg) )
  {
    LOG_ERROR(errorMsg); 
  }
  programPath = vtksys::SystemTools::GetParentDirectory(programPath.c_str()); 

  return programPath; 
}

//----------------------------------------------------------------------
TrackedUltrasoundCapturingGUI::~TrackedUltrasoundCapturingGUI() 
{
  if ( this->m_USCapturing != NULL )
  {
    this->m_USCapturing->Delete(); 
    this->m_USCapturing = NULL; 
  }

  if ( this->m_RecordingToolStateDisplayWidget != NULL )
  {
    delete m_RecordingToolStateDisplayWidget; 
    m_RecordingToolStateDisplayWidget = NULL; 
  }

  if ( this->m_SyncToolStateDisplayWidget != NULL )
  {
    delete m_SyncToolStateDisplayWidget; 
    m_SyncToolStateDisplayWidget = NULL; 
  }

}

//----------------------------------------------------------------------
PlusStatus TrackedUltrasoundCapturingGUI::Initialize()
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::Initialize");

  this->Initialized = false; 

  // Initialize the tracked US capturing class 
  if ( this->m_USCapturing->Initialize() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to initialize TrackedUltrasoundCapturing!"); 
    return PLUS_FAIL; 
  }

  // Update labels
  this->SyncVideoOffsetLabel->setText(tr(LABEL_SYNC_VIDEO_OFFSET + "  " + QString::number(this->m_USCapturing->GetVideoOffsetMs(), 'f', 2) + " ms")); 
  this->RecordingFrameRateLabel->setText(tr(LABEL_RECORDING_FRAME_RATE + "  " + QString::number(this->m_USCapturing->GetFrameRate()) + " fps")); 
  this->RecordingFrameRateSlider->setValue( this->m_USCapturing->GetFrameRate() ); 

  // Add an observer for progress bar
  this->m_USCapturing->SetUpdateRequestCallbackFunction(UpdateRequestCallback);
  this->m_USCapturing->SetProgressBarUpdateCallbackFunction(UpdateProgressBarRequestCallback); 

  // Sign up interactor to receive char event
  vtkSmartPointer<vtkInteractorCallback> cb = vtkSmartPointer<vtkInteractorCallback>::New();
  this->CapturingRealtimeRenderer->GetRenderWindow()->GetInteractor()->AddObserver(vtkCommand::CharEvent, cb);
  this->SyncRealtimeRenderer->GetRenderWindow()->GetInteractor()->AddObserver(vtkCommand::CharEvent, cb);

  // Set the focus 
  this->button(this->NextButton)->setFocus(); 

  this->Initialized = true;

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------
bool TrackedUltrasoundCapturingGUI::validateCurrentPage()
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::validateCurrentPage");
  if ( !this->Initialized )
  {
    int init = QMessageBox::message(tr("Tracked Ultrasound Capturing"),
      tr("Tracked Ultrasound Capturing needs to connect to devices."));

    return false; 
  }

  if ( this->currentPage() == this->SettingsPage )
  {
    if ( STRCASECMP(this->m_USCapturing->GetOutputFolder(), this->SettingsOutputFolderName->text().ascii()) != 0 )
    {
      this->m_USCapturing->SetOutputFolder(this->SettingsOutputFolderName->text().ascii()); 
      LOG_INFO("Output folder changed to: " << this->m_USCapturing->GetOutputFolder() ); 
    }

    if ( !vtksys::SystemTools::FileExists(this->m_USCapturing->GetOutputFolder(), false) )
    {
      vtksys::SystemTools::MakeDirectory(this->m_USCapturing->GetOutputFolder()); 
      LOG_DEBUG("Created new folder: " << this->m_USCapturing->GetOutputFolder() ); 
    }

    if ( STRCASECMP(this->m_USCapturing->GetImageSequenceFileName(), this->SettingsOutputImageSequenceFileName->text().ascii()) != 0 )
    {
      this->m_USCapturing->SetImageSequenceFileName(this->SettingsOutputImageSequenceFileName->text().ascii()); 
      LOG_INFO("Default image  sequence file name changed to: " << this->m_USCapturing->GetImageSequenceFileName() ); 
    }

  }

  return QWizard::validateCurrentPage(); 
}

//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::initializePage( int id )
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::initializePage");
  if ( this->page(id) )
  {
    this->page(id)->initializePage(); 
  }

  if ( this->GetRenderer(id) )
  {
    if (this->GetRenderer(id)->GetRenderWindow()->HasRenderer(this->m_USCapturing->GetRealtimeRenderer()))
    {
      this->GetRenderer(id)->GetRenderWindow()->RemoveRenderer(this->m_USCapturing->GetRealtimeRenderer()); 
    }
    this->GetRenderer(id)->GetRenderWindow()->AddRenderer(this->m_USCapturing->GetRealtimeRenderer()); 
  }
}

//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::cleanupPage( int id )
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::cleanupPage");
  if ( this->page(id) )
  {
    this->page(id)->cleanupPage(); 
  }

  if ( this->GetRenderer(id) )
  {
    if (this->GetRenderer(id)->GetRenderWindow()->HasRenderer(this->m_USCapturing->GetRealtimeRenderer()))
    {
      this->GetRenderer(id)->GetRenderWindow()->RemoveRenderer(this->m_USCapturing->GetRealtimeRenderer()); 
    }
  }

  this->initializePage(this->previousId()); 
}

//----------------------------------------------------------------------
int TrackedUltrasoundCapturingGUI::previousId()
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::previousId");
  int previousIdIndex = this->pageIds().indexOf(this->currentId()) - 1; 

  if ( previousIdIndex < 0 )
  {
    previousIdIndex = 0; 
  }

  return this->pageIds()[previousIdIndex]; 
}

//----------------------------------------------------------------------
int TrackedUltrasoundCapturingGUI::nextId() const
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::nextId");

  if ( this->m_USCapturing == NULL || this->m_USCapturing->GetDataCollector() == NULL )
  {
    return this->currentPage()->nextId(); 
  }

  // if we don't have tracking skip SynchronizationPage
  vtkTracker* tracker = this->m_USCapturing->GetDataCollector()->GetTracker(); 
  if ( this->currentPage() == this->SettingsPage && tracker == NULL ) 
  {
    return this->SynchronizationPage->nextId(); 
  } 

  return this->currentPage()->nextId(); 
}


//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::done( int r )
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::done");
  if ( this->close() )
  {	
    QWizard::done(r); 
  }
}

// Callback function for progress notification.
//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::UpdateRequestCallback()
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::UpdateRequestCallback");
  emit TrackedUltrasoundCapturingGUI::GetInstance()->Update(); 
}

//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::UpdateWidgets()
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::UpdateWidgets");

  // Update renderer window
  if ( this->GetRenderer(this->currentId()) != NULL )
  {
    this->GetRenderer(this->currentId())->update(); 
  }

  // Update recorded frame numbers
  this->RecordedFrames->setText(QString::number(this->m_USCapturing->GetNumberOfRecordedFrames())); 

  if (this->m_RecordingToolStateDisplayWidget->IsInitialized()) 
  {
		this->m_RecordingToolStateDisplayWidget->Update();
	}

  if (this->m_SyncToolStateDisplayWidget->IsInitialized()) 
  {
		this->m_SyncToolStateDisplayWidget->Update();
	}

  // Update transform matrix 
  if ( this->m_USCapturing->GetDataCollector() != NULL )
  {
    vtkTracker* tracker = this->m_USCapturing->GetDataCollector()->GetTracker(); 
    if ( tracker == NULL || !tracker->IsTracking() )
    {
      this->SyncTrackerMatrix->hide(); 
      this->CapturingTrackerMatrix->hide(); 
    }
    else
    {
      this->SyncTrackerMatrix->show(); 
      this->CapturingTrackerMatrix->show(); 

      int defaultToolNumber = tracker->GetFirstPortNumberByType(TRACKER_TOOL_PROBE);
      if ( defaultToolNumber == -1 )
      {
        if ( tracker->GetFirstActiveTool(defaultToolNumber) != PLUS_SUCCESS )
        {
          this->SyncTrackerMatrix->hide(); 
          this->CapturingTrackerMatrix->hide(); 
          return; 
        }
      }

      if (tracker->GetTool( defaultToolNumber )==NULL)
      {
        LOG_WARNING("Cannot get tracker tool (number="<<defaultToolNumber<<")");        
        this->SyncTrackerMatrix->hide(); 
        this->CapturingTrackerMatrix->hide(); 
        return;
      }
      vtkTrackerBuffer* trackerBuffer = tracker->GetTool( defaultToolNumber )->GetBuffer();  

      vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
      TrackerBufferItem bufferItem; 
      if ( trackerBuffer->GetLatestTrackerBufferItem(&bufferItem, false) != ITEM_OK )
      {
        LOG_WARNING("Failed to get latest tracker item from buffer!"); 
      }
      else
      {
        if (bufferItem.GetMatrix(transformMatrix)!=PLUS_SUCCESS)
        {
          LOG_ERROR("Failed to get transformMatrix"); 
        }

        if ( bufferItem.GetStatus() != TR_OK )
        {
          // if the buffer item status is not valid, display only identity matrix
          transformMatrix->Identity(); 
        }

      }

      // Copy matrix to the table widget
      for ( int r = 0; r < 4; r++ )
      {
        for ( int c = 0; c < 4; c++ )
        {
          this->SyncTrackerMatrix->item(r,c)->setText(QString::number(transformMatrix->GetElement(r,c), 'f', 2)); 
          this->CapturingTrackerMatrix->item(r,c)->setText(QString::number(transformMatrix->GetElement(r,c), 'f', 2)); 
        }
      }

    }

  }

  qApp->processEvents();
}

// Callback function for progress notification.
//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::UpdateProgressBarRequestCallback( int percent )
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::UpdateProgressBarRequestCallback");
  emit TrackedUltrasoundCapturingGUI::GetInstance()->UpdateProgressBar(percent); 
}

//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::UpdateProgressBarRequest( int percent )
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::UpdateProgressBarRequest");
  const int pageID = this->currentId(); 

  if ( this->GetProgressBar(pageID) == NULL )
  {
    return; 
  }

  this->GetProgressBar(pageID)->setMinimum(0); 
  this->GetProgressBar(pageID)->setMaximum(100); 
  this->GetProgressBar(pageID)->setValue(percent); 
  this->GetProgressBar(pageID)->update(); 

  this->UpdateWidgets(); 
}

//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::closeEvent(QCloseEvent* closeEvent)
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::closeEvent");
  int quit = QMessageBox::question (this, tr("Tracked Ultrasound Capturing"),
    tr("Do you want to close the application?"),
    QMessageBox::Yes | QMessageBox::No,
    QMessageBox::No);

  if ( quit == QMessageBox::No )
  {
    closeEvent->ignore();
    return; 
  }
  else
  {
    closeEvent->accept(); 
  }

  if ( this->m_USCapturing->GetSynchronizing() )
  {
    this->CancelSyncButtonClicked(); 

    while ( this->m_USCapturing->GetSynchronizing() )
    {
      vtkAccurateTimer::GetInstance()->Delay(0.1); 
      this->UpdateWidgets(); 
    }
  }

  if ( this->m_USCapturing->GetRecording() )
  {
    this->StopButtonClicked(); 
  }

  if ( this->m_USCapturing->GetNumberOfRecordedFrames() > 0 )
  {
    int save = QMessageBox::question (this, tr("Tracked Ultrasound Capturing"),
      tr("Dou you want save your changes?"),
      QMessageBox::Save | QMessageBox::Discard ,
      QMessageBox::Save);

    if ( save == QMessageBox::Save)
    {
      this->SaveButtonClicked(); 
    }
  }
}

//----------------------------------------------------------------------
QProgressBar* TrackedUltrasoundCapturingGUI::GetProgressBar( int pageID )
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::GetProgressBar");
  QProgressBar* progressbar = NULL; 
  if ( this->page(pageID) == this->CapturingPage )
  {
    progressbar = this->CapturingProgressBar; 
  }
  else if ( this->page(pageID) == this->SynchronizationPage )
  {
    progressbar = this->SyncProgressBar; 
  }
  return progressbar; 
}


//----------------------------------------------------------------------
QVTKWidget* TrackedUltrasoundCapturingGUI::GetRenderer( int pageID )
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::GetRenderer");
  QVTKWidget* renderer = NULL; 
  if ( this->page(pageID) == this->CapturingPage )
  {
    renderer = this->CapturingRealtimeRenderer; 
  }
  else if ( this->page(pageID) == this->SynchronizationPage )
  {
    renderer = this->SyncRealtimeRenderer; 
  }

  return renderer; 
}

//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::DisableWizardButtons()
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::DisableWizardButtons");
  this->button(this->BackButton)->setEnabled(false); 
  this->button(this->NextButton)->setEnabled(false); 
  this->button(this->CommitButton)->setEnabled(false); 
  this->button(this->FinishButton)->setEnabled(false); 
  this->button(this->CancelButton)->setEnabled(false); 
  this->button(this->HelpButton)->setEnabled(false); 
  this->button(this->CustomButton1)->setEnabled(false); 
  this->button(this->CustomButton2)->setEnabled(false); 
  this->button(this->CustomButton3)->setEnabled(false); 
}

//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::EnableWizardButtons()
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::EnableWizardButtons");
  this->button(this->BackButton)->setEnabled(true); 
  this->button(this->NextButton)->setEnabled(true); 
  this->button(this->CommitButton)->setEnabled(true); 
  this->button(this->FinishButton)->setEnabled(true); 
  this->button(this->CancelButton)->setEnabled(true); 
  this->button(this->HelpButton)->setEnabled(true); 
  this->button(this->CustomButton1)->setEnabled(true); 
  this->button(this->CustomButton2)->setEnabled(true); 
  this->button(this->CustomButton3)->setEnabled(true); 
}

//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::SnapshotButtonClicked()
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::SnapshotButtonClicked");
  LOG_INFO( "Frame captured..." ); 
  this->m_USCapturing->RecordTrackedFrame(); 
}

//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::RecordTrackedFrame()
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::RecordTrackedFrame");
  this->m_USCapturing->UpdateRecording();
}

//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::RecordButtonClicked()
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::RecordButtonClicked");
  this->SnapshotButton->setEnabled(false); 
  this->SaveButton->setEnabled(false); 
  this->SaveAsButton->setEnabled(false); 
  this->ResetBufferButton->setEnabled(false); 
  this->StopButton->setEnabled(true); 
  this->RecordButton->hide(); 
  this->StopButton->show();
  this->StopButton->setFocus();
  this->DisableWizardButtons(); 
  this->UpdateWidgets(); 

  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor)); 

  this->m_RecordingTimer->start(1000.0 / this->m_USCapturing->GetFrameRate()); 

  this->m_USCapturing->StartRecording(); 
}

//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::StopButtonClicked()
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::StopButtonClicked");
  this->StopButton->setEnabled(false); 
  this->UpdateWidgets(); 

  this->m_RecordingTimer->stop(); 
  this->m_USCapturing->StopRecording(); 

  this->StopButton->hide(); 
  this->RecordButton->show(); 
  this->SnapshotButton->setEnabled(true); 
  this->RecordButton->setEnabled(true); 
  this->StopButton->setEnabled(true); 
  this->SaveButton->setEnabled(true); 
  this->SaveAsButton->setEnabled(true); 
  this->ResetBufferButton->setEnabled(true); 
  this->RecordButton->setFocus(); 
  QApplication::restoreOverrideCursor(); 
  this->EnableWizardButtons(); 
  this->UpdateWidgets(); 
}

//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::SaveButtonClicked()
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::SaveButtonClicked");
  this->SaveButton->setEnabled(false); 
  this->SaveAsButton->setEnabled(false); 
  this->SnapshotButton->setEnabled(false); 
  this->RecordButton->setEnabled(false); 
  this->DisableWizardButtons(); 
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor)); 
  this->UpdateWidgets(); 

  this->m_USCapturing->SaveData(); 

  this->SaveButton->setEnabled(true); 
  this->SaveAsButton->setEnabled(true); 
  this->SnapshotButton->setEnabled(true); 
  this->RecordButton->setEnabled(true); 
  this->EnableWizardButtons(); 
  QApplication::restoreOverrideCursor(); 
  this->UpdateWidgets(); 
}

//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::SaveAsButtonClicked()
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::SaveAsButtonClicked");
  QString fileName = QFileDialog::getSaveFileName(this,tr("Save As"), QString(this->m_USCapturing->GetOutputFolder()), tr("SequenceMetaFiles (*.mha *.mhd)") );

  if (fileName.isEmpty())
  {
    return ;
  }

  this->SaveButton->setEnabled(false); 
  this->SaveAsButton->setEnabled(false); 
  this->SnapshotButton->setEnabled(false); 
  this->RecordButton->setEnabled(false); 
  this->DisableWizardButtons(); 
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor)); 
  this->UpdateWidgets(); 

  this->m_USCapturing->SaveAsData(fileName.ascii()); 

  this->SaveButton->setEnabled(true); 
  this->SaveAsButton->setEnabled(true); 
  this->SnapshotButton->setEnabled(true); 
  this->RecordButton->setEnabled(true); 
  this->EnableWizardButtons(); 
  QApplication::restoreOverrideCursor(); 
  this->UpdateWidgets(); 
}

//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::SynchronizeButtonClicked()
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::SynchronizeButtonClicked");
  this->DisableWizardButtons(); 
  this->GetProgressBar( this->currentId() )->show(); 
  this->SynchronizeButton->hide();
  this->CancelSyncButton->show(); 
  this->UpdateWidgets();

  this->m_USCapturing->SynchronizeFrameToTracker(); 

  this->EnableWizardButtons();	
  this->GetProgressBar( this->currentId() )->hide(); 
  this->SynchronizeButton->show();
  this->CancelSyncButton->hide(); 
  this->SyncVideoOffsetLabel->setText(tr(LABEL_SYNC_VIDEO_OFFSET + "  " + QString::number(this->m_USCapturing->GetVideoOffsetMs(), 'f', 2) + " ms")); 
  this->UpdateWidgets();
}

//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::CancelSyncButtonClicked()
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::CancelSyncButtonClicked");
  this->m_USCapturing->GetDataCollector()->CancelSyncRequestOn(); 
  this->EnableWizardButtons(); 
  this->GetProgressBar( this->currentId() )->hide(); 
  this->SynchronizeButton->show();
  this->CancelSyncButton->hide(); 
  this->SyncVideoOffsetLabel->setText(tr(LABEL_SYNC_VIDEO_OFFSET + "  " + QString::number(this->m_USCapturing->GetVideoOffsetMs(), 'f', 2) + " ms")); 
  this->UpdateWidgets();
}

//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::ZeroOffsetButtonClicked()
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::ZeroOffsetButtonClicked");
  this->m_USCapturing->SetLocalTimeOffset(0,0); 
  this->SyncVideoOffsetLabel->setText(tr(LABEL_SYNC_VIDEO_OFFSET + "  " + QString::number(this->m_USCapturing->GetVideoOffsetMs(), 'f', 2) + " ms")); 
  this->UpdateWidgets();
}


//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::SetFrameRate()
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::SetFrameRate");
  double frameRate = this->RecordingFrameRateSlider->value(); 
  this->m_USCapturing->SetFrameRate( frameRate ); 
  this->m_RecordingTimer->setInterval(1000.0 / this->m_USCapturing->GetFrameRate());
  this->RecordingFrameRateLabel->setText(tr(LABEL_RECORDING_FRAME_RATE + "  " + QString::number(this->m_USCapturing->GetFrameRate()) + " fps")); 
  this->UpdateWidgets(); 
}

//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::OpenOutputFolderButtonClicked()
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::OpenOutputFolderButtonClicked");
  QString folderName = QFileDialog::getExistingDirectory(this,tr("Open Folder"), QString(this->GetProgramPath().c_str()));

  if (folderName.isEmpty())
  {
    return ;
  }

  this->SettingsOutputFolderName->setText(folderName); 
  this->m_USCapturing->SetOutputFolder(folderName.ascii()); 
  LOG_INFO("Output folder changed to: " << this->m_USCapturing->GetOutputFolder() ); 

  this->UpdateWidgets(); 
}

//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::ChangeLogLevel()
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::ChangeLogLevel");
  if ( STRCASECMP(this->LogLevelComboBox->currentText().ascii(), "ERROR") == 0 )
  {
    vtkPlusLogger::Instance()->SetLogLevel(vtkPlusLogger::LOG_LEVEL_ERROR); 
  }
  else if ( STRCASECMP(this->LogLevelComboBox->currentText().ascii(), "WARNING") == 0 )
  {
    vtkPlusLogger::Instance()->SetLogLevel(vtkPlusLogger::LOG_LEVEL_WARNING); 
  }
  else if ( STRCASECMP(this->LogLevelComboBox->currentText().ascii(), "INFO") == 0 )
  {
    vtkPlusLogger::Instance()->SetLogLevel(vtkPlusLogger::LOG_LEVEL_INFO); 
  }
  else if ( STRCASECMP(this->LogLevelComboBox->currentText().ascii(), "DEBUG") == 0 )
  {
    vtkPlusLogger::Instance()->SetLogLevel(vtkPlusLogger::LOG_LEVEL_DEBUG); 
  }
  else if ( STRCASECMP(this->LogLevelComboBox->currentText().ascii(), "TRACE") == 0 )
  {
    vtkPlusLogger::Instance()->SetLogLevel(vtkPlusLogger::LOG_LEVEL_TRACE); 
  }

  LOG_INFO("Log level changed to: " << this->LogLevelComboBox->currentText().ascii() )
}

//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::ResetBufferButtonClicked()
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::ResetBufferButtonClicked");

  int reset = QMessageBox::question (this, tr("Tracked Ultrasound Capturing"),
    tr("Dou you want to discard recorded frames from the buffer?"),
    QMessageBox::Yes | QMessageBox::No,
    QMessageBox::No);

  if ( reset == QMessageBox::No )
  {
    return; 
  }

  this->m_USCapturing->ClearTrackedFrameContainer(); 
}

//-----------------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::ConnectToDevicesByConfigFile(std::string aConfigFile)
{
  LOG_TRACE("TrackedUltrasoundCapturingGUI::ConnectToDevicesByConfigFile");

  this->m_USCapturing->SetInputConfigFileName( aConfigFile.c_str() ); 

  // If connection has been successfully created then this action should disconnect
  if (! m_DeviceSetSelectorWidget->GetConnectionSuccessful()) 
  {
    LOG_INFO("Connect to devices"); 

    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

    // Create dialog
    QDialog* connectDialog = new QDialog(this, Qt::Dialog);
    connectDialog->setModal(true);
    connectDialog->setMinimumSize(QSize(360,80));
    connectDialog->setCaption(tr("Tracked Ultrasound Capturing"));
    connectDialog->setBackgroundColor(QColor(224, 224, 224));

    QLabel* connectLabel = new QLabel(QString("Connecting to devices, please wait..."), connectDialog);
    connectLabel->setFont(QFont("SansSerif", 16));

    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(connectLabel);

    connectDialog->setLayout(layout);
    connectDialog->show();

    QApplication::processEvents();

    // Initialize the calibrator 
    if ( this->Initialize() != PLUS_SUCCESS )
    { 
      LOG_ERROR("Failed to initialize Trackedultrasound Capturing!"); 
      m_DeviceSetSelectorWidget->SetConnectionSuccessful(false);
      m_SyncToolStateDisplayWidget->InitializeTools(NULL, false);
      m_RecordingToolStateDisplayWidget->InitializeTools(NULL, false);

	  // Close dialog
	  connectDialog->done(0);
      QApplication::restoreOverrideCursor();
	  return;
    }

    // Start data collection 
    if ( this->m_USCapturing->GetDataCollector()->Start() != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to start collecting data!");
      m_DeviceSetSelectorWidget->SetConnectionSuccessful(false);

      m_SyncToolStateDisplayWidget->InitializeTools(NULL, false);
      m_RecordingToolStateDisplayWidget->InitializeTools(NULL, false);

	  // Close dialog
	  connectDialog->done(0);
      QApplication::restoreOverrideCursor();
	  return;
    }
    else
    {
      if ( this->m_USCapturing->GetDataCollector()->GetTrackingEnabled() )
      {
        int defaultToolNumber =  this->m_USCapturing->GetDataCollector()->GetTracker()->GetFirstPortNumberByType( TRACKER_TOOL_PROBE ); 
        if ( defaultToolNumber == -1 )
        {
          if ( this->m_USCapturing->GetDataCollector()->GetTracker()->GetFirstActiveTool( defaultToolNumber ) != PLUS_SUCCESS )
          {
            LOG_ERROR("Failed to get first active tool number!"); 
            return; 
          }
        }
        this->m_USCapturing->DefaultFrameTransformName = this->m_USCapturing->GetDataCollector()->GetTracker()->GetTool(defaultToolNumber)->GetToolName(); 
      }

      m_DeviceSetSelectorWidget->SetConnectionSuccessful(true);
      if (m_RecordingToolStateDisplayWidget->InitializeTools(this->m_USCapturing->GetDataCollector(), true) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to initialize tools for tool display widget!"); 
      }

      if (m_SyncToolStateDisplayWidget->InitializeTools(this->m_USCapturing->GetDataCollector(), true) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to initialize tools for tool display widget!"); 
      }
    }

    // Close dialog
    connectDialog->done(0);

    QApplication::restoreOverrideCursor();
  } 
  else 
  {
    this->m_USCapturing->GetDataCollector()->Stop(); 
    this->m_USCapturing->GetDataCollector()->Disconnect(); 

    m_DeviceSetSelectorWidget->SetConnectionSuccessful(false);
    m_SyncToolStateDisplayWidget->InitializeTools(NULL, false);
    m_RecordingToolStateDisplayWidget->InitializeTools(NULL, false);
  }
}

