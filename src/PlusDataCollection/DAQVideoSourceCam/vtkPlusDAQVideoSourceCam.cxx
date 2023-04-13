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
#include "vtkPlusDAQVideoSourceCam.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkObjectFactory.h>

// DAQ System
#include "usb3_frm13_import.h"

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusDAQVideoSourceCam);

//----------------------------------------------------------------------------
vtkPlusDAQVideoSourceCam::vtkPlusDAQVideoSourceCam()
{
  this->RequireImageOrientationInConfiguration = true;
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkPlusDAQVideoSourceCam::~vtkPlusDAQVideoSourceCam()
{
}

//----------------------------------------------------------------------------
void vtkPlusDAQVideoSourceCam::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "DAQVideoSourceCam: CameraLink Camera" << std::endl;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDAQVideoSourceCam::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  LOG_DEBUG("Configure CameraLink Camera");

  this->m_dataMode = DATAMODE_8;
  
  const char* dataModeString = deviceConfig->GetAttribute("DataMode");
  if (dataModeString)
  {
    switch(std::atoi(dataModeString)){
    case 8:
      this->m_dataMode = DATAMODE_8;
      break;
    case 16:
      this->m_dataMode = DATAMODE_16;
      break;
    default:
      this->m_dataMode = DATAMODE_8;
    }
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDAQVideoSourceCam::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDAQVideoSourceCam::InternalConnect()
{
  this->deviceRunning = false;

  if (!OpenDAQDevice())
  {
    LOG_ERROR("DAQVideoSourceCam CameraLink: Failed to open.");
    return PLUS_FAIL;
  }

  this->cameraMode = CAMERAMODE_SCAN;
  switch (this->m_dataMode) {
  case DATAMODE_8:
    this->m_nbytesMode = 2;
    break;
  default:
    this->m_nbytesMode = 4;
  }

  if (!LVDS_Init())
  {
    LOG_ERROR("DAQVideoSourceCam CameraLink: Failed to init.");
    return PLUS_FAIL;
  }
  LVDS_SetDataMode(this->m_dataMode);
  LVDS_CameraMode(this->cameraMode);
  LVDS_SetDeUse(true);
  LVDS_GetResolution(&(this->m_width), &(this->m_height));
  this->m_nwidth = this->m_width / 2;
  this->m_nheight = this->m_height;
  this->m_maxBuffSize = this->m_width * this->m_height;
  this->pImgBuf = new CAMERADATATYPE_t[this->m_maxBuffSize];
  this->m_dwCharCount = this->m_nbytesMode * this->m_maxBuffSize;
  this->pImgBufAux = new unsigned char[this->m_dwCharCount];
  
  if (!LVDS_Start())
  {
    LOG_ERROR("DAQVideoSourceCam CameraLink: Failed to start");
    return PLUS_FAIL;
  }
  this->deviceRunning = true;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDAQVideoSourceCam::InternalDisconnect()
{
  LVDS_Stop();
  CloseDAQDevice();
  this->pImgBuf = nullptr;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDAQVideoSourceCam::InternalUpdate()
{
  std::ostringstream ss;
  int ix,iy;

  if (!this->deviceRunning)
  {
    LOG_ERROR("vtkPlusDAQVideoSourceCam::InternalUpdate Unable to read data");
    return PLUS_SUCCESS; 
  }

  this->m_currentTime = vtkIGSIOAccurateTimer::GetSystemTime();
  if (!LVDS_GetFrame(&m_dwCharCount, (unsigned char*)(this->pImgBufAux)))
  {
    LOG_DEBUG("vtkPlusDAQVideoSourceCam::InternalUpdate Unable to receive frame");
    return PLUS_SUCCESS; 
  }

  // Byte assignment to output image buffer
  for (ix = 0, iy = 0; ix < this->m_maxBuffSize; ix+=2, iy += this->m_nbytesMode) {
    ((unsigned char*)this->pImgBuf)[ix + 0] = this->pImgBufAux[iy + 0] ;
    ((unsigned char*)this->pImgBuf)[ix + 1] = this->pImgBufAux[iy + 1];
  }

  vtkPlusDataSource* aSource(nullptr);
  if (this->GetFirstActiveOutputVideoSource(aSource) == PLUS_FAIL || aSource == nullptr)
  {
    LOG_DEBUG("Unable to grab a video source. Skipping frame.");
    return PLUS_SUCCESS;
  }

  if (aSource->GetNumberOfItems() == 0)
  {
    // Init the buffer with the metadata from the first frame
    aSource->SetImageType(US_IMG_BRIGHTNESS);
    aSource->SetPixelType(VTK_UNSIGNED_SHORT);
    aSource->SetNumberOfScalarComponents(1);
    aSource->SetInputFrameSize(this->m_nwidth, this->m_nheight, 1);
  }

  // Add the frame to the stream buffer
  FrameSizeType frameSize = { static_cast<unsigned int>(this->m_nwidth), static_cast<unsigned int>(this->m_nheight), 1 };
  if (aSource->AddItem(this->pImgBuf, aSource->GetInputImageOrientation(), frameSize, VTK_UNSIGNED_SHORT, 1, US_IMG_BRIGHTNESS, 0, this->FrameNumber, this->m_currentTime, this->m_currentTime) == PLUS_FAIL)
  {
    return PLUS_FAIL;
  }
  this->FrameNumber++;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDAQVideoSourceCam::NotifyConfigured()
{
  if (this->OutputChannels.size() > 1)
  {
    LOG_WARNING("vtkPlusDAQVideoSourceCam is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
  }

  if (this->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined for vtkPlusDAQVideoSourceCam. Cannot proceed.");
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}
