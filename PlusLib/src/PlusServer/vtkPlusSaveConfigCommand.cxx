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
  this->SetFilename(aConfig->GetAttribute("Filename"));
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSaveConfigCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{  
  if (vtkPlusCommand::WriteConfiguration(aConfig)!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }  
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
  LOG_INFO("vtkPlusSaveConfigCommand::Execute");

  this->ResetResponse();
  if (GetFilename()==NULL)
  {
    this->SetFilename(this->CommandProcessor->GetPlusServer()->GetConfigFilename());
  }
  this->ResponseMessage = std::string("SaveConfig (") + (this->Filename?this->Filename:"undefined") + ")";

  if( this->GetDataCollector() == NULL)
  {
    this->ResponseMessage += " can't access data collector, ";
    return PLUS_FAIL;
  }
  if( this->GetDataCollector()->WriteConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS
    || this->GetTransformRepository()->WriteConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS )
  {
    this->ResponseMessage += " unable to write configuration";
    return PLUS_FAIL;
  }
  PlusCommon::PrintXML(this->GetFilename(), vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData());
  this->ResponseMessage += " completed successfully";
  return PLUS_SUCCESS;
}
