/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusSetCameraControlParameterCommand.h"

#include "vtkPlusDataCollector.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusCommandProcessor.h"
#include "vtkPlusCameraControlParameters.h"
#include "vtkPlusMmfVideoSource.h"

#include <vtkVariant.h>
#include <vtkSmartPointer.h>

#include <limits>
#include <iterator>
#include <string>

namespace
{
  static const std::string SET_CAMERA_PARAMETER_CMD = "SetCameraControlParameters";
}

vtkStandardNewMacro(vtkPlusSetCameraControlParameterCommand);

//----------------------------------------------------------------------------
vtkPlusSetCameraControlParameterCommand::vtkPlusSetCameraControlParameterCommand()
{
  this->CameraDeviceId = "";
  this->RequestedParameterChanges.clear();
}

//----------------------------------------------------------------------------
vtkPlusSetCameraControlParameterCommand::~vtkPlusSetCameraControlParameterCommand()
{
  this->RequestedParameterChanges.clear();
}

//----------------------------------------------------------------------------
void vtkPlusSetCameraControlParameterCommand::SetNameToSetCameraControlParameter()
{
  SetName(SET_CAMERA_PARAMETER_CMD);
}

//----------------------------------------------------------------------------
void vtkPlusSetCameraControlParameterCommand::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkPlusSetCameraControlParameterCommand::GetCommandNames(std::list<std::string>& cmdNames)
{
  cmdNames.clear();
  cmdNames.push_back(SET_CAMERA_PARAMETER_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusSetCameraControlParameterCommand::GetDescription(const std::string& commandName)
{
  std::string desc;
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, SET_CAMERA_PARAMETER_CMD))
  {
    desc += SET_CAMERA_PARAMETER_CMD;
    desc += ": Set camera image parameter. Attributes: CameraDeviceId: ID of the camera device.";
  }
  return desc;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSetCameraControlParameterCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  this->RequestedParameterChanges.clear();
  if (vtkPlusCommand::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  this->SetCameraDeviceId(aConfig->GetAttribute("CameraDeviceId"));

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
PlusStatus vtkPlusSetCameraControlParameterCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::WriteConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(CameraDeviceId, aConfig);

  // Write parameters as nested elements
  std::map<std::string, std::string>::iterator paramIt;
  for (paramIt = this->RequestedParameterChanges.begin(); paramIt != this->RequestedParameterChanges.end(); ++paramIt)
  {
    vtkSmartPointer<vtkXMLDataElement> paramElem = vtkSmartPointer<vtkXMLDataElement>::New();
    paramElem->SetName("Parameter");
    paramElem->SetAttribute("Name", paramIt->first.c_str());
    paramElem->SetAttribute("Value", paramIt->second.c_str());
    aConfig->AddNestedElement(paramElem);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSetCameraControlParameterCommand::Execute()
{
  LOG_DEBUG("vtkPlusSetCameraControlParameterCommand::Execute: " << (!this->Name.empty() ? this->Name : "(undefined)")
    << ", device: " << (this->CameraDeviceId.empty() ? "(undefined)" : this->CameraDeviceId));

  if (this->Name.empty())
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", "No command name specified.");
    return PLUS_FAIL;
  }
  else if (!igsioCommon::IsEqualInsensitive(this->Name, SET_CAMERA_PARAMETER_CMD))
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", "Unknown command name: " + this->Name + ".");
    return PLUS_FAIL;
  }

  vtkPlusMmfVideoSource* cameraDevice = vtkPlusMmfVideoSource::SafeDownCast(this->GetCameraDevice());
  if (cameraDevice == NULL)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", std::string("Device ")
      + (this->CameraDeviceId.empty() ? "(undefined)" : this->CameraDeviceId) + std::string(" is not found."));
    return PLUS_FAIL;
  }

  std::string cameraDeviceId = (cameraDevice->GetDeviceId().empty() ? "(unknown)" : cameraDevice->GetDeviceId());
  std::string resultString = "<CommandReply>";
  std::string error = "";
  std::map < std::string, std::pair<IANA_ENCODING_TYPE, std::string> > metaData;
  PlusStatus status = PLUS_SUCCESS;

  vtkPlusCameraControlParameters* cameraControlParameters = cameraDevice->GetCameraControlParameters();

  std::map<std::string, std::string>::iterator paramIt;
  for (paramIt = this->RequestedParameterChanges.begin(); paramIt != this->RequestedParameterChanges.end(); ++paramIt)
  {
    std::string parameterName = paramIt->first;
    std::string value = paramIt->second;
    resultString += "<Parameter Name=\"" + parameterName + "\"";

    if (parameterName == vtkPlusCameraControlParameters::KEY_AUTO_EXPOSURE
      || parameterName == vtkPlusCameraControlParameters::KEY_AUTO_FOCUS)
    {
      // boolean type parameter
      bool parameterValue = STRCASECMP(value.c_str(), "true") == 0;
      cameraControlParameters->SetValue<std::string>(parameterName, parameterValue ? "true" : "false");
    }
    else if (parameterName == vtkPlusCameraControlParameters::KEY_PAN_DEGREES
      || parameterName == vtkPlusCameraControlParameters::KEY_TILT_DEGREES
      || parameterName == vtkPlusCameraControlParameters::KEY_ROLL_DEGREES
      || parameterName == vtkPlusCameraControlParameters::KEY_ZOOM_MM
      || parameterName == vtkPlusCameraControlParameters::KEY_FOCUS_MM)
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
      cameraControlParameters->SetValue<double>(parameterName, parameterValue);
    }
    else if (parameterName == vtkPlusCameraControlParameters::KEY_EXPOSURE_LOG2SECONDS
      || parameterName == vtkPlusCameraControlParameters::KEY_IRIS_FSTOP)
    {
      // integer type parameter
      bool valid = false;
      int parameterValue = vtkVariant(value).ToInt(&valid);
      if (!valid)
      {
        error += "Failed to parse " + parameterName + ". ";
        resultString += " Success=\"false\"/>";
        metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, "FAIL");
        status = PLUS_FAIL;
        continue;
      }
      cameraControlParameters->SetValue<int>(parameterName, parameterValue);
    }
    else
    {
      error += "Invalid parameter " + parameterName + ". ";
      resultString += " Success=\"false\"/>";
      metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, "FAIL");
      status = PLUS_FAIL;
      continue;
    }

    if (cameraDevice->SetNewCameraControlParameters(*cameraControlParameters) == PLUS_FAIL)
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

