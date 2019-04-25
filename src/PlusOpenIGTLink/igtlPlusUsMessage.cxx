/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "igsioTrackedFrame.h"
#include "igtlPlusUsMessage.h"
#include "igtl_image.h"
#include "igtl_header.h"
#include "igtl_util.h"
#include "vtkPlusIgtlMessageFactory.h"

namespace igtl
{

  //----------------------------------------------------------------------------
  size_t PlusUsMessage::MessageHeader::GetMessageHeaderSize()
  {
    size_t headersize = 13 * sizeof(igtl_uint32);
    return headersize;
  }

  //----------------------------------------------------------------------------
  void PlusUsMessage::MessageHeader::ConvertEndianness()
  {
    if (igtl_is_little_endian())
    {
      m_DataType = BYTE_SWAP_INT32(m_DataType);
      m_TransmitFrequency = BYTE_SWAP_INT32(m_TransmitFrequency);
      m_SamplingFrequency = BYTE_SWAP_INT32(m_SamplingFrequency);
      m_DataRate = BYTE_SWAP_INT32(m_DataRate);
      m_LineDensity = BYTE_SWAP_INT32(m_LineDensity);

      m_SteeringAngle = BYTE_SWAP_INT32(m_SteeringAngle);
      m_ProbeID = BYTE_SWAP_INT32(m_ProbeID);
      m_ExtensionAngle = BYTE_SWAP_INT32(m_ExtensionAngle);
      m_Elements = BYTE_SWAP_INT32(m_Elements);
      m_Pitch = BYTE_SWAP_INT32(m_Pitch);

      m_Radius = BYTE_SWAP_INT32(m_Radius);
      m_ProbeAngle = BYTE_SWAP_INT32(m_ProbeAngle);
      m_TxOffset = BYTE_SWAP_INT32(m_TxOffset);
    }
  }

  //----------------------------------------------------------------------------
  PlusUsMessage::PlusUsMessage()
    : ImageMessage()
  {
    this->m_SendMessageType = "USMESSAGE";
  }

  //----------------------------------------------------------------------------
  PlusUsMessage::~PlusUsMessage()
  {
  }

  //----------------------------------------------------------------------------
  igtl::MessageBase::Pointer PlusUsMessage::Clone()
  {
    igtl::MessageBase::Pointer clone;
    {
      vtkSmartPointer<vtkPlusIgtlMessageFactory> factory = vtkSmartPointer<vtkPlusIgtlMessageFactory>::New();
      clone = dynamic_cast<igtl::MessageBase*>(factory->CreateSendMessage(this->GetMessageType(), this->GetHeaderVersion()).GetPointer());
    }

    igtl::PlusUsMessage::Pointer msg = dynamic_cast<igtl::PlusUsMessage*>(clone.GetPointer());

    int bodySize = this->m_MessageSize - IGTL_HEADER_SIZE;
    msg->InitBuffer();
    msg->CopyHeader(this);
    msg->AllocateBuffer(bodySize);
    if (bodySize > 0)
    {
      msg->CopyBody(this);
    }

#if OpenIGTLink_HEADER_VERSION >= 2
    msg->m_MetaDataHeader = this->m_MetaDataHeader;
    msg->m_MetaDataMap = this->m_MetaDataMap;
    msg->m_IsExtendedHeaderUnpacked = this->m_IsExtendedHeaderUnpacked;
#endif

    return clone;
  }

  //----------------------------------------------------------------------------
  igsioTrackedFrame& PlusUsMessage::GetTrackedFrame()
  {
    return this->m_TrackedFrame;
  }

