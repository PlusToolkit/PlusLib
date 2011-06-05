/*=========================================================================

Program:   Visualization Toolkit
Module:    $RCSfile: vtkVideoBuffer2.cxx,v $

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

Author: Danielle Pace
Robarts Research Institute and The University of Western Ontario

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information. 

=========================================================================*/
#include "PlusConfigure.h"
#include "vtkVideoBuffer2.h"
#include "vtkObjectFactory.h"
#include "vtkVideoFrame2.h"
#include "vtkDoubleArray.h"
#include "vtkUnsignedLongLongArray.h"
#include "vtkCriticalSection.h"

vtkCxxRevisionMacro(vtkVideoBuffer2, "$Revision: 1.4 $");
vtkStandardNewMacro(vtkVideoBuffer2);

//----------------------------------------------------------------------------
vtkVideoBuffer2::vtkVideoBuffer2()
{
	this->FrameArray = 0;
	this->FilteredTimeStampArray = vtkDoubleArray::New();
	this->UnfilteredTimeStampArray = vtkDoubleArray::New();
	this->FrameNumberArray = vtkUnsignedLongLongArray::New(); 
	this->FrameUIDArray = vtkUnsignedLongLongArray::New(); 
	this->FrameFormat = vtkVideoFrame2::New();
	this->Mutex = vtkCriticalSection::New();
	this->BufferSize = 0;
	this->NumberOfItems = 0;
	this->WritePointer = 0;
	this->CurrentTimeStamp = 0.0;
	this->LocalTimeOffset = 0.0; 
	this->FrameUID = 0; 

	// serves to instantiate the frame array and the time stamp array
	//this->SetBufferSize(30);
}

//----------------------------------------------------------------------------
vtkVideoBuffer2::~vtkVideoBuffer2()
{ 
	this->SetBufferSize(0);
	this->NumberOfItems = 0;

	if (this->FrameArray)
	{
		delete [] this->FrameArray;
		this->FrameArray = NULL; 
	}

	if (this->FilteredTimeStampArray)
	{
		this->FilteredTimeStampArray->Delete();
		this->FilteredTimeStampArray = NULL; 
	}

	if (this->UnfilteredTimeStampArray)
	{
		this->UnfilteredTimeStampArray->Delete();
		this->UnfilteredTimeStampArray = NULL; 
	}

	if ( this->FrameNumberArray )
	{
		this->FrameNumberArray->Delete(); 
		this->FrameNumberArray = NULL; 
	}

	if ( this->FrameUIDArray )
	{
		this->FrameUIDArray->Delete(); 
		this->FrameUIDArray = NULL; 
	}


	this->FrameFormat->Delete();
	this->Mutex->Delete();
}

//----------------------------------------------------------------------------
void vtkVideoBuffer2::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);

	os << indent << "BufferSize: " << this->BufferSize << "\n";
	os << indent << "NumberOfItems: " << this->NumberOfItems << "\n";
	os << indent << "CurrentTimeStamp: " << this->CurrentTimeStamp << "\n";
	os << indent << "Local time offset: " << this->LocalTimeOffset << "\n";
	if (this->FrameFormat)
	{
		os << indent << "FrameFormat:\n";
		this->FrameFormat->PrintSelf(os, indent.GetNextIndent());
	}
	if (this->FilteredTimeStampArray)
	{
		os << indent << "FilteredTimeStampArray:\n";
		this->FilteredTimeStampArray->PrintSelf(os, indent.GetNextIndent());
	}

	if (this->UnfilteredTimeStampArray)
	{
		os << indent << "UnfilteredTimeStampArray:\n";
		this->UnfilteredTimeStampArray->PrintSelf(os, indent.GetNextIndent());
	}

	if ( this->FrameNumberArray)
	{
		os << indent << "FrameNumberArray:\n";
		this->FrameNumberArray->PrintSelf(os, indent.GetNextIndent());
	}

	if ( this->FrameUIDArray ) 
	{
		os << indent << "FrameUIDArray:\n"; 
		this->FrameUIDArray->PrintSelf(os, indent.GetNextIndent()); 
	}

}

