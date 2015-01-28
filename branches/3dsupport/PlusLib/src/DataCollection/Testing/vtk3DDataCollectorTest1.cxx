/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
  \file vtk3DDataCollectorTest1.cxx 
  \brief This program acquires tracked ultrasound data and displays it on the screen (in a 2D viewer).
*/ 

#include "PlusConfigure.h"
#include "TrackedFrame.h"
#include "vtkDataCollector.h"
#include "vtkImageData.h" 
#include "vtkMatrix4x4.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusDevice.h"
#include "vtkSavedDataSource.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx"

int main(int argc, char **argv)
{
  std::string inputConfigFileName;

  int verboseLevel=vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Name of the input configuration file.");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");  

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }
  
  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if (inputConfigFileName.empty())
  {
    std::cerr << "input-config-file-name is required" << std::endl;
    exit(EXIT_FAILURE);
  }  

  ///////////////

  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
  if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, inputConfigFileName.c_str())==PLUS_FAIL)
  {  
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str()); 
    return EXIT_FAILURE;
  }
  
  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement);

  vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New(); 

  if( dataCollector->ReadConfiguration( configRootElement ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Configuration incorrect for vtkDataCollectorTest1.");
    exit( EXIT_FAILURE );
  }
  vtkPlusDevice* videoDevice = NULL;
  vtkPlusDevice* trackerDevice = NULL;

  if( dataCollector->GetDevice(videoDevice, "VideoDevice") != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to locate the device with Id=\"VideoDevice\". Check config file.");
    exit(EXIT_FAILURE);
  }
  vtkSavedDataSource* videoSource = dynamic_cast<vtkSavedDataSource*>(videoDevice); 
  if ( videoSource == NULL )
  {
    LOG_ERROR( "Unable to cast video source to vtkSavedDataSource." );
    exit( EXIT_FAILURE );
  }

  if( dataCollector->GetDevice(trackerDevice, "TrackerDevice") != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to locate the device with Id=\"TrackerDevice\". Check config file.");
    exit(EXIT_FAILURE);
  }
  vtkSavedDataSource* tracker = dynamic_cast<vtkSavedDataSource*>(trackerDevice); 
  if ( tracker == NULL )
  {
    LOG_ERROR( "Unable to cast tracker to vtkSavedDataSource" );
    exit( EXIT_FAILURE );
  }

  if ( dataCollector->Connect() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to connect to devices!" ); 
    exit( EXIT_FAILURE );
  }

  if ( dataCollector->Start() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to start data collection!" ); 
    exit( EXIT_FAILURE );
  }

  dataCollector->Disconnect();

  std::cout << "vtk3DDataCollectorTest1 completed successfully!" << std::endl;
  return EXIT_SUCCESS; 

}
