
#include "PlusConfigure.h"

#include "vtkSmartPointer.h"
#include "vtksys/CommandLineArguments.hxx"

#include "vtkDataCollector.h"
#include "vtkSavedDataTracker.h"
#include "vtkSavedDataVideoSource.h"
#include "vtkTracker.h"
#include "vtkTrackerTool.h"

#include "vtkOpenIGTLinkBroadcaster.h"



/**
 * @returns 0 on success, not 0 on error.
 */
int
ProcessBroadcasterStatus( vtkOpenIGTLinkBroadcaster::Status broadcasterStatus, std::string errorMessage )
{
  switch ( broadcasterStatus )
    {
    
    case vtkOpenIGTLinkBroadcaster::STATUS_OK:
      break;
    
    case vtkOpenIGTLinkBroadcaster::STATUS_HOST_NOT_FOUND:
      LOG_ERROR( "Could not connect to host: " << errorMessage );
      return 1;
    
    case vtkOpenIGTLinkBroadcaster::STATUS_MISSING_DEFAULT_TOOL:
      LOG_ERROR( "Default tool not defined in configuration." );
      return 1;
    
    case vtkOpenIGTLinkBroadcaster::STATUS_NOT_INITIALIZED:
    default:
      LOG_ERROR( "Couldn't initialize broadcaster for an unknown reason." );
      return 1;
    }
  
  return 0;
}



/**
 * @returns 0 on normal tracking.
 */
int
ProcessTrackerStatus( TrackerStatus status )
{
  if ( status == TR_OK )
    {
    return 0;
    }
  else if ( status == TR_MISSING || status == TR_OUT_OF_VIEW )
    {
    LOG_INFO( "Tracker data missing." );
    return 1;
    }
  else if ( status == TR_REQ_TIMEOUT )
    {
    LOG_INFO( "Tracker requested timeout." );
    return 1;
    }
  else
    {
    LOG_INFO( "Unknown tracker status." );
    return 1;
    }
}







/**
 * This program implements broadcasting image and tracking data collected by
 * vtkDataCollector to OpenIGTLink servers. This is an OpenIGTLink client.
 */
