/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkOpenIGTLinkVideoSource.h"

#include "PlusVideoFrame.h"
#include "TrackedFrame.h"
#include "igtlImageMessage.h"
#include "igtlMessageHeader.h"
#include "igtlPlusClientInfoMessage.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkPlusBuffer.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusIgtlMessageCommon.h"

#include "vtksys/SystemTools.hxx"

#include <vector>
#include <string>

static const int CLIENT_SOCKET_TIMEOUT_MSEC = 500; 

vtkStandardNewMacro(vtkOpenIGTLinkVideoSource);

#ifdef _WIN32
  #include <Winsock2.h>
#endif

//----------------------------------------------------------------------------
vtkOpenIGTLinkVideoSource::vtkOpenIGTLinkVideoSource()
: MessageType(NULL)
, ServerAddress(NULL)
, ServerPort(-1)
, NumberOfRetryAttempts(10)
, DelayBetweenRetryAttemptsSec(0.100) // there is already a delay with a CLIENT_SOCKET_TIMEOUT_MSEC timeout, so we just add a little extra idle delay
, IgtlMessageCrcCheckEnabled(0)
, ClientSocket(igtl::ClientSocket::New())
, ReconnectOnReceiveTimeout(true)
, UseReceivedTimestamps(true)
{
  this->RequireImageOrientationInConfiguration = true;

  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates=true;
  this->AcquisitionRate = 30;
}

//----------------------------------------------------------------------------
vtkOpenIGTLinkVideoSource::~vtkOpenIGTLinkVideoSource()
{
  if ( this->Recording )
  {
    this->StopRecording();
  }
  this->ClientSocket = NULL;
}

