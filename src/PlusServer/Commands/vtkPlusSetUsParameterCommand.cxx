/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusSetUsParameterCommand.h"

#include "vtkPlusDataCollector.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusCommandProcessor.h"
#include "vtkPlusUsImagingParameters.h"
#include "vtkPlusUsDevice.h"

#include <vtkVariant.h>
#include <vtkSmartPointer.h>

#include <limits>
#include <iterator>
#include <string>

namespace
{
  static const std::string SET_US_PARAMETER_CMD = "SetUsParameter";
}

vtkStandardNewMacro(vtkPlusSetUsParameterCommand);

//----------------------------------------------------------------------------
vtkPlusSetUsParameterCommand::vtkPlusSetUsParameterCommand()
{
  this->UsDeviceId = "";
  this->RequestedParameterChanges.clear();
}

//----------------------------------------------------------------------------
vtkPlusSetUsParameterCommand::~vtkPlusSetUsParameterCommand()
{
  this->RequestedParameterChanges.clear();
}

//----------------------------------------------------------------------------
void vtkPlusSetUsParameterCommand::SetNameToSetUsParameter()
{
  SetName(SET_US_PARAMETER_CMD);
}

//----------------------------------------------------------------------------
void vtkPlusSetUsParameterCommand::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkPlusSetUsParameterCommand::GetCommandNames(std::list<std::string>& cmdNames)
{
  cmdNames.clear();
  cmdNames.push_back(SET_US_PARAMETER_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusSetUsParameterCommand::GetDescription(const std::string& commandName)
{
  std::string desc;
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, SET_US_PARAMETER_CMD))
  {
    desc += SET_US_PARAMETER_CMD;
    //TODO:
    desc += ": Set depth image parameter. Attributes: UsDeviceId: ID of the ultrasound device.";
  }

  return desc;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSetUsParameterCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  this->RequestedParameterChanges.clear();
  if (vtkPlusCommand::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  this->SetUsDeviceId(aConfig->GetAttribute("UsDeviceId"));

  // Parse nested elements and store requested parameter changes
  for (int elemIndex = 0; elemIndex < aConfig->GetNumberOfNestedElements(); ++elemIndex)
  {
    vtkXMLDataElement* currentElem = aConfig->GetNestedElement(elemIndex);
    if (igsioCommon::IsEqualInsensitive(currentElem->GetName(), "Parameter"))
    {
      const char* parameterName = currentElem->GetAttribute("Name");
      const char* parameterValue = currentElem->GetAttribute("Value");
      if (!parameterName || !parameterValue)
      {
        LOG_ERROR("Unable to find required Name or Value attribute in " << (currentElem->GetName() ? currentElem->GetName() : "(undefined)") << " element in SetUsParameter command");
        continue;
      }

      this->RequestedParameterChanges[parameterName] = parameterValue;
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSetUsParameterCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::WriteConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(UsDeviceId, aConfig);

  // Write parameters as nested elements
  std::map<std::string, std::string>::iterator paramIt;
  for (paramIt = this->RequestedParameterChanges.begin(); paramIt != this->RequestedParameterChanges.end(); ++paramIt)
  {
    vtkSmartPointer<vtkXMLDataElement> paramElem = vtkSmartPointer<vtkXMLDataElement>::New();
    paramElem->SetAttribute("Name", paramIt->first.c_str());
    paramElem->SetAttribute("Value", paramIt->second.c_str());
    aConfig->AddNestedElement(paramElem);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSetUsParameterCommand::Execute()
{
  LOG_DEBUG("vtkPlusSetUsParameterCommand::Execute: " << (!this->Name.empty() ? this->Name : "(undefined)")
            << ", device: " << (this->UsDeviceId.empty() ? "(undefined)" : this->UsDeviceId));

  if (this->Name.empty())
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", "No command name specified.");
    return PLUS_FAIL;
  }
  else if (!igsioCommon::IsEqualInsensitive(this->Name, SET_US_PARAMETER_CMD))
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", "Unknown command name: " + this->Name + ".");
    return PLUS_FAIL;
  }

  vtkPlusUsDevice* usDevice = GetUsDevice();
  if (usDevice == NULL)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", std::string("Device ")
                               + (this->UsDeviceId.empty() ? "(undefined)" : this->UsDeviceId) + std::string(" is not found."));
    return PLUS_FAIL;
  }

  std::string usDeviceId = (usDevice->GetDeviceId().empty() ? "(unknown)" : usDevice->GetDeviceId());
  vtkPlusUsImagingParameters* imagingParameters = usDevice->GetImagingParameters();
  std::string resultString = "<CommandReply>";
  std::string error = "";
  std::map < std::string, std::pair<IANA_ENCODING_TYPE, std::string> > metaData;
  PlusStatus status = PLUS_SUCCESS;

  std::map<std::string, std::string>::iterator paramIt;
  for (paramIt = this->RequestedParameterChanges.begin(); paramIt != this->RequestedParameterChanges.end(); ++paramIt)
  {
    std::string parameterName = paramIt->first;
    std::string value = paramIt->second;
    resultString += "<Parameter Name=\"" + parameterName + "\"";

    if (parameterName == vtkPlusUsImagingParameters::KEY_TGC)
    {
      std::stringstream ss;
      ss.str(value);
      std::vector<double> numbers((std::istream_iterator<double>(ss)), std::istream_iterator<double>());
      if (numbers.size() != 3)
      {
        error += "Failed to parse " + parameterName + ".";
        resultString += " Success=\"false\"/>";
        metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, "FAIL");
        status = PLUS_FAIL;
        continue;
      }
      imagingParameters->SetTimeGainCompensation(numbers);
    }
    else if (parameterName == vtkPlusUsImagingParameters::KEY_IMAGESIZE)
    {
      std::stringstream ss;
      ss.str(value);
      std::vector<int> numbers((std::istream_iterator<int>(ss)), std::istream_iterator<int>());
      if (numbers.size() != 3)
      {
        error += "Failed to parse " + parameterName + ". ";
        resultString += " Success=\"false\"/>";
        metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, "FAIL");
        status = PLUS_FAIL;
        continue;
      }
      imagingParameters->SetImageSize(numbers[0], numbers[1], numbers[2]);
    }
    else if (parameterName == vtkPlusUsImagingParameters::KEY_FREQUENCY
             || parameterName == vtkPlusUsImagingParameters::KEY_DEPTH
             || parameterName == vtkPlusUsImagingParameters::KEY_SECTOR
             || parameterName == vtkPlusUsImagingParameters::KEY_GAIN
             || parameterName == vtkPlusUsImagingParameters::KEY_INTENSITY
             || parameterName == vtkPlusUsImagingParameters::KEY_CONTRAST
             || parameterName == vtkPlusUsImagingParameters::KEY_POWER
             || parameterName == vtkPlusUsImagingParameters::KEY_DYNRANGE
             || parameterName == vtkPlusUsImagingParameters::KEY_ZOOM
             || parameterName == vtkPlusUsImagingParameters::KEY_SOUNDVELOCITY
             || parameterName == vtkPlusUsImagingParameters::KEY_VOLTAGE)
    {
      // double type parameter
      bool valid = false;
      double parameterValue = vtkVariant(value).ToDouble(&valid);
      if (!valid)
      {
        error += "Failed to parse " + parameterName + ". ";
        resultString += " Success=\"false\"/>";
        metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, "FAIL");
        status = PLUS_FAIL;
        continue;
      }
      imagingParameters->SetValue<double>(parameterName, parameterValue);
    }
    else
    {
      error += "Invalid parameter " + parameterName + ". ";
      resultString += " Success=\"false\"/>";
      metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, "FAIL");
      status = PLUS_FAIL;
      continue;
    }

    if (usDevice->SetNewImagingParameters(*imagingParameters) == PLUS_FAIL)
    {
      error += "Failed to set " + parameterName + ". ";
      resultString += " Success=\"false\"/>";
      metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, "FAIL");
      status = PLUS_FAIL;
      continue;
    }

    resultString += " Success=\"true\"/>";
    metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, "SUCCESS");
  } // For each parameter
  resultString += "</CommandReply>";
  
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

