/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusDataCollector.h"
#include "vtkPlusCapistranoCommand.h"
#include "vtkPlusCapistranoVideoSource.h"
#include "vtkPlusUsImagingParameters.h"

#include "vtkImageData.h"
#include "vtkDICOMImageReader.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusCommandProcessor.h"
#include "vtkIGSIOTrackedFrameList.h"
#include "vtkPlusVolumeReconstructor.h"
#include "vtkPlusVirtualVolumeReconstructor.h"
#include <vtkImageFlip.h>
#include <vtkPointData.h>
#include <vtkDirectory.h>


vtkStandardNewMacro(vtkPlusCapistranoCommand);

namespace
{
  static const std::string CAPISTRANO_COMMAND = "CapistranoCommand";
}

//----------------------------------------------------------------------------
vtkPlusCapistranoCommand::vtkPlusCapistranoCommand()
  : ResponseExpected(true)
{
  // It handles only one command, set its name by default
  this->SetName(CAPISTRANO_COMMAND);
}

//----------------------------------------------------------------------------
vtkPlusCapistranoCommand::~vtkPlusCapistranoCommand()
{
}

//----------------------------------------------------------------------------
void vtkPlusCapistranoCommand::GetCommandNames(std::list<std::string>& cmdNames)
{
  cmdNames.clear();
  cmdNames.push_back(CAPISTRANO_COMMAND);
}

//----------------------------------------------------------------------------
std::string vtkPlusCapistranoCommand::GetDescription(const std::string& commandName)
{
  std::string desc;
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, CAPISTRANO_COMMAND))
  {
    desc += CAPISTRANO_COMMAND;
    desc += ": Send commands to Capistrano device.";
  }
  return desc;
}

//----------------------------------------------------------------------------
std::string vtkPlusCapistranoCommand::GetDeviceId() const
{
  return this->DeviceId;
}

//----------------------------------------------------------------------------
void vtkPlusCapistranoCommand::SetDeviceId(const std::string& deviceId)
{
  this->DeviceId = deviceId;
}

//----------------------------------------------------------------------------
std::string vtkPlusCapistranoCommand::GetCommandName() const
{
  return this->CommandName;
}

//----------------------------------------------------------------------------
void vtkPlusCapistranoCommand::SetCommandName(const std::string& text)
{
  this->CommandName = text;
}

//----------------------------------------------------------------------------
std::string vtkPlusCapistranoCommand::GetCommandValue() const
{
  return this->CommandValue;
}

//----------------------------------------------------------------------------
void vtkPlusCapistranoCommand::SetCommandValue(const std::string& text)
{
  this->CommandValue = text;
}

