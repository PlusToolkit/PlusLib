/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusDataCollector.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusCommandProcessor.h"
#include "vtkPlusReconstructVolumeCommand.h"
#include "vtkIGSIOTrackedFrameList.h"
#include "vtkIGSIOTransformRepository.h"
#include "vtkPlusVolumeReconstructor.h"
#include "vtkPlusVirtualVolumeReconstructor.h"
#include <limits>

namespace
{
  static const double UNDEFINED_VALUE = std::numeric_limits<double>::max();
  static const std::string RECONSTRUCT_PRERECORDED_CMD = "ReconstructVolume";
  static const std::string START_LIVE_RECONSTRUCTION_CMD = "StartVolumeReconstruction";
  static const std::string SUSPEND_LIVE_RECONSTRUCTION_CMD = "SuspendVolumeReconstruction";
  static const std::string RESUME_LIVE_RECONSTRUCTION_CMD = "ResumeVolumeReconstruction";
  static const std::string STOP_LIVE_RECONSTRUCTION_CMD = "StopVolumeReconstruction";
  static const std::string GET_LIVE_RECONSTRUCTION_SNAPSHOT_CMD = "GetVolumeReconstructionSnapshot";
}

vtkStandardNewMacro(vtkPlusReconstructVolumeCommand);

//----------------------------------------------------------------------------
vtkPlusReconstructVolumeCommand::vtkPlusReconstructVolumeCommand()
  : ApplyHoleFilling(true)
{
  this->OutputOrigin[0] = UNDEFINED_VALUE;
  this->OutputOrigin[1] = UNDEFINED_VALUE;
  this->OutputOrigin[2] = UNDEFINED_VALUE;
  this->OutputSpacing[0] = UNDEFINED_VALUE;
  this->OutputSpacing[1] = UNDEFINED_VALUE;
  this->OutputSpacing[2] = UNDEFINED_VALUE;
  this->OutputExtent[0] = 0;
  this->OutputExtent[1] = -1;
  this->OutputExtent[2] = 0;
  this->OutputExtent[3] = -1;
  this->OutputExtent[4] = 0;
  this->OutputExtent[5] = -1;
}

//----------------------------------------------------------------------------
vtkPlusReconstructVolumeCommand::~vtkPlusReconstructVolumeCommand()
{
}

//----------------------------------------------------------------------------
void vtkPlusReconstructVolumeCommand::SetNameToReconstruct()
{
  SetName(RECONSTRUCT_PRERECORDED_CMD);
}
void vtkPlusReconstructVolumeCommand::SetNameToStart()
{
  SetName(START_LIVE_RECONSTRUCTION_CMD);
}
void vtkPlusReconstructVolumeCommand::SetNameToStop()
{
  SetName(STOP_LIVE_RECONSTRUCTION_CMD);
}
void vtkPlusReconstructVolumeCommand::SetNameToSuspend()
{
  SetName(SUSPEND_LIVE_RECONSTRUCTION_CMD);
}
void vtkPlusReconstructVolumeCommand::SetNameToResume()
{
  SetName(RESUME_LIVE_RECONSTRUCTION_CMD);
}
void vtkPlusReconstructVolumeCommand::SetNameToGetSnapshot()
{
  SetName(GET_LIVE_RECONSTRUCTION_SNAPSHOT_CMD);
}

