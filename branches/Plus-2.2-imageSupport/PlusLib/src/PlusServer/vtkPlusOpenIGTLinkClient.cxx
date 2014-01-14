/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "vtkPlusOpenIGTLinkClient.h"

#include "vtkMultiThreader.h"
#include "vtkXMLUtilities.h"
#include "vtkRecursiveCriticalSection.h"

#include "igtlMessageHeader.h"
#include "igtlOSUtil.h"
#include "igtlServerSocket.h"

#include "igtlStringMessage.h"
#include "vtkPlusCommand.h"

static const int CLIENT_SOCKET_TIMEOUT_MSEC = 500; 

vtkCxxRevisionMacro( vtkPlusOpenIGTLinkClient, "$Revision: 1.0 $" );
vtkStandardNewMacro( vtkPlusOpenIGTLinkClient ); 

//----------------------------------------------------------------------------
/*! Protected constructor. */
vtkPlusOpenIGTLinkClient::vtkPlusOpenIGTLinkClient()
: DataReceiverThreadId(-1)
, DataReceiverActive(std::make_pair(false,false))
, Mutex(vtkSmartPointer<vtkRecursiveCriticalSection>::New())
, SocketMutex(vtkSmartPointer<vtkRecursiveCriticalSection>::New())
, ClientSocket(igtl::ClientSocket::New())
, Threader(vtkSmartPointer<vtkMultiThreader>::New())
, ServerHost(NULL)
, ServerPort(-1)
{
}

