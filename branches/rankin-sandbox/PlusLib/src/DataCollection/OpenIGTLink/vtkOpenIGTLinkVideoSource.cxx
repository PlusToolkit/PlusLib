/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkOpenIGTLinkVideoSource.h"

#include "igtlImageMessage.h"
#include "PlusVideoFrame.h"
#include "TrackedFrame.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusIgtlMessageCommon.h"

vtkStandardNewMacro(vtkOpenIGTLinkVideoSource);

//----------------------------------------------------------------------------
vtkOpenIGTLinkVideoSource::vtkOpenIGTLinkVideoSource()
{
  this->RequireImageOrientationInConfiguration = true;

}

//----------------------------------------------------------------------------
vtkOpenIGTLinkVideoSource::~vtkOpenIGTLinkVideoSource()
{
}

//----------------------------------------------------------------------------
void vtkOpenIGTLinkVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkVideoSource::InternalUpdate()
{
  LOG_TRACE( "vtkOpenIGTLinkVideoSource::InternalUpdate" );
  if (!this->Recording)
  {
    // drop the frame, we are not recording data now
    return PLUS_SUCCESS;
  }

  igtl::MessageHeader::Pointer headerMsg;
  ReceiveMessageHeaderWithErrorHandling(headerMsg);
  
  if (headerMsg.IsNull())
  {
    OnReceiveTimeout();
    return PLUS_FAIL;
  }

  // We've received valid header data
  headerMsg->Unpack(this->IgtlMessageCrcCheckEnabled);

  // Set unfiltered and filtered timestamp by converting UTC to system timestamp
  double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();

  TrackedFrame trackedFrame;
  if (strcmp(headerMsg->GetDeviceType(), "IMAGE") == 0)
  {
    if (vtkPlusIgtlMessageCommon::UnpackImageMessage( headerMsg, this->ClientSocket, trackedFrame, this->ImageMessageEmbeddedTransformName, this->IgtlMessageCrcCheckEnabled)!=PLUS_SUCCESS)
    {
      LOG_ERROR("Couldn't get image from OpenIGTLink server!"); 
      return PLUS_FAIL;
    }
  }
  else if (strcmp(headerMsg->GetDeviceType(), "TRACKEDFRAME") == 0)
  {
    if ( vtkPlusIgtlMessageCommon::UnpackTrackedFrameMessage( headerMsg, this->ClientSocket, trackedFrame, this->IgtlMessageCrcCheckEnabled ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Couldn't get tracked frame from OpenIGTLink server!"); 
      return PLUS_FAIL; 
    }
    double unfilteredTimestampUtc = trackedFrame.GetTimestamp();
    if (this->UseReceivedTimestamps)
    {
      // Use the timestamp in the OpenIGTLink message
      // The received timestamp is in UTC and timestampts in the buffer are in system time, so conversion is needed
      unfilteredTimestamp = vtkAccurateTimer::GetSystemTimeFromUniversalTime(unfilteredTimestampUtc); 
    }
  }
  else
  {
    // if the data type is unknown, skip reading. 
    this->ClientSocket->Skip(headerMsg->GetBodySizeToRead(), 0);
    return PLUS_SUCCESS; 
  }

  // No need to filter already filtered timestamped items received over OpenIGTLink 
  // If the original timestamps are not used it's still safer not to use filtering, as filtering assumes uniform framerate, which is not guaranteed
  double filteredTimestamp = unfilteredTimestamp;

  // The timestamps are already defined, so we don't need to filter them, 
  // for simplicity, we increase frame number always by 1.
  this->FrameNumber++;

  vtkPlusDataSource* aSource=NULL;
  if( this->GetFirstActiveOutputVideoSource(aSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve the video source in the OpenIGTLinkVideo device.");
    return PLUS_FAIL;
  }

  // If the buffer is empty, set the pixel type and frame size to the first received properties 
  if ( aSource->GetNumberOfItems() == 0 )
  {
    PlusVideoFrame* videoFrame=trackedFrame.GetImageData();
    if (videoFrame==NULL)
    {
      LOG_ERROR("Invalid video frame received, cannot use it to initialize the video buffer");
      return PLUS_FAIL;
    }
    aSource->SetPixelType( videoFrame->GetVTKScalarPixelType() );
    aSource->SetNumberOfScalarComponents( videoFrame->GetNumberOfScalarComponents() );
    aSource->SetImageType( videoFrame->GetImageType() );
    aSource->SetInputFrameSize( trackedFrame.GetFrameSize() );
  }
  TrackedFrame::FieldMapType customFields=trackedFrame.GetCustomFields();
  PlusStatus status = aSource->AddItem( trackedFrame.GetImageData(), this->FrameNumber, unfilteredTimestamp, filteredTimestamp, &customFields); 
  this->Modified();

  return status;
}

//-----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(ImageMessageEmbeddedTransformName, deviceConfig);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
  deviceConfig->SetAttribute("ImageMessageEmbeddedTransformName", this->ImageMessageEmbeddedTransformName.GetTransformName().c_str());
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkVideoSource::NotifyConfigured()
{
  if( this->OutputChannels.size() > 1 )
  {
    LOG_WARNING("vtkOpenIGTLinkVideoSource is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
    return PLUS_FAIL;
  }

  if( this->OutputChannels.empty() )
  {
    LOG_ERROR("No output channels defined for vtkOpenIGTLinkVideoSource. Cannot proceed." );
    this->SetCorrectlyConfigured(false);
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkVideoSource::SetImageMessageEmbeddedTransformName(const char* nameString)
{
  return this->ImageMessageEmbeddedTransformName.SetTransformName(nameString);
}
