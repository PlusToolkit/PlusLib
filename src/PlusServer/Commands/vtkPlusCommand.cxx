/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "igtl_header.h"
#include "vtkImageData.h"
#include "vtkMatrix4x4.h"
#include "vtkPlusCommand.h"
#include "vtkPlusCommandProcessor.h"
#include "vtkVersion.h"

const std::string vtkPlusCommand::DEVICE_NAME_COMMAND = "CMD";
const std::string vtkPlusCommand::DEVICE_NAME_REPLY = "ACK";

//----------------------------------------------------------------------------
vtkPlusCommand::vtkPlusCommand()
  : CommandProcessor(NULL)
  , ClientId(0)
  , Id(0)
  , RespondWithCommandMessage(true)
{
}

//----------------------------------------------------------------------------
vtkPlusCommand::~vtkPlusCommand()
{
}

//----------------------------------------------------------------------------
void vtkPlusCommand::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  if (aConfig == NULL)
  {
    LOG_ERROR("vtkPlusCommand::ReadConfiguration failed, input is NULL");
    return PLUS_FAIL;
  }
  SetName(aConfig->GetAttribute("Name"));
  if (ValidateName() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{
  if (aConfig == NULL)
  {
    LOG_ERROR("vtkPlusCommand::WriteConfiguration failed, input is NULL");
    return PLUS_FAIL;
  }
  aConfig->SetName("Command");
  if (!this->Name.empty())
  {
    aConfig->SetAttribute("Name", this->Name.c_str());
  }
  else
  {
    // command name not set, so set the first command name as a default
    std::list<std::string> cmdNames;
    GetCommandNames(cmdNames);
    if (!cmdNames.empty())
    {
      aConfig->SetAttribute("Name", cmdNames.front().c_str());
    }
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusCommand::SetCommandProcessor(vtkPlusCommandProcessor* processor)
{
  this->CommandProcessor = processor;
}

//----------------------------------------------------------------------------
void vtkPlusCommand::SetClientId(int clientId)
{
  this->ClientId = clientId;
}

//----------------------------------------------------------------------------
int vtkPlusCommand::GetClientId() const
{
  return this->ClientId;
}

//----------------------------------------------------------------------------
bool vtkPlusCommand::GetRespondWithCommandMessage() const
{
  return this->RespondWithCommandMessage;
}

//----------------------------------------------------------------------------
std::string vtkPlusCommand::GetName() const
{
  return this->Name;
}

//----------------------------------------------------------------------------
void vtkPlusCommand::SetName(const std::string& name)
{
  this->Name = name;
}

//----------------------------------------------------------------------------
std::string vtkPlusCommand::GetDeviceName() const
{
  return this->DeviceName;
}

//----------------------------------------------------------------------------
void vtkPlusCommand::SetDeviceName(const std::string& deviceName)
{
  this->DeviceName = deviceName;
}

//----------------------------------------------------------------------------
uint32_t vtkPlusCommand::GetId() const
{
  return this->Id;
}

//----------------------------------------------------------------------------
vtkPlusDataCollector* vtkPlusCommand::GetDataCollector()
{
  if (this->CommandProcessor == NULL)
  {
    LOG_ERROR("CommandProcessor is invalid");
    return NULL;
  }

  vtkPlusOpenIGTLinkServer* server = this->CommandProcessor->GetPlusServer();
  if (server == NULL)
  {
    LOG_ERROR("CommandProcessor::PlusServer is invalid");
    return NULL;
  }

  vtkPlusDataCollector* dataCollector = server->GetDataCollector();
  if (dataCollector == NULL)
  {
    LOG_ERROR("CommandProcessor::PlusServer::DataCollector is invalid");
    return NULL;
  }
  return dataCollector;
}

//----------------------------------------------------------------------------
vtkPlusTransformRepository* vtkPlusCommand::GetTransformRepository()
{
  if (this->CommandProcessor == NULL)
  {
    LOG_ERROR("CommandProcessor is invalid");
    return NULL;
  }

  vtkPlusOpenIGTLinkServer* server = this->CommandProcessor->GetPlusServer();
  if (server == NULL)
  {
    LOG_ERROR("CommandProcessor::PlusServer is invalid");
    return NULL;
  }

  vtkPlusTransformRepository* aRepository = server->GetTransformRepository();
  if (aRepository == NULL)
  {
    LOG_ERROR("CommandProcessor::PlusServer::TransformRepository is invalid");
    return NULL;
  }
  return aRepository;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCommand::ValidateName()
{
  if (this->Name.empty())
  {
    LOG_ERROR("Command name is not specified");
    return PLUS_FAIL;
  }
  std::list<std::string> cmdNames;
  GetCommandNames(cmdNames);
  for (std::list<std::string>::iterator it = cmdNames.begin(); it != cmdNames.end(); ++it)
  {
    if (PlusCommon::IsEqualInsensitive(*it, this->Name))
    {
      // command found
      return PLUS_SUCCESS;
    }
  }
  LOG_ERROR("Command name " << this->Name << " is not recognized");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
std::string vtkPlusCommand::GenerateReplyDeviceName(uint32_t Id)
{
  std::ostringstream ss;
  ss << DEVICE_NAME_REPLY << "_" << Id;
  return ss.str();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCommand::GenerateCommandDeviceName(const std::string& uid, std::string& outDeviceName)
{
  if (uid.empty())
  {
    return PLUS_FAIL;
  }

  outDeviceName = std::string(DEVICE_NAME_COMMAND) + "_" + uid;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
bool vtkPlusCommand::IsCommandDeviceName(const std::string& deviceName)
{
  std::string prefix = GetPrefixFromCommandDeviceName(deviceName);
  if (prefix.compare(DEVICE_NAME_COMMAND) != 0)
  {
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------
bool vtkPlusCommand::IsReplyDeviceName(const std::string& deviceName, const std::string& uid)
{
  std::string prefix = GetPrefixFromCommandDeviceName(deviceName);
  if (prefix.compare(DEVICE_NAME_REPLY) != 0)
  {
    // not ACK_...
    return false;
  }
  if (uid.empty())
  {
    // ACK is received and no uid check is needed
    return true;
  }
  std::string uidInDeviceName = GetUidFromCommandDeviceName(deviceName);
  if (uidInDeviceName.compare(uid) != 0)
  {
    // uid mismatch
    return false;
  }
  // this is an ACK_... message and the uid matches
  return true;
}

//----------------------------------------------------------------------------
std::string vtkPlusCommand::GetUidFromCommandDeviceName(const std::string& deviceName)
{
  std::string uid("");
  std::size_t separatorPos = deviceName.find("_");
  if (separatorPos != std::string::npos)
  {
    uid = deviceName.substr(separatorPos + 1);
  }
  return uid;
}

//----------------------------------------------------------------------------
std::string vtkPlusCommand::GetPrefixFromCommandDeviceName(const std::string& deviceName)
{
  std::string prefix(deviceName);
  std::size_t separatorPos = deviceName.find("_");
  if (separatorPos != std::string::npos)
  {
    prefix = deviceName.substr(0, separatorPos);
  }
  return prefix;
}

//------------------------------------------------------------------------------
void vtkPlusCommand::PopCommandResponses(PlusCommandResponseList& responses)
{
  // Append this->CommandResponses to 'responses'.
  // Elements appended to 'responses' are removed from this->CommandResponseQueue.
  responses.splice(responses.end(), this->CommandResponseQueue, this->CommandResponseQueue.begin(), this->CommandResponseQueue.end());
}

//------------------------------------------------------------------------------
void vtkPlusCommand::QueueCommandResponse(PlusStatus status, const std::string& message, const std::string& error, const std::map<std::string, std::string>* values)
{
  vtkSmartPointer<vtkPlusCommandCommandResponse> commandResponse = vtkSmartPointer<vtkPlusCommandCommandResponse>::New();
  commandResponse->SetClientId(this->ClientId);
  commandResponse->SetOriginalId(this->Id);
  commandResponse->SetDeviceName(this->DeviceName);
  commandResponse->SetCommandName(this->GetName());
  commandResponse->SetStatus(status);
  commandResponse->SetRespondWithCommandMessage(this->RespondWithCommandMessage);
  commandResponse->SetErrorString(error);
  commandResponse->SetResultString(message);
  if (values != NULL)
  {
    commandResponse->SetParameters(*values);
  }
  this->CommandResponseQueue.push_back(commandResponse);
}