/***************************************************************************
# Copyright 
# MUSiiC Laboratory
# Hyun-Jae Kang, Philipp J Stolka,Emad M Boctor
# Johns Hopkins Medical Institutions
# Department of Radiology, Division of Medical Imaging Physics

# For commercial use/licensing, please contact Emad Boctor, Ph.D. at eboctor@jhmi.edu.
# ***************************************************************************/

#include "PlusConfigure.h"
#include "igtlPlusUsMessage.h"
#include "igtl_image.h"

namespace igtl
{
  //----------------------------------------------------------------------------
  PlusUsMessage::PlusUsMessage():ImageMessage()
  {
    m_DeviceName 	="USMessage";		
  }

  //----------------------------------------------------------------------------
  PlusUsMessage::~PlusUsMessage()
  {
  }

  //----------------------------------------------------------------------------
  TrackedFrame PlusUsMessage::GetTrackedFrame()
  {
    return this->m_TrackedFrame;
  }

  //----------------------------------------------------------------------------
  PlusStatus PlusUsMessage::SetTrackedFrame( const TrackedFrame& trackedFrame ) 
  {
    this->m_TrackedFrame = trackedFrame; 

    double timestamp = this->m_TrackedFrame.GetTimestamp();

    igtl::TimeStamp::Pointer igtlFrameTime = igtl::TimeStamp::New();
    igtlFrameTime->SetTime( timestamp );

    int offset[3]={0}; 
    int imageSizePixels[3]={0}; 
    int size[3]={0}; 

    // NOTE: MUSiiC library expects the frame size in the format 
    // as Ultrasonix provide, not like Plus (Plus: if vector data switch width and 
    // height, because the image is not rasterized like a bitmap, but written rayline by rayline)
    this->m_TrackedFrame.GetFrameSize(size); 
    imageSizePixels[0] = size[1]; 
    imageSizePixels[1] = size[0];
    imageSizePixels[2] = 1;

    int scalarType = PlusVideoFrame::GetIGTLScalarPixelType( this->m_TrackedFrame.GetImageData()->GetITKScalarPixelType() ); 

    this->SetDimensions( imageSizePixels );
    this->SetSubVolume(imageSizePixels, offset); 
    this->SetScalarType( scalarType );
    this->SetSpacing(0.2,0.2,1); 
    this->AllocateScalars();

    unsigned char* igtlImagePointer = (unsigned char*)( this->GetScalarPointer() );
    unsigned char* plusImagePointer = (unsigned char*)( this->m_TrackedFrame.GetImageData()->GetBufferPointer() );

    memcpy(igtlImagePointer, plusImagePointer, this->GetImageSize());

    this->SetTimeStamp( igtlFrameTime );

    this->m_MessageHeader.m_DataType = 0; 
    if ( this->m_TrackedFrame.IsCustomFrameFieldDefined("SonixDataType") )
    {
      const char* fieldValue = this->m_TrackedFrame.GetCustomFrameField("SonixDataType"); 
      PlusCommon::StringToInt(fieldValue, this->m_MessageHeader.m_DataType); 
    }

    this->m_MessageHeader.m_TransmitFrequency = 0;
    if ( this->m_TrackedFrame.IsCustomFrameFieldDefined("SonixTransmitFrequency") )
    {
      const char* fieldValue = this->m_TrackedFrame.GetCustomFrameField("SonixTransmitFrequency"); 
      PlusCommon::StringToInt(fieldValue, this->m_MessageHeader.m_TransmitFrequency); 
    }

    this->m_MessageHeader.m_SamplingFrequency = 0;
    if ( this->m_TrackedFrame.IsCustomFrameFieldDefined("SonixSamplingFrequency") )
    {
      const char* fieldValue = this->m_TrackedFrame.GetCustomFrameField("SonixSamplingFrequency"); 
      PlusCommon::StringToInt(fieldValue, this->m_MessageHeader.m_SamplingFrequency); 
    }

    this->m_MessageHeader.m_DataRate = 0;
    if ( this->m_TrackedFrame.IsCustomFrameFieldDefined("SonixDataRate") )
    {
      const char* fieldValue = this->m_TrackedFrame.GetCustomFrameField("SonixDataRate"); 
      PlusCommon::StringToInt(fieldValue, this->m_MessageHeader.m_DataRate); 
    }

    this->m_MessageHeader.m_LineDensity = 0;
    if ( this->m_TrackedFrame.IsCustomFrameFieldDefined("SonixLineDensity") )
    {
      const char* fieldValue = this->m_TrackedFrame.GetCustomFrameField("SonixLineDensity"); 
      PlusCommon::StringToInt(fieldValue, this->m_MessageHeader.m_LineDensity); 
    }

    this->m_MessageHeader.m_SteeringAngle = 0;
    if ( this->m_TrackedFrame.IsCustomFrameFieldDefined("SonixSteeringAngle") )
    {
      const char* fieldValue = this->m_TrackedFrame.GetCustomFrameField("SonixSteeringAngle"); 
      PlusCommon::StringToInt(fieldValue, this->m_MessageHeader.m_SteeringAngle); 
    }

    this->m_MessageHeader.m_ProbeID = 0;
    if ( this->m_TrackedFrame.IsCustomFrameFieldDefined("SonixProbeID") )
    {
      const char* fieldValue = this->m_TrackedFrame.GetCustomFrameField("SonixProbeID"); 
      PlusCommon::StringToInt(fieldValue, this->m_MessageHeader.m_ProbeID); 
    }

    this->m_MessageHeader.m_ExtensionAngle = 0;
    if ( this->m_TrackedFrame.IsCustomFrameFieldDefined("SonixExtensionAngle") )
    {
      const char* fieldValue = this->m_TrackedFrame.GetCustomFrameField("SonixExtensionAngle"); 
      PlusCommon::StringToInt(fieldValue, this->m_MessageHeader.m_ExtensionAngle); 
    }

    this->m_MessageHeader.m_Elements = 0;
    if ( this->m_TrackedFrame.IsCustomFrameFieldDefined("SonixElements") )
    {
      const char* fieldValue = this->m_TrackedFrame.GetCustomFrameField("SonixElements"); 
      PlusCommon::StringToInt(fieldValue, this->m_MessageHeader.m_Elements); 
    }

    this->m_MessageHeader.m_Pitch = 0;
    if ( this->m_TrackedFrame.IsCustomFrameFieldDefined("SonixPitch") )
    {
      const char* fieldValue = this->m_TrackedFrame.GetCustomFrameField("SonixPitch"); 
      PlusCommon::StringToInt(fieldValue, this->m_MessageHeader.m_Pitch); 
    }

    this->m_MessageHeader.m_Radius = 0;
    if ( this->m_TrackedFrame.IsCustomFrameFieldDefined("SonixRadius") )
    {
      const char* fieldValue = this->m_TrackedFrame.GetCustomFrameField("SonixRadius"); 
      PlusCommon::StringToInt(fieldValue, this->m_MessageHeader.m_Radius); 
    }

    this->m_MessageHeader.m_ProbeAngle = 0;
    if ( this->m_TrackedFrame.IsCustomFrameFieldDefined("SonixProbeAngle") )
    {
      const char* fieldValue = this->m_TrackedFrame.GetCustomFrameField("SonixProbeAngle"); 
      PlusCommon::StringToInt(fieldValue, this->m_MessageHeader.m_ProbeAngle); 
    }

    this->m_MessageHeader.m_TxOffset = 0;
    if ( this->m_TrackedFrame.IsCustomFrameFieldDefined("SonixTxOffset") )
    {
      const char* fieldValue = this->m_TrackedFrame.GetCustomFrameField("SonixTxOffset"); 
      PlusCommon::StringToInt(fieldValue, this->m_MessageHeader.m_TxOffset); 
    }
   
    return PLUS_SUCCESS; 
  }

  //----------------------------------------------------------------------------
  int PlusUsMessage::GetBodyPackSize()
  {
    return GetSubVolumeImageSize() + IGTL_IMAGE_HEADER_SIZE + this->m_MessageHeader.GetMessageHeaderSize();
  }

  //----------------------------------------------------------------------------
  int PlusUsMessage::PackBody()
  {
    igtl::ImageMessage::PackBody();

    MessageHeader* header = (MessageHeader*)(m_Image + GetSubVolumeImageSize() );
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
  int PlusUsMessage::UnpackBody()
  {
    igtl::ImageMessage::UnpackBody();

    MessageHeader* header = (MessageHeader*)(m_Image[GetSubVolumeImageSize()]);

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