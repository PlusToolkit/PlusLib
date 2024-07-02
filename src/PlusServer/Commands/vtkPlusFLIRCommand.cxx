/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusDataCollector.h"
#include "vtkPlusFLIRCommand.h"
#include "vtkPlusFLIRSpinnakerCam.h"
#include "vtkPlusUsImagingParameters.h"

#include "vtkImageData.h"
#include "vtkDICOMImageReader.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusCommandProcessor.h"
#include "vtkIGSIOTrackedFrameList.h"
#include "vtkPlusVolumeReconstructor.h"
#include "vtkPlusVirtualVolumeReconstructor.h"
#include <vtkImageFlip.h>
#include <vtkPointData.h>
#include <vtkDirectory.h>


vtkStandardNewMacro(vtkPlusFLIRCommand);

namespace
{
  static const std::string FLIR_COMMAND = "FLIRCommand";
  static const std::string FLIR_AUTO_FOCUS = "AutoFocus";
}

//----------------------------------------------------------------------------
vtkPlusFLIRCommand::vtkPlusFLIRCommand()
  : ResponseExpected(true)
{
  // It handles only one command, set its name by default
  this->SetName(FLIR_COMMAND);
}

//----------------------------------------------------------------------------
vtkPlusFLIRCommand::~vtkPlusFLIRCommand()
{
}

//----------------------------------------------------------------------------
void vtkPlusFLIRCommand::GetCommandNames(std::list<std::string>& cmdNames)
{
  cmdNames.clear();
  cmdNames.push_back(FLIR_COMMAND);
}

//----------------------------------------------------------------------------
std::string vtkPlusFLIRCommand::GetDescription(const std::string& commandName)
{
  std::string desc;
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, FLIR_COMMAND))
  {
    desc += FLIR_COMMAND;
    desc += ": Send commands to FLIR device.";
  }
  return desc;
}

//----------------------------------------------------------------------------
std::string vtkPlusFLIRCommand::GetDeviceId() const
{
  return this->DeviceId;
}

//----------------------------------------------------------------------------
void vtkPlusFLIRCommand::SetDeviceId(const std::string& deviceId)
{
  this->DeviceId = deviceId;
}

//----------------------------------------------------------------------------
std::string vtkPlusFLIRCommand::GetCommandName() const
{
  return this->CommandName;
}

//----------------------------------------------------------------------------
void vtkPlusFLIRCommand::SetCommandName(const std::string& text)
{
  this->CommandName = text;
}

//----------------------------------------------------------------------------
std::string vtkPlusFLIRCommand::GetCommandValue() const
{
  return this->CommandValue;
}

//----------------------------------------------------------------------------
void vtkPlusFLIRCommand::SetCommandValue(const std::string& text)
{
  this->CommandValue = text;
}

//----------------------------------------------------------------------------
void vtkPlusFLIRCommand::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusFLIRCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  LOG_DEBUG("vtkPlusFLIRCommand::ReadConfiguration");

  this->RequestedParameterChanges.clear();
  if (vtkPlusCommand::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  if (vtkPlusCommand::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  XML_READ_STRING_ATTRIBUTE_OPTIONAL(DeviceId, aConfig);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(CommandName, aConfig);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(CommandValue, aConfig);
  
  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkPlusFLIRCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::WriteConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(DeviceId, aConfig);
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(CommandName, aConfig);
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(CommandValue, aConfig);

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkPlusFLIRCommand::Execute()
{
  PlusStatus status = PLUS_SUCCESS;

  LOG_DEBUG("vtkPlusFLIRCommand::Execute: " << (!this->CommandName.empty() ? this->CommandName : "(undefined)")
    << ", device: " << (this->DeviceId.empty() ? "(undefined)" : this->DeviceId)
    << ", value: " << (this->CommandValue.empty() ? "(undefined)" : this->CommandValue));

  vtkPlusDataCollector* dataCollector = GetDataCollector();
  if (dataCollector == NULL)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", "Invalid data collector.");
    return PLUS_FAIL;
  }

  // Get device pointer
  if (this->DeviceId.empty())
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", "No DeviceId specified.");
    return PLUS_FAIL;
  }
  vtkPlusDevice* aDevice = NULL;
  if (dataCollector->GetDevice(aDevice, this->DeviceId) != PLUS_SUCCESS)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", std::string("Device ")
      + (this->DeviceId.empty() ? "(undefined)" : this->DeviceId) + std::string(" is not found."));
    return PLUS_FAIL;
  }
  vtkPlusFLIRSpinnakerCam* device = dynamic_cast<vtkPlusFLIRSpinnakerCam*>(aDevice);

  // Send text (and receive response)
  std::string textToSend;
  std::string response;
  if (!this->GetCommandValue().empty())
  {
    textToSend = GetCommandValue();
  }

  
  //  if (igsioCommon::IsEqualInsensitive(this->GetCommandName(), FLIR_COMMAND))
  if (igsioCommon::IsEqualInsensitive(this->GetCommandName(), FLIR_AUTO_FOCUS))
  {
    // response = device->GetRTS() ? "True" : "False";
    device->AutoFocus();
    LOG_DEBUG("vtkPlusFLIRCommand::vtkPlusFLIRDevice::FLIR_COMMAND");
    status = PLUS_SUCCESS;
  }
  else
  {
    status = PLUS_FAIL;
  }
  if (status != PLUS_SUCCESS)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", std::string("Failed to execute command '") + GetCommandName() + "'"
      + " on device " + (this->DeviceId.empty() ? "(undefined)" : this->DeviceId));
    return PLUS_FAIL;
  }
  this->QueueCommandResponse(PLUS_SUCCESS, response);
  return status;
}
