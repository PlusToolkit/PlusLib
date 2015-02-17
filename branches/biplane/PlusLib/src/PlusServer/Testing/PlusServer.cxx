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
#include "vtkDataCollector.h"
#include "vtkOpenIGTLinkVideoSource.h"
#include "vtkPlusBuffer.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusOpenIGTLinkServer.h"
#include "vtkSmartPointer.h"
#include "vtkTransformRepository.h"
#include "vtksys/CommandLineArguments.hxx"

// For catching Ctrl-C
#include <csignal>
#include <cstdlib>
#include <cstdio>

// Connect/disconnect clients to server for testing purposes
PlusStatus ConnectClients( int listeningPort, std::vector< vtkSmartPointer<vtkOpenIGTLinkVideoSource> >& testClientList, int numberOfClientsToConnect, vtkSmartPointer<vtkXMLDataElement> configRootElement ); 
PlusStatus DisconnectClients( std::vector< vtkSmartPointer<vtkOpenIGTLinkVideoSource> >& testClientList );

// Forward declare signal handler
void SignalInterruptHandler(int s);
static bool stopRequested = false;

int main( int argc, char** argv )
{
  // Check command line arguments.
  bool printHelp(false);
  std::string inputConfigFileName;
  std::string testingConfigFileName;
  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;
  double runTimeSec = 0.0;

  const int numOfTestClientsToConnect = 5; // only if testing is enabled S

  vtksys::CommandLineArguments args;
  args.Initialize( argc, argv );

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  args.AddArgument( "--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Name of the input configuration file." );
  args.AddArgument( "--running-time", vtksys::CommandLineArguments::EQUAL_ARGUMENT,&runTimeSec, "Server running time period in seconds. If the parameter is not defined or 0 then the server runs infinitely." );
  args.AddArgument( "--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)" );
  args.AddArgument( "--testing-config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &testingConfigFileName, "Enable testing mode (testing PlusServer functionality by running a few OpenIGTLink clients)" );

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments." << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE); 
  }
  
  if ( printHelp )
  {
    std::cout << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS);
  }

  vtkSmartPointer<vtkImageData> indata = vtkSmartPointer<vtkImageData>::New();
  indata->SetDimensions(100,100,10);
  indata->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
  PlusVideoFrame::FlipInfoType flip;
  int or[3] = {0,0,0};
  int si[3] = {100,100,10};
  vtkSmartPointer<vtkImageData> outdata = vtkSmartPointer<vtkImageData>::New();
  outdata->SetDimensions(100,100,10);
  outdata->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
  PlusVideoFrame::FlipClipImage(indata, flip, or, si, outdata);
  
  vtkPlusLogger::Instance()->SetLogLevel( verboseLevel );

  if ( inputConfigFileName.empty() )
  {
    LOG_ERROR("--config-file argument is required!"); 
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE); 
  }

  // Create Plus OpenIGTLink server.
  LOG_DEBUG( "Initializing Plus OpenIGTLink server... " );
  vtkSmartPointer< vtkPlusOpenIGTLinkServer > server = vtkSmartPointer< vtkPlusOpenIGTLinkServer >::New();
  if (server->Start(inputConfigFileName)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to start OpenIGTLink server");
    exit(EXIT_FAILURE);
  }

  double startTime = vtkAccurateTimer::GetSystemTime(); 

  // *************************** Testing **************************
  std::vector< vtkSmartPointer<vtkOpenIGTLinkVideoSource> > testClientList; 
  if ( !testingConfigFileName.empty() )
  {
    vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
    if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, testingConfigFileName.c_str())==PLUS_FAIL)
    {  
      LOG_ERROR("Unable to read test configuration from file " << testingConfigFileName.c_str()); 
      DisconnectClients( testClientList );
      server->Stop(); 
      exit(EXIT_FAILURE);      
    }

    // Connect clients to server 
    if ( ConnectClients( server->GetListeningPort(), testClientList, numOfTestClientsToConnect, configRootElement ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to connect clients to PlusServer!"); 
      DisconnectClients( testClientList );
      server->Stop(); 
      exit(EXIT_FAILURE);
    }
    vtkAccurateTimer::Delay( 1.0 ); // make sure the threads have some time to connect regardless of the specified runTimeSec
    LOG_INFO("Clients are connected");
  }
  // *************************** End of testing **************************

  std::cout << "Press Ctrl-C to quit:" << std::endl;

  // Set up signal catching
  signal(SIGINT, SignalInterruptHandler);

  bool neverStop = (runTimeSec==0.0);

  // Run server until requested 
  const double commandQueuePollIntervalSec=0.010;
  while ( ((vtkAccurateTimer::GetSystemTime() < startTime + runTimeSec) || neverStop)
    && (!stopRequested) )
  {
    server->ProcessPendingCommands();
    // Need to process messages while waiting because some devices (such as the vtkWin32VideoSource2) require event processing
    vtkAccurateTimer::DelayWithEventProcessing(commandQueuePollIntervalSec);
  }

  // *************************** Testing **************************
  if ( !testingConfigFileName.empty() )
  {
    LOG_INFO("Requested testing time elapsed");
    // make sure all the clients are still connected 
    int numOfActuallyConnectedClients=server->GetNumberOfConnectedClients();
    if ( numOfActuallyConnectedClients != numOfTestClientsToConnect )
    {
      LOG_ERROR("Number of connected clients to PlusServer doesn't match the requirements (" 
        << numOfActuallyConnectedClients << " out of " << numOfTestClientsToConnect << ")."); 
      DisconnectClients( testClientList );
      server->Stop(); 
      exit(EXIT_FAILURE);
    }

    // Disconnect clients from server
    LOG_INFO("Disconnecting clients...");
    if ( DisconnectClients( testClientList ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to disconnect clients from PlusServer!"); 
      server->Stop(); 
      exit(EXIT_FAILURE);
    }
    LOG_INFO("Clients are disconnected");
  }
  // *************************** End of testing **************************


  server->Stop();
  
  if ( !testingConfigFileName.empty() )
  {
    LOG_INFO("Test is successfully completed");
  }

  return EXIT_SUCCESS;
}

