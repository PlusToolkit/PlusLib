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
#include <limits>

namespace
{
  static const double UNDEFINED_VALUE = std::numeric_limits<double>::max();
  static const std::string SET_DEPTH_CMD = std::string("Set") + std::string(vtkPlusUsImagingParameters::KEY_DEPTH);
  static const std::string SET_FREQUENCY_CMD = std::string("Set") + std::string(vtkPlusUsImagingParameters::KEY_FREQUENCY);
}

vtkStandardNewMacro(vtkPlusSetUsParameterCommand);

//----------------------------------------------------------------------------
vtkPlusSetUsParameterCommand::vtkPlusSetUsParameterCommand()
  : ParameterDoubleValue(UNDEFINED_VALUE)
{
  this->UsDeviceId = "";
}

//----------------------------------------------------------------------------
vtkPlusSetUsParameterCommand::~vtkPlusSetUsParameterCommand()
{
}

//----------------------------------------------------------------------------
void vtkPlusSetUsParameterCommand::SetNameToSetDepth()
{
  SetName(SET_DEPTH_CMD);
}
void vtkPlusSetUsParameterCommand::SetNameToSetFrequency()
{
  SetName(SET_FREQUENCY_CMD);
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
  cmdNames.push_back(SET_DEPTH_CMD);
  cmdNames.push_back(SET_FREQUENCY_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusSetUsParameterCommand::GetDescription(const std::string& commandName)
{
  std::string desc;
  if (commandName.empty() || PlusCommon::IsEqualInsensitive(commandName, SET_DEPTH_CMD))
  {
    desc += SET_DEPTH_CMD;
    desc += ": Set depth image parameter. Attributes: UsDeviceId: ID of the ultrasound device.";
  }
  if (commandName.empty() || PlusCommon::IsEqualInsensitive(commandName, SET_FREQUENCY_CMD))
  {
    desc += SET_FREQUENCY_CMD;
    desc += ": Set frequency image parameter. Attributes: UsDeviceId: ID of the ultrasound device.";
  }

  return desc;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSetUsParameterCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, ParameterDoubleValue, aConfig);

  this->SetUsDeviceId(aConfig->GetAttribute("UsDeviceId"));

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSetUsParameterCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::WriteConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  if (this->ParameterDoubleValue != UNDEFINED_VALUE)
  {
    aConfig->SetDoubleAttribute("ParameterDoubleValue", ParameterDoubleValue);
  }

  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(UsDeviceId, aConfig);

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

  vtkPlusUsDevice* usDevice = GetUsDevice();
  if (usDevice == NULL)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", std::string("Device ")
                               + (this->UsDeviceId.empty() ? "(undefined)" : this->UsDeviceId) + std::string(" is not found."));
    return PLUS_FAIL;
  }

  std::string usDeviceId = (usDevice->GetDeviceId().empty() ? "(unknown)" : usDevice->GetDeviceId());

  std::string baseMessage = this->Name + std::string("(") + usDeviceId + std::string(")");
  if (PlusCommon::IsEqualInsensitive(this->Name, SET_DEPTH_CMD))
  {
    if (this->ParameterDoubleValue == UNDEFINED_VALUE)
    {
      this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", baseMessage + " failed: undefined parameter value");
      return PLUS_FAIL;
    }

    vtkPlusUsImagingParameters* imagingParameters = usDevice->GetCurrentImagingParameters();
    imagingParameters->SetDepthMm(this->ParameterDoubleValue);
    if (usDevice->SetNewImagingParameters(*imagingParameters) == PLUS_FAIL)
    {
      this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", baseMessage + " failed: unable to set new parameter to ultrasound device");
      return PLUS_FAIL;
    }

    this->QueueCommandResponse(PLUS_SUCCESS, std::string("Request successfully sent to change depth to ") + vtkVariant(ParameterDoubleValue).ToString());
    return PLUS_SUCCESS;
  }
  else if (PlusCommon::IsEqualInsensitive(this->Name, SET_FREQUENCY_CMD))
  {
    if (this->ParameterDoubleValue == UNDEFINED_VALUE)
    {
      this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", baseMessage + " failed: undefined parameter value");
      return PLUS_FAIL;
    }

    vtkPlusUsImagingParameters* imagingParameters = usDevice->GetCurrentImagingParameters();
    imagingParameters->SetFrequencyMhz(this->ParameterDoubleValue);
    if (usDevice->SetNewImagingParameters(*imagingParameters) == PLUS_FAIL)
    {
      this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", baseMessage + " failed: unable to set new parameter to ultrasound device");
      return PLUS_FAIL;
    }

    this->QueueCommandResponse(PLUS_SUCCESS, std::string("Request successfully sent to change frequency to ") + vtkVariant(ParameterDoubleValue).ToString());
    return PLUS_SUCCESS;
  }

  this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", baseMessage + "Unknown command name: " + this->Name + ".");
  return PLUS_FAIL;
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
