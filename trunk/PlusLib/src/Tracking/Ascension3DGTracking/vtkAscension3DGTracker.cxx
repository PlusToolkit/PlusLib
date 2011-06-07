#include "PlusConfigure.h"

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

	// Set the maximum number of sensors that this class can handle
	this->SetNumberOfTools(12); 
	this->NumberOfSensors = 0; 
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



PlusStatus
vtkAscension3DGTracker
::Connect()
{
	LOG_TRACE( "vtkAscension3DGTracker::Connect" ); 
		
  if ( this->Probe()!=PLUS_SUCCESS )
  {
    LOG_ERROR("Connection probe failed");
    return PLUS_FAIL;
  }
  
  this->CheckReturnStatus( atc::InitializeBIRDSystem() );
  
  atc::SYSTEM_CONFIGURATION systemConfig;
  
  if (this->CheckReturnStatus( atc::GetBIRDSystemConfiguration( &systemConfig ) ) != PLUS_SUCCESS)
  {
    LOG_ERROR("Connection initialization failed");
    return PLUS_FAIL;
  }  
    // Change to metric units.
  
  int metric = 1;
  if (this->CheckReturnStatus( atc::SetSystemParameter( atc::METRIC, &metric, sizeof( metric ) ) )!= PLUS_SUCCESS)
  {
    LOG_ERROR("Connection set to metric units failed");
    return PLUS_FAIL;
  }  
  
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
  
  
	this->NumberOfSensors = systemConfig.numberSensors; 
  
	// Enable tools
	for ( int tool = 0; tool < this->GetNumberOfTools(); tool ++ )
	{
		if ( tool < this->GetNumberOfSensors() && this->SensorAttached[ tool ] )
		  {
		  this->GetTool( tool )->EnabledOn();
		  }
	}

	// Set tool names
	
	return PLUS_SUCCESS; 
}



PlusStatus
vtkAscension3DGTracker
::Disconnect()
{
	LOG_TRACE( "vtkAscension3DGTracker::Disconnect" ); 
	return this->StopTracking(); 
}



PlusStatus
vtkAscension3DGTracker
::Probe()
{
	LOG_TRACE( "vtkAscension3DGTracker::Probe" ); 
	
  return PLUS_SUCCESS; 
} 



/**
 * @returns 1 on success, 0 on failure.
 */
PlusStatus
vtkAscension3DGTracker
::InternalStartTracking()
{
	LOG_TRACE( "vtkAscension3DGTracker::InternalStartTracking" ); 
	if ( this->Tracking )
	{
		return PLUS_SUCCESS;
	}
  
	if ( ! this->InitAscension3DGTracker() )
	{
		LOG_ERROR( "Couldn't initialize vtkAscension3DGTracker" );
		this->Tracking = 0;
		return PLUS_FAIL;
	} 

	
	// for accurate timing
	this->Timer->Initialize();
	this->Tracking = 1;
  
  
    // Turn on the first attached transmitter.
  
  atc::BOARD_CONFIGURATION boardConfig;
  this->CheckReturnStatus( GetBoardConfiguration( 0, &boardConfig ) );
  
  short selectID = TRANSMITTER_OFF;
  int i = 0;
  bool found = false;
  while( ( i < boardConfig.numberTransmitters ) && ( found == false ) )
    {
    atc::TRANSMITTER_CONFIGURATION transConfig;
    this->CheckReturnStatus( GetTransmitterConfiguration( i, &transConfig ) );
    if ( transConfig.attached )
      {
      selectID = i;
      found = true;
      }
    ++ i;
    }
  
  if (this->CheckReturnStatus( atc::SetSystemParameter( atc::SELECT_TRANSMITTER, &selectID, sizeof( selectID ) ) ) != PLUS_SUCCESS)
  {
    LOG_ERROR("Select transmitter failed");
    return PLUS_FAIL;
  }
  
	return PLUS_SUCCESS;
}



