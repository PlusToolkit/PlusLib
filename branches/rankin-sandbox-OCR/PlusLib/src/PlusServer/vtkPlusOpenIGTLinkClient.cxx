/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "igtlCommandMessage.h"
#include "igtlMessageHeader.h"
#include "igtlOSUtil.h"
#include "igtlServerSocket.h"
#include "vtkMultiThreader.h"
#include "vtkPlusCommand.h"
#include "vtkPlusIgtlMessageCommon.h"
#include "vtkPlusOpenIGTLinkClient.h"
#include "vtkRecursiveCriticalSection.h"
#include "vtkXMLUtilities.h"

static const int CLIENT_SOCKET_TIMEOUT_MSEC = 500;

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
PlusStatus vtkPlusOpenIGTLinkClient::Connect(double timeoutSec/*=-1*/)
{
  const double retryDelaySec = 1.0;
  int errorCode = 1;
  double startTimeSec = vtkAccurateTimer::GetSystemTime();
  while (errorCode!=0)
  {
    errorCode = this->ClientSocket->ConnectToServer( this->ServerHost, this->ServerPort );
    if (vtkAccurateTimer::GetSystemTime()-startTimeSec > timeoutSec)
    {
      // time is up
      break;
    }
    vtkAccurateTimer::DelayWithEventProcessing(retryDelaySec);
  }

  if ( errorCode != 0 )
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
  vtkSmartPointer<vtkXMLDataElement> cmdConfig = vtkSmartPointer<vtkXMLDataElement>::New();
  command->WriteConfiguration(cmdConfig);
  std::ostringstream xmlStr;
  vtkXMLUtilities::FlattenElement(cmdConfig, xmlStr);
  xmlStr << std::ends;

  // TODO : determine a way of configurable client name
  std::stringstream deviceNameSs;
  deviceNameSs << "PlusClient_" << PLUSLIB_VERSION;

  igtl::CommandMessage::Pointer commandMessage = igtl::CommandMessage::New();
  commandMessage->SetDeviceName( deviceNameSs.str().c_str() );
  commandMessage->SetCommandId( command->GetId() );
  commandMessage->SetCommandName( command->GetName() );
  commandMessage->SetCommandContent( xmlStr.str().c_str() );
  commandMessage->Pack();

  // Send the string message to the server.

  // int alive = this->ClientSocket->GetConnected();
  LOG_DEBUG( "Sending message: " << xmlStr.str() );
  int success = 0;
  {
    PlusLockGuard<vtkRecursiveCriticalSection> socketGuard(this->SocketMutex);
    success = this->ClientSocket->Send( commandMessage->GetPackPointer(), commandMessage->GetPackSize() );
  }
  if ( !success )
  {
    LOG_ERROR( "OpenIGTLink client couldn't send command to server." );
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkClient::ReceiveReply(bool& result, uint32_t& outOriginalCommandId, uint8_t outErrorString[IGTL_COMMAND_NAME_SIZE], std::string& outContentXML, double timeoutSec/*=0*/)
{
  double startTimeSec=vtkAccurateTimer::GetSystemTime();
  while (1)
  {
    {
      // save command reply
      PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);
      if (!this->Replies.empty())
      {
        igtl::RTSCommandMessage::Pointer rtsCommandMsg = this->Replies.front();
        vtkSmartPointer<vtkXMLDataElement> cmdElement = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromString(rtsCommandMsg->GetCommandContent().c_str()));
        if(cmdElement == NULL )
        {
          LOG_ERROR("Unable to parse command reply as XML. Skipping.");
          continue;
        }

        XML_FIND_NESTED_ELEMENT_OPTIONAL(resultElement, cmdElement, "Result");
        if( resultElement != NULL)
        {
          result = STRCASECMP(resultElement->GetCharacterData(), "true") == 0 ? true : false;
        }

        strcpy((char*)outErrorString, rtsCommandMsg->GetCommandErrorString().c_str());
        outOriginalCommandId = rtsCommandMsg->GetCommandId();
        outContentXML = rtsCommandMsg->GetCommandContent();
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

    if (self->OnMessageReceived(headerMsg.GetPointer()))
    {
      // The message body is read and processed
      continue;
    }

    igtl::MessageBase::Pointer bodyMsg = self->IgtlMessageFactory->CreateReceiveMessage(headerMsg);
    if( bodyMsg.IsNull() )
    {
      LOG_ERROR("Unable to create message of type: " << headerMsg->GetMessageType());
      continue;
    }

    if ( typeid(*bodyMsg) == typeid(igtl::RTSCommandMessage) )
    {
      igtl::RTSCommandMessage::Pointer rtsCommandMsg = dynamic_cast<igtl::RTSCommandMessage*>(bodyMsg.GetPointer());
      rtsCommandMsg->SetMessageHeader(headerMsg); 
      rtsCommandMsg->AllocatePack(); 
      {
        PlusLockGuard<vtkRecursiveCriticalSection> socketGuard(self->SocketMutex);
        self->ClientSocket->Receive(rtsCommandMsg->GetPackBodyPointer(), rtsCommandMsg->GetPackBodySize() ); 
      }

      int c = rtsCommandMsg->Unpack(1);
      if ( !(c & igtl::MessageHeader::UNPACK_BODY)) 
      {
        LOG_ERROR("Failed to receive reply (invalid body)");
        continue;
      }
      {
        // save command reply
        PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(self->Mutex);
        self->Replies.push_back(rtsCommandMsg);
        LOG_DEBUG("Reply received for command " << rtsCommandMsg->GetCommandId() << " with content: " << rtsCommandMsg->GetCommandContent() );
      }      
    }
    else
    {
      // if the device type is unknown, skip reading. 
      LOG_TRACE("Received message: " << headerMsg->GetMessageType() << " (not processed)");
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

//----------------------------------------------------------------------------
int vtkPlusOpenIGTLinkClient::SocketReceive(void* data, int length)
{
  PlusLockGuard<vtkRecursiveCriticalSection> socketGuard(this->SocketMutex);
  return ClientSocket->Receive(data, length);
}
