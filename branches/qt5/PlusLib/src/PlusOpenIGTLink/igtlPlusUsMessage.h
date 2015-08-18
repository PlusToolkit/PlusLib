/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 
#ifndef __igtlPlusUsMessage_h
#define __igtlPlusUsMessage_h

#include "vtkPlusOpenIGTLinkExport.h"

#include "igtlImageMessage.h"
#include "igtl_types.h"

class TrackedFrame;

namespace igtl
{
  /*! 
  \class PlusUsMessage 
  \brief IGTL message helper class for sending USMessage device messages 
  as IMAGE type message from tracked frame (for MUSiiC igtlMUSMessage)
  \ingroup PlusLibOpenIGTLink
  */
  class vtkPlusOpenIGTLinkExport PlusUsMessage: public igtl::ImageMessage
  {
  public:
    typedef PlusUsMessage                   Self;
    typedef igtl::ImageMessage              Superclass;
    typedef igtl::SmartPointer<Self>        Pointer;
    typedef igtl::SmartPointer<const Self>  ConstPointer;

    igtlTypeMacro(igtl::PlusUsMessage, igtl::ImageMessage)
      igtlNewMacro(igtl::PlusUsMessage);

  public:

    /*! Set Plus TrackedFrame */ 
    PlusStatus SetTrackedFrame( const TrackedFrame& trackedFrame); 

    /*! Get Plus TrackedFrame */ 
    TrackedFrame& GetTrackedFrame(); 

  protected:

    struct MessageHeader 
    {
      size_t GetMessageHeaderSize();
      void ConvertEndianness();

      igtl_int32 m_DataType;              // US data type
      igtl_int32 m_TransmitFrequency;     // transmit frequency of ultrasound probe
      igtl_int32 m_SamplingFrequency;     // sampling frequency of ultrasound machine
      igtl_int32 m_DataRate;              // frame rate or pulse repetition period in Doppler modes
      igtl_int32 m_LineDensity;           // Line density: can be used to calculate elment spacing if potch and native # elements is known

      igtl_int32 m_SteeringAngle;         // Steering Angle of Ultrasound image
      igtl_int32 m_ProbeID;               // ID of ultrasound probe
      igtl_int32 m_ExtensionAngle;        // Extension Angle
      igtl_int32 m_Elements;              // The number of elements in the probe that acquire the image
      igtl_int32 m_Pitch;                 // The spacing between elements of the probe that acquired the image

      igtl_int32 m_Radius;                // The curvature of the probe that acquired the image
      igtl_int32 m_ProbeAngle;            // The field of view of the probe that acquired the image. (Phased array)
      igtl_int32 m_TxOffset;              // For phased array, the number of elements that are offset in the steered image
    };

    virtual int GetBodyPackSize();
    virtual int PackBody();
    virtual int UnpackBody();

    PlusUsMessage();
    ~PlusUsMessage();

    TrackedFrame m_TrackedFrame; 
    MessageHeader m_MessageHeader; 
  };
}

#endif 
