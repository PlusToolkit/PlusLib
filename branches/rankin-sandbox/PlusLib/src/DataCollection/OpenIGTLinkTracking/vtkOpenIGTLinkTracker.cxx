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
#include "vtkPlusBuffer.h"
#include "vtkPlusDataSource.h"
#include "vtkTransform.h"
#include "vtkXMLDataElement.h"
#include "vtksys/SystemTools.hxx"
#include <sstream>
#include <set>

static const int CLIENT_SOCKET_TIMEOUT_MSEC = 500; 

vtkStandardNewMacro(vtkOpenIGTLinkTracker);

#ifdef _WIN32
  #include <Winsock2.h>
#endif

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
, UseLastTransformsOnReceiveTimeout(false)
, UseReceivedTimestamps(true)
{
  this->RequireImageOrientationInConfiguration = false;
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

  // Clear buffers on connect
  this->ClearAllBuffers();   

  if ( this->ClientSocket->GetConnected() )
  {
    return PLUS_SUCCESS; 
  }

  return ClientSocketReconnect();  
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
  PlusStatus socketStatus=ReceiveMessageHeader(headerMsg);
  if (socketStatus==PLUS_FAIL)
  {
    // There is a socket error
    if (this->GetReconnectOnReceiveTimeout())
    {
      LOG_ERROR("Socket error in device "<<this->GetDeviceId()<<": failed to receive OpenIGTLink transforms. Attempt to reconnect.");
      ClientSocketReconnect();
    }
    else
    {
      LOG_ERROR("Socket error in device "<<this->GetDeviceId()<<": failed to receive OpenIGTLink transforms");
    }
  }
  if (headerMsg.IsNull())
  {
    // Has not received data
    if (this->UseLastTransformsOnReceiveTimeout)
    {
      // The server only sends update if a transform is modified, it's not an error
      LOG_TRACE("No OpenIGTLink message has been received in device "<<this->GetDeviceId());
      // Store the last known transform values (useful when the server only notifies about transform changes
      double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();
      StoreMostRecentTransformValues(unfilteredTimestamp);
      return PLUS_SUCCESS;
    }
    else
    {
      if (this->GetReconnectOnReceiveTimeout())
      {
        LOG_WARNING("No OpenIGTLink message has been received in device "<<this->GetDeviceId()<<": failed to receive OpenIGTLink transforms. Attempt to reconnect.");
        ClientSocketReconnect();
      }
      else
      {
        LOG_WARNING("No OpenIGTLink message has been received in device "<<this->GetDeviceId());
      }
      return PLUS_FAIL;
    }
  }

  // We've received valid header data
  headerMsg->Unpack(this->IgtlMessageCrcCheckEnabled);
  if (strcmp( headerMsg->GetDeviceType(), "TDATA" ) == 0 )
  {
    // TDATA message
    return ProcessTDataMessage(headerMsg);
  }
  
  // TRANSFORM or POSITION message
  return ProcessTransformMessage(headerMsg);
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkTracker::ClientSocketReconnect()
{ 
  LOG_DEBUG("Attempt to connect to client socket in device "<<this->GetDeviceId());
  
  if ( this->ClientSocket->GetConnected() )
  {
    this->ClientSocket->CloseSocket();  
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
PlusStatus vtkOpenIGTLinkTracker::ReceiveMessageHeader(igtl::MessageHeader::Pointer &headerMsg)
{  
  headerMsg = igtl::MessageHeader::New();
  headerMsg->InitPack();

  int numOfBytesReceived = 0;
  RETRY_UNTIL_TRUE(
    (numOfBytesReceived = this->ClientSocket->Receive( headerMsg->GetPackPointer(), headerMsg->GetPackSize() ))!=0,
    this->NumberOfRetryAttempts, this->DelayBetweenRetryAttemptsSec);

  if ( numOfBytesReceived > 0)
  {
    // Data is received
    if ( numOfBytesReceived != headerMsg->GetPackSize() )
    {
      // Received data is not as we expected
      LOG_ERROR("Couldn't receive data from OpenIGTLink tracker (unexpected header size)"); 
      headerMsg=NULL;
      return PLUS_FAIL; 
    }
    return PLUS_SUCCESS;
  }
   
  // No data has been received
  headerMsg=NULL; // this will indicate the caller that no data has been read

  bool socketError = numOfBytesReceived<0; /* -1 == SOCKET_ERROR */
#ifdef _WIN32
  // On Windows try to get some more details about the socket error
  if (socketError)
  {     
    int socketErrorCode=WSAGetLastError();
    if (socketErrorCode==WSAETIMEDOUT)
    {
      // timeout, it just means that no data was received, no need to reconnect
      LOG_TRACE("No data coming from OpenIGTLink Tracker (timeout)");
      socketError=false;
    }
    else
    {
      LPTSTR errorMsgPtr = 0;
      if(FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, socketErrorCode, 0, (LPTSTR)&errorMsgPtr, 0, NULL) != 0)
      {
        LOG_DEBUG("No data coming from OpenIGTLink Tracker (socket error: "<<errorMsgPtr<<")");
      }
      else
      {
        LOG_DEBUG("No data coming from OpenIGTLink Tracker (unknown socket error)");
      }
    }
  }
  else
  {
    LOG_DEBUG("No data coming from OpenIGTLink Tracker");
  }
#else
  LOG_DEBUG("No data coming from OpenIGTLink Tracker");
#endif  
   
  return socketError ? PLUS_FAIL : PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkTracker::StoreMostRecentTransformValues(double unfilteredTimestamp)
{
  PlusStatus status=PLUS_SUCCESS;
  // Set status for tools with non-detected markers
  for ( DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    vtkSmartPointer<vtkMatrix4x4> toolMatrix=vtkSmartPointer<vtkMatrix4x4>::New();
    // retrieve latest transform value from the buffer
    if (it->second->GetBuffer()->GetNumberOfItems()>0)
    {
      BufferItemUidType latestItemUid = it->second->GetBuffer()->GetLatestItemUidInBuffer();
      StreamBufferItem item; 
      if ( it->second->GetBuffer()->GetStreamBufferItem(latestItemUid, &item) == ITEM_OK )
      {
        if (item.GetUnfilteredTimestamp(this->GetLocalTimeOffsetSec()) >= unfilteredTimestamp)
        {
          // this item already has an updated value for this timestamp, no need to store it again
          continue;
        }
        if (item.GetMatrix(toolMatrix)!=PLUS_SUCCESS)
        {
          LOG_WARNING("Failed to get matrix from buffer item with UID: " << latestItemUid );
          status=PLUS_FAIL;
        }
      }
      else
      {
        LOG_WARNING("Failed to get buffer item with UID: " << latestItemUid );
        status=PLUS_FAIL;
      }
    }
    if ( this->ToolTimeStampedUpdateWithoutFiltering(it->second->GetSourceId(), toolMatrix, TOOL_OK, unfilteredTimestamp, unfilteredTimestamp) != PLUS_SUCCESS )
    {
      LOG_INFO("ToolTimeStampedUpdate failed for tool: " << it->second->GetSourceId() << " with timestamp: " << std::fixed << unfilteredTimestamp); 
      status=PLUS_FAIL;
    }
  }
  
  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkTracker::ProcessTDataMessage(igtl::MessageHeader::Pointer headerMsg)
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
    double unfilteredTimestampUtc = igtlTimestamp->GetTimeStamp();  

    // Get igtl transform name 
    std::string igtlTransformName = tdataElem->GetName();

    // Set internal transform name
    PlusTransformName transformName(igtlTransformName.c_str(), this->TrackerInternalCoordinateSystemName);
    if ( this->ToolTimeStampedUpdateWithoutFiltering(transformName.GetTransformName().c_str(), toolMatrix, TOOL_OK, unfilteredTimestamp, filteredTimestamp) == PLUS_SUCCESS )
    {
      identifiedToolNames.insert(transformName.GetTransformName());
    }
    else
    {
      LOG_INFO("ToolTimeStampedUpdate failed for tool: " << transformName.From() << " with timestamp: " << std::fixed << unfilteredTimestamp); 
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

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkTracker::ProcessTransformMessage(igtl::MessageHeader::Pointer headerMsg)
{
  double unfilteredTimestampUtc = 0; 
  vtkSmartPointer<vtkMatrix4x4> toolMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  std::string igtlTransformName; 
  
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
    
  double unfilteredTimestamp = 0;
  if (this->UseReceivedTimestamps)
  {
    // Use the timestamp in the OpenIGTLink message
    // The received timestamp is in UTC and timestampts in the buffer are in system time, so conversion is needed
    unfilteredTimestamp = vtkAccurateTimer::GetSystemTimeFromUniversalTime(unfilteredTimestampUtc); 
  }
  else
  {
    unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();
  }  

  // No need to filter already filtered timestamped items received over OpenIGTLink 
  // If the original timestamps are not used it's still safer not to use filtering, as filtering assumes uniform framerate, which is not guaranteed
  double filteredTimestamp = unfilteredTimestamp;

  // Store the transform that we've just received
  if ( this->ToolTimeStampedUpdateWithoutFiltering(transformName.GetTransformName().c_str(), toolMatrix, TOOL_OK, unfilteredTimestamp, filteredTimestamp) != PLUS_SUCCESS )
  {
    LOG_INFO("ToolTimeStampedUpdate failed for tool: " << transformName.GetTransformName() << " with timestamp: " << std::fixed << unfilteredTimestamp); 
    return PLUS_FAIL;
  }
  if ( this->UseLastTransformsOnReceiveTimeout )
  {
    // Store all the other transforms with the last known value
    StoreMostRecentTransformValues(filteredTimestamp);
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

  const char* useLastTransformsOnReceiveTimeout = trackerConfig->GetAttribute("UseLastTransformsOnReceiveTimeout"); 
  if ( useLastTransformsOnReceiveTimeout != NULL )
  {
    this->UseLastTransformsOnReceiveTimeout = (STRCASECMP(useLastTransformsOnReceiveTimeout, "true") == 0 ? true : false);
  }

  const char* useReceivedTimestamps = trackerConfig->GetAttribute("UseReceivedTimestamps"); 
  if ( useReceivedTimestamps != NULL )
  {
    this->UseReceivedTimestamps = (STRCASECMP(useReceivedTimestamps, "true") == 0 ? true : false);
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
