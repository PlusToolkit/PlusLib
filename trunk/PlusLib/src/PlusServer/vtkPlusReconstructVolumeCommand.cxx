/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"

#include "vtkObjectFactory.h"
#include "vtkImageData.h"

#include "vtkPlusReconstructVolumeCommand.h"
#include "vtkPlusCommandProcessor.h"
#include "vtkVolumeReconstructor.h"
#include "vtkTransformRepository.h"
#include "vtkTrackedFrameList.h"

vtkStandardNewMacro( vtkPlusReconstructVolumeCommand );

//----------------------------------------------------------------------------
vtkPlusReconstructVolumeCommand::vtkPlusReconstructVolumeCommand()
: InputSeqFilename(NULL)
, OutputVolFilename(NULL)
, OutputVolDeviceName(NULL)
, VolumeReconstructor(vtkSmartPointer<vtkVolumeReconstructor>::New())
{
}

//----------------------------------------------------------------------------
vtkPlusReconstructVolumeCommand::~vtkPlusReconstructVolumeCommand()
{
  SetInputSeqFilename(NULL);
  SetOutputVolFilename(NULL);
  SetOutputVolDeviceName(NULL);
}

//----------------------------------------------------------------------------
void vtkPlusReconstructVolumeCommand::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
void vtkPlusReconstructVolumeCommand::GetCommandNames(std::list<std::string> &cmdNames)
{ 
  cmdNames.clear(); 
  cmdNames.push_back("ReconstructVolume");
}

//----------------------------------------------------------------------------
std::string vtkPlusReconstructVolumeCommand::GetDescription(const char* commandName)
{ 
  return "ReconstructVolume: Reconstructs a volume from a file and writes the result to a file. Attributes: \
         InputSeqFilename: name of the input sequence metafile name that contains the list of frames\
         OutputVolFilename: name of the output volume file name\
         ";
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
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusReconstructVolumeCommand::Execute()
{
  vtkTransformRepository* transformRepository = this->CommandProcessor->GetPlusServer()->GetTransformRepository();

  // Print calibration transform
  std::ostringstream osTransformRepo; 
  transformRepository->Print(osTransformRepo); 
  LOG_DEBUG("Transform repository: \n" << osTransformRepo.str());  

  // Read image sequence
  if (this->InputSeqFilename==NULL)
  {
    LOG_INFO("vtkPlusReconstructVolumeCommand::Execute: failed, InputSeqFilename has not been defined");    
    SetCommandCompleted(PLUS_FAIL,"Volume reconstruction failed, InputSeqFilename has not been defined");
    return PLUS_FAIL;
  }
  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
  trackedFrameList->ReadFromSequenceMetafile(this->InputSeqFilename); 

  // Reconstruct volume 

  // Initialize reconstructor from the XML configuration
  vtkXMLDataElement* configRootElement = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData();
  if (configRootElement == NULL)
  {
    LOG_INFO("vtkPlusReconstructVolumeCommand::Execute: failed, invalid configuration");
    SetCommandCompleted(PLUS_FAIL,"Volume reconstruction failed, invalid configuration");
    return PLUS_FAIL;
  }
  vtkXMLDataElement* reconConfig = configRootElement->FindNestedElementWithName("VolumeReconstruction");
  this->VolumeReconstructor->ReadConfiguration(reconConfig);
  {
    LOG_INFO("vtkPlusReconstructVolumeCommand::Execute: failed, could not read VolumeReconstruction element in the XML tree");
    SetCommandCompleted(PLUS_FAIL,"Volume reconstruction failed, could not read VolumeReconstruction element in the XML tree");
    return PLUS_FAIL;
  }
  
  if ( this->VolumeReconstructor->SetOutputExtentFromFrameList(trackedFrameList, transformRepository) != PLUS_SUCCESS )
  {
    LOG_INFO("vtkPlusReconstructVolumeCommand::Execute: failed, Failed to set output extent of volume" ); 
    SetCommandCompleted(PLUS_FAIL,"vtkPlusReconstructVolumeCommand::Execute: failed, image or reference coordinate frame name is invalid");
    return PLUS_FAIL;
  }

  const int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames(); 
  int numberOfFramesAddedToVolume=0; 
  for ( int frameIndex = 0; frameIndex < numberOfFrames; frameIndex+=this->VolumeReconstructor->GetSkipInterval() )
  {
    LOG_DEBUG("Frame: "<<frameIndex);
    TrackedFrame* frame = trackedFrameList->GetTrackedFrame( frameIndex );
    if ( transformRepository->SetTransforms(*frame) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to update transform repository with frame #" << frameIndex ); 
      continue; 
    }
    // Insert slice for reconstruction
    bool insertedIntoVolume=false;
    if ( this->VolumeReconstructor->AddTrackedFrame(frame, transformRepository, &insertedIntoVolume ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to add tracked frame to volume with frame #" << frameIndex); 
      continue; 
    }
    if ( insertedIntoVolume )
    {
      numberOfFramesAddedToVolume++; 
    }
  }
  trackedFrameList->Clear(); 
  LOG_DEBUG("Number of frames added to the volume: " << numberOfFramesAddedToVolume << " out of " << numberOfFrames ); 

  if (this->OutputVolFilename!=NULL)
  {
    LOG_DEBUG("Saving volume to file: "<<this->OutputVolFilename);
    this->VolumeReconstructor->SaveReconstructedVolumeToMetafile(this->OutputVolFilename);
  }  
  
  LOG_INFO("vtkPlusReconstructVolumeCommand::Execute: completed");
  if (this->OutputVolDeviceName!=NULL)
  {    
    LOG_DEBUG("Send image to client through OpenIGTLink");
    vtkImageData* volumeToSend = vtkImageData::New(); // will be deleted by the command processor
    if (this->VolumeReconstructor->ExtractGrayLevels(volumeToSend) != PLUS_SUCCESS)
    {
      LOG_ERROR("Extracting gray levels failed!");
      return PLUS_FAIL;
    }
    vtkMatrix4x4* originOrientationMatrix=vtkMatrix4x4::New(); // will be deleted by the command processor
    originOrientationMatrix->Element[0][3]=volumeToSend->GetOrigin()[0];
    originOrientationMatrix->Element[1][3]=volumeToSend->GetOrigin()[1];
    originOrientationMatrix->Element[2][3]=volumeToSend->GetOrigin()[2];
    this->CommandProcessor->QueueReply(this->ClientId, PLUS_SUCCESS, "Volume reconstruction completed, image sent to the client", this->OutputVolDeviceName, volumeToSend, originOrientationMatrix);
    this->Completed=true;
  }
  else
  {
    SetCommandCompleted(PLUS_SUCCESS,"Volume reconstruction completed");
  }
  return PLUS_SUCCESS;
} 
