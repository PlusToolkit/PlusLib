/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
\file PlusServer.cxx 
\brief Start Plus OpenIGTLink server for broadcasting selected IGTL messages. 
If testing enabled this program tests Plus server and Plus client. The communication in this test
happens between two threads. In real life, it happens between two programs.
*/ 

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkDataCollector.h"
#include "vtkPlusOpenIGTLinkServer.h"
#include "vtkTransformRepository.h"
#include "vtkOpenIGTLinkVideoSource.h"

// Connect/disconnect clients to server for testing purposes 
PlusStatus ConnectClients( int listeningPort, std::vector< vtkSmartPointer<vtkOpenIGTLinkVideoSource> >& testClientList, int numberOfClientsToConnect ); 
PlusStatus DisconnectClients( std::vector< vtkSmartPointer<vtkOpenIGTLinkVideoSource> >& testClientList ); 

int main( int argc, char** argv )
{
  // Check command line arguments.
  std::string inputConfigFileName;
  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;
  double runTime = 60;
  bool testing = false; 

  const int numOfTestClientsToConnect = 5; // only if testing is enabled S

  vtksys::CommandLineArguments args;
  args.Initialize( argc, argv );

  args.AddArgument( "--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Name of the input configuration file." );
  args.AddArgument( "--running-time", vtksys::CommandLineArguments::EQUAL_ARGUMENT,&runTime, "Server running time period in seconds (Default 60sec)" );
  args.AddArgument( "--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)" );
  args.AddArgument( "--testing", vtksys::CommandLineArguments::NO_ARGUMENT, &testing, "Enable testing mode (testing PlusServer functionality)" );

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments." << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE); 
  }
  
  vtkPlusLogger::Instance()->SetLogLevel( verboseLevel );

  if ( inputConfigFileName.empty() )
  {
    LOG_ERROR("--config-file argument is required!"); 
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE); 
  }

  // Read main configuration file
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(
    vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str()));
  if (configRootElement == NULL)
  {	
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str()); 
    exit(EXIT_FAILURE);
  }

  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement);

  // Create data collector instance 
  vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New();
  if ( dataCollector->ReadConfiguration( configRootElement ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Datacollector failed to read configuration!"); 
    exit(EXIT_FAILURE);
  }

  // Create transform repository instance 
  vtkSmartPointer<vtkTransformRepository> transformRepository = vtkSmartPointer<vtkTransformRepository>::New(); 
  if ( transformRepository->ReadConfiguration( configRootElement ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Transform repository failed to read configuration!"); 
    exit(EXIT_FAILURE);
  }

  LOG_DEBUG( "Initializing data collector... " );
  if ( dataCollector->Connect() != PLUS_SUCCESS )
  {
    LOG_ERROR("Datacollector failed to connect to devices!"); 
    exit(EXIT_FAILURE);
  }

  if ( dataCollector->Start() != PLUS_SUCCESS )
  {
    LOG_ERROR("Datacollector failed to start!"); 
    exit(EXIT_FAILURE);
  }

  // Create Plus OpenIGTLink server.
  LOG_DEBUG( "Initializing Plus OpenIGTLink server... " );
  vtkSmartPointer< vtkPlusOpenIGTLinkServer > server = vtkSmartPointer< vtkPlusOpenIGTLinkServer >::New();
  server->SetDataCollector( dataCollector );
  if ( server->ReadConfiguration(configRootElement) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read PlusOpenIGTLinkServer configuration!"); 
    exit(EXIT_FAILURE);
  }

  server->SetTransformRepository( transformRepository ); 
  if ( server->Start() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to start Plus OpenIGTLink server!"); 
    exit(EXIT_FAILURE);
  }

  double startTime = vtkAccurateTimer::GetSystemTime(); 


  // *************************** Testing **************************
  std::vector< vtkSmartPointer<vtkOpenIGTLinkVideoSource> > testClientList; 
  if ( testing ) 
  {
    // Connect clients to server 
    if ( ConnectClients( server->GetListeningPort(), testClientList, numOfTestClientsToConnect ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to connect clients to PlusServer!"); 
      DisconnectClients( testClientList );
      server->Stop(); 
      exit(EXIT_FAILURE);
    }
  }
  // *************************** End of testing **************************


  // Run server until requested 
  while ( vtkAccurateTimer::GetSystemTime() < startTime + runTime )
  {
#ifdef _WIN32
    // Need to process messages because some devices (such as the vtkWin32VideoSource2) require event processing
    MSG Msg;
    while (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&Msg);
      DispatchMessage(&Msg);
    }
    Sleep(1); // give a chance to other threads to get CPU time now
#else
    vtkAccurateTimer::Delay( 0.2 );
#endif
  }

  // *************************** Testing **************************
  if ( testing ) 
  {
    // make sure all the clients are still connected 
    if ( server->GetNumberOfConnectedClients() != numOfTestClientsToConnect )
    {
      LOG_ERROR("Number of connected clients to PlusServer doesn't match the requirements (" 
        << server->GetNumberOfConnectedClients() << " out of " << numOfTestClientsToConnect << ")."); 
      DisconnectClients( testClientList );
      server->Stop(); 
      exit(EXIT_FAILURE);
    }

    // Disconnect clients from server 
    if ( DisconnectClients( testClientList ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to disconnect clients from PlusServer!"); 
      server->Stop(); 
      exit(EXIT_FAILURE);
    }
  }
  // *************************** End of testing **************************


  server->Stop(); 

  return EXIT_SUCCESS;
}

// -------------------------------------------------
PlusStatus ConnectClients( int listeningPort, std::vector< vtkSmartPointer<vtkOpenIGTLinkVideoSource> >& testClientList, int numberOfClientsToConnect )
{
  int numberOfErrors = 0; 

  // Clear test client list 
  testClientList.clear(); 

  for ( int i = 0; i < numberOfClientsToConnect; ++i )
  {
    vtkSmartPointer<vtkOpenIGTLinkVideoSource> client = vtkSmartPointer<vtkOpenIGTLinkVideoSource>::New(); 
    client->SetServerAddress("localhost"); 
    client->SetServerPort(listeningPort); 
    client->SetFrameBufferSize( 10 ); 
    client->SetMessageType( "TrackedFrame" ); 
    client->SetDeviceImageOrientation( US_IMG_ORIENT_MF ); 

    if ( client->Connect() != PLUS_SUCCESS )
    {
      LOG_ERROR("Client #" << i+1 << " couldn't connect to server."); 
      ++numberOfErrors;
      continue; 
    }

    LOG_DEBUG("Client #" << i+1 << " successfully connected to server!"); 

    if ( client->StartRecording() != PLUS_SUCCESS )
    {
      LOG_ERROR("Client #" << i+1 << " couldn't start recording frames."); 
      client->Disconnect(); 
      ++numberOfErrors;
      continue;
    }

    // Add connected client to list
    testClientList.push_back(client); 
  }

  return ( numberOfErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL ); 
}

// -------------------------------------------------
PlusStatus DisconnectClients( std::vector< vtkSmartPointer<vtkOpenIGTLinkVideoSource> >& testClientList )
{
  int numberOfErrors = 0; 
  for ( int i = 0; i < testClientList.size(); ++i )
  {
    if ( testClientList[i]->StopRecording() != PLUS_SUCCESS )
    {
      LOG_ERROR("Client #" << i+1 << " failed to stop recording"); 
      ++numberOfErrors;
    }

    if ( testClientList[i]->Disconnect() != PLUS_SUCCESS )
    {
      LOG_ERROR("Client #" << i+1 << " failed to disconnect from server"); 
      ++numberOfErrors;
      continue;
    }

    LOG_DEBUG("Client #" << i+1 << " successfully disconnected from server!"); 
  }

  return ( numberOfErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL ); 
}
