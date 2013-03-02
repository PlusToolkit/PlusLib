/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "vtkObjectFactory.h"
#include "vtkPlusStreamBuffer.h"
#include "vtkPlusDataSource.h"
#include "vtkVirtualStreamMixer.h"

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkVirtualStreamMixer, "$Revision: 1.0$");
vtkStandardNewMacro(vtkVirtualStreamMixer);

//----------------------------------------------------------------------------
vtkVirtualStreamMixer::vtkVirtualStreamMixer()
: vtkPlusDevice()
, OutputChannel(NULL)
{
  this->AcquisitionRate = vtkPlusDevice::VIRTUAL_DEVICE_FRAME_RATE;

  // No need for StartThreadForInternalUpdates, as capturing is performed in other devices, here we just collect references to buffers
}

//----------------------------------------------------------------------------
vtkVirtualStreamMixer::~vtkVirtualStreamMixer()
{
}

//----------------------------------------------------------------------------
void vtkVirtualStreamMixer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
vtkPlusChannel* vtkVirtualStreamMixer::GetChannel() const
{
  // Virtual stream mixers always have exactly one output stream
  return this->GetOutputChannel();
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamMixer::ReadConfiguration( vtkXMLDataElement* element)
{
  if( Superclass::ReadConfiguration(element) == PLUS_FAIL )
  {
    return PLUS_FAIL;
  }

  if (this->OutputChannels.empty())
  {
    LOG_WARNING("vtkVirtualStreamMixer device "<<this->GetDeviceId()<<" does not have any output channels");
  }
  else
  {
    SetOutputChannel(this->OutputChannels[0]);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
double vtkVirtualStreamMixer::GetAcquisitionRate() const
{
  // Determine frame rate from the video input device with the lowest frame rate  
  bool lowestRateKnown=false;
  double lowestRate=30; // just a usual value (FPS)  

  for( ChannelContainerConstIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it )
  {
    vtkPlusChannel* anInputChannel = (*it);

    // Get the lowest rate from all image streams
    vtkPlusDataSource* videoSource = NULL;
    if( anInputChannel->HasVideoSource() )
    {
      lowestRate = anInputChannel->GetOwnerDevice()->GetAcquisitionRate();
      lowestRateKnown=true;
    }

    // Get the lowest rate from all tool streams
    for( DataSourceContainerConstIterator inputToolIter = anInputChannel->GetToolsStartConstIterator(); inputToolIter != anInputChannel->GetToolsEndConstIterator(); ++inputToolIter )
    {
      vtkPlusDataSource* anTool = inputToolIter->second;
      if (anInputChannel->GetOwnerDevice()->GetAcquisitionRate() < lowestRate || !lowestRateKnown)
      {
        lowestRate = anInputChannel->GetOwnerDevice()->GetAcquisitionRate();
        lowestRateKnown=true;
      }
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
PlusStatus vtkVirtualStreamMixer::NotifyConfigured()
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
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
bool vtkVirtualStreamMixer::IsTracker() const
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
PlusStatus vtkVirtualStreamMixer::Reset()
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
bool vtkVirtualStreamMixer::IsResettable()
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
