/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "vtkObjectFactory.h"
#include "vtkPlusStream.h"
#include "vtkPlusStreamBuffer.h"
#include "vtkPlusStreamTool.h"
#include "vtkVirtualStreamMixer.h"

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkVirtualStreamMixer, "$Revision: 1.0$");
vtkStandardNewMacro(vtkVirtualStreamMixer);

//----------------------------------------------------------------------------
vtkVirtualStreamMixer::vtkVirtualStreamMixer()
: vtkPlusDevice()
{
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
  return this->OutputStreams[0];
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamMixer::InternalUpdate()
{
  // TODO : verify that the buffers in the output stream match the merged buffers in the input streams
  // if an input stream changes, it will re-map it

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamMixer::Reset()
{
  Superclass::Reset();

  for( StreamContainerIterator it = this->InputStreams.begin(); it != this->InputStreams.end(); ++it )
  {
    vtkPlusStream* str = (*it);
    if( str->GetOwnerDevice()->IsTracker() )
    {
      str->GetOwnerDevice()->Reset();
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
double vtkVirtualStreamMixer::GetAcquisitionRate() const
{
  vtkPlusStreamBuffer* aBuff = NULL;
  for( StreamContainerConstIterator it = this->InputStreams.begin(); it != this->InputStreams.end(); ++it )
  {
    aBuff = NULL;
    vtkPlusStream* anInputStream = (*it);
    if( anInputStream->GetBuffer(aBuff, 0) == PLUS_SUCCESS )
    {
      StreamBufferItem item;
      if( aBuff->GetLatestStreamBufferItem(&item) == ITEM_OK && item.HasValidVideoData() )
      {
        return anInputStream->GetOwnerDevice()->GetAcquisitionRate();
      }
    }
  }

  return this->AcquisitionRate;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamMixer::NotifyConfigured()
{
  // First, empty whatever is there, because this can be called at any point after a configuration
  this->OutputStreams[0]->Clear();

  // take input streams, check for name conflicts, copy pointer to output stream[0]
  for( StreamContainerIterator it = this->InputStreams.begin(); it != this->InputStreams.end(); ++it )
  {
    vtkPlusStream* anInputStream = (*it);
    for( StreamBufferMapContainerConstIterator bufIt = anInputStream->GetBuffersStartConstIterator(); bufIt != anInputStream->GetBuffersEndConstIterator(); ++bufIt )
    {
      vtkPlusStreamBuffer* inputBuffer = bufIt->second;

      for(StreamBufferMapContainerConstIterator outputBufferIt = this->OutputStreams[0]->GetBuffersStartConstIterator(); outputBufferIt != this->OutputStreams[0]->GetBuffersEndConstIterator(); ++it)
      {
        if( outputBufferIt->second == inputBuffer )
        {
          LOG_ERROR("Buffer already found in the output stream. Trying to add the same inputBuffer twice.");
          break;
        }
      }

      // This input inputBuffer is not in the output stream, put it in!
      int port;
      this->OutputStreams[0]->AddBuffer(inputBuffer, port);
    }

    for( ToolContainerConstIteratorType inputToolIter = anInputStream->GetToolBuffersStartConstIterator(); inputToolIter != anInputStream->GetToolBuffersEndConstIterator(); ++inputToolIter )
    {
      vtkPlusStreamTool* anInputTool = inputToolIter->second;

      bool found = false;
      for( ToolContainerConstIteratorType outputToolIt = this->OutputStreams[0]->GetToolBuffersStartConstIterator(); outputToolIt != this->OutputStreams[0]->GetToolBuffersEndConstIterator(); ++outputToolIt )
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
        this->OutputStreams[0]->AddTool(anInputTool);
      }
    }
  }

  return PLUS_SUCCESS;
}
