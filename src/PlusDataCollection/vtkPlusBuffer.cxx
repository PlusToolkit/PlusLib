/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "igsioMath.h"
#include "igsioTrackedFrame.h"
#include "vtkPlusBuffer.h"
#include "vtkPlusDevice.h"
#include "vtkPlusSequenceIO.h"
#include "vtkIGSIOTrackedFrameList.h"

// VTK includes
#include <vtkDoubleArray.h>
#include <vtkImageData.h>
#include <vtkIntArray.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkUnsignedLongLongArray.h>

// vtkAddon includes
#include <vtkStreamingVolumeCodec.h>

static const double NEGLIGIBLE_TIME_DIFFERENCE = 0.00001; // in seconds, used for comparing between exact timestamps
static const double ANGLE_INTERPOLATION_WARNING_THRESHOLD_DEG = 10; // if the interpolated orientation differs from both the interpolated orientation by more than this threshold then display a warning

vtkStandardNewMacro(vtkPlusBuffer);

#define LOCAL_LOG_ERROR(msg) \
{ \
  std::ostringstream msgStream; \
  if( this->DescriptiveName == NULL ) \
  { \
    msgStream << " " << msg << std::ends; \
  } \
  else \
  { \
    msgStream << this->DescriptiveName << ": " << msg << std::ends; \
  } \
  std::string finalStr(msgStream.str()); \
  LOG_ERROR(finalStr); \
}
#define LOCAL_LOG_WARNING(msg) \
{ \
  std::ostringstream msgStream; \
  if( this->DescriptiveName == NULL ) \
{ \
  msgStream << " " << msg << std::ends; \
} \
  else \
{ \
  msgStream << this->DescriptiveName << ": " << msg << std::ends; \
} \
  std::string finalStr(msgStream.str()); \
  LOG_WARNING(finalStr); \
}
#define LOCAL_LOG_DEBUG(msg) \
{ \
  std::ostringstream msgStream; \
  if( this->DescriptiveName == NULL ) \
{ \
  msgStream << " " << msg << std::ends; \
} \
  else \
{ \
  msgStream << this->DescriptiveName << ": " << msg << std::ends; \
} \
  std::string finalStr(msgStream.str()); \
  LOG_DEBUG(finalStr); \
}

//----------------------------------------------------------------------------
// vtkPlusBuffer
//----------------------------------------------------------------------------
vtkPlusBuffer::vtkPlusBuffer()
  : PixelType(VTK_UNSIGNED_CHAR)
  , NumberOfScalarComponents(1)
  , ImageType(US_IMG_BRIGHTNESS)
  , ImageOrientation(US_IMG_ORIENT_MF)
  , StreamBuffer(vtkPlusTimestampedCircularBuffer::New())
  , MaxAllowedTimeDifference(0.5)
  , DescriptiveName(NULL)
{
  this->FrameSize[0] = 0;
  this->FrameSize[1] = 0;
  this->FrameSize[2] = 1; // by default we assume we have a single-slice image

  // 150 is a reasonable default value, it means that we keep the last 5 secods of acquired data @30fps
  // (and last 2.5 seconds @60fps). It should be enough to have all the needed data available and
  // it does not consume too much memory, even for images.
  this->SetBufferSize(150);
}

//----------------------------------------------------------------------------
vtkPlusBuffer::~vtkPlusBuffer()
{
  if (this->StreamBuffer != NULL)
  {
    this->StreamBuffer->Delete();
    this->StreamBuffer = NULL;
  }

  this->SetDescriptiveName(nullptr);
}

