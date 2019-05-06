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

#include <iterator>
#include <limits>
#include <map>
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
    if (igsioCommon::IsEqualInsensitive(currentElem->GetName(), "SetFlag"))
    {
      const char* parameterName = currentElem->GetAttribute("Name");
      const char* parameterValue = currentElem->GetAttribute("Value");

      if (!parameterName || !parameterValue)
      {
        LOG_ERROR("Unable to find required Name or Value attribute in " << (currentElem->GetName() ? currentElem->GetName() : "(undefined)") << " element in Atracsys SetFlag command");
        continue;
      }

      this->CommandList[parameterName] = parameterValue;
    }
    else if (igsioCommon::IsEqualInsensitive(currentElem->GetName(), "SetLED"))
    {
      const char* rValue = currentElem->GetAttribute("Red");
      const char* gValue = currentElem->GetAttribute("Green");
      const char* bValue = currentElem->GetAttribute("Blue");
      const char* freqValue = currentElem->GetAttribute("Frequency");

      if (!rValue || !gValue || !bValue || !freqValue)
      {
        LOG_ERROR("Unable to find required Red, Green, Blue or Frequency attribute in " << (currentElem->GetName() ? currentElem->GetName() : "(undefined)") << " element in Atracsys SetLED command");
        continue;
      }

      bool valid = false;
      this->LedR = vtkVariant(rValue).ToInt(&valid);
      if (!valid)
      {
        LOG_ERROR("Failed to parse Red value in AtracsysCommand SetLED message.");
        continue;
      }
      this->LedG = vtkVariant(gValue).ToInt(&valid);
      if (!valid)
      {
        LOG_ERROR("Failed to parse Green value in AtracsysCommand SetLED message.");
        continue;
      }
      this->LedB = vtkVariant(bValue).ToInt(&valid);
      if (!valid)
      {
        LOG_ERROR("Failed to parse Blue value in AtracsysCommand SetLED message.");
        continue;
      }
      this->LedFreq = vtkVariant(freqValue).ToInt(&valid);
      if (!valid)
      {
        LOG_ERROR("Failed to parse Frequency value in AtracsysCommand SetLED message.");
        continue;
      }

      this->CommandList["SetLED"] = ""; // flag value to update LED with provided values
    }
    else if (igsioCommon::IsEqualInsensitive(currentElem->GetName(), "EnableTool"))
    {
      const char* toolId = currentElem->GetAttribute("ToolId");
      const char* enabled = currentElem->GetAttribute("Enabled");

      if (!toolId || !enabled)
      {
        LOG_ERROR("Unable to find required ToolId or Enabled attribute in " << (currentElem->GetName() ? currentElem->GetName() : "(undefined)") << " element in Atracsys AddTool command");
        continue;
      }

      this->CommandList["EnableTool"] = toolId;
      this->EnableDisableTools[toolId] = enabled;
    }
    else if (igsioCommon::IsEqualInsensitive(currentElem->GetName(), "AddTool"))
    {
      const char* toolId = currentElem->GetAttribute("ToolId");
      const char* geometry = currentElem->GetAttribute("Geometry");

      if (!toolId || !geometry)
      {
        LOG_ERROR("Unable to find required Name or Value attribute in " << (currentElem->GetName() ? currentElem->GetName() : "(undefined)") << " element in Atracsys AddTool command");
        continue;
      }

      this->CommandList["AddTool"] = toolId;
      this->Markers[toolId] = geometry;
    }
    else
    {
      // invalid command
      LOG_ERROR("Invalid command name (" << currentElem->GetName() << ") provided to AtracsysCommand.")
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
    if (igsioCommon::IsEqualInsensitive(commandIt->first, "SetLED"))
    {
      // write SetLED element
      vtkSmartPointer<vtkXMLDataElement> ledElem = vtkSmartPointer<vtkXMLDataElement>::New();
      ledElem->SetName("SetLED");
      ledElem->SetAttribute("Red", std::to_string(this->LedR).c_str());
      ledElem->SetAttribute("Green", std::to_string(this->LedG).c_str());
      ledElem->SetAttribute("Blue", std::to_string(this->LedB).c_str());
      ledElem->SetAttribute("Frequency", std::to_string(this->LedFreq).c_str());
      aConfig->AddNestedElement(ledElem);
    }
    else if (igsioCommon::IsEqualInsensitive(commandIt->first, "EnableTool"))
    {
      // write AddTool element
      vtkSmartPointer<vtkXMLDataElement> enableToolElem = vtkSmartPointer<vtkXMLDataElement>::New();
      enableToolElem->SetName("EnableTool");
      enableToolElem->SetAttribute("ToolId", commandIt->second.c_str());
      std::map<std::string, std::string>::iterator enabledToolValue = this->EnableDisableTools.find(commandIt->second);
      enableToolElem->SetAttribute("Enabled", enabledToolValue->second.c_str());
      aConfig->AddNestedElement(enableToolElem);
    }
    else if (igsioCommon::IsEqualInsensitive(commandIt->first, "AddTool"))
    {
      // write AddTool element
      vtkSmartPointer<vtkXMLDataElement> addToolElem = vtkSmartPointer<vtkXMLDataElement>::New();
      addToolElem->SetName("AddTool");
      addToolElem->SetAttribute("ToolId", commandIt->second.c_str());
      std::map<std::string, std::string>::iterator marker = this->Markers.find(commandIt->second);
      addToolElem->SetAttribute("Geometry", marker->second.c_str());
      aConfig->AddNestedElement(addToolElem);
    }
    else
    {
      // write SetFlag element
      vtkSmartPointer<vtkXMLDataElement> flagElem = vtkSmartPointer<vtkXMLDataElement>::New();
      flagElem->SetName("SetFlag");
      flagElem->SetAttribute("Name", commandIt->first.c_str());
      flagElem->SetAttribute("Value", commandIt->second.c_str());
      aConfig->AddNestedElement(flagElem);
    }
  }

  return PLUS_SUCCESS;
}

