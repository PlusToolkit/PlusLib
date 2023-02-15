/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusUSDigitalEncoderCommand.h"
#include "vtkPlusUSDigitalEncodersTracker.h"

#include "vtkPlusDataCollector.h"

vtkStandardNewMacro(vtkPlusUSDigitalEncoderCommand);

namespace
{
  static const std::string ENCODER_CMD = "USDigitalEncoder";
}

//----------------------------------------------------------------------------
vtkPlusUSDigitalEncoderCommand::vtkPlusUSDigitalEncoderCommand()
  : ResponseExpected(true)
{
  this->SetName(ENCODER_CMD);
}

//----------------------------------------------------------------------------
vtkPlusUSDigitalEncoderCommand::~vtkPlusUSDigitalEncoderCommand()
{
}

//----------------------------------------------------------------------------
void vtkPlusUSDigitalEncoderCommand::SetNameToEncoderCommand()
{
  this->SetName(ENCODER_CMD);
}

//----------------------------------------------------------------------------
void vtkPlusUSDigitalEncoderCommand::GetCommandNames(std::list<std::string>& cmdNames)
{
  cmdNames.clear();
  cmdNames.push_back(ENCODER_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusUSDigitalEncoderCommand::GetDescription(const std::string& commandName)
{
  std::string desc;
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, ENCODER_CMD))
  {
    desc += ENCODER_CMD;
    desc += ": Send commands to a USDigital Encoder device.";
  }
  return desc;
}

//----------------------------------------------------------------------------
std::string vtkPlusUSDigitalEncoderCommand::GetDeviceId() const
{
  return this->DeviceId;
}

//----------------------------------------------------------------------------
void vtkPlusUSDigitalEncoderCommand::SetDeviceId(const std::string& deviceId)
{
  this->DeviceId = deviceId;
}

