#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "BrachyStepper.h"
#include "CmsBrachyStepper.h"
#include <stdlib.h>
#include <iostream>

int main (int argc, char* argv[])
{
	bool printHelp(false); 
	bool resetStepper(false); 
	std::string stepperType("Burdette Medical Systems Digital Stepper"); 
	std::string inputComPort("COM2");
	int inputNumberOfTrials(20); 
	int samplingTimeMs(50);

	int verboseLevel=PlusLogger::LOG_LEVEL_INFO;

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
	args.AddArgument("--reset", vtksys::CommandLineArguments::NO_ARGUMENT, &resetStepper, "Reset the stepper (need's to calibrate it).");	
	args.AddArgument("--com-port", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputComPort, "Com port number (Default: COM2)." );
	args.AddArgument("--number-of-trials", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputNumberOfTrials, "Number of trials (Default: 20)." );
	args.AddArgument("--sampling-time", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &samplingTimeMs, "Sampling time in milliseconds (Default: 20)." );
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (Default: 3; 1=error only, 2=warning, 3=info, 4=debug.)");	
	args.AddArgument("--brachy-stepper-type", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &stepperType, "Set brachy stepper type (Default: Burdette Medical Systems Digital Stepper; Burdette Medical Systems Digital Stepper, Burdette Medical Systems Digital Motorized Stepper, CMS Accuseed DS300.)");	

	if ( !args.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "\n\nCmsBrachyStepperTest help:" << args.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

	if ( printHelp ) 
	{
		std::cout << "\n\nCmsBrachyStepperTest help:" << args.GetHelp() << std::endl;
		exit(EXIT_SUCCESS); 

	}

	PlusLogger::Instance()->SetLogLevel(verboseLevel);


	for ( int reconnect = 0; reconnect < 5; reconnect++ )
	{
		LOG_INFO("Starting test " << reconnect + 1 ); 

		CmsBrachyStepper *myStepper = new CmsBrachyStepper(inputComPort.c_str(), 19200);

		if ( STRCASECMP("Burdette Medical Systems Digital Stepper", stepperType.c_str()) == 0 )
		{
			myStepper->SetBrachyStepperType(BrachyStepper::BURDETTE_MEDICAL_SYSTEMS_DIGITAL_STEPPER); 
		}
		else if ( STRCASECMP("Burdette Medical Systems Digital Motorized Stepper", stepperType.c_str()) == 0 )
		{
			myStepper->SetBrachyStepperType(BrachyStepper::BURDETTE_MEDICAL_SYSTEMS_DIGITAL_MOTORIZED_STEPPER); 
		}
		else if ( STRCASECMP("CMS Accuseed DS300", stepperType.c_str()) == 0 )
		{
			myStepper->SetBrachyStepperType(BrachyStepper::CMS_ACCUSEED_DS300); 
		}

		if (!myStepper->StartTracking())
		{
			LOG_ERROR("Couldn't start tracking!");
			//return EXIT_FAILURE; 
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


		LOG_INFO( "Checking stepper's calibration state...");
		if (!myStepper->IsStepperCalibrated())
		{
			std::cout << std::endl << "The stepper needs to be calibrated!" << std::endl << std::endl;
			std::string CalibMsg;
			while(!myStepper->CalibrateStepper(CalibMsg))
			{
				std::cout << CalibMsg << " ";
				system("pause"); 
			}
		}

		LOG_INFO( "Getting stepper's calibration state...");
		int pState(0), gState(0), rState(0);
		if (myStepper->GetCalibrationState(pState, gState, rState))
		{
			std::cout << "Probe state = " << pState << "  Grid state = " << gState  << "  Rotation state = " << rState << std::endl;
		}
		else
		{
			LOG_ERROR("Wrong answer received!");
			return EXIT_FAILURE; 
		}

		LOG_INFO( "Getting stepper's rotate state...");
		int State=-1;
		if (myStepper->GetRotateState(State))
		{
			std::cout << "Rotate state = " << State << std::endl;
		}
		else
		{
			LOG_ERROR("Wrong answer received!");
			return EXIT_FAILURE; 
		}

		LOG_INFO( "Getting motorized stepper's tick count...");
		int MotorizationCode=-1;
		if (myStepper->GetMotorizationCode(MotorizationCode)) {
			if (MotorizationCode > 0)
			{
				std::cout << "Tick count = " << MotorizationCode << std::endl;
			}
			else
			{
				std::cout << "Stepper is not motorized" << std::endl;
				//LOG_ERROR("Wrong answer received!");
				//return EXIT_FAILURE; 
			}
		}
		else
		{
			LOG_ERROR("Wrong answer received!");
			return EXIT_FAILURE; 
		}

		LOG_INFO( "Getting stepper's status info...");
		unsigned int Status=-1;
		if (myStepper->GetStatusInfo(Status))
		{
			std::cout << "Status: " << std::hex << Status << std::dec << std::endl;
		}
		else
		{
			LOG_ERROR("Wrong answer received!");
			return EXIT_FAILURE; 
		}


		LOG_INFO( "Getting stepper's version info...");
		int iVerHi=0; int iVerLo=0; int iModelNum=0; int iSerialNum=0;
		if (myStepper->GetVersionInfo(iVerHi, iVerLo, iModelNum, iSerialNum))
		{
			std::cout << "Version = " << iVerHi << "." << iVerLo << "\tModel = " << iModelNum << "\tSerialno = " << iSerialNum << std::endl;
		}
		else
		{
			LOG_ERROR("Wrong answer received!");
			return EXIT_FAILURE; 
		}

		LOG_INFO( "Getting stepper's probe reference data...");
		double count = 0; double dist = 0; double scale = 0;
		if (myStepper->GetProbeReferenceData(count, dist, scale))
		{
			std::cout << "Count = " << count << "\tDist = " << dist << "\tScale = " << scale << " mm" << std::endl;
		}
		else
		{
			LOG_ERROR("Wrong answer received!");
			return EXIT_FAILURE; 
		}

		LOG_INFO( "Getting stepper's grid reference data...");
		if (myStepper->GetGridReferenceData(count, dist, scale))
		{
			std::cout << "Count = " << count << "\tDist = " << dist << "\tScale = " << scale << " mm" << std::endl;
		}
		else
		{
			LOG_ERROR("Wrong answer received!");
			return EXIT_FAILURE; 
		}

		LOG_INFO( "Getting stepper's rotation reference data...");
		if (myStepper->GetRotationReferenceData(count, dist, scale))
		{
			std::cout << "Count = " << count << "\tDist = " << dist << "\tScale = " << scale << " deg" << std::endl;
		}
		else
		{
			LOG_ERROR("Wrong answer received!");
			return EXIT_FAILURE; 
		}

		if ( myStepper->IsStepperMotorized() )
		{
			LOG_INFO( "Testing motorized stepper..");
			for ( int i = 1; i <= 10; i++ )
			{
				int retCode; 
				myStepper->MoveProbeToPosition( -5 * i , retCode); 
				if ( retCode != 0 )
				{
					LOG_ERROR("Unable to move probe to position: " << -5*i << "mm"); 
				}
				else
				{
					LOG_INFO( "Probe moved to position: " << -5*i << "mm");
				}
				vtkAccurateTimer::GetInstance()->Delay(1); 
				//vtkAccurateTimer::Delay(1 + 0.001*samplingTimeMs); 
			}

			for ( int i = 9; i >= 0; i-- )
			{
				int retCode; 
				myStepper->MoveProbeToPosition( -5 * i , retCode); 
				if ( retCode != 0 )
				{
					LOG_ERROR("Unable to move probe to position: " << -5*i << "mm"); 
				}
				else
				{
					LOG_INFO( "Probe moved to position: " << -5*i << "mm");
				}
				vtkAccurateTimer::GetInstance()->Delay(1); 
				//vtkAccurateTimer::Delay(1 + 0.001*samplingTimeMs); 
			}
		}


		LOG_INFO( "Getting stepper's probe position...");
		for (int i = 0; i<inputNumberOfTrials; i++)
		{
			double pposition=0, gposition=0, rposition=0;
			unsigned long positionRequestNumber(0); 
			if (myStepper->GetProbePositions(pposition, gposition, rposition, positionRequestNumber))
			{
				std::cout << "Probe = " << pposition << " mm\tGrid = " << gposition << " mm\tRotate = " << rposition << " deg\tRequestNumber = " << positionRequestNumber << std::endl << std::flush;
			}
			else
			{
				LOG_ERROR("Wrong answer received!");
				//return EXIT_FAILURE; 
			}

			vtkAccurateTimer::Delay(0.001*samplingTimeMs); 
		}

		LOG_INFO( "Stopping stepper...\n");
		myStepper->StopTracking(); 
	}

	LOG_INFO( "Exit successfully...");

	return EXIT_SUCCESS;
} 