/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusCommandProcessor.h"
#include "vtkPlusOpenIGTLinkServer.h"
#include "vtkPlusVersionCommand.h"

vtkStandardNewMacro(vtkPlusVersionCommand);

namespace
{
  static const std::string VERSION_CMD = "Version";
}

//----------------------------------------------------------------------------
vtkPlusVersionCommand::vtkPlusVersionCommand()
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
std::string vtkPlusVersionCommand::GetDescription(const std::string& commandName)
{
  std::string desc;
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, VERSION_CMD))
  {
    desc += VERSION_CMD;
    desc += ": Send and receive version to/from the device.";
  }
  return desc;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVersionCommand::Execute()
{
  igtl::MessageBase::MetaDataMap metadata;
  metadata["PlusVersion"] = std::pair<IANA_ENCODING_TYPE, std::string>(IANA_TYPE_US_ASCII, PlusCommon::GetPlusLibVersionString());
#if defined(_MSC_VER) && _MSC_VER < 1700
  metadata["IGTLProtocolVersion"] = std::pair<IANA_ENCODING_TYPE, std::string>(IANA_TYPE_US_ASCII, igsioCommon::ToString<int>(this->CommandProcessor->GetPlusServer()->GetIGTLProtocolVersion()));
  metadata["IGTLHeaderVersion"] = std::pair<IANA_ENCODING_TYPE, std::string>(IANA_TYPE_US_ASCII, igsioCommon::ToString<int>(this->CommandProcessor->GetPlusServer()->GetIGTLHeaderVersion()));
#else
  metadata["IGTLProtocolVersion"] = std::pair<IANA_ENCODING_TYPE, std::string>(IANA_TYPE_US_ASCII, std::to_string(this->CommandProcessor->GetPlusServer()->GetIGTLProtocolVersion()));
  metadata["IGTLHeaderVersion"] = std::pair<IANA_ENCODING_TYPE, std::string>(IANA_TYPE_US_ASCII, std::to_string(this->CommandProcessor->GetPlusServer()->GetIGTLHeaderVersion()));
#endif
  this->QueueCommandResponse(PLUS_SUCCESS, "Success.", "", &metadata);
  return PLUS_SUCCESS;
}
