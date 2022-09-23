/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusRevopoint3DCamera.h"

// Local includes
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkObjectFactory.h>

// Revopoint SDK includes
#include "3DCamera.hpp"

// Stl includes
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPlusRevopoint3DCamera);

namespace
{
  enum RevoSourceType
  {
    DEPTH = 1 << 0,
    PCL = 1 << 1
  };

  struct StreamConfigBase
  {
    std::string Id;
    StreamInfo StreamProps;
    vtkPlusDataSource* Source{nullptr};
  };

  struct PCLStreamConfig : StreamConfigBase
  {
  };

  struct ConcreteStreamConfig : StreamConfigBase
  {
    Intrinsics Intr;
    Extrinsics Extr;
  };

  struct DepthStreamConfig : ConcreteStreamConfig
  {
    int DepthRange[2];
  };

  std::string convertErrorCode(ERROR_CODE error)
  {
    switch (error)
    {
      case ERROR_CODE::ERROR_PARAM:
        return "Bad param";
      case ERROR_CODE::ERROR_DEVICE_NOT_FOUND:
        return "Device not found";
      case ERROR_CODE::ERROR_DEVICE_NOT_CONNECT:
        return "Device not connected";
      case ERROR_CODE::ERROR_DEVICE_BUSY:
        return "Device busy";
      case ERROR_CODE::ERROR_STREAM_NOT_START:
        return "Device not started";
      case ERROR_CODE::ERROR_STREAM_BUSY:
        return "Stream busy";
      case ERROR_CODE::ERROR_FRAME_TIMEOUT:
        return "Frame timeout";
      case ERROR_CODE::ERROR_NOT_SUPPORT:
        return "Not supported";
      case ERROR_CODE::ERROR_PROPERTY_GET_FAILED:
        return "Getting property failed";
      case ERROR_CODE::ERROR_PROPERTY_SET_FAILED:
        return "Setting property failed";
      default:
        break;
    }
    return "Success";
  }
}

//----------------------------------------------------------------------------
class vtkPlusRevopoint3DCamera::vtkInternal
{
public:
  vtkPlusRevopoint3DCamera* External;
  DepthStreamConfig DepthStream;
  PCLStreamConfig PCLStream;
  cs::ICameraPtr Camera;
  cs::IFramePtr DepthFrame;
  std::unique_ptr<float[]> PCLFrame;
  int AvailableStreamsFlag{0};

  vtkInternal(vtkPlusRevopoint3DCamera* external)
    : External(external)
  {
    this->Camera = cs::getCameraPtr();
  }

  PlusStatus CheckBasicConfig()
  {
    if (!this->DepthStream.Id.empty())
    {
      this->AvailableStreamsFlag |= RevoSourceType::DEPTH;
    }

    if (!this->PCLStream.Id.empty())
    {
      this->AvailableStreamsFlag |= RevoSourceType::PCL;
    }

    if ((this->AvailableStreamsFlag & RevoSourceType::PCL) && !(this->AvailableStreamsFlag & RevoSourceType::DEPTH))
    {
      LOG_ERROR("Invalid configuration: PCL stream is required but no Depth stream set");
      return PLUS_FAIL;
    }

    return PLUS_SUCCESS;
  }

  PlusStatus UpdateDepthStreamConfig()
  {
    std::vector<StreamInfo> streamProps;
    this->Camera->getStreamInfos(STREAM_TYPE::STREAM_TYPE_DEPTH, streamProps);

    bool foundMatchingConfig{false};
    for (auto streamProp : streamProps)
    {
      LOG_TRACE("New stream configuration for format: " << streamProp.format);
      if (streamProp.format == this->DepthStream.StreamProps.format)
      {
        LOG_INFO("New depth stream compliant configuration: " << streamProp.width << "x" << streamProp.height << "@" << streamProp.fps);
        if (this->DepthStream.StreamProps.width != streamProp.width)
        {
          continue;
        }

        if (this->DepthStream.StreamProps.fps > streamProp.fps)
        {
          this->DepthStream.StreamProps.fps = streamProp.fps;
          LOG_WARNING("Expected frame rate too high for current bandwidth. Actual framerate will be @" << streamProp.fps << " fps.");
        }

        this->DepthStream.StreamProps.height = streamProp.height;
        foundMatchingConfig = true;
        break;
      }
    }

    if (!foundMatchingConfig)
    {
      LOG_ERROR("No matching configuration found");
      LOG_ERROR("Available configurations:");
      for (auto streamProp : streamProps)
      {
        if (streamProp.format == this->DepthStream.StreamProps.format)
        {
          LOG_ERROR("- " << streamProp.width << "x" << streamProp.height << "@" << streamProp.fps);
        }
      }
      return PLUS_FAIL;
    }

    this->Camera->getIntrinsics(STREAM_TYPE::STREAM_TYPE_DEPTH, this->DepthStream.Intr);
    this->Camera->getExtrinsics(this->DepthStream.Extr);

    PropertyExtension value;
    value.depthRange.min = this->DepthStream.DepthRange[0];
    value.depthRange.max = this->DepthStream.DepthRange[1];
    value.algorithmContrast = 5;

    auto ret = this->Camera->setPropertyExtension(PROPERTY_TYPE_EXTENSION::PROPERTY_EXT_DEPTH_RANGE, value);
    if (ret != ERROR_CODE::SUCCESS)
    {
      LOG_ERROR("Failed to set camera depth range: " << convertErrorCode(ret));
      return PLUS_FAIL;
    }

    return PLUS_SUCCESS;
  }

