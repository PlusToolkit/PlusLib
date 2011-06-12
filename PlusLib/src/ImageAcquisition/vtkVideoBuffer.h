/*=========================================================================

Program:   Visualization Toolkit
Module:    $RCSfile: vtkVideoBuffer.h,v $

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

Author: Danielle Pace
Robarts Research Institute and The University of Western Ontario

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information. 

=========================================================================*/
// .NAME vtkVideoBuffer - Store a collection of video frames
// .SECTION Description
// vtkVideoBuffer is a structure for holding video frames that are
// captured either from a vtkRenderWindow or from some other video
// source.  The buffer can be locked, to allow some video frames to
// be written to while other frames are being read from.  Hopefully
// an additional class will be written that will take a vtkVideoBuffer
// and compress it into a movie file.
// .SECTION See Also
// vtkVideoFrame2 vtkVideoSource2 vtkWin32VideoSource2 vtkMILVideoSource2

#ifndef __vtkVideoBuffer_h
#define __vtkVideoBuffer_h

#include "vtkObject.h"

#include "vtkObjectFactory.h"
#include "vtkTimestampedCircularBuffer.h"

class vtkVideoBufferObject; 
class vtkVideoFrame2;
class vtkImageData; 
class TimestampedBufferItem; 

class VTK_EXPORT VideoBufferItem : public TimestampedBufferItem
{
public:
	VideoBufferItem(); 
	~VideoBufferItem(); 
	VideoBufferItem(const VideoBufferItem& videoBufferItem); 

	// Copy video buffer item 
	PlusStatus DeepCopy(VideoBufferItem* videoBufferItem); 

	// Set video frame format 
	PlusStatus SetFrameFormat(vtkVideoFrame2* frameFormat); 

	// Set/get video frame 
	PlusStatus SetFrame(vtkVideoFrame2* frame); 
	PlusStatus SetFrame(vtkImageData* frame); 
	PlusStatus SetFrame(unsigned char *imageDataPtr, 
		const int frameSizeInPx[3],
		const int numberOfBitsPerPixel, 
		const int	numberOfBytesToSkip ); 

	vtkVideoFrame2* GetFrame() const { return this->Frame; }

protected:
	vtkVideoFrame2 *Frame;
}; 


class VTK_EXPORT vtkVideoBuffer : public vtkObject
{
public:	
	
	static vtkVideoBuffer *New();
	vtkTypeRevisionMacro(vtkVideoBuffer,vtkObject);
	void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Set/Get the size of the buffer, i.e. the maximum number of
	// video frames that it will hold.  The default is 30.
	virtual void SetBufferSize(int n);
	virtual int GetBufferSize(); 

	// Description:
	// Set/Get the format of the video frames in the buffer (the get method
	// returns a frame that contains no data, just the format 
	// description).  If the buffer is not empty, changing the frame format has the
	// side-effect of deleting the frames within the buffer.
	virtual void SetFrameFormat(vtkVideoFrame2 *);
	vtkVideoFrame2 *GetFrameFormat() { return this->FrameFormat; };

	// Description:
	// Update video buffer 
	virtual void UpdateBuffer(); 
	
	// Description:
	// Add a frame plus a timestamp to the buffer with frame index.  If the timestamp is
	// less than or equal to the previous timestamp, or if the frame's format
	// doesn't match the buffer's frame format, then nothing will be done.
	virtual PlusStatus AddItem(vtkImageData* frame, const double unfilteredTimestamp, const double filteredTimestamp, const long frameNumber); 
	virtual PlusStatus AddItem(unsigned char *imageDataPtr, 
		const int frameSizeInPx[3],
		const int numberOfBitsPerPixel, 
		const int numberOfBytesToSkip, 
		const double unfilteredTimestamp, 
		const double filteredTimestamp, 
		const long frameNumber);

	virtual ItemStatus GetVideoBufferItem(const BufferItemUidType uid, VideoBufferItem* bufferItem);
	virtual ItemStatus GetLatestVideoBufferItem(VideoBufferItem* bufferItem) { return this->GetVideoBufferItem( this->GetLatestItemUidInBuffer(), bufferItem); }; 
	virtual ItemStatus GetOldestVideoBufferItem(VideoBufferItem* bufferItem) { return this->GetVideoBufferItem( this->GetOldestItemUidInBuffer(), bufferItem); }; 
	virtual ItemStatus GetVideoBufferItemFromTime( const double time, VideoBufferItem* bufferItem); 


	virtual BufferItemUidType GetOldestItemUidInBuffer() { return this->VideoBuffer->GetOldestItemUidInBuffer(); }
	virtual BufferItemUidType GetLatestItemUidInBuffer() { return this->VideoBuffer->GetLatestItemUidInBuffer(); }
	virtual ItemStatus GetItemUidFromTime(const double time, BufferItemUidType& uid) { return this->VideoBuffer->GetItemUidFromTime(time, uid); }

	virtual void SetLocalTimeOffset(double offset);
	virtual double GetLocalTimeOffset();

	virtual int GetNumberOfItems() { return this->VideoBuffer->GetNumberOfItems(); }

	virtual double GetFrameRate( bool ideal = false) { return this->VideoBuffer->GetFrameRate(ideal); }

	// Description:
	// Make this buffer into a copy of another buffer.  You should
	// Lock both of the buffers before doing this.
	virtual void DeepCopy(vtkVideoBuffer* buffer); 

	// Description:
	// Clear buffer (set the buffer pointer to the first element)
	virtual void Clear(); 


/*
	// Description:
	// Get frame from the buffer by frame UID
	// If ItemStatus is NOT ITEM_OK, vtkVideoFrame2*  will be NULL
	virtual ItemStatus GetFrame(const BufferItemUidType uid, vtkVideoFrame2*& frame); 
	virtual ItemStatus UpdateImageData(const BufferItemUidType uid, vtkTrackedFrame *frame);
*/

	

protected:
	vtkVideoBuffer();
	~vtkVideoBuffer();

	// Description:
	// Compares frame format with new frame imaging parameters
	// Returns true if it matches, otherwise false
	virtual bool CheckFrameFormat( const int frameSizeInPx[3], const int numberOfBitsPerPixel ); 
	virtual bool CheckFrameFormat( vtkVideoFrame2* frame ); 

	// holds the formatting information for the buffer's frames, but no data.
	vtkVideoFrame2 *FrameFormat;
	vtkTimestampedCircularBuffer<VideoBufferItem>* VideoBuffer; 

private:
	vtkVideoBuffer(const vtkVideoBuffer&);
	void operator=(const vtkVideoBuffer&);
};




#endif
