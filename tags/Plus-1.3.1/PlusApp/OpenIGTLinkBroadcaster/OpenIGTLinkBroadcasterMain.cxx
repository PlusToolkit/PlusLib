
#include "PlusConfigure.h"

#include "vtkSmartPointer.h"
#include "vtksys/CommandLineArguments.hxx"



#include "vtkOpenIGTLinkBroadcaster.h"


#include "OpenIGTLinkBroadcasterWidget.h"
#include <QtGui/QApplication>



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
  int         verboseLevel = vtkPlusLogger::LOG_LEVEL_DEFAULT;
  
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
    std::cout << "Help: " << args.GetHelp() << std::endl;
    return 1;
  }
  
  
    // Set the logging level.
  
  vtkPlusLogger::Instance()->SetLogLevel( verboseLevel );
  
  
  
  
  
  
  QApplication app(argc, argv);

	OpenIGTLinkBroadcasterWidget mainWidget;
	mainWidget.Initialize( inputConfigFileName, inputVideoBufferMetafile, inputTrackerBufferMetafile );
	
	mainWidget.show();
  
	return app.exec();
  
  
  // return 0;
}