//----------------------------------------------------------------------------
void vtkVideoBuffer2::Lock()
{
	this->Mutex->Lock();
}

//----------------------------------------------------------------------------
void vtkVideoBuffer2::Unlock()
{
	this->Mutex->Unlock();
}

//----------------------------------------------------------------------------
// Sets the buffer size, and copies the maximum number of the most current old
// frames and timestamps
void vtkVideoBuffer2::SetBufferSize(int bufsize)
{
	int i;
	vtkVideoFrame2 **framearray;
	vtkDoubleArray* filteredtimestamps;
	vtkDoubleArray* unfilteredtimestamps;
	vtkUnsignedLongLongArray* framenumberarray; 
	vtkUnsignedLongLongArray* frameuidarray; 

	if (bufsize < 0)
	{
		vtkErrorMacro("SetBufferSize: invalid buffer size");
		return;
	}

	if (bufsize == this->BufferSize && bufsize != 0)
	{
		return;
	}

	// if we don't have a frame array, we'll make one
	if (this->FrameArray == 0)
	{
		if (bufsize > 0)
		{
			this->WritePointer = 0;
			this->NumberOfItems = 0;
			this->CurrentTimeStamp = 0.0;
			this->FrameArray = new vtkVideoFrame2 *[bufsize];
			this->BufferSize = bufsize;
			this->FilteredTimeStampArray->SetNumberOfValues(this->BufferSize);
			this->UnfilteredTimeStampArray->SetNumberOfValues(this->BufferSize);
			this->FrameNumberArray->SetNumberOfValues(this->BufferSize); 
			this->FrameUIDArray->SetNumberOfValues(this->BufferSize); 
			for (i = 0; i < bufsize; i++)
			{
				this->FrameArray[i] = this->FrameFormat->MakeObject();
				this->FilteredTimeStampArray->SetValue(i, 0.0);
				this->UnfilteredTimeStampArray->SetValue(i, 0.0);
				this->FrameNumberArray->SetValue(i, 0);
				this->FrameUIDArray->SetValue(i, 0); 
			} 
			this->Modified();
		}
	}
	// if we already have a frame array and are changing its buffer size
	else 
	{
		if (bufsize > 0)
		{
			framearray = new vtkVideoFrame2 *[bufsize];
			filteredtimestamps = vtkDoubleArray::New();
			filteredtimestamps->SetNumberOfValues(bufsize);
			unfilteredtimestamps = vtkDoubleArray::New();
			unfilteredtimestamps->SetNumberOfValues(bufsize);
			framenumberarray = vtkUnsignedLongLongArray::New(); 
			framenumberarray->SetNumberOfValues(bufsize); 
			frameuidarray = vtkUnsignedLongLongArray::New(); 
			frameuidarray->SetNumberOfValues(bufsize); 
		}
		else
		{
			framearray = NULL;
			filteredtimestamps = NULL;
			unfilteredtimestamps = NULL;
			framenumberarray = NULL; 
			frameuidarray = NULL; 
			this->NumberOfItems = 0;
			this->CurrentTimeStamp = 0.0;
		}

		int index = this->WritePointer;
		if (index < 0)
		{
			// because '%' can give negative results on some platforms
			index = index + this->BufferSize;
		}

		// if the new buffer is smaller than the old buffer
		if (this->BufferSize > bufsize)
		{
			// copy the most recent frames and timestamps
			for (i = 0; i < bufsize; i++)
			{
				framearray[i] = this->FrameArray[index];
				filteredtimestamps->SetValue(i, this->FilteredTimeStampArray->GetValue(index));
				unfilteredtimestamps->SetValue(i, this->UnfilteredTimeStampArray->GetValue(index));
				framenumberarray->SetValue(i, this->FrameNumberArray->GetValue(index)); 
				frameuidarray->SetValue(i, this->FrameUIDArray->GetValue(index)); 
				index = (index - 1) % this->BufferSize;
				if (index < 0)
				{
					// because '%' can give negative results on some platforms
					index = index + this->BufferSize;
				}
			}
			// delete the older frames and timestamps that won't fit in the new buffer
			for (i = 0; i < this->BufferSize - bufsize; i++)
			{
				this->FrameArray[index]->Delete();
				this->FilteredTimeStampArray->SetValue(index, 0.0);
				this->UnfilteredTimeStampArray->SetValue(index, 0.0);
				this->FrameNumberArray->SetValue(index, 0); 
				this->FrameUIDArray->SetValue(index, 0); 
				index = (index - 1) % this->BufferSize;
				if (index < 0)
				{
					// because '%' can give negative results on some platforms
					index = index + this->BufferSize;
				}
			}
			// set the current index
			this->WritePointer = 0;
		}
		// if the new buffer is bigger than the old buffer
		else if (bufsize > this->BufferSize)
		{
			// copy the old frames and timestamps
			for (i = 0; i < this->BufferSize; i++)
			{
				framearray[i] = this->FrameArray[i];
				filteredtimestamps->SetValue(i, this->FilteredTimeStampArray->GetValue(i));
				unfilteredtimestamps->SetValue(i, this->UnfilteredTimeStampArray->GetValue(i));
				framenumberarray->SetValue(i, this->FrameNumberArray->GetValue(i)); 
				frameuidarray->SetValue(i, this->FrameUIDArray->GetValue(i)); 
			}
			// create new frames and timestamps
			for (i = this->BufferSize; i < bufsize; i++)
			{
				framearray[i] = this->FrameFormat->MakeObject();
				filteredtimestamps->SetValue(i, 0.0);
				unfilteredtimestamps->SetValue(i, 0.0);
				framenumberarray->SetValue(i, 0.0); 
				frameuidarray->SetValue(i, 0); 
			}
		}

		// update the frame array and the time stamp array
		if (this->FrameArray)
		{
			delete [] this->FrameArray;
		}
		this->FrameArray = framearray;
		if (this->FilteredTimeStampArray)
		{
			this->FilteredTimeStampArray->Delete();
		}
		this->FilteredTimeStampArray = filteredtimestamps;

		if (this->UnfilteredTimeStampArray)
		{
			this->UnfilteredTimeStampArray->Delete();
		}
		this->UnfilteredTimeStampArray = unfilteredtimestamps;

		if ( this->FrameNumberArray )
		{
			this->FrameNumberArray->Delete(); 
		}
		this->FrameNumberArray = framenumberarray; 

		if ( this->FrameUIDArray )
		{
			this->FrameUIDArray->Delete(); 
		}
		this->FrameUIDArray = frameuidarray; 

		// update the buffer size and the number of items
		this->BufferSize = bufsize;
		if (this->NumberOfItems > this->BufferSize)
		{
			this->NumberOfItems = this->BufferSize;
		}

		this->Modified();
	}
}

