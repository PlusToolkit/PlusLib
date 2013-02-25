/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "TrackedFrame.h"
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

static const double DELAY_ON_SENDING_ERROR_SEC = 0.02; 
static const double DELAY_ON_NO_NEW_FRAMES_SEC = 0.005; 
static const int CLIENT_SOCKET_TIMEOUT_MSEC = 500; 

vtkCxxRevisionMacro( vtkPlusOpenIGTLinkServer, "$Revision: 1.0 $" );
vtkStandardNewMacro( vtkPlusOpenIGTLinkServer ); 

vtkCxxSetObjectMacro(vtkPlusOpenIGTLinkServer, TransformRepository, vtkTransformRepository);
vtkCxxSetObjectMacro(vtkPlusOpenIGTLinkServer, DataCollector, vtkDataCollector);

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
, DataReceiverThreadId(-1)
, ConnectionActive(std::make_pair(false,false))
, DataSenderActive(std::make_pair(false,false))
, DataReceiverActive(std::make_pair(false,false))
, DataCollector(NULL)
, TransformRepository(NULL)
, Threader(vtkSmartPointer<vtkMultiThreader>::New())
, Mutex(vtkSmartPointer<vtkRecursiveCriticalSection>::New())
, ServerSocket(igtl::ServerSocket::New())
, SendValidTransformsOnly(true)
, IgtlMessageCrcCheckEnabled(0)
, PlusCommandProcessor(vtkSmartPointer<vtkPlusCommandProcessor>::New())
, OutputDeviceId(NULL)
, OutputChannelId(NULL)
, BroadcastChannel(NULL)
{
  
}

//----------------------------------------------------------------------------
vtkPlusOpenIGTLinkServer::~vtkPlusOpenIGTLinkServer()
{
  this->Stop();
  SetTransformRepository(NULL); // remove reference to prevent memory leaks
  SetDataCollector(NULL); // remove reference to prevent memory leaks
}

