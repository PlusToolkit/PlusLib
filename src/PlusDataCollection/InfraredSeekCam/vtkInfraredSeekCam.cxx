/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkInfraredSeekCam.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkObjectFactory.h>

// OpenCV includes
#include <opencv2/highgui/highgui.hpp>

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkInfraredSeekCam);

//----------------------------------------------------------------------------
vtkInfraredSeekCam::vtkInfraredSeekCam()
{
  this->RequireImageOrientationInConfiguration = true;
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkInfraredSeekCam::~vtkInfraredSeekCam()
{
}

//----------------------------------------------------------------------------
void vtkInfraredSeekCam::PrintSelf(ostream& os, vtkIndent indent)
{
 
   this->Superclass::PrintSelf(os, indent);

   os << indent << "InfraredSeekCam: Pro Seek Camera" << std::endl;
}

//-----------------------------------------------------------------------------
PlusStatus vtkInfraredSeekCam::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  LOG_DEBUG("Configure Pro Seek Camera");

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkInfraredSeekCam::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkInfraredSeekCam::FreezeDevice(bool freeze)
{
    
  if (freeze)
  {
    this->Disconnect();
  }
  else
  {
    this->Connect();
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkInfraredSeekCam::InternalConnect()
{

  // Select the camera. Seek Pro is default.

  this->Capture = std::make_shared<LibSeek::SeekThermalPro>();

  this->Frame = std::make_shared<cv::Mat>();
   
  if (!this->Capture->open())
  {
    LOG_ERROR("Failed to open seek pro");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkInfraredSeekCam::InternalDisconnect()
{
  this->Capture = nullptr; // automatically closes resources/connections
  this->Frame = nullptr;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkInfraredSeekCam::InternalUpdate()
{

  if (!this->Capture->isOpened())
  {
    // No need to update if we're not able to read data
    LOG_ERROR("vtkInfraredSeekCam::InternalUpdate Unable to read date");
    return PLUS_SUCCESS;
  }

  // Capture one frame from the SeekPro capture device
  if (!this->Capture->read(*this->Frame))
  {
    LOG_ERROR("Unable to receive frame");
    return PLUS_FAIL;
  }
  
  // TODO: check these sentences
  vtkPlusDataSource* aSource(nullptr);
  if (this->GetFirstActiveOutputVideoSource(aSource) == PLUS_FAIL || aSource == nullptr)
  {
    LOG_ERROR("Unable to grab a video source. Skipping frame.");
    return PLUS_FAIL;
  }

  if (aSource->GetNumberOfItems() == 0)
  {
    // Init the buffer with the metadata from the first frame
    aSource->SetImageType(US_IMG_BRIGHTNESS);
    aSource->SetPixelType(VTK_TYPE_UINT16);
    aSource->SetNumberOfScalarComponents(1);
    aSource->SetInputFrameSize(this->Frame->cols, this->Frame->rows, 1);
  }

  // Add the frame to the stream buffer
  int frameSize[3] = { this->Frame->cols, this->Frame->rows, 1 };
  if (aSource->AddItem(this->Frame->data, aSource->GetInputImageOrientation(), frameSize, VTK_TYPE_UINT16, 1, US_IMG_BRIGHTNESS, 0, this->FrameNumber) == PLUS_FAIL)
  {
    return PLUS_FAIL;
  }

  this->FrameNumber++;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkInfraredSeekCam::NotifyConfigured()
{
  if (this->OutputChannels.size() > 1)
  {
    LOG_WARNING("vtkInfraredSeekCam is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
  }

  if (this->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined for vtkInfraredSeekCam. Cannot proceed.");
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}