//----------------------------------------------------------------------------
void vtkPlusUSDigitalEncoderCommand::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Device Id: " << this->DeviceId;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncoderCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  this->CommandList.clear();
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(DeviceId, aConfig);

  // Parse nested elements and store requested parameter changes
  for (int elemIndex = 0; elemIndex < aConfig->GetNumberOfNestedElements(); ++elemIndex)
  {
    vtkXMLDataElement* currentElem = aConfig->GetNestedElement(elemIndex);
    if (igsioCommon::IsEqualInsensitive(currentElem->GetName(), vtkPlusUSDigitalEncodersTracker::USDIGITALENCODER_COMMAND_SET_ORIGIN))
    {
      const char* encoderId = currentElem->GetAttribute("EncoderId");
      if (!encoderId)
      {
        LOG_ERROR("Unable to find required EncoderId in " << (currentElem->GetName() ? currentElem->GetName() : "(undefined)") << " element in USDigitalEncoder command");
        continue;
      }
      this->CommandList.push_back(std::make_pair(vtkPlusUSDigitalEncodersTracker::USDIGITALENCODER_COMMAND_SET_ORIGIN, encoderId));
    }
    else if (igsioCommon::IsEqualInsensitive(currentElem->GetName(), vtkPlusUSDigitalEncodersTracker::USDIGITALENCODER_COMMAND_SET_ALL_ORIGIN))
    {
      this->CommandList.push_back(std::make_pair(vtkPlusUSDigitalEncodersTracker::USDIGITALENCODER_COMMAND_SET_ALL_ORIGIN, ""));
    }
    else if (igsioCommon::IsEqualInsensitive(currentElem->GetName(), vtkPlusUSDigitalEncodersTracker::USDIGITALENCODER_COMMAND_GET_POSITION))
    {
      const char* encoderId = currentElem->GetAttribute("EncoderId");
      if (!encoderId)
      {
        LOG_ERROR("Unable to find required EncoderId in " << (currentElem->GetName() ? currentElem->GetName() : "(undefined)") << " element in USDigitalEncoder command");
        continue;
      }
      this->CommandList.push_back(std::make_pair(vtkPlusUSDigitalEncodersTracker::USDIGITALENCODER_COMMAND_GET_POSITION, encoderId));
    }
    else {
      LOG_ERROR("Invalid command name (" << currentElem->GetName() << ") provided to USDigitalEncoderCommand.");
    }
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncoderCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::WriteConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(DeviceId, aConfig);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncoderCommand::Execute()
{
  LOG_DEBUG("vtkPlusUSDigitalEncoderCommand::Execute: " << (!this->Name.empty() ? this->Name : "(undefined)")
            << ", device: " << (this->DeviceId.empty() ? "(undefined)" : this->DeviceId));

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

  vtkPlusUSDigitalEncodersTracker* device = dynamic_cast<vtkPlusUSDigitalEncodersTracker*>(aDevice);
  std::string resultString = "<CommandReply>";
  std::string error = "";
  std::map < std::string, std::pair<IANA_ENCODING_TYPE, std::string> > metaData;
  PlusStatus status = PLUS_SUCCESS;

  std::list<std::pair<std::string, std::string>>::iterator commandIt;
  for (commandIt = this->CommandList.begin(); commandIt != this->CommandList.end(); ++commandIt)
  {
    std::string commandName = commandIt->first;
    std::string encoderId = commandIt->second;

    resultString += "<Command Name=\"" + commandName + "\" ";

    if (commandName == vtkPlusUSDigitalEncodersTracker::USDIGITALENCODER_COMMAND_SET_ORIGIN)
    {
      int encoderId_int = std::stoi(encoderId);
      if (device->SetUSDigitalA2EncoderOriginWithID(encoderId_int) == PLUS_SUCCESS)
      {
        resultString += "Success=\"true\"/>";
      }
      else
      {
        resultString += "Success=\"false\"/>";
        status = PLUS_FAIL;
      }
    }
    else if (commandName == vtkPlusUSDigitalEncodersTracker::USDIGITALENCODER_COMMAND_SET_ALL_ORIGIN)
    {
      if (device->SetAllUSDigitalA2EncoderOrigin() == PLUS_SUCCESS)
      {
        resultString += "Success=\"true\"/>";
      }
      else
      {
        resultString += "Success=\"false\"/>";
        status = PLUS_FAIL;
      }
    }
    else if (commandName == vtkPlusUSDigitalEncodersTracker::USDIGITALENCODER_COMMAND_GET_POSITION)
    {
      int encoderId_int = std::stoi(encoderId);
      long encoderPos;
      if (device->GetUSDigitalA2EncoderPositionWithID(encoderId_int, &encoderPos) == PLUS_SUCCESS)
      {
        std::stringstream ss;
        ss << encoderPos;
        resultString += "Success=\"true\" EncoderId=\"" + encoderId + "\" Position=\"" + ss.str() + "\"/>";
      }
      else
      {
        resultString += "Success=\"false\" EncoderId=\"" + encoderId + "\"/>";
        status = PLUS_FAIL;
      }
    }
  }
  resultString += "</CommandReply>";

  if (status != PLUS_SUCCESS)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", std::string("Failed to execute command '")
                               + " for device " + (this->DeviceId.empty() ? "(undefined)" : this->DeviceId));
    return PLUS_FAIL;
  }

  vtkSmartPointer<vtkPlusCommandRTSCommandResponse> commandResponse = vtkSmartPointer<vtkPlusCommandRTSCommandResponse>::New();
  commandResponse->UseDefaultFormatOff();
  commandResponse->SetClientId(this->ClientId);
  commandResponse->SetOriginalId(this->Id);
  commandResponse->SetDeviceName(this->DeviceName);
  commandResponse->SetCommandName(this->GetName());
  commandResponse->SetStatus(status);
  commandResponse->SetRespondWithCommandMessage(this->RespondWithCommandMessage);
  commandResponse->SetErrorString(error);
  commandResponse->SetResultString(resultString);
  commandResponse->SetParameters(metaData);
  this->CommandResponseQueue.push_back(commandResponse);
  return PLUS_SUCCESS;
}
