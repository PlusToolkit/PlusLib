/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

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
  int inputComPort(2);
  int inputNumberOfTrials(20); 
  int samplingTimeMs(50);

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");  
  args.AddArgument("--reset", vtksys::CommandLineArguments::NO_ARGUMENT, &resetStepper, "Reset the stepper (need's to calibrate it).");  
  args.AddArgument("--com-port", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputComPort, "Com port number (Default: 2)." );
  args.AddArgument("--number-of-trials", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputNumberOfTrials, "Number of trials (Default: 20)." );
  args.AddArgument("--sampling-time", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &samplingTimeMs, "Sampling time in milliseconds (Default: 20)." );
  args.AddArgument("--brachy-stepper-type", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &stepperType, "Set brachy stepper type (Default: Burdette Medical Systems Digital Stepper; Burdette Medical Systems Digital Stepper, Burdette Medical Systems Digital Motorized Stepper, CMS Accuseed DS300.)");  
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");  

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "\n\nCmsBrachyStepperTest help:" << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }
  
  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if ( printHelp ) 
  {
    std::cout << "\n\nCmsBrachyStepperTest help:" << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS); 

  }  

  for ( int reconnect = 0; reconnect < 5; reconnect++ )
  {
    LOG_INFO("Starting test " << reconnect + 1 ); 

    CmsBrachyStepper *myStepper = new CmsBrachyStepper(inputComPort, 19200);

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

    if (!myStepper->Connect())
    {
      LOG_ERROR("Couldn't connect to tracker!");
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
      std::cout << std::endl << "The stepper needs to be initialized!" << std::endl << std::endl;
      std::string CalibMsg;
      while(!myStepper->InitializeStepper(CalibMsg))
      {
        std::cout << CalibMsg << " ";
        system("pause"); 
      }
    }

    LOG_INFO( "Getting stepper's calibration state...");
    int pState(0), gState(0), rState(0);
    if (myStepper->GetCalibrationState(pState, gState, rState))
    {
      LOG_INFO("Probe state = " << pState << "  Grid state = " << gState  << "  Rotation state = " << rState );
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
      LOG_INFO("Rotate state = " << State);
    }
    else
    {
      LOG_ERROR("Wrong answer received!");
      return EXIT_FAILURE; 
    }

    LOG_INFO( "Getting motorized stepper's tick count...");
    int MotorizationScaleFactor=-1;
    if (myStepper->GetMotorizationScaleFactor(MotorizationScaleFactor)) {
      if (MotorizationScaleFactor > 0)
      {
        LOG_INFO("Tick count = " << MotorizationScaleFactor);
      }
      else
      {
        LOG_INFO("Stepper is not motorized"); 
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
      LOG_INFO("Status: " << std::hex << Status << std::dec);
    }
    else
    {
      LOG_ERROR("Wrong answer received!");
      return EXIT_FAILURE; 
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

    LOG_INFO( "Getting stepper's probe reference data...");
    double count = 0; double dist = 0; double scale = 0;
    if (myStepper->GetProbeReferenceData(count, dist, scale))
    {
      LOG_INFO("Count = " << count << "\tDist = " << dist << "\tScale = " << scale << " mm" );
    }
    else
    {
      LOG_ERROR("Wrong answer received!");
      return EXIT_FAILURE; 
    }

    LOG_INFO( "Getting stepper's grid reference data...");
    if (myStepper->GetGridReferenceData(count, dist, scale))
    {
      LOG_INFO("Count = " << count << "\tDist = " << dist << "\tScale = " << scale << " mm");
    }
    else
    {
      LOG_ERROR("Wrong answer received!");
      return EXIT_FAILURE; 
    }

    LOG_INFO( "Getting stepper's rotation reference data...");
    if (myStepper->GetRotationReferenceData(count, dist, scale))
    {
      LOG_INFO("Count = " << count << "\tDist = " << dist << "\tScale = " << scale << " deg" );
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
