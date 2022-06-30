/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusGenericSerialCommand.h"
#include "vtkPlusGenericSerialDevice.h"

#include "vtkPlusDataCollector.h"

vtkStandardNewMacro(vtkPlusGenericSerialCommand);

namespace
{
  static const std::string SERIAL_CMD_NAME = "SerialCommand";
}

//----------------------------------------------------------------------------
vtkPlusGenericSerialCommand::vtkPlusGenericSerialCommand()
  : ResponseExpected(true)
{
  // It handles only one command, set its name by default
  this->SetName(SERIAL_CMD_NAME);
}

//----------------------------------------------------------------------------
vtkPlusGenericSerialCommand::~vtkPlusGenericSerialCommand()
{
}

//----------------------------------------------------------------------------
void vtkPlusGenericSerialCommand::SetNameToSerial()
{
  this->SetName(SERIAL_CMD_NAME);
}

//----------------------------------------------------------------------------
void vtkPlusGenericSerialCommand::GetCommandNames(std::list<std::string>& cmdNames)
{
  cmdNames.clear();
  cmdNames.push_back(SERIAL_CMD_NAME);
}

//----------------------------------------------------------------------------
std::string vtkPlusGenericSerialCommand::GetDescription(const std::string& commandName)
{
  std::string desc;
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, SERIAL_CMD_NAME))
  {
    desc += SERIAL_CMD_NAME;
    desc += ": Send text data to the device.";
  }
  return desc;
}

//----------------------------------------------------------------------------
std::string vtkPlusGenericSerialCommand::GetDeviceId() const
{
  return this->DeviceId;
}

//----------------------------------------------------------------------------
void vtkPlusGenericSerialCommand::SetDeviceId(const std::string& deviceId)
{
  this->DeviceId = deviceId;
}

//----------------------------------------------------------------------------
std::string vtkPlusGenericSerialCommand::GetCommandName() const
{
  return this->CommandName;
}

//----------------------------------------------------------------------------
void vtkPlusGenericSerialCommand::SetCommandName(const std::string& text)
{
  this->CommandName = text;
}

//----------------------------------------------------------------------------
std::string vtkPlusGenericSerialCommand::GetCommandValue() const
{
  return this->CommandValue;
}

//----------------------------------------------------------------------------
void vtkPlusGenericSerialCommand::SetCommandValue(const std::string& text)
{
  this->CommandValue = text;
}

//----------------------------------------------------------------------------
void vtkPlusGenericSerialCommand::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGenericSerialCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  XML_READ_STRING_ATTRIBUTE_OPTIONAL(DeviceId, aConfig);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(CommandName, aConfig);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(CommandValue, aConfig);
  return PLUS_SUCCESS;

}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGenericSerialCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::WriteConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(DeviceId, aConfig);
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(CommandName, aConfig);
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(CommandValue, aConfig);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGenericSerialCommand::Execute()
{
  LOG_DEBUG("vtkPlusGenericSerialCommand::Execute: " << (!this->CommandName.empty() ? this->CommandName : "(undefined)")
            << ", device: " << (this->DeviceId.empty() ? "(undefined)" : this->DeviceId)
            << ", value: " << (this->CommandValue.empty() ? "(undefined)" : this->CommandValue));

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
  vtkPlusDevice* aDevice = NULL;
  if (dataCollector->GetDevice(aDevice, this->DeviceId) != PLUS_SUCCESS)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", std::string("Device ")
                               + (this->DeviceId.empty() ? "(undefined)" : this->DeviceId) + std::string(" is not found."));
    return PLUS_FAIL;
  }
  vtkPlusGenericSerialDevice* device = dynamic_cast<vtkPlusGenericSerialDevice*>(aDevice);  

  // Send text (and receive response)
  std::string textToSend;
  std::string response;
  if (!this->GetCommandValue().empty())
  {
    textToSend = GetCommandValue();
  }
  PlusStatus status;
  if (igsioCommon::IsEqualInsensitive(this->GetCommandName(), vtkPlusGenericSerialDevice::SERIAL_COMMAND_GET_RTS))
  {
    response = device->GetRTS() ? "True" : "False";
    status = PLUS_SUCCESS;
  }
  else if (igsioCommon::IsEqualInsensitive(this->GetCommandName(), vtkPlusGenericSerialDevice::SERIAL_COMMAND_SET_RTS))
  {
    bool onOff = textToSend == "True" ? true : false;
    status = device->SetRTS(onOff);
  }
  else if (igsioCommon::IsEqualInsensitive(this->GetCommandName(), vtkPlusGenericSerialDevice::SERIAL_COMMAND_GET_CTS))
  {
    bool onOff;
    status = device->GetCTS(onOff);
    response = onOff ? "True" : "False";
  }
  else
  {
    status = PLUS_FAIL;
  }
  if (status != PLUS_SUCCESS)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", std::string("Failed to execute command '") + GetCommandName() + "'"
                               + " on device " + (this->DeviceId.empty() ? "(undefined)" : this->DeviceId));
    return PLUS_FAIL;
  }
  this->QueueCommandResponse(PLUS_SUCCESS, response);
  return PLUS_SUCCESS;
}