PlusStatus vtkPlusAtracsysCommand::StringToBool(std::string strVal, bool& boolVal)
{
  if (igsioCommon::IsEqualInsensitive(strVal, "TRUE"))
  {
    boolVal = true;
    return PLUS_SUCCESS;
  }
  else if (igsioCommon::IsEqualInsensitive(strVal, "FALSE"))
  {
    boolVal = false;
    return PLUS_SUCCESS;
  }
  return PLUS_FAIL;
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

  vtkPlusAtracsysTracker* atracsysDevice = GetAtracsysDevice();
  if (atracsysDevice == NULL)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", std::string("Device ")
                               + (this->AtracsysDeviceId.empty() ? "(undefined)" : this->AtracsysDeviceId) + std::string(" is not found."));
    return PLUS_FAIL;
  }

  std::string atracsysDeviceId = (atracsysDevice->GetDeviceId().empty() ? "(unknown)" : atracsysDevice->GetDeviceId());
  std::string resultString = "<CommandReply>";
  std::string error = "";
  std::map <std::string, std::pair<IANA_ENCODING_TYPE, std::string> > metaData;
  PlusStatus status = PLUS_SUCCESS;

  std::map<std::string, std::string>::iterator commandIt;
  for (commandIt = this->CommandList.begin(); commandIt != this->CommandList.end(); commandIt++)
  {
    std::string commandName = commandIt->first;
    std::string value = commandIt->second;
    resultString += "<Parameter Name=\"" + commandName + "\"";

    PlusStatus conversion = PLUS_SUCCESS;
    PlusStatus setInDevice = PLUS_SUCCESS;

    if (commandName == vtkPlusAtracsysTracker::ATRACSYS_COMMAND_LED_ENABLED)
    {
      bool enabled;
      conversion = this->StringToBool(value, enabled);
      setInDevice = atracsysDevice->SetLedEnabled(enabled);
    }
    else if (commandName == vtkPlusAtracsysTracker::ATRACSYS_COMMAND_LASER_ENABLED)
    {
      bool enabled;
      conversion = this->StringToBool(value, enabled);
      setInDevice = atracsysDevice->SetLaserEnabled(enabled);
    }
    else if (commandName == vtkPlusAtracsysTracker::ATRACSYS_COMMAND_VIDEO_ENABLED)
    {
      bool enabled;
      conversion = this->StringToBool(value, enabled);
      setInDevice = atracsysDevice->SetVideoEnabled(enabled);
    }
    else if (commandName == vtkPlusAtracsysTracker::ATRACSYS_COMMAND_LED_RGBF)
    {
      // value is a placeholder, we can ignore it
      setInDevice = atracsysDevice->SetUserLEDState(this->LedR, this->LedG, this->LedB, this->LedFreq, true);
    }
    else if (commandName == vtkPlusAtracsysTracker::ATRACSYS_COMMAND_TOOL_ENABLED)
    {
      // value contains ToolId to enable / disable
      std::string strEnabled = this->EnableDisableTools.find(value)->second;
      bool boolEnabled;
      conversion = this->StringToBool(strEnabled, boolEnabled);
      setInDevice = atracsysDevice->SetToolEnabled(value, boolEnabled);
    }
    else if (commandName == vtkPlusAtracsysTracker::ATRACSYS_COMMAND_ADD_GEOMETRY)
    {
      // value contains ToolId of geometry to add
      std::string geometry = this->Markers.find(value)->second;
      setInDevice = atracsysDevice->AddGeometry(value, geometry);
    }
    else
    {
      LOG_WARNING("Unrecognized AtracsysCommand recieved with name: " << commandName
        << ". Please see the documentation for a list of available commands.");
      return PLUS_FAIL;
    }

    if (conversion == PLUS_FAIL || setInDevice == PLUS_FAIL)
    {
      status = PLUS_FAIL;
    }

    resultString += " Success=\"true\"/>";
    metaData[commandName] = std::make_pair(IANA_TYPE_US_ASCII, "SUCCESS");
  }
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
vtkPlusAtracsysTracker* vtkPlusAtracsysCommand::GetAtracsysDevice()
{
  vtkPlusDataCollector* dataCollector = GetDataCollector();
  if (dataCollector == NULL)
  {
    LOG_ERROR("Data collector is invalid");
    return NULL;
  }
  if (!this->AtracsysDeviceId.empty())
  {
    // Reconstructor device ID is specified
    vtkPlusDevice* device = NULL;
    if (dataCollector->GetDevice(device, this->AtracsysDeviceId) != PLUS_SUCCESS)
    {
      LOG_ERROR("No Atracsys device has been found by the name " << this->AtracsysDeviceId);
      return NULL;
    }
    // device found
    vtkPlusAtracsysTracker* atracsysDevice = vtkPlusAtracsysTracker::SafeDownCast(device);
    if (atracsysDevice == NULL)
    {
      // wrong type
      LOG_ERROR("The specified device " << this->AtracsysDeviceId << " is not an Atracsys device");
      return NULL;
    }
    return atracsysDevice;
  }
  else
  {
    // No Atracsys device id is specified, auto-detect the first one and use that
    for (DeviceCollectionConstIterator it = dataCollector->GetDeviceConstIteratorBegin(); it != dataCollector->GetDeviceConstIteratorEnd(); ++it)
    {
      vtkPlusAtracsysTracker* atracsysDevice = vtkPlusAtracsysTracker::SafeDownCast(*it);
      if (atracsysDevice != NULL)
      {
        // found an Atracsys device
        this->SetAtracsysDeviceId(atracsysDevice->GetDeviceId());
        return atracsysDevice;
      }
    }
    LOG_ERROR("No Atracsys device has been found");
    return NULL;
  }
}
