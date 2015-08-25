/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkOpenIGTLinkDevice.h"

#include "igtlPlusClientInfoMessage.h"
#include "vtkPlusDataSource.h"

static const int CLIENT_SOCKET_TIMEOUT_MSEC = 500;

#ifdef _WIN32
  #include <Winsock2.h>
#endif

//----------------------------------------------------------------------------
vtkOpenIGTLinkDevice::vtkOpenIGTLinkDevice()
: MessageType(NULL)
, ServerAddress(NULL)
, ServerPort(-1)
, NumberOfRetryAttempts(3) // try a few times, but adding of data items is blocked while trying to reconnect, so don't make it too long
, DelayBetweenRetryAttemptsSec(0.100) // there is already a delay with a CLIENT_SOCKET_TIMEOUT_MSEC timeout, so we just add a little extra idle delay
, IgtlMessageCrcCheckEnabled(0)
, ClientSocket(igtl::ClientSocket::New())
, ReconnectOnReceiveTimeout(true)
, UseReceivedTimestamps(true)
{
  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates=true;
  this->AcquisitionRate = 30;
}

//----------------------------------------------------------------------------
vtkOpenIGTLinkDevice::~vtkOpenIGTLinkDevice()
{
  if ( this->Recording )
  {
    this->StopRecording();
  }
  this->ClientSocket = NULL;
}

//----------------------------------------------------------------------------
void vtkOpenIGTLinkDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  if ( this->ServerAddress )
  {
    os << indent << "Server address: " << this->ServerAddress << "\n";
  }
  os << indent << "Server port: " << this->ServerPort << "\n";
  if ( this->MessageType )
  {
    os << indent << "Message type: " << this->MessageType << "\n";
  }

}
//----------------------------------------------------------------------------
std::string vtkOpenIGTLinkDevice::GetSdkVersion()
{
  std::ostringstream version; 
  version << "OpenIGTLink v" << OPENIGTLINK_VERSION; 
  return version.str(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkDevice::InternalConnect()
{
  LOG_TRACE( "vtkOpenIGTLinkDevice::InternalConnect" );

  // Clear buffers on connect
  this->ClearAllBuffers();   

  if ( this->ClientSocket->GetConnected() )
  {
    return PLUS_SUCCESS; 
  }

  return ClientSocketReconnect();
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkDevice::InternalDisconnect()
{
  LOG_TRACE( "vtkOpenIGTLinkDevice::Disconnect" ); 

  this->ClientSocket->CloseSocket(); 
  return this->StopRecording();
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkDevice::Probe()
{
  LOG_TRACE( "vtkOpenIGTLinkDevice::Probe" ); 

  PlusStatus status = PLUS_FAIL; 
  if ( this->Connect() == PLUS_SUCCESS )
  {
    status = PLUS_SUCCESS; 
    this->Disconnect(); 
  }
  
  return status; 
} 

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkDevice::ClientSocketReconnect()
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

  return SendRequestedMessageTypes();
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkDevice::SendRequestedMessageTypes()
{
  // If we specified message type, try to send it to the server
  if ( this->MessageType == NULL )
  {
    return PLUS_SUCCESS;
  }
  // Send client info request to the server
  PlusIgtlClientInfo clientInfo; 
  // Set message type
  clientInfo.IgtlMessageTypes.push_back(this->MessageType); 

  // We need the following tool names from the server 
  for ( DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it )
  {
    PlusTransformName tName( it->second->GetSourceId() ); 
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
  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkOpenIGTLinkDevice::OnReceiveTimeout()
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
}

//----------------------------------------------------------------------------
void vtkOpenIGTLinkDevice::ReceiveMessageHeaderWithErrorHandling(igtl::MessageHeader::Pointer &headerMsg)
{
  PlusStatus socketStatus=ReceiveMessageHeader(headerMsg);
  if (socketStatus==PLUS_FAIL || !this->ClientSocket->GetConnected())
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
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkDevice::ReceiveMessageHeader(igtl::MessageHeader::Pointer &headerMsg)
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
      LOG_ERROR("Couldn't receive data from OpenIGTLink device "<<this->GetDeviceId()<<": (unexpected header size)"); 
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
      LOG_TRACE("No data coming from OpenIGTLink device "<<this->GetDeviceId()<<": (timeout)");
      socketError=false;
    }
    else
    {
      LPTSTR errorMsgPtr = 0;
      if(FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, socketErrorCode, 0, (LPTSTR)&errorMsgPtr, 0, NULL) != 0)
      {
        LOG_DEBUG("No data coming from OpenIGTLink device "<<this->GetDeviceId()<<": (socket error: "<<errorMsgPtr<<")");
      }
      else
      {
        LOG_DEBUG("No data coming from OpenIGTLink device "<<this->GetDeviceId()<<": (unknown socket error)");
      }
    }
  }
  else
  {
    LOG_DEBUG("No data coming from OpenIGTLink device "<<this->GetDeviceId());
  }
#else
  LOG_DEBUG("No data coming from OpenIGTLink device "<<this->GetDeviceId());
#endif

  return socketError ? PLUS_FAIL : PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkDevice::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  XML_READ_STRING_ATTRIBUTE_REQUIRED(ServerAddress, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_REQUIRED(int, ServerPort, deviceConfig);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(MessageType, deviceConfig);  
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(IgtlMessageCrcCheckEnabled, deviceConfig);  
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(UseReceivedTimestamps, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(ReconnectOnReceiveTimeout, deviceConfig);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkDevice::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
  deviceConfig->SetAttribute("ServerAddress", this->ServerAddress);
  deviceConfig->SetIntAttribute("ServerPort", this->ServerPort);
  deviceConfig->SetAttribute("MessageType", this->MessageType);
  deviceConfig->SetAttribute("IgtlMessageCrcCheckEnabled", this->IgtlMessageCrcCheckEnabled?"true":"false");
  deviceConfig->SetAttribute("UseReceivedTimestamps", this->UseReceivedTimestamps?"true":"false");
  deviceConfig->SetAttribute("ReconnectOnReceiveTimeout", this->ReconnectOnReceiveTimeout?"true":"false");
  return PLUS_SUCCESS;
}
