/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "PlusMath.h"
#include "vtkTrackedFrameProcessor.h"
#include "vtkTrackedFrameList.h"
#include "vtkTransformRepository.h"

//----------------------------------------------------------------------------
vtkCxxSetObjectMacro(vtkTrackedFrameProcessor, InputFrames, vtkTrackedFrameList);
vtkCxxSetObjectMacro(vtkTrackedFrameProcessor, TransformRepository, vtkTransformRepository);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
vtkTrackedFrameProcessor::vtkTrackedFrameProcessor()
{
  this->InputFrames = NULL;
  this->TransformRepository = NULL;
  this->OutputFrames = vtkTrackedFrameList::New();
}

//----------------------------------------------------------------------------
vtkTrackedFrameProcessor::~vtkTrackedFrameProcessor()
{
  SetInputFrames(NULL);
  SetTransformRepository(NULL);
  this->OutputFrames->Delete();
  this->OutputFrames = NULL;
}

//----------------------------------------------------------------------------
void vtkTrackedFrameProcessor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//-----------------------------------------------------------------------------
PlusStatus vtkTrackedFrameProcessor::ReadConfiguration(vtkXMLDataElement* processingElement)
{
  XML_VERIFY_ELEMENT(processingElement, this->GetTagName());
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkTrackedFrameProcessor::WriteConfiguration(vtkXMLDataElement* processingElement)
{
  XML_VERIFY_ELEMENT(processingElement, this->GetTagName());
  processingElement->SetAttribute("Type", this->GetProcessorTypeName());
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
  PlusStatus vtkTrackedFrameProcessor::Update()
{
  this->OutputFrames->Clear();
  if (this->InputFrames==NULL || this->InputFrames->GetNumberOfTrackedFrames()==0)
  {
    // nothing to do
    return PLUS_SUCCESS;
  }
  PlusStatus status = PLUS_SUCCESS;
  for (int frameIndex=0; frameIndex<this->InputFrames->GetNumberOfTrackedFrames(); frameIndex++)
  {
    TrackedFrame* inputFrame = this->InputFrames->GetTrackedFrame(frameIndex);
    // Update the transform repository with the tracking information in the frame.
    // After this we can query any transform from the repository.
    if (this->TransformRepository && this->TransformRepository->SetTransforms(*inputFrame) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to set repository transforms from tracked frame!"); 
      status = PLUS_FAIL;
      continue;
    }
    // Create a clone of the input frame in the output buffer
    // TODO: not very efficient that we copy the image data as well, we could just instantiate an empty output frame
    this->OutputFrames->AddTrackedFrame(inputFrame);
    TrackedFrame* outputFrame = this->OutputFrames->GetTrackedFrame(this->OutputFrames->GetNumberOfTrackedFrames()-1); // the last frame that just has been added
    // Do the actual processing
    if (this->ProcessFrame(inputFrame, outputFrame)!=PLUS_SUCCESS)
    {
      status = PLUS_FAIL;
    }
  }

  return status;
}

//-----------------------------------------------------------------------------
PlusStatus vtkTrackedFrameProcessor::ProcessFrame(TrackedFrame* inputFrame, TrackedFrame* outputFrame)
{
  return PLUS_SUCCESS;
}
