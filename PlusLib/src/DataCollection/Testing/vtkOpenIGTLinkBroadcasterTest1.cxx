/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
  \file vtkOpenIGTLinkBroadcasterTest1.cxx 
  \brief This test program sends the contents of a saved tracked image sequence 
  and sends it through OpenIGTLink
*/ 

#include <ctime>
#include <iostream>

#include "PlusConfigure.h"

#include "vtkMatrix4x4.h"
#include "vtkSmartPointer.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkXMLUtilities.h"

#include "vtkDataCollector.h"
#include "vtkSavedDataTracker.h"
#include "vtkSavedDataVideoSource.h"

#include "vtkOpenIGTLinkBroadcaster.h"

#include "OpenIGTLinkReceiveServer.h"
#include "vtkVideoBuffer.h"



enum {
  BC_EXIT_SUCCESS = 0,
  BC_EXIT_FAILURE
};

const double DELAY_BETWEEN_MESSAGES_SEC = 0.18;



/**
 * Print acutal transform on the screen for debugging.
 */
void PrintActualTransforms( vtkDataCollector* dataCollector )
{
  std::stringstream ss;
    
  if ( dataCollector->GetTrackingEnabled() == false )
  {
    LOG_ERROR("Tracking is not enabled!");
    return;
  }

  TrackedFrame trackedFrame;
  if ( dataCollector->GetTrackedFrame(&trackedFrame) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get tracked frame!"); 
    return; 
  }

  ss << "Timestamp: " << trackedFrame.GetTimestamp() << std::endl;

  std::vector<PlusTransformName> transformNames; 
  trackedFrame.GetCustomFrameTransformNameList(transformNames);

  std::vector<PlusTransformName>::iterator it;
  for (it = transformNames.begin(); it != transformNames.end(); ++it)
  {
    TrackedFrameFieldStatus status = FIELD_INVALID;
    trackedFrame.GetCustomFrameTransformStatus(*it, status);

    if ( status != FIELD_OK )
    {
      std::string transformNameStr; 
      it->GetTransformName(transformNameStr); 
      ss  << "Transform '" << transformNameStr << "' missing (probably tracker tool is out of view)" << std::endl; 
      continue;
    }

    vtkSmartPointer< vtkMatrix4x4 > transformMatrix  = vtkSmartPointer< vtkMatrix4x4 >::New();
    if ( trackedFrame.GetCustomFrameTransform(*it, transformMatrix) != PLUS_SUCCESS )
    {
      std::string transformNameStr; 
      it->GetTransformName(transformNameStr); 
      LOG_ERROR("Unable to get transform '" << transformNameStr << "'!");
      continue;
    }

    ss  << std::fixed 
    << transformMatrix->GetElement(0,0) << "   " << transformMatrix->GetElement(0,1) << "   "
      << transformMatrix->GetElement(0,2) << "   " << transformMatrix->GetElement(0,3) << std::endl
    << transformMatrix->GetElement(1,0) << "   " << transformMatrix->GetElement(1,1) << "   "
      << transformMatrix->GetElement(1,2) << "   " << transformMatrix->GetElement(1,3) << std::endl
    << transformMatrix->GetElement(2,0) << "   " << transformMatrix->GetElement(2,1) << "   "
      << transformMatrix->GetElement(2,2) << "   " << transformMatrix->GetElement(2,3) << std::endl
    << transformMatrix->GetElement(3,0) << "   " << transformMatrix->GetElement(3,1) << "   "
      << transformMatrix->GetElement(3,2) << "   " << transformMatrix->GetElement(3,3) << std::endl; 
  }
}



/**
 * Tests OpenIGTLink broadcasting of images and transforms.
 */
int main( int argc, char** argv )
{
  
    // Parse command line arguments.
  
  std::string inputConfigFileName;
  std::string inputVideoBufferMetafile;
  std::string inputTrackerBufferMetafile;
  bool inputReplay(false); 
  int verboseLevel=vtkPlusLogger::LOG_LEVEL_DEFAULT;
  
  vtksys::CommandLineArguments args;
  args.Initialize( argc, argv );
  
  args.AddArgument( "--input-config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT,
                    &inputConfigFileName, "Name of the input configuration file." );
  args.AddArgument( "--replay", vtksys::CommandLineArguments::NO_ARGUMENT,
                    &inputReplay, "Replay tracked frames after reached the latest one." );
  args.AddArgument( "--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, 
                      &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug 5=trace)");  
  
  if ( ! args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit( BC_EXIT_FAILURE );
  }
  
  if ( inputConfigFileName.empty() )
  {
    std::cerr << "input-config-file is required" << std::endl;
    exit( BC_EXIT_FAILURE );
  }
  
  vtkPlusLogger::Instance()->SetLogLevel( verboseLevel );
  
  
    // Prepare data collector object.
  
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str()));
  if ( configRootElement == NULL )
  {	
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str()); 
    return BC_EXIT_FAILURE;
  }

  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData( configRootElement );

  vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New();
  
  dataCollector->ReadConfiguration( configRootElement );
  
  LOG_INFO("Initializing data collector... ");
  dataCollector->Connect();
  
  
    // Prepare server to receive messages.
  
  OpenIGTLinkReceiveServer receiveServer( 18944 );
  receiveServer.Start();
  
  
    // Prepare the OpenIGTLink broadcaster.
  
  vtkSmartPointer< vtkOpenIGTLinkBroadcaster > broadcaster = vtkSmartPointer< vtkOpenIGTLinkBroadcaster >::New();
  broadcaster->SetDataCollector( dataCollector );
  
  if (broadcaster->Initialize() != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to initialize OpenIGTLinkBroadcaster!");
    exit( BC_EXIT_FAILURE );
  }

  if (broadcaster->ReadConfiguration(configRootElement) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to read OpenIGTLinkBroadcaster configuration!");
    exit( BC_EXIT_FAILURE );
  }
  
    // Starting data collector.
  
  LOG_INFO("Start data collector... ");
  dataCollector->Start();
  
    // Determine number of iterations the broadcaster should run for.
  
  unsigned int numberOfBroadcastedMessages = 10;
  
  
    // Send messages in each itreation.
  
  for ( int i = 0; i < numberOfBroadcastedMessages; ++ i )
  {
    vtkAccurateTimer::Delay( DELAY_BETWEEN_MESSAGES_SEC );
    LOG_INFO( "Iteration: " << i );
    
    if (broadcaster->SendMessages() != PLUS_SUCCESS)
    {
      LOG_ERROR( "Broadcaster couldn't send messages!" );
    }

    PrintActualTransforms( dataCollector );
  }

  
    // Stopping receive server and data collector (they have threads).
  
  receiveServer.Stop();
  int numReceivedMessages = receiveServer.GetNumberOfReceivedMessages();
  LOG_INFO( "Received OpenIGTLink messages: " << numReceivedMessages );
  
  
  LOG_INFO("Stopping data collector... ");
  dataCollector->Stop();
  
  return BC_EXIT_SUCCESS;
}

