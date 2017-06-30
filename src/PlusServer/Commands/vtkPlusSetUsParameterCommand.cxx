/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "vtkPlusSetUsParameterCommand.h"

#include "PlusConfigure.h"
#include "vtkPlusDataCollector.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusCommandProcessor.h"
#include "vtkPlusUsImagingParameters.h"
#include "vtkPlusUsDevice.h"

#include <vtkVariant.h>
#include <vtkSmartPointer.h>
#include <limits>

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
  if (commandName.empty() || PlusCommon::IsEqualInsensitive(commandName, SET_US_PARAMETER_CMD))
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
  for (int elemIndex=0; elemIndex<aConfig->GetNumberOfNestedElements(); ++elemIndex)
  {
    vtkXMLDataElement* currentElem = aConfig->GetNestedElement(elemIndex);
    if (PlusCommon::IsEqualInsensitive(currentElem->GetName(), "Parameter"))
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
  for (paramIt=this->RequestedParameterChanges.begin(); paramIt!=this->RequestedParameterChanges.end(); ++paramIt)
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
            << ", device: " << (this->UsDeviceId.empty() ? "(undefined)" : this->UsDeviceId) );

  if (this->Name.empty())
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", "No command name specified.");
    return PLUS_FAIL;
  }
  else if (!PlusCommon::IsEqualInsensitive(this->Name, SET_US_PARAMETER_CMD))
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
  vtkPlusUsImagingParameters* imagingParameters = usDevice->GetCurrentImagingParameters();
  std::string message = this->Name + std::string("(") + usDeviceId + std::string(")");
  PlusStatus status = PLUS_SUCCESS;

  std::map<std::string, std::string>::iterator paramIt;
  for (paramIt=this->RequestedParameterChanges.begin(); paramIt!=this->RequestedParameterChanges.end(); ++paramIt)
  {
    message += " Parameter " + paramIt->first + "=" + paramIt->second + ": ";

    if (PlusCommon::IsEqualInsensitive(paramIt->first, vtkPlusUsImagingParameters::KEY_DEPTH))
    {
      bool valid=false;
      double depth = vtkVariant(paramIt->second).ToDouble(&valid);
      imagingParameters->SetDepthMm(depth);
      if (valid == false)
      {
        message += "Failed to parse";
        status = PLUS_FAIL;
        continue;
      }
      if (usDevice->SetNewImagingParameters(*imagingParameters) == PLUS_FAIL)
      {
        message += "Failed to set";
        status = PLUS_FAIL;
        continue;
      }
      message += "Success";
    }
    else if (PlusCommon::IsEqualInsensitive(paramIt->first, vtkPlusUsImagingParameters::KEY_FREQUENCY))
    {
      bool valid=false;
      double frequency = vtkVariant(paramIt->second).ToDouble(&valid);
      imagingParameters->SetFrequencyMhz(frequency);
      if (valid == false)
      {
        message += "Failed to parse";
        status = PLUS_FAIL;
        continue;
      }
      if (usDevice->SetNewImagingParameters(*imagingParameters) == PLUS_FAIL)
      {
        message += "Failed to set";
        status = PLUS_FAIL;
        continue;
      }
      message += "Success";
    }
  } // For each parameter

  this->QueueCommandResponse(status, "Command " + std::string(status==PLUS_SUCCESS ? "succeeded" : "failed. See error message."), message);
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
