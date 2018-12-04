/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __igtlPlusTrackedFrameMessage_h
#define __igtlPlusTrackedFrameMessage_h

#include "vtkPlusOpenIGTLinkExport.h"

#include "igsioTrackedFrame.h"
#include "igtl_types.h"
#include "igtl_win32header.h"
#include "igtlMessageBase.h"
#include "igtlObject.h"
#include "igtl_header.h"
#include "igtl_util.h"
#include "vtkMatrix4x4.h"
#include "vtkSmartPointer.h"
#include <string>

namespace igtl
{
  // This command prevents 4-byte alignment in the struct (which enables m_FrameSize[3])
#pragma pack(1)     /* For 1-byte boundary in memory */

  /*!
    \class PlusTrackedFrameMessage
    \brief IGTL message helper class for tracked frame messages
    \ingroup PlusLibOpenIGTLink
  */
  class vtkPlusOpenIGTLinkExport PlusTrackedFrameMessage: public MessageBase
  {
  public:
    igtlTypeMacro(igtl::PlusTrackedFrameMessage, igtl::MessageBase);
    igtlNewMacro(igtl::PlusTrackedFrameMessage);

  public:
    /*! Override clone so that we use the plus igtl factory */
    virtual igtl::MessageBase::Pointer Clone();

    /*! Set Plus TrackedFrame */
    PlusStatus SetTrackedFrame(const igsioTrackedFrame& trackedFrame, const std::vector<igsioTransformName>& requestedTransforms);

    /*! Get Plus TrackedFrame */
    igsioTrackedFrame GetTrackedFrame();

    /*! Set the embedded transform of the underlying image */
    PlusStatus SetEmbeddedImageTransform(vtkSmartPointer<vtkMatrix4x4> matrix);

    /*! Get the embedded transform of the underlying image */
    vtkSmartPointer<vtkMatrix4x4> GetEmbeddedImageTransform();

  protected:
    class TrackedFrameHeader
    {
    public:
      TrackedFrameHeader()
        : m_ScalarType()
        , m_NumberOfComponents(0)
        , m_ImageType(0)
        , m_ImageDataSizeInBytes(0)
        , m_XmlDataSizeInBytes(0)
        , m_ImageOrientation(0)
      {
        m_FrameSize[0] = m_FrameSize[1] = m_FrameSize[2] = 0;
        for (int i = 0; i < 4; ++i)
        {
          for (int j = 0; j < 4; ++j)
          {
            m_EmbeddedImageTransform[i][j] = (i == j) ? 1.f : 0.f;
          }
        }
      }

      size_t GetMessageHeaderSize()
      {
        size_t headersize = 0;
        headersize += sizeof(igtl_uint16);        // m_ScalarType
        headersize += sizeof(igtl_uint16);        // m_NumberOfComponents
        headersize += sizeof(igtl_uint16);        // m_ImageType
        headersize += sizeof(igtl_uint16) * 3;    // m_FrameSize[3]
        headersize += sizeof(igtl_uint32);        // m_ImageDataSizeInBytes
        headersize += sizeof(igtl_uint32);        // m_XmlDataSizeInBytes
        headersize += sizeof(igtl_uint16);        // m_ImageOrientation
        headersize += sizeof(igtl::Matrix4x4);    // m_EmbeddedImageTransform[4][4]

        return headersize;
      }

      void ConvertEndianness()
      {
        if (igtl_is_little_endian())
        {
          m_ScalarType = BYTE_SWAP_INT16(m_ScalarType);
          m_NumberOfComponents = BYTE_SWAP_INT16(m_NumberOfComponents);
          m_ImageType = BYTE_SWAP_INT16(m_ImageType);
          m_FrameSize[0] = BYTE_SWAP_INT16(m_FrameSize[0]);
          m_FrameSize[1] = BYTE_SWAP_INT16(m_FrameSize[1]);
          m_FrameSize[2] = BYTE_SWAP_INT16(m_FrameSize[2]);
          m_ImageDataSizeInBytes = BYTE_SWAP_INT32(m_ImageDataSizeInBytes);
          m_XmlDataSizeInBytes = BYTE_SWAP_INT32(m_XmlDataSizeInBytes);
          m_ImageOrientation = BYTE_SWAP_INT16(m_ImageOrientation);
        }
      }

      igtl_uint16     m_ScalarType;             /* scalar type                     */
      igtl_uint16     m_NumberOfComponents;     /* number of scalar components */
      igtl_uint16     m_ImageType;              /* image type */
      igtl_uint16     m_FrameSize[3];           /* entire image volume size */
      igtl_uint32     m_ImageDataSizeInBytes;   /* size of the image, in bytes */
      igtl_uint32     m_XmlDataSizeInBytes;     /* size of the xml data, in bytes */
      igtl_uint16     m_ImageOrientation;       /* orientation of the image */
      igtl::Matrix4x4 m_EmbeddedImageTransform; /* matrix representing the IJK to world transformation */
    };

    virtual int  CalculateContentBufferSize();
    virtual int  PackContent();
    virtual int  UnpackContent();

    PlusTrackedFrameMessage();
    ~PlusTrackedFrameMessage();

    igsioTrackedFrame m_TrackedFrame;
    std::string m_TrackedFrameXmlData;

    TrackedFrameHeader m_MessageHeader;
  };

#pragma pack()

} // namespace igtl

#endif
