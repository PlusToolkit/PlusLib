/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "vtkPlusOpenIGTLinkServer.h"
#include "vtkDataCollector.h"
#include "vtkTransformRepository.h" 
#include "TrackedFrame.h"
#include "vtkObjectFactory.h"
#include "vtkTrackedFrameList.h"

#include "vtkMutexLock.h"

#include "igtlImageMessage.h"
#include "igtlMessageHeader.h"
#include "igtlPlusClientInfoMessage.h"
#include "igtlStatusMessage.h"

#include "vtkPlusIgtlMessageFactory.h" 

static const double DELAY_ON_SENDING_ERROR_SEC = 0.02; 
static const double DELAY_ON_NO_NEW_FRAMES_SEC = 0.005; 

vtkCxxRevisionMacro( vtkPlusOpenIGTLinkServer, "$Revision: 1.0 $" );
vtkStandardNewMacro( vtkPlusOpenIGTLinkServer ); 

vtkCxxSetObjectMacro(vtkPlusOpenIGTLinkServer, TransformRepository, vtkTransformRepository);
vtkCxxSetObjectMacro(vtkPlusOpenIGTLinkServer, DataCollector, vtkDataCollector);

//----------------------------------------------------------------------------
vtkPlusOpenIGTLinkServer::vtkPlusOpenIGTLinkServer()
{
  this->ListeningPort = -1;
  this->LastSentTrackedFrameTimestamp = 0; 
  this->NumberOfRetryAttempts = 3; 

  this->MaxTimeSpentWithProcessingMs = 100; 
  this->LastProcessingTimePerFrameMs = -1;

  this->ConnectionReceiverThreadId = -1;
  this->DataSenderThreadId = -1; 
  this->DataReceiverThreadId = -1; 

  this->ConnectionActive = std::make_pair(false,false); 
  this->DataSenderActive = std::make_pair(false,false);
  this->DataReceiverActive = std::make_pair(false,false);
  
  this->DataCollector = NULL;
  this->TransformRepository = NULL; 
  
  this->Threader = vtkSmartPointer<vtkMultiThreader>::New();
  this->Mutex = vtkSmartPointer<vtkMutexLock>::New();
  
  this->ServerSocket = igtl::ServerSocket::New();
}

