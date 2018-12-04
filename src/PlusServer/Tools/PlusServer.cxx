/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

/*!
\file PlusServer.cxx
\brief Start Plus OpenIGTLink server for broadcasting selected IGTL messages.
If testing enabled this program tests Plus server and Plus client. The communication in this test
happens between two threads. In real life, it happens between two programs.
*/

#include "PlusConfigure.h"
#include "igsioCommon.h"
#include "vtkNew.h"
#include "vtkPlusDataCollector.h"
#include "vtkPlusOpenIGTLinkVideoSource.h"
#include "vtkPlusBuffer.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusOpenIGTLinkServer.h"
#include "vtkSmartPointer.h"
#include "vtkIGSIOTransformRepository.h"
#include "vtksys/CommandLineArguments.hxx"

// For catching Ctrl-C
#include <csignal>
#include <cstdlib>
#include <cstdio>

// Forward declare signal handler
void SignalInterruptHandler(int s);
static bool stopRequested = false;
#ifdef _WIN32
void CheckConsoleWindowCloseRequested(HWND consoleHwnd);
#endif

//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  // Check command line arguments.
  bool printHelp(false);
  std::string inputConfigFileName;
  std::string testingConfigFileName;
  int verboseLevel = vtkIGSIOLogger::LOG_LEVEL_UNDEFINED;
  double runTimeSec = 0.0;

  const int numOfTestClientsToConnect = 5; // only if testing is enabled S

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Name of the input configuration file.");
  args.AddArgument("--running-time", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &runTimeSec, "Server running time period in seconds. If the parameter is not defined or 0 then the server runs infinitely.");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");

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

  vtkIGSIOLogger::Instance()->SetLogLevel(verboseLevel);

  if (inputConfigFileName.empty())
  {
    LOG_ERROR("--config-file argument is required!");
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  LOG_INFO("Logging at level " << vtkIGSIOLogger::Instance()->GetLogLevel() << " (" << vtkIGSIOLogger::Instance()->GetLogLevelString() << ") to file: " << vtkIGSIOLogger::Instance()->GetLogFileName());

  // Read main configuration file
  vtkNew<vtkPlusDataCollector> dataCollector;
  if (dataCollector->ReadConfiguration(inputConfigFileName) != PLUS_SUCCESS)
  {
    LOG_ERROR("Datacollector failed to read configuration");
    exit(EXIT_FAILURE);
  }

  // Recover config root element
  LOG_INFO("Server status: Reading configuration.");
  vtkXMLDataElement* configRootElement = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData();

  // Create transform repository instance
  vtkNew<vtkIGSIOTransformRepository> transformRepository;
  if (transformRepository->ReadConfiguration(configRootElement) != PLUS_SUCCESS)
  {
    LOG_ERROR("Transform repository failed to read configuration");
    exit(EXIT_FAILURE);
  }

  LOG_INFO("Server status: Connecting to devices.");
  if (dataCollector->Connect() != PLUS_SUCCESS)
  {
    LOG_ERROR("Datacollector failed to connect to devices");
    exit(EXIT_FAILURE);
  }

  if (dataCollector->Start() != PLUS_SUCCESS)
  {
    LOG_ERROR("Datacollector failed to start");
    exit(EXIT_FAILURE);
  }

  LOG_INFO("Server status: Starting servers.");
  std::vector<vtkPlusOpenIGTLinkServer*> serverList;
  int serverCount(0);
  for (int i = 0; i < configRootElement->GetNumberOfNestedElements(); ++i)
  {
    vtkXMLDataElement* serverElement = configRootElement->GetNestedElement(i);
    if (STRCASECMP(serverElement->GetName(), "PlusOpenIGTLinkServer") != 0)
    {
      continue;
    }

    serverCount++;

    // This is a PlusServer tag, let's create it
    vtkSmartPointer<vtkPlusOpenIGTLinkServer> server = vtkSmartPointer<vtkPlusOpenIGTLinkServer>::New();
    LOG_DEBUG("Initializing Plus OpenIGTLink server... ");
    if (server->Start(dataCollector.GetPointer(), transformRepository.GetPointer(), serverElement, vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to start OpenIGTLink server");
      exit(EXIT_FAILURE);
    }
    serverList.push_back(server);
  }
  if (serverCount == 0)
  {
    LOG_ERROR("No vtkPlusOpenIGTLinkServer tags were found in the configuration file. Please add at least one.");
    exit(EXIT_FAILURE);
  }

  double startTime = vtkIGSIOAccurateTimer::GetSystemTime();

  LOG_INFO("Server status: Server(s) are running.");
  LOG_INFO("Press Ctrl-C to quit.");

  // Set up signal catching
  signal(SIGINT, SignalInterruptHandler);
#ifdef _WIN32
  HWND consoleHwnd = GetConsoleWindow();
#endif

  bool neverStop = (runTimeSec == 0.0);

  // Run server until requested
  const double commandQueuePollIntervalSec = 0.010;
  while ((neverStop || (vtkIGSIOAccurateTimer::GetSystemTime() < startTime + runTimeSec)) && !stopRequested)
  {
    for (std::vector<vtkPlusOpenIGTLinkServer*>::iterator it = serverList.begin(); it != serverList.end(); ++it)
    {
      (*it)->ProcessPendingCommands();
    }
#if _WIN32
    // Check if received message that requested process termination (non-Windows systems always use signals).
    // Need to do it before processing messages.
    CheckConsoleWindowCloseRequested(consoleHwnd);
#endif
    // Need to process messages while waiting because some devices (such as the vtkPlusWin32VideoSource2) require event processing
    vtkIGSIOAccurateTimer::DelayWithEventProcessing(commandQueuePollIntervalSec);
  }

  for (std::vector<vtkPlusOpenIGTLinkServer*>::iterator it = serverList.begin(); it != serverList.end(); ++it)
  {
    (*it)->Stop();
  }

  LOG_INFO("Shutdown successful.");

  return EXIT_SUCCESS;
}

// -------------------------------------------------
void SignalInterruptHandler(int s)
{
  LOG_INFO("Stop requested...");
  stopRequested = true;
}

#ifdef _WIN32
//-----------------------------------------------------------------------------
// On Windows Qt cannot send SIGINT signal to indicate that the process should exit (ctrl-c),
// it can only send WM_CLOSE message to all of its windows. Therefore, we check for WM_CLOSE
// messages and stop if we receive one.
void CheckConsoleWindowCloseRequested(HWND consoleHwnd)
{
  MSG msg;
  if (PeekMessage(&msg, 0, WM_CLOSE, WM_CLOSE, PM_NOREMOVE))
  {
    // Qt usually sends WM_CLOSE with a proper (non-NULL) HWND when calling QProcess::terminate()
    // the launched application. However, on WindowsXP embedded we receive a NULL as HWND, so
    // accept that, too.
    if (msg.hwnd == NULL || msg.hwnd == consoleHwnd)
    {
      stopRequested = true;
    }
  }
}
#endif
