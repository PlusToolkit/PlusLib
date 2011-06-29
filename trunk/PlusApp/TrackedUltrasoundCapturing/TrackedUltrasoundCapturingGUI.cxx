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
	VTK_LOG_TO_CONSOLE_ON;

	this->Initialized = false; 

	// Create the UI widgets 
	setupUi(this);

	// Hide buttons/labels
	this->StopButton->hide(); 
	this->CancelSyncButton->hide(); 
	this->CapturingProgressBar->hide(); 
	this->SyncProgressBar->hide(); 
	this->DisconnectButton->hide(); 

	this->HideTrackerToolWidgets(); 

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

	// Set the renderers aspect ratio to 4:3
	this->m_RendererAspectRatio = 4.0 / 3.0; 

	this->m_USCapturing = TrackedUltrasoundCapturing::New(); 

	// ********************* Make default values ************************
	// Find config file 
	std::string configPath = vtksys::SystemTools::CollapseFullPath("../config/USDataCollectionConfig.xml", this->GetProgramPath().c_str()); 
	if ( vtksys::SystemTools::FileExists( configPath.c_str(), true) )
	{
		this->m_USCapturing->SetInputConfigFileName(configPath.c_str());
		this->SettingsConfigFileName->setText(configPath.c_str()); 
	}

	// Set output directory
	std::string outputPath = vtksys::SystemTools::CollapseFullPath("./Output", this->GetProgramPath().c_str()); 
	this->m_USCapturing->SetOutputFolder(outputPath.c_str()); 
	this->SettingsOutputFolderName->setText(outputPath.c_str()); 

	// Set image sequence file name
	this->m_USCapturing->SetImageSequenceFileName("TrackedUltrasound"); 
	this->SettingsOutputImageSequenceFileName->setText(this->m_USCapturing->GetImageSequenceFileName()); 
	LOG_INFO("Default image  sequence file name changed to: " << this->m_USCapturing->GetImageSequenceFileName() ); 

	// Set frame rate 
	this->m_USCapturing->SetFrameRate(10); 

	// Select log level 
	PlusLogger::Instance()->SetLogLevel(PlusLogger::LOG_LEVEL_INFO); 
	QString strLogLevel; 
	switch ( PlusLogger::Instance()->GetLogLevel() )
	{
	case PlusLogger::LOG_LEVEL_ERROR:
		strLogLevel = "Error"; 
		break;
	case PlusLogger::LOG_LEVEL_WARNING:
		strLogLevel = "Warning"; 
		break;
	case PlusLogger::LOG_LEVEL_INFO:
		strLogLevel = "Info"; 
		break;
	case PlusLogger::LOG_LEVEL_DEBUG:
		strLogLevel = "Debug"; 
		break;
	case PlusLogger::LOG_LEVEL_TRACE:
		strLogLevel = "Trace"; 
		break;
	}

	this->LogLevelComboBox->setCurrentIndex( this->LogLevelComboBox->findText( strLogLevel, Qt::MatchContains )); 

	this->ConnectToDevicesButton->setFocus(); 

	this->UpdateWidgets();

	// TODO: remove these! No more need to choos tool numbers
	this->MainToolComboBox->setEnabled(false); 
	this->MainToolTransformName->setEnabled(false); 
	this->ReferenceToolComboBox->setEnabled(false); 
	this->ReferenceToolTransformName->setEnabled(false); 
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
}

//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::Initialize()
{
	LOG_TRACE("TrackedUltrasoundCapturingGUI::Initialize");

	// Initialize the tracked US capturing class 
	this->m_USCapturing->Initialize(); 

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
}

