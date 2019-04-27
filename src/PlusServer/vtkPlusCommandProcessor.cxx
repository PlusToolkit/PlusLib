/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkIGSIORecursiveCriticalSection.h"
#include "vtkPlusCommandProcessor.h"

// Command includes
#include "vtkPlusCommand.h"
#include "vtkPlusGetImageCommand.h"
#include "vtkPlusReconstructVolumeCommand.h"
#ifdef PLUS_USE_STEALTHLINK
  #include "vtkPlusStealthLinkCommand.h"
#endif
#ifdef PLUS_USE_CLARIUS
#include "vtkPlusClariusCommand.h"
#endif
#ifdef PLUS_USE_OPTIMET_CONOPROBE
  #include "vtkPlusConoProbeLinkCommand.h"
#endif
#ifdef PLUS_USE_ATRACSYS
  #include "vtkPlusAtracsysCommand.h"
#endif

#include "vtkPlusAddRecordingDeviceCommand.h"
#include "vtkPlusGetPolydataCommand.h"
#include "vtkPlusGetTransformCommand.h"
#include "vtkPlusGetUsParameterCommand.h"
#include "vtkPlusRequestIdsCommand.h"
#include "vtkPlusSaveConfigCommand.h"
#include "vtkPlusSendTextCommand.h"
#include "vtkPlusSetUsParameterCommand.h"
#include "vtkPlusStartStopRecordingCommand.h"
#include "vtkPlusUpdateTransformCommand.h"
#include "vtkPlusVersionCommand.h"

// IGTL includes
#include "igtl_header.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkXMLUtilities.h>

vtkStandardNewMacro(vtkPlusCommandProcessor);

//----------------------------------------------------------------------------
vtkPlusCommandProcessor::vtkPlusCommandProcessor()
  : PlusServer(NULL)
  , Threader(vtkSmartPointer<vtkMultiThreader>::New())
  , Mutex(vtkSmartPointer<vtkIGSIORecursiveCriticalSection>::New())
  , CommandExecutionActive(std::make_pair(false, false))
  , CommandExecutionThreadId(-1)
{
  // Register default commands
  RegisterPlusCommand(vtkSmartPointer<vtkPlusGetImageCommand>::New());
  RegisterPlusCommand(vtkSmartPointer<vtkPlusGetPolydataCommand>::New());
  RegisterPlusCommand(vtkSmartPointer<vtkPlusGetTransformCommand>::New());
  RegisterPlusCommand(vtkSmartPointer<vtkPlusReconstructVolumeCommand>::New());
  RegisterPlusCommand(vtkSmartPointer<vtkPlusRequestIdsCommand>::New());
  RegisterPlusCommand(vtkSmartPointer<vtkPlusSaveConfigCommand>::New());
  RegisterPlusCommand(vtkSmartPointer<vtkPlusSendTextCommand>::New());
  RegisterPlusCommand(vtkSmartPointer<vtkPlusStartStopRecordingCommand>::New());
  RegisterPlusCommand(vtkSmartPointer<vtkPlusUpdateTransformCommand>::New());
  RegisterPlusCommand(vtkSmartPointer<vtkPlusVersionCommand>::New());
  RegisterPlusCommand(vtkSmartPointer<vtkPlusSetUsParameterCommand>::New());
  RegisterPlusCommand(vtkSmartPointer<vtkPlusGetUsParameterCommand>::New());
  RegisterPlusCommand(vtkSmartPointer<vtkPlusAddRecordingDeviceCommand>::New());
#ifdef PLUS_USE_STEALTHLINK
  RegisterPlusCommand(vtkSmartPointer<vtkPlusStealthLinkCommand>::New());
#endif
#ifdef PLUS_USE_CLARIUS
  RegisterPlusCommand(vtkSmartPointer<vtkPlusClariusCommand>::New());
#endif
#ifdef PLUS_USE_OPTIMET_CONOPROBE
  RegisterPlusCommand(vtkSmartPointer<vtkPlusConoProbeLinkCommand>::New());
#endif
}

//----------------------------------------------------------------------------
vtkPlusCommandProcessor::~vtkPlusCommandProcessor()
{
  SetPlusServer(NULL);
}

