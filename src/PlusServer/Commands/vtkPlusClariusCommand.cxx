/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusDataCollector.h"
#include "vtkPlusClariusCommand.h"
#include "Clarius/vtkPlusClarius.h"

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

static const std::string SAVE_CLARIUS_RAW_DATA_CMD = "SaveRawData";

vtkStandardNewMacro(vtkPlusClariusCommand);

//----------------------------------------------------------------------------
vtkPlusClariusCommand::vtkPlusClariusCommand()
  : CompressRawData(false)
  , RawDataLastNSeconds(-1.0)
{
}

//----------------------------------------------------------------------------
vtkPlusClariusCommand::~vtkPlusClariusCommand()
{
}

//----------------------------------------------------------------------------
void vtkPlusClariusCommand::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkPlusClariusCommand::GetCommandNames(std::list<std::string>& cmdNames)
{
  cmdNames.clear();
  cmdNames.push_back(SAVE_CLARIUS_RAW_DATA_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusClariusCommand::GetDescription(const std::string& commandName)
{
  std::string desc;
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName,  SAVE_CLARIUS_RAW_DATA_CMD))
  {
    desc += SAVE_CLARIUS_RAW_DATA_CMD;
    desc += ": Acquire the raw data from the Clarius. The data will be saved in the output directory as a .tar";
  }
  return desc;
}

//----------------------------------------------------------------------------
void vtkPlusClariusCommand::SetNameToSaveRawData()
{
  this->SetName(SAVE_CLARIUS_RAW_DATA_CMD);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusClariusCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(CompressRawData, aConfig);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(ClariusDeviceId, aConfig);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(OutputFilename, aConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, RawDataLastNSeconds, aConfig); 

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusClariusCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::WriteConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  XML_WRITE_BOOL_ATTRIBUTE(CompressRawData, aConfig);
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(ClariusDeviceId, aConfig);
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(OutputFilename, aConfig);
  std::stringstream ss;
  ss << RawDataLastNSeconds;
  std::string string = ss.str();
  aConfig->SetAttribute("RawDataLastNSeconds", string.c_str());
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusClariusCommand::Execute()
{
  LOG_DEBUG("vtkPlusClariusCommand::Execute: " << (!this->Name.empty() ? this->Name : "(undefined)")
            << ", device: " << (this->ClariusDeviceId.empty() ? "(undefined)" : this->ClariusDeviceId));

  if (this->Name.empty())
  {
    this->QueueCommandResponse(PLUS_FAIL, "Clarius command failed, no command name specified");
    return PLUS_FAIL;
  }

  vtkPlusClarius* clariusDevice = this->GetClariusDevice();
  if (clariusDevice == NULL)
  {
    this->QueueCommandResponse(PLUS_FAIL, std::string("Clarius command failed: device ")
                               + (this->ClariusDeviceId.empty() ? "(undefined)" : this->ClariusDeviceId) + " is not found");
    return PLUS_FAIL;
  }

  if (igsioCommon::IsEqualInsensitive(this->Name, SAVE_CLARIUS_RAW_DATA_CMD))
  {
    LOG_INFO("Acquiring the raw data from Clarius: Device ID: " << this->GetClariusDeviceId());

    if (!this->GetOutputFilename().empty())
    {
      clariusDevice->SetRawDataOutputFilename(this->GetOutputFilename());
    }

    this->QueueCommandResponse(PLUS_SUCCESS, "Clarius: Raw data request received");
    clariusDevice->SetCompressRawData(this->GetCompressRawData());
    return clariusDevice->RequestLastNSecondsRawData(this->GetRawDataLastNSeconds());
    ;
  }
  this->QueueCommandResponse(PLUS_FAIL, "vtkPlusClariusCommand::Execute: failed, unknown command name: " + this->Name);
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
vtkPlusClarius* vtkPlusClariusCommand::GetClariusDevice()
{
  vtkPlusDataCollector* dataCollector = GetDataCollector();
  if (dataCollector == NULL)
  {
    LOG_ERROR("Data collector is invalid");
    return NULL;
  }
  if (!GetClariusDeviceId().empty())
  {
    // Reconstructor device ID is specified
    vtkPlusDevice* device = NULL;
    if (dataCollector->GetDevice(device, GetClariusDeviceId()) != PLUS_SUCCESS)
    {
      LOG_ERROR("No Clarius device has been found by the name " << this->GetClariusDeviceId());
      return NULL;
    }
    // device found
    vtkPlusClarius* ClariusDevice = vtkPlusClarius::SafeDownCast(device);
    if (ClariusDevice == NULL)
    {
      // wrong type
      LOG_ERROR("The specified device " << GetClariusDeviceId() << " is not Clarius Device");
      return NULL;
    }
    return ClariusDevice;
  }
  else
  {
    // No Clarius device id is specified, auto-detect the first one and use that
    for (DeviceCollectionConstIterator it = dataCollector->GetDeviceConstIteratorBegin(); it != dataCollector->GetDeviceConstIteratorEnd(); ++it)
    {
      vtkPlusClarius* ClariusDevice = vtkPlusClarius::SafeDownCast(*it);
      if (ClariusDevice != NULL)
      {
        // found a recording device
        SetClariusDeviceId(ClariusDevice->GetDeviceId());
        return ClariusDevice;
      }
    }
    LOG_ERROR("No Clarius Device has been found");
    return NULL;
  }
}
