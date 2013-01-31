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
, OutputStream(NULL)
{
  this->AcquisitionRate = vtkPlusDevice::VIRTUAL_DEVICE_FRAME_RATE;
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
vtkPlusChannel* vtkVirtualStreamMixer::GetStream() const
{
  // Virtual stream mixers always have exactly one output stream
  return this->GetOutputStream();
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamMixer::ReadConfiguration( vtkXMLDataElement* element)
{
  if( Superclass::ReadConfiguration(element) == PLUS_FAIL )
  {
    return PLUS_FAIL;
  }

  SetOutputStream(this->OutputChannels[0]);

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
  this->GetOutputStream()->Clear();

  for( ChannelContainerIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it )
  {
    vtkPlusChannel* anInputChannel = (*it);
    vtkPlusDataSource* aSource = NULL;

    if( anInputChannel->HasVideoSource() && anInputChannel->GetVideoSource(aSource) == PLUS_SUCCESS )
    {
      this->GetOutputStream()->SetVideoSource(aSource);
    }

    for( DataSourceContainerConstIterator inputToolIter = anInputChannel->GetToolsStartConstIterator(); inputToolIter != anInputChannel->GetToolsEndConstIterator(); ++inputToolIter )
    {
      vtkPlusDataSource* anInputTool = inputToolIter->second;

      bool found = false;
      for( DataSourceContainerConstIterator outputToolIt = this->GetOutputStream()->GetToolsStartConstIterator(); outputToolIt != this->GetOutputStream()->GetToolsEndConstIterator(); ++outputToolIt )
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
        this->GetOutputStream()->AddTool(anInputTool);
      }
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkVirtualStreamMixer::SetToolLocalTimeOffsetSec( double aTimeOffsetSec )
{
  // tools in input streams (owned by other devices)
  for( ChannelContainerConstIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it)
  {
    vtkPlusChannel* stream = *it;
    // Now check any and all tool buffers
    for( DataSourceContainerConstIterator it = stream->GetOwnerDevice()->GetToolIteratorBegin(); it != stream->GetOwnerDevice()->GetToolIteratorEnd(); ++it)
    {
      vtkPlusDataSource* tool = it->second;
      tool->GetBuffer()->SetLocalTimeOffsetSec(aTimeOffsetSec);
    }
  }
}

//----------------------------------------------------------------------------
double vtkVirtualStreamMixer::GetToolLocalTimeOffsetSec()
{
  // tools in input streams (owned by other devices)
  for( ChannelContainerConstIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it)
  {
    vtkPlusChannel* stream = *it;
    // Now check any and all tool buffers
    for( DataSourceContainerConstIterator it = stream->GetOwnerDevice()->GetToolIteratorBegin(); it != stream->GetOwnerDevice()->GetToolIteratorEnd(); ++it)
    {
      vtkPlusDataSource* tool = it->second;
      double aTimeOffsetSec = tool->GetBuffer()->GetLocalTimeOffsetSec();
      return aTimeOffsetSec;
    }
  }
  LOG_ERROR("Failed to get tool local time offset");
  return 0.0;
}

//----------------------------------------------------------------------------
void vtkVirtualStreamMixer::SetVideoLocalTimeOffsetSec( double aTimeOffsetSec )
{
  // images in input streams (owned by other devices)
  for( ChannelContainerConstIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it)
  {
    vtkPlusChannel* aChannel = *it;
    vtkPlusDataSource* aSource = NULL;

    if( aChannel->HasVideoSource() && aChannel->GetVideoSource(aSource) == PLUS_SUCCESS )
    {
      aSource->GetBuffer()->SetLocalTimeOffsetSec(aTimeOffsetSec);
    }
  }
}

//----------------------------------------------------------------------------
double vtkVirtualStreamMixer::GetVideoLocalTimeOffsetSec()
{
  // images in input streams (owned by other devices)
  for( ChannelContainerConstIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it)
  {
    vtkPlusChannel* aChannel = *it;
    vtkPlusDataSource* aSource = NULL;

    if( aChannel->HasVideoSource() && aChannel->GetVideoSource(aSource) == PLUS_SUCCESS )
    {
      double aTimeOffsetSec = aSource->GetBuffer()->GetLocalTimeOffsetSec();
      return aTimeOffsetSec;
    }
  }

  LOG_ERROR("Failed to get image local time offset");
  return 0.0;
}

