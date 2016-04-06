/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusUsDevice.h"
#include "vtkPlusUsImagingParameters.h"

#ifdef PLUS_USE_tesseract
#include "vtkPlusVirtualTextRecognizer.h"
#endif

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusUsDevice);

//----------------------------------------------------------------------------
vtkPlusUsDevice::vtkPlusUsDevice()
: vtkPlusDevice()
, RequestedImagingParameters(vtkPlusUsImagingParameters::New())
, CurrentImagingParameters(vtkPlusUsImagingParameters::New())
, ImageToTransducerTransformName(NULL)
, TextRecognizerInputChannelName(NULL)
{
  this->CurrentTransducerOriginPixels[0]=-1;
  this->CurrentTransducerOriginPixels[1]=-1;

  this->CurrentPixelSpacingMm[0]=-1;
  this->CurrentPixelSpacingMm[1]=-1;
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
PlusStatus vtkPlusUsDevice::ReadConfiguration(vtkXMLDataElement* deviceConfig)
{
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(ImageToTransducerTransformName, deviceConfig);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(TextRecognizerInputChannelName, deviceConfig);
  XML_FIND_NESTED_ELEMENT_OPTIONAL(imagingParams, deviceConfig, vtkPlusUsImagingParameters::XML_ELEMENT_TAG);

  if( imagingParams == NULL )
  {
    return PLUS_FAIL;
  }

  return this->RequestedImagingParameters->ReadConfiguration(deviceConfig);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsDevice::WriteConfiguration(vtkXMLDataElement* deviceConfig)
{
  XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING(imagingParams, deviceConfig, vtkPlusUsImagingParameters::XML_ELEMENT_TAG);
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_NULL(ImageToTransducerTransformName, deviceConfig);
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_NULL(TextRecognizerInputChannelName, deviceConfig);

  return this->CurrentImagingParameters->WriteConfiguration(deviceConfig);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsDevice::InternalUpdate()
{
  if( this->InputChannel != NULL )
  {
    double aTimestamp(UNDEFINED_TIMESTAMP);
    PlusTrackedFrame frame;
    if( this->InputChannel->GetTrackedFrame(aTimestamp, frame, false) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to retrieve frame from the input channel. No parameters can be retrieved.");
      return PLUS_FAIL;
    }

    this->FrameFields = frame.GetCustomFields();
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsDevice::NotifyConfigured()
{
  if( this->TextRecognizerInputChannelName != NULL )
  {
    if( ImageToTransducerTransform.SetTransformName(this->ImageToTransducerTransformName) != PLUS_SUCCESS )
    {
      LOG_ERROR("Transform name is not properly formatted. It should be of the format <From>ToTransducer.");
      return PLUS_FAIL;
    }

    for( ChannelContainerIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it )
    {
      vtkPlusChannel* channel = *it;
      if( STRCASECMP(channel->GetChannelId(), this->TextRecognizerInputChannelName) == 0 )
      {
        this->InputChannel = channel;
        return PLUS_SUCCESS;
      }
    }
    LOG_ERROR("Unable to find channel " << this->TextRecognizerInputChannelName << ". Did you add it in the XML?");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsDevice::SetNewImagingParameters(const vtkPlusUsImagingParameters& newImagingParameters)
{
  if( this->RequestedImagingParameters->DeepCopy(newImagingParameters) == PLUS_FAIL )
  {
    LOG_ERROR("Unable to deep copy new imaging parameters.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsDevice::AddVideoItemToVideoSources(const std::vector<vtkPlusDataSource*>& videoSources, const PlusVideoFrame& frame, long frameNumber, double unfilteredTimestamp/*=UNDEFINED_TIMESTAMP*/, double filteredTimestamp/*=UNDEFINED_TIMESTAMP*/, const PlusTrackedFrame::FieldMapType* customFields /*= NULL*/)
{
  PlusTrackedFrame::FieldMapType localCustomFields;
  if( this->ImageToTransducerTransformName != NULL && customFields != NULL )
  {
    localCustomFields = *customFields;
    CalculateImageToTransducer(localCustomFields);
  }

  return Superclass::AddVideoItemToVideoSources(videoSources, frame, frameNumber, unfilteredTimestamp, filteredTimestamp, &localCustomFields);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsDevice::AddVideoItemToVideoSources(const std::vector<vtkPlusDataSource*>& videoSources, void* imageDataPtr, US_IMAGE_ORIENTATION usImageOrientation, const int frameSizeInPx[3], PlusCommon::VTKScalarPixelType pixelType, int numberOfScalarComponents, US_IMAGE_TYPE imageType, int numberOfBytesToSkip, long frameNumber, double unfilteredTimestamp/*=UNDEFINED_TIMESTAMP*/, double filteredTimestamp/*=UNDEFINED_TIMESTAMP*/, const PlusTrackedFrame::FieldMapType* customFields/*= NULL*/)
{
  PlusTrackedFrame::FieldMapType localCustomFields;
  if( this->ImageToTransducerTransformName != NULL && customFields != NULL )
  {
    localCustomFields = *customFields;
    CalculateImageToTransducer(localCustomFields);
  }

  return Superclass::AddVideoItemToVideoSources(videoSources, imageDataPtr, usImageOrientation, frameSizeInPx, pixelType, numberOfScalarComponents, imageType, numberOfBytesToSkip, frameNumber, unfilteredTimestamp, filteredTimestamp, &localCustomFields);
}

//----------------------------------------------------------------------------
void vtkPlusUsDevice::CalculateImageToTransducer(PlusTrackedFrame::FieldMapType& customFields)
{
  std::ostringstream imageToTransducerName;
  imageToTransducerName << ImageToTransducerTransform.GetTransformName() << "Transform";

  std::ostringstream imageToTransducerTransformStr;
  imageToTransducerTransformStr << this->CurrentPixelSpacingMm[0] << " 0 0 " << -1.0*this->CurrentTransducerOriginPixels[0]*this->CurrentPixelSpacingMm[0];
  imageToTransducerTransformStr << " 0 " << this->CurrentPixelSpacingMm[1] << " 0 " << -1.0*this->CurrentTransducerOriginPixels[1]*this->CurrentPixelSpacingMm[1];
  imageToTransducerTransformStr << " 0 0 " << this->CurrentPixelSpacingMm[2] << " " << -1.0*this->CurrentTransducerOriginPixels[2]*this->CurrentPixelSpacingMm[2];
  imageToTransducerTransformStr << " 0 0 0 1";
  customFields[imageToTransducerName.str()] = imageToTransducerTransformStr.str();
  imageToTransducerName << "Status";
  customFields[imageToTransducerName.str()] = "OK";
}