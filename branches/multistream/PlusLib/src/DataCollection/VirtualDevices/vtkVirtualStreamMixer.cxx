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
  // verify that the buffers in the output stream match the merged buffers in the input streams
  // if an input stream changes, it will re-map it
  for( StreamContainerIterator it = this->InputStreams.begin(); it != this->InputStreams.end(); ++it )
  {
    vtkPlusStream* str = *it;
    // for buffers
    //    can be many buffers
    // for tool buffers
    //    always only 1 buffer per tool
    //    check tool name conflict (maintain temporary list of taken tool names)
    //    assume tools in a stream all belong to the same device

    // TODO : how to resolve tool name collisions... error or rename?
    // TODO : initial thoughts -> error, another virtual device should be responsible for performing whatever name resolution is necessary
    //          that will enable case by case resolving
  }
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
  // TODO : Which device to choose from...
  return this->AcquisitionRate;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamMixer::NotifyConfigured()
{
  // TODO : take input streams, check for name conflicts/image conflicts, copy pointer to output stream[0]
  // buffers are empty, capturing hasn't started
  // how to determine valid image data in an input stream?
  for( StreamContainerIterator it = this->InputStreams.begin(); it != this->InputStreams.end(); ++it )
  {
    vtkPlusStream* anInputStream = (*it);
    for( StreamBufferMapContainerConstIterator bufIt = anInputStream->GetBuffersStartConstIterator(); bufIt != anInputStream->GetBuffersEndConstIterator(); ++bufIt )
    {
      vtkPlusStreamBuffer* buffer = bufIt->second;
      bool found = false;
      for(StreamBufferMapContainerConstIterator outputBufferIt = this->OutputStreams[0]->GetBuffersStartConstIterator(); outputBufferIt != this->OutputStreams[0]->GetBuffersEndConstIterator(); ++it)
      {
        if( outputBufferIt->second == buffer )
        {
          found = true;
          break;
        }
      }
      if(!found)
      {
        // This input buffer is not in the output stream, put it in!
        int port;
        this->OutputStreams[0]->AddBuffer(buffer, port);
      }
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