//----------------------------------------------------------------------------
// Sets the frame format.  If the format is different from the old format,
// deletes all of the old frames and fills the buffer with new frames of the
// new format
void vtkVideoBuffer2::SetFrameFormat(vtkVideoFrame2 *format)
{
	// if the new format matches the old format, we don't need to do anything
	if (format)
	{
		int frameSize[3];
		int frameExtent[6];
		int frameFormatSize[3];
		int frameFormatExtent[6];
		format->GetFrameSize(frameSize);
		format->GetFrameExtent(frameExtent);
		this->FrameFormat->GetFrameSize(frameFormatSize);
		this->FrameFormat->GetFrameExtent(frameFormatExtent);

		if (frameSize[0] == frameFormatSize[0] &&
			frameSize[1] == frameFormatSize[1] &&
			frameSize[2] == frameFormatSize[2] &&
			frameExtent[0] == frameFormatExtent[0] &&
			frameExtent[1] == frameFormatExtent[1] &&
			frameExtent[2] == frameFormatExtent[2] &&
			frameExtent[3] == frameFormatExtent[3] &&
			frameExtent[4] == frameFormatExtent[4] &&
			frameExtent[5] == frameFormatExtent[5] &&
			format->GetPixelFormat() == this->FrameFormat->GetPixelFormat() &&
			format->GetBitsPerPixel() == this->FrameFormat->GetBitsPerPixel() &&
			format->GetRowAlignment() == this->FrameFormat->GetRowAlignment() &&
			format->GetTopDown() == this->FrameFormat->GetTopDown() &&
			format->GetBytesInFrame() == this->FrameFormat->GetBytesInFrame() &&
			format->GetOpacity() == this->FrameFormat->GetOpacity() &&
			format->GetCompression() == this->FrameFormat->GetCompression() &&
			format->GetFrameGrabberType() == this->FrameFormat->GetFrameGrabberType() )
		{
			return;
		}
	}

	// set the new frame format
	if (this->FrameFormat)
	{
		this->FrameFormat->Delete();
	}
	if (format)
	{
		format->Register(this);
		this->FrameFormat = format;
	}
	else
	{
		this->FrameFormat = vtkVideoFrame2::New();
	}

	this->Modified();

	// replace all the frames with ones in the new format
	for (int i = 0; i < this->BufferSize; i++)
	{
		this->FrameArray[i]->Delete();
		this->FrameArray[i] = this->FrameFormat->MakeObject();
		this->FilteredTimeStampArray->SetValue(i, 0.0);
		this->UnfilteredTimeStampArray->SetValue(i, 0.0);
		this->FrameNumberArray->SetValue(i, 0); 
		this->FrameUIDArray->SetValue(i, 0); 
	}

	this->WritePointer = 0;
	this->NumberOfItems = 0;
	this->CurrentTimeStamp = 0.0;
}

