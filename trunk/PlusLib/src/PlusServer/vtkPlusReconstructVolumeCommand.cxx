/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "vtkDataCollector.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusCommandProcessor.h"
#include "vtkPlusReconstructVolumeCommand.h"
#include "vtkTrackedFrameList.h"
#include "vtkTransformRepository.h"
#include "vtkVolumeReconstructor.h"
#include "vtkVirtualVolumeReconstructor.h"

static const int MAX_NUMBER_OF_FRAMES_ADDED_PER_EXECUTE=50;
static const double SKIPPED_PERIOD_REPORTING_THRESHOLD_SEC=0.2; // log a warning if volume reconstruction cannot keep up with the acquisition and skips more than this time period of acquired frames

static const char RECONSTRUCT_PRERECORDED_CMD[]="ReconstructVolume";
static const char START_LIVE_RECONSTRUCTION_CMD[]="StartVolumeReconstruction";
static const char SUSPEND_LIVE_RECONSTRUCTION_CMD[]="SuspendVolumeReconstruction";
static const char RESUME_LIVE_RECONSTRUCTION_CMD[]="ResumeVolumeReconstruction";
static const char STOP_LIVE_RECONSTRUCTION_CMD[]="StopVolumeReconstruction";
static const char GET_LIVE_RECONSTRUCTION_SNAPSHOT_CMD[]="GetVolumeReconstructionSnapshot";

vtkStandardNewMacro( vtkPlusReconstructVolumeCommand );

//----------------------------------------------------------------------------
vtkPlusReconstructVolumeCommand::vtkPlusReconstructVolumeCommand()
: InputSeqFilename(NULL)
, OutputVolFilename(NULL)
, OutputVolDeviceName(NULL)
, VolumeReconstructorDeviceId(NULL)
{
}

//----------------------------------------------------------------------------
vtkPlusReconstructVolumeCommand::~vtkPlusReconstructVolumeCommand()
{
  SetInputSeqFilename(NULL);
  SetOutputVolFilename(NULL);
  SetOutputVolDeviceName(NULL);
  SetVolumeReconstructorDeviceId(NULL);
}

//----------------------------------------------------------------------------
void vtkPlusReconstructVolumeCommand::SetNameToReconstruct() { SetName(RECONSTRUCT_PRERECORDED_CMD); }
void vtkPlusReconstructVolumeCommand::SetNameToStart() { SetName(START_LIVE_RECONSTRUCTION_CMD); }
void vtkPlusReconstructVolumeCommand::SetNameToStop() { SetName(STOP_LIVE_RECONSTRUCTION_CMD); }
void vtkPlusReconstructVolumeCommand::SetNameToSuspend() { SetName(SUSPEND_LIVE_RECONSTRUCTION_CMD); }
void vtkPlusReconstructVolumeCommand::SetNameToResume() { SetName(RESUME_LIVE_RECONSTRUCTION_CMD); }
void vtkPlusReconstructVolumeCommand::SetNameToGetSnapshot() { SetName(GET_LIVE_RECONSTRUCTION_SNAPSHOT_CMD); }

