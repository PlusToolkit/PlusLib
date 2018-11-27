/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "igtlPlusTrackedFrameMessage.h"
#include "vtkImageData.h"
#include "vtkMatrix4x4.h"
#include "vtkPlusIgtlMessageFactory.h"

namespace igtl
{
  //----------------------------------------------------------------------------
  PlusTrackedFrameMessage::PlusTrackedFrameMessage()
    : MessageBase()
  {
    this->m_SendMessageType = "TRACKEDFRAME";
  }

  //----------------------------------------------------------------------------
  PlusTrackedFrameMessage::~PlusTrackedFrameMessage()
  {
  }

  //----------------------------------------------------------------------------
  igtl::MessageBase::Pointer PlusTrackedFrameMessage::Clone()
  {
    igtl::MessageBase::Pointer clone;
    {
      vtkSmartPointer<vtkPlusIgtlMessageFactory> factory = vtkSmartPointer<vtkPlusIgtlMessageFactory>::New();
      clone = dynamic_cast<igtl::MessageBase*>(factory->CreateSendMessage(this->GetMessageType(), this->GetHeaderVersion()).GetPointer());
    }

    igtl::PlusTrackedFrameMessage::Pointer msg = dynamic_cast<igtl::PlusTrackedFrameMessage*>(clone.GetPointer());

    int bodySize = this->m_MessageSize - IGTL_HEADER_SIZE;
    msg->InitBuffer();
    msg->CopyHeader(this);
    msg->AllocateBuffer(bodySize);
    if (bodySize > 0)
    {
      msg->CopyBody(this);
    }

    return clone;
  }

  //----------------------------------------------------------------------------
  PlusStatus PlusTrackedFrameMessage::SetTrackedFrame(const igsioTrackedFrame& trackedFrame, const std::vector<igsioTransformName>& requestedTransforms)
  {
    this->m_TrackedFrame = trackedFrame;

    if (this->m_TrackedFrame.GetTrackedFrameInXmlData(this->m_TrackedFrameXmlData, requestedTransforms) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to pack Plus TrackedFrame message - unable to get tracked frame in xml data.");
      return PLUS_FAIL;
    }

    FrameSizeType frameSize = this->m_TrackedFrame.GetFrameSize();
    if (frameSize[0] > static_cast<unsigned int>(std::numeric_limits<igtl_uint16>::max()) ||
        frameSize[1] > static_cast<unsigned int>(std::numeric_limits<igtl_uint16>::max()) ||
        frameSize[2] > static_cast<unsigned int>(std::numeric_limits<igtl_uint16>::max()))
    {
      LOG_ERROR("Frame size element is too large to be sent over OpenIGTLink. Cannot set tracked frame.");
      return PLUS_FAIL;
    }

    this->m_MessageHeader.m_FrameSize[0] = frameSize[0];
    this->m_MessageHeader.m_FrameSize[1] = frameSize[1];
    this->m_MessageHeader.m_FrameSize[2] = frameSize[2];
    this->m_MessageHeader.m_XmlDataSizeInBytes = this->m_TrackedFrameXmlData.size();
    this->m_MessageHeader.m_ScalarType = PlusCommon::GetIGTLScalarPixelTypeFromVTK(this->m_TrackedFrame.GetImageData()->GetVTKScalarPixelType());

    unsigned int numberOfScalarComponents(1);
    if (m_TrackedFrame.GetImageData()->GetNumberOfScalarComponents(numberOfScalarComponents) == PLUS_FAIL)
    {
      LOG_ERROR("Unable to retrieve number of scalar components.");
      return PLUS_FAIL;
    }
    this->m_MessageHeader.m_NumberOfComponents = numberOfScalarComponents;
    this->m_MessageHeader.m_ImageType = m_TrackedFrame.GetImageData()->GetImageType();
    this->m_MessageHeader.m_ImageDataSizeInBytes = this->m_TrackedFrame.GetImageData()->GetFrameSizeInBytes();
    this->m_MessageHeader.m_ImageOrientation = (igtl_uint16)this->m_TrackedFrame.GetImageData()->GetImageOrientation();

    return PLUS_SUCCESS;
  }

  //----------------------------------------------------------------------------
  igsioTrackedFrame PlusTrackedFrameMessage::GetTrackedFrame()
  {
    return this->m_TrackedFrame;
  }

  //----------------------------------------------------------------------------
  PlusStatus PlusTrackedFrameMessage::SetEmbeddedImageTransform(vtkSmartPointer<vtkMatrix4x4> matrix)
  {
    for (int i = 0; i < 4; ++i)
    {
      for (int j = 0; j < 4; ++j)
      {
        m_MessageHeader.m_EmbeddedImageTransform[i][j] = matrix->GetElement(i, j);
      }
    }

    return PLUS_SUCCESS;
  }

  //----------------------------------------------------------------------------
  vtkSmartPointer<vtkMatrix4x4> PlusTrackedFrameMessage::GetEmbeddedImageTransform()
  {
    vtkSmartPointer<vtkMatrix4x4> mat(vtkSmartPointer<vtkMatrix4x4>::New());
    for (int i = 0; i < 4; ++i)
    {
      for (int j = 0; j < 4; ++j)
      {
        mat->SetElement(i, j, m_MessageHeader.m_EmbeddedImageTransform[i][j]);
      }
    }
    return mat;
  }

