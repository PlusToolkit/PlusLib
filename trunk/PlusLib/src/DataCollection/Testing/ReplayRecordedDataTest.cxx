/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
\file This program tests if a recorded tracked ultrasound buffer can be read.
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

  vtkDataCollectorHardwareDevice* dataCollectorHardwareDevice = dynamic_cast<vtkDataCollectorHardwareDevice*>(dataCollector.GetPointer());
  if ( dataCollectorHardwareDevice == NULL )
  {
    LOG_ERROR("Failed to create the propertype of data collector!");
    exit( 1 );
  }

  dataCollectorHardwareDevice->ReadConfiguration( configRootElement );

  if ( dataCollectorHardwareDevice->GetAcquisitionType() == SYNCHRO_VIDEO_SAVEDDATASET )
  {
    if ( inputVideoBufferMetafile.empty() )
    {
      LOG_ERROR( "Video source metafile missing." );
      return 1;
    }

    vtkSavedDataVideoSource* videoSource =
      dynamic_cast< vtkSavedDataVideoSource* >( dataCollectorHardwareDevice->GetVideoSource() );
    if ( videoSource == NULL )
    {
      LOG_ERROR( "Invalid saved data video source." );
      exit( 1 );
    }
    videoSource->SetSequenceMetafile( inputVideoBufferMetafile.c_str() );
    videoSource->SetReplayEnabled( true ); 
  }

  if ( dataCollectorHardwareDevice->GetTrackerType() == TRACKER_SAVEDDATASET )
  {
    if ( inputTrackerBufferMetafile.empty() )
    {
      LOG_ERROR( "Tracker source metafile missing." );
      return 1;
    }
    vtkSavedDataTracker* tracker = static_cast< vtkSavedDataTracker* >( dataCollectorHardwareDevice->GetTracker() );
    tracker->SetSequenceMetafile( inputTrackerBufferMetafile.c_str() );
    tracker->SetReplayEnabled( true ); 
    tracker->Connect();
  }

  LOG_DEBUG( "Initializing data collector... " );
  dataCollectorHardwareDevice->Connect();

  // TODO: Check if the read transforms are really the same as in the ones recorded in the data file.

  dataCollectorHardwareDevice->Disconnect();
  
  return 0;
}
