/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.

Developed by ULL & IACTEC-IAC group
=========================================================Plus=header=end*/
// FLIR Spinnaker
#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusFLIRSpinnakerCam.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkObjectFactory.h>

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace std;

SystemPtr psystem;
CameraPtr pCam;
ImageProcessor processor;

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusFLIRSpinnakerCam);

//----------------------------------------------------------------------------
vtkPlusFLIRSpinnakerCam::vtkPlusFLIRSpinnakerCam()
{
  this->RequireImageOrientationInConfiguration = true;
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkPlusFLIRSpinnakerCam::~vtkPlusFLIRSpinnakerCam()
{
}

//----------------------------------------------------------------------------
void vtkPlusFLIRSpinnakerCam::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "FLIRSpinnakerCam: FLIR Systems Spinnaker Camera" << std::endl;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusFLIRSpinnakerCam::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{

  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  LOG_DEBUG("Configure FLIR Systems Spinnaker");
  /***
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

  LOG_DEBUG("FLIR Systems Spinnaker: ExposureTime = " << this->dwExposure << "    TimeBaseExposure = " << this->wTimeBaseExposure);
**/
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusFLIRSpinnakerCam::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
	/**
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
	**/
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusFLIRSpinnakerCam::FreezeDevice(bool freeze)
{
/**
  if (freeze)
  {
    this->Disconnect();
  }
  else
  {
    this->Connect();
  }
**/
  return PLUS_SUCCESS;
}


PlusStatus SetPixelFormat(INodeMap& nodeMap) {
  CEnumerationPtr ptrPixelFormat = nodeMap.GetNode("PixelFormat");
  if (!IsReadable(ptrPixelFormat) ||
    !IsWritable(ptrPixelFormat))
  {
    LOG_ERROR("Unable to get or set pixel format. Aborting.");
    return PLUS_FAIL;
  }
  CEnumEntryPtr ptrPixelFormatMono8 = ptrPixelFormat->GetEntryByName("Mono8");
  if (!IsReadable(ptrPixelFormatMono8))
  {
    LOG_ERROR("Unable to get pixel format to Mono8. Aborting.");
    return PLUS_FAIL;
  }

  ptrPixelFormat->SetIntValue(ptrPixelFormatMono8->GetValue());
  LOG_DEBUG("Pixel format set to " << ptrPixelFormatMono8->GetSymbolic() << ".");

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkPlusFLIRSpinnakerCam::InternalConnect()
{
  // Retrieve singleton reference to system object
  psystem = System::GetInstance();
  const LibraryVersion spinnakerLibraryVersion = psystem->GetLibraryVersion();
  LOG_DEBUG("Spinnaker library version: " << spinnakerLibraryVersion.major << "." << spinnakerLibraryVersion.minor
    << "." << spinnakerLibraryVersion.type << "." << spinnakerLibraryVersion.build);
  CameraList camList = psystem->GetCameras();
  const unsigned int numCameras = camList.GetSize();
  
  if (numCameras == 0)
  {
    // Clear camera list before releasing system
    camList.Clear();
    // Release system
    psystem->ReleaseInstance();
    LOG_ERROR("No FLIR cameras detected!");
    return PLUS_FAIL;
  }
 
  // For this version, takes the first cam in the list.
  pCam = camList.GetByIndex(0);
  INodeMap& nodeMapTLDevice = pCam->GetTLDeviceNodeMap();
  pCam->Init();
  INodeMap& nodeMap = pCam->GetNodeMap();

  // Set pixel format
  if (SetPixelFormat(nodeMap) == PLUS_FAIL) {
    camList.Clear();
    psystem->ReleaseInstance();
    return PLUS_FAIL;
  }

  LOG_DEBUG("Number of FLIR cameras detected: " << numCameras);

  try{
    CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
    if (!IsReadable(ptrAcquisitionMode) ||
      !IsWritable(ptrAcquisitionMode))
    {
      LOG_ERROR("Unable to set acquisition mode to continuous (enum retrieval). Aborting.");
      camList.Clear();
      psystem->ReleaseInstance();
      return PLUS_FAIL;
    }

    // Retrieve entry node from enumeration node
    CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");
    if (!IsReadable(ptrAcquisitionModeContinuous))
    {
      LOG_ERROR("Unable to set acquisition mode to continuous (enum retrieval 2). Aborting.");
      camList.Clear();
      psystem->ReleaseInstance();
      return PLUS_FAIL;
    }
    // Retrieve integer value from entry node
    const int64_t acquisitionModeContinuous = ptrAcquisitionModeContinuous->GetValue();

    // Set integer value from entry node as new value of enumeration node
    ptrAcquisitionMode->SetIntValue(acquisitionModeContinuous);

    LOG_DEBUG("Acquisition mode set to continuous...");

    // Acquisition start
    pCam->BeginAcquisition();
    LOG_DEBUG("Acquisition starting...");

    processor.SetColorProcessing(SPINNAKER_COLOR_PROCESSING_ALGORITHM_HQ_LINEAR);

  }
  catch (Spinnaker::Exception& e)
  {
    LOG_ERROR("Error starting acquisition: " << e.what());
    camList.Clear();
    psystem->ReleaseInstance();
    return PLUS_FAIL;
  }
	/***
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

  pCam = nullptr;

  iRet = PCO_OpenCamera(&(pCam), 0);
  if (iRet != PCO_NOERROR)
  {
    LOG_ERROR("PCO Ultraviolet: camera not detected.");
    return PLUS_FAIL;
  }
  else {
    LOG_DEBUG("PCO Ultraviolet: camera found.");
  }
  strDescription.wSize = sizeof(PCO_Description);
  iRet = PCO_GetCameraDescription(pCam, &strDescription);

  iRet = PCO_GetRecordingState(pCam, &RecordingState);
  if (RecordingState)
  {
    iRet = PCO_SetRecordingState(pCam, 0);
  }

  iRet = PCO_GetDelayExposureTime(pCam, &(auxDelay), &(auxExposure), &(auxTimeBaseDelay), &(auxTimeBaseExposure));

  this->dwDelay = auxDelay;
  if (this->dwExposure == -1) {
    this->dwExposure = auxExposure;
  }
  this->wTimeBaseDelay = auxTimeBaseDelay;
  if (this->wTimeBaseExposure == -1) {
    this->wTimeBaseExposure = auxTimeBaseExposure;
  }

  if ((iRet = PCO_SetDelayExposureTime(pCam, this->dwDelay, this->dwExposure, this->wTimeBaseDelay, this->wTimeBaseExposure)) != 0) {
    LOG_ERROR("PCO Ultraviolet: SetExposureTime (PCO_SetDelayExposureTime) failed with errorcode " << iRet);
    return PLUS_FAIL;
  }

  // Arm Camera before next Set...  
  iRet = PCO_ArmCamera(pCam);

  iRet = PCO_GetCameraHealthStatus(pCam, &CameraWarning, &CameraError, &CameraStatus);
  if (CameraError != 0)
  {
    LOG_ERROR("PCO Ultraviolet: Camera has error status " << CameraError);
    iRet = PCO_CloseCamera(pCam);
    return PLUS_FAIL;
  }

  strCamType.wSize = sizeof(PCO_CameraType);
  iRet = PCO_GetCameraType(pCam, &strCamType);
  if (iRet != PCO_NOERROR)
  {
    LOG_ERROR("PCO Ultraviolet: PCO_GetCameraType failed with errorcode " << iRet);
    iRet = PCO_CloseCamera(pCam);
    return PLUS_FAIL;
  }

  if (strCamType.wInterfaceType == INTERFACE_CAMERALINK)
  {
    PCO_SC2_CL_TRANSFER_PARAM cl_par;
    iRet = PCO_GetTransferParameter(pCam, (void*)&cl_par, sizeof(PCO_SC2_CL_TRANSFER_PARAM));
    LOG_DEBUG("PCO Ultraviolet Camlink Settings: Baudrate=" << cl_par.baudrate << ", Clockfreq=" << cl_par.ClockFrequency << ", Dataformat=" << cl_par.DataFormat << ", Transmit=" << cl_par.Transmit);
  }

  iRet = PCO_GetSizes(pCam, &(this->XResAct), &(this->YResAct), &(this->XResMax), &(this->YResMax));
  bufsize = (this->XResAct) * (this->YResAct) * sizeof(WORD);

  this->pImgBuf = nullptr;
  this->BufNum = -1;

  iRet = PCO_AllocateBuffer(pCam, &this->BufNum, this->bufsize, &(this->pImgBuf), &(this->BufEvent));
  iRet = PCO_SetImageParameters(pCam, this->XResAct, this->YResAct, IMAGEPARAMETERS_READ_FROM_SEGMENTS, NULL, 0);
  **/
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusFLIRSpinnakerCam::InternalDisconnect()
{
  // Deinitialize camera
  try {
    pCam->DeInit();
    psystem->ReleaseInstance();
  }
  catch (Spinnaker::Exception& e)
  {
    LOG_ERROR("Error: " << e.what());
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusFLIRSpinnakerCam::InternalUpdate()
{
  ImagePtr pResultImage = pCam->GetNextImage(1000);

  if (pResultImage->IsIncomplete())
  {
    // Retrieve and print the image status description
    LOG_DEBUG("Image incomplete: " << Image::GetImageStatusDescription(pResultImage->GetImageStatus()));
  }
  else {
    const size_t width = pResultImage->GetWidth();
    const size_t height = pResultImage->GetHeight();
    ImagePtr convertedImage = processor.Convert(pResultImage, PixelFormat_Mono8);

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
      aSource->SetPixelType(VTK_UNSIGNED_CHAR);
      aSource->SetNumberOfScalarComponents(1);
      aSource->SetInputFrameSize(width, height, 1);
    }

    // Add the frame to the stream buffer
    FrameSizeType frameSize = { static_cast<unsigned int>(width), static_cast<unsigned int>(height), 1 };
    if (aSource->AddItem(convertedImage->GetData(), aSource->GetInputImageOrientation(), frameSize, VTK_UNSIGNED_CHAR, 1, US_IMG_BRIGHTNESS, 0, this->FrameNumber) == PLUS_FAIL)
    {
      return PLUS_FAIL;
    }
    this->FrameNumber++;
  }
  return PLUS_SUCCESS;

/**
  int iRet;

  if (pCam == nullptr)
  {
    LOG_ERROR("vtkPlusFLIRSpinnakerCam::InternalUpdate Unable to read date");
    return PLUS_SUCCESS;
  }

  iRet = PCO_SetRecordingState(pCam, 1);

  iRet = PCO_GetImageEx(pCam, 1, 0, 0, this->BufNum, this->XResAct, this->YResAct, 16);
  if (iRet != PCO_NOERROR)
  {
    LOG_ERROR("vtkPlusFLIRSpinnakerCam::InternalUpdate Unable to receive frame");
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
**/
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusFLIRSpinnakerCam::NotifyConfigured()
{
  if (this->OutputChannels.size() > 1)
  {
    LOG_WARNING("vtkPlusFLIRSpinnakerCam is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
  }

  if (this->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined for vtkPlusFLIRSpinnakerCam. Cannot proceed.");
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}
