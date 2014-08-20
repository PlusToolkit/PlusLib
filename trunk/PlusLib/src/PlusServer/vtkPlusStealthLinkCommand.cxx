/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "vtkDataCollector.h"
#include "vtkPlusStealthLinkCommand.h"
#include "StealthLink/vtkStealthLinkTracker.h"

#include "vtkImageData.h"
#include "vtkDICOMImageReader.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusCommandProcessor.h"
#include "vtkTrackedFrameList.h"
#include "vtkVolumeReconstructor.h"
#include "vtkVirtualVolumeReconstructor.h"
#include <vtkImageFlip.h>
#include <vtkPointData.h>
#include <vtkDirectory.h>

static const char GET_STEALTHLINK_EXAM_DATA_CMD[]="ExamData";

vtkStandardNewMacro( vtkPlusStealthLinkCommand );

//----------------------------------------------------------------------------
vtkPlusStealthLinkCommand::vtkPlusStealthLinkCommand()
: StealthLinkDeviceId(NULL)
, DicomImagesOutputDirectory(NULL)
, VolumeEmbeddedTransformToFrame(NULL)
, KeepReceivedDicomFiles(false)
{
}

//----------------------------------------------------------------------------
vtkPlusStealthLinkCommand::~vtkPlusStealthLinkCommand()
{
  SetStealthLinkDeviceId(NULL);
  SetDicomImagesOutputDirectory(NULL);
  SetVolumeEmbeddedTransformToFrame(NULL);
}

