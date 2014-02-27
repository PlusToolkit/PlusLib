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
{
  this->AcquisitionRate = vtkPlusDevice::VIRTUAL_DEVICE_FRAME_RATE;

  // No need for StartThreadForInternalUpdates, as capturing is performed in other devices, here we just collect references to buffers
}

//----------------------------------------------------------------------------
vtkVirtualMixer::~vtkVirtualMixer()
{
  // Clear reference to rf processor
  if (!this->OutputChannels.empty())
  {
    vtkPlusChannel* outputChannel=this->OutputChannels[0];
    if ( outputChannel != NULL && outputChannel->GetRfProcessor() != NULL )
    {
      outputChannel->SetRfProcessor(NULL);
    }
  }

  // Mixer fakes an output channel to enable "GetTrackedFrame" functionality
  // We don't want the plus device deconstructor destroying sources in output channels it doesn't own
  // So clear it before it runs (see #756)
  this->OutputChannels.clear();
}

//----------------------------------------------------------------------------
void vtkVirtualMixer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
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
    vtkSmartPointer<vtkPlusChannel> aChannel = vtkSmartPointer<vtkPlusChannel>::New();
    std::stringstream ss;
    ss << "aMixerId_" << vtkAccurateTimer::GetSystemTime();
    aChannel->SetChannelId(ss.str().c_str());
    this->AddOutputChannel(aChannel);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
double vtkVirtualMixer::GetAcquisitionRate() const
{
  // Determine frame rate from the video input device with the lowest frame rate  
  bool lowestRateKnown = false;
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
  if( this->OutputChannels.empty() )
  {
    LOG_ERROR("No output channels defined" );
    return PLUS_FAIL;
  }
  vtkPlusChannel* outputChannel=this->OutputChannels[0];

  outputChannel->RemoveTools();
  outputChannel->Clear();

  for( ChannelContainerIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it )
  {
    vtkPlusChannel* anInputChannel = (*it);
    vtkPlusDataSource* aSource = NULL;

    if( anInputChannel->HasVideoSource() && anInputChannel->GetVideoSource(aSource) == PLUS_SUCCESS )
    {
      outputChannel->SetVideoSource(aSource);
      this->AddVideo(aSource);
    }

    for( DataSourceContainerConstIterator inputToolIter = anInputChannel->GetToolsStartConstIterator(); inputToolIter != anInputChannel->GetToolsEndConstIterator(); ++inputToolIter )
    {
      vtkPlusDataSource* anInputTool = inputToolIter->second;

      bool found = false;
      for( DataSourceContainerConstIterator outputToolIt = outputChannel->GetToolsStartConstIterator(); outputToolIt != outputChannel->GetToolsEndConstIterator(); ++outputToolIt )
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
        outputChannel->AddTool(anInputTool);
        if( this->AddTool(anInputTool, false) != PLUS_SUCCESS )
        {
          LOG_ERROR("Unable to add tool " << anInputTool->GetSourceId() << " to device " << this->GetDeviceId());
        }
      }
    }

    if( anInputChannel->GetRfProcessor() != NULL && outputChannel->GetRfProcessor() == NULL )
    {
      outputChannel->SetRfProcessor(anInputChannel->GetRfProcessor());
    }
    else if( anInputChannel->GetRfProcessor() != NULL && outputChannel->GetRfProcessor() != NULL )
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
