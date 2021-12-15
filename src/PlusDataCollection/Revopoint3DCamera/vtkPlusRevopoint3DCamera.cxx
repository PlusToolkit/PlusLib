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
struct SurfaceStreamConfig
{
    std::string Id;
    StreamInfo StreamProps;
    int DepthRange[2];
    Intrinsics Intr;
    Extrinsics Extr;
    vtkPlusDataSource* Source{nullptr};
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
    SurfaceStreamConfig DepthStream;
    cs::ICameraPtr Camera;
    cs::IFramePtr DepthFrame;

    vtkInternal(vtkPlusRevopoint3DCamera* external)
        : External(external)
    {
        this->Camera = cs::getCameraPtr();
    }

    PlusStatus UpdateConfig()
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

        ret = this->Camera->setPropertyExtension(PROPERTY_TYPE_EXTENSION::PROPERTY_EXT_CONTRAST_MIN, value);
        if (ret != ERROR_CODE::SUCCESS)
        {
            LOG_ERROR("Failed to set camera internal parameters: " << convertErrorCode(ret));
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
        this->DepthFrame.reset();
        auto ret = this->Camera->getFrame(STREAM_TYPE::STREAM_TYPE_DEPTH, DepthFrame);
        if (ret != ERROR_CODE::SUCCESS)
        {
            LOG_ERROR("Failed to retrieve frame: " << convertErrorCode(ret));
        }
    }

    bool IsFrameValid() const
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
    os << indent << "Source: " << this->Internal->DepthStream.Id << std::endl;
    os << indent << indent << "Frame Rate: " << this->Internal->DepthStream.StreamProps.fps << std::endl;
    os << indent << indent << "Frame Width: " << this->Internal->DepthStream.StreamProps.width << std::endl;
    os << indent << indent << "Frame Height: " << this->Internal->DepthStream.StreamProps.height << std::endl;
    os << indent << indent << "Depth Range: [" << this->Internal->DepthStream.DepthRange[0] << "," << Internal->DepthStream.DepthRange[1] << "]" << std::endl;
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

            SurfaceStreamConfig stream;
            stream.Id = toolId;
            stream.DepthRange[0] = 5;
            stream.DepthRange[1] = 500;
            XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_REQUIRED(int, FrameRate, stream.StreamProps.fps, dataElement);
            XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_REQUIRED(int, FrameWidth, stream.StreamProps.width, dataElement);
            XML_READ_VECTOR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, 2, DepthRange, stream.DepthRange, dataElement);

            stream.StreamProps.format = STREAM_FORMAT::STREAM_FORMAT_Z16;
            this->Internal->DepthStream = stream;
            break;
        }
        else
        {
            LOG_ERROR("DataSource with unknown type.");
            return PLUS_FAIL;
        }
    }

    if (this->Internal->DepthStream.Id.empty())
    {
        LOG_ERROR("No DataSource configured.");
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
PlusStatus vtkPlusRevopoint3DCamera::InternalConnect()
{
    GetVideoSource(this->Internal->DepthStream.Id.c_str(), this->Internal->DepthStream.Source);
    if (Internal->DepthStream.Source == nullptr)
    {
        LOG_ERROR("Invalid source for tool id " << this->Internal->DepthStream.Id);
        return PLUS_FAIL;
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

    if (Internal->DepthStream.Source->GetNumberOfItems() == 0)
    {
        // depth output is raw depth data
        LOG_TRACE("Setting up depth frame");
        this->Internal->DepthStream.Source->SetImageType(US_IMG_BRIGHTNESS);
        this->Internal->DepthStream.Source->SetPixelType(VTK_TYPE_UINT16);
        this->Internal->DepthStream.Source->SetNumberOfScalarComponents(1);
        this->Internal->DepthStream.Source->SetInputFrameSize(this->Internal->DepthStream.StreamProps.width, this->Internal->DepthStream.StreamProps.height, 1);
    }

    if (!this->Internal->IsFrameValid())
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
    if (this->Internal->DepthStream.Source->AddItem((void*)this->Internal->DepthFrame->getData(FRAME_DATA_FORMAT::FRAME_DATA_FORMAT_Z16), this->Internal->DepthStream.Source->GetInputImageOrientation(), frameSizeDepth, VTK_TYPE_UINT16, 1, US_IMG_BRIGHTNESS, 0, this->FrameNumber) == PLUS_FAIL)
    {
        LOG_ERROR("Unable to send DEPTH image. Skipping frame.");
        return PLUS_FAIL;
    }
    this->Modified();

    this->FrameNumber++;
    return PLUS_SUCCESS;
}