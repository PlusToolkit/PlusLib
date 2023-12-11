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

PlusStatus ConfigureExposure(INodeMap& nodeMap,DWORD exposureTimeToSet)
{
  LOG_DEBUG("*** CONFIGURING EXPOSURE ***");

  try
  {
    //
    // Turn off automatic exposure mode
    //
    // *** NOTES ***
    // Automatic exposure prevents the manual configuration of exposure
    // time and needs to be turned off.
    //
    // *** LATER ***
    // Exposure time can be set automatically or manually as needed. This
    // example turns automatic exposure off to set it manually and back
    // on in order to return the camera to its default state.
    //
    CEnumerationPtr ptrExposureAuto = nodeMap.GetNode("ExposureAuto");
    if (!IsReadable(ptrExposureAuto) ||
      !IsWritable(ptrExposureAuto))
    {
      LOG_ERROR("Unable to disable automatic exposure (node retrieval). Aborting...");
      return PLUS_FAIL;
    }

    CEnumEntryPtr ptrExposureAutoOff = ptrExposureAuto->GetEntryByName("Off");
    if (!IsReadable(ptrExposureAutoOff))
    {
      LOG_ERROR("Unable to disable automatic exposure (enum entry retrieval). Aborting...");
      return PLUS_FAIL;
    }

    ptrExposureAuto->SetIntValue(ptrExposureAutoOff->GetValue());

    LOG_DEBUG("Automatic exposure disabled...");

    //
    // Set exposure time manually; exposure time recorded in microseconds
    //
    // *** NOTES ***
    // The node is checked for availability and writability prior to the
    // setting of the node. Further, it is ensured that the desired exposure
    // time does not exceed the maximum. Exposure time is counted in
    // microseconds. This information can be found out either by
    // retrieving the unit with the GetUnit() method or by checking SpinView.
    //
    CFloatPtr ptrExposureTime = nodeMap.GetNode("ExposureTime");
    if (!IsReadable(ptrExposureTime) ||
      !IsWritable(ptrExposureTime))
    {
      LOG_ERROR("Unable to get or set exposure time. Aborting...");
      return PLUS_FAIL;
    }

    // Ensure desired exposure time does not exceed the maximum
    const double exposureTimeMax = ptrExposureTime->GetMax();

    if (exposureTimeToSet > exposureTimeMax)
    {
      exposureTimeToSet = exposureTimeMax;
    }

    ptrExposureTime->SetValue(exposureTimeToSet);

    LOG_DEBUG("Exposure time set to " << exposureTimeToSet << " us...");
  }
  catch (Spinnaker::Exception& e)
  {
    LOG_ERROR("Error: " << e.what());
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

// This function returns the camera to its default state by re-enabling automatic
// exposure.
PlusStatus ResetExposure(INodeMap& nodeMap)
{
  try
  {
    //
    // Turn automatic exposure back on
    //
    // *** NOTES ***
    // Automatic exposure is turned on in order to return the camera to its
    // default state.
    //
    CEnumerationPtr ptrExposureAuto = nodeMap.GetNode("ExposureAuto");
    if (!IsReadable(ptrExposureAuto) ||
      !IsWritable(ptrExposureAuto))
    {
      LOG_DEBUG("Unable to enable automatic exposure (node retrieval). Non-fatal error...");
    }

    CEnumEntryPtr ptrExposureAutoContinuous = ptrExposureAuto->GetEntryByName("Continuous");
    if (!IsReadable(ptrExposureAutoContinuous))
    {
      LOG_DEBUG("Unable to enable automatic exposure (enum entry retrieval). Non-fatal error...");
    }

    ptrExposureAuto->SetIntValue(ptrExposureAutoContinuous->GetValue());

    LOG_DEBUG("Automatic exposure enabled...");
  }
  catch (Spinnaker::Exception& e)
  {
    LOG_ERROR("Error: " << e.what());
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}



//-----------------------------------------------------------------------------
PlusStatus vtkPlusFLIRSpinnakerCam::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{

  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  LOG_DEBUG("Configure FLIR Systems Spinnaker");
  const char* ExposureTimeString = deviceConfig->GetAttribute("ExposureTime");
  if (ExposureTimeString)
  {
    this->dwExposure = std::atoi(ExposureTimeString);
  }
  else
  {
    this->dwExposure = 0;
  }

  LOG_DEBUG("FLIR Systems Spinnaker: ExposureTime = " << this->dwExposure);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusFLIRSpinnakerCam::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusFLIRSpinnakerCam::FreezeDevice(bool freeze)
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

    if (this->dwExposure > 0) {
      ConfigureExposure(nodeMap, this->dwExposure);
    }
    else {
      ResetExposure(nodeMap);
    }

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
