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
#include "igtlCommon.h"
#include "igtlTrackingDataMessage.h"
#include "igtl_header.h"
#include "vtkPlusGetTransformCommand.h"
#include "vtkPlusOpenIGTLinkClient.h"
#include "vtkPlusReconstructVolumeCommand.h"
#include "vtkPlusRequestIdsCommand.h"
#include "vtkPlusSaveConfigCommand.h"
#include "vtkPlusSendTextCommand.h"
#include "vtkPlusStartStopRecordingCommand.h"
#include "vtkPlusUpdateTransformCommand.h"
#include "vtkPlusVersionCommand.h"
#include "vtkPlusIgtlMessageFactory.h"
#ifdef PLUS_USE_STEALTHLINK
  #include "vtkPlusStealthLinkCommand.h"
#endif
#include <vtkNew.h>
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
static bool StopClientRequested = false;

//----------------------------------------------------------------------------
// A customized vtkPlusOpenIGTLinkClient that can display the received transformation matrices
class vtkPlusOpenIGTLinkClientWithTransformLogging : public vtkPlusOpenIGTLinkClient
{
public:
  static vtkPlusOpenIGTLinkClientWithTransformLogging* New();
  vtkTypeMacro(vtkPlusOpenIGTLinkClientWithTransformLogging, vtkPlusOpenIGTLinkClient);

  bool OnMessageReceived(igtl::MessageHeader::Pointer messageHeader)
  {
    bool messageBodyReceived = false;
    igtl::MessageBase::Pointer bodyMsg = this->IgtlMessageFactory->CreateReceiveMessage(messageHeader);
    if (bodyMsg.IsNull())
    {
      LOG_ERROR("Unable to create message of type: " << messageHeader->GetMessageType());
      return false;
    }

    if (typeid(*bodyMsg) == typeid(igtl::TransformMessage))
    {
      igtl::TransformMessage::Pointer transformMsg = dynamic_cast<igtl::TransformMessage*>(bodyMsg.GetPointer());
      transformMsg->SetMessageHeader(messageHeader);
      transformMsg->AllocatePack();
      SocketReceive(transformMsg->GetBufferBodyPointer(), transformMsg->GetBufferBodySize());
      messageBodyReceived = true;

      int c = transformMsg->Unpack(1);
      if (!(c & igtl::MessageHeader::UNPACK_BODY))
      {
        LOG_ERROR("Failed to receive TRANSFORM reply (invalid body)");
        return messageBodyReceived;
      }

      // Store the transform data into a matrix
      igtl::Matrix4x4 mx;
      transformMsg->GetMatrix(mx);
      LOG_INFO("Matrix for " << transformMsg->GetDeviceName() << " TRANSFORM received: ");
      igtl::PrintMatrix(mx);
    }
    else
    {
      LOG_INFO("Received " << messageHeader->GetMessageType() << " message.");
    }

    return messageBodyReceived;
  }

protected:
  vtkPlusOpenIGTLinkClientWithTransformLogging() {};
  virtual ~vtkPlusOpenIGTLinkClientWithTransformLogging() {};
private:
  vtkPlusOpenIGTLinkClientWithTransformLogging(const vtkPlusOpenIGTLinkClientWithTransformLogging&);
  void operator=(const vtkPlusOpenIGTLinkClientWithTransformLogging&);
};

vtkStandardNewMacro(vtkPlusOpenIGTLinkClientWithTransformLogging);

// Utility functions for sending commands

//----------------------------------------------------------------------------
// Print command XML string
void PrintCommand(vtkPlusCommand* command)
{
  vtkSmartPointer<vtkXMLDataElement> cmdConfig = vtkSmartPointer<vtkXMLDataElement>::New();
  command->WriteConfiguration(cmdConfig);
  std::ostringstream xmlStr;
  vtkXMLUtilities::FlattenElement(cmdConfig, xmlStr);
  xmlStr << std::ends;
  LOG_INFO(">>> Command: " << xmlStr.str());
}

