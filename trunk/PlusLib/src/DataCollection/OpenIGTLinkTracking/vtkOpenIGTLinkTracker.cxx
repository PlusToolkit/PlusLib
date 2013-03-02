/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "igtlMessageHeader.h"
#include "igtlPlusClientInfoMessage.h"
#include "igtlPositionMessage.h"
#include "igtlTrackingDataMessage.h"
#include "igtlTransformMessage.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkOpenIGTLinkTracker.h"
#include "vtkPlusIgtlMessageCommon.h"
#include "vtkPlusStreamBuffer.h"
#include "vtkPlusDataSource.h"
#include "vtkTransform.h"
#include "vtkXMLDataElement.h"
#include "vtksys/SystemTools.hxx"
#include <sstream>
#include <set>

static const int CLIENT_SOCKET_TIMEOUT_MSEC = 500; 

vtkStandardNewMacro(vtkOpenIGTLinkTracker);

//----------------------------------------------------------------------------
vtkOpenIGTLinkTracker::vtkOpenIGTLinkTracker()
: MessageType(NULL)
, ServerAddress(NULL)
, ServerPort(-1)
, NumberOfRetryAttempts(10)
, DelayBetweenRetryAttemptsSec(0.100) // there is already a delay with a CLIENT_SOCKET_TIMEOUT_MSEC timeout, so we just add a little extra idle delay
, IgtlMessageCrcCheckEnabled(0)
, ClientSocket(igtl::ClientSocket::New())
, ReconnectOnReceiveTimeout(true)
, TrackerInternalCoordinateSystemName(NULL)
{
  this->RequireDeviceImageOrientationInDeviceSetConfiguration = false;
  this->RequireFrameBufferSizeInDeviceSetConfiguration = false;
  this->RequireAcquisitionRateInDeviceSetConfiguration = false;
  this->RequireAveragedItemsForFilteringInDeviceSetConfiguration = false;
  this->RequireToolAveragedItemsForFilteringInDeviceSetConfiguration = true;
  this->RequireLocalTimeOffsetSecInDeviceSetConfiguration = false;
  this->RequireUsImageOrientationInDeviceSetConfiguration = false;
  this->RequireRfElementInDeviceSetConfiguration = false;
  this->TrackerInternalCoordinateSystemName=NULL;
  SetTrackerInternalCoordinateSystemName("Reference");

  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates=true;
}

//----------------------------------------------------------------------------
vtkOpenIGTLinkTracker::~vtkOpenIGTLinkTracker() 
{
  if ( this->Recording )
  {
    this->StopRecording();
  }
  SetTrackerInternalCoordinateSystemName(NULL);
}

