/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusAzureKinect.h"

// Local includes
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkObjectFactory.h>

// Kinect azure includes
#include <k4a/k4a.hpp>

// Stl includes
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPlusAzureKinect);

namespace
{
  enum KinectSourceType
  {
    RGB = 1 << 0,
    DEPTH = 1 << 1,
    PCL = 1 << 2
  };

  struct KinectStreamConfig
  {
    KinectSourceType Type;
    std::string Id;
    uint32_t FrameRate{0};
    uint32_t Width{0};
    uint32_t Height{0};
    vtkPlusDataSource* Source{nullptr};
  };

  std::string KinectSourceTypeAsString(KinectSourceType type)
  {
    switch (type)
    {
      case KinectSourceType::RGB:
        return "RGB";
      case KinectSourceType::DEPTH:
        return "DEPTH";
      case KinectSourceType::PCL:
        return "PCL";
      default:
        return "";
    }
  }

  std::tuple<bool, k4a_fps_t> ToKinectFrameRate(int rate)
  {
    if (rate == 5)
    {
      return std::make_tuple(true, K4A_FRAMES_PER_SECOND_5);
    }
    else if (rate == 15)
    {
      return std::make_tuple(true, K4A_FRAMES_PER_SECOND_15);
    }
    else if (rate == 30)
    {
      return std::make_tuple(true, K4A_FRAMES_PER_SECOND_30);
    }

    return std::make_tuple(false, K4A_FRAMES_PER_SECOND_5);
  }

  std::tuple<bool, k4a_depth_mode_t, int> ToKinectDepthMode(int width)
  {
    if (width == 320)
    {
      return std::make_tuple(true, K4A_DEPTH_MODE_NFOV_2X2BINNED, 288);
    }
    else if (width == 640)
    {
      return std::make_tuple(true, K4A_DEPTH_MODE_NFOV_UNBINNED, 576);
    }
    else if (width == 512)
    {
      return std::make_tuple(true, K4A_DEPTH_MODE_WFOV_2X2BINNED, 512);
    }
    else if (width == 1024)
    {
      return std::make_tuple(true, K4A_DEPTH_MODE_WFOV_UNBINNED, 1024);
    }

    return std::make_tuple(false, K4A_DEPTH_MODE_OFF, 0);
  }

  std::tuple<bool, k4a_color_resolution_t, int> ToKinectColorMode(int width)
  {
    if (width == 1280)
    {
      return std::make_tuple(true, K4A_COLOR_RESOLUTION_720P, 720);
    }
    else if (width == 1920)
    {
      return std::make_tuple(true, K4A_COLOR_RESOLUTION_1080P, 1080);
    }
    else if (width == 2560)
    {
      return std::make_tuple(true, K4A_COLOR_RESOLUTION_1440P, 1440);
    }
    else if (width == 2048)
    {
      return std::make_tuple(true, K4A_COLOR_RESOLUTION_1536P, 1536);
    }
    else if (width == 3840)
    {
      return std::make_tuple(true, K4A_COLOR_RESOLUTION_2160P, 2160);
    }
    else if (width == 4096)
    {
      return std::make_tuple(true, K4A_COLOR_RESOLUTION_3072P, 3072);
    }

    return std::make_tuple(false, K4A_COLOR_RESOLUTION_OFF, 0);
  }
}

//----------------------------------------------------------------------------
class vtkPlusAzureKinect::vtkInternal
{
public:
  vtkPlusAzureKinect* External;
  std::vector<KinectStreamConfig> StreamList;
  k4a::device Device;
  k4a_device_configuration_t DeviceConfig;
  k4a::calibration Calibration;
  k4a::transformation Transformation;
  k4a::image ColorImage;
  k4a::image DepthImage;
  k4a::image PointsCloudImage;
  bool AlignDepthStream{false};
  int AvailableStreamsFlag{0};