  //----------------------------------------------------------------------------
  PlusStatus PlusUsMessage::SetTrackedFrame(const igsioTrackedFrame& trackedFrame)
  {
    this->m_TrackedFrame = trackedFrame;

    double timestamp = this->m_TrackedFrame.GetTimestamp();

    igtl::TimeStamp::Pointer igtlFrameTime = igtl::TimeStamp::New();
    igtlFrameTime->SetTime(timestamp);

    std::array<int, 3> offset = {0};
    FrameSizeType imageSizePixels = {0};

    // NOTE: MUSiiC library expects the frame size in the format
    // as Ultrasonix provide, not like Plus (Plus: if vector data switch width and
    // height, because the image is not rasterized like a bitmap, but written rayline by rayline)
    FrameSizeType size = this->m_TrackedFrame.GetFrameSize();
    imageSizePixels[0] = size[1];
    imageSizePixels[1] = size[0];
    imageSizePixels[2] = 1;

    int scalarType = PlusCommon::GetIGTLScalarPixelTypeFromVTK(this->m_TrackedFrame.GetImageData()->GetVTKScalarPixelType());

    this->SetDimensions(static_cast<int>(imageSizePixels[0]), static_cast<int>(imageSizePixels[1]), static_cast<int>(imageSizePixels[2]));
    this->SetSubVolume(static_cast<int>(imageSizePixels[0]), static_cast<int>(imageSizePixels[1]), static_cast<int>(imageSizePixels[2]), offset[0], offset[1], offset[2]);
    this->SetScalarType(scalarType);
    this->SetSpacing(0.2, 0.2, 1);
    this->AllocateScalars();

    unsigned char* igtlImagePointer = (unsigned char*)(this->GetScalarPointer());
    unsigned char* plusImagePointer = (unsigned char*)(this->m_TrackedFrame.GetImageData()->GetScalarPointer());

    memcpy(igtlImagePointer, plusImagePointer, this->GetImageSize());

    this->SetTimeStamp(igtlFrameTime);

    this->m_MessageHeader.m_DataType = 0;
    if (this->m_TrackedFrame.IsFrameFieldDefined("SonixDataType"))
    {
      std::string fieldValue = this->m_TrackedFrame.GetFrameField("SonixDataType");
      igsioCommon::StringToNumber<igtl_int32>(fieldValue, this->m_MessageHeader.m_DataType);
    }

    this->m_MessageHeader.m_TransmitFrequency = 0;
    if (this->m_TrackedFrame.IsFrameFieldDefined("SonixTransmitFrequency"))
    {
      std::string fieldValue = this->m_TrackedFrame.GetFrameField("SonixTransmitFrequency");
      igsioCommon::StringToNumber<igtl_int32>(fieldValue, this->m_MessageHeader.m_TransmitFrequency);
    }

    this->m_MessageHeader.m_SamplingFrequency = 0;
    if (this->m_TrackedFrame.IsFrameFieldDefined("SonixSamplingFrequency"))
    {
      std::string fieldValue = this->m_TrackedFrame.GetFrameField("SonixSamplingFrequency");
      igsioCommon::StringToNumber<igtl_int32>(fieldValue, this->m_MessageHeader.m_SamplingFrequency);
    }

    this->m_MessageHeader.m_DataRate = 0;
    if (this->m_TrackedFrame.IsFrameFieldDefined("SonixDataRate"))
    {
      std::string fieldValue = this->m_TrackedFrame.GetFrameField("SonixDataRate");
      igsioCommon::StringToNumber<igtl_int32>(fieldValue, this->m_MessageHeader.m_DataRate);
    }

    this->m_MessageHeader.m_LineDensity = 0;
    if (this->m_TrackedFrame.IsFrameFieldDefined("SonixLineDensity"))
    {
      std::string fieldValue = this->m_TrackedFrame.GetFrameField("SonixLineDensity");
      igsioCommon::StringToNumber<igtl_int32>(fieldValue, this->m_MessageHeader.m_LineDensity);
    }

    this->m_MessageHeader.m_SteeringAngle = 0;
    if (this->m_TrackedFrame.IsFrameFieldDefined("SonixSteeringAngle"))
    {
      std::string fieldValue = this->m_TrackedFrame.GetFrameField("SonixSteeringAngle");
      igsioCommon::StringToNumber<igtl_int32>(fieldValue, this->m_MessageHeader.m_SteeringAngle);
    }

    this->m_MessageHeader.m_ProbeID = 0;
    if (this->m_TrackedFrame.IsFrameFieldDefined("SonixProbeID"))
    {
      std::string fieldValue = this->m_TrackedFrame.GetFrameField("SonixProbeID");
      igsioCommon::StringToNumber<igtl_int32>(fieldValue, this->m_MessageHeader.m_ProbeID);
    }

    this->m_MessageHeader.m_ExtensionAngle = 0;
    if (this->m_TrackedFrame.IsFrameFieldDefined("SonixExtensionAngle"))
    {
      std::string fieldValue = this->m_TrackedFrame.GetFrameField("SonixExtensionAngle");
      igsioCommon::StringToNumber<igtl_int32>(fieldValue, this->m_MessageHeader.m_ExtensionAngle);
    }

    this->m_MessageHeader.m_Elements = 0;
    if (this->m_TrackedFrame.IsFrameFieldDefined("SonixElements"))
    {
      std::string fieldValue = this->m_TrackedFrame.GetFrameField("SonixElements");
      igsioCommon::StringToNumber<igtl_int32>(fieldValue, this->m_MessageHeader.m_Elements);
    }

    this->m_MessageHeader.m_Pitch = 0;
    if (this->m_TrackedFrame.IsFrameFieldDefined("SonixPitch"))
    {
      std::string fieldValue = this->m_TrackedFrame.GetFrameField("SonixPitch");
      igsioCommon::StringToNumber<igtl_int32>(fieldValue, this->m_MessageHeader.m_Pitch);
    }

    this->m_MessageHeader.m_Radius = 0;
    if (this->m_TrackedFrame.IsFrameFieldDefined("SonixRadius"))
    {
      std::string fieldValue = this->m_TrackedFrame.GetFrameField("SonixRadius");
      igsioCommon::StringToNumber<igtl_int32>(fieldValue, this->m_MessageHeader.m_Radius);
    }

    this->m_MessageHeader.m_ProbeAngle = 0;
    if (this->m_TrackedFrame.IsFrameFieldDefined("SonixProbeAngle"))
    {
      std::string fieldValue = this->m_TrackedFrame.GetFrameField("SonixProbeAngle");
      igsioCommon::StringToNumber<igtl_int32>(fieldValue, this->m_MessageHeader.m_ProbeAngle);
    }

    this->m_MessageHeader.m_TxOffset = 0;
    if (this->m_TrackedFrame.IsFrameFieldDefined("SonixTxOffset"))
    {
      std::string fieldValue = this->m_TrackedFrame.GetFrameField("SonixTxOffset");
      igsioCommon::StringToNumber<igtl_int32>(fieldValue, this->m_MessageHeader.m_TxOffset);
    }

    return PLUS_SUCCESS;
  }

