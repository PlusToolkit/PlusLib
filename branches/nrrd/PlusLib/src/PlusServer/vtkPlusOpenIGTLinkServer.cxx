/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "TrackedFrame.h"
#include "igtlImageMetaMessage.h"
#include "igtlImageMessage.h"
#include "igtlMessageHeader.h"
#include "igtlPlusClientInfoMessage.h"
#include "igtlStatusMessage.h"
#include "vtkDataCollector.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusCommandProcessor.h"
#include "vtkPlusIgtlMessageCommon.h"
#include "vtkPlusIgtlMessageFactory.h" 
#include "vtkPlusOpenIGTLinkServer.h"
#include "vtkRecursiveCriticalSection.h"
#include "vtkTrackedFrameList.h"
#include "vtkTransformRepository.h" 
#include "vtkPlusCommand.h"

static const double DELAY_ON_SENDING_ERROR_SEC = 0.02; 
static const double DELAY_ON_NO_NEW_FRAMES_SEC = 0.005; 
static const int CLIENT_SOCKET_TIMEOUT_MSEC = 500;
static const int NUMBER_OF_RECENT_COMMAND_IDS_STORED = 10;
static const int IGTL_EMPTY_DATA_SIZE = -1;

//----------------------------------------------------------------------------
// If a frame cannot be retrieved from the device buffers (because it was overwritten by new frames)
// then we skip a SAMPLING_SKIPPING_MARGIN_SEC long period to allow the application to catch up.
// This time should be long enough to comfortably retrieve a frame from the buffer.
static const double SAMPLING_SKIPPING_MARGIN_SEC=0.1;

vtkStandardNewMacro( vtkPlusOpenIGTLinkServer ); 

vtkCxxSetObjectMacro(vtkPlusOpenIGTLinkServer, TransformRepository, vtkTransformRepository);
vtkCxxSetObjectMacro(vtkPlusOpenIGTLinkServer, DataCollector, vtkDataCollector);

int vtkPlusOpenIGTLinkServer::ClientIdCounter=1;

