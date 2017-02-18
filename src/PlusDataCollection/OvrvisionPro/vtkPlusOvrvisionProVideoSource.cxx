/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusOvrvisionProVideoSource.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtksys/SystemTools.hxx"

// OpenCV includes
#include <cv.h>
#include <opencv2/imgproc.hpp>

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusOvrvisionProVideoSource);

//----------------------------------------------------------------------------
vtkPlusOvrvisionProVideoSource::vtkPlusOvrvisionProVideoSource()
  : RequestedFormat(OVR::OV_CAM20VR_VGA)
  , ProcessingMode(OVR::OV_CAMQT_NONE)
  , CameraSync(false)
  , Framerate(-1)
  , Exposure(7808)
  , LeftEyeDataSource(NULL)
  , RightEyeDataSource(NULL)
{
  this->RequireImageOrientationInConfiguration = true;

  // Poll-based device
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkPlusOvrvisionProVideoSource::~vtkPlusOvrvisionProVideoSource()
{
  if (!this->Connected)
  {
    this->Disconnect();
  }
}

//----------------------------------------------------------------------------
void vtkPlusOvrvisionProVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "Resolution: " << Resolution[0] << ", " << Resolution[1] << std::endl;
  os << indent << "Framerate: " << Framerate << std::endl;
  os << indent << "CameraSync: " << CameraSync << std::endl;
  os << indent << "ProcessingMode: " << ProcessingModeName << std::endl;
  os << indent << "LeftEyeDataSourceName: " << LeftEyeDataSourceName << std::endl;
  os << indent << "RightEyeDataSourceName: " << RightEyeDataSourceName << std::endl;
  os << indent << "Vendor: " << Vendor << std::endl;
}

//----------------------------------------------------------------------------
void vtkPlusOvrvisionProVideoSource::SetLeftEyeDataSourceName(const std::string& leftEyeDataSourceName)
{
  this->LeftEyeDataSourceName = leftEyeDataSourceName;
}

//----------------------------------------------------------------------------
void vtkPlusOvrvisionProVideoSource::SetVendor(const std::string& vendor)
{
  this->Vendor = vendor;
}

