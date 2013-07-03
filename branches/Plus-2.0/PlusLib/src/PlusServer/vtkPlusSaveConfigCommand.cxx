/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "vtkDataCollector.h"
#include "vtkPlusCommandProcessor.h"
#include "vtkPlusSaveConfigCommand.h"
#include "vtkTransformRepository.h"
#include "vtkVirtualDiscCapture.h"

vtkStandardNewMacro( vtkPlusSaveConfigCommand );

static const char SAVE_CONFIG_CMD[] = "SaveConfig";

//----------------------------------------------------------------------------
vtkPlusSaveConfigCommand::vtkPlusSaveConfigCommand()
: Filename(NULL)
{
}

//----------------------------------------------------------------------------
vtkPlusSaveConfigCommand::~vtkPlusSaveConfigCommand()
{
  this->SetFilename(NULL);
}

//----------------------------------------------------------------------------
void vtkPlusSaveConfigCommand::SetNameToSaveConfig()
{ 
  this->SetName(SAVE_CONFIG_CMD);
}

//----------------------------------------------------------------------------
void vtkPlusSaveConfigCommand::GetCommandNames(std::list<std::string>& cmdNames)
{ 
  cmdNames.clear(); 
  cmdNames.push_back(SAVE_CONFIG_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusSaveConfigCommand::GetDescription(const char* commandName)
{ 
  std::string desc;
  if (commandName == NULL || STRCASECMP(commandName, SAVE_CONFIG_CMD))
  {
    desc += SAVE_CONFIG_CMD;
    desc += ": Tell the data collector to save the config file.";
  }
  return desc;
}

//----------------------------------------------------------------------------
void vtkPlusSaveConfigCommand::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
  os << indent << "Filename: " << this->Filename;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSaveConfigCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{  
  if (vtkPlusCommand::ReadConfiguration(aConfig)!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  if( aConfig->GetAttribute("Filename") != NULL )
  {
    this->SetFilename(aConfig->GetAttribute("Filename"));
  }
  else
  {
    this->SetFilename(this->CommandProcessor->GetPlusServer()->GetConfigFilename());
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSaveConfigCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{  
  if (vtkPlusCommand::WriteConfiguration(aConfig)!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }  

  // Common parameters
  aConfig->SetAttribute("Name",this->Name);

  // Start parameters
  if( this->GetFilename() != NULL )
  {
    aConfig->SetAttribute("Filename", this->GetFilename());
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSaveConfigCommand::Execute()
{
  if (this->Name == NULL)
  {
    LOG_ERROR("Command failed, no command name specified");
    this->SetCommandCompleted(PLUS_FAIL, "Command failed, no command name specified");
    return PLUS_FAIL;
  }

  PlusStatus status = PLUS_SUCCESS;
  std::string reply = std::string("SaveConfig (") + this->Filename + 
    ") " + this->Name + " ";  

  LOG_INFO("vtkPlusSaveConfigCommand::Execute: " << this->Name);

  if (STRCASECMP(this->Name, SAVE_CONFIG_CMD) == 0)
  {    
    if( this->GetDataCollector() != NULL)
    {
      if( this->GetDataCollector()->WriteConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS
        || this->GetTransformRepository()->WriteConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS )
      {
        reply += "unable to write configuration, ";
        status = PLUS_FAIL;
      }
      else
      {
        PlusCommon::PrintXML(this->GetFilename(), vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData());
      }
    }
    else
    {
      reply += "can't access data collector, ";
      status = PLUS_FAIL;
    }
  }
  else
  {
    reply += "unknown command, ";
    status = PLUS_FAIL;
  }

  if (status == PLUS_SUCCESS)
  {
    reply += "completed successfully";
  }
  else
  {
    reply += "failed";
  }
  this->SetCommandCompleted(status, reply);
  return status;
}