  PlusStatus UpdatePCLStreamConfig()
  {
    this->PCLStream.StreamProps = this->DepthStream.StreamProps;
    return PLUS_SUCCESS;
  }

  PlusStatus UpdateConfig()
  {
    if ((this->AvailableStreamsFlag & RevoSourceType::DEPTH) && this->UpdateDepthStreamConfig() != PLUS_SUCCESS)
    {
      return PLUS_FAIL;
    }

    if ((this->AvailableStreamsFlag & RevoSourceType::PCL) && this->UpdatePCLStreamConfig() != PLUS_SUCCESS)
    {
      return PLUS_FAIL;
    }

    this->External->InternalUpdateRate = this->DepthStream.StreamProps.fps;
    this->External->AcquisitionRate = this->DepthStream.StreamProps.fps;

    return PLUS_SUCCESS;
  }

  PlusStatus Connect()
  {
    auto ret = this->Camera->connect();
    if (ret != ERROR_CODE::SUCCESS)
    {
      LOG_ERROR("Failed to connect camera: " << convertErrorCode(ret));
      return PLUS_FAIL;
    }

    return PLUS_SUCCESS;
  }

  PlusStatus Disconnect()
  {
    auto ret = this->Camera->disconnect();
    if (ret != ERROR_CODE::SUCCESS)
    {
      LOG_ERROR("Failed to disconnect camera: " << convertErrorCode(ret));
      return PLUS_FAIL;
    }

    return PLUS_SUCCESS;
  }

  PlusStatus Start()
  {
    auto ret = this->Camera->startStream(STREAM_TYPE::STREAM_TYPE_DEPTH, this->DepthStream.StreamProps);
    if (ret != ERROR_CODE::SUCCESS)
    {
      LOG_ERROR("Failed to start camera: " << convertErrorCode(ret));
      return PLUS_FAIL;
    }

    return PLUS_SUCCESS;
  }

  PlusStatus Stop()
  {
    auto ret = this->Camera->stopStream(STREAM_TYPE::STREAM_TYPE_DEPTH);
    if (ret != ERROR_CODE::SUCCESS)
    {
      LOG_ERROR("Failed to stop camera: " << convertErrorCode(ret));
      return PLUS_FAIL;
    }

    return PLUS_SUCCESS;
  }

  void Update()
  {
    if (this->AvailableStreamsFlag & RevoSourceType::DEPTH)
    {
      this->DepthFrame.reset();
      auto ret = this->Camera->getFrame(STREAM_TYPE::STREAM_TYPE_DEPTH, DepthFrame);
      if (ret != ERROR_CODE::SUCCESS)
      {
        LOG_ERROR("Failed to retrieve frame: " << convertErrorCode(ret));
      }
    }

    if ((this->AvailableStreamsFlag & RevoSourceType::PCL) && this->IsDepthFrameValid())
    {
      float scale = 0.1f;
      PropertyExtension value;
      if (SUCCESS ==
          this->Camera->getPropertyExtension(PROPERTY_EXT_DEPTH_SCALE, value))
      {
        scale = value.depthScale;
      }

      cs::Pointcloud pc;
      pc.generatePoints(reinterpret_cast<unsigned short*>(const_cast<char*>(this->DepthFrame->getData())),
                        this->DepthFrame->getWidth(), this->DepthFrame->getHeight(),
                        scale, &this->DepthStream.Intr, nullptr, &this->DepthStream.Extr);
      auto vertices = pc.getVertices();
      this->PCLFrame.reset(new float[vertices.size() * 3]);

      for (std::size_t idx = 0; idx < vertices.size(); ++idx)
      {
        this->PCLFrame[idx * 3] = vertices[idx].x;
        this->PCLFrame[idx * 3 + 1] = vertices[idx].y;
        this->PCLFrame[idx * 3 + 2] = vertices[idx].z;
      }
    }
  }

