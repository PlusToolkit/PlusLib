/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "vtkPlusCommand.h"
#include "vtkPlusCommandProcessor.h"
#include "vtkVersion.h"

static const char* DEVICE_NAME_COMMAND = "CMD";
static const char* DEVICE_NAME_REPLY = "ACK";

//----------------------------------------------------------------------------
vtkPlusCommand::vtkPlusCommand()
: Completed(false)
, CommandProcessor(NULL)
, ClientId(0)
, Id(NULL)
, Name(NULL)
, DeviceName(NULL)
{
}

//----------------------------------------------------------------------------
vtkPlusCommand::~vtkPlusCommand()
{  
  this->SetName(NULL);
  this->SetDeviceName(NULL);
  this->SetId(NULL);
}

//----------------------------------------------------------------------------
void vtkPlusCommand::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
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
  if (aConfig==NULL)
  {
    LOG_ERROR("vtkPlusCommand::WriteConfiguration failed, input is NULL");
    return PLUS_FAIL;
  }
  aConfig->SetName("Command");
  if (this->Name!=NULL)
  {
    aConfig->SetAttribute("Name", this->Name);
  }
  else
  {
    // command name not set, so set the first command name as a default
    std::list<std::string> cmdNames;
    GetCommandNames(cmdNames);
    if (!cmdNames.empty())
    {
      aConfig->SetAttribute("Name",cmdNames.front().c_str());
    }
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusCommand::SetCommandProcessor( vtkPlusCommandProcessor *processor )
{  
  this->CommandProcessor=processor;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCommand::SetCommandCompleted(PlusStatus replyStatus, const std::string& replyString)
{
  if (this->CommandProcessor==NULL)
  {
    LOG_ERROR("vtkPlusCommand::SetCommandCompleted failed, command processor is invalid");
    return PLUS_FAIL;
  }
  else
  {
    this->CommandProcessor->QueueReply(this->ClientId, replyStatus, replyString, this->GetReplyDeviceName());
  }
  this->Completed=true;
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
bool vtkPlusCommand::IsCompleted()
{  
  return this->Completed;
}

//----------------------------------------------------------------------------
void vtkPlusCommand::SetClientId(int clientId)
{  
  this->ClientId=clientId;
}

//----------------------------------------------------------------------------
vtkDataCollector* vtkPlusCommand::GetDataCollector()
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

  vtkDataCollector* dataCollector = server->GetDataCollector();
  if (dataCollector == NULL)
  {
    LOG_ERROR("CommandProcessor::PlusServer::DataCollector is invalid");
    return NULL;
  }
  return dataCollector;
}

//----------------------------------------------------------------------------
vtkTransformRepository* vtkPlusCommand::GetTransformRepository()
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

  vtkTransformRepository* aRepository = server->GetTransformRepository();
  if( aRepository == NULL )
  {
    LOG_ERROR("CommandProcessor::PlusServer::TransformRepository is invalid");
    return NULL;
  }
  return aRepository;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCommand::ValidateName()
{  
  if (this->Name==NULL)
  {
    LOG_ERROR("Command name is not specified");
    return PLUS_FAIL;
  }
  std::list<std::string> cmdNames;
  GetCommandNames(cmdNames);
  for (std::list<std::string>::iterator it=cmdNames.begin(); it!=cmdNames.end(); ++it)
  {
    if (STRCASECMP(it->c_str(),this->Name)==0)
    {
      // command found
      return PLUS_SUCCESS;
    }
  }
  LOG_ERROR("Command name "<<this->Name<<" is not recognized");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
std::string vtkPlusCommand::GetReplyDeviceName()
{
  std::string uid;
  if( this->Id != NULL)
  {
    uid=this->Id;
  }
  return GenerateReplyDeviceName(uid);
}

//----------------------------------------------------------------------------
std::string vtkPlusCommand::GenerateCommandDeviceName(const std::string &uid)
{
  std::stringstream ss;
  ss << DEVICE_NAME_COMMAND;
  if( !uid.empty() )
  {
    ss << "_" << uid;
  }
  return ss.str();
}

//----------------------------------------------------------------------------
std::string vtkPlusCommand::GenerateReplyDeviceName(const std::string &uid)
{
  std::stringstream ss;
  ss << DEVICE_NAME_REPLY;
  if( !uid.empty() )
  {
    ss << "_" << uid;
  }
  return ss.str();
}

//----------------------------------------------------------------------------
bool vtkPlusCommand::IsReplyDeviceName(const std::string &deviceName, const std::string &uid)
{
  std::string prefix=GetPrefixFromCommandDeviceName(deviceName);
  if (prefix.compare(DEVICE_NAME_REPLY)!=0)
  {
    // not ACK_...
    return false;
  }
  if (uid.empty())
  {
    // ACK is received and no uid check is needed
    return true;
  }
  std::string uidInDeviceName=GetUidFromCommandDeviceName(deviceName);
  if (uidInDeviceName.compare(uid)!=0)
  {
    // uid mismatch
    return false;
  }
  // this is an ACK_... message and the uid matches
  return true;
}

//----------------------------------------------------------------------------
std::string vtkPlusCommand::GetUidFromCommandDeviceName(const std::string &deviceName)
{
  std::string uid("");
  std::size_t separatorPos=deviceName.find("_");
  if( separatorPos != std::string::npos )
  {
    uid = deviceName.substr(separatorPos+1);
  }
  return uid;
}

//----------------------------------------------------------------------------
std::string vtkPlusCommand::GetPrefixFromCommandDeviceName(const std::string &deviceName)
{
  std::string prefix(deviceName);
  std::size_t separatorPos=deviceName.find("_");
  if( separatorPos != std::string::npos )
  {
    prefix = deviceName.substr(0, separatorPos);
  }
  return prefix;
}
