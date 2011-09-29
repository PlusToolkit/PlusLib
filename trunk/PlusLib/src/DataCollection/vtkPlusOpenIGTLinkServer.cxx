
#include "vtkPlusOpenIGTLinkServer.h"

#include "vtkMultiThreader.h"
#include "vtkSmartPointer.h"

#include "igtlImageMessage.h"
#include "igtlMessageHeader.h"
#include "igtlOSUtil.h"
#include "igtlServerSocket.h"

#include "igtlStringMessage1.h"

#include "vtkPlusCommand.h"
#include "vtkPlusCommandFactory.h"



#define DELETE_IF_NOT_NULL( Object ) {\
  if ( Object != NULL ) {\
    Object->Delete();\
    Object = NULL;\
  }\
}\



vtkCxxRevisionMacro( vtkPlusOpenIGTLinkServer, "$Revision: 1.0 $" );
vtkStandardNewMacro( vtkPlusOpenIGTLinkServer ); 




void
vtkPlusOpenIGTLinkServer
::PrintSelf( ostream& os, vtkIndent indent )
{
	this->Superclass::PrintSelf( os, indent );
}



void
vtkPlusOpenIGTLinkServer
::SetDataCollector( vtkDataCollector* dataCollector )
{
  this->DataCollector = dataCollector;
}



int
vtkPlusOpenIGTLinkServer
::GetBufferedMessageCount()
{
  return this->MessageQueue->GetSize();
}



int
vtkPlusOpenIGTLinkServer
::Initialize( std::string &strError )
{
  if (    this->DataCollector == NULL
       || this->DataCollector->GetTracker() == NULL )
  {
    LOG_ERROR( "Tried to initialize vtkPlusOpenIGTLinkServer without DataCollector." );
    return 1;
  }
  
  return 0;
}



PlusStatus
vtkPlusOpenIGTLinkServer
::Start()
{
  if ( this->DataCollector == NULL )
  {
    LOG_WARNING( "Tried to start OpenIGTLink server without a vtkDataCollector" );
    return PLUS_FAIL;
  }
  
  if ( this->ThreadId >= 0 )
  {
    LOG_DEBUG( "Tried to start OpenIGTLink server, but it was already running." );
    return PLUS_SUCCESS;
  }
  
  this->Active = true;
  this->ThreadId = this->Threader->SpawnThread( (vtkThreadFunctionType)&vtkCommunicationThread, this );
  
  return PLUS_SUCCESS;
}



PlusStatus
vtkPlusOpenIGTLinkServer
::Stop()
{
  if ( this->ThreadId < 0 )
  {
    return PLUS_SUCCESS;
  }
  
  this->Active = false;
  LOG_INFO( "Server::Active set to false." );
  vtkAccurateTimer::Delay( 0.5 );  // TODO: Use waitable object to wait for thread to stop.
  this->ThreadId = -1;
  
  return PLUS_SUCCESS;
}



void*
vtkPlusOpenIGTLinkServer
::vtkCommunicationThread( vtkMultiThreader::ThreadInfo* data )
{
  vtkPlusOpenIGTLinkServer* self = (vtkPlusOpenIGTLinkServer*)( data->UserData );
  
  int r = self->ServerSocket->CreateServer( self->NetworkPort );
  
  if ( r < 0 )
  {
    std::cerr << "Cannot create a server socket." << std::endl;
    return NULL;
  }
  
  
  LOG_INFO( "Server thread started" );
  
  while ( self->GetActive() )
  {
    self->Mutex->Lock();
    self->WaitForConnection();
    self->Mutex->Unlock();
    
    if ( self->ClientSocket.IsNotNull() ) // If client connected.
    {
      LOG_INFO( "Server received client connection." );
      self->ReceiveController();
    }
  }
  
  if ( self->ClientSocket.IsNotNull() )
  {
    self->ClientSocket->CloseSocket();
  }
  
  if ( self->ServerSocket.IsNotNull() )
  {
    self->ServerSocket->CloseSocket();
  }
  
  self->ThreadId = -1;
  return NULL;
}



/**
 * Execute the next command in the command buffer, which is filled by the 
 * communication thread.
 */
void
vtkPlusOpenIGTLinkServer
::ExecuteNextCommand()
{
  igtl::MessageBase* nextMessage = this->MessageQueue->PullMessage();
  if ( nextMessage == NULL ) return;
  
  igtl::StringMessage1* nextStringMessage = (igtl::StringMessage1*)nextMessage;
  
  this->React( nextStringMessage->GetString() );
  // nextCommand
}



