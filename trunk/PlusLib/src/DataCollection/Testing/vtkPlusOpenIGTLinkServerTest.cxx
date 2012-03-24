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
#include "vtkTransformRepository.h"


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
  double       runTime = 60; 

  vtksys::CommandLineArguments args;
  args.Initialize( argc, argv );

  args.AddArgument( "--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Name of the input configuration file." );
  args.AddArgument( "--port", vtksys::CommandLineArguments::EQUAL_ARGUMENT,&port, "Port number for OpenIGTLink communication." );
  args.AddArgument( "--run-time", vtksys::CommandLineArguments::EQUAL_ARGUMENT,&runTime, "Server run time period in seconds (Default 60sec)" );
  args.AddArgument( "--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug 5=trace)" );  

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

  vtkSmartPointer<vtkTransformRepository> transformRepository = vtkSmartPointer<vtkTransformRepository>::New(); 
  transformRepository->ReadConfiguration( configRootElement ); 
  
  
  LOG_DEBUG( "Initializing data collector... " );
  dataCollector->Connect();

  dataCollector->Start(); 

  // Create a server.

  LOG_DEBUG( "Initializing server... " );
  vtkSmartPointer< vtkPlusOpenIGTLinkServer > server = vtkSmartPointer< vtkPlusOpenIGTLinkServer >::New();
  server->SetDataCollector( dataCollector );
  server->SetTransformRepository( transformRepository ); 
  server->SetNetworkPort( port );
  server->Start();

  double startTime = vtkAccurateTimer::GetSystemTime(); 

  while ( vtkAccurateTimer::GetSystemTime() < startTime + runTime )
  {
    vtkAccurateTimer::Delay( 0.1 );
  }

  server->Stop(); 

  return EXIT_SUCCESS;
}