  //----------------------------------------------------------------------------
  int PlusTrackedFrameMessage::CalculateContentBufferSize()
  {
    return this->m_MessageHeader.GetMessageHeaderSize()
           + this->m_MessageHeader.m_ImageDataSizeInBytes
           + this->m_MessageHeader.m_XmlDataSizeInBytes;
  }

  //----------------------------------------------------------------------------
  int PlusTrackedFrameMessage::PackContent()
  {
    AllocateBuffer();

    // Copy header
    TrackedFrameHeader* header = (TrackedFrameHeader*)(this->m_Content);
    header->m_ScalarType = this->m_MessageHeader.m_ScalarType;
    header->m_NumberOfComponents = this->m_MessageHeader.m_NumberOfComponents;
    header->m_ImageType = this->m_MessageHeader.m_ImageType;
    header->m_FrameSize[0] = this->m_MessageHeader.m_FrameSize[0];
    header->m_FrameSize[1] = this->m_MessageHeader.m_FrameSize[1];
    header->m_FrameSize[2] = this->m_MessageHeader.m_FrameSize[2];
    header->m_ImageDataSizeInBytes = this->m_MessageHeader.m_ImageDataSizeInBytes;
    header->m_XmlDataSizeInBytes = this->m_MessageHeader.m_XmlDataSizeInBytes;
    header->m_ImageOrientation = this->m_MessageHeader.m_ImageOrientation;
    memcpy(header->m_EmbeddedImageTransform, this->m_MessageHeader.m_EmbeddedImageTransform, sizeof(igtl::Matrix4x4));

    // Copy xml data
    char* xmlData = (char*)(this->m_Content + header->GetMessageHeaderSize());
    strncpy(xmlData, this->m_TrackedFrameXmlData.c_str(), this->m_TrackedFrameXmlData.size());
    header->m_XmlDataSizeInBytes = this->m_MessageHeader.m_XmlDataSizeInBytes;

    // Copy image data
    void* imageData = (void*)(this->m_Content + header->GetMessageHeaderSize() + header->m_XmlDataSizeInBytes);
    memcpy(imageData, this->m_TrackedFrame.GetImageData()->GetScalarPointer(), this->m_TrackedFrame.GetImageData()->GetFrameSizeInBytes());

    // Set timestamp
    igtl::TimeStamp::Pointer timestamp = igtl::TimeStamp::New();
    timestamp->SetTime(this->m_TrackedFrame.GetTimestamp());
    this->SetTimeStamp(timestamp);

    // Convert header endian
    header->ConvertEndianness();

    return 1;
  }

  //----------------------------------------------------------------------------
  int PlusTrackedFrameMessage::UnpackContent()
  {
    TrackedFrameHeader* header = (TrackedFrameHeader*)(this->m_Content);

    // Convert header endian
    header->ConvertEndianness();

    // Copy header
    this->m_MessageHeader.m_ScalarType = header->m_ScalarType;
    this->m_MessageHeader.m_NumberOfComponents = header->m_NumberOfComponents;
    this->m_MessageHeader.m_ImageType = header->m_ImageType;
    this->m_MessageHeader.m_FrameSize[0] = header->m_FrameSize[0];
    this->m_MessageHeader.m_FrameSize[1] = header->m_FrameSize[1];
    this->m_MessageHeader.m_FrameSize[2] = header->m_FrameSize[2];
    this->m_MessageHeader.m_ImageDataSizeInBytes = header->m_ImageDataSizeInBytes;
    this->m_MessageHeader.m_XmlDataSizeInBytes = header->m_XmlDataSizeInBytes;
    this->m_MessageHeader.m_ImageOrientation = header->m_ImageOrientation;
    memcpy(this->m_MessageHeader.m_EmbeddedImageTransform, header->m_EmbeddedImageTransform, sizeof(igtl::Matrix4x4));

    // Copy xml data
    char* xmlData = (char*)(this->m_Content + header->GetMessageHeaderSize());
    this->m_TrackedFrameXmlData.assign(xmlData, header->m_XmlDataSizeInBytes);
    if (this->m_TrackedFrame.SetTrackedFrameFromXmlData(this->m_TrackedFrameXmlData) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set tracked frame data from xml received in Plus TrackedFrame message");
      return 0;
    }

    // Copy image data
    void* imageData = (void*)(this->m_Content + header->GetMessageHeaderSize() + header->m_XmlDataSizeInBytes);
    FrameSizeType frameSize = { header->m_FrameSize[0], header->m_FrameSize[1], header->m_FrameSize[2] };
    if (this->m_TrackedFrame.GetImageData()->AllocateFrame(frameSize, PlusCommon::GetVTKScalarPixelTypeFromIGTL(header->m_ScalarType), header->m_NumberOfComponents) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to allocate memory for frame received in Plus TrackedFrame message");
      return 0;
    }

    // Carry the image type forward
    m_TrackedFrame.GetImageData()->SetImageType((US_IMAGE_TYPE)header->m_ImageType);

    memcpy(this->m_TrackedFrame.GetImageData()->GetScalarPointer(), imageData, header->m_ImageDataSizeInBytes);
    m_TrackedFrame.GetImageData()->GetImage()->Modified();

    // Set timestamp
    igtl::TimeStamp::Pointer timestamp = igtl::TimeStamp::New();
    this->GetTimeStamp(timestamp);
    this->m_TrackedFrame.SetTimestamp(timestamp->GetTimeStamp());

    return 1;
  }
}