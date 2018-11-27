/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusDataCollector.h"
#include "vtkPlusGetImageCommand.h"

#include "vtkImageData.h"
#include "vtkPlusCommandProcessor.h"

namespace
{
  static const std::string GET_IMAGE_META_DATA = "GET_IMGMETA";
  static const std::string GET_IMAGE = "GET_IMAGE";

  static const char DeviceNameImageIdSeparator = '-';
}

vtkStandardNewMacro(vtkPlusGetImageCommand);

//----------------------------------------------------------------------------
vtkPlusGetImageCommand::vtkPlusGetImageCommand()
  : ImageMetaDatasetsCount(1)
{

}

//----------------------------------------------------------------------------
vtkPlusGetImageCommand::~vtkPlusGetImageCommand()
{
}

//----------------------------------------------------------------------------
void vtkPlusGetImageCommand::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkPlusGetImageCommand::GetCommandNames(std::list<std::string>& cmdNames)
{
  cmdNames.clear();
  cmdNames.push_back(GET_IMAGE_META_DATA);
  cmdNames.push_back(GET_IMAGE);
}

//----------------------------------------------------------------------------
std::string vtkPlusGetImageCommand::GetDescription(const std::string& commandName)
{
  std::string desc;
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, GET_IMAGE_META_DATA))
  {
    desc += GET_IMAGE_META_DATA;
    desc += ": Acquire the image meta data information from all the devices that are connected.";
  }
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, GET_IMAGE))
  {
    desc += GET_IMAGE;
    desc += "Acquire the volume data and the ijkToRas transformation of the data from the specified device.";
  }
  return desc;
}

//----------------------------------------------------------------------------
void vtkPlusGetImageCommand::SetNameToGetImageMeta()
{
  SetName(GET_IMAGE_META_DATA);
}