//----------------------------------------------------------------------------
void vtkPlusReconstructVolumeCommand::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkPlusReconstructVolumeCommand::GetCommandNames(std::list<std::string>& cmdNames)
{
  cmdNames.clear();
  cmdNames.push_back(RECONSTRUCT_PRERECORDED_CMD);
  cmdNames.push_back(START_LIVE_RECONSTRUCTION_CMD);
  cmdNames.push_back(SUSPEND_LIVE_RECONSTRUCTION_CMD);
  cmdNames.push_back(RESUME_LIVE_RECONSTRUCTION_CMD);
  cmdNames.push_back(STOP_LIVE_RECONSTRUCTION_CMD);
  cmdNames.push_back(GET_LIVE_RECONSTRUCTION_SNAPSHOT_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusReconstructVolumeCommand::GetDescription(const std::string& commandName)
{
  std::string desc;
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, RECONSTRUCT_PRERECORDED_CMD))
  {
    desc += RECONSTRUCT_PRERECORDED_CMD;
    desc += ": Reconstruct a volume from a file and writes the result to a file. Attributes: InputSeqFilename: name of the input sequence file name that contains the list of frames. OutputVolFilename: name of the output volume file name (optional). OutputVolDeviceName: name of the OpenIGTLink device for the IMAGE message (optional).";
  }
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, START_LIVE_RECONSTRUCTION_CMD))
  {
    desc += START_LIVE_RECONSTRUCTION_CMD;
    desc += ": Start adding acquired frames to the volume. Attributes: VolumeReconstructorDeviceId: ID of the volume reconstructor device. OutputVolFilename: name of the output volume file name (optional). OutputVolDeviceName: name of the OpenIGTLink device for the IMAGE message (optional).";
  }
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, SUSPEND_LIVE_RECONSTRUCTION_CMD))
  {
    desc += SUSPEND_LIVE_RECONSTRUCTION_CMD;
    desc += ": Suspend adding acquired frames to the volume. Attributes: VolumeReconstructorDeviceId: ID of the volume reconstructor device.";
  }
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, RESUME_LIVE_RECONSTRUCTION_CMD))
  {
    desc += RESUME_LIVE_RECONSTRUCTION_CMD;
    desc += ": Resume adding acquired frames to the volume. Attributes: VolumeReconstructorDeviceId: ID of the volume reconstructor device.";
  }
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, STOP_LIVE_RECONSTRUCTION_CMD))
  {
    desc += STOP_LIVE_RECONSTRUCTION_CMD;
    desc += ": Stop adding acquired frames to the volume, finalize reconstruction, and save/send the results. Attributes: VolumeReconstructorDeviceId: ID of the volume reconstructor device. OutputVolFilename: name of the output volume file name (optional). OutputVolDeviceName: name of the OpenIGTLink device for the IMAGE message (optional).";
  }
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, GET_LIVE_RECONSTRUCTION_SNAPSHOT_CMD))
  {
    desc += GET_LIVE_RECONSTRUCTION_SNAPSHOT_CMD;
    desc += ": Request a snapshot of the live reconstruction result. Attributes: VolumeReconstructorDeviceId: ID of the volume reconstructor device. OutputVolFilename: name of the output volume file name (optional). OutputVolDeviceName: name of the OpenIGTLink device for the IMAGE message (optional). ApplyHoleFilling: if FALSE then holes will not be filled (optional, default: TRUE).";
  }

  return desc;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusReconstructVolumeCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(InputSeqFilename, aConfig);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(OutputVolFilename, aConfig);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(OutputVolDeviceName, aConfig);
  this->SetVolumeReconstructorDeviceId(aConfig->GetAttribute("VolumeReconstructorDeviceId"));

  // output volume parameters
  // origin and spacing is defined in the reference coordinate system
  XML_READ_VECTOR_ATTRIBUTE_OPTIONAL(double, 3, OutputSpacing, aConfig);
  XML_READ_VECTOR_ATTRIBUTE_OPTIONAL(double, 3, OutputOrigin, aConfig);
  XML_READ_VECTOR_ATTRIBUTE_OPTIONAL(int, 6, OutputExtent, aConfig);

  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(ApplyHoleFilling, aConfig);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusReconstructVolumeCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::WriteConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(VolumeReconstructorDeviceId, aConfig);
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(InputSeqFilename, aConfig);
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(OutputVolFilename, aConfig);
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(OutputVolDeviceName, aConfig);

  if (this->OutputOrigin[0] != UNDEFINED_VALUE
      && this->OutputOrigin[1] != UNDEFINED_VALUE
      && this->OutputOrigin[2] != UNDEFINED_VALUE)
  {
    aConfig->SetVectorAttribute("OutputOrigin", 3, this->OutputOrigin);
  }

  if (this->OutputSpacing[0] != UNDEFINED_VALUE
      && this->OutputSpacing[1] != UNDEFINED_VALUE
      && this->OutputSpacing[2] != UNDEFINED_VALUE)
  {
    aConfig->SetVectorAttribute("OutputSpacing", 3, this->OutputSpacing);
  }

  if (this->OutputExtent[1] >= this->OutputExtent[0]
      && this->OutputExtent[3] >= this->OutputExtent[2]
      && this->OutputExtent[5] >= this->OutputExtent[4])
  {
    aConfig->SetVectorAttribute("OutputExtent", 6, this->OutputExtent);
  }

  XML_WRITE_BOOL_ATTRIBUTE(ApplyHoleFilling, aConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusReconstructVolumeCommand::Execute()
{
  LOG_DEBUG("vtkPlusReconstructVolumeCommand::Execute: " << (!this->Name.empty() ? this->Name : "(undefined)")
            << ", device: " << (this->VolumeReconstructorDeviceId.empty() ? "(undefined)" : this->VolumeReconstructorDeviceId));

  if (this->Name.empty())
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", "No command name specified.");
    return PLUS_FAIL;
  }

  vtkPlusVirtualVolumeReconstructor* reconstructorDevice = GetVolumeReconstructorDevice();
  if (reconstructorDevice == NULL)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", std::string("Device ")
                               + (this->VolumeReconstructorDeviceId.empty() ? "(undefined)" : this->VolumeReconstructorDeviceId) + std::string(" is not found."));
    return PLUS_FAIL;
  }

  // If output volume filename and/or device name is specified then update it in the reconstructor device
  if (!this->GetOutputVolFilename().empty())
  {
    reconstructorDevice->SetOutputVolFilename(this->GetOutputVolFilename());
  }
  if (!this->GetOutputVolDeviceName().empty())
  {
    reconstructorDevice->SetOutputVolDeviceName(this->GetOutputVolDeviceName());
  }
  std::string outputVolFilename = (!reconstructorDevice->GetOutputVolFilename().empty() ? reconstructorDevice->GetOutputVolFilename() : "");
  std::string outputVolDeviceName = (!reconstructorDevice->GetOutputVolDeviceName().empty() ? reconstructorDevice->GetOutputVolDeviceName() : "");

  std::string reconstructorDeviceId = (reconstructorDevice->GetDeviceId().empty() ? "(unknown)" : reconstructorDevice->GetDeviceId());

  // Set output volume size and resolution
  if (igsioCommon::IsEqualInsensitive(this->Name, RECONSTRUCT_PRERECORDED_CMD) || igsioCommon::IsEqualInsensitive(this->Name, START_LIVE_RECONSTRUCTION_CMD))
  {
    // Only allow changing the output volume when we start the reconstruction
    if (this->OutputOrigin[0] != UNDEFINED_VALUE && this->OutputOrigin[1] != UNDEFINED_VALUE && this->OutputOrigin[2] != UNDEFINED_VALUE)
    {
      reconstructorDevice->SetOutputOrigin(this->OutputOrigin);
    }

    if (this->OutputSpacing[0] != UNDEFINED_VALUE && this->OutputSpacing[1] != UNDEFINED_VALUE && this->OutputSpacing[2] != UNDEFINED_VALUE)
    {
      reconstructorDevice->SetOutputSpacing(this->OutputSpacing);
    }

    if (this->OutputExtent[1] >= this->OutputExtent[0] && this->OutputExtent[3] >= this->OutputExtent[2] && this->OutputExtent[5] >= this->OutputExtent[4])
    {
      reconstructorDevice->SetOutputExtent(this->OutputExtent);
    }
  }

  std::string baseMessage = this->Name + std::string("(") + reconstructorDeviceId + std::string(")");
  if (igsioCommon::IsEqualInsensitive(this->Name, RECONSTRUCT_PRERECORDED_CMD))
  {
    LOG_INFO("Volume reconstruction from sequence file: " << (!this->InputSeqFilename.empty() ? this->InputSeqFilename : "(undefined)") << ", device: " << reconstructorDeviceId);
    if (reconstructorDevice->GetEnableReconstruction())
    {
      this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", baseMessage + " Reconstruction from sequence file failed: live volume reconstruction is in progress.");
      return PLUS_FAIL;
    }
    if (reconstructorDevice->UpdateTransformRepository(this->CommandProcessor->GetPlusServer()->GetTransformRepository()) != PLUS_SUCCESS)
    {
      this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", baseMessage + " Reconstruction from sequence file failed: cannot get transform repository.");
      return PLUS_FAIL;
    }
    reconstructorDevice->Reset(); // Clear volume
    vtkSmartPointer<vtkImageData> volumeToSend = vtkSmartPointer<vtkImageData>::New();
    std::string errorMessage;
    if (reconstructorDevice->GetReconstructedVolumeFromFile(this->InputSeqFilename, volumeToSend, errorMessage) != PLUS_SUCCESS)
    {
      this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", baseMessage + " Reconstruction from sequence file failed: " + errorMessage);
      return PLUS_FAIL;
    }
    std::string statusMessage;
    PlusStatus status = ProcessImageReply(volumeToSend, outputVolFilename, outputVolDeviceName, statusMessage);
    this->QueueCommandResponse(status, std::string("Command ") + std::string((status == PLUS_SUCCESS ? "succeeded." : "failed. See error message.")), baseMessage + " Reconstruction from sequence file completed: " + statusMessage);
    return status;
  }
  else if (igsioCommon::IsEqualInsensitive(this->Name, START_LIVE_RECONSTRUCTION_CMD))
  {
    LOG_INFO("Volume reconstruction from live frames starting, device: " << reconstructorDeviceId);
    if (reconstructorDevice->GetEnableReconstruction())
    {
      this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", baseMessage + " Reconstruction starting from live frames failed: live volume reconstruction is in progress.");
      return PLUS_FAIL;
    }
    if (reconstructorDevice->UpdateTransformRepository(this->CommandProcessor->GetPlusServer()->GetTransformRepository()) != PLUS_SUCCESS)
    {
      this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", baseMessage + " Reconstruction starting from live frames failed: cannot get transform repository.");
      return PLUS_FAIL;
    }
    reconstructorDevice->Reset(); // Clear volume
    reconstructorDevice->SetEnableReconstruction(true);
    this->QueueCommandResponse(PLUS_SUCCESS, baseMessage + " Volume reconstruction from live frames started.");
    return PLUS_SUCCESS;
  }
  else if (igsioCommon::IsEqualInsensitive(this->Name, STOP_LIVE_RECONSTRUCTION_CMD))
  {
    // it's stopped if: not in progress (it may be just suspended) and no frames have been recorded
    if (!reconstructorDevice->GetEnableReconstruction() && reconstructorDevice->GetTotalFramesRecorded() == 0)
    {
      this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", baseMessage + " Reconstruction stop from live frames failed: live volume reconstruction is already stopped, device.");
      return PLUS_FAIL;
    }

    LOG_INFO("Volume reconstruction from live frames stopping, device: " << reconstructorDeviceId);
    reconstructorDevice->SetEnableReconstruction(false);
    vtkSmartPointer<vtkImageData> volumeToSend = vtkSmartPointer<vtkImageData>::New();
    std::string errorMessage;
    if (reconstructorDevice->GetReconstructedVolume(volumeToSend, errorMessage) != PLUS_SUCCESS)
    {
      this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", baseMessage + "Reconstruction stop from live frames failed: " + errorMessage);
      return PLUS_FAIL;
    }
    reconstructorDevice->Reset(); // Clear volume
    std::string statusMessage;
    PlusStatus status = ProcessImageReply(volumeToSend, outputVolFilename, outputVolDeviceName, statusMessage);
    this->QueueCommandResponse(status, std::string("Command ") + std::string((status == PLUS_SUCCESS ? "succeeded." : "failed. See error message.")), baseMessage + " Reconstruction from live frames completed: " + statusMessage);
    return status;
  }
  else if (igsioCommon::IsEqualInsensitive(this->Name, GET_LIVE_RECONSTRUCTION_SNAPSHOT_CMD))
  {
    LOG_INFO("Volume reconstruction from live frames snapshot request, device: " << reconstructorDeviceId);
    vtkSmartPointer<vtkImageData> volumeToSend = vtkSmartPointer<vtkImageData>::New();
    std::string errorMessage;
    if (reconstructorDevice->GetReconstructedVolume(volumeToSend, errorMessage, this->ApplyHoleFilling) != PLUS_SUCCESS)
    {
      this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", baseMessage + " Reconstruction snapshot request failed, device: " + errorMessage);
      return PLUS_FAIL;
    }
    std::string statusMessage;
    PlusStatus status = ProcessImageReply(volumeToSend, outputVolFilename, outputVolDeviceName, statusMessage);
    this->QueueCommandResponse(status, std::string("Command ") + std::string((status == PLUS_SUCCESS ? "succeeded." : "failed. See error message.")), baseMessage + " " + statusMessage);
    return status;
  }
  else if (igsioCommon::IsEqualInsensitive(this->Name, SUSPEND_LIVE_RECONSTRUCTION_CMD))
  {
    LOG_INFO("Volume reconstruction from live frames suspending, device: " << reconstructorDeviceId);
    if (!reconstructorDevice->GetEnableReconstruction())
    {
      this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", baseMessage + " Reconstruction suspend from live frames failed: live volume reconstruction is not in progress.");
      return PLUS_FAIL;
    }
    reconstructorDevice->SetEnableReconstruction(false);
    this->QueueCommandResponse(PLUS_SUCCESS, "Volume reconstruction from live frames suspended, device: " + reconstructorDeviceId);
    return PLUS_SUCCESS;
  }
  else if (igsioCommon::IsEqualInsensitive(this->Name, RESUME_LIVE_RECONSTRUCTION_CMD))
  {
    LOG_INFO("Volume reconstruction from live frames resuming, device: " << reconstructorDeviceId);
    if (reconstructorDevice->GetEnableReconstruction())
    {
      this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", baseMessage + " Reconstruction resume from live frames failed: live volume reconstruction is already in progress.");
      return PLUS_FAIL;
    }
    reconstructorDevice->SetEnableReconstruction(true);
    this->QueueCommandResponse(PLUS_SUCCESS, baseMessage + " Volume reconstruction from live frames resumed.");
    return PLUS_SUCCESS;
  }

  this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", baseMessage + "Unknown command name: " + this->Name + ".");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusReconstructVolumeCommand::ProcessImageReply(vtkImageData* volumeToSend, const std::string& outputVolFilename, const std::string& outputVolDeviceName, std::string& resultMessage)
{
  PlusStatus status = PLUS_SUCCESS;
  resultMessage.clear();
  if (!outputVolFilename.empty())
  {
    std::string outputVolFileFullPath = vtkPlusConfig::GetInstance()->GetOutputPath(outputVolFilename);
    LOG_INFO("Saving reconstructed volume to file: " << outputVolFileFullPath);
    if (vtkPlusVolumeReconstructor::SaveReconstructedVolumeToFile(volumeToSend, outputVolFileFullPath) != PLUS_SUCCESS)
    {
      status = PLUS_FAIL;
      resultMessage += std::string("saving reconstructed volume to ") + outputVolFileFullPath + " failed";
    }
    else
    {
      resultMessage += std::string("saved reconstructed volume to file: ") + outputVolFileFullPath;
    }
  }
  if (!outputVolDeviceName.empty())
  {
    // send the reconstructed volume with the reply
    LOG_DEBUG("Send image to client through OpenIGTLink");
    vtkSmartPointer<vtkPlusCommandImageResponse> imageResponse = vtkSmartPointer<vtkPlusCommandImageResponse>::New();
    imageResponse->SetClientId(this->ClientId);
    imageResponse->SetImageName(outputVolDeviceName);
    imageResponse->SetImageData(volumeToSend);
    vtkSmartPointer<vtkMatrix4x4> volumeToReferenceTransform = vtkSmartPointer<vtkMatrix4x4>::New();
    imageResponse->SetImageToReferenceTransform(volumeToReferenceTransform);
    volumeToReferenceTransform->Identity(); // we leave it as identity, as the volume coordinate system is, the same as the reference coordinate system (we may extend this later so that the client can request the volume in any coordinate system)
    LOG_INFO("Send reconstructed volume to client through OpenIGTLink");
    if (!resultMessage.empty())
    {
      resultMessage += ", ";
    }
    resultMessage += std::string("image sent as: ") + outputVolDeviceName;
    this->CommandResponseQueue.push_back(imageResponse);
  }
  return status;
}

