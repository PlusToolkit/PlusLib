/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "igsioCommon.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusOpenIGTLinkServer.h"
#include "vtkPlusDataCollector.h"
#include "vtkPlusOpenIGTLinkVideoSource.h"
#include "vtkIGSIOTransformRepository.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtksys/CommandLineArguments.hxx>

// -------------------------------------------------
PlusStatus ConnectClients(int listeningPort, std::vector< vtkSmartPointer<vtkPlusOpenIGTLinkVideoSource> >& testClientList, int numberOfClientsToConnect, vtkSmartPointer<vtkXMLDataElement> configRootElement)
{
  if (configRootElement == NULL)
  {
    LOG_ERROR("PlusServer client configuration is missing");
    return PLUS_FAIL;
  }

  int numberOfErrors = 0;

  // Clear test client list
  testClientList.clear();

  for (int i = 0; i < numberOfClientsToConnect; ++i)
  {
    vtkSmartPointer<vtkPlusOpenIGTLinkVideoSource> client = vtkSmartPointer<vtkPlusOpenIGTLinkVideoSource>::New();
    client->SetDeviceId("OpenIGTLinkVideoReceiveDevice");
    client->ReadConfiguration(configRootElement);
    client->SetDeviceId(std::string("OpenIGTLinkVideoReceiveDevice") + igsioCommon::ToString<int>(i));
    client->SetServerAddress("127.0.0.1");
    client->SetServerPort(listeningPort);
    if (client->OutputChannelCount() == 0)
    {
      LOG_ERROR("No output channels in openIGTLink client.");
      ++numberOfErrors;
      continue;
    }

    vtkPlusChannel* aChannel = *(client->GetOutputChannelsStart());
    vtkPlusDataSource* aSource(NULL);
    if (aChannel->GetVideoSource(aSource) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to retrieve the video source.");
      continue;
    }
    client->SetBufferSize(*aChannel, 10);
    client->SetMessageType("TrackedFrame");
    igsioTransformName name("Image", "Reference");
    client->SetImageMessageEmbeddedTransformName(name);
    aSource->SetInputImageOrientation(US_IMG_ORIENT_MF);

    if (client->Connect() != PLUS_SUCCESS)
    {
      LOG_ERROR("Client #" << i + 1 << " couldn't connect to server.");
      ++numberOfErrors;
      continue;
    }

    LOG_DEBUG("Client #" << i + 1 << " successfully connected to server!");

    if (client->StartRecording() != PLUS_SUCCESS)
    {
      LOG_ERROR("Client #" << i + 1 << " couldn't start recording frames.");
      client->Disconnect();
      ++numberOfErrors;
      continue;
    }

    // Add connected client to list
    testClientList.push_back(client);
  }

  return (numberOfErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL);
}

// -------------------------------------------------
PlusStatus DisconnectClients(std::vector< vtkSmartPointer<vtkPlusOpenIGTLinkVideoSource> >& testClientList)
{
  int numberOfErrors = 0;
  for (unsigned int i = 0; i < testClientList.size(); ++i)
  {
    if (testClientList[i]->StopRecording() != PLUS_SUCCESS)
    {
      LOG_ERROR("Client #" << i + 1 << " failed to stop recording");
      ++numberOfErrors;
    }

    if (testClientList[i]->Disconnect() != PLUS_SUCCESS)
    {
      LOG_ERROR("Client #" << i + 1 << " failed to disconnect from server");
      ++numberOfErrors;
      continue;
    }

    LOG_DEBUG("Client #" << i + 1 << " successfully disconnected from server!");
  }

  return (numberOfErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL);
}

