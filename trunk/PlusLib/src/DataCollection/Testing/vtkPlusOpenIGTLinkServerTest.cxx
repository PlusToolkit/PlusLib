
#include <iostream>

#include "vtkSmartPointer.h"
#include "vtksys/CommandLineArguments.hxx"

#include "PlusConfigure.h"
#include "vtkDataCollector.h"
#include "vtkSavedDataTracker.h"
#include "vtkSavedDataVideoSource.h"

#include "vtkPlusOpenIGTLinkServer.h"



int main( int argc, char** argv )
{
  
    // Check command line arguments.
  
  std::string  InputConfigFileName;
  std::string  InputVideoBufferMetafile;
  std::string  InputTrackerBufferMetafile;
  int          Port = 0;
  int          VerboseLevel = PlusLogger::LOG_LEVEL_INFO;
  
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
  
  PlusLogger::Instance()->SetLogLevel( VerboseLevel );
  PlusLogger::Instance()->SetDisplayLogLevel( VerboseLevel );
  
  
    // Prepare data collector object.
  
  vtkDataCollector* dataCollector = vtkDataCollector::New();
  dataCollector->ReadConfigurationFromFile( InputConfigFileName.c_str() );
  
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
  
  LOG_DEBUG("Initializing data collector... ");
  dataCollector->Initialize();
  
  
    // Create a server.
  
  LOG_DEBUG("Initializing server... ");
  vtkSmartPointer< vtkPlusOpenIGTLinkServer > server = vtkSmartPointer< vtkPlusOpenIGTLinkServer >::New();
  server->SetDataCollector( dataCollector );
  
  
    // Create a client to connect to the server.
  
  
  
  
  return 0;
}
