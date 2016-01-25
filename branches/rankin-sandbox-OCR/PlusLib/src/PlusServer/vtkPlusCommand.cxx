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

const char* vtkPlusCommand::DEVICE_NAME_COMMAND = "CMD";
const char* vtkPlusCommand::DEVICE_NAME_REPLY = "ACK";

//----------------------------------------------------------------------------
vtkPlusCommand::vtkPlusCommand()
: CommandProcessor(NULL)
, ClientId(0)
, Id(0)
, Name(NULL)
, DeviceName(NULL)
{
}

//----------------------------------------------------------------------------
vtkPlusCommand::~vtkPlusCommand()
{  
  this->SetName(NULL);
  this->SetDeviceName(NULL);
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
std::string vtkPlusCommand::GenerateReplyDeviceName(uint32_t Id)
{
  std::ostringstream ss;
  ss << DEVICE_NAME_REPLY << Id;
  return ss.str();
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

//------------------------------------------------------------------------------
void vtkPlusCommand::PopCommandResponses(PlusCommandResponseList &responses)
{
  // Append this->CommandResponses to 'responses'.
  // Elements appended to 'responses' are removed from this->CommandResponseQueue.
  responses.splice(responses.end(),this->CommandResponseQueue,this->CommandResponseQueue.begin(),this->CommandResponseQueue.end());
}

//------------------------------------------------------------------------------
void vtkPlusCommand::QueueCommandResponse(const std::string& reply, PlusStatus status)
{
  vtkSmartPointer<vtkPlusCommandCommandResponse> commandResponse = vtkSmartPointer<vtkPlusCommandCommandResponse>::New();  
  commandResponse->SetClientId(this->ClientId);
  commandResponse->SetDeviceName(this->DeviceName);
  commandResponse->SetStatus(status);
  commandResponse->SetVersion(this->Version);
  commandResponse->SetErrorString(reply);
  this->CommandResponseQueue.push_back(commandResponse);
}