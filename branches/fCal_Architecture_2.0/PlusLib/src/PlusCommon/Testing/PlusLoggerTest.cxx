#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"

int main(int argc, char **argv)
{
  VTK_LOG_TO_CONSOLE_ON; 

	bool printHelp(false);

	int verboseLevel = PlusLogger::LOG_LEVEL_INFO;

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	
	
	if ( !args.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << args.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

	if ( printHelp ) 
	{
		std::cout << "Help: " << args.GetHelp() << std::endl;
		exit(EXIT_SUCCESS); 

	}

	PlusLogger::Instance()->SetLogLevel(verboseLevel);
  PlusLogger::Instance()->SetDisplayLogLevel(verboseLevel);

  std::cout << "Verbose level: " << verboseLevel << std::endl;

  LOG_ERROR("This is a test error message");
  LOG_WARNING("This is a test warning message");
  LOG_INFO("This is a test info message");
  LOG_DEBUG("This is a test debug message");
  LOG_TRACE("This is a test trace message");

	return EXIT_SUCCESS; 
 }
