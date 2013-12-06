/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "PlusConfigure.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkPlusBuffer.h"
#include "vtkPlusDataSource.h"
#include "vtkTransform.h"
#include "vtkXMLDataElement.h"
#include "vtksys/SystemTools.hxx"
#include <sstream>

static const char* QUALITY_PORT_NAME="quality";

vtkStandardNewMacro(vtkAscension3DGTrackerBase);
typedef DOUBLE_POSITION_ANGLES_MATRIX_QUATERNION_TIME_Q_BUTTON_RECORD AscensionRecordType;

//-------------------------------------------------------------------------
vtkAscension3DGTrackerBase::vtkAscension3DGTrackerBase()
{
  this->LocalTrackerBuffer = NULL;
  this->AscensionRecordBuffer = NULL; 

  this->TransmitterAttached = false;
  this->NumberOfSensors = 0;
  this->FilterAcWideNotch = 0;
  this->FilterAcNarrowNotch = 0;
  this->FilterDcAdaptive = 0.0;
  this->FilterLargeChange = 0;
  this->FilterAlpha = false;

  this->RequireImageOrientationInConfiguration = false;
  this->RequireFrameBufferSizeInDeviceSetConfiguration = false;
  this->RequireAcquisitionRateInDeviceSetConfiguration = true;
  this->RequireAveragedItemsForFilteringInDeviceSetConfiguration = false;
  this->RequireToolAveragedItemsForFilteringInDeviceSetConfiguration = true;
  this->RequireLocalTimeOffsetSecInDeviceSetConfiguration = true;
  this->RequireUsImageOrientationInDeviceSetConfiguration = false;
  this->RequireRfElementInDeviceSetConfiguration = false;

  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates=true; 
}

//-------------------------------------------------------------------------
vtkAscension3DGTrackerBase::~vtkAscension3DGTrackerBase() 
{
  if ( this->Recording )
  {
    this->StopRecording();
  }

  if ( this->AscensionRecordBuffer != NULL )
  {
    delete this->AscensionRecordBuffer;
    this->AscensionRecordBuffer = NULL;
  }

  if ( this->LocalTrackerBuffer != NULL )
  {
    this->LocalTrackerBuffer->Delete();
    this->LocalTrackerBuffer = NULL;
  }
}

//-------------------------------------------------------------------------
void vtkAscension3DGTrackerBase::PrintSelf( ostream& os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );
}