//----------------------------------------------------------------------------
void vtkOpenIGTLinkTracker::PrintSelf( ostream& os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
std::string vtkOpenIGTLinkTracker::GetSdkVersion()
{
  std::ostringstream version; 
  version << "OpenIGTLink v" << PLUS_OPENIGTLINK_VERSION; 
  return version.str(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkTracker::InternalConnect()
{
  LOG_TRACE( "vtkOpenIGTLinkTracker::InternalConnect" ); 

  if ( this->ClientSocket->GetConnected() )
  {
    return PLUS_SUCCESS; 
  }

  if ( this->ServerAddress == NULL )
  {
    LOG_ERROR("Unable to connect OpenIGTLink server - server address is undefined" ); 
    return PLUS_FAIL; 
  }

  if ( this->ServerPort < 0 )
  {
    LOG_ERROR("Unable to connect OpenIGTLink server - server port is invalid: " << this->ServerPort ); 
    return PLUS_FAIL; 
  }

  int errorCode = 0; // 0 means success
  RETRY_UNTIL_TRUE( 
    (errorCode = this->ClientSocket->ConnectToServer( this->ServerAddress, this->ServerPort ))==0,
    this->NumberOfRetryAttempts, this->DelayBetweenRetryAttemptsSec);

  if ( errorCode != 0 )
  {
    LOG_ERROR( "Cannot connect to the server (" << this->ServerAddress << ":" << this->ServerPort << ")." );
    return PLUS_FAIL;
  }
  else
  {
    LOG_DEBUG( "Client successfully connected to server (" << this->ServerAddress << ":" << this->ServerPort << ")."  );
  }

  this->ClientSocket->SetTimeout(CLIENT_SOCKET_TIMEOUT_MSEC); 

  // Clear buffers on connect
  this->ClearAllBuffers(); 

  // If we need TDATA, request server to start streaming.
  if ( std::string( this->MessageType ).compare( "TDATA" ) == 0 )
  {
    igtl::StartTrackingDataMessage::Pointer sttMsg = igtl::StartTrackingDataMessage::New();
    sttMsg->SetDeviceName("");
    sttMsg->SetResolution(50);
    sttMsg->SetCoordinateName(this->TrackerInternalCoordinateSystemName);
    sttMsg->Pack();

    int retValue = 0;
    RETRY_UNTIL_TRUE( 
      (retValue = this->ClientSocket->Send( sttMsg->GetPackPointer(), sttMsg->GetPackSize() ))!=0,
      this->NumberOfRetryAttempts, this->DelayBetweenRetryAttemptsSec);

    if ( retValue == 0 )
    {
      LOG_ERROR("Failed to send STT_TDATA message to server!"); 
      return PLUS_FAIL; 
    }
  }

  // If we specified message type, try to send it to the server
  if ( this->MessageType != NULL )
  {
    // Send client info request to the server
    PlusIgtlClientInfo clientInfo; 
    // Set message type
    clientInfo.IgtlMessageTypes.push_back(this->MessageType); 

    // We need the following tool names from the server 
    for ( DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it )
    {
      PlusTransformName tName( it->second->GetSourceId(), this->GetToolReferenceFrameName() ); 
      clientInfo.TransformNames.push_back( tName ); 
    }

    // Pack client info message 
    igtl::PlusClientInfoMessage::Pointer clientInfoMsg = igtl::PlusClientInfoMessage::New(); 
    clientInfoMsg->SetClientInfo(clientInfo); 
    clientInfoMsg->Pack(); 

    // Send message to server 
    int retValue = 0;
    RETRY_UNTIL_TRUE( 
      (retValue = this->ClientSocket->Send( clientInfoMsg->GetPackPointer(), clientInfoMsg->GetPackSize() ))!=0,
      this->NumberOfRetryAttempts, this->DelayBetweenRetryAttemptsSec);

    if ( retValue == 0 )
    {
      LOG_ERROR("Failed to send PlusClientInfo message to server!"); 
      return PLUS_FAIL; 
    }
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkTracker::InternalDisconnect()
{
  LOG_TRACE( "vtkOpenIGTLinkTracker::Disconnect" ); 

  // If we need TDATA, request server to stop streaming.
  if ( std::string( this->MessageType ).compare( "TDATA" ) == 0 )
  {
    igtl::StopTrackingDataMessage::Pointer stpMsg = igtl::StopTrackingDataMessage::New();
    stpMsg->SetDeviceName("");
    stpMsg->Pack();

    int retValue = 0;
    RETRY_UNTIL_TRUE( 
      (retValue = this->ClientSocket->Send( stpMsg->GetPackPointer(), stpMsg->GetPackSize() ))!=0,
      this->NumberOfRetryAttempts, this->DelayBetweenRetryAttemptsSec);

    if ( retValue == 0 )
    {
      LOG_ERROR("Failed to send STP_TDATA message to server!"); 
      return PLUS_FAIL; 
    } 
  }

  this->ClientSocket->CloseSocket(); 
  return this->StopRecording(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkTracker::Probe()
{
  LOG_TRACE( "vtkOpenIGTLinkTracker::Probe" ); 

  PlusStatus trackerStatus = PLUS_FAIL; 
  if ( this->Connect() == PLUS_SUCCESS )
  {
    trackerStatus = PLUS_SUCCESS; 
    this->Disconnect(); 
  }
  
  return trackerStatus; 
} 

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkTracker::InternalStartRecording()
{
  LOG_TRACE( "vtkOpenIGTLinkTracker::InternalStopRecording" ); 
  if ( this->Recording )
  {
    return PLUS_SUCCESS;
  }

  return this->Connect(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkTracker::InternalStopRecording()
{
  LOG_TRACE( "vtkOpenIGTLinkTracker::InternalStopRecording" ); 
  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkTracker::InternalUpdate()
{
  LOG_TRACE( "vtkOpenIGTLinkTracker::InternalUpdate" ); 

  if ( ! this->Recording )
  {
    LOG_ERROR("called Update() when not tracking" );
    return PLUS_FAIL;
  }

  igtl::MessageHeader::Pointer headerMsg;
  headerMsg = igtl::MessageHeader::New();
  headerMsg->InitPack();

  int numOfBytesReceived = 0;
  RETRY_UNTIL_TRUE( 
    (numOfBytesReceived = this->ClientSocket->Receive( headerMsg->GetPackPointer(), headerMsg->GetPackSize() ))!=0,
    this->NumberOfRetryAttempts, this->DelayBetweenRetryAttemptsSec);
   
  if ( numOfBytesReceived == 0 ) 
  {
    // No message received - data has not been sent yet
    LOG_WARNING("No data coming from OpenIGTLink Tracker!");
    if( this->GetReconnectOnReceiveTimeout() )
    {
      this->ClientSocket->CloseSocket(); 
      return this->Connect(); 
    }
    return PLUS_FAIL;
  }

  // Received data is not as we expected
  if ( numOfBytesReceived != headerMsg->GetPackSize() )
  {
    LOG_ERROR("Couldn't receive data from OpenIGTLink tracker"); 
    return PLUS_FAIL; 
  }

  vtkSmartPointer<vtkMatrix4x4> toolMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  double unfilteredTimestampUtc = 0; 

  igtl::TimeStamp::Pointer igtlTimestamp = igtl::TimeStamp::New(); 
  igtl::Matrix4x4 igtlMatrix;
  igtl::IdentityMatrix(igtlMatrix);
  std::string igtlTransformName; 
  
  headerMsg->Unpack(this->IgtlMessageCrcCheckEnabled);
  if (strcmp(headerMsg->GetDeviceType(), "TRANSFORM") == 0)
  {
    if ( vtkPlusIgtlMessageCommon::UnpackTransformMessage(headerMsg, this->ClientSocket.GetPointer(), toolMatrix, igtlTransformName, unfilteredTimestampUtc, this->IgtlMessageCrcCheckEnabled) != PLUS_SUCCESS )
    {
      LOG_ERROR("Couldn't receive transform message from server!"); 
      return PLUS_FAIL;
    }
  }
  else if (strcmp(headerMsg->GetDeviceType(), "POSITION") == 0)
  {
    float position[3] = {0}; 
    if ( vtkPlusIgtlMessageCommon::UnpackPositionMessage(headerMsg, this->ClientSocket.GetPointer(), position, igtlTransformName, unfilteredTimestampUtc, this->IgtlMessageCrcCheckEnabled) != PLUS_SUCCESS )
    {
      LOG_ERROR("Couldn't receive position message from server!"); 
      return PLUS_FAIL;
    }

    toolMatrix->Identity(); 
    toolMatrix->SetElement(0,3, position[0]); 
    toolMatrix->SetElement(1,3, position[1]); 
    toolMatrix->SetElement(2,3, position[2]); 
  }
  else if (strcmp( headerMsg->GetDeviceType(), "TDATA" ) == 0 )
  {
    igtl::TrackingDataMessage::Pointer tdataMsg = igtl::TrackingDataMessage::New();
    tdataMsg->SetMessageHeader( headerMsg );
    tdataMsg->AllocatePack();

    this->ClientSocket->Receive( tdataMsg->GetPackBodyPointer(), tdataMsg->GetPackBodySize() );
    int c = tdataMsg->Unpack( this->IgtlMessageCrcCheckEnabled );
    if ( ! ( c & igtl::MessageHeader::UNPACK_BODY ) )
    {
      LOG_ERROR( "Couldn't receive TDATA message from server!" );
      return PLUS_FAIL;
    }

    // for now just use system time, all coordinates will be sequential.
    double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();
    double filteredTimestamp = unfilteredTimestamp; // No need to filter already filtered timestamped items received over OpenIGTLink 
    // We store the list of identified tools (tools we get information about from the tracker).
    // The tools that are missing from the tracker message are assumed to be out of view. 
    std::set<std::string> identifiedToolNames;
    for ( int i = 0; i < tdataMsg->GetNumberOfTrackingDataElements(); ++ i )
    {
      igtl::TrackingDataElement::Pointer tdataElem = igtl::TrackingDataElement::New();
      tdataMsg->GetTrackingDataElement( i, tdataElem );

      igtl::Matrix4x4 igtlMatrix;
      tdataElem->GetMatrix(igtlMatrix);
      vtkSmartPointer<vtkMatrix4x4> toolMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
      // convert igtl matrix to vtk matrix 
      for ( int r = 0; r < 4; r++ )
      {
        for ( int c = 0; c < 4; c++ )
        {
          toolMatrix->SetElement(r,c, igtlMatrix[r][c]); 
        }
      }

      // Get timestamp 
      igtl::TimeStamp::Pointer igtlTimestamp = igtl::TimeStamp::New(); 
      tdataMsg->GetTimeStamp(igtlTimestamp); 
      unfilteredTimestampUtc = igtlTimestamp->GetTimeStamp();  

      // Get igtl transform name 
      igtlTransformName = tdataElem->GetName();

      // Set internal transform name
      PlusTransformName transformName(igtlTransformName.c_str(), this->TrackerInternalCoordinateSystemName);
      if ( this->ToolTimeStampedUpdateWithoutFiltering(transformName.From().c_str(), toolMatrix, TOOL_OK, unfilteredTimestamp, filteredTimestamp) == PLUS_SUCCESS )
      {
        identifiedToolNames.insert(transformName.From());
      }
      else
      {
        LOG_ERROR("ToolTimeStampedUpdate failed for tool: " << transformName.From() << " with timestamp: " << std::fixed << unfilteredTimestamp); 
        // DO NOT return here: we want to update the other tools.
      }
    }
    // Set status for non-detected tools
    vtkSmartPointer<vtkMatrix4x4> toolMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    toolMatrix->Identity();
    for ( DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
    {    
      if (identifiedToolNames.find(it->second->GetSourceId())!=identifiedToolNames.end())
      {
        // this tool has been found and update has been already called with the correct transform
        LOG_TRACE("Tool "<<it->second->GetSourceId()<<": found");
        continue;
      }
      LOG_TRACE("Tool "<<it->second->GetSourceId()<<": not found");
      this->ToolTimeStampedUpdateWithoutFiltering(it->second->GetSourceId(), toolMatrix, TOOL_OUT_OF_VIEW, unfilteredTimestamp, filteredTimestamp);
    }
    return PLUS_SUCCESS;
  }
  else
  {
    // if the data type is unknown, skip reading. 
    this->ClientSocket->Skip(headerMsg->GetBodySizeToRead(), 0);
    return PLUS_SUCCESS; 
  }

  // Set transform name
  PlusTransformName transformName;
  if ( transformName.SetTransformName( igtlTransformName.c_str() ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to update tracker tool - unrecognized transform name: " << igtlTransformName ); 
    return PLUS_FAIL; 
  }
  
  // Convert timestamp from UTC to system  
  double unfilteredTimestamp = vtkAccurateTimer::GetSystemTimeFromUniversalTime(unfilteredTimestampUtc); 

  double filteredTimestamp = unfilteredTimestamp; // No need to filter already filtered timestamped items received over OpenIGTLink 
  if ( this->ToolTimeStampedUpdateWithoutFiltering(transformName.From().c_str(), toolMatrix, TOOL_OK, unfilteredTimestamp, filteredTimestamp) != PLUS_SUCCESS )
  {
    LOG_ERROR("ToolTimeStampedUpdate failed for tool: " << transformName.From() << " with timestamp: " << std::fixed << unfilteredTimestamp); 
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkTracker::ReadConfiguration( vtkXMLDataElement* config )
{
  // Read superclass configuration first
  Superclass::ReadConfiguration( config ); 

  LOG_TRACE( "vtkOpenIGTLinkTracker::ReadConfiguration" ); 
  if ( config == NULL ) 
  {
    LOG_ERROR("Unable to find OpenIGTLinkTracker XML data element");
    return PLUS_FAIL; 
  }

  vtkXMLDataElement* trackerConfig = this->FindThisDeviceElement(config);
  if (trackerConfig == NULL) 
  {
    LOG_ERROR("Cannot find Tracker element in XML tree!");
		return PLUS_FAIL;
  }

  const char* messageType = trackerConfig->GetAttribute("MessageType"); 
  if ( messageType != NULL )
  {
    this->SetMessageType(messageType); 
  }

  const char* serverAddress = trackerConfig->GetAttribute("ServerAddress"); 
  if ( serverAddress != NULL )
  {
    this->SetServerAddress(serverAddress); 
  }
  else
  {
    LOG_ERROR("Unable to find ServerAddress attribute!"); 
    return PLUS_FAIL; 
  }

  int serverPort = -1; 
  if ( trackerConfig->GetScalarAttribute("ServerPort", serverPort ) )
  {
    this->SetServerPort(serverPort); 
  }
  else
  {
    LOG_ERROR("Unable to find ServerPort attribute!"); 
    return PLUS_FAIL; 
  }

  const char* trackerInternalCoordinateSystemName = trackerConfig->GetAttribute("TrackerInternalCoordinateSystemName"); 
  if ( trackerInternalCoordinateSystemName != NULL )
  {
    this->SetTrackerInternalCoordinateSystemName(trackerInternalCoordinateSystemName); 
  }

  const char* reconnect = trackerConfig->GetAttribute("ReconnectOnReceiveTimeout"); 
  if ( reconnect != NULL )
  {
    this->SetReconnectOnReceiveTimeout(STRCASECMP(reconnect, "true") == 0 ? true : false);
  }

  const char* igtlMessageCrcCheckEnabled = trackerConfig->GetAttribute("IgtlMessageCrcCheckEnabled"); 
  if ( igtlMessageCrcCheckEnabled != NULL )
  {
    if ( STRCASECMP(igtlMessageCrcCheckEnabled, "true") == 0 )
    {
      this->SetIgtlMessageCrcCheckEnabled(1);
    }
    else
    {
      this->SetIgtlMessageCrcCheckEnabled(0);
    }
  }
  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkTracker::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
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

  trackerConfig->SetAttribute("MessageType", this->MessageType);
  trackerConfig->SetAttribute("ServerAddress", this->ServerAddress);
  trackerConfig->SetIntAttribute("ServerPort", this->ServerPort);
  trackerConfig->SetAttribute("TrackerInternalCoordinateSystemName", this->TrackerInternalCoordinateSystemName);
  trackerConfig->SetAttribute("ReconnectOnReceiveTimeout", this->ReconnectOnReceiveTimeout?"true":"false");
  trackerConfig->SetAttribute("IgtlMessageCrcCheckEnabled", this->IgtlMessageCrcCheckEnabled?"true":"false");
  
  return PLUS_SUCCESS;
}
