
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
  
  this->Threader->TerminateThread( this->ThreadId );
  this->ThreadId = -1;
  
  return PLUS_SUCCESS;
}



void*
vtkPlusOpenIGTLinkServer
::vtkCommunicationThread( vtkMultiThreader::ThreadInfo* data )
{
  vtkPlusOpenIGTLinkServer* self = (vtkPlusOpenIGTLinkServer*)( data->UserData );
  
  int r = self->ServerSocket->CreateServer( self->NetworkPort );
  
  // igtl::ServerSocket::Pointer serverSocket = igtl::ServerSocket::New();
  // int r = serverSocket->CreateServer( self->NetworkPort );
  
  if ( r < 0 )
    {
    std::cerr << "Cannot create a server socket." << std::endl;
    return NULL;
    }
  
  
  // igtl::Socket::Pointer socket;
  
  
  while ( self->GetActive() )
    {
    self->Mutex->Lock();
    // socket = serverSocket->WaitForConnection( 500 );
    self->WaitForConnection();
    self->Mutex->Unlock();
    
    if ( self->ClientSocket.IsNotNull() ) // if client connected
      {
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
 * Protected constructor.
 */
vtkPlusOpenIGTLinkServer
::vtkPlusOpenIGTLinkServer()
{
  this->NetworkPort = -1;
  this->DataCollector = NULL;
  this->ThreadId = -1;
  
  this->Threader = vtkMultiThreader::New();
  
  this->Mutex = vtkMutexLock::New();
  
  this->ServerSocket = igtl::ServerSocket::New();
  // this->ClientSocket = igtl::ClientSocket::New();
}



/**
 * Destructor.
 */
vtkPlusOpenIGTLinkServer
::~vtkPlusOpenIGTLinkServer()
{
  this->Stop();
  
  if ( this->Mutex )
    {
    this->Mutex->Delete();
    }
}



int
vtkPlusOpenIGTLinkServer
::WaitForConnection()
{
  while ( ! this->GetActive() )
    {
    this->ClientSocket = this->ServerSocket->WaitForConnection( 500 );
    if ( this->ClientSocket.IsNotNull() )
      {
      return 1;
      }
    }
  
  if ( this->ClientSocket.IsNotNull() )
    {
    this->ClientSocket->CloseSocket();
    }
  
  return 0;
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
    
    int rs = this->ClientSocket->Receive( header->GetPackPointer(), header->GetPackSize() );
    if ( rs == 0 )
      {
      LOG_INFO( "Server could not receive package." );
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
    
    LOG_INFO( "Server received message." );
    int c = strMessage->Unpack( 1 );
    if ( ! ( c & igtl::MessageHeader::UNPACK_BODY ) )
      {
      LOG_WARNING( "Lost OpenIGTLink package detected!" );
      continue;
      }
    
    LOG_INFO( "Server received string: " << strMessage->GetString() );
    
      // TODO: Instead of doing this on the thread, message would have to be placed
      // in a thread-safe buffer.
    
    this->React( strMessage->GetString() );
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
    command->Execute();
    }
  else
    {
    LOG_ERROR( "Command could not be created from message: " << input );
    }
    
  command->Delete();
}

