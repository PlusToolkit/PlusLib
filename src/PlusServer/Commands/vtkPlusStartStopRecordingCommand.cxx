/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusChannel.h"
#include "vtkPlusCommandProcessor.h"
#include "vtkPlusDataCollector.h"
#include "vtkPlusStartStopRecordingCommand.h"
#include "vtkPlusVirtualCapture.h"
#include "vtkPlusDeviceFactory.h"

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusStartStopRecordingCommand);

//----------------------------------------------------------------------------

namespace
{
  static const std::string START_CMD = "StartRecording";
  static const std::string SUSPEND_CMD = "SuspendRecording";
  static const std::string RESUME_CMD = "ResumeRecording";
  static const std::string STOP_CMD = "StopRecording";
}

//----------------------------------------------------------------------------
vtkPlusStartStopRecordingCommand::vtkPlusStartStopRecordingCommand()
  : EnableCompression(false)
  , CodecFourCC("")
{
}

//----------------------------------------------------------------------------
vtkPlusStartStopRecordingCommand::~vtkPlusStartStopRecordingCommand()
{
}

//----------------------------------------------------------------------------
void vtkPlusStartStopRecordingCommand::SetNameToStart() { SetName(START_CMD); }
void vtkPlusStartStopRecordingCommand::SetNameToSuspend() { SetName(SUSPEND_CMD); }
void vtkPlusStartStopRecordingCommand::SetNameToResume() { SetName(RESUME_CMD); }
void vtkPlusStartStopRecordingCommand::SetNameToStop() { SetName(STOP_CMD); }

//----------------------------------------------------------------------------
void vtkPlusStartStopRecordingCommand::GetCommandNames(std::list<std::string>& cmdNames)
{
  cmdNames.clear();
  cmdNames.push_back(START_CMD);
  cmdNames.push_back(SUSPEND_CMD);
  cmdNames.push_back(RESUME_CMD);
  cmdNames.push_back(STOP_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusStartStopRecordingCommand::GetDescription(const std::string& commandName)
{
  std::string desc;
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, START_CMD))
  {
    desc += START_CMD;
    desc += ": Start collecting data into file with a VirtualCapture device. Attributes: OutputFilename: name of the output file (optional if base file name is specified in config file). CaptureDeviceId: ID of the capture device, if not specified then the first VirtualCapture device will be started (optional)";
  }
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, SUSPEND_CMD))
  {
    desc += SUSPEND_CMD;
    desc += ": Suspend data collection. Attributes: CaptureDeviceId: (optional)";
  }
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, RESUME_CMD))
  {
    desc += RESUME_CMD;
    desc += ": Resume suspended data collection. Attributes: CaptureDeviceId (optional)";
  }
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, STOP_CMD))
  {
    desc += STOP_CMD;
    desc += ": Stop collecting data into file with a VirtualCapture device. Attributes: OutputFilename: name of the output file (optional if base file name is specified in config file). CaptureDeviceId (optional)";
  }
  return desc;
}

