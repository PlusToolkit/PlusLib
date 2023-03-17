/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.

Developed by ULL & IACTEC-IAC group
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusDAQUsb3Frm13BCam.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkObjectFactory.h>

// TEEV2
#include "usb3_frm13_import.h"

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusDAQUsb3Frm13BCam);

//----------------------------------------------------------------------------
vtkPlusDAQUsb3Frm13BCam::vtkPlusDAQUsb3Frm13BCam()
{
  this->RequireImageOrientationInConfiguration = true;
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkPlusDAQUsb3Frm13BCam::~vtkPlusDAQUsb3Frm13BCam()
{
}

//----------------------------------------------------------------------------
void vtkPlusDAQUsb3Frm13BCam::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "DAQUsb3Frm13BCam: CameraLink Camera" << std::endl;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDAQUsb3Frm13BCam::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  LOG_DEBUG("Configure CameraLink Camera");
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDAQUsb3Frm13BCam::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDAQUsb3Frm13BCam::InternalConnect()
{
  this->deviceRunning = false;

  if (!OpenDAQDevice())
  {
    LOG_ERROR("DAQ System CameraLink: Failed to open.");
    return PLUS_FAIL;
  }

  if (!LVDS_Init())
  {
    LOG_ERROR("DAQ System CameraLink: Failed to init.");
    return PLUS_FAIL;
  }

  this->colorDepth = COLORDEPTH_32;
  this->dataMode = DATAMODE_32;
  this->cameraMode = CAMERAMODE_SCAN;
  LVDS_GetResolution(&(this->width), &(this->height));
  this->maxBuffSize = this->width * this->height * this->colorDepth;
  this->pImgBuf = new unsigned short[this->maxBuffSize];

  if (!LVDS_Start())
  {
    LOG_ERROR("DAQ System CameraLink: Failed to start");
    return PLUS_FAIL;
  }
  this->deviceRunning = true;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDAQUsb3Frm13BCam::InternalDisconnect()
{
  LVDS_Stop();
  CloseDAQDevice();
  this->pImgBuf = nullptr;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDAQUsb3Frm13BCam::InternalUpdate()
{
  DWORD dwCount; 
  if (!this->deviceRunning)
  {
    LOG_ERROR("vtkPlusDAQUsb3Frm13BCam::InternalUpdate Unable to read data");
    return PLUS_SUCCESS; 
  }

  dwCount = this->maxBuffSize;

  if (!LVDS_GetFrame(&dwCount, this->pImgBuf))
  {
    LOG_ERROR("vtkPlusDAQUsb3Frm13BCam::InternalUpdate Unable to receive frame");
    return PLUS_SUCCESS; 
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
    aSource->SetImageType(US_IMG_BRIGHTNESS);
    aSource->SetPixelType(VTK_UNSIGNED_INT);     
    aSource->SetNumberOfScalarComponents(1);
    aSource->SetInputFrameSize(this->width, this->height, 1);
  }

  // Add the frame to the stream buffer
  FrameSizeType frameSize = { static_cast<unsigned int>(this->width), static_cast<unsigned int>(this->height), 1 };
   if (aSource->AddItem(this->pImgBuf, aSource->GetInputImageOrientation(), frameSize, VTK_UNSIGNED_INT, 1, US_IMG_BRIGHTNESS, 0, this->FrameNumber) == PLUS_FAIL)
  {
    return PLUS_FAIL;
  }
  this->FrameNumber++;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDAQUsb3Frm13BCam::NotifyConfigured()
{
  if (this->OutputChannels.size() > 1)
  {
    LOG_WARNING("vtkPlusDAQUsb3Frm13BCam is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
  }

  if (this->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined for vtkPlusDAQUsb3Frm13BCam. Cannot proceed.");
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}
