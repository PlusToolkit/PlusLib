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
  int          verboseLevel = vtkPlusLogger::LOG_LEVEL_DEFAULT;
  double       runTime = 60; 

  vtksys::CommandLineArguments args;
  args.Initialize( argc, argv );

  args.AddArgument( "--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Name of the input configuration file." );
  args.AddArgument( "--running-time", vtksys::CommandLineArguments::EQUAL_ARGUMENT,&runTime, "Server running time period in seconds (Default 60sec)" );
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
    exit(EXIT_FAILURE);
  }

  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement);

  vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New();

  if ( dataCollector->ReadConfiguration( configRootElement ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Datacollector failed to read configuration!"); 
    exit(EXIT_FAILURE);
  }

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

  // Create a server.

  LOG_DEBUG( "Initializing server... " );
  vtkSmartPointer< vtkPlusOpenIGTLinkServer > server = vtkSmartPointer< vtkPlusOpenIGTLinkServer >::New();
  server->SetDataCollector( dataCollector );
  if ( server->ReadConfiguration(configRootElement) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read PlusOpenIGTLinkServer configuration!"); 
    exit(EXIT_FAILURE);
  }

  server->SetTransformRepository( transformRepository ); 
  server->Start();

  double startTime = vtkAccurateTimer::GetSystemTime(); 

  while ( vtkAccurateTimer::GetSystemTime() < startTime + runTime )
  {
    vtkAccurateTimer::Delay( 0.2 );
  }

  server->Stop(); 

  return EXIT_SUCCESS;
}
