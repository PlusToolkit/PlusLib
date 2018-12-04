/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusDataCollector.h"
#include "vtkPlusStealthLinkCommand.h"
#include "StealthLink/vtkPlusStealthLinkTracker.h"

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

static const std::string GET_STEALTHLINK_EXAM_DATA_CMD = "ExamData";

vtkStandardNewMacro(vtkPlusStealthLinkCommand);

//----------------------------------------------------------------------------
vtkPlusStealthLinkCommand::vtkPlusStealthLinkCommand()
  : KeepReceivedDicomFiles(false)
{
}

//----------------------------------------------------------------------------
vtkPlusStealthLinkCommand::~vtkPlusStealthLinkCommand()
{
}

//----------------------------------------------------------------------------
void vtkPlusStealthLinkCommand::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkPlusStealthLinkCommand::GetCommandNames(std::list<std::string>& cmdNames)
{
  cmdNames.clear();
  cmdNames.push_back(GET_STEALTHLINK_EXAM_DATA_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusStealthLinkCommand::GetDescription(const std::string& commandName)
{
  std::string desc;
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName,  GET_STEALTHLINK_EXAM_DATA_CMD))
  {
    desc += GET_STEALTHLINK_EXAM_DATA_CMD;
    desc += ": Acquire the exam data from the StealthLink Server. The exam data contains the image being displayed on the StealthLink Server. The 3D volume will be constructed using these images";
  }
  return desc;
}