//----------------------------------------------------------------------------
void vtkPlusBuffer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Frame size in pixel: " << this->GetFrameSize()[0] << "   " << this->GetFrameSize()[1] << "   " << this->GetFrameSize()[2] << std::endl;
  os << indent << "Scalar pixel type: " << vtkImageScalarTypeNameMacro(this->GetPixelType()) << std::endl;
  os << indent << "Image type: " << igsioCommon::GetStringFromUsImageType(this->GetImageType()) << std::endl;
  os << indent << "Image orientation: " << igsioCommon::GetStringFromUsImageOrientation(this->GetImageOrientation()) << std::endl;

  os << indent << "StreamBuffer: " << this->StreamBuffer << "\n";
  if (this->StreamBuffer)
  {
    this->StreamBuffer->PrintSelf(os, indent.GetNextIndent());
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBuffer::AllocateMemoryForFrames()
{
  igsioLockGuard<StreamItemCircularBuffer> dataBufferGuardedLock(this->StreamBuffer);
  PlusStatus result = PLUS_SUCCESS;

  for (int i = 0; i < this->StreamBuffer->GetBufferSize(); ++i)
  {
    if (!this->StreamBuffer->GetBufferItemPointerFromBufferIndex(i)->GetFrame().IsFrameEncoded())
    {
      if (this->StreamBuffer->GetBufferItemPointerFromBufferIndex(i)->GetFrame().AllocateFrame(this->GetFrameSize(), this->GetPixelType(), this->GetNumberOfScalarComponents()) != PLUS_SUCCESS)
      {
        LOCAL_LOG_ERROR("Failed to allocate memory for frame " << i);
        result = PLUS_FAIL;
      }
    }
  }
  return result;
}

//----------------------------------------------------------------------------
void vtkPlusBuffer::SetLocalTimeOffsetSec(double offsetSec)
{
  this->StreamBuffer->SetLocalTimeOffsetSec(offsetSec);
}

//----------------------------------------------------------------------------
double vtkPlusBuffer::GetLocalTimeOffsetSec()
{
  return this->StreamBuffer->GetLocalTimeOffsetSec();
}

//----------------------------------------------------------------------------
int vtkPlusBuffer::GetBufferSize()
{
  return this->StreamBuffer->GetBufferSize();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBuffer::SetBufferSize(int bufsize)
{
  if (bufsize < 0)
  {
    LOCAL_LOG_ERROR("Invalid buffer size requested: " << bufsize);
    return PLUS_FAIL;
  }
  if (this->StreamBuffer->GetBufferSize() == bufsize)
  {
    // no change
    return PLUS_SUCCESS;
  }

  PlusStatus result = PLUS_SUCCESS;
  if (this->StreamBuffer->SetBufferSize(bufsize) != PLUS_SUCCESS)
  {
    result = PLUS_FAIL;
  }
  if (this->AllocateMemoryForFrames() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  return result;
}

//----------------------------------------------------------------------------
bool vtkPlusBuffer::CheckFrameFormat(const FrameSizeType& frameSizeInPx, igsioCommon::VTKScalarPixelType pixelType, US_IMAGE_TYPE imgType, int numberOfScalarComponents)
{
  // don't add a frame if it doesn't match the buffer frame format
  if (frameSizeInPx[0] != this->GetFrameSize()[0] ||
      frameSizeInPx[1] != this->GetFrameSize()[1] ||
      frameSizeInPx[2] != this->GetFrameSize()[2])
  {
    LOCAL_LOG_WARNING("Frame format and buffer frame format does not match (expected frame size: " << this->GetFrameSize()[0]
                      << "x" << this->GetFrameSize()[1] << "x" << this->GetFrameSize()[2] << "  received: " << frameSizeInPx[0] << "x" << frameSizeInPx[1] << "x" << frameSizeInPx[2] << ")!");
    return false;
  }

  if (pixelType != this->GetPixelType())
  {
    LOCAL_LOG_WARNING("Frame pixel type (" << vtkImageScalarTypeNameMacro(pixelType)
                      << ") and buffer pixel type (" << vtkImageScalarTypeNameMacro(this->GetPixelType()) << ") mismatch");
    return false;
  }

  if (imgType != this->GetImageType())
  {
    LOCAL_LOG_WARNING("Frame image type (" << igsioCommon::GetStringFromUsImageType(imgType) << ") and buffer image type (" << igsioCommon::GetStringFromUsImageType(this->GetImageType()) << ") mismatch");
    return false;
  }

  if (numberOfScalarComponents != this->GetNumberOfScalarComponents())
  {
    LOCAL_LOG_WARNING("Frame number of scalar components (" << numberOfScalarComponents << ") and buffer number of components (" << this->GetNumberOfScalarComponents() << ") mismatch");
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBuffer::AddItem(vtkImageData* frame,
                                  US_IMAGE_ORIENTATION usImageOrientation,
                                  US_IMAGE_TYPE imageType,
                                  long frameNumber,
                                  const std::array<int, 3>& clipRectangleOrigin,
                                  const std::array<int, 3>& clipRectangleSize,
                                  double unfilteredTimestamp/*=UNDEFINED_TIMESTAMP*/,
                                  double filteredTimestamp/*=UNDEFINED_TIMESTAMP*/,
                                  const igsioFieldMapType* customFields /*=NULL*/)
{
  if (frame == NULL)
  {
    LOCAL_LOG_ERROR("vtkPlusBuffer: Unable to add NULL frame to video buffer!");
    return PLUS_FAIL;
  }

  const int* frameExtent = frame->GetExtent();
  if (frameExtent[1] - frameExtent[0] + 1 < 0 || frameExtent[3] - frameExtent[2] + 1 < 0 || frameExtent[5] - frameExtent[4] + 1 < 0)
  {
    LOG_ERROR("Invalid negative values sent to vtkPlusUsDevice::AddItem. Aborting.");
    return PLUS_FAIL;
  }

  FrameSizeType frameSize = {static_cast<unsigned int>(frameExtent[1] - frameExtent[0] + 1), static_cast<unsigned int>(frameExtent[3] - frameExtent[2] + 1), static_cast<unsigned int>(frameExtent[5] - frameExtent[4] + 1)};
  return this->AddItem(reinterpret_cast<unsigned char*>(frame->GetScalarPointer()),
                       usImageOrientation,
                       frameSize,
                       frame->GetScalarType(),
                       frame->GetNumberOfScalarComponents(),
                       imageType,
                       0,
                       frameNumber,
                       clipRectangleOrigin,
                       clipRectangleSize,
                       unfilteredTimestamp,
                       filteredTimestamp,
                       customFields);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBuffer::AddItem(const igsioVideoFrame* frame,
                                  long frameNumber,
                                  const std::array<int, 3>& clipRectangleOrigin,
                                  const std::array<int, 3>& clipRectangleSize,
                                  double unfilteredTimestamp/*=UNDEFINED_TIMESTAMP*/,
                                  double filteredTimestamp/*=UNDEFINED_TIMESTAMP*/,
                                  const igsioFieldMapType* customFields /*=NULL*/)
{
  if (frame == NULL)
  {
    LOCAL_LOG_ERROR("vtkPlusBuffer: Unable to add NULL frame to video buffer!");
    return PLUS_FAIL;
  }

  if (frame->IsFrameEncoded())
  {
    unsigned int numberOfComponents = 0;
    frame->GetNumberOfScalarComponents(numberOfComponents);
    int dimensions[3] = { 0, 0, 0 };
    frame->GetEncodedFrame()->GetDimensions(dimensions);
    FrameSizeType frameSize = { static_cast<unsigned int>(dimensions[0]),
                                static_cast<unsigned int>(dimensions[1]),
                                static_cast<unsigned int>(dimensions[2])
                              };
    return this->AddItem(NULL,
                         frame->GetImageOrientation(),
                         frameSize,
                         this->PixelType,
                         numberOfComponents,
                         frame->GetImageType(),
                         0,
                         frameNumber,
                         clipRectangleOrigin,
                         clipRectangleSize,
                         unfilteredTimestamp,
                         filteredTimestamp,
                         customFields,
                         frame->GetEncodedFrame());
  }
  else
  {
    return this->AddItem(frame->GetImage(),
                         frame->GetImageOrientation(),
                         frame->GetImageType(),
                         frameNumber,
                         clipRectangleOrigin,
                         clipRectangleSize,
                         unfilteredTimestamp,
                         filteredTimestamp,
                         customFields);
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBuffer::AddItem(const igsioFieldMapType& fields,
                                  long frameNumber,
                                  double unfilteredTimestamp/*=UNDEFINED_TIMESTAMP*/,
                                  double filteredTimestamp/*=UNDEFINED_TIMESTAMP*/)
{
  if (fields.empty())
  {
    return PLUS_SUCCESS;
  }

  if (unfilteredTimestamp == UNDEFINED_TIMESTAMP)
  {
    unfilteredTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();
  }
  if (filteredTimestamp == UNDEFINED_TIMESTAMP)
  {
    bool filteredTimestampProbablyValid = true;
    if (this->StreamBuffer->CreateFilteredTimeStampForItem(frameNumber, unfilteredTimestamp, filteredTimestamp, filteredTimestampProbablyValid) != PLUS_SUCCESS)
    {
      LOCAL_LOG_DEBUG("Failed to create filtered timestamp for tracker buffer item with item index: " << frameNumber);
      return PLUS_FAIL;
    }
    if (!filteredTimestampProbablyValid)
    {
      LOG_INFO("Filtered timestamp is probably invalid for tracker buffer item with item index=" << frameNumber << ", time=" << unfilteredTimestamp << ". The item may have been tagged with an inaccurate timestamp, therefore it will not be recorded.");
      return PLUS_SUCCESS;
    }
  }
  else
  {
    this->StreamBuffer->AddToTimeStampReport(frameNumber, unfilteredTimestamp, filteredTimestamp);
  }

  int bufferIndex(0);
  BufferItemUidType itemUid;

  igsioLockGuard<StreamItemCircularBuffer> dataBufferGuardedLock(this->StreamBuffer);
  if (this->StreamBuffer->PrepareForNewItem(filteredTimestamp, itemUid, bufferIndex) != PLUS_SUCCESS)
  {
    // Just a debug message, because we want to avoid unnecessary warning messages if the timestamp is the same as last one
    LOCAL_LOG_DEBUG("vtkPlusBuffer: Failed to prepare for adding new frame to tracker buffer!");
    return PLUS_FAIL;
  }

  // get the pointer to the correct location in the tracker buffer, where this data needs to be copied
  StreamBufferItem* newObjectInBuffer = this->StreamBuffer->GetBufferItemPointerFromBufferIndex(bufferIndex);
  if (newObjectInBuffer == NULL)
  {
    LOCAL_LOG_ERROR("vtkPlusBuffer: Failed to get pointer to data buffer object from the tracker buffer for the new frame!");
    return PLUS_FAIL;
  }

  newObjectInBuffer->SetFilteredTimestamp(filteredTimestamp);
  newObjectInBuffer->SetUnfilteredTimestamp(unfilteredTimestamp);
  newObjectInBuffer->SetIndex(frameNumber);
  newObjectInBuffer->SetUid(itemUid);

  // Add custom fields
  for (igsioFieldMapType::const_iterator it = fields.begin(); it != fields.end(); ++it)
  {
    newObjectInBuffer->SetFrameField(it->first, it->second.second, it->second.first);
    std::string name(it->first);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBuffer::AddItem(void* imageDataPtr,
                                  US_IMAGE_ORIENTATION usImageOrientation,
                                  const FrameSizeType& inputFrameSizeInPx,
                                  igsioCommon::VTKScalarPixelType pixelType,
                                  unsigned int numberOfScalarComponents,
                                  US_IMAGE_TYPE imageType,
                                  int numberOfBytesToSkip,
                                  long frameNumber,
                                  const std::array<int, 3>& clipRectangleOrigin,
                                  const std::array<int, 3>& clipRectangleSize,
                                  double unfilteredTimestamp /*= UNDEFINED_TIMESTAMP*/,
                                  double filteredTimestamp /*= UNDEFINED_TIMESTAMP*/,
                                  const igsioFieldMapType* customFields /*= NULL */,
                                  vtkStreamingVolumeFrame* encodedFrame /*=NULL*/)
{
  if (unfilteredTimestamp == UNDEFINED_TIMESTAMP)
  {
    unfilteredTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();
  }

  if (filteredTimestamp == UNDEFINED_TIMESTAMP)
  {
    bool filteredTimestampProbablyValid = true;
    if (this->StreamBuffer->CreateFilteredTimeStampForItem(frameNumber, unfilteredTimestamp, filteredTimestamp, filteredTimestampProbablyValid) != PLUS_SUCCESS)
    {
      LOCAL_LOG_WARNING("Failed to create filtered timestamp for video buffer item with item index: " << frameNumber);
      return PLUS_FAIL;
    }
    if (!filteredTimestampProbablyValid)
    {
      LOG_INFO("Filtered timestamp is probably invalid for video buffer item with item index=" << frameNumber << ", time=" <<
               unfilteredTimestamp << ". The item may have been tagged with an inaccurate timestamp, therefore it will not be recorded.");
      return PLUS_SUCCESS;
    }
  }
  else
  {
    this->StreamBuffer->AddToTimeStampReport(frameNumber, unfilteredTimestamp, filteredTimestamp);
  }

  if (imageDataPtr == NULL && encodedFrame == NULL)
  {
    LOG_ERROR("vtkPlusBuffer: Unable to add NULL frame to video buffer!");
    return PLUS_FAIL;
  }

  igsioVideoFrame::FlipInfoType flipInfo;
  if (igsioVideoFrame::GetFlipAxes(usImageOrientation, imageType, this->ImageOrientation, flipInfo) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to convert image data to the requested orientation, from " << igsioCommon::GetStringFromUsImageOrientation(usImageOrientation) <<
              " to " << igsioCommon::GetStringFromUsImageOrientation(this->ImageOrientation));
    return PLUS_FAIL;
  }

  // Calculate the output frame size to validate that buffer is correctly setup
  FrameSizeType outputFrameSizeInPx = { inputFrameSizeInPx[0], inputFrameSizeInPx[1], inputFrameSizeInPx[2] };
  if (igsioCommon::IsClippingRequested(clipRectangleOrigin, clipRectangleSize))
  {
    outputFrameSizeInPx[0] = clipRectangleSize[0];
    outputFrameSizeInPx[1] = clipRectangleSize[1];
    outputFrameSizeInPx[2] = clipRectangleSize[2];
  }

  if (flipInfo.tranpose == igsioVideoFrame::TRANSPOSE_IJKtoKIJ)
  {
    unsigned int temp = outputFrameSizeInPx[0];
    outputFrameSizeInPx[0] = outputFrameSizeInPx[2];
    outputFrameSizeInPx[2] = outputFrameSizeInPx[1];
    outputFrameSizeInPx[1] = temp;
  }

  if (!this->CheckFrameFormat(outputFrameSizeInPx, pixelType, imageType, numberOfScalarComponents))
  {
    LOG_ERROR("vtkPlusBuffer: Unable to add frame to video buffer - frame format doesn't match!");
    return PLUS_FAIL;
  }

  int bufferIndex(0);
  BufferItemUidType itemUid;
  igsioLockGuard<StreamItemCircularBuffer> dataBufferGuardedLock(this->StreamBuffer);
  if (this->StreamBuffer->PrepareForNewItem(filteredTimestamp, itemUid, bufferIndex) != PLUS_SUCCESS)
  {
    // Just a debug message, because we want to avoid unnecessary warning messages if the timestamp is the same as last one
    LOCAL_LOG_DEBUG("vtkPlusBuffer: Failed to prepare for adding new frame to video buffer!");
    return PLUS_FAIL;
  }

  // get the pointer to the correct location in the frame buffer, where this data needs to be copied
  StreamBufferItem* newObjectInBuffer = this->StreamBuffer->GetBufferItemPointerFromBufferIndex(bufferIndex);
  if (newObjectInBuffer == NULL)
  {
    LOCAL_LOG_ERROR("vtkPlusBuffer: Failed to get pointer to video buffer object from the video buffer for the new frame!");
    return PLUS_FAIL;
  }

  FrameSizeType receivedFrameSize = { 0, 0, 0 };
  newObjectInBuffer->GetFrame().GetFrameSize(receivedFrameSize);

  if (imageDataPtr && !encodedFrame &&
      (outputFrameSizeInPx[0] != receivedFrameSize[0]
       || outputFrameSizeInPx[1] != receivedFrameSize[1]
       || outputFrameSizeInPx[2] != receivedFrameSize[2]))
  {
    LOCAL_LOG_ERROR("Input frame size is different from buffer frame size (input: " <<
                    outputFrameSizeInPx[0] << "x" << outputFrameSizeInPx[1] << "x" << outputFrameSizeInPx[2] <<
                    ",   buffer: " <<
                    receivedFrameSize[0] << "x" << receivedFrameSize[1] << "x" << receivedFrameSize[2] << ")!");
    return PLUS_FAIL;
  }

  // Skip the numberOfBytesToSkip bytes, e.g. header size
  if (imageDataPtr != NULL)
  {
    unsigned char* byteImageDataPtr = reinterpret_cast<unsigned char*>(imageDataPtr);
    byteImageDataPtr += numberOfBytesToSkip;

    if (igsioVideoFrame::GetOrientedClippedImage(byteImageDataPtr, flipInfo, imageType, pixelType, numberOfScalarComponents, inputFrameSizeInPx, newObjectInBuffer->GetFrame(), clipRectangleOrigin, clipRectangleSize) != PLUS_SUCCESS)
    {
      LOCAL_LOG_ERROR("Failed to convert input US image to the requested orientation!");
      return PLUS_FAIL;
    }
  }
  else if (encodedFrame != NULL)
  {
    newObjectInBuffer->GetFrame().SetEncodedFrame(encodedFrame);
  }

  newObjectInBuffer->SetFilteredTimestamp(filteredTimestamp);
  newObjectInBuffer->SetUnfilteredTimestamp(unfilteredTimestamp);
  newObjectInBuffer->SetIndex(frameNumber);
  newObjectInBuffer->SetUid(itemUid);
  newObjectInBuffer->GetFrame().SetImageType(imageType);

  // Add custom fields
  if (customFields != NULL)
  {
    for (igsioFieldMapType::const_iterator it = customFields->begin(); it != customFields->end(); ++it)
    {
      newObjectInBuffer->SetFrameField(it->first, it->second.second, it->second.first);
      std::string name(it->first);
      if (name.find("Transform") != std::string::npos)
      {
        newObjectInBuffer->SetValidTransformData(true);
      }
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBuffer::AddItem(void* imageDataPtr, const FrameSizeType& frameSize, unsigned int inputFrameSizeInBytes, US_IMAGE_TYPE imageType, long frameNumber, double unfilteredTimestamp /*= UNDEFINED_TIMESTAMP*/, double filteredTimestamp /*= UNDEFINED_TIMESTAMP*/, const igsioFieldMapType* customFields /*= NULL*/)
{
  if (unfilteredTimestamp == UNDEFINED_TIMESTAMP)
  {
    unfilteredTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();
  }

  if (filteredTimestamp == UNDEFINED_TIMESTAMP)
  {
    bool filteredTimestampProbablyValid = true;
    if (this->StreamBuffer->CreateFilteredTimeStampForItem(frameNumber, unfilteredTimestamp, filteredTimestamp, filteredTimestampProbablyValid) != PLUS_SUCCESS)
    {
      LOCAL_LOG_WARNING("Failed to create filtered timestamp for video buffer item with item index: " << frameNumber);
      return PLUS_FAIL;
    }
    if (!filteredTimestampProbablyValid)
    {
      LOG_INFO("Filtered timestamp is probably invalid for video buffer item with item index=" << frameNumber << ", time=" <<
               unfilteredTimestamp << ". The item may have been tagged with an inaccurate timestamp, therefore it will not be recorded.");
      return PLUS_SUCCESS;
    }
  }
  else
  {
    this->StreamBuffer->AddToTimeStampReport(frameNumber, unfilteredTimestamp, filteredTimestamp);
  }

  if (imageDataPtr == NULL)
  {
    LOG_ERROR("vtkPlusBuffer: Unable to add NULL frame to video buffer!");
    return PLUS_FAIL;
  }

  if (frameSize[0] != this->GetFrameSize()[0] || frameSize[1] != this->GetFrameSize()[1] || frameSize[2] != this->GetFrameSize()[2])
  {
    LOG_ERROR("vtkPlusBuffer: Unable to add frame to video buffer - frame format doesn't match!");
    return PLUS_FAIL;
  }

  int bufferIndex(0);
  BufferItemUidType itemUid;
  igsioLockGuard<StreamItemCircularBuffer> dataBufferGuardedLock(this->StreamBuffer);
  if (this->StreamBuffer->PrepareForNewItem(filteredTimestamp, itemUid, bufferIndex) != PLUS_SUCCESS)
  {
    // Just a debug message, because we want to avoid unnecessary warning messages if the timestamp is the same as last one
    LOCAL_LOG_DEBUG("vtkPlusBuffer: Failed to prepare for adding new frame to video buffer!");
    return PLUS_FAIL;
  }

  // get the pointer to the correct location in the frame buffer, where this data needs to be copied
  StreamBufferItem* newObjectInBuffer = this->StreamBuffer->GetBufferItemPointerFromBufferIndex(bufferIndex);
  if (newObjectInBuffer == NULL)
  {
    LOCAL_LOG_ERROR("vtkPlusBuffer: Failed to get pointer to video buffer object from the video buffer for the new frame!");
    return PLUS_FAIL;
  }

  unsigned int bufferFrameSizeBytes = newObjectInBuffer->GetFrame().GetFrameSizeInBytes();
  if (bufferFrameSizeBytes < inputFrameSizeInBytes)
  {
    LOCAL_LOG_ERROR("Input frame size is larger than buffer frame size (input: " << inputFrameSizeInBytes << ",   buffer: " << bufferFrameSizeBytes << ")!");
    return PLUS_FAIL;
  }

  newObjectInBuffer->SetFilteredTimestamp(filteredTimestamp);
  newObjectInBuffer->SetUnfilteredTimestamp(unfilteredTimestamp);
  newObjectInBuffer->SetIndex(frameNumber);
  newObjectInBuffer->SetUid(itemUid);
  newObjectInBuffer->GetFrame().SetImageType(imageType);
  memcpy(newObjectInBuffer->GetFrame().GetImage()->GetScalarPointer(), imageDataPtr, inputFrameSizeInBytes);

  // Add custom fields
  if (customFields != NULL)
  {
    for (igsioFieldMapType::const_iterator it = customFields->begin(); it != customFields->end(); ++it)
    {
      newObjectInBuffer->SetFrameField(it->first, it->second.second, it->second.first);
      std::string name(it->first);
      if (name.find("Transform") != std::string::npos)
      {
        newObjectInBuffer->SetValidTransformData(true);
      }
    }
  }

  newObjectInBuffer->SetFrameField("FrameSizeInBytes", igsioCommon::ToString<unsigned int>(inputFrameSizeInBytes));

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBuffer::AddTimeStampedItem(vtkMatrix4x4* matrix, ToolStatus status, unsigned long frameNumber, double unfilteredTimestamp, double filteredTimestamp/*=UNDEFINED_TIMESTAMP*/, const igsioFieldMapType* customFields /*= NULL*/)
{
  if (matrix == NULL)
  {
    LOCAL_LOG_ERROR("vtkPlusBuffer: Unable to add NULL matrix to tracker buffer!");
    return PLUS_FAIL;
  }
  if (unfilteredTimestamp == UNDEFINED_TIMESTAMP)
  {
    unfilteredTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();
  }
  if (filteredTimestamp == UNDEFINED_TIMESTAMP)
  {
    bool filteredTimestampProbablyValid = true;
    if (this->StreamBuffer->CreateFilteredTimeStampForItem(frameNumber, unfilteredTimestamp, filteredTimestamp, filteredTimestampProbablyValid) != PLUS_SUCCESS)
    {
      LOCAL_LOG_DEBUG("Failed to create filtered timestamp for tracker buffer item with item index: " << frameNumber);
      return PLUS_FAIL;
    }
    if (!filteredTimestampProbablyValid)
    {
      LOG_INFO("Filtered timestamp is probably invalid for tracker buffer item with item index=" << frameNumber << ", time=" << unfilteredTimestamp << ". The item may have been tagged with an inaccurate timestamp, therefore it will not be recorded.");
      return PLUS_SUCCESS;
    }
  }
  else
  {
    this->StreamBuffer->AddToTimeStampReport(frameNumber, unfilteredTimestamp, filteredTimestamp);
  }

  int bufferIndex(0);
  BufferItemUidType itemUid;

  igsioLockGuard<StreamItemCircularBuffer> dataBufferGuardedLock(this->StreamBuffer);
  if (this->StreamBuffer->PrepareForNewItem(filteredTimestamp, itemUid, bufferIndex) != PLUS_SUCCESS)
  {
    // Just a debug message, because we want to avoid unnecessary warning messages if the timestamp is the same as last one
    LOCAL_LOG_DEBUG("vtkPlusBuffer: Failed to prepare for adding new frame to tracker buffer!");
    return PLUS_FAIL;
  }

  // get the pointer to the correct location in the tracker buffer, where this data needs to be copied
  StreamBufferItem* newObjectInBuffer = this->StreamBuffer->GetBufferItemPointerFromBufferIndex(bufferIndex);
  if (newObjectInBuffer == NULL)
  {
    LOCAL_LOG_ERROR("vtkPlusBuffer: Failed to get pointer to data buffer object from the tracker buffer for the new frame!");
    return PLUS_FAIL;
  }

  PlusStatus itemStatus = newObjectInBuffer->SetMatrix(matrix);
  newObjectInBuffer->SetStatus(status);
  newObjectInBuffer->SetFilteredTimestamp(filteredTimestamp);
  newObjectInBuffer->SetUnfilteredTimestamp(unfilteredTimestamp);
  newObjectInBuffer->SetIndex(frameNumber);
  newObjectInBuffer->SetUid(itemUid);

  // Add custom fields
  if (customFields != NULL)
  {
    for (igsioFieldMapType::const_iterator it = customFields->begin(); it != customFields->end(); ++it)
    {
      newObjectInBuffer->SetFrameField(it->first, it->second.second, it->second.first);
      std::string name(it->first);
      if (name.find("Transform") != std::string::npos)
      {
        newObjectInBuffer->SetValidTransformData(true);
      }
    }
  }

  return itemStatus;
}

//----------------------------------------------------------------------------
ItemStatus vtkPlusBuffer::GetLatestTimeStamp(double& latestTimestamp)
{
  return this->StreamBuffer->GetLatestTimeStamp(latestTimestamp);
}

//----------------------------------------------------------------------------
ItemStatus vtkPlusBuffer::GetOldestTimeStamp(double& oldestTimestamp)
{
  return this->StreamBuffer->GetOldestTimeStamp(oldestTimestamp);
}

//----------------------------------------------------------------------------
ItemStatus vtkPlusBuffer::GetTimeStamp(BufferItemUidType uid, double& timestamp)
{
  return this->StreamBuffer->GetTimeStamp(uid, timestamp);
}

//----------------------------------------------------------------------------
ItemStatus vtkPlusBuffer::GetIndex(BufferItemUidType uid, unsigned long& index)
{
  return this->StreamBuffer->GetIndex(uid, index);
}


//----------------------------------------------------------------------------
ItemStatus vtkPlusBuffer::GetBufferIndexFromTime(const double time, int& bufferIndex)
{
  return this->StreamBuffer->GetBufferIndexFromTime(time, bufferIndex);
}

//----------------------------------------------------------------------------
void vtkPlusBuffer::SetAveragedItemsForFiltering(int averagedItemsForFiltering)
{
  this->StreamBuffer->SetAveragedItemsForFiltering(averagedItemsForFiltering);
}

//----------------------------------------------------------------------------
int vtkPlusBuffer::GetAveragedItemsForFiltering()
{
  return this->StreamBuffer->GetAveragedItemsForFiltering();
}

//----------------------------------------------------------------------------
void vtkPlusBuffer::SetStartTime(double startTime)
{
  this->StreamBuffer->SetStartTime(startTime);
}

//----------------------------------------------------------------------------
double vtkPlusBuffer::GetStartTime()
{
  return this->StreamBuffer->GetStartTime();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBuffer::GetTimeStampReportTable(vtkTable* timeStampReportTable)
{
  return this->StreamBuffer->GetTimeStampReportTable(timeStampReportTable);
}

//----------------------------------------------------------------------------
ItemStatus vtkPlusBuffer::GetStreamBufferItem(BufferItemUidType uid, StreamBufferItem* bufferItem)
{
  if (bufferItem == NULL)
  {
    LOCAL_LOG_ERROR("Unable to copy data buffer item into a NULL data buffer item!");
    return ITEM_UNKNOWN_ERROR;
  }

  igsioLockGuard<StreamItemCircularBuffer> dataBufferGuardedLock(this->StreamBuffer);

  StreamBufferItem* dataItem = NULL;
  ItemStatus itemStatus = this->StreamBuffer->GetBufferItemPointerFromUid(uid, dataItem);
  if (itemStatus != ITEM_OK)
  {
    LOCAL_LOG_WARNING("Failed to retrieve data item");
    return itemStatus;
  }

  if (bufferItem->DeepCopy(dataItem) != PLUS_SUCCESS)
  {
    LOCAL_LOG_WARNING("Failed to copy data item");
    return ITEM_UNKNOWN_ERROR;
  }

  return ITEM_OK;
}

//----------------------------------------------------------------------------
void vtkPlusBuffer::DeepCopy(vtkPlusBuffer* buffer)
{
  LOG_TRACE("vtkPlusBuffer::DeepCopy");

  this->StreamBuffer->DeepCopy(buffer->StreamBuffer);
  if (buffer->GetFrameSize()[0] != -1 && buffer->GetFrameSize()[1] != -1 && buffer->GetFrameSize()[2] != -1)
  {
    this->SetFrameSize(buffer->GetFrameSize());
  }
  this->SetPixelType(buffer->GetPixelType());
  this->SetImageType(buffer->GetImageType());
  this->SetNumberOfScalarComponents(buffer->GetNumberOfScalarComponents());
  this->SetImageOrientation(buffer->GetImageOrientation());
  this->SetBufferSize(buffer->GetBufferSize());
}

//----------------------------------------------------------------------------
void vtkPlusBuffer::Clear()
{
  this->StreamBuffer->Clear();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBuffer::SetFrameSize(unsigned int x, unsigned int y, unsigned int z, bool allocateFrames/*=true*/)
{
  if (x != 0 && y != 0 && z == 0)
  {
    LOCAL_LOG_WARNING("Single slice images should have a dimension of z=1");
    z = 1;
  }
  if (this->FrameSize[0] == x && this->FrameSize[1] == y && this->FrameSize[2] == z)
  {
    // no change
    return PLUS_SUCCESS;
  }
  this->FrameSize[0] = x;
  this->FrameSize[1] = y;
  this->FrameSize[2] = z;
  if (allocateFrames)
  {
    return AllocateMemoryForFrames();
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBuffer::SetFrameSize(const FrameSizeType& frameSize, bool allocateFrames/*=true*/)
{
  return SetFrameSize(frameSize[0], frameSize[1], frameSize[2], allocateFrames);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBuffer::SetPixelType(igsioCommon::VTKScalarPixelType pixelType)
{
  if (pixelType == this->PixelType)
  {
    // no change
    return PLUS_SUCCESS;
  }
  this->PixelType = pixelType;
  return AllocateMemoryForFrames();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBuffer::SetNumberOfScalarComponents(unsigned int numberOfScalarComponents)
{
  if (numberOfScalarComponents == this->NumberOfScalarComponents)
  {
    // no change
    return PLUS_SUCCESS;
  }
  this->NumberOfScalarComponents = numberOfScalarComponents;
  return AllocateMemoryForFrames();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBuffer::SetImageType(US_IMAGE_TYPE imgType)
{
  if (imgType < US_IMG_TYPE_XX || imgType >= US_IMG_TYPE_LAST)
  {
    LOCAL_LOG_ERROR("Invalid image type attempted to set in the video buffer: " << imgType);
    return PLUS_FAIL;
  }
  this->ImageType = imgType;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBuffer::SetImageOrientation(US_IMAGE_ORIENTATION imgOrientation)
{
  if (imgOrientation < US_IMG_ORIENT_XX || imgOrientation >= US_IMG_ORIENT_LAST)
  {
    LOCAL_LOG_ERROR("Invalid image orientation attempted to set in the video buffer: " << imgOrientation);
    return PLUS_FAIL;
  }
  this->ImageOrientation = imgOrientation;
  for (int frameNumber = 0; frameNumber < this->StreamBuffer->GetBufferSize(); frameNumber++)
  {
    this->StreamBuffer->GetBufferItemPointerFromBufferIndex(frameNumber)->GetFrame().SetImageOrientation(imgOrientation);
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
int vtkPlusBuffer::GetNumberOfBytesPerScalar()
{
  return igsioVideoFrame::GetNumberOfBytesPerScalar(GetPixelType());
}

//----------------------------------------------------------------------------
int vtkPlusBuffer::GetNumberOfBytesPerPixel()
{
  return this->GetNumberOfScalarComponents() * igsioVideoFrame::GetNumberOfBytesPerScalar(GetPixelType());
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBuffer::CopyImagesFromTrackedFrameList(vtkIGSIOTrackedFrameList* sourceTrackedFrameList, TIMESTAMP_FILTERING_OPTION timestampFiltering, bool copyFrameFields)
{
  int numberOfErrors = 0;

  const int numberOfVideoFrames = sourceTrackedFrameList->GetNumberOfTrackedFrames();
  LOCAL_LOG_DEBUG("CopyImagesFromTrackedFrameList will copy " << numberOfVideoFrames << " frames");

  FrameSizeType frameSize = {0, 0, 0};
  sourceTrackedFrameList->GetTrackedFrame(0)->GetImageData()->GetFrameSize(frameSize);
  bool isFrameEncoded = sourceTrackedFrameList->GetTrackedFrame(0)->GetImageData()->IsFrameEncoded();
  this->SetFrameSize(frameSize, !isFrameEncoded);
  if (!isFrameEncoded)
  {
    this->SetPixelType(sourceTrackedFrameList->GetTrackedFrame(0)->GetImageData()->GetVTKScalarPixelType());
  }
  unsigned int numberOfScalarComponents(1);
  if (sourceTrackedFrameList->GetTrackedFrame(0)->GetImageData()->GetNumberOfScalarComponents(numberOfScalarComponents) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to retrieve number of scalar components.");
    return PLUS_FAIL;
  }
  this->SetNumberOfScalarComponents(numberOfScalarComponents);

  if (this->SetBufferSize(numberOfVideoFrames) != PLUS_SUCCESS)
  {
    LOCAL_LOG_ERROR("Failed to set video buffer size!");
    return PLUS_FAIL;
  }

  bool requireTimestamp = false;
  if (timestampFiltering == READ_FILTERED_AND_UNFILTERED_TIMESTAMPS || timestampFiltering == READ_FILTERED_IGNORE_UNFILTERED_TIMESTAMPS)
  {
    requireTimestamp = true;
  }

  bool requireUnfilteredTimestamp = false;
  if (timestampFiltering == READ_FILTERED_AND_UNFILTERED_TIMESTAMPS || timestampFiltering == READ_UNFILTERED_COMPUTE_FILTERED_TIMESTAMPS)
  {
    requireUnfilteredTimestamp = true;
  }

  bool requireFrameStatus = false;
  bool requireFrameNumber = false;
  if (timestampFiltering == READ_UNFILTERED_COMPUTE_FILTERED_TIMESTAMPS)
  {
    // frame status and number is required for the filtered timestamp computation
    requireFrameStatus = true;
    requireFrameNumber = true;
  }

  LOG_INFO("Copy buffer to video buffer...");
  for (int frameNumber = 0; frameNumber < numberOfVideoFrames; frameNumber++)
  {
    igsioFieldMapType customFields;
    if (copyFrameFields)
    {
      // Copy all custom fields
      igsioFieldMapType sourceCustomFields = sourceTrackedFrameList->GetTrackedFrame(frameNumber)->GetCustomFields();
      igsioFieldMapType::iterator fieldIterator;
      for (fieldIterator = sourceCustomFields.begin(); fieldIterator != sourceCustomFields.end(); fieldIterator++)
      {
        // skip special fields
        if (igsioCommon::IsEqualInsensitive(fieldIterator->first, "TimeStamp"))
        {
          continue;
        }
        if (igsioCommon::IsEqualInsensitive(fieldIterator->first, "UnfilteredTimestamp"))
        {
          continue;
        }
        if (igsioCommon::IsEqualInsensitive(fieldIterator->first, "FrameNumber"))
        {
          continue;
        }
        // add custom field
        customFields[fieldIterator->first] = fieldIterator->second;
      }
    }

    // read filtered timestamp
    double timestamp(0);
    std::string strTimestamp = sourceTrackedFrameList->GetTrackedFrame(frameNumber)->GetFrameField("Timestamp");
    if (!strTimestamp.empty())
    {
      if (igsioCommon::StringToNumber<double>(strTimestamp, timestamp) != PLUS_SUCCESS && requireTimestamp)
      {
        LOCAL_LOG_ERROR("Unable to convert Timestamp '" << strTimestamp << "' to double for frame #" << frameNumber);
        numberOfErrors++;
        continue;
      }
    }
    else if (requireTimestamp)
    {
      LOCAL_LOG_ERROR("Unable to read Timestamp field of frame #" << frameNumber);
      numberOfErrors++;
      continue;
    }

    // read unfiltered timestamp
    double unfilteredtimestamp(0);
    std::string strUnfilteredTimestamp = sourceTrackedFrameList->GetTrackedFrame(frameNumber)->GetFrameField("UnfilteredTimestamp");
    if (!strUnfilteredTimestamp.empty())
    {
      if (igsioCommon::StringToNumber<double>(strUnfilteredTimestamp, unfilteredtimestamp) != PLUS_SUCCESS && requireUnfilteredTimestamp)
      {
        LOCAL_LOG_ERROR("Unable to convert UnfilteredTimestamp '" << strUnfilteredTimestamp << "' to double for frame #" << frameNumber);
        numberOfErrors++;
        continue;
      }
    }
    else if (requireUnfilteredTimestamp)
    {
      LOCAL_LOG_ERROR("Unable to read UnfilteredTimestamp field of frame #" << frameNumber);
      numberOfErrors++;
      continue;
    }

    // read frame number
    const std::string strFrameNumber = sourceTrackedFrameList->GetTrackedFrame(frameNumber)->GetFrameField("FrameNumber");
    unsigned long frmnum(0);
    if (!strFrameNumber.empty())
    {
      if (igsioCommon::StringToNumber<unsigned long>(strFrameNumber, frmnum) != PLUS_SUCCESS && requireFrameNumber)
      {
        LOCAL_LOG_ERROR("Unable to convert FrameNumber '" << strFrameNumber << "' to integer for frame #" << frameNumber);
        numberOfErrors++;
        continue;
      }
    }
    else if (requireFrameNumber)
    {
      LOCAL_LOG_ERROR("Unable to read FrameNumber field of frame #" << frameNumber);
      numberOfErrors++;
      continue;
    }

    std::array<int, 3> clipRectOrigin = {igsioCommon::NO_CLIP, igsioCommon::NO_CLIP, igsioCommon::NO_CLIP};
    std::array<int, 3> clipRectSize = {igsioCommon::NO_CLIP, igsioCommon::NO_CLIP, igsioCommon::NO_CLIP};
    switch (timestampFiltering)
    {
      case READ_FILTERED_AND_UNFILTERED_TIMESTAMPS:
        if (this->AddItem(sourceTrackedFrameList->GetTrackedFrame(frameNumber)->GetImageData(), frmnum, clipRectOrigin, clipRectSize, unfilteredtimestamp, timestamp, &customFields) != PLUS_SUCCESS)
        {
          LOCAL_LOG_WARNING("Failed to add video frame to buffer from sequence metafile with frame #" << frameNumber);
        }
        break;
      case READ_UNFILTERED_COMPUTE_FILTERED_TIMESTAMPS:
        if (this->AddItem(sourceTrackedFrameList->GetTrackedFrame(frameNumber)->GetImageData(), frmnum, clipRectOrigin, clipRectSize, unfilteredtimestamp, UNDEFINED_TIMESTAMP, &customFields) != PLUS_SUCCESS)
        {
          LOCAL_LOG_WARNING("Failed to add video frame to buffer from sequence metafile with frame #" << frameNumber);
        }
        break;
      case READ_FILTERED_IGNORE_UNFILTERED_TIMESTAMPS:
        if (this->AddItem(sourceTrackedFrameList->GetTrackedFrame(frameNumber)->GetImageData(), frmnum, clipRectOrigin, clipRectSize, timestamp, timestamp, &customFields) != PLUS_SUCCESS)
        {
          LOCAL_LOG_WARNING("Failed to add video frame to buffer from sequence metafile with frame #" << frameNumber);
        }
        break;
      default:
        break;
    }
  }

  return (numberOfErrors > 0 ? PLUS_FAIL : PLUS_SUCCESS);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBuffer::WriteToSequenceFile(const char* filename, bool useCompression /*=false*/)
{
  LOG_TRACE("vtkPlusBuffer::WriteToSequenceFile");

  vtkSmartPointer<vtkIGSIOTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkIGSIOTrackedFrameList>::New();

  PlusStatus status = PLUS_SUCCESS;

  for (BufferItemUidType frameUid = this->GetOldestItemUidInBuffer(); frameUid <= this->GetLatestItemUidInBuffer(); ++frameUid)
  {
    StreamBufferItem bufferItem;
    if (this->GetStreamBufferItem(frameUid, &bufferItem) != ITEM_OK)
    {
      LOCAL_LOG_ERROR("Unable to get frame from buffer with UID: " << frameUid);
      status = PLUS_FAIL;
      continue;
    }

    igsioTrackedFrame* trackedFrame = new igsioTrackedFrame;

    // Add image data
    trackedFrame->SetImageData(bufferItem.GetFrame());

    // Add tracking data
    vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
    bufferItem.GetMatrix(matrix);
    trackedFrame->SetFrameTransform(igsioTransformName("Tool", "Tracker"), matrix);
    trackedFrame->SetFrameTransformStatus(igsioTransformName("Tool", "Tracker"), bufferItem.GetStatus());

    // Add filtered timestamp
    double filteredTimestamp = bufferItem.GetFilteredTimestamp(this->GetLocalTimeOffsetSec());
    std::ostringstream timestampFieldValue;
    timestampFieldValue << std::fixed << filteredTimestamp;
    trackedFrame->SetFrameField("Timestamp", timestampFieldValue.str());

    // Add unfiltered timestamp
    double unfilteredTimestamp = bufferItem.GetUnfilteredTimestamp(this->GetLocalTimeOffsetSec());
    std::ostringstream unfilteredtimestampFieldValue;
    unfilteredtimestampFieldValue << std::fixed << unfilteredTimestamp;
    trackedFrame->SetFrameField("UnfilteredTimestamp", unfilteredtimestampFieldValue.str());

    // Add frame number
    unsigned long frameNumber = bufferItem.GetIndex();
    std::ostringstream frameNumberFieldValue;
    frameNumberFieldValue << std::fixed << frameNumber;
    trackedFrame->SetFrameField("FrameNumber", frameNumberFieldValue.str());

    // Add custom fields
    const igsioFieldMapType& customFields = bufferItem.GetFrameFieldMap();
    for (igsioFieldMapType::const_iterator cf = customFields.begin(); cf != customFields.end(); ++cf)
    {
      trackedFrame->SetFrameField(cf->first, cf->second.second, cf->second.first);
    }

    // Add tracked frame to the list
    trackedFrameList->TakeTrackedFrame(trackedFrame);
  }

  // Save tracked frames to metafile
  if (vtkPlusSequenceIO::Write(filename, trackedFrameList, trackedFrameList->GetImageOrientation(), useCompression) != PLUS_SUCCESS)
  {
    LOCAL_LOG_ERROR("Failed to save tracked frames to sequence metafile!");
    return PLUS_FAIL;
  }

  return status;
}

//-----------------------------------------------------------------------------
void vtkPlusBuffer::SetTimeStampReporting(bool enable)
{
  this->StreamBuffer->SetTimeStampReporting(enable);
}

//-----------------------------------------------------------------------------
bool vtkPlusBuffer::GetTimeStampReporting()
{
  return this->StreamBuffer->GetTimeStampReporting();
}

//----------------------------------------------------------------------------
// Returns the two buffer items that are closest previous and next buffer items relative to the specified time.
// itemA is the closest item
PlusStatus vtkPlusBuffer::GetPrevNextBufferItemFromTime(double time, StreamBufferItem& itemA, StreamBufferItem& itemB)
{
  igsioLockGuard<StreamItemCircularBuffer> dataBufferGuardedLock(this->StreamBuffer);

  // The returned item is computed by interpolation between itemA and itemB in time. The itemA is the closest item to the requested time.
  // Accept itemA (the closest item) as is if it is very close to the requested time.
  // Accept interpolation between itemA and itemB if all the followings are true:
  //   - both itemA and itemB exist and are valid
  //   - time difference between the requested time and itemA is below a threshold
  //   - time difference between the requested time and itemB is below a threshold

  // itemA is the item that is the closest to the requested time, get its UID and time
  BufferItemUidType itemAuid(0);
  ItemStatus status = this->StreamBuffer->GetItemUidFromTime(time, itemAuid);
  if (status != ITEM_OK)
  {
    switch (status)
    {
      case ITEM_NOT_AVAILABLE_YET:
        LOCAL_LOG_DEBUG("vtkPlusBuffer: Cannot get any item from the data buffer for time: " << std::fixed << time << ". Item is not available yet.");
        break;
      case ITEM_NOT_AVAILABLE_ANYMORE:
        LOCAL_LOG_DEBUG("vtkPlusBuffer: Cannot get any item from the data buffer for time: " << std::fixed << time << ". Item is not available anymore.");
        break;
      default:
        break;
    }
    return PLUS_FAIL;
  }
  status = this->GetStreamBufferItem(itemAuid, &itemA);
  if (status != ITEM_OK)
  {
    LOCAL_LOG_ERROR("vtkPlusBuffer: Failed to get data buffer item with Uid: " << itemAuid);
    return PLUS_FAIL;
  }

  // If tracker is out of view, etc. then we don't have a valid before and after the requested time, so we cannot do interpolation
  if (itemA.GetStatus() != TOOL_OK)
  {
    // tracker is out of view, ...
    LOCAL_LOG_DEBUG("vtkPlusBuffer: Cannot do data interpolation. The closest item to the requested time (time: " << std::fixed << time << ", uid: " << itemAuid << ") is invalid.");
    return PLUS_FAIL;
  }

  double itemAtime(0);
  status = this->StreamBuffer->GetTimeStamp(itemAuid, itemAtime);
  if (status != ITEM_OK)
  {
    LOCAL_LOG_ERROR("vtkPlusBuffer: Failed to get data buffer timestamp (time: " << std::fixed << time << ", uid: " << itemAuid << ")");
    return PLUS_FAIL;
  }

  // If the time difference is negligible then don't interpolate, just return the closest item
  if (fabs(itemAtime - time) < NEGLIGIBLE_TIME_DIFFERENCE)
  {
    //No need for interpolation, it's very close to the closest element
    itemB.DeepCopy(&itemA);
    return PLUS_SUCCESS;
  }

  // If the closest item is too far, then we don't do interpolation
  if (fabs(itemAtime - time) > this->GetMaxAllowedTimeDifference())
  {
    LOCAL_LOG_ERROR("vtkPlusBuffer: Cannot perform interpolation, time difference compared to itemA is too big " << std::fixed << fabs(itemAtime - time) << " ( closest item time: " << itemAtime << ", requested time: " << time << ").");
    return PLUS_FAIL;
  }

  // Find the closest item on the other side of the timescale (so that time is between itemAtime and itemBtime)
  BufferItemUidType itemBuid(0);
  if (time < itemAtime)
  {
    // itemBtime < time <itemAtime
    itemBuid = itemAuid - 1;
  }
  else
  {
    // itemAtime < time <itemBtime
    itemBuid = itemAuid + 1;
  }
  if (itemBuid < this->GetOldestItemUidInBuffer() || itemBuid > this->GetLatestItemUidInBuffer())
  {
    // itemB is not available
    LOCAL_LOG_ERROR("vtkPlusBuffer: Cannot perform interpolation, itemB is not available " << std::fixed << " ( itemBuid: " << itemBuid << ", oldest UID: " << this->GetOldestItemUidInBuffer() << ", latest UID: " << this->GetLatestItemUidInBuffer());
    return PLUS_FAIL;
  }
  // Get item B details
  double itemBtime(0);
  status = this->StreamBuffer->GetTimeStamp(itemBuid, itemBtime);
  if (status != ITEM_OK)
  {
    LOCAL_LOG_ERROR("Cannot do interpolation: Failed to get data buffer timestamp with Uid: " << itemBuid);
    return PLUS_FAIL;
  }
  // If the next closest item is too far, then we don't do interpolation
  if (fabs(itemBtime - time) > this->GetMaxAllowedTimeDifference())
  {
    LOCAL_LOG_ERROR("vtkPlusBuffer: Cannot perform interpolation, time difference compared to itemB is too big " << std::fixed << fabs(itemBtime - time) << " ( itemBtime: " << itemBtime << ", requested time: " << time << ").");
    return PLUS_FAIL;
  }
  // Get the item
  status = this->GetStreamBufferItem(itemBuid, &itemB);
  if (status != ITEM_OK)
  {
    LOCAL_LOG_ERROR("vtkPlusBuffer: Failed to get data buffer item with Uid: " << itemBuid);
    return PLUS_FAIL;
  }
  // If there is no valid element on the other side of the requested time, then we cannot do an interpolation
  if (itemB.GetStatus() != TOOL_OK)
  {
    LOCAL_LOG_DEBUG("vtkPlusBuffer: Cannot get a second element (uid=" << itemBuid << ") on the other side of the requested time (" << std::fixed << time << ")");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
ItemStatus vtkPlusBuffer::GetStreamBufferItemFromTime(double time, StreamBufferItem* bufferItem, DataItemTemporalInterpolationType interpolation)
{
  switch (interpolation)
  {
    case EXACT_TIME:
      return GetStreamBufferItemFromExactTime(time, bufferItem);
    case INTERPOLATED:
      return GetInterpolatedStreamBufferItemFromTime(time, bufferItem);
    case CLOSEST_TIME:
      return GetStreamBufferItemFromClosestTime(time, bufferItem);
    default:
      LOCAL_LOG_WARNING("Unknown interpolation type: " << interpolation << ". Defaulting to exact time request.");
      return GetStreamBufferItemFromExactTime(time, bufferItem);
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBuffer::ModifyBufferItemFrameField(BufferItemUidType uid, const std::string& key, const std::string& value)
{
  StreamBufferItem* item;
  auto itemStatus = this->StreamBuffer->GetBufferItemPointerFromUid(uid, item);
  if (itemStatus == ITEM_OK)
  {
    item->SetFrameField(key, value);
  }
  return itemStatus == ITEM_OK ? PLUS_SUCCESS : PLUS_FAIL;
}

//----------------------------------------------------------------------------
ItemStatus vtkPlusBuffer::GetStreamBufferItemFromExactTime(double time, StreamBufferItem* bufferItem)
{
  ItemStatus status = GetStreamBufferItemFromClosestTime(time, bufferItem);
  if (status != ITEM_OK)
  {
    LOCAL_LOG_WARNING("vtkPlusBuffer: Failed to get data buffer timestamp (time: " << std::fixed << time << ")");
    return status;
  }

  // If the time difference is not negligible then return with failure
  double itemTime = bufferItem->GetFilteredTimestamp(this->StreamBuffer->GetLocalTimeOffsetSec());
  if (fabs(itemTime - time) > NEGLIGIBLE_TIME_DIFFERENCE)
  {
    LOCAL_LOG_WARNING("vtkPlusBuffer: Cannot find an item exactly at the requested time (requested time: " << std::fixed << time << ", item time: " << itemTime << ")");
    return ITEM_UNKNOWN_ERROR;
  }

  return status;
}

//----------------------------------------------------------------------------
ItemStatus vtkPlusBuffer::GetStreamBufferItemFromClosestTime(double time, StreamBufferItem* bufferItem)
{
  igsioLockGuard<StreamItemCircularBuffer> dataBufferGuardedLock(this->StreamBuffer);

  BufferItemUidType itemUid(0);
  ItemStatus status = this->StreamBuffer->GetItemUidFromTime(time, itemUid);
  if (status != ITEM_OK)
  {
    switch (status)
    {
      case ITEM_NOT_AVAILABLE_YET:
        LOCAL_LOG_WARNING("vtkPlusBuffer: Cannot get any item from the buffer for time: " << std::fixed << time << ". Item is not available yet.");
        break;
      case ITEM_NOT_AVAILABLE_ANYMORE:
        LOCAL_LOG_WARNING("vtkPlusBuffer: Cannot get any item from the buffer for time: " << std::fixed << time << ". Item is not available anymore.");
        break;
      default:
        break;
    }
    return status;
  }

  status = this->GetStreamBufferItem(itemUid, bufferItem);
  if (status != ITEM_OK)
  {
    LOCAL_LOG_ERROR("vtkPlusBuffer: Failed to get buffer item with Uid: " << itemUid);
    return status;
  }

  return status;
}

//----------------------------------------------------------------------------
// Interpolate the matrix for the given timestamp from the two nearest
// transforms in the buffer.
// The rotation is interpolated with SLERP interpolation, and the
// position is interpolated with linear interpolation.
// The flags correspond to the closest element.
ItemStatus vtkPlusBuffer::GetInterpolatedStreamBufferItemFromTime(double time, StreamBufferItem* bufferItem)
{
  StreamBufferItem itemA;
  StreamBufferItem itemB;

  if (GetPrevNextBufferItemFromTime(time, itemA, itemB) != PLUS_SUCCESS)
  {
    // cannot get two neighbors, so cannot do interpolation
    // it may be normal (e.g., when tracker out of view), so don't return with an error
    ItemStatus status = GetStreamBufferItemFromClosestTime(time, bufferItem);
    // Update the timestamp to match the requested time
    bufferItem->SetFilteredTimestamp(time);
    bufferItem->SetUnfilteredTimestamp(time);
    if (status != ITEM_OK)
    {
      LOCAL_LOG_ERROR("vtkPlusBuffer: Failed to get data buffer timestamp (time: " << std::fixed << time << ")");
      return status;
    }
    bufferItem->SetStatus(TOOL_MISSING);   // if we return at any point due to an error then it means that the interpolation is not successful, so the item is missing
    return ITEM_OK;
  }

  if (itemA.GetUid() == itemB.GetUid())
  {
    // exact match, no need for interpolation
    bufferItem->DeepCopy(&itemA);
    return ITEM_OK;
  }

  //============== Get item weights ==================

  double itemAtime(0);
  if (this->StreamBuffer->GetTimeStamp(itemA.GetUid(), itemAtime) != ITEM_OK)
  {
    LOCAL_LOG_ERROR("vtkPlusBuffer: Failed to get data buffer timestamp (time: " << std::fixed << time << ", uid: " << itemA.GetUid() << ")");
    return ITEM_UNKNOWN_ERROR;
  }

  double itemBtime(0);
  if (this->StreamBuffer->GetTimeStamp(itemB.GetUid(), itemBtime) != ITEM_OK)
  {
    LOCAL_LOG_ERROR("vtkPlusBuffer: Failed to get data buffer timestamp (time: " << std::fixed << time << ", uid: " << itemB.GetUid() << ")");
    return ITEM_UNKNOWN_ERROR;
  }

  if (fabs(itemAtime - itemBtime) < NEGLIGIBLE_TIME_DIFFERENCE)
  {
    // exact time match, no need for interpolation
    bufferItem->DeepCopy(&itemA);
    bufferItem->SetFilteredTimestamp(time);
    bufferItem->SetUnfilteredTimestamp(time);
    return ITEM_OK;
  }

  double itemAweight = fabs(itemBtime - time) / fabs(itemAtime - itemBtime);
  double itemBweight = 1 - itemAweight;

  //============== Get transform matrices ==================

  vtkSmartPointer<vtkMatrix4x4> itemAmatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  if (itemA.GetMatrix(itemAmatrix) != PLUS_SUCCESS)
  {
    LOCAL_LOG_ERROR("Failed to get item A matrix");
    return ITEM_UNKNOWN_ERROR;
  }
  double matrixA[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
  double xyzA[3] = {0, 0, 0};
  for (int i = 0; i < 3; i++)
  {
    matrixA[i][0] = itemAmatrix->GetElement(i, 0);
    matrixA[i][1] = itemAmatrix->GetElement(i, 1);
    matrixA[i][2] = itemAmatrix->GetElement(i, 2);
    xyzA[i] = itemAmatrix->GetElement(i, 3);
  }

  vtkSmartPointer<vtkMatrix4x4> itemBmatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  if (itemB.GetMatrix(itemBmatrix) != PLUS_SUCCESS)
  {
    LOCAL_LOG_ERROR("Failed to get item B matrix");
    return ITEM_UNKNOWN_ERROR;
  }
  double matrixB[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
  double xyzB[3] = {0, 0, 0};
  for (int i = 0; i < 3; i++)
  {
    matrixB[i][0] = itemBmatrix->GetElement(i, 0);
    matrixB[i][1] = itemBmatrix->GetElement(i, 1);
    matrixB[i][2] = itemBmatrix->GetElement(i, 2);
    xyzB[i] = itemBmatrix->GetElement(i, 3);
  }

  //============== Interpolate rotation ==================

  double matrixAquat[4] = {0, 0, 0, 0};
  vtkMath::Matrix3x3ToQuaternion(matrixA, matrixAquat);
  double matrixBquat[4] = {0, 0, 0, 0};
  vtkMath::Matrix3x3ToQuaternion(matrixB, matrixBquat);
  double interpolatedRotationQuat[4] = {0, 0, 0, 0};
  igsioMath::Slerp(interpolatedRotationQuat, itemBweight, matrixAquat, matrixBquat);
  double interpolatedRotation[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
  vtkMath::QuaternionToMatrix3x3(interpolatedRotationQuat, interpolatedRotation);

  vtkSmartPointer<vtkMatrix4x4> interpolatedMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  for (int i = 0; i < 3; i++)
  {
    interpolatedMatrix->Element[i][0] = interpolatedRotation[i][0];
    interpolatedMatrix->Element[i][1] = interpolatedRotation[i][1];
    interpolatedMatrix->Element[i][2] = interpolatedRotation[i][2];
    interpolatedMatrix->Element[i][3] = xyzA[i] * itemAweight + xyzB[i] * itemBweight;
  }

  //============== Interpolate time ==================

  double itemAunfilteredTimestamp = itemA.GetUnfilteredTimestamp(0.0);   // 0.0 because timestamps in the buffer are in local time
  double itemBunfilteredTimestamp = itemB.GetUnfilteredTimestamp(0.0);   // 0.0 because timestamps in the buffer are in local time
  double interpolatedUnfilteredTimestamp = itemAunfilteredTimestamp * itemAweight + itemBunfilteredTimestamp * itemBweight;

  //============== Write interpolated results into the bufferItem ==================

  bufferItem->DeepCopy(&itemA);
  bufferItem->SetMatrix(interpolatedMatrix);
  bufferItem->SetFilteredTimestamp(time - this->StreamBuffer->GetLocalTimeOffsetSec());   // global = local + offset => local = global - offset
  bufferItem->SetUnfilteredTimestamp(interpolatedUnfilteredTimestamp);

  double angleDiffA = igsioMath::GetOrientationDifference(interpolatedMatrix, itemAmatrix);
  double angleDiffB = igsioMath::GetOrientationDifference(interpolatedMatrix, itemBmatrix);
  if (fabs(angleDiffA) > ANGLE_INTERPOLATION_WARNING_THRESHOLD_DEG && fabs(angleDiffB) > ANGLE_INTERPOLATION_WARNING_THRESHOLD_DEG)
  {
    static vtkIGSIOLogHelper helper(5.f, 5000, vtkPlusLogger::LOG_LEVEL_WARNING);
    if (helper.ShouldWeLog(true))
    {
      LOCAL_LOG_WARNING("Angle difference between interpolated orientations is large (" << fabs(angleDiffA) << " and " << fabs(angleDiffB) << " deg, warning threshold is " << ANGLE_INTERPOLATION_WARNING_THRESHOLD_DEG << "), interpolation may be inaccurate. Consider moving the tools slower.");
    }
  }

  return ITEM_OK;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusBuffer::CopyTransformFromTrackedFrameList(vtkIGSIOTrackedFrameList* sourceTrackedFrameList, TIMESTAMP_FILTERING_OPTION timestampFiltering, igsioTransformName& transformName)
{
  int numberOfErrors = 0;

  int numberOfFrames = sourceTrackedFrameList->GetNumberOfTrackedFrames();
  this->SetBufferSize(numberOfFrames + 1);

  bool requireTimestamp = false;
  if (timestampFiltering == READ_FILTERED_AND_UNFILTERED_TIMESTAMPS || timestampFiltering == READ_FILTERED_IGNORE_UNFILTERED_TIMESTAMPS)
  {
    requireTimestamp = true;
  }

  bool requireUnfilteredTimestamp = false;
  if (timestampFiltering == READ_FILTERED_AND_UNFILTERED_TIMESTAMPS || timestampFiltering == READ_UNFILTERED_COMPUTE_FILTERED_TIMESTAMPS)
  {
    requireUnfilteredTimestamp = true;
  }

  bool requireFrameStatus = false;
  bool requireFrameNumber = false;
  if (timestampFiltering == READ_UNFILTERED_COMPUTE_FILTERED_TIMESTAMPS)
  {
    // frame status and number is required for the filtered timestamp computation
    requireFrameStatus = true;
    requireFrameNumber = true;
  }

  for (int frameNumber = 0; frameNumber < numberOfFrames; frameNumber++)
  {

    // read filtered timestamp
    double timestamp(0);
    const std::string strTimestamp = sourceTrackedFrameList->GetTrackedFrame(frameNumber)->GetFrameField("Timestamp");
    if (!strTimestamp.empty())
    {
      if (igsioCommon::StringToNumber<double>(strTimestamp, timestamp) != PLUS_SUCCESS && requireTimestamp)
      {
        LOCAL_LOG_ERROR("Unable to convert Timestamp '" << strTimestamp << "' to double");
        numberOfErrors++;
        continue;
      }
    }
    else if (requireTimestamp)
    {
      LOCAL_LOG_ERROR("Unable to read Timestamp field of frame #" << frameNumber);
      numberOfErrors++;
      continue;
    }

    // read unfiltered timestamp
    double unfilteredtimestamp(0);
    const std::string strUnfilteredTimestamp = sourceTrackedFrameList->GetTrackedFrame(frameNumber)->GetFrameField("UnfilteredTimestamp");
    if (!strUnfilteredTimestamp.empty())
    {
      if (igsioCommon::StringToNumber<double>(strUnfilteredTimestamp, unfilteredtimestamp) != PLUS_SUCCESS && requireUnfilteredTimestamp)
      {
        LOCAL_LOG_ERROR("Unable to convert UnfilteredTimestamp '" << strUnfilteredTimestamp << "' to double");
        numberOfErrors++;
        continue;
      }
    }
    else if (requireUnfilteredTimestamp)
    {
      LOCAL_LOG_ERROR("Unable to read UnfilteredTimestamp field of frame #" << frameNumber);
      numberOfErrors++;
      continue;
    }

    // read status
    ToolStatus toolStatus(TOOL_OK);
    if (sourceTrackedFrameList->GetTrackedFrame(frameNumber)->GetFrameTransformStatus(transformName, toolStatus) != PLUS_SUCCESS && requireFrameStatus)
    {
      LOCAL_LOG_ERROR("Unable to read TransformStatus field of frame #" << frameNumber);
      numberOfErrors++;
      continue;
    }

    // read frame number
    const std::string strFrameNumber = sourceTrackedFrameList->GetTrackedFrame(frameNumber)->GetFrameField("FrameNumber");
    unsigned long frmnum(0);
    if (!strFrameNumber.empty())
    {
      if (igsioCommon::StringToNumber<unsigned long>(strFrameNumber, frmnum) != PLUS_SUCCESS && requireFrameNumber)
      {
        LOCAL_LOG_ERROR("Unable to convert FrameNumber '" << strFrameNumber << "' to integer for frame #" << frameNumber);
        numberOfErrors++;
        continue;
      }
    }
    else if (requireFrameNumber)
    {
      LOCAL_LOG_ERROR("Unable to read FrameNumber field of frame #" << frameNumber);
      numberOfErrors++;
      continue;
    }

    double copiedTransform[16] = {0};
    if (!sourceTrackedFrameList->GetTrackedFrame(frameNumber)->GetFrameTransform(transformName, copiedTransform))
    {
      std::string strTransformName;
      transformName.GetTransformName(strTransformName);
      LOCAL_LOG_ERROR("Unable to get the " << strTransformName << " frame transform for frame #" << frameNumber);
      numberOfErrors++;
      continue;
    }

    vtkSmartPointer<vtkMatrix4x4> copiedTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    copiedTransformMatrix->DeepCopy(copiedTransform);

    switch (timestampFiltering)
    {
      case READ_FILTERED_AND_UNFILTERED_TIMESTAMPS:
        this->AddTimeStampedItem(copiedTransformMatrix, toolStatus, frmnum, unfilteredtimestamp, timestamp);
        break;
      case READ_UNFILTERED_COMPUTE_FILTERED_TIMESTAMPS:
        this->AddTimeStampedItem(copiedTransformMatrix, toolStatus, frmnum, unfilteredtimestamp);
        break;
      case READ_FILTERED_IGNORE_UNFILTERED_TIMESTAMPS:
        this->AddTimeStampedItem(copiedTransformMatrix, toolStatus, frmnum, timestamp, timestamp);
        break;
      default:
        break;
    }

  }

  return (numberOfErrors > 0 ? PLUS_FAIL : PLUS_SUCCESS);
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusBuffer::GetFrameSize(unsigned int& _arg1, unsigned int& _arg2, unsigned int& _arg3) const
{
  _arg1 = this->FrameSize[0];
  _arg2 = this->FrameSize[1];
  _arg3 = this->FrameSize[2];

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
FrameSizeType vtkPlusBuffer::GetFrameSize() const
{
  return this->FrameSize;
}

//-----------------------------------------------------------------------------
bool vtkPlusBuffer::GetLatestItemHasValidVideoData()
{
  return this->StreamBuffer->GetLatestItemHasValidVideoData();
}

//-----------------------------------------------------------------------------
bool vtkPlusBuffer::GetLatestItemHasValidTransformData()
{
  return this->StreamBuffer->GetLatestItemHasValidTransformData();
}

//----------------------------------------------------------------------------
bool vtkPlusBuffer::GetLatestItemHasValidFieldData()
{
  return this->StreamBuffer->GetLatestItemHasValidFieldData();
}

#undef LOCAL_LOG_ERROR
#undef LOCAL_LOG_WARNING
#undef LOCAL_LOG_DEBUG