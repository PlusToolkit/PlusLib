/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusIgtlMessageCommon_h
#define __vtkPlusIgtlMessageCommon_h

#include "PlusConfigure.h"
#include "vtkObject.h" 
#include "igtlMessageBase.h"
#include "igtlSocket.h"

//----------------------------------------------------------------------------
// IGT message types
#include "igtlImageMessage.h"
#include "igtlTransformMessage.h"
#include "igtlPositionMessage.h"
#include "igtlPlusTrackedFrameMessage.h"
#include "igtlPlusUsMessage.h"

class vtkXMLDataElement; 
class TrackedFrame; 
class vtkTransformRepository; 

/*!
\class vtkPlusIgtlMessageCommon 
\brief Helper class for OpenIGTLink message generation

This class is a helper class for OpenIGTLink message pack/unpack 

\ingroup PlusLibOpenIGTLink
*/ 
class VTK_EXPORT vtkPlusIgtlMessageCommon: public vtkObject
{
public:
  
  static vtkPlusIgtlMessageCommon *New();
  vtkTypeRevisionMacro(vtkPlusIgtlMessageCommon,vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  /*! Pack tracked frame message from tracked frame */ 
  static PlusStatus PackTrackedFrameMessage( igtl::PlusTrackedFrameMessage::Pointer trackedFrameMessage, TrackedFrame& trackedFrame); 

  /*! Unpack tracked frame message to tracked frame */ 
  static PlusStatus UnpackTrackedFrameMessage( igtl::MessageHeader::Pointer headerMsg, igtl::Socket *socket, TrackedFrame& trackedFrame, int crccheck); 

  /*! Pack US message from tracked frame */ 
  static PlusStatus PackUsMessage( igtl::PlusUsMessage::Pointer usMessage, TrackedFrame& trackedFrame); 

  /*! Unpack US message to tracked frame */ 
  static PlusStatus UnpackUsMessage( igtl::MessageHeader::Pointer headerMsg, igtl::Socket *socket, TrackedFrame& trackedFrame, int crccheck); 

  /*! Pack image message from tracked frame */ 
  static PlusStatus PackImageMessage(igtl::ImageMessage::Pointer imageMessage, TrackedFrame& trackedFrame, igtl::Matrix4x4& igtlMatrix ); 

  /*! Unpack image message to tracked frame */ 
  static PlusStatus UnpackImageMessage( igtl::MessageHeader::Pointer headerMsg, igtl::Socket *socket, TrackedFrame& trackedFrame, const PlusTransformName &embeddedTransformName, int crccheck); 

  /*! Pack transform message from tracked frame */ 
  static PlusStatus PackTransformMessage(igtl::TransformMessage::Pointer transformMessage, PlusTransformName& transformName, 
    igtl::Matrix4x4& igtlMatrix, double timestamp ); 

  /*! Unpack transform message */ 
  static PlusStatus UnpackTransformMessage(igtl::MessageHeader::Pointer headerMsg, igtl::Socket *socket, 
    vtkMatrix4x4* transformMatrix, std::string& transformName, double& timestamp, int crccheck ); 

  /*! Pack position message from tracked frame */ 
  static PlusStatus PackPositionMessage(igtl::PositionMessage::Pointer positionMessage, PlusTransformName& transformName, 
    igtl::Matrix4x4& igtlMatrix, double timestamp ); 

  /*! Unpack position message */ 
  static PlusStatus UnpackPositionMessage(igtl::MessageHeader::Pointer headerMsg, igtl::Socket* socket, 
    float position[3], std::string& positionName, double& timestamp, int crccheck );

  /*! Generate igtl::Matrix4x4 with the selected transform name from the transform repository */ 
  static PlusStatus GetIgtlMatrix(igtl::Matrix4x4& igtlMatrix, vtkTransformRepository* transformRepository, PlusTransformName& transformName); 

protected:
  vtkPlusIgtlMessageCommon();
  virtual ~vtkPlusIgtlMessageCommon();

private:
  vtkPlusIgtlMessageCommon(const vtkPlusIgtlMessageCommon&);
  void operator=(const vtkPlusIgtlMessageCommon&);
}; 

#endif 