//----------------------------------------------------------------------------
void vtkPlusStartStopRecordingCommand::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
vtkPlusCommand* vtkPlusStartStopRecordingCommand::Clone()
{
  return New();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStartStopRecordingCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  // Common parameters
  XML_READ_CSTRING_ATTRIBUTE_OPTIONAL(CaptureDeviceId, aConfig);
  XML_READ_CSTRING_ATTRIBUTE_OPTIONAL(ChannelId, aConfig);

  if (CaptureDeviceId.empty() && ChannelId.empty())
  {
    LOG_ERROR("Neither CaptureDeviceId nor ChannelId specified. Aborting.");
    return PLUS_FAIL;
  }

  // Start/Stop Common parameters
  XML_READ_CSTRING_ATTRIBUTE_OPTIONAL(OutputFilename, aConfig);

  // Start-only parameters
  if (this->GetName() == START_CMD)
  {
    XML_READ_BOOL_ATTRIBUTE_OPTIONAL(EnableCompression, aConfig);
    XML_READ_STRING_ATTRIBUTE_OPTIONAL(CodecFourCC, aConfig);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStartStopRecordingCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::WriteConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  XML_WRITE_STRING_ATTRIBUTE_REMOVE_IF_EMPTY(CaptureDeviceId, aConfig);
  XML_WRITE_STRING_ATTRIBUTE_REMOVE_IF_EMPTY(ChannelId, aConfig);
  XML_WRITE_STRING_ATTRIBUTE_REMOVE_IF_EMPTY(OutputFilename, aConfig);

  if (this->GetName() == START_CMD)
  {
    XML_WRITE_BOOL_ATTRIBUTE(EnableCompression, aConfig);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
vtkPlusVirtualCapture* vtkPlusStartStopRecordingCommand::GetCaptureDevice(const std::string& captureDeviceId)
{
  vtkPlusDataCollector* dataCollector = GetDataCollector();
  if (dataCollector == NULL)
  {
    LOG_ERROR("Data collector is invalid");
    return NULL;
  }
  vtkPlusVirtualCapture* captureDevice = NULL;
  if (!captureDeviceId.empty())
  {
    // Capture device ID is specified
    vtkPlusDevice* device = NULL;
    if (dataCollector->GetDevice(device, captureDeviceId) != PLUS_SUCCESS)
    {
      LOG_ERROR("No VirtualCapture has been found by the name " << captureDeviceId);
      return NULL;
    }
    // device found
    captureDevice = vtkPlusVirtualCapture::SafeDownCast(device);
    if (captureDevice == NULL)
    {
      // wrong type
      LOG_ERROR("The specified device " << captureDeviceId << " is not a VirtualCapture device.");
      return NULL;
    }
  }
  else
  {
    // No capture device id is specified, auto-detect the first one and use that
    for (DeviceCollectionConstIterator it = dataCollector->GetDeviceConstIteratorBegin(); it != dataCollector->GetDeviceConstIteratorEnd(); ++it)
    {
      captureDevice = vtkPlusVirtualCapture::SafeDownCast(*it);
      if (captureDevice != NULL)
      {
        // found a recording device
        break;
      }
    }
    if (captureDevice == NULL)
    {
      LOG_ERROR("No VirtualCapture has been found");
      return NULL;
    }
  }
  return captureDevice;
}

//----------------------------------------------------------------------------
vtkPlusVirtualCapture* vtkPlusStartStopRecordingCommand::GetOrCreateCaptureDevice(const std::string& channelId)
{
  vtkPlusDataCollector* dataCollector = GetDataCollector();
  if (dataCollector == NULL)
  {
    LOG_ERROR("Data collector is invalid");
    return nullptr;
  }

  if (channelId.empty())
  {
    return nullptr;
  }

  vtkPlusChannel* channel(nullptr);
  if (dataCollector->GetChannel(channel, channelId) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to locate channel. Aborting.");
    return nullptr;
  }

  vtkPlusVirtualCapture* foundDevice(nullptr);
  for (auto iter = dataCollector->GetDeviceConstIteratorBegin(); iter != dataCollector->GetDeviceConstIteratorEnd(); ++iter)
  {
    if (dynamic_cast<vtkPlusVirtualCapture*>(*iter) != nullptr)
    {
      std::vector<vtkPlusDevice*> devices;
      (*iter)->GetInputDevices(devices);
      for (auto it = devices.begin(); it != devices.end(); ++it)
      {
        vtkPlusChannel* aChannel;
        if ((*it)->GetOutputChannelByName(aChannel, channelId) == PLUS_SUCCESS)
        {
          foundDevice = dynamic_cast<vtkPlusVirtualCapture*>(*iter);
        }
      }
    }
  }

  if (foundDevice != nullptr)
  {
    return foundDevice;
  }

  // Capture device for this input channel does not exist
  vtkPlusDevice* device = channel->GetOwnerDevice();
  assert(device != nullptr);

  vtkPlusDevice* newDevice(nullptr);
  vtkSmartPointer<vtkPlusDeviceFactory> factory = vtkSmartPointer<vtkPlusDeviceFactory>::New();
  if (factory->CreateInstance("VirtualCapture", newDevice, channelId + "_capture") != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to create capture device. Aborting.");
    return nullptr;
  }
  auto capDevice = dynamic_cast<vtkPlusVirtualCapture*>(newDevice);
  newDevice->SetDataCollector(dataCollector);
  newDevice->AddInputChannel(channel);
  capDevice->SetEnableFileCompression(this->EnableCompression);
  if (!this->CodecFourCC.empty())
  {
    capDevice->SetEncodingFourCC(this->CodecFourCC);
  }
  capDevice->SetBaseFilename(channelId + "_capture.nrrd");

  return capDevice;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStartStopRecordingCommand::Execute()
{
  LOG_INFO("vtkPlusStartStopRecordingCommand::Execute:");

  if (this->Name.empty())
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", "No command name specified");
    return PLUS_FAIL;
  }

  vtkPlusVirtualCapture* captureDevice;
  if (!this->CaptureDeviceId.empty())
  {
    captureDevice = GetCaptureDevice(this->CaptureDeviceId);
  }
  else if (!this->ChannelId.empty())
  {
    captureDevice = GetOrCreateCaptureDevice(this->ChannelId);
  }

  if (captureDevice == NULL)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", std::string("VirtualCapture has not been found (")
                               + (!this->CaptureDeviceId.empty() ? this->CaptureDeviceId : !this->ChannelId.empty() ? this->ChannelId : "auto-detect") + "), " + this->Name);
    return PLUS_FAIL;
  }

  std::string responseMessageBase = std::string("VirtualCapture (") + captureDevice->GetDeviceId() + ") " + this->Name + " ";
  LOG_INFO("vtkPlusStartStopRecordingCommand::Execute: " << this->Name);

  if (igsioCommon::IsEqualInsensitive(this->Name, START_CMD))
  {
    if (captureDevice->GetEnableCapturing())
    {
      this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", responseMessageBase + std::string("Recording to file is already in progress."));
      return PLUS_FAIL;
    }
    std::string outputFilename = captureDevice->GetBaseFilename();
    if (!this->OutputFilename.empty())
    {
      outputFilename = this->OutputFilename;
    }
    // EnableFileCompression must be set before OpenFile is called so that it can be disabled for file types that
    // don't support compression
    captureDevice->SetEnableFileCompression(this->GetEnableCompression());
    if (captureDevice->OpenFile(outputFilename.c_str()) != PLUS_SUCCESS)
    {
      this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", responseMessageBase + std::string("Failed to open file ") + (!this->OutputFilename.empty() ? this->OutputFilename : "(undefined)") + std::string("."));
      return PLUS_FAIL;
    }
    captureDevice->SetEnableCapturing(true);
    this->QueueCommandResponse(PLUS_SUCCESS, responseMessageBase + "successful.");
    return PLUS_SUCCESS;
  }
  else if (igsioCommon::IsEqualInsensitive(this->Name, SUSPEND_CMD))
  {
    if (!captureDevice->GetEnableCapturing())
    {
      this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", responseMessageBase + std::string("Suspend recording failed: recording to file is not in progress."));
      return PLUS_FAIL;
    }
    captureDevice->SetEnableCapturing(false);
    this->QueueCommandResponse(PLUS_SUCCESS, responseMessageBase + "successful.");
    return PLUS_SUCCESS;
  }
  else if (igsioCommon::IsEqualInsensitive(this->Name, RESUME_CMD))
  {
    if (captureDevice->GetEnableCapturing())
    {
      this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", responseMessageBase + std::string("Resume recording failed: recording to file is already in progress."));
      return PLUS_FAIL;
    }
    captureDevice->SetEnableCapturing(true);
    this->QueueCommandResponse(PLUS_SUCCESS, responseMessageBase + "successful.");
    return PLUS_SUCCESS;
  }
  else if (igsioCommon::IsEqualInsensitive(this->Name, STOP_CMD))
  {
    // it's stopped if: not in progress (it may be just suspended) and no frames have been recorded
    if (!captureDevice->GetEnableCapturing() && captureDevice->GetTotalFramesRecorded() == 0)
    {
      this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", responseMessageBase + std::string("Recording to file is already stopped."));
      return PLUS_FAIL;
    }

    captureDevice->SetEnableCapturing(false);

    // Once the file is closed, the filename is no longer valid, so we need to get the filename now
    std::string resultFilename = captureDevice->GetOutputFileName();
    // If we override the output filename then that will be the result filename
    if (!this->OutputFilename.empty())
    {
      resultFilename = this->OutputFilename;
    }

    long numberOfFramesRecorded = captureDevice->GetTotalFramesRecorded();
    std::string actualOutputFilename;
    if (captureDevice->CloseFile(this->OutputFilename.c_str(), &actualOutputFilename) != PLUS_SUCCESS)
    {
      this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", responseMessageBase + "Failed to finalize file: " + resultFilename);
      return PLUS_FAIL;
    }
    std::ostringstream ss;
    ss << "Recording " << numberOfFramesRecorded << " frames successful to file " << actualOutputFilename;
    this->QueueCommandResponse(PLUS_SUCCESS, responseMessageBase + ss.str());
    return PLUS_SUCCESS;
  }

  this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", responseMessageBase + "Unknown command: " + this->Name);
  return PLUS_FAIL;
}