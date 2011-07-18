#include "PlusConfigure.h"
#include "vtkVideoBuffer.h"
#include "vtkObjectFactory.h"
#include "vtkDoubleArray.h"
#include "vtkUnsignedLongLongArray.h"
#include "vtkCriticalSection.h"
#include "vtkImageData.h"

#include "itkImage.h"

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

  if ( videoItem.GetFrame().IsNotNull() )
  {
    int frameSize[2] = {videoItem.GetFrame()->GetLargestPossibleRegion().GetSize()[0], videoItem.GetFrame()->GetLargestPossibleRegion().GetSize()[1]}; 
    if ( this->AllocateFrame(frameSize) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to allocate memory for the new frame in the buffer!"); 
    }
    else
    {
      memcpy(this->Frame->GetBufferPointer(), videoItem.GetFrame()->GetBufferPointer(), this->GetFrameSizeInBytes() ); 
    }
  }

  this->FilteredTimeStamp = videoItem.FilteredTimeStamp; 
  this->UnfilteredTimeStamp = videoItem.UnfilteredTimeStamp; 
  this->Index = videoItem.Index; 
  this->Uid = videoItem.Uid; 

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

  if ( videoItem->GetFrame().IsNotNull() )
  {
    int frameSize[2] = {videoItem->GetFrame()->GetLargestPossibleRegion().GetSize()[0], videoItem->GetFrame()->GetLargestPossibleRegion().GetSize()[1]}; 
    if ( this->AllocateFrame(frameSize) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to allocate memory for the new frame in the buffer!"); 
    }
    else
    {
      memcpy(this->Frame->GetBufferPointer(), videoItem->GetFrame()->GetBufferPointer(), this->GetFrameSizeInBytes() ); 
    }
  }

  this->FilteredTimeStamp = videoItem->FilteredTimeStamp; 
  this->UnfilteredTimeStamp = videoItem->UnfilteredTimeStamp; 
  this->Index = videoItem->Index; 
  this->Uid = videoItem->Uid; 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus VideoBufferItem::AllocateFrame(int imageSize[2])
{
  if ( this->Frame.IsNull() )
  {
    this->Frame = ImageType::New(); 
  }

  ImageType::SizeType size = {imageSize[0], imageSize[1]};
  ImageType::IndexType start = {0,0};
  ImageType::RegionType region;
  region.SetSize(size);
  region.SetIndex(start);
  this->Frame->SetRegions(region);

  try
  {
    this->Frame->Allocate();
  }
  catch (itk::ExceptionObject & err) 
  {		
    LOG_ERROR("Unable to allocate memory for image: " << err.GetDescription() );
    return PLUS_FAIL;
  }	

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus VideoBufferItem::SetFrame(const ImageType::Pointer& frame)
{
  if ( this->Frame.IsNull() )
  {
    LOG_ERROR("Failed to add frame into video buffer item - frame is NULL!"); 
    return PLUS_FAIL;     
  }

  if ( frame->GetLargestPossibleRegion() != this->Frame->GetLargestPossibleRegion() )
  {
    LOG_ERROR("Input image region doesn't match buffer image region!"); 
    return PLUS_FAIL; 
  }

  memcpy(this->Frame->GetBufferPointer(), frame->GetBufferPointer(), this->GetFrameSizeInBytes() ); 

  return PLUS_SUCCESS; 
}


//----------------------------------------------------------------------------
unsigned long VideoBufferItem::GetFrameSizeInBytes()
{
  const unsigned long imageWidthInPixels = this->Frame->GetLargestPossibleRegion().GetSize()[0]; 
  const unsigned long imageHeightInPixels = this->Frame->GetLargestPossibleRegion().GetSize()[1]; 
  unsigned long frameSizeInBytes = imageWidthInPixels * imageHeightInPixels * sizeof(PixelType);

  return frameSizeInBytes; 
}

//----------------------------------------------------------------------------
PlusStatus VideoBufferItem::SetFrame(vtkImageData* frame)
{
  if ( this->Frame.IsNull() )
  {
    LOG_ERROR("Failed to add frame into video buffer item - frame is NULL!"); 
    return PLUS_FAIL; 
  }

  const unsigned long imageWidthInPixels = this->Frame->GetLargestPossibleRegion().GetSize()[0]; 
  const unsigned long imageHeightInPixels = this->Frame->GetLargestPossibleRegion().GetSize()[1]; 

  int* frameExtent = frame->GetExtent(); 

  if ( ( frameExtent[1] - frameExtent[0] + 1 ) != imageWidthInPixels ||
    ( frameExtent[3] - frameExtent[2] + 1 ) != imageHeightInPixels )
  {
    LOG_ERROR("Input image size doesn't match buffer image size!"); 
    return PLUS_FAIL; 
  }

  return UsImageConverterCommon::ConvertVtkImageToItkImage(frame, this->Frame); 
}

//----------------------------------------------------------------------------
PlusStatus VideoBufferItem::SetFrame(unsigned char *imageDataPtr, 
                                     const int frameSizeInPx[3],
                                     int numberOfBitsPerPixel, 
                                     int numberOfBytesToSkip )
{
  if ( imageDataPtr == NULL )
  {
    LOG_ERROR( "Failed to add NULL frame to video buffer!"); 
    return PLUS_FAIL;
  }

  if ( this->Frame.IsNull() )
  {
    LOG_ERROR( "Unable to add frame to video buffer - need to allocate frame first!"); 
    return PLUS_FAIL; 
  }

  const unsigned long imageWidthInPixels = this->Frame->GetLargestPossibleRegion().GetSize()[0]; 
  const unsigned long imageHeightInPixels = this->Frame->GetLargestPossibleRegion().GetSize()[1]; 

  if ( frameSizeInPx[0] != imageWidthInPixels 
    || frameSizeInPx[1] != imageHeightInPixels )
  {
    LOG_ERROR("Input frame size is different from buffer frame size (input: " << frameSizeInPx[0] << "x" << frameSizeInPx[1]
      << ",   buffer: " << imageWidthInPixels << "x" << imageHeightInPixels << ")!"); 
    return PLUS_FAIL; 
  }

  // Skip the numberOfBytesToSkip bytes, e.g. header size
  imageDataPtr += numberOfBytesToSkip; 

  memcpy(this->Frame->GetBufferPointer(), imageDataPtr, this->GetFrameSizeInBytes() );

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
//						vtkVideoBuffer
//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkVideoBuffer, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkVideoBuffer);

//----------------------------------------------------------------------------
vtkVideoBuffer::vtkVideoBuffer()
{
  this->VideoBuffer = vtkTimestampedCircularBuffer<VideoBufferItem>::New(); 
  this->SetFrameSize(0,0); 
  this->SetNumberOfBitsPerPixel(0); 
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
  os << indent << "Number of bits per pixel: " << this->GetNumberOfBitsPerPixel() << std::endl; 
}

//----------------------------------------------------------------------------
void vtkVideoBuffer::UpdateBufferFrameFormats()
{
  this->VideoBuffer->Lock(); 
  for ( int i = 0; i < this->VideoBuffer->GetBufferSize(); ++i )
  {
    this->VideoBuffer->GetBufferItem(i)->AllocateFrame(this->GetFrameSize()); 
  }
  this->VideoBuffer->Unlock(); 
}

//----------------------------------------------------------------------------
void vtkVideoBuffer::SetLocalTimeOffset(double offset)
{
  this->VideoBuffer->SetLocalTimeOffset(offset); 
}

//----------------------------------------------------------------------------
double vtkVideoBuffer::GetLocalTimeOffset()
{
  return this->VideoBuffer->GetLocalTimeOffset(); 
}

//----------------------------------------------------------------------------
int vtkVideoBuffer::GetBufferSize()
{
  return this->VideoBuffer->GetBufferSize(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkVideoBuffer::SetBufferSize(int bufsize)
{
  return this->VideoBuffer->SetBufferSize(bufsize); 
}

//----------------------------------------------------------------------------
bool vtkVideoBuffer::CheckFrameFormat( const int frameSizeInPx[2], int numberOfBitsPerPixel )
{
  // don't add a frame if it doesn't match the buffer frame format
  if (frameSizeInPx[0] != this->GetFrameSize()[0]||
    frameSizeInPx[1] != this->GetFrameSize()[1] )
  {
    LOG_WARNING("Frame format and buffer frame format does not match (expected frame size: " << this->GetFrameSize()[0] 
    << "x" << this->GetFrameSize()[1] << "  received: " << frameSizeInPx[0] << "x" << frameSizeInPx[1] << ")!"); 
    return false;
  }

  if ( numberOfBitsPerPixel != this->GetNumberOfBitsPerPixel() )
  {
    LOG_WARNING("Frame format and buffer frame format does not match (expected pixel size: " << this->GetNumberOfBitsPerPixel() 
      << "bits  received:" << numberOfBitsPerPixel << "bits)!"); 
    return false; 
  }

  return true;
}

//----------------------------------------------------------------------------
PlusStatus vtkVideoBuffer::AddItem(unsigned char* imageDataPtr,                               
                                   US_IMAGE_ORIENTATION  usImageOrientation, 
                                   const int    frameSizeInPx[2],
                                   int    numberOfBitsPerPixel, 
                                   int	numberOfBytesToSkip, 
                                   long   frameNumber)
{
  double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();
  return this->AddTimeStampedItem(imageDataPtr, usImageOrientation, frameSizeInPx, numberOfBitsPerPixel, numberOfBytesToSkip, unfilteredTimestamp, frameNumber); 
}

//----------------------------------------------------------------------------
PlusStatus vtkVideoBuffer::AddTimeStampedItem(unsigned char* imageDataPtr,                               
                                              US_IMAGE_ORIENTATION  usImageOrientation, 
                                              const int    frameSizeInPx[2],
                                              int    numberOfBitsPerPixel, 
                                              int	numberOfBytesToSkip, 
                                              double unfilteredTimestamp, 
                                              long   frameNumber)
{

  double filteredTimestamp(0); 
  if ( this->VideoBuffer->CreateFilteredTimeStampForItem(frameNumber, unfilteredTimestamp, filteredTimestamp) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to create filtered timestamp for buffer item with item index: " << frameNumber ); 
    return PLUS_FAIL; 
  }

  return this->AddTimeStampedItem(imageDataPtr, usImageOrientation, frameSizeInPx, numberOfBitsPerPixel, numberOfBytesToSkip, unfilteredTimestamp, filteredTimestamp, frameNumber); 
}


//----------------------------------------------------------------------------
PlusStatus vtkVideoBuffer::AddTimeStampedItem(unsigned char* imageDataPtr,                               
                                              US_IMAGE_ORIENTATION  usImageOrientation, 
                                              const int    frameSizeInPx[2],
                                              int    numberOfBitsPerPixel, 
                                              int	numberOfBytesToSkip, 
                                              double unfilteredTimestamp, 
                                              double filteredTimestamp,
                                              long   frameNumber)
{
  if ( imageDataPtr == NULL )
  {
    LOG_ERROR( "vtkVideoBuffer: Unable to add NULL frame to video buffer!"); 
    return PLUS_FAIL; 
  }

  if ( !this->CheckFrameFormat(frameSizeInPx, numberOfBitsPerPixel) )
  {
    LOG_ERROR( "vtkVideoBuffer: Unable to add frame to video buffer - frame format doesn't match!"); 
    return PLUS_FAIL; 
  }

  if ( UsImageConverterCommon::GetMFOrientedImage(imageDataPtr, usImageOrientation, frameSizeInPx, numberOfBitsPerPixel) != PLUS_SUCCESS )
  {
	LOG_ERROR("Failed to convert input US image to MF orientation!"); 
	return PLUS_FAIL; 
  }

  int bufferIndex(0); 
  BufferItemUidType itemUid; 
  this->VideoBuffer->Lock(); 
  if ( this->VideoBuffer->PrepareForNewItem(filteredTimestamp, itemUid, bufferIndex) != PLUS_SUCCESS )
  {
    this->VideoBuffer->Unlock(); 
    // Just a debug message, because we want to avoid unnecessary warning messages if the timestamp is the same as last one
    LOG_DEBUG( "vtkVideoBuffer: Failed to prepare for adding new frame to video buffer!"); 
    return PLUS_FAIL; 
  }

  // get the pointer to the correct location in the frame buffer, where this data needs to be copied
  VideoBufferItem* newObjectInBuffer = this->VideoBuffer->GetBufferItem(bufferIndex); 
  if ( newObjectInBuffer == NULL )
  {
    this->VideoBuffer->Unlock(); 
    LOG_ERROR( "vtkVideoBuffer: Failed to get pointer to video buffer object from the video buffer for the new frame!"); 
    return PLUS_FAIL; 
  }

  PlusStatus status = newObjectInBuffer->SetFrame(imageDataPtr, frameSizeInPx, numberOfBitsPerPixel, numberOfBytesToSkip); 
  newObjectInBuffer->SetFilteredTimestamp(filteredTimestamp); 
  newObjectInBuffer->SetUnfilteredTimestamp(unfilteredTimestamp); 
  newObjectInBuffer->SetIndex(frameNumber); 
  newObjectInBuffer->SetUid(itemUid); 
  this->VideoBuffer->Unlock(); 

  return status; 
}

//----------------------------------------------------------------------------
PlusStatus vtkVideoBuffer::AddItem(vtkImageData* frame, US_IMAGE_ORIENTATION usImageOrientation, long frameNumber)
{
  double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();
  return this->AddTimeStampedItem(frame, usImageOrientation, unfilteredTimestamp, frameNumber); 
}

//----------------------------------------------------------------------------
PlusStatus vtkVideoBuffer::AddTimeStampedItem(vtkImageData* frame, US_IMAGE_ORIENTATION usImageOrientation, double unfilteredTimestamp, long frameNumber)
{
  vtkSmartPointer<vtkImageData> mfOrientedImage = vtkSmartPointer<vtkImageData>::New(); 
  if ( UsImageConverterCommon::GetMFOrientedImage(frame, usImageOrientation, mfOrientedImage) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to add video item to buffer: couldn't get MF oriented frame!"); 
    return PLUS_FAIL; 
  }

  const int* frameExtent = mfOrientedImage->GetExtent(); 
  const int frameSize[2] = {(frameExtent[1] - frameExtent[0] + 1), (frameExtent[3] - frameExtent[2] + 1)}; 
  const int numberOfBits = mfOrientedImage->GetScalarSize() * 8; 
  return this->AddTimeStampedItem( reinterpret_cast<unsigned char*>(mfOrientedImage->GetScalarPointer()), US_IMG_ORIENT_MF , frameSize, numberOfBits, 0, unfilteredTimestamp, frameNumber); 
}

//----------------------------------------------------------------------------
PlusStatus vtkVideoBuffer::AddTimeStampedItem(vtkImageData* frame, US_IMAGE_ORIENTATION usImageOrientation, double unfilteredTimestamp, double filteredTimestamp, long frameNumber)
{
  vtkSmartPointer<vtkImageData> mfOrientedImage = vtkSmartPointer<vtkImageData>::New(); 
  if ( UsImageConverterCommon::GetMFOrientedImage(frame, usImageOrientation, mfOrientedImage) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to add video item to buffer: couldn't get MF oriented frame!"); 
    return PLUS_FAIL; 
  }

  const int* frameExtent = mfOrientedImage->GetExtent(); 
  const int frameSize[2] = {(frameExtent[1] - frameExtent[0] + 1), (frameExtent[3] - frameExtent[2] + 1)}; 
  const int numberOfBits = mfOrientedImage->GetScalarSize() * 8; 
  return this->AddTimeStampedItem( reinterpret_cast<unsigned char*>(mfOrientedImage->GetScalarPointer()), US_IMG_ORIENT_MF , frameSize, numberOfBits, 0, unfilteredTimestamp, filteredTimestamp, frameNumber); 
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
void vtkVideoBuffer::SetSmoothingFactor( double smoothingFactor)
{
  this->VideoBuffer->SetSmoothingFactor(smoothingFactor); 
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

  VideoBufferItem* videoItem = this->VideoBuffer->GetBufferItem(uid); 

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
  this->SetNumberOfBitsPerPixel( buffer->GetNumberOfBitsPerPixel() ); 
  this->SetBufferSize(buffer->GetBufferSize()); 
  this->UpdateBufferFrameFormats(); 
}

//----------------------------------------------------------------------------
void vtkVideoBuffer::Clear()
{
  this->VideoBuffer->Clear(); 
}