PlusStatus
vtkAscension3DGTracker
::InternalStopTracking()
{
	LOG_TRACE( "vtkAscension3DGTracker::InternalStopTracking" ); 
	
	short selectID = TRANSMITTER_OFF;
  if (this->CheckReturnStatus( atc::SetSystemParameter( atc::SELECT_TRANSMITTER, &selectID, sizeof( selectID ) ) )
    != PLUS_SUCCESS)
  {
    LOG_ERROR("Select transmitter failed");
    return PLUS_FAIL;
  }
	
	return PLUS_SUCCESS;
}



/**
 * This function is called by the tracker thread.
 */
PlusStatus
vtkAscension3DGTracker
::InternalUpdate()
{
	LOG_TRACE( "vtkAscension3DGTracker::InternalUpdate" ); 
	if ( ! this->Tracking )
	{
		LOG_ERROR( "Called Update() when SavedDataTracker was not tracking" );
		return PLUS_FAIL;
	}
  
  
  if ( ! this->Tracking )
    {
    LOG_ERROR("called Update() when not tracking" );
    return PLUS_FAIL;
    }
  
  // TODO: Frame number is fake here!
  ++ this->FrameNumber;
  
  double unfilteredtimestamp( 0 ), filteredtimestamp( 0 ); 
  this->Timer->GetTimeStampForFrame( this->FrameNumber, unfilteredtimestamp, filteredtimestamp );
  
  
  atc::SYSTEM_CONFIGURATION sysConfig;
  if (this->CheckReturnStatus( atc::GetBIRDSystemConfiguration( &sysConfig ) )
    != PLUS_SUCCESS)
  {
    LOG_ERROR("Cannot get system configuration");
    return PLUS_FAIL;
  }
  
  typedef atc::DOUBLE_POSITION_ANGLES_MATRIX_QUATERNION_TIME_Q_BUTTON_RECORD RecordType;
  RecordType *record = new RecordType[ sysConfig.numberSensors ];
  if (this->CheckReturnStatus( atc::GetSynchronousRecord( ALL_SENSORS,
    record, sysConfig.numberSensors * sizeof( RecordType ) ) ) != PLUS_SUCCESS)
  {
    LOG_ERROR("Cannot get synchronous record");
    return PLUS_FAIL;
  }
  
  if ( this->GetNumberOfSensors() != sysConfig.numberSensors )
    {
    LOG_ERROR( "Changing sensors while tracking is not supported. Reconnect necessary." );
    this->StopTracking();
    this->Disconnect();
    return PLUS_FAIL;
    }
  
  
    // Set up reference matrix.
  
  bool saturated, attached, inMotionBox;
  bool transmitterRunning, transmitterAttached, globalError;
    
  int flags = 0;
  vtkSmartPointer< vtkMatrix4x4 > mTrackerToReference = vtkSmartPointer< vtkMatrix4x4 >::New();
    mTrackerToReference->Identity();
  
  if ( this->GetReferenceTool() >= 0 )
    {
    atc::DEVICE_STATUS status = atc::GetSensorStatus( this->GetReferenceTool() );
    
    saturated = status & SATURATED;
    attached = ! ( status & NOT_ATTACHED );
    inMotionBox = ! ( status & OUT_OF_MOTIONBOX );
    transmitterRunning = !( status & NO_TRANSMITTER_RUNNING );
    transmitterAttached = !( status & NO_TRANSMITTER_ATTACHED );
    globalError = status & GLOBAL_ERROR;
    
    if ( ! attached )
      {
      flags |= TR_MISSING;
      }
    else if ( ! inMotionBox )
      {
      flags |= TR_OUT_OF_VIEW;
      }
    else
		  {
		  for ( int row = 0; row < 3; ++ row )
        {
        for ( int col = 0; col < 3; ++ col )
          {
          mTrackerToReference->SetElement( row, col, record[ this->GetReferenceTool() ].s[ row ][ col ] );
          }
        }
      
      mTrackerToReference->SetElement( 0, 3, record[ this->GetReferenceTool() ].x );
      mTrackerToReference->SetElement( 1, 3, record[ this->GetReferenceTool() ].y );
      mTrackerToReference->SetElement( 2, 3, record[ this->GetReferenceTool() ].z );
    
		  }
		mTrackerToReference->Invert();
    }
  
  
  for ( unsigned short sensorIndex = 0; sensorIndex < sysConfig.numberSensors; ++ sensorIndex )
    {
    atc::DEVICE_STATUS status = atc::GetSensorStatus( sensorIndex );
    
    saturated = status & SATURATED;
    attached = ! ( status & NOT_ATTACHED );
    inMotionBox = ! ( status & OUT_OF_MOTIONBOX );
    transmitterRunning = !( status & NO_TRANSMITTER_RUNNING );
    transmitterAttached = !( status & NO_TRANSMITTER_ATTACHED );
    globalError = status & GLOBAL_ERROR;
    
    vtkSmartPointer< vtkMatrix4x4 > mToolToTracker = vtkSmartPointer< vtkMatrix4x4 >::New();
    mToolToTracker->Identity();
    for ( int row = 0; row < 3; ++ row )
      {
      for ( int col = 0; col < 3; ++ col )
        {
        mToolToTracker->SetElement( row, col, record[ sensorIndex ].s[ row ][ col ] );
        }
      }
    
    mToolToTracker->SetElement( 0, 3, record[ sensorIndex ].x );
    mToolToTracker->SetElement( 1, 3, record[ sensorIndex ].y );
    mToolToTracker->SetElement( 2, 3, record[ sensorIndex ].z );
    
    
    if ( ! attached ) flags |= TR_MISSING;
    if ( ! inMotionBox ) flags |= TR_OUT_OF_VIEW;
    
    
      // Apply reference to get Tool-to-Reference.
    
    vtkSmartPointer< vtkMatrix4x4 > mToolToReference = vtkSmartPointer< vtkMatrix4x4 >::New();
      
    if ( sensorIndex != this->GetReferenceTool() )
      {
      vtkMatrix4x4::Multiply4x4( mTrackerToReference, mToolToTracker, mToolToReference );
      this->ToolUpdate( sensorIndex, mToolToReference, flags, this->FrameNumber, unfilteredtimestamp, filteredtimestamp );
      }
    else
      {
      this->ToolUpdate( sensorIndex, mToolToTracker, flags, this->FrameNumber, unfilteredtimestamp, filteredtimestamp );
      }
    }
  
  return PLUS_SUCCESS;
}