//----------------------------------------------------------------------------
vtkPlusDevice* vtkPlusSetCameraControlParameterCommand::GetCameraDevice()
{
  vtkPlusDataCollector* dataCollector = GetDataCollector();
  if (dataCollector == NULL)
  {
    LOG_ERROR("Data collector is invalid");
    return NULL;
  }
  vtkPlusMmfVideoSource* cameraDevice = NULL;
  if (!this->CameraDeviceId.empty())
  {
    // Camera device ID is specified
    vtkPlusDevice* device = NULL;
    if (dataCollector->GetDevice(device, this->CameraDeviceId) != PLUS_SUCCESS)
    {
      LOG_ERROR("No camera device has been found by the name " << this->CameraDeviceId);
      return NULL;
    }
    // device found
    cameraDevice = dynamic_cast<vtkPlusMmfVideoSource*>(device);
    if (cameraDevice == NULL)
    {
      // wrong type
      LOG_ERROR("The specified device " << this->CameraDeviceId << " is not a camera device");
      return NULL;
    }
  }
  else
  {
    // No camera device id is specified, auto-detect the first one and use that
    for (DeviceCollectionConstIterator it = dataCollector->GetDeviceConstIteratorBegin(); it != dataCollector->GetDeviceConstIteratorEnd(); ++it)
    {
      cameraDevice = dynamic_cast<vtkPlusMmfVideoSource*>(*it);
      if (cameraDevice != NULL)
      {
        // found a camera device
        break;
      }
    }
    if (cameraDevice == NULL)
    {
      LOG_ERROR("No camera has been found");
      return NULL;
    }
  }
  return cameraDevice;
}
