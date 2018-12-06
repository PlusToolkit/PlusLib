/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "PlusCommon.h"
#include "PlusConfigure.h"
#include "igsioTrackedFrame.h"
#include "vtkPlusChannel.h"
#include "vtkPlusCommand.h"
#include "vtkPlusCommandProcessor.h"
#include "vtkPlusDataCollector.h"
#include "vtkPlusIgtlMessageCommon.h"
#include "vtkPlusIgtlMessageFactory.h"
#include "vtkPlusOpenIGTLinkServer.h"
#include "vtkIGSIORecursiveCriticalSection.h"
#include "vtkIGSIOTrackedFrameList.h"
#include "vtkIGSIOTransformRepository.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkObjectFactory.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>

// OpenIGTLink includes
#include <igtlCommandMessage.h>
#include <igtlImageMessage.h>
#include <igtlImageMetaMessage.h>
#include <igtlMessageHeader.h>
#include <igtlPlusClientInfoMessage.h>
#include <igtlPointMessage.h>
#include <igtlPolyDataMessage.h>
#include <igtlStatusMessage.h>
#include <igtlStringMessage.h>
#include <igtlTrackingDataMessage.h>

// OpenIGTLinkIO includes
#include <igtlioPolyDataConverter.h>

#if defined(WIN32)
  #include "vtkPlusOpenIGTLinkServerWin32.cxx"
#elif defined(__APPLE__)
  #include "vtkPlusOpenIGTLinkServerMacOSX.cxx"
#elif defined(__linux__)
  #include "vtkPlusOpenIGTLinkServerLinux.cxx"
#endif

// STL includes
#include <fstream>
#include <streambuf>

namespace
{
  const double DELAY_ON_SENDING_ERROR_SEC = 0.02;
  const double DELAY_ON_NO_NEW_FRAMES_SEC = 0.005;
  const int NUMBER_OF_RECENT_COMMAND_IDS_STORED = 10;
  const int IGTL_EMPTY_DATA_SIZE = -1;
  const double SERVER_START_CHECK_DELAY_SEC = 2.0;
  const double SERVER_START_CHECK_DELAY_INTERVAL_SEC = 0.05;

  //----------------------------------------------------------------------------
  // If a frame cannot be retrieved from the device buffers (because it was overwritten by new frames)
  // then we skip a SAMPLING_SKIPPING_MARGIN_SEC long period to allow the application to catch up.
  // This time should be long enough to comfortably retrieve a frame from the buffer.
  const double SAMPLING_SKIPPING_MARGIN_SEC = 0.1;
}

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusOpenIGTLinkServer);
int vtkPlusOpenIGTLinkServer::ClientIdCounter = 1;
const float vtkPlusOpenIGTLinkServer::CLIENT_SOCKET_TIMEOUT_SEC = 0.5f;

//----------------------------------------------------------------------------
vtkPlusOpenIGTLinkServer::vtkPlusOpenIGTLinkServer()
  : ServerSocket(igtl::ServerSocket::New())
  , TransformRepository(NULL)
  , DataCollector(NULL)
  , Threader(vtkSmartPointer<vtkMultiThreader>::New())
  , IGTLProtocolVersion(OpenIGTLink_PROTOCOL_VERSION)
  , IGTLHeaderVersion(IGTL_HEADER_VERSION_2)
  , ListeningPort(-1)
  , NumberOfRetryAttempts(10)
  , DelayBetweenRetryAttemptsSec(0.05)
  , MaxNumberOfIgtlMessagesToSend(100)
  , ConnectionReceiverThreadId(-1)
  , DataSenderThreadId(-1)
  , IgtlMessageFactory(vtkSmartPointer<vtkPlusIgtlMessageFactory>::New())
  , IgtlClientsMutex(vtkSmartPointer<vtkIGSIORecursiveCriticalSection>::New())
  , LastSentTrackedFrameTimestamp(0)
  , MaxTimeSpentWithProcessingMs(50)
  , LastProcessingTimePerFrameMs(-1)
  , SendValidTransformsOnly(true)
  , DefaultClientSendTimeoutSec(CLIENT_SOCKET_TIMEOUT_SEC)
  , DefaultClientReceiveTimeoutSec(CLIENT_SOCKET_TIMEOUT_SEC)
  , IgtlMessageCrcCheckEnabled(0)
  , PlusCommandProcessor(vtkSmartPointer<vtkPlusCommandProcessor>::New())
  , MessageResponseQueueMutex(vtkSmartPointer<vtkIGSIORecursiveCriticalSection>::New())
  , BroadcastChannel(NULL)
  , LogWarningOnNoDataAvailable(true)
  , KeepAliveIntervalSec(CLIENT_SOCKET_TIMEOUT_SEC / 2.0)
  , GracePeriodLogLevel(vtkPlusLogger::LOG_LEVEL_DEBUG)
  , MissingInputGracePeriodSec(0.0)
  , BroadcastStartTime(0.0)
{

}

