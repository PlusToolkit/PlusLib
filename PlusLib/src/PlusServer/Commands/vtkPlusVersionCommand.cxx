/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusCommandProcessor.h"
#include "vtkPlusOpenIGTLinkServer.h"
#include "vtkPlusVersionCommand.h"

vtkStandardNewMacro( vtkPlusVersionCommand );

static const char VERSION_CMD[] = "Version";

//----------------------------------------------------------------------------
vtkPlusVersionCommand::vtkPlusVersionCommand()
  : vtkPlusCommand()
{
  // It handles only one command, set its name by default
  this->SetName(VERSION_CMD);
}

//----------------------------------------------------------------------------
vtkPlusVersionCommand::~vtkPlusVersionCommand()
{

}

//----------------------------------------------------------------------------
void vtkPlusVersionCommand::SetNameToVersion()
{
  this->SetName(VERSION_CMD);
}

//----------------------------------------------------------------------------
void vtkPlusVersionCommand::GetCommandNames(std::list<std::string>& cmdNames)
{
  cmdNames.clear();
  cmdNames.push_back(VERSION_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusVersionCommand::GetDescription(const char* commandName)
{
  std::string desc;
  if (commandName == NULL || STRCASECMP(commandName, VERSION_CMD))
  {
    desc += VERSION_CMD;
    desc += ": Send and receive version to/from the device.";
  }
  return desc;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVersionCommand::Execute()
{
  std::map<std::string, std::string> metadata;
  metadata["Version"] = std::to_string(this->CommandProcessor->GetPlusServer()->GetIGTLProtocolVersion());
  this->QueueCommandResponse(PLUS_SUCCESS, "Success.", "", &metadata);
  return PLUS_SUCCESS;
}
