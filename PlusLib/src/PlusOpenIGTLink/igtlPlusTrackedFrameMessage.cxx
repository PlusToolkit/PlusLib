/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "igtlPlusTrackedFrameMessage.h"
#include "vtkImageData.h"
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
  igtl::PlusTrackedFrameMessage::Pointer clone;
  {
    vtkSmartPointer<vtkPlusIgtlMessageFactory> factory = vtkSmartPointer<vtkPlusIgtlMessageFactory>::New();
    clone = dynamic_cast<igtl::PlusTrackedFrameMessage*>(factory->CreateSendMessage(this->GetMessageType(), this->GetHeaderVersion()).GetPointer());
  }

  int bodySize = this->m_MessageSize - IGTL_HEADER_SIZE;
  clone->InitBuffer();
  clone->CopyHeader(this);
  clone->AllocateBuffer(bodySize);
  if (bodySize > 0)
  {
    clone->CopyBody(this);
  }

#if OpenIGTLink_HEADER_VERSION >= 2
  clone->m_MetaDataHeader = this->m_MetaDataHeader;
  clone->m_MetaDataMap = this->m_MetaDataMap;
  clone->m_IsExtendedHeaderUnpacked = this->m_IsExtendedHeaderUnpacked;
#endif

  return clone;
}

//----------------------------------------------------------------------------
PlusStatus PlusTrackedFrameMessage::SetTrackedFrame( const PlusTrackedFrame& trackedFrame )
{
  this->m_TrackedFrame = trackedFrame;

  if ( this->m_TrackedFrame.GetTrackedFrameInXmlData(this->m_TrackedFrameXmlData) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to pack Plus TrackedFrame message - unable to get tracked frame in xml data." );
    return PLUS_FAIL;
  }

  // Frame size
  this->m_MessageHeader.m_FrameSize[0] = this->m_TrackedFrame.GetFrameSize()[0];
  this->m_MessageHeader.m_FrameSize[1] = this->m_TrackedFrame.GetFrameSize()[1];
  this->m_MessageHeader.m_FrameSize[2] = this->m_TrackedFrame.GetFrameSize()[2];
  // XML data size
  this->m_MessageHeader.m_XmlDataSizeInBytes = this->m_TrackedFrameXmlData.size();
  // Pixel type
  this->m_MessageHeader.m_ScalarType = PlusVideoFrame::GetIGTLScalarPixelTypeFromVTK( this->m_TrackedFrame.GetImageData()->GetVTKScalarPixelType() );
  // Number of scalar components
  this->m_MessageHeader.m_NumberOfComponents = m_TrackedFrame.GetImageData()->GetNumberOfScalarComponents();
  // Image type
  this->m_MessageHeader.m_ImageType = m_TrackedFrame.GetImageData()->GetImageType();
  // Image data size
  this->m_MessageHeader.m_ImageDataSizeInBytes = this->m_TrackedFrame.GetImageData()->GetFrameSizeInBytes();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusTrackedFrame PlusTrackedFrameMessage::GetTrackedFrame()
{
  return this->m_TrackedFrame;
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

  // Copy xml data
  char* xmlData = (char*)( this->m_Content + header->GetMessageHeaderSize() );
  strncpy( xmlData, this->m_TrackedFrameXmlData.c_str(), this->m_TrackedFrameXmlData.size() );
  header->m_XmlDataSizeInBytes = this->m_MessageHeader.m_XmlDataSizeInBytes;

  // Copy image data
  void* imageData = (void*)(this->m_Content + header->GetMessageHeaderSize() + header->m_XmlDataSizeInBytes);
  memcpy( imageData, this->m_TrackedFrame.GetImageData()->GetScalarPointer(), this->m_TrackedFrame.GetImageData()->GetFrameSizeInBytes());

  // Set timestamp
  igtl::TimeStamp::Pointer timestamp = igtl::TimeStamp::New();
  timestamp->SetTime( this->m_TrackedFrame.GetTimestamp() );
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

  // Copy xml data
  char* xmlData = (char*)(this->m_Content + header->GetMessageHeaderSize());
  this->m_TrackedFrameXmlData.assign(xmlData, header->m_XmlDataSizeInBytes );
  if ( this->m_TrackedFrame.SetTrackedFrameFromXmlData(this->m_TrackedFrameXmlData) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set tracked frame data from xml received in Plus TrackedFrame message");
    return 0;
  }

  // Copy image data
  void* imageData = (void*)(this->m_Content + header->GetMessageHeaderSize() + header->m_XmlDataSizeInBytes);
  int frameSize[3] = { header->m_FrameSize[0], header->m_FrameSize[1], header->m_FrameSize[2] };
  if ( this->m_TrackedFrame.GetImageData()->AllocateFrame( frameSize, PlusVideoFrame::GetVTKScalarPixelTypeFromIGTL(header->m_ScalarType), header->m_NumberOfComponents ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to allocate memory for frame received in Plus TrackedFrame message");
    return 0;
  }

  // Carry the image type forward
  m_TrackedFrame.GetImageData()->SetImageType( (US_IMAGE_TYPE)header->m_ImageType );

  memcpy( this->m_TrackedFrame.GetImageData()->GetScalarPointer(), imageData, header->m_ImageDataSizeInBytes );
  m_TrackedFrame.GetImageData()->GetImage()->Modified();

  // Set timestamp
  igtl::TimeStamp::Pointer timestamp = igtl::TimeStamp::New();
  this->GetTimeStamp(timestamp);
  this->m_TrackedFrame.SetTimestamp(timestamp->GetTimeStamp());

  return 1;
}

}