/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __igtlPlusTrackedFrameMessage_h
#define __igtlPlusTrackedFrameMessage_h

#include <string>

#include "igtlObject.h"
#include "igtlutil/igtl_util.h"
#include "igtlutil/igtl_header.h"
#include "igtlMessageBase.h"
#include "igtl_types.h"
#include "igtl_win32header.h"
#include "TrackedFrame.h"

namespace igtl
{

/*! 
  \class PlusTrackedFrameMessage 
  \brief IGTL message helper class for tracked frame messages
  \ingroup PlusLibOpenIGTLink
*/
class IGTLCommon_EXPORT PlusTrackedFrameMessage: public MessageBase
{
public:
  typedef PlusTrackedFrameMessage                 Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro( igtl::PlusTrackedFrameMessage, igtl::MessageBase );
  igtlNewMacro( igtl::PlusTrackedFrameMessage );

public:
  
  /*! Set Plus TrackedFrame */ 
  PlusStatus SetTrackedFrame( const TrackedFrame& trackedFrame); 

  /*! Get Plus TrackedFrame */ 
  TrackedFrame GetTrackedFrame(); 
  
protected:
  
  struct MessageHeader 
  {
    size_t GetMessageHeaderSize()
    {
      size_t headersize = 0; 
      headersize += sizeof(igtl_uint16);  // m_Version
      headersize += sizeof(igtl_uint16);  // m_ScalarType
      headersize += sizeof(igtl_uint16) * 2;  // m_FrameSize[2]
      headersize += sizeof(igtl_uint32);  // m_ImageDataSizeInBytes
      headersize += sizeof(igtl_uint32);  // m_XmlDataSizeInBytes

      return headersize; 
    }

    void ConvertEndianness()
    {
      if (igtl_is_little_endian()) 
      {
        m_Version = BYTE_SWAP_INT16(m_Version); 
        m_ScalarType = BYTE_SWAP_INT16(m_ScalarType); 
        m_FrameSize[0] = BYTE_SWAP_INT16(m_FrameSize[0]); 
        m_FrameSize[1] = BYTE_SWAP_INT16(m_FrameSize[1]); 
        m_ImageDataSizeInBytes = BYTE_SWAP_INT32(m_ImageDataSizeInBytes);
        m_XmlDataSizeInBytes = BYTE_SWAP_INT32(m_XmlDataSizeInBytes);
      }
    }

    igtl_uint16 m_Version;          /* data format version number(1)   */
    igtl_uint16 m_ScalarType;      /* scalar type                     */
    igtl_uint16 m_FrameSize[2];    /* entire image volume size */
    igtl_uint32 m_ImageDataSizeInBytes; 
    igtl_uint32 m_XmlDataSizeInBytes; 
  };

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

  PlusTrackedFrameMessage();
  ~PlusTrackedFrameMessage();

  TrackedFrame m_TrackedFrame; 
  std::string m_TrackedFrameXmlData; 

  MessageHeader m_MessageHeader; 
};


} // namespace igtl

#endif 