//----------------------------------------------------------------------------
void vtkPlusOpenIGTLinkServer::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkServer::Start()
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

  if ( this->DataReceiverThreadId < 0 )
  {
    this->DataReceiverActive.first = true; 
    this->DataReceiverThreadId = this->Threader->SpawnThread( (vtkThreadFunctionType)&DataReceiverThread, this );
  }

  if ( !this->DefaultIgtlMessageTypes.empty() )
  {
    std::ostringstream messageTypes;
    for ( int i = 0; i < this->DefaultIgtlMessageTypes.size(); ++i )
    {
      messageTypes << this->DefaultIgtlMessageTypes[i] << " "; 
    }
    LOG_INFO("Server default message types to send: " << messageTypes.str() ); 
  }

  if ( !this->DefaultTransformNames.empty() )
  {
    std::ostringstream transformNames;
    for ( int i = 0; i < this->DefaultTransformNames.size(); ++i )
    {
      std::string tn; 
      this->DefaultTransformNames[i].GetTransformName(tn); 
      transformNames << tn << " "; 
    }
    LOG_INFO("Server default transform names to send: " << transformNames.str() ); 
  }

  if ( !this->DefaultImageStreams.empty() )
  {
    std::ostringstream imageNames;
    for ( int i = 0; i < this->DefaultImageStreams.size(); ++i )
    {
      imageNames << this->DefaultImageStreams[i].Name << " (EmbeddedTransformToFrame: " << this->DefaultImageStreams[i].EmbeddedTransformToFrame << ") "; 
    }
    LOG_INFO("Server default images to send: " << imageNames.str() ); 
  }

  this->PlusCommandProcessor->SetPlusServer(this);
  //this->PlusCommandProcessor->Start();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkServer::Stop()
{
  
  /*
  // Stop command processor thread 
  if ( this->PlusCommandProcessor->IsRunning() )
  {
    this->PlusCommandProcessor->Stop();
    while ( this->PlusCommandProcessor->IsRunning() )
    {
      // Wait until the thread stops 
      vtkAccurateTimer::Delay( 0.2 ); 
    }
  }
  */

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

  // Stop data sender thread 
  if ( this->DataSenderThreadId >= 0 )
  {
    this->DataSenderActive.first = false; 
    while ( this->DataSenderActive.second )
    {
      // Wait until the thread stops 
      vtkAccurateTimer::Delay( 0.2 ); 
    } 
    this->DataSenderThreadId = -1;
  }

  // Stop connection receiver thread
  if ( this->ConnectionReceiverThreadId >= 0 )
  {
    this->ConnectionActive.first = false;
    while ( this->ConnectionActive.second )
    {
      // Wait until the thread stops 
      vtkAccurateTimer::Delay( 0.2 ); 
    }
    this->ConnectionReceiverThreadId = -1;
    LOG_INFO( "Plus OpenIGTLink server stopped."); 
  }

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
  else
  {
    self->ConnectionActive.second = true; 
  }

  // Wait for connections until we want to stop the thread
  while ( self->ConnectionActive.first )
  {
    igtl::ClientSocket::Pointer newClientSocket = self->ServerSocket->WaitForConnection( CLIENT_SOCKET_TIMEOUT_MSEC );
    if (newClientSocket.IsNotNull())
    {
      // Lock before we change the clients list 
      PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(self->Mutex);

      newClientSocket->SetTimeout( CLIENT_SOCKET_TIMEOUT_MSEC ); 

      PlusIgtlClientInfo client; 
      client.ClientSocket = newClientSocket;

      self->IgtlClients.push_back(client); 

      int port = -1; 
      std::string address; 
#if (PLUS_OPENIGTLINK_VERSION_MAJOR > 1) || ( PLUS_OPENIGTLINK_VERSION_MAJOR == 1 && PLUS_OPENIGTLINK_VERSION_MINOR > 9 ) || ( PLUS_OPENIGTLINK_VERSION_MAJOR == 1 && PLUS_OPENIGTLINK_VERSION_MINOR == 9 && PLUS_OPENIGTLINK_VERSION_PATCH > 4 )
      client.ClientSocket->GetSocketAddressAndPort(address, port);
#endif
      LOG_INFO( "Server received new client connection (" << address << ":" << port << ")." );
      LOG_INFO( "Number of connected clients: " << self->GetNumberOfConnectedClients() ); 
    }
  }

  // Close client sockets 
  std::list<PlusIgtlClientInfo>::iterator clientIterator; 
  for ( clientIterator = self->IgtlClients.begin(); clientIterator != self->IgtlClients.end(); ++clientIterator)
  {
    if ( (*clientIterator).ClientSocket.IsNotNull() )
    {
      (*clientIterator).ClientSocket->CloseSocket(); 
    }
  }
  self->IgtlClients.clear(); 

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
  if( self->GetOutputDeviceId() == NULL || self->DataCollector->GetDevice(aDevice, std::string(self->GetOutputDeviceId())) != PLUS_SUCCESS )
  {
    DeviceCollection aCollection;
    if( self->DataCollector->GetDevices(aCollection) != PLUS_SUCCESS || aCollection.size() == 0 )
    {
      LOG_ERROR("Unable to retrieve device by ID: " << (self->GetOutputDeviceId() == NULL ? "MISSINGID" : self->GetOutputDeviceId()) << " and no devices to fall back on." );
      return NULL;
    }
    aDevice = aCollection[0];
  }
  if( self->GetOutputChannelId() == NULL || aDevice->GetOutputChannelByName(self->BroadcastChannel, self->GetOutputChannelId()) != PLUS_SUCCESS )
  {
    if( aDevice->OutputChannelCount() == 0 )
    {
      LOG_ERROR("Unable to retrieve channel by ID: " << (self->GetOutputChannelId() == NULL ? "MISSINGID" : self->GetOutputChannelId()) << " and no channels to fall back on." );
      return NULL;
    }
    self->BroadcastChannel = *(aDevice->GetOutputChannelsStart());
  }
  self->BroadcastChannel->GetMostRecentTimestamp(self->LastSentTrackedFrameTimestamp);

  std::list<std::string>::iterator messageTypeIterator; 
  double elapsedTimeSinceLastPacketSentSec = 0; 
  while ( self->ConnectionActive.first && self->DataSenderActive.first )
  {
    if ( self->IgtlClients.empty() )
    {
      // No client connected, wait for a while 
      vtkAccurateTimer::Delay(0.2);
      self->LastSentTrackedFrameTimestamp=0; // next time start sending from the most recent timestamp
      continue; 
    }

    // Send remote command execution replies to clients
    PlusCommandReplyList replies;
    self->PlusCommandProcessor->GetCommandReplies(replies);
    if (!replies.empty())
    {
      PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(self->Mutex);

      // Create a reply message (as a STATUS message)
      for (PlusCommandReplyList::iterator replyIt=replies.begin(); replyIt!=replies.end(); replyIt++)
      {        
        igtl::ClientSocket::Pointer clientSocket=self->GetClientSocket(replyIt->ClientId);

        // Send image message (optional)
        if (replyIt->ImageData!=NULL)
        {
          // TODO: now all images are broadcast to all clients, it should be more configurable (the command should be able
          // to specify if the image should be sent to the requesting client or all of them)

          std::string imageName="PlusServerImage";
          if (!replyIt->ImageName.empty())
          {
            imageName=replyIt->ImageName;
          }

          vtkSmartPointer<vtkMatrix4x4> imageToReferenceTransform=vtkSmartPointer<vtkMatrix4x4>::New();
          if (replyIt->ImageToReferenceTransform!=NULL)
          {
            imageToReferenceTransform=vtkSmartPointer<vtkMatrix4x4>::Take(replyIt->ImageToReferenceTransform);
            replyIt->ImageToReferenceTransform=NULL;
          }

          igtl::ImageMessage::Pointer imageMsg = igtl::ImageMessage::New();
          imageMsg->SetDeviceName(imageName.c_str());                  
          if ( vtkPlusIgtlMessageCommon::PackImageMessage(imageMsg, replyIt->ImageData, 
            imageToReferenceTransform, vtkAccurateTimer::GetSystemTime()) != PLUS_SUCCESS )
          {
            LOG_ERROR("Failed to pack image into reply to client"); 
          }
          else
          {       
            // broadcast image result
            std::list<PlusIgtlClientInfo>::iterator clientIterator; 
            for ( clientIterator = self->IgtlClients.begin(); clientIterator != self->IgtlClients.end(); ++clientIterator)
            {
              if (clientIterator->ClientSocket.IsNull())
              {
                LOG_WARNING("Message reply cannot be sent to client, probably client has been disconnected");
                continue;
              }
              clientIterator->ClientSocket->Send(imageMsg->GetPackPointer(), imageMsg->GetPackSize());
            }            
          }
          replyIt->ImageData->UnRegister(NULL);
          replyIt->ImageData=NULL;
        }        

        if (clientSocket.IsNull())
        {
          LOG_WARNING("Message reply cannot be sent to client, probably client has been disconnected");
          continue;
        }

        // Send command reply
        igtl::StringMessage::Pointer replyMsg = igtl::StringMessage::New();
        replyMsg->SetDeviceName(replyIt->DeviceName.c_str()); 
        // TODO: send the replyIt->Status as well?
        replyMsg->SetString(replyIt->ReplyString.c_str());
        replyMsg->Pack(); 
        clientSocket->Send(replyMsg->GetPackPointer(), replyMsg->GetPackSize());

      }
    }

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

    if ( self->BroadcastChannel->GetTrackedFrameList(self->LastSentTrackedFrameTimestamp, trackedFrameList, numberOfFramesToGet) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get tracked frame list from data collector (last recorded timestamp: " << std::fixed << self->LastSentTrackedFrameTimestamp ); 
      self->KeepAlive(); 
      elapsedTimeSinceLastPacketSentSec = 0; 
      self->LastSentTrackedFrameTimestamp = 0; // next time start from the most recent timestamp
      vtkAccurateTimer::Delay(DELAY_ON_SENDING_ERROR_SEC); 
      continue; 
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
  vtkPlusOpenIGTLinkServer* self = (vtkPlusOpenIGTLinkServer*)( data->UserData );
  self->DataReceiverActive.second = true; 

  std::list<PlusIgtlClientInfo>::iterator clientIterator; 
  std::list<PlusIgtlClientInfo> igtlClients; 
  while ( self->ConnectionActive.first && self->DataReceiverActive.first )
  {
    // make a copy of client infos to avoid lock 
    {
      PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(self->Mutex);
      igtlClients = self->IgtlClients; 
    }

    if ( igtlClients.empty() )
    {
      // No client connected, wait for a while 
      vtkAccurateTimer::Delay(0.2);
      continue; 
    }

    for ( clientIterator = igtlClients.begin(); clientIterator != igtlClients.end(); ++clientIterator)
    {
      PlusIgtlClientInfo client = (*clientIterator); 
      igtl::MessageHeader::Pointer headerMsg;
      headerMsg = igtl::MessageHeader::New();
      headerMsg->InitPack();

      // Receive generic header from the socket
      int numOfBytesReceived = client.ClientSocket->Receive( headerMsg->GetPackPointer(), headerMsg->GetPackSize() );
      if ( numOfBytesReceived == 0  // No message received
        || numOfBytesReceived != headerMsg->GetPackSize() // Received data is not as we expected
        )
      {
        continue; 
      }

      headerMsg->Unpack(self->IgtlMessageCrcCheckEnabled);
      if (strcmp(headerMsg->GetDeviceType(), "CLIENTINFO") == 0)
      {
        igtl::PlusClientInfoMessage::Pointer clientInfoMsg = igtl::PlusClientInfoMessage::New(); 
        clientInfoMsg->SetMessageHeader(headerMsg); 
        clientInfoMsg->AllocatePack(); 

        client.ClientSocket->Receive(clientInfoMsg->GetPackBodyPointer(), clientInfoMsg->GetPackBodySize() ); 

        int c = clientInfoMsg->Unpack(self->IgtlMessageCrcCheckEnabled);
        if (c & igtl::MessageHeader::UNPACK_BODY) 
        {
          int port = -1; 
          std::string clientAddress; 
#if (PLUS_OPENIGTLINK_VERSION_MAJOR > 1) || ( PLUS_OPENIGTLINK_VERSION_MAJOR == 1 && PLUS_OPENIGTLINK_VERSION_MINOR > 9 ) || ( PLUS_OPENIGTLINK_VERSION_MAJOR == 1 && PLUS_OPENIGTLINK_VERSION_MINOR == 9 && PLUS_OPENIGTLINK_VERSION_PATCH > 4 )
          client.ClientSocket->GetSocketAddressAndPort(clientAddress, port);
#endif
          // Message received from client, need to lock to modify client info
          PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(self->Mutex);
          std::list<PlusIgtlClientInfo>::iterator it = std::find(self->IgtlClients.begin(), self->IgtlClients.end(), client ); 
          if ( it != self->IgtlClients.end() )
          {
            // Copy client info
            (*it).ShallowCopy(clientInfoMsg->GetClientInfo()); 
            LOG_INFO("Message received from client (" << clientAddress << ":" << port << ")."); 
          }
        }
      }
      else if (strcmp(headerMsg->GetDeviceType(), "GET_STATUS") == 0)
      {
        // Just ping server, we can skip message and respond
        client.ClientSocket->Skip(headerMsg->GetBodySizeToRead(), 0);

        igtl::StatusMessage::Pointer replyMsg = igtl::StatusMessage::New(); 
        replyMsg->SetCode(igtl::StatusMessage::STATUS_OK); 
        replyMsg->Pack(); 
        client.ClientSocket->Send(replyMsg->GetPackPointer(), replyMsg->GetPackBodySize()); 
      }
      else if ( (strcmp(headerMsg->GetDeviceType(), "STRING") == 0) )
      {
        // Received a remote command execution message
        // The command is encoded in in an XML string in a STRING message body

        igtl::StringMessage::Pointer commandMsg = igtl::StringMessage::New(); 
        commandMsg->SetMessageHeader(headerMsg); 
        commandMsg->AllocatePack(); 
        client.ClientSocket->Receive(commandMsg->GetPackBodyPointer(), commandMsg->GetPackBodySize() ); 
      
        int c = commandMsg->Unpack(self->IgtlMessageCrcCheckEnabled);
        if (c & igtl::MessageHeader::UNPACK_BODY) 
        {          
          const char* deviceName="UNKNOWN";
          if (headerMsg->GetDeviceName()!=NULL)
          {
            deviceName=headerMsg->GetDeviceName();
          }
          else
          {
            LOG_ERROR("Received message from unknown device");
          }
          LOG_INFO("Received command from device "<<deviceName<<": "<<commandMsg->GetString());

          self->PlusCommandProcessor->QueueCommand(client.ClientId, commandMsg->GetString(), deviceName);
        }
        else
        {
          LOG_ERROR("STRING message unpacking failed");
        }        
      }
      else
      {
        // if the device type is unknown, skip reading. 
        LOG_WARNING("Unknown OpenIGTLink message is received. Device type: "<<headerMsg->GetDeviceType()<<". Device name: "<<headerMsg->GetDeviceName()<<".");
        client.ClientSocket->Skip(headerMsg->GetBodySizeToRead(), 0);
        continue; 
      }

    } // clientIterator

  } // ConnectionActive

  // Close thread
  self->DataReceiverThreadId = -1;
  self->DataReceiverActive.second = false; 
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
      LOG_ERROR("Failed to set current transforms to transform repository!"); 
      numberOfErrors++;
    }
  }

  // Convert relative timestamp to UTC
  double timestampSystem = trackedFrame.GetTimestamp(); // save original timestamp, we'll restore it later
  double timestampUniversal = vtkAccurateTimer::GetUniversalTimeFromSystemTime(timestampSystem);
  trackedFrame.SetTimestamp(timestampUniversal);  

  // Lock before we send message to the clients 
  PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);
  bool clientDisconnected = false;

  std::list<PlusIgtlClientInfo>::iterator clientIterator = this->IgtlClients.begin();
  while ( clientIterator != this->IgtlClients.end() )
  {
    PlusIgtlClientInfo client = (*clientIterator);

    // Create igt messages
    std::vector<igtl::MessageBase::Pointer> igtlMessages; 
    std::vector<igtl::MessageBase::Pointer>::iterator igtlMessageIterator; 

    // Set message types 
    std::vector<std::string> messageTypes = this->DefaultIgtlMessageTypes; 
    if ( !client.IgtlMessageTypes.empty() )
    {
      messageTypes = client.IgtlMessageTypes; 
    }

    // Set transform names 
    std::vector<PlusTransformName> transformNames = this->DefaultTransformNames; 
    if ( !client.TransformNames.empty() )
    {
      transformNames = client.TransformNames; 
    }

    // Set image transform names
    std::vector<PlusIgtlClientInfo::ImageStream> imageStreams = this->DefaultImageStreams; 
    if ( !client.ImageStreams.empty() )
    {
      imageStreams = client.ImageStreams; 
    }

    vtkSmartPointer<vtkPlusIgtlMessageFactory> igtlMessageFactory = vtkSmartPointer<vtkPlusIgtlMessageFactory>::New(); 
    if ( igtlMessageFactory->PackMessages( messageTypes, igtlMessages, trackedFrame, transformNames, imageStreams, this->SendValidTransformsOnly, this->TransformRepository ) != PLUS_SUCCESS )
    {
      LOG_WARNING("Failed to pack all IGT messages!"); 
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
        (retValue = client.ClientSocket->Send( igtlMessage->GetPackPointer(), igtlMessage->GetPackSize()))!=0,
        this->NumberOfRetryAttempts, this->DelayBetweenRetryAttemptsSec);
      if ( retValue == 0 )
      {
        clientDisconnected = true; 
        igtl::TimeStamp::Pointer ts = igtl::TimeStamp::New(); 
        igtlMessage->GetTimeStamp(ts); 

        LOG_DEBUG( "Client disconnected - could not send " << igtlMessage->GetDeviceType() << " message to client (device name: " << igtlMessage->GetDeviceName()
          << "  Timestamp: " << std::fixed <<  ts->GetTimeStamp() << ").");
        break; 
      }

    } // igtlMessageIterator

    if ( clientDisconnected )
    {
      int port = -1; 
      std::string address; 
#if (PLUS_OPENIGTLINK_VERSION_MAJOR > 1) || ( PLUS_OPENIGTLINK_VERSION_MAJOR == 1 && PLUS_OPENIGTLINK_VERSION_MINOR > 9 ) || ( PLUS_OPENIGTLINK_VERSION_MAJOR == 1 && PLUS_OPENIGTLINK_VERSION_MINOR == 9 && PLUS_OPENIGTLINK_VERSION_PATCH > 4 )
      client.ClientSocket->GetSocketAddressAndPort(address, port); 
#endif
      LOG_INFO( "Client disconnected (" <<  address << ":" << port << ")."); 
      clientIterator = this->IgtlClients.erase(clientIterator);
      LOG_INFO( "Number of connected clients: " << GetNumberOfConnectedClients() ); 
      clientDisconnected = false; 
      continue; 
    }

    // Send messages to the next client 
    ++clientIterator; 

  } // clientIterator

  // restore original timestamp
  trackedFrame.SetTimestamp(timestampSystem);

  return ( numberOfErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL );
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkServer::KeepAlive()
{
  int numberOfErrors = 0; 

  // Lock before we send message to the clients 
  PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);
  bool clientDisconnected = false;

  std::list<PlusIgtlClientInfo>::iterator clientIterator = this->IgtlClients.begin();
  while ( clientIterator != this->IgtlClients.end() )
  {
    PlusIgtlClientInfo client = (*clientIterator);

    igtl::StatusMessage::Pointer replyMsg = igtl::StatusMessage::New(); 
    replyMsg->SetCode(igtl::StatusMessage::STATUS_OK); 
    replyMsg->Pack(); 

    int retValue = 0;
    RETRY_UNTIL_TRUE( 
      (retValue = client.ClientSocket->Send( replyMsg->GetPackPointer(), replyMsg->GetPackSize() ))!=0,
      this->NumberOfRetryAttempts, this->DelayBetweenRetryAttemptsSec);
    bool clientDisconnected = false; 
    if ( retValue == 0 )
    {
      clientDisconnected = true; 
      igtl::TimeStamp::Pointer ts = igtl::TimeStamp::New(); 
      replyMsg->GetTimeStamp(ts); 

      LOG_DEBUG( "Client disconnected - could not send " << replyMsg->GetDeviceType() << " message to client (device name: " << replyMsg->GetDeviceName()
        << "  Timestamp: " << std::fixed <<  ts->GetTimeStamp() << ").");
    }

    if ( clientDisconnected )
    {
      int port = -1; 
      std::string address; 
#if (PLUS_OPENIGTLINK_VERSION_MAJOR > 1) || ( PLUS_OPENIGTLINK_VERSION_MAJOR == 1 && PLUS_OPENIGTLINK_VERSION_MINOR > 9 ) || ( PLUS_OPENIGTLINK_VERSION_MAJOR == 1 && PLUS_OPENIGTLINK_VERSION_MINOR == 9 && PLUS_OPENIGTLINK_VERSION_PATCH > 4 )
      client.ClientSocket->GetSocketAddressAndPort(address, port); 
#endif
      LOG_INFO( "Client disconnected (" <<  address << ":" << port << ")."); 
      clientIterator = this->IgtlClients.erase(clientIterator);
      LOG_INFO( "Number of connected clients: " << GetNumberOfConnectedClients() ); 
      clientDisconnected = false; 
      continue; 
    }

    // Send messages to the next client 
    ++clientIterator; 

  } // clientIterator

  LOG_DEBUG("Keep alive packet sent to clients..."); 
  return ( numberOfErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL );
}

