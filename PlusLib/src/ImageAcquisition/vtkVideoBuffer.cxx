/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkVideoBuffer.h"
#include "vtkObjectFactory.h"
#include "vtkDoubleArray.h"
#include "vtkUnsignedLongLongArray.h"
#include "vtkCriticalSection.h"
#include "vtkImageData.h"
#include "vtkTrackedFrameList.h"
#include "TrackedFrame.h"

vtkCxxRevisionMacro(vtkVideoBuffer, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkVideoBuffer);

//----------------------------------------------------------------------------
//						VideoBufferItem
//----------------------------------------------------------------------------
VideoBufferItem::VideoBufferItem()
{
}

//----------------------------------------------------------------------------
VideoBufferItem::~VideoBufferItem()
{
}

//----------------------------------------------------------------------------
VideoBufferItem::VideoBufferItem(const VideoBufferItem &videoItem)
{
  *this = videoItem;
}

//----------------------------------------------------------------------------
VideoBufferItem& VideoBufferItem::operator=(VideoBufferItem const&videoItem)
{
  // Handle self-assignment
  if (this == &videoItem)
  {
    return *this;
  }

  this->Frame = videoItem.Frame;
  this->FilteredTimeStamp = videoItem.FilteredTimeStamp; 
  this->UnfilteredTimeStamp = videoItem.UnfilteredTimeStamp; 
  this->Index = videoItem.Index; 
  this->Uid = videoItem.Uid; 
  this->CustomFrameFields = videoItem.CustomFrameFields;

  return *this;
}

//----------------------------------------------------------------------------
PlusStatus VideoBufferItem::DeepCopy(VideoBufferItem* videoItem)
{
  if ( videoItem == NULL )
  {
    LOG_ERROR("Failed to deep copy video buffer item - buffer item NULL!"); 
    return PLUS_FAIL; 
  }

  (*this)=(*videoItem);

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
// vtkVideoBuffer
//----------------------------------------------------------------------------
vtkVideoBuffer::vtkVideoBuffer()
{
  this->FrameSize[0]=0; 
  this->FrameSize[1]=0;
  this->PixelType=itk::ImageIOBase::UCHAR; 
  this->VideoBuffer = vtkTimestampedCircularBuffer<VideoBufferItem>::New(); 
  AllocateMemoryForFrames();
}

//----------------------------------------------------------------------------
vtkVideoBuffer::~vtkVideoBuffer()
{ 
  if ( this->VideoBuffer != NULL )
  {
    this->VideoBuffer->Delete(); 
    this->VideoBuffer = NULL; 
  }
}

//----------------------------------------------------------------------------
void vtkVideoBuffer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Frame size in pixel: " << this->GetFrameSize()[0] << "   " << this->GetFrameSize()[1] << std::endl; 
  os << indent << "ITK scalar pixel type: " << this->GetPixelType() << std::endl; 
}

//----------------------------------------------------------------------------
PlusStatus vtkVideoBuffer::AllocateMemoryForFrames()
{
  PlusLockGuard<VideoBufferType> videoBufferGuardedLock(this->VideoBuffer);
  PlusStatus result=PLUS_SUCCESS;

  for ( int i = 0; i < this->VideoBuffer->GetBufferSize(); ++i )
  {
    if (this->VideoBuffer->GetBufferItemFromBufferIndex(i)->GetFrame().AllocateFrame(this->GetFrameSize(), this->GetPixelType())!=PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to allocate memory for frame "<<i);
      result=PLUS_FAIL;
    }
  }
  return result;
}

//----------------------------------------------------------------------------
void vtkVideoBuffer::SetLocalTimeOffsetSec(double offsetSec)
{
  this->VideoBuffer->SetLocalTimeOffsetSec(offsetSec); 
}

//----------------------------------------------------------------------------
double vtkVideoBuffer::GetLocalTimeOffsetSec()
{
  return this->VideoBuffer->GetLocalTimeOffsetSec(); 
}

