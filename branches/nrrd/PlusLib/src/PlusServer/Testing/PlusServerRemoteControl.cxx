/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
\file PlusServerRemoteControl.cxx 
\brief Client to remote control PlusServer through OpenIGTLink
*/ 

#include "PlusConfigure.h"
#include "vtkPlusOpenIGTLinkClient.h"
#include "vtkPlusReconstructVolumeCommand.h"
#include "vtkPlusRequestIdsCommand.h"
#include "vtkPlusSaveConfigCommand.h"
#include "vtkPlusSendTextCommand.h"
#include "vtkPlusStartStopRecordingCommand.h"
#include "vtkPlusUpdateTransformCommand.h"
#ifdef PLUS_USE_STEALTHLINK
  #include "vtkPlusStealthLinkCommand.h"
#endif
#include "vtksys/CommandLineArguments.hxx"
#include "vtksys/Process.h"
#include "vtkXMLUtilities.h"

#include "igtlTransformMessage.h"

// For catching Ctrl-C
#include <csignal>
#include <cstdlib>
#include <cstdio>

//----------------------------------------------------------------------------
// For CTRL-C signal handling
static bool StopClientRequested=false;

//----------------------------------------------------------------------------
// A customized vtkPlusOpenIGTLinkClient that can display the received transformation matrices
class vtkPlusOpenIGTLinkClientWithTransformLogging : public vtkPlusOpenIGTLinkClient
{
public:

  static vtkPlusOpenIGTLinkClientWithTransformLogging *New();
  vtkTypeMacro( vtkPlusOpenIGTLinkClientWithTransformLogging, vtkPlusOpenIGTLinkClient );

  bool OnMessageReceived(igtl::MessageHeader::Pointer messageHeader)
  {
    bool messageBodyReceived=false;
    if (strcmp(messageHeader->GetDeviceType(), "TRANSFORM") != 0)
    {
      // not a transform message
      return messageBodyReceived;
    }

    igtl::TransformMessage::Pointer transformMsg = igtl::TransformMessage::New(); 
    transformMsg->SetMessageHeader(messageHeader); 
    transformMsg->AllocatePack();    
    SocketReceive(transformMsg->GetPackBodyPointer(), transformMsg->GetPackBodySize());
    messageBodyReceived=true;

    int c = transformMsg->Unpack(1);
    if ( !(c & igtl::MessageHeader::UNPACK_BODY)) 
    {
      LOG_ERROR("Failed to receive TRANSFORM reply (invalid body)");
      return messageBodyReceived;
    }

    //store the transform data into a matrix
    igtl::Matrix4x4 mx;
    transformMsg->GetMatrix(mx);
    LOG_INFO("Matrix for "<<transformMsg->GetDeviceName()<<" TRANSFORM received: ");
    igtl::PrintMatrix(mx);
    
    return messageBodyReceived;
  }

protected:
  vtkPlusOpenIGTLinkClientWithTransformLogging() {};
  virtual ~vtkPlusOpenIGTLinkClientWithTransformLogging() {};
private:
  vtkPlusOpenIGTLinkClientWithTransformLogging( const vtkPlusOpenIGTLinkClientWithTransformLogging& );
  void operator=( const vtkPlusOpenIGTLinkClientWithTransformLogging& );
};

vtkStandardNewMacro( vtkPlusOpenIGTLinkClientWithTransformLogging ); 

