/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "igtl_header.h"
#include "vtkPlusChannel.h"
#include "vtkPlusCommandProcessor.h"
#include "vtkPlusCommandResponse.h"
#include "vtkPlusDataCollector.h"
#include "vtkPlusDeviceFactory.h"
#include "vtkPlusOpenIGTLinkServer.h"
#include "vtkPlusRequestIdsCommand.h"

vtkStandardNewMacro(vtkPlusRequestIdsCommand);

static const char REQUEST_CHANNEL_IDS_CMD[] = "RequestChannelIds";
static const char REQUEST_DEVICE_CHANNEL_IDS_CMD[] = "RequestDeviceChannelIds";
static const char REQUEST_DEVICE_IDS_CMD[] = "RequestDeviceIds";
static const char REQUEST_INPUT_DEVICE_IDS_CMD[] = "RequestInputDeviceIds";

//----------------------------------------------------------------------------
vtkPlusRequestIdsCommand::vtkPlusRequestIdsCommand()
  : DeviceType(NULL)
  , DeviceId(NULL)
{
}

//----------------------------------------------------------------------------
vtkPlusRequestIdsCommand::~vtkPlusRequestIdsCommand()
{
}

//----------------------------------------------------------------------------
void vtkPlusRequestIdsCommand::SetNameToRequestChannelIds()
{
  SetName(REQUEST_CHANNEL_IDS_CMD);
}
void vtkPlusRequestIdsCommand::SetNameToRequestDeviceIds()
{
  SetName(REQUEST_DEVICE_IDS_CMD);
}
void vtkPlusRequestIdsCommand::SetNameToRequestInputDeviceIds()
{
  SetName(REQUEST_INPUT_DEVICE_IDS_CMD);
}
void vtkPlusRequestIdsCommand::SetNameToRequestDeviceChannelIds()
{
  SetName(REQUEST_DEVICE_CHANNEL_IDS_CMD);
}

//----------------------------------------------------------------------------
void vtkPlusRequestIdsCommand::GetCommandNames(std::list<std::string>& cmdNames)
{
  cmdNames.clear();
  cmdNames.push_back(REQUEST_CHANNEL_IDS_CMD);
  cmdNames.push_back(REQUEST_DEVICE_IDS_CMD);
  cmdNames.push_back(REQUEST_INPUT_DEVICE_IDS_CMD);
  cmdNames.push_back(REQUEST_DEVICE_CHANNEL_IDS_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusRequestIdsCommand::GetDescription(const char* commandName)
{
  std::string desc;
  if (commandName == NULL || STRCASECMP(commandName, REQUEST_CHANNEL_IDS_CMD))
  {
    desc += REQUEST_CHANNEL_IDS_CMD;
    desc += ": Request the list of channels for all devices.";
  }
  if (commandName == NULL || STRCASECMP(commandName, REQUEST_DEVICE_IDS_CMD))
  {
    desc += REQUEST_DEVICE_IDS_CMD;
    desc += ": Request the list of devices. Attributes: DeviceType: restrict the returned list of devices to a specific type (VirtualDiscCapture, VirtualVolumeReconstructor, etc.)";
  }
  if (commandName == NULL || STRCASECMP(commandName, REQUEST_INPUT_DEVICE_IDS_CMD))
  {
    desc += REQUEST_INPUT_DEVICE_IDS_CMD;
    desc += ": Request the list of devices that are used as input to the requested device. Attributes: DeviceId: the id of the device to query.";
  }
  if (commandName == NULL || STRCASECMP(commandName, REQUEST_DEVICE_CHANNEL_IDS_CMD))
  {
    desc += REQUEST_DEVICE_CHANNEL_IDS_CMD;
    desc += ": Request the list of channels for a given device. Attributes: DeviceId: the id of the device to query.";
  }
  return desc;
}

//----------------------------------------------------------------------------
void vtkPlusRequestIdsCommand::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusRequestIdsCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(DeviceType, aConfig);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(DeviceId, aConfig);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusRequestIdsCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::WriteConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_NULL(DeviceId, aConfig);
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_NULL(DeviceType, aConfig);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusRequestIdsCommand::Execute()
{
  if (this->Name == NULL)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", "No command name specified.");
    return PLUS_FAIL;
  }

  vtkPlusDataCollector* dataCollector = this->GetDataCollector();
  if (dataCollector == NULL)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", "No data collector.");
    return PLUS_FAIL;
  }

  DeviceCollection aCollection;
  if (dataCollector->GetDevices(aCollection) != PLUS_SUCCESS)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed, see error message.", "Unable to retrieve devices.");
    return PLUS_FAIL;
  }

  auto _FindDevice = [this](DeviceCollection & aCollection)
  {
    for (auto aDevice : aCollection)
    {
      if (this->DeviceId == aDevice->GetDeviceId())
      {
        return aDevice;
      }
    }
    return (vtkPlusDevice*)nullptr;
  };

  if (STRCASECMP(this->Name, REQUEST_CHANNEL_IDS_CMD) == 0)
  {
    std::string responseMessage;
    bool addSeparator = false;
    std::map<std::string, std::string> keyValuePairs;

    for (DeviceCollectionConstIterator deviceIt = aCollection.begin(); deviceIt != aCollection.end(); ++deviceIt)
    {
      vtkPlusDevice* aDevice = *deviceIt;
      if (aDevice->OutputChannelCount() > 0)
      {
        for (ChannelContainerConstIterator it = aDevice->GetOutputChannelsStart(); it != aDevice->GetOutputChannelsEnd(); ++it)
        {
          if (addSeparator)
          {
            responseMessage += ",";
          }
          responseMessage += (*it)->GetChannelId();
          addSeparator = true;
          keyValuePairs[(*it)->GetChannelId()] = (*it)->GetChannelId();
        }
      }
    }
    std::ostringstream oss;
    PlusIgtlClientInfo info;
    if (this->CommandProcessor->GetPlusServer()->GetClientInfo(this->GetClientId(), info) != PLUS_SUCCESS)
    {
      LOG_WARNING("Unable to locate client data for client id: " << this->GetClientId());
    }
    if (info.ClientHeaderVersion <= IGTL_HEADER_VERSION_2)
    {
      oss << responseMessage;
    }
    else
    {
      oss << "Found: " << keyValuePairs.size() << " parameter" << (keyValuePairs.size() > 1 ? "s." : ".");
    }

    this->QueueCommandResponse(PLUS_SUCCESS, oss.str(), "", &keyValuePairs);
    return PLUS_SUCCESS;
  }
  else if (STRCASECMP(this->Name, REQUEST_DEVICE_IDS_CMD) == 0)
  {
    std::string responseMessage;
    bool addSeparator = false;
    std::map<std::string, std::string> keyValuePairs;

    for (DeviceCollectionConstIterator deviceIt = aCollection.begin(); deviceIt != aCollection.end(); ++deviceIt)
    {
      vtkPlusDevice* aDevice = *deviceIt;
      if (aDevice == NULL)
      {
        continue;
      }
      std::string deviceClassName;
      if (this->DeviceType != NULL)
      {
        // Translate requested device ID (factory name) to c++ class name
        vtkSmartPointer<vtkPlusDeviceFactory> factory = vtkSmartPointer<vtkPlusDeviceFactory>::New();
        if (factory->GetDeviceClassName(this->DeviceType, deviceClassName) != PLUS_SUCCESS)
        {
          this->QueueCommandResponse(PLUS_FAIL, "Command failed, see error message.", "Unknown device type requested.");
          return PLUS_FAIL;
        }
      }
      if (deviceClassName.empty() || aDevice->GetClassName() == deviceClassName)
      {
        if (addSeparator)
        {
          responseMessage += ",";
        }
        responseMessage += aDevice->GetDeviceId();
        addSeparator = true;

        keyValuePairs[aDevice->GetDeviceId()] = aDevice->GetDeviceId();
      }
    }
    std::ostringstream oss;
    PlusIgtlClientInfo info;
    if (this->CommandProcessor->GetPlusServer()->GetClientInfo(this->GetClientId(), info) != PLUS_SUCCESS)
    {
      LOG_WARNING("Unable to locate client data for client id: " << this->GetClientId());
    }
    if (info.ClientHeaderVersion <= IGTL_HEADER_VERSION_2)
    {
      oss << responseMessage;
    }
    else
    {
      oss << "Found: " << keyValuePairs.size() << " parameter" << (keyValuePairs.size() > 1 ? "s." : ".");
    }
    this->QueueCommandResponse(PLUS_SUCCESS, oss.str(), "", &keyValuePairs);
    return PLUS_SUCCESS;
  }
  else if (STRCASECMP(this->Name, REQUEST_INPUT_DEVICE_IDS_CMD) == 0)
  {
    if (this->DeviceId == nullptr)
    {
      this->QueueCommandResponse(PLUS_FAIL, "Command failed, see error message.", "Unknown device id requested.");
      return PLUS_FAIL;
    }

    std::vector<vtkPlusDevice*> list;
    auto device = _FindDevice(aCollection);
    if (device == nullptr)
    {
      this->QueueCommandResponse(PLUS_FAIL, "Command failed, see error message.", std::string("Device with id: ") + this->DeviceId + " not found.");
      return PLUS_FAIL;
    }

    if (device->GetInputDevicesRecursive(list) == PLUS_FAIL)
    {
      this->QueueCommandResponse(PLUS_FAIL, "Command failed, see error message.", "Error retrieving input devices.");
      return PLUS_FAIL;;
    }
    list.insert(list.begin(), device);

    std::ostringstream oss;
    for (auto device : list)
    {
      oss << device->GetDeviceId();
      oss << ",";
    }

    this->QueueCommandResponse(PLUS_SUCCESS, oss.str(), "", nullptr);
    return PLUS_SUCCESS;
  }
  else if (STRCASECMP(this->Name, REQUEST_DEVICE_CHANNEL_IDS_CMD) == 0)
  {
    auto device = _FindDevice(aCollection);
    if (device == nullptr)
    {
      this->QueueCommandResponse(PLUS_FAIL, "Command failed, see error message.", std::string("Device with id: ") + this->DeviceId + " not found.");
      return PLUS_FAIL;
    }

    std::ostringstream oss;
    for (auto chanIter = device->GetOutputChannelsStart(); chanIter != device->GetOutputChannelsEnd(); ++chanIter)
    {
      oss << (*chanIter)->GetChannelId();
      oss << ",";
    }

    this->QueueCommandResponse(PLUS_SUCCESS, oss.str(), "", nullptr);
    return PLUS_SUCCESS;
  }

  this->QueueCommandResponse(PLUS_FAIL, "Command failed, see error message.", "Unknown command.");
  return PLUS_FAIL;
}