//----------------------------------------------------------------------------
void vtkPlusStealthLinkCommand::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
void vtkPlusStealthLinkCommand::GetCommandNames(std::list<std::string> &cmdNames)
{ 
  cmdNames.clear(); 
  cmdNames.push_back(GET_STEALTHLINK_EXAM_DATA_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusStealthLinkCommand::GetDescription(const char* commandName)
{ 
  std::string desc;
  if (commandName==NULL || STRCASECMP(commandName, GET_STEALTHLINK_EXAM_DATA_CMD))
  {
    desc+=GET_STEALTHLINK_EXAM_DATA_CMD;
    desc+=": Acquire the exam data from the StealthLink Server. The exam data contains the image being displayed on the StealthLink Server. The 3D volume will be constructed using these images";
  }
  return desc;
}
//----------------------------------------------------------------------------
void vtkPlusStealthLinkCommand::SetNameToGetExam() { SetName(GET_STEALTHLINK_EXAM_DATA_CMD); }
//----------------------------------------------------------------------------
void vtkPlusStealthLinkCommand::SetKeepReceivedDicomFiles(bool keepReceivedDicomFiles) { this->KeepReceivedDicomFiles = keepReceivedDicomFiles; }
//----------------------------------------------------------------------------
bool vtkPlusStealthLinkCommand::GetKeepReceivedDicomFiles() { return this->KeepReceivedDicomFiles; }
//----------------------------------------------------------------------------
PlusStatus vtkPlusStealthLinkCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{  
  if (vtkPlusCommand::ReadConfiguration(aConfig)!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  if(aConfig->GetAttribute("DicomImagesOutputDirectory"))
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

  if(aConfig->GetAttribute("VolumeEmbeddedTransformToFrame"))
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
  if(vtkPlusCommand::WriteConfiguration(aConfig)!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_NULL(StealthLinkDeviceId, aConfig);
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_NULL(DicomImagesOutputDirectory, aConfig);
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_NULL(VolumeEmbeddedTransformToFrame, aConfig);
  XML_WRITE_BOOL_ATTRIBUTE(KeepReceivedDicomFiles, aConfig);
  return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
PlusStatus vtkPlusStealthLinkCommand::Execute()
{  
  LOG_DEBUG("vtkPlusStealthLinkCommand::Execute: "<<(this->Name?this->Name:"(undefined)")
    <<", device: "<<(this->StealthLinkDeviceId==NULL?"(undefined)":this->StealthLinkDeviceId) );

  if (this->Name==NULL)
  {
    this->QueueStringResponse("StealthLink command failed, no command name specified",PLUS_FAIL);
    return PLUS_FAIL;
  }

  vtkStealthLinkTracker* stealthLinkDevice = GetStealthLinkDevice();
  if (stealthLinkDevice==NULL)
  {
    this->QueueStringResponse(std::string("StealthLink command failed: device ")
      +(this->StealthLinkDeviceId==NULL?"(undefined)":this->StealthLinkDeviceId)+" is not found",PLUS_FAIL);
    return PLUS_FAIL;
  }

  if (STRCASECMP(this->Name, GET_STEALTHLINK_EXAM_DATA_CMD)==0)
  {
    LOG_INFO("Acquiring the exam data from StealthLink Server: Device ID: "<<GetStealthLinkDeviceId());
    
    stealthLinkDevice->SetDicomImagesOutputDirectory(this->GetDicomImagesOutputDirectory());
    stealthLinkDevice->SetKeepReceivedDicomFiles(this->GetKeepReceivedDicomFiles());
    std::string requestedImageId;
    std::string assignedImageId;
    vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
    vtkSmartPointer<vtkMatrix4x4> ijkToReferenceTransform = vtkSmartPointer<vtkMatrix4x4>::New();
    // make available all transforms to the device so that it can compute ijkToReferenceTransform (Reference = VolumeEmbeddedTransformToFrame)
    stealthLinkDevice->UpdateTransformRepository(this->CommandProcessor->GetPlusServer()->GetTransformRepository());
    if (stealthLinkDevice->GetImage(requestedImageId, assignedImageId, std::string(this->GetVolumeEmbeddedTransformToFrame()),imageData,ijkToReferenceTransform)!=PLUS_SUCCESS)
    {
      this->QueueStringResponse("vtkPlusStealthLinkCommand::Execute: failed, failed to receive image",PLUS_FAIL);
      return PLUS_FAIL;
    }    
    std::string resultMessage;
    PlusStatus status = ProcessImageReply(assignedImageId,imageData,ijkToReferenceTransform,resultMessage);
    this->QueueStringResponse("Volume sending completed: "+resultMessage,status);
    return PLUS_SUCCESS;
  }
  this->QueueStringResponse("vtkPlusStealthLinkCommand::Execute: failed, unknown command name: "+std::string(this->Name),PLUS_FAIL);
  return PLUS_FAIL;
} 
//----------------------------------------------------------------------------
PlusStatus vtkPlusStealthLinkCommand::ProcessImageReply(const std::string& imageId, vtkImageData* volumeToSend,vtkMatrix4x4* imageToReferenceOrientationMatrix,std::string& resultMessage)
{
  LOG_DEBUG("Send image to client through OpenIGTLink");
  vtkSmartPointer<vtkPlusCommandImageResponse> imageResponse=vtkSmartPointer<vtkPlusCommandImageResponse>::New();
  this->CommandResponseQueue.push_back(imageResponse);
  imageResponse->SetClientId(this->ClientId);
  imageResponse->SetImageName(imageId);
  imageResponse->SetImageData(volumeToSend);
  imageResponse->SetImageToReferenceTransform(imageToReferenceOrientationMatrix);
  LOG_INFO("Send reconstructed volume to client through OpenIGTLink");
  resultMessage.clear();
  resultMessage=std::string(", volume sent as: ")+imageResponse->GetImageName();
  return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
vtkStealthLinkTracker* vtkPlusStealthLinkCommand::GetStealthLinkDevice()
{
  vtkDataCollector* dataCollector=GetDataCollector();
  if (dataCollector==NULL)
  {
    LOG_ERROR("Data collector is invalid");    
    return NULL;
  }
  if (GetStealthLinkDeviceId()!=NULL)
  {
    // Reconstructor device ID is specified
    vtkPlusDevice* device=NULL;
    if (dataCollector->GetDevice(device, GetStealthLinkDeviceId())!=PLUS_SUCCESS)
    {
      LOG_ERROR("No StealthLink device has been found by the name "<<this->GetStealthLinkDeviceId());
      return NULL;
    }
    // device found
    vtkStealthLinkTracker *stealthLinkDevice = vtkStealthLinkTracker::SafeDownCast(device);
    if (stealthLinkDevice==NULL)
    {
      // wrong type
      LOG_ERROR("The specified device "<<GetStealthLinkDeviceId()<<" is not StealthLink Device");
      return NULL;
    }
    return stealthLinkDevice;
  }
  else
  {
    // No stealthlink device id is specified, auto-detect the first one and use that
    for( DeviceCollectionConstIterator it = dataCollector->GetDeviceConstIteratorBegin(); it != dataCollector->GetDeviceConstIteratorEnd(); ++it )
    {
      vtkStealthLinkTracker *stealthLinkDevice = vtkStealthLinkTracker::SafeDownCast(*it);
      if (stealthLinkDevice!=NULL)
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
 
