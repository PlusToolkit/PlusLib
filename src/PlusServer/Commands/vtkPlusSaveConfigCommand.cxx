/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusDataCollector.h"
#include "vtkPlusCommandProcessor.h"
#include "vtkPlusCommandResponse.h"
#include "vtkPlusSaveConfigCommand.h"
#include "vtkIGSIOTransformRepository.h"
#include "vtkPlusVirtualCapture.h"

vtkStandardNewMacro(vtkPlusSaveConfigCommand);

namespace
{
  static const std::string SAVE_CONFIG_CMD = "SaveConfig";
}

//----------------------------------------------------------------------------
vtkPlusSaveConfigCommand::vtkPlusSaveConfigCommand()
{
}

//----------------------------------------------------------------------------
vtkPlusSaveConfigCommand::~vtkPlusSaveConfigCommand()
{

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
std::string vtkPlusSaveConfigCommand::GetDescription(const std::string& commandName)
{
  std::string desc;
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, SAVE_CONFIG_CMD))
  {
    desc += SAVE_CONFIG_CMD;
    desc += ": Tell the data collector to save the config file.";
  }
  return desc;
}

//----------------------------------------------------------------------------
void vtkPlusSaveConfigCommand::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Filename: " << this->Filename;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSaveConfigCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  this->SetFilename(aConfig->GetAttribute("Filename"));
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSaveConfigCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::WriteConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  // Start parameters
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(Filename, aConfig);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSaveConfigCommand::Execute()
{
  LOG_INFO("vtkPlusSaveConfigCommand::Execute");

  if (GetFilename().empty())
  {
    this->SetFilename(this->CommandProcessor->GetPlusServer()->GetConfigFilename());
  }

  std::string baseMessageString = std::string("SaveConfig (") + (!this->Filename.empty() ? this->Filename : "undefined") + ")";

  if (this->GetDataCollector() == NULL)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", baseMessageString + " Can't access data collector.");
    return PLUS_FAIL;
  }
  if (this->GetDataCollector()->WriteConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS
      || this->GetTransformRepository()->WriteConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", baseMessageString + " Unable to write configuration.");
    return PLUS_FAIL;
  }

  igsioCommon::XML::PrintXML(this->GetFilename(), vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData());
  this->QueueCommandResponse(PLUS_SUCCESS, baseMessageString + " Completed successfully.");
  return PLUS_SUCCESS;
}