//----------------------------------------------------------------------------
void vtkPlusCapistranoCommand::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCapistranoCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  this->RequestedParameterChanges.clear();
  if (vtkPlusCommand::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  this->SetDeviceId(aConfig->GetAttribute("UsDeviceId"));

  // Parse nested elements and store requested parameter changes
  for (int elemIndex = 0; elemIndex < aConfig->GetNumberOfNestedElements(); ++elemIndex)
  {
    vtkXMLDataElement* currentElem = aConfig->GetNestedElement(elemIndex);
    if (igsioCommon::IsEqualInsensitive(currentElem->GetName(), "Parameter"))
    {
      const char* parameterName = currentElem->GetAttribute("Name");
      const char* parameterValue = currentElem->GetAttribute("Value");
      if (!parameterValue)
      {
        parameterValue = "None";
      }
      if (!parameterName)
      {
        LOG_ERROR("Unable to find required Name" << (currentElem->GetName() ? currentElem->GetName() : "(undefined)") << " element in CapistranoCommand command");
        continue;
      }
      this->RequestedParameterChanges.push_back(std::make_pair(parameterName, parameterValue));
    }
  }

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkPlusCapistranoCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
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
PlusStatus vtkPlusCapistranoCommand::Execute()
{
  LOG_DEBUG("vtkPlusSetUsParameterCommand::Execute: " << (!this->Name.empty() ? this->Name : "(undefined)")
            << ", device: " << (this->DeviceId.empty() ? "(undefined)" : this->DeviceId));

  if (this->Name.empty())
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", "No command name specified.");
    return PLUS_FAIL;
  }
  else if (!igsioCommon::IsEqualInsensitive(this->Name, CAPISTRANO_COMMAND))
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", "Unknown command name: " + this->Name + ".");
    return PLUS_FAIL;
  }
  vtkPlusDataCollector* dataCollector = GetDataCollector();
  vtkPlusDevice* aDevice = NULL;
  if (dataCollector->GetDevice(aDevice, this->DeviceId) != PLUS_SUCCESS)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", std::string("Device ")
                               + (this->DeviceId.empty() ? "(undefined)" : this->DeviceId) + std::string(" is not found."));
    return PLUS_FAIL;
  }
  vtkPlusCapistranoVideoSource* device = dynamic_cast<vtkPlusCapistranoVideoSource*>(aDevice);

  std::string DeviceId = (aDevice->GetDeviceId().empty() ? "(unknown)" : aDevice->GetDeviceId());
  std::string resultString = "<CommandReply>";
  std::string error = "";
  std::map < std::string, std::pair<IANA_ENCODING_TYPE, std::string> > metaData;
  PlusStatus status = PLUS_SUCCESS;

  std::list<std::pair<std::string, std::string>>::iterator paramIt;
  for (paramIt = this->RequestedParameterChanges.begin(); paramIt != this->RequestedParameterChanges.end(); ++paramIt)
  {
    std::string parameterName = paramIt->first;
    std::string value = paramIt->second;
    resultString += "<Parameter Name=\"" + parameterName + "\"";
    if (parameterName == vtkPlusCapistranoVideoSource::CAPISTRANO_SET_WOBBLE_RATE
        || parameterName == vtkPlusCapistranoVideoSource::CAPISTRANO_SET_MIS_MODE
        || parameterName == vtkPlusCapistranoVideoSource::CAPISTRANO_SET_MIS_PULSE_PERIOD
        || parameterName == vtkPlusCapistranoVideoSource::CAPISTRANO_SET_JITTER_COMPENSATION
        || parameterName == vtkPlusCapistranoVideoSource::CAPISTRANO_FREEZE_PROBE
        || parameterName == vtkPlusCapistranoVideoSource::CAPISTRANO_SET_BIDIRECTIONAL_MODE)
        {
          bool valid = false;
          double parameterValue = vtkVariant(value).ToDouble(&valid);
          if (!valid)
          {
            error += "Failed to parse " + parameterName + ". ";
            resultString += " Success=\"false\"";
            metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, "FAIL");
            status = PLUS_FAIL;
            continue;
          }
        }
    else if (parameterName == vtkPlusCapistranoVideoSource::CAPISTRANO_GET_WOBBLE_RATE
             || parameterName == vtkPlusCapistranoVideoSource::CAPISTRANO_GET_JITTER_COMPENSATION
             || parameterName == vtkPlusCapistranoVideoSource::CAPISTRANO_IS_PROBE_FROZEN)
        {
          if (value != "None")
          {
            LOG_WARNING("This command does not require a value");
          }
        }
    else
    {
      error += "Invalid parameter " + parameterName + ". ";
      resultString += " Success=\"false\"";
      metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, "FAIL");
      status = PLUS_FAIL;
      continue;
    } 
    if (parameterName == vtkPlusCapistranoVideoSource::CAPISTRANO_SET_WOBBLE_RATE)
    {
      int wobbleRate_int = std::stoi(value);
      status = device->SetWobbleRate((unsigned char)wobbleRate_int);
      resultString += " Success=\"true\"";
      metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, "SUCCESS");
    }
    else if (parameterName == vtkPlusCapistranoVideoSource::CAPISTRANO_GET_WOBBLE_RATE)
    {
      value = std::to_string(device->GetWobbleRate());
      std::stringstream ss;
      ss << value;
      resultString += " Success=\"true\"";
      resultString += " Value=\"" + ss.str() + "\"";
      metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, ss.str());
      status = PLUS_SUCCESS;
    }
    else if (parameterName == vtkPlusCapistranoVideoSource::CAPISTRANO_SET_BIDIRECTIONAL_MODE)
    {
      bool bidi = std::stoi(value);
      device->SetBidirectionalMode(bidi);
      resultString += " Success=\"true\"";
      metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, "SUCCESS");
    }
    else if (parameterName == vtkPlusCapistranoVideoSource::CAPISTRANO_FREEZE_PROBE)
    {
      bool freezeProbe_bool = std::stoi(value);
      if (device->FreezeDevice(freezeProbe_bool) == PLUS_SUCCESS)
      {
        resultString += " Success=\"true\"";
        metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, "SUCCESS");
      }
      else
      {
        LOG_ERROR("Failed to freeze probe"); 
        resultString += " Success=\"false\"";
        metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, "FAIL");
      }
    }
    else if (parameterName == vtkPlusCapistranoVideoSource::CAPISTRANO_IS_PROBE_FROZEN)
    {
      value = device->IsFrozen() ? "1" : "0";
      std::stringstream ss;
      ss << value;
      resultString += " Success=\"true\"";
      resultString += " Value=\"" + ss.str() + "\"";
      metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, ss.str());
      status = PLUS_SUCCESS;
    }
    else if (parameterName == vtkPlusCapistranoVideoSource::CAPISTRANO_SET_JITTER_COMPENSATION)
    {
      int jitterCompensation_int = std::stoi(value);
      status = device->SetJitterCompensation((unsigned char)jitterCompensation_int);
      resultString += " Success=\"true\"";
      metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, "SUCCESS");
    }
    else if (parameterName == vtkPlusCapistranoVideoSource::CAPISTRANO_GET_JITTER_COMPENSATION)
    {
      value = std::to_string(device->GetJitterCompensation());
      std::stringstream ss;
      ss << value;
      resultString += " Success=\"true\"";
      resultString += " Value=\"" + ss.str() + "\"";
      metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, ss.str());
      status = PLUS_SUCCESS;
    }
    else if (parameterName == vtkPlusCapistranoVideoSource::CAPISTRANO_SET_MIS_MODE)
    {
      bool MISMode_bool = std::stoi(value);
      if (device->SetMISMode(MISMode_bool) == PLUS_SUCCESS)
      {
        resultString += " Success=\"true\"";
        metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, "SUCCESS");
      }
      else
      {
        LOG_ERROR("Failed to set MIS mode"); 
        resultString += " Success=\"false\"";
        metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, "FAIL");
      }
    }
    else if (parameterName == vtkPlusCapistranoVideoSource::CAPISTRANO_SET_MIS_PULSE_PERIOD)
    {
      int misPulse_int = std::stoi(value);
      if (device->SetMISPulsePeriod((unsigned char)misPulse_int) == PLUS_SUCCESS)
      {
        resultString += " Success=\"true\"";
        metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, "SUCCESS");
      }
      else
      {
        LOG_ERROR("Failed to set MIS pulse period"); 
        resultString += " Success=\"false\"";
        metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, "FAIL");
      }
    }
    if (status != PLUS_SUCCESS)
    {
      error += "Failed to set " + parameterName + ". ";
      resultString += " Success=\"false\"";
      metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, "FAIL");
      status = PLUS_FAIL;
      continue;
    }
    resultString += "/>";
  } // For each parameter
  resultString += "</CommandReply>";

  if (status != PLUS_SUCCESS)
  {
    LOG_WARNING("Failed to set US parameter, result string was: " << resultString);
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

  return status;
}