//------------------------------------------------------------------------------
int vtkPlusOpenIGTLinkServer::GetNumberOfConnectedClients()
{
  // Lock before we send message to the clients 
  PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);
  return this->IgtlClients.size(); 
}

//------------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkServer::ReadConfiguration(vtkXMLDataElement* aConfigurationData)
{
  LOG_TRACE("vtkPlusOpenIGTLinkServer::ReadConfiguration");

  if ( aConfigurationData == NULL )
  {
    LOG_ERROR("Unable to configure Plus server! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  vtkXMLDataElement* plusOpenIGTLinkServerConfig = aConfigurationData->FindNestedElementWithName("PlusOpenIGTLinkServer");
  if (plusOpenIGTLinkServerConfig == NULL)
  {
    LOG_ERROR("Cannot find PlusOpenIGTLinkServer element in XML tree!");
    return PLUS_FAIL;
  }

  const char* outputDeviceId = plusOpenIGTLinkServerConfig->GetAttribute("OutputDeviceId");
  this->SetOutputDeviceId(outputDeviceId);

  const char* outputChannelId = plusOpenIGTLinkServerConfig->GetAttribute("OutputChannelId");
  this->SetOutputChannelId(outputChannelId);

  int maxTimeSpentWithProcessingMs = 0;
  if ( plusOpenIGTLinkServerConfig->GetScalarAttribute("MaxTimeSpentWithProcessingMs", maxTimeSpentWithProcessingMs) ) 
  {
    this->MaxTimeSpentWithProcessingMs = maxTimeSpentWithProcessingMs; 
  }

  int maxNumberOfIgtlMessagesToSend = 0; 
  if ( plusOpenIGTLinkServerConfig->GetScalarAttribute("MaxNumberOfIgtlMessagesToSend", maxNumberOfIgtlMessagesToSend) ) 
  {
    this->MaxNumberOfIgtlMessagesToSend = maxNumberOfIgtlMessagesToSend; 
  }

  int listeningPort = -1; 
  if ( plusOpenIGTLinkServerConfig->GetScalarAttribute("ListeningPort", listeningPort) ) 
  {
    this->SetListeningPort(listeningPort); 
  }
  else
  {
    LOG_ERROR("Unable to find listening port for PlusOpenIGTLinkServer"); 
    return PLUS_FAIL; 
  }

  // Query configuration to determine transform sending behaviour
  const char * sendAttribute = plusOpenIGTLinkServerConfig->GetAttribute("SendValidTransformsOnly");
  if( sendAttribute != NULL )
  {
    this->SendValidTransformsOnly = STRCASECMP(sendAttribute, "true") == 0;
  }

  const char* igtlMessageCrcCheckEnabled = plusOpenIGTLinkServerConfig->GetAttribute("IgtlMessageCrcCheckEnabled"); 
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

  vtkXMLDataElement* defaultClientInfo = plusOpenIGTLinkServerConfig->FindNestedElementWithName("DefaultClientInfo"); 

  if ( defaultClientInfo != NULL )
  {
    // Get message types
    vtkXMLDataElement* messageTypes = defaultClientInfo->FindNestedElementWithName("MessageTypes"); 
    if ( messageTypes != NULL )
    {
      for ( int i = 0; i < messageTypes->GetNumberOfNestedElements(); ++i )
      {
        const char* name = messageTypes->GetNestedElement(i)->GetName(); 
        if ( name == NULL || STRCASECMP( name, "Message") != 0 )
        {
          continue; 
        }
        const char* type = messageTypes->GetNestedElement(i)->GetAttribute("Type"); 
        if ( type != NULL )
        {
          this->DefaultIgtlMessageTypes.push_back(type); 
        }
      } 
    }

    // Get transform names
    vtkXMLDataElement* transformNames = defaultClientInfo->FindNestedElementWithName("TransformNames"); 
    if ( transformNames != NULL )
    {
      for ( int i = 0; i < transformNames->GetNumberOfNestedElements(); ++i )
      {
        const char* transform = transformNames->GetNestedElement(i)->GetName(); 
        if ( transform == NULL || STRCASECMP( transform, "Transform") != 0 )
        {
          continue; 
        }
        const char* name = transformNames->GetNestedElement(i)->GetAttribute("Name"); 
        if ( name != NULL )
        {
          PlusTransformName tName; 
          if ( tName.SetTransformName(name) != PLUS_SUCCESS )
          {
            LOG_WARNING( "Invalid transform name: " << name ); 
            continue; 
          }
          this->DefaultTransformNames.push_back(tName); 
        }
      } // transformNames
    }

    // Get image names
    vtkXMLDataElement* imageNames = defaultClientInfo->FindNestedElementWithName( "ImageNames" );
    if ( imageNames != NULL )
    {
      for ( int i = 0; i < imageNames->GetNumberOfNestedElements(); ++ i )
      {
        const char* image = imageNames->GetNestedElement( i )->GetName();
        if ( image == NULL  ||  STRCASECMP( image, "Image" ) != 0 )
        {
          continue;
        }

        const char* name = imageNames->GetNestedElement( i )->GetAttribute( "Name" );
        if ( name == NULL )
        {
          LOG_WARNING("Name attribute of ImageNames/Image element is missing. This element will be ignored.");
          continue;
        }

        const char* embeddedTransformToFrame = imageNames->GetNestedElement( i )->GetAttribute( "EmbeddedTransformToFrame" );
        if ( embeddedTransformToFrame == NULL )
        {
          LOG_WARNING("EmbeddedTransformToFrame attribute of ImageNames/Image element is missing. This element will be ignored.");
          continue;
        }

        PlusIgtlClientInfo::ImageStream imageStream; 
        imageStream.Name = name;
        imageStream.EmbeddedTransformToFrame = embeddedTransformToFrame; 
        this->DefaultImageStreams.push_back(imageStream);

      }
    }
  }

  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
igtl::ClientSocket::Pointer vtkPlusOpenIGTLinkServer::GetClientSocket(int clientId)
{
  // Close client sockets 
  std::list<PlusIgtlClientInfo>::iterator clientIterator; 
  for ( clientIterator = this->IgtlClients.begin(); clientIterator != this->IgtlClients.end(); ++clientIterator)
  {
    if (clientIterator->ClientId==clientId)
    {
      return clientIterator->ClientSocket;
    }
  }
  return NULL;
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