//----------------------------------------------------------------------------
vtkPlusOpenIGTLinkServer::~vtkPlusOpenIGTLinkServer()
{
  this->Stop();
  this->SetTransformRepository(NULL);
  this->SetDataCollector(NULL);
  this->SetConfigFilename(NULL);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkServer::QueueMessageResponseForClient(int clientId, igtl::MessageBase::Pointer message)
{
  bool found(false);
  {
    igsioLockGuard<vtkIGSIORecursiveCriticalSection> igtlClientsMutexGuardedLock(this->IgtlClientsMutex);
    for (std::list<ClientData>::iterator clientIterator = this->IgtlClients.begin(); clientIterator != this->IgtlClients.end(); ++clientIterator)
    {
      if (clientIterator->ClientId == clientId)
      {
        found = true;
        break;
      }
    }
  }

  if (!found)
  {
    LOG_ERROR("Requested clientId " << clientId << " not found in list.");
    return PLUS_FAIL;
  }

  igsioLockGuard<vtkIGSIORecursiveCriticalSection> mutexGuardedLock(this->MessageResponseQueueMutex);
  this->MessageResponseQueue[clientId].push_back(message);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusOpenIGTLinkServer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkServer::StartOpenIGTLinkService()
{
  if (this->DataCollector == NULL)
  {
    LOG_WARNING("Tried to start OpenIGTLink server without a vtkPlusDataCollector");
    return PLUS_FAIL;
  }

  if (this->ConnectionReceiverThreadId < 0)
  {
    this->ConnectionActive.Request = true;
    this->ConnectionReceiverThreadId = this->Threader->SpawnThread((vtkThreadFunctionType)&ConnectionReceiverThread, this);
  }

  if (this->DataSenderThreadId < 0)
  {
    this->DataSenderActive.Request = true;
    this->DataSenderThreadId = this->Threader->SpawnThread((vtkThreadFunctionType)&DataSenderThread, this);
  }

  // Wait a short duration to see if both threads initialized properly, check at 50ms interval
  RETRY_UNTIL_TRUE(this->ConnectionActive.Respond,
                   vtkMath::Round(SERVER_START_CHECK_DELAY_SEC / SERVER_START_CHECK_DELAY_INTERVAL_SEC),
                   SERVER_START_CHECK_DELAY_INTERVAL_SEC);
  if (!this->ConnectionActive.Respond)
  {
    LOG_ERROR("Unable to initialize receiver and sender processes.");
    return PLUS_FAIL;
  }

  std::ostringstream ss;
  ss << "Data sent by default: ";
  this->DefaultClientInfo.PrintSelf(ss, vtkIndent(0));
  LOG_DEBUG(ss.str());

  this->PlusCommandProcessor->SetPlusServer(this);

  this->BroadcastStartTime = vtkIGSIOAccurateTimer::GetSystemTime();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkServer::StopOpenIGTLinkService()
{
  // Stop connection receiver thread
  if (this->ConnectionReceiverThreadId >= 0)
  {
    this->ConnectionActive.Request = false;
    while (this->ConnectionActive.Respond)
    {
      // Wait until the thread stops
      vtkIGSIOAccurateTimer::DelayWithEventProcessing(0.2);
    }
    this->ConnectionReceiverThreadId = -1;
    LOG_DEBUG("ConnectionReceiverThread stopped");
  }

  // Disconnect clients (stop receiving thread, close socket)
  std::vector< int > clientIds;
  {
    // Get all the client ids and release the lock
    igsioLockGuard<vtkIGSIORecursiveCriticalSection> igtlClientsMutexGuardedLock(this->IgtlClientsMutex);
    for (std::list<ClientData>::iterator clientIterator = this->IgtlClients.begin(); clientIterator != this->IgtlClients.end(); ++clientIterator)
    {
      clientIds.push_back(clientIterator->ClientId);
    }
  }
  for (std::vector< int >::iterator it = clientIds.begin(); it != clientIds.end(); ++it)
  {
    DisconnectClient(*it);
  }

  LOG_INFO("Plus OpenIGTLink server stopped.");

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void* vtkPlusOpenIGTLinkServer::ConnectionReceiverThread(vtkMultiThreader::ThreadInfo* data)
{
  vtkPlusOpenIGTLinkServer* self = (vtkPlusOpenIGTLinkServer*)(data->UserData);

  int r = self->ServerSocket->CreateServer(self->ListeningPort);
  if (r < 0)
  {
    LOG_ERROR("Cannot create a server socket.");
    return NULL;
  }

  PrintServerInfo(self);

  self->ConnectionActive.Respond = true;

  // Wait for connections until we want to stop the thread
  while (self->ConnectionActive.Request)
  {
    igtl::ClientSocket::Pointer newClientSocket = self->ServerSocket->WaitForConnection(CLIENT_SOCKET_TIMEOUT_SEC * 1000);
    if (newClientSocket.IsNotNull())
    {
      // Lock before we change the clients list
      igsioLockGuard<vtkIGSIORecursiveCriticalSection> igtlClientsMutexGuardedLock(self->IgtlClientsMutex);
      ClientData newClient;
      self->IgtlClients.push_back(newClient);

      ClientData* client = &(self->IgtlClients.back());   // get a reference to the client data that is stored in the list
      client->ClientId = self->ClientIdCounter;
      self->ClientIdCounter++;
      client->ClientSocket = newClientSocket;
      client->ClientSocket->SetReceiveTimeout(self->DefaultClientReceiveTimeoutSec * 1000);
      client->ClientSocket->SetSendTimeout(self->DefaultClientSendTimeoutSec * 1000);
      client->ClientInfo = self->DefaultClientInfo;
      client->Server = self;

      int port = 0;
      std::string address = "unknown";
#if (OPENIGTLINK_VERSION_MAJOR > 1) || ( OPENIGTLINK_VERSION_MAJOR == 1 && OPENIGTLINK_VERSION_MINOR > 9 ) || ( OPENIGTLINK_VERSION_MAJOR == 1 && OPENIGTLINK_VERSION_MINOR == 9 && OPENIGTLINK_VERSION_PATCH > 4 )
      newClientSocket->GetSocketAddressAndPort(address, port);
#endif
      LOG_INFO("Received new client connection (client " << client->ClientId << " at " << address << ":" << port << "). Number of connected clients: " << self->GetNumberOfConnectedClients());

      client->DataReceiverActive.first = true;
      client->DataReceiverThreadId = self->Threader->SpawnThread((vtkThreadFunctionType)&DataReceiverThread, client);
    }
  }

  // Close server socket
  if (self->ServerSocket.IsNotNull())
  {
    self->ServerSocket->CloseSocket();
  }

  // Close thread
  self->ConnectionReceiverThreadId = -1;
  self->ConnectionActive.Respond = false;
  return NULL;
}

//----------------------------------------------------------------------------
void* vtkPlusOpenIGTLinkServer::DataSenderThread(vtkMultiThreader::ThreadInfo* data)
{
  vtkPlusOpenIGTLinkServer* self = (vtkPlusOpenIGTLinkServer*)(data->UserData);
  self->DataSenderActive.Respond = true;

  vtkPlusDevice* aDevice(NULL);
  vtkPlusChannel* aChannel(NULL);

  DeviceCollection aCollection;
  if (self->DataCollector->GetDevices(aCollection) != PLUS_SUCCESS || aCollection.size() == 0)
  {
    LOG_ERROR("Unable to retrieve devices. Check configuration and connection.");
    return NULL;
  }

  // Find the requested channel ID in all the devices
  for (DeviceCollectionIterator it = aCollection.begin(); it != aCollection.end(); ++it)
  {
    aDevice = *it;
    if (aDevice->GetOutputChannelByName(aChannel, self->GetOutputChannelId()) == PLUS_SUCCESS)
    {
      break;
    }
  }

  if (aChannel == NULL)
  {
    // The requested channel ID is not found
    if (!self->GetOutputChannelId().empty())
    {
      // the user explicitly requested a specific channel, but none was found by that name
      // this is an error
      LOG_ERROR("Unable to start data sending. OutputChannelId not found: " << self->GetOutputChannelId());
      return NULL;
    }
    // the user did not specify any channel, so just use the first channel that can be found in any device
    for (DeviceCollectionIterator it = aCollection.begin(); it != aCollection.end(); ++it)
    {
      aDevice = *it;
      if (aDevice->OutputChannelCount() > 0)
      {
        aChannel = *(aDevice->GetOutputChannelsStart());
        break;
      }
    }
  }

  // If we didn't find any channel then return
  if (aChannel == NULL)
  {
    LOG_WARNING("There are no channels to broadcast. Only command processing is available.");
  }

  self->BroadcastChannel = aChannel;
  if (self->BroadcastChannel)
  {
    self->BroadcastChannel->GetMostRecentTimestamp(self->LastSentTrackedFrameTimestamp);
  }

  double elapsedTimeSinceLastPacketSentSec = 0;
  while (self->ConnectionActive.Request && self->DataSenderActive.Request)
  {
    bool clientsConnected = false;
    {
      igsioLockGuard<vtkIGSIORecursiveCriticalSection> igtlClientsMutexGuardedLock(self->IgtlClientsMutex);
      if (!self->IgtlClients.empty())
      {
        clientsConnected = true;
      }
    }
    if (!clientsConnected)
    {
      // No client connected, wait for a while
      vtkIGSIOAccurateTimer::Delay(0.2);
      self->LastSentTrackedFrameTimestamp = 0; // next time start sending from the most recent timestamp
      continue;
    }

    if (self->HasGracePeriodExpired())
    {
      self->GracePeriodLogLevel = vtkPlusLogger::LOG_LEVEL_WARNING;
    }

    SendMessageResponses(*self);

    // Send remote command execution replies to clients before sending any images/transforms/etc...
    SendCommandResponses(*self);

    // Send image/tracking/string data
    SendLatestFramesToClients(*self, elapsedTimeSinceLastPacketSentSec);
  }
  // Close thread
  self->DataSenderThreadId = -1;
  self->DataSenderActive.Respond = false;
  return NULL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkServer::SendLatestFramesToClients(vtkPlusOpenIGTLinkServer& self, double& elapsedTimeSinceLastPacketSentSec)
{
  vtkSmartPointer<vtkIGSIOTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkIGSIOTrackedFrameList>::New();
  double startTimeSec = vtkIGSIOAccurateTimer::GetSystemTime();

  // Acquire tracked frames since last acquisition (minimum 1 frame)
  if (self.LastProcessingTimePerFrameMs < 1)
  {
    // if processing was less than 1ms/frame then assume it was 1ms (1000FPS processing speed) to avoid division by zero
    self.LastProcessingTimePerFrameMs = 1;
  }
  int numberOfFramesToGet = std::max(self.MaxTimeSpentWithProcessingMs / self.LastProcessingTimePerFrameMs, 1);
  // Maximize the number of frames to send
  numberOfFramesToGet = std::min(numberOfFramesToGet, self.MaxNumberOfIgtlMessagesToSend);

  if (self.BroadcastChannel != NULL)
  {
    if ((self.BroadcastChannel->HasVideoSource() && !self.BroadcastChannel->GetVideoDataAvailable())
        || (self.BroadcastChannel->ToolCount() > 0 && !self.BroadcastChannel->GetTrackingDataAvailable())
        || (self.BroadcastChannel->FieldCount() > 0 && !self.BroadcastChannel->GetFieldDataAvailable()))
    {
      if (self.LogWarningOnNoDataAvailable)
      {
        LOG_DYNAMIC("No data is broadcasted, as no data is available yet.", self.GracePeriodLogLevel);
      }
    }
    else
    {
      double oldestDataTimestamp = 0;
      if (self.BroadcastChannel->GetOldestTimestamp(oldestDataTimestamp) == PLUS_SUCCESS)
      {
        if (self.LastSentTrackedFrameTimestamp < oldestDataTimestamp)
        {
          LOG_INFO("OpenIGTLink broadcasting started. No data was available between " << self.LastSentTrackedFrameTimestamp << "-" << oldestDataTimestamp << "sec, therefore no data were broadcasted during this time period.");
          self.LastSentTrackedFrameTimestamp = oldestDataTimestamp + SAMPLING_SKIPPING_MARGIN_SEC;
        }
        static vtkIGSIOLogHelper logHelper(60.0, 500000);
        CUSTOM_RETURN_WITH_FAIL_IF(self.BroadcastChannel->GetTrackedFrameList(self.LastSentTrackedFrameTimestamp, trackedFrameList, numberOfFramesToGet) != PLUS_SUCCESS,
                                   "Failed to get tracked frame list from data collector (last recorded timestamp: " << std::fixed << self.LastSentTrackedFrameTimestamp);
      }
    }
  }

  // There is no new frame in the buffer
  if (trackedFrameList->GetNumberOfTrackedFrames() == 0)
  {
    vtkIGSIOAccurateTimer::Delay(DELAY_ON_NO_NEW_FRAMES_SEC);
    elapsedTimeSinceLastPacketSentSec += vtkIGSIOAccurateTimer::GetSystemTime() - startTimeSec;

    // Send keep alive packet to clients
    if (elapsedTimeSinceLastPacketSentSec > self.KeepAliveIntervalSec)
    {
      self.KeepAlive();
      elapsedTimeSinceLastPacketSentSec = 0;
      return PLUS_SUCCESS;
    }

    return PLUS_FAIL;
  }

  for (unsigned int i = 0; i < trackedFrameList->GetNumberOfTrackedFrames(); ++i)
  {
    // Send tracked frame
    self.SendTrackedFrame(*trackedFrameList->GetTrackedFrame(i));
    elapsedTimeSinceLastPacketSentSec = 0;
  }

  // Compute time spent with processing one frame in this round
  double computationTimeMs = (vtkIGSIOAccurateTimer::GetSystemTime() - startTimeSec) * 1000.0;

  // Update last processing time if new tracked frames have been acquired
  if (trackedFrameList->GetNumberOfTrackedFrames() > 0)
  {
    self.LastProcessingTimePerFrameMs = computationTimeMs / trackedFrameList->GetNumberOfTrackedFrames();
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkServer::SendMessageResponses(vtkPlusOpenIGTLinkServer& self)
{
  igsioLockGuard<vtkIGSIORecursiveCriticalSection> mutexGuardedLock(self.MessageResponseQueueMutex);
  if (!self.MessageResponseQueue.empty())
  {
    for (ClientIdToMessageListMap::iterator it = self.MessageResponseQueue.begin(); it != self.MessageResponseQueue.end(); ++it)
    {
      igsioLockGuard<vtkIGSIORecursiveCriticalSection> igtlClientsMutexGuardedLock(self.IgtlClientsMutex);
      igtl::ClientSocket::Pointer clientSocket = NULL;

      for (std::list<ClientData>::iterator clientIterator = self.IgtlClients.begin(); clientIterator != self.IgtlClients.end(); ++clientIterator)
      {
        if (clientIterator->ClientId == it->first)
        {
          clientSocket = clientIterator->ClientSocket;
          break;
        }
      }
      if (clientSocket.IsNull())
      {
        LOG_WARNING("Message reply cannot be sent to client " << it->first << ", probably client has been disconnected.");
        continue;
      }

      for (std::vector<igtl::MessageBase::Pointer>::iterator messageIt = it->second.begin(); messageIt != it->second.end(); ++messageIt)
      {
        clientSocket->Send((*messageIt)->GetBufferPointer(), (*messageIt)->GetBufferSize());
      }
    }
    self.MessageResponseQueue.clear();
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkServer::SendCommandResponses(vtkPlusOpenIGTLinkServer& self)
{
  PlusCommandResponseList replies;
  self.PlusCommandProcessor->PopCommandResponses(replies);
  if (!replies.empty())
  {
    for (PlusCommandResponseList::iterator responseIt = replies.begin(); responseIt != replies.end(); responseIt++)
    {
      igtl::MessageBase::Pointer igtlResponseMessage = self.CreateIgtlMessageFromCommandResponse(*responseIt);
      if (igtlResponseMessage.IsNull())
      {
        LOG_ERROR("Failed to create OpenIGTLink message from command response");
        continue;
      }
      igtlResponseMessage->Pack();

      // Only send the response to the client that requested the command
      LOG_DEBUG("Send command reply to client " << (*responseIt)->GetClientId() << ": " << igtlResponseMessage->GetDeviceName());
      igsioLockGuard<vtkIGSIORecursiveCriticalSection> igtlClientsMutexGuardedLock(self.IgtlClientsMutex);
      igtl::ClientSocket::Pointer clientSocket = NULL;
      for (std::list<ClientData>::iterator clientIterator = self.IgtlClients.begin(); clientIterator != self.IgtlClients.end(); ++clientIterator)
      {
        if (clientIterator->ClientId == (*responseIt)->GetClientId())
        {
          clientSocket = clientIterator->ClientSocket;
          break;
        }
      }

      if (clientSocket.IsNull())
      {
        LOG_WARNING("Message reply cannot be sent to client " << (*responseIt)->GetClientId() << ", probably client has been disconnected");
        continue;
      }
      clientSocket->Send(igtlResponseMessage->GetBufferPointer(), igtlResponseMessage->GetBufferSize());
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void* vtkPlusOpenIGTLinkServer::DataReceiverThread(vtkMultiThreader::ThreadInfo* data)
{
  ClientData* client = (ClientData*)(data->UserData);
  client->DataReceiverActive.second = true;
  vtkPlusOpenIGTLinkServer* self = client->Server;

  /*! Store the IDs of recent commands to be able to detect duplicate command IDs */
  std::deque<uint32_t> previousCommandIds;

  // Make copy of frequently used data to avoid locking of client data
  igtl::ClientSocket::Pointer clientSocket = client->ClientSocket;
  int clientId = client->ClientId;

  igtl::MessageHeader::Pointer headerMsg = self->IgtlMessageFactory->CreateHeaderMessage(IGTL_HEADER_VERSION_1);

  while (client->DataReceiverActive.first)
  {
    headerMsg->InitBuffer();

    // Receive generic header from the socket
    int bytesReceived = clientSocket->Receive(headerMsg->GetBufferPointer(), headerMsg->GetBufferSize());
    if (bytesReceived == IGTL_EMPTY_DATA_SIZE || bytesReceived != headerMsg->GetBufferSize())
    {
      vtkIGSIOAccurateTimer::Delay(0.1);
      continue;
    }

    headerMsg->Unpack(self->IgtlMessageCrcCheckEnabled);

    {
      igsioLockGuard<vtkIGSIORecursiveCriticalSection> igtlClientsMutexGuardedLock(self->IgtlClientsMutex);
      // Keep track of the highest known version of message ever sent by this client, this is the version that we reply with
      // (upper bounded by the servers version)
      if (headerMsg->GetHeaderVersion() > client->ClientInfo.GetClientHeaderVersion())
      {
        client->ClientInfo.SetClientHeaderVersion(std::min<int>(self->GetIGTLHeaderVersion(), headerMsg->GetHeaderVersion()));
      }
    }

    igtl::MessageBase::Pointer bodyMessage = self->IgtlMessageFactory->CreateReceiveMessage(headerMsg);
    if (bodyMessage.IsNull())
    {
      LOG_ERROR("Unable to receive message from client: " << client->ClientId);
      continue;
    }

    if (typeid(*bodyMessage) == typeid(igtl::PlusClientInfoMessage))
    {
      igtl::PlusClientInfoMessage::Pointer clientInfoMsg = dynamic_cast<igtl::PlusClientInfoMessage*>(bodyMessage.GetPointer());
      clientInfoMsg->SetMessageHeader(headerMsg);
      clientInfoMsg->AllocateBuffer();

      clientSocket->Receive(clientInfoMsg->GetBufferBodyPointer(), clientInfoMsg->GetBufferBodySize());

      int c = clientInfoMsg->Unpack(self->IgtlMessageCrcCheckEnabled);
      if (c & igtl::MessageHeader::UNPACK_BODY)
      {
        // Message received from client, need to lock to modify client info
        igsioLockGuard<vtkIGSIORecursiveCriticalSection> igtlClientsMutexGuardedLock(self->IgtlClientsMutex);
        client->ClientInfo = clientInfoMsg->GetClientInfo();
        LOG_DEBUG("Client info message received from client " << clientId);
      }
    }
    else if (typeid(*bodyMessage) == typeid(igtl::GetStatusMessage))
    {
      // Just ping server, we can skip message and respond
      clientSocket->Skip(headerMsg->GetBodySizeToRead(), 0);

      igtl::StatusMessage::Pointer replyMsg = dynamic_cast<igtl::StatusMessage*>(self->IgtlMessageFactory->CreateSendMessage("STATUS", client->ClientInfo.GetClientHeaderVersion()).GetPointer());
      replyMsg->SetCode(igtl::StatusMessage::STATUS_OK);
      replyMsg->Pack();
      clientSocket->Send(replyMsg->GetBufferPointer(), replyMsg->GetBufferSize());
    }
    else if (typeid(*bodyMessage) == typeid(igtl::StringMessage)
             && vtkPlusCommand::IsCommandDeviceName(headerMsg->GetDeviceName()))
    {
      igtl::StringMessage::Pointer stringMsg = dynamic_cast<igtl::StringMessage*>(bodyMessage.GetPointer());
      stringMsg->SetMessageHeader(headerMsg);
      stringMsg->AllocateBuffer();
      clientSocket->Receive(stringMsg->GetBufferBodyPointer(), stringMsg->GetBufferBodySize());

      // We are receiving old style commands, handle it
      int c = stringMsg->Unpack(self->IgtlMessageCrcCheckEnabled);
      if (c & igtl::MessageHeader::UNPACK_BODY)
      {
        std::string deviceName(headerMsg->GetDeviceName());
        if (deviceName.empty())
        {
          self->PlusCommandProcessor->QueueStringResponse(PLUS_FAIL, std::string(vtkPlusCommand::DEVICE_NAME_REPLY), clientId, "Unable to read DeviceName.");
          continue;
        }

        uint32_t uid(0);
        try
        {
#if (_MSC_VER == 1500)
          std::istringstream ss(vtkPlusCommand::GetUidFromCommandDeviceName(deviceName));
          ss >> uid;
#else
          uid = std::stoi(vtkPlusCommand::GetUidFromCommandDeviceName(deviceName));
#endif
        }
        catch (std::invalid_argument e)
        {
          LOG_ERROR("Unable to extract command UID from device name string.");
          // Removing support for malformed command strings, reply with error
          self->PlusCommandProcessor->QueueStringResponse(PLUS_FAIL, std::string(vtkPlusCommand::DEVICE_NAME_REPLY), clientId, "Malformed DeviceName. Expected CMD_cmdId (ex: CMD_001)");
          continue;
        }

        deviceName = vtkPlusCommand::GetPrefixFromCommandDeviceName(deviceName);

        if (std::find(previousCommandIds.begin(), previousCommandIds.end(), uid) != previousCommandIds.end())
        {
          // Command already exists
          LOG_WARNING("Already received a command with id = " << uid << " from client " << clientId << ". This repeated command will be ignored.");
          continue;
        }
        // New command, remember its ID
        previousCommandIds.push_back(uid);
        if (previousCommandIds.size() > NUMBER_OF_RECENT_COMMAND_IDS_STORED)
        {
          previousCommandIds.pop_front();
        }

        LOG_DEBUG("Received command from client " << clientId << ", device " << deviceName << " with UID " << uid << ": " << stringMsg->GetString());

        vtkSmartPointer<vtkXMLDataElement> cmdElement = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromString(stringMsg->GetString()));
        std::string commandName = std::string(cmdElement->GetAttribute("Name") == NULL ? "" : cmdElement->GetAttribute("Name"));

        self->PlusCommandProcessor->QueueCommand(false, clientId, commandName, stringMsg->GetString(), deviceName, uid, stringMsg->GetMetaData());
      }

    }
    else if (typeid(*bodyMessage) == typeid(igtl::CommandMessage))
    {
      igtl::CommandMessage::Pointer commandMsg = dynamic_cast<igtl::CommandMessage*>(bodyMessage.GetPointer());
      commandMsg->SetMessageHeader(headerMsg);
      commandMsg->AllocateBuffer();
      clientSocket->Receive(commandMsg->GetBufferBodyPointer(), commandMsg->GetBufferBodySize());

      int c = commandMsg->Unpack(self->IgtlMessageCrcCheckEnabled);
      if (c & igtl::MessageHeader::UNPACK_BODY)
      {
        std::string deviceName(headerMsg->GetDeviceName());

        uint32_t uid;
        uid = commandMsg->GetCommandId();

        if (std::find(previousCommandIds.begin(), previousCommandIds.end(), uid) != previousCommandIds.end())
        {
          // Command already exists
          LOG_WARNING("Already received a command with id = " << uid << " from client " << clientId << ". This repeated command will be ignored.");
          continue;
        }
        // New command, remember its ID
        previousCommandIds.push_back(uid);
        if (previousCommandIds.size() > NUMBER_OF_RECENT_COMMAND_IDS_STORED)
        {
          previousCommandIds.pop_front();
        }

        LOG_DEBUG("Received header version " << commandMsg->GetHeaderVersion() << " command " << commandMsg->GetCommandName()
                  << " from client " << clientId << ", device " << deviceName << " with UID " << uid << ": " << commandMsg->GetCommandContent());

        self->PlusCommandProcessor->QueueCommand(true, clientId, commandMsg->GetCommandName(), commandMsg->GetCommandContent(), deviceName, uid, commandMsg->GetMetaData());
      }
      else
      {
        LOG_ERROR("STRING message unpacking failed for client " << clientId);
      }
    }
    else if (typeid(*bodyMessage) == typeid(igtl::StartTrackingDataMessage))
    {
      std::string deviceName("");

      igtl::StartTrackingDataMessage::Pointer startTracking = dynamic_cast<igtl::StartTrackingDataMessage*>(bodyMessage.GetPointer());
      startTracking->SetMessageHeader(headerMsg);
      startTracking->AllocateBuffer();

      clientSocket->Receive(startTracking->GetBufferBodyPointer(), startTracking->GetBufferBodySize());

      int c = startTracking->Unpack(self->IgtlMessageCrcCheckEnabled);
      if (c & igtl::MessageHeader::UNPACK_BODY)
      {
        client->ClientInfo.SetTDATAResolution(startTracking->GetResolution());
        client->ClientInfo.SetTDATARequested(true);
      }
      else
      {
        LOG_ERROR("Client " << clientId << " STT_TDATA failed: could not retrieve startTracking message");
        return NULL;
      }

      igtl::MessageBase::Pointer msg = self->IgtlMessageFactory->CreateSendMessage("RTS_TDATA", client->ClientInfo.GetClientHeaderVersion());
      igtl::RTSTrackingDataMessage* rtsMsg = dynamic_cast<igtl::RTSTrackingDataMessage*>(msg.GetPointer());
      rtsMsg->SetStatus(0);
      rtsMsg->Pack();
      self->QueueMessageResponseForClient(client->ClientId, msg);
    }
    else if (typeid(*bodyMessage) == typeid(igtl::StopTrackingDataMessage))
    {
      igtl::StopTrackingDataMessage::Pointer stopTracking = dynamic_cast<igtl::StopTrackingDataMessage*>(bodyMessage.GetPointer());
      stopTracking->SetMessageHeader(headerMsg);
      stopTracking->AllocateBuffer();

      clientSocket->Receive(stopTracking->GetBufferBodyPointer(), stopTracking->GetBufferBodySize());

      client->ClientInfo.SetTDATARequested(false);
      igtl::MessageBase::Pointer msg = self->IgtlMessageFactory->CreateSendMessage("RTS_TDATA", client->ClientInfo.GetClientHeaderVersion());
      igtl::RTSTrackingDataMessage* rtsMsg = dynamic_cast<igtl::RTSTrackingDataMessage*>(msg.GetPointer());
      rtsMsg->SetStatus(0);
      rtsMsg->Pack();
      self->QueueMessageResponseForClient(client->ClientId, msg);
    }
    else if (typeid(*bodyMessage) == typeid(igtl::GetPolyDataMessage))
    {
      igtl::GetPolyDataMessage::Pointer polyDataMessage = dynamic_cast<igtl::GetPolyDataMessage*>(bodyMessage.GetPointer());
      polyDataMessage->SetMessageHeader(headerMsg);
      polyDataMessage->AllocateBuffer();

      clientSocket->Receive(polyDataMessage->GetBufferBodyPointer(), polyDataMessage->GetBufferBodySize());

      int c = polyDataMessage->Unpack(self->IgtlMessageCrcCheckEnabled);
      if (c & igtl::MessageHeader::UNPACK_BODY)
      {
        std::string fileName;
        // Check metadata for requisite parameters, if absent, check deviceName
        if (polyDataMessage->GetHeaderVersion() > IGTL_HEADER_VERSION_1)
        {
          if (!polyDataMessage->GetMetaDataElement("filename", fileName))
          {
            fileName = polyDataMessage->GetDeviceName();
            if (fileName.empty())
            {
              LOG_ERROR("GetPolyData message sent with no filename in either metadata or deviceName field.");
              continue;
            }
          }
        }
        else
        {
          fileName = polyDataMessage->GetDeviceName();
          if (fileName.empty())
          {
            LOG_ERROR("GetPolyData message sent with no filename in either metadata or deviceName field.");
            continue;
          }
        }

        vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
        reader->SetFileName(fileName.c_str());
        reader->Update();

        auto polyData = reader->GetOutput();
        if (polyData != nullptr)
        {
          igtl::MessageBase::Pointer msg = self->IgtlMessageFactory->CreateSendMessage("POLYDATA", client->ClientInfo.GetClientHeaderVersion());
          igtl::PolyDataMessage* polyMsg = dynamic_cast<igtl::PolyDataMessage*>(msg.GetPointer());

          igtlioPolyDataConverter::ContentData data;
          data.deviceName = "PlusServer";
          data.polydata = polyData;

          igtlioBaseConverter::HeaderData header;
          header.deviceName = "PlusServer";

          igtlioPolyDataConverter::toIGTL(header, data, (igtl::PolyDataMessage::Pointer*)&msg);
          if (!msg->SetMetaDataElement("fileName", IANA_TYPE_US_ASCII, fileName))
          {
            LOG_ERROR("Filename too long to be sent back to client. Aborting.");
            continue;
          }
          self->QueueMessageResponseForClient(client->ClientId, msg);
          continue;
        }

        igtl::MessageBase::Pointer msg = self->IgtlMessageFactory->CreateSendMessage("RTS_POLYDATA", polyDataMessage->GetHeaderVersion());
        igtl::RTSPolyDataMessage* rtsPolyMsg = dynamic_cast<igtl::RTSPolyDataMessage*>(msg.GetPointer());
        rtsPolyMsg->SetStatus(false);
        self->QueueMessageResponseForClient(client->ClientId, rtsPolyMsg);
      }
      else
      {
        LOG_ERROR("Client " << clientId << " GET_POLYDATA failed: could not retrieve message");
        return NULL;
      }
    }
    else if (typeid(*bodyMessage) == typeid(igtl::StatusMessage))
    {
      // status message is used as a keep-alive, don't do anything
      clientSocket->Skip(headerMsg->GetBodySizeToRead(), 0);
    }
    else if (typeid(*bodyMessage) == typeid(igtl::GetImageMetaMessage))
    {
      igtl::GetImageMetaMessage::Pointer getImageMetaMsg = dynamic_cast<igtl::GetImageMetaMessage*>(bodyMessage.GetPointer());
      getImageMetaMsg->SetMessageHeader(headerMsg);
      getImageMetaMsg->AllocateBuffer();

      clientSocket->Receive(getImageMetaMsg->GetBufferBodyPointer(), getImageMetaMsg->GetBufferBodySize());

      int c = getImageMetaMsg->Unpack(self->IgtlMessageCrcCheckEnabled);
      if (c & igtl::MessageHeader::UNPACK_BODY)
      {
        // Image meta message
        std::string deviceName("");
        if (headerMsg->GetDeviceName() != NULL)
        {
          deviceName = headerMsg->GetDeviceName();
        }
        self->PlusCommandProcessor->QueueGetImageMetaData(clientId, deviceName);
      }
      else
      {
        LOG_ERROR("Client " << clientId << " GET_IMGMETA failed: could not retrieve message");
        return NULL;
      }
    }
    else if (typeid(*bodyMessage) == typeid(igtl::GetImageMessage))
    {
      igtl::GetImageMessage::Pointer getImageMsg = dynamic_cast<igtl::GetImageMessage*>(bodyMessage.GetPointer());
      getImageMsg->SetMessageHeader(headerMsg);
      getImageMsg->AllocateBuffer();

      clientSocket->Receive(getImageMsg->GetBufferBodyPointer(), getImageMsg->GetBufferBodySize());

      int c = getImageMsg->Unpack(self->IgtlMessageCrcCheckEnabled);
      if (c & igtl::MessageHeader::UNPACK_BODY)
      {
        std::string deviceName("");
        if (headerMsg->GetDeviceName() != NULL)
        {
          deviceName = headerMsg->GetDeviceName();
        }
        else
        {
          LOG_ERROR("Please select the image you want to acquire");
          return NULL;
        }
        self->PlusCommandProcessor->QueueGetImage(clientId, deviceName);
      }
      else
      {
        LOG_ERROR("Client " << clientId << " GET_IMAGE failed: could not retrieve message");
        return NULL;
      }

    }
    else if (typeid(*bodyMessage) == typeid(igtl::GetPointMessage))
    {
      igtl::GetPointMessage* getPointMsg = dynamic_cast<igtl::GetPointMessage*>(bodyMessage.GetPointer());
      getPointMsg->SetMessageHeader(headerMsg);
      getPointMsg->AllocateBuffer();

      clientSocket->Receive(getPointMsg->GetBufferBodyPointer(), getPointMsg->GetBufferBodySize());

      int c = getPointMsg->Unpack(self->IgtlMessageCrcCheckEnabled);
      if (c & igtl::MessageHeader::UNPACK_BODY)
      {
        std::string fileName;
        if (!getPointMsg->GetMetaDataElement("Filename", fileName))
        {
          fileName = getPointMsg->GetDeviceName();
        }

        if (igsioCommon::Tail(fileName, 4) != "fcsv")
        {
          LOG_WARNING("Filename does not end in fcsv. GetPoint behaviour may not function correctly.");
        }

        if (!vtksys::SystemTools::FileExists(fileName) &&
            !vtksys::SystemTools::FileExists(vtkPlusConfig::GetInstance()->GetImagePath(fileName)))
        {
          LOG_ERROR("File: " << fileName << " requested but does not exist. Cannot get POINT data from it.");
          return NULL;
        }

        igtl::MessageBase::Pointer msg = self->IgtlMessageFactory->CreateSendMessage("POINT", client->ClientInfo.GetClientHeaderVersion());
        igtl::PointMessage* pointMsg = dynamic_cast<igtl::PointMessage*>(msg.GetPointer());

        std::ifstream t(fileName);
        if (!t.is_open())
        {
          t.open(vtkPlusConfig::GetInstance()->GetImagePath(fileName));
          if (!t.is_open())
          {
            LOG_ERROR("Cannot read file: " << fileName);
            return NULL;
          }
        }
        std::stringstream buffer;
        buffer << t.rdbuf();
        std::vector<std::string> lines = igsioCommon::SplitStringIntoTokens(buffer.str(), '\n', false);
        for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it)
        {
          std::string line = igsioCommon::Trim(*it);
          if (line[0] == '#')
          {
            continue;
          }

          std::vector<std::string> tokens = igsioCommon::SplitStringIntoTokens(line, ',', true);
          igtl::PointElement::Pointer elem = igtl::PointElement::New();
          elem->SetPosition(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
          elem->SetName(tokens[0].c_str());
          elem->SetGroupName("Point");
          pointMsg->AddPointElement(elem);
        }

        self->QueueMessageResponseForClient(client->ClientId, pointMsg);
      }
      else
      {
        LOG_ERROR("Client " << clientId << " GET_POINT failed: could not retrieve message");
        return NULL;
      }
    }
    else
    {
      // if the device type is unknown, skip reading.
      LOG_WARNING("Unknown OpenIGTLink message is received from client " << clientId << ". Device type: " << headerMsg->GetMessageType()
                  << ". Device name: " << headerMsg->GetDeviceName() << ".");
      clientSocket->Skip(headerMsg->GetBodySizeToRead(), 0);
      continue;
    }
  } // ConnectionActive

  // Close thread
  client->DataReceiverThreadId = -1;
  client->DataReceiverActive.second = false;
  return NULL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkServer::SendTrackedFrame(igsioTrackedFrame& trackedFrame)
{
  int numberOfErrors = 0;

  // Update transform repository with the tracked frame
  if (this->TransformRepository != NULL)
  {
    if (this->TransformRepository->SetTransforms(trackedFrame) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set current transforms to transform repository");
      numberOfErrors++;
    }
  }

  // Convert relative timestamp to UTC
  double timestampSystem = trackedFrame.GetTimestamp(); // save original timestamp, we'll restore it later
  double timestampUniversal = vtkIGSIOAccurateTimer::GetUniversalTimeFromSystemTime(timestampSystem);
  trackedFrame.SetTimestamp(timestampUniversal);

  std::vector<int> disconnectedClientIds;
  {
    // Lock before we send message to the clients
    igsioLockGuard<vtkIGSIORecursiveCriticalSection> igtlClientsMutexGuardedLock(this->IgtlClientsMutex);
    for (std::list<ClientData>::iterator clientIterator = this->IgtlClients.begin(); clientIterator != this->IgtlClients.end(); ++clientIterator)
    {
      igtl::ClientSocket::Pointer clientSocket = (*clientIterator).ClientSocket;

      // Create IGT messages
      std::vector<igtl::MessageBase::Pointer> igtlMessages;
      std::vector<igtl::MessageBase::Pointer>::iterator igtlMessageIterator;

      if (this->IgtlMessageFactory->PackMessages(clientIterator->ClientId, clientIterator->ClientInfo, igtlMessages, trackedFrame, this->SendValidTransformsOnly, this->TransformRepository) != PLUS_SUCCESS)
      {
        LOG_WARNING("Failed to pack all IGT messages");
      }

      // Send all messages to a client
      for (igtlMessageIterator = igtlMessages.begin(); igtlMessageIterator != igtlMessages.end(); ++igtlMessageIterator)
      {
        igtl::MessageBase::Pointer igtlMessage = (*igtlMessageIterator);
        if (igtlMessage.IsNull())
        {
          continue;
        }

        int retValue = 0;
        RETRY_UNTIL_TRUE((retValue = clientSocket->Send(igtlMessage->GetBufferPointer(), igtlMessage->GetBufferSize())) != 0, this->NumberOfRetryAttempts, this->DelayBetweenRetryAttemptsSec);
        if (retValue == 0)
        {
          disconnectedClientIds.push_back(clientIterator->ClientId);
          igtl::TimeStamp::Pointer ts = igtl::TimeStamp::New();
          igtlMessage->GetTimeStamp(ts);
          LOG_INFO("Client disconnected - could not send " << igtlMessage->GetMessageType() << " message to client (device name: " << igtlMessage->GetDeviceName()
                   << "  Timestamp: " << std::fixed << ts->GetTimeStamp() << ").");
          break;
        }

        // Update the TDATA timestamp, even if TDATA isn't sent (cheaper than checking for existing TDATA message type)
        clientIterator->ClientInfo.SetLastTDATASentTimeStamp(trackedFrame.GetTimestamp());
      }
    }
  }

  // Clean up disconnected clients
  for (std::vector< int >::iterator it = disconnectedClientIds.begin(); it != disconnectedClientIds.end(); ++it)
  {
    DisconnectClient(*it);
  }

  // restore original timestamp
  trackedFrame.SetTimestamp(timestampSystem);

  return (numberOfErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL);
}

//----------------------------------------------------------------------------
void vtkPlusOpenIGTLinkServer::DisconnectClient(int clientId)
{
  // Stop the client's data receiver thread
  {
    // Request thread stop
    igsioLockGuard<vtkIGSIORecursiveCriticalSection> igtlClientsMutexGuardedLock(this->IgtlClientsMutex);
    for (std::list<ClientData>::iterator clientIterator = this->IgtlClients.begin(); clientIterator != this->IgtlClients.end(); ++clientIterator)
    {
      if (clientIterator->ClientId != clientId)
      {
        continue;
      }
      clientIterator->DataReceiverActive.first = false;
      break;
    }
  }

  // Wait for the thread to stop
  bool clientDataReceiverThreadStillActive = false;
  do
  {
    clientDataReceiverThreadStillActive = false;
    {
      // check if any of the receiver threads are still active
      igsioLockGuard<vtkIGSIORecursiveCriticalSection> igtlClientsMutexGuardedLock(this->IgtlClientsMutex);
      for (std::list<ClientData>::iterator clientIterator = this->IgtlClients.begin(); clientIterator != this->IgtlClients.end(); ++clientIterator)
      {
        if (clientIterator->ClientId != clientId)
        {
          continue;
        }
        if (clientIterator->DataReceiverThreadId > 0)
        {
          if (clientIterator->DataReceiverActive.second)
          {
            // thread still running
            clientDataReceiverThreadStillActive = true;
          }
          else
          {
            // thread stopped
            clientIterator->DataReceiverThreadId = -1;
          }
          break;
        }
      }
    }
    if (clientDataReceiverThreadStillActive)
    {
      // give some time for the threads to finish
      vtkIGSIOAccurateTimer::DelayWithEventProcessing(0.2);
    }
  }
  while (clientDataReceiverThreadStillActive);

  // Close socket and remove client from the list
  int port = 0;
  std::string address = "unknown";
  {
    igsioLockGuard<vtkIGSIORecursiveCriticalSection> igtlClientsMutexGuardedLock(this->IgtlClientsMutex);
    for (std::list<ClientData>::iterator clientIterator = this->IgtlClients.begin(); clientIterator != this->IgtlClients.end(); ++clientIterator)
    {
      if (clientIterator->ClientId != clientId)
      {
        continue;
      }
      if (clientIterator->ClientSocket.IsNotNull())
      {
#if (OPENIGTLINK_VERSION_MAJOR > 1) || ( OPENIGTLINK_VERSION_MAJOR == 1 && OPENIGTLINK_VERSION_MINOR > 9 ) || ( OPENIGTLINK_VERSION_MAJOR == 1 && OPENIGTLINK_VERSION_MINOR == 9 && OPENIGTLINK_VERSION_PATCH > 4 )
        clientIterator->ClientSocket->GetSocketAddressAndPort(address, port);
#endif
        clientIterator->ClientSocket->CloseSocket();
      }
      this->IgtlClients.erase(clientIterator);
      break;
    }
  }

#if defined(OpenIGTLink_ENABLE_VIDEOSTREAMING)
  this->IgtlMessageFactory->RemoveClientEncoders(clientId);
#endif

  LOG_INFO("Client disconnected (" <<  address << ":" << port << "). Number of connected clients: " << GetNumberOfConnectedClients());
}

//----------------------------------------------------------------------------
void vtkPlusOpenIGTLinkServer::KeepAlive()
{
  LOG_TRACE("Keep alive packet sent to clients...");

  std::vector< int > disconnectedClientIds;

  {
    // Lock before we send message to the clients
    igsioLockGuard<vtkIGSIORecursiveCriticalSection> igtlClientsMutexGuardedLock(this->IgtlClientsMutex);

    for (std::list<ClientData>::iterator clientIterator = this->IgtlClients.begin(); clientIterator != this->IgtlClients.end(); ++clientIterator)
    {
      igtl::StatusMessage::Pointer replyMsg = igtl::StatusMessage::New();
      replyMsg->SetCode(igtl::StatusMessage::STATUS_OK);
      replyMsg->Pack();

      int retValue = 0;
      RETRY_UNTIL_TRUE(
        (retValue = clientIterator->ClientSocket->Send(replyMsg->GetPackPointer(), replyMsg->GetPackSize())) != 0,
        this->NumberOfRetryAttempts, this->DelayBetweenRetryAttemptsSec);
      if (retValue == 0)
      {
        disconnectedClientIds.push_back(clientIterator->ClientId);
        igtl::TimeStamp::Pointer ts = igtl::TimeStamp::New();
        replyMsg->GetTimeStamp(ts);

        LOG_DEBUG("Client disconnected - could not send " << replyMsg->GetMessageType() << " message to client (device name: " << replyMsg->GetDeviceName()
                  << "  Timestamp: " << std::fixed <<  ts->GetTimeStamp() << ").");
      }
    } // clientIterator
  } // unlock client list

  // Clean up disconnected clients
  for (std::vector< int >::iterator it = disconnectedClientIds.begin(); it != disconnectedClientIds.end(); ++it)
  {
    DisconnectClient(*it);
  }
}

//------------------------------------------------------------------------------
unsigned int vtkPlusOpenIGTLinkServer::GetNumberOfConnectedClients() const
{
  // Lock before we send message to the clients
  igsioLockGuard<vtkIGSIORecursiveCriticalSection> igtlClientsMutexGuardedLock(this->IgtlClientsMutex);
  return this->IgtlClients.size();
}

//------------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkServer::GetClientInfo(unsigned int clientId, PlusIgtlClientInfo& outClientInfo) const
{
  igsioLockGuard<vtkIGSIORecursiveCriticalSection> igtlClientsMutexGuardedLock(this->IgtlClientsMutex);
  for (std::list<ClientData>::const_iterator it = this->IgtlClients.begin(); it != this->IgtlClients.end(); ++it)
  {
    if (it->ClientId == clientId)
    {
      outClientInfo = it->ClientInfo;
      return PLUS_SUCCESS;
    }
  }

  return PLUS_FAIL;
}

//------------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkServer::ReadConfiguration(vtkXMLDataElement* serverElement, const std::string& aFilename)
{
  LOG_TRACE("vtkPlusOpenIGTLinkServer::ReadConfiguration");

  if (aFilename.empty())
  {
    LOG_ERROR("Unable to configure PlusServer without an acceptable config file submitted.");
    return PLUS_FAIL;
  }
  this->SetConfigFilename(aFilename);

  XML_READ_SCALAR_ATTRIBUTE_REQUIRED(int, ListeningPort, serverElement);
  XML_READ_STRING_ATTRIBUTE_REQUIRED(OutputChannelId, serverElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, MissingInputGracePeriodSec, serverElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, MaxTimeSpentWithProcessingMs, serverElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, MaxNumberOfIgtlMessagesToSend, serverElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, NumberOfRetryAttempts, serverElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, DelayBetweenRetryAttemptsSec, serverElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, KeepAliveIntervalSec, serverElement);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(SendValidTransformsOnly, serverElement);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(IgtlMessageCrcCheckEnabled, serverElement);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(LogWarningOnNoDataAvailable, serverElement);

  this->DefaultClientInfo.IgtlMessageTypes.clear();
  this->DefaultClientInfo.TransformNames.clear();
  this->DefaultClientInfo.ImageStreams.clear();
  this->DefaultClientInfo.StringNames.clear();
  this->DefaultClientInfo.SetTDATAResolution(0);
  this->DefaultClientInfo.SetTDATARequested(false);

  vtkXMLDataElement* defaultClientInfo = serverElement->FindNestedElementWithName("DefaultClientInfo");
  if (defaultClientInfo != NULL)
  {
    if (this->DefaultClientInfo.SetClientInfoFromXmlData(defaultClientInfo) != PLUS_SUCCESS)
    {
      return PLUS_FAIL;
    }
  }

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float, DefaultClientSendTimeoutSec, serverElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float, DefaultClientReceiveTimeoutSec, serverElement);

  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
int vtkPlusOpenIGTLinkServer::ProcessPendingCommands()
{
  return this->PlusCommandProcessor->ExecuteCommands();
}

//------------------------------------------------------------------------------
bool vtkPlusOpenIGTLinkServer::HasGracePeriodExpired()
{
  return (vtkIGSIOAccurateTimer::GetSystemTime() - this->BroadcastStartTime) > this->MissingInputGracePeriodSec;
}

//------------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkServer::Start(vtkPlusDataCollector* dataCollector, vtkIGSIOTransformRepository* transformRepository, vtkXMLDataElement* serverElement, const std::string& configFilePath)
{
  if (serverElement == NULL)
  {
    LOG_ERROR("NULL configuration sent to vtkPlusOpenIGTLinkServer::Start. Unable to start PlusServer.");
    return PLUS_FAIL;
  }

  this->SetDataCollector(dataCollector);
  if (this->ReadConfiguration(serverElement, configFilePath.c_str()) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to read PlusOpenIGTLinkServer configuration");
    return PLUS_FAIL;
  }

  this->SetTransformRepository(transformRepository);
  if (this->StartOpenIGTLinkService() != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to start Plus OpenIGTLink server");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkServer::Stop()
{
  PlusStatus status = PLUS_SUCCESS;

  if (StopOpenIGTLinkService() != PLUS_SUCCESS)
  {
    status = PLUS_FAIL;
  }

  SetDataCollector(NULL);

  SetTransformRepository(NULL);

  return status;
}

//------------------------------------------------------------------------------
igtl::MessageBase::Pointer vtkPlusOpenIGTLinkServer::CreateIgtlMessageFromCommandResponse(vtkPlusCommandResponse* response)
{
  int replyHeaderVersion = IGTL_HEADER_VERSION_1;
  PlusIgtlClientInfo info;
  if (GetClientInfo(response->GetClientId(), info) == PLUS_FAIL)
  {
    LOG_ERROR("Unable to find client with ID: " << response->GetClientId());
  }
  else
  {
    replyHeaderVersion = info.GetClientHeaderVersion();
  }
  vtkPlusCommandStringResponse* stringResponse = vtkPlusCommandStringResponse::SafeDownCast(response);
  if (stringResponse)
  {
    igtl::StringMessage::Pointer igtlMessage = dynamic_cast<igtl::StringMessage*>(this->IgtlMessageFactory->CreateSendMessage("STRING", replyHeaderVersion).GetPointer());
    igtlMessage->SetDeviceName(stringResponse->GetDeviceName().c_str());
    igtlMessage->SetString(stringResponse->GetMessage());
    LOG_DEBUG("String response: " << stringResponse->GetMessage());
    return igtlMessage.GetPointer();
  }

  vtkPlusCommandImageResponse* imageResponse = vtkPlusCommandImageResponse::SafeDownCast(response);
  if (imageResponse)
  {
    std::string imageName = imageResponse->GetImageName();
    if (imageName.empty())
    {
      imageName = "PlusServerImage";
    }

    vtkSmartPointer<vtkMatrix4x4> imageToReferenceTransform = vtkSmartPointer<vtkMatrix4x4>::New();
    if (imageResponse->GetImageToReferenceTransform() != NULL)
    {
      imageToReferenceTransform = imageResponse->GetImageToReferenceTransform();
    }

    vtkImageData* imageData = imageResponse->GetImageData();
    if (imageData == NULL)
    {
      LOG_ERROR("Invalid image data in command response");
      return NULL;
    }

    igtl::ImageMessage::Pointer igtlMessage = dynamic_cast<igtl::ImageMessage*>(this->IgtlMessageFactory->CreateSendMessage("IMAGE", replyHeaderVersion).GetPointer());
    igtlMessage->SetDeviceName(imageName.c_str());

    if (vtkPlusIgtlMessageCommon::PackImageMessage(igtlMessage, imageData, *imageToReferenceTransform, vtkIGSIOAccurateTimer::GetSystemTime()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to create image mesage from command response");
      return NULL;
    }
    return igtlMessage.GetPointer();
  }

  vtkPlusCommandPolydataResponse* polydataResponse = vtkPlusCommandPolydataResponse::SafeDownCast(response);
  if (polydataResponse)
  {
    std::string polydataName = polydataResponse->GetPolyDataName();
    if (polydataName.empty())
    {
      polydataName = "UnknownFile";
    }

    vtkSmartPointer<vtkPolyData> polyData = polydataResponse->GetPolyData();
    if (polyData == NULL)
    {
      LOG_ERROR("Missing polydata in command response");
      return NULL;
    }

    igtl::PolyDataMessage::Pointer igtlMessage = dynamic_cast<igtl::PolyDataMessage*>(this->IgtlMessageFactory->CreateSendMessage("POLYDATA", replyHeaderVersion).GetPointer());
    igtlMessage->SetDeviceName("PlusServer");
    igtlMessage->SetMetaDataElement("fileName", IANA_TYPE_US_ASCII, polydataName);

    if (vtkPlusIgtlMessageCommon::PackPolyDataMessage(igtlMessage, polyData, vtkIGSIOAccurateTimer::GetSystemTime()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to create polydata mesage from command response");
      return NULL;
    }
    return igtlMessage.GetPointer();
  }

  vtkPlusCommandImageMetaDataResponse* imageMetaDataResponse = vtkPlusCommandImageMetaDataResponse::SafeDownCast(response);
  if (imageMetaDataResponse)
  {
    std::string imageMetaDataName = "PlusServerImageMetaData";
    igsioCommon::ImageMetaDataList imageMetaDataList;
    imageMetaDataResponse->GetImageMetaDataItems(imageMetaDataList);
    igtl::ImageMetaMessage::Pointer igtlMessage = dynamic_cast<igtl::ImageMetaMessage*>(this->IgtlMessageFactory->CreateSendMessage("IMGMETA", replyHeaderVersion).GetPointer());
    igtlMessage->SetDeviceName(imageMetaDataName.c_str());
    if (vtkPlusIgtlMessageCommon::PackImageMetaMessage(igtlMessage, imageMetaDataList) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to create image mesage from command response");
      return NULL;
    }
    return igtlMessage.GetPointer();
  }

  vtkPlusCommandRTSCommandResponse* commandResponse = vtkPlusCommandRTSCommandResponse::SafeDownCast(response);
  if (commandResponse)
  {
    if (!commandResponse->GetRespondWithCommandMessage())
    {
      // Incoming command was a v1/v2 style command, reply as such
      igtl::StringMessage::Pointer igtlMessage = dynamic_cast<igtl::StringMessage*>(this->IgtlMessageFactory->CreateSendMessage("STRING", replyHeaderVersion).GetPointer());
      igtlMessage->SetDeviceName(vtkPlusCommand::GenerateReplyDeviceName(commandResponse->GetOriginalId()));

      std::ostringstream replyStr;
      replyStr << "<CommandReply";
      replyStr << " Status=\"" << (commandResponse->GetStatus() == PLUS_SUCCESS ? "SUCCESS" : "FAIL") << "\"";
      replyStr << " Message=\"";
      // Write to XML, encoding special characters, such as " ' \ < > &
      vtkXMLUtilities::EncodeString(commandResponse->GetResultString().c_str(), VTK_ENCODING_NONE, replyStr, VTK_ENCODING_NONE, 1 /* encode special characters */);
      replyStr << "\"";
      replyStr << " />";

      igtlMessage->SetString(replyStr.str().c_str());
      LOG_DEBUG("Command response: " << replyStr.str());
      return igtlMessage.GetPointer();
    }
    else
    {
      // Incoming command was a modern style command, reply using our latest
      igtl::RTSCommandMessage::Pointer igtlMessage = dynamic_cast<igtl::RTSCommandMessage*>(this->IgtlMessageFactory->CreateSendMessage("RTS_COMMAND", replyHeaderVersion).GetPointer());
      //TODO : should this device name be the name of the server?
      igtlMessage->SetDeviceName(commandResponse->GetDeviceName().c_str());
      igtlMessage->SetCommandName(commandResponse->GetCommandName());
      igtlMessage->SetCommandId(commandResponse->GetOriginalId());

      // Send command result details both in XML and in metadata, slowly phase towards metadata
      std::ostringstream replyStr;
      if (commandResponse->GetUseDefaultFormat())
      {

        replyStr << "<CommandReply";
        replyStr << " Name=\"" << commandResponse->GetCommandName() << "\"";
        replyStr << " Status=\"" << (commandResponse->GetStatus() ? "SUCCESS" : "FAIL") << "\"";
        if (commandResponse->GetStatus() == PLUS_FAIL)
        {
          replyStr << " Error=\"" << commandResponse->GetErrorString() << "\"";
        }
        replyStr << " Message=\"" << commandResponse->GetResultString() << "\"></CommandReply>";
        igtlMessage->SetMetaDataElement("Message", IANA_TYPE_US_ASCII, commandResponse->GetResultString());
      }
      else
      {
        replyStr << commandResponse->GetResultString();
      }
      igtlMessage->SetCommandContent(replyStr.str());

      for (igtl::MessageBase::MetaDataMap::const_iterator it = begin(commandResponse->GetParameters()); it != end(commandResponse->GetParameters()); ++it)
      {
        igtlMessage->SetMetaDataElement(it->first, it->second.first, it->second.second);
      }

      igtlMessage->SetMetaDataElement("Status", IANA_TYPE_US_ASCII, (commandResponse->GetStatus() ? "SUCCESS" : "FAIL"));
      if (commandResponse->GetStatus() == PLUS_FAIL)
      {
        igtlMessage->SetMetaDataElement("Error", IANA_TYPE_US_ASCII, commandResponse->GetErrorString());
      }

      LOG_DEBUG("Command response: " << replyStr.str());
      return igtlMessage.GetPointer();
    }
  }

  LOG_ERROR("vtkPlusOpenIGTLinkServer::CreateIgtlMessageFromCommandResponse failed: invalid command response");
  return NULL;
}
