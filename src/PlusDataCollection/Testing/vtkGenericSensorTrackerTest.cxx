/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkCallbackCommand.h"
#include "vtkCommand.h"
#include "vtkPlusDataSource.h"
#include "vtkSmartPointer.h"
#include "vtkPlusGenericSensorTracker.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtksys/SystemTools.hxx"
#include "igtlOSUtil.h"

void PrintLogsCallback(vtkObject* obj, unsigned long eid, void* clientdata, void* calldata);

int main(int argc, char** argv)
{

  bool printHelp(false);
  std::string inputConfigFileName = "Testing/PlusDeviceSet_DataCollectionOnly_GenericSensor.xml";

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;
  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");
  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Config file containing the device configuration.");

  if (!args.Parse())
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "\n\nHelp:" << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if (printHelp)
  {
    std::cout << "\n\nHelp:" << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS);
  }

  vtkNew<vtkPlusGenericSensorTracker> sensorTracker;
  sensorTracker->SetDeviceId("TrackerDevice");
  if (!vtksys::SystemTools::FileExists(inputConfigFileName))
  {
    LOG_ERROR("Bad configuration file: " << inputConfigFileName);
    exit(EXIT_FAILURE);
  }

  LOG_DEBUG("Reading config file: " << inputConfigFileName);
  auto* configRead = vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str());

  if (!configRead)
  {
    LOG_ERROR("Failed to read configuration file");
    exit(EXIT_FAILURE);
  }

  LOG_TRACE("Config file: " << *configRead);
  if (sensorTracker->ReadConfiguration(configRead) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to read configuration");
    exit(EXIT_FAILURE);
  }

  sensorTracker->CreateDefaultOutputChannel(NULL, true);

  // Add an observer to warning and error events for redirecting it to the stdout
  vtkNew<vtkCallbackCommand> callbackCommand;
  callbackCommand->SetCallback(PrintLogsCallback);
  sensorTracker->AddObserver("WarningEvent", callbackCommand);
  sensorTracker->AddObserver("ErrorEvent", callbackCommand);

  sensorTracker->StartRecording();
  igtl::Sleep(1000);
  sensorTracker->StopRecording();

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