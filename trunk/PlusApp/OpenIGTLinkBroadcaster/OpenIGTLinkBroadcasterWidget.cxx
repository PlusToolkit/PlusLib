
#include "OpenIGTLinkBroadcasterWidget.h"

#include "vtkDataCollector.h"
#include "vtkDataCollectorHardwareDevice.h"
#include "vtkDataCollectorFile.h"

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



void OpenIGTLinkBroadcasterWidget::Initialize( std::string configFileName )
{
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkXMLUtilities::ReadElementFromFile( configFileName.c_str() );
  if ( configRootElement == NULL )
  {	
    LOG_ERROR("Unable to read configuration from file " << configFileName.c_str()); 
    return;
  }
  
  this->m_DataCollector->ReadConfiguration( configRootElement );


  
  LOG_INFO( "Initializing data collector." );
  this->m_DataCollector->Connect();
  
  
    // Prepare the OpenIGTLink broadcaster.
  
  this->m_OpenIGTLinkBroadcaster->SetDataCollector( this->m_DataCollector );
  
  if ( this->m_OpenIGTLinkBroadcaster->Initialize() != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to initialize broadcaster!");
  }

  if (m_OpenIGTLinkBroadcaster->ReadConfiguration(configRootElement) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to read OpenIGTLinkBroadcaster configuration!");
  }

  
    // Determine delay from frequency for the tracker.

  double frequency = 0.1;
  if (m_DataCollector->GetFrameRate(frequency)  != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to get frame rate from data collector!");
  }

  double delayTracking = 1.0 / frequency;

  LOG_INFO( "Tracker frequency = " << frequency );
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
  vtkSmartPointer< vtkMatrix4x4 > mToolToReference = vtkSmartPointer< vtkMatrix4x4 >::New();
  
  if ( m_DataCollector->GetTrackingEnabled() )
  {
    TrackedFrame trackedFrame; 
    if ( m_DataCollector->GetTrackedFrame(&trackedFrame) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get tracked frame!"); 
    }
    else
    {
      //TODO
      LOG_WARNING("TEMPORARY ISSUE - cannot print transforms"); // Need to get all toolInfos from broadcaster and get the transforms OR make some logging possible in the broadcaster itself
      /*
      TrackedFrameFieldStatus status = FIELD_INVALID;
      PlusTransformName transformName("Probe", "Reference"); //TODO!!!!!
      trackedFrame.GetCustomFrameTransform(transformName, mToolToReference); 
      trackedFrame.GetCustomFrameTransformStatus(transformName, status); 
      if ( status == FIELD_OK )
      {
        LOG_INFO( "Tool position: " << mToolToReference->GetElement( 0, 3 ) << " "
          << mToolToReference->GetElement( 1, 3 ) << " "
          << mToolToReference->GetElement( 2, 3 ) << " " );
      }
      */
    }
  }
  else
  {
    LOG_INFO( "Unable to connect to tracker." );
  }
  
  
  if ( this->Paused == false )
  {
    if ( this->m_OpenIGTLinkBroadcaster->SendMessages() != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to broadcast messages!");
    }
  }
}