//----------------------------------------------------------------------------
PlusStatus ExecuteStartAcquisition(vtkPlusOpenIGTLinkClient* client, const std::string& deviceId, std::string outputFilename, bool enableCompression, int commandId)
{
  vtkSmartPointer<vtkPlusStartStopRecordingCommand> cmd = vtkSmartPointer<vtkPlusStartStopRecordingCommand>::New();
  cmd->SetNameToStart();
  cmd->SetId(commandId);
  cmd->SetOutputFilename(outputFilename.c_str());
  cmd->SetEnableCompression(enableCompression);
  if (!deviceId.empty())
  {
    cmd->SetCaptureDeviceId(deviceId.c_str());
  }
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteStopAcquisition(vtkPlusOpenIGTLinkClient* client, const std::string& deviceId, std::string outputFilename, int commandId)
{
  vtkSmartPointer<vtkPlusStartStopRecordingCommand> cmd = vtkSmartPointer<vtkPlusStartStopRecordingCommand>::New();
  cmd->SetNameToStop();
  cmd->SetId(commandId);
  cmd->SetOutputFilename(outputFilename.c_str());
  if (!deviceId.empty())
  {
    cmd->SetCaptureDeviceId(deviceId.c_str());
  }
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteSuspendAcquisition(vtkPlusOpenIGTLinkClient* client, const std::string& deviceId, int commandId)
{
  vtkSmartPointer<vtkPlusStartStopRecordingCommand> cmd = vtkSmartPointer<vtkPlusStartStopRecordingCommand>::New();
  cmd->SetNameToSuspend();
  cmd->SetId(commandId);
  if (!deviceId.empty())
  {
    cmd->SetCaptureDeviceId(deviceId.c_str());
  }
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteResumeAcquisition(vtkPlusOpenIGTLinkClient* client, const std::string& deviceId, int commandId)
{
  vtkSmartPointer<vtkPlusStartStopRecordingCommand> cmd = vtkSmartPointer<vtkPlusStartStopRecordingCommand>::New();
  cmd->SetNameToResume();
  cmd->SetId(commandId);
  if (!deviceId.empty())
  {
    cmd->SetCaptureDeviceId(deviceId.c_str());
  }
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteReconstructFromFile(vtkPlusOpenIGTLinkClient* client,
                                      const std::string& deviceId,
                                      const std::string& inputFilename,
                                      const std::string& outputFilename,
                                      const std::string& outputImageName, int commandId)
{
  vtkSmartPointer<vtkPlusReconstructVolumeCommand> cmd = vtkSmartPointer<vtkPlusReconstructVolumeCommand>::New();
  cmd->SetNameToReconstruct();
  cmd->SetId(commandId);
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
PlusStatus ExecuteStartReconstruction(vtkPlusOpenIGTLinkClient* client, const std::string& deviceId, int commandId)
{
  vtkSmartPointer<vtkPlusReconstructVolumeCommand> cmd = vtkSmartPointer<vtkPlusReconstructVolumeCommand>::New();
  cmd->SetNameToStart();
  cmd->SetId(commandId);
  if (!deviceId.empty())
  {
    cmd->SetVolumeReconstructorDeviceId(deviceId.c_str());
  }
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteSuspendReconstruction(vtkPlusOpenIGTLinkClient* client, const std::string& deviceId, int commandId)
{
  vtkSmartPointer<vtkPlusReconstructVolumeCommand> cmd = vtkSmartPointer<vtkPlusReconstructVolumeCommand>::New();
  cmd->SetNameToSuspend();
  cmd->SetId(commandId);
  if (!deviceId.empty())
  {
    cmd->SetVolumeReconstructorDeviceId(deviceId.c_str());
  }
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteResumeReconstruction(vtkPlusOpenIGTLinkClient* client, const std::string& deviceId, int commandId)
{
  vtkSmartPointer<vtkPlusReconstructVolumeCommand> cmd = vtkSmartPointer<vtkPlusReconstructVolumeCommand>::New();
  cmd->SetNameToResume();
  cmd->SetId(commandId);
  if (!deviceId.empty())
  {
    cmd->SetVolumeReconstructorDeviceId(deviceId.c_str());
  }
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteGetSnapshotReconstruction(vtkPlusOpenIGTLinkClient* client, const std::string& deviceId, const std::string& outputFilename, const std::string& outputImageName, int commandId)
{
  vtkSmartPointer<vtkPlusReconstructVolumeCommand> cmd = vtkSmartPointer<vtkPlusReconstructVolumeCommand>::New();
  cmd->SetNameToGetSnapshot();
  cmd->SetId(commandId);
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
PlusStatus ExecuteStopReconstruction(vtkPlusOpenIGTLinkClient* client, const std::string& deviceId, const std::string& outputFilename, const std::string& outputImageName, int commandId)
{
  vtkSmartPointer<vtkPlusReconstructVolumeCommand> cmd = vtkSmartPointer<vtkPlusReconstructVolumeCommand>::New();
  cmd->SetNameToStop();
  cmd->SetId(commandId);
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
PlusStatus ExecuteGetExamData(vtkPlusOpenIGTLinkClient* client, const std::string& deviceId, const std::string& dicomOutputDirectory, const std::string& volumeEmbeddedTransformToFrame,
                              const bool& keepReceivedDicomFiles, int commandId)
{
  vtkSmartPointer<vtkPlusStealthLinkCommand> cmd = vtkSmartPointer<vtkPlusStealthLinkCommand>::New();
  cmd->SetNameToGetExam();
  cmd->SetId(commandId);
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
PlusStatus ExecuteVersion(vtkPlusOpenIGTLinkClient* client, int commandId)
{
  vtkSmartPointer<vtkPlusVersionCommand> cmd = vtkSmartPointer<vtkPlusVersionCommand>::New();
  cmd->SetNameToVersion();
  cmd->SetId(commandId);
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteGetChannelIds(vtkPlusOpenIGTLinkClient* client, int commandId)
{
  vtkSmartPointer<vtkPlusRequestIdsCommand> cmd = vtkSmartPointer<vtkPlusRequestIdsCommand>::New();
  cmd->SetNameToRequestChannelIds();
  cmd->SetId(commandId);
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteGetDeviceIds(vtkPlusOpenIGTLinkClient* client, const std::string& deviceType, int commandId)
{
  vtkSmartPointer<vtkPlusRequestIdsCommand> cmd = vtkSmartPointer<vtkPlusRequestIdsCommand>::New();
  cmd->SetNameToRequestDeviceIds();
  cmd->SetId(commandId);
  cmd->SetDeviceType(deviceType.c_str());
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteUpdateTransform(vtkPlusOpenIGTLinkClient* client,
                                  const std::string& transformName,
                                  const std::string& transformValue,
                                  const std::string& transformError,
                                  const std::string& transformDate,
                                  const std::string& transformPersistent, int commandId)
{
  vtkSmartPointer<vtkPlusUpdateTransformCommand> cmd = vtkSmartPointer<vtkPlusUpdateTransformCommand>::New();
  cmd->SetNameToUpdateTransform();
  cmd->SetId(commandId);
  cmd->SetTransformName(transformName.c_str());
  double value = 0.0;
  igsioCommon::StringToDouble(transformError.c_str(), value);
  cmd->SetTransformError(value);
  cmd->SetTransformDate(transformDate.c_str());
  cmd->SetTransformPersistent(igsioCommon::IsEqualInsensitive(transformPersistent, "TRUE"));
  std::vector<std::string> elems;
  vtkMatrix4x4* transformValueMatrix = vtkMatrix4x4::New();
  igsioCommon::SplitStringIntoTokens(transformValue, ' ', elems);
  if (elems.size() != 16)
  {
    LOG_ERROR("Invalid formatting of matrix string.");
    return PLUS_FAIL;
  }
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      igsioCommon::StringToDouble((elems[i * 4 + j]).c_str(), value);
      transformValueMatrix->SetElement(i, j, value);
    }
  }
  cmd->SetTransformValue(transformValueMatrix);
  transformValueMatrix->Delete();
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteGetTransform(vtkPlusOpenIGTLinkClient* client, const std::string& transformName, int commandId)
{
  vtkSmartPointer<vtkPlusGetTransformCommand> cmd = vtkSmartPointer<vtkPlusGetTransformCommand>::New();
  cmd->SetNameToGetTransform();
  cmd->SetId(commandId);
  cmd->SetTransformName(transformName.c_str());
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteGetPoint(vtkPlusOpenIGTLinkClient* client, const std::string& inputFilename)
{
  vtkNew<vtkPlusIgtlMessageFactory> factory;
  igtl::MessageBase::Pointer msg = factory->CreateSendMessage("GET_POINT", IGTL_HEADER_VERSION_2);
  msg->AllocateBuffer();
  msg->SetMetaDataElement("Filename", IANA_TYPE_US_ASCII, inputFilename);
  msg->Pack();
  return client->SendMessage(msg);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteSaveConfig(vtkPlusOpenIGTLinkClient* client, const std::string& outputFilename, int commandId)
{
  vtkSmartPointer<vtkPlusSaveConfigCommand> cmd = vtkSmartPointer<vtkPlusSaveConfigCommand>::New();
  cmd->SetNameToSaveConfig();
  cmd->SetId(commandId);
  cmd->SetFilename(outputFilename.c_str());
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteStartTDATA(vtkPlusOpenIGTLinkClient* client, int commandId)
{
  vtkSmartPointer<vtkPlusIgtlMessageFactory> factory = vtkSmartPointer<vtkPlusIgtlMessageFactory>::New();
  igtl::MessageBase::Pointer msg = factory->CreateSendMessage("STT_TDATA", IGTL_HEADER_VERSION_1);
  igtl::StartTrackingDataMessage* startMsg = dynamic_cast<igtl::StartTrackingDataMessage*>(msg.GetPointer());
  startMsg->SetResolution(25);
  startMsg->Pack();
  return client->SendMessage(msg);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteStopTDATA(vtkPlusOpenIGTLinkClient* client, int commandId)
{
  vtkSmartPointer<vtkPlusIgtlMessageFactory> factory = vtkSmartPointer<vtkPlusIgtlMessageFactory>::New();
  igtl::MessageBase::Pointer msg = factory->CreateSendMessage("STP_TDATA", IGTL_HEADER_VERSION_1);
  igtl::StopTrackingDataMessage* stopMsg = dynamic_cast<igtl::StopTrackingDataMessage*>(msg.GetPointer());
  stopMsg->Pack();
  return client->SendMessage(msg);
}

//----------------------------------------------------------------------------
PlusStatus ExecuteSendText(vtkPlusOpenIGTLinkClient* client, const std::string& deviceId, const std::string& text, bool responseExpected, int commandId)
{
  vtkSmartPointer<vtkPlusSendTextCommand> cmd = vtkSmartPointer<vtkPlusSendTextCommand>::New();
  cmd->SetNameToSendText();
  cmd->SetId(commandId);
  cmd->SetText(text.c_str());
  cmd->SetDeviceId(deviceId.c_str());
  cmd->SetResponseExpected(responseExpected);
  PrintCommand(cmd);
  return client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus ReceiveAndPrintReply(vtkPlusOpenIGTLinkClient* client,
                                bool& didTimeout,
                                std::string& outContent,
                                std::string& outErrorMessage,
                                igtl::MessageBase::MetaDataMap& parameters,
                                int timeoutSec = 30)
{
  int32_t commandId;
  std::string commandName;

  PlusStatus result;

  const double replyTimeoutSec = timeoutSec;
  if (client->ReceiveReply(result, commandId, outErrorMessage, outContent, parameters, commandName, replyTimeoutSec) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to receive reply to the command");
    didTimeout = true;
    return PLUS_FAIL;
  }

  LOG_INFO("Command ID: " << commandId);
  LOG_INFO("Status: " << (result == PLUS_SUCCESS ? "SUCCESS" : "FAIL"));
  if (result == PLUS_FAIL)
  {
    LOG_INFO("Error: " << outErrorMessage);
  }
  LOG_INFO("Message: " << outContent);
  for (igtl::MessageBase::MetaDataMap::const_iterator it = parameters.begin(); it != parameters.end(); ++it)
  {
    LOG_INFO(it->first << ": " << it->second.second);
  }

  return result;
}

//----------------------------------------------------------------------------
PlusStatus StartPlusServerProcess(const std::string& configFile, vtksysProcess*& processPtr)
{
  processPtr = NULL;
  std::string executablePath = vtkPlusConfig::GetInstance()->GetPlusExecutablePath("PlusServer");

  if (!vtksys::SystemTools::FileExists(executablePath.c_str(), true))
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
    std::string param1 = std::string("--config-file=") + configFile;
    command.push_back(param1.c_str()); // command parameter
    command.push_back(0); // The array must end with a NULL pointer.
    vtksysProcess_SetCommand(processPtr, &*command.begin());

    // Redirect PlusServer output to this process output (otherwise server execution would be blocked)
    vtksysProcess_SetPipeFile(processPtr, vtksysProcess_Pipe_STDOUT, "PlusServerRemoteControlStdOut.log");
    vtksysProcess_SetPipeFile(processPtr, vtksysProcess_Pipe_STDERR, "PlusServerRemoteControlStdErr.log");

    LOG_INFO("Start PlusServer...");
    vtksysProcess_Execute(processPtr);
    LOG_DEBUG("PlusServer started");

    return PLUS_SUCCESS;
  }
  catch (...)
  {
    LOG_ERROR("Failed to start PlusServer");
    return PLUS_FAIL;
  }
}

//----------------------------------------------------------------------------
void StopPlusServerProcess(vtksysProcess*& processPtr)
{
  if (processPtr == NULL)
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
  const char captureDeviceId[] = "CaptureDevice";
  const char capturingOutputFileName[] = "OpenIGTTrackedVideoRecordingTest.mha"; // must match the extension defined in the config file

  const char volumeReconstructionDeviceId[] = "VolumeReconstructorDevice";
  const char* batchReconstructionInputFileName = capturingOutputFileName;
  const char batchReconstructionOutputImageName[] = "VolumeReconstructedBatch";
  const char snapshotReconstructionOutputImageName[] = "VolumeReconstructedSnapshot";
  const char liveReconstructionOutputImageName[] = "VolumeReconstructedLive";
  const char batchReconstructionOutputFileName[] = "VolumeReconstructedBatch.nrrd";
  const char snapshotReconstructionOutputFileName[] = "VolumeReconstructedSnapshot.nrrd";
  const char liveReconstructionOutputFileName[] = "VolumeReconstructedLive.nrrd";

  std::string replyMessage;
  std::string errorMessage;
  igtl::MessageBase::MetaDataMap parameters;
  int commandId = 1;
  bool didTimeout(false);

  if (client->GetServerIGTLVersion() >= OpenIGTLink_PROTOCOL_VERSION_3)
  {
    ExecuteVersion(client, commandId++);
    PlusStatus result = ReceiveAndPrintReply(client, didTimeout, replyMessage, errorMessage, parameters, 3);
    if (result == PLUS_FAIL && didTimeout && client->GetServerIGTLVersion() >= OpenIGTLink_PROTOCOL_VERSION_3)
    {
      // Version 3 and greater expect a reply to the version command, if it timed out, it is an error
      LOG_ERROR("Version handshake to the server timed out but it was unexpected.");
      exit(EXIT_FAILURE);
    }
    else if (result == PLUS_SUCCESS && parameters.find("Version") != parameters.end())
    {
      LOG_INFO("Server IGTL Version: " << parameters["Version"].second);
    }
    else
    {
      LOG_ERROR("Version handshake to the server failed.");
      exit(EXIT_FAILURE);
    }
    parameters.clear();
  }

  // Basic commands
  ExecuteGetChannelIds(client, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, didTimeout, replyMessage, errorMessage, parameters));
  if (client->GetServerIGTLVersion() < OpenIGTLink_PROTOCOL_VERSION_3)
  {
    if (replyMessage != "TrackedVideoStream")
    {
      LOG_ERROR("Incorrect reply sent. Got: " << replyMessage << ". Expected: \"TrackedVideoStream\"");
    }
  }
  else
  {
    if (parameters.size() == 0 || parameters.find("TrackedVideoStream") == parameters.end())
    {
      LOG_ERROR("Empty result or entry not found in list of device IDs.");
    }
    else
    {
      if (!igsioCommon::IsEqualInsensitive(parameters["TrackedVideoStream"].second, "TrackedVideoStream"))
      {
        LOG_ERROR("Incorrect parameter returned. Got: " << parameters["TrackedVideoStream"].second << ". Expected: \"TrackedVideoStream\"");
      }
    }
  }
  parameters.clear();

  ExecuteGetDeviceIds(client, "VirtualVolumeReconstructor", commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, didTimeout, replyMessage, errorMessage, parameters));
  if (client->GetServerIGTLVersion() < OpenIGTLink_PROTOCOL_VERSION_3)
  {
    if (replyMessage != "VolumeReconstructorDevice")
    {
      LOG_ERROR("Incorrect reply sent. Got: " << replyMessage << ". Expected: \"VolumeReconstructorDevice\"");
    }
  }
  else
  {
    if (parameters.size() == 0)
    {
      LOG_ERROR("Empty result returned for list of device IDs.");
    }
    else
    {
      if (!igsioCommon::IsEqualInsensitive(parameters["VolumeReconstructorDevice"].second, "VolumeReconstructorDevice"))
      {
        LOG_ERROR("Incorrect parameter returned.");
      }
    }
  }
  parameters.clear();

  ExecuteUpdateTransform(client, "Test1ToReference", "1 0 0 10 0 1.2 0.1 12 0.1 0.2 -0.9 -20 0 0 0 1", "1.4", "100314_182141", "TRUE", commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, didTimeout, replyMessage, errorMessage, parameters));
  parameters.clear();
  ExecuteGetTransform(client, "Test1ToReference", commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, didTimeout, replyMessage, errorMessage, parameters));
  parameters.clear();
  ExecuteSaveConfig(client, "Test1SavedConfig.xml", commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, didTimeout, replyMessage, errorMessage, parameters));
  parameters.clear();
  ExecuteStartTDATA(client, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, didTimeout, replyMessage, errorMessage, parameters));
  parameters.clear();
  ExecuteStopTDATA(client, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, didTimeout, replyMessage, errorMessage, parameters));
  parameters.clear();

  // Capturing
  ExecuteStartAcquisition(client, captureDeviceId, capturingOutputFileName, false, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, didTimeout, replyMessage, errorMessage, parameters));
  parameters.clear();
  vtkIGSIOAccurateTimer::DelayWithEventProcessing(2.0);
  ExecuteSuspendAcquisition(client, captureDeviceId, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, didTimeout, replyMessage, errorMessage, parameters));
  parameters.clear();
  vtkIGSIOAccurateTimer::DelayWithEventProcessing(2.0);
  ExecuteResumeAcquisition(client, captureDeviceId, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, didTimeout, replyMessage, errorMessage, parameters));
  parameters.clear();
  vtkIGSIOAccurateTimer::DelayWithEventProcessing(2.0);
  ExecuteStopAcquisition(client, captureDeviceId, capturingOutputFileName, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, didTimeout, replyMessage, errorMessage, parameters));
  parameters.clear();
  vtkIGSIOAccurateTimer::DelayWithEventProcessing(2.0);

  // Volume reconstruction from file
  ExecuteReconstructFromFile(client, volumeReconstructionDeviceId, batchReconstructionInputFileName, batchReconstructionOutputFileName, batchReconstructionOutputImageName, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, didTimeout, replyMessage, errorMessage, parameters));
  parameters.clear();
  vtkIGSIOAccurateTimer::DelayWithEventProcessing(2.0);

  // Live volume reconstruction
  ExecuteStartReconstruction(client, volumeReconstructionDeviceId, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, didTimeout, replyMessage, errorMessage, parameters));
  parameters.clear();
  vtkIGSIOAccurateTimer::DelayWithEventProcessing(2.0);
  ExecuteSuspendReconstruction(client, volumeReconstructionDeviceId, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, didTimeout, replyMessage, errorMessage, parameters));
  parameters.clear();
  vtkIGSIOAccurateTimer::DelayWithEventProcessing(2.0);
  ExecuteResumeReconstruction(client, volumeReconstructionDeviceId, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, didTimeout, replyMessage, errorMessage, parameters));
  parameters.clear();
  vtkIGSIOAccurateTimer::DelayWithEventProcessing(2.0);
  ExecuteGetSnapshotReconstruction(client, volumeReconstructionDeviceId, snapshotReconstructionOutputFileName, snapshotReconstructionOutputImageName, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, didTimeout, replyMessage, errorMessage, parameters));
  parameters.clear();
  vtkIGSIOAccurateTimer::DelayWithEventProcessing(2.0);
  ExecuteGetSnapshotReconstruction(client, volumeReconstructionDeviceId, snapshotReconstructionOutputFileName, snapshotReconstructionOutputImageName, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, didTimeout, replyMessage, errorMessage, parameters));
  parameters.clear();
  vtkIGSIOAccurateTimer::DelayWithEventProcessing(2.0);
  ExecuteGetSnapshotReconstruction(client, volumeReconstructionDeviceId, snapshotReconstructionOutputFileName, snapshotReconstructionOutputImageName, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, didTimeout, replyMessage, errorMessage, parameters));
  parameters.clear();
  vtkIGSIOAccurateTimer::DelayWithEventProcessing(2.0);
  ExecuteStopReconstruction(client, volumeReconstructionDeviceId, liveReconstructionOutputFileName, liveReconstructionOutputImageName, commandId++);
  RETURN_IF_FAIL(ReceiveAndPrintReply(client, didTimeout, replyMessage, errorMessage, parameters));
  parameters.clear();
  vtkIGSIOAccurateTimer::DelayWithEventProcessing(2.0);

  return PLUS_SUCCESS;
}

