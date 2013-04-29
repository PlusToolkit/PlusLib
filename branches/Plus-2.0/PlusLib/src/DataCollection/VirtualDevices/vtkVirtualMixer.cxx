/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "vtkObjectFactory.h"
#include "vtkPlusBuffer.h"
#include "vtkPlusDataSource.h"
#include "vtkVirtualMixer.h"

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkVirtualMixer, "$Revision: 1.0$");
vtkStandardNewMacro(vtkVirtualMixer);

//----------------------------------------------------------------------------
vtkVirtualMixer::vtkVirtualMixer()
: vtkPlusDevice()
, OutputChannel(NULL)
{
  this->AcquisitionRate = vtkPlusDevice::VIRTUAL_DEVICE_FRAME_RATE;

  // No need for StartThreadForInternalUpdates, as capturing is performed in other devices, here we just collect references to buffers
}

//----------------------------------------------------------------------------
vtkVirtualMixer::~vtkVirtualMixer()
{
  // Clear reference to rf processor
  if( this->OutputChannel != NULL && this->OutputChannel->GetRfProcessor() != NULL )
  {
    this->OutputChannel->SetRfProcessor(NULL);
  }
}

//----------------------------------------------------------------------------
void vtkVirtualMixer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
vtkPlusChannel* vtkVirtualMixer::GetChannel() const
{
  // Virtual stream mixers always have exactly one output stream
  return this->GetOutputChannel();
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualMixer::ReadConfiguration( vtkXMLDataElement* element)
{
  if( Superclass::ReadConfiguration(element) == PLUS_FAIL )
  {
    return PLUS_FAIL;
  }

  if (this->OutputChannels.empty())
  {
    LOG_WARNING("vtkVirtualMixer device " << this->GetDeviceId() << " does not have any output channels");
  }
  else
  {
    SetOutputChannel(this->OutputChannels[0]);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
double vtkVirtualMixer::GetAcquisitionRate() const
{
  // Determine frame rate from the video input device with the lowest frame rate  
  bool lowestRateKnown=false;
  double lowestRate=30; // just a usual value (FPS)  

  for( ChannelContainerConstIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it )
  {
    vtkPlusChannel* anInputChannel = (*it);

    // Get the lowest rate from all image streams
    if( anInputChannel->HasVideoSource() )
    {
      lowestRate = anInputChannel->GetOwnerDevice()->GetAcquisitionRate();
      lowestRateKnown = true;
    }

    if (anInputChannel->GetOwnerDevice()->GetAcquisitionRate() < lowestRate || !lowestRateKnown)
    {
      lowestRate = anInputChannel->GetOwnerDevice()->GetAcquisitionRate();
      lowestRateKnown = true;
    }
  }

  if( !lowestRateKnown )
  {
    // Couldn't determine the lowest acquisition rate, so just use the one that was set by default
    lowestRate = this->AcquisitionRate;
  }
  return lowestRate;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualMixer::NotifyConfigured()
{
  // First, empty whatever is there, because this can be called at any point after a configuration
  this->GetOutputChannel()->RemoveTools();
  this->GetOutputChannel()->Clear();

  for( ChannelContainerIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it )
  {
    vtkPlusChannel* anInputChannel = (*it);
    vtkPlusDataSource* aSource = NULL;

    if( anInputChannel->HasVideoSource() && anInputChannel->GetVideoSource(aSource) == PLUS_SUCCESS )
    {
      this->GetOutputChannel()->SetVideoSource(aSource);
      this->AddVideo(aSource);
    }

    for( DataSourceContainerConstIterator inputToolIter = anInputChannel->GetToolsStartConstIterator(); inputToolIter != anInputChannel->GetToolsEndConstIterator(); ++inputToolIter )
    {
      vtkPlusDataSource* anInputTool = inputToolIter->second;

      bool found = false;
      for( DataSourceContainerConstIterator outputToolIt = this->GetOutputChannel()->GetToolsStartConstIterator(); outputToolIt != this->GetOutputChannel()->GetToolsEndConstIterator(); ++outputToolIt )
      {
        vtkPlusDataSource* anOutputTool = outputToolIt->second;
        // Check for double adds or name conflicts
        if( anInputTool == anOutputTool )
        {
          found = true;
          LOG_ERROR("Tool already exists in the output stream. Somehow the same tool is part of two input streams. Consider using a virtual device to resolve them first.");
          break;
        }
        else if( anInputTool->GetSourceId() == anOutputTool->GetSourceId() )
        {
          found = true;
          LOG_ERROR("Name collision! Two tools are outputting the same transform. Consider using a virtual device to resolve them first.");
          break;
        }
      }

      if( !found )
      {
        this->GetOutputChannel()->AddTool(anInputTool);
        this->AddTool(anInputTool);
      }
    }

    if( anInputChannel->GetRfProcessor() != NULL && this->GetOutputChannel()->GetRfProcessor() == NULL )
    {
      this->GetOutputChannel()->SetRfProcessor(anInputChannel->GetRfProcessor());
    }
    else if( anInputChannel->GetRfProcessor() != NULL && this->GetOutputChannel()->GetRfProcessor() != NULL )
    {
      LOG_WARNING("Multiple RfProcessors defined in InputChannels to mixer: " << this->GetDeviceId() << ". Check input configuration.");
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
bool vtkVirtualMixer::IsTracker() const
{
  for( ChannelContainerConstIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it )
  {
    vtkPlusChannel* aChannel = *it;
    if( aChannel->GetOwnerDevice()->IsTracker() )
    {
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualMixer::Reset()
{
  int numErrors(0);
  for( ChannelContainerConstIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it )
  {
    vtkPlusChannel* aChannel = *it;
    if( aChannel->GetOwnerDevice()->Reset() != PLUS_SUCCESS )
    {
      numErrors++;
    }
  }

  return numErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL;
}

//----------------------------------------------------------------------------
bool vtkVirtualMixer::IsResettable()
{
  for( ChannelContainerConstIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it )
  {
    vtkPlusChannel* aChannel = *it;
    if( aChannel->GetOwnerDevice()->IsResettable() )
    {
      return true;
    }
  }

  return false;
}
