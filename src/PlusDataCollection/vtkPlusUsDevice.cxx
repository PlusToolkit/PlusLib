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

vtkStandardNewMacro( vtkPlusUsDevice );

//----------------------------------------------------------------------------
vtkPlusUsDevice::vtkPlusUsDevice()
  : vtkPlusDevice()
  , RequestedImagingParameters( vtkPlusUsImagingParameters::New() )
  , CurrentImagingParameters( vtkPlusUsImagingParameters::New() )
  , TextRecognizerInputChannelName( NULL )
  , InputChannel( NULL )
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
  RequestedImagingParameters->Delete();
  CurrentImagingParameters->Delete();
}

//----------------------------------------------------------------------------
void vtkPlusUsDevice::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
  os << indent << "Requested imaging parameters: " << std::endl;
  this->RequestedImagingParameters->PrintSelf( os, indent );
  os << indent << "Current imaging parameters: " << std::endl;
  this->CurrentImagingParameters->PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsDevice::ReadConfiguration( vtkXMLDataElement* rootConfigElement )
{
  vtkXMLDataElement* deviceConfig = this->FindThisDeviceElement( rootConfigElement );
  if ( deviceConfig == NULL )
  {
    LOG_ERROR( "Unable to continue configuration of " << this->GetClassName() << ". Could not find corresponding element." );
    return PLUS_FAIL;
  }

  XML_READ_CSTRING_ATTRIBUTE_OPTIONAL( TextRecognizerInputChannelName, deviceConfig );
  const char* transformName = deviceConfig->GetAttribute( "ImageToTransducerTransformName" );
  if( transformName != NULL && this->ImageToTransducerTransform.SetTransformName( transformName ) != PLUS_SUCCESS )
  {
    LOG_ERROR( "Transform name is not properly formatted. It should be of the format <From>ToTransducer." );
  }
  XML_FIND_NESTED_ELEMENT_OPTIONAL( imagingParams, deviceConfig, vtkPlusUsImagingParameters::XML_ELEMENT_TAG );

  if( imagingParams != NULL )
  {
    this->RequestedImagingParameters->ReadConfiguration( deviceConfig );
  }

  return Superclass::ReadConfiguration( rootConfigElement );
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsDevice::WriteConfiguration( vtkXMLDataElement* deviceConfig )
{
  XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING( imagingParams, deviceConfig, vtkPlusUsImagingParameters::XML_ELEMENT_TAG );
  XML_WRITE_CSTRING_ATTRIBUTE_IF_NOT_NULL( TextRecognizerInputChannelName, deviceConfig );
  if( this->TextRecognizerInputChannelName != NULL )
  {
    deviceConfig->SetAttribute( "ImageToTransducerTransformName", this->ImageToTransducerTransform.GetTransformName().c_str() );
  }

  return this->CurrentImagingParameters->WriteConfiguration( deviceConfig );
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsDevice::InternalUpdate()
{
  if( this->InputChannel != NULL )
  {
    double aTimestamp( UNDEFINED_TIMESTAMP );
    PlusTrackedFrame frame;
    if( this->InputChannel->GetTrackedFrame( aTimestamp, frame, false ) != PLUS_SUCCESS )
    {
      LOG_ERROR( "Unable to retrieve frame from the input channel. No parameters can be retrieved." );
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
    for( ChannelContainerIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it )
    {
      vtkPlusChannel* channel = *it;
      if( STRCASECMP( channel->GetChannelId(), this->TextRecognizerInputChannelName ) == 0 )
      {
        this->InputChannel = channel;
        return PLUS_SUCCESS;
      }
    }
    LOG_ERROR( "Unable to find channel " << this->TextRecognizerInputChannelName << ". Did you add it in the XML?" );
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsDevice::SetNewImagingParameters( const vtkPlusUsImagingParameters& newImagingParameters )
{
  if( this->RequestedImagingParameters->DeepCopy( newImagingParameters ) == PLUS_FAIL )
  {
    LOG_ERROR( "Unable to deep copy new imaging parameters." );
    return PLUS_FAIL;
  }

  if (this->RequestImagingParameterChange() == PLUS_FAIL)
  {
    LOG_ERROR( "Failed to request change of imaging parameters in the device." );
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsDevice::AddVideoItemToVideoSources( const std::vector<vtkPlusDataSource*>& videoSources, const PlusVideoFrame& frame, long frameNumber, double unfilteredTimestamp/*=UNDEFINED_TIMESTAMP*/, double filteredTimestamp/*=UNDEFINED_TIMESTAMP*/, const PlusTrackedFrame::FieldMapType* customFields /*= NULL*/ )
{
  PlusTrackedFrame::FieldMapType localCustomFields;
  if ( !this->ImageToTransducerTransform.GetTransformName().empty() && customFields != NULL )
  {
    localCustomFields = *customFields;
    this->CalculateImageToTransducer( localCustomFields );
  }

  return Superclass::AddVideoItemToVideoSources( videoSources, frame, frameNumber, unfilteredTimestamp, filteredTimestamp, &localCustomFields );
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsDevice::AddVideoItemToVideoSources( const std::vector<vtkPlusDataSource*>& videoSources, void* imageDataPtr, US_IMAGE_ORIENTATION usImageOrientation, const int frameSizeInPx[3], PlusCommon::VTKScalarPixelType pixelType, int numberOfScalarComponents, US_IMAGE_TYPE imageType, int numberOfBytesToSkip, long frameNumber, double unfilteredTimestamp/*=UNDEFINED_TIMESTAMP*/, double filteredTimestamp/*=UNDEFINED_TIMESTAMP*/, const PlusTrackedFrame::FieldMapType* customFields/*= NULL*/ )
{
  if ( frameSizeInPx[0] < 0 || frameSizeInPx[1] < 0 || frameSizeInPx[2] < 0 || numberOfScalarComponents < 0 )
  {
    LOG_ERROR( "Invalid negative values sent to vtkPlusUsDevice::AddVideoItemToVideoSources. Aborting." );
    return PLUS_FAIL;
  }

  unsigned int frameSizeInPxUint[3] = { static_cast<unsigned int>( frameSizeInPx[0] ), static_cast<unsigned int>( frameSizeInPx[1] ), static_cast<unsigned int>( frameSizeInPx[2] ) };
  return this->AddVideoItemToVideoSources(videoSources, imageDataPtr, usImageOrientation, frameSizeInPxUint, pixelType, static_cast<unsigned int>(numberOfScalarComponents), imageType, numberOfBytesToSkip, frameNumber, unfilteredTimestamp, filteredTimestamp, customFields);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsDevice::AddVideoItemToVideoSources( const std::vector<vtkPlusDataSource*>& videoSources, void* imageDataPtr, US_IMAGE_ORIENTATION usImageOrientation, const unsigned int frameSizeInPx[3], PlusCommon::VTKScalarPixelType pixelType, unsigned int numberOfScalarComponents, US_IMAGE_TYPE imageType, int numberOfBytesToSkip, long frameNumber, double unfilteredTimestamp /*= UNDEFINED_TIMESTAMP*/, double filteredTimestamp /*= UNDEFINED_TIMESTAMP*/, const PlusTrackedFrame::FieldMapType* customFields /*= NULL*/ )
{
  PlusTrackedFrame::FieldMapType localCustomFields;
  if ( customFields != NULL )
  {
    localCustomFields = *customFields;
    if ( this->ImageToTransducerTransform.IsValid() )
    {
      this->CalculateImageToTransducer( localCustomFields );
    }
  }

  return Superclass::AddVideoItemToVideoSources( videoSources, imageDataPtr, usImageOrientation, frameSizeInPx, pixelType, numberOfScalarComponents, imageType, numberOfBytesToSkip, frameNumber, unfilteredTimestamp, filteredTimestamp, &localCustomFields );
}

//----------------------------------------------------------------------------
void vtkPlusUsDevice::CalculateImageToTransducer( PlusTrackedFrame::FieldMapType& customFields )
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