// Utility functions for sending commands

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
PlusStatus ExecuteStartAcquisition(vtkPlusOpenIGTLinkClient* client, const std::string &deviceId)
{
  vtkSmartPointer<vtkPlusStartStopRecordingCommand> cmd=vtkSmartPointer<vtkPlusStartStopRecordingCommand>::New();    
  cmd->SetNameToStart();
  if ( !deviceId.empty() )
  {
    cmd->SetCaptureDeviceId(deviceId.c_str());
  }
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteStopAcquisition(vtkPlusOpenIGTLinkClient* client, const std::string &deviceId, std::string outputFilename)
{
  vtkSmartPointer<vtkPlusStartStopRecordingCommand> cmd=vtkSmartPointer<vtkPlusStartStopRecordingCommand>::New();
  cmd->SetNameToStop();
  if (outputFilename.empty())
  {
    outputFilename="PlusServerRecording.mha";
  }
  cmd->SetOutputFilename(outputFilename.c_str());
  if ( !deviceId.empty() )
  {
    cmd->SetCaptureDeviceId(deviceId.c_str());
  }
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteSuspendAcquisition(vtkPlusOpenIGTLinkClient* client, const std::string &deviceId)
{
  vtkSmartPointer<vtkPlusStartStopRecordingCommand> cmd=vtkSmartPointer<vtkPlusStartStopRecordingCommand>::New();
  cmd->SetNameToSuspend();
  if ( !deviceId.empty() )
  {
    cmd->SetCaptureDeviceId(deviceId.c_str());
  }
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteResumeAcquisition(vtkPlusOpenIGTLinkClient* client, const std::string &deviceId)
{
  vtkSmartPointer<vtkPlusStartStopRecordingCommand> cmd=vtkSmartPointer<vtkPlusStartStopRecordingCommand>::New();
  cmd->SetNameToResume();
  if ( !deviceId.empty() )
  {
    cmd->SetCaptureDeviceId(deviceId.c_str());
  }
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteReconstructFromFile(vtkPlusOpenIGTLinkClient* client, const std::string &deviceId, const std::string &inputFilename, const std::string &outputFilename, const std::string &outputImageName)
{
  vtkSmartPointer<vtkPlusReconstructVolumeCommand> cmd=vtkSmartPointer<vtkPlusReconstructVolumeCommand>::New();
  cmd->SetNameToReconstruct();
  if (!deviceId.empty())
  {
    cmd->SetVolumeReconstructorDeviceId(deviceId.c_str());
  }
  cmd->SetInputSeqFilename(inputFilename.c_str());
  if (!outputFilename.empty())
  {
    cmd->SetOutputVolFilename(outputFilename.c_str());
  }
  if (!outputImageName.empty())
  {
    cmd->SetOutputVolDeviceName(outputImageName.c_str());
  }
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteStartReconstruction(vtkPlusOpenIGTLinkClient* client, const std::string &deviceId)
{
  vtkSmartPointer<vtkPlusReconstructVolumeCommand> cmd=vtkSmartPointer<vtkPlusReconstructVolumeCommand>::New();
  cmd->SetNameToStart();
  if (!deviceId.empty())
  {
    cmd->SetVolumeReconstructorDeviceId(deviceId.c_str());
  }
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteSuspendReconstruction(vtkPlusOpenIGTLinkClient* client, const std::string &deviceId)
{
  vtkSmartPointer<vtkPlusReconstructVolumeCommand> cmd=vtkSmartPointer<vtkPlusReconstructVolumeCommand>::New();
  cmd->SetNameToSuspend();
  if (!deviceId.empty())
  {
    cmd->SetVolumeReconstructorDeviceId(deviceId.c_str());
  }
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteResumeReconstruction(vtkPlusOpenIGTLinkClient* client, const std::string &deviceId)
{
  vtkSmartPointer<vtkPlusReconstructVolumeCommand> cmd=vtkSmartPointer<vtkPlusReconstructVolumeCommand>::New();
  cmd->SetNameToResume();
  if (!deviceId.empty())
  {
    cmd->SetVolumeReconstructorDeviceId(deviceId.c_str());
  }
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteGetSnapshotReconstruction(vtkPlusOpenIGTLinkClient* client, const std::string &deviceId, const std::string &outputFilename, const std::string &outputImageName)
{
  vtkSmartPointer<vtkPlusReconstructVolumeCommand> cmd=vtkSmartPointer<vtkPlusReconstructVolumeCommand>::New();
  cmd->SetNameToGetSnapshot();
  if (!deviceId.empty())
  {
    cmd->SetVolumeReconstructorDeviceId(deviceId.c_str());
  }
  if (!outputFilename.empty())
  {
    cmd->SetOutputVolFilename(outputFilename.c_str());
  }
  if (!outputImageName.empty())
  {
    cmd->SetOutputVolDeviceName(outputImageName.c_str());
  }
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteStopReconstruction(vtkPlusOpenIGTLinkClient* client, const std::string &deviceId, const std::string &outputFilename, const std::string &outputImageName)
{
  vtkSmartPointer<vtkPlusReconstructVolumeCommand> cmd=vtkSmartPointer<vtkPlusReconstructVolumeCommand>::New();
  cmd->SetNameToStop();
  if (!deviceId.empty())
  {
    cmd->SetVolumeReconstructorDeviceId(deviceId.c_str());
  }
  if (!outputFilename.empty())
  {
    cmd->SetOutputVolFilename(outputFilename.c_str());
  }
  if (!outputImageName.empty())
  {
    cmd->SetOutputVolDeviceName(outputImageName.c_str());
  }
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}
//----------------------------------------------------------------------------
#ifdef PLUS_USE_STEALTHLINK
PlusStatus ExecuteGetExamData(vtkPlusOpenIGTLinkClient* client,const std::string &deviceId, const std::string &dicomOutputDirectory, const std::string& volumeEmbeddedTransformToFrame,
                                                                                      const bool& keepReceivedDicomFiles)
{
  vtkSmartPointer<vtkPlusStealthLinkCommand> cmd=vtkSmartPointer<vtkPlusStealthLinkCommand>::New();
  cmd->SetNameToGetExam();
  if (!deviceId.empty())
  {
    cmd->SetStealthLinkDeviceId(deviceId.c_str());
  }
  if (!dicomOutputDirectory.empty())
  {
    cmd->SetDicomImagesOutputDirectory(dicomOutputDirectory.c_str());
  }
  if (!volumeEmbeddedTransformToFrame.empty())
  {
    cmd->SetVolumeEmbeddedTransformToFrame(volumeEmbeddedTransformToFrame.c_str());
  }
  cmd->SetKeepReceivedDicomFiles(keepReceivedDicomFiles);
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}
#endif
//----------------------------------------------------------------------------
PlusStatus ExecuteGetChannelIds(vtkPlusOpenIGTLinkClient* client)
{
  vtkSmartPointer<vtkPlusRequestIdsCommand> cmd=vtkSmartPointer<vtkPlusRequestIdsCommand>::New();
  cmd->SetNameToRequestChannelIds();
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteGetDeviceIds(vtkPlusOpenIGTLinkClient* client, const std::string &deviceType)
{
  vtkSmartPointer<vtkPlusRequestIdsCommand> cmd=vtkSmartPointer<vtkPlusRequestIdsCommand>::New();
  cmd->SetNameToRequestDeviceIds();
  cmd->SetDeviceType(deviceType.c_str());
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteUpdateTransform(vtkPlusOpenIGTLinkClient* client, const std::string &transformName, const std::string &transformValue, const std::string &transformError, const std::string &transformDate, const std::string &transformPersistent)
{
  vtkSmartPointer<vtkPlusUpdateTransformCommand> cmd = vtkSmartPointer<vtkPlusUpdateTransformCommand>::New();
  cmd->SetNameToUpdateTransform();
  cmd->SetTransformName(transformName.c_str());
  double value = 0.0;
  PlusCommon::StringToDouble(transformError.c_str(), value);
  cmd->SetTransformError(value);
  cmd->SetTransformDate(transformDate.c_str());
  cmd->SetTransformPersistent(transformPersistent.compare("TRUE") == 0 );
  std::vector<std::string> elems;
  vtkMatrix4x4* transformValueMatrix = vtkMatrix4x4::New();
  PlusCommon::SplitStringIntoTokens(transformValue, ' ', elems);
  if( elems.size() != 16 )
  {
    LOG_ERROR("Invalid formatting of matrix string.");
    return PLUS_FAIL;
  }
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      PlusCommon::StringToDouble((elems[i*4 + j]).c_str(), value);
      transformValueMatrix->SetElement(i, j, value);
    }
  }
  cmd->SetTransformValue(transformValueMatrix);
  transformValueMatrix->Delete();
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteSaveConfig(vtkPlusOpenIGTLinkClient* client, const std::string &outputFilename)
{
  vtkSmartPointer<vtkPlusSaveConfigCommand> cmd = vtkSmartPointer<vtkPlusSaveConfigCommand>::New();
  cmd->SetNameToSaveConfig();
  cmd->SetFilename(outputFilename.c_str());
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteSendText(vtkPlusOpenIGTLinkClient* client, const std::string &deviceId, const std::string &text, bool responseExpected)
{
  vtkSmartPointer<vtkPlusSendTextCommand> cmd = vtkSmartPointer<vtkPlusSendTextCommand>::New();
  cmd->SetNameToSendText();
  cmd->SetText(text.c_str());
  cmd->SetDeviceId(deviceId.c_str());
  cmd->SetResponseExpected(responseExpected);
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus PrintReply(vtkPlusOpenIGTLinkClient* client)
{
  std::string reply;
  const double replyTimeoutSec=30;
  if (client->ReceiveReply(reply, replyTimeoutSec)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to receive reply to the command");
    return PLUS_FAIL;
  }
  LOG_INFO("Reply: "<<reply);
  vtkSmartPointer<vtkXMLDataElement> replyElement = vtkSmartPointer<vtkXMLDataElement>::Take( vtkXMLUtilities::ReadElementFromString(reply.c_str()) );
  if (replyElement == NULL)
  {	
    LOG_ERROR("Unable to parse reply"); 
    return PLUS_FAIL;
  }
  if (replyElement->GetAttribute("Status")==NULL)
  {
    LOG_ERROR("Status: <missing>");
    return PLUS_FAIL;
  }
  PlusStatus status=PLUS_FAIL;
  if (STRCASECMP(replyElement->GetAttribute("Status"),"SUCCESS")==0)
  {
    status=PLUS_SUCCESS;
  }
  else if (STRCASECMP(replyElement->GetAttribute("Status"),"FAIL")==0)
  {
    status=PLUS_FAIL;
  }
  else
  {
    LOG_ERROR("Invalid status: "<<replyElement->GetAttribute("Status"));
  }
  LOG_INFO("Status: "<<(status==PLUS_SUCCESS?"SUCCESS":"FAIL"));
  LOG_INFO("Message: "<<(replyElement->GetAttribute("Message")?replyElement->GetAttribute("Message"):"<none>"));
  return status;
}

//----------------------------------------------------------------------------
PlusStatus StartPlusServerProcess(const std::string& configFile, vtksysProcess* &processPtr)
{
  processPtr = NULL;
  std::string executablePath = vtkPlusConfig::GetInstance()->GetPlusExecutablePath("PlusServer");

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
    std::string param1 = std::string("--config-file=")+configFile;
    command.push_back(param1.c_str()); // command parameter
    command.push_back(0); // The array must end with a NULL pointer.
    vtksysProcess_SetCommand(processPtr, &*command.begin());

    // Redirect PlusServer output to this process output (otherwise server execution would be blocked)
    vtksysProcess_SetPipeFile(processPtr, vtksysProcess_Pipe_STDOUT, "PlusServerRemoteControlStdOut.log");
    vtksysProcess_SetPipeFile(processPtr, vtksysProcess_Pipe_STDERR, "PlusServerRemoteControlStdErr.log");

    LOG_INFO("Start PlusServer..." );
    vtksysProcess_Execute(processPtr);
    LOG_DEBUG("PlusServer started" );

    return PLUS_SUCCESS;
  }
  catch (...)
  {
    LOG_ERROR("Failed to start PlusServer"); 
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
  const char capturingOutputFileName[]="OpenIGTTrackedVideoRecordingTest.mha";

  const char volumeReconstructionDeviceId[]="VolumeReconstructorDevice";
  const char* batchReconstructionInputFileName=capturingOutputFileName;
  const char batchReconstructionOutputFileName[]="VolumeReconstructedBatch.mha";
  const char batchReconstructionOutputImageName[]="VolumeReconstructedBatch";
  const char snapshotReconstructionOutputFileName[]="VolumeReconstructedSnapshot.mha";
  const char snapshotReconstructionOutputImageName[]="VolumeReconstructedSnapshot";
  const char liveReconstructionOutputFileName[]="VolumeReconstructedLive.mha";
  const char liveReconstructionOutputImageName[]="VolumeReconstructedLive";

  // Basic commands
  ExecuteGetChannelIds(client);
  RETURN_IF_FAIL(PrintReply(client));
  ExecuteGetDeviceIds(client, "VirtualVolumeReconstructor");
  RETURN_IF_FAIL(PrintReply(client));
  ExecuteUpdateTransform(client, "Test1ToReference", "1 0 0 10 0 1.2 0.1 12 0.1 0.2 -0.9 -20 0 0 0 1", "1.4", "100314_182141", "TRUE");
  RETURN_IF_FAIL(PrintReply(client));
  ExecuteSaveConfig(client, "Test1SavedConfig.xml");
  RETURN_IF_FAIL(PrintReply(client));

  // Capturing
  ExecuteStartAcquisition(client, captureDeviceId);
  RETURN_IF_FAIL(PrintReply(client));
  vtkAccurateTimer::DelayWithEventProcessing(2.0);
  ExecuteSuspendAcquisition(client, captureDeviceId);
  RETURN_IF_FAIL(PrintReply(client));
  vtkAccurateTimer::DelayWithEventProcessing(2.0);
  ExecuteResumeAcquisition(client, captureDeviceId);
  RETURN_IF_FAIL(PrintReply(client));
  vtkAccurateTimer::DelayWithEventProcessing(2.0);
  ExecuteStopAcquisition(client, captureDeviceId, capturingOutputFileName);
  RETURN_IF_FAIL(PrintReply(client));
  vtkAccurateTimer::DelayWithEventProcessing(2.0);

  // Volume reconstruction from file
  ExecuteReconstructFromFile(client, volumeReconstructionDeviceId, batchReconstructionInputFileName, batchReconstructionOutputFileName, batchReconstructionOutputImageName);
  RETURN_IF_FAIL(PrintReply(client));
  vtkAccurateTimer::DelayWithEventProcessing(2.0);

  // Live volume reconstruction
  ExecuteStartReconstruction(client, volumeReconstructionDeviceId);
  RETURN_IF_FAIL(PrintReply(client));
  vtkAccurateTimer::DelayWithEventProcessing(2.0);
  ExecuteSuspendReconstruction(client, volumeReconstructionDeviceId);
  RETURN_IF_FAIL(PrintReply(client));
  vtkAccurateTimer::DelayWithEventProcessing(2.0);
  ExecuteResumeReconstruction(client, volumeReconstructionDeviceId);
  RETURN_IF_FAIL(PrintReply(client));
  vtkAccurateTimer::DelayWithEventProcessing(2.0);
  ExecuteGetSnapshotReconstruction(client, volumeReconstructionDeviceId, snapshotReconstructionOutputFileName, snapshotReconstructionOutputImageName);
  RETURN_IF_FAIL(PrintReply(client));
  vtkAccurateTimer::DelayWithEventProcessing(2.0);
  ExecuteGetSnapshotReconstruction(client, volumeReconstructionDeviceId, snapshotReconstructionOutputFileName, snapshotReconstructionOutputImageName);
  RETURN_IF_FAIL(PrintReply(client));
  vtkAccurateTimer::DelayWithEventProcessing(2.0);
  ExecuteGetSnapshotReconstruction(client, volumeReconstructionDeviceId, snapshotReconstructionOutputFileName, snapshotReconstructionOutputImageName);
  RETURN_IF_FAIL(PrintReply(client));
  vtkAccurateTimer::DelayWithEventProcessing(2.0);
  ExecuteStopReconstruction(client, volumeReconstructionDeviceId, liveReconstructionOutputFileName, liveReconstructionOutputImageName);
  RETURN_IF_FAIL(PrintReply(client));
  vtkAccurateTimer::DelayWithEventProcessing(2.0);

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
  std::string inputFilename="PlusServerRecording.mha";
  std::string outputFilename;
  std::string outputImageName;
  std::string transformName;
  std::string transformError;
  std::string transformDate;
  std::string transformPersistent;
  std::string transformValue;
  std::string dicomOutputDirectory;
  std::string volumeEmbeddedTransformToFrame;
  std::string text;
  bool keepReceivedDicomFiles = false;
  bool responseExpected = false;
  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;
  bool keepConnected=false;
  std::string serverConfigFileName;
  bool runTests=false;

  vtksys::CommandLineArguments args;
  args.Initialize( argc, argv );

  args.AddArgument( "--host", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &serverHost, "Host name of the OpenIGTLink server (default: 127.0.0.1)" );
  args.AddArgument( "--port", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &serverPort, "Port address of the OpenIGTLink server (default: 18944)" );
  args.AddArgument( "--command", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &command, 
    "Command name to be executed on the server (START_ACQUISITION, STOP_ACQUISITION, SUSPEND_ACQUISITION, RESUME_ACQUISITION, \
    RECONSTRUCT, START_RECONSTRUCTION, SUSPEND_RECONSTRUCTION, RESUME_RECONSTRUCTION, STOP_RECONSTRUCTION, GET_RECONSTRUCTION_SNAPSHOT, GET_CHANNEL_IDS, GET_DEVICE_IDS, GET_EXAM_DATA, SEND_TEXT)" );
  args.AddArgument( "--device", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &deviceId, "ID of the controlled device (optional, default: first VirtualStreamCapture or VirtualVolumeReconstructor device). In case of GET_DEVICE_IDS it is not an ID but a device type." );
  args.AddArgument( "--input-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputFilename, "File name of the input, used for RECONSTRUCT command" );
  args.AddArgument( "--output-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFilename, "File name of the output, used for START command (optional, default: 'PlusServerRecording.mha' for acquisition, no output for volume reconstruction)" );
  args.AddArgument( "--output-image-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputImageName, "OpenIGTLink device name of the reconstructed file (optional, default: image is not sent)" );
  args.AddArgument( "--text", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &text, "Text to be sent to the device" );
  args.AddArgument( "--transform-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &transformName, "The name of the transform to update. Form=[From]To[To]Transform" );
  args.AddArgument( "--transform-date", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &transformDate, "The date of the transform to update." );
  args.AddArgument( "--transform-error", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &transformError, "The error of the transform to update." );
  args.AddArgument( "--transform-persistent", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &transformPersistent, "The persistence of the transform to update." );
  args.AddArgument( "--transform-value", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &transformValue, "The actual transformation matrix to update." );
  args.AddArgument( "--keep-connected", vtksys::CommandLineArguments::NO_ARGUMENT, &keepConnected, "Keep the connection to the server after command completion (exits on CTRL-C).");  
  args.AddArgument( "--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)" );
  args.AddArgument( "--dicom-directory", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &dicomOutputDirectory, "The folder directory for the dicom images acquired from the StealthLink Server");
  args.AddArgument( "--volumeEmbeddedTransformToFrame", vtksys::CommandLineArguments::EQUAL_ARGUMENT,&volumeEmbeddedTransformToFrame, "The reference frame in which the dicom image will be represented. Ex: RAS,LPS,Reference,Tracker etc");
  args.AddArgument( "--keepReceivedDicomFiles", vtksys::CommandLineArguments::NO_ARGUMENT, &keepReceivedDicomFiles, "Keep the dicom files in the designated folder after having acquired them from the server");
  args.AddArgument( "--response-expected", vtksys::CommandLineArguments::NO_ARGUMENT, &responseExpected, "Wait for a response after sending text");
  args.AddArgument( "--server-config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &serverConfigFileName, "Starts a PlusServer instance with the provided config file. When this process exits, the server is stopped." );
  args.AddArgument( "--run-tests", vtksys::CommandLineArguments::NO_ARGUMENT, &runTests, "Test execution of all remote control commands. Requires a running PlusServer, which can be launched by --server-config-file");

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments." << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }
  
  if ( command.empty() && !keepConnected && !runTests)
  {
    LOG_ERROR("The program has nothing to do, as neither --command, --keep-connected, nor --run-tests is specifed"); 
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE); 
  }

  vtkPlusLogger::Instance()->SetLogLevel( verboseLevel );

  // Start a PlusServer
  vtksysProcess* plusServerProcess = NULL;
  if (!serverConfigFileName.empty())
  {
    if (StartPlusServerProcess(serverConfigFileName, plusServerProcess)!=PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to start PlusServer");
      exit(EXIT_FAILURE);
    }
  }
  // From this point PlusServer may be running, therefore before calling exit() the server process must be stopped (otherwise the process does not stop ever)

  // Connect to PlusServer
  vtkSmartPointer<vtkPlusOpenIGTLinkClient> client = vtkSmartPointer<vtkPlusOpenIGTLinkClient>::New();
  if (keepConnected)
  {
    // We use vtkPlusOpenIGTLinkClientWithTransformLogging instead of vtkPlusOpenIGTLinkClient to log the received transforms
    client = vtkSmartPointer<vtkPlusOpenIGTLinkClientWithTransformLogging>::New();
  }
  client->SetServerHost(serverHost.c_str());
  client->SetServerPort(serverPort);
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
    if (STRCASECMP(command.c_str(),"START_ACQUISITION")==0) { commandExecutionStatus = ExecuteStartAcquisition(client, deviceId); }
    else if (STRCASECMP(command.c_str(),"STOP_ACQUISITION")==0) { commandExecutionStatus = ExecuteStopAcquisition(client, deviceId, outputFilename); }
    else if (STRCASECMP(command.c_str(),"SUSPEND_ACQUISITION")==0) { commandExecutionStatus = ExecuteSuspendAcquisition(client, deviceId); }
    else if (STRCASECMP(command.c_str(),"RESUME_ACQUISITION")==0) { commandExecutionStatus = ExecuteResumeAcquisition(client, deviceId); }
    else if (STRCASECMP(command.c_str(),"START_RECONSTRUCTION")==0) { commandExecutionStatus = ExecuteStartReconstruction(client, deviceId); }
    else if (STRCASECMP(command.c_str(),"SUSPEND_RECONSTRUCTION")==0) { commandExecutionStatus = ExecuteSuspendReconstruction(client, deviceId); }
    else if (STRCASECMP(command.c_str(),"RESUME_RECONSTRUCTION")==0) { commandExecutionStatus = ExecuteResumeReconstruction(client, deviceId); }
    else if (STRCASECMP(command.c_str(),"GET_RECONSTRUCTION_SNAPSHOT")==0) { commandExecutionStatus = ExecuteGetSnapshotReconstruction(client, deviceId, outputFilename, outputImageName); }
    else if (STRCASECMP(command.c_str(),"STOP_RECONSTRUCTION")==0) { commandExecutionStatus = ExecuteStopReconstruction(client, deviceId, outputFilename, outputImageName); }
    else if (STRCASECMP(command.c_str(),"RECONSTRUCT")==0) { commandExecutionStatus = ExecuteReconstructFromFile(client, deviceId, inputFilename, outputFilename, outputImageName); }
    else if (STRCASECMP(command.c_str(),"GET_CHANNEL_IDS")==0) { commandExecutionStatus = ExecuteGetChannelIds(client); }
    else if (STRCASECMP(command.c_str(),"GET_DEVICE_IDS")==0) { commandExecutionStatus = ExecuteGetDeviceIds(client, deviceId /* actually a device type */); }
    else if (STRCASECMP(command.c_str(), "UPDATE_TRANSFORM")==0) { commandExecutionStatus = ExecuteUpdateTransform(client, transformName, transformValue, transformError, transformDate, transformPersistent); }
    else if (STRCASECMP(command.c_str(), "SAVE_CONFIG")==0) { commandExecutionStatus = ExecuteSaveConfig(client, outputFilename); }
    else if (STRCASECMP(command.c_str(), "SEND_TEXT")==0) { commandExecutionStatus = ExecuteSendText(client, deviceId, text, responseExpected); }
    else if (STRCASECMP(command.c_str(), "GET_EXAM_DATA")==0)
    {
#ifdef PLUS_USE_STEALTHLINK
      commandExecutionStatus = ExecuteGetExamData(client, deviceId,dicomOutputDirectory,volumeEmbeddedTransformToFrame,keepReceivedDicomFiles);
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
      if (PrintReply(client)!=PLUS_SUCCESS)
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
      vtkAccurateTimer::DelayWithEventProcessing(commandQueuePollIntervalSec);
    }
  }

  client->Disconnect();
  StopPlusServerProcess(plusServerProcess);
  return processReturnValue;
}
