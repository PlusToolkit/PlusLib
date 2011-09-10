
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



static
void*
vtkCommunicationThread( vtkMultiThreader::ThreadInfo* data )
{
  vtkPlusOpenIGTLinkServer* self = (vtkPlusOpenIGTLinkServer*)( data->UserData );
  
  igtl::ServerSocket::Pointer serverSocket = igtl::ServerSocket::New();
  int r = serverSocket->CreateServer( self->NetworkPort );
  
  
  if ( r < 0 )
    {
    std::cerr << "Cannot create a server socket." << std::endl;
    return NULL;
    }

  igtl::Socket::Pointer socket;
  
  while ( self->GetActive() )
    {
    self->Mutex->Lock();
    std::cerr << "wait for connection" << std::endl;
    socket = serverSocket->WaitForConnection( 500 );
    self->Mutex->Unlock();
    
    if ( socket.IsNotNull() ) // if client connected
      {
      std::cerr << "socket not null" << std::endl;
      
      igtl::MessageHeader::Pointer header = igtl::MessageHeader::New();
      
      for ( int i = 0; i < 100; ++ i )
        {
        header->InitPack();
        
        int rs = socket->Receive( header->GetPackPointer(), header->GetPackSize() );
        if ( rs == 0 )
          {
          LOG_ERROR( "No OpenIGTLink header read." );
          socket->CloseSocket();
          continue;
          }
        if ( rs != header->GetPackSize() )
          {
          LOG_WARNING( "Failed to read message from the Plus client." );
          continue;
          }
        
        header->Unpack();  // Deserialize the header
        
          // Check data type and receive data body
        
        if ( strcmp( header->GetDeviceType(), "STRING1" ) == 0 )
          {
          igtl::StringMessage1::Pointer strMessage = igtl::StringMessage1::New();
          strMessage->SetMessageHeader( header );
          strMessage->AllocatePack();
          
          socket->Receive( strMessage->GetPackBodyPointer(), strMessage->GetPackBodySize() );
          
          LOG_INFO( "Server received message." );
          int c = strMessage->Unpack( 1 );
          if ( ! ( c & igtl::MessageHeader::UNPACK_BODY ) )
            {
            LOG_WARNING( "Lost OpenIGTLink package detected!" );
            continue;
            }
          
          LOG_INFO( "  String: " << strMessage->GetString() );
          
          self->React( socket, strMessage->GetString() );
          }
        else
          {
          LOG_WARNING( "Unexpected message type received: " << header->GetDeviceType() );
          socket->Skip( header->GetBodySizeToRead(), 0 );
          }
        }
      
      }
    }
  
  if ( socket.IsNotNull() )
    {
    socket->CloseSocket();
    }
  
  return NULL;
}



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
  
  this->Mutex->Lock();
  this->Mutex->Unlock();
  
  this->Threader->TerminateThread( this->ThreadId );
  this->ThreadId = -1;
  
  return PLUS_SUCCESS;
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



void
vtkPlusOpenIGTLinkServer
::React( igtl::Socket::Pointer& socket, std::string input )
{
  vtkSmartPointer< vtkPlusCommandFactory > commandFactory = vtkSmartPointer< vtkPlusCommandFactory >::New();
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

