/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusOpenCVCaptureVideoSource.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkObjectFactory.h>

// OpenCV includes
#if CV_MAJOR_VERSION > 3
  #include <opencv2/calib3d.hpp>
#endif
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusOpenCVCaptureVideoSource);

//----------------------------------------------------------------------------
vtkPlusOpenCVCaptureVideoSource::vtkPlusOpenCVCaptureVideoSource()
  : VideoURL("")
  , FourCC("")
  , RequestedCaptureAPI(cv::CAP_ANY)
  , DeviceIndex(-1)
  , Capture(nullptr)
  , Frame(nullptr)
  , UndistortedFrame(nullptr)
  , CameraMatrix(nullptr)
  , DistortionCoefficients(nullptr)
  , AutofocusEnabled(false)
  , AutoexposureEnabled(false)
{
  this->FrameSize = { 0, 0, 0 };
  this->RequireImageOrientationInConfiguration = true;
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkPlusOpenCVCaptureVideoSource::~vtkPlusOpenCVCaptureVideoSource()
{
}

//----------------------------------------------------------------------------
void vtkPlusOpenCVCaptureVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "VideoURL: " << this->VideoURL << std::endl;
  os << indent << "DeviceIndex: " << this->DeviceIndex << std::endl;
  os << indent << "RequestedCaptureAPI: " << vtkPlusOpenCVCaptureVideoSource::StringFromCaptureAPI(this->RequestedCaptureAPI) << std::endl;

  if (this->CameraMatrix != nullptr)
  {
    os << indent << "CamerMatrix: " << *this->CameraMatrix << std::endl;
  }
  if (this->DistortionCoefficients != nullptr)
  {
    os << indent << "DistortionCoefficients: " << *this->DistortionCoefficients << std::endl;
  }
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusOpenCVCaptureVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusOpenCVCaptureVideoSource::ReadConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_STRING_ATTRIBUTE_OPTIONAL(VideoURL, deviceConfig);
  std::string captureApi;
  XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(CaptureAPI, captureApi, deviceConfig);
  if (!captureApi.empty())
  {
    this->RequestedCaptureAPI = CaptureAPIFromString(captureApi);
  }

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, DeviceIndex, deviceConfig);
  int frameSize[3] = { 0, 0, 1 };
  XML_READ_VECTOR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, 2, FrameSize, frameSize, deviceConfig);
  if (deviceConfig->GetAttribute("FrameSize") != NULL)
  {
    std::copy(std::begin(frameSize), std::end(frameSize), this->FrameSize.begin());
  }

  XML_READ_STRING_ATTRIBUTE_OPTIONAL(FourCC, deviceConfig);
  if (!this->FourCC.empty() && this->FourCC.length() != 4)
  {
    LOG_WARNING("Unable to parse desired FourCC string.");
    this->FourCC = "";
  }

  std::vector<double> camMat;
  camMat.resize(9);
  XML_READ_STD_ARRAY_ATTRIBUTE_NONMEMBER_EXACT_OPTIONAL(double, CameraMatrix, 9, camMat, deviceConfig);
  if (deviceConfig->GetAttribute("CameraMatrix") != NULL)
  {
    this->CameraMatrix = std::make_shared<cv::Mat>(3, 3, CV_64F);
    memcpy(this->CameraMatrix->data, camMat.data(), sizeof(double) * 9);
  }

  std::vector<double> distCoeffs(8, std::numeric_limits<double>::infinity());
  XML_READ_STD_ARRAY_ATTRIBUTE_NONMEMBER_OPTIONAL(double, DistortionCoefficients, 8, distCoeffs, deviceConfig);
  if (deviceConfig->GetAttribute("DistortionCoefficients") != NULL)
  {
    std::vector<double>::difference_type count = std::count_if(distCoeffs.begin(), distCoeffs.end(), [this](const double & val) {return val != std::numeric_limits<double>::infinity(); });
    this->DistortionCoefficients = std::make_shared<cv::Mat>(count, 1, CV_64F);
    // Assumes all values are front filled
    for (std::vector<double>::difference_type i = 0; i < count; ++i)
    {
      this->DistortionCoefficients->at<double>(i, 0) = distCoeffs[i];
    }
  }

  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(AutofocusEnabled, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(AutoexposureEnabled, deviceConfig);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusOpenCVCaptureVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusOpenCVCaptureVideoSource::WriteConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  XML_WRITE_STRING_ATTRIBUTE_REMOVE_IF_EMPTY(VideoURL, deviceConfig);
  if (this->RequestedCaptureAPI != cv::CAP_ANY)
  {
    deviceConfig->SetAttribute("CaptureAPI", StringFromCaptureAPI(this->RequestedCaptureAPI).c_str());
  }
  if (this->DeviceIndex >= 0)
  {
    deviceConfig->SetIntAttribute("DeviceIndex", this->DeviceIndex);
  }
  if (this->CameraMatrix != nullptr)
  {
    deviceConfig->SetVectorAttribute("CameraMatrix", 9, this->CameraMatrix->ptr<double>(0));
  }
  if (this->DistortionCoefficients != nullptr)
  {
    deviceConfig->SetVectorAttribute("DistortionCoefficients", this->DistortionCoefficients->rows, this->DistortionCoefficients->ptr<double>(0));
  }

  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(FourCC, deviceConfig);

  XML_WRITE_BOOL_ATTRIBUTE(AutofocusEnabled, deviceConfig);
  XML_WRITE_BOOL_ATTRIBUTE(AutoexposureEnabled, deviceConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenCVCaptureVideoSource::FreezeDevice(bool freeze)
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
PlusStatus vtkPlusOpenCVCaptureVideoSource::InternalConnect()
{
  if (!this->VideoURL.empty())
  {
    this->Capture = std::make_shared<cv::VideoCapture>(this->VideoURL, this->RequestedCaptureAPI);
  }
  else if (this->DeviceIndex >= 0)
  {
    this->Capture = std::make_shared<cv::VideoCapture>(this->DeviceIndex + this->RequestedCaptureAPI);
  }
  else
  {
    LOG_ERROR("No device identification method defined. Please add either \"VideoURL\" or \"DeviceIndex\" attribute to configuration.");
    return PLUS_FAIL;
  }

  if (!this->Capture->isOpened())
  {
    LOG_ERROR("Unable to open OpenCV video device.");
    return PLUS_FAIL;
  }

  if (!this->Capture->set(cv::CAP_PROP_FPS, this->AcquisitionRate))
  {
    LOG_WARNING("Unable to set requested acquisition rate: " << this->AcquisitionRate);
  }
  if (!this->FourCC.empty() && !this->Capture->set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc(this->FourCC[0], this->FourCC[1], this->FourCC[2], this->FourCC[3])))
  {
    LOG_WARNING("Unable to set requested fourCC code: " << this->FourCC);
  }
  if (this->FrameSize[1] != 0)
  {
    if (!this->Capture->set(cv::CAP_PROP_FRAME_HEIGHT, this->FrameSize[1]))
    {
      LOG_ERROR("Unable to set the requested height of the capture device.");
    }
    if (!this->Capture->set(cv::CAP_PROP_FRAME_WIDTH, this->FrameSize[0]))
    {
      LOG_ERROR("Unable to set the requested width of the capture device.");
    }
  }
  if (!this->Capture->set(cv::CAP_PROP_AUTOFOCUS, this->AutofocusEnabled ? 1 : 0))
  {
    if (this->AutofocusEnabled)
    {
      LOG_WARNING("Could not set the autofocus property.");
    }
  }
  if (!this->Capture->set(cv::CAP_PROP_AUTO_EXPOSURE, this->AutoexposureEnabled ? 1 : 0))
  {
    if (this->AutoexposureEnabled)
    {
      LOG_WARNING("Could not set the autoexposure property.");
    }
  }

  this->FrameSize[0] = cvRound(this->Capture->get(cv::CAP_PROP_FRAME_WIDTH));
  this->FrameSize[1] = cvRound(this->Capture->get(cv::CAP_PROP_FRAME_HEIGHT));
  this->AcquisitionRate = cvRound(this->Capture->get(cv::CAP_PROP_FPS));

  this->Frame = std::make_shared<cv::Mat>(this->FrameSize[1], this->FrameSize[0], CV_8UC3);

  if (this->CameraMatrix != nullptr && this->DistortionCoefficients != nullptr)
  {
    this->UndistortedFrame = std::make_shared<cv::Mat>(this->FrameSize[1], this->FrameSize[0], CV_8UC3);
  }
  else
  {
    this->UndistortedFrame = this->Frame;
  }

  if (!this->Capture->isOpened())
  {
    if (!this->VideoURL.empty())
    {
      LOG_ERROR("Unable to open device at URL: " << this->VideoURL);
    }
    else
    {
      LOG_ERROR("Unable to open device at device index: " << this->DeviceIndex);
    }
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenCVCaptureVideoSource::InternalDisconnect()
{
  this->Capture = nullptr; // automatically closes resources/connections
  this->Frame = nullptr;
  this->UndistortedFrame = nullptr;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenCVCaptureVideoSource::InternalUpdate()
{
  LOG_TRACE("vtkPlusOpenCVCaptureVideoSource::InternalUpdate");

  if (!this->Capture->isOpened())
  {
    // No need to update if we're not able to read data
    return PLUS_SUCCESS;
  }

  // Capture one frame from the OpenCV capture device
  if (!this->Capture->read(*this->Frame))
  {
    LOG_ERROR("Unable to receive frame");
    return PLUS_FAIL;
  }

  if (this->CameraMatrix != nullptr && this->DistortionCoefficients != nullptr)
  {
    cv::undistort(*this->Frame, *this->UndistortedFrame, *this->CameraMatrix, *this->DistortionCoefficients);
  }

  // BGR -> RGB color
  cv::cvtColor(*this->UndistortedFrame, *this->UndistortedFrame, cv::COLOR_BGR2RGB);

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
    aSource->SetInputFrameSize(this->UndistortedFrame->cols, this->UndistortedFrame->rows, 1);
  }

  // Add the frame to the stream buffer
  FrameSizeType frameSize = { static_cast<unsigned int>(this->UndistortedFrame->cols), static_cast<unsigned int>(this->UndistortedFrame->rows), 1 };
  if (aSource->AddItem(this->UndistortedFrame->data, aSource->GetInputImageOrientation(), frameSize, VTK_UNSIGNED_CHAR, 3, US_IMG_RGB_COLOR, 0, this->FrameNumber) == PLUS_FAIL)
  {
    return PLUS_FAIL;
  }

  this->FrameNumber++;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenCVCaptureVideoSource::NotifyConfigured()
{
  if (this->OutputChannels.size() > 1)
  {
    LOG_WARNING("vtkPlusOpenCVCaptureVideoSource is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
  }

  if (this->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined for vtkPlusOpenCVCaptureVideoSource. Cannot proceed.");
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }

  if ((this->CameraMatrix != nullptr) != (this->DistortionCoefficients != nullptr)) // XOR
  {
    LOG_WARNING("Only one of CameraMatrix or DistortionCoefficients defined in config file, cannot perform undistortion.");
  }

  if (this->VideoURL.empty() && this->DeviceIndex < 0)
  {
    LOG_ERROR("No device identification method defined. Please add either \"VideoURL\" or \"DeviceIndex\" attribute to configuration.");
    return PLUS_FAIL;
  }

  if (this->DeviceIndex >= 0 && this->RequestedCaptureAPI == cv::CAP_FFMPEG)
  {
    LOG_ERROR("Cannot index FFMPEG devices by DeviceIndex. VideoURL must be used instead.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
cv::VideoCaptureAPIs vtkPlusOpenCVCaptureVideoSource::CaptureAPIFromString(const std::string& apiString)
{
  if (apiString.compare("CAP_ANY") == 0)
  {
    return cv::CAP_ANY;
  }
  else if (apiString.compare("CAP_VFW") == 0)
  {
    return cv::CAP_VFW;
  }
  else if (apiString.compare("CAP_V4L") == 0)
  {
    return cv::CAP_V4L;
  }
  else if (apiString.compare("CAP_V4L2") == 0)
  {
    return cv::CAP_V4L2;
  }
  else if (apiString.compare("CAP_FIREWIRE") == 0)
  {
    return cv::CAP_FIREWIRE;
  }
  else if (apiString.compare("CAP_FIREWARE") == 0)
  {
    return cv::CAP_FIREWARE;
  }
  else if (apiString.compare("CAP_IEEE1394") == 0)
  {
    return cv::CAP_IEEE1394;
  }
  else if (apiString.compare("CAP_DC1394") == 0)
  {
    return cv::CAP_DC1394;
  }
  else if (apiString.compare("CAP_CMU1394") == 0)
  {
    return cv::CAP_CMU1394;
  }
  else if (apiString.compare("CAP_QT") == 0)
  {
    return cv::CAP_QT;
  }
  else if (apiString.compare("CAP_UNICAP") == 0)
  {
    return cv::CAP_UNICAP;
  }
  else if (apiString.compare("CAP_DSHOW") == 0)
  {
    return cv::CAP_DSHOW;
  }
  else if (apiString.compare("CAP_PVAPI") == 0)
  {
    return cv::CAP_PVAPI;
  }
  else if (apiString.compare("CAP_OPENNI") == 0)
  {
    return cv::CAP_OPENNI;
  }
  else if (apiString.compare("CAP_OPENNI_ASUS") == 0)
  {
    return cv::CAP_OPENNI_ASUS;
  }
  else if (apiString.compare("CAP_ANDROID") == 0)
  {
    return cv::CAP_ANDROID;
  }
  else if (apiString.compare("CAP_XIAPI") == 0)
  {
    return cv::CAP_XIAPI;
  }
  else if (apiString.compare("CAP_AVFOUNDATION") == 0)
  {
    return cv::CAP_AVFOUNDATION;
  }
  else if (apiString.compare("CAP_GIGANETIX") == 0)
  {
    return cv::CAP_GIGANETIX;
  }
  else if (apiString.compare("CAP_MSMF") == 0)
  {
    return cv::CAP_MSMF;
  }
  else if (apiString.compare("CAP_WINRT") == 0)
  {
    return cv::CAP_WINRT;
  }
  else if (apiString.compare("CAP_INTELPERC") == 0)
  {
    return cv::CAP_INTELPERC;
  }
  else if (apiString.compare("CAP_OPENNI2") == 0)
  {
    return cv::CAP_OPENNI2;
  }
  else if (apiString.compare("CAP_OPENNI2_ASUS") == 0)
  {
    return cv::CAP_OPENNI2_ASUS;
  }
  else if (apiString.compare("CAP_GPHOTO2") == 0)
  {
    return cv::CAP_GPHOTO2;
  }
  else if (apiString.compare("CAP_GSTREAMER") == 0)
  {
    return cv::CAP_GSTREAMER;
  }
  else if (apiString.compare("CAP_FFMPEG") == 0)
  {
    return cv::CAP_FFMPEG;
  }
  else if (apiString.compare("CAP_IMAGES") == 0)
  {
    return cv::CAP_IMAGES;
  }
  else if (apiString.compare("CAP_ARAVIS") == 0)
  {
    return cv::CAP_ARAVIS;
  }

  LOG_WARNING("Unable to match requested API " << apiString << ". Defaulting to CAP_ANY");
  return cv::CAP_ANY;
}

#define _StringFromEnum(x) std::string(#x)
//----------------------------------------------------------------------------
std::string vtkPlusOpenCVCaptureVideoSource::StringFromCaptureAPI(cv::VideoCaptureAPIs api)
{
  switch (api)
  {
  case cv::CAP_ANY:
    return _StringFromEnum(CAP_ANY);
  case cv::CAP_VFW:
    return _StringFromEnum(CAP_VFW);
  case cv::CAP_FIREWIRE:
    return _StringFromEnum(CAP_FIREWIRE);
  case cv::CAP_QT:
    return _StringFromEnum(CAP_QT);
  case cv::CAP_UNICAP:
    return _StringFromEnum(CAP_UNICAP);
  case cv::CAP_DSHOW:
    return _StringFromEnum(CAP_DSHOW);
  case cv::CAP_PVAPI:
    return _StringFromEnum(CAP_PVAPI);
  case cv::CAP_OPENNI:
    return _StringFromEnum(CAP_OPENNI);
  case cv::CAP_OPENNI_ASUS:
    return _StringFromEnum(CAP_OPENNI_ASUS);
  case cv::CAP_ANDROID:
    return _StringFromEnum(CAP_ANDROID);
  case cv::CAP_XIAPI:
    return _StringFromEnum(CAP_XIAPI);
  case cv::CAP_AVFOUNDATION:
    return _StringFromEnum(CAP_AVFOUNDATION);
  case cv::CAP_GIGANETIX:
    return _StringFromEnum(CAP_GIGANETIX);
  case cv::CAP_MSMF:
    return _StringFromEnum(CAP_MSMF);
  case cv::CAP_WINRT:
    return _StringFromEnum(CAP_WINRT);
  case cv::CAP_INTELPERC:
    return _StringFromEnum(CAP_INTELPERC);
  case cv::CAP_OPENNI2:
    return _StringFromEnum(CAP_OPENNI2);
  case cv::CAP_OPENNI2_ASUS:
    return _StringFromEnum(CAP_OPENNI2_ASUS);
  case cv::CAP_GPHOTO2:
    return _StringFromEnum(CAP_GPHOTO2);
  case cv::CAP_GSTREAMER:
    return _StringFromEnum(CAP_GSTREAMER);
  case cv::CAP_FFMPEG:
    return _StringFromEnum(CAP_FFMPEG);
  case cv::CAP_IMAGES:
    return _StringFromEnum(CAP_IMAGES);
  case cv::CAP_ARAVIS:
    return _StringFromEnum(CAP_ARAVIS);
  default:
    return "CAP_ANY";
  }
}
#undef _StringFromEnum