//----------------------------------------------------------------------------
vtkPlusUsDevice* vtkPlusSetUsParameterCommand::GetUsDevice()
{
  vtkPlusDataCollector* dataCollector = GetDataCollector();
  if (dataCollector == NULL)
  {
    LOG_ERROR("Data collector is invalid");
    return NULL;
  }
  vtkPlusUsDevice* usDevice = NULL;
  if (!this->UsDeviceId.empty())
  {
    // Ultrasound device ID is specified
    vtkPlusDevice* device = NULL;
    if (dataCollector->GetDevice(device, this->UsDeviceId) != PLUS_SUCCESS)
    {
      LOG_ERROR("No ultrasound device has been found by the name " << this->UsDeviceId);
      return NULL;
    }
    // device found
    usDevice = vtkPlusUsDevice::SafeDownCast(device);
    if (usDevice == NULL)
    {
      // wrong type
      LOG_ERROR("The specified device " << this->UsDeviceId << " is not UsDevice");
      return NULL;
    }
  }
  else
  {
    // No ultrasound device id is specified, auto-detect the first one and use that
    for (DeviceCollectionConstIterator it = dataCollector->GetDeviceConstIteratorBegin(); it != dataCollector->GetDeviceConstIteratorEnd(); ++it)
    {
      usDevice = vtkPlusUsDevice::SafeDownCast(*it);
      if (usDevice != NULL)
      {
        // found an ultrasound device
        break;
      }
    }
    if (usDevice == NULL)
    {
      LOG_ERROR("No UsDevice has been found");
      return NULL;
    }
  }
  return usDevice;
}