//----------------------------------------------------------------------------
vtkPlusOpenIGTLinkServer::vtkPlusOpenIGTLinkServer()
: ListeningPort(-1)
, LastSentTrackedFrameTimestamp(0)
, NumberOfRetryAttempts(10) 
, DelayBetweenRetryAttemptsSec(0.100)
, MaxNumberOfIgtlMessagesToSend(100)
, MaxTimeSpentWithProcessingMs(50)
, LastProcessingTimePerFrameMs(-1)
, ConnectionReceiverThreadId(-1)
, DataSenderThreadId(-1)
, ConnectionActive(std::make_pair(false,false))
, DataSenderActive(std::make_pair(false,false))
, DataCollector(NULL)
, TransformRepository(NULL)
, Threader(vtkSmartPointer<vtkMultiThreader>::New())
, IgtlClientsMutex(vtkSmartPointer<vtkRecursiveCriticalSection>::New())
, ServerSocket(igtl::ServerSocket::New())
, SendValidTransformsOnly(true)
, IgtlMessageCrcCheckEnabled(0)
, PlusCommandProcessor(vtkSmartPointer<vtkPlusCommandProcessor>::New())
, OutputChannelId(NULL)
, BroadcastChannel(NULL)
, ConfigFilename(NULL)
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
void vtkPlusOpenIGTLinkServer::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkServer::StartOpenIGTLinkService()
{
  if ( this->DataCollector == NULL )
  {
    LOG_WARNING( "Tried to start OpenIGTLink server without a vtkDataCollector" );
    return PLUS_FAIL;
  }

  if ( this->ConnectionReceiverThreadId < 0 )
  {
    this->ConnectionActive.first = true;
    this->ConnectionReceiverThreadId = this->Threader->SpawnThread( (vtkThreadFunctionType)&ConnectionReceiverThread, this );
    LOG_INFO( "Plus OpenIGTLink server started on port: " << this->ListeningPort ); 
  }

  if ( this->DataSenderThreadId < 0 )
  {
    this->DataSenderActive.first = true;
    this->DataSenderThreadId = this->Threader->SpawnThread( (vtkThreadFunctionType)&DataSenderThread, this );
  }

  std::ostringstream ss; 
  ss << "Data sent by default: ";
  this->DefaultClientInfo.PrintSelf(ss, vtkIndent(0));
  LOG_DEBUG(ss.str());

  this->PlusCommandProcessor->SetPlusServer(this);

  this->BroadcastStartTime = vtkAccurateTimer::GetSystemTime();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkServer::StopOpenIGTLinkService()
{
  // Stop connection receiver thread
  if ( this->ConnectionReceiverThreadId >= 0 )
  {
    this->ConnectionActive.first = false;
    while ( this->ConnectionActive.second )
    {
      // Wait until the thread stops 
      vtkAccurateTimer::DelayWithEventProcessing( 0.2 ); 
    }
    this->ConnectionReceiverThreadId = -1;
    LOG_DEBUG("ConnectionReceiverThread stopped");
  }

  // Disconnect clients (stop receiving thread, close socket)
  std::vector< int > clientIds;
  {
    // Get all the client ids and release the lock
    PlusLockGuard<vtkRecursiveCriticalSection> igtlClientsMutexGuardedLock(this->IgtlClientsMutex);
    for (std::list<ClientData>::iterator clientIterator = this->IgtlClients.begin(); clientIterator != this->IgtlClients.end(); ++clientIterator)
    {
      clientIds.push_back(clientIterator->ClientId);
    }
  }
  for (std::vector< int >::iterator it=clientIds.begin(); it!=clientIds.end(); ++it)
  {
    DisconnectClient(*it);
  }

  LOG_INFO( "Plus OpenIGTLink server stopped.");

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void* vtkPlusOpenIGTLinkServer::ConnectionReceiverThread( vtkMultiThreader::ThreadInfo* data )
{
  vtkPlusOpenIGTLinkServer* self = (vtkPlusOpenIGTLinkServer*)( data->UserData );

  int r = self->ServerSocket->CreateServer( self->ListeningPort );
  if ( r < 0 )
  {
    LOG_ERROR( "Cannot create a server socket." );
    return NULL;
  }

  self->ConnectionActive.second = true;

  // Wait for connections until we want to stop the thread
  while ( self->ConnectionActive.first )
  {
    igtl::ClientSocket::Pointer newClientSocket = self->ServerSocket->WaitForConnection( CLIENT_SOCKET_TIMEOUT_MSEC );
    if (newClientSocket.IsNotNull())
    {
      // Lock before we change the clients list 
      PlusLockGuard<vtkRecursiveCriticalSection> igtlClientsMutexGuardedLock(self->IgtlClientsMutex);
      ClientData newClient;
      self->IgtlClients.push_back(newClient);

      ClientData* client = &(self->IgtlClients.back()); // get a reference to the client data that is stored in the list
      client->ClientId=self->ClientIdCounter;
      self->ClientIdCounter++;
      client->ClientSocket = newClientSocket;
      client->ClientSocket->SetTimeout( CLIENT_SOCKET_TIMEOUT_MSEC );
      client->ClientInfo = self->DefaultClientInfo;
      client->Server = self;

      int port = 0;
      std::string address = "unknown";
#if (OPENIGTLINK_VERSION_MAJOR > 1) || ( OPENIGTLINK_VERSION_MAJOR == 1 && OPENIGTLINK_VERSION_MINOR > 9 ) || ( OPENIGTLINK_VERSION_MAJOR == 1 && OPENIGTLINK_VERSION_MINOR == 9 && OPENIGTLINK_VERSION_PATCH > 4 )
      newClientSocket->GetSocketAddressAndPort(address, port);
#endif
      LOG_INFO( "Received new client connection (client "<<client->ClientId<<" at "<< address << ":" << port << "). Number of connected clients: " << self->GetNumberOfConnectedClients() );

      client->DataReceiverActive.first = true;
      client->DataReceiverThreadId = self->Threader->SpawnThread( (vtkThreadFunctionType)&DataReceiverThread, client);
    }
  }

  // Close server socket 
  if ( self->ServerSocket.IsNotNull() )
  {
    self->ServerSocket->CloseSocket();
  }
  // Close thread
  self->ConnectionReceiverThreadId = -1;
  self->ConnectionActive.second = false; 
  return NULL;
}

//----------------------------------------------------------------------------
void* vtkPlusOpenIGTLinkServer::DataSenderThread( vtkMultiThreader::ThreadInfo* data )
{
  vtkPlusOpenIGTLinkServer* self = (vtkPlusOpenIGTLinkServer*)( data->UserData );
  self->DataSenderActive.second = true; 

  vtkPlusDevice* aDevice(NULL);
  vtkPlusChannel* aChannel(NULL);

  DeviceCollection aCollection;
  if( self->DataCollector->GetDevices(aCollection) != PLUS_SUCCESS || aCollection.size() == 0 )
  {
    LOG_ERROR("Unable to retrieve devices. Check configuration and connection.");
    return NULL;
  }

  // Find the requested channel ID in all the devices
  for( DeviceCollectionIterator it = aCollection.begin(); it != aCollection.end(); ++it )
  {
    aDevice = *it;
    if( aDevice->GetOutputChannelByName(aChannel, self->GetOutputChannelId() ) == PLUS_SUCCESS )
    {
      break;
    }
  }
  // The requested channel ID is not found, try to find any channel in any device
  if( aChannel == NULL )
  {
    for( DeviceCollectionIterator it = aCollection.begin(); it != aCollection.end(); ++it )
    {
      aDevice = *it;
      if( aDevice->OutputChannelCount() > 0 )
      {
        aChannel = *(aDevice->GetOutputChannelsStart());
        break;
      }
    }
  }
  // If we didn't find any channel then return
  if( aChannel == NULL )
  {
    LOG_WARNING("There are no channels to broadcast. Only command processing is available.");
  }

  self->BroadcastChannel = aChannel;
  if (self->BroadcastChannel)
  {
    self->BroadcastChannel->GetMostRecentTimestamp(self->LastSentTrackedFrameTimestamp);
  }

  double elapsedTimeSinceLastPacketSentSec = 0; 
  while ( self->ConnectionActive.first && self->DataSenderActive.first )
  {
    bool clientsConnected = false;
    {
      PlusLockGuard<vtkRecursiveCriticalSection> igtlClientsMutexGuardedLock(self->IgtlClientsMutex);
      if ( !self->IgtlClients.empty() )
      {
        clientsConnected =true;
      }
    }
    if ( !clientsConnected )
    {
      // No client connected, wait for a while 
      vtkAccurateTimer::Delay(0.2);
      self->LastSentTrackedFrameTimestamp=0; // next time start sending from the most recent timestamp
      continue;
    }

    if( self->HasGracePeriodExpired() )
    {
      self->GracePeriodLogLevel = vtkPlusLogger::LOG_LEVEL_WARNING;
    }

    // Send remote command execution replies to clients

    PlusCommandResponseList replies;
    self->PlusCommandProcessor->PopCommandResponses(replies);
    if (!replies.empty())
    {
      for (PlusCommandResponseList::iterator responseIt=replies.begin(); responseIt!=replies.end(); responseIt++)
      {
        igtl::MessageBase::Pointer igtlResponseMessage = self->CreateIgtlMessageFromCommandResponse(*responseIt);
        if (igtlResponseMessage.IsNull())
        {
          LOG_ERROR("Failed to create OpenIGTLink message from command response");
          continue;
        }
        igtlResponseMessage->Pack();

        bool broadcastResponse=false;
        
        // We treat image messages as special case: we send the results to all clients
        // TODO: now all images are broadcast to all clients, it should be more configurable (the command should be able
        // to specify if the image should be sent to the requesting client or all of them)
        vtkPlusCommandImageResponse* imageResponse=vtkPlusCommandImageResponse::SafeDownCast(*responseIt);
        if (imageResponse)
        {
          broadcastResponse=true;
        }

        if (broadcastResponse)
        {
          LOG_DEBUG("Broadcast command reply: "<<igtlResponseMessage->GetDeviceName());
          PlusLockGuard<vtkRecursiveCriticalSection> igtlClientsMutexGuardedLock(self->IgtlClientsMutex);
          for (std::list<ClientData>::iterator clientIterator = self->IgtlClients.begin(); clientIterator != self->IgtlClients.end(); ++clientIterator)
          {
            if (clientIterator->ClientSocket.IsNull())
            {
              LOG_WARNING("Message reply cannot be sent to client "<<clientIterator->ClientId<<", probably client has been disconnected");
              continue;
            }
            clientIterator->ClientSocket->Send(igtlResponseMessage->GetPackPointer(), igtlResponseMessage->GetPackSize());
          }
        }
        else
        {
          // Only send the response to the client that requested the command
          LOG_DEBUG("Send command reply to client "<<(*responseIt)->GetClientId()<<": "<<igtlResponseMessage->GetDeviceName());
          PlusLockGuard<vtkRecursiveCriticalSection> igtlClientsMutexGuardedLock(self->IgtlClientsMutex);
          igtl::ClientSocket::Pointer clientSocket=NULL;
          for ( std::list<ClientData>::iterator clientIterator = self->IgtlClients.begin(); clientIterator != self->IgtlClients.end(); ++clientIterator)
          {
            if (clientIterator->ClientId==(*responseIt)->GetClientId())
            {
              clientSocket = clientIterator->ClientSocket;
              break;
            }
          }
          if (clientSocket.IsNull())
          {
            LOG_WARNING("Message reply cannot be sent to client "<<(*responseIt)->GetClientId()<<", probably client has been disconnected");
            continue;
          }          
          clientSocket->Send(igtlResponseMessage->GetPackPointer(), igtlResponseMessage->GetPackSize());
        }

      }
    }

    // Send image/tracking/string data

    vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
    double startTimeSec = vtkAccurateTimer::GetSystemTime();

    // Acquire tracked frames since last acquisition (minimum 1 frame)
    if (self->LastProcessingTimePerFrameMs < 1)
    {
      // if processing was less than 1ms/frame then assume it was 1ms (1000FPS processing speed) to avoid division by zero
      self->LastProcessingTimePerFrameMs=1;
    }
    int numberOfFramesToGet = std::max(self->MaxTimeSpentWithProcessingMs / self->LastProcessingTimePerFrameMs, 1); 
    // Maximize the number of frames to send
    numberOfFramesToGet = std::min(numberOfFramesToGet, self->MaxNumberOfIgtlMessagesToSend); 

    if (self->BroadcastChannel!=NULL)
    {
      if ( ( self->BroadcastChannel->HasVideoSource() && !self->BroadcastChannel->GetVideoDataAvailable())
        || (!self->BroadcastChannel->HasVideoSource() && !self->BroadcastChannel->GetTrackingDataAvailable()) )
      {
        LOG_DYNAMIC("No data is broadcasted, as no data is available yet.", self->GracePeriodLogLevel); 
      }
      else
      {
        double oldestDataTimestamp=0;
        if (self->BroadcastChannel->GetOldestTimestamp(oldestDataTimestamp)==PLUS_SUCCESS)
        {
          if (self->LastSentTrackedFrameTimestamp<oldestDataTimestamp)
          {
            LOG_INFO("OpenIGTLink broadcasting started. No data was available between "<<self->LastSentTrackedFrameTimestamp<<"-"<<oldestDataTimestamp<<"sec, therefore no data were broadcasted during this time period.");
            self->LastSentTrackedFrameTimestamp=oldestDataTimestamp+SAMPLING_SKIPPING_MARGIN_SEC;
          }
          if ( self->BroadcastChannel->GetTrackedFrameList(self->LastSentTrackedFrameTimestamp, trackedFrameList, numberOfFramesToGet) != PLUS_SUCCESS )
          {
            LOG_ERROR("Failed to get tracked frame list from data collector (last recorded timestamp: " << std::fixed << self->LastSentTrackedFrameTimestamp ); 
            vtkAccurateTimer::Delay(DELAY_ON_SENDING_ERROR_SEC); 
          }
        }      
      }
    }

    // There is no new frame in the buffer
    if ( trackedFrameList->GetNumberOfTrackedFrames() == 0 )
    {
      vtkAccurateTimer::Delay(DELAY_ON_NO_NEW_FRAMES_SEC);
      elapsedTimeSinceLastPacketSentSec += vtkAccurateTimer::GetSystemTime() - startTimeSec; 

      // Send keep alive packet to clients 
      if ( 1000* elapsedTimeSinceLastPacketSentSec > ( CLIENT_SOCKET_TIMEOUT_MSEC / 2.0 ) )
      {
        self->KeepAlive(); 
        elapsedTimeSinceLastPacketSentSec = 0; 
      }

      continue;
    }

    for ( int i = 0; i < trackedFrameList->GetNumberOfTrackedFrames(); ++i )
    {
      // Send tracked frame
      self->SendTrackedFrame( *trackedFrameList->GetTrackedFrame(i) ); 
      elapsedTimeSinceLastPacketSentSec = 0; 
    }

    // Compute time spent with processing one frame in this round
    double computationTimeMs = (vtkAccurateTimer::GetSystemTime() - startTimeSec) * 1000.0;

    // Update last processing time if new tracked frames have been aquired
    if (trackedFrameList->GetNumberOfTrackedFrames() > 0 )
    {
      self->LastProcessingTimePerFrameMs = computationTimeMs / trackedFrameList->GetNumberOfTrackedFrames();
    } 
  }
  // Close thread
  self->DataSenderThreadId = -1;
  self->DataSenderActive.second = false; 
  return NULL;
}

//----------------------------------------------------------------------------
void* vtkPlusOpenIGTLinkServer::DataReceiverThread( vtkMultiThreader::ThreadInfo* data )
{
  ClientData* client = (ClientData*)( data->UserData );
  client->DataReceiverActive.second = true;
  vtkPlusOpenIGTLinkServer* self = client->Server;

  /*! Store the IDs of recent commands to be able to detect duplicate command IDs */
  std::deque<std::string> previousCommandIds;

  // Make copy of frequently used data to avoid locking of client data
  igtl::ClientSocket::Pointer clientSocket = client->ClientSocket;
  int clientId = client->ClientId;

  while ( client->DataReceiverActive.first )
  {
    igtl::MessageHeader::Pointer headerMsg;
    headerMsg = igtl::MessageHeader::New();
    headerMsg->InitPack();

    // Receive generic header from the socket
    int bytesReceived = clientSocket->Receive( headerMsg->GetPackPointer(), headerMsg->GetPackSize() );
    if ( bytesReceived == IGTL_EMPTY_DATA_SIZE || bytesReceived != headerMsg->GetPackSize() )
    {
      continue; 
    }

    headerMsg->Unpack(self->IgtlMessageCrcCheckEnabled);
    if (strcmp(headerMsg->GetDeviceType(), "CLIENTINFO") == 0)
    {
      igtl::PlusClientInfoMessage::Pointer clientInfoMsg = igtl::PlusClientInfoMessage::New(); 
      clientInfoMsg->SetMessageHeader(headerMsg); 
      clientInfoMsg->AllocatePack(); 

      clientSocket->Receive(clientInfoMsg->GetPackBodyPointer(), clientInfoMsg->GetPackBodySize() ); 

      int c = clientInfoMsg->Unpack(self->IgtlMessageCrcCheckEnabled);
      if (c & igtl::MessageHeader::UNPACK_BODY) 
      {
        // Message received from client, need to lock to modify client info
        PlusLockGuard<vtkRecursiveCriticalSection> igtlClientsMutexGuardedLock(self->IgtlClientsMutex);
        client->ClientInfo=clientInfoMsg->GetClientInfo(); 
        LOG_DEBUG("Client info message received from client " << clientId); 
      }
    }
    else if (strcmp(headerMsg->GetDeviceType(), "GET_STATUS") == 0)
    {
      // Just ping server, we can skip message and respond
      clientSocket->Skip(headerMsg->GetBodySizeToRead(), 0);

      igtl::StatusMessage::Pointer replyMsg = igtl::StatusMessage::New(); 
      replyMsg->SetCode(igtl::StatusMessage::STATUS_OK); 
      replyMsg->Pack(); 
      clientSocket->Send(replyMsg->GetPackPointer(), replyMsg->GetPackBodySize()); 
    }
    else if ( (strcmp(headerMsg->GetDeviceType(), "STRING") == 0) )
    {
      // Received a remote command execution message
      // The command is encoded in in an XML string in a STRING message body

      igtl::StringMessage::Pointer commandMsg = igtl::StringMessage::New(); 
      commandMsg->SetMessageHeader(headerMsg); 
      commandMsg->AllocatePack(); 
      clientSocket->Receive(commandMsg->GetPackBodyPointer(), commandMsg->GetPackBodySize() ); 
        
      int c = commandMsg->Unpack(self->IgtlMessageCrcCheckEnabled);
      if (c & igtl::MessageHeader::UNPACK_BODY) 
      {          
        const char* deviceName = "UNKNOWN";
        if (headerMsg->GetDeviceName() != NULL)
        {
          deviceName = headerMsg->GetDeviceName();
        }
        else
        {
          LOG_ERROR("Received message from unknown device from client "<<clientId);
        }

        std::string deviceNameStr=vtkPlusCommand::GetPrefixFromCommandDeviceName(deviceName);
        std::string uid=vtkPlusCommand::GetUidFromCommandDeviceName(deviceName);;
        if( !uid.empty() )
        {
          if( std::find(previousCommandIds.begin(), previousCommandIds.end(), uid) != previousCommandIds.end() )
          {
            // Command already exists
            LOG_WARNING("Already received a command with id = " << uid << " from client " << clientId <<". This repeated command will be ignored.");
            continue;
          }
          // New command, remember its ID
          previousCommandIds.push_back(uid);
          if (previousCommandIds.size()>NUMBER_OF_RECENT_COMMAND_IDS_STORED)
          {
            previousCommandIds.pop_front();
          }
        }
        std::ostringstream ss;
        ss << "Received command from client "<<clientId<<", device " << deviceNameStr;
        if( !uid.empty() )
        {
          ss << " with UID " << uid;
        }
        ss << ": " << commandMsg->GetString();
        LOG_DEBUG(ss.str());

        self->PlusCommandProcessor->QueueCommand(clientId, commandMsg->GetString(), deviceNameStr, uid);
      }
      else
      {
        LOG_ERROR("STRING message unpacking failed for client "<<clientId);
      }        
    }
    else if ( (strcmp(headerMsg->GetDeviceType(), "GET_IMGMETA") == 0) )
    {
      std::string deviceName("");
      if (headerMsg->GetDeviceName() != NULL)
      {
        deviceName = headerMsg->GetDeviceName();
      }
      self->PlusCommandProcessor->QueueGetImageMetaData(clientId, deviceName);
    }
    else if(strcmp(headerMsg->GetDeviceType(), "GET_IMAGE") == 0)
    {
      std::string deviceName("");
      if (headerMsg->GetDeviceName() != NULL)
      {
        deviceName = headerMsg->GetDeviceName();
      }
      else
      {
        LOG_ERROR("Client "<<clientId<<" GET_IMAGE failed: the image you want to acquire has to be specified in the OpenIGTLink device name");
        return NULL;
      }
      self->PlusCommandProcessor->QueueGetImage(clientId, deviceName);
    }
    else
    {
      // if the device type is unknown, skip reading. 
      LOG_WARNING("Unknown OpenIGTLink message is received from client "<<clientId<<". Device type: "<<headerMsg->GetDeviceType()<<". Device name: "<<headerMsg->GetDeviceName()<<".");
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
PlusStatus vtkPlusOpenIGTLinkServer::SendTrackedFrame( TrackedFrame& trackedFrame )
{
  int numberOfErrors = 0; 

  // Update transform repository with the tracked frame 
  if ( this->TransformRepository != NULL )
  {
    if ( this->TransformRepository->SetTransforms(trackedFrame) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to set current transforms to transform repository"); 
      numberOfErrors++;
    }
  }

  // Convert relative timestamp to UTC
  double timestampSystem = trackedFrame.GetTimestamp(); // save original timestamp, we'll restore it later
  double timestampUniversal = vtkAccurateTimer::GetUniversalTimeFromSystemTime(timestampSystem);
  trackedFrame.SetTimestamp(timestampUniversal);  

  std::vector< int > disconnectedClientIds;

  {
    // Lock before we send message to the clients 
    PlusLockGuard<vtkRecursiveCriticalSection> igtlClientsMutexGuardedLock(this->IgtlClientsMutex);
    for( std::list<ClientData>::iterator clientIterator = this->IgtlClients.begin(); clientIterator != this->IgtlClients.end(); ++clientIterator)
    {
      igtl::ClientSocket::Pointer clientSocket = (*clientIterator).ClientSocket;

      // Create igt messages
      std::vector<igtl::MessageBase::Pointer> igtlMessages; 
      std::vector<igtl::MessageBase::Pointer>::iterator igtlMessageIterator; 

      vtkSmartPointer<vtkPlusIgtlMessageFactory> igtlMessageFactory = vtkSmartPointer<vtkPlusIgtlMessageFactory>::New(); 
      if ( igtlMessageFactory->PackMessages( clientIterator->ClientInfo, igtlMessages, trackedFrame, this->SendValidTransformsOnly, this->TransformRepository ) != PLUS_SUCCESS )
      {
        LOG_WARNING("Failed to pack all IGT messages"); 
      }

      // Send all messages to a client 
      for ( igtlMessageIterator = igtlMessages.begin(); igtlMessageIterator != igtlMessages.end(); ++igtlMessageIterator )
      {
        igtl::MessageBase::Pointer igtlMessage = (*igtlMessageIterator); 
        if ( igtlMessage.IsNull() )
        {
          continue; 
        }

        int retValue = 0;
        RETRY_UNTIL_TRUE( 
          (retValue = clientSocket->Send( igtlMessage->GetPackPointer(), igtlMessage->GetPackSize()))!=0,
          this->NumberOfRetryAttempts, this->DelayBetweenRetryAttemptsSec);
        if ( retValue == 0 )
        {
          disconnectedClientIds.push_back(clientIterator->ClientId);
          igtl::TimeStamp::Pointer ts = igtl::TimeStamp::New(); 
          igtlMessage->GetTimeStamp(ts);
          LOG_DEBUG( "Client disconnected - could not send " << igtlMessage->GetDeviceType() << " message to client (device name: " << igtlMessage->GetDeviceName()
            << "  Timestamp: " << std::fixed <<  ts->GetTimeStamp() << ").");
          break; 
        }

      } // igtlMessageIterator

    } // clientIterator
  } // unlock client list

  // Clean up disconnected clients
  for (std::vector< int >::iterator it=disconnectedClientIds.begin(); it!=disconnectedClientIds.end(); ++it)
  {
    DisconnectClient(*it);
  }

  // restore original timestamp
  trackedFrame.SetTimestamp(timestampSystem);

  return ( numberOfErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL );
}

//----------------------------------------------------------------------------
void vtkPlusOpenIGTLinkServer::DisconnectClient(int clientId)
{
  // Stop the client's data receiver thread
  {
    // Request thread stop
    PlusLockGuard<vtkRecursiveCriticalSection> igtlClientsMutexGuardedLock(this->IgtlClientsMutex);
    for (std::list<ClientData>::iterator clientIterator = this->IgtlClients.begin(); clientIterator != this->IgtlClients.end(); ++clientIterator)
    {
      if (clientIterator->ClientId!=clientId)
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
      PlusLockGuard<vtkRecursiveCriticalSection> igtlClientsMutexGuardedLock(this->IgtlClientsMutex);
      for (std::list<ClientData>::iterator clientIterator = this->IgtlClients.begin(); clientIterator != this->IgtlClients.end(); ++clientIterator)
      {
        if (clientIterator->ClientId!=clientId)
        {
          continue;
        }
        if (clientIterator->DataReceiverThreadId>0)
        {
          if (clientIterator->DataReceiverActive.second)
          {
            // thread still running
            clientDataReceiverThreadStillActive = true;
          }
          else
          {
            // thread stopped
            clientIterator->DataReceiverThreadId=-1;
          }
          break;
        }
      }
    }
    if (clientDataReceiverThreadStillActive)
    {
      // give some time for the threads to finish
      vtkAccurateTimer::DelayWithEventProcessing( 0.2 );
    }
  } while (clientDataReceiverThreadStillActive);

  // Close socket and remove client from the list
  int port = 0;
  std::string address = "unknown";
  {
    PlusLockGuard<vtkRecursiveCriticalSection> igtlClientsMutexGuardedLock(this->IgtlClientsMutex);
    for (std::list<ClientData>::iterator clientIterator = this->IgtlClients.begin(); clientIterator != this->IgtlClients.end(); ++clientIterator)
    {
      if (clientIterator->ClientId!=clientId)
      {
        continue;
      }
      if ( clientIterator->ClientSocket.IsNotNull() )
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
  LOG_INFO( "Client disconnected (" <<  address << ":" << port << "). Number of connected clients: " << GetNumberOfConnectedClients() );
}

//----------------------------------------------------------------------------
void vtkPlusOpenIGTLinkServer::KeepAlive()
{
  LOG_TRACE("Keep alive packet sent to clients..."); 

  std::vector< int > disconnectedClientIds;

  {
    // Lock before we send message to the clients 
    PlusLockGuard<vtkRecursiveCriticalSection> igtlClientsMutexGuardedLock(this->IgtlClientsMutex);

    for( std::list<ClientData>::iterator clientIterator = this->IgtlClients.begin(); clientIterator != this->IgtlClients.end(); ++clientIterator)
    {
      igtl::StatusMessage::Pointer replyMsg = igtl::StatusMessage::New(); 
      replyMsg->SetCode(igtl::StatusMessage::STATUS_OK); 
      replyMsg->Pack();

      int retValue = 0;
      RETRY_UNTIL_TRUE( 
        (retValue = clientIterator->ClientSocket->Send( replyMsg->GetPackPointer(), replyMsg->GetPackSize() ))!=0,
        this->NumberOfRetryAttempts, this->DelayBetweenRetryAttemptsSec);
      if ( retValue == 0 )
      {
        disconnectedClientIds.push_back(clientIterator->ClientId);
        igtl::TimeStamp::Pointer ts = igtl::TimeStamp::New(); 
        replyMsg->GetTimeStamp(ts); 

        LOG_DEBUG( "Client disconnected - could not send " << replyMsg->GetDeviceType() << " message to client (device name: " << replyMsg->GetDeviceName()
          << "  Timestamp: " << std::fixed <<  ts->GetTimeStamp() << ").");
      }
    } // clientIterator
  } // unlock client list

  // Clean up disconnected clients
  for (std::vector< int >::iterator it=disconnectedClientIds.begin(); it!=disconnectedClientIds.end(); ++it)
  {
    DisconnectClient(*it);
  }
}

//------------------------------------------------------------------------------
int vtkPlusOpenIGTLinkServer::GetNumberOfConnectedClients()
{
  // Lock before we send message to the clients 
  PlusLockGuard<vtkRecursiveCriticalSection> igtlClientsMutexGuardedLock(this->IgtlClientsMutex);
  return this->IgtlClients.size(); 
}

//------------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkServer::ReadConfiguration(vtkXMLDataElement* serverElement, const char* aFilename)
{
  LOG_TRACE("vtkPlusOpenIGTLinkServer::ReadConfiguration");

  if( aFilename == NULL )
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
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(SendValidTransformsOnly, serverElement);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(IgtlMessageCrcCheckEnabled, serverElement);

  this->DefaultClientInfo.IgtlMessageTypes.clear();
  this->DefaultClientInfo.TransformNames.clear();
  this->DefaultClientInfo.ImageStreams.clear();
  this->DefaultClientInfo.StringNames.clear();

  vtkXMLDataElement* defaultClientInfo = serverElement->FindNestedElementWithName("DefaultClientInfo"); 
  if ( defaultClientInfo != NULL )
  {
    if (this->DefaultClientInfo.SetClientInfoFromXmlData(defaultClientInfo)!=PLUS_SUCCESS)
    {
      return PLUS_FAIL;
    }
  }

  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
int vtkPlusOpenIGTLinkServer::ProcessPendingCommands()
{
  return this->PlusCommandProcessor->ExecuteCommands();
}

//------------------------------------------------------------------------------
vtkDataCollector* vtkPlusOpenIGTLinkServer::GetDataCollector()
{
  return this->DataCollector;
}

//------------------------------------------------------------------------------
vtkTransformRepository* vtkPlusOpenIGTLinkServer::GetTransformRepository()
{
  return this->TransformRepository;
}

//------------------------------------------------------------------------------
bool vtkPlusOpenIGTLinkServer::HasGracePeriodExpired()
{
  return (vtkAccurateTimer::GetSystemTime() - this->BroadcastStartTime) > this->MissingInputGracePeriodSec;
}

//------------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkServer::Start(vtkDataCollector* dataCollector, vtkTransformRepository* transformRepository, vtkXMLDataElement* serverElement, const std::string& configFilePath)
{
  if( serverElement == NULL )
  {
    LOG_ERROR("NULL configuration sent to vtkPlusOpenIGTLinkServer::Start. Unable to start PlusServer.");
    return PLUS_FAIL;
  }

  this->SetDataCollector( dataCollector );
  if ( this->ReadConfiguration(serverElement, configFilePath.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read PlusOpenIGTLinkServer configuration"); 
    return PLUS_FAIL;
  }

  this->SetTransformRepository( transformRepository ); 
  if ( this->StartOpenIGTLinkService() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to start Plus OpenIGTLink server"); 
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkServer::Stop()
{
  PlusStatus status=PLUS_SUCCESS;

  if (StopOpenIGTLinkService()!=PLUS_SUCCESS)
  {
    status=PLUS_FAIL;
  }

  SetDataCollector(NULL);

  SetTransformRepository(NULL);

  return status;
}

//------------------------------------------------------------------------------
igtl::MessageBase::Pointer vtkPlusOpenIGTLinkServer::CreateIgtlMessageFromCommandResponse(vtkPlusCommandResponse* response)
{
  vtkPlusCommandStringResponse* stringResponse=vtkPlusCommandStringResponse::SafeDownCast(response);
  if (stringResponse)
  {
    igtl::StringMessage::Pointer igtlMessage = igtl::StringMessage::New();
    igtlMessage->SetDeviceName(stringResponse->GetDeviceName().c_str());
    if (stringResponse->GetDeviceName().empty())
    {
      LOG_WARNING("OpenIGTLink STRING message device name is empty");
    }
    std::ostringstream replyStr;
    replyStr << "<CommandReply";
    replyStr << " Status=\"" << (stringResponse->GetStatus() == PLUS_SUCCESS ? "SUCCESS" : "FAIL") << "\"";
    replyStr << " Message=\"";
    // Write to XML, encoding special characters, such as " ' \ < > &
    vtkXMLUtilities::EncodeString(stringResponse->GetMessage().c_str(), VTK_ENCODING_NONE, replyStr, VTK_ENCODING_NONE, 1 /* encode special characters */ );
    replyStr << "\"";
    replyStr << " />";

    igtlMessage->SetString(replyStr.str().c_str());
    LOG_DEBUG("Command response: "<<replyStr.str());
    return igtlMessage.GetPointer();
  }

  vtkPlusCommandImageResponse* imageResponse=vtkPlusCommandImageResponse::SafeDownCast(response);
  if (imageResponse)
  {
    std::string imageName=imageResponse->GetImageName();    
    if (imageName.empty())
    {
      imageName="PlusServerImage";
    }

    vtkSmartPointer<vtkMatrix4x4> imageToReferenceTransform=vtkSmartPointer<vtkMatrix4x4>::New();
    if (imageResponse->GetImageToReferenceTransform()!=NULL)
    {
      imageToReferenceTransform=imageResponse->GetImageToReferenceTransform();
    }

    vtkImageData* imageData=imageResponse->GetImageData();
    if (imageData==NULL)
    {
      LOG_ERROR("Invalid image data in command response");
      return NULL;
    }

    igtl::ImageMessage::Pointer igtlMessage = igtl::ImageMessage::New();
    igtlMessage->SetDeviceName(imageName.c_str());  
    
    if ( vtkPlusIgtlMessageCommon::PackImageMessage(igtlMessage, imageData, 
      imageToReferenceTransform, vtkAccurateTimer::GetSystemTime()) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to create image mesage from command response");
      return NULL;
    }
    return igtlMessage.GetPointer();
  }
  vtkPlusCommandImageMetaDataResponse* imageMetaDataResponse=vtkPlusCommandImageMetaDataResponse::SafeDownCast(response);
  if (imageMetaDataResponse)
  {
    std::string imageMetaDataName="PlusServerImageMetaData";
    PlusCommon::ImageMetaDataList imageMetaDataList;
    imageMetaDataResponse->GetImageMetaDataItems(imageMetaDataList);
    igtl::ImageMetaMessage::Pointer igtlMessage = igtl::ImageMetaMessage::New();
    igtlMessage->SetDeviceName(imageMetaDataName.c_str());                  
    if ( vtkPlusIgtlMessageCommon::PackImageMetaMessage(igtlMessage,imageMetaDataList) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to create image mesage from command response");
      return NULL;
    }
    return igtlMessage.GetPointer();
  }  

  LOG_ERROR("vtkPlusOpenIGTLinkServer::CreateIgtlMessageFromCommandResponse failed: invalid command response");
  return NULL;
}
