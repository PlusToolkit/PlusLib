#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "BrachyStepper.h"
#include "CivcoBrachyStepper.h"
#include <stdlib.h>
#include <iostream>

int main (int argc, char* argv[])
{
	bool printHelp(false); 
	bool resetStepper(false); 
	int inputComPort(2);
	int inputNumberOfTrials(20); 
	int samplingTimeMs(50);

	int verboseLevel=vtkPlusLogger::LOG_LEVEL_INFO;

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
	args.AddArgument("--reset", vtksys::CommandLineArguments::NO_ARGUMENT, &resetStepper, "Reset the stepper.");	
	args.AddArgument("--com-port", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputComPort, "Com port number (Default: 2)." );
	args.AddArgument("--number-of-trials", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputNumberOfTrials, "Number of trials (Default: 20)." );
	args.AddArgument("--sampling-time", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &samplingTimeMs, "Sampling time in milliseconds (Default: 20)." );
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (Default: 3; 1=error only, 2=warning, 3=info, 4=debug.)");	

	if ( !args.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "\n\nCivcoBrachyStepperTest help:" << args.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

	if ( printHelp ) 
	{
		std::cout << "\n\nCivcoBrachyStepperTest help:" << args.GetHelp() << std::endl;
		exit(EXIT_SUCCESS); 

	}

	vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);
  vtkPlusLogger::Instance()->SetDisplayLogLevel(verboseLevel);


	for ( int reconnect = 0; reconnect < 5; reconnect++ )
	{
		LOG_INFO("Starting test " << reconnect + 1 ); 

		CivcoBrachyStepper *myStepper = new CivcoBrachyStepper(inputComPort, 19200);

		if ( myStepper->Connect() != PLUS_SUCCESS )
		{
			LOG_ERROR("Couldn't connect to tracker!");
			return EXIT_FAILURE; 
		}


		if ( resetStepper && reconnect == 0)
		{
			LOG_INFO( "Resetting stepper..."); 
			if (!myStepper->ResetStepper())
			{
				resetStepper = false; 
				LOG_ERROR("Wrong answer received!");
				return EXIT_FAILURE; 
			}
		}

		LOG_INFO( "Getting stepper's device info...");

    std::string version; 
    std::string model; 
    std::string serial; 
		if (myStepper->GetDeviceModelInfo(version, model, serial) == PLUS_SUCCESS )
		{
			LOG_INFO("Version = " << version << "\tModel = " << model << "\tSerial = " << serial ); 
		}
		else
		{
			LOG_ERROR("Wrong answer received!");
			return EXIT_FAILURE; 
		}


		LOG_INFO( "Getting stepper's encoder values...");
		for (int i = 0; i<inputNumberOfTrials; i++)
		{
			double pposition=0, gposition=0, rposition=0;
			unsigned long positionRequestNumber(0); 
			if (myStepper->GetEncoderValues(pposition, gposition, rposition, positionRequestNumber))
			{
				LOG_INFO("Probe = " << pposition << " mm\tGrid = " << gposition << " mm\tRotate = " << rposition << " deg\tRequestNumber = " << positionRequestNumber);
			}
			else
			{
				LOG_ERROR("Wrong answer received!");
				//return EXIT_FAILURE; 
			}

			vtkAccurateTimer::Delay(0.001*samplingTimeMs); 
		}

		LOG_INFO( "Stopping stepper...\n");
		myStepper->Disconnect(); 
	}

	LOG_INFO( "Exit successfully...");

	return EXIT_SUCCESS;
} 