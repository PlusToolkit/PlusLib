/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusUsDevice.h"
#include "vtkPlusUsImagingParameters.h"
#include "vtkPlusDataSource.h"

#ifdef PLUS_USE_tesseract
  #include "vtkPlusVirtualTextRecognizer.h"
#endif

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusUsDevice);

//----------------------------------------------------------------------------
vtkPlusUsDevice::vtkPlusUsDevice()
  : vtkPlusDevice()
  , ImagingParameters( vtkPlusUsImagingParameters::New() )
  , ImageToTransducerTransformName( NULL )
{
  this->CurrentTransducerOriginPixels[0] = 0;
  this->CurrentTransducerOriginPixels[1] = 0;
  this->CurrentTransducerOriginPixels[2] = 0;

  this->CurrentPixelSpacingMm[0] = 1;
  this->CurrentPixelSpacingMm[1] = 1;
  this->CurrentPixelSpacingMm[2] = 1;
}

//----------------------------------------------------------------------------
vtkPlusUsDevice::~vtkPlusUsDevice()
{
  ImagingParameters->Delete();
}

//----------------------------------------------------------------------------
void vtkPlusUsDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Imaging parameters: " << std::endl;
  this->ImagingParameters->PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsDevice::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  vtkXMLDataElement* deviceConfig = this->FindThisDeviceElement(rootConfigElement);
  if (deviceConfig == NULL)
  {
    LOG_ERROR("Unable to continue configuration of " << this->GetClassName() << ". Could not find corresponding element.");
    return PLUS_FAIL;
  }

  XML_READ_CSTRING_ATTRIBUTE_OPTIONAL(ImageToTransducerTransformName, deviceConfig);
  const char* transformName = deviceConfig->GetAttribute("ImageToTransducerTransformName");
  if (transformName != NULL && this->ImageToTransducerTransform.SetTransformName(transformName) != PLUS_SUCCESS)
  {
    LOG_ERROR("Transform name is not properly formatted. It should be of the format <From>ToTransducer.");
    this->SetImageToTransducerTransformName(NULL);
  }
  XML_FIND_NESTED_ELEMENT_OPTIONAL(imagingParams, deviceConfig, vtkPlusUsImagingParameters::XML_ELEMENT_TAG);

  if (imagingParams != NULL)
  {
    this->ImagingParameters->ReadConfiguration( deviceConfig );

    if (this->InternalApplyImagingParameterChange() == PLUS_FAIL)
    {
      LOG_ERROR("Failed to change imaging parameters in the device");
      return PLUS_FAIL;
    }
  }

  return Superclass::ReadConfiguration(rootConfigElement);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsDevice::WriteConfiguration(vtkXMLDataElement* deviceConfig)
{
  XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING(imagingParams, deviceConfig, vtkPlusUsImagingParameters::XML_ELEMENT_TAG);

  XML_WRITE_CSTRING_ATTRIBUTE_IF_NOT_NULL(ImageToTransducerTransformName, deviceConfig);

  return this->ImagingParameters->WriteConfiguration( deviceConfig );
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsDevice::SetNewImagingParameters(const vtkPlusUsImagingParameters& newImagingParameters)
{
  if( this->ImagingParameters->DeepCopy( newImagingParameters ) == PLUS_FAIL )
  {
    LOG_ERROR("Unable to deep copy new imaging parameters");
    return PLUS_FAIL;
  }

  if (this->InternalApplyImagingParameterChange() == PLUS_FAIL)
  {
    LOG_ERROR("Failed to change imaging parameters in the device");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsDevice::AddVideoItemToVideoSource(vtkPlusDataSource& videoSource, const igsioVideoFrame& frame, long frameNumber, double unfilteredTimestamp /*= UNDEFINED_TIMESTAMP*/, double filteredTimestamp /*= UNDEFINED_TIMESTAMP*/, const igsioTrackedFrame::FieldMapType* customFields /*= NULL*/)
{
  igsioTrackedFrame::FieldMapType localCustomFields;
  if (!this->ImageToTransducerTransform.GetTransformName().empty())
  {
    if (customFields != NULL)
    {
      localCustomFields = *customFields;
    }
    this->CalculateImageToTransducer(localCustomFields);
  }

  return videoSource.AddItem(&frame, frameNumber, unfilteredTimestamp, filteredTimestamp, &localCustomFields);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsDevice::AddVideoItemToVideoSource(vtkPlusDataSource& videoSource, void* imageDataPtr, US_IMAGE_ORIENTATION usImageOrientation, const FrameSizeType& frameSizeInPx, igsioCommon::VTKScalarPixelType pixelType, unsigned int numberOfScalarComponents, US_IMAGE_TYPE imageType, int numberOfBytesToSkip, long frameNumber, double unfilteredTimestamp /*= UNDEFINED_TIMESTAMP*/, double filteredTimestamp /*= UNDEFINED_TIMESTAMP*/, const igsioTrackedFrame::FieldMapType* customFields /*= NULL*/)
{
  igsioTrackedFrame::FieldMapType localCustomFields;
  if (!this->ImageToTransducerTransform.GetTransformName().empty())
  {
    if (customFields != NULL)
    {
      localCustomFields = *customFields;
    }
    this->CalculateImageToTransducer(localCustomFields);
  }

  return videoSource.AddItem(imageDataPtr, usImageOrientation, frameSizeInPx, pixelType, numberOfScalarComponents, imageType, numberOfBytesToSkip, frameNumber, unfilteredTimestamp, filteredTimestamp, customFields);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsDevice::AddVideoItemToVideoSources(const std::vector<vtkPlusDataSource*>& videoSources, const igsioVideoFrame& frame, long frameNumber, double unfilteredTimestamp/*=UNDEFINED_TIMESTAMP*/, double filteredTimestamp/*=UNDEFINED_TIMESTAMP*/, const igsioTrackedFrame::FieldMapType* customFields /*= NULL*/)
{
  igsioTrackedFrame::FieldMapType localCustomFields;
  if (!this->ImageToTransducerTransform.GetTransformName().empty() && customFields != NULL)
  {
    localCustomFields = *customFields;
    this->CalculateImageToTransducer(localCustomFields);
  }

  return Superclass::AddVideoItemToVideoSources(videoSources, frame, frameNumber, unfilteredTimestamp, filteredTimestamp, &localCustomFields);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsDevice::AddVideoItemToVideoSources(const std::vector<vtkPlusDataSource*>& videoSources, void* imageDataPtr, US_IMAGE_ORIENTATION usImageOrientation, const FrameSizeType& frameSizeInPx, igsioCommon::VTKScalarPixelType pixelType, unsigned int numberOfScalarComponents, US_IMAGE_TYPE imageType, int numberOfBytesToSkip, long frameNumber, double unfilteredTimestamp /*= UNDEFINED_TIMESTAMP*/, double filteredTimestamp /*= UNDEFINED_TIMESTAMP*/, const igsioTrackedFrame::FieldMapType* customFields /*= NULL*/)
{
  igsioTrackedFrame::FieldMapType localCustomFields;
  if (customFields != NULL)
  {
    localCustomFields = *customFields;
    if (this->ImageToTransducerTransform.IsValid())
    {
      this->CalculateImageToTransducer(localCustomFields);
    }
  }

  return Superclass::AddVideoItemToVideoSources(videoSources, imageDataPtr, usImageOrientation, frameSizeInPx, pixelType, numberOfScalarComponents, imageType, numberOfBytesToSkip, frameNumber, unfilteredTimestamp, filteredTimestamp, &localCustomFields);
}

//----------------------------------------------------------------------------
void vtkPlusUsDevice::CalculateImageToTransducer(igsioTrackedFrame::FieldMapType& customFields)
{
  std::ostringstream imageToTransducerName;
  imageToTransducerName << ImageToTransducerTransform.GetTransformName() << "Transform";

  std::ostringstream imageToTransducerTransformStr;
  imageToTransducerTransformStr << this->CurrentPixelSpacingMm[0] << " 0 0 " << -1.0 * this->CurrentTransducerOriginPixels[0]*this->CurrentPixelSpacingMm[0];
  imageToTransducerTransformStr << " 0 " << this->CurrentPixelSpacingMm[1] << " 0 " << -1.0 * this->CurrentTransducerOriginPixels[1]*this->CurrentPixelSpacingMm[1];
  imageToTransducerTransformStr << " 0 0 " << this->CurrentPixelSpacingMm[2] << " " << -1.0 * this->CurrentTransducerOriginPixels[2]*this->CurrentPixelSpacingMm[2];
  imageToTransducerTransformStr << " 0 0 0 1";
  customFields[imageToTransducerName.str()] = imageToTransducerTransformStr.str();
  imageToTransducerName << "Status";
  customFields[imageToTransducerName.str()] = "OK";
}