// -------------------------------------------------
PlusStatus ConnectClients( int listeningPort, std::vector< vtkSmartPointer<vtkOpenIGTLinkVideoSource> >& testClientList, int numberOfClientsToConnect, vtkSmartPointer<vtkXMLDataElement> configRootElement )
{
  if (configRootElement==NULL)
  {
    LOG_ERROR("PlusServer client configuration is missing");
    return PLUS_FAIL;
  }

  int numberOfErrors = 0; 

  // Clear test client list 
  testClientList.clear(); 

  for ( int i = 0; i < numberOfClientsToConnect; ++i )
  {
    vtkSmartPointer<vtkOpenIGTLinkVideoSource> client = vtkSmartPointer<vtkOpenIGTLinkVideoSource>::New(); 
    client->SetDeviceId("OpenIGTLinkVideoSenderDevice");
    client->ReadConfiguration(configRootElement);
    client->SetServerAddress("localhost");
    client->SetServerPort(listeningPort); 
    if( client->OutputChannelCount() == 0 )
    {
      LOG_ERROR("No output channels in openIGTLink client.");
      ++numberOfErrors;
      continue;
    }
    vtkPlusChannel* aChannel = *(client->GetOutputChannelsStart());
    vtkPlusDataSource* aSource(NULL);
    if( aChannel->GetVideoSource(aSource) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to retrieve the video source.");
      continue;
    }
    client->SetBufferSize( *aChannel, 10 ); 
    client->SetMessageType( "TrackedFrame" ); 
    aSource->SetImageOrientation( US_IMG_ORIENT_MF );

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

// -------------------------------------------------
void SignalInterruptHandler(int s)
{
  LOG_INFO("Stop requested...");
  stopRequested = true;
}