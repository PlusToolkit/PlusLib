/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
  \file ReplayRecordedDataTest.cxx
  \brief This program tests if a recorded tracked ultrasound buffer can be read.
*/ 

#include "vtkSmartPointer.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkXMLUtilities.h"

#include "PlusConfigure.h"
#include "vtkDataCollectorHardwareDevice.h"
#include "vtkSavedDataTracker.h"
#include "vtkSavedDataVideoSource.h"


int main( int argc, char** argv )
{

  // Check command line arguments.

  std::string  inputConfigFileName;
  std::string  inputVideoBufferMetafile;
  std::string  inputTrackerBufferMetafile;
  int          verboseLevel = vtkPlusLogger::LOG_LEVEL_DEFAULT;

  vtksys::CommandLineArguments args;
  args.Initialize( argc, argv );

  args.AddArgument( "--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT,
    &inputConfigFileName, "Name of the input configuration file." );
  args.AddArgument( "--input-video-buffer-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT,
    &inputVideoBufferMetafile, "Video buffer sequence metafile." );
  args.AddArgument( "--input-tracker-buffer-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT,
    &inputTrackerBufferMetafile, "Tracker buffer sequence metafile." );
  args.AddArgument( "--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, 
    &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug 5=trace)" );  

  if ( ! args.Parse() )
  {
    std::cerr << "Problem parsing arguments." << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    return EXIT_FAILURE;
  }

  vtkPlusLogger::Instance()->SetLogLevel( verboseLevel );

  // Prepare data collector object.
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(
    vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str()));
  if (configRootElement == NULL)
  {	
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str()); 
    return EXIT_FAILURE;
  }

  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement);

  vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New();

  vtkDataCollectorHardwareDevice* dataCollectorHardwareDevice = dynamic_cast<vtkDataCollectorHardwareDevice*>(dataCollector.GetPointer());
  if ( dataCollectorHardwareDevice == NULL )
  {
    LOG_ERROR("Failed to create the propertype of data collector!");
    return EXIT_FAILURE;
  }

  dataCollectorHardwareDevice->ReadConfiguration( configRootElement );

  if ( !inputVideoBufferMetafile.empty() )
  {
    vtkSavedDataVideoSource* videoSource =
      dynamic_cast< vtkSavedDataVideoSource* >( dataCollectorHardwareDevice->GetVideoSource() );
    if ( videoSource == NULL )
    {
      LOG_ERROR( "Invalid saved data video source." );
      return EXIT_FAILURE;
    }
    videoSource->SetSequenceMetafile( inputVideoBufferMetafile.c_str() );
    videoSource->SetReplayEnabled( true ); 
  }

  if ( !inputTrackerBufferMetafile.empty() )
  {
    vtkSavedDataTracker* tracker = dynamic_cast< vtkSavedDataTracker* >( dataCollectorHardwareDevice->GetTracker() );
    if ( tracker == NULL )
    {
      LOG_ERROR( "Invalid saved data tracker source." );
      return EXIT_FAILURE;
    }
    tracker->SetSequenceMetafile( inputTrackerBufferMetafile.c_str() );
    tracker->SetReplayEnabled( true ); 
  }

  LOG_DEBUG( "Initializing data collector... " );
  dataCollectorHardwareDevice->Connect();

  // TODO: Check if the read transforms are really the same as in the ones recorded in the data file.

  dataCollectorHardwareDevice->Disconnect();
  
  return EXIT_SUCCESS;
}
