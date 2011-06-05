/*=========================================================================

Program:   Visualization Toolkit
Module:    $RCSfile: vtkVideoBuffer2.h,v $

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

Author: Danielle Pace
Robarts Research Institute and The University of Western Ontario

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information. 

=========================================================================*/
// .NAME vtkVideoBuffer2 - Store a collection of video frames
// .SECTION Description
// vtkVideoBuffer2 is a structure for holding video frames that are
// captured either from a vtkRenderWindow or from some other video
// source.  The buffer can be locked, to allow some video frames to
// be written to while other frames are being read from.  Hopefully
// an additional class will be written that will take a vtkVideoBuffer2
// and compress it into a movie file.
// .SECTION See Also
// vtkVideoFrame2 vtkVideoSource2 vtkWin32VideoSource2 vtkMILVideoSource2

#ifndef __vtkVideoBuffer2_h
#define __vtkVideoBuffer2_h

#include "vtkObject.h"

class vtkCriticalSection;
class vtkVideoFrame2;
class vtkDoubleArray;
class vtkUnsignedLongLongArray; 

class VTK_EXPORT vtkVideoBuffer2 : public vtkObject
{
public:
	typedef unsigned __int64 FrameUidType;
	enum FrameStatus { FRAME_OK, FRAME_NOT_AVAILABLE_YET, FRAME_NOT_AVAILABLE_ANYMORE };

	static vtkVideoBuffer2 *New();
	vtkTypeRevisionMacro(vtkVideoBuffer2,vtkObject);
	void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Set/Get the size of the buffer, i.e. the maximum number of
	// video frames that it will hold.  The default is 30.
	virtual void SetBufferSize(int n);
	vtkGetMacro(BufferSize, int);

	// Description:
	// Set/Get the number of items in the list (this is not the same as
	// the buffer size, but is rather the number of transforms that
	// have been added to the list).  This will never be greater than
	// the BufferSize.
	vtkGetMacro(NumberOfItems, int);

	// Description:
	// Set/Get the format of the video frames in the buffer (the get method
	// returns a frame that contains no data, just the format 
	// description).  If the buffer is not empty, changing the frame format has the
	// side-effect of deleting the frames within the buffer.
	virtual void SetFrameFormat(vtkVideoFrame2 *);
	vtkVideoFrame2 *GetFrameFormat() { return this->FrameFormat; };

	// Description:
	// Lock/Unlock the buffer: this should be done before changing or accessing
	// the data in the buffer if the buffer is being used from multiple
	// threads.  
	virtual void Lock();
	virtual void Unlock();

	// Description:
	// Add a frame plus a timestamp to the buffer with frame index.  If the timestamp is
	// less than or equal to the previous timestamp, or if the frame's format
	// doesn't match the buffer's frame format, then nothing will be done.
	virtual void AddItem(vtkVideoFrame2* frame, double unfilteredTimestamp, double filteredTimestamp, int frameNumber);

	// Description:
	// Get frame from the buffer by frame UID
	// If FrameStatus is NOT FRAME_OK, vtkVideoFrame2*  will be NULL
	virtual FrameStatus GetFrame(const FrameUidType uid, vtkVideoFrame2*& frame); 

	// Description:
	// Get most recent frame from the frame buffer
	virtual FrameStatus GetLatestFrame(vtkVideoFrame2* latestFrame); 

		// Description:
	// Get a frame from the buffer by buffer index, starting from 0 to (BufferSize - 1)
	// This function is mainly needed for frame buffer initialization and internal access of buffer. 
	virtual vtkVideoFrame2 *GetFrameByBufferIndex(const int bufferIndex);

	// Description:
	// Get next writable frame from the buffer
	virtual vtkVideoFrame2 *GetFrameToWrite(); 

	// Description:
	// Given a timestamp, compute the nearest frame UID
	// This assumes that the times motonically increase
	virtual FrameStatus GetFrameUidFromTime(const double time, FrameUidType& uid );

	// Description:
	// Get frame UID from buffer index
	virtual FrameStatus GetFrameUidFromBufferIndex(const int bufferIndex, FrameUidType &uid ); 

	// Description:
	// Get the most recent frame UID 
	virtual FrameUidType GetLatestFrameUidInBuffer() 
	{ 
		return this->FrameUID; 
	}

	// Description:
	// Get the oldest frame UID in the buffer 
	virtual FrameUidType GetOldestFrameUidInBuffer() 
	{ 
		// LatestFrameUid - ( NumberOfItems - 1 ) is the oldest element in the buffer, 
		// but that one is the WritePointer location, the oldest readable frame is the subsequent
		return ( this->FrameUID - ( this->NumberOfItems - 1 ) + 1 ); 
	} 
	
	// Description:
	// Get frame status by frame UID 
	virtual FrameStatus GetFrameStatus(const FrameUidType uid ); 

	// Description:
	// Get timestamp by frame UID associated with the video frame data in the buffer
	virtual FrameStatus GetLatestTimeStamp(double &timestamp) { return this->GetTimeStamp(this->FrameUID, timestamp); } ; 
	virtual FrameStatus GetTimeStamp(const FrameUidType uid, double &timestamp) { return this->GetFilteredTimeStamp(uid, timestamp); }
	virtual FrameStatus GetFilteredTimeStamp(const FrameUidType uid, double &filteredTimestamp); 
	virtual FrameStatus GetUnfilteredTimeStamp(const FrameUidType uid, double &unfilteredTimestamp); 

	// Description:
	// Get a frame number from the buffer by frame UID. 
	virtual FrameStatus GetFrameNumber(const FrameUidType uid, unsigned long &frameNumber); 

	// Description:
	// Get buffer index by frame UID 
	// Returns buffer index if the FrameStatus is FRAME_OK, otherwise -1; 
	virtual int GetBufferIndex( FrameUidType uid ); 

	// Description:
	// Given a timestamp, compute the nearest buffer index 
	// This assumes that the times motonically increase
	virtual FrameStatus GetBufferIndexFromTime(const double time, int& bufferIndex );

	// Description:
	// Make this buffer into a copy of another buffer.  You should
	// Lock both of the buffers before doing this.
	virtual void DeepCopy(vtkVideoBuffer2* buffer); 

	// Description:	
	// Get/Set the local time offset (global = local + offset)
	vtkSetMacro(LocalTimeOffset, double); 
	vtkGetMacro(LocalTimeOffset, double);

	// Description:
	// Get the frame rate from the buffer based on the number of frames in the buffer
	// and the elapsed time.
	// Ideal frame rate shows the mean of the frame periods in the buffer based on the frame 
	// number difference (aka the device frame rate)
	virtual double GetFrameRate(bool ideal = false);

	// Description:
	// Clear buffer (set the buffer pointer to the first element)
	virtual void Clear(); 

protected:
	vtkVideoBuffer2();
	~vtkVideoBuffer2();

	vtkVideoFrame2 **FrameArray;
	vtkDoubleArray *FilteredTimeStampArray;
	vtkDoubleArray *UnfilteredTimeStampArray;
	vtkUnsignedLongLongArray *FrameNumberArray; 
	vtkUnsignedLongLongArray *FrameUIDArray; 

	// holds the formatting information for the buffer's frames, but no data.
	vtkVideoFrame2 *FrameFormat;

	vtkCriticalSection *Mutex;

	int BufferSize;
	int NumberOfItems;
	// Next image will be written here
	int WritePointer;
	double CurrentTimeStamp;
	double LocalTimeOffset; 
	
	FrameUidType FrameUID; 

private:
	vtkVideoBuffer2(const vtkVideoBuffer2&);
	void operator=(const vtkVideoBuffer2&);
};

#endif
