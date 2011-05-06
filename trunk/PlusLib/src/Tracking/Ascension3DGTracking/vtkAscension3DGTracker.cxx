
#include "vtkAscension3DGTracker.h"

#include <sstream>


#ifdef PLUS_USE_Ascension3DGm
namespace atc
{
#include "ATC3DGm.h"
}
#else /* PLUS_USE_Ascension3DGm */
namespace atc
{
#include "ATC3DG.h"
}
#endif /* PLUS_USE_Ascension3DGm */


#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtksys/SystemTools.hxx"
#include "vtkTransform.h"
#include "vtkXMLDataElement.h"

#include "PlusConfigure.h"
#include "vtkTracker.h"
#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"
#include "vtkFrameToTimeConverter.h"
#include "vtkTrackedFrameList.h"



vtkAscension3DGTracker*
vtkAscension3DGTracker::New()
{
	// First try to create the object from the vtkObjectFactory
	vtkObject* ret = vtkObjectFactory::CreateInstance("vtkAscension3DGTracker");
	if( ret )
	{
		return ( vtkAscension3DGTracker* )ret;
	}
	// If the factory was unable to create the object, then create it here.
	return new vtkAscension3DGTracker;
}



vtkAscension3DGTracker
::vtkAscension3DGTracker()
{
	this->LocalTrackerBuffer = NULL;
	this->Tracking = 0;
	
	this->TransmitterAttached = false;
	this->FrameNumber = 0;
}



vtkAscension3DGTracker
::~vtkAscension3DGTracker() 
{
	if ( this->Tracking )
	{
		this->StopTracking();
	}

	if ( this->LocalTrackerBuffer != NULL )
	{
		this->LocalTrackerBuffer->Delete(); 
		this->LocalTrackerBuffer = NULL; 
	}
}



void
vtkAscension3DGTracker
::PrintSelf( ostream& os, vtkIndent indent )
{
	vtkTracker::PrintSelf( os, indent );
}



/** 
 * @returns 1 on success, 0 on failure.
 */
int
vtkAscension3DGTracker
::Connect()
{
	LOG_TRACE( "vtkAscension3DGTracker::Connect" ); 
		
	if ( ! this->Probe() ) return 0;
	
  
  this->CheckReturnStatus( atc::InitializeBIRDSystem() );
  
  atc::SYSTEM_CONFIGURATION systemConfig;
  
  int success = 1;
  success = this->CheckReturnStatus( atc::GetBIRDSystemConfiguration( &systemConfig ) );
  if ( ! success ) return 0;
  
    // Change to metric units.
  
  int metric = 1;
  success = this->CheckReturnStatus( atc::SetSystemParameter( atc::METRIC, &metric, sizeof( metric ) ) );
  if ( ! success ) return 0;
  
  
    // Go through all tools.
  
  int sensorID;
  atc::DATA_FORMAT_TYPE formatType = atc::DOUBLE_POSITION_ANGLES_MATRIX_QUATERNION_TIME_Q_BUTTON;
  
  for ( sensorID = 0; sensorID < systemConfig.numberSensors; ++ sensorID )
    {
    this->CheckReturnStatus( atc::SetSensorParameter( sensorID, atc::DATA_FORMAT, &formatType, sizeof( formatType ) ) );
    
    atc::DEVICE_STATUS status;
    status = atc::GetSensorStatus( sensorID );
    
    this->SensorSaturated.push_back( ( status & SATURATED ) ? true : false );
    this->SensorAttached.push_back( ( status & NOT_ATTACHED ) ? false : true );
    this->SensorInMotion.push_back( ( status & OUT_OF_MOTIONBOX ) ? false : true );
    this->TransmitterAttached = ( ( status & NO_TRANSMITTER_ATTACHED ) ? false : true );
    }
  
  
	// TODO: Do we need other tools than the defult tool? 
	this->SetNumberOfTools( systemConfig.numberSensors );
  
	// Enable tools
	for ( int tool = 0; tool < this->GetNumberOfTools(); tool ++ )
	{
		if ( this->SensorAttached[ tool ] )
		  {
		  this->GetTool( tool )->EnabledOn();
		  }
	}

	// Set tool names
	
	return 1; 
}



void
vtkAscension3DGTracker
::Disconnect()
{
	LOG_TRACE( "vtkAscension3DGTracker::Disconnect" ); 
	this->StopTracking(); 
}



int
vtkAscension3DGTracker
::Probe()
{
	LOG_TRACE( "vtkAscension3DGTracker::Probe" ); 
	
  return 1; 
} 



/**
 * @returns 1 on success, 0 on failure.
 */
int
vtkAscension3DGTracker
::InternalStartTracking()
{
	LOG_TRACE( "vtkAscension3DGTracker::InternalStartTracking" ); 
	if ( this->Tracking )
	{
		return 1;
	}
  
	if ( ! this->InitAscension3DGTracker() )
	{
		LOG_ERROR( "Couldn't initialize vtkAscension3DGTracker" );
		this->Tracking = 0;
		return 0;
	} 

	
	// for accurate timing
	this->Timer->Initialize();
	this->Tracking = 1;
  
  
    // Turn on the first attached transmitter.
  
  atc::BOARD_CONFIGURATION boardConfig;
  int success = this->CheckReturnStatus( GetBoardConfiguration( 0, &boardConfig ) );
  
  short selectID = TRANSMITTER_OFF;
  int i = 0;
  bool found = false;
  while( ( i < boardConfig.numberTransmitters ) && ( found == false ) )
    {
    atc::TRANSMITTER_CONFIGURATION transConfig;
    success = this->CheckReturnStatus( GetTransmitterConfiguration( i, &transConfig ) );
    if ( transConfig.attached )
      {
      selectID = i;
      found = true;
      }
    ++ i;
    }
  
  success = this->CheckReturnStatus( atc::SetSystemParameter( atc::SELECT_TRANSMITTER, &selectID, sizeof( selectID ) ) );
  if ( ! success ) return 0;
  
	return 1;
}



