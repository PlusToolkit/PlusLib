/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusRTSPVideoSource.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkObjectFactory.h>

// OpenCV includes
#include <opencv2/videoio.hpp>

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusRTSPVideoSource);

//----------------------------------------------------------------------------
vtkPlusRTSPVideoSource::vtkPlusRTSPVideoSource()
  : StreamURL("")
{
  this->RequireImageOrientationInConfiguration = true;
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkPlusRTSPVideoSource::~vtkPlusRTSPVideoSource()
{
}

//----------------------------------------------------------------------------
void vtkPlusRTSPVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "StreamURL: " << this->StreamURL << std::endl;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusRTSPVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusRTSPVideoSource::ReadConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  XML_READ_STRING_ATTRIBUTE_REQUIRED(StreamURL, deviceConfig);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusRTSPVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
  deviceConfig->SetAttribute("StreamURL", this->StreamURL.c_str());
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusRTSPVideoSource::FreezeDevice(bool freeze)
{

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusRTSPVideoSource::InternalConnect()
{
  this->Capture = std::make_shared<cv::VideoCapture>(this->StreamURL, cv::CAP_FFMPEG);
  this->Frame = std::make_shared<cv::Mat>();

  if (!this->Capture->isOpened())
  {
    LOG_ERROR("Unable to open stream at " << this->StreamURL);
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusRTSPVideoSource::InternalDisconnect()
{
  this->Capture = nullptr; // automatically closes resources/connections
  this->Frame = nullptr;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusRTSPVideoSource::InternalUpdate()
{
  LOG_TRACE("vtkPlusRTSPVideoSource::InternalUpdate");

  // Capture one frame from the RTSP device
  if (!this->Capture->read(*this->Frame))
  {
    LOG_ERROR("Unable to receive frame");
    return PLUS_FAIL;
  }

  vtkPlusDataSource* aSource(nullptr);
  if (this->GetFirstActiveOutputVideoSource(aSource) == PLUS_FAIL || aSource == nullptr)
  {
    LOG_ERROR("Unable to grab a video source. Skipping frame.");
    return PLUS_FAIL;
  }

  if (aSource->GetNumberOfItems() == 0)
  {
    // Init the buffer with the metadata from the first frame
    aSource->SetImageType(US_IMG_RGB_COLOR);
    aSource->SetPixelType(VTK_UNSIGNED_CHAR);
    aSource->SetNumberOfScalarComponents(3);
    aSource->SetInputFrameSize(this->Frame->cols, this->Frame->rows, 1);
  }

  // Add the frame to the stream buffer
  int frameSize[3] = { this->Frame->cols, this->Frame->rows, 1 };
  if (aSource->AddItem(this->Frame->data, US_IMG_ORIENT_MF, frameSize, VTK_UNSIGNED_CHAR, 3, US_IMG_RGB_COLOR, 0, this->FrameNumber) == PLUS_FAIL)
  {
    return PLUS_FAIL;
  }

  this->FrameNumber++;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusRTSPVideoSource::NotifyConfigured()
{
  if (this->OutputChannels.size() > 1)
  {
    LOG_WARNING("vtkPlusRTSPVideoSource is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
  }

  if (this->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined for vtkPlusRTSPVideoSource. Cannot proceed.");
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}