//----------------------------------------------------------------------------
vtkPlusOpenIGTLinkServer::~vtkPlusOpenIGTLinkServer()
{
  this->Stop();
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

  if ( this->DefaultImageTransformName.IsValid() )
  {
    std::string tn; 
    this->DefaultImageTransformName.GetTransformName(tn);
    LOG_INFO("Server default image transform name to send: " << tn ); 
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkServer::Stop()
{
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
    igtl::ClientSocket::Pointer newClientSocket = self->ServerSocket->WaitForConnection( 500 );
    if (newClientSocket.IsNotNull())
    {
      // Lock before we change the clients list 
      PlusLockGuard<vtkMutexLock> updateMutexGuardedLock(self->Mutex);

      newClientSocket->SetTimeout( 500 ); // Needs OpenIGTLink revision 7701 (trunk) or later.

      PlusIgtlClientInfo client; 
      client.ClientSocket = newClientSocket;

      self->IgtlClients.push_back(client); 

      int port = -1; 
      std::string address; 
#if (PLUS_OPENIGTLINK_VERSION_MAJOR > 1) || ( PLUS_OPENIGTLINK_VERSION_MAJOR == 1 && PLUS_OPENIGTLINK_VERSION_MINOR > 9 ) || ( PLUS_OPENIGTLINK_VERSION_MAJOR == 1 && PLUS_OPENIGTLINK_VERSION_MINOR == 9 && PLUS_OPENIGTLINK_VERSION_PATCH > 4 )
      client.ClientSocket->GetSocketAddressAndPort(address, port);
#endif
      LOG_INFO( "Server received new client connection (" << address << ":" << port << ")." );
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
  
  self->DataCollector->GetMostRecentTimestamp(self->LastSentTrackedFrameTimestamp);

  std::list<std::string>::iterator messageTypeIterator; 
  while ( self->ConnectionActive.first && self->DataSenderActive.first )
  {
    if ( self->IgtlClients.empty() )
    {
      // No client connected, wait for a while 
      vtkAccurateTimer::Delay(0.2);
      continue; 
    }

    vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
    double startTimeSec = vtkAccurateTimer::GetSystemTime();
    
    // Acquire tracked frames since last acquisition (minimum 1 frame)
    if (self->LastProcessingTimePerFrameMs<1)
    {
      // if processing was less than 1ms/frame then assume it was 1ms (1000FPS processing speed) to avoid division by zero
      self->LastProcessingTimePerFrameMs=1;
    }
    int numberOfFramesToGet = std::max(self->MaxTimeSpentWithProcessingMs / self->LastProcessingTimePerFrameMs, 1); 
    
    if ( self->DataCollector->GetTrackedFrameList(self->LastSentTrackedFrameTimestamp, trackedFrameList, numberOfFramesToGet) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get tracked frame list from data collector (last recorded timestamp: " << std::fixed << self->LastSentTrackedFrameTimestamp ); 
      vtkAccurateTimer::Delay(DELAY_ON_SENDING_ERROR_SEC); 
      continue; 
    }

    // There is no new frame in the buffer
    if ( trackedFrameList->GetNumberOfTrackedFrames() == 0 )
    {
      vtkAccurateTimer::Delay(DELAY_ON_NO_NEW_FRAMES_SEC); 
      continue;
    }
   
    for ( int i = 0; i < trackedFrameList->GetNumberOfTrackedFrames(); ++i )
    {
      // Send tracked frame
      self->SendTrackedFrame( *trackedFrameList->GetTrackedFrame(i) ); 
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
      PlusLockGuard<vtkMutexLock> updateMutexGuardedLock(self->Mutex);
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

      headerMsg->Unpack();
      if (strcmp(headerMsg->GetDeviceType(), "CLIENTINFO") == 0)
      {
        igtl::PlusClientInfoMessage::Pointer clientInfoMsg = igtl::PlusClientInfoMessage::New(); 
        clientInfoMsg->SetMessageHeader(headerMsg); 
        clientInfoMsg->AllocatePack(); 

        client.ClientSocket->Receive(clientInfoMsg->GetPackBodyPointer(), clientInfoMsg->GetPackBodySize() ); 
        //  If 1 is specified it performs CRC check and unpack the data only if CRC passes
        int c = clientInfoMsg->Unpack(1);
        if (c & igtl::MessageHeader::UNPACK_BODY) 
        {
          int port = -1; 
          std::string clientAddress; 
#if (PLUS_OPENIGTLINK_VERSION_MAJOR > 1) || ( PLUS_OPENIGTLINK_VERSION_MAJOR == 1 && PLUS_OPENIGTLINK_VERSION_MINOR > 9 ) || ( PLUS_OPENIGTLINK_VERSION_MAJOR == 1 && PLUS_OPENIGTLINK_VERSION_MINOR == 9 && PLUS_OPENIGTLINK_VERSION_PATCH > 4 )
          client.ClientSocket->GetSocketAddressAndPort(clientAddress, port);
#endif
          // Message received from client, need to lock to modify client info
          PlusLockGuard<vtkMutexLock> updateMutexGuardedLock(self->Mutex);
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

        igtl::StatusMessage::Pointer statusMsg = igtl::StatusMessage::New(); 
        statusMsg->SetCode(igtl::StatusMessage::STATUS_OK); 
        statusMsg->Pack(); 
        client.ClientSocket->Send(statusMsg->GetPackPointer(), statusMsg->GetPackBodySize()); 
      }
      else
      {
        // if the device type is unknown, skip reading. 
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

  // Lock before we send message to the clients 
  PlusLockGuard<vtkMutexLock> updateMutexGuardedLock(this->Mutex);
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

    // Set image transform name
    PlusTransformName imageTransformName = this->DefaultImageTransformName; 
    if ( client.ImageTransformName.IsValid() )
    {
      imageTransformName = client.ImageTransformName; 
    }

    vtkSmartPointer<vtkPlusIgtlMessageFactory> igtlMessageFactory = vtkSmartPointer<vtkPlusIgtlMessageFactory>::New(); 
    if ( igtlMessageFactory->PackMessages( messageTypes, igtlMessages, trackedFrame, transformNames, imageTransformName, this->TransformRepository ) != PLUS_SUCCESS )
    {
      LOG_WARNING("Failed to pack all IGT messages!"); 
    }

    // Send all messages to a client 
    for (  igtlMessageIterator = igtlMessages.begin(); igtlMessageIterator != igtlMessages.end(); ++igtlMessageIterator )
    {
      igtl::MessageBase::Pointer igtlMessage = (*igtlMessageIterator); 
      if ( igtlMessage.IsNull() )
      {
        continue; 
      }

      int retValue = 0, numOfTries = 0; 
      while ( retValue == 0 && numOfTries < this->NumberOfRetryAttempts )
      {
        retValue = client.ClientSocket->Send( igtlMessage->GetPackPointer(), igtlMessage->GetPackSize() ); 
        numOfTries++; 
      }

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
      clientDisconnected = false; 
      continue; 
    }

    // Send messages to the next client 
    ++clientIterator; 

  } // clientIterator

  return ( numberOfErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL );
}

//------------------------------------------------------------------------------
int vtkPlusOpenIGTLinkServer::GetNumberOfConnectedClients()
{
  // Lock before we send message to the clients 
  PlusLockGuard<vtkMutexLock> updateMutexGuardedLock(this->Mutex);
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

    // Get image transform name
    vtkXMLDataElement* imageTransform = defaultClientInfo->FindNestedElementWithName("ImageTransform"); 
    if ( imageTransform != NULL )
    {
      const char* name = imageTransform->GetAttribute("Name"); 
      if ( name != NULL )
      {
        PlusTransformName tName; 
        if ( tName.SetTransformName(name) != PLUS_SUCCESS )
        {
          LOG_WARNING( "Invalid transform name: " << name ); 
        }
        else
        {
          this->DefaultImageTransformName = tName; 
        }
      }
    }

  }

  return PLUS_SUCCESS;
}

