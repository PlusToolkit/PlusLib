/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtkPlusCommandProcessor.h"
#include "vtkPlusConoProbeLinkCommand.h"
#include "vtkPlusOptimetConoProbeMeasurer.h"
#include "vtkPlusDataCollector.h"

namespace
{
  static const std::string SHOW_CMD = "ShowProbeDialog";
}

vtkStandardNewMacro(vtkPlusConoProbeLinkCommand);

//----------------------------------------------------------------------------
vtkPlusConoProbeLinkCommand::vtkPlusConoProbeLinkCommand()
{
}

//----------------------------------------------------------------------------
vtkPlusConoProbeLinkCommand::~vtkPlusConoProbeLinkCommand()
{
}

//----------------------------------------------------------------------------
void vtkPlusConoProbeLinkCommand::SetNameToShow()
{
  this->SetName(SHOW_CMD);
}

//----------------------------------------------------------------------------
void vtkPlusConoProbeLinkCommand::GetCommandNames(std::list<std::string>& cmdNames)
{
  cmdNames.clear();
  cmdNames.push_back(SHOW_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusConoProbeLinkCommand::GetDescription(const std::string& commandName)
{
  std::string desc;
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, SHOW_CMD))
  {
    desc += SHOW_CMD;
    desc += ": Opens the Probe Dialog part of the Optimet Smart32 SDK. ConoProbeDeviceId: ID of the ConoProbe device.";
  }
  return desc;
}

//----------------------------------------------------------------------------
void vtkPlusConoProbeLinkCommand::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusConoProbeLinkCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  XML_READ_STRING_ATTRIBUTE_REQUIRED(ConoProbeDeviceId, aConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusConoProbeLinkCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::WriteConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  XML_WRITE_STRING_ATTRIBUTE_REMOVE_IF_EMPTY(ConoProbeDeviceId, aConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
vtkPlusOptimetConoProbeMeasurer* vtkPlusConoProbeLinkCommand::GetConoProbeDevice(const std::string& conoProbeDeviceID)
{
  vtkPlusDataCollector* dataCollector = GetDataCollector();
  if (dataCollector == NULL)
  {
    LOG_ERROR("Data collector is invalid");
    return NULL;
  }
  vtkPlusOptimetConoProbeMeasurer* conoProbeDevice = NULL;
  if (!conoProbeDeviceID.empty())
  {
    // ConoProbe device ID is specified
    vtkPlusDevice* device = NULL;
    if (dataCollector->GetDevice(device, conoProbeDeviceID) != PLUS_SUCCESS)
    {
      LOG_ERROR("No OptimetConoProbe has been found by the name " << conoProbeDeviceID);
      return NULL;
    }
    // device found
    conoProbeDevice = vtkPlusOptimetConoProbeMeasurer::SafeDownCast(device);
    if (conoProbeDevice == NULL)
    {
      // wrong type
      LOG_ERROR("The specified device " << conoProbeDeviceID << " is not VirtualStreamCapture");
      return NULL;
    }
  }
  else
  {
    // No ConoProbe device id is specified, auto-detect the first one and use that
    for (DeviceCollectionConstIterator it = dataCollector->GetDeviceConstIteratorBegin(); it != dataCollector->GetDeviceConstIteratorEnd(); ++it)
    {
      conoProbeDevice = vtkPlusOptimetConoProbeMeasurer::SafeDownCast(*it);
      if (conoProbeDevice != NULL)
      {
        // found a recording device
        break;
      }
    }
    if (conoProbeDevice == NULL)
    {
      LOG_ERROR("No OptimetConoProbe has been found");
      return NULL;
    }
  }
  return conoProbeDevice;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusConoProbeLinkCommand::Execute()
{
  LOG_INFO("vtkPlusConoProbeLinkCommand::Execute:");

  if (this->Name.empty())
  {
    this->QueueStringResponse("Command failed, no command name specified", PLUS_FAIL);
    return PLUS_FAIL;
  }

  vtkPlusOptimetConoProbeMeasurer* conoProbeDevice = GetConoProbeDevice(this->ConoProbeDeviceId);
  if (conoProbeDevice == NULL)
  {
    this->QueueStringResponse(std::string("OptimetConoProbe has not been found (")
                              + (!this->ConoProbeDeviceId.empty() ? this->ConoProbeDeviceId : "auto-detect") + "), " + this->Name + " failed", PLUS_FAIL);
    return PLUS_FAIL;
  }

  std::string responseMessageBase = std::string("OptimetConoProbe (") + conoProbeDevice->GetDeviceId() + ") " + this->Name;
  LOG_INFO("vtkPlusConoProbeLinkCommand::Execute: " << this->Name);
  if (igsioCommon::IsEqualInsensitive(this->Name, SHOW_CMD))
  {
    if (conoProbeDevice->ShowProbeDialog())
    {
      this->QueueStringResponse(responseMessageBase + " successful", PLUS_SUCCESS);
      return PLUS_SUCCESS;
    }
    this->QueueStringResponse(responseMessageBase + " failed", PLUS_FAIL);
    return PLUS_FAIL;
  }

  this->QueueStringResponse(responseMessageBase + " unknown command: " + this->Name, PLUS_FAIL);
  return PLUS_FAIL;
}