/**
 * Protected constructor.
 */
vtkPlusOpenIGTLinkServer
::vtkPlusOpenIGTLinkServer()
{
  this->NetworkPort = -1;
  this->ThreadId = -1;
  
  this->DataCollector = NULL;
  this->Threader = vtkMultiThreader::New();
  this->MessageQueue = vtkIGTLMessageQueue::New();

  this->Mutex = vtkMutexLock::New();
  
  this->ServerSocket = igtl::ServerSocket::New();
  this->ClientSocket = NULL;
}



/**
 * Destructor.
 */
vtkPlusOpenIGTLinkServer
::~vtkPlusOpenIGTLinkServer()
{
  this->Stop();
  
  DELETE_IF_NOT_NULL( this->Threader )
  DELETE_IF_NOT_NULL( this->MessageQueue )
  DELETE_IF_NOT_NULL( this->Mutex )
}



void
vtkPlusOpenIGTLinkServer
::WaitForConnection()
{
  while ( this->GetActive() )
  {
    igtl::ClientSocket::Pointer newClientSocket = this->ServerSocket->WaitForConnection( 500 );
    if (newClientSocket.IsNotNull())
    {
      if ( this->ClientSocket.IsNotNull() )  // a new client tries to connect
      {
        LOG_DEBUG( "Server busy with previous client, closing previous connection." );
        this->ClientSocket->CloseSocket();
      }    
      
      this->ClientSocket = newClientSocket;  // new connection is accepted
      this->ClientSocket->SetTimeout( 300 ); // Needs OpenIGTLink revision 7701 (trunk) or later.
      LOG_DEBUG( "Server received new client connection." );
      return;
    }  
  }
}



void
vtkPlusOpenIGTLinkServer
::ReceiveController()
{
  igtl::MessageHeader::Pointer header = igtl::MessageHeader::New();
  
  if ( this->ClientSocket.IsNull() )
  {
    return;
  }
  
  
  while ( this->GetActive() )
  {
    if ( ! this->ClientSocket->GetConnected() )
    {
      break;
    }
    
    header->InitPack();
    
    LOG_INFO( "Socket receiving..." );
    int rs = this->ClientSocket->Receive( header->GetPackPointer(), header->GetPackSize() );
    LOG_INFO( "Socket receiving stopped." );

    if ( rs == 0 )
    {
      LOG_INFO( "Server could not receive package before timeout." );
      this->ClientSocket->CloseSocket();
      break;
    }
    else if ( rs != header->GetPackSize() )
    {
      LOG_WARNING( "Irregluar size " << rs << " expecting " << header->GetPackSize() );
      break;
    }
    
    header->Unpack();  // Deserialize the header
    
    
      // Check data type and receive data body
    
    if ( strcmp( header->GetDeviceType(), "STRING1" ) != 0 )
    {
      LOG_WARNING( "Unexpected message type received: " << header->GetDeviceType() );
      this->ClientSocket->Skip( header->GetBodySizeToRead() );
    }
      
    igtl::StringMessage1::Pointer strMessage = igtl::StringMessage1::New();
    strMessage->SetMessageHeader( header );
    strMessage->AllocatePack();
    
    this->ClientSocket->Receive( strMessage->GetPackBodyPointer(), strMessage->GetPackBodySize() );
    
    int c = strMessage->Unpack( 1 );
    if ( ! ( c & igtl::MessageHeader::UNPACK_BODY ) )
    {
      LOG_WARNING( "Lost OpenIGTLink package detected!" );
      continue;
    }
    
    LOG_INFO( "Server received string: " << strMessage->GetString() );
    
    this->MessageQueue->PushMessage( strMessage );  // Messages can be executed later from the queue.
  }
}



void
vtkPlusOpenIGTLinkServer
::React( std::string input )
{
  vtkSmartPointer< vtkPlusCommandFactory > commandFactory =
      vtkSmartPointer< vtkPlusCommandFactory >::New();
  
  vtkPlusCommand* command = commandFactory->CreatePlusCommand( input );
  
  if ( command != NULL )
  {
    command->SetDataCollector( this->DataCollector );
    LOG_INFO( "Executing: " << command->GetStringRepresentation() );
    command->Execute();
  }
  else
  {
    LOG_ERROR( "Command could not be created from message: " << input );
  }
    
  command->Delete();
}