int main( int argc, char *argv[] )
{
    
    // Define command line arguments.
  
  std::string inputConfigFileName;
  std::string inputVideoBufferMetafile;
  std::string inputTrackerBufferMetafile;
  int         verboseLevel = vtkPlusLogger::LOG_LEVEL_INFO;
  
  vtksys::CommandLineArguments args;
  args.Initialize( argc, argv );
  
  args.AddArgument( "--input-config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT,
                      &inputConfigFileName, "Name of the input configuration file." );
  args.AddArgument( "--input-video-buffer-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT,
                      &inputVideoBufferMetafile, "Video buffer sequence metafile." );
  args.AddArgument( "--input-tracker-buffer-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT,
                      &inputTrackerBufferMetafile, "Tracker buffer sequence metafile." );
  args.AddArgument( "--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT,
                      &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug 5=trace)" );  
  
  
  
    // Try to parse command line arguments.
  
  if ( ! args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    return 1;  // PLUS_FAIL was 0, so it was not used here.
  }
  
  
    // Check command line arguments.
  
  if ( inputConfigFileName.empty() )
  {
    std::cerr << "input-config-file is required" << std::endl;
    return 1;
  }
  
  
    // Set the logging level.
  
  vtkPlusLogger::Instance()->SetLogLevel( verboseLevel );
  vtkPlusLogger::Instance()->SetDisplayLogLevel( verboseLevel );
  
  
  
    // Prepare data collector object.
  
  vtkSmartPointer< vtkDataCollector > dataCollector = vtkSmartPointer< vtkDataCollector >::New();
  dataCollector->ReadConfigurationFromFile( inputConfigFileName.c_str() );
  
  if ( dataCollector->GetAcquisitionType() == SYNCHRO_VIDEO_SAVEDDATASET )
    {
    vtkSavedDataVideoSource* videoSource = static_cast< vtkSavedDataVideoSource* >( dataCollector->GetVideoSource() );
    
    if ( ! inputVideoBufferMetafile.empty() )
      {
      videoSource->SetSequenceMetafile( inputVideoBufferMetafile.c_str() );
      }
    }
  
  if ( dataCollector->GetTrackerType() == TRACKER_SAVEDDATASET )
    {
    vtkSavedDataTracker* tracker = static_cast< vtkSavedDataTracker* >( dataCollector->GetTracker() );
      
    if ( ! inputTrackerBufferMetafile.empty() )
      {
      tracker->SetSequenceMetafile( inputTrackerBufferMetafile.c_str() );
      }
    }
  
  
  LOG_INFO( "Initializing data collector." );
  dataCollector->Initialize();
  
  
    // Prepare the OpenIGTLink broadcaster.
  
  vtkOpenIGTLinkBroadcaster::Status broadcasterStatus = vtkOpenIGTLinkBroadcaster::STATUS_NOT_INITIALIZED;
  vtkSmartPointer< vtkOpenIGTLinkBroadcaster > broadcaster = vtkSmartPointer< vtkOpenIGTLinkBroadcaster >::New();
  broadcaster->SetDataCollector( dataCollector );
  
  std::string errorMessage;
  broadcasterStatus = broadcaster->Initialize( errorMessage );
  int error = ProcessBroadcasterStatus( broadcasterStatus, errorMessage );
  if ( error != 0 ) return error;
  
    
  
    // Determine delay from frequency for the tracker.
  
  double delayTracking = 1.0 / dataCollector->GetTracker()->GetFrequency();
  
  LOG_INFO( "Tracker frequency = " << dataCollector->GetTracker()->GetFrequency() );
  LOG_DEBUG( "Tracker delay = " << delayTracking );
  
  
    // Start data collection and broadcasting.
  
  LOG_INFO( "Start data collector..." );
  dataCollector->Start();
  
  int defaultTool = dataCollector->GetTracker()->GetFirstPortNumberByType(TRACKER_TOOL_PROBE);

  unsigned int numBroadcastedMessages = UINT_MAX;
  
  if (    dataCollector->GetTrackerType() == TRACKER_SAVEDDATASET
       && dataCollector->GetTrackerType() == SYNCHRO_VIDEO_SAVEDDATASET )
    {
      numBroadcastedMessages = dataCollector->GetTracker()->GetTool( defaultTool )->GetBuffer()->GetBufferSize();
    }
  
  
  for ( int i = 0; i < numBroadcastedMessages; ++ i )
    {
    vtkAccurateTimer::Delay( delayTracking );
    
    LOG_DEBUG( "Iteration = " << i );
    
    vtkSmartPointer< vtkMatrix4x4 > mToolToReference = vtkSmartPointer< vtkMatrix4x4 >::New();
    
    if ( dataCollector->GetTracker()->IsTracking() )
      {
      double timeTracker = 0.0;
      TrackerStatus status = TR_OK;
      dataCollector->GetTransformWithTimestamp(
        mToolToReference, timeTracker, status, defaultTool );
      int error = ProcessTrackerStatus( status );
      if ( error == 0 )
        {
        LOG_INFO( "Tool position: " << mToolToReference->GetElement( 0, 3 ) << " "
                                    << mToolToReference->GetElement( 1, 3 ) << " "
                                    << mToolToReference->GetElement( 2, 3 ) << " " );
        }
      }
    else
      {
      LOG_INFO( "Unable to connect to tracker." );
      }
    
    broadcasterStatus = broadcaster->SendMessages( errorMessage );
    int error = ProcessBroadcasterStatus( broadcasterStatus, errorMessage );
    if ( error != 0 )
      {
      LOG_INFO( "Unable to broadcast. Stopping application." );
      break;
      }
    }
  
  
  LOG_INFO( "Stopping data collector." );
  dataCollector->Stop();
  
  return 0;
}

