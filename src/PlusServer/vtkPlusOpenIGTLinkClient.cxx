/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "igtlCommandMessage.h"
#include "igtlCommon.h"
#include "igtlMessageHeader.h"
#include "igtlOSUtil.h"
#include "igtlServerSocket.h"
#include "vtkMultiThreader.h"
#include "vtkPlusCommand.h"
#include "vtkPlusIgtlMessageCommon.h"
#include "vtkPlusOpenIGTLinkClient.h"
#include "vtkIGSIORecursiveCriticalSection.h"
#include "vtkXMLUtilities.h"

const float vtkPlusOpenIGTLinkClient::CLIENT_SOCKET_TIMEOUT_SEC = 0.5;

vtkStandardNewMacro(vtkPlusOpenIGTLinkClient);

//----------------------------------------------------------------------------
/*! Protected constructor. */
vtkPlusOpenIGTLinkClient::vtkPlusOpenIGTLinkClient()
  : IgtlMessageFactory(vtkSmartPointer<vtkPlusIgtlMessageFactory>::New())
  , DataReceiverActive(std::make_pair(false, false))
  , DataReceiverThreadId(-1)
  , Threader(vtkSmartPointer<vtkMultiThreader>::New())
  , Mutex(vtkSmartPointer<vtkIGSIORecursiveCriticalSection>::New())
  , SocketMutex(vtkSmartPointer<vtkIGSIORecursiveCriticalSection>::New())
  , ClientSocket(igtl::ClientSocket::New())
  , LastGeneratedCommandId(0)
  , ServerPort(-1)
  , ServerHost("")
  , ServerIGTLVersion(IGTL_HEADER_VERSION_1)
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
  double startTimeSec = vtkIGSIOAccurateTimer::GetSystemTime();
  while (errorCode != 0)
  {
    errorCode = this->ClientSocket->ConnectToServer(this->ServerHost.c_str(), this->ServerPort);
    if (vtkIGSIOAccurateTimer::GetSystemTime() - startTimeSec > timeoutSec)
    {
      // time is up
      break;
    }
    vtkIGSIOAccurateTimer::DelayWithEventProcessing(retryDelaySec);
  }

  if (errorCode != 0)
  {
    LOG_ERROR("Cannot connect to the server.");
    return PLUS_FAIL;
  }
  LOG_TRACE("Client successfully connected to server.");

  this->ClientSocket->SetTimeout(CLIENT_SOCKET_TIMEOUT_SEC * 1000);

  if (this->DataReceiverThreadId < 0)
  {
    this->DataReceiverActive.first = true;
    this->DataReceiverThreadId = this->Threader->SpawnThread((vtkThreadFunctionType)&DataReceiverThread, this);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkClient::Disconnect()
{
  {
    igsioLockGuard<vtkIGSIORecursiveCriticalSection> socketGuard(this->SocketMutex);
    this->ClientSocket->CloseSocket();
  }

  // Stop data receiver thread
  if (this->DataReceiverThreadId >= 0)
  {
    this->DataReceiverActive.first = false;
    while (this->DataReceiverActive.second)
    {
      // Wait until the thread stops
      vtkIGSIOAccurateTimer::Delay(0.2);
    }
    this->DataReceiverThreadId = -1;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkClient::SendCommand(vtkPlusCommand* command)
{
  // Get the XML string
  vtkSmartPointer<vtkXMLDataElement> cmdConfig = vtkSmartPointer<vtkXMLDataElement>::New();
  command->WriteConfiguration(cmdConfig);
  std::ostringstream xmlStr;
  vtkXMLUtilities::FlattenElement(cmdConfig, xmlStr);
  xmlStr << std::ends;

  std::ostringstream commandUidStringStream;

  // Ensure commandUid is populated
  igtlUint32 commandUid;
  if (command->GetId())
  {
    commandUid = command->GetId();
  }
  else
  {
    if (igtl::IGTLProtocolToHeaderLookup(this->GetServerIGTLVersion()) < IGTL_HEADER_VERSION_2)
    {
      // command UID is not specified, generate one automatically from the timestamp
      commandUid = vtkIGSIOAccurateTimer::GetUniversalTime();
    }
    else
    {
      // command UID is not specified, generate one automatically
      commandUid = LastGeneratedCommandId;
      LastGeneratedCommandId++;
    }
  }

  // Generate the device name
  std::ostringstream deviceNameSs;
  if (igtl::IGTLProtocolToHeaderLookup(this->GetServerIGTLVersion()) >= IGTL_HEADER_VERSION_2)
  {
    // TODO : determine a way of configurable client name
    deviceNameSs << "PlusClient_" << PLUSLIB_VERSION;
  }
  else
  {
    std::string deviceName;
    commandUidStringStream << commandUid;
    vtkPlusCommand::GenerateCommandDeviceName(commandUidStringStream.str(), deviceName);
    deviceNameSs << deviceName;
  }

  igtl::MessageBase::Pointer message;
  if (igtl::IGTLProtocolToHeaderLookup(this->GetServerIGTLVersion()) < IGTL_HEADER_VERSION_2)
  {
    igtl::StringMessage::Pointer strMsg = dynamic_cast<igtl::StringMessage*>(this->IgtlMessageFactory->CreateSendMessage("STRING", igtl::IGTLProtocolToHeaderLookup(this->GetServerIGTLVersion())).GetPointer());
    strMsg->SetDeviceName(deviceNameSs.str().c_str());
    std::string xmlString = xmlStr.str();
    strMsg->SetString(xmlString.c_str());
    strMsg->Pack();
    message = strMsg;
  }
  else
  {
    igtl::CommandMessage::Pointer cmdMsg = dynamic_cast<igtl::CommandMessage*>(this->IgtlMessageFactory->CreateSendMessage("COMMAND", igtl::IGTLProtocolToHeaderLookup(this->GetServerIGTLVersion())).GetPointer());
    cmdMsg->SetDeviceName(deviceNameSs.str().c_str());
    cmdMsg->SetCommandId(commandUid);
    cmdMsg->SetCommandName(command->GetName());
    cmdMsg->SetCommandContent(xmlStr.str().c_str());
    cmdMsg->Pack();
    message = cmdMsg;
  }

  // Send the string message to the server.
  LOG_DEBUG("Sending message: " << xmlStr.str());
  int success = 0;
  {
    igsioLockGuard<vtkIGSIORecursiveCriticalSection> socketGuard(this->SocketMutex);
    success = this->ClientSocket->Send(message->GetBufferPointer(), message->GetBufferSize());
  }
  if (!success)
  {
    LOG_ERROR("OpenIGTLink client couldn't send command to server.");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkClient::SendMessage(igtl::MessageBase::Pointer packedMessage)
{
  int success = 0;
  {
    igsioLockGuard<vtkIGSIORecursiveCriticalSection> socketGuard(this->SocketMutex);
    success = this->ClientSocket->Send(packedMessage->GetBufferPointer(), packedMessage->GetBufferSize());
  }
  if (!success)
  {
    LOG_ERROR("OpenIGTLink client couldn't send message to server.");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkClient::ReceiveReply(PlusStatus& result, int32_t& outOriginalCommandId, std::string& outErrorString,
    std::string& outContent, igtl::MessageBase::MetaDataMap& outParameters,
    std::string& outCommandName, double timeoutSec/*=0*/)
{
  double startTimeSec = vtkIGSIOAccurateTimer::GetSystemTime();
  while (1)
  {
    {
      // save command reply
      igsioLockGuard<vtkIGSIORecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);
      if (!this->Replies.empty())
      {
        igtl::MessageBase::Pointer message = this->Replies.front();
        if (typeid(*message) == typeid(igtl::StringMessage))
        {
          // Process the command as v1/v2 string reply
          igtl::StringMessage::Pointer strMsg = dynamic_cast<igtl::StringMessage*>(message.GetPointer());

          if (vtkPlusCommand::IsReplyDeviceName(strMsg->GetDeviceName()))
          {
            if (igsioCommon::StringToInt<int32_t>(vtkPlusCommand::GetUidFromCommandDeviceName(strMsg->GetDeviceName()).c_str(), outOriginalCommandId) != PLUS_SUCCESS)
            {
              LOG_ERROR("Failed to get UID from command device name.");
              continue;
            }
          }
          vtkSmartPointer<vtkXMLDataElement> cmdElement = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromString(strMsg->GetString()));
          if (cmdElement == NULL)
          {
            LOG_ERROR("Unable to parse command reply as XML. Skipping.");
            continue;
          }
          if (cmdElement->GetAttribute("Status") == NULL)
          {
            LOG_ERROR("No status returned. Skipping.");
            continue;
          }
          result = std::string(cmdElement->GetAttribute("Status")) == "SUCCESS" ? PLUS_SUCCESS : PLUS_FAIL;
          if (cmdElement->GetAttribute("Message") == NULL)
          {
            LOG_ERROR("No message returned. Skipping.");
            continue;
          }
          outContent = cmdElement->GetAttribute("Message");
        }
        else if (typeid(*message) == typeid(igtl::RTSCommandMessage))
        {
          // Process the command as v3 RTS_Command
          igtl::RTSCommandMessage::Pointer rtsCommandMsg = dynamic_cast<igtl::RTSCommandMessage*>(message.GetPointer());

          vtkSmartPointer<vtkXMLDataElement> cmdElement = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromString(rtsCommandMsg->GetCommandContent().c_str()));

          outCommandName = rtsCommandMsg->GetCommandName();
          outOriginalCommandId = rtsCommandMsg->GetCommandId();

          XML_FIND_NESTED_ELEMENT_OPTIONAL(resultElement, cmdElement, "Result");
          if (resultElement != NULL)
          {
            result = STRCASECMP(resultElement->GetCharacterData(), "true") == 0 ? PLUS_SUCCESS : PLUS_FAIL;
          }
          XML_FIND_NESTED_ELEMENT_OPTIONAL(errorElement, cmdElement, "Error");
          if (!result && errorElement == NULL)
          {
            LOG_ERROR("Server sent error without reason. Notify server developers.");
          }
          else if (!result && errorElement != NULL)
          {
            outErrorString = errorElement->GetCharacterData();
          }
          XML_FIND_NESTED_ELEMENT_REQUIRED(messageElement, cmdElement, "Message");
          outContent = messageElement->GetCharacterData();

          outParameters = rtsCommandMsg->GetMetaData();
        }
        else if (typeid(*message) == typeid(igtl::RTSTrackingDataMessage))
        {
          igtl::RTSTrackingDataMessage* rtsTrackingMsg = dynamic_cast<igtl::RTSTrackingDataMessage*>(message.GetPointer());

          result = rtsTrackingMsg->GetStatus() == 0 ? PLUS_SUCCESS : PLUS_FAIL;
          outContent = (rtsTrackingMsg->GetStatus() == 0 ? "SUCCESS" : "FAILURE");
          outCommandName = "RTSTrackingDataMessage";
          outOriginalCommandId = -1;
        }

        this->Replies.pop_front();
        return PLUS_SUCCESS;
      }
    }
    if (vtkIGSIOAccurateTimer::GetSystemTime() - startTimeSec > timeoutSec)
    {
      LOG_DEBUG("vtkPlusOpenIGTLinkClient::ReceiveReply timeout passed (" << timeoutSec << "sec)");
      return PLUS_FAIL;
    }
    vtkIGSIOAccurateTimer::Delay(0.010);
  }
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
void vtkPlusOpenIGTLinkClient::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void* vtkPlusOpenIGTLinkClient::DataReceiverThread(vtkMultiThreader::ThreadInfo* data)
{
  vtkPlusOpenIGTLinkClient* self = (vtkPlusOpenIGTLinkClient*)(data->UserData);
  self->DataReceiverActive.second = true;

  while (self->DataReceiverActive.first)
  {

    igtl::MessageHeader::Pointer headerMsg = self->IgtlMessageFactory->CreateHeaderMessage(IGTL_HEADER_VERSION_1);

    // Receive generic header from the socket
    int numOfBytesReceived = 0;
    {
      igsioLockGuard<vtkIGSIORecursiveCriticalSection> socketGuard(self->SocketMutex);
      numOfBytesReceived = self->ClientSocket->Receive(headerMsg->GetBufferPointer(), headerMsg->GetBufferSize());
    }
    if (numOfBytesReceived == 0   // No message received
        || numOfBytesReceived != headerMsg->GetPackSize() // Received data is not as we expected
       )
    {
      // Failed to receive data, maybe the socket is disconnected
      vtkIGSIOAccurateTimer::Delay(0.1);
      continue;
    }

    int c = headerMsg->Unpack(1);
    if (!(c & igtl::MessageHeader::UNPACK_HEADER))
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
    if (bodyMsg.IsNull())
    {
      LOG_ERROR("Unable to create message of type: " << headerMsg->GetMessageType());
      continue;
    }

    // Only accept string messages if they have a deviceName of the format ACK_xyz
    if ((typeid(*bodyMsg) == typeid(igtl::StringMessage)
         && vtkPlusCommand::IsReplyDeviceName(headerMsg->GetDeviceName(), ""))
        || typeid(*bodyMsg) == typeid(igtl::RTSCommandMessage))
    {
      bodyMsg->SetMessageHeader(headerMsg);
      bodyMsg->AllocateBuffer();
      {
        igsioLockGuard<vtkIGSIORecursiveCriticalSection> socketGuard(self->SocketMutex);
        self->ClientSocket->Receive(bodyMsg->GetBufferBodyPointer(), bodyMsg->GetBufferBodySize());
      }

      int c = bodyMsg->Unpack(1);
      if (!(c & igtl::MessageHeader::UNPACK_BODY))
      {
        LOG_ERROR("Failed to receive reply (invalid body)");
        continue;
      }
      {
        // save command reply
        igsioLockGuard<vtkIGSIORecursiveCriticalSection> updateMutexGuardedLock(self->Mutex);
        self->Replies.push_back(bodyMsg);
      }
    }
    else if (typeid(*bodyMsg) == typeid(igtl::RTSTrackingDataMessage))
    {
      bodyMsg->SetMessageHeader(headerMsg);
      bodyMsg->AllocateBuffer();
      {
        igsioLockGuard<vtkIGSIORecursiveCriticalSection> socketGuard(self->SocketMutex);
        self->ClientSocket->Receive(bodyMsg->GetBufferBodyPointer(), bodyMsg->GetBufferBodySize());
      }

      int c = bodyMsg->Unpack(1);
      if (!(c & igtl::MessageHeader::UNPACK_BODY))
      {
        LOG_ERROR("Failed to receive reply (invalid body)");
        continue;
      }
      {
        // save command reply
        igsioLockGuard<vtkIGSIORecursiveCriticalSection> updateMutexGuardedLock(self->Mutex);
        self->Replies.push_back(bodyMsg);
      }
    }
    else
    {
      // if the incoming message is not a reply to a command, we discard it and continue
      LOG_TRACE("Received message: " << headerMsg->GetMessageType() << " (not processed)");
      {
        igsioLockGuard<vtkIGSIORecursiveCriticalSection> socketGuard(self->SocketMutex);
        self->ClientSocket->Skip(headerMsg->GetBodySizeToRead(), 0);
      }
    }
  }

  // Close thread
  self->DataReceiverThreadId = -1;
  self->DataReceiverActive.second = false;
  return NULL;
}

//----------------------------------------------------------------------------
int vtkPlusOpenIGTLinkClient::SocketReceive(void* data, int length)
{
  igsioLockGuard<vtkIGSIORecursiveCriticalSection> socketGuard(this->SocketMutex);
  return ClientSocket->Receive(data, length);
}