//----------------------------------------------------------------------------
void vtkPlusOvrvisionProVideoSource::SetProcessingModeName(const std::string& processingModeName)
{
  this->ProcessingModeName = processingModeName;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOvrvisionProVideoSource::InternalConnect()
{
  LOG_TRACE("vtkPlusOvrvisionProVideoSource::InternalConnect");

  if (!OvrvisionProHandle.Open(0, RequestedFormat, Vendor.c_str()))     // We don't need to share it with OpenGL/D3D, but in the future we could access the images in GPU memory
  {
    LOG_ERROR("Unable to connect to OvrvisionPro device.");
    return PLUS_FAIL;
  }

  if (OvrvisionProHandle.GetCamWidth() != Resolution[0])
  {
    LOG_ERROR("Improperly configured device. Cannot connect. Width (cam)" << OvrvisionProHandle.GetCamWidth() << " != (config)" << Resolution[0]);
    OvrvisionProHandle.Close();
    return PLUS_FAIL;
  }

  if (OvrvisionProHandle.GetCamHeight() != Resolution[1])
  {
    LOG_ERROR("Improperly configured device. Cannot connect. Height (cam)" << OvrvisionProHandle.GetCamHeight() << " != (config)" << Resolution[1]);
    OvrvisionProHandle.Close();
    return PLUS_FAIL;
  }

  int frameSize[3] = { Resolution[0], Resolution[1], 1 };
  LeftEyeDataSource->SetInputFrameSize(frameSize);
  LeftEyeDataSource->SetNumberOfScalarComponents(3);
  RightEyeDataSource->SetInputFrameSize(frameSize);
  RightEyeDataSource->SetNumberOfScalarComponents(3);

  OvrvisionProHandle.SetCameraSyncMode(CameraSync);
  OvrvisionProHandle.SetCameraExposure(Exposure);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOvrvisionProVideoSource::InternalDisconnect()
{
  LOG_DEBUG("vtkPlusOvrvisionProVideoSource::InternalDisconnect");

  if (OvrvisionProHandle.isOpen())
  {
    OvrvisionProHandle.Close();
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOvrvisionProVideoSource::InternalUpdate()
{
  int numErrors(0);

  // Query the SDK for the latest frames
  if (this->IsCapturingRGB)
  {
    OvrvisionProHandle.PreStoreCamData(this->ProcessingMode);
  }
  else
  {
    OvrvisionProHandle.PreStoreCamData(OVR::OV_CAMQT_NONE);
  }

  // Greyscale simply means r = g = b = a (see PreStoreCamData source)
  cv::Mat matLeft(OvrvisionProHandle.GetCamHeight(), OvrvisionProHandle.GetCamWidth(), CV_8UC4, OvrvisionProHandle.GetCamImageBGRA(OVR::OV_CAMEYE_LEFT));
  cv::Mat matRight(OvrvisionProHandle.GetCamHeight(), OvrvisionProHandle.GetCamWidth(), CV_8UC4, OvrvisionProHandle.GetCamImageBGRA(OVR::OV_CAMEYE_RIGHT));

  if (this->IsCapturingRGB)
  {
    // Convert From BGRA to RGB
    cv::cvtColor(matLeft, matLeft, cv::COLOR_BGRA2RGB);
    cv::cvtColor(matRight, matRight, cv::COLOR_BGRA2RGB);
  }
  else
  {
    cv::cvtColor(matLeft, matLeft, cv::COLOR_BGRA2GRAY);
    cv::cvtColor(matRight, matRight, cv::COLOR_BGRA2GRAY);
  }

  // Add them to our local buffers
  if (LeftEyeDataSource->AddItem(matLeft.data,
                                 LeftEyeDataSource->GetInputImageOrientation(),
                                 LeftEyeDataSource->GetInputFrameSize(),
                                 VTK_UNSIGNED_CHAR,
                                 matLeft.channels(),
                                 matLeft.channels() == 3 ? US_IMG_RGB_COLOR : US_IMG_BRIGHTNESS,
                                 0,
                                 this->FrameNumber) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to add left eye image to data source.");
    numErrors++;
  }

  if (RightEyeDataSource->AddItem(matRight.data,
                                  RightEyeDataSource->GetInputImageOrientation(),
                                  RightEyeDataSource->GetInputFrameSize(),
                                  VTK_UNSIGNED_CHAR,
                                  matRight.channels(),
                                  matRight.channels() == 3 ? US_IMG_RGB_COLOR : US_IMG_BRIGHTNESS,
                                  0,
                                  this->FrameNumber) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to add right eye image to data source.");
    numErrors++;
  }

  this->FrameNumber++;

  return numErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL;
}

//----------------------------------------------------------------------------
bool vtkPlusOvrvisionProVideoSource::ConfigureRequestedFormat()
{
  RegionOfInterest.offsetX = 0;
  RegionOfInterest.offsetY = 0;
  RegionOfInterest.width = Resolution[0];
  RegionOfInterest.height = Resolution[1];

  switch (Framerate)
  {
    case 15:
      if (Resolution[0] == 2560 && Resolution[1] == 1920)
      {
        RequestedFormat = OVR::OV_CAM5MP_FULL;
        return true;
      }
      if (Resolution[0] == 1280 && Resolution[1] == 960)
      {
        RequestedFormat = OVR::OV_CAM20HD_FULL;
        return true;
      }
      return false;
    case 30:
      if (Resolution[0] == 1920 && Resolution[1] == 1080)
      {
        RequestedFormat = OVR::OV_CAM5MP_FHD;
        return true;
      }
      if (Resolution[0] == 640 && Resolution[1] == 480)
      {
        RequestedFormat = OVR::OV_CAM20VR_VGA;
        return true;
      }
      return false;
    case 45:
      if (Resolution[0] == 1280 && Resolution[1] == 960)
      {
        RequestedFormat = OVR::OV_CAMHD_FULL;
        return true;
      }
      return false;
    case 60:
      if (Resolution[0] == 960 && Resolution[1] == 950)
      {
        RequestedFormat = OVR::OV_CAMVR_FULL;
        return true;
      }
      if (Resolution[0] == 1280 && Resolution[1] == 800)
      {
        RequestedFormat = OVR::OV_CAMVR_WIDE;
        return true;
      }
      return false;
    case 90:
      if (Resolution[0] == 640 && Resolution[1] == 480)
      {
        RequestedFormat = OVR::OV_CAMVR_VGA;
        return true;
      }
      return false;
    case 120:
      if (Resolution[0] == 320 && Resolution[1] == 240)
      {
        RequestedFormat = OVR::OV_CAMVR_QVGA;
        return true;
      }
      return false;
    default:
      LOG_ERROR("Unsupported framerate requested.");
  }

  return false;
}

//----------------------------------------------------------------------------
void vtkPlusOvrvisionProVideoSource::ConfigureProcessingMode()
{
  ProcessingMode = OVR::OV_CAMQT_NONE;
  if (PlusCommon::IsEqualInsensitive(ProcessingModeName, "OV_CAMQT_DMSRMP"))
  {
    ProcessingMode = OVR::OV_CAMQT_DMSRMP;
  }
  else if (PlusCommon::IsEqualInsensitive(ProcessingModeName, "OV_CAMQT_DMS"))
  {
    ProcessingMode = OVR::OV_CAMQT_DMS;
  }
  else
  {
    LOG_WARNING("Unrecognized processing mode detected.");
  }
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusOvrvisionProVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_VECTOR_ATTRIBUTE_REQUIRED(int, 2, Resolution, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_REQUIRED(int, Framerate, deviceConfig);
  XML_READ_STRING_ATTRIBUTE_REQUIRED(LeftEyeDataSourceName, deviceConfig);
  XML_READ_STRING_ATTRIBUTE_REQUIRED(RightEyeDataSourceName, deviceConfig);
  XML_READ_STRING_ATTRIBUTE_REQUIRED(Vendor, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, Exposure, deviceConfig);

  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(CameraSync, deviceConfig);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(ProcessingModeName, deviceConfig);

  if (!ConfigureRequestedFormat())
  {
    return PLUS_FAIL;
  }

  if (!ProcessingModeName.empty())
  {
    ConfigureProcessingMode();
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusOvrvisionProVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  int resolution[2];
  switch (RequestedFormat)
  {
    case OVR::OV_CAM5MP_FULL:
      //!2560x1920 @15fps
      resolution[0] = 2560;
      resolution[1] = 1920;
      deviceConfig->SetVectorAttribute("Resolution", 2, resolution);
      deviceConfig->SetIntAttribute("Framerate", 15);
      break;
    case OVR::OV_CAM5MP_FHD:
      //!1920x1080 @30fps
      resolution[0] = 1920;
      resolution[1] = 1080;
      deviceConfig->SetVectorAttribute("Resolution", 2, resolution);
      deviceConfig->SetIntAttribute("Framerate", 30);
      break;
    case OVR::OV_CAMHD_FULL:
      //!1280x960  @45fps
      resolution[0] = 1280;
      resolution[1] = 960;
      deviceConfig->SetVectorAttribute("Resolution", 2, resolution);
      deviceConfig->SetIntAttribute("Framerate", 45);
      break;
    case OVR::OV_CAMVR_FULL:
      //!960x950   @60fps
      resolution[0] = 960;
      resolution[1] = 950;
      deviceConfig->SetVectorAttribute("Resolution", 2, resolution);
      deviceConfig->SetIntAttribute("Framerate", 60);
      break;
    case OVR::OV_CAMVR_WIDE:
      //!1280x800  @60fps
      resolution[0] = 1280;
      resolution[1] = 800;
      deviceConfig->SetVectorAttribute("Resolution", 2, resolution);
      deviceConfig->SetIntAttribute("Framerate", 60);
      break;
    case OVR::OV_CAMVR_VGA:
      //!640x480   @90fps
      resolution[0] = 640;
      resolution[1] = 480;
      deviceConfig->SetVectorAttribute("Resolution", 2, resolution);
      deviceConfig->SetIntAttribute("Framerate", 90);
      break;
    case OVR::OV_CAMVR_QVGA:
      //!320x240   @120fps
      resolution[0] = 320;
      resolution[1] = 240;
      deviceConfig->SetVectorAttribute("Resolution", 2, resolution);
      deviceConfig->SetIntAttribute("Framerate", 120);
      break;
    case OVR::OV_CAM20HD_FULL:
      //!1280x960  @15fps
      resolution[0] = 1280;
      resolution[1] = 960;
      deviceConfig->SetVectorAttribute("Resolution", 2, resolution);
      deviceConfig->SetIntAttribute("Framerate", 15);
      break;
    case OVR::OV_CAM20VR_VGA:
      //!640x480   @30fps
      resolution[0] = 640;
      resolution[1] = 480;
      deviceConfig->SetVectorAttribute("Resolution", 2, resolution);
      deviceConfig->SetIntAttribute("Framerate", 30);
      break;
  }

  if (CameraSync)
  {
    deviceConfig->SetAttribute("CameraSync", "TRUE");
  }

  if (ProcessingMode != OVR::OV_CAMQT_NONE)
  {
    switch (ProcessingMode)
    {
      case OVR::OV_CAMQT_DMS:
        deviceConfig->SetAttribute("ProcessingModeName", "OV_CAMQT_DMS");
        break;
      case OVR::OV_CAMQT_DMSRMP:
        deviceConfig->SetAttribute("ProcessingModeName", "OV_CAMQT_DMSRMP");
        break;
    }
  }

  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(Vendor, deviceConfig);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusOvrvisionProVideoSource::NotifyConfigured()
{
  // OvrvisionSDK requires two data sources, left eye and right eye
  if (this->GetDataSource(LeftEyeDataSourceName, LeftEyeDataSource) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to locate data source for left eye labelled: " << LeftEyeDataSourceName);
    return PLUS_FAIL;
  }

  if (this->GetDataSource(RightEyeDataSourceName, RightEyeDataSource) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to locate data source for right eye labelled: " << RightEyeDataSourceName);
    return PLUS_FAIL;
  }

  if (LeftEyeDataSource->GetImageType() != RightEyeDataSource->GetImageType())
  {
    LOG_ERROR("Mistmatch between left and right eye data source image types.");
    return PLUS_FAIL;
  }

  if (LeftEyeDataSource->GetImageType() == US_IMG_RGB_COLOR)
  {
    this->IsCapturingRGB = true;
  }

  if (RightEyeDataSource->GetImageType() != US_IMG_RGB_COLOR)
  {
    LOG_ERROR("Right eye data source must be configured for image type US_IMG_RGB_COLOR. Aborting.");
    return PLUS_FAIL;
  }

  if (this->OutputChannels.size() != 2)
  {
    LOG_ERROR("OvrvisionPro device requires exactly 2 output channels. One for each eye.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
std::string vtkPlusOvrvisionProVideoSource::GetLeftEyeDataSourceName() const
{
  return this->LeftEyeDataSourceName;
}

//----------------------------------------------------------------------------
std::string vtkPlusOvrvisionProVideoSource::GetRightEyeDataSourceName() const
{
  return this->RightEyeDataSourceName;
}

//----------------------------------------------------------------------------
std::string vtkPlusOvrvisionProVideoSource::GetProcessingModeName() const
{
  return this->ProcessingModeName;
}

//----------------------------------------------------------------------------
std::string vtkPlusOvrvisionProVideoSource::GetVendor() const
{
  return this->Vendor;
}