PlusStatus
vtkAscension3DGTracker
::InitAscension3DGTracker()
{
	LOG_TRACE( "vtkAscension3DGTracker::InitAscension3DGTracker" ); 
	return this->Connect(); 
}



PlusStatus
vtkAscension3DGTracker
::ReadConfiguration( vtkXMLDataElement* config )
{
	// Read superclass configuration first
	Superclass::ReadConfiguration(config); 

	LOG_TRACE( "vtkAscension3DGTracker::ReadConfiguration" ); 
	if ( config == NULL ) 
	{
		LOG_ERROR("Unable to find Ascension3DGTracker XML data element");
		return PLUS_FAIL; 
	}

	if ( this->ConfigurationData == NULL ) 
	{
		this->ConfigurationData = vtkXMLDataElement::New(); 
	}

	// Save config data
	this->ConfigurationData->DeepCopy( config ); 

  return PLUS_SUCCESS;
}



PlusStatus
vtkAscension3DGTracker
::WriteConfiguration( vtkXMLDataElement* config )
{
	LOG_TRACE( "vtkAscension3DGTracker::WriteConfiguration" ); 
	if ( config == NULL )
	{
    LOG_ERROR("Invalid write configuration pointer");
		return PLUS_FAIL;
	}

	//config->SetName("Ascension3DGTracker");
  
  LOG_ERROR("Not implemented");
  return PLUS_FAIL;
}


PlusStatus
vtkAscension3DGTracker
::CheckReturnStatus( int status )
{
  if( status != atc::BIRD_ERROR_SUCCESS )
  {
    char buffer[ 512 ];
    atc::GetErrorText( status, buffer, sizeof( buffer ), atc::SIMPLE_MESSAGE );
    LOG_ERROR(buffer);
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