  vtkInternal(vtkPlusAzureKinect* external)
    : External(external)
  {
    this->DeviceConfig.color_format = K4A_IMAGE_FORMAT_COLOR_BGRA32;
    this->DeviceConfig.synchronized_images_only = true;
    this->DeviceConfig.depth_mode = K4A_DEPTH_MODE_OFF;
    this->DeviceConfig.color_resolution = K4A_COLOR_RESOLUTION_OFF;
    this->DeviceConfig.wired_sync_mode = K4A_WIRED_SYNC_MODE_STANDALONE;
    this->DeviceConfig.depth_delay_off_color_usec = 0;
    this->DeviceConfig.subordinate_delay_off_master_usec = 0;
  }

  PlusStatus UpdateConfig()
  {
    bool status{false};
    std::vector<uint32_t> frameRates;
    for (auto& stream : this->StreamList)
    {
      std::tie(status, DeviceConfig.camera_fps) = ToKinectFrameRate(stream.FrameRate);
      if (!status)
      {
        LOG_ERROR("Invalid frame rate detected: 5, 15, 30 expected");
        return PLUS_FAIL;
      }

      frameRates.emplace_back(stream.FrameRate);

      if (stream.Type == KinectSourceType::RGB)
      {
        std::tie(status, DeviceConfig.color_resolution, stream.Height) = ToKinectColorMode(stream.Width);
      }
      else if (stream.Type == KinectSourceType::DEPTH)
      {
        std::tie(status, DeviceConfig.depth_mode, stream.Height) = ToKinectDepthMode(stream.Width);
      }

      if (!status)
      {
        LOG_ERROR("Invalid stream configuration");
        return PLUS_FAIL;
      }
    }

    auto rgbStream = std::find_if(std::begin(this->StreamList), std::end(this->StreamList), [](const KinectStreamConfig & stream)
    {
      return stream.Type == KinectSourceType::RGB;
    });
    auto depthStream = std::find_if(std::begin(this->StreamList), std::end(this->StreamList), [](const KinectStreamConfig & stream)
    {
      return stream.Type == KinectSourceType::DEPTH;
    });
    auto pclStream = std::find_if(std::begin(this->StreamList), std::end(this->StreamList), [](const KinectStreamConfig & stream)
    {
      return stream.Type == KinectSourceType::PCL;
    });

    if (rgbStream != std::end(this->StreamList))
    {
      this->AvailableStreamsFlag |= KinectSourceType::RGB;
    }

    if (depthStream != std::end(this->StreamList))
    {
      this->AvailableStreamsFlag |= KinectSourceType::DEPTH;
    }

    if (pclStream != std::end(this->StreamList))
    {
      this->AvailableStreamsFlag |= KinectSourceType::PCL;
    }

    if ((this->AvailableStreamsFlag & KinectSourceType::PCL) && !(this->AvailableStreamsFlag & KinectSourceType::DEPTH))
    {
      LOG_ERROR("Invalid configuration: PCL stream is required but no Depth stream set");
      return PLUS_FAIL;
    }

    if (!(this->AvailableStreamsFlag & KinectSourceType::RGB) || !(this->AvailableStreamsFlag & KinectSourceType::DEPTH))
    {
      DeviceConfig.synchronized_images_only = false;
    }

    if (AlignDepthStream)
    {
      if (!DeviceConfig.synchronized_images_only)
      {
        LOG_ERROR("Invalid configuration: AlignDepthStream set to TRUE but no RGB or Depth stream set");
        return PLUS_FAIL;
      }

      depthStream->Width = rgbStream->Width;
      depthStream->Height = rgbStream->Height;
    }

    if ((this->AvailableStreamsFlag & KinectSourceType::PCL) && (this->AvailableStreamsFlag & KinectSourceType::RGB))
    {
      pclStream->Width = rgbStream->Width;
      pclStream->Height = rgbStream->Height;
    }
    else if (this->AvailableStreamsFlag & KinectSourceType::PCL)
    {
      pclStream->Width = depthStream->Width;
      pclStream->Height = depthStream->Height;
    }

    std::sort(std::begin(frameRates), std::end(frameRates));
    frameRates.erase(std::unique(std::begin(frameRates), std::end(frameRates)), std::end(frameRates));

    if (frameRates.size() > 1)
    {
      LOG_WARNING("Detecting different frame rates for the streams: selecting the lowest rate (" << frameRates[0] << " fps)");
    }

    this->External->InternalUpdateRate = frameRates[0];
    this->External->AcquisitionRate = frameRates[0];

    return PLUS_SUCCESS;
  }