//----------------------------------------------------------------------
bool TrackedUltrasoundCapturingGUI::validateCurrentPage()
{
	LOG_TRACE("TrackedUltrasoundCapturingGUI::validateCurrentPage");
	if ( !this->Initialized )
	{
		int init = QMessageBox::question (this, tr("Tracked Ultrasound Capturing"),
			tr("Tracked Ultrasound Capturing needs to connect to devices.\nDou you want to connect now?"),
			QMessageBox::Yes | QMessageBox::No,
			QMessageBox::No);

		if ( init == QMessageBox::Yes )
		{
			this->ConnectToDevicesButtonClicked();
		}
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

		this->ChangeTransformNames(); 
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
	//LOG_TRACE("TrackedUltrasoundCapturingGUI::UpdateWidgets");

	// Update renderer window
	if ( this->GetRenderer(this->currentId()) != NULL )
	{
		this->GetRenderer(this->currentId())->update(); 
	}

	// Update recorded frame numbers
	this->RecordedFrames->setText(QString::number(this->m_USCapturing->GetNumberOfRecordedFrames())); 

	// Update transform matrix 
	if ( this->m_USCapturing->GetDataCollector() != NULL )
	{
		vtkTracker* tracker = this->m_USCapturing->GetDataCollector()->GetTracker(); 
		if ( tracker == NULL )
		{
			this->SyncTrackerOutOfViewLabel->hide(); 
			this->CapturingTrackerOutOfViewLabel->hide(); 
			this->SyncTrackerMatrix->hide(); 
			this->CapturingTrackerMatrix->hide(); 
		}
		else
		{

			const int mainToolNumber = this->m_USCapturing->GetDataCollector()->GetDefaultToolPortNumber(); 
			vtkTrackerBuffer* trackerBuffer = tracker->GetTool( mainToolNumber )->GetBuffer();  

			trackerBuffer->Lock(); 
			long flag = trackerBuffer->GetFlags(0); 
			vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
			trackerBuffer->GetMatrix(transformMatrix, 0); 
			trackerBuffer->Unlock(); 

			if ( flag == TR_OK )
			{
				this->SyncTrackerOutOfViewLabel->hide(); 
				this->CapturingTrackerOutOfViewLabel->hide(); 
				this->SyncTrackerMatrix->show(); 
				this->CapturingTrackerMatrix->show(); 

				for ( int r = 0; r < 4; r++ )
				{
					for ( int c = 0; c < 4; c++ )
					{
						this->SyncTrackerMatrix->item(r,c)->setText(QString::number(transformMatrix->GetElement(r,c), 'f', 2)); 
						this->CapturingTrackerMatrix->item(r,c)->setText(QString::number(transformMatrix->GetElement(r,c), 'f', 2)); 
					}
				}
			}
			else
			{
				this->SyncTrackerMatrix->hide(); 
				this->CapturingTrackerMatrix->hide(); 
				this->SyncTrackerOutOfViewLabel->show(); 
				this->CapturingTrackerOutOfViewLabel->show(); 
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
		tr("Dou you want to close the application?"),
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
void TrackedUltrasoundCapturingGUI::SetRendererAspectRatio(double ratio)
{
	if ( ratio > 0 ) 
	{
		//int height = this->GetRenderer(this->currentId())->height(); 
		//this->GetRenderer(this->currentId())->setMaximumWidth( height * ratio ); 
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
	//LOG_TRACE("TrackedUltrasoundCapturingGUI::GetRenderer");
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
void TrackedUltrasoundCapturingGUI::HideTrackerToolWidgets()
{
	LOG_TRACE("TrackedUltrasoundCapturingGUI::HideTrackerToolWidgets");
	this->MainToolLabel->hide(); 
	this->MainToolComboBox->hide(); 
	this->MainToolTransformNameLabel->hide(); 
	this->MainToolTransformName->hide(); 
	this->ReferenceToolLabel->hide(); 
	this->ReferenceToolComboBox->hide(); 
	this->ReferenceToolTransformNameLabel->hide(); 
	this->ReferenceToolTransformName->hide(); 
}

//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::ShowTrackerToolWidgets()
{
	LOG_TRACE("TrackedUltrasoundCapturingGUI::ShowTrackerToolWidgets");
	this->MainToolLabel->show(); 
	this->MainToolComboBox->show(); 
	this->MainToolTransformNameLabel->show(); 
	this->MainToolTransformName->show(); 
	this->ReferenceToolLabel->show(); 
	this->ReferenceToolComboBox->show(); 
	this->ReferenceToolTransformNameLabel->show(); 
	this->ReferenceToolTransformName->show(); 
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
void TrackedUltrasoundCapturingGUI::OpenConfigFileButtonClicked()
{
	LOG_TRACE("TrackedUltrasoundCapturingGUI::OpenConfigFileButtonClicked");
	QString fileName = QFileDialog::getOpenFileName(this,tr("Open"), QString(this->GetProgramPath().c_str()), tr("XML configuration files (*.xml)") );

	if (fileName.isEmpty())
	{
		return ;
	}

	this->SettingsConfigFileName->setText(fileName); 
	this->m_USCapturing->SetInputConfigFileName(fileName.ascii()); 

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
void TrackedUltrasoundCapturingGUI::ConnectToDevicesButtonClicked()
{
	LOG_TRACE("TrackedUltrasoundCapturingGUI::ConnectToDevicesButtonClicked");
	this->ConnectToDevicesButton->setEnabled(false); 
	this->DisableWizardButtons(); 
	this->UpdateWidgets(); 

	// Initialize the calibrator 
	this->Initialize(); 

	this->DisconnectButton->show(); 
	this->ConnectToDevicesButton->setEnabled(true); 
	this->ConnectToDevicesButton->hide(); 
	this->ShowTrackerToolWidgets(); 
	this->EnableWizardButtons(); 
	this->UpdateWidgets(); 
}

//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::DisconnectButtonClicked()
{
	LOG_TRACE("TrackedUltrasoundCapturingGUI::DisconnectButtonClicked");
	this->Initialized = false; 
	this->DisconnectButton->setEnabled(false); 
	this->HideTrackerToolWidgets(); 
	this->UpdateWidgets(); 

	this->m_USCapturing->GetDataCollector()->Disconnect(); 

	this->DisconnectButton->setEnabled(true); 
	this->DisconnectButton->hide(); 
	this->ConnectToDevicesButton->show(); 
	this->UpdateWidgets(); 
}

//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::ChangeLogLevel()
{
	LOG_TRACE("TrackedUltrasoundCapturingGUI::ChangeLogLevel");
	if ( STRCASECMP(this->LogLevelComboBox->currentText().ascii(), "ERROR") == 0 )
	{
		PlusLogger::Instance()->SetLogLevel(PlusLogger::LOG_LEVEL_ERROR); 
	}
	else if ( STRCASECMP(this->LogLevelComboBox->currentText().ascii(), "WARNING") == 0 )
	{
		PlusLogger::Instance()->SetLogLevel(PlusLogger::LOG_LEVEL_WARNING); 
	}
	else if ( STRCASECMP(this->LogLevelComboBox->currentText().ascii(), "INFO") == 0 )
	{
		PlusLogger::Instance()->SetLogLevel(PlusLogger::LOG_LEVEL_INFO); 
	}
	else if ( STRCASECMP(this->LogLevelComboBox->currentText().ascii(), "DEBUG") == 0 )
	{
		PlusLogger::Instance()->SetLogLevel(PlusLogger::LOG_LEVEL_DEBUG); 
	}
	else if ( STRCASECMP(this->LogLevelComboBox->currentText().ascii(), "TRACE") == 0 )
	{
		PlusLogger::Instance()->SetLogLevel(PlusLogger::LOG_LEVEL_TRACE); 
	}

	LOG_INFO("Log level changed to: " << this->LogLevelComboBox->currentText().ascii() )
}

//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::ChangeMainToolID()
{
	LOG_TRACE("TrackedUltrasoundCapturingGUI::ChangeMainToolID");
	std::string currentTool(this->MainToolComboBox->currentText().ascii()); 
	vtkTracker * tracker = this->m_USCapturing->GetDataCollector()->GetTracker(); 

	for ( int i = 0; i < tracker-> GetNumberOfTools(); i++ )
	{
		if ( STRCASECMP(currentTool.c_str(), this->GetToolID(i).c_str()) == 0 )
		{
			LOG_INFO("Selected main tool: " << this->GetToolID(i) ); 

			//this->m_USCapturing->GetDataCollector()->SetMainToolNumber(i); 

			std::string mainToolName = this->m_USCapturing->GetDataCollector()->GetDefaultToolName(); 
			this->MainToolTransformName->setText(QString(mainToolName.c_str())); 

			return; 
		}
	}
}

//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::ChangeReferenceToolID()
{
	LOG_TRACE("TrackedUltrasoundCapturingGUI::ChangeReferenceToolID");
	vtkTracker * tracker = this->m_USCapturing->GetDataCollector()->GetTracker(); 
	std::string currentTool(this->ReferenceToolComboBox->currentText().ascii()); 

	for ( int i = -1; i < tracker->GetNumberOfTools(); i++ )
	{
		if ( STRCASECMP(currentTool.c_str(), this->GetToolID(i).c_str()) == 0 )
		{
			LOG_INFO("Selected reference tool: " << this->GetToolID(i) ); 

			// TODO: Remove tool selection from gui! 
			// tracker->SetReferenceTool(i);

			if ( i < 0 )
			{
				this->ReferenceToolTransformName->clear(); 
			}
			else
			{
				std::string refToolName = tracker->GetTool(i)->GetToolName(); 
				this->ReferenceToolTransformName->setText(QString(refToolName.c_str())); 
			}

			return; 
		}
	}

}

//----------------------------------------------------------------------
std::string TrackedUltrasoundCapturingGUI::GetToolID( int tool)
{
	LOG_TRACE("TrackedUltrasoundCapturingGUI::GetToolID");
	if ( tool < 0 ) 
	{
		return "None"; 
	}

	vtkTracker * tracker = this->m_USCapturing->GetDataCollector()->GetTracker(); 

	std::string toolPartNumber = tracker->GetTool(tool)->GetToolPartNumber(); 
	std::string toolName = tracker->GetTool(tool)->GetToolName(); 
	std::ostringstream toolID; 

	if ( !toolPartNumber.empty() )
	{
		toolID << "(" << tool << ") " << toolPartNumber; 
	}
	else
	{
		toolID << "(" << tool << ") " << toolName; 
	}

	return toolID.str(); 
}

//----------------------------------------------------------------------
void TrackedUltrasoundCapturingGUI::ChangeTransformNames()
{
	LOG_TRACE("TrackedUltrasoundCapturingGUI::ChangeTransformNames");
	std::string mainTool(this->MainToolComboBox->currentText().ascii()); 
	std::string referenceTool(this->ReferenceToolComboBox->currentText().ascii()); 

	vtkTracker * tracker = this->m_USCapturing->GetDataCollector()->GetTracker(); 

	if ( tracker == NULL )
	{
		LOG_DEBUG("No need to change transform names if there is no tracker!"); 
		return; 
	}

	for ( int i = 0; i < tracker->GetNumberOfTools(); i++ )
	{
		if ( STRCASECMP(mainTool.c_str(), this->GetToolID(i).c_str()) == 0 )
		{
			if ( STRCASECMP(tracker->GetTool(i)->GetToolName(), this->MainToolTransformName->text().ascii()) != 0 )
			{
				tracker->GetTool(i)->SetToolName(this->MainToolTransformName->text().ascii()); 
				LOG_INFO("Tool " << i << " name changed to: " << tracker->GetTool(i)->GetToolName() );
			}
		}

		if ( STRCASECMP(referenceTool.c_str(), this->GetToolID(i).c_str()) == 0 )
		{
			if ( STRCASECMP(tracker->GetTool(i)->GetToolName(), this->ReferenceToolTransformName->text().ascii()) != 0 )
			{
				tracker->GetTool(i)->SetToolName(this->ReferenceToolTransformName->text().ascii()); 
				LOG_INFO("Tool " << i << " name changed to: " << tracker->GetTool(i)->GetToolName() );
			}
		}
	}
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