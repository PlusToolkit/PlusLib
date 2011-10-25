/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
  \file This program tests Plus server and Plus client. The communication in this test
  happens between two threads. In real life, it happens between two programs.
*/ 

#include <iostream>
#include <string>

#include "igtlClientSocket.h"

#include "vtkSmartPointer.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkXMLUtilities.h"

#include "PlusConfigure.h"
#include "vtkDataCollector.h"
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

  std::string  InputConfigFileName;
  std::string  InputVideoBufferMetafile;
  std::string  InputTrackerBufferMetafile;
  int          Port = 0;
  int          VerboseLevel = vtkPlusLogger::LOG_LEVEL_INFO;

  vtksys::CommandLineArguments args;
  args.Initialize( argc, argv );

  args.AddArgument( "--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT,
    &InputConfigFileName, "Name of the input configuration file." );
  args.AddArgument( "--input-video-buffer-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT,
    &InputVideoBufferMetafile, "Video buffer sequence metafile." );
  args.AddArgument( "--input-tracker-buffer-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT,
    &InputTrackerBufferMetafile, "Tracker buffer sequence metafile." );
  args.AddArgument( "--port", vtksys::CommandLineArguments::EQUAL_ARGUMENT,
    &Port, "Port number for OpenIGTLink communication." );
  args.AddArgument( "--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, 
    &VerboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug 5=trace)" );  

  if ( ! args.Parse() )
  {
    std::cerr << "Problem parsing arguments." << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    return 1;
  }

  vtkPlusLogger::Instance()->SetLogLevel( VerboseLevel );
  vtkPlusLogger::Instance()->SetDisplayLogLevel( VerboseLevel );
  
  
    // Prepare data collector object.
  
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkXMLUtilities::ReadElementFromFile(InputConfigFileName.c_str());
  if (configRootElement == NULL)
  {	
    LOG_ERROR("Unable to read configuration from file " << InputConfigFileName.c_str()); 
    return 1;
  }
  
  vtkDataCollector* dataCollector = vtkDataCollector::New();
  dataCollector->ReadConfiguration( configRootElement );
  
  if ( dataCollector->GetAcquisitionType() == SYNCHRO_VIDEO_SAVEDDATASET )
  {
    if ( InputVideoBufferMetafile.empty() )
    {
      LOG_ERROR( "Video source metafile missing." );
      return 1;
    }
    
    vtkSavedDataVideoSource* videoSource =
      dynamic_cast< vtkSavedDataVideoSource* >( dataCollector->GetVideoSource() );
    if ( videoSource == NULL )
    {
      LOG_ERROR( "Invalid saved data video source." );
      exit( 1 );
    }
    videoSource->SetSequenceMetafile( InputVideoBufferMetafile.c_str() );
    videoSource->SetReplayEnabled( true ); 
  }
  
  if ( dataCollector->GetTrackerType() == TRACKER_SAVEDDATASET )
  {
    if ( InputTrackerBufferMetafile.empty() )
    {
      LOG_ERROR( "Tracker source metafile missing." );
      return 1;
    }
    vtkSavedDataTracker* tracker = static_cast< vtkSavedDataTracker* >( dataCollector->GetTracker() );
    tracker->SetSequenceMetafile( InputTrackerBufferMetafile.c_str() );
    tracker->SetReplayEnabled( true ); 
    tracker->Connect();
  }
  
  LOG_DEBUG( "Initializing data collector... " );
  dataCollector->Connect();
  
  
    // Create a server.
  
  LOG_DEBUG( "Initializing server... " );
  vtkSmartPointer< vtkPlusOpenIGTLinkServer > server =
    vtkSmartPointer< vtkPlusOpenIGTLinkServer >::New();
  server->SetDataCollector( dataCollector );
  server->SetNetworkPort( Port );
  server->Start();
  
  vtkAccurateTimer::Delay( 0.1 );
  
  
    // Create a client to connect to the server.
  
  vtkSmartPointer< vtkPlusOpenIGTLinkClient > plusClient =
    vtkSmartPointer< vtkPlusOpenIGTLinkClient >::New();
  plusClient->SetNetworkPort( Port );
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
