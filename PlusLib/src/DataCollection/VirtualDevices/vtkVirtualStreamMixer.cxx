/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "vtkObjectFactory.h"
#include "vtkPlusStreamBuffer.h"
#include "vtkPlusStreamTool.h"
#include "vtkVirtualStreamMixer.h"

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkVirtualStreamMixer, "$Revision: 1.0$");
vtkStandardNewMacro(vtkVirtualStreamMixer);

const double HUGE_DOUBLE = 1.7e308;

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
  double lowestRate(HUGE_DOUBLE);

  vtkPlusStreamBuffer* aBuff = NULL;
  for( StreamContainerConstIterator it = this->InputStreams.begin(); it != this->InputStreams.end(); ++it )
  {
    aBuff = NULL;
    vtkPlusStream* anInputStream = (*it);
    if( anInputStream->BufferCount() > 0 && anInputStream->GetBuffer(aBuff, 0) == PLUS_SUCCESS )
    {
      StreamBufferItem item;
      if( aBuff->GetLatestStreamBufferItem(&item) == ITEM_OK && item.HasValidVideoData() && anInputStream->GetOwnerDevice()->GetAcquisitionRate() < lowestRate )
      {
        lowestRate = anInputStream->GetOwnerDevice()->GetAcquisitionRate();
      }
    }
  }

  if( lowestRate == HUGE_DOUBLE )
  {
    lowestRate = this->AcquisitionRate;
  }
  return lowestRate;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamMixer::NotifyConfigured()
{
  // First, empty whatever is there, because this can be called at any point after a configuration
  this->GetOutputStream()->Clear();

  // take input streams, check for name conflicts, copy pointer to output stream
  for( StreamContainerIterator it = this->InputStreams.begin(); it != this->InputStreams.end(); ++it )
  {
    vtkPlusStream* anInputStream = (*it);
    for( StreamBufferMapContainerConstIterator bufIt = anInputStream->GetBuffersStartConstIterator(); bufIt != anInputStream->GetBuffersEndConstIterator(); ++bufIt )
    {
      vtkPlusStreamBuffer* inputBuffer = bufIt->second;

      for(StreamBufferMapContainerConstIterator outputBufferIt = this->GetOutputStream()->GetBuffersStartConstIterator(); outputBufferIt != this->GetOutputStream()->GetBuffersEndConstIterator(); ++outputBufferIt)
      {
        if( outputBufferIt->second == inputBuffer )
        {
          LOG_ERROR("Buffer already found in the output stream. Trying to add the same inputBuffer twice.");
          break;
        }
      }

      // This input inputBuffer is not in the output stream, put it in!
      this->GetOutputStream()->AddBuffer(inputBuffer, vtkPlusStream::FIND_PORT);
    }

    for( ToolContainerConstIterator inputToolIter = anInputStream->GetToolBuffersStartConstIterator(); inputToolIter != anInputStream->GetToolBuffersEndConstIterator(); ++inputToolIter )
    {
      vtkPlusStreamTool* anInputTool = inputToolIter->second;

      bool found = false;
      for( ToolContainerConstIterator outputToolIt = this->GetOutputStream()->GetToolBuffersStartConstIterator(); outputToolIt != this->GetOutputStream()->GetToolBuffersEndConstIterator(); ++outputToolIt )
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
void vtkVirtualStreamMixer::InternalWriteOutputStreams( vtkXMLDataElement* rootXMLElement )
{
  // Do not call parent function, this replaces the parent call
  LOG_TRACE("vtkVirtualStreamMixer::InternalWriteOutputStreams( " << rootXMLElement->GetName() << ")");

  for( StreamContainerConstIterator it = this->OutputStreams.begin(); it != this->OutputStreams.end(); ++it)
  {
    vtkPlusStream* aStream = *it;
    vtkXMLDataElement* streamElement = this->FindOutputStreamElement(rootXMLElement, aStream->GetStreamId());
    aStream->WriteCompactConfiguration(streamElement);
  }
}

//----------------------------------------------------------------------------
void vtkVirtualStreamMixer::InternalWriteInputStreams( vtkXMLDataElement* rootXMLElement )
{
  // Do not call parent function, this replaces the parent call
  LOG_TRACE("vtkVirtualStreamMixer::InternalWriteInputStreams( " << rootXMLElement->GetName() << ")");

  for( StreamContainerConstIterator it = this->InputStreams.begin(); it != this->InputStreams.end(); ++it)
  {
    vtkPlusStream* aStream = *it;
    vtkXMLDataElement* streamElement = this->FindInputStreamElement(rootXMLElement, aStream->GetStreamId());
    aStream->WriteCompactConfiguration(streamElement);
  }
}
