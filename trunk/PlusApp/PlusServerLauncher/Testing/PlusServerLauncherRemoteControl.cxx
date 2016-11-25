/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

/*!
\file PlusServerLauncherRemoteControl.cxx
\brief Client to remote control PlusServerLauncher through OpenIGTLink
*/

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtksys/Process.h"
#include "vtkXMLUtilities.h"

// For catching Ctrl-C
#include <csignal>
#include <cstdlib>
#include <cstdio>

//----------------------------------------------------------------------------
// For CTRL-C signal handling
static bool StopClientRequested=false;

//----------------------------------------------------------------------------
// Print command XML string
void PrintCommand(vtkPlusCommand* command)
{
  vtkSmartPointer<vtkXMLDataElement> cmdConfig=vtkSmartPointer<vtkXMLDataElement>::New();
  command->WriteConfiguration(cmdConfig);
  std::ostringstream xmlStr;
  vtkXMLUtilities::FlattenElement(cmdConfig, xmlStr);
  xmlStr << std::ends;
  LOG_INFO(">>> Command: "<<xmlStr.str());
}

//----------------------------------------------------------------------------
PlusStatus StartPlusServerLauncherProcess(const std::string& configFile, vtksysProcess* &processPtr)
{
  processPtr = NULL;
  std::string executablePath = vtkPlusConfig::GetInstance()->GetPlusExecutablePath("PlusServerLauncher");

  if ( !vtksys::SystemTools::FileExists( executablePath.c_str(), true) )
  {
    LOG_ERROR("Unable to find executable at: " << executablePath);
    return PLUS_FAIL;
  }

  try
  {
    processPtr = vtksysProcess_New();

    // Command name and parameters
    std::vector<const char*> command;
    command.push_back(executablePath.c_str()); // command name
    //std::string param1 = std::string("--config-file=")+configFile;
    //command.push_back(param1.c_str()); // command parameter
    command.push_back(0); // The array must end with a NULL pointer.
    vtksysProcess_SetCommand(processPtr, &*command.begin());

    // Redirect PlusServer output to this process output (otherwise server execution would be blocked)
    vtksysProcess_SetPipeFile(processPtr, vtksysProcess_Pipe_STDOUT, "PlusServerLauncherRemoteControlStdOut.log");
    vtksysProcess_SetPipeFile(processPtr, vtksysProcess_Pipe_STDERR, "PlusServerLauncherRemoteControlStdErr.log");

    LOG_INFO("Start PlusServer..." );
    vtksysProcess_Execute(processPtr);
    LOG_DEBUG("PlusServer started" );

    return PLUS_SUCCESS;
  }
  catch (...)
  {
    LOG_ERROR("Failed to start PlusServerLauncher");
    return PLUS_FAIL;
  }
}

//----------------------------------------------------------------------------
void StopPlusServerProcess(vtksysProcess* &processPtr)
{
  if (processPtr==NULL)
  {
    return;
  }
  vtksysProcess_Kill(processPtr);
  processPtr = NULL;
}

#define RETURN_IF_FAIL(cmd) if (cmd!=PLUS_SUCCESS) { return PLUS_FAIL; };

