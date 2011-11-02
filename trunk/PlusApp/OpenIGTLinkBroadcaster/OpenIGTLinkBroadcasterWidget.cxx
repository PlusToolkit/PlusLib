
#include "OpenIGTLinkBroadcasterWidget.h"

#include "vtkDataCollector.h"
#include "vtkOpenIGTLinkBroadcaster.h"
#include "vtkSavedDataTracker.h"
#include "vtkSavedDataVideoSource.h"
#include "vtkTracker.h"
#include "vtkTrackerTool.h"

#include "vtkXMLUtilities.h"

#include <QTimer>



OpenIGTLinkBroadcasterWidget::OpenIGTLinkBroadcasterWidget( QWidget *parent )
  : QWidget( parent )
{
  this->m_DataCollector = vtkDataCollector::New();
  this->m_OpenIGTLinkBroadcaster = vtkOpenIGTLinkBroadcaster::New();
  this->m_Timer = NULL;
  
  this->Paused = false;
  this->StylusCalibrationOn = false;
  
  
  ui.setupUi( this );
  
  
    // Make ui connections.
  
  connect( ui.pushButton_PlayPause, SIGNAL( released() ),
           this, SLOT( PlayPausePressed() ) );
  connect( ui.checkBox_StylusCalibration, SIGNAL( stateChanged( int ) ),
           this, SLOT( StylusCalibrationChanged( int ) ) );
}



OpenIGTLinkBroadcasterWidget::~OpenIGTLinkBroadcasterWidget()
{
  if ( this->m_DataCollector != NULL )
    {
    this->m_DataCollector->Stop();
    this->m_DataCollector->Disconnect();
    this->m_DataCollector->Delete();
    this->m_DataCollector = NULL;
    }
  
  if ( this->m_OpenIGTLinkBroadcaster != NULL )
    {
    this->m_OpenIGTLinkBroadcaster->Delete();
    this->m_OpenIGTLinkBroadcaster = NULL;
    }
  
  if ( m_Timer != NULL )
  {
    m_Timer->stop();
		delete m_Timer;
		m_Timer = NULL;
	}
}



void OpenIGTLinkBroadcasterWidget::Initialize( std::string configFileName, std::string videoBufferFileName, std::string trackerBufferFileName )
{
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkXMLUtilities::ReadElementFromFile( configFileName.c_str() );
  if ( configRootElement == NULL )
  {	
    LOG_ERROR("Unable to read configuration from file " << configFileName.c_str()); 
    return;
  }
  this->m_DataCollector->ReadConfiguration( configRootElement );
  
    
  if ( this->m_DataCollector->GetAcquisitionType() == SYNCHRO_VIDEO_SAVEDDATASET )
  {
    vtkSavedDataVideoSource* videoSource = static_cast< vtkSavedDataVideoSource* >( this->m_DataCollector->GetVideoSource() );
    
    if ( ! videoBufferFileName.empty() )
    {
      videoSource->SetSequenceMetafile( videoBufferFileName.c_str() );
    }
	else
    {
      std::cout << "Error: Video buffer file not specified." << std::endl;
      return;
    }
  }
  
  if ( this->m_DataCollector->GetTrackerType() == TRACKER_SAVEDDATASET )
  {
    vtkSavedDataTracker* tracker = static_cast< vtkSavedDataTracker* >( this->m_DataCollector->GetTracker() );
      
    if ( ! trackerBufferFileName.empty() )
    {
      tracker->SetSequenceMetafile( trackerBufferFileName.c_str() );
    }
    else
    {
      std::cout << "Error: Tracker buffer file not specified" << std::endl;
      return;
    }
  }
  
  
  LOG_INFO( "Initializing data collector." );
  this->m_DataCollector->Connect();
  
  
    // Prepare the OpenIGTLink broadcaster.
  
  vtkOpenIGTLinkBroadcaster::Status broadcasterStatus = vtkOpenIGTLinkBroadcaster::STATUS_NOT_INITIALIZED;
  this->m_OpenIGTLinkBroadcaster->SetDataCollector( this->m_DataCollector );
  
  std::string errorMessage;
  broadcasterStatus = this->m_OpenIGTLinkBroadcaster->Initialize( errorMessage );
  if ( broadcasterStatus != vtkOpenIGTLinkBroadcaster::STATUS_OK )
    {
    std::cout << "Error in broadcaster:" << errorMessage << std::endl;
    }
  
  
    // Determine delay from frequency for the tracker.
  
  double delayTracking = 1.0 / this->m_DataCollector->GetTracker()->GetFrequency();
  
  LOG_INFO( "Tracker frequency = " << this->m_DataCollector->GetTracker()->GetFrequency() );
  LOG_DEBUG( "Tracker delay = " << delayTracking );
  
  
    // Start data collection and broadcasting.
  
  LOG_INFO( "Start data collector..." );
  this->m_DataCollector->Start();
  
  
    // Set up timer.
  
  m_Timer = new QTimer( this );
	connect( m_Timer, SIGNAL( timeout() ), this, SLOT( SendMessages() ) );
	m_Timer->start( delayTracking * 1000 );
	
}



void OpenIGTLinkBroadcasterWidget::PlayPausePressed()
{
  if ( this->Paused )
    {
    this->Paused = false;
    ui.pushButton_PlayPause->setText( "Pause" );
    }
  else
    {
    this->Paused = true;
    ui.pushButton_PlayPause->setText( "Play" );
    }
}



void OpenIGTLinkBroadcasterWidget::StylusCalibrationChanged( int newValue )
{
  if ( newValue != 0 )
    {
    this->StylusCalibrationOn = true;
    this->m_OpenIGTLinkBroadcaster->SetApplyStylusCalibration( true );
    }
  else
    {
    this->StylusCalibrationOn = false;
    this->m_OpenIGTLinkBroadcaster->SetApplyStylusCalibration( false );
    }
}



void OpenIGTLinkBroadcasterWidget::SendMessages()
{
  int defaultTool = this->m_DataCollector->GetTracker()->GetFirstPortNumberByType( TRACKER_TOOL_PROBE );

  vtkSmartPointer< vtkMatrix4x4 > mToolToReference = vtkSmartPointer< vtkMatrix4x4 >::New();
  
  if ( this->m_DataCollector->GetTracker()->IsTracking() )
  {
    double timeTracker = 0.0;
    TrackerStatus status = TR_OK;
    this->m_DataCollector->GetTransformWithTimestamp( mToolToReference, timeTracker, status, defaultTool );
    if ( status == TR_OK )
    {
      LOG_INFO( "Tool position: " << mToolToReference->GetElement( 0, 3 ) << " "
                                  << mToolToReference->GetElement( 1, 3 ) << " "
                                  << mToolToReference->GetElement( 2, 3 ) << " " );
    }
  }
  else
  {
    LOG_INFO( "Unable to connect to tracker." );
  }
  
  
  vtkOpenIGTLinkBroadcaster::Status broadcasterStatus = vtkOpenIGTLinkBroadcaster::STATUS_NOT_INITIALIZED;
  std::string errorMessage;
  if ( this->Paused == false )
  {
    
    broadcasterStatus = this->m_OpenIGTLinkBroadcaster->SendMessages( errorMessage );
  }
  if ( broadcasterStatus != vtkOpenIGTLinkBroadcaster::STATUS_OK )
  {
    LOG_WARNING( "Could not broadcast messages." );
  }
}