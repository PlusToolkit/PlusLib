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

vtkStandardNewMacro( vtkPlusGetImageCommand );

//----------------------------------------------------------------------------
vtkPlusGetImageCommand::vtkPlusGetImageCommand()
: DeviceId(NULL)
{
}

//----------------------------------------------------------------------------
vtkPlusGetImageCommand::~vtkPlusGetImageCommand()
{
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
    <<", device: "<<(this->DeviceId==NULL?"(undefined)":this->DeviceId) );
	vtkDataCollector* dataCollector=GetDataCollector();
  if (dataCollector==NULL)
  {
		this->QueueStringResponse(std::string("vtkPlusGetImage command failed: device ")
      +std::string("No connected devices found"),PLUS_FAIL);
    return PLUS_FAIL;
  }

  if (STRCASECMP(this->Name, GET_IMAGE_META_DATA)==0)
  {
		return this->ProcessImageMetaReply(dataCollector);
  }
  else if (STRCASECMP(this->Name, GET_IMAGE)==0)
  { 
		return this->ProcessImageReply(dataCollector);
  }
	this->QueueStringResponse("vtkPlusStealthLinkCommand::Execute: failed, unknown command name: "+std::string(this->Name),PLUS_SUCCESS);
  return PLUS_FAIL;
} 
//----------------------------------------------------------------------------
PlusStatus vtkPlusGetImageCommand::ProcessImageReply(vtkDataCollector* dataCollector)
{
	vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
	vtkSmartPointer<vtkMatrix4x4> ijkToRasTransform = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkPlusDevice* plusDevice;
	for( DeviceCollectionConstIterator it = dataCollector->GetDeviceConstIteratorBegin(); it != dataCollector->GetDeviceConstIteratorEnd(); ++it )
	{
		plusDevice = (*it);
		LOG_INFO("Acquiring the image data from the device with DeviceId: "<<plusDevice->GetDeviceId());
		if(plusDevice->GetImage(std::string(this->GetDeviceId()), std::string(""),std::string("Ras"),imageData,ijkToRasTransform))
		{
			LOG_DEBUG("Send image to client through OpenIGTLink");
			vtkSmartPointer<vtkPlusCommandImageResponse> imageResponse=vtkSmartPointer<vtkPlusCommandImageResponse>::New();
			this->CommandResponseQueue.push_back(imageResponse);
	    imageResponse->SetClientId(this->ClientId);
	    imageResponse->SetImageName(this->GetDeviceId());
      imageResponse->SetImageData(imageData);
	    imageResponse->SetImageToReferenceTransform(ijkToRasTransform);
			LOG_DEBUG("Volume sent as: " << imageResponse->GetImageName());
      return PLUS_SUCCESS;
		}
	}
	LOG_ERROR("Could not find the image");
  return PLUS_FAIL;
}
//----------------------------------------------------------------------------
PlusStatus vtkPlusGetImageCommand::ProcessImageMetaReply(vtkDataCollector* dataCollector)
{
	vtkPlusDevice* plusDevice;
	PlusCommon::ImageMetaDataList imageMetaDataList;
	for( DeviceCollectionConstIterator it = dataCollector->GetDeviceConstIteratorBegin(); it != dataCollector->GetDeviceConstIteratorEnd(); ++it )
	{
		plusDevice = (*it);
		plusDevice->GetImageMetaData(imageMetaDataList);
	}
  if(imageMetaDataList.size() == 0)
	{
	  LOG_INFO("There are currently no images on the devices that are connected through plus");
		return PLUS_FAIL;
	}
	LOG_DEBUG("Send image meta data to client through OpenIGTLink");
	vtkSmartPointer<vtkPlusCommandImageMetaDataResponse > imageMetaDataResponse=vtkSmartPointer<vtkPlusCommandImageMetaDataResponse >::New();
	this->CommandResponseQueue.push_back(imageMetaDataResponse);
	imageMetaDataResponse->SetClientId(this->ClientId);
	imageMetaDataResponse->SetImageMetaDataItems(imageMetaDataList); 
  return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------