/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusDataCollector.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusOpenIGTLinkServer.h"
#include "vtkPlusOpenIGTLinkTracker.h"

// VTK includes
#include <vtkCallbackCommand.h>
#include <vtkSmartPointer.h>
#include <vtksys/CommandLineArguments.hxx>

// STL includes
// TODO: uncomment the lines below when VS2010 is dropped:
//#include <chrono>
//#include <thread>

auto server = vtkSmartPointer<vtkPlusOpenIGTLinkServer>::New();

void PrintLogsCallback(vtkObject* obj, unsigned long eid, void* clientdata, void* calldata);

PlusStatus StartServer(vtkXMLDataElement* configRootElement, const std::string& configFilePath)
{
  LOG_INFO("Server status: Reading configuration.");
  // Create data collector instance
  vtkSmartPointer<vtkPlusDataCollector> dataCollector = vtkSmartPointer<vtkPlusDataCollector>::New();
  if (dataCollector->ReadConfiguration(configRootElement) != PLUS_SUCCESS)
  {
    LOG_ERROR("Datacollector failed to read configuration");
    return PLUS_FAIL;
  }

  // Create transform repository instance
  vtkSmartPointer<vtkIGSIOTransformRepository> transformRepository = vtkSmartPointer<vtkIGSIOTransformRepository>::New();
  if (transformRepository->ReadConfiguration(configRootElement) != PLUS_SUCCESS)
  {
    LOG_ERROR("Transform repository failed to read configuration");
    return PLUS_FAIL;
  }

  LOG_INFO("Server status: Connecting to devices.");
  if (dataCollector->Connect() != PLUS_SUCCESS)
  {
    LOG_ERROR("Datacollector failed to connect to devices");
    return PLUS_FAIL;
  }

  if (dataCollector->Start() != PLUS_SUCCESS)
  {
    LOG_ERROR("Datacollector failed to start");
    return PLUS_FAIL;
  }

  XML_FIND_NESTED_ELEMENT_REQUIRED(serverElement, configRootElement, "PlusOpenIGTLinkServer");

  LOG_DEBUG("Initializing Plus OpenIGTLink server... ");
  if (server->Start(dataCollector, transformRepository, serverElement, configFilePath) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

int main(int argc, char** argv)
{
  bool printHelp(false);
  std::string clientConfigFileName;
  std::string serverConfigFileName;
  bool renderingOff(false);

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  int verboseLevel = vtkIGSIOLogger::LOG_LEVEL_UNDEFINED;

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  args.AddArgument("--client-config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &clientConfigFileName, "Config file containing the client configuration.");
  args.AddArgument("--server-config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &serverConfigFileName, "Config file containing the server configuration.");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");

  if (!args.Parse())
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "\n\nvtkPlusOpenIGTLinkTrackerTest help:" << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkIGSIOLogger::Instance()->SetLogLevel(verboseLevel);

  if (printHelp)
  {
    std::cout << "\n\nvtkPlusOpenIGTLinkTrackerTest help:" << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS);
  }

  // Read server config file
  LOG_INFO("Reading server config file...");
  auto serverConfigRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
  if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(serverConfigRootElement, serverConfigFileName.c_str()) == PLUS_FAIL)
  {
    LOG_ERROR("Unable to read configuration from file " << serverConfigFileName.c_str());
    return EXIT_FAILURE;
  }

  if (StartServer(serverConfigRootElement, serverConfigFileName) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to start server");
    return EXIT_FAILURE;
  }

  // Read client config file
  LOG_INFO("Reading client config file...");
  auto clientConfigRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
  if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(clientConfigRootElement, clientConfigFileName.c_str()) == PLUS_FAIL)
  {
    LOG_ERROR("Unable to read configuration from file " << clientConfigFileName.c_str());
    return EXIT_FAILURE;
  }
  auto client = vtkSmartPointer<vtkPlusOpenIGTLinkTracker>::New();

  // Add an observer to warning and error events for redirecting it to the stdout
  auto callbackCommand = vtkSmartPointer<vtkCallbackCommand>::New();
  callbackCommand->SetCallback(PrintLogsCallback);
  client->AddObserver("WarningEvent", callbackCommand);
  client->AddObserver("ErrorEvent", callbackCommand);
  client->SetDeviceId("TrackerDevice");
  if (client->ReadConfiguration(clientConfigRootElement) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to configure client.");
    return EXIT_FAILURE;
  }

  LOG_INFO("Connect client...");
  if (client->Connect() != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to connect client to server.");
    return EXIT_FAILURE;
  }

  if (client->StartRecording() != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to start recording of client device.");
    return EXIT_FAILURE;
  }

  // TODO: use the line below when VS2010 is dropped:
  // std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  #ifdef _WIN32
    Sleep(2000);
  #else
    usleep(2000000);
  #endif

  auto channel = *client->GetOutputChannelsStart();
  if (channel == nullptr)
  {
    LOG_ERROR("No channel configured for device.");
    exit(EXIT_FAILURE);
  }

  igsioTrackedFrame frame;
  if (channel->GetTrackedFrame(frame) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to retrieve frame from device.");
    exit(EXIT_FAILURE);
  }

  client->Disconnect();
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
