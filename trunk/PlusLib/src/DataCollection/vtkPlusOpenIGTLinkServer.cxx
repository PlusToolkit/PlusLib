
#include "vtkPlusOpenIGTLinkServer.h"

#include "vtkMultiThreader.h"

#include "igtlImageMessage.h"
#include "igtlMessageHeader.h"
#include "igtlOSUtil.h"
#include "igtlServerSocket.h"

#include "igtlStringMessage1.h"



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
    exit( 0 );
    }

  igtl::Socket::Pointer socket;
  
  while ( 1 )
    {
    if ( self->GetActive() == false ) return NULL;
    
    socket = serverSocket->WaitForConnection( 100 );
    if ( socket.IsNotNull() ) // if client connected
      {
      std::cerr << "A client is connected." << std::endl;
      
        // Create a message buffer to receive header
      
      igtl::MessageHeader::Pointer header = igtl::MessageHeader::New();

      for ( int i = 0; i < 10; i ++ )  // TODO: Why 10?
        {
        header->InitPack();
        
          // Receive generic header from the socket
        
        int rs = socket->Receive( header->GetPackPointer(), header->GetPackSize() );
        if ( rs == 0 )
          {
          socket->CloseSocket();
          }
        if ( rs != header->GetPackSize() )
          {
          continue;
          }
        
        
        header->Unpack();  // Deserialize the header
        
        std::cout << "Server received message from device: " << header->GetDeviceType() << std::endl;
        
        
          // Check data type and receive data body
        
        if ( strcmp( header->GetDeviceType(), "STRING1" ) == 0 )
          {
          std::cerr << "Received a string1 message." << std::endl;
          
            // socket->Skip( headerMsg->GetBodySizeToRead(), 0 );
            // SendImageMeta(socket, headerMsg->GetDeviceName());
          
          igtl::StringMessage1::Pointer strMessage = igtl::StringMessage1::New();
          strMessage->SetMessageHeader( header );
          strMessage->AllocatePack();
          
          socket->Receive( strMessage->GetPackBodyPointer(), strMessage->GetPackBodySize() );
          
          int c = strMessage->Unpack( 1 );
          if ( ! ( c & igtl::MessageHeader::UNPACK_BODY ) )
            {
            LOG_WARNING( "Lost OpenIGTLink package detected!" );
            continue;
            }
          
          self->Respond( socket, strMessage->GetString() );
          }
        else
          {
            // if the data type is unknown, skip reading.
          std::cout << "Receiving : " << header->GetDeviceType() << std::endl;
          socket->Skip( header->GetBodySizeToRead(), 0 );
          }
        
        }
      }
    }
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
  this->Active = false;
  
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
}



/**
 * Destructor.
 */
vtkPlusOpenIGTLinkServer
::~vtkPlusOpenIGTLinkServer()
{
  this->Stop();
  vtkAccurateTimer::Delay( 0.3 );
}



void
vtkPlusOpenIGTLinkServer
::Respond( igtl::Socket::Pointer& socket, std::string input )
{
  igtl::StringMessage1::Pointer response = igtl::StringMessage1::New();
  response->SetDeviceName( "PlusServer" );
  response->SetString( "<c></c>" );
  response->Pack();
  
  socket->Send( response->GetPackPointer(), response->GetPackSize() );
}