//-------------------------------------------------------------------------
PlusStatus vtkAscension3DGTrackerBase::InternalConnect()
{
  LOG_TRACE("vtkAscension3DGTracker::Connect" );

  if ( this->Probe()!=PLUS_SUCCESS )
  {
    LOG_ERROR("Connection probe failed");
    return PLUS_FAIL;
  }

  this->CheckReturnStatus( InitializeBIRDSystem() );

  SYSTEM_CONFIGURATION systemConfig;

  if (this->CheckReturnStatus( GetBIRDSystemConfiguration( &systemConfig ) ) != PLUS_SUCCESS)
  {
    LOG_ERROR("Connection initialization failed");
    return PLUS_FAIL;
  }  

  // Change to metric units.
  int metric = 1;
  if (this->CheckReturnStatus( SetSystemParameter( METRIC, &metric, sizeof( metric ) ) )!= PLUS_SUCCESS)
  {
    LOG_ERROR("Connection set to metric units failed");
    return PLUS_FAIL;
  }

  // Go through all tools.

  int sensorID;
  DATA_FORMAT_TYPE formatType = DOUBLE_POSITION_ANGLES_MATRIX_QUATERNION_TIME_Q_BUTTON;

  for ( sensorID = 0; sensorID < systemConfig.numberSensors; ++ sensorID )
  {
    // Set data format
    this->CheckReturnStatus( SetSensorParameter( sensorID, DATA_FORMAT, &formatType, sizeof( formatType ) ) );

    // Set filtering
    this->CheckReturnStatus( SetSensorParameter( sensorID, FILTER_AC_WIDE_NOTCH, &this->FilterAcWideNotch, sizeof( int ) ) );
    this->CheckReturnStatus( SetSensorParameter( sensorID, FILTER_AC_NARROW_NOTCH, &this->FilterAcNarrowNotch, sizeof( int ) ) );
    this->CheckReturnStatus( SetSensorParameter( sensorID, FILTER_DC_ADAPTIVE, &this->FilterDcAdaptive, sizeof( double ) ) );
    this->CheckReturnStatus( SetSensorParameter( sensorID, FILTER_LARGE_CHANGE, &this->FilterLargeChange, sizeof( int ) ) );

    tagADAPTIVE_PARAMETERS alphaStruct;
    alphaStruct.alphaMin[0] = alphaStruct.alphaMin[1] = alphaStruct.alphaMin[2] = alphaStruct.alphaMin[3]
      = alphaStruct.alphaMin[4] = alphaStruct.alphaMin[5] = alphaStruct.alphaMin[6] = 655;
    alphaStruct.alphaMax[0] = alphaStruct.alphaMax[1] = alphaStruct.alphaMax[2] = alphaStruct.alphaMax[3]
      = alphaStruct.alphaMax[4] = alphaStruct.alphaMax[5] = alphaStruct.alphaMax[6] = 29491;
    alphaStruct.vm[0] = 2;
    alphaStruct.vm[1] = alphaStruct.vm[2] = alphaStruct.vm[3]
      = alphaStruct.vm[4] = alphaStruct.vm[5] = alphaStruct.vm[6] = 4;
    alphaStruct.alphaOn = this->FilterAlpha;
    this->CheckReturnStatus( SetSensorParameter( sensorID, FILTER_ALPHA_PARAMETERS, &alphaStruct, sizeof( alphaStruct ) ) );

    DEVICE_STATUS status;
    status = GetSensorStatus( sensorID );

    this->SensorSaturated.push_back( ( status & SATURATED ) ? true : false );
    this->SensorAttached.push_back( ( status & NOT_ATTACHED ) ? false : true );
    this->SensorInMotion.push_back( ( status & OUT_OF_MOTIONBOX ) ? false : true );
    this->TransmitterAttached = ( ( status & NO_TRANSMITTER_ATTACHED ) ? false : true );
  }

  this->NumberOfSensors = systemConfig.numberSensors; 

  if ( this->AscensionRecordBuffer == NULL )
  {
    this->AscensionRecordBuffer = new AscensionRecordType[this->NumberOfSensors ];
  }

  // Enable tools
  for ( int i = 0; i < this->GetNumberOfSensors(); i++ )
  {
    if ( this->SensorAttached[ i ] )
    {
      std::ostringstream portName; 
      portName << i; 
      vtkPlusDataSource* tool = NULL; 
      if ( this->GetToolByPortName(portName.str().c_str(), tool) != PLUS_SUCCESS )
      {
        LOG_WARNING("Undefined connected tool found on port '" << portName.str() << "', disabled it until not defined in the config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName() ); 
        this->SensorAttached[ i ] = false; 
      }
    }
  }

  // Check that all tools were connected that was defined in the configuration file
  for ( DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    std::stringstream convert(it->second->GetPortName());
    int port(-1); 
    if ( ! (convert >> port ) )
    {
      LOG_ERROR("Failed to convert tool '" << it->second->GetSourceId() << "' port name '" << it->second->GetPortName() << "' to integer, please check config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName() ); 
      return PLUS_FAIL; 
    }

    if ( !this->SensorAttached[ port ] )
    {
      LOG_WARNING("Sensor not attached for tool '" << it->second->GetSourceId() << "' on port name '" << it->second->GetPortName() << "', please check config file: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName() ); 
    }
  }
  return PLUS_SUCCESS; 
}

//-------------------------------------------------------------------------
PlusStatus vtkAscension3DGTrackerBase::InternalDisconnect()
{
  LOG_TRACE( "vtkAscension3DGTracker::Disconnect" ); 
  return this->StopRecording(); 
}

//-------------------------------------------------------------------------
PlusStatus vtkAscension3DGTrackerBase::Probe()
{
  LOG_TRACE( "vtkAscension3DGTracker::Probe" ); 

  return PLUS_SUCCESS; 
} 

//-------------------------------------------------------------------------
PlusStatus vtkAscension3DGTrackerBase::InternalStartRecording()
{
  LOG_TRACE( "vtkAscension3DGTracker::InternalStartRecording" ); 
  if ( this->Recording )
  {
    return PLUS_SUCCESS;
  }

  // Turn on the first attached transmitter.

  BOARD_CONFIGURATION boardConfig;
  this->CheckReturnStatus( GetBoardConfiguration( 0, &boardConfig ) );

  short selectID = TRANSMITTER_OFF;
  int i = 0;
  bool found = false;
  while( ( i < boardConfig.numberTransmitters ) && ( found == false ) )
  {
     TRANSMITTER_CONFIGURATION transConfig;
    this->CheckReturnStatus( GetTransmitterConfiguration( i, &transConfig ) );
    if ( transConfig.attached )
    {
      selectID = i;
      found = true;
    }
    ++ i;
  }

  if (this->CheckReturnStatus( SetSystemParameter( SELECT_TRANSMITTER, &selectID, sizeof( selectID ) ) ) != PLUS_SUCCESS)
  {
    LOG_ERROR("Select transmitter failed");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkAscension3DGTrackerBase::InternalStopRecording()
{
  LOG_TRACE( "vtkAscension3DGTracker::InternalStopRecording" ); 

  short selectID = TRANSMITTER_OFF;
  if (this->CheckReturnStatus( SetSystemParameter( SELECT_TRANSMITTER, &selectID, sizeof( selectID ) ) )
    != PLUS_SUCCESS)
  {
    LOG_ERROR("Select transmitter failed");
    return PLUS_FAIL;
  }

  if (this->CheckReturnStatus( CloseBIRDSystem() )
    != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to close BIRD system");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkAscension3DGTrackerBase::InternalUpdate()
{
  LOG_TRACE( "vtkAscension3DGTracker::InternalUpdate" ); 

  if ( ! this->Recording )
  {
    LOG_ERROR("called Update() when not tracking" );
    return PLUS_FAIL;
  }

  SYSTEM_CONFIGURATION sysConfig;
  if (this->CheckReturnStatus( GetBIRDSystemConfiguration( &sysConfig ) )
    != PLUS_SUCCESS)
  {
    LOG_ERROR("Cannot get system configuration");
    return PLUS_FAIL;
  }

  if ( this->GetNumberOfSensors() != sysConfig.numberSensors )
  {
    LOG_ERROR( "Changing sensors while tracking is not supported. Reconnect necessary." );
    this->StopRecording();
    this->Disconnect();
    return PLUS_FAIL;
  }

  AscensionRecordType* record = static_cast<AscensionRecordType*>(this->AscensionRecordBuffer); 
  if ( record == NULL )
  {
    LOG_ERROR( "Ascension record buffer is invalid, reconnect necessary." );
    return PLUS_FAIL; 
  }

#ifdef ATC_READ_ALL_SENSOR_AT_ONCE
  // Request data for all the sensors at once
  if (this->CheckReturnStatus(   GetAsynchronousRecord( ALL_SENSORS, record, sysConfig.numberSensors * sizeof( AscensionRecordType ) ) ) != PLUS_SUCCESS)
  {
    LOG_ERROR("Cannot get synchronous record");
    return PLUS_FAIL;
  }
#else
  // Request data from each sensor one-by-one. This method works well in some cases (with 3DGm systems?) when the
  // ALL_SENSORS data request times out.
  // Scan the sensors and request a record if the sensor is physically attached
  for(int sensorIndex=0;sensorIndex<sysConfig.numberSensors;sensorIndex++)
  {
    if(SensorAttached[sensorIndex])
    {
      // sensor attached so get record
      if (this->CheckReturnStatus( GetAsynchronousRecord(sensorIndex, record+sensorIndex, sizeof(*record)) ) != PLUS_SUCCESS)
      {
        LOG_ERROR("Cannot get synchronous record for sensor "<<sensorIndex);
        return PLUS_FAIL;
      }
    }
  }
#endif

  // Set up reference matrix.

  bool saturated(false), attached(false), inMotionBox(false);
  bool transmitterRunning(false), transmitterAttached(false), globalError(false);

  ToolStatus toolStatus = TOOL_OK;
  const double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();
  int numberOfErrors(0); 
  // Vector to store the quality value for each tool
  std::vector<unsigned short> qualityValues(sysConfig.numberSensors,0);
  for ( unsigned short sensorIndex = 0; sensorIndex < sysConfig.numberSensors; ++ sensorIndex )
  {
    if ( ! SensorAttached [ sensorIndex ] )
    {
      // Sensor disabled because it was not defined in the configuration file
      continue; 
    }

    DEVICE_STATUS status = GetSensorStatus( sensorIndex );

    saturated = status & SATURATED;
    attached = ! ( status & NOT_ATTACHED );
    inMotionBox = ! ( status & OUT_OF_MOTIONBOX );
    transmitterRunning = !( status & NO_TRANSMITTER_RUNNING );
    if( !transmitterRunning )
    {
      LOG_WARNING("Attempting to produce data but sensor is not running.");
    }
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
    qualityValues[sensorIndex] = record[sensorIndex].quality;
    
    mToolToTracker->Invert();

    mToolToTracker->SetElement( 0, 3, record[ sensorIndex ].x );
    mToolToTracker->SetElement( 1, 3, record[ sensorIndex ].y );
    mToolToTracker->SetElement( 2, 3, record[ sensorIndex ].z );

    if ( ! attached ) toolStatus = TOOL_MISSING;
    if ( ! inMotionBox ) toolStatus = TOOL_OUT_OF_VIEW;
    
    std::ostringstream toolPortName; 
    toolPortName << sensorIndex; 

    vtkPlusDataSource* tool = NULL;
    if ( this->GetToolByPortName(toolPortName.str().c_str(), tool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to find tool on port: " << toolPortName.str() ); 
      numberOfErrors++; 
      continue; 
    }
          
    // Devices has no frame numbering, so just auto increment tool frame number
    unsigned long frameNumber = tool->GetFrameNumber() + 1 ; 
    this->ToolTimeStampedUpdate( tool->GetSourceId(), mToolToTracker, toolStatus, frameNumber, unfilteredTimestamp);
  }
  
  vtkPlusDataSource* qualityTool = NULL;
  if ( this->GetToolByPortName(QUALITY_PORT_NAME, qualityTool) == PLUS_SUCCESS && qualityTool!=NULL )
  {
    vtkSmartPointer< vtkMatrix4x4 > qualityStorageMatrix = vtkSmartPointer< vtkMatrix4x4 >::New();    
    qualityStorageMatrix->SetElement(3,0,-1);
    qualityStorageMatrix->SetElement(3,1,-1);
    qualityStorageMatrix->SetElement(3,2,-1);
    qualityStorageMatrix->SetElement(0,0,-1);
    qualityStorageMatrix->SetElement(1,1,-1);
    qualityStorageMatrix->SetElement(2,2,-1);
    // The "quality" tool is defined, which contains the quality values for all the tools
    for ( unsigned short sensorIndex = 0; sensorIndex < sysConfig.numberSensors; ++sensorIndex )
    {
      switch (sensorIndex)
      {
      case 0: qualityStorageMatrix->SetElement(3,0,qualityValues[sensorIndex]); break;
      case 1: qualityStorageMatrix->SetElement(3,1,qualityValues[sensorIndex]); break;
      case 2: qualityStorageMatrix->SetElement(3,2,qualityValues[sensorIndex]); break;
      case 3: qualityStorageMatrix->SetElement(0,0,qualityValues[sensorIndex]); break;
      case 4: qualityStorageMatrix->SetElement(1,1,qualityValues[sensorIndex]); break;
      case 5: qualityStorageMatrix->SetElement(2,2,qualityValues[sensorIndex]); break;
      default:
        // Usually only 4 tools can be connected to the tracker, so 6 values should cover everything.
        // If we need to store more quality values then we can store more matrices.
        LOG_WARNING("Quality value of sensor "<<sensorIndex<<" cannot be stored. Quality values can only be stored for maximum 6 tools.");
      }
    }
    // Devices has no frame numbering, so just auto increment tool frame number
    unsigned long frameNumber = qualityTool->GetFrameNumber() + 1 ; 
    this->ToolTimeStampedUpdate( qualityTool->GetSourceId(), qualityStorageMatrix, TOOL_OK, frameNumber, unfilteredTimestamp);
  }

  return (numberOfErrors > 0 ? PLUS_FAIL : PLUS_SUCCESS);
}

//-------------------------------------------------------------------------
PlusStatus vtkAscension3DGTrackerBase::CheckReturnStatus( int status )
{
  if( status != BIRD_ERROR_SUCCESS )
  {
    char buffer[ 512 ];
    GetErrorText( status, buffer, sizeof( buffer ), SIMPLE_MESSAGE );
    LOG_ERROR(buffer);
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkAscension3DGTrackerBase::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  if ( rootConfigElement == NULL ) 
  {
    LOG_WARNING("Unable to find BrachyTracker XML data element");
    return PLUS_FAIL; 
  }

  // Read superclass configuration first
  if( Superclass::ReadConfiguration(rootConfigElement) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to continue configuration of Ascension tracker. Generic device configuration failed.");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* trackerConfig = this->FindThisDeviceElement(rootConfigElement);
  if (trackerConfig == NULL) 
  {
    LOG_ERROR("Cannot find Tracker element in XML tree!");
    return PLUS_FAIL;
  }

  int filterAcWideNotch = 0; 
  if ( trackerConfig->GetScalarAttribute("FilterAcWideNotch", filterAcWideNotch) ) 
  {
    this->SetFilterAcWideNotch(filterAcWideNotch); 
  }

  int filterAcNarrowNotch = 0; 
  if ( trackerConfig->GetScalarAttribute("FilterAcNarrowNotch", filterAcNarrowNotch) ) 
  {
    this->SetFilterAcNarrowNotch(filterAcNarrowNotch); 
  }

  double filterDcAdaptive = 0.0; 
  if ( trackerConfig->GetScalarAttribute("FilterDcAdaptive", filterDcAdaptive) ) 
  {
    this->SetFilterDcAdaptive(filterDcAdaptive); 
  }

  int filterLargeChange = 0; 
  if ( trackerConfig->GetScalarAttribute("FilterLargeChange", filterLargeChange) ) 
  {
    this->SetFilterLargeChange(filterLargeChange); 
  }

  int filterAlpha = 0;
  if ( trackerConfig->GetScalarAttribute("FilterAlpha", filterAlpha) ) 
  {
    this->SetFilterAlpha(filterAlpha>0?true:false); 
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkAscension3DGTrackerBase::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  if ( rootConfigElement == NULL )
  {
    LOG_ERROR("Configuration is invalid");
    return PLUS_FAIL;
  }

  // Write configuration 
  Superclass::WriteConfiguration(rootConfigElement); 

  vtkXMLDataElement* trackerConfig = this->FindThisDeviceElement(rootConfigElement);
  if ( trackerConfig == NULL) 
  {
    LOG_ERROR("Cannot find Tracker element in XML tree!");
    return PLUS_FAIL;
  }

  trackerConfig->SetIntAttribute("FilterAcWideNotch", this->GetFilterAcWideNotch()); 
  trackerConfig->SetIntAttribute("FilterAcNarrowNotch", this->GetFilterAcNarrowNotch()); 
  trackerConfig->SetDoubleAttribute("FilterDcAdaptive", this->GetFilterDcAdaptive()); 
  trackerConfig->SetIntAttribute("FilterLargeChange", this->GetFilterLargeChange()); 
  trackerConfig->SetIntAttribute("FilterAlpha", (this->GetFilterAlpha()?1:0));

  return PLUS_SUCCESS;
}
