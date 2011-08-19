
#include "PlusConfigure.h"

#include <iostream>
#include <string>

#include "vtkSmartPointer.h"
#include "vtksys/CommandLineArguments.hxx"

#include "igtlImageMessage.h"
#include "igtlMessageHeader.h"
#include "igtlServerSocket.h"
#include "igtlTransformMessage.h"

#include "igtlStringMessage1.h"



/**
 * Operates Plus functionalities in OpenIGTLink server mode
 * based on client requests.
 */
int main( int argc, char** argv )
{
  
    // Define command line arguments.
  
  std::string inputConfigFileName;
  std::string inputVideoBufferMetafile;
  std::string inputTrackerBufferMetafile;
  int         verboseLevel = PlusLogger::LOG_LEVEL_INFO;
  
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
  
  PlusLogger::Instance()->SetLogLevel( verboseLevel );
  PlusLogger::Instance()->SetDisplayLogLevel( verboseLevel );
  
  
  return 0;
}