// -------------------------------------------------
// For CTRL-C signal handling
void SignalInterruptHandler(int s)
{
  StopClientRequested = true;
}

//----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  // Check command line arguments.
  std::string serverHost = "127.0.0.1";
  int serverPort = 18944;
  std::string command;
  std::string deviceId;
  std::string inputFilename = "PlusServerRecording.nrrd";
  std::string outputFilename;
  std::string outputImageName;
  std::string transformName;
  std::string transformError;
  std::string transformDate;
  std::string transformPersistent;
  std::string transformValue;
  std::string dicomOutputDirectory;
  std::string volumeEmbeddedTransformToFrame;
  int serverHeaderVersion(-1);
  std::string text;
  bool keepReceivedDicomFiles = false;
  bool responseExpected = false;
  bool enableCompression = false;
  int verboseLevel = vtkIGSIOLogger::LOG_LEVEL_UNDEFINED;
  bool keepConnected = false;
  std::string serverConfigFileName;
  bool runTests = false;
  int serverIGTLVersion(-1);
  int commandId(0);

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--host", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &serverHost, "Host name of the OpenIGTLink server (default: 127.0.0.1)");
  args.AddArgument("--port", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &serverPort, "Port address of the OpenIGTLink server (default: 18944)");
  args.AddArgument("--command", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &command,
                   "Command name to be executed on the server (START_ACQUISITION, STOP_ACQUISITION, SUSPEND_ACQUISITION, RESUME_ACQUISITION, RECONSTRUCT, START_RECONSTRUCTION, SUSPEND_RECONSTRUCTION, RESUME_RECONSTRUCTION, STOP_RECONSTRUCTION, GET_RECONSTRUCTION_SNAPSHOT, GET_CHANNEL_IDS, GET_DEVICE_IDS, GET_EXAM_DATA, SEND_TEXT, UPDATE_TRANSFORM, GET_TRANSFORM, GET_POINT)");
  args.AddArgument("--command-id", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &commandId, "Command ID to send to the server.");
  args.AddArgument("--server-igtl-version", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &serverHeaderVersion, "The version of IGTL used by the server. Remove this parameter when querying is dynamic.");
  args.AddArgument("--device", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &deviceId, "ID of the controlled device (optional, default: first VirtualStreamCapture or VirtualVolumeReconstructor device). In case of GET_DEVICE_IDS it is not an ID but a device type.");
  args.AddArgument("--input-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputFilename, "File name of the input, used for RECONSTRUCT command");
  args.AddArgument("--output-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFilename, "File name of the output, used for START command (optional, default: 'PlusServerRecording.nrrd' for acquisition, no output for volume reconstruction)");
  args.AddArgument("--output-image-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputImageName, "OpenIGTLink device name of the reconstructed file (optional, default: image is not sent)");
  args.AddArgument("--text", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &text, "Text to be sent to the device");
  args.AddArgument("--transform-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &transformName, "The name of the transform to update. Form=[From]To[To]Transform");
  args.AddArgument("--transform-date", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &transformDate, "The date of the transform to update.");
  args.AddArgument("--transform-error", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &transformError, "The error of the transform to update.");
  args.AddArgument("--transform-persistent", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &transformPersistent, "The persistence of the transform to update.");
  args.AddArgument("--transform-value", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &transformValue, "The actual transformation matrix to update.");
  args.AddArgument("--use-compression", vtksys::CommandLineArguments::NO_ARGUMENT, &enableCompression, "Set capture device to record compressed data. Only supported with .nrrd capture.");
  args.AddArgument("--keep-connected", vtksys::CommandLineArguments::NO_ARGUMENT, &keepConnected, "Keep the connection to the server after command completion (exits on CTRL-C).");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");
  args.AddArgument("--dicom-directory", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &dicomOutputDirectory, "The folder directory for the dicom images acquired from the StealthLink Server");
  args.AddArgument("--volumeEmbeddedTransformToFrame", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &volumeEmbeddedTransformToFrame, "The reference frame in which the dicom image will be represented. Ex: RAS,LPS,Reference,Tracker etc");
  args.AddArgument("--keepReceivedDicomFiles", vtksys::CommandLineArguments::NO_ARGUMENT, &keepReceivedDicomFiles, "Keep the dicom files in the designated folder after having acquired them from the server");
  args.AddArgument("--response-expected", vtksys::CommandLineArguments::NO_ARGUMENT, &responseExpected, "Wait for a response after sending text");
  args.AddArgument("--server-config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &serverConfigFileName, "Starts a PlusServer instance with the provided config file. When this process exits, the server is stopped.");
  args.AddArgument("--run-tests", vtksys::CommandLineArguments::NO_ARGUMENT, &runTests, "Test execution of all remote control commands. Requires a running PlusServer, which can be launched by --server-config-file");

  if (!args.Parse())
  {
    std::cerr << "Problem parsing arguments." << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if (command.empty() && !keepConnected && !runTests)
  {
    LOG_ERROR("The program has nothing to do, as neither --command, --keep-connected, nor --run-tests is specifed");
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkIGSIOLogger::Instance()->SetLogLevel(verboseLevel);

  // Start a PlusServer
  vtksysProcess* plusServerProcess = NULL;
  if (!serverConfigFileName.empty())
  {
    if (StartPlusServerProcess(serverConfigFileName, plusServerProcess) != PLUS_SUCCESS)
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
  if (serverIGTLVersion > 0)
  {
    client->SetServerIGTLVersion(serverIGTLVersion);
  }
  if (client->Connect(15.0) == PLUS_FAIL)
  {
    LOG_ERROR("Failed to connect to server at " << serverHost << ":" << serverPort);
    StopPlusServerProcess(plusServerProcess);
    exit(EXIT_FAILURE);
  }

  int processReturnValue = EXIT_SUCCESS;

  // Run a command
  if (!command.empty())
  {
    PlusStatus commandExecutionStatus = PLUS_SUCCESS;
    // Execute command
    if (igsioCommon::IsEqualInsensitive(command, "START_ACQUISITION"))
    {
      commandExecutionStatus = ExecuteStartAcquisition(client, deviceId, outputFilename, enableCompression, commandId);
    }
    else if (igsioCommon::IsEqualInsensitive(command, "STOP_ACQUISITION"))
    {
      commandExecutionStatus = ExecuteStopAcquisition(client, deviceId, outputFilename, commandId);
    }
    else if (igsioCommon::IsEqualInsensitive(command, "SUSPEND_ACQUISITION"))
    {
      commandExecutionStatus = ExecuteSuspendAcquisition(client, deviceId, commandId);
    }
    else if (igsioCommon::IsEqualInsensitive(command, "RESUME_ACQUISITION"))
    {
      commandExecutionStatus = ExecuteResumeAcquisition(client, deviceId, commandId);
    }
    else if (igsioCommon::IsEqualInsensitive(command, "START_RECONSTRUCTION"))
    {
      commandExecutionStatus = ExecuteStartReconstruction(client, deviceId, commandId);
    }
    else if (igsioCommon::IsEqualInsensitive(command, "SUSPEND_RECONSTRUCTION"))
    {
      commandExecutionStatus = ExecuteSuspendReconstruction(client, deviceId, commandId);
    }
    else if (igsioCommon::IsEqualInsensitive(command, "RESUME_RECONSTRUCTION"))
    {
      commandExecutionStatus = ExecuteResumeReconstruction(client, deviceId, commandId);
    }
    else if (igsioCommon::IsEqualInsensitive(command, "GET_RECONSTRUCTION_SNAPSHOT"))
    {
      commandExecutionStatus = ExecuteGetSnapshotReconstruction(client, deviceId, outputFilename, outputImageName, commandId);
    }
    else if (igsioCommon::IsEqualInsensitive(command, "STOP_RECONSTRUCTION"))
    {
      commandExecutionStatus = ExecuteStopReconstruction(client, deviceId, outputFilename, outputImageName, commandId);
    }
    else if (igsioCommon::IsEqualInsensitive(command, "RECONSTRUCT"))
    {
      commandExecutionStatus = ExecuteReconstructFromFile(client, deviceId, inputFilename, outputFilename, outputImageName, commandId);
    }
    else if (igsioCommon::IsEqualInsensitive(command, "GET_CHANNEL_IDS"))
    {
      commandExecutionStatus = ExecuteGetChannelIds(client, commandId);
    }
    else if (igsioCommon::IsEqualInsensitive(command, "GET_DEVICE_IDS"))
    {
      commandExecutionStatus = ExecuteGetDeviceIds(client, deviceId /* actually a device type */, commandId);
    }
    else if (igsioCommon::IsEqualInsensitive(command, "UPDATE_TRANSFORM"))
    {
      commandExecutionStatus = ExecuteUpdateTransform(client, transformName, transformValue, transformError, transformDate, transformPersistent, commandId);
    }
    else if (igsioCommon::IsEqualInsensitive(command, "SAVE_CONFIG"))
    {
      commandExecutionStatus = ExecuteSaveConfig(client, outputFilename, commandId);
    }
    else if (igsioCommon::IsEqualInsensitive(command, "SEND_TEXT"))
    {
      commandExecutionStatus = ExecuteSendText(client, deviceId, text, responseExpected, commandId);
    }
    else if (igsioCommon::IsEqualInsensitive(command, "GET_TRANSFORM"))
    {
      commandExecutionStatus = ExecuteGetTransform(client, transformName, commandId);
    }
    else if (igsioCommon::IsEqualInsensitive(command, "GET_POINT"))
    {
      commandExecutionStatus = ExecuteGetPoint(client, inputFilename);
    }
    else if (igsioCommon::IsEqualInsensitive(command, "GET_EXAM_DATA"))
    {
#ifdef PLUS_USE_STEALTHLINK
      commandExecutionStatus = ExecuteGetExamData(client, deviceId, dicomOutputDirectory, volumeEmbeddedTransformToFrame, keepReceivedDicomFiles, commandId);
#else
      LOG_ERROR("Plus is not built with StealthLink support");
      commandExecutionStatus = PLUS_FAIL;
#endif
    }
    else
    {
      LOG_ERROR("Unknown command: " << command);
      client->Disconnect();
      commandExecutionStatus = PLUS_FAIL;
    }
    if (commandExecutionStatus == PLUS_SUCCESS)
    {
      std::string replyMessage;
      std::string errorMessage;
      bool didTimeout;
      igtl::MessageBase::MetaDataMap parameters;
      if (ReceiveAndPrintReply(client, didTimeout, replyMessage, errorMessage, parameters) != PLUS_SUCCESS)
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
    if (RunTests(client) != PLUS_SUCCESS)
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
    const double commandQueuePollIntervalSec = 0.010;
    while (!StopClientRequested)
    {
      // the customized client logs the transformation matrices in the data receiver thread
      vtkIGSIOAccurateTimer::DelayWithEventProcessing(commandQueuePollIntervalSec);
    }
  }

  client->Disconnect();
  StopPlusServerProcess(plusServerProcess);
  return processReturnValue;
}