
#include <iostream>
#include <sstream>
#include <string>

#include "igtlOSUtil.h"
#include "igtlStringMessage1.h"
#include "igtlClientSocket.h"



/**
 * Tests sending OpenIGTLink string messages, extension to OpenIGTLink v.1.
 */
int main( int argc, char** argv )
{
  if ( argc != 3 )
    {
    std::cout << "Usage: " << argv[ 0 ] << " host port" << std::endl;
    return 0;
    }
  
  char* hostname = argv[ 1 ];
  int   port     = atoi( argv[ 2 ] );
  
  
    // Set up connection.
  
  igtl::ClientSocket::Pointer socket;
  socket = igtl::ClientSocket::New();
  int r = socket->ConnectToServer( hostname, port );
  
  if ( r != 0 )
    {
    std::cout << "Cannot connect to address: " << hostname << ":" << port << std::endl;
    return 1;
    }
  
  
  
    // Allocate string message class.
  
  igtl::StringMessage1::Pointer message = igtl::StringMessage1::New();
  
  
    // Sending messages.
  
  for ( int i = 0; i < 5; ++ i )
    {
    std::stringstream ss;
    ss << "hello " << i;
    
    message->SetString( ss.str() );
    message->Pack();
    socket->Send( message->GetPackPointer(), message->GetPackSize() );
    igtl::Sleep( 100 );
    }
  
  message->SetString( "plus datacollector start" );
  message->Pack();
  socket->Send( message->GetPackPointer(), message->GetPackSize() );
  
  igtl::Sleep( 2000 );
  
  message->SetString( "plus datacollector stop" );
  message->Pack();
  socket->Send( message->GetPackPointer(), message->GetPackSize() );
  
  return 0;
}
