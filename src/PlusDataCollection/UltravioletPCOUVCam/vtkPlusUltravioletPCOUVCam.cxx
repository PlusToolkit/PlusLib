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
#include "vtkPlusUltravioletPCOUVCam.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkObjectFactory.h>

// PCOUV
#include "pco_err.h"
#include "sc2_SDKStructures.h"
#include "SC2_SDKAddendum.h"
#include "SC2_CamExport.h"
#include "SC2_Defs.h"

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusUltravioletPCOUVCam);

//----------------------------------------------------------------------------
vtkPlusUltravioletPCOUVCam::vtkPlusUltravioletPCOUVCam()
{
  this->RequireImageOrientationInConfiguration = true;
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkPlusUltravioletPCOUVCam::~vtkPlusUltravioletPCOUVCam()
{
}

//----------------------------------------------------------------------------
void vtkPlusUltravioletPCOUVCam::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "UltravioletPCOUVCam: PCO Ultraviolet Camera" << std::endl;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusUltravioletPCOUVCam::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  LOG_DEBUG("Configure PCO Ultaviolet");

  const char* ExposureTimeString = deviceConfig->GetAttribute("ExposureTime");
  const char* TimeBaseExposureString = deviceConfig->GetAttribute("TimeBaseExposure");
  if (ExposureTimeString)
  {
    this->dwExposure = std::atoi(ExposureTimeString);
  }
  else
  {
    this->dwExposure = -1;
  }

  if (TimeBaseExposureString)
  {
    this->wTimeBaseExposure = std::atoi(TimeBaseExposureString);
  }
  else
  {
    this->wTimeBaseExposure = -1;
  }

  LOG_DEBUG("PCO Ultaviolet: ExposureTime = " << this->dwExposure << "    TimeBaseExposure = " << this->wTimeBaseExposure);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusUltravioletPCOUVCam::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUltravioletPCOUVCam::FreezeDevice(bool freeze)
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
PlusStatus vtkPlusUltravioletPCOUVCam::InternalConnect()
{
  PCO_Description strDescription;
  PCO_CameraType strCamType;
  int iRet;
  DWORD CameraWarning, CameraError, CameraStatus;
  WORD XResAct, YResAct, XResMax, YResMax;
  WORD RecordingState;
  DWORD auxDelay;
  DWORD auxExposure;
  WORD auxTimeBaseDelay;
  WORD auxTimeBaseExposure;

  this->cam = nullptr;

  iRet = PCO_OpenCamera(&(this->cam), 0);
  if (iRet != PCO_NOERROR)
  {
    LOG_ERROR("PCO Ultraviolet: camera not detected.");
    return PLUS_FAIL;
  }
  else {
    LOG_DEBUG("PCO Ultraviolet: camera found.");
  }
  strDescription.wSize = sizeof(PCO_Description);
  iRet = PCO_GetCameraDescription(this->cam, &strDescription);

  iRet = PCO_GetRecordingState(this->cam, &RecordingState);
  if (RecordingState)
  {
    iRet = PCO_SetRecordingState(this->cam, 0);
  }

  iRet = PCO_GetDelayExposureTime(this->cam, &(auxDelay), &(auxExposure), &(auxTimeBaseDelay), &(auxTimeBaseExposure));

  this->dwDelay = auxDelay;
  if (this->dwExposure == -1) {
    this->dwExposure = auxExposure;
  }
  this->wTimeBaseDelay = auxTimeBaseDelay;
  if (this->wTimeBaseExposure == -1) {
    this->wTimeBaseExposure = auxTimeBaseExposure;
  }

  if ((iRet = PCO_SetDelayExposureTime(this->cam, this->dwDelay, this->dwExposure, this->wTimeBaseDelay, this->wTimeBaseExposure)) != 0) {
    LOG_ERROR("PCO Ultraviolet: SetExposureTime (PCO_SetDelayExposureTime) failed with errorcode " << iRet);
    return PLUS_FAIL;
  }

  // Arm Camera before next Set...  
  iRet = PCO_ArmCamera(this->cam);

  iRet = PCO_GetCameraHealthStatus(this->cam, &CameraWarning, &CameraError, &CameraStatus);
  if (CameraError != 0)
  {
    LOG_ERROR("PCO Ultraviolet: Camera has error status " << CameraError);
    iRet = PCO_CloseCamera(this->cam);
    return PLUS_FAIL;
  }

  strCamType.wSize = sizeof(PCO_CameraType);
  iRet = PCO_GetCameraType(this->cam, &strCamType);
  if (iRet != PCO_NOERROR)
  {
    LOG_ERROR("PCO Ultraviolet: PCO_GetCameraType failed with errorcode " << iRet);
    iRet = PCO_CloseCamera(this->cam);
    return PLUS_FAIL;
  }

  if (strCamType.wInterfaceType == INTERFACE_CAMERALINK)
  {
    PCO_SC2_CL_TRANSFER_PARAM cl_par;
    iRet = PCO_GetTransferParameter(this->cam, (void*)&cl_par, sizeof(PCO_SC2_CL_TRANSFER_PARAM));
    LOG_DEBUG("PCO Ultraviolet Camlink Settings: Baudrate=" << cl_par.baudrate << ", Clockfreq=" << cl_par.ClockFrequency << ", Dataformat=" << cl_par.DataFormat << ", Transmit=" << cl_par.Transmit);
  }

  iRet = PCO_GetSizes(this->cam, &(this->XResAct), &(this->YResAct), &(this->XResMax), &(this->YResMax));
  bufsize = (this->XResAct) * (this->YResAct) * sizeof(WORD);

  this->pImgBuf = nullptr;
  this->BufNum = -1;

  iRet = PCO_AllocateBuffer(this->cam, &this->BufNum, this->bufsize, &(this->pImgBuf), &(this->BufEvent));
  iRet = PCO_SetImageParameters(this->cam, this->XResAct, this->YResAct, IMAGEPARAMETERS_READ_FROM_SEGMENTS, NULL, 0);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUltravioletPCOUVCam::InternalDisconnect()
{
  int iRet;
  iRet = PCO_CloseCamera(this->cam);
  this->pImgBuf = nullptr;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUltravioletPCOUVCam::InternalUpdate()
{
  int iRet;

  if (this->cam == nullptr)
  {
    LOG_ERROR("vtkPlusUltravioletPCOUVCam::InternalUpdate Unable to read date");
    return PLUS_SUCCESS;
  }

  iRet = PCO_SetRecordingState(this->cam, 1);

  iRet = PCO_GetImageEx(this->cam, 1, 0, 0, this->BufNum, this->XResAct, this->YResAct, 16);
  if (iRet != PCO_NOERROR)
  {
    LOG_ERROR("vtkPlusUltravioletPCOUVCam::InternalUpdate Unable to receive frame");
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
    aSource->SetInputFrameSize(this->XResAct, this->YResAct, 1);
  }

  // Add the frame to the stream buffer
  FrameSizeType frameSize = { static_cast<unsigned int>(this->XResAct), static_cast<unsigned int>(this->YResAct), 1 };
  if (aSource->AddItem(this->pImgBuf, aSource->GetInputImageOrientation(), frameSize, VTK_UNSIGNED_SHORT, 1, US_IMG_BRIGHTNESS, 0, this->FrameNumber) == PLUS_FAIL)
  {
    return PLUS_FAIL;
  }

  this->FrameNumber++;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUltravioletPCOUVCam::NotifyConfigured()
{
  if (this->OutputChannels.size() > 1)
  {
    LOG_WARNING("vtkPlusUltravioletPCOUVCam is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
  }

  if (this->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined for vtkPlusUltravioletPCOUVCam. Cannot proceed.");
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}