//----------------------------------------------------------------------------
PlusStatus RunTests(vtkPlusOpenIGTLinkClient* client)
{
  const char captureDeviceId[]="CaptureDevice";
  const char capturingOutputFileName[]="OpenIGTTrackedVideoRecordingTest.mha"; // must match the extension defined in the config file

  const char volumeReconstructionDeviceId[]="VolumeReconstructorDevice";
  const char* batchReconstructionInputFileName=capturingOutputFileName;
  const char batchReconstructionOutputImageName[]="VolumeReconstructedBatch";
  const char snapshotReconstructionOutputImageName[]="VolumeReconstructedSnapshot";
  const char liveReconstructionOutputImageName[]="VolumeReconstructedLive";

  // There is no NRRD support in VTK5, so only use it with VTK6
#if (VTK_MAJOR_VERSION < 6)
  const char batchReconstructionOutputFileName[]="VolumeReconstructedBatch.mha";
  const char snapshotReconstructionOutputFileName[]="VolumeReconstructedSnapshot.mha";
  const char liveReconstructionOutputFileName[]="VolumeReconstructedLive.mha";
#else
  const char batchReconstructionOutputFileName[]="VolumeReconstructedBatch.nrrd";
  const char snapshotReconstructionOutputFileName[]="VolumeReconstructedSnapshot.nrrd";
  const char liveReconstructionOutputFileName[]="VolumeReconstructedLive.nrrd";
#endif

  std::string replyMessage;
  std::string errorMessage;
  std::map<std::string, std::string> parameters;
  int commandId = 1;

  // Basic commands
  ExecuteGetChannelIds(client, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, replyMessage, errorMessage, parameters));
  if( client->GetServerIGTLVersion() <= IGTL_HEADER_VERSION_2 )
  {
    if( replyMessage != "TrackedVideoStream" )
    {
      LOG_ERROR("Incorrect reply sent. Got: " << replyMessage << ". Expected: \"TrackedVideoStream\"");
    }
  }
  else
  {
    if( parameters.size() == 0 || parameters.find("TrackedVideoStream") == parameters.end())
    {
      LOG_ERROR("Empty result or entry not found in list of device IDs.");
    }
    else
    {
      if( parameters["TrackedVideoStream"].compare("TrackedVideoStream") != 0 )
      {
        LOG_ERROR("Incorrect parameter returned. Got: " << parameters["TrackedVideoStream"] << ". Expected: \"TrackedVideoStream\"");
      }
    }
  }
  parameters.clear();

  ExecuteGetDeviceIds(client, "VirtualVolumeReconstructor", commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, replyMessage, errorMessage, parameters));
  if( client->GetServerIGTLVersion() <= IGTL_HEADER_VERSION_2 )
  {
    if( replyMessage != "VolumeReconstructorDevice" )
    {
      LOG_ERROR("Incorrect reply sent. Got: " << replyMessage << ". Expected: \"VolumeReconstructorDevice\"");
    }
  }
  else
  {
    if( parameters.size() == 0)
    {
      LOG_ERROR("Empty result returned for list of device IDs.");
    }
    else
    {
      if( parameters["VolumeReconstructorDevice"].compare("VolumeReconstructorDevice") != 0 )
      {
        LOG_ERROR("Incorrect parameter returned.");
      }
    }
  }
  parameters.clear();

  ExecuteUpdateTransform(client, "Test1ToReference", "1 0 0 10 0 1.2 0.1 12 0.1 0.2 -0.9 -20 0 0 0 1", "1.4", "100314_182141", "TRUE", commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, replyMessage, errorMessage, parameters));
  parameters.clear();
  ExecuteGetTransform(client, "Test1ToReference", commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, replyMessage, errorMessage, parameters));
  parameters.clear();
  ExecuteSaveConfig(client, "Test1SavedConfig.xml", commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, replyMessage, errorMessage, parameters));
  parameters.clear();

  // Capturing
  ExecuteStartAcquisition(client, captureDeviceId, false, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, replyMessage, errorMessage, parameters));
  parameters.clear();
  vtkPlusAccurateTimer::DelayWithEventProcessing(2.0);
  ExecuteSuspendAcquisition(client, captureDeviceId, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, replyMessage, errorMessage, parameters));
  parameters.clear();
  vtkPlusAccurateTimer::DelayWithEventProcessing(2.0);
  ExecuteResumeAcquisition(client, captureDeviceId, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, replyMessage, errorMessage, parameters));
  parameters.clear();
  vtkPlusAccurateTimer::DelayWithEventProcessing(2.0);
  ExecuteStopAcquisition(client, captureDeviceId, capturingOutputFileName, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, replyMessage, errorMessage, parameters));
  parameters.clear();
  vtkPlusAccurateTimer::DelayWithEventProcessing(2.0);

  // Volume reconstruction from file
  ExecuteReconstructFromFile(client, volumeReconstructionDeviceId, batchReconstructionInputFileName, batchReconstructionOutputFileName, batchReconstructionOutputImageName, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, replyMessage, errorMessage, parameters));
  parameters.clear();
  vtkPlusAccurateTimer::DelayWithEventProcessing(2.0);

  // Live volume reconstruction
  ExecuteStartReconstruction(client, volumeReconstructionDeviceId, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, replyMessage, errorMessage, parameters));
  parameters.clear();
  vtkPlusAccurateTimer::DelayWithEventProcessing(2.0);
  ExecuteSuspendReconstruction(client, volumeReconstructionDeviceId, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, replyMessage, errorMessage, parameters));
  parameters.clear();
  vtkPlusAccurateTimer::DelayWithEventProcessing(2.0);
  ExecuteResumeReconstruction(client, volumeReconstructionDeviceId, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, replyMessage, errorMessage, parameters));
  parameters.clear();
  vtkPlusAccurateTimer::DelayWithEventProcessing(2.0);
  ExecuteGetSnapshotReconstruction(client, volumeReconstructionDeviceId, snapshotReconstructionOutputFileName, snapshotReconstructionOutputImageName, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, replyMessage, errorMessage, parameters));
  parameters.clear();
  vtkPlusAccurateTimer::DelayWithEventProcessing(2.0);
  ExecuteGetSnapshotReconstruction(client, volumeReconstructionDeviceId, snapshotReconstructionOutputFileName, snapshotReconstructionOutputImageName, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, replyMessage, errorMessage, parameters));
  parameters.clear();
  vtkPlusAccurateTimer::DelayWithEventProcessing(2.0);
  ExecuteGetSnapshotReconstruction(client, volumeReconstructionDeviceId, snapshotReconstructionOutputFileName, snapshotReconstructionOutputImageName, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, replyMessage, errorMessage, parameters));
  parameters.clear();
  vtkPlusAccurateTimer::DelayWithEventProcessing(2.0);
  ExecuteStopReconstruction(client, volumeReconstructionDeviceId, liveReconstructionOutputFileName, liveReconstructionOutputImageName, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, replyMessage, errorMessage, parameters));
  parameters.clear();
  vtkPlusAccurateTimer::DelayWithEventProcessing(2.0);

  return PLUS_SUCCESS;
}

