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

PlusStatus vtkPlusFLIRSpinnakerCam::AutoFocus()
{
  INodeMap& nodeMap = pCam->GetNodeMap();

  try {
    CCommandPtr ptrAutoFocus = nodeMap.GetNode("AutoFocus");
    if (!IsAvailable(ptrAutoFocus)) {
      LOG_DEBUG("AutoFocus not available.");
    }
    else if (!IsWritable(ptrAutoFocus)) {
      LOG_DEBUG("AutoFocus not writable.");
    }
    else {
      ptrAutoFocus->Execute();
      LOG_DEBUG("Autofocus On.");
    }
  }
  catch (Spinnaker::Exception& e) {
    LOG_DEBUG("Error: " << e.what());
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;

}


int ConfigureGVCPHeartbeat(CameraPtr pCam, bool enable)
{
  //
  // Write to boolean node controlling the camera's heartbeat
  //
  // *** NOTES ***
  // This applies only to GEV cameras.
  //
  // GEV cameras have a heartbeat built in, but when debugging applications the
  // camera may time out due to its heartbeat. Disabling the heartbeat prevents
  // this timeout from occurring, enabling us to continue with any necessary 
  // debugging.
  //
  // *** LATER ***
  // Make sure that the heartbeat is reset upon completion of the debugging.  
  // If the application is terminated unexpectedly, the camera may not locked
  // to Spinnaker indefinitely due to the the timeout being disabled.  When that 
  // happens, a camera power cycle will reset the heartbeat to its default setting.

  // Retrieve TL device nodemap
  INodeMap& nodeMapTLDevice = pCam->GetTLDeviceNodeMap();

  // Retrieve GenICam nodemap
  INodeMap& nodeMap = pCam->GetNodeMap();

  CEnumerationPtr ptrDeviceType = nodeMapTLDevice.GetNode("DeviceType");
  if (!IsReadable(ptrDeviceType))
  {
    LOG_ERROR("Unable to read DeviceType (node retrieval, !IsReadable).");
    return PLUS_FAIL;
  }

  if (ptrDeviceType->GetIntValue() != DeviceType_GigEVision)
  {
    LOG_DEBUG("Device is not a GigEVision. Skipping.");
    return PLUS_SUCCESS;
  }

  if (enable)
  {
    LOG_DEBUG("Resetting heartbeat.");
  }
  else
  {
    LOG_DEBUG("Disabling heartbeat.");
  }

CBooleanPtr ptrDeviceHeartbeat = nodeMap.GetNode("GevGVCPHeartbeatDisable");
if (!IsWritable(ptrDeviceHeartbeat))
{
  LOG_DEBUG("Unable to configure heartbeat. Continuing with execution as this may be non-fatal.");
  return PLUS_FAIL;
}

  ptrDeviceHeartbeat->SetValue(enable);

  if (!enable)
  {
    LOG_DEBUG("WARNING: Heartbeat has been disabled for the rest of this example run." << endl
    << "         Heartbeat will be reset upon the completion of this run.  If the " << endl
    << "         example is aborted unexpectedly before the heartbeat is reset, the" << endl
    << "         camera may need to be power cycled to reset the heartbeat.");
  }
  else
  {
    LOG_DEBUG("Heartbeat has been reset.");
  }

  return PLUS_SUCCESS;
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
    if (!IsReadable(ptrExposureAuto))
    {
      LOG_ERROR("Unable to disable automatic exposure (node retrieval, !IsReadable). Aborting...");
      return PLUS_FAIL;
    }

    if (!IsWritable(ptrExposureAuto))
    {
      LOG_ERROR("Unable to disable automatic exposure (node retrieval, !IsWritable). Aborting...");
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
    LOG_ERROR("Error Configuring Exposure (Spinnaker SDK): " << e.what());
    return PLUS_FAIL;
  }


  return PLUS_SUCCESS;
}

// This function returns the camera to its default state by re-enabling automatic
// exposure.
PlusStatus ResetExposure(INodeMap& nodeMap)
{
  CEnumerationPtr ptrExposureAuto = nullptr;
  try
  {
    //
    // Turn automatic exposure back on
    //
    // *** NOTES ***
    // Automatic exposure is turned on in order to return the camera to its
    // default state.
    //
    ptrExposureAuto = nodeMap.GetNode("ExposureAuto");

    if (!IsReadable(ptrExposureAuto) ||
      !IsWritable(ptrExposureAuto))
    {
      LOG_DEBUG("Unable to enable automatic exposure (node retrieval). Non-fatal error...");
      return PLUS_SUCCESS;
    }

    CEnumEntryPtr ptrExposureAutoContinuous = ptrExposureAuto->GetEntryByName("Continuous");
    if (!IsReadable(ptrExposureAutoContinuous))
    {
      LOG_DEBUG("Unable to enable automatic exposure (enum entry retrieval). Non-fatal error...");
      return PLUS_SUCCESS;
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

int PrintDeviceInfo(INodeMap& nodeMap)
{
  int result = PLUS_SUCCESS;

  LOG_DEBUG("*** DEVICE INFORMATION ***");

  try
  {
    FeatureList_t features;
    CCategoryPtr category = nodeMap.GetNode("DeviceInformation");
    if (IsReadable(category))
    {
      category->GetFeatures(features);

      FeatureList_t::const_iterator it;
      for (it = features.begin(); it != features.end(); ++it)
      {
        CNodePtr pfeatureNode = *it;
        CValuePtr pValue = (CValuePtr)pfeatureNode;
        LOG_DEBUG(pfeatureNode->GetName() << " : " << (IsReadable(pValue) ? pValue->ToString() : "Node not readable"));
       }
    }
    else
    {
      LOG_DEBUG("Device control information not readable.");
    }
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
  std::string VideoFormatString = deviceConfig->GetAttribute("VideoFormat");
  if (ExposureTimeString)
  {
    this->dwExposure = std::atoi(ExposureTimeString);

    LOG_DEBUG("FLIR Systems Spinnaker: ExposureTime = " << this->dwExposure);

  }
  else
  {
    this->dwExposure = 0;
  }

  /* To uppercase */
  std::transform(VideoFormatString.begin(), VideoFormatString.end(), VideoFormatString.begin(),
                        [](unsigned char c) { return std::toupper(c); } 
                      );

  if (0 == VideoFormatString.compare("MONO8")){
    this->iVideoFormat = Mono8;
  } else if (0 == VideoFormatString.compare("MONO16")){
    this->iVideoFormat = Mono16;
  } else
  {
    this->iVideoFormat = Mono16; 
  }

  LOG_DEBUG("FLIR Systems Spinnaker: Video mode = " << VideoFormatString);

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


PlusStatus vtkPlusFLIRSpinnakerCam::SetPixelFormat(INodeMap& nodeMap) {
  CEnumerationPtr ptrPixelFormat = nodeMap.GetNode("PixelFormat");
  CEnumerationPtr ptrImageCompressionMode = nodeMap.GetNode("ImageCompressionMode");
  CEnumEntryPtr ptrPixelVideoFormat;
 
  if (!IsReadable(ptrPixelFormat) ||
    !IsWritable(ptrPixelFormat))
  {
    LOG_ERROR("Unable to get or set pixel format. Aborting.");
    return PLUS_FAIL;
  }

  std::string videoFormatString;

  switch (this->iVideoFormat) {
    case Mono8:
                videoFormatString = "Mono8";
                break;
    case Mono16:
    default:
                videoFormatString = "Mono16";
                break;
  }
  ptrPixelVideoFormat = ptrPixelFormat->GetEntryByName(videoFormatString.c_str());
  if (!IsReadable(ptrPixelVideoFormat))
  {
    LOG_ERROR("Unable to get pixel format to " << videoFormatString << ". Aborting.");
    return PLUS_FAIL;
  }

  ptrPixelFormat->SetIntValue(ptrPixelVideoFormat->GetValue());
  LOG_DEBUG("Pixel format set to " << ptrPixelVideoFormat->GetSymbolic() << ".");

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
  // CameraList camList = psystem->GetCameras();
  camList = psystem->GetCameras();

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
  // Mostrar capacidades de la cámara
  PrintDeviceInfo(nodeMapTLDevice);

  pCam->Init();

  INodeMap& nodeMap = pCam->GetNodeMap();

  ConfigureGVCPHeartbeat(pCam, false);

  // Set pixel format
  if (SetPixelFormat(nodeMap) == PLUS_FAIL) {
    camList.Clear();
    psystem->ReleaseInstance();
    return PLUS_FAIL;
  }

  LOG_DEBUG("Number of FLIR cameras detected: " << numCameras);

/********************************/
  try {
    CCommandPtr ptrAutoFocus = nodeMap.GetNode("AutoFocus");
    if (!IsAvailable(ptrAutoFocus)) {
      LOG_DEBUG("AutoFocus not available.");
    } else if (!IsWritable(ptrAutoFocus)) {
      LOG_DEBUG("AutoFocus not writable.");
    } else {
        ptrAutoFocus->Execute();
        LOG_DEBUG("Autofocus On.");
    }
  }
  catch (Spinnaker::Exception& e) {
    LOG_DEBUG("Error: " << e.what());
  }

/********************************/
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

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusFLIRSpinnakerCam::InternalDisconnect()
{
  // Deinitialize camera
  try {
    pCam->EndAcquisition();
    pCam->DeInit();
    camList.Clear();
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
  int pixelType = VTK_UNSIGNED_CHAR;
  int numberOfScalarComponents = 1;
  US_IMAGE_TYPE imageType = US_IMG_BRIGHTNESS;
  ImagePtr convertedImage;

  this->m_currentTime = vtkIGSIOAccurateTimer::GetSystemTime();

  if (pResultImage->IsIncomplete())
  {
    // Retrieve and print the image status description
    LOG_DEBUG("Image incomplete: " << Image::GetImageStatusDescription(pResultImage->GetImageStatus()));
  }
  else {
    const size_t width = pResultImage->GetWidth();
    const size_t height = pResultImage->GetHeight();


    switch (this->iVideoFormat) {
    case Mono8:
      convertedImage = processor.Convert(pResultImage, PixelFormat_Mono8);
      pixelType = VTK_UNSIGNED_CHAR;
      numberOfScalarComponents = 1;
      imageType = US_IMG_BRIGHTNESS;
      break;
    case Mono16:
    default:
      convertedImage = processor.Convert(pResultImage, PixelFormat_Mono16);
      pixelType = VTK_UNSIGNED_SHORT;
      numberOfScalarComponents = 1;
      imageType = US_IMG_BRIGHTNESS;
      break;
      //aSource->SetImageType(US_IMG_RGB_COLOR);
      //aSource->SetNumberOfScalarComponents(3);
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
      aSource->SetImageType(imageType);
      aSource->SetPixelType(pixelType);
      aSource->SetNumberOfScalarComponents(numberOfScalarComponents);
      aSource->SetInputFrameSize(width, height, 1);
    }

    // Add the frame to the stream buffer
    FrameSizeType frameSize = { static_cast<unsigned int>(width), static_cast<unsigned int>(height), 1 };
    if (aSource->AddItem(convertedImage->GetData(), aSource->GetInputImageOrientation(), frameSize, pixelType, numberOfScalarComponents, imageType, 0, this->FrameNumber, this->m_currentTime, this->m_currentTime) == PLUS_FAIL)
    {
      return PLUS_FAIL;
    }
    this->FrameNumber++;
  }
  return PLUS_SUCCESS;

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