//----------------------------------------------------------------------------
void vtkPlusReconstructVolumeCommand::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
void vtkPlusReconstructVolumeCommand::GetCommandNames(std::list<std::string> &cmdNames)
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
std::string vtkPlusReconstructVolumeCommand::GetDescription(const char* commandName)
{ 
  std::string desc;
  if (commandName==NULL || STRCASECMP(commandName, RECONSTRUCT_PRERECORDED_CMD))
  {
    desc+=RECONSTRUCT_PRERECORDED_CMD;
    desc+=": Reconstruct a volume from a file and writes the result to a file. Attributes: InputSeqFilename: name of the input sequence metafile name that contains the list of frames. OutputVolFilename: name of the output volume file name (optional). OutputVolDeviceName: name of the OpenIGTLink device for the IMAGE message (optional).";
  }
  if (commandName==NULL || STRCASECMP(commandName, START_LIVE_RECONSTRUCTION_CMD))
  {
    desc+=START_LIVE_RECONSTRUCTION_CMD;
    desc+=": Start adding acquired frames to the volume. Attributes: VolumeReconstructorDeviceId: ID of the volume reconstructor device. OutputVolFilename: name of the output volume file name (optional). OutputVolDeviceName: name of the OpenIGTLink device for the IMAGE message (optional).";
  }
  if (commandName==NULL || STRCASECMP(commandName, SUSPEND_LIVE_RECONSTRUCTION_CMD))
  {
    desc+=SUSPEND_LIVE_RECONSTRUCTION_CMD;
    desc+=": Suspend adding acquired frames to the volume. Attributes: VolumeReconstructorDeviceId: ID of the volume reconstructor device.";
  }
  if (commandName==NULL || STRCASECMP(commandName, RESUME_LIVE_RECONSTRUCTION_CMD))
  {
    desc+=RESUME_LIVE_RECONSTRUCTION_CMD;
    desc+=": Resume adding acquired frames to the volume. Attributes: VolumeReconstructorDeviceId: ID of the volume reconstructor device.";
  }
  if (commandName==NULL || STRCASECMP(commandName, STOP_LIVE_RECONSTRUCTION_CMD))
  {
    desc+=STOP_LIVE_RECONSTRUCTION_CMD;
    desc+=": Stop adding acquired frames to the volume, finalize reconstruction, and save/send the results. Attributes: VolumeReconstructorDeviceId: ID of the volume reconstructor device.";
  }
  if (commandName==NULL || STRCASECMP(commandName, GET_LIVE_RECONSTRUCTION_SNAPSHOT_CMD))
  {
    desc+=GET_LIVE_RECONSTRUCTION_SNAPSHOT_CMD;
    desc+=": Request a snapshot of the live reconstruction result. Attributes: VolumeReconstructorDeviceId: ID of the volume reconstructor device.";
  }
  
  return desc;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusReconstructVolumeCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{  
  if (vtkPlusCommand::ReadConfiguration(aConfig)!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  this->SetInputSeqFilename(aConfig->GetAttribute("InputSeqFilename"));
  this->SetOutputVolFilename(aConfig->GetAttribute("OutputVolFilename"));
  this->SetOutputVolDeviceName(aConfig->GetAttribute("OutputVolDeviceName"));
  this->SetVolumeReconstructorDeviceId(aConfig->GetAttribute("VolumeReconstructorDeviceId"));
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusReconstructVolumeCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{  
  if (vtkPlusCommand::WriteConfiguration(aConfig)!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  if (this->VolumeReconstructorDeviceId!=NULL)
  {
    aConfig->SetAttribute("VolumeReconstructorDeviceId",this->VolumeReconstructorDeviceId);     
  }
  if (this->InputSeqFilename!=NULL)
  {
    aConfig->SetAttribute("InputSeqFilename",this->InputSeqFilename);
  }
  if (this->OutputVolFilename!=NULL)
  {
    aConfig->SetAttribute("OutputVolFilename",this->OutputVolFilename);
  }
  if (this->OutputVolDeviceName!=NULL)
  {
    aConfig->SetAttribute("OutputVolDeviceName",this->OutputVolDeviceName);
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusReconstructVolumeCommand::Execute()
{  
  std::string message;

  LOG_DEBUG("vtkPlusReconstructVolumeCommand::Execute: "<<(this->Name?this->Name:"(undefined)")
    <<", device: "<<(this->VolumeReconstructorDeviceId==NULL?"(undefined)":this->VolumeReconstructorDeviceId) );

  if (this->Name==NULL)
  {
    message="Volume reconstruction command failed, no command name specified";
    LOG_ERROR(message);
    SetCommandCompleted(PLUS_FAIL,message);
    return PLUS_FAIL;
  }
  
  vtkSmartPointer<vtkImageData> volumeToSend=vtkSmartPointer<vtkImageData>::New();

  vtkVirtualVolumeReconstructor* reconstructorDevice=GetVolumeReconstructorDevice();
  if (reconstructorDevice==NULL)
  {
    message=std::string("Volume reconstruction command failed: device ")
      +(this->VolumeReconstructorDeviceId==NULL?"(undefined)":this->VolumeReconstructorDeviceId)+" is not found";
    LOG_ERROR(message);
    SetCommandCompleted(PLUS_FAIL,message);
    return PLUS_FAIL;
  }

  std::string reconstructorDeviceId=(reconstructorDevice->GetDeviceId()==NULL?"(unknown)":reconstructorDevice->GetDeviceId());
  
  if (STRCASECMP(this->Name, RECONSTRUCT_PRERECORDED_CMD)==0)
  {
    LOG_INFO("Volume reconstruction from sequence file: "<<(this->InputSeqFilename?this->InputSeqFilename:"(undefined), device: ")<<reconstructorDeviceId);
    if (reconstructorDevice->UpdateTransformRepository(this->CommandProcessor->GetPlusServer()->GetTransformRepository())!=PLUS_SUCCESS)
    {
      message="Volume reconstruction from sequence file failed: cannot get transform repository, device: "+reconstructorDeviceId;
      LOG_ERROR(message);
      SetCommandCompleted(PLUS_FAIL,message);
      return PLUS_FAIL;
    }
    reconstructorDevice->Reset(); // Clear volume
    if (reconstructorDevice->GetReconstructedVolumeFromFile(this->InputSeqFilename, volumeToSend, message)!=PLUS_SUCCESS)
    {
      SetCommandCompleted(PLUS_FAIL,message);
    }
    return SetCommandCompletedWithImage(volumeToSend,"Volume reconstruction from sequence file completed");
  }
  else if (STRCASECMP(this->Name, START_LIVE_RECONSTRUCTION_CMD)==0)
  {    
    LOG_INFO("Volume reconstruction from live frames starting, device: "<<reconstructorDeviceId);
    if (reconstructorDevice->UpdateTransformRepository(this->CommandProcessor->GetPlusServer()->GetTransformRepository())!=PLUS_SUCCESS)
    {
      message="Volume reconstruction starting from live frames failed: cannot get transform repository, device: "+reconstructorDeviceId;
      LOG_ERROR(message);
      SetCommandCompleted(PLUS_FAIL,message);
      return PLUS_FAIL;
    }
    reconstructorDevice->Reset(); // Clear volume
    reconstructorDevice->SetEnableReconstruction(true);
    message="Volume reconstruction from live frames started, device: "+reconstructorDeviceId;
    return SetCommandCompleted(PLUS_SUCCESS,message);
  }
  else if (STRCASECMP(this->Name, STOP_LIVE_RECONSTRUCTION_CMD)==0)
  {
    LOG_INFO("Volume reconstruction from live frames stopping, device: "<<reconstructorDeviceId);
    reconstructorDevice->SetEnableReconstruction(false);
    if (reconstructorDevice->GetReconstructedVolume(volumeToSend, message)!=PLUS_SUCCESS)
    {
      SetCommandCompleted(PLUS_FAIL,message);
    }
    reconstructorDevice->Reset(); // Clear volume
    return SetCommandCompletedWithImage(volumeToSend,"Volume reconstruction from live frames stopped");
  }     
  else if (STRCASECMP(this->Name, GET_LIVE_RECONSTRUCTION_SNAPSHOT_CMD)==0)
  {    
    LOG_INFO("Volume reconstruction from live frames snapshot request, device: "<<reconstructorDeviceId);
    if (reconstructorDevice->GetReconstructedVolume(volumeToSend, message)!=PLUS_SUCCESS)
    {
      SetCommandCompleted(PLUS_FAIL,message);
    }
    return SetCommandCompletedWithImage(volumeToSend,"Volume reconstruction snapshot created");
  }
  else if (STRCASECMP(this->Name, SUSPEND_LIVE_RECONSTRUCTION_CMD)==0)
  {
    LOG_INFO("Volume reconstruction from live frames suspending, device: "<<reconstructorDeviceId);
    reconstructorDevice->SetEnableReconstruction(false);
    message="Volume reconstruction from live frames suspended, device: "+reconstructorDeviceId;
    return SetCommandCompleted(PLUS_SUCCESS,message);
  }
  else if (STRCASECMP(this->Name, RESUME_LIVE_RECONSTRUCTION_CMD)==0)
  {
    LOG_INFO("Volume reconstruction from live frames resuming, device: "<<reconstructorDeviceId);
    reconstructorDevice->SetEnableReconstruction(true);
    message="Volume reconstruction from live frames resumed, device: "+reconstructorDeviceId;
    return SetCommandCompleted(PLUS_SUCCESS,message);
  }

  message=std::string("vtkPlusReconstructVolumeCommand::Execute: failed, unknown command name ")+this->Name;
  LOG_ERROR(message);
  SetCommandCompleted(PLUS_FAIL,message);
  return PLUS_FAIL;
} 

//----------------------------------------------------------------------------
PlusStatus vtkPlusReconstructVolumeCommand::SetCommandCompletedWithImage(vtkImageData* volumeToSend, const std::string &message)
{
  std::string reply=message;
  if (this->OutputVolFilename != NULL)
  {
    std::string outputVolFileFullPath = vtkPlusConfig::GetInstance()->GetOutputPath(this->OutputVolFilename);
    LOG_INFO("Saving reconstructed volume to file: " << outputVolFileFullPath);
    reply+=std::string(", saved reconstructed volume to file: ")+outputVolFileFullPath;
    vtkVolumeReconstructor::SaveReconstructedVolumeToMetafile(volumeToSend, outputVolFileFullPath.c_str());
  }    
  if (this->OutputVolDeviceName != NULL)
  {    
    // send the reconstructed volume with the reply
    LOG_DEBUG("Send image to client through OpenIGTLink");
    vtkMatrix4x4* volumeToReferenceTransform = vtkMatrix4x4::New(); // will be deleted by the command processor
    volumeToReferenceTransform->Identity(); // we leave it as identity, as the volume coordinate system is, the same as the reference coordinate system (we may extend this later so that the client can request the volume in any coordinate system)
    LOG_INFO("Send reconstructed volume to client through OpenIGTLink");
    reply+=std::string(", image sent as: ")+(this->OutputVolDeviceName?this->OutputVolDeviceName:"(undefined)");
    volumeToSend->Register(NULL); // make sure the image is not deleted until the command processor deletes it
    this->CommandProcessor->QueueReply(this->ClientId, PLUS_SUCCESS, reply, this->GetReplyDeviceName(), this->OutputVolDeviceName, volumeToSend, volumeToReferenceTransform);
  }
  else
  {
    // send only a status reply
    this->CommandProcessor->QueueReply(this->ClientId, PLUS_SUCCESS, reply, this->GetReplyDeviceName());
  }  
  this->Completed=true;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
vtkVirtualVolumeReconstructor* vtkPlusReconstructVolumeCommand::GetVolumeReconstructorDevice()
{
  vtkDataCollector* dataCollector=GetDataCollector();
  if (dataCollector==NULL)
  {
    LOG_ERROR("Data collector is invalid");    
    return NULL;
  }
  vtkVirtualVolumeReconstructor *reconstructorDevice=NULL;
  if (this->VolumeReconstructorDeviceId!=NULL)
  {
    // Reconstructor device ID is specified
    vtkPlusDevice* device=NULL;
    if (dataCollector->GetDevice(device, this->VolumeReconstructorDeviceId)!=PLUS_SUCCESS)
    {
      LOG_ERROR("No volume reconstructor device has been found by the name "<<this->VolumeReconstructorDeviceId);
      return NULL;
    }
    // device found
    reconstructorDevice = vtkVirtualVolumeReconstructor::SafeDownCast(device);
    if (reconstructorDevice==NULL)
    {
      // wrong type
      LOG_ERROR("The specified device "<<this->VolumeReconstructorDeviceId<<" is not VirtualVolumeReconstructorDevice");
      return NULL;
    }
  }
  else
  {
    // No volume reconstruction device id is specified, auto-detect the first one and use that
    for( DeviceCollectionConstIterator it = dataCollector->GetDeviceConstIteratorBegin(); it != dataCollector->GetDeviceConstIteratorEnd(); ++it )
    {
      reconstructorDevice = vtkVirtualVolumeReconstructor::SafeDownCast(*it);
      if (reconstructorDevice!=NULL)
      {      
        // found a recording device
        break;
      }
    }
    if (reconstructorDevice==NULL)
    {
      LOG_ERROR("No VirtualVolumeReconstructor has been found");
      return NULL;
    }
  }  
  return reconstructorDevice;
}
