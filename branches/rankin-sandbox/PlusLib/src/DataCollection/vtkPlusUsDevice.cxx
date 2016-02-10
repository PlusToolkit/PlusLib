/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusUsDevice.h"

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusUsDevice);

//----------------------------------------------------------------------------
vtkPlusUsDevice::vtkPlusUsDevice()
: vtkPlusDevice()
, RequestedImagingParameters(vtkUsImagingParameters::New())
, CurrentImagingParameters(vtkUsImagingParameters::New())
{
}

//----------------------------------------------------------------------------
vtkPlusUsDevice::~vtkPlusUsDevice()
{
  RequestedImagingParameters->Delete();
  CurrentImagingParameters->Delete();
}

//----------------------------------------------------------------------------
void vtkPlusUsDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Requested imaging parameters: " << std::endl;
  this->RequestedImagingParameters->PrintSelf(os, indent);
  os << indent << "Current imaging parameters: " << std::endl;
  this->CurrentImagingParameters->PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsDevice::SetNewImagingParameters(const vtkUsImagingParameters& newImagingParameters)
{
  if( this->RequestedImagingParameters->DeepCopy(newImagingParameters) == PLUS_FAIL )
  {
    LOG_ERROR("Unable to deep copy new imaging parameters.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsDevice::AddVideoItemToVideoSources(const std::vector<vtkPlusDataSource*>& videoSources, const PlusVideoFrame& frame, long frameNumber, double unfilteredTimestamp/*=UNDEFINED_TIMESTAMP*/, double filteredTimestamp/*=UNDEFINED_TIMESTAMP*/, const TrackedFrame::FieldMapType* customFields /*= NULL*/)
{
  // TODO : any US device specific entries here

  return Superclass::AddVideoItemToVideoSources(videoSources, frame, frameNumber, unfilteredTimestamp, filteredTimestamp, customFields);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsDevice::AddVideoItemToVideoSources(const std::vector<vtkPlusDataSource*>& videoSources, void* imageDataPtr, US_IMAGE_ORIENTATION usImageOrientation, const int frameSizeInPx[3], PlusCommon::VTKScalarPixelType pixelType, int numberOfScalarComponents, US_IMAGE_TYPE imageType, int numberOfBytesToSkip, long frameNumber, double unfilteredTimestamp/*=UNDEFINED_TIMESTAMP*/, double filteredTimestamp/*=UNDEFINED_TIMESTAMP*/, const TrackedFrame::FieldMapType* customFields/*= NULL*/)
{
  // TODO : any US device specific entries here

  return Superclass::AddVideoItemToVideoSources(videoSources, imageDataPtr, usImageOrientation, frameSizeInPx, pixelType, numberOfScalarComponents, imageType, numberOfBytesToSkip, frameNumber, unfilteredTimestamp, filteredTimestamp, customFields);
}
