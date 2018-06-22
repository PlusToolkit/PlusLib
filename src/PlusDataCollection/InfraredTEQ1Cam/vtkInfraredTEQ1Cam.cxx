/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.

Developed by MACBIOIDI-ULPGC & IACTEC-IAC group
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkInfraredTEQ1Cam.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkObjectFactory.h>

// TEQ1
#include "i3system_TE.h"

using namespace i3;

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkInfraredTEQ1Cam);

//----------------------------------------------------------------------------
vtkInfraredTEQ1Cam::vtkInfraredTEQ1Cam()
{
  this->RequireImageOrientationInConfiguration = true;
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkInfraredTEQ1Cam::~vtkInfraredTEQ1Cam()
{
}

//----------------------------------------------------------------------------
void vtkInfraredTEQ1Cam::PrintSelf(ostream& os, vtkIndent indent)
{

  this->Superclass::PrintSelf(os, indent);

  os << indent << "InfraredTEQ1Cam: Pro Seek Camera" << std::endl;
}

//-----------------------------------------------------------------------------
PlusStatus vtkInfraredTEQ1Cam::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  LOG_DEBUG("Configure Thermal Expert Q1");

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkInfraredTEQ1Cam::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkInfraredTEQ1Cam::FreezeDevice(bool freeze)
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
PlusStatus vtkInfraredTEQ1Cam::InternalConnect()
{
  SCANINFO* pScan = new SCANINFO[MAX_USB_NUM];
  ScanTE(nullptr, pScan);
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
    LOG_ERROR("Thermal Expert Q1: camera not detected.");
    return PLUS_FAIL;
  }

  this->pTE = OpenTE_B(nullptr, I3_TE_Q1, this->device);
  if (! this->pTE)
  {
    LOG_ERROR("Thermal Expert Q1: Failed to open.");
    return PLUS_FAIL;
  }

  if (this->pTE->ReadFlashData() == 1)
  {
    int width = this->pTE->GetImageWidth();
    int height = this->pTE->GetImageHeight();
    this->pImgBuf = new float[width * height];
    this->width = width;
    this->height = height;
  }
  else
  {
    LOG_ERROR("Thermal Expert Q1: Fail to Read Flash Data.");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkInfraredTEQ1Cam::InternalDisconnect()
{
  this->pTE->CloseTE();
  this->pImgBuf = nullptr;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkInfraredTEQ1Cam::InternalUpdate()
{
  if (!this->pTE)
  {
    LOG_ERROR("vtkInfraredTEQ1Cam::InternalUpdate Unable to read date");
    return PLUS_SUCCESS;
  }

  if (!this->pTE->RecvImage(this->pImgBuf))
  {
    LOG_ERROR("vtkInfraredTEQ1Cam::InternalUpdate Unable to receive frame");
    return PLUS_SUCCESS;
  }

  pTE->CalcEntireTemp(this->pImgBuf, false);

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
    aSource->SetPixelType(VTK_TYPE_FLOAT32);
    aSource->SetNumberOfScalarComponents(1);
    aSource->SetInputFrameSize(this->width, this->height, 1);
  }

  // Add the frame to the stream buffer
  FrameSizeType frameSize = { static_cast<unsigned int>(this->width), static_cast<unsigned int>(this->height), 1 };
  if (aSource->AddItem(this->pImgBuf, aSource->GetInputImageOrientation(), frameSize, VTK_TYPE_FLOAT32, 1, US_IMG_BRIGHTNESS, 0, this->FrameNumber) == PLUS_FAIL)
  {
    return PLUS_FAIL;
  }

  this->FrameNumber++;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkInfraredTEQ1Cam::NotifyConfigured()
{
  if (this->OutputChannels.size() > 1)
  {
    LOG_WARNING("vtkInfraredTEQ1Cam is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
  }

  if (this->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined for vtkInfraredTEQ1Cam. Cannot proceed.");
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}
