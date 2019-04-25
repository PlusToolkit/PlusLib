/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusGetUsParameterCommand.h"

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
  static const std::string GET_US_PARAMETER_CMD = "GetUsParameter";
}

vtkStandardNewMacro(vtkPlusGetUsParameterCommand);

//----------------------------------------------------------------------------
vtkPlusGetUsParameterCommand::vtkPlusGetUsParameterCommand()
{
  this->UsDeviceId = "";
  this->RequestedParameters.clear();
}

//----------------------------------------------------------------------------
vtkPlusGetUsParameterCommand::~vtkPlusGetUsParameterCommand()
{
  this->RequestedParameters.clear();
}

//----------------------------------------------------------------------------
void vtkPlusGetUsParameterCommand::SetNameToGetUsParameter()
{
  SetName(GET_US_PARAMETER_CMD);
}

//----------------------------------------------------------------------------
void vtkPlusGetUsParameterCommand::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkPlusGetUsParameterCommand::GetCommandNames(std::list<std::string>& cmdNames)
{
  cmdNames.clear();
  cmdNames.push_back(GET_US_PARAMETER_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusGetUsParameterCommand::GetDescription(const std::string& commandName)
{
  std::string desc;
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, GET_US_PARAMETER_CMD))
  {
    desc += GET_US_PARAMETER_CMD;
    //TODO:
    desc += ": Get ultrasound image parameter. Attributes: UsDeviceId: ID of the ultrasound device.";
  }

  return desc;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGetUsParameterCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  this->RequestedParameters.clear();
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
      if (!parameterName)
      {
        LOG_ERROR("Unable to find required Name attribute in " << (currentElem->GetName() ? currentElem->GetName() : "(undefined)") << " element in GetUsParameter command");
        continue;
      }

      this->RequestedParameters.push_back(parameterName);
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGetUsParameterCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::WriteConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(UsDeviceId, aConfig);

  // Write parameters as nested elements
  std::vector<std::string>::iterator paramIt;
  for (paramIt = this->RequestedParameters.begin(); paramIt != this->RequestedParameters.end(); ++paramIt)
  {
    vtkSmartPointer<vtkXMLDataElement> paramElem = vtkSmartPointer<vtkXMLDataElement>::New();
    paramElem->SetAttribute("Name", paramIt->c_str());
    aConfig->AddNestedElement(paramElem);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGetUsParameterCommand::Execute()
{
  LOG_DEBUG("vtkPlusGetUsParameterCommand::Execute: " << (!this->Name.empty() ? this->Name : "(undefined)")
            << ", device: " << (this->UsDeviceId.empty() ? "(undefined)" : this->UsDeviceId));

  if (this->Name.empty())
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", "No command name specified.");
    return PLUS_FAIL;
  }
  else if (!igsioCommon::IsEqualInsensitive(this->Name, GET_US_PARAMETER_CMD))
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

  std::map < std::string, std::pair<IANA_ENCODING_TYPE, std::string> > metaData;

  vtkPlusUsImagingParameters* imagingParameters = usDevice->GetImagingParameters();
  std::string resultString = "<CommandReply>";
  std::string error = "";
  PlusStatus status = PLUS_SUCCESS;

  std::vector<std::string>::iterator paramIt;
  for (paramIt = this->RequestedParameters.begin(); paramIt != this->RequestedParameters.end(); ++paramIt)
  {
    std::string parameterName = *paramIt;
    resultString += "<Parameter Name=\"" + parameterName + "\"";

    if (parameterName == vtkPlusUsImagingParameters::KEY_TGC)
    {
      if (imagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_TGC))
      {
        std::stringstream ss;
        std::vector<double> numbers = imagingParameters->GetTimeGainCompensation();
        for (std::vector<double>::iterator numberIt; numberIt != numbers.begin(); numberIt != numbers.end())
        {
          ss << *numberIt << " ";
        }
        resultString += " Success=\"true\"";
        resultString += " Value=\"" + ss.str() + "\"";
        metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, ss.str());
      }
      else
      {
        resultString += " Success=\"false\"";
        error += parameterName + " is not set. ";
        status = PLUS_FAIL;
      }
    }
    else if (parameterName == vtkPlusUsImagingParameters::KEY_IMAGESIZE)
    {
      if (imagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_IMAGESIZE))
      {
        std::stringstream ss;
        FrameSizeType imageSize = imagingParameters->GetImageSize();
        for (FrameSizeType::iterator imageSizeIt = imageSize.begin(); imageSizeIt != imageSize.end(); ++imageSizeIt)
        {
          ss << *imageSizeIt << " ";
        }
        resultString += " Success=\"true\"";
        resultString += " Value=\"" + ss.str() + "\"";
        metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, ss.str());
      }
      else
      {
        resultString += " Success=\"false\"";
        error += parameterName + " is not set. ";
        status = PLUS_FAIL;
      }
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
      if (imagingParameters->IsSet(parameterName))
      {
        // double type parameter
        double value = 0;
        if (imagingParameters->GetValue<double>(parameterName, value) == PLUS_SUCCESS)
        {
          std::stringstream ss;
          ss << value;
          resultString += " Success=\"true\"";
          resultString += " Value=\"" + ss.str() + "\"";
          metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, ss.str());
        }
      }
      else
      {
        resultString += " Success=\"false\"";
        error += parameterName + " is not set. ";
        status = PLUS_FAIL;
      }
    }
    else
    {
      error += "Invalid parameter " + parameterName + ". ";
      resultString += " Success=\"false\"";
      status = PLUS_FAIL;
    }
    resultString += "/>";

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
vtkPlusUsDevice* vtkPlusGetUsParameterCommand::GetUsDevice()
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
