/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkPlusOpenHapticsDevice.h"
#include "vtkSmartPointer.h"
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"

void PrintLogsCallback(vtkObject* obj, unsigned long eid, void* clientdata, void* calldata);


int main(int argc, char **argv)
{
  std::string deviceName = "Plus";
  double frameRate=20;
  bool printHelp(false);

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  args.AddArgument("--device-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &deviceName, "Name of the OpenHaptics device to connect");
  args.AddArgument("--frame-rate", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &frameRate, "Requested acquisition frame rate (in FPS, default = 30)");
  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "\n\nvtkPlusOpenHapticsDeviceTest help:" << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if ( printHelp )
  {
    std::cout << "\n\nvtkPlusOpenHapticsDeviceTest help:" << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS);
  }

  vtkSmartPointer<vtkPlusOpenHapticsDevice> openHapticsDevice = vtkSmartPointer<vtkPlusOpenHapticsDevice>::New();

  openHapticsDevice->SetAcquisitionRate(frameRate);
  openHapticsDevice->SetDeviceName(deviceName);
  // Add an observer to warning and error events for redirecting it to the stdout
  vtkSmartPointer<vtkCallbackCommand> callbackCommand = vtkSmartPointer<vtkCallbackCommand>::New();
  callbackCommand->SetCallback(PrintLogsCallback);
  openHapticsDevice->AddObserver("WarningEvent", callbackCommand);
  openHapticsDevice->AddObserver("ErrorEvent", callbackCommand);

  LOG_INFO("Initialize...");

  if ( openHapticsDevice->Connect()!=PLUS_SUCCESS )
  {
    LOG_ERROR( "Unable to connect to device");
    exit(EXIT_FAILURE);
  }

  LOG_INFO("Start recording...");
  if ( openHapticsDevice->StartRecording()!=PLUS_SUCCESS )
  {
    LOG_ERROR( "Unable to connect to device");
    exit(EXIT_FAILURE);
  }


  openHapticsDevice->StopRecording();
  openHapticsDevice->Disconnect();
  LOG_INFO("Exit successfully");
  exit(EXIT_SUCCESS);
}

// Callback function for error and warning redirects
void PrintLogsCallback(vtkObject* obj, unsigned long eid, void* clientdata, void* calldata)
{
  if (eid == vtkCommand::GetEventIdFromString("WarningEvent"))
  {
    LOG_WARNING((const char*)calldata);
  }
  else if (eid == vtkCommand::GetEventIdFromString("ErrorEvent"))
  {
    LOG_ERROR((const char*)calldata);
  }
}