//----------------------------------------------------------------------------
vtkPlusOpenIGTLinkClient::~vtkPlusOpenIGTLinkClient()
{
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkClient::Connect()
{
  int r = this->ClientSocket->ConnectToServer( this->ServerHost, this->ServerPort );

  if ( r != 0 )
  {
    LOG_ERROR( "Cannot connect to the server." );
    return PLUS_FAIL;
  }
  LOG_TRACE( "Client successfully connected to server." );
  
  this->ClientSocket->SetTimeout( CLIENT_SOCKET_TIMEOUT_MSEC ); 

  if ( this->DataReceiverThreadId < 0 )
  {
    this->DataReceiverActive.first = true; 
    this->DataReceiverThreadId = this->Threader->SpawnThread( (vtkThreadFunctionType)&DataReceiverThread, this );
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkClient::Disconnect()
{
  {
    PlusLockGuard<vtkRecursiveCriticalSection> socketGuard(this->SocketMutex);
    this->ClientSocket->CloseSocket();
  }

  // Stop data receiver thread 
  if ( this->DataReceiverThreadId >=0 )
  {
    this->DataReceiverActive.first = false; 
    while ( this->DataReceiverActive.second )
    {
      // Wait until the thread stops 
      vtkAccurateTimer::Delay( 0.2 ); 
    }
    this->DataReceiverThreadId = -1; 
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkClient::SendCommand( vtkPlusCommand* command )
{  
  // Convert the command to a string message.

  // Get the XML string
  vtkSmartPointer<vtkXMLDataElement> cmdConfig=vtkSmartPointer<vtkXMLDataElement>::New();
  command->WriteConfiguration(cmdConfig);
  std::ostringstream xmlStr;
  vtkXMLUtilities::FlattenElement(cmdConfig, xmlStr);
  xmlStr << std::ends;

  // Get the device name, generate unique command identifier from timestamp (CMD_2342342)
  std::string commandUid;
  if (command->GetId())
  {
    commandUid=command->GetId();
  }
  else
  {
    // command UID is not specified, generate one automatically from the timestamp
    std::ostringstream commandUidStr;
    commandUidStr << std::fixed << vtkAccurateTimer::GetUniversalTime() << std::ends;
    commandUid=commandUidStr.str();
  }
  std::string deviceNameString=vtkPlusCommand::GenerateCommandDeviceName(commandUid);

  igtl::StringMessage::Pointer stringMessage = igtl::StringMessage::New();
  stringMessage->SetDeviceName( deviceNameString.c_str() );
  std::string xmlString=xmlStr.str();
  stringMessage->SetString( xmlString.c_str() );
  stringMessage->Pack();

  // Send the string message to the server.

  // int alive = this->ClientSocket->GetConnected();
  LOG_DEBUG( "Sending message: " << xmlStr.str() );
  int success = 0;
  {
    PlusLockGuard<vtkRecursiveCriticalSection> socketGuard(this->SocketMutex);
    success = this->ClientSocket->Send( stringMessage->GetPackPointer(), stringMessage->GetPackSize() );
  }
  if ( !success )
  {
    LOG_ERROR( "OpenIGTLink client couldn't send command to server." );
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkClient::ReceiveReply(std::string &replyStr, double timeoutSec/*=0*/)
{  
  replyStr.clear();

  double startTimeSec=vtkAccurateTimer::GetSystemTime();
  while (1)
  {
    {
      // save command reply
      PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);
      if (!this->Replies.empty())
      {
        replyStr=this->Replies.front();
        this->Replies.pop_front();
        return PLUS_SUCCESS;
      }
    }      
    if (vtkAccurateTimer::GetSystemTime()-startTimeSec>timeoutSec)
    {
      LOG_DEBUG("vtkPlusOpenIGTLinkClient::ReceiveReply timeout passed ("<<timeoutSec<<"sec)");
      return PLUS_FAIL;
    }
    vtkAccurateTimer::Delay(0.010);    
  }
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
void vtkPlusOpenIGTLinkClient::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
void* vtkPlusOpenIGTLinkClient::DataReceiverThread( vtkMultiThreader::ThreadInfo* data )
{
  vtkPlusOpenIGTLinkClient* self = (vtkPlusOpenIGTLinkClient*)( data->UserData );
  self->DataReceiverActive.second = true; 

  while ( self->DataReceiverActive.first )
  {
    igtl::MessageHeader::Pointer headerMsg;
    headerMsg = igtl::MessageHeader::New();
    headerMsg->InitPack();

    // Receive generic header from the socket
    int numOfBytesReceived = 0;
    {
      PlusLockGuard<vtkRecursiveCriticalSection> socketGuard(self->SocketMutex);
      numOfBytesReceived = self->ClientSocket->Receive( headerMsg->GetPackPointer(), headerMsg->GetPackSize() );
    }
    if ( numOfBytesReceived == 0  // No message received
      || numOfBytesReceived != headerMsg->GetPackSize() // Received data is not as we expected
      )
    {
      // Failed to receive data, maybe the socket is disconnected
      vtkAccurateTimer::Delay(0.1);      
      continue; 
    }

    int c = headerMsg->Unpack(1);
    if ( !(c & igtl::MessageHeader::UNPACK_HEADER)) 
    {
      LOG_ERROR("Failed to receive reply (invalid header)");      
      continue;
    }

    if (strcmp(headerMsg->GetDeviceType(), "STRING") == 0
      && vtkPlusCommand::IsReplyDeviceName(headerMsg->GetDeviceName(),""))
    {

      igtl::StringMessage::Pointer replyMsg = igtl::StringMessage::New(); 
      replyMsg->SetMessageHeader(headerMsg); 
      replyMsg->AllocatePack(); 
      {
        PlusLockGuard<vtkRecursiveCriticalSection> socketGuard(self->SocketMutex);
        self->ClientSocket->Receive(replyMsg->GetPackBodyPointer(), replyMsg->GetPackBodySize() ); 
      }

      int c = replyMsg->Unpack(1);
      if ( !(c & igtl::MessageHeader::UNPACK_BODY)) 
      {
        LOG_ERROR("Failed to receive reply (invalid body)");
        continue;
      }
      {
        // save command reply
        PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(self->Mutex);
        self->Replies.push_back(replyMsg->GetString());
        //LOG_INFO("Reply received: "<<replyMsg->GetStatusString());
      }      
    }
    else
    {
      // if the device type is unknown, skip reading. 
      LOG_TRACE("Received message: "<<headerMsg->GetDeviceType());      
      {
        PlusLockGuard<vtkRecursiveCriticalSection> socketGuard(self->SocketMutex);
        self->ClientSocket->Skip(headerMsg->GetBodySizeToRead(), 0);
      }
    }
  } // ConnectionActive

  // Close thread
  self->DataReceiverThreadId = -1;
  self->DataReceiverActive.second = false; 
  return NULL;
}