int
vtkAscension3DGTracker
::InternalStopTracking()
{
	LOG_TRACE( "vtkAscension3DGTracker::InternalStopTracking" ); 
	
	short selectID = TRANSMITTER_OFF;
	this->CheckReturnStatus( atc::SetSystemParameter( atc::SELECT_TRANSMITTER, &selectID, sizeof( selectID ) ) );
	
	return 1;
}



/**
 * This function is called by the tracker thread.
 */
void
vtkAscension3DGTracker
::InternalUpdate()
{
	LOG_TRACE( "vtkAscension3DGTracker::InternalUpdate" ); 
	if ( ! this->Tracking )
	{
		LOG_WARNING( "Called Update() when SavedDataTracker was not tracking" );
		return;
	}
  
  
  if ( ! this->Tracking )
    {
    vtkWarningMacro( << "called Update() when not tracking" );
    return;
    }
  
  // TODO: Frame number is fake here!
  ++ this->FrameNumber;
  
  double unfilteredtimestamp(0), filteredtimestamp(0); 
  this->Timer->GetTimeStampForFrame( this->FrameNumber, unfilteredtimestamp, filteredtimestamp );
  
  
  atc::SYSTEM_CONFIGURATION sysConfig;
  int success = this->CheckReturnStatus( atc::GetBIRDSystemConfiguration( &sysConfig ) );
  if ( ! success ) return;
  
  typedef atc::DOUBLE_POSITION_ANGLES_MATRIX_QUATERNION_TIME_Q_BUTTON_RECORD RecordType;
  RecordType *record = new RecordType[ sysConfig.numberSensors ];
  success = this->CheckReturnStatus( atc::GetSynchronousRecord( ALL_SENSORS,
                                     record, sysConfig.numberSensors * sizeof( RecordType ) ) );
  if ( ! success ) return;
  
  
  if ( this->GetNumberOfTools() != sysConfig.numberSensors )
    {
    vtkErrorMacro( "Changing sensors while tracking is not supported. Reconnect necessary." );
    this->StopTracking();
    this->Disconnect();
    return;
    }
  
  
  for ( unsigned short sensorIndex = 0; sensorIndex < sysConfig.numberSensors; ++ sensorIndex )
    {
    atc::DEVICE_STATUS status = atc::GetSensorStatus( sensorIndex );
    
    bool saturated, attached, inMotionBox;
    bool transmitterRunning, transmitterAttached, globalError;
    
    saturated = status & SATURATED;
    attached = ! ( status & NOT_ATTACHED );
    inMotionBox = ! ( status & OUT_OF_MOTIONBOX );
    transmitterRunning = !( status & NO_TRANSMITTER_RUNNING );
    transmitterAttached = !( status & NO_TRANSMITTER_ATTACHED );
    globalError = status & GLOBAL_ERROR;
    
    vtkSmartPointer< vtkMatrix4x4 > vtkM = vtkSmartPointer< vtkMatrix4x4 >::New();
    vtkM->Identity();
    for ( int row = 0; row < 3; ++ row )
      {
      for ( int col = 0; col < 3; ++ col )
        {
        vtkM->SetElement( row, col, record[ sensorIndex ].s[ row ][ col ] );
        }
      }
    vtkM->SetElement( 0, 3, record[ sensorIndex ].x );
    vtkM->SetElement( 1, 3, record[ sensorIndex ].y );
    vtkM->SetElement( 2, 3, record[ sensorIndex ].z );
    
    int flags = 0;
    if ( ! attached ) flags |= TR_MISSING;
    if ( ! inMotionBox ) flags |= TR_OUT_OF_VIEW;
    // TODO: Handle all kinds of errors.
    
    this->ToolUpdate( sensorIndex, vtkM, flags, this->FrameNumber, unfilteredtimestamp, filteredtimestamp );
    }
  
  
}



bool
vtkAscension3DGTracker
::InitAscension3DGTracker()
{
	LOG_TRACE( "vtkAscension3DGTracker::InitSavedDataTracker" ); 
	return this->Connect(); 
}



void
vtkAscension3DGTracker
::ReadConfiguration( vtkXMLDataElement* config )
{
	LOG_TRACE( "vtkAscension3DGTracker::ReadConfiguration" ); 
	if ( config == NULL ) 
	{
		LOG_WARNING("Unable to find Ascension3DGTracker XML data element");
		return; 
	}

	if ( this->ConfigurationData == NULL ) 
	{
		this->ConfigurationData = vtkXMLDataElement::New(); 
	}

	// Save config data
	this->ConfigurationData->DeepCopy( config ); 

}



void
vtkAscension3DGTracker
::WriteConfiguration( vtkXMLDataElement* config )
{
	LOG_TRACE( "vtkAscension3DGTracker::WriteConfiguration" ); 
	if ( config == NULL )
	{
		config = vtkXMLDataElement::New(); 
	}

	config->SetName("Ascension3DGTracker");
}



/**
 * @returns 1 on success, 0 on failure
 */
int
vtkAscension3DGTracker
::CheckReturnStatus( int status )
{
  if( status != atc::BIRD_ERROR_SUCCESS )
  {
    char buffer[ 512 ];
    atc::GetErrorText( status, buffer, sizeof( buffer ), atc::SIMPLE_MESSAGE );
    std::cout << "Error: " << buffer << std::endl;
    return 0;
  }
  return 1;
}

