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

  /*! Pack image message from tracked frame */ 
  static PlusStatus PackImageMessage(igtl::ImageMessage::Pointer imageMessage, TrackedFrame& trackedFrame, igtl::Matrix4x4& igtlMatrix ); 

  /*! Pack transform message from tracked frame */ 
  static PlusStatus PackTransformMessage(igtl::TransformMessage::Pointer transformMessage, PlusTransformName& transformName, 
    igtl::Matrix4x4& igtlMatrix, double timestamp ); 

  /*! Unpack transform message to tracked frame */ 
  static PlusStatus UnpackTransformMessage(igtl::MessageHeader::Pointer headerMsg, igtl::Socket *socket, 
    vtkMatrix4x4* transformMatrix, std::string& transformName, double& timestamp ); 

  /*! Pack position message from tracked frame */ 
  static PlusStatus PackPositionMessage(igtl::PositionMessage::Pointer positionMessage, PlusTransformName& transformName, 
    igtl::Matrix4x4& igtlMatrix, double timestamp ); 

  /*! Unpack position message to tracked frame */ 
  static PlusStatus UnpackPositionMessage(igtl::MessageHeader::Pointer headerMsg, igtl::Socket* socket, 
    float position[3], std::string& positionName, double& timestamp );

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
