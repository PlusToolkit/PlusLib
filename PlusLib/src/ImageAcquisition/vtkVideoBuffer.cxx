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
        LOG_ERROR("Unable to allocate memory for image: " << err);
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
									 const int numberOfBitsPerPixel, 
									 const int	numberOfBytesToSkip )
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
		LOG_ERROR("Input frame size is different from buffer frame size (input: " << frameSizeInPx[0] << "x" << frameSizeInPx[1] << "x" 
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
bool vtkVideoBuffer::CheckFrameFormat( const int frameSizeInPx[2], const int numberOfBitsPerPixel )
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
                                   const char*  usImageOrientation, 
                                   const int    frameSizeInPx[2],
                                   const int    numberOfBitsPerPixel, 
                                   const int	numberOfBytesToSkip, 
                                   const double unfilteredTimestamp, 
                                   const double filteredTimestamp, 
                                   const long   frameNumber)
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

	int bufferIndex(0); 
	BufferItemUidType itemUid; 
    this->VideoBuffer->Lock(); 
	if ( this->VideoBuffer->PrepareForNewFrame(filteredTimestamp, itemUid, bufferIndex) != PLUS_SUCCESS )
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
PlusStatus vtkVideoBuffer::AddItem(vtkImageData* frame, const char* usImageOrientation, const double unfilteredTimestamp, const double filteredTimestamp, const long frameNumber)
{
    UsImageConverterCommon::US_IMAGE_ORIENTATION imgOrientation = UsImageConverterCommon::GetUsImageOrientationFromString(usImageOrientation); 

    vtkSmartPointer<vtkImageData> mfOrientedImage = vtkSmartPointer<vtkImageData>::New(); 
    if ( UsImageConverterCommon::GetMFOrientedImage(frame, imgOrientation, mfOrientedImage) != PLUS_SUCCESS )
    {
        LOG_ERROR("Failed to add video item to buffer: couldn't get MF oriented frame!"); 
        return PLUS_FAIL; 
    }

	const int* frameExtent = mfOrientedImage->GetExtent(); 
	const int frameSize[3] = {(frameExtent[1] - frameExtent[0] + 1), (frameExtent[3] - frameExtent[2] + 1), (frameExtent[5] - frameExtent[4] + 1) }; 
	const int numberOfBits = mfOrientedImage->GetScalarSize() * 8; 
    return this->AddItem( reinterpret_cast<unsigned char*>(mfOrientedImage->GetScalarPointer()), "MF" , frameSize, numberOfBits, 0, unfilteredTimestamp, filteredTimestamp, frameNumber); 
}

//----------------------------------------------------------------------------
ItemStatus vtkVideoBuffer::GetLatestTimeStamp( double& latestTimestamp )
{
    return this->VideoBuffer->GetLatestTimeStamp(latestTimestamp); 
}

//----------------------------------------------------------------------------
ItemStatus vtkVideoBuffer::GetVideoBufferItem(const BufferItemUidType uid, VideoBufferItem* bufferItem)
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
		status = ITEM_UNKNOWN_ERROR; 
	}
	
	return status; 
}

//----------------------------------------------------------------------------
ItemStatus vtkVideoBuffer::GetVideoBufferItemFromTime( const double time, VideoBufferItem* bufferItem)
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