  //----------------------------------------------------------------------------
  int PlusUsMessage::CalculateContentBufferSize()
  {
    return GetSubVolumeImageSize() + IGTL_IMAGE_HEADER_SIZE + this->m_MessageHeader.GetMessageHeaderSize();
  }

  //----------------------------------------------------------------------------
  int PlusUsMessage::PackContent()
  {
    igtl::ImageMessage::PackContent();

    MessageHeader* header = (MessageHeader*)(m_Image + GetSubVolumeImageSize());
    header->m_DataType = this->m_MessageHeader.m_DataType;
    header->m_TransmitFrequency = this->m_MessageHeader.m_TransmitFrequency;
    header->m_SamplingFrequency = this->m_MessageHeader.m_SamplingFrequency;
    header->m_DataRate = this->m_MessageHeader.m_DataRate;
    header->m_LineDensity = this->m_MessageHeader.m_LineDensity;

    header->m_SteeringAngle = this->m_MessageHeader.m_SteeringAngle;
    header->m_ProbeID = this->m_MessageHeader.m_ProbeID;
    header->m_ExtensionAngle = this->m_MessageHeader.m_ExtensionAngle;
    header->m_Elements = this->m_MessageHeader.m_Elements;
    header->m_Pitch = this->m_MessageHeader.m_Pitch;

    header->m_Radius = this->m_MessageHeader.m_Radius;
    header->m_ProbeAngle = this->m_MessageHeader.m_ProbeAngle;
    header->m_TxOffset = this->m_MessageHeader.m_TxOffset;

    // Convert header endian
    header->ConvertEndianness();

    return 1;
  }

  //----------------------------------------------------------------------------
  int PlusUsMessage::UnpackContent()
  {
    igtl::ImageMessage::UnpackContent();

    MessageHeader* header = (MessageHeader*)(m_Image + GetSubVolumeImageSize());

    // Convert header endian
    header->ConvertEndianness();

    this->m_MessageHeader.m_DataType = header->m_DataType;
    this->m_MessageHeader.m_TransmitFrequency = header->m_TransmitFrequency;
    this->m_MessageHeader.m_SamplingFrequency = header->m_SamplingFrequency;
    this->m_MessageHeader.m_DataRate = header->m_DataRate;
    this->m_MessageHeader.m_LineDensity = header->m_LineDensity;

    this->m_MessageHeader.m_SteeringAngle = header->m_SteeringAngle;
    this->m_MessageHeader.m_ProbeID = header->m_ProbeID;
    this->m_MessageHeader.m_ExtensionAngle = header->m_ExtensionAngle;
    this->m_MessageHeader.m_Elements = header->m_Elements;
    this->m_MessageHeader.m_Pitch = header->m_Pitch;

    this->m_MessageHeader.m_Radius = header->m_Radius;
    this->m_MessageHeader.m_ProbeAngle = header->m_ProbeAngle;
    this->m_MessageHeader.m_TxOffset = header->m_TxOffset;

    return 1;
  }
} //namespace igtl