/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "igsioTrackedFrame.h"
#include "igsioVideoFrame.h"
#include "vtkPlusIgtlMessageCommon.h"
#include "vtkIGSIOTrackedFrameList.h"
#include "vtkIGSIOTransformRepository.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkTransform.h>
#include <vtkNew.h>

// OpenIGTLink includes
#include <igtl_tdata.h>

// OpenIGTLinkIO includes
#include <igtlioImageConverter.h>
#include <igtlioPolyDataConverter.h>
#include <igtlioTransformConverter.h>
#if defined(OpenIGTLink_ENABLE_VIDEOSTREAMING)
  #include <igtlioVideoConverter.h>
#endif

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusIgtlMessageCommon);

//----------------------------------------------------------------------------
vtkPlusIgtlMessageCommon::vtkPlusIgtlMessageCommon()
{
}

//----------------------------------------------------------------------------
vtkPlusIgtlMessageCommon::~vtkPlusIgtlMessageCommon()
{
}

//----------------------------------------------------------------------------
void vtkPlusIgtlMessageCommon::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIgtlMessageCommon::GetIgtlMatrix(igtl::Matrix4x4& igtlMatrix,
    vtkIGSIOTransformRepository* transformRepository,
    igsioTransformName& transformName)
{
  igtl::IdentityMatrix(igtlMatrix);

  if (transformRepository == NULL || !transformName.IsValid())
  {
    LOG_ERROR("GetIgtlMatrix failed: Transform repository or transform name is invalid");
    return PLUS_FAIL;
  }

  ToolStatus status(TOOL_INVALID);
  vtkSmartPointer<vtkMatrix4x4> vtkMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  if (transformRepository->GetTransform(transformName, vtkMatrix, &status) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to get transform from transform repository (" << transformName.From() << " to " << transformName.To() << ")");
    return PLUS_FAIL;
  }

  if (status != TOOL_OK)
  {
    LOG_DEBUG("Skipped transformation matrix - Invalid transform in the transform repository (" << transformName.From() << " to " << transformName.To() << ")");
    return PLUS_FAIL;
  }

  // Copy VTK matrix to IGTL matrix
  igtlioTransformConverter::VTKToIGTLTransform(*vtkMatrix, igtlMatrix);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIgtlMessageCommon::PackTrackedFrameMessage(igtl::PlusTrackedFrameMessage::Pointer trackedFrameMessage,
    igsioTrackedFrame& trackedFrame,
    vtkSmartPointer<vtkMatrix4x4> embeddedImageTransform,
    const std::vector<igsioTransformName>& requestedTransforms)
{
  if (trackedFrameMessage.IsNull())
  {
    LOG_ERROR("Failed to pack tracked frame message - input tracked frame message is NULL");
    return PLUS_FAIL;
  }

  PlusStatus status = trackedFrameMessage->SetTrackedFrame(trackedFrame, requestedTransforms);
  if (status == PLUS_FAIL)
  {
    return status;
  }
  status = trackedFrameMessage->SetEmbeddedImageTransform(embeddedImageTransform);
  if (status == PLUS_FAIL)
  {
    return status;
  }

  trackedFrameMessage->Pack();

  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIgtlMessageCommon::UnpackTrackedFrameMessage(igtl::MessageHeader::Pointer headerMsg,
    igtl::Socket* socket,
    igsioTrackedFrame& trackedFrame,
    const igsioTransformName& embeddedTransformName,
    int crccheck)
{
  if (headerMsg.IsNull())
  {
    LOG_ERROR("Unable to unpack tracked frame message - header message is NULL!");
    return PLUS_FAIL;
  }

  if (socket == NULL)
  {
    LOG_ERROR("Unable to unpack tracked frame message - socket is NULL!");
    return PLUS_FAIL;
  }

  igtl::PlusTrackedFrameMessage::Pointer trackedFrameMsg = dynamic_cast<igtl::PlusTrackedFrameMessage*>(headerMsg.GetPointer());
  if (trackedFrameMsg.IsNull())
  {
    trackedFrameMsg = igtl::PlusTrackedFrameMessage::New();
  }
  trackedFrameMsg->SetMessageHeader(headerMsg);
  trackedFrameMsg->AllocateBuffer();

  socket->Receive(trackedFrameMsg->GetBufferBodyPointer(), trackedFrameMsg->GetBufferBodySize());

  int c = trackedFrameMsg->Unpack(crccheck);
  if (!(c & igtl::MessageHeader::UNPACK_BODY))
  {
    LOG_ERROR("Couldn't receive tracked frame message from server!");
    return PLUS_FAIL;
  }

  // if CRC check is OK. get tracked frame data.
  trackedFrame = trackedFrameMsg->GetTrackedFrame();

  if (embeddedTransformName.IsValid())
  {
    // Save the transform that is embedded in the TRACKEDFRAME message into the tracked frame
    trackedFrame.SetFrameTransform(embeddedTransformName, trackedFrameMsg->GetEmbeddedImageTransform());
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIgtlMessageCommon::PackUsMessage(igtl::PlusUsMessage::Pointer usMessage, igsioTrackedFrame& trackedFrame)
{
  if (usMessage.IsNull())
  {
    LOG_ERROR("Failed to pack US message - input US message is NULL");
    return PLUS_FAIL;
  }

  PlusStatus status = usMessage->SetTrackedFrame(trackedFrame);
  usMessage->Pack();

  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIgtlMessageCommon::UnpackUsMessage(igtl::MessageHeader::Pointer headerMsg,
    igtl::Socket* socket,
    igsioTrackedFrame& trackedFrame,
    int crccheck)
{
  if (headerMsg.IsNull())
  {
    LOG_ERROR("Unable to unpack US message - header message is NULL!");
    return PLUS_FAIL;
  }

  if (socket == NULL)
  {
    LOG_ERROR("Unable to unpack US message - socket is NULL!");
    return PLUS_FAIL;
  }

  igtl::PlusUsMessage::Pointer usMsg = igtl::PlusUsMessage::New();
  usMsg->SetMessageHeader(headerMsg);
  usMsg->AllocateBuffer();

  socket->Receive(usMsg->GetBufferBodyPointer(), usMsg->GetBufferBodySize());

  int c = usMsg->Unpack(crccheck);
  if (!(c & igtl::MessageHeader::UNPACK_BODY))
  {
    LOG_ERROR("Couldn't receive US message from server!");
    return PLUS_FAIL;
  }

  // if CRC check is OK. get tracked frame data.
  trackedFrame = usMsg->GetTrackedFrame();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIgtlMessageCommon::PackImageMessage(igtl::ImageMessage::Pointer imageMessage,
    igsioTrackedFrame& trackedFrame,
    const vtkMatrix4x4& matrix)
{
  if (imageMessage.IsNull())
  {
    LOG_ERROR("Failed to pack image message - input image message is NULL");
    return PLUS_FAIL;
  }

  if (!trackedFrame.GetImageData()->IsImageValid())
  {
    LOG_WARNING("Unable to send image message - image data is NOT valid!");
    return PLUS_FAIL;
  }

  double timestamp = trackedFrame.GetTimestamp();
  vtkImageData* frameImage = trackedFrame.GetImageData()->GetImage();

  igtl::TimeStamp::Pointer igtlFrameTime = igtl::TimeStamp::New();
  igtlFrameTime->SetTime(timestamp);

  int imageSizePixels[3] = { 0 };
  int subSizePixels[3] = { 0 };
  int subOffset[3] = { 0 };
  double imageSpacingMm[3] = { 0 };
  double imageOriginMm[3] = { 0 };
  int scalarType = PlusCommon::GetIGTLScalarPixelTypeFromVTK(trackedFrame.GetImageData()->GetVTKScalarPixelType());
  unsigned int numScalarComponents(1);
  if (trackedFrame.GetImageData()->GetNumberOfScalarComponents(numScalarComponents) == PLUS_FAIL)
  {
    LOG_ERROR("Unable to retrieve number of scalar components.");
    return PLUS_FAIL;
  }

  frameImage->GetDimensions(imageSizePixels);
  frameImage->GetSpacing(imageSpacingMm);
  frameImage->GetOrigin(imageOriginMm);
  frameImage->GetDimensions(subSizePixels);

  float spacingFloat[3] = { 0 };
  for (int i = 0; i < 3; ++ i)
  {
    spacingFloat[ i ] = (float)imageSpacingMm[ i ];
  }

  imageMessage->SetDimensions(imageSizePixels);
  imageMessage->SetSpacing(spacingFloat);
  imageMessage->SetNumComponents(numScalarComponents);
  imageMessage->SetScalarType(scalarType);
  imageMessage->SetEndian(igtl_is_little_endian() ? igtl::ImageMessage::ENDIAN_LITTLE : igtl::ImageMessage::ENDIAN_BIG);
  imageMessage->SetSubVolume(subSizePixels, subOffset);
  imageMessage->AllocateScalars();

  unsigned char* igtlImagePointer = (unsigned char*)(imageMessage->GetScalarPointer());
  unsigned char* vtkImagePointer = (unsigned char*)(frameImage->GetScalarPointer());

  memcpy(igtlImagePointer, vtkImagePointer, imageMessage->GetImageSize());

  // Convert VTK transform to IGTL transform.
  if (igtlioImageConverter::VTKTransformToIGTLImage(matrix, imageSizePixels, imageSpacingMm, imageOriginMm, imageMessage) != 1)
  {
    LOG_ERROR("Failed to pack image message - unable to compute IJKToRAS transform");
    return PLUS_FAIL;
  }

  imageMessage->SetTimeStamp(igtlFrameTime);
  imageMessage->Pack();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIgtlMessageCommon::PackImageMessage(igtl::ImageMessage::Pointer imageMessage,
    vtkImageData* image,
    const vtkMatrix4x4& imageToReferenceTransform,
    double timestamp)
{
  if (imageMessage.IsNull())
  {
    LOG_ERROR("Failed to pack image message - input image message is NULL");
    return PLUS_FAIL;
  }

  int imageSizePixels[3] = { 0 };
  image->GetDimensions(imageSizePixels);
  imageMessage->SetDimensions(imageSizePixels);

  int subSizePixels[3] = { 0 };
  image->GetDimensions(subSizePixels);
  int subOffset[3] = { 0 };
  imageMessage->SetSubVolume(subSizePixels, subOffset);

  double imageSpacingMm[3] = { 0 };
  image->GetSpacing(imageSpacingMm);
  float spacingFloat[3] = { 0 };
  for (int i = 0; i < 3; ++i)
  {
    spacingFloat[i] = (float)imageSpacingMm[i];
  }
  imageMessage->SetSpacing(spacingFloat);

  double imageOriginMm[3] = { 0 };
  image->GetOrigin(imageOriginMm);
  // imageMessage->SetOrigin() is not used, because origin and normal is set later by igtlioImageConverter::VTKTransformToIGTLImage()

  int scalarType = PlusCommon::GetIGTLScalarPixelTypeFromVTK(image->GetScalarType());
  imageMessage->SetScalarType(scalarType);
  imageMessage->SetEndian(igtl_is_little_endian() ? igtl::ImageMessage::ENDIAN_LITTLE : igtl::ImageMessage::ENDIAN_BIG);
  imageMessage->AllocateScalars();

  unsigned char* igtlImagePointer = (unsigned char*)(imageMessage->GetScalarPointer());
  unsigned char* vtkImagePointer = (unsigned char*)(image->GetScalarPointer());

  memcpy(igtlImagePointer, vtkImagePointer, imageMessage->GetImageSize());

  if (igtlioImageConverter::VTKTransformToIGTLImage(imageToReferenceTransform, imageSizePixels, imageSpacingMm, imageOriginMm, imageMessage) != 1)
  {
    LOG_ERROR("Failed to pack image message - unable to compute IJKToRAS transform");
    return PLUS_FAIL;
  }

  igtl::TimeStamp::Pointer igtlTime = igtl::TimeStamp::New();
  igtlTime->SetTime(timestamp);
  imageMessage->SetTimeStamp(igtlTime);

  imageMessage->Pack();

  return PLUS_SUCCESS;

}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIgtlMessageCommon::UnpackImageMessage(igtl::MessageHeader::Pointer headerMsg,
    igtl::Socket* socket,
    igsioTrackedFrame& trackedFrame,
    const igsioTransformName& embeddedTransformName,
    int crccheck)
{
  if (headerMsg.IsNull())
  {
    LOG_ERROR("Unable to unpack image message - header message is NULL!");
    return PLUS_FAIL;
  }

  if (socket == NULL)
  {
    LOG_ERROR("Unable to unpack image message - socket is NULL!");
    return PLUS_FAIL;
  }

  // Message body handler for IMAGE
  igtl::ImageMessage::Pointer imgMsg = dynamic_cast<igtl::ImageMessage*>(headerMsg.GetPointer());
  if (imgMsg.IsNull())
  {
    imgMsg = igtl::ImageMessage::New();
  }
  imgMsg->SetMessageHeader(headerMsg);
  imgMsg->AllocateBuffer();

  socket->Receive(imgMsg->GetBufferBodyPointer(), imgMsg->GetBufferBodySize());

  int c = imgMsg->Unpack(crccheck);
  if (!(c & igtl::MessageHeader::UNPACK_BODY))
  {
    LOG_ERROR("Couldn't receive image message from server!");
    return PLUS_FAIL;
  }

  // if CRC check is OK. Read data.
  igtl::TimeStamp::Pointer igtlTimestamp = igtl::TimeStamp::New();
  imgMsg->GetTimeStamp(igtlTimestamp);

  int imgSize[3] = {0}; // image dimension in pixels
  imgMsg->GetDimensions(imgSize);

  if (imgSize[0] < 0 || imgSize[1] < 0 || imgSize[2] < 0)
  {
    LOG_ERROR("Image with negative dimension. Aborting.");
    return PLUS_FAIL;
  }
  FrameSizeType imageSize = {static_cast<unsigned int>(imgSize[0]), static_cast<unsigned int>(imgSize[1]), static_cast<unsigned int>(imgSize[2]) };

  // Set scalar pixel type
  igsioCommon::VTKScalarPixelType pixelType = PlusCommon::GetVTKScalarPixelTypeFromIGTL(imgMsg->GetScalarType());
  igsioVideoFrame frame;
  if (frame.AllocateFrame(imageSize, pixelType, imgMsg->GetNumComponents()) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to allocate image data for tracked frame!");
    return PLUS_FAIL;
  }

  // Set the image type to support color images
  if (imgMsg->GetScalarType() == igtl::ImageMessage::TYPE_INT8)
  {
    frame.SetImageType((imgMsg->GetNumComponents() == igtl::ImageMessage::DTYPE_VECTOR) ? US_IMG_RGB_COLOR : US_IMG_BRIGHTNESS);
  }

  // Copy image to buffer
  memcpy(frame.GetScalarPointer(), imgMsg->GetScalarPointer(), frame.GetFrameSizeInBytes());

  trackedFrame.SetImageData(frame);
  trackedFrame.SetTimestamp(igtlTimestamp->GetTimeStamp());

  if (embeddedTransformName.IsValid())
  {
    vtkSmartPointer<vtkMatrix4x4> vtkMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    if (igtlioImageConverter::IGTLImageToVTKTransform(imgMsg, vtkMatrix) != 1)
    {
      LOG_ERROR("Failed to unpack image message - unable to extract IJKToRAS transform");
      return PLUS_FAIL;
    }
    trackedFrame.SetFrameTransform(embeddedTransformName, vtkMatrix);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIgtlMessageCommon::PackImageMetaMessage(igtl::ImageMetaMessage::Pointer imageMetaMessage,
    igsioCommon::ImageMetaDataList& imageMetaDataList)
{
  if (imageMetaMessage.IsNull())
  {
    LOG_ERROR("Failed to pack image message - input image message is NULL");
    return PLUS_FAIL;
  }
  for (igsioCommon::ImageMetaDataList::iterator it = imageMetaDataList.begin(); it != imageMetaDataList.end(); it++)
  {
    igtl::ImageMetaElement::Pointer imageMetaElement = igtl::ImageMetaElement::New();
    igtl::TimeStamp::Pointer timeStamp =  igtl::TimeStamp::New();
    if (!imageMetaElement->SetName(it->Description.c_str()))
    {
      LOG_ERROR("vtkPlusIgtlMessageCommon::PackImageMetaMessage failed: image name is too long " << it->Id);
      return PLUS_FAIL;
    }
    if (!imageMetaElement->SetDeviceName(it->Id.c_str()))
    {
      LOG_ERROR("vtkPlusIgtlMessageCommon::PackImageMetaMessage failed: device name is too long " << it->Id);
      return PLUS_FAIL;
    }
    imageMetaElement->SetModality(it->Modality.c_str());
    if (!imageMetaElement->SetPatientName(it->PatientName.c_str()))
    {
      LOG_ERROR("vtkPlusIgtlMessageCommon::PackImageMetaMessage failed: patient name is too long " << it->Id);
      return PLUS_FAIL;
    }
    if (!imageMetaElement->SetPatientID(it->PatientId.c_str()))
    {
      LOG_ERROR("vtkPlusIgtlMessageCommon::PackImageMetaMessage failed: patient id is too long " << it->Id);
      return PLUS_FAIL;
    }
    timeStamp->SetTime(it->TimeStampUtc);
    imageMetaElement->SetTimeStamp(timeStamp);
    imageMetaElement->SetSize(it->Size[0], it->Size[1], it->Size[2]);
    imageMetaElement->SetScalarType(it->ScalarType);
    imageMetaMessage->AddImageMetaElement(imageMetaElement);
  }
  imageMetaMessage->Pack();
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
#if defined(OpenIGTLink_ENABLE_VIDEOSTREAMING)
PlusStatus vtkPlusIgtlMessageCommon::PackVideoMessage(igtl::VideoMessage::Pointer videoMessage,
    igsioTrackedFrame& trackedFrame, GenericEncoder* encoder, const vtkMatrix4x4& matrix)
{
  if (videoMessage.IsNull())
  {
    LOG_ERROR("Failed to pack image message - input image message is NULL");
    return PLUS_FAIL;
  }

  if (!trackedFrame.GetImageData()->IsImageValid())
  {
    LOG_WARNING("Unable to send image message - image data is NOT valid!");
    return PLUS_FAIL;
  }

  double timestamp = trackedFrame.GetTimestamp();
  vtkImageData* frameImage = trackedFrame.GetImageData()->GetImage();

  int imageSizePixels[3] = { 0 };
  int subSizePixels[3] = { 0 };
  int subOffset[3] = { 0 };
  double imageSpacingMm[3] = { 0 };
  double imageOriginMm[3] = { 0 };
  int scalarType = PlusCommon::GetIGTLScalarPixelTypeFromVTK(trackedFrame.GetImageData()->GetVTKScalarPixelType());
  unsigned int numScalarComponents(1);
  if (trackedFrame.GetImageData()->GetNumberOfScalarComponents(numScalarComponents) == PLUS_FAIL)
  {
    LOG_ERROR("Unable to retrieve number of scalar components.");
    return PLUS_FAIL;
  }

  frameImage->GetDimensions(imageSizePixels);
  frameImage->GetSpacing(imageSpacingMm);
  frameImage->GetOrigin(imageOriginMm);
  frameImage->GetDimensions(subSizePixels);

  float spacingFloat[3] = { 0 };
  for (int i = 0; i < 3; ++i)
  {
    spacingFloat[i] = (float)imageSpacingMm[i];
  }

  igtlioVideoConverter::HeaderData headerData = igtlioVideoConverter::HeaderData();
  igtlioVideoConverter::ContentData contentData = igtlioVideoConverter::ContentData();
  contentData.videoMessage = videoMessage;
  contentData.image = frameImage;
  contentData.transform = vtkSmartPointer<vtkMatrix4x4>::New();
  contentData.transform->DeepCopy(&matrix);
  headerData.deviceName = videoMessage->GetDeviceName();

  if (!igtlioVideoConverter::toIGTL(headerData, contentData, encoder))
  {
    LOG_ERROR("Could not create video message!");
    return PLUS_FAIL;
  }

  igtl::TimeStamp::Pointer igtlFrameTime = igtl::TimeStamp::New();
  igtlFrameTime->SetTime(timestamp);
  videoMessage->SetTimeStamp(igtlFrameTime);
  videoMessage->Pack();

  return PLUS_SUCCESS;
}
#endif

//-------------------------------------------------------------------------------
PlusStatus vtkPlusIgtlMessageCommon::PackTransformMessage(igtl::TransformMessage::Pointer transformMessage,
    igsioTransformName& transformName,
    igtl::Matrix4x4& igtlMatrix,
    ToolStatus status,
    double timestamp)
{
  if (transformMessage.IsNull())
  {
    LOG_ERROR("Failed to pack transform message - input transform message is NULL");
    return PLUS_FAIL;
  }

  igtl::TimeStamp::Pointer igtlTime = igtl::TimeStamp::New();
  igtlTime->SetTime(timestamp);

  std::string strTransformName;
  transformName.GetTransformName(strTransformName);

  transformMessage->SetMetaDataElement("TransformValid", status == TOOL_OK);
  transformMessage->SetMetaDataElement("TransformStatus", IANA_TYPE_US_ASCII, igsioCommon::ConvertToolStatusToString(status));
  transformMessage->SetMatrix(igtlMatrix);
  transformMessage->SetTimeStamp(igtlTime);
  transformMessage->SetDeviceName(strTransformName.c_str());
  transformMessage->Pack();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIgtlMessageCommon::PackPolyDataMessage(igtl::PolyDataMessage::Pointer polydataMessage,
    vtkSmartPointer<vtkPolyData> polyData,
    double timestamp)
{
  if (polydataMessage.IsNull())
  {
    LOG_ERROR("Failed to pack poly data message - input polydata message is NULL");
    return PLUS_FAIL;
  }

  igtl::TimeStamp::Pointer igtlTime = igtl::TimeStamp::New();
  igtlTime->SetTime(timestamp);

  igtlioPolyDataConverter::VTKPolyDataToIGTL(polyData, polydataMessage);
  polydataMessage->SetTimeStamp(igtlTime);
  polydataMessage->Pack();

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------
PlusStatus vtkPlusIgtlMessageCommon::PackTrackingDataMessage(igtl::TrackingDataMessage::Pointer trackingDataMessage,
    const std::vector<igsioTransformName>& names,
    const vtkIGSIOTransformRepository& repository,
    double timestamp)
{
  if (trackingDataMessage.IsNull())
  {
    LOG_ERROR("Failed to pack tracking data message - input tracking data message is NULL");
    return PLUS_FAIL;
  }

  igtl::TimeStamp::Pointer igtlTime = igtl::TimeStamp::New();
  igtlTime->SetTime(timestamp);

  for (auto it = names.begin(); it != names.end(); ++it)
  {
    vtkNew<vtkMatrix4x4> vtkMat;
    ToolStatus status(TOOL_INVALID);
    if (repository.GetTransform(*it, vtkMat.GetPointer(), &status) != PLUS_SUCCESS)
    {
      LOG_ERROR("Transform " << it->From() << "To" << it->To() << " not found in repository.");
      continue;
    }
    igtl::Matrix4x4 matrix;
    if (igtlioTransformConverter::VTKToIGTLTransform(*vtkMat.GetPointer(), matrix) != 1)
    {
      LOG_ERROR("Unable to convert from VTK to IGTL transform.");
      continue;
    }

    igtl::TrackingDataElement::Pointer trackElement = igtl::TrackingDataElement::New();
    std::string shortenedName = it->GetTransformName().empty() ? "UnknownToUnknown" : it->GetTransformName().substr(0, IGTL_TDATA_LEN_NAME);
    trackElement->SetName(shortenedName.c_str());
    trackElement->SetType(igtl::TrackingDataElement::TYPE_6D);
    trackElement->SetMatrix(matrix);
    trackingDataMessage->AddTrackingDataElement(trackElement);
    trackingDataMessage->SetMetaDataElement(shortenedName + "Status", IANA_TYPE_US_ASCII,  igsioCommon::ConvertToolStatusToString(status));
  }

  trackingDataMessage->SetTimeStamp(igtlTime);
  trackingDataMessage->Pack();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIgtlMessageCommon::UnpackTrackingDataMessage(igtl::MessageHeader::Pointer headerMsg,
    igtl::Socket* socket,
    std::vector<igsioTransformName>& names,
    vtkIGSIOTransformRepository& repository,
    double& timestamp,
    int crccheck)
{
  if (headerMsg.IsNull())
  {
    LOG_ERROR("Unable to unpack transform message - header message is NULL!");
    return PLUS_FAIL;
  }

  if (socket == NULL)
  {
    LOG_ERROR("Unable to unpack transform message - socket is NULL!");
    return PLUS_FAIL;
  }

  igtl::TrackingDataMessage::Pointer tdMsg = igtl::TrackingDataMessage::New();
  tdMsg->SetMessageHeader(headerMsg);
  tdMsg->InitBuffer();

  socket->Receive(tdMsg->GetBufferBodyPointer(), tdMsg->GetBufferBodySize());

  int c = tdMsg->Unpack(crccheck);
  if (!(c & igtl::MessageHeader::UNPACK_BODY))
  {
    LOG_ERROR("Couldn't receive tracking data message from server!");
    return PLUS_FAIL;
  }

  // if CRC check is OK. Read transform data.
  int numberOfTools = tdMsg->GetNumberOfTrackingDataElements();

  if (numberOfTools > 1)
  {
    LOG_INFO("Found " << numberOfTools << " tools");
  }

  for (int i = 0; i < numberOfTools; ++i)
  {
    igtl::TrackingDataElement::Pointer currentTrackingData;
    tdMsg->GetTrackingDataElement(i, currentTrackingData);
    std::string name = currentTrackingData->GetName();

    igtl::Matrix4x4 igtlMatrix;
    currentTrackingData->GetMatrix(igtlMatrix);

    ToolStatus status(TOOL_UNKNOWN);
    std::string statusStr;
    if (tdMsg->GetMetaDataElement(std::string(currentTrackingData->GetName()) + "Status", statusStr))
    {
      status = igsioCommon::ConvertStringToToolStatus(statusStr);
    }
    vtkSmartPointer<vtkMatrix4x4> mat = vtkSmartPointer<vtkMatrix4x4>::New();
    if (igtlioTransformConverter::IGTLToVTKTransform(igtlMatrix, mat) != 1)
    {
      LOG_ERROR("Unable to unpack transform message - cannot convert from IGTL to VTK");
      continue;
    }
    igsioTransformName transName(name);
    if (!transName.IsValid())
    {
      LOG_ERROR("Invalid transform name sent via TDATA message. Skipping.");
      continue;
    }
    names.push_back(name);
    repository.SetTransform(transName, mat, status);
  }

  // Get timestamp
  igtl::TimeStamp::Pointer igtlTimestamp = igtl::TimeStamp::New();
  tdMsg->GetTimeStamp(igtlTimestamp);
  timestamp = igtlTimestamp->GetTimeStamp();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIgtlMessageCommon::UnpackTransformMessage(igtl::MessageHeader::Pointer headerMsg,
    igtl::Socket* socket,
    vtkMatrix4x4* transformMatrix,
    ToolStatus& toolStatus,
    std::string& transformName,
    double& timestamp,
    int crccheck)
{
  if (headerMsg.IsNull())
  {
    LOG_ERROR("Unable to unpack transform message - header message is NULL!");
    return PLUS_FAIL;
  }

  if (socket == NULL)
  {
    LOG_ERROR("Unable to unpack transform message - socket is NULL!");
    return PLUS_FAIL;
  }

  if (transformMatrix == NULL)
  {
    LOG_ERROR("Unable to unpack transform message - matrix is NULL!");
    return PLUS_FAIL;
  }

  igtl::TransformMessage::Pointer transMsg = dynamic_cast<igtl::TransformMessage*>(headerMsg.GetPointer());
  if (transMsg.IsNull())
  {
    transMsg = igtl::TransformMessage::New();
  }
  transMsg->SetMessageHeader(headerMsg);
  transMsg->AllocateBuffer();

  socket->Receive(transMsg->GetBufferBodyPointer(), transMsg->GetBufferBodySize());

  int c = transMsg->Unpack(crccheck);
  if (!(c & igtl::MessageHeader::UNPACK_BODY))
  {
    LOG_ERROR("Couldn't receive transform message from server!");
    return PLUS_FAIL;
  }
  // if CRC check is OK. Read transform data.
  igtl::Matrix4x4 igtlMatrix;
  igtl::IdentityMatrix(igtlMatrix);
  transMsg->GetMatrix(igtlMatrix);

  // convert igtl matrix to vtk matrix
  vtkSmartPointer<vtkMatrix4x4> mat = vtkSmartPointer<vtkMatrix4x4>::New();
  if (igtlioTransformConverter::IGTLToVTKTransform(igtlMatrix, transformMatrix) != 1)
  {
    LOG_ERROR("Unable to unpack transform message - cannot convert from IGTL to VTK");
    return PLUS_FAIL;
  }

  // Get timestamp
  igtl::TimeStamp::Pointer igtlTimestamp = igtl::TimeStamp::New();
  transMsg->GetTimeStamp(igtlTimestamp);
  timestamp = igtlTimestamp->GetTimeStamp();

  // Status
  std::string statusStr;
  toolStatus = TOOL_UNKNOWN;
  if (transMsg->GetMetaDataElement("TransformStatus", statusStr))
  {
    toolStatus = igsioCommon::ConvertStringToToolStatus(statusStr);
  }

  // Get transform name
  transformName = transMsg->GetDeviceName();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIgtlMessageCommon::PackPositionMessage(igtl::PositionMessage::Pointer positionMessage,
    igsioTransformName& transformName,
    ToolStatus status,
    float position[3],
    float quaternion[4],
    double timestamp)
{
  if (positionMessage.IsNull())
  {
    LOG_ERROR("Failed to pack position message - input position message is NULL");
    return PLUS_FAIL;
  }

  igtl::TimeStamp::Pointer igtlTime = igtl::TimeStamp::New();
  igtlTime->SetTime(timestamp);

  std::string strTransformName;
  transformName.GetTransformName(strTransformName);

  positionMessage->SetPosition(position);
  positionMessage->SetQuaternion(quaternion);
  positionMessage->SetMetaDataElement("TransformStatus", IANA_TYPE_US_ASCII, igsioCommon::ConvertToolStatusToString(status));
  positionMessage->SetTimeStamp(igtlTime);
  positionMessage->SetDeviceName(strTransformName.c_str());
  positionMessage->Pack();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIgtlMessageCommon::UnpackPositionMessage(igtl::MessageHeader::Pointer headerMsg,
    igtl::Socket* socket,
    vtkMatrix4x4* transformMatrix,
    std::string& transformName,
    ToolStatus& toolStatus,
    double& timestamp,
    int crccheck)
{
  if (headerMsg.IsNull())
  {
    LOG_ERROR("Unable to unpack position message - header message is NULL!");
    return PLUS_FAIL;
  }

  if (socket == NULL)
  {
    LOG_ERROR("Unable to unpack position message - socket is NULL!");
    return PLUS_FAIL;
  }

  if (transformMatrix == NULL)
  {
    LOG_ERROR("Unable to unpack position message - transformMatrix is NULL!");
    return PLUS_FAIL;
  }

  igtl::PositionMessage::Pointer posMsg = dynamic_cast<igtl::PositionMessage*>(headerMsg.GetPointer());
  if (posMsg.IsNull())
  {
    posMsg = igtl::PositionMessage::New();
  }
  posMsg->SetMessageHeader(headerMsg);
  posMsg->AllocateBuffer();

  socket->Receive(posMsg->GetBufferBodyPointer(), posMsg->GetBufferBodySize());

  //  If crccheck is specified it performs CRC check and unpack the data only if CRC passes
  int c = posMsg->Unpack(crccheck);
  if (!(c & igtl::MessageHeader::UNPACK_BODY))
  {
    LOG_ERROR("Couldn't receive position message from server!");
    return PLUS_FAIL;
  }

  // if CRC check is OK. Read position data.
  float position[3] = {0};
  posMsg->GetPosition(position);

  float quaternion[4] = {0, 0, 0, 1};
  posMsg->GetQuaternion(quaternion);

  // Orientation
  igtl::Matrix4x4 igtlMatrix;
  igtl::IdentityMatrix(igtlMatrix);
  igtl::QuaternionToMatrix(quaternion, igtlMatrix);

  // Position
  transformMatrix->SetElement(0, 3, igtlMatrix[0][3]);
  transformMatrix->SetElement(1, 3, igtlMatrix[1][3]);
  transformMatrix->SetElement(2, 3, igtlMatrix[2][3]);

  // Status
  std::string statusStr;
  toolStatus = TOOL_UNKNOWN;
  if (posMsg->GetMetaDataElement("Status", statusStr))
  {
    toolStatus = igsioCommon::ConvertStringToToolStatus(statusStr);
  }

  // Get timestamp
  igtl::TimeStamp::Pointer igtlTimestamp = igtl::TimeStamp::New();
  posMsg->GetTimeStamp(igtlTimestamp);
  timestamp = igtlTimestamp->GetTimeStamp();

  // Get transform name
  transformName = posMsg->GetDeviceName();

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------
PlusStatus vtkPlusIgtlMessageCommon::PackStringMessage(igtl::StringMessage::Pointer stringMessage,
    const char* stringName,
    const char* stringValue,
    double timestamp)
{
  if (stringMessage.IsNull())
  {
    LOG_ERROR("Failed to pack string message - input string message is NULL");
    return PLUS_FAIL;
  }

  igtl::TimeStamp::Pointer igtlTime = igtl::TimeStamp::New();
  igtlTime->SetTime(timestamp);

  stringMessage->SetString(stringValue);   // assume default encoding
  stringMessage->SetTimeStamp(igtlTime);
  stringMessage->SetDeviceName(stringName);
  stringMessage->Pack();

  return PLUS_SUCCESS;
}