//----------------------------------------------------------------------------
void vtkPlusGetImageCommand::SetNameToGetImage()
{
  SetName(GET_IMAGE);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGetImageCommand::Execute()
{
  std::string baseMessage = std::string("vtkPlusGetImageCommand ") + (!this->Name.empty() ? this->Name : "(undefined)")
                            + ", device: (" + (this->GetImageId().empty() ? "(undefined)" : this->GetImageId()) + ")";

  std::string errorString;
  if (igsioCommon::IsEqualInsensitive(this->Name, GET_IMAGE_META_DATA))
  {
    std::string imageIdStr(this->GetImageId());
    if (imageIdStr.size() > 0)
    {
      LOG_ERROR("The implementation of the GET_IMGMETA is not implemented for the case in which an id is specified. The id is supposed to be empty and then it will send the meta data from all connected devices")
      return PLUS_FAIL;
    }
    if (this->ExecuteImageMetaReply(errorString) != PLUS_SUCCESS)
    {
      this->QueueCommandResponse(PLUS_FAIL, baseMessage + " Failed. See error message.", errorString);
      return PLUS_FAIL;
    }
    return PLUS_SUCCESS;
  }
  else if (igsioCommon::IsEqualInsensitive(this->Name, GET_IMAGE))
  {
    if (this->ExecuteImageReply(errorString) != PLUS_SUCCESS)
    {
      this->QueueCommandResponse(PLUS_FAIL, baseMessage + " Failed. See error message.", errorString);
      return PLUS_FAIL;
    }
    return PLUS_SUCCESS;
  }
  this->QueueCommandResponse(PLUS_FAIL, baseMessage + " Failed. See error message.", std::string("Unknown command name: ") + this->Name);
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGetImageCommand::ExecuteImageReply(std::string& outErrorString)
{
  vtkPlusDataCollector* dataCollector = GetDataCollector();
  if (dataCollector == NULL)
  {
    outErrorString = "The DataCollector is NULL.";
    return PLUS_FAIL;
  }

  vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
  vtkSmartPointer<vtkMatrix4x4> ijkToRasTransform = vtkSmartPointer<vtkMatrix4x4>::New();
  for (DeviceCollectionConstIterator it = dataCollector->GetDeviceConstIteratorBegin(); it != dataCollector->GetDeviceConstIteratorEnd(); ++it)
  {
    vtkPlusDevice* plusDevice = (*it);
    if (plusDevice->GetDeviceId().empty())
    {
      outErrorString = "PLUS device has a NULL id.";
      return PLUS_FAIL;
    }

    std::string imageIdStr(this->GetImageId()); // SLD-001
    size_t dashFound = imageIdStr.find_last_of(DeviceNameImageIdSeparator);
    if (dashFound == std::string::npos)
    {
      outErrorString = "ImageId has to contain a dash right after the deviceId.";
      return PLUS_FAIL;
    }

    std::string requestedDeviceId = imageIdStr.substr(0, dashFound); //SLD
    std::string requestedImageId = imageIdStr.substr(dashFound + 1); // 001
    std::string deviceIdStr(plusDevice->GetDeviceId()); // SLD
    if (requestedDeviceId.compare(deviceIdStr) == 0)
    {
      std::string assignedImageId("");
      if (plusDevice->GetImage(requestedImageId, assignedImageId, std::string("Ras"), imageData, ijkToRasTransform))
      {
        if (assignedImageId.compare(requestedImageId) != 0)
        {
          outErrorString = "The assignedImageId does not match requestedImageId.";
          return PLUS_FAIL;
        }

        vtkSmartPointer<vtkPlusCommandImageResponse> imageResponse = vtkSmartPointer<vtkPlusCommandImageResponse>::New();
        this->CommandResponseQueue.push_back(imageResponse);
        imageResponse->SetClientId(this->ClientId);
        imageResponse->SetImageName(this->GetImageId());
        imageResponse->SetImageData(imageData);
        imageResponse->SetRespondWithCommandMessage(this->RespondWithCommandMessage);
        imageResponse->SetImageToReferenceTransform(ijkToRasTransform);

        return PLUS_SUCCESS;
      }
    }
  }
  outErrorString = "Could not find the image.";
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGetImageCommand::ExecuteImageMetaReply(std::string& outErrorString)
{
  vtkPlusDataCollector* dataCollector = GetDataCollector();
  if (dataCollector == NULL)
  {
    outErrorString = "The DataCollector is NULL.";
    return PLUS_FAIL;
  }
  vtkPlusDevice* plusDevice;
  igsioCommon::ImageMetaDataList imageMetaDataList;
  for (DeviceCollectionConstIterator it = dataCollector->GetDeviceConstIteratorBegin(); it != dataCollector->GetDeviceConstIteratorEnd(); ++it)
  {
    plusDevice = (*it);
    if (plusDevice == NULL)
    {
      outErrorString = "NULL device found in data collector list.";
      return PLUS_FAIL;
    }
    if (plusDevice->GetDeviceId().empty())
    {
      outErrorString = "PLUS device has a NULL deviceId.";
      return PLUS_FAIL;
    }
    igsioCommon::ImageMetaDataList imageMetaDataListDevice;
    imageMetaDataListDevice.clear();
    plusDevice->GetImageMetaData(imageMetaDataListDevice);
    for (igsioCommon::ImageMetaDataList::iterator it = imageMetaDataListDevice.begin(); it != imageMetaDataListDevice.end(); it++)
    {
      if (it->Id.find(DeviceNameImageIdSeparator) != std::string::npos)
      {
        outErrorString = "DeviceId cannot contain a dash.";
        return PLUS_FAIL;
      }
      it->Id = std::string(plusDevice->GetDeviceId()) + DeviceNameImageIdSeparator + (*it).Id;
    }
    imageMetaDataList.splice(imageMetaDataList.end(), imageMetaDataListDevice, imageMetaDataListDevice.begin(), imageMetaDataListDevice.end());
  }
  if (imageMetaDataList.size() == 0)
  {
    LOG_INFO("There are currently no images on the devices that are connected through plus.");
  }

  vtkSmartPointer<vtkPlusCommandImageMetaDataResponse > imageMetaDataResponse = vtkSmartPointer<vtkPlusCommandImageMetaDataResponse>::New();
  this->CommandResponseQueue.push_back(imageMetaDataResponse);
  imageMetaDataResponse->SetClientId(this->ClientId);
  imageMetaDataResponse->SetRespondWithCommandMessage(this->GetRespondWithCommandMessage());
  imageMetaDataResponse->SetImageMetaDataItems(imageMetaDataList);
  return PLUS_SUCCESS;
}