  PlusStatus Connect()
  {
    try
    {
      this->Device = k4a::device::open(0);
    }
    catch (const k4a::error& err)
    {
      LOG_ERROR("Failed to connect: " << err.what());
      return PLUS_FAIL;
    }

    return PLUS_SUCCESS;
  }

  PlusStatus Disconnect()
  {
    this->Device.close();
    return PLUS_SUCCESS;
  }

  PlusStatus Start()
  {
    try
    {
      this->Device.start_cameras(&(this->DeviceConfig));
      this->Calibration = this->Device.get_calibration(this->DeviceConfig.depth_mode, this->DeviceConfig.color_resolution);
      this->Transformation = k4a::transformation(this->Calibration);
    }
    catch (const k4a::error& err)
    {
      LOG_ERROR("Failed to start camera: " << err.what());
      return PLUS_FAIL;
    }

    return PLUS_SUCCESS;
  }

  PlusStatus Stop()
  {
    this->Device.stop_cameras();
    return PLUS_SUCCESS;
  }

  void Update()
  {
    k4a::capture capture;
    if (!this->Device.get_capture(&capture, std::chrono::milliseconds(0)))
    {
      return;
    }

    if (this->AvailableStreamsFlag & KinectSourceType::DEPTH)
    {
      this->DepthImage = capture.get_depth_image();
    }

    if (this->AvailableStreamsFlag & KinectSourceType::RGB)
    {
      this->ColorImage = capture.get_color_image();
    }

    if (this->ColorImage && this->DepthImage && this->AlignDepthStream)
    {
      this->DepthImage = this->Transformation.depth_image_to_color_camera(this->DepthImage);
    }

    if (!(this->AvailableStreamsFlag & KinectSourceType::PCL))
    {
      return;
    }

    if (this->AvailableStreamsFlag & KinectSourceType::RGB)
    {
      this->PointsCloudImage = this->Transformation.depth_image_to_point_cloud(
                                 (this->AlignDepthStream ? this->DepthImage : this->Transformation.depth_image_to_color_camera(this->DepthImage)), K4A_CALIBRATION_TYPE_COLOR);
    }
    else
    {
      this->PointsCloudImage = this->Transformation.depth_image_to_point_cloud(this->DepthImage,
                               K4A_CALIBRATION_TYPE_DEPTH);
    }
  }
};