// -------------------------------------------------
vtkSmartPointer<vtkPlusOpenIGTLinkServer> StartServer(const std::string& inputConfigFileName)
{
  // Read main configuration file
  std::string configFilePath = inputConfigFileName;
  if (!vtksys::SystemTools::FileExists(configFilePath.c_str(), true))
  {
    configFilePath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(inputConfigFileName);
    if (!vtksys::SystemTools::FileExists(configFilePath.c_str(), true))
    {
      LOG_ERROR("Reading device set configuration file failed: " << inputConfigFileName << " does not exist in the current directory or in " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory());
      exit(EXIT_FAILURE);
    }
  }
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(configFilePath.c_str()));
  if (configRootElement == NULL)
  {
    LOG_ERROR("Reading device set configuration file failed: syntax error in " << inputConfigFileName);
    exit(EXIT_FAILURE);
  }

  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationFileName(inputConfigFileName);
  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement);

  // Print configuration file contents for debugging purposes
  LOG_DEBUG("Device set configuration is read from file: " << inputConfigFileName);
  std::ostringstream xmlFileContents;
  igsioCommon::XML::PrintXML(xmlFileContents, vtkIndent(1), configRootElement);
  LOG_DEBUG("Device set configuration file contents: " << std::endl << xmlFileContents.str());

  LOG_INFO("Server status: Reading configuration.");
  // Create data collector instance
  vtkSmartPointer<vtkPlusDataCollector> dataCollector = vtkSmartPointer<vtkPlusDataCollector>::New();
  if (dataCollector->ReadConfiguration(configRootElement) != PLUS_SUCCESS)
  {
    LOG_ERROR("Datacollector failed to read configuration");
    return nullptr;
  }

  // Create transform repository instance
  vtkSmartPointer<vtkIGSIOTransformRepository> transformRepository = vtkSmartPointer<vtkIGSIOTransformRepository>::New();
  if (transformRepository->ReadConfiguration(configRootElement) != PLUS_SUCCESS)
  {
    LOG_ERROR("Transform repository failed to read configuration");
    return nullptr;
  }

  LOG_INFO("Server status: Connecting to devices.");
  if (dataCollector->Connect() != PLUS_SUCCESS)
  {
    LOG_ERROR("Datacollector failed to connect to devices");
    return nullptr;
  }

  if (dataCollector->Start() != PLUS_SUCCESS)
  {
    LOG_ERROR("Datacollector failed to start");
    return nullptr;
  }

  LOG_INFO("Server status: Starting servers.");
  std::vector<vtkPlusOpenIGTLinkServer*> serverList;
  for (int i = 0; i < configRootElement->GetNumberOfNestedElements(); ++i)
  {
    vtkXMLDataElement* serverElement = configRootElement->GetNestedElement(i);
    if (STRCASECMP(serverElement->GetName(), "PlusOpenIGTLinkServer") != 0)
    {
      continue;
    }

    // This is a PlusServer tag, let's create it
    vtkSmartPointer<vtkPlusOpenIGTLinkServer> server = vtkSmartPointer<vtkPlusOpenIGTLinkServer>::New();
    LOG_DEBUG("Initializing Plus OpenIGTLink server... ");
    if (server->Start(dataCollector, transformRepository, serverElement, configFilePath) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to start OpenIGTLink server");
      break;
    }
    return server;
  }

  return nullptr;
}

//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  // Check command line arguments.
  bool printHelp(false);
  std::string inputConfigFileName;
  std::string testingConfigFileName;
  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  const double WAIT_TIME_SEC = 5.0;
  const int NUM_TEST_CLIENTS = 5; // only if testing is enabled S

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  args.AddArgument("--server-config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Name of the server configuration file.");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");
  args.AddArgument("--testing-config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &testingConfigFileName, "Name of the testing configuration file");

  if (!args.Parse())
  {
    std::cerr << "Problem parsing arguments." << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if (printHelp)
  {
    std::cout << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if (inputConfigFileName.empty())
  {
    LOG_ERROR("--server-config-file argument is required!");
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if (testingConfigFileName.empty())
  {
    LOG_ERROR("--testing-config-file argument is required!");
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  LOG_INFO("Logging at level " << vtkPlusLogger::Instance()->GetLogLevel() << " (" << vtkPlusLogger::Instance()->GetLogLevelString() << ") to file: " << vtkPlusLogger::Instance()->GetLogFileName());

  // Start a server
  vtkSmartPointer<vtkPlusOpenIGTLinkServer> server = StartServer(inputConfigFileName);
  if (server == nullptr)
  {
    LOG_ERROR("Unable to start server.");
    exit(EXIT_FAILURE);
  }

  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
  if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, testingConfigFileName.c_str()) == PLUS_FAIL)
  {
    LOG_ERROR("Unable to read test configuration from file " << testingConfigFileName.c_str());
    exit(EXIT_FAILURE);
  }

  std::vector< vtkSmartPointer<vtkPlusOpenIGTLinkVideoSource> > outTestClients;
  if (ConnectClients(server->GetListeningPort(), outTestClients, NUM_TEST_CLIENTS, configRootElement) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to connect clients to PlusServer!");
    DisconnectClients(outTestClients);
    exit(EXIT_FAILURE);
  }
  LOG_INFO("Clients are connected");

  const double commandQueuePollIntervalSec = 0.010;
  const double startTime = vtkIGSIOAccurateTimer::GetSystemTime();
  while (vtkIGSIOAccurateTimer::GetSystemTime() < startTime + WAIT_TIME_SEC)
  {
    server->ProcessPendingCommands();

    // Need to process messages while waiting because some devices (such as the vtkPlusWin32VideoSource2) require event processing
    vtkIGSIOAccurateTimer::DelayWithEventProcessing(commandQueuePollIntervalSec);
  }

  LOG_INFO("Requested testing time elapsed");

  // Make sure all the clients are still connected
  unsigned int numOfActuallyConnectedClients = server->GetNumberOfConnectedClients();
  if (numOfActuallyConnectedClients != outTestClients.size())
  {
    LOG_ERROR("Number of connected clients to PlusServer doesn't match the requirements ("
              << numOfActuallyConnectedClients << " out of " << outTestClients.size() << ").");
    DisconnectClients(outTestClients);
    exit(EXIT_FAILURE);
  }

  // Disconnect clients from server
  LOG_INFO("Disconnecting clients...");
  if (DisconnectClients(outTestClients) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to disconnect clients from PlusServer!");
    exit(EXIT_FAILURE);
  }
  LOG_INFO("Clients are disconnected");

  return EXIT_SUCCESS;
}