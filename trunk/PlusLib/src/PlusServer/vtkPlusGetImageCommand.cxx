/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "vtkDataCollector.h"
#include "vtkPlusGetImageCommand.h"

#include "vtkImageData.h"
#include "vtkPlusCommandProcessor.h"

static const char GET_IMAGE_META_DATA[]="GET_IMGMETA";
static const char GET_IMAGE[]="GET_IMAGE";
static const char GetRasName[] = "Ras";

static const char DeviceNameImageIdSeparator='-';

vtkStandardNewMacro( vtkPlusGetImageCommand );

//----------------------------------------------------------------------------
vtkPlusGetImageCommand::vtkPlusGetImageCommand()
  : ImageId(NULL)
{
  this->ImageMetaDatasetsCount = 1;
}

//----------------------------------------------------------------------------
vtkPlusGetImageCommand::~vtkPlusGetImageCommand()
{
  this->SetImageId(NULL);
}

//----------------------------------------------------------------------------
void vtkPlusGetImageCommand::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
void vtkPlusGetImageCommand::GetCommandNames(std::list<std::string> &cmdNames)
{ 
  cmdNames.clear(); 
  cmdNames.push_back(GET_IMAGE_META_DATA);
  cmdNames.push_back(GET_IMAGE);
}

//----------------------------------------------------------------------------
std::string vtkPlusGetImageCommand::GetDescription(const char* commandName)
{ 
  std::string desc;
  if (commandName==NULL || STRCASECMP(commandName, GET_IMAGE_META_DATA))
  {
    desc+=GET_IMAGE_META_DATA;
    desc+=": Acquire the image meta data information from all the devices that are connected.";
  }
  if (commandName==NULL || STRCASECMP(commandName, GET_IMAGE))
  {
    desc+=GET_IMAGE;
    desc+="Acquire the volume data and the ijkToRas transformation of the data from the specified device.";
  }
  return desc;
}
//----------------------------------------------------------------------------
void vtkPlusGetImageCommand::SetNameToGetImageMeta() { SetName(GET_IMAGE_META_DATA); }
//----------------------------------------------------------------------------
void vtkPlusGetImageCommand::SetNameToGetImage() { SetName(GET_IMAGE); }
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
PlusStatus vtkPlusGetImageCommand::Execute()
{  
  LOG_DEBUG("vtkPlusGetImageCommand::Execute: "<<(this->Name?this->Name:"(undefined)")
    <<", device: "<<(this->GetImageId()==NULL?"(undefined)":this->GetImageId()) );

  if (STRCASECMP(this->Name, GET_IMAGE_META_DATA)==0)
  {
    std::string imageIdStr(this->GetImageId());
    if(imageIdStr.size() > 0)
    {
      LOG_ERROR("The implementation of the GET_IMGMETA is not implemented for the case in which an id is specified. The id is supposed to be empty and then it will send the meta data from all connected devices")
      return PLUS_FAIL;
    }
    return this->ExecuteImageMetaReply();
  }
  else if (STRCASECMP(this->Name, GET_IMAGE)==0)
  { 
    return this->ExecuteImageReply();
  }
  LOG_ERROR("vtkPlusStealthLinkCommand::Execute: failed, unknown command name: "+std::string(this->Name));
  return PLUS_FAIL;
} 
//----------------------------------------------------------------------------
PlusStatus vtkPlusGetImageCommand::ExecuteImageReply()
{
  vtkDataCollector* dataCollector=GetDataCollector();
  if (dataCollector==NULL)
  {
    LOG_ERROR("vtkPlusGetImageCommand failed. The Datacollector is NULL");
    return PLUS_FAIL;
  }
  vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
  vtkSmartPointer<vtkMatrix4x4> ijkToRasTransform = vtkSmartPointer<vtkMatrix4x4>::New();	
  for( DeviceCollectionConstIterator it = dataCollector->GetDeviceConstIteratorBegin(); it != dataCollector->GetDeviceConstIteratorEnd(); ++it )
  {
    vtkPlusDevice* plusDevice = (*it);
    if(plusDevice->GetDeviceId() ==NULL)
    {
      LOG_ERROR("The device id is Null");
      return PLUS_FAIL;
    }
    std::string imageIdStr(this->GetImageId()); // SLD-001
    unsigned dashFound = imageIdStr.find_last_of(DeviceNameImageIdSeparator);
    if(dashFound == std::string::npos)
    {
      LOG_ERROR("ImageId has to contain a dash right after the device id");
      return PLUS_FAIL;
    }
    std::string requestedDeviceId = imageIdStr.substr(0,dashFound); //SLD
    std::string requestedImageId = imageIdStr.substr(dashFound+1); // 001
    std::string deviceIdStr(plusDevice->GetDeviceId()); // SLD
    if(requestedDeviceId.compare(deviceIdStr) == 0)
    {
      LOG_INFO("Acquiring the image from " <<  deviceIdStr);
      std::string assignedImageId("");
      if(plusDevice->GetImage(requestedImageId, assignedImageId,std::string("Ras"),imageData,ijkToRasTransform))
      {
        if(assignedImageId.compare(requestedImageId) !=0)
        {
          LOG_ERROR("The assignedImageId does not match requestedImageId");
          return PLUS_FAIL;
        }
        LOG_DEBUG("Send image to client through OpenIGTLink");
        vtkSmartPointer<vtkPlusCommandImageResponse> imageResponse=vtkSmartPointer<vtkPlusCommandImageResponse>::New();
        this->CommandResponseQueue.push_back(imageResponse);
        imageResponse->SetClientId(this->ClientId);
        imageResponse->SetImageName(this->GetImageId());
        imageResponse->SetImageData(imageData);
        imageResponse->SetImageToReferenceTransform(ijkToRasTransform);
        LOG_DEBUG("Volume sent as: " << imageResponse->GetImageName());
        return PLUS_SUCCESS;
      }
    }
  }
  LOG_ERROR("Could not find the image");
  return PLUS_FAIL;
}
//----------------------------------------------------------------------------
PlusStatus vtkPlusGetImageCommand::ExecuteImageMetaReply()
{
  vtkDataCollector* dataCollector=GetDataCollector();
  if (dataCollector==NULL)
  {
    LOG_ERROR("vtkPlusGetImageCommand failed. The Datacollector is NULL");
    return PLUS_FAIL;
  }
  vtkPlusDevice* plusDevice;
  PlusCommon::ImageMetaDataList imageMetaDataList;
  for( DeviceCollectionConstIterator it = dataCollector->GetDeviceConstIteratorBegin(); it != dataCollector->GetDeviceConstIteratorEnd(); ++it )
  {
    plusDevice = (*it);
    if(plusDevice == NULL)
    {
      LOG_ERROR("The plus device is NULL");
      return PLUS_FAIL;
    }
    if(plusDevice->GetDeviceId() == NULL)
    {
      LOG_ERROR("The plus deviceId is NULL");
      return PLUS_FAIL;
    }
    PlusCommon::ImageMetaDataList imageMetaDataListDevice;
    imageMetaDataListDevice.clear();
    plusDevice->GetImageMetaData(imageMetaDataListDevice); 
    for(PlusCommon::ImageMetaDataList::iterator it = imageMetaDataListDevice.begin(); it!= imageMetaDataListDevice.end(); it++)
    {
      if(it->Id.find(DeviceNameImageIdSeparator) != std::string::npos)
      {
        LOG_ERROR("The Id cannot contain dash");
        return PLUS_FAIL;
      }
      it->Id = std::string(plusDevice->GetDeviceId()) + DeviceNameImageIdSeparator + (*it).Id;
    }
    imageMetaDataList.splice(imageMetaDataList.end(),imageMetaDataListDevice,imageMetaDataListDevice.begin(),imageMetaDataListDevice.end());
  }
  if(imageMetaDataList.size() == 0)
  {
    LOG_INFO("There are currently no images on the devices that are connected through plus");
  }
  LOG_DEBUG("Send image meta data to client through OpenIGTLink");
  vtkSmartPointer<vtkPlusCommandImageMetaDataResponse > imageMetaDataResponse=vtkSmartPointer<vtkPlusCommandImageMetaDataResponse >::New();
  this->CommandResponseQueue.push_back(imageMetaDataResponse);
  imageMetaDataResponse->SetClientId(this->ClientId);
  imageMetaDataResponse->SetImageMetaDataItems(imageMetaDataList);
  return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------