/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "PlusTrackedFrame.h"
#include "PlusVideoFrame.h"
#include "vtkPlusIgtlMessageCommon.h"
#include "vtkPlusTrackedFrameList.h"
#include "vtkPlusTransformRepository.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkTransform.h>

// OpenIGTLink includes
#include <igtl_tdata.h>

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
// static
PlusStatus vtkPlusIgtlMessageCommon::GetIgtlMatrix(igtl::Matrix4x4& igtlMatrix, vtkPlusTransformRepository* transformRepository, PlusTransformName& transformName)
{
  igtl::IdentityMatrix(igtlMatrix);

  if (transformRepository == NULL || !transformName.IsValid())
  {
    LOG_ERROR("GetIgtlMatrix failed: Transform repository or transform name is invalid");
    return PLUS_FAIL;
  }

  bool valid = false;
  vtkSmartPointer<vtkMatrix4x4> vtkMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  if (transformRepository->GetTransform(transformName, vtkMatrix, &valid) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to get transform from transform repository (" << transformName.From() << " to " << transformName.To() << ")");
    return PLUS_FAIL;
  }

  if (!valid)
  {
    LOG_WARNING("Skipped transformation matrix - Invalid transform in the transform repository (" << transformName.From() << " to " << transformName.To() << ")");
    return PLUS_FAIL;
  }

  // Copy vtk matrix to igt matrix
  for (int r = 0; r < 4; ++r)
  {
    for (int c = 0; c < 4; ++c)
    {
      igtlMatrix[r][c] = vtkMatrix->GetElement(r, c);
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// static
PlusStatus vtkPlusIgtlMessageCommon::PackTrackedFrameMessage(igtl::PlusTrackedFrameMessage::Pointer trackedFrameMessage, PlusTrackedFrame& trackedFrame, vtkSmartPointer<vtkMatrix4x4> embeddedImageTransform, const std::vector<PlusTransformName>& requestedTransforms)
{
  if (trackedFrameMessage.IsNull())
  {
    LOG_ERROR("Failed to pack tracked frame message - input tracked frame message is NULL"); ;
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
// static
PlusStatus vtkPlusIgtlMessageCommon::UnpackTrackedFrameMessage(igtl::MessageHeader::Pointer headerMsg, igtl::Socket* socket, PlusTrackedFrame& trackedFrame, const PlusTransformName& embeddedTransformName, int crccheck)
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
    trackedFrame.SetCustomFrameTransform(embeddedTransformName, trackedFrameMsg->GetEmbeddedImageTransform());
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// static
PlusStatus vtkPlusIgtlMessageCommon::PackUsMessage(igtl::PlusUsMessage::Pointer usMessage, PlusTrackedFrame& trackedFrame)
{
  if (usMessage.IsNull())
  {
    LOG_ERROR("Failed to pack US message - input US message is NULL"); ;
    return PLUS_FAIL;
  }

  PlusStatus status = usMessage->SetTrackedFrame(trackedFrame);
  usMessage->Pack();

  return status;
}

//----------------------------------------------------------------------------
// static
PlusStatus vtkPlusIgtlMessageCommon::UnpackUsMessage(igtl::MessageHeader::Pointer headerMsg, igtl::Socket* socket, PlusTrackedFrame& trackedFrame, int crccheck)
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
// static
PlusStatus vtkPlusIgtlMessageCommon::PackVideoMessage(igtl::VideoMessage::Pointer videoMessage, PlusTrackedFrame& trackedFrame, H264Encoder* videoStreamEncoder)
{
  if (videoStreamEncoder == NULL)
  {
    LOG_ERROR("Failed to pack video message - input video message encoder is NULL");;
    return PLUS_FAIL;
  }

  if (videoMessage.IsNull())
  {
      LOG_ERROR("Failed to pack video message - input video message is NULL");;
      return PLUS_FAIL;
  }

  if (!trackedFrame.GetImageData()->IsImageValid())
  {
      LOG_WARNING("Unable to send video message - video data is NOT valid!");
      return PLUS_FAIL;
  }

  double timestamp = trackedFrame.GetTimestamp();
  vtkImageData* frameImage = trackedFrame.GetImageData()->GetImage();

  igtl::TimeStamp::Pointer igtlFrameTime = igtl::TimeStamp::New();
  igtlFrameTime->SetTime(timestamp);

  int imageSizePixels[3] = { 0 };  double imageSpacingMm[3] = { 0 };
  int scalarType = PlusVideoFrame::GetIGTLScalarPixelTypeFromVTK(trackedFrame.GetImageData()->GetVTKScalarPixelType());
  int numScalarComponents = trackedFrame.GetImageData()->GetNumberOfScalarComponents();

  frameImage->GetDimensions(imageSizePixels);
  frameImage->GetSpacing(imageSpacingMm);

  float spacingFloat[3] = { 0 };
  for (int i = 0; i < 3; ++i)
  {
      spacingFloat[i] = (float)imageSpacingMm[i];
  }
  unsigned char* vtkImagePointer = new unsigned char[imageSizePixels[0] * imageSizePixels[1]*3/2];
  memset(vtkImagePointer, 0, imageSizePixels[0] * imageSizePixels[1] * 3 / 2);
  memcpy(vtkImagePointer, frameImage->GetScalarPointer(), imageSizePixels[0] * imageSizePixels[1]);
  int iSourceWidth = imageSizePixels[0];
  int iSourceHeight = imageSizePixels[1];
  SSourcePicture* pSrcPic = new SSourcePicture();
  pSrcPic->iColorFormat = videoFormatI420; // currently only format 420 is supported.
  pSrcPic->uiTimeStamp = 0;
  pSrcPic->iPicWidth  = imageSizePixels[0];
  pSrcPic->iPicHeight = imageSizePixels[1];
  if (videoStreamEncoder->GetPicHeight() != iSourceHeight
    || videoStreamEncoder->GetPicWidth() != iSourceWidth)
  {
    videoStreamEncoder->SetPicHeight(iSourceHeight);
    videoStreamEncoder->SetPicWidth(iSourceWidth);
    videoStreamEncoder->InitializeEncoder();
  }
  pSrcPic->pData[0] = vtkImagePointer;
  pSrcPic->pData[1] = pSrcPic->pData[0] + (iSourceWidth * iSourceHeight);
  pSrcPic->pData[2] = pSrcPic->pData[1] + (iSourceWidth * iSourceHeight >> 2);
  bool isGrayImage = true;
  int iEncFrames = videoStreamEncoder->EncodeSingleFrameIntoVideoMSG(pSrcPic, videoMessage, isGrayImage);
  delete vtkImagePointer;
  vtkImagePointer = NULL;
  if (iEncFrames == 0)
    return PLUS_SUCCESS;
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
// static
PlusStatus vtkPlusIgtlMessageCommon::PackImageMessage(igtl::ImageMessage::Pointer imageMessage, PlusTrackedFrame& trackedFrame, igtl::Matrix4x4& igtlMatrix)
{
  if (imageMessage.IsNull())
  {
    LOG_ERROR("Failed to pack image message - input image message is NULL"); ;
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

  int imageSizePixels[3] = {0}, subSizePixels[3] = {0}, subOffset[3] = {0};
  double imageSpacingMm[3] = {0};
  int scalarType = PlusVideoFrame::GetIGTLScalarPixelTypeFromVTK(trackedFrame.GetImageData()->GetVTKScalarPixelType());
  int numScalarComponents = trackedFrame.GetImageData()->GetNumberOfScalarComponents();

  frameImage->GetDimensions(imageSizePixels);
  frameImage->GetSpacing(imageSpacingMm);
  frameImage->GetDimensions(subSizePixels);

  float spacingFloat[3] = {0};
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
  // VTK and Plus: corner image origin
  // OpenIGTLink image message: center image origin

  int* isize = imageSizePixels;
  double* spacing = imageSpacingMm;
  vtkSmartPointer< vtkMatrix4x4 > rtimgTransform = vtkSmartPointer< vtkMatrix4x4 >::New();
  for (int row = 0; row < 4; ++ row)
  {
    for (int col = 0; col < 4; ++ col)
    {
      rtimgTransform->SetElement(row, col, igtlMatrix[ row ][ col ]);
    }
  }

  //--- Adopted from OpenIGTLinkIF\MRML\vtkIGTLToMRMLImage.cxx ---//

  float ntx = rtimgTransform->Element[0][0] / (float)spacing[0];
  float nty = rtimgTransform->Element[1][0] / (float)spacing[0];
  float ntz = rtimgTransform->Element[2][0] / (float)spacing[0];
  float nsx = rtimgTransform->Element[0][1] / (float)spacing[1];
  float nsy = rtimgTransform->Element[1][1] / (float)spacing[1];
  float nsz = rtimgTransform->Element[2][1] / (float)spacing[1];
  float nnx = rtimgTransform->Element[0][2] / (float)spacing[2];
  float nny = rtimgTransform->Element[1][2] / (float)spacing[2];
  float nnz = rtimgTransform->Element[2][2] / (float)spacing[2];
  float px  = rtimgTransform->Element[0][3];
  float py  = rtimgTransform->Element[1][3];
  float pz  = rtimgTransform->Element[2][3];

  // Shift the center
  // NOTE: The center of the image should be shifted due to different
  // definitions of image origin between VTK (Slicer) and OpenIGTLink;
  // OpenIGTLink image has its origin at the center, while VTK image
  // has one at the corner.

  float hfovi = (float)spacing[0] * (float)(isize[0] - 1) / 2.0;
  float hfovj = (float)spacing[1] * (float)(isize[1] - 1) / 2.0;
  float hfovk = (float)spacing[2] * (float)(isize[2] - 1) / 2.0;

  float cx = ntx * hfovi + nsx * hfovj + nnx * hfovk;
  float cy = nty * hfovi + nsy * hfovj + nny * hfovk;
  float cz = ntz * hfovi + nsz * hfovj + nnz * hfovk;

  px = px + cx;
  py = py + cy;
  pz = pz + cz;

  igtl::Matrix4x4 matrix; // Image origin and orientation matrix
  matrix[0][0] = ntx;
  matrix[1][0] = nty;
  matrix[2][0] = ntz;
  matrix[0][1] = nsx;
  matrix[1][1] = nsy;
  matrix[2][1] = nsz;
  matrix[0][2] = nnx;
  matrix[1][2] = nny;
  matrix[2][2] = nnz;
  matrix[0][3] = px;
  matrix[1][3] = py;
  matrix[2][3] = pz;

  //////

  imageMessage->SetMatrix(matrix);
  imageMessage->SetTimeStamp(igtlFrameTime);

  imageMessage->Pack();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// static
PlusStatus vtkPlusIgtlMessageCommon::UnpackImageMessage(igtl::MessageHeader::Pointer headerMsg, igtl::Socket* socket, PlusTrackedFrame& trackedFrame, const PlusTransformName& embeddedTransformName, int crccheck)
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

  // Set scalar pixel type
  PlusCommon::VTKScalarPixelType pixelType = PlusVideoFrame::GetVTKScalarPixelTypeFromIGTL(imgMsg->GetScalarType());
  PlusVideoFrame frame;
  if (frame.AllocateFrame(imgSize, pixelType, imgMsg->GetNumComponents()) != PLUS_SUCCESS)
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
    // Save the transform that is embedded in the IMAGE message into the tracked frame
    // igtlMatrix origin is in the image center
    // vtkMatrix origin is in the image corner
    vtkSmartPointer<vtkMatrix4x4> igtlMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    {
      igtl::Matrix4x4 igtlMatrixSource;
      imgMsg->GetMatrix(igtlMatrixSource);
      for (int row = 0; row < 4; ++ row)
      {
        for (int col = 0; col < 4; ++ col)
        {
          igtlMatrix->SetElement(row, col, igtlMatrixSource[row][col]);
        }
      }
    }
    vtkSmartPointer<vtkTransform> igtlToVtkTransform = vtkSmartPointer<vtkTransform>::New();
    igtlToVtkTransform->Translate(-imgSize[ 0 ] / 2.0, -imgSize[ 1 ] / 2.0, -imgSize[ 2 ] / 2.0);
    vtkSmartPointer< vtkMatrix4x4 > vtkMatrix = vtkSmartPointer< vtkMatrix4x4 >::New();
    vtkMatrix4x4::Multiply4x4(igtlMatrix, igtlToVtkTransform->GetMatrix(), vtkMatrix);
    trackedFrame.SetCustomFrameTransform(embeddedTransformName, vtkMatrix);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// static
PlusStatus vtkPlusIgtlMessageCommon::PackImageMessage(igtl::ImageMessage::Pointer imageMessage, vtkImageData* volume, vtkMatrix4x4* volumeToReferenceTransform, double timestamp)
{
  if (imageMessage.IsNull())
  {
    LOG_ERROR("Failed to pack image message - input image message is NULL"); ;
    return PLUS_FAIL;
  }

  int volumeSizePixels[3] = {0};
  volume->GetDimensions(volumeSizePixels);
  imageMessage->SetDimensions(volumeSizePixels);

  int subSizePixels[3] = {0};
  volume->GetDimensions(subSizePixels);
  int subOffset[3] = {0};
  imageMessage->SetSubVolume(subSizePixels, subOffset);

  double volumeSpacingMm[3] = {0};
  volume->GetSpacing(volumeSpacingMm);
  float spacingFloat[3] = {0};
  for (int i = 0; i < 3; ++ i)
  {
    spacingFloat[ i ] = (float)volumeSpacingMm[ i ];
  }
  imageMessage->SetSpacing(spacingFloat);

  double volumeOriginMm[3] = {0};
  volume->GetOrigin(volumeOriginMm);
  // imageMessage->SetOrigin() is not used, because origin and normal is set later by imageMessage->SetMatrix()

  int scalarType = PlusVideoFrame::GetIGTLScalarPixelTypeFromVTK(volume->GetScalarType());
  imageMessage->SetScalarType(scalarType);

  imageMessage->SetEndian(igtl_is_little_endian() ? igtl::ImageMessage::ENDIAN_LITTLE : igtl::ImageMessage::ENDIAN_BIG);

  imageMessage->AllocateScalars();

  unsigned char* igtlImagePointer = (unsigned char*)(imageMessage->GetScalarPointer());
  unsigned char* vtkImagePointer = (unsigned char*)(volume->GetScalarPointer());

  memcpy(igtlImagePointer, vtkImagePointer, imageMessage->GetImageSize());

  ////// Adopted from OpenIGTLinkIF\MRML\vtkIGTLToMRMLImage.cxx

  vtkSmartPointer<vtkMatrix4x4> ijkToVolumeTransform = vtkSmartPointer<vtkMatrix4x4>::New();
  ijkToVolumeTransform->Identity();
  ijkToVolumeTransform->Element[0][0] = volumeSpacingMm[0];
  ijkToVolumeTransform->Element[1][1] = volumeSpacingMm[1];
  ijkToVolumeTransform->Element[2][2] = volumeSpacingMm[2];
  ijkToVolumeTransform->Element[0][3] = volumeOriginMm[0];
  ijkToVolumeTransform->Element[1][3] = volumeOriginMm[1];
  ijkToVolumeTransform->Element[2][3] = volumeOriginMm[2];
  vtkSmartPointer<vtkMatrix4x4> ijkToReferenceTransform = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkMatrix4x4::Multiply4x4(volumeToReferenceTransform, ijkToVolumeTransform, ijkToReferenceTransform);
  float ntx = ijkToReferenceTransform->Element[0][0] / (float)volumeSpacingMm[0];
  float nty = ijkToReferenceTransform->Element[1][0] / (float)volumeSpacingMm[0];
  float ntz = ijkToReferenceTransform->Element[2][0] / (float)volumeSpacingMm[0];
  float nsx = ijkToReferenceTransform->Element[0][1] / (float)volumeSpacingMm[1];
  float nsy = ijkToReferenceTransform->Element[1][1] / (float)volumeSpacingMm[1];
  float nsz = ijkToReferenceTransform->Element[2][1] / (float)volumeSpacingMm[1];
  float nnx = ijkToReferenceTransform->Element[0][2] / (float)volumeSpacingMm[2];
  float nny = ijkToReferenceTransform->Element[1][2] / (float)volumeSpacingMm[2];
  float nnz = ijkToReferenceTransform->Element[2][2] / (float)volumeSpacingMm[2];
  float px  = ijkToReferenceTransform->Element[0][3];
  float py  = ijkToReferenceTransform->Element[1][3];
  float pz  = ijkToReferenceTransform->Element[2][3];

  // Shift the center
  // NOTE: The center of the image should be shifted due to different
  // definitions of image origin between VTK (Slicer) and OpenIGTLink;
  // OpenIGTLink image has its origin at the center, while VTK image
  // has one at the corner.
  float hfovi = (float)volumeSpacingMm[0] * (float)(volumeSizePixels[0] - 1) / 2.0;
  float hfovj = (float)volumeSpacingMm[1] * (float)(volumeSizePixels[1] - 1) / 2.0;
  float hfovk = (float)volumeSpacingMm[2] * (float)(volumeSizePixels[2] - 1) / 2.0;
  float cx = ntx * hfovi + nsx * hfovj + nnx * hfovk;
  float cy = nty * hfovi + nsy * hfovj + nny * hfovk;
  float cz = ntz * hfovi + nsz * hfovj + nnz * hfovk;
  px = px + cx;
  py = py + cy;
  pz = pz + cz;

  igtl::Matrix4x4 matrix; // Image origin and orientation matrix
  matrix[0][0] = ntx;
  matrix[1][0] = nty;
  matrix[2][0] = ntz;
  matrix[0][1] = nsx;
  matrix[1][1] = nsy;
  matrix[2][1] = nsz;
  matrix[0][2] = nnx;
  matrix[1][2] = nny;
  matrix[2][2] = nnz;
  matrix[0][3] = px;
  matrix[1][3] = py;
  matrix[2][3] = pz;

  imageMessage->SetMatrix(matrix);

  ///////

  igtl::TimeStamp::Pointer igtlTime = igtl::TimeStamp::New();
  igtlTime->SetTime(timestamp);
  imageMessage->SetTimeStamp(igtlTime);

  imageMessage->Pack();

  return PLUS_SUCCESS;

}

//----------------------------------------------------------------------------
//static
//--------------------------------------------------------------------------
PlusStatus vtkPlusIgtlMessageCommon::PackImageMetaMessage(igtl::ImageMetaMessage::Pointer imageMetaMessage, PlusCommon::ImageMetaDataList& imageMetaDataList)
{
  if (imageMetaMessage.IsNull())
  {
    LOG_ERROR("Failed to pack image message - input image message is NULL"); ;
    return PLUS_FAIL;
  }
  for (PlusCommon::ImageMetaDataList::iterator it = imageMetaDataList.begin(); it != imageMetaDataList.end(); it++)
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
//-------------------------------------------------------------------------------
// static
PlusStatus vtkPlusIgtlMessageCommon::PackTransformMessage(igtl::TransformMessage::Pointer transformMessage, PlusTransformName& transformName,
    igtl::Matrix4x4& igtlMatrix, double timestamp)
{
  if (transformMessage.IsNull())
  {
    LOG_ERROR("Failed to pack transform message - input transform message is NULL"); ;
    return PLUS_FAIL;
  }

  igtl::TimeStamp::Pointer igtlTime = igtl::TimeStamp::New();
  igtlTime->SetTime(timestamp);

  std::string strTransformName;
  transformName.GetTransformName(strTransformName);

  transformMessage->SetMatrix(igtlMatrix);
  transformMessage->SetTimeStamp(igtlTime);
  transformMessage->SetDeviceName(strTransformName.c_str());
  transformMessage->Pack();

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------
// static
PlusStatus vtkPlusIgtlMessageCommon::PackTrackingDataMessage(igtl::TrackingDataMessage::Pointer trackingDataMessage, const std::map<std::string, vtkSmartPointer<vtkMatrix4x4> >& transforms, double timestamp)
{
  if (trackingDataMessage.IsNull())
  {
    LOG_ERROR("Failed to pack tracking data message - input tracking data message is NULL"); ;
    return PLUS_FAIL;
  }

  igtl::TimeStamp::Pointer igtlTime = igtl::TimeStamp::New();
  igtlTime->SetTime(timestamp);

  for (std::map<std::string, vtkSmartPointer<vtkMatrix4x4> >::const_iterator transformIterator = transforms.begin(); transformIterator != transforms.end(); ++transformIterator)
  {
    igtl::Matrix4x4 matrix;
    for (int i = 0; i < 4; ++i)
    {
      for (int j = 0; j < 4; ++j)
      {
        matrix[i][j] = transformIterator->second->GetElement(i, j);
      }
    }

    igtl::TrackingDataElement::Pointer trackElement = igtl::TrackingDataElement::New();
    std::string shortenedName = transformIterator->first.empty() ? "UnknownToUnknown" : transformIterator->first.substr(0, IGTL_TDATA_LEN_NAME);
    trackElement->SetName(shortenedName.c_str());
    trackElement->SetType(igtl::TrackingDataElement::TYPE_6D);
    trackElement->SetMatrix(matrix);
    trackingDataMessage->AddTrackingDataElement(trackElement);
  }

  trackingDataMessage->SetTimeStamp(igtlTime);
  trackingDataMessage->Pack();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// static
PlusStatus vtkPlusIgtlMessageCommon::UnpackTrackingDataMessage(igtl::MessageHeader::Pointer headerMsg, igtl::Socket* socket,
    std::map<std::string, vtkSmartPointer<vtkMatrix4x4> >& outTransforms, double& timestamp, int crccheck)
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
    igtl::IdentityMatrix(igtlMatrix);
    currentTrackingData->GetMatrix(igtlMatrix);

    // convert igtl matrix to vtk matrix
    vtkSmartPointer<vtkMatrix4x4> mat = vtkSmartPointer<vtkMatrix4x4>::New();
    for (int r = 0; r < 4; r++)
    {
      for (int c = 0; c < 4; c++)
      {
        mat->SetElement(r, c, igtlMatrix[r][c]);
      }
    }

    outTransforms[name] = mat;
  }

  // Get timestamp
  igtl::TimeStamp::Pointer igtlTimestamp = igtl::TimeStamp::New();
  tdMsg->GetTimeStamp(igtlTimestamp);
  timestamp = igtlTimestamp->GetTimeStamp();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// static
PlusStatus vtkPlusIgtlMessageCommon::UnpackTransformMessage(igtl::MessageHeader::Pointer headerMsg, igtl::Socket* socket,
    vtkMatrix4x4* transformMatrix, std::string& transformName, double& timestamp, int crccheck)
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
  for (int r = 0; r < 4; r++)
  {
    for (int c = 0; c < 4; c++)
    {
      transformMatrix->SetElement(r, c, igtlMatrix[r][c]);
    }
  }

  // Get timestamp
  igtl::TimeStamp::Pointer igtlTimestamp = igtl::TimeStamp::New();
  transMsg->GetTimeStamp(igtlTimestamp);
  timestamp = igtlTimestamp->GetTimeStamp();

  // Get transform name
  transformName = transMsg->GetDeviceName();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// static
PlusStatus vtkPlusIgtlMessageCommon::PackPositionMessage(igtl::PositionMessage::Pointer positionMessage, PlusTransformName& transformName,
    float position[3], float quaternion[4], double timestamp)
{
  if (positionMessage.IsNull())
  {
    LOG_ERROR("Failed to pack position message - input position message is NULL"); ;
    return PLUS_FAIL;
  }

  igtl::TimeStamp::Pointer igtlTime = igtl::TimeStamp::New();
  igtlTime->SetTime(timestamp);

  std::string strTransformName;
  transformName.GetTransformName(strTransformName);

  positionMessage->SetPosition(position);
  positionMessage->SetQuaternion(quaternion);
  positionMessage->SetTimeStamp(igtlTime);
  positionMessage->SetDeviceName(strTransformName.c_str());
  positionMessage->Pack();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// static
PlusStatus vtkPlusIgtlMessageCommon::UnpackPositionMessage(igtl::MessageHeader::Pointer headerMsg, igtl::Socket* socket,
    vtkMatrix4x4* transformMatrix, std::string& transformName, double& timestamp, int crccheck)
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

  // Get timestamp
  igtl::TimeStamp::Pointer igtlTimestamp = igtl::TimeStamp::New();
  posMsg->GetTimeStamp(igtlTimestamp);
  timestamp = igtlTimestamp->GetTimeStamp();

  // Get transform name
  transformName = posMsg->GetDeviceName();

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------
// static
PlusStatus vtkPlusIgtlMessageCommon::PackStringMessage(igtl::StringMessage::Pointer stringMessage, const char* stringName, const char* stringValue, double timestamp)
{
  if (stringMessage.IsNull())
  {
    LOG_ERROR("Failed to pack string message - input string message is NULL"); ;
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

