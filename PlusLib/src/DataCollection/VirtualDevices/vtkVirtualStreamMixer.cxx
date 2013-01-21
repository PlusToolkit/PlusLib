/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "vtkObjectFactory.h"
#include "vtkPlusStreamBuffer.h"
#include "vtkPlusStreamTool.h"
#include "vtkPlusStreamImage.h"
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
vtkPlusStream* vtkVirtualStreamMixer::GetStream() const
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

  SetOutputStream(this->OutputStreams[0]);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
double vtkVirtualStreamMixer::GetAcquisitionRate() const
{
  // Determine frame rate from the video input device with the lowest frame rate  
  bool lowestRateKnown=false;
  double lowestRate=30; // just a usual value (FPS)  
  for( StreamContainerConstIterator it = this->InputStreams.begin(); it != this->InputStreams.end(); ++it )
  {
    vtkPlusStream* anInputStream = (*it);

    vtkPlusStreamImage* anImage = NULL;
    vtkPlusStreamTool* aTool = NULL;
    
    if( anInputStream->ImageCount() > 0 && anInputStream->GetImage(anImage, 0) == PLUS_SUCCESS )
    {
      StreamBufferItem item;
      if( anImage->GetBuffer()->GetNumberOfItems() > 0 && anImage->GetBuffer()->GetLatestStreamBufferItem(&item) == ITEM_OK && item.HasValidVideoData() )
      {
        if (anInputStream->GetOwnerDevice()->GetAcquisitionRate() < lowestRate || !lowestRateKnown)
        {
          lowestRate = anInputStream->GetOwnerDevice()->GetAcquisitionRate();
        }
      }
    }
    else if( anInputStream->ToolCount() > 0 && anInputStream->GetTool(aTool, 0) == PLUS_SUCCESS )
    {
      StreamBufferItem item;
      if( aTool->GetBuffer()->GetLatestStreamBufferItem(&item) == ITEM_OK && item.HasValidTransformData() && anInputStream->GetOwnerDevice()->GetAcquisitionRate() < lowestRate )
      {
        lowestRate = anInputStream->GetOwnerDevice()->GetAcquisitionRate();
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

  for( StreamContainerIterator it = this->InputStreams.begin(); it != this->InputStreams.end(); ++it )
  {
    vtkPlusStream* anInputStream = (*it);
    for( ImageContainerConstIterator inputImageIter = anInputStream->GetImagesStartConstIterator(); inputImageIter != anInputStream->GetImagesEndConstIterator(); ++inputImageIter )
    {
      vtkPlusStreamImage* anInputImage = inputImageIter->second;

      bool found = false;
      for( ImageContainerConstIterator outputImageIter = this->GetOutputStream()->GetImagesStartConstIterator(); outputImageIter != this->GetOutputStream()->GetImagesEndConstIterator(); ++outputImageIter )
      {
        vtkPlusStreamImage* anOutputImage = outputImageIter->second;
        // Check for double adds or name conflicts
        if( anInputImage == anOutputImage )
        {
          found = true;
          LOG_ERROR("Image already exists in the output stream. Somehow the same image is part of two input streams. Consider using a virtual device to resolve them first.");
          break;
        }
        else if( anInputImage->GetImageName() == anOutputImage->GetImageName() )
        {
          found = true;
          LOG_ERROR("Name collision! Two images are outputting the same source. Consider using a virtual device to resolve them first.");
          break;
        }
      }

      if( !found )
      {
        this->GetOutputStream()->AddImage(anInputImage);
      }
    }

    for( ToolContainerConstIterator inputToolIter = anInputStream->GetToolsStartConstIterator(); inputToolIter != anInputStream->GetToolsEndConstIterator(); ++inputToolIter )
    {
      vtkPlusStreamTool* anInputTool = inputToolIter->second;

      bool found = false;
      for( ToolContainerConstIterator outputToolIt = this->GetOutputStream()->GetToolsStartConstIterator(); outputToolIt != this->GetOutputStream()->GetToolsEndConstIterator(); ++outputToolIt )
      {
        vtkPlusStreamTool* anOutputTool = outputToolIt->second;
        // Check for double adds or name conflicts
        if( anInputTool == anOutputTool )
        {
          found = true;
          LOG_ERROR("Tool already exists in the output stream. Somehow the same tool is part of two input streams. Consider using a virtual device to resolve them first.");
          break;
        }
        else if( anInputTool->GetToolName() == anOutputTool->GetToolName() )
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
  for( StreamContainerConstIterator it = this->InputStreams.begin(); it != this->InputStreams.end(); ++it)
  {
    vtkPlusStream* stream = *it;
    // Now check any and all tool buffers
    for( ToolContainerConstIterator it = stream->GetOwnerDevice()->GetToolIteratorBegin(); it != stream->GetOwnerDevice()->GetToolIteratorEnd(); ++it)
    {
      vtkPlusStreamTool* tool = it->second;
      tool->GetBuffer()->SetLocalTimeOffsetSec(aTimeOffsetSec);
    }
  }
}

//----------------------------------------------------------------------------
double vtkVirtualStreamMixer::GetToolLocalTimeOffsetSec()
{
  // tools in input streams (owned by other devices)
  for( StreamContainerConstIterator it = this->InputStreams.begin(); it != this->InputStreams.end(); ++it)
  {
    vtkPlusStream* stream = *it;
    // Now check any and all tool buffers
    for( ToolContainerConstIterator it = stream->GetOwnerDevice()->GetToolIteratorBegin(); it != stream->GetOwnerDevice()->GetToolIteratorEnd(); ++it)
    {
      vtkPlusStreamTool* tool = it->second;
      double aTimeOffsetSec = tool->GetBuffer()->GetLocalTimeOffsetSec();
      return aTimeOffsetSec;
    }
  }
  LOG_ERROR("Failed to get tool local time offset");
  return 0.0;
}