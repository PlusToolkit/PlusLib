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

static const int MAX_NUMBER_OF_FRAMES_ADDED_PER_EXECUTE=25;

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
, TrackedVideoDeviceId(NULL)
, VolumeReconstructor(vtkSmartPointer<vtkVolumeReconstructor>::New())
, TransformRepository(vtkSmartPointer<vtkTransformRepository>::New())
, LastRecordedFrameTimestamp(0)
, EnableAddingFrames(false)
, StopReconstructionRequested(false)
, ReferencedCommandId(0)
, LiveReconstructionInProgress(false)
, ReconstructionSnapshotRequested(false)
{
}

//----------------------------------------------------------------------------
vtkPlusReconstructVolumeCommand::~vtkPlusReconstructVolumeCommand()
{
  SetInputSeqFilename(NULL);
  SetOutputVolFilename(NULL);
  SetOutputVolDeviceName(NULL);
  SetTrackedVideoDeviceId(NULL);
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
    desc+=": Start adding acquired frames to the volume. Attributes: TrackedVideoDeviceId. OutputVolFilename: name of the output volume file name (optional). OutputVolDeviceName: name of the OpenIGTLink device for the IMAGE message (optional).";
  }
  if (commandName==NULL || STRCASECMP(commandName, SUSPEND_LIVE_RECONSTRUCTION_CMD))
  {
    desc+=SUSPEND_LIVE_RECONSTRUCTION_CMD;
    desc+=": Suspend adding acquired frames to the volume. ReferencedCommandId: id of the start live reconstruction command.";
  }
  if (commandName==NULL || STRCASECMP(commandName, RESUME_LIVE_RECONSTRUCTION_CMD))
  {
    desc+=RESUME_LIVE_RECONSTRUCTION_CMD;
    desc+=": Resume adding acquired frames to the volume. ReferencedCommandId: id of the start live reconstruction command.";
  }
  if (commandName==NULL || STRCASECMP(commandName, STOP_LIVE_RECONSTRUCTION_CMD))
  {
    desc+=STOP_LIVE_RECONSTRUCTION_CMD;
    desc+=": Stop adding acquired frames to the volume, finalize reconstruction, and save/send the results. ReferencedCommandId: id of the start live reconstruction command.";
  }
  if (commandName==NULL || STRCASECMP(commandName, GET_LIVE_RECONSTRUCTION_SNAPSHOT_CMD))
  {
    desc+=GET_LIVE_RECONSTRUCTION_SNAPSHOT_CMD;
    desc+=": Request a snapshot of the live reconstruction result. ReferencedCommandId: id of the start live reconstruction command.";
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
  SetInputSeqFilename(aConfig->GetAttribute("InputSeqFilename"));
  SetOutputVolFilename(aConfig->GetAttribute("OutputVolFilename"));
  SetOutputVolDeviceName(aConfig->GetAttribute("OutputVolDeviceName"));
  SetTrackedVideoDeviceId(aConfig->GetAttribute("TrackedVideoDeviceId"));
  aConfig->GetScalarAttribute("ReferencedCommandId",this->ReferencedCommandId);     
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusReconstructVolumeCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{  
  if (vtkPlusCommand::WriteConfiguration(aConfig)!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
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
  if (this->TrackedVideoDeviceId!=NULL)
  {
    aConfig->SetAttribute("TrackedVideoDeviceId",this->TrackedVideoDeviceId);
  }
  if (this->ReferencedCommandId!=0)
  {
    aConfig->SetIntAttribute("ReferencedCommandId",this->ReferencedCommandId);     
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusReconstructVolumeCommand::Execute()
{
  if (this->Name==NULL)
  {
    LOG_ERROR("Command failed, no command name specified");
    SetCommandCompleted(PLUS_FAIL,"Command failed, no command name specified");
    return PLUS_FAIL;
  }

  LOG_INFO("vtkPlusReconstructVolumeCommand::Execute: "<<this->Name);
  if (STRCASECMP(this->Name, RECONSTRUCT_PRERECORDED_CMD)==0)
  {    
    // Read image sequence
    if (this->InputSeqFilename==NULL)
    {
      LOG_INFO("vtkPlusReconstructVolumeCommand::Execute: failed, InputSeqFilename has not been defined");    
      SetCommandCompleted(PLUS_FAIL,"Volume reconstruction failed, InputSeqFilename has not been defined");
      return PLUS_FAIL;
    }
    vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
    std::string inputImageSeqFileFullPath=vtkPlusConfig::GetInstance()->GetOutputPath(this->InputSeqFilename);
    if (trackedFrameList->ReadFromSequenceMetafile(inputImageSeqFileFullPath.c_str())==PLUS_FAIL)
    {
      LOG_INFO("vtkPlusReconstructVolumeCommand::Execute: failed, unable to open input file: "<<inputImageSeqFileFullPath);
      SetCommandCompleted(PLUS_FAIL,"Volume reconstruction failed, unable to open input file specified in InputSeqFilename");
      return PLUS_FAIL;
    } 

    // Set up volume reconstructor
    if (InitializeReconstruction()!=PLUS_SUCCESS)
    {
      LOG_INFO("vtkPlusReconstructVolumeCommand::Execute: failed, InitializeReconstruction error");    
      SetCommandCompleted(PLUS_FAIL,"Volume reconstruction failed, InitializeReconstruction error");
      return PLUS_FAIL;
    }
    // Determine volume extents automatically
    if ( this->VolumeReconstructor->SetOutputExtentFromFrameList(trackedFrameList, this->TransformRepository) != PLUS_SUCCESS )
    {
      LOG_INFO("vtkPlusReconstructVolumeCommand::Execute: failed, Failed to set output extent of volume" ); 
      SetCommandCompleted(PLUS_FAIL,"vtkPlusReconstructVolumeCommand::Execute: failed, image or reference coordinate frame name is invalid");
      return PLUS_FAIL;
    }
    // Paste slices
    AddFrames(trackedFrameList);
    this->Completed=true;
    return SendReconstructionResults();
  }
  else if (STRCASECMP(this->Name, START_LIVE_RECONSTRUCTION_CMD)==0)
  {
    vtkPlusDevice* trackedVideoDevice(NULL);
    vtkPlusChannel* trackedVideoChannel(NULL);
    vtkDataCollector* dataCollector=this->GetDataCollector();
    if (dataCollector!=NULL)
    {
      dataCollector->GetDevice(trackedVideoDevice, this->TrackedVideoDeviceId);
    }
    if( trackedVideoDevice != NULL )
    {
      // In the future volume reconstructor will appear in the config file as a separate device, which receive data from a preset output channel (see https://www.assembla.com/spaces/plus/tickets/739-create-volume-reconstructor-device)
      // So, it is enough to identify it by device name.
      // Until this is implemented, the user specifies the data producer device ID and the reconstructor algorithm will capture the first channel of this producer device.
      trackedVideoChannel = *(trackedVideoDevice->GetOutputChannelsStart());
    }

    if (!this->LiveReconstructionInProgress)
    {
      // This is the first call of Execute (as reconstruction is not yet in progress)
      if (trackedVideoDevice==NULL)
      {
        LOG_ERROR("Cannot start reconstruction, tracked video device with id "<<(this->TrackedVideoDeviceId==NULL?"NULL":this->TrackedVideoDeviceId)<<" is not found");
        SetCommandCompleted(PLUS_FAIL,"Volume reconstruction failed, TrackedVideoDevice with the specified ID is not found");
        return PLUS_FAIL;
      }
      if (InitializeReconstruction()!=PLUS_SUCCESS)
      {
        LOG_INFO("vtkPlusReconstructVolumeCommand::Execute: failed, InitializeReconstruction error");    
        SetCommandCompleted(PLUS_FAIL,"Volume reconstruction failed, InitializeReconstruction error");
        return PLUS_FAIL;
      }
      SetEnableAddingFrames(true);
      this->LiveReconstructionInProgress=true;
      std::string replyDeviceName=vtkPlusCommand::GetDefaultReplyDeviceName(this->DeviceName);
      this->CommandProcessor->QueueReply(this->ClientId, PLUS_SUCCESS, "Live volume reconstruction started", replyDeviceName);
      // keep the command active until stop requested
    }
    // Live reconstruction is in progress
    if (this->StopReconstructionRequested)
    {
      // stop requested
      SetEnableAddingFrames(false);
      this->LiveReconstructionInProgress=false;
      this->Completed=true;
      return SendReconstructionResults();
    }
    if (this->ReconstructionSnapshotRequested)
    {
      this->ReconstructionSnapshotRequested=false;
      SendReconstructionResults();
    }
    // Add frames
    if (this->EnableAddingFrames)
    {
      if (trackedVideoChannel!=NULL)
      {
        vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
        trackedVideoChannel->GetTrackedFrameList(this->LastRecordedFrameTimestamp, trackedFrameList, MAX_NUMBER_OF_FRAMES_ADDED_PER_EXECUTE);
        AddFrames(trackedFrameList);
      }
      else
      {
        LOG_ERROR("Cannot add frames for live volume reconstruction, tracked video device with id "<<(this->TrackedVideoDeviceId==NULL?"NULL":this->TrackedVideoDeviceId)<<" is not found or does not have any output channels");
      }
    }
    // the command is not completed yet (this->Active is still true), so Execute will be called again
    return PLUS_SUCCESS;
  }
  else
  {    
    vtkPlusReconstructVolumeCommand* referencedCommand=vtkPlusReconstructVolumeCommand::SafeDownCast(this->CommandProcessor->GetQueuedCommand(this->ClientId, this->ReferencedCommandId));
    if (referencedCommand==NULL)
    {
      LOG_ERROR("vtkPlusReconstructVolumeCommand::Execute: failed, no active vtkPlusReconstructVolumeCommand command found with Id="<<this->ReferencedCommandId);    
      SetCommandCompleted(PLUS_FAIL,"Volume reconstruction failed, referenced volume reconstruction command not found");
      return PLUS_FAIL;
    }    
    if (STRCASECMP(this->Name, SUSPEND_LIVE_RECONSTRUCTION_CMD)==0)
    {    
      referencedCommand->SetEnableAddingFrames(false);
    }
    else if (STRCASECMP(this->Name, RESUME_LIVE_RECONSTRUCTION_CMD)==0)
    {    
      referencedCommand->SetEnableAddingFrames(true);
    }
    else if (STRCASECMP(this->Name, STOP_LIVE_RECONSTRUCTION_CMD)==0)
    {    
      referencedCommand->SetStopReconstructionRequested(true);
    }     
    else if (STRCASECMP(this->Name, GET_LIVE_RECONSTRUCTION_SNAPSHOT_CMD)==0)
    {    
      referencedCommand->SetReconstructionSnapshotRequested(true);
    }         
    else
    {
      LOG_ERROR("vtkPlusReconstructVolumeCommand::Execute: failed, unknown command name "<<this->Name);    
      SetCommandCompleted(PLUS_FAIL,"Volume reconstruction failed, unknown command name");
      return PLUS_FAIL;
    }
    SetCommandCompleted(PLUS_SUCCESS,"Live volume reconstruction control command processing completed");
    return PLUS_SUCCESS;
  }
} 

//----------------------------------------------------------------------------
PlusStatus vtkPlusReconstructVolumeCommand::InitializeReconstruction()
{
  // Create a copy of the transform repository to allow using it for volume reconstruction while being also used in other threads
  vtkTransformRepository* transformRepositoryShared = this->CommandProcessor->GetPlusServer()->GetTransformRepository();
  this->TransformRepository->DeepCopy(transformRepositoryShared);    

  // Initialize reconstructor from the XML configuration
  vtkXMLDataElement* configRootElement = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData();
  if (this->VolumeReconstructor->ReadConfiguration(configRootElement)!=PLUS_SUCCESS)
  {
    LOG_ERROR("vtkPlusReconstructVolumeCommand::Execute: failed, could not read VolumeReconstruction element in the XML tree");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusReconstructVolumeCommand::AddFrames(vtkTrackedFrameList* trackedFrameList)
{
  PlusStatus status=PLUS_SUCCESS;
  const int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames(); 
  int numberOfFramesAddedToVolume=0; 
  for ( int frameIndex = 0; frameIndex < numberOfFrames; frameIndex+=this->VolumeReconstructor->GetSkipInterval() )
  {
    LOG_TRACE("Adding frame to volume reconstructor: "<<frameIndex);
    TrackedFrame* frame = trackedFrameList->GetTrackedFrame( frameIndex );
    if ( this->TransformRepository->SetTransforms(*frame) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to update transform repository with frame #" << frameIndex ); 
      status=PLUS_FAIL;
      continue; 
    }
    // Insert slice for reconstruction
    bool insertedIntoVolume=false;
    if ( this->VolumeReconstructor->AddTrackedFrame(frame, this->TransformRepository, &insertedIntoVolume ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to add tracked frame to volume with frame #" << frameIndex); 
      status=PLUS_FAIL;
      continue; 
    }
    if ( insertedIntoVolume )
    {
      numberOfFramesAddedToVolume++; 
    }
  }
  trackedFrameList->Clear(); 

  LOG_DEBUG("Number of frames added to the volume: " << numberOfFramesAddedToVolume << " out of " << numberOfFrames ); 
  
  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusReconstructVolumeCommand::SendReconstructionResults()
{
  if (this->OutputVolFilename!=NULL)
  {
    std::string outputVolFileFullPath=vtkPlusConfig::GetInstance()->GetOutputPath(this->OutputVolFilename);
    LOG_DEBUG("Saving volume to file: "<<outputVolFileFullPath);
    this->VolumeReconstructor->SaveReconstructedVolumeToMetafile(outputVolFileFullPath.c_str());
  }    
  if (this->OutputVolDeviceName!=NULL)
  {    
    // send the reconstructed volume with the reply
    LOG_DEBUG("Send image to client through OpenIGTLink");
    vtkImageData* volumeToSend = vtkImageData::New(); // will be deleted by the command processor
    if (this->VolumeReconstructor->ExtractGrayLevels(volumeToSend) != PLUS_SUCCESS)
    {
      LOG_ERROR("Extracting gray levels failed!");
      return PLUS_FAIL;
    }
    vtkMatrix4x4* volumeToReferenceTransform=vtkMatrix4x4::New(); // will be deleted by the command processor
    volumeToReferenceTransform->Identity(); // we leave it as identity, as the volume coordinate system is the same as the reference coordinate system (we may extend this later so that the client can request the volume in any coordinate system)

    this->CommandProcessor->QueueReply(this->ClientId, PLUS_SUCCESS, "Volume reconstruction completed, image sent to the client", vtkPlusCommand::GetDefaultReplyDeviceName(this->DeviceName), this->OutputVolDeviceName, volumeToSend, volumeToReferenceTransform);
  }
  else
  {
    // send only a status reply
    this->CommandProcessor->QueueReply(this->ClientId, PLUS_SUCCESS, "Volume reconstruction completed", vtkPlusCommand::GetDefaultReplyDeviceName(this->DeviceName));
  }
  LOG_INFO("Volume reconstruction results are sent to disk/client");
  return PLUS_SUCCESS;
}