//----------------------------------------------------------------------------
vtkPlusAzureKinect::vtkPlusAzureKinect()
  : Internal(new vtkInternal(this))
{
  this->RequireImageOrientationInConfiguration = true;
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkPlusAzureKinect::~vtkPlusAzureKinect()
{
  delete this->Internal;
  this->Internal = nullptr;
}

//----------------------------------------------------------------------------
void vtkPlusAzureKinect::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "Kinect Azure Configuration" << std::endl;
  os << indent << "AlignDepthStream: " << this->Internal->AlignDepthStream << std::endl;
  for (const auto& config : this->Internal->StreamList)
  {
    os << indent << "Source: " << config.Id << std::endl;
    os << indent << indent << "Type: " << KinectSourceTypeAsString(config.Type) << std::endl;
    os << indent << indent << "Frame Rate: " << config.FrameRate << std::endl;
    os << indent << indent << "Frame Width: " << config.Width << std::endl;
    os << indent << indent << "Frame Height: " << config.Height << std::endl;
  }
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusAzureKinect::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  XML_READ_BOOL_ATTRIBUTE_NONMEMBER_OPTIONAL(AlignDepthStream, this->Internal->AlignDepthStream, deviceConfig);
  XML_FIND_NESTED_ELEMENT_REQUIRED(dataSourcesElement, deviceConfig, "DataSources");

  LOG_TRACE("Reading custom configuration fields in " << dataSourcesElement->GetNumberOfNestedElements() << " nested elements");
  for (int nestedElementIndex = 0; nestedElementIndex < dataSourcesElement->GetNumberOfNestedElements(); ++nestedElementIndex)
  {
    vtkXMLDataElement* dataElement = dataSourcesElement->GetNestedElement(nestedElementIndex);
    if (STRCASECMP(dataElement->GetName(), "DataSource") != 0)
    {
      continue;
    }

    LOG_TRACE("Found a new data source");

    if (dataElement->GetAttribute("Type") != NULL && STRCASECMP(dataElement->GetAttribute("Type"), "Video") == 0)
    {
      const char* toolId = dataElement->GetAttribute("Id");
      if (toolId == nullptr)
      {
        // tool doesn't have ID needed to generate transform
        LOG_ERROR("Failed to initialize Kinect Azure DataSource: Id is missing");
        continue;
      }

      LOG_TRACE("Data source name: " << toolId);

      KinectStreamConfig config;
      config.Id = toolId;
      XML_READ_ENUM3_ATTRIBUTE_NONMEMBER_REQUIRED(FrameType, config.Type, dataElement, "RGB", KinectSourceType::RGB, "DEPTH", KinectSourceType::DEPTH, "PCL", KinectSourceType::PCL);
      XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_REQUIRED(int, FrameRate, config.FrameRate, dataElement);
      XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_REQUIRED(int, FrameSize, config.Width, dataElement);
      this->Internal->StreamList.push_back(config);
    }
    else
    {
      LOG_ERROR("DataSource with unknown Type.");
      return PLUS_FAIL;
    }
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusAzureKinect::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  // write bool AlignDepthStream
  if (this->Internal->AlignDepthStream)
  {
    deviceConfig->SetAttribute("AlignDepthStream", "TRUE");
  }
  else
  {
    deviceConfig->SetAttribute("AlignDepthStream", "FALSE");
  }
  return PLUS_SUCCESS;
}

PlusStatus vtkPlusAzureKinect::NotifyConfigured()
{
  return this->Internal->UpdateConfig();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAzureKinect::InternalConnect()
{
  for (auto& stream : this->Internal->StreamList)
  {
    GetVideoSource(stream.Id.c_str(), stream.Source);
    if (stream.Source == nullptr)
    {
      LOG_ERROR("Invalid source for tool id " << stream.Id);
      return PLUS_FAIL;
    }
  }
  return this->Internal->Connect();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAzureKinect::InternalDisconnect()
{
  return this->Internal->Disconnect();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAzureKinect::InternalStartRecording()
{
  this->FrameNumber = 0;

  return this->Internal->Start();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAzureKinect::InternalStopRecording()
{
  return this->Internal->Stop();
}

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
PlusStatus vtkPlusAzureKinect::InternalUpdate()
{
  this->Internal->Update();

  // forward video data to PlusDataSource
  for (auto& stream : this->Internal->StreamList)
  {
    if (stream.Source->GetNumberOfItems() == 0 && stream.Type == KinectSourceType::RGB)
    {
      LOG_TRACE("Setting up color frame");
      stream.Source->SetImageType(US_IMG_RGB_COLOR);
      stream.Source->SetPixelType(VTK_UNSIGNED_CHAR);
      stream.Source->SetNumberOfScalarComponents(3);
      stream.Source->SetInputFrameSize(stream.Width, stream.Height, 1);
    }
    else if (stream.Source->GetNumberOfItems() == 0 && stream.Type == KinectSourceType::DEPTH)
    {
      // depth output is raw depth data
      LOG_TRACE("Setting up depth frame");
      stream.Source->SetImageType(US_IMG_BRIGHTNESS);
      stream.Source->SetPixelType(VTK_TYPE_UINT16);
      stream.Source->SetNumberOfScalarComponents(1);
      stream.Source->SetInputFrameSize(stream.Width, stream.Height, 1);
    }
    else if (stream.Source->GetNumberOfItems() == 0 && stream.Type == KinectSourceType::PCL)
    {
      // depth output is raw pcl data
      LOG_TRACE("Setting up pcl frame");
      stream.Source->SetImageType(US_IMG_BRIGHTNESS);
      stream.Source->SetPixelType(VTK_TYPE_INT16);
      stream.Source->SetNumberOfScalarComponents(3);
      stream.Source->SetInputFrameSize(stream.Width, stream.Height, 1);
    }

    if (stream.Type == KinectSourceType::RGB)
    {
      if (!this->Internal->ColorImage)
      {
        if (stream.Source->GetNumberOfItems() == 0)
        {
          LOG_TRACE("Waiting for KinectAzure RGB images");
        }
        else
        {
          LOG_WARNING("Failed to get KinectAzure RGB image");
        }
        return PLUS_FAIL;
      }

      // Kinect Azure has a BGRA layout thus convert to RGB
      const auto* bgraBuffer = this->Internal->ColorImage.get_buffer();
      unsigned rgbSize = this->Internal->ColorImage.get_size() * 3 / 4;
      std::unique_ptr<uint8_t[]> rgbBuffer(new uint8_t[rgbSize]);

      for (auto i = 0U; i < this->Internal->ColorImage.get_size(); i += 4)
      {
        rgbBuffer[3 * i / 4] = bgraBuffer[i + 2];
        rgbBuffer[3 * i / 4 + 1] = bgraBuffer[i + 1];
        rgbBuffer[3 * i / 4 + 2] = bgraBuffer[i];
      }

      FrameSizeType frameSizeColor = {stream.Width, stream.Height, 1};
      if (stream.Source->AddItem(reinterpret_cast<void*>(rgbBuffer.get()), stream.Source->GetInputImageOrientation(), frameSizeColor, VTK_UNSIGNED_CHAR, 3, US_IMG_RGB_COLOR, 0, this->FrameNumber) == PLUS_FAIL)
      {
        LOG_ERROR("Unable to send RGB image. Skipping frame.");
        return PLUS_FAIL;
      }
      this->Modified();
    }
    else if (stream.Type == KinectSourceType::DEPTH)
    {
      if (!this->Internal->DepthImage)
      {
        if (stream.Source->GetNumberOfItems() == 0)
        {
          LOG_TRACE("Waiting for KinectAzure DEPTH images");
        }
        else
        {
          LOG_WARNING("Failed to get KinectAzure DEPTH image");
        }
        return PLUS_FAIL;
      }

      FrameSizeType frameSizeDepth = {stream.Width, stream.Height, 1};
      if (stream.Source->AddItem(reinterpret_cast<void*>(this->Internal->DepthImage.get_buffer()), stream.Source->GetInputImageOrientation(), frameSizeDepth, VTK_TYPE_UINT16, 1, US_IMG_BRIGHTNESS, 0, this->FrameNumber) == PLUS_FAIL)
      {
        LOG_ERROR("Unable to send DEPTH image. Skipping frame.");
        return PLUS_FAIL;
      }
      this->Modified();
    }
    else if (stream.Type == KinectSourceType::PCL)
    {
      if (!this->Internal->PointsCloudImage)
      {
        if (stream.Source->GetNumberOfItems() == 0)
        {
          LOG_TRACE("Waiting for KinectAzure PCL images");
        }
        else
        {
          LOG_WARNING("Failed to get KinectAzure PCL image");
        }
        return PLUS_FAIL;
      }

      FrameSizeType frameSizeDepth = {stream.Width, stream.Height, 1};
      if (stream.Source->AddItem(reinterpret_cast<void*>(this->Internal->PointsCloudImage.get_buffer()), stream.Source->GetInputImageOrientation(), frameSizeDepth, VTK_TYPE_INT16, 3, US_IMG_BRIGHTNESS, 0, this->FrameNumber) == PLUS_FAIL)
      {
        LOG_ERROR("Unable to send PCL image. Skipping frame.");
        return PLUS_FAIL;
      }
      this->Modified();
    }
  }

  this->FrameNumber++;
  return PLUS_SUCCESS;
}