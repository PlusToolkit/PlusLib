/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtkOpenIGTLinkTracker.h"
#include "vtkPlusIgtlMessageCommon.h"

#include <sstream>

#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtksys/SystemTools.hxx"
#include "vtkTransform.h"
#include "vtkXMLDataElement.h"

#include "PlusConfigure.h"
#include "vtkTracker.h"
#include "vtkTrackerTool.h"
#include "vtkPlusDataBuffer.h"

#include "igtlMessageHeader.h"
#include "igtlTransformMessage.h"
#include "igtlPositionMessage.h"
#include "igtlPlusClientInfoMessage.h"

static const int CLIENT_SOCKET_TIMEOUT_MSEC = 500; 

vtkStandardNewMacro(vtkOpenIGTLinkTracker);

//----------------------------------------------------------------------------
vtkOpenIGTLinkTracker::vtkOpenIGTLinkTracker()
{
  this->MessageType = NULL; 
  this->ServerAddress = NULL; 
  this->ServerPort = -1; 
  this->NumberOfRetryAttempts = 10; 
  this->DelayBetweenRetryAttemptsSec = 0.100; // there is already a delay with a CLIENT_SOCKET_TIMEOUT_MSEC timeout, so we just add a little extra idle delay
  this->IgtlMessageCrcCheckEnabled = 0; 
  this->ClientSocket = igtl::ClientSocket::New(); 
}

//----------------------------------------------------------------------------
vtkOpenIGTLinkTracker::~vtkOpenIGTLinkTracker() 
{
  if ( this->Recording )
  {
    this->StopTracking();
  }
}

//----------------------------------------------------------------------------
void vtkOpenIGTLinkTracker::PrintSelf( ostream& os, vtkIndent indent )
{
  vtkTracker::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
std::string vtkOpenIGTLinkTracker::GetSdkVersion()
{
  std::ostringstream version; 
  version << "OpenIGTLink v" << PLUS_OPENIGTLINK_VERSION; 
  return version.str(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkTracker::Connect()
{
  LOG_TRACE( "vtkOpenIGTLinkTracker::Connect" ); 

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

  // If we specified message type, try to send it to the server
  if ( this->MessageType != NULL )
  {
    // Send clinet info request to the server
    PlusIgtlClientInfo clientInfo; 
    // Set message type
    clientInfo.IgtlMessageTypes.push_back(this->MessageType); 

    // We need the following tool names from the server 
    for ( ToolIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it )
    {
      PlusTransformName tName( it->second->GetToolName(), this->GetToolReferenceFrameName() ); 
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
PlusStatus vtkOpenIGTLinkTracker::Disconnect()
{
  LOG_TRACE( "vtkOpenIGTLinkTracker::Disconnect" ); 
  this->ClientSocket->CloseSocket(); 
  return this->StopTracking(); 
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
PlusStatus vtkOpenIGTLinkTracker::InternalStartTracking()
{
  LOG_TRACE( "vtkOpenIGTLinkTracker::InternalStartTracking" ); 
  if ( this->Recording )
  {
    return PLUS_SUCCESS;
  }

  return this->Connect(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkTracker::InternalStopTracking()
{
  LOG_TRACE( "vtkOpenIGTLinkTracker::InternalStopTracking" ); 
  
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
	// No message received - server disconnected
    LOG_ERROR("OpenIGTLink tracker connection lost with server - try to reconnect!");
    this->ClientSocket->CloseSocket(); 
    return this->Connect(); 
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

	vtkXMLDataElement* dataCollectionConfig = config->FindNestedElementWithName("DataCollection");
	if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
		return PLUS_FAIL;
	}

  vtkXMLDataElement* trackerConfig = dataCollectionConfig->FindNestedElementWithName("Tracker"); 
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
