/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/


#include "PlusConfigure.h"

#include <iostream>
#include <string>

#include "vtkSmartPointer.h"
#include "vtksys/CommandLineArguments.hxx"

#include "igtlImageMessage.h"
#include "igtlMessageHeader.h"
#include "igtlServerSocket.h"
#include "igtlTransformMessage.h"

#include "igtlStringMessage1.h"



/**
 * Operates Plus functionalities in OpenIGTLink server mode
 * based on client requests.
 */
int main( int argc, char** argv )
{
  
    // Define command line arguments.
  
  int         portNumber;
  std::string inputConfigFileName;
  std::string inputVideoBufferMetafile;
  std::string inputTrackerBufferMetafile;
  int         verboseLevel = vtkPlusLogger::LOG_LEVEL_INFO;
  
  vtksys::CommandLineArguments args;
  args.Initialize( argc, argv );
  
  args.AddArgument( "--port", vtksys::CommandLineArguments::EQUAL_ARGUMENT,
                      &portNumber, "Port number for OpenIGTLink connection." );
  args.AddArgument( "--input-config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT,
                      &inputConfigFileName, "Name of the input configuration file." );
  args.AddArgument( "--input-video-buffer-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT,
                      &inputVideoBufferMetafile, "Video buffer sequence metafile." );
  args.AddArgument( "--input-tracker-buffer-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT,
                      &inputTrackerBufferMetafile, "Tracker buffer sequence metafile." );
  args.AddArgument( "--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT,
                      &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug 5=trace)" );  
  
  
    // Try to parse command line arguments.
  
  if ( ! args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    return 1;  // PLUS_FAIL was 0, so it was not used here.
  }
  
  
    // Check command line arguments.
  
  if ( inputConfigFileName.empty() )
  {
    std::cerr << "input-config-file is required" << std::endl;
    return 1;
  }
  
  
    // Set the logging level.
  
  vtkPlusLogger::Instance()->SetLogLevel( verboseLevel );
  vtkPlusLogger::Instance()->SetDisplayLogLevel( verboseLevel );
  
  
    // Create server socket.
  
  igtl::ServerSocket::Pointer serverSocket = igtl::ServerSocket::New();
  int r = serverSocket->CreateServer( portNumber );

  if ( r < 0 )
    {
    LOG_WARNING( "Cannot create a server socket on port " << portNumber );
    return 0;
    }
  
  
  igtl::Socket::Pointer socket;
  
  while ( 1 )
    {
    socket = serverSocket->WaitForConnection( 100 );
    
    
    if ( socket.IsNotNull() )
      {
      std::cout << "A client is connected." << std::endl;
      
      
        // Create a message buffer to receive header.
      
      igtl::MessageHeader::Pointer headerMsg;
      headerMsg = igtl::MessageHeader::New();
      
      
      for (int i = 0; i < 100; i ++)
        {
        headerMsg->InitPack();
        
        
          // Receive generic header from the socket
        
        int rs = socket->Receive( headerMsg->GetPackPointer(), headerMsg->GetPackSize() );
        if ( rs == 0 )
          {
          socket->CloseSocket();
          }
        if ( rs != headerMsg->GetPackSize() )
          {
          continue;
          }
        
        
          // Deserialize the header
        
        headerMsg->Unpack();
        
        
          // Check data type and receive data body
        
        std::cout << "DeviceType: " << headerMsg->GetDeviceType() << std::endl;
        
        /*
        if ( strcmp( headerMsg->GetDeviceType(), "GET_IMGMETA" ) == 0 )
          {
          std::cerr << "Received a GET_IMGMETA message." << std::endl;
          //socket->Skip(headerMsg->GetBodySizeToRead(), 0);
          SendImageMeta(socket, headerMsg->GetDeviceName());
          }
        else
          {
          // if the data type is unknown, skip reading.
          std::cerr << "Receiving : " << headerMsg->GetDeviceType() << std::endl;
          socket->Skip(headerMsg->GetBodySizeToRead(), 0);
          }
        */
        
        }
      }
    }
  
  return 0;
}