  bool IsDepthFrameValid() const
  {
    return this->DepthFrame && !(this->DepthFrame->empty()) && (this->DepthFrame->getWidth() == this->DepthStream.StreamProps.width) &&
           (this->DepthFrame->getHeight() == this->DepthStream.StreamProps.height);
  }
};

//----------------------------------------------------------------------------
vtkPlusRevopoint3DCamera::vtkPlusRevopoint3DCamera()
  : Internal(new vtkInternal(this))
{
  this->RequireImageOrientationInConfiguration = true;
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkPlusRevopoint3DCamera::~vtkPlusRevopoint3DCamera()
{
  delete this->Internal;
  this->Internal = nullptr;
}

//----------------------------------------------------------------------------
void vtkPlusRevopoint3DCamera::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "Revopoint Surface Configuration" << std::endl;

  if (this->Internal->AvailableStreamsFlag & RevoSourceType::DEPTH)
  {
    os << indent << "DEPTH Stream" << std::endl;
    os << indent << "Source: " << this->Internal->DepthStream.Id << std::endl;
    os << indent << indent << "Frame Rate: " << this->Internal->DepthStream.StreamProps.fps << std::endl;
    os << indent << indent << "Frame Width: " << this->Internal->DepthStream.StreamProps.width << std::endl;
    os << indent << indent << "Frame Height: " << this->Internal->DepthStream.StreamProps.height << std::endl;
    os << indent << indent << "Depth Range: [" << this->Internal->DepthStream.DepthRange[0] << "," << Internal->DepthStream.DepthRange[1] << "]" << std::endl;
  }

  if (this->Internal->AvailableStreamsFlag & RevoSourceType::PCL)
  {
    os << indent << "PCL Stream" << std::endl;
    os << indent << "Source: " << this->Internal->PCLStream.Id << std::endl;
  }
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusRevopoint3DCamera::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  XML_FIND_NESTED_ELEMENT_REQUIRED(dataSourcesElement, deviceConfig, "DataSources");

  LOG_TRACE("Reading custom configuration fields in " << dataSourcesElement->GetNumberOfNestedElements() << " nested elements");

  for (int nestedElementIndex = 0; nestedElementIndex < dataSourcesElement->GetNumberOfNestedElements(); ++nestedElementIndex)
  {
    vtkXMLDataElement* dataElement = dataSourcesElement->GetNestedElement(nestedElementIndex);
    if (std::string(dataElement->GetName()) != "DataSource")
    {
      continue;
    }

    LOG_TRACE("Found a new data source");
    if (dataElement->GetAttribute("Type") != nullptr && std::string(dataElement->GetAttribute("Type")) == "Video")
    {
      const char* toolId = dataElement->GetAttribute("Id");
      if (toolId == nullptr)
      {
        LOG_ERROR("Failed to initialize Revopoint Surface DataSource: Id is missing");
        return PLUS_FAIL;
      }

      LOG_TRACE("Data source name: " << toolId);

      RevoSourceType sourceType;
      XML_READ_ENUM2_ATTRIBUTE_NONMEMBER_REQUIRED(FrameType, sourceType, dataElement, "DEPTH", RevoSourceType::DEPTH, "PCL", RevoSourceType::PCL);

      if (sourceType == RevoSourceType::DEPTH)
      {
        DepthStreamConfig stream;
        stream.Id = toolId;
        stream.DepthRange[0] = 5;
        stream.DepthRange[1] = 500;
        XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_REQUIRED(int, FrameRate, stream.StreamProps.fps, dataElement);
        XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_REQUIRED(int, FrameWidth, stream.StreamProps.width, dataElement);
        XML_READ_VECTOR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, 2, DepthRange, stream.DepthRange, dataElement);
        stream.StreamProps.format = STREAM_FORMAT::STREAM_FORMAT_Z16;
        this->Internal->DepthStream = stream;
      }
      else
      {
        this->Internal->PCLStream.Id = toolId;
      }
    }
    else
    {
      LOG_ERROR("DataSource with unknown type.");
      return PLUS_FAIL;
    }
  }

  if (this->Internal->DepthStream.Id.empty())
  {
    LOG_ERROR("Required DEPTH type data source is missing.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusRevopoint3DCamera::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusRevopoint3DCamera::NotifyConfigured()
{
  return this->Internal->CheckBasicConfig();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusRevopoint3DCamera::InternalConnect()
{
  if (this->Internal->AvailableStreamsFlag & RevoSourceType::DEPTH)
  {
    GetVideoSource(this->Internal->DepthStream.Id.c_str(), this->Internal->DepthStream.Source);
    if (Internal->DepthStream.Source == nullptr)
    {
      LOG_ERROR("Invalid source for tool id " << this->Internal->DepthStream.Id);
      return PLUS_FAIL;
    }
  }

  if (this->Internal->AvailableStreamsFlag & RevoSourceType::PCL)
  {
    GetVideoSource(this->Internal->PCLStream.Id.c_str(), this->Internal->PCLStream.Source);
    if (Internal->PCLStream.Source == nullptr)
    {
      LOG_ERROR("Invalid source for tool id " << this->Internal->PCLStream.Id);
      return PLUS_FAIL;
    }
  }

  if (this->Internal->Connect() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  // Checking for configuration needs a connected camera
  return this->Internal->UpdateConfig();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusRevopoint3DCamera::InternalDisconnect()
{
  return this->Internal->Disconnect();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusRevopoint3DCamera::InternalStartRecording()
{
  this->FrameNumber = 0;

  return this->Internal->Start();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusRevopoint3DCamera::InternalStopRecording()
{
  return this->Internal->Stop();
}

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
PlusStatus vtkPlusRevopoint3DCamera::InternalUpdate()
{
  this->Internal->Update();

  if ((this->Internal->AvailableStreamsFlag & RevoSourceType::DEPTH) && this->Internal->DepthStream.Source->GetNumberOfItems() == 0)
  {
    // depth output is raw depth data
    LOG_TRACE("Setting up depth frame");
    this->Internal->DepthStream.Source->SetImageType(US_IMG_BRIGHTNESS);
    this->Internal->DepthStream.Source->SetPixelType(VTK_TYPE_UINT16);
    this->Internal->DepthStream.Source->SetNumberOfScalarComponents(1);
    this->Internal->DepthStream.Source->SetInputFrameSize(this->Internal->DepthStream.StreamProps.width, this->Internal->DepthStream.StreamProps.height, 1);
  }
  
  if ((this->Internal->AvailableStreamsFlag & RevoSourceType::PCL) && this->Internal->PCLStream.Source->GetNumberOfItems() == 0)
  {
    LOG_TRACE("Setting up pcl frame");
    this->Internal->PCLStream.Source->SetImageType(US_IMG_BRIGHTNESS);
    this->Internal->PCLStream.Source->SetPixelType(VTK_TYPE_FLOAT32);
    this->Internal->PCLStream.Source->SetNumberOfScalarComponents(3);
    this->Internal->PCLStream.Source->SetInputFrameSize(this->Internal->PCLStream.StreamProps.width, this->Internal->PCLStream.StreamProps.height, 1);
  }

  if (this->Internal->AvailableStreamsFlag & RevoSourceType::DEPTH)
  {
    if (!this->Internal->IsDepthFrameValid())
    {
      if (this->Internal->DepthStream.Source->GetNumberOfItems() == 0)
      {
        LOG_TRACE("Waiting for Revopoint Surface depth images");
      }
      else
      {
        LOG_WARNING("Failed to get Revopoint Surface depth image");
      }
      return PLUS_FAIL;
    }
    FrameSizeType frameSizeDepth = {static_cast<unsigned>(this->Internal->DepthStream.StreamProps.width), static_cast<unsigned>(this->Internal->DepthStream.StreamProps.height), 1};
    if (this->Internal->DepthStream.Source->AddItem(reinterpret_cast<void*>(const_cast<char*>(this->Internal->DepthFrame->getData(FRAME_DATA_FORMAT::FRAME_DATA_FORMAT_Z16))), this->Internal->DepthStream.Source->GetInputImageOrientation(), frameSizeDepth, VTK_TYPE_UINT16, 1, US_IMG_BRIGHTNESS, 0, this->FrameNumber) == PLUS_FAIL)
    {
      LOG_ERROR("Unable to send DEPTH image. Skipping frame.");
      return PLUS_FAIL;
    }
    this->Modified();
  }

  if (this->Internal->AvailableStreamsFlag & RevoSourceType::PCL)
  {
    FrameSizeType frameSizeDepth = {static_cast<unsigned>(this->Internal->PCLStream.StreamProps.width), static_cast<unsigned>(this->Internal->PCLStream.StreamProps.height), 1};
    if (this->Internal->PCLStream.Source->AddItem(reinterpret_cast<void*>(this->Internal->PCLFrame.get()), this->Internal->PCLStream.Source->GetInputImageOrientation(), frameSizeDepth, VTK_TYPE_FLOAT32, 3, US_IMG_BRIGHTNESS, 0, this->FrameNumber) == PLUS_FAIL)
    {
      LOG_ERROR("Unable to send PCL image. Skipping frame.");
      return PLUS_FAIL;
    }
    this->Modified();
  }

  this->FrameNumber++;
  return PLUS_SUCCESS;
}