//----------------------------------------------------------------------------
int vtkVideoBuffer::GetBufferSize()
{
  return this->VideoBuffer->GetBufferSize(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkVideoBuffer::SetBufferSize(int bufsize)
{
  if (bufsize<0)
  {
    LOG_ERROR("Invalid buffer size requested: "<<bufsize);
    return PLUS_FAIL;
  }
  if (this->VideoBuffer->GetBufferSize()==bufsize)
  {
    // no change
    return PLUS_SUCCESS;
  }
  
  PlusStatus result=PLUS_SUCCESS;
  if (this->VideoBuffer->SetBufferSize(bufsize)!=PLUS_SUCCESS)
  {
    result=PLUS_FAIL;
  }
  if (AllocateMemoryForFrames()!=PLUS_SUCCESS)
  {
    result=PLUS_FAIL;
  }

  return result;
}

//----------------------------------------------------------------------------
bool vtkVideoBuffer::CheckFrameFormat( const int frameSizeInPx[2], PlusCommon::ITKScalarPixelType pixelType)
{
  // don't add a frame if it doesn't match the buffer frame format
  if (frameSizeInPx[0] != this->GetFrameSize()[0]||
    frameSizeInPx[1] != this->GetFrameSize()[1] )
  {
    LOG_WARNING("Frame format and buffer frame format does not match (expected frame size: " << this->GetFrameSize()[0] 
    << "x" << this->GetFrameSize()[1] << "  received: " << frameSizeInPx[0] << "x" << frameSizeInPx[1] << ")!"); 
    return false;
  }

  if ( pixelType != this->GetPixelType() )
  {
    LOG_WARNING("Frame pixel type ("<<pixelType<<") and buffer pixel type (" << this->GetPixelType() <<") mismatch"); 
    return false; 
  }

  return true;
}

//----------------------------------------------------------------------------
PlusStatus vtkVideoBuffer::AddItem(void* imageDataPtr, US_IMAGE_ORIENTATION  usImageOrientation, 
  const int frameSizeInPx[2], PlusCommon::ITKScalarPixelType pixelType, int	numberOfBytesToSkip, long frameNumber,  
  double unfilteredTimestamp/*=UNDEFINED_TIMESTAMP*/, double filteredTimestamp/*=UNDEFINED_TIMESTAMP*/,
  const char* customFrameFieldName/*=NULL*/, const char* customFrameFieldValue/*=NULL*/)
{
  if (unfilteredTimestamp==UNDEFINED_TIMESTAMP)
  {
    unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();
  }

  if (filteredTimestamp==UNDEFINED_TIMESTAMP)
  {
    bool filteredTimestampProbablyValid=true;
    if ( this->VideoBuffer->CreateFilteredTimeStampForItem(frameNumber, unfilteredTimestamp, filteredTimestamp, filteredTimestampProbablyValid) != PLUS_SUCCESS )
    {
      LOG_WARNING("Failed to create filtered timestamp for video buffer item with item index: " << frameNumber ); 
      return PLUS_FAIL; 
    }
    if (!filteredTimestampProbablyValid)
    {
      LOG_INFO("Filtered timestamp is probably invalid for video buffer item with item index=" << frameNumber << ", time="<<unfilteredTimestamp<<". The item may have been tagged with an inaccurate timestamp, therefore it will not be recorded." ); 
      return PLUS_SUCCESS;
    }
  }

  if ( imageDataPtr == NULL )
  {
    LOG_ERROR( "vtkVideoBuffer: Unable to add NULL frame to video buffer!"); 
    return PLUS_FAIL; 
  }

  if ( !this->CheckFrameFormat(frameSizeInPx, pixelType) )
  {
    LOG_ERROR( "vtkVideoBuffer: Unable to add frame to video buffer - frame format doesn't match!"); 
    return PLUS_FAIL; 
  }

  int bufferIndex(0); 
  BufferItemUidType itemUid; 
  PlusLockGuard<VideoBufferType> videoBufferGuardedLock(this->VideoBuffer);
  if ( this->VideoBuffer->PrepareForNewItem(filteredTimestamp, itemUid, bufferIndex) != PLUS_SUCCESS )
  {
    // Just a debug message, because we want to avoid unnecessary warning messages if the timestamp is the same as last one
    LOG_DEBUG( "vtkVideoBuffer: Failed to prepare for adding new frame to video buffer!"); 
    return PLUS_FAIL; 
  }

  // get the pointer to the correct location in the frame buffer, where this data needs to be copied
  VideoBufferItem* newObjectInBuffer = this->VideoBuffer->GetBufferItemFromBufferIndex(bufferIndex); 
  if ( newObjectInBuffer == NULL )
  {
    LOG_ERROR( "vtkVideoBuffer: Failed to get pointer to video buffer object from the video buffer for the new frame!"); 
    return PLUS_FAIL; 
  }

  int receivedFrameSize[2]={0,0};
  newObjectInBuffer->GetFrame().GetFrameSize(receivedFrameSize); 

  if ( frameSizeInPx[0] != receivedFrameSize[0] 
    || frameSizeInPx[1] != receivedFrameSize[1] )
  {
    LOG_ERROR("Input frame size is different from buffer frame size (input: " << frameSizeInPx[0] << "x" << frameSizeInPx[1]
      << ",   buffer: " << receivedFrameSize[0] << "x" << receivedFrameSize[1] << ")!"); 
    return PLUS_FAIL; 
  }

  // Skip the numberOfBytesToSkip bytes, e.g. header size
  unsigned char* byteImageDataPtr=reinterpret_cast<unsigned char*>(imageDataPtr);
  byteImageDataPtr += numberOfBytesToSkip; 

  if (PlusVideoFrame::GetMFOrientedImage(byteImageDataPtr, usImageOrientation, frameSizeInPx, pixelType, newObjectInBuffer->GetFrame())!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to convert input US image to MF orientation!"); 
    return PLUS_FAIL; 
  }

  newObjectInBuffer->SetFilteredTimestamp(filteredTimestamp); 
  newObjectInBuffer->SetUnfilteredTimestamp(unfilteredTimestamp); 
  newObjectInBuffer->SetIndex(frameNumber); 
  newObjectInBuffer->SetUid(itemUid); 
  
  // Add a custom field
  if (customFrameFieldName!=NULL && customFrameFieldValue!=NULL)
  {
		newObjectInBuffer->SetCustomFrameField(customFrameFieldName, customFrameFieldValue);
  }
  else if (customFrameFieldName!=NULL && customFrameFieldValue==NULL)
  {
    LOG_WARNING("Custom field is not added to frame: customFrameFieldName is "<<customFrameFieldName<<", but customFrameFieldValue is undefined");
  }
  else if (customFrameFieldName==NULL && customFrameFieldValue!=NULL)
  {
    LOG_WARNING("Custom field is not added to frame: customFrameFieldValue is "<<customFrameFieldValue<<", but customFrameFieldName is undefined");
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkVideoBuffer::AddItem(vtkImageData* frame, US_IMAGE_ORIENTATION usImageOrientation, long frameNumber, double unfilteredTimestamp/*=UNDEFINED_TIMESTAMP*/, double filteredTimestamp/*=UNDEFINED_TIMESTAMP*/)
{
  if ( frame == NULL )
  {
    LOG_ERROR( "vtkVideoBuffer: Unable to add NULL frame to video buffer!"); 
    return PLUS_FAIL; 
  }

  if (unfilteredTimestamp==UNDEFINED_TIMESTAMP)
  {
    unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();
  }

  if (filteredTimestamp==UNDEFINED_TIMESTAMP)
  {
    bool filteredTimestampProbablyValid=true;
    if ( this->VideoBuffer->CreateFilteredTimeStampForItem(frameNumber, unfilteredTimestamp, filteredTimestamp, filteredTimestampProbablyValid) != PLUS_SUCCESS )
    {
      LOG_WARNING("Failed to create filtered timestamp for video buffer item with item index: " << frameNumber ); 
      return PLUS_FAIL; 
    }
    if (!filteredTimestampProbablyValid)
    {
      LOG_INFO("Filtered timestamp is probably invalid for video buffer item with item index=" << frameNumber << ", time="<<unfilteredTimestamp<<". The item may have been tagged with an inaccurate timestamp, therefore it will not be recorded." ); 
      return PLUS_SUCCESS;
    }
  }

  vtkSmartPointer<vtkImageData> mfOrientedImage = vtkSmartPointer<vtkImageData>::New(); 
  if ( PlusVideoFrame::GetMFOrientedImage(frame, usImageOrientation, mfOrientedImage) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to add video item to buffer: couldn't get MF oriented frame!"); 
    return PLUS_FAIL; 
  }

  const int* frameExtent = mfOrientedImage->GetExtent(); 
  const int frameSize[2] = {(frameExtent[1] - frameExtent[0] + 1), (frameExtent[3] - frameExtent[2] + 1)}; 
  PlusCommon::ITKScalarPixelType pixelType=PlusVideoFrame::GetITKScalarPixelType(frame->GetScalarType());
  return this->AddItem( reinterpret_cast<unsigned char*>(mfOrientedImage->GetScalarPointer()), US_IMG_ORIENT_MF , frameSize, pixelType, 0, frameNumber, unfilteredTimestamp, filteredTimestamp); 
}

//----------------------------------------------------------------------------
PlusStatus vtkVideoBuffer::AddItem(const PlusVideoFrame* frame, US_IMAGE_ORIENTATION usImageOrientation, long frameNumber, double unfilteredTimestamp/*=UNDEFINED_TIMESTAMP*/, double filteredTimestamp/*=UNDEFINED_TIMESTAMP*/)
{
  if ( frame == NULL )
  {
    LOG_ERROR( "vtkVideoBuffer: Unable to add NULL frame to video buffer!"); 
    return PLUS_FAIL; 
  }

  if (unfilteredTimestamp==UNDEFINED_TIMESTAMP)
  {
    unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();
  }

  if (filteredTimestamp==UNDEFINED_TIMESTAMP)
  {
    bool filteredTimestampProbablyValid=true;
    if ( this->VideoBuffer->CreateFilteredTimeStampForItem(frameNumber, unfilteredTimestamp, filteredTimestamp, filteredTimestampProbablyValid) != PLUS_SUCCESS )
    {
      LOG_WARNING("Failed to create filtered timestamp for video buffer item with item index: " << frameNumber ); 
      return PLUS_FAIL; 
    }
    if (!filteredTimestampProbablyValid)
    {
      LOG_INFO("Filtered timestamp is probably invalid for video buffer item with item index=" << frameNumber << ", time="<<unfilteredTimestamp<<". The item may have been tagged with an inaccurate timestamp, therefore it will not be recorded." ); 
      return PLUS_SUCCESS;
    }
  }

  unsigned char* pixelBufferPointer = static_cast<unsigned char*>(frame->GetBufferPointer()); 
  int frameSize[2]={0,0};
  frame->GetFrameSize(frameSize);    

  return this->AddItem(pixelBufferPointer, usImageOrientation, frameSize, frame->GetITKScalarPixelType(), 0 /* no skip*/, frameNumber, unfilteredTimestamp, filteredTimestamp);  
}

//----------------------------------------------------------------------------
ItemStatus vtkVideoBuffer::GetLatestTimeStamp( double& latestTimestamp )
{
  return this->VideoBuffer->GetLatestTimeStamp(latestTimestamp); 
}

//----------------------------------------------------------------------------
ItemStatus vtkVideoBuffer::GetOldestTimeStamp( double& oldestTimestamp )
{
  return this->VideoBuffer->GetOldestTimeStamp(oldestTimestamp); 
}

//----------------------------------------------------------------------------
ItemStatus vtkVideoBuffer::GetTimeStamp( BufferItemUidType uid, double& timestamp)
{
  return this->VideoBuffer->GetTimeStamp(uid, timestamp); 
}

//----------------------------------------------------------------------------
void vtkVideoBuffer::SetAveragedItemsForFiltering( int averagedItemsForFiltering)
{
  this->VideoBuffer->SetAveragedItemsForFiltering(averagedItemsForFiltering); 
}

//----------------------------------------------------------------------------
void vtkVideoBuffer::SetStartTime( double startTime)
{
  this->VideoBuffer->SetStartTime(startTime); 
}

//----------------------------------------------------------------------------
double vtkVideoBuffer::GetStartTime()
{
  return this->VideoBuffer->GetStartTime(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkVideoBuffer::GetTimeStampReportTable(vtkTable* timeStampReportTable)
{
  return this->VideoBuffer->GetTimeStampReportTable(timeStampReportTable); 
}

//----------------------------------------------------------------------------
ItemStatus vtkVideoBuffer::GetVideoBufferItem(BufferItemUidType uid, VideoBufferItem* bufferItem)
{
  if ( bufferItem == NULL )
  {
    LOG_ERROR("Unable to copy video buffer item into a NULL video buffer item!"); 
    return ITEM_UNKNOWN_ERROR; 
  }

  ItemStatus status = this->VideoBuffer->GetFrameStatus(uid); 
  if ( status != ITEM_OK )
  {
    if (  status == ITEM_NOT_AVAILABLE_ANYMORE )
    {
      LOG_WARNING("Failed to get video buffer item: video item not available anymore"); 
    }
    else if (  status == ITEM_NOT_AVAILABLE_YET )
    {
      LOG_WARNING("Failed to get video buffer item: video item not available yet"); 
    }
    else
    {
      LOG_WARNING("Failed to get video buffer item!"); 
    }
    return status; 
  }

  VideoBufferItem* videoItem = this->VideoBuffer->GetBufferItemFromUid(uid); 

  if ( bufferItem->DeepCopy(videoItem) != PLUS_SUCCESS )
  {
    LOG_WARNING("Failed to copy video item!"); 
    return ITEM_UNKNOWN_ERROR; 
  }

  // Check the status again to make sure the writer didn't change it
  return this->VideoBuffer->GetFrameStatus(uid); 
}

//----------------------------------------------------------------------------
ItemStatus vtkVideoBuffer::GetVideoBufferItemFromTime( double time, VideoBufferItem* bufferItem)
{
  BufferItemUidType uid(0); 
  ItemStatus status = this->VideoBuffer->GetItemUidFromTime(time, uid); 
  if ( status != ITEM_OK )
  {
    if (  status == ITEM_NOT_AVAILABLE_ANYMORE )
    {
      LOG_WARNING("Failed to get video buffer item from time: video item not available anymore"); 
    }
    else if (  status == ITEM_NOT_AVAILABLE_YET )
    {
      LOG_WARNING("Failed to get video buffer item from time: video item not available yet"); 

    }
    else
    {
      LOG_WARNING("Failed to get video buffer item from time!"); 
    }
    return status; 
  }

  return this->GetVideoBufferItem(uid, bufferItem);  
}

//----------------------------------------------------------------------------
void vtkVideoBuffer::DeepCopy(vtkVideoBuffer* buffer)
{
  this->VideoBuffer->DeepCopy( buffer->VideoBuffer ); 
  this->SetFrameSize( buffer->GetFrameSize() ); 
  this->SetPixelType(buffer->GetPixelType());
  this->SetBufferSize(buffer->GetBufferSize()); 
}

//----------------------------------------------------------------------------
void vtkVideoBuffer::Clear()
{
  this->VideoBuffer->Clear(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkVideoBuffer::SetFrameSize(int x, int y)
{
  if (x<0 || y<0)
  {
    LOG_ERROR("Invalid frame size requested: "<<x<<", "<<y);
    return PLUS_FAIL;
  }
  if (this->FrameSize[0]==x && this->FrameSize[1]==y)
  {
    // no change
    return PLUS_SUCCESS;
  }
  this->FrameSize[0]=x;
  this->FrameSize[1]=y;
  return AllocateMemoryForFrames();
}

//----------------------------------------------------------------------------
PlusStatus vtkVideoBuffer::SetFrameSize(int frameSize[2])
{
  return SetFrameSize(frameSize[0], frameSize[1]);
}

//----------------------------------------------------------------------------
PlusStatus vtkVideoBuffer::SetPixelType(PlusCommon::ITKScalarPixelType pixelType) 
{
  if (pixelType==this->PixelType)
  {
    // no change
    return PLUS_SUCCESS;
  }
  this->PixelType=pixelType;
  return AllocateMemoryForFrames();
}

//----------------------------------------------------------------------------
int vtkVideoBuffer::GetNumberOfBytesPerPixel()
{
  return PlusVideoFrame::GetNumberOfBytesPerPixel(GetPixelType());
}

//----------------------------------------------------------------------------
PlusStatus vtkVideoBuffer::CopyImagesFromTrackedFrameList(vtkTrackedFrameList *sourceTrackedFrameList, TIMESTAMP_FILTERING_OPTION timestampFiltering)
{
  int numberOfErrors=0;

  const int numberOfVideoFrames = sourceTrackedFrameList->GetNumberOfTrackedFrames(); 
  LOG_DEBUG("CopyImagesFromTrackedFrameList will copy "<< numberOfVideoFrames<< " frames"); 

  int frameSize[2]={0,0};
  sourceTrackedFrameList->GetTrackedFrame(0)->GetImageData()->GetFrameSize(frameSize);
  this->SetFrameSize(frameSize); 
  this->SetPixelType(sourceTrackedFrameList->GetTrackedFrame(0)->GetImageData()->GetITKScalarPixelType());

  if ( this->SetBufferSize(numberOfVideoFrames) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set video buffer size!"); 
    return PLUS_FAIL;
  }

  bool requireTimestamp=false;  
  if (timestampFiltering == READ_FILTERED_AND_UNFILTERED_TIMESTAMPS || timestampFiltering == READ_FILTERED_IGNORE_UNFILTERED_TIMESTAMPS)
  {
    requireTimestamp=true;
  }

  bool requireUnfilteredTimestamp=false;
  if (timestampFiltering == READ_FILTERED_AND_UNFILTERED_TIMESTAMPS || timestampFiltering == READ_UNFILTERED_COMPUTE_FILTERED_TIMESTAMPS)
  {
    requireUnfilteredTimestamp=true;
  }

  bool requireFrameStatus=false;
  bool requireFrameNumber=false;
  if (timestampFiltering==READ_UNFILTERED_COMPUTE_FILTERED_TIMESTAMPS)
  {
    // frame status and number is required for the filtered timestamp computation
    requireFrameStatus=true;
    requireFrameNumber=true;
  }

  LOG_INFO("Copy buffer to video buffer..."); 
  for ( int frameNumber = 0; frameNumber < numberOfVideoFrames; frameNumber++ )
  {
    // read filtered timestamp
    double timestamp(0); 
    const char* strTimestamp = sourceTrackedFrameList->GetTrackedFrame(frameNumber)->GetCustomFrameField("Timestamp");
    if ( strTimestamp != NULL )
    {
      if ( PlusCommon::StringToDouble(strTimestamp, timestamp) != PLUS_SUCCESS && requireTimestamp )
      {
        LOG_ERROR("Unable to convert Timestamp '"<< strTimestamp << "' to double for frame #" << frameNumber); 
        numberOfErrors++; 
        continue; 
      }
    }
    else if (requireTimestamp)
    {
      LOG_ERROR("Unable to read Timestamp field of frame #" << frameNumber); 
      numberOfErrors++; 
      continue; 
    }

    // read unfiltered timestamp
    double unfilteredtimestamp(0);  
    const char* strUnfilteredTimestamp = sourceTrackedFrameList->GetTrackedFrame(frameNumber)->GetCustomFrameField("UnfilteredTimestamp"); 
    if ( strUnfilteredTimestamp != NULL )
    {
      if ( PlusCommon::StringToDouble(strUnfilteredTimestamp, unfilteredtimestamp) != PLUS_SUCCESS && requireUnfilteredTimestamp )
      {
        LOG_ERROR("Unable to convert UnfilteredTimestamp '"<< strUnfilteredTimestamp << "' to double for frame #" << frameNumber); 
        numberOfErrors++; 
        continue; 
      }
    }
    else if (requireUnfilteredTimestamp)
    {
      LOG_ERROR("Unable to read UnfilteredTimestamp field of frame #" << frameNumber); 
      numberOfErrors++; 
      continue; 
    }

    // read frame number
    const char* strFrameNumber = sourceTrackedFrameList->GetTrackedFrame(frameNumber)->GetCustomFrameField("FrameNumber"); 
    unsigned long frmnum(0); 
    if ( strFrameNumber != NULL )
    {
      if ( PlusCommon::StringToLong(strFrameNumber, frmnum) != PLUS_SUCCESS && requireFrameNumber )
      {
        LOG_ERROR("Unable to convert FrameNumber '"<< strFrameNumber << "' to integer for frame #" << frameNumber); 
        numberOfErrors++; 
        continue; 
      }
    }
    else if (requireFrameNumber)
    {
      LOG_ERROR("Unable to read FrameNumber field of frame #" << frameNumber); 
      numberOfErrors++; 
      continue; 
    }
    
    // AddItem is called with US_IMG_ORIENT_MF orientation always
    // because images in the tracked frame list always stored in MF orientation 
    switch (timestampFiltering)
    {
    case READ_FILTERED_AND_UNFILTERED_TIMESTAMPS:
      if ( this->AddItem(sourceTrackedFrameList->GetTrackedFrame(frameNumber)->GetImageData(), US_IMG_ORIENT_MF, frmnum, unfilteredtimestamp, timestamp) != PLUS_SUCCESS )
      {
        LOG_WARNING("Failed to add video frame to buffer from sequence metafile with frame #" << frameNumber ); 
      }
      break;
    case READ_UNFILTERED_COMPUTE_FILTERED_TIMESTAMPS:
      if ( this->AddItem(sourceTrackedFrameList->GetTrackedFrame(frameNumber)->GetImageData(), US_IMG_ORIENT_MF, frmnum, unfilteredtimestamp) != PLUS_SUCCESS )
      {
        LOG_WARNING("Failed to add video frame to buffer from sequence metafile with frame #" << frameNumber ); 
      }
      break;
    case READ_FILTERED_IGNORE_UNFILTERED_TIMESTAMPS:
      if ( this->AddItem(sourceTrackedFrameList->GetTrackedFrame(frameNumber)->GetImageData(), US_IMG_ORIENT_MF, frmnum, timestamp, timestamp) != PLUS_SUCCESS )
      {
        LOG_WARNING("Failed to add video frame to buffer from sequence metafile with frame #" << frameNumber ); 
      }
      break;
    default:
      break;
    }

  }

  return (numberOfErrors>0 ? PLUS_FAIL:PLUS_SUCCESS );
}

//----------------------------------------------------------------------------
PlusStatus vtkVideoBuffer::WriteToMetafile( const char* outputFolder, const char* metaFileName, bool useCompression /*=false*/ )
{
  LOG_TRACE("vtkVideoBuffer::WriteToMetafile");

  const int numberOfFrames = this->GetNumberOfItems();
  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();

  PlusStatus status = PLUS_SUCCESS;

  for ( BufferItemUidType frameUid = this->GetOldestItemUidInBuffer(); frameUid <= this->GetLatestItemUidInBuffer(); ++frameUid )
  {
    VideoBufferItem videoItem;
    if ( this->GetVideoBufferItem(frameUid, &videoItem) != ITEM_OK )
    {
      LOG_ERROR("Unable to get frame from buffer with UID: " << frameUid);
      status=PLUS_FAIL;
      continue;
    }

    TrackedFrame trackedFrame;
    trackedFrame.SetImageData(videoItem.GetFrame());

    // Add filtered timestamp
    double filteredTimestamp = videoItem.GetFilteredTimestamp( this->GetLocalTimeOffsetSec() );
    std::ostringstream timestampFieldValue;
    timestampFieldValue << std::fixed << filteredTimestamp;
    trackedFrame.SetCustomFrameField("Timestamp", timestampFieldValue.str());

    // Add unfiltered timestamp
    double unfilteredTimestamp = videoItem.GetUnfilteredTimestamp( this->GetLocalTimeOffsetSec() );
    std::ostringstream unfilteredtimestampFieldValue;
    unfilteredtimestampFieldValue << std::fixed << unfilteredTimestamp;
    trackedFrame.SetCustomFrameField("UnfilteredTimestamp", unfilteredtimestampFieldValue.str());

    // Add frame number
    unsigned long frameNumber = videoItem.GetIndex(); 
    std::ostringstream frameNumberFieldValue;
    frameNumberFieldValue << std::fixed << frameNumber;
    trackedFrame.SetCustomFrameField("FrameNumber", frameNumberFieldValue.str());

    // Add tracked frame to the list
    trackedFrameList->AddTrackedFrame(&trackedFrame);
  }

  // Save tracked frames to metafile
  if ( trackedFrameList->SaveToSequenceMetafile(outputFolder, metaFileName, vtkTrackedFrameList::SEQ_METAFILE_MHA, useCompression) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to save tracked frames to sequence metafile!");
    return PLUS_FAIL;
  }

  return status;
}
