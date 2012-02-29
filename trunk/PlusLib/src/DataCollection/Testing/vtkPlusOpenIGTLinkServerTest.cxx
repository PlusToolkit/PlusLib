/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
  \file vtkPlusOpenIGTLinkServerTest.cxx 
  \brief This program tests Plus server and Plus client. The communication in this test
  happens between two threads. In real life, it happens between two programs.
*/ 

#include <iostream>
#include <string>

#include "igtlClientSocket.h"

#include "vtkSmartPointer.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkXMLUtilities.h"

#include "PlusConfigure.h"
#include "vtkDataCollectorHardwareDevice.h"
#include "vtkSavedDataTracker.h"
#include "vtkSavedDataVideoSource.h"

#include "igtlStringMessage1.h"
#include "vtkPlusCommandFactory.h"
#include "vtkPlusOpenIGTLinkClient.h"
#include "vtkPlusOpenIGTLinkServer.h"
#include "vtkPlusStartDataCollectionCommand.h"
#include "vtkPlusStopDataCollectionCommand.h"


int main( int argc, char** argv )
{
  vtkObjectFactory::RegisterFactory( vtkSmartPointer< vtkPlusCommandFactory >::New() );
  vtkObjectFactory::RegisterFactory( vtkSmartPointer< vtkPlusStartDataCollectionCommand >::New() );


  // Check command line arguments.

  std::string  inputConfigFileName;
  std::string  inputVideoBufferMetafile;
  std::string  inputTrackerBufferMetafile;
  int          port = 0;
  int          verboseLevel = vtkPlusLogger::LOG_LEVEL_DEFAULT;

  vtksys::CommandLineArguments args;
  args.Initialize( argc, argv );

  args.AddArgument( "--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT,
    &inputConfigFileName, "Name of the input configuration file." );
  args.AddArgument( "--port", vtksys::CommandLineArguments::EQUAL_ARGUMENT,
    &port, "Port number for OpenIGTLink communication." );
  args.AddArgument( "--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, 
    &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug 5=trace)" );  

  if ( ! args.Parse() )
  {
    std::cerr << "Problem parsing arguments." << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    return 1;
  }

  vtkPlusLogger::Instance()->SetLogLevel( verboseLevel );


  // Prepare data collector object.

  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(
    vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str()));
  if (configRootElement == NULL)
  {	
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str()); 
    return 1;
  }

  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement);

  vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New();

  dataCollector->ReadConfiguration( configRootElement );
  
  
  LOG_DEBUG( "Initializing data collector... " );
  dataCollector->Connect();


  // Create a server.

  LOG_DEBUG( "Initializing server... " );
  vtkSmartPointer< vtkPlusOpenIGTLinkServer > server = vtkSmartPointer< vtkPlusOpenIGTLinkServer >::New();
  server->SetDataCollector( dataCollector );
  server->SetNetworkPort( port );
  server->Start();

  vtkAccurateTimer::Delay( 0.1 );


  // Create a client to connect to the server.

  vtkSmartPointer< vtkPlusOpenIGTLinkClient > plusClient =
    vtkSmartPointer< vtkPlusOpenIGTLinkClient >::New();
  plusClient->SetNetworkPort( port );
  plusClient->SetServerAddress( "localhost" );

  int r = plusClient->ConnectToServer();
  if ( r != 0 )
  {
    LOG_WARNING( "Client could not connect to the server." );
    return 1;
  }
  else
  {
    LOG_DEBUG( "Client connection successful." );
  }


  // Prepare commands to be sent.

  vtkSmartPointer< vtkPlusStartDataCollectionCommand > startCommand =
    vtkSmartPointer< vtkPlusStartDataCollectionCommand >::New();
  vtkSmartPointer< vtkPlusStopDataCollectionCommand > stopCommand =
    vtkSmartPointer< vtkPlusStopDataCollectionCommand >::New();


  // Start data collector.

  bool success = plusClient->SendCommand( startCommand );
  vtkAccurateTimer::Delay( 0.1 );
  while ( server->GetBufferedMessageCount() > 0 )
  {
    LOG_INFO( "Executing next command in queue." );
    server->ExecuteNextCommand();
  }

  vtkAccurateTimer::Delay( 0.5 );


  // Stop data collector.

  success = plusClient->SendCommand( stopCommand );
  vtkAccurateTimer::Delay( 0.1 );
  while ( server->GetBufferedMessageCount() > 0 )
  {
    LOG_INFO( "Executing next command in queue." );
    server->ExecuteNextCommand();
  }


  vtkAccurateTimer::Delay( 0.1 );
  server->Stop();
  LOG_INFO( "Server stopped." );
  vtkAccurateTimer::Delay( 0.1 );


  return EXIT_SUCCESS;
}
