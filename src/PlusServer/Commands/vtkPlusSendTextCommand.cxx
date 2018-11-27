/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusSendTextCommand.h"

#include "vtkPlusDataCollector.h"

vtkStandardNewMacro(vtkPlusSendTextCommand);

namespace
{
  static const std::string SEND_TEXT_CMD = "SendText";
}

//----------------------------------------------------------------------------
vtkPlusSendTextCommand::vtkPlusSendTextCommand()
  : ResponseExpected(true)
{
  // It handles only one command, set its name by default
  this->SetName(SEND_TEXT_CMD);
}

//----------------------------------------------------------------------------
vtkPlusSendTextCommand::~vtkPlusSendTextCommand()
{
}

//----------------------------------------------------------------------------
void vtkPlusSendTextCommand::SetNameToSendText()
{
  this->SetName(SEND_TEXT_CMD);
}

//----------------------------------------------------------------------------
void vtkPlusSendTextCommand::GetCommandNames(std::list<std::string>& cmdNames)
{
  cmdNames.clear();
  cmdNames.push_back(SEND_TEXT_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusSendTextCommand::GetDescription(const std::string& commandName)
{
  std::string desc;
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, SEND_TEXT_CMD))
  {
    desc += SEND_TEXT_CMD;
    desc += ": Send text data to the device.";
  }
  return desc;
}

//----------------------------------------------------------------------------
std::string vtkPlusSendTextCommand::GetDeviceId() const
{
  return this->DeviceId;
}

//----------------------------------------------------------------------------
void vtkPlusSendTextCommand::SetDeviceId(const std::string& deviceId)
{
  this->DeviceId = deviceId;
}

//----------------------------------------------------------------------------
std::string vtkPlusSendTextCommand::GetText() const
{
  return this->Text;
}

//----------------------------------------------------------------------------
void vtkPlusSendTextCommand::SetText(const std::string& text)
{
  this->Text = text;
}

//----------------------------------------------------------------------------
std::string vtkPlusSendTextCommand::GetResponseText() const
{
  return this->ResponseText;
}

//----------------------------------------------------------------------------
void vtkPlusSendTextCommand::SetResponseText(const std::string& responseText)
{
  this->ResponseText = responseText;
}

//----------------------------------------------------------------------------
void vtkPlusSendTextCommand::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Text: " << this->Text;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSendTextCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(DeviceId, aConfig);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(Text, aConfig);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSendTextCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::WriteConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(DeviceId, aConfig);
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(Text, aConfig);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSendTextCommand::Execute()
{
  LOG_DEBUG("vtkPlusSendTextCommand::Execute: " << (!this->Name.empty() ? this->Name : "(undefined)")
            << ", device: " << (this->DeviceId.empty() ? "(undefined)" : this->DeviceId)
            << ", text: " << (this->Text.empty() ? "(undefined)" : this->Text));

  vtkPlusDataCollector* dataCollector = GetDataCollector();
  if (dataCollector == NULL)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", "Invalid data collector.");
    return PLUS_FAIL;
  }

  // Get device pointer
  if (this->DeviceId.empty())
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", "No DeviceId specified.");
    return PLUS_FAIL;
  }
  vtkPlusDevice* device = NULL;
  if (dataCollector->GetDevice(device, this->DeviceId) != PLUS_SUCCESS)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", std::string("Device ")
                               + (this->DeviceId.empty() ? "(undefined)" : this->DeviceId) + std::string(" is not found."));
    return PLUS_FAIL;
  }

  // Send text (and receive response)
  std::string textToSend;
  std::string response;
  if (!this->GetText().empty())
  {
    textToSend = GetText();
  }
  PlusStatus status = device->SendText(GetText(), GetResponseExpected() ? &response : NULL);
  SetResponseText(response.c_str());
  if (status != PLUS_SUCCESS)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", std::string("Failed to send text '") + GetText() + "'"
                               + " to device " + (this->DeviceId.empty() ? "(undefined)" : this->DeviceId));
    return PLUS_FAIL;
  }
  this->QueueCommandResponse(PLUS_SUCCESS, response);
  return PLUS_SUCCESS;
}
