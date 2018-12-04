/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusAddRecordingDeviceCommand.h"
#include "vtkPlusChannel.h"
#include "vtkPlusCommandProcessor.h"
#include "vtkPlusDataCollector.h"
#include "vtkPlusDevice.h"
#include "vtkPlusDeviceFactory.h"
#include "vtkPlusOpenIGTLinkServer.h"
#include "vtkPlusVirtualCapture.h"

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusAddRecordingDeviceCommand);

//----------------------------------------------------------------------------

namespace
{
  static const std::string ADD_RECORDING_DEVICE_CMD = "AddRecordingDevice";
}

//----------------------------------------------------------------------------
vtkPlusAddRecordingDeviceCommand::vtkPlusAddRecordingDeviceCommand()
{
  // It handles only one command, set its name by default
  this->SetName(ADD_RECORDING_DEVICE_CMD);
}

//----------------------------------------------------------------------------
vtkPlusAddRecordingDeviceCommand::~vtkPlusAddRecordingDeviceCommand()
{

}

//----------------------------------------------------------------------------
void vtkPlusAddRecordingDeviceCommand::SetNameToAddRecordingDevice()
{
  this->SetName(ADD_RECORDING_DEVICE_CMD);
}

//----------------------------------------------------------------------------
void vtkPlusAddRecordingDeviceCommand::GetCommandNames(std::list<std::string>& cmdNames)
{
  cmdNames.clear();
  cmdNames.push_back(ADD_RECORDING_DEVICE_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusAddRecordingDeviceCommand::GetDescription(const std::string& commandName)
{
  std::string desc;
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, ADD_RECORDING_DEVICE_CMD))
  {
    desc += ADD_RECORDING_DEVICE_CMD;
    desc += ": Add a virtual capture device with inputs defined by metadata \"InputChannels\".";
  }
  return desc;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAddRecordingDeviceCommand::Execute()
{
  igtl::MessageBase::MetaDataMap::iterator it = this->MetaData.find("InputChannels");
  if (it == end(this->MetaData) || it->second.second.empty())
  {
    LOG_ERROR("Input channels not defined or empty, nothing to record.");
    this->QueueCommandResponse(PLUS_FAIL, "Input channels not defined or empty.", "Please specify \"InputChannels\" metadata argument as a \"Separator\" separated list.");
    return PLUS_FAIL;
  }

  // Parse InputChannels to see if it's valid
  char separator = ';';
  if (this->MetaData.find("Separator") != end(this->MetaData) && !this->MetaData["Separator"].second.empty())
  {
    // Grab first character of string
    separator = this->MetaData["Separator"].second[0];
  }

  std::string inputChannels = it->second.second;
  std::vector<std::string> tokens = igsioCommon::SplitStringIntoTokens(inputChannels, separator, false);
  if (tokens.size() == 0)
  {
    LOG_ERROR("Input channels ill defined, nothing to record.");
    this->QueueCommandResponse(PLUS_FAIL, "Input channels ill defined.", "Please specify \"InputChannels\" metadata argument as a \"Separator\" separated list.");
    return PLUS_FAIL;
  }

  std::vector<vtkPlusChannel*> channels;
  for (std::vector<std::string>::iterator it = begin(tokens); it != end(tokens); ++it)
  {
    vtkPlusChannel* channel(nullptr);
    if (this->GetDataCollector()->GetChannel(channel, *it) == PLUS_FAIL)
    {
      LOG_ERROR("Input channel " << *it << " not found. Aborting.");
      this->QueueCommandResponse(PLUS_FAIL, "Input channel " + *it + " not found.", "Please use \"RequestIds\" command to retrieve list of current channels.");
      return PLUS_FAIL;
    }
    channels.push_back(channel);
  }

  // Check for a given ID, if not create a unique one
  std::string deviceId;
  if (this->MetaData.find("Id") == end(this->MetaData) || this->MetaData["Id"].second.empty())
  {
    int counter = 0;
    while (true)
    {
      std::string candidateName = "VirtualCapure[" + igsioCommon::ToString<int>(counter++) + "]";
      vtkPlusDevice* device(nullptr);
      if (this->GetDataCollector()->GetDevice(device, candidateName) == PLUS_FAIL)
      {
        deviceId = candidateName;
        break;
      }
    }
  }
  else
  {
    deviceId = this->MetaData["Id"].second;
  }

  vtkPlusDevice* device(nullptr);
  if (this->GetDataCollector()->GetDeviceFactory().CreateInstance("VirtualCapture", device, deviceId) == PLUS_FAIL)
  {
    LOG_ERROR("Unable to create device of type \"VirtualCapture\" with ID: " << deviceId);
    this->QueueCommandResponse(PLUS_FAIL, "Device creation failed.", "See server log for error details.");
    return PLUS_FAIL;
  }

  // Configure device
  vtkPlusVirtualCapture* captureDevice = dynamic_cast<vtkPlusVirtualCapture*>(device);

  // Configuration options
  std::string baseFilename("GeneratedVirtualCapture.nrrd");
  if (this->MetaData.find("BaseFilename") == end(this->MetaData) || this->MetaData["BaseFilename"].second.empty())
  {
    baseFilename = this->MetaData["BaseFilename"].second;
    if (vtksys::SystemTools::GetFilenameExtension(baseFilename) == "")
    {
      baseFilename = baseFilename + ".nrrd";
    }
  }
  captureDevice->SetBaseFilename(baseFilename);

  bool enableFileCompression(true);
  if (this->MetaData.find("EnableFileCompression") == end(this->MetaData) || this->MetaData["EnableFileCompression"].second.empty())
  {
    enableFileCompression = igsioCommon::IsEqualInsensitive(this->MetaData["EnableFileCompression"].second, "TRUE");
  }
  captureDevice->SetEnableFileCompression(enableFileCompression);


  bool enableCapturingOnStart(true);
  if (this->MetaData.find("EnableCapturingOnStart") == end(this->MetaData) || this->MetaData["EnableCapturingOnStart"].second.empty())
  {
    enableCapturingOnStart = igsioCommon::IsEqualInsensitive(this->MetaData["EnableCapturingOnStart"].second, "TRUE");
  }
  captureDevice->SetEnableCapturing(enableCapturingOnStart);

  if (this->MetaData.find("RequestedFrameRate") == end(this->MetaData) || this->MetaData["RequestedFrameRate"].second.empty())
  {
    std::stringstream ss;
    ss << this->MetaData["RequestedFrameRate"].second;
    try
    {
      int requestedFrameRate(0);
      ss >> requestedFrameRate;
      captureDevice->SetRequestedFrameRate(requestedFrameRate);
    }
    catch (...) {}
  }

  int frameBufferSize(0);
  if (this->MetaData.find("FrameBufferSize") == end(this->MetaData) || this->MetaData["FrameBufferSize"].second.empty())
  {
    std::stringstream ss;
    ss << this->MetaData["FrameBufferSize"].second;
    try
    {
      ss >> frameBufferSize;
      captureDevice->SetFrameBufferSize(frameBufferSize);
    }
    catch (...) {}
  }

  for (std::vector<vtkPlusChannel*>::iterator it = begin(channels); it != end(channels); ++it)
  {
    captureDevice->AddInputChannel(*it);
  }

  if (this->GetDataCollector()->AddDevice(captureDevice) == PLUS_FAIL)
  {
    LOG_ERROR("Unable to add capture device.");
    this->QueueCommandResponse(PLUS_FAIL, "Unable to add capture device.", "See server log for error details.");
    return PLUS_FAIL;
  }

  this->QueueCommandResponse(PLUS_SUCCESS, "Success.", "");
  return PLUS_SUCCESS;
}