//----------------------------------------------------------------------------
void vtkPlusCommandProcessor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Registered commands: ";
  for (auto iter = this->RegisteredCommands.begin(); iter != this->RegisteredCommands.end(); ++iter)
  {
    os << indent << "  " << iter->first << std::endl;
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCommandProcessor::Start()
{
  if (this->CommandExecutionThreadId < 0)
  {
    this->CommandExecutionActive.first = true;
    this->CommandExecutionThreadId = this->Threader->SpawnThread((vtkThreadFunctionType)&CommandExecutionThread, this);
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCommandProcessor::Stop()
{
  // Stop the command execution thread
  if (this->CommandExecutionThreadId >= 0)
  {
    this->CommandExecutionActive.first = false;
    while (this->CommandExecutionActive.second)
    {
      // Wait until the thread stops
      vtkIGSIOAccurateTimer::Delay(0.2);
    }
    this->CommandExecutionThreadId = -1;
  }

  LOG_DEBUG("Command execution thread stopped");

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void* vtkPlusCommandProcessor::CommandExecutionThread(vtkMultiThreader::ThreadInfo* data)
{
  vtkPlusCommandProcessor* self = (vtkPlusCommandProcessor*)(data->UserData);

  self->CommandExecutionActive.second = true;

  // Execute commands until a stop is requested
  while (self->CommandExecutionActive.first)
  {
    self->ExecuteCommands();
    // no commands in the queue, wait a bit before checking again
    const double commandQueuePollIntervalSec = 0.010;
#ifdef _WIN32
    Sleep(commandQueuePollIntervalSec * 1000);
#else
    usleep(commandQueuePollIntervalSec * 1000000);
#endif
  }

  // Close thread
  self->CommandExecutionThreadId = -1;
  self->CommandExecutionActive.second = false;
  return NULL;
}

//----------------------------------------------------------------------------
int vtkPlusCommandProcessor::ExecuteCommands()
{
  // Implemented in a while loop to not block the mutex during command execution, only during management of the queue.
  int numberOfExecutedCommands(0);
  while (1)
  {
    vtkSmartPointer<vtkPlusCommand> cmd; // next command to be processed
    {
      igsioLockGuard<vtkIGSIORecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);
      if (this->CommandQueue.empty())
      {
        return numberOfExecutedCommands;
      }
      cmd = this->CommandQueue.front();
      this->CommandQueue.pop_front();
    }

    LOG_DEBUG("Executing command");
    if (cmd->Execute() != PLUS_SUCCESS)
    {
      LOG_ERROR("Command execution failed");
    }

    // move the response objects from the command to the processor's queue
    {
      igsioLockGuard<vtkIGSIORecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);
      cmd->PopCommandResponses(this->CommandResponseQueue);
    }

    numberOfExecutedCommands++;
  }

  // we never actually reach this point
  return numberOfExecutedCommands;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCommandProcessor::RegisterPlusCommand(vtkPlusCommand* cmd)
{
  if (cmd == NULL)
  {
    LOG_ERROR("vtkPlusCommandProcessor::RegisterPlusCommand received an invalid command object");
    return PLUS_FAIL;
  }

  std::list<std::string> cmdNames;
  cmd->GetCommandNames(cmdNames);
  if (cmdNames.empty())
  {
    LOG_ERROR("Cannot register command: command name is empty");
    return PLUS_FAIL;
  }

  for (std::list<std::string>::iterator nameIt = cmdNames.begin(); nameIt != cmdNames.end(); ++nameIt)
  {
    this->RegisteredCommands[*nameIt] = cmd;
    cmd->Register(this);
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
vtkPlusCommand* vtkPlusCommandProcessor::CreatePlusCommand(const std::string& commandName, const std::string& commandStr, const igtl::MessageBase::MetaDataMap& metaData)
{
  vtkSmartPointer<vtkXMLDataElement> cmdElement = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromString(commandStr.c_str()));
  if (cmdElement.GetPointer() == NULL)
  {
    LOG_ERROR("failed to parse XML command string (received: " + commandStr + ")");
    return NULL;
  }

  if (STRCASECMP(cmdElement->GetName(), "Command") != 0)
  {
    LOG_ERROR("Command element expected (received: " + commandStr + ")");
    return NULL;
  }

  if (this->RegisteredCommands.find(commandName) == this->RegisteredCommands.end())
  {
    // unregistered command
    LOG_ERROR("Unknown command: " << commandName);
    return NULL;
  }

  vtkPlusCommand* cmd = (this->RegisteredCommands[commandName])->Clone();
  cmd->SetMetaData(metaData);
  if (cmd->ReadConfiguration(cmdElement) != PLUS_SUCCESS)
  {
    cmd->Delete();
    cmd = NULL;
    LOG_ERROR("Failed to initialize command from string: " + commandStr);
    return NULL;
  }
  return cmd;
}

//------------------------------------------------------------------------------
PlusStatus vtkPlusCommandProcessor::QueueCommand(bool respondUsingIGTLCommand, unsigned int clientId, const std::string& commandName, const std::string& commandString, const std::string& deviceName, uint32_t uid, const igtl::MessageBase::MetaDataMap& metaData)
{
  if (commandString.empty())
  {
    LOG_ERROR("Command string is undefined");
    return PLUS_FAIL;
  }

  if (commandName.empty())
  {
    LOG_ERROR("Command name is undefined");
    return PLUS_FAIL;
  }

  vtkSmartPointer<vtkPlusCommand> cmd = vtkSmartPointer<vtkPlusCommand>::Take(CreatePlusCommand(commandName, commandString, metaData));
  if (cmd.GetPointer() == NULL)
  {
    if (!respondUsingIGTLCommand)
    {
      this->QueueStringResponse(PLUS_FAIL, deviceName, clientId, std::string("Error attempting to process command."));
    }
    else
    {
      this->QueueCommandResponse(PLUS_FAIL, deviceName, clientId, commandName, uid, "Error attempting to process command.", "Unknown command type requested.");
    }
    return PLUS_FAIL;
  }

  cmd->SetCommandProcessor(this);
  cmd->SetClientId(clientId);
  cmd->SetDeviceName(deviceName.c_str());
  cmd->SetId(uid);
  cmd->SetRespondWithCommandMessage(respondUsingIGTLCommand);

  // Add command to the execution queue
  igsioLockGuard<vtkIGSIORecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);
  this->CommandQueue.push_back(cmd);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCommandProcessor::QueueStringResponse(PlusStatus status, const std::string& deviceName, unsigned int clientId, const std::string& replyString)
{
  vtkSmartPointer<vtkPlusCommandStringResponse> response = vtkSmartPointer<vtkPlusCommandStringResponse>::New();
  response->SetDeviceName(deviceName);
  std::ostringstream replyStr;
  replyStr << "<CommandReply";
  replyStr << " Status=\"" << (status == PLUS_SUCCESS ? "SUCCESS" : "FAIL") << "\"";
  replyStr << " Message=\"";
  // Write to XML, encoding special characters, such as " ' \ < > &
  vtkXMLUtilities::EncodeString(replyString.c_str(), VTK_ENCODING_NONE, replyStr, VTK_ENCODING_NONE, 1 /* encode special characters */);
  replyStr << "\"";
  replyStr << " />";

  response->SetMessage(replyStr.str());
  response->SetClientId(clientId);
  response->SetStatus(status);

  // Add response to the command response queue
  igsioLockGuard<vtkIGSIORecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);
  this->CommandResponseQueue.push_back(response);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCommandProcessor::QueueCommandResponse(PlusStatus status, const std::string& deviceName, unsigned int clientId, const std::string& commandName, uint32_t uid, const std::string& replyString, const std::string& errorString)
{
  vtkSmartPointer<vtkPlusCommandRTSCommandResponse> response = vtkSmartPointer<vtkPlusCommandRTSCommandResponse>::New();
  response->SetClientId(clientId);
  response->SetDeviceName(deviceName);
  response->SetOriginalId(uid);
  response->SetRespondWithCommandMessage(true);
  response->SetErrorString(errorString);
  response->SetStatus(status);

  // Add response to the command response queue
  igsioLockGuard<vtkIGSIORecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);
  this->CommandResponseQueue.push_back(response);

  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
PlusStatus vtkPlusCommandProcessor::QueueGetImageMetaData(unsigned int clientId, const std::string& deviceName)
{
  vtkSmartPointer<vtkPlusGetImageCommand> cmdGetImage = vtkSmartPointer<vtkPlusGetImageCommand>::New();
  cmdGetImage->SetCommandProcessor(this);
  cmdGetImage->SetClientId(clientId);
  cmdGetImage->SetDeviceName(deviceName.c_str());
  cmdGetImage->SetNameToGetImageMeta();
  cmdGetImage->SetImageId(deviceName.c_str());
  {
    // Add command to the execution queue
    igsioLockGuard<vtkIGSIORecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);
    this->CommandQueue.push_back(cmdGetImage);
  }
  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
PlusStatus vtkPlusCommandProcessor::QueueGetImage(unsigned int clientId, const std::string& deviceName)
{
  vtkSmartPointer<vtkPlusGetImageCommand> cmdGetImage = vtkSmartPointer<vtkPlusGetImageCommand>::New();
  cmdGetImage->SetCommandProcessor(this);
  cmdGetImage->SetClientId(clientId);
  cmdGetImage->SetDeviceName(deviceName.c_str());
  cmdGetImage->SetNameToGetImage();
  cmdGetImage->SetImageId(deviceName.c_str());
  {
    // Add command to the execution queue
    igsioLockGuard<vtkIGSIORecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);
    this->CommandQueue.push_back(cmdGetImage);
  }
  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
void vtkPlusCommandProcessor::PopCommandResponses(PlusCommandResponseList& responses)
{
  igsioLockGuard<vtkIGSIORecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);
  // Add reply to the sending queue
  // Append this->CommandResponses to 'responses'.
  // Elements appended to 'responses' are removed from this->CommandResponses.
  responses.splice(responses.end(), this->CommandResponseQueue, this->CommandResponseQueue.begin(), this->CommandResponseQueue.end());
}

//------------------------------------------------------------------------------
bool vtkPlusCommandProcessor::IsRunning()
{
  return this->CommandExecutionActive.second;
}