// -------------------------------------------------
// For CTRL-C signal handling
void SignalInterruptHandler(int s)
{
  StopClientRequested = true;
}


//----------------------------------------------------------------------------
int main( int argc, char** argv )
{
  // Check command line arguments.
  std::string serverHost="127.0.0.1";
  int serverPort = 18944;
  std::string command;
  std::string deviceId;
  std::string inputFilename="PlusServerRecording.nrrd";
  std::string outputFilename;
  std::string outputImageName;
  std::string transformName;
  std::string transformError;
  std::string transformDate;
  std::string transformPersistent;
  std::string transformValue;
  std::string dicomOutputDirectory;
  std::string volumeEmbeddedTransformToFrame;
  int serverIGTLVersion;
  std::string text;
  bool keepReceivedDicomFiles = false;
  bool responseExpected = false;
  bool enableCompression = false;
  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;
  bool keepConnected=false;
  std::string serverConfigFileName;
  bool runTests=false;
  int commandId(0);

  vtksys::CommandLineArguments args;
  args.Initialize( argc, argv );

  args.AddArgument( "--host", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &serverHost, "Host name of the OpenIGTLink server (default: 127.0.0.1)" );
  args.AddArgument( "--port", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &serverPort, "Port address of the OpenIGTLink server (default: 18944)" );
  args.AddArgument( "--command-id", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &commandId, "Command ID to send to the server.");
  args.AddArgument( "--server-igtl-version", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &serverIGTLVersion, "The version of IGTL used by the server. Remove this parameter when querying is dynamic.");
  args.AddArgument( "--device", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &deviceId, "ID of the controlled device (optional, default: first VirtualStreamCapture or VirtualVolumeReconstructor device). In case of GET_DEVICE_IDS it is not an ID but a device type." );
  args.AddArgument( "--input-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputFilename, "File name of the input, used for RECONSTRUCT command" );
  args.AddArgument( "--output-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFilename, "File name of the output, used for START command (optional, default: 'PlusServerRecording.nrrd' for acquisition, no output for volume reconstruction)" );
  args.AddArgument( "--output-image-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputImageName, "OpenIGTLink device name of the reconstructed file (optional, default: image is not sent)" );
  args.AddArgument( "--text", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &text, "Text to be sent to the device" );
  args.AddArgument( "--transform-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &transformName, "The name of the transform to update. Form=[From]To[To]Transform" );
  args.AddArgument( "--transform-date", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &transformDate, "The date of the transform to update." );
  args.AddArgument( "--transform-error", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &transformError, "The error of the transform to update." );
  args.AddArgument( "--transform-persistent", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &transformPersistent, "The persistence of the transform to update." );
  args.AddArgument( "--transform-value", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &transformValue, "The actual transformation matrix to update." );
  args.AddArgument( "--use-compression", vtksys::CommandLineArguments::NO_ARGUMENT, &enableCompression, "Set capture device to record compressed data. Only supported with .nrrd capture." );
  args.AddArgument( "--keep-connected", vtksys::CommandLineArguments::NO_ARGUMENT, &keepConnected, "Keep the connection to the server after command completion (exits on CTRL-C).");
  args.AddArgument( "--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)" );
  args.AddArgument( "--dicom-directory", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &dicomOutputDirectory, "The folder directory for the dicom images acquired from the StealthLink Server");
  args.AddArgument( "--volumeEmbeddedTransformToFrame", vtksys::CommandLineArguments::EQUAL_ARGUMENT,&volumeEmbeddedTransformToFrame, "The reference frame in which the dicom image will be represented. Ex: RAS,LPS,Reference,Tracker etc");
  args.AddArgument( "--keepReceivedDicomFiles", vtksys::CommandLineArguments::NO_ARGUMENT, &keepReceivedDicomFiles, "Keep the dicom files in the designated folder after having acquired them from the server");
  args.AddArgument( "--response-expected", vtksys::CommandLineArguments::NO_ARGUMENT, &responseExpected, "Wait for a response after sending text");
  args.AddArgument( "--server-config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &serverConfigFileName, "Starts a PlusServer instance with the provided config file. When this process exits, the server is stopped." );
  // TODO : add dynamic querying of server (GetCapabilities?) to enable dynamic version negotiation
  args.AddArgument( "--server-igtl-version", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &serverIGTLVersion, "The version of IGTL used by the server. Remove this parameter when querying is dynamic.");
  args.AddArgument( "--run-tests", vtksys::CommandLineArguments::NO_ARGUMENT, &runTests, "Test execution of all remote control commands. Requires a running PlusServer, which can be launched by --server-config-file");

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments." << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkIGTLIOLogicPointer remoteLogic = vtkIGTLIOLogicPointer::New();
  vtkIGTLIOConnector connector = remoteLogic->Connect(serverHost, serverPort, CLIENT, 5.0 /* timeout in sec*/);
  if (connector.isNull())
  {
    LOG_ERROR("Failed to start PlusServer");
    exit(EXIT_FAILURE);
  }

  vtkIGTLIOCommandPointer command = remoteConnector->SendCommandQuery("PlusServerLauncher", "<Command ... />", SYNCHRONOUS, 5.0 /* timeout in sec*/);
  std::string response = command.GetResponse();

  vtkIGTLIOCommandPointer command = remoteLogic->SendCommandQuery("PlusServerLauncher", "<Command ... />", SYNCHRONOUS, 5.0 /* timeout in sec*/, connector);
  std::string response = command.GetResponse();
  
  vtkIGTLIOCommandPointer command = remoteConnector->SendCommandQuery("PlusServerLauncher", "<Command ... />", ASYNCHRONOUS, 5.0 /* timeout in sec*/);
  // do some work, may check command.GetStatus() ...
  std::string response = command.GetResponse();
  


  client->SetServerHost(serverHost.c_str());
  client->SetServerPort(serverPort);
  client->SetServerIGTLVersion(serverIGTLVersion);
  if (client->Connect(15.0)==PLUS_FAIL)
  {
    LOG_ERROR("Failed to connect to server at "<<serverHost<<":"<<serverPort);
    StopPlusServerProcess(plusServerProcess);
    exit(EXIT_FAILURE);
  }

  

  int processReturnValue = EXIT_SUCCESS;

  // Run a command
  if ( !command.empty() )
  {
    PlusStatus commandExecutionStatus = PLUS_SUCCESS;
    // Execute command
    if (STRCASECMP(command.c_str(),"START_ACQUISITION")==0)
    {
      commandExecutionStatus = ExecuteStartAcquisition(client, deviceId, enableCompression, commandId);
    }
    else if (STRCASECMP(command.c_str(),"STOP_ACQUISITION")==0)
    {
      commandExecutionStatus = ExecuteStopAcquisition(client, deviceId, outputFilename, commandId);
    }
    else if (STRCASECMP(command.c_str(),"SUSPEND_ACQUISITION")==0)
    {
      commandExecutionStatus = ExecuteSuspendAcquisition(client, deviceId, commandId);
    }
    else if (STRCASECMP(command.c_str(),"RESUME_ACQUISITION")==0)
    {
      commandExecutionStatus = ExecuteResumeAcquisition(client, deviceId, commandId);
    }
    else if (STRCASECMP(command.c_str(),"START_RECONSTRUCTION")==0)
    {
      commandExecutionStatus = ExecuteStartReconstruction(client, deviceId, commandId);
    }
    else if (STRCASECMP(command.c_str(),"SUSPEND_RECONSTRUCTION")==0)
    {
      commandExecutionStatus = ExecuteSuspendReconstruction(client, deviceId, commandId);
    }
    else if (STRCASECMP(command.c_str(),"RESUME_RECONSTRUCTION")==0)
    {
      commandExecutionStatus = ExecuteResumeReconstruction(client, deviceId, commandId);
    }
    else if (STRCASECMP(command.c_str(),"GET_RECONSTRUCTION_SNAPSHOT")==0)
    {
      commandExecutionStatus = ExecuteGetSnapshotReconstruction(client, deviceId, outputFilename, outputImageName, commandId);
    }
    else if (STRCASECMP(command.c_str(),"STOP_RECONSTRUCTION")==0)
    {
      commandExecutionStatus = ExecuteStopReconstruction(client, deviceId, outputFilename, outputImageName, commandId);
    }
    else if (STRCASECMP(command.c_str(),"RECONSTRUCT")==0)
    {
      commandExecutionStatus = ExecuteReconstructFromFile(client, deviceId, inputFilename, outputFilename, outputImageName, commandId);
    }
    else if (STRCASECMP(command.c_str(),"GET_CHANNEL_IDS")==0)
    {
      commandExecutionStatus = ExecuteGetChannelIds(client, commandId);
    }
    else if (STRCASECMP(command.c_str(),"GET_DEVICE_IDS")==0)
    {
      commandExecutionStatus = ExecuteGetDeviceIds(client, deviceId /* actually a device type */, commandId);
    }
    else if (STRCASECMP(command.c_str(), "UPDATE_TRANSFORM")==0)
    {
      commandExecutionStatus = ExecuteUpdateTransform(client, transformName, transformValue, transformError, transformDate, transformPersistent, commandId);
    }
    else if (STRCASECMP(command.c_str(), "SAVE_CONFIG")==0)
    {
      commandExecutionStatus = ExecuteSaveConfig(client, outputFilename, commandId);
    }
    else if (STRCASECMP(command.c_str(), "SEND_TEXT")==0)
    {
      commandExecutionStatus = ExecuteSendText(client, deviceId, text, responseExpected, commandId);
    }
    else if (STRCASECMP(command.c_str(), "GET_TRANSFORM")==0)
    {
      commandExecutionStatus = ExecuteGetTransform(client, transformName, commandId);
    }
    else if (STRCASECMP(command.c_str(), "GET_EXAM_DATA")==0)
    {
#ifdef PLUS_USE_STEALTHLINK
      commandExecutionStatus = ExecuteGetExamData(client, deviceId,dicomOutputDirectory,volumeEmbeddedTransformToFrame,keepReceivedDicomFiles, commandId);
#else
      LOG_ERROR("Plus is not built with StealthLink support");
      commandExecutionStatus = PLUS_FAIL;
#endif
    }
    else
    {
      LOG_ERROR("Unknown command: "<<command);
      client->Disconnect();
      commandExecutionStatus = PLUS_FAIL;
    }
    if (commandExecutionStatus == PLUS_SUCCESS)
    {
      std::string replyMessage;
      std::string errorMessage;
      std::map<std::string, std::string> parameters;
      if (ReceiveAndPrintReply(client, replyMessage, errorMessage, parameters) != PLUS_SUCCESS)
      {
        processReturnValue = EXIT_FAILURE;
      }
    }
    else
    {
      // command execution failed
      processReturnValue = EXIT_FAILURE;
    }
    if (!keepConnected)
    {
      // we don't need to remain connected if a command has been executed
      StopClientRequested = true;
    }
  }

  // Run automatic tests
  if (runTests)
  {
    if (RunTests(client)!=PLUS_SUCCESS)
    {
      processReturnValue = EXIT_FAILURE;
    }
    // we don't need to remain connected if tests have been executed
    StopClientRequested = true;
  }

  // Remain connected until the user requests to stop
  if (!StopClientRequested)
  {
    std::cout << "Press Ctrl-C to quit:" << std::endl;
    // Set up signal catching
    signal(SIGINT, SignalInterruptHandler);
    // Run client until requested
    const double commandQueuePollIntervalSec=0.010;
    while (!StopClientRequested)
    {
      // the customized client logs the transformation matrices in the data receiver thread
      vtkPlusAccurateTimer::DelayWithEventProcessing(commandQueuePollIntervalSec);
    }
  }

  client->Disconnect();
  StopPlusServerProcess(plusServerProcess);
  return processReturnValue;
}