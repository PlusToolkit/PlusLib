/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusOpenIGTLinkVideoSource.h"

#include "igtlImageMessage.h"
//#include "igsioVideoFrame.h"
//#include "igsioTrackedFrame.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusIgtlMessageCommon.h"

vtkStandardNewMacro(vtkPlusOpenIGTLinkVideoSource);

//----------------------------------------------------------------------------
vtkPlusOpenIGTLinkVideoSource::vtkPlusOpenIGTLinkVideoSource()
{
  this->RequireImageOrientationInConfiguration = true;
}

//----------------------------------------------------------------------------
vtkPlusOpenIGTLinkVideoSource::~vtkPlusOpenIGTLinkVideoSource()
{
}

//----------------------------------------------------------------------------
void vtkPlusOpenIGTLinkVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkVideoSource::InternalUpdate()
{
  LOG_TRACE("vtkPlusOpenIGTLinkVideoSource::InternalUpdate");
  if (!this->IsRecording() || !this->GetConnected())
  {
    // drop the frame, we are not recording data now
    return PLUS_SUCCESS;
  }

  igtl::MessageHeader::Pointer headerMsg;
  if (ReceiveMessageHeader(headerMsg) == PLUS_FAIL)
  {
    if (!this->IsRecording() || !this->GetConnected())
    {
      // Disconnect while waiting for message, exit gracefully
      return PLUS_SUCCESS;
    }
    OnReceiveTimeout();
    return PLUS_FAIL;
  }

  if (headerMsg == nullptr)
  {
    // Not a problem, just no messages received this timeout period
    return PLUS_SUCCESS;
  }

  // We've received valid header data
  headerMsg->Unpack(this->IgtlMessageCrcCheckEnabled);

  // Set unfiltered and filtered timestamp by converting UTC to system timestamp
  double unfilteredTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();

  igsioTrackedFrame trackedFrame;
  igtl::MessageBase::Pointer bodyMsg = this->MessageFactory->CreateReceiveMessage(headerMsg);

  if (typeid(*bodyMsg) == typeid(igtl::ImageMessage))
  {
    if (vtkPlusIgtlMessageCommon::UnpackImageMessage(bodyMsg, this->ClientSocket, trackedFrame, this->ImageMessageEmbeddedTransformName, this->IgtlMessageCrcCheckEnabled) != PLUS_SUCCESS)
    {
      LOG_ERROR("Couldn't get image from OpenIGTLink server!");
      return PLUS_FAIL;
    }
  }
  else if (typeid(*bodyMsg) == typeid(igtl::PlusTrackedFrameMessage))
  {
    if (vtkPlusIgtlMessageCommon::UnpackTrackedFrameMessage(bodyMsg, this->ClientSocket, trackedFrame, this->ImageMessageEmbeddedTransformName, this->IgtlMessageCrcCheckEnabled) != PLUS_SUCCESS)
    {
      LOG_ERROR("Couldn't get tracked frame from OpenIGTLink server!");
      return PLUS_FAIL;
    }
    double unfilteredTimestampUtc = trackedFrame.GetTimestamp();
    if (this->UseReceivedTimestamps)
    {
      // Use the timestamp in the OpenIGTLink message
      // The received timestamp is in UTC and timestamps in the buffer are in system time, so conversion is needed
      unfilteredTimestamp = vtkIGSIOAccurateTimer::GetSystemTimeFromUniversalTime(unfilteredTimestampUtc);
    }
  }
  else
  {
    // if the data type is unknown, skip reading.
    igsioLockGuard<vtkIGSIORecursiveCriticalSection> socketGuard(this->SocketMutex);
    this->ClientSocket->Skip(headerMsg->GetBodySizeToRead(), 0);
    return PLUS_SUCCESS;
  }

  // No need to filter already filtered timestamped items received over OpenIGTLink
  // If the original timestamps are not used it's still safer not to use filtering, as filtering assumes uniform frame rate, which is not guaranteed
  double filteredTimestamp = unfilteredTimestamp;

  // The timestamps are already defined, so we don't need to filter them,
  // for simplicity, we increase frame number always by 1.
  this->FrameNumber++;

  vtkPlusDataSource* aSource = NULL;
  if (this->GetFirstActiveOutputVideoSource(aSource) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to retrieve the video source in the OpenIGTLinkVideo device.");
    return PLUS_FAIL;
  }

  // If the buffer is empty, set the pixel type and frame size to the first received properties
  if (aSource->GetNumberOfItems() == 0)
  {
    igsioVideoFrame* videoFrame = trackedFrame.GetImageData();
    if (videoFrame == NULL)
    {
      LOG_ERROR("Invalid video frame received, cannot use it to initialize the video buffer");
      return PLUS_FAIL;
    }
    aSource->SetPixelType(videoFrame->GetVTKScalarPixelType());
    unsigned int numberOfScalarComponents(1);
    if (videoFrame->GetNumberOfScalarComponents(numberOfScalarComponents) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to retrieve number of scalar components.");
      return PLUS_FAIL;
    }
    aSource->SetNumberOfScalarComponents(numberOfScalarComponents);
    aSource->SetImageType(videoFrame->GetImageType());
    aSource->SetInputFrameSize(trackedFrame.GetFrameSize());
  }
  igsioTrackedFrame::FieldMapType customFields = trackedFrame.GetCustomFields();
  PlusStatus status = aSource->AddItem(trackedFrame.GetImageData(), this->FrameNumber, unfilteredTimestamp, filteredTimestamp, &customFields);
  this->Modified();

  return status;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(ImageMessageEmbeddedTransformName, deviceConfig);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
  deviceConfig->SetAttribute("ImageMessageEmbeddedTransformName", this->ImageMessageEmbeddedTransformName.GetTransformName().c_str());
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenIGTLinkVideoSource::NotifyConfigured()
{
  if (this->OutputChannels.size() > 1)
  {
    LOG_WARNING("vtkPlusOpenIGTLinkVideoSource is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
    return PLUS_FAIL;
  }

  if (this->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined for vtkPlusOpenIGTLinkVideoSource. Cannot proceed.");
    this->SetCorrectlyConfigured(false);
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}