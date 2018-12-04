/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusIgtlMessageCommon_h
#define __vtkPlusIgtlMessageCommon_h

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusOpenIGTLinkExport.h"

// VTK includes
#include <vtkObject.h>

// OpenIGTLink includes
#include <igtlImageMessage.h>
#include <igtlImageMetaMessage.h>
#include <igtlMessageBase.h>
#include <igtlPlusTrackedFrameMessage.h>
#include <igtlPlusUsMessage.h>
#include <igtlPolyDataMessage.h>
#include <igtlPositionMessage.h>
#include <igtlSocket.h>
#include <igtlStringMessage.h>
#include <igtlTrackingDataMessage.h>
#include <igtlTransformMessage.h>
#if defined(OpenIGTLink_ENABLE_VIDEOSTREAMING)
  #include <igtlCodecCommonClasses.h>
  #include <igtlVideoMessage.h>
#endif

class vtkXMLDataElement;
//class igsioTrackedFrame; 
class vtkPolyData;
//class vtkIGSIOTransformRepository;

/*!
\class vtkPlusIgtlMessageCommon
\brief Helper class for OpenIGTLink message generation

This class is a helper class for OpenIGTLink message pack/unpack

\ingroup PlusLibOpenIGTLink
*/
class vtkPlusOpenIGTLinkExport vtkPlusIgtlMessageCommon: public vtkObject
{
public:
  static vtkPlusIgtlMessageCommon* New();
  vtkTypeMacro(vtkPlusIgtlMessageCommon, vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Pack tracked frame message from tracked frame */
  static PlusStatus PackTrackedFrameMessage(igtl::PlusTrackedFrameMessage::Pointer trackedFrameMessage, igsioTrackedFrame& trackedFrame, vtkSmartPointer<vtkMatrix4x4> embeddedImageTransform, const std::vector<igsioTransformName>& requestedTransforms);

  /*! Unpack tracked frame message to tracked frame */
  static PlusStatus UnpackTrackedFrameMessage(igtl::MessageHeader::Pointer headerMsg, igtl::Socket* socket, igsioTrackedFrame& trackedFrame, const igsioTransformName& embeddedTransformName, int crccheck);

  /*! Pack US message from tracked frame */
  static PlusStatus PackUsMessage(igtl::PlusUsMessage::Pointer usMessage, igsioTrackedFrame& trackedFrame);

  /*! Unpack US message to tracked frame */
  static PlusStatus UnpackUsMessage(igtl::MessageHeader::Pointer headerMsg, igtl::Socket* socket, igsioTrackedFrame& trackedFrame, int crccheck);

  /*! Pack image message from tracked frame */
  static PlusStatus PackImageMessage(igtl::ImageMessage::Pointer imageMessage, igsioTrackedFrame& trackedFrame, const vtkMatrix4x4& imageToReferenceTransform);

  /*! Pack image message from vtkImageData volume */
  static PlusStatus PackImageMessage(igtl::ImageMessage::Pointer imageMessage, vtkImageData* image, const vtkMatrix4x4& imageToReferenceTransform, double timestamp);

  /*! Unpack image message to tracked frame */
  static PlusStatus UnpackImageMessage(igtl::MessageHeader::Pointer headerMsg, igtl::Socket* socket, igsioTrackedFrame& trackedFrame, const igsioTransformName& embeddedTransformName, int crccheck);

  /*! Pack image meta deta message from vtkPlusServer::ImageMetaDataList  */
  static PlusStatus PackImageMetaMessage(igtl::ImageMetaMessage::Pointer imageMetaMessage, igsioCommon::ImageMetaDataList& imageMetaDataList);

#if defined(OpenIGTLink_ENABLE_VIDEOSTREAMING)
  /*! Pack video message from tracked frame */
  static PlusStatus PackVideoMessage(igtl::VideoMessage::Pointer videoMessage, igsioTrackedFrame& trackedFrame, igtl::GenericEncoder* encoder, const vtkMatrix4x4& videoToReferenceTransform);
#endif

  /*! Pack transform message from tracked frame */
  static PlusStatus PackTransformMessage(igtl::TransformMessage::Pointer transformMessage, igsioTransformName& transformName,
                                         igtl::Matrix4x4& igtlMatrix, ToolStatus status, double timestamp);

  /*! Pack poly data message from polydata */
  static PlusStatus PackPolyDataMessage(igtl::PolyDataMessage::Pointer polydataMessage, vtkSmartPointer<vtkPolyData> polyData, double timestamp);

  /*! Pack data message from tracked frame */
  static PlusStatus PackTrackingDataMessage(igtl::TrackingDataMessage::Pointer tdataMessage, const std::vector<igsioTransformName>& names, const vtkIGSIOTransformRepository& repository, double timestamp);

  /*! Unpack data message */
  static PlusStatus UnpackTrackingDataMessage(igtl::MessageHeader::Pointer headerMsg, igtl::Socket* socket,
      std::vector<igsioTransformName>& names, vtkIGSIOTransformRepository& repository, double& timestamp, int crccheck);

  /*! Unpack transform message */
  static PlusStatus UnpackTransformMessage(igtl::MessageHeader::Pointer headerMsg, igtl::Socket* socket,
      vtkMatrix4x4* transformMatrix, ToolStatus& toolStatus, std::string& transformName, double& timestamp, int crccheck);

  /*! Pack position message from tracked frame */
  static PlusStatus PackPositionMessage(igtl::PositionMessage::Pointer positionMessage, igsioTransformName& transformName, ToolStatus status,
                                        float position[3], float quaternion[4], double timestamp);

  /*! Unpack position message */
  static PlusStatus UnpackPositionMessage(igtl::MessageHeader::Pointer headerMsg, igtl::Socket* socket,
                                          vtkMatrix4x4* transformMatrix, std::string& transformName, ToolStatus& toolStatus, double& timestamp, int crccheck);

  /*! Pack string message */
  static PlusStatus PackStringMessage(igtl::StringMessage::Pointer stringMessage, const char* stringName, const char* stringValue, double timestamp);


  /*! Generate igtl::Matrix4x4 with the selected transform name from the transform repository */
  static PlusStatus GetIgtlMatrix(igtl::Matrix4x4& igtlMatrix, vtkIGSIOTransformRepository* transformRepository, igsioTransformName& transformName);

protected:
  vtkPlusIgtlMessageCommon();
  virtual ~vtkPlusIgtlMessageCommon();

private:
  vtkPlusIgtlMessageCommon(const vtkPlusIgtlMessageCommon&);
  void operator=(const vtkPlusIgtlMessageCommon&);
};

#endif
