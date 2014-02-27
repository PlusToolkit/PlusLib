/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"

#include "igtlPlusTrackedFrameMessage.h"

namespace igtl 
{

  //----------------------------------------------------------------------------
  PlusTrackedFrameMessage::PlusTrackedFrameMessage() : MessageBase()
  {
    this->m_DefaultBodyType = "TRACKEDFRAME";
  }

  //----------------------------------------------------------------------------
  PlusTrackedFrameMessage::~PlusTrackedFrameMessage()
  {
  }

  //----------------------------------------------------------------------------
  PlusStatus PlusTrackedFrameMessage::SetTrackedFrame( const TrackedFrame& trackedFrame ) 
  {
    this->m_TrackedFrame = trackedFrame; 

    if ( this->m_TrackedFrame.GetTrackedFrameInXmlData(this->m_TrackedFrameXmlData) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to pack Plus TrackedFrame message - unable to get tracked frame in xml data." ); 
      return PLUS_FAIL; 
    }

    // Version 
    this->m_MessageHeader.m_Version = IGTL_HEADER_VERSION; 
    // Frame size 
    this->m_MessageHeader.m_FrameSize[0] = this->m_TrackedFrame.GetFrameSize()[0];
    this->m_MessageHeader.m_FrameSize[1] = this->m_TrackedFrame.GetFrameSize()[1];
    // XML data size 
    this->m_MessageHeader.m_XmlDataSizeInBytes = this->m_TrackedFrameXmlData.size(); 
    // Pixel type 
    this->m_MessageHeader.m_ScalarType = PlusVideoFrame::GetIGTLScalarPixelTypeFromVTK( this->m_TrackedFrame.GetImageData()->GetVTKScalarPixelType() ); 
    // Image data size 
    this->m_MessageHeader.m_ImageDataSizeInBytes = this->m_TrackedFrame.GetImageData()->GetFrameSizeInBytes(); 

    return PLUS_SUCCESS; 

  }

  //----------------------------------------------------------------------------
  TrackedFrame PlusTrackedFrameMessage::GetTrackedFrame()
  {
    return this->m_TrackedFrame;
  }

  //----------------------------------------------------------------------------
  int PlusTrackedFrameMessage::GetBodyPackSize()
  {
    return this->m_MessageHeader.GetMessageHeaderSize() 
      + this->m_MessageHeader.m_ImageDataSizeInBytes 
      + this->m_MessageHeader.m_XmlDataSizeInBytes; 
  }

  //----------------------------------------------------------------------------
  int PlusTrackedFrameMessage::PackBody()
  {
    AllocatePack();

    // Copy header
    MessageHeader* header = (MessageHeader*)( this->m_Body );
    header->m_Version = this->m_MessageHeader.m_Version; 
    header->m_FrameSize[0] = this->m_MessageHeader.m_FrameSize[0]; 
    header->m_FrameSize[1] = this->m_MessageHeader.m_FrameSize[1];
    header->m_ScalarType = this->m_MessageHeader.m_ScalarType; 
    header->m_XmlDataSizeInBytes = this->m_MessageHeader.m_XmlDataSizeInBytes; 
    header->m_ImageDataSizeInBytes = this->m_MessageHeader.m_ImageDataSizeInBytes; 

    // Copy xml data 
    char* xmlData = (char*)( this->m_Body + header->GetMessageHeaderSize() );
    strncpy( xmlData, this->m_TrackedFrameXmlData.c_str(), this->m_TrackedFrameXmlData.size() );

    // Copy image data 
    void* imageData = (void*)( this->m_Body + header->GetMessageHeaderSize() + header->m_XmlDataSizeInBytes ); 
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
  int PlusTrackedFrameMessage::UnpackBody()
  {
    MessageHeader* header = (MessageHeader*)( this->m_Body );

    // Convert header endian
    header->ConvertEndianness(); 

    // Copy header
    this->m_MessageHeader.m_Version = header->m_Version; 
    this->m_MessageHeader.m_FrameSize[0] = header->m_FrameSize[0]; 
    this->m_MessageHeader.m_FrameSize[1] = header->m_FrameSize[1];
    this->m_MessageHeader.m_ScalarType = header->m_ScalarType; 
    this->m_MessageHeader.m_XmlDataSizeInBytes = header->m_XmlDataSizeInBytes; 
    this->m_MessageHeader.m_ImageDataSizeInBytes = header->m_ImageDataSizeInBytes; 

    // Copy xml data 
    char* xmlData = (char*)( this->m_Body + header->GetMessageHeaderSize() );
    this->m_TrackedFrameXmlData.assign(xmlData, header->m_XmlDataSizeInBytes ); 
    if ( this->m_TrackedFrame.SetTrackedFrameFromXmlData(xmlData) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to set tracked frame data from xml received in Plus TrackedFrame message"); 
      return 0;
    }

    // Copy image data 
    void* imageData = (void*)( this->m_Body + header->GetMessageHeaderSize() + header->m_XmlDataSizeInBytes ); 
    int frameSize[2] = { header->m_FrameSize[0], header->m_FrameSize[1] };
    // greyscale images only, so set number of components to 1
    if ( this->m_TrackedFrame.GetImageData()->AllocateFrame( frameSize, PlusVideoFrame::GetVTKScalarPixelTypeFromIGTL(header->m_ScalarType),1 ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to allocate memory for frame received in Plus TrackedFrame message"); 
      return 0; 
    }

    memcpy( this->m_TrackedFrame.GetImageData()->GetScalarPointer(), imageData, header->m_ImageDataSizeInBytes ); 

    // Set timestamp 
    igtl::TimeStamp::Pointer timestamp = igtl::TimeStamp::New(); 
    this->GetTimeStamp(timestamp); 
    this->m_TrackedFrame.SetTimestamp(timestamp->GetTimeStamp()); 

    return 1;
  }

}