//----------------------------------------------------------------------------
vtkPlusVirtualVolumeReconstructor* vtkPlusReconstructVolumeCommand::GetVolumeReconstructorDevice()
{
  vtkPlusDataCollector* dataCollector = GetDataCollector();
  if (dataCollector == NULL)
  {
    LOG_ERROR("Data collector is invalid");
    return NULL;
  }
  vtkPlusVirtualVolumeReconstructor* reconstructorDevice = NULL;
  if (!this->VolumeReconstructorDeviceId.empty())
  {
    // Reconstructor device ID is specified
    vtkPlusDevice* device = NULL;
    if (dataCollector->GetDevice(device, this->VolumeReconstructorDeviceId) != PLUS_SUCCESS)
    {
      LOG_ERROR("No volume reconstructor device has been found by the name " << this->VolumeReconstructorDeviceId);
      return NULL;
    }
    // device found
    reconstructorDevice = vtkPlusVirtualVolumeReconstructor::SafeDownCast(device);
    if (reconstructorDevice == NULL)
    {
      // wrong type
      LOG_ERROR("The specified device " << this->VolumeReconstructorDeviceId << " is not VirtualVolumeReconstructorDevice");
      return NULL;
    }
  }
  else
  {
    // No volume reconstruction device id is specified, auto-detect the first one and use that
    for (DeviceCollectionConstIterator it = dataCollector->GetDeviceConstIteratorBegin(); it != dataCollector->GetDeviceConstIteratorEnd(); ++it)
    {
      reconstructorDevice = vtkPlusVirtualVolumeReconstructor::SafeDownCast(*it);
      if (reconstructorDevice != NULL)
      {
        // found a recording device
        break;
      }
    }
    if (reconstructorDevice == NULL)
    {
      LOG_ERROR("No VirtualVolumeReconstructor has been found");
      return NULL;
    }
  }
  return reconstructorDevice;
}
