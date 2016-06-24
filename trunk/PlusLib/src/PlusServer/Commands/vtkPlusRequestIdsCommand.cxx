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

vtkStandardNewMacro( vtkPlusRequestIdsCommand );

static const char REQUEST_CHANNEL_ID_CMD[]="RequestChannelIds";
static const char REQUEST_DEVICE_ID_CMD[]="RequestDeviceIds";

//----------------------------------------------------------------------------
vtkPlusRequestIdsCommand::vtkPlusRequestIdsCommand()
  : DeviceType(NULL)
{
}

//----------------------------------------------------------------------------
vtkPlusRequestIdsCommand::~vtkPlusRequestIdsCommand()
{
}

//----------------------------------------------------------------------------
void vtkPlusRequestIdsCommand::SetNameToRequestChannelIds()
{
  SetName(REQUEST_CHANNEL_ID_CMD);
}
void vtkPlusRequestIdsCommand::SetNameToRequestDeviceIds()
{
  SetName(REQUEST_DEVICE_ID_CMD);
}

//----------------------------------------------------------------------------
void vtkPlusRequestIdsCommand::GetCommandNames(std::list<std::string> &cmdNames)
{
  cmdNames.clear();
  cmdNames.push_back(REQUEST_CHANNEL_ID_CMD);
  cmdNames.push_back(REQUEST_DEVICE_ID_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusRequestIdsCommand::GetDescription(const char* commandName)
{
  std::string desc;
  if (commandName == NULL || STRCASECMP(commandName, REQUEST_CHANNEL_ID_CMD) )
  {
    desc += REQUEST_CHANNEL_ID_CMD;
    desc += ": Request the list of channels for all devices.";
  }
  if (commandName == NULL || STRCASECMP(commandName, REQUEST_DEVICE_ID_CMD) )
  {
    desc += REQUEST_DEVICE_ID_CMD;
    desc += ": Request the list of devices. Attributes: DeviceType: restrict the returned list of devices to a specific type (VirtualDiscCapture, VirtualVolumeReconstructor, etc.)";
  }
  return desc;
}

//----------------------------------------------------------------------------
void vtkPlusRequestIdsCommand::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusRequestIdsCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  this->SetDeviceType(aConfig->GetAttribute("DeviceType"));
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusRequestIdsCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::WriteConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
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
  if( dataCollector->GetDevices(aCollection) != PLUS_SUCCESS )
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed, see error message.", "Unable to retrieve devices.");
    return PLUS_FAIL;
  }

  if (STRCASECMP(this->Name, REQUEST_CHANNEL_ID_CMD) == 0)
  {
    std::string responseMessage;
    bool addSeparator = false;
    std::map<std::string, std::string> keyValuePairs;

    for( DeviceCollectionConstIterator deviceIt = aCollection.begin(); deviceIt != aCollection.end(); ++deviceIt)
    {
      vtkPlusDevice* aDevice = *deviceIt;
      if( aDevice->OutputChannelCount() > 0 )
      {
        for( ChannelContainerConstIterator it = aDevice->GetOutputChannelsStart(); it != aDevice->GetOutputChannelsEnd(); ++it )
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
    if( this->CommandProcessor->GetPlusServer()->GetClientInfo(this->GetClientId(), info) != PLUS_SUCCESS )
    {
      LOG_WARNING("Unable to locate client data for client id: " << this->GetClientId());
    }
    if( info.ClientHeaderVersion <= IGTL_HEADER_VERSION_2 )
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
  else if (STRCASECMP(this->Name, REQUEST_DEVICE_ID_CMD) == 0)
  {
    std::string responseMessage;
    bool addSeparator = false;
    std::map<std::string, std::string> keyValuePairs;

    for( DeviceCollectionConstIterator deviceIt = aCollection.begin(); deviceIt != aCollection.end(); ++deviceIt)
    {
      vtkPlusDevice* aDevice = *deviceIt;
      if (aDevice==NULL)
      {
        continue;
      }
      std::string deviceClassName;
      if (this->DeviceType!=NULL)
      {
        // Translate requested device ID (factory name) to c++ class name
        vtkSmartPointer<vtkPlusDeviceFactory> factory = vtkSmartPointer<vtkPlusDeviceFactory>::New();
        if( factory->GetDeviceClassName(this->DeviceType, deviceClassName) != PLUS_SUCCESS )
        {
          this->QueueCommandResponse(PLUS_FAIL, "Command failed, see error message.", "Unknown device type requested.");
          return PLUS_FAIL;
        }
      }
      if (deviceClassName.empty() || aDevice->GetClassName()==deviceClassName)
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
    if( this->CommandProcessor->GetPlusServer()->GetClientInfo(this->GetClientId(), info) != PLUS_SUCCESS )
    {
      LOG_WARNING("Unable to locate client data for client id: " << this->GetClientId());
    }
    if( info.ClientHeaderVersion <= IGTL_HEADER_VERSION_2 )
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

  this->QueueCommandResponse(PLUS_FAIL, "Command failed, see error message.", "Unknown command.");
  return PLUS_FAIL;
}
