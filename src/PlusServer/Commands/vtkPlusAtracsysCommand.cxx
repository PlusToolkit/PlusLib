/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusAtracsysCommand.h"
#include "vtkPlusAtracsysTracker.h"

#include "vtkPlusDataCollector.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDevice.h"

#include <vtkVariant.h>
#include <vtkSmartPointer.h>

#include <limits>
#include <iterator>
#include <string>

namespace
{
  static const std::string ATRACSYS_CMD_NAME = "AtracsysCommand";
}

vtkStandardNewMacro(vtkPlusAtracsysCommand);

//----------------------------------------------------------------------------
vtkPlusAtracsysCommand::vtkPlusAtracsysCommand()
{
  this->AtracsysDeviceId = "";
}

//----------------------------------------------------------------------------
vtkPlusAtracsysCommand::~vtkPlusAtracsysCommand()
{
}

//----------------------------------------------------------------------------
void vtkPlusAtracsysCommand::SetNameToSetUsParameter()
{
  SetName(ATRACSYS_CMD_NAME);
}

//----------------------------------------------------------------------------
void vtkPlusAtracsysCommand::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkPlusAtracsysCommand::GetCommandNames(std::list<std::string>& cmdNames)
{
  cmdNames.clear();
  cmdNames.push_back(ATRACSYS_CMD_NAME);
}

//----------------------------------------------------------------------------
std::string vtkPlusAtracsysCommand::GetDescription(const std::string& commandName)
{
  std::string desc;
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, ATRACSYS_CMD_NAME))
  {
    desc += ATRACSYS_CMD_NAME;
    //TODO:
    desc += ": Send command to Atracsys device.";
  }

  return desc;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAtracsysCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  this->SetAtracsysDeviceId(aConfig->GetAttribute("DeviceId"));

  // Parse nested elements and store requested parameter changes
  for (int elemIndex = 0; elemIndex < aConfig->GetNumberOfNestedElements(); ++elemIndex)
  {
    vtkXMLDataElement* currentElem = aConfig->GetNestedElement(elemIndex);
    if (igsioCommon::IsEqualInsensitive(currentElem->GetName(), "Command"))
    {
      const char* parameterName = currentElem->GetAttribute("Name");
      const char* parameterValue = currentElem->GetAttribute("Value");
      if (!parameterName || !parameterValue)
      {
        LOG_ERROR("Unable to find required Name or Value attribute in " << (currentElem->GetName() ? currentElem->GetName() : "(undefined)") << " element in SetUsParameter command");
        continue;
      }

      this->CommandList[parameterName] = parameterValue;
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAtracsysCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::WriteConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(AtracsysDeviceId, aConfig);

  // Write parameters as nested elements
  std::map<std::string, std::string>::iterator commandIt;
  for (commandIt = this->CommandList.begin(); commandIt != this->CommandList.end(); commandIt++)
  { 
    // TODO: How does the paramElem know it should be a <Command>?
    vtkSmartPointer<vtkXMLDataElement> paramElem = vtkSmartPointer<vtkXMLDataElement>::New();
    paramElem->SetAttribute("Name", commandIt->first.c_str());
    paramElem->SetAttribute("Value", commandIt->second.c_str());
    aConfig->AddNestedElement(paramElem);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAtracsysCommand::Execute()
{
  LOG_DEBUG("vtkPlusAtracsysCommand::Execute: " << (!this->Name.empty() ? this->Name : "(undefined)")
            << ", device: " << (this->AtracsysDeviceId.empty() ? "(undefined)" : this->AtracsysDeviceId));

  if (this->Name.empty())
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", "No command name specified.");
    return PLUS_FAIL;
  }
  else if (!igsioCommon::IsEqualInsensitive(this->Name, ATRACSYS_CMD_NAME))
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", "Unknown command name: " + this->Name + ".");
    return PLUS_FAIL;
  }

  vtkPlusDevice* atracsysDevice = GetAtracsysDevice();
  if (atracsysDevice == NULL)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", std::string("Device ")
                               + (this->AtracsysDeviceId.empty() ? "(undefined)" : this->AtracsysDeviceId) + std::string(" is not found."));
    return PLUS_FAIL;
  }

  std::string atracsysDeviceId = (atracsysDevice->GetDeviceId().empty() ? "(unknown)" : atracsysDevice->GetDeviceId());
  // vtkPlusUsImagingParameters* imagingParameters = usDevice->GetImagingParameters();
  std::string resultString = "<CommandReply>";
  std::string error = "";
  std::map < std::string, std::pair<IANA_ENCODING_TYPE, std::string> > metaData;
  PlusStatus status = PLUS_SUCCESS;

  std::map<std::string, std::string>::iterator commandIt;
  for (commandIt = this->CommandList.begin(); commandIt != this->CommandList.end(); commandIt++)
  {
    std::string commandName = commandIt->first;
    std::string value = commandIt->second;
    resultString += "<Parameter Name=\"" + commandName + "\"";

    if (commandName == vtkPlusAtracsysTracker::ATRACSYS_COMMAND_LED_ENABLED)
    {

    }
    else if (commandName == vtkPlusAtracsysTracker::ATRACSYS_COMMAND_LED_R)
    {

    }
    else if (commandName == vtkPlusAtracsysTracker::ATRACSYS_COMMAND_LED_G)
    {

    }
    else if (commandName == vtkPlusAtracsysTracker::ATRACSYS_COMMAND_LED_B)
    {

    }
    else if (commandName == vtkPlusAtracsysTracker::ATRACSYS_COMMAND_LED_FREQ)
    {

    }
    else if (commandName == vtkPlusAtracsysTracker::ATRACSYS_COMMAND_TOOL_ENABLED)
    {

    }
    else if (commandName == vtkPlusAtracsysTracker::ATRACSYS_COMMAND_LASER_ENABLED)
    {

    }
    else if (commandName == vtkPlusAtracsysTracker::ATRACSYS_COMMAND_VIDEO_ENABLED)
    {

    }
    else if (commandName == vtkPlusAtracsysTracker::ATRACSYS_COMMAND_ADD_GEOMETRY)
    {

    }
    else
    {
      LOG_WARNING("Unrecognized AtracsysCommand recieved with name: " << commandName
        << ". Please see the documentation for a list of available commands.");
    }

    resultString += " Success=\"true\"/>";
    metaData[commandName] = std::make_pair(IANA_TYPE_US_ASCII, "SUCCESS");
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
vtkPlusDevice* vtkPlusAtracsysCommand::GetAtracsysDevice()
{
  vtkPlusDataCollector* dataCollector = GetDataCollector();
  if (dataCollector == NULL)
  {
    LOG_ERROR("Data collector is invalid");
    return NULL;
  }
  vtkPlusDevice* atracsysDevice = NULL;
  if (!this->AtracsysDeviceId.empty())
  {
    // Atracsys device ID is specified
    vtkPlusDevice* device = NULL;
    if (dataCollector->GetDevice(device, this->AtracsysDeviceId) != PLUS_SUCCESS)
    {
      LOG_ERROR("No Atracsys device has been found with the name " << this->AtracsysDeviceId);
      return NULL;
    }
    // device found
    atracsysDevice = vtkPlusDevice::SafeDownCast(device);
    if (atracsysDevice == NULL)
    {
      // wrong type
      LOG_ERROR("The specified device " << this->AtracsysDeviceId << " is not an Atracsys device");
      return NULL;
    }
  }
  else
  {
    // No Atracsys device id is specified, auto-detect the first one and use that
    for (DeviceCollectionConstIterator it = dataCollector->GetDeviceConstIteratorBegin(); it != dataCollector->GetDeviceConstIteratorEnd(); ++it)
    {
      atracsysDevice = vtkPlusDevice::SafeDownCast(*it);
      if (atracsysDevice != NULL)
      {
        // found an ultrasound device
        break;
      }
    }
    if (atracsysDevice == NULL)
    {
      LOG_ERROR("No Atracsys device has been found");
      return NULL;
    }
  }
  return atracsysDevice;
}
