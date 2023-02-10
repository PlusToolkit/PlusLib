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
#include "vtkPlusInfraredTEEV2Cam.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkObjectFactory.h>

// TEEV2
#include "i3system_TE.h"

using namespace i3;

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusInfraredTEEV2Cam);

//----------------------------------------------------------------------------
vtkPlusInfraredTEEV2Cam::vtkPlusInfraredTEEV2Cam()
{
  this->RequireImageOrientationInConfiguration = true;
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkPlusInfraredTEEV2Cam::~vtkPlusInfraredTEEV2Cam()
{
}

//----------------------------------------------------------------------------
void vtkPlusInfraredTEEV2Cam::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "InfraredTEEV2Cam: Thermal Expert EV2 Camera" << std::endl;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusInfraredTEEV2Cam::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  LOG_DEBUG("Configure Thermal Expert EV2");
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusInfraredTEEV2Cam::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusInfraredTEEV2Cam::FreezeDevice(bool freeze)
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
PlusStatus vtkPlusInfraredTEEV2Cam::InternalConnect()
{
  SCANINFO* pScan = new SCANINFO[MAX_USB_NUM];
  ScanTE(pScan);
  this->device = -1;
  for (int i = 0; i < MAX_USB_NUM; i++)
  {
    if (pScan[i].bDevCon)
    {
      this->device = i;
      break;
    }
  }
  delete pScan;

  if (this->device == -1)
  {
    LOG_ERROR("Thermal Expert EV2: camera not detected.");
    return PLUS_FAIL;
  }
  else {
    LOG_DEBUG("Thermal Expert EV2 Camera handle: " << this->device);
  }

  this->pTE = OpenTE_A(this->device);

  if (!this->pTE)
  {
    LOG_ERROR("Thermal Expert EV2: Failed to open.");
    return PLUS_FAIL;
  }

  int width = this->pTE->GetImageWidth();
  int height = this->pTE->GetImageHeight();
  this->pImgBuf = new unsigned short[width * height]; 
  this->width = width;
  this->height = height;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusInfraredTEEV2Cam::InternalDisconnect()
{
  this->pTE->CloseTE();
  this->pImgBuf = nullptr;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusInfraredTEEV2Cam::InternalUpdate()
{
  if (!this->pTE)
  {
    LOG_ERROR("vtkPlusInfraredTEEV2Cam::InternalUpdate Unable to read date");
    return PLUS_SUCCESS; 
  }

  if (!this->pTE->RecvImage(this->pImgBuf))
  {
    LOG_ERROR("vtkPlusInfraredTEEV2Cam::InternalUpdate Unable to receive frame");
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
    aSource->SetPixelType(VTK_UNSIGNED_SHORT);     
    aSource->SetNumberOfScalarComponents(1);
    aSource->SetInputFrameSize(this->width, this->height, 1);
  }

  // Add the frame to the stream buffer
  FrameSizeType frameSize = { static_cast<unsigned int>(this->width), static_cast<unsigned int>(this->height), 1 };
   if (aSource->AddItem(this->pImgBuf, aSource->GetInputImageOrientation(), frameSize, VTK_UNSIGNED_SHORT, 1, US_IMG_BRIGHTNESS, 0, this->FrameNumber) == PLUS_FAIL)
  {
    return PLUS_FAIL;
  }
  this->FrameNumber++;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusInfraredTEEV2Cam::NotifyConfigured()
{
  if (this->OutputChannels.size() > 1)
  {
    LOG_WARNING("vtkPlusInfraredTEEV2Cam is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
  }

  if (this->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined for vtkPlusInfraredTEEV2Cam. Cannot proceed.");
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}