//----------------------------------------------------------------------------
void vtkOpenIGTLinkVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  if ( this->ServerAddress )
  {
    os << indent << "Server address: " << this->ServerAddress << "\n";
  }
  os << indent << "Server port: " << this->ServerPort << "\n";
  if ( this->MessageType )
  {
    os << indent << "Message type: " << this->ServerAddress << "\n";
  }

}
//----------------------------------------------------------------------------
std::string vtkOpenIGTLinkVideoSource::GetSdkVersion()
{
  std::ostringstream version; 
  version << "OpenIGTLink v" << OPENIGTLINK_VERSION; 
  return version.str(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkVideoSource::InternalConnect()
{
  LOG_TRACE( "vtkOpenIGTLinkVideoSource::InternalConnect" );

  // Clear buffers on connect
  this->ClearAllBuffers();   

  if ( this->ClientSocket->GetConnected() )
  {
    return PLUS_SUCCESS; 
  }

  return ClientSocketReconnect();
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkVideoSource::InternalDisconnect()
{
  LOG_TRACE( "vtkOpenIGTLinkVideoSource::Disconnect" ); 

  this->ClientSocket->CloseSocket(); 
  return this->StopRecording();
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkVideoSource::Probe()
{
  LOG_TRACE( "vtkOpenIGTLinkVideoSource::Probe" ); 

  PlusStatus status = PLUS_FAIL; 
  if ( this->Connect() == PLUS_SUCCESS )
  {
    status = PLUS_SUCCESS; 
    this->Disconnect(); 
  }
  
  return status; 
} 

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkVideoSource::InternalUpdate()
{
  LOG_TRACE( "vtkOpenIGTLinkVideoSource::InternalUpdate" );
  if (!this->Recording)
  {
    // drop the frame, we are not recording data now
    return PLUS_SUCCESS;
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

  // We've received valid header data
  headerMsg->Unpack(this->IgtlMessageCrcCheckEnabled);

  // Set unfiltered and filtered timestamp by converting UTC to system timestamp
  double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();

  TrackedFrame trackedFrame;
  if (strcmp(headerMsg->GetDeviceType(), "IMAGE") == 0)
  {
    if (vtkPlusIgtlMessageCommon::UnpackImageMessage( headerMsg, this->ClientSocket, trackedFrame, this->ImageMessageEmbeddedTransformName, this->IgtlMessageCrcCheckEnabled)!=PLUS_SUCCESS)
    {
      LOG_ERROR("Couldn't get image from OpenIGTLink server!"); 
      return PLUS_FAIL;
    }
  }
  else if (strcmp(headerMsg->GetDeviceType(), "TRACKEDFRAME") == 0)
  {
    if ( vtkPlusIgtlMessageCommon::UnpackTrackedFrameMessage( headerMsg, this->ClientSocket, trackedFrame, this->IgtlMessageCrcCheckEnabled ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Couldn't get tracked frame from OpenIGTLink server!"); 
      return PLUS_FAIL; 
    }
    double unfilteredTimestampUtc = trackedFrame.GetTimestamp();
    if (this->UseReceivedTimestamps)
    {
      // Use the timestamp in the OpenIGTLink message
      // The received timestamp is in UTC and timestampts in the buffer are in system time, so conversion is needed
      unfilteredTimestamp = vtkAccurateTimer::GetSystemTimeFromUniversalTime(unfilteredTimestampUtc); 
    }
  }
  else
  {
    // if the data type is unknown, skip reading. 
    this->ClientSocket->Skip(headerMsg->GetBodySizeToRead(), 0);
    return PLUS_SUCCESS; 
  }

  // No need to filter already filtered timestamped items received over OpenIGTLink 
  // If the original timestamps are not used it's still safer not to use filtering, as filtering assumes uniform framerate, which is not guaranteed
  double filteredTimestamp = unfilteredTimestamp;

  // The timestamps are already defined, so we don't need to filter them, 
  // for simplicity, we increase frame number always by 1.
  this->FrameNumber++;

  vtkPlusDataSource* aSource=NULL;
  if( this->GetFirstActiveOutputVideoSource(aSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve the video source in the OpenIGTLinkVideo device.");
    return PLUS_FAIL;
  }

  // If the buffer is empty, set the pixel type and frame size to the first received properties 
  if ( aSource->GetBuffer()->GetNumberOfItems() == 0 )
  {
    PlusVideoFrame* videoFrame=trackedFrame.GetImageData();
    if (videoFrame==NULL)
    {
      LOG_ERROR("Invalid video frame received, cannot use it to initialize the video buffer");
      return PLUS_FAIL;
    }
    aSource->GetBuffer()->SetPixelType( videoFrame->GetVTKScalarPixelType() );
    aSource->GetBuffer()->SetNumberOfScalarComponents( videoFrame->GetNumberOfScalarComponents() );
    aSource->GetBuffer()->SetImageType( videoFrame->GetImageType() );
    aSource->GetBuffer()->SetFrameSize( trackedFrame.GetFrameSize() );
  }
  TrackedFrame::FieldMapType customFields=trackedFrame.GetCustomFields();
  PlusStatus status = aSource->GetBuffer()->AddItem( trackedFrame.GetImageData(), this->FrameNumber, unfilteredTimestamp, filteredTimestamp, &customFields); 
  this->Modified();

  return status;
}
//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkVideoSource::ClientSocketReconnect()
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
PlusStatus vtkOpenIGTLinkVideoSource::ReceiveMessageHeader(igtl::MessageHeader::Pointer &headerMsg)
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
      LOG_ERROR("Couldn't receive data from OpenIGTLink video source (unexpected header size)"); 
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
      LOG_TRACE("No data coming from OpenIGTLink video source (timeout)");
      socketError=false;
    }
    else
    {
      LPTSTR errorMsgPtr = 0;
      if(FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, socketErrorCode, 0, (LPTSTR)&errorMsgPtr, 0, NULL) != 0)
      {
        LOG_DEBUG("No data coming from OpenIGTLink video source (socket error: "<<errorMsgPtr<<")");
      }
      else
      {
        LOG_DEBUG("No data coming from OpenIGTLink video source (unknown socket error)");
      }
    }
  }
  else
  {
    LOG_DEBUG("No data coming from OpenIGTLink video source");
  }
#else
  LOG_DEBUG("No data coming from OpenIGTLink video source");
#endif  
   
  return socketError ? PLUS_FAIL : PLUS_SUCCESS;
}



//-----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  XML_READ_STRING_ATTRIBUTE_REQUIRED(ServerAddress, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_REQUIRED(int, ServerPort, deviceConfig);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(MessageType, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(IgtlMessageCrcCheckEnabled, deviceConfig);  
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(ImageMessageEmbeddedTransformName, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(UseReceivedTimestamps, deviceConfig);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  deviceConfig->SetAttribute("MessageType", this->MessageType);
  deviceConfig->SetAttribute("ServerAddress", this->ServerAddress);
  deviceConfig->SetIntAttribute("ServerPort", this->ServerPort);
  deviceConfig->SetAttribute("ImageMessageEmbeddedTransformName", this->ImageMessageEmbeddedTransformName.GetTransformName().c_str());
  deviceConfig->SetAttribute("UseReceivedTimestamps", this->UseReceivedTimestamps?"true":"false");
  deviceConfig->SetAttribute("ReconnectOnReceiveTimeout", this->ReconnectOnReceiveTimeout?"true":"false");
  deviceConfig->SetAttribute("IgtlMessageCrcCheckEnabled", this->IgtlMessageCrcCheckEnabled?"true":"false");
  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkVideoSource::NotifyConfigured()
{
  if( this->OutputChannels.size() > 1 )
  {
    LOG_WARNING("vtkOpenIGTLinkVideoSource is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
    return PLUS_FAIL;
  }

  if( this->OutputChannels.empty() )
  {
    LOG_ERROR("No output channels defined for vtkOpenIGTLinkVideoSource. Cannot proceed." );
    this->SetCorrectlyConfigured(false);
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkVideoSource::SetImageMessageEmbeddedTransformName(const char* nameString)
{
  return this->ImageMessageEmbeddedTransformName.SetTransformName(nameString);
}