//----------------------------------------------------------------------------
void vtkPlusStealthLinkCommand::SetNameToGetExam()
{
  this->SetName(GET_STEALTHLINK_EXAM_DATA_CMD);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStealthLinkCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  if (aConfig->GetAttribute("DicomImagesOutputDirectory"))
  {
    this->SetDicomImagesOutputDirectory(aConfig->GetAttribute("DicomImagesOutputDirectory"));
  }
  else
  {
    LOG_INFO("The dicom images from stealthlink will be saved into the: " << vtkPlusConfig::GetInstance()->GetOutputDirectory() << "/StealthLinkDicomOutput");
    std::string dicomImagesDefaultOutputDirectory = vtkPlusConfig::GetInstance()->GetOutputDirectory() +  std::string("/StealthLinkDicomOutput");
    this->SetDicomImagesOutputDirectory(dicomImagesDefaultOutputDirectory.c_str());
  }

  XML_READ_STRING_ATTRIBUTE_OPTIONAL(StealthLinkDeviceId, aConfig);

  if (aConfig->GetAttribute("VolumeEmbeddedTransformToFrame"))
  {
    this->SetVolumeEmbeddedTransformToFrame(aConfig->GetAttribute("VolumeEmbeddedTransformToFrame"));
  }
  else
  {
    LOG_INFO("The dicom images from stealthlink will be represented in Ras coordinate system");
    this->SetVolumeEmbeddedTransformToFrame("Ras");
  }

  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(KeepReceivedDicomFiles, aConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStealthLinkCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::WriteConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(StealthLinkDeviceId, aConfig);
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(DicomImagesOutputDirectory, aConfig);
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(VolumeEmbeddedTransformToFrame, aConfig);
  XML_WRITE_BOOL_ATTRIBUTE(KeepReceivedDicomFiles, aConfig);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStealthLinkCommand::Execute()
{
  LOG_DEBUG("vtkPlusStealthLinkCommand::Execute: " << (!this->Name.empty() ? this->Name : "(undefined)")
            << ", device: " << (this->StealthLinkDeviceId.empty() ? "(undefined)" : this->StealthLinkDeviceId));

  if (this->Name.empty())
  {
    this->QueueCommandResponse(PLUS_FAIL, "StealthLink command failed, no command name specified");
    return PLUS_FAIL;
  }

  vtkPlusStealthLinkTracker* stealthLinkDevice = GetStealthLinkDevice();
  if (stealthLinkDevice == NULL)
  {
    this->QueueCommandResponse(PLUS_FAIL, std::string("StealthLink command failed: device ")
                               + (this->StealthLinkDeviceId.empty() ? "(undefined)" : this->StealthLinkDeviceId) + " is not found");
    return PLUS_FAIL;
  }

  if (igsioCommon::IsEqualInsensitive(this->Name, GET_STEALTHLINK_EXAM_DATA_CMD))
  {
    LOG_INFO("Acquiring the exam data from StealthLink Server: Device ID: " << GetStealthLinkDeviceId());

    stealthLinkDevice->SetDicomImagesOutputDirectory(this->GetDicomImagesOutputDirectory());
    stealthLinkDevice->SetKeepReceivedDicomFiles(this->GetKeepReceivedDicomFiles());
    std::string requestedImageId;
    std::string assignedImageId;
    vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
    vtkSmartPointer<vtkMatrix4x4> ijkToReferenceTransform = vtkSmartPointer<vtkMatrix4x4>::New();
    // make available all transforms to the device so that it can compute ijkToReferenceTransform (Reference = VolumeEmbeddedTransformToFrame)
    stealthLinkDevice->UpdateTransformRepository(this->CommandProcessor->GetPlusServer()->GetTransformRepository());
    if (stealthLinkDevice->GetImage(requestedImageId, assignedImageId, std::string(this->GetVolumeEmbeddedTransformToFrame()), imageData, ijkToReferenceTransform) != PLUS_SUCCESS)
    {
      this->QueueCommandResponse(PLUS_FAIL, "vtkPlusStealthLinkCommand::Execute: failed, failed to receive image");
      return PLUS_FAIL;
    }
    std::string resultMessage;
    PlusStatus status = ProcessImageReply(assignedImageId, imageData, ijkToReferenceTransform, resultMessage);
    this->QueueCommandResponse(status, "Volume sending completed: " + resultMessage);
    return PLUS_SUCCESS;
  }
  this->QueueCommandResponse(PLUS_FAIL, "vtkPlusStealthLinkCommand::Execute: failed, unknown command name: " + this->Name);
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStealthLinkCommand::ProcessImageReply(const std::string& imageId, vtkImageData* volumeToSend, vtkMatrix4x4* imageToReferenceOrientationMatrix, std::string& resultMessage)
{
  LOG_DEBUG("Send image to client through OpenIGTLink");
  vtkSmartPointer<vtkPlusCommandImageResponse> imageResponse = vtkSmartPointer<vtkPlusCommandImageResponse>::New();
  this->CommandResponseQueue.push_back(imageResponse);
  imageResponse->SetClientId(this->ClientId);
  imageResponse->SetImageName(imageId);
  imageResponse->SetImageData(volumeToSend);
  imageResponse->SetImageToReferenceTransform(imageToReferenceOrientationMatrix);
  LOG_INFO("Send reconstructed volume to client through OpenIGTLink");
  resultMessage.clear();
  resultMessage = std::string(", volume sent as: ") + imageResponse->GetImageName();
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
vtkPlusStealthLinkTracker* vtkPlusStealthLinkCommand::GetStealthLinkDevice()
{
  vtkPlusDataCollector* dataCollector = GetDataCollector();
  if (dataCollector == NULL)
  {
    LOG_ERROR("Data collector is invalid");
    return NULL;
  }
  if (!GetStealthLinkDeviceId().empty())
  {
    // Reconstructor device ID is specified
    vtkPlusDevice* device = NULL;
    if (dataCollector->GetDevice(device, GetStealthLinkDeviceId()) != PLUS_SUCCESS)
    {
      LOG_ERROR("No StealthLink device has been found by the name " << this->GetStealthLinkDeviceId());
      return NULL;
    }
    // device found
    vtkPlusStealthLinkTracker* stealthLinkDevice = vtkPlusStealthLinkTracker::SafeDownCast(device);
    if (stealthLinkDevice == NULL)
    {
      // wrong type
      LOG_ERROR("The specified device " << GetStealthLinkDeviceId() << " is not StealthLink Device");
      return NULL;
    }
    return stealthLinkDevice;
  }
  else
  {
    // No stealthlink device id is specified, auto-detect the first one and use that
    for (DeviceCollectionConstIterator it = dataCollector->GetDeviceConstIteratorBegin(); it != dataCollector->GetDeviceConstIteratorEnd(); ++it)
    {
      vtkPlusStealthLinkTracker* stealthLinkDevice = vtkPlusStealthLinkTracker::SafeDownCast(*it);
      if (stealthLinkDevice != NULL)
      {
        // found a recording device
        SetStealthLinkDeviceId(stealthLinkDevice->GetDeviceId());
        return stealthLinkDevice;
      }
    }
    LOG_ERROR("No StealthLink Device has been found");
    return NULL;
  }
}