//----------------------------------------------------------------------------
void vtkVideoBuffer2::AddItem(vtkVideoFrame2* frame, double unfilteredTimestamp, double filteredTimestamp, int frameNumber)
{
	if (filteredTimestamp <= this->CurrentTimeStamp)
	{
		return;
	}

	// don't add a frame if it doesn't match the buffer frame format
	int frameSize[3];
	int frameExtent[6];
	int frameFormatSize[3];
	int frameFormatExtent[6];
	frame->GetFrameSize(frameSize);
	frame->GetFrameExtent(frameExtent);
	this->FrameFormat->GetFrameSize(frameFormatSize);
	this->FrameFormat->GetFrameExtent(frameFormatExtent);
	if (frameSize[0] != frameFormatSize[0] ||
		frameSize[1] != frameFormatSize[1] ||
		frameSize[2] != frameFormatSize[2] ||
		frameExtent[0] != frameFormatExtent[0] ||
		frameExtent[1] != frameFormatExtent[1] ||
		frameExtent[2] != frameFormatExtent[2] ||
		frameExtent[3] != frameFormatExtent[3] ||
		frameExtent[4] != frameFormatExtent[4] ||
		frameExtent[5] != frameFormatExtent[5] ||
		frame->GetPixelFormat() != this->FrameFormat->GetPixelFormat() ||
		frame->GetBitsPerPixel() != this->FrameFormat->GetBitsPerPixel() ||
		frame->GetRowAlignment() != this->FrameFormat->GetRowAlignment() ||
		frame->GetTopDown() != this->FrameFormat->GetTopDown() ||
		frame->GetOpacity() != this->FrameFormat->GetOpacity() ||
		frame->GetCompression() != this->FrameFormat->GetCompression() ||
		frame->GetFrameGrabberType() != this->FrameFormat->GetFrameGrabberType() )
	{
		return;
	}

	
	// Increase frame unique ID
	this->FrameUID++; 
	
	// add the frame and timestamp, and update buffer
	this->CurrentTimeStamp = filteredTimestamp;
	this->FrameArray[this->WritePointer] = frame;
	this->FilteredTimeStampArray->SetValue(this->WritePointer, filteredTimestamp);
	this->UnfilteredTimeStampArray->SetValue(this->WritePointer, unfilteredTimestamp);
	this->FrameNumberArray->SetValue(this->WritePointer, frameNumber); 
	this->FrameUIDArray->SetValue(this->WritePointer, this->FrameUID); 
	this->NumberOfItems++;
	if (this->NumberOfItems > this->BufferSize)
	{
		this->NumberOfItems = this->BufferSize;
	}

	// Increase the write pointer
	if ( ++this->WritePointer >= this->BufferSize )
	{
		this->WritePointer = 0; 
	}

	/*LOG_TRACE("*** New frame at: " << std::fixed << filteredTimestamp); */

	this->Modified();
}

//----------------------------------------------------------------------------
vtkVideoBuffer2::FrameStatus vtkVideoBuffer2::GetFrameStatus(const FrameUidType uid )
{
	if ( uid < this->GetOldestFrameUidInBuffer() ) 
	{
		return FRAME_NOT_AVAILABLE_ANYMORE; 
	}
	else if ( uid > this->GetLatestFrameUidInBuffer() )
	{
		return FRAME_NOT_AVAILABLE_YET; 
	}
	else
	{
		return FRAME_OK; 
	}
}

//----------------------------------------------------------------------------
vtkVideoBuffer2::FrameStatus vtkVideoBuffer2::GetFrameUidFromBufferIndex( const int bufferIndex, FrameUidType &uid )
{
	if (this->BufferSize <= 0 
		|| bufferIndex >= this->GetBufferSize() 
		|| bufferIndex < 0 )
	{
		LOG_ERROR("vtkVideoBuffer2: Unable to get frame UID from buffer index! Buffer index is not valid (bufferIndex=" 
			<< bufferIndex << ", bufferSize=" << this->GetBufferSize() << ")." ); 
		uid = 0; 
	}
	else
	{
		uid = this->FrameUIDArray->GetValue(bufferIndex); 
	}

	return this->GetFrameStatus(uid); 
}

//----------------------------------------------------------------------------
int vtkVideoBuffer2::GetBufferIndex( const FrameUidType uid )
{
	if ( this->GetFrameStatus( uid ) != FRAME_OK )
	{
		return -1; 
	}
	
	int readIndex = (this->WritePointer - 1) % this->BufferSize;

	if ( readIndex < 0 ) 
	{
		readIndex += this->BufferSize;
	}
	
	const int bufferIndex = readIndex - (this->FrameUID - uid); 
	
	return bufferIndex; 
}

//----------------------------------------------------------------------------
vtkVideoFrame2* vtkVideoBuffer2::GetFrameByBufferIndex(const int bufferIndex)
{
	if (this->BufferSize <= 0 
		|| bufferIndex >= this->GetBufferSize() 
		|| bufferIndex < 0 )
	{
		return NULL;
	}
	
	return this->FrameArray[bufferIndex];
}

//----------------------------------------------------------------------------
vtkVideoBuffer2::FrameStatus vtkVideoBuffer2::GetLatestFrame(vtkVideoFrame2* latestFrame)
{
	return this->GetFrame( this->FrameUID, latestFrame ); 
}

//----------------------------------------------------------------------------
vtkVideoBuffer2::FrameStatus vtkVideoBuffer2::GetFrame(const FrameUidType uid, vtkVideoFrame2*& frame)
{
	FrameStatus status = this->GetFrameStatus( uid ); 
	if ( status != FRAME_OK )
	{
		frame = NULL; 
		return status; 
	}

	const int bufferIndex = this->GetBufferIndex( uid ); 
	frame = this->GetFrameByBufferIndex(bufferIndex); 

	return status; 
}

//----------------------------------------------------------------------------
vtkVideoFrame2* vtkVideoBuffer2::GetFrameToWrite() 
{ 
	if (this->BufferSize <= 0 
		|| this->WritePointer >= this->GetBufferSize() 
		|| this->WritePointer < 0 )
	{
		return NULL;
	}

	return this->FrameArray[this->WritePointer]; 
}

//----------------------------------------------------------------------------
vtkVideoBuffer2::FrameStatus vtkVideoBuffer2::GetFilteredTimeStamp(const FrameUidType uid, double &filteredTimestamp)
{ 
	FrameStatus status = this->GetFrameStatus( uid ); 
	if ( status != FRAME_OK )
	{
		filteredTimestamp = 0.0; 
		return status; 
	}

	const int bufferIndex = this->GetBufferIndex( uid ); 
	filteredTimestamp = this->FilteredTimeStampArray->GetValue(bufferIndex) + this->LocalTimeOffset; 
	return status;
}

//----------------------------------------------------------------------------
vtkVideoBuffer2::FrameStatus vtkVideoBuffer2::GetUnfilteredTimeStamp(const FrameUidType uid, double &unfilteredTimestamp)
{ 
	FrameStatus status = this->GetFrameStatus( uid ); 
	if ( status != FRAME_OK )
	{
		unfilteredTimestamp = 0.0; 
		return status; 
	}

	const int bufferIndex = this->GetBufferIndex( uid ); 
	unfilteredTimestamp = this->UnfilteredTimeStampArray->GetValue(bufferIndex) + this->LocalTimeOffset; 
	return status;
}


//----------------------------------------------------------------------------
vtkVideoBuffer2::FrameStatus vtkVideoBuffer2::GetFrameNumber(const FrameUidType uid, unsigned long &frameNumber)
{ 
	FrameStatus status = this->GetFrameStatus( uid ); 
	if ( status != FRAME_OK )
	{
		frameNumber = 0; 
		return status; 
	}

	const int bufferIndex = this->GetBufferIndex( uid ); 
	frameNumber = this->FrameNumberArray->GetValue(bufferIndex); 
	return status;
}


//----------------------------------------------------------------------------
vtkVideoBuffer2::FrameStatus vtkVideoBuffer2::GetBufferIndexFromTime(const double time, int& bufferIndex )
{
	FrameUidType frameUid(0); 
	FrameStatus frameStatus = this->GetFrameUidFromTime(time, frameUid); 

	if ( frameStatus != FRAME_OK )
	{
		return frameStatus; 
	}
	
	bufferIndex = this->GetBufferIndex(frameUid); 
	return frameStatus; 
}


//----------------------------------------------------------------------------
// do a simple divide-and-conquer search for the transform
// that best matches the given timestamp
vtkVideoBuffer2::FrameStatus vtkVideoBuffer2::GetFrameUidFromTime(const double time, FrameUidType& uid )
{
	FrameUidType lo = this->GetOldestFrameUidInBuffer();
	FrameUidType hi = this->GetLatestFrameUidInBuffer();

	double tlo(0); 
	// minimum time
	FrameStatus loStatus = this->GetTimeStamp(lo, tlo); 
	if ( loStatus != FRAME_OK )
	{
		LOG_WARNING("vtkVideoBuffer2: Unable to get lo timestamp for frame UID: " << lo ); 
		return loStatus; 
	}

	double thi(0); 
	// maximum time
	FrameStatus hiStatus = this->GetTimeStamp(hi, thi); 
	if ( hiStatus != FRAME_OK )
	{
		LOG_WARNING("vtkVideoBuffer2: Unable to get hi timestamp for frame UID: " << hi ); 
		return hiStatus; 
	} 

	if (time < tlo)
	{
		return FRAME_NOT_AVAILABLE_ANYMORE; 
	}
	else if (time > thi)
	{
		return FRAME_NOT_AVAILABLE_YET;
	}

	for (;;)
	{
		if (hi-lo == 1)
		{
			if (time - tlo > thi - time)
			{
				uid = hi; 
				return FRAME_OK;
			}
			else
			{
				uid = lo; 
				return FRAME_OK;
			}
		}

		int mid = (lo+hi)/2;
		double tmid(0);
		FrameStatus midStatus = this->GetTimeStamp(mid, tmid); 
		if ( midStatus != FRAME_OK )
		{
			LOG_WARNING("vtkVideoBuffer2: Unable to get mid timestamp for frame UID: " << tmid ); 
			return midStatus; 
		}

		if (time < tmid)
		{
			hi = mid;
			thi = tmid;
		}
		else
		{
			lo = mid;
			tlo = tmid;
		}
	}
}

//----------------------------------------------------------------------------
void vtkVideoBuffer2::DeepCopy(vtkVideoBuffer2* buffer)
{
	this->SetFrameFormat(buffer->GetFrameFormat()); 
	this->SetBufferSize(buffer->GetBufferSize()); 

	this->WritePointer = buffer->WritePointer;
	this->NumberOfItems = buffer->NumberOfItems;
	this->CurrentTimeStamp = buffer->CurrentTimeStamp;
	this->LocalTimeOffset = buffer->LocalTimeOffset;
	this->FrameUID = buffer->FrameUID; 

	for ( int i = 0; i < this->BufferSize; i++ )
	{
		int * extent = this->GetFrameByBufferIndex(i)->GetFrameExtent(); 
		this->GetFrameByBufferIndex(i)->Allocate(); 
		buffer->GetFrameByBufferIndex(i)->CopyData(this->GetFrameByBufferIndex(i)->GetVoidPointer(0), extent, extent, this->GetFrameByBufferIndex(i)->GetPixelFormat()) ; 
		this->FilteredTimeStampArray->SetValue(i, buffer->FilteredTimeStampArray->GetValue(i));
		this->UnfilteredTimeStampArray->SetValue(i, buffer->UnfilteredTimeStampArray->GetValue(i));
		this->FrameNumberArray->SetValue(i, buffer->FrameNumberArray->GetValue(i)); 
		this->FrameUIDArray->SetValue(i, buffer->FrameUIDArray->GetValue(i)); 
	}
}

//----------------------------------------------------------------------------
void vtkVideoBuffer2::Clear()
{
	this->WritePointer = 0; 
	this->NumberOfItems = 0; 
	this->CurrentTimeStamp = 0; 

}

//----------------------------------------------------------------------------
double vtkVideoBuffer2::GetFrameRate(bool ideal /*=false*/)
{
	std::vector<double> framePeriods; 
	for ( FrameUidType frame = this->GetLatestFrameUidInBuffer(); frame > this->GetOldestFrameUidInBuffer(); --frame )
	{
		double time(0); 
		if ( this->GetTimeStamp(frame, time) != FRAME_OK )
		{
			continue; 
		}

		unsigned long framenum(0); 
		if ( this->GetFrameNumber(frame, framenum) != FRAME_OK)
		{
			continue; 
		}
		
		double prevtime(0); 
		if ( this->GetTimeStamp(frame - 1, prevtime) != FRAME_OK )
		{
			continue; 
		}
		
		unsigned long prevframenum(0); 
		if ( this->GetFrameNumber(frame - 1, framenum) != FRAME_OK)
		{
			continue; 
		}

		int frameDiff = framenum - prevframenum; 
		double frameperiod = (time - prevtime); 
		if ( ideal )
		{
			frameperiod /= (1.0 * frameDiff);
		}

		if ( frameperiod > 0 )
		{
			framePeriods.push_back(frameperiod); 
		}
	}

	const int numberOfFramePeriods =  framePeriods.size(); 
	double samplingPeriod(0); 
	for ( int i = 0; i < numberOfFramePeriods; i++ )
	{
		samplingPeriod += framePeriods[i] / (1.0 * numberOfFramePeriods); 
	}

	double frameRate(0); 
	if ( samplingPeriod != 0 )
	{
		frameRate = 1.0/samplingPeriod;
	}

	return frameRate; 
}
