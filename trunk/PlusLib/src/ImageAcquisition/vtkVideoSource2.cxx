/*=========================================================================

Program:   Visualization Toolkit
Module:    $RCSfile: vtkVideoSource2.cxx,v $

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

#include "vtkVideoSource2.h"

#include "vtkCriticalSection.h"
#include "vtkDataArray.h"
#include "vtkImageData.h"
#include "vtkMultiThreader.h"
#include "vtkMutexLock.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTimerLog.h"
#include "vtkUnsignedCharArray.h"
#include "vtkWindows.h"
#include "vtkVideoBuffer2.h"
#include "vtkVideoFrame2.h"
#include "vtkDoubleArray.h"
#include "vtkBMPWriter.h"
#include "vtkJPEGWriter.h"
#include "vtkMetaImageWriter.h"
//#include "vtkMINCImageWriter.h"
#include "vtkPNGWriter.h"
#include "vtkPNMWriter.h"
#include "vtkTIFFWriter.h"
#include "vtkBMPReader.h"
#include "vtkJPEGReader.h"
#include "vtkMetaImageReader.h"
//#include "vtkMINCImageReader.h"
#include "vtkPNGReader.h"
#include "vtkPNMReader.h"
#include "vtkTIFFReader.h"
#include "vtkImageData.h"
#include "vtksys/SystemTools.hxx"
#include "vtkGnuplotExecuter.h"
#include "vtkHTMLGenerator.h"


#include <ctype.h>
#include <time.h>

//---------------------------------------------------------------
// Important Mutex rules:
// 
// The frame grabs are generally done asynchronously, and it is necessary
// to ensure that when the frame buffer is valid when it is being written 
// to or read from
//
// The following information can only be changed within a mutex lock,
// and the lock must not be released until the frame buffer agrees with the
// information.
//
// FrameArray
// TimeStampArray
// BufferSize
// FrameFormat->FrameSize
// FrameFormat->FrameExtent
// FrameFormat->BitsPerPixel
// FrameFormat->RowAlignment
// FrameFormat->TopDown
// FrameFormat->Opacity
//
// After one of the above has been changed, and before the mutex is released,
// the following must be called to update the frame buffer:
//
// UpdateFrameBuffer()
//
// Likewise, the following function must only be called from within a
// mutex lock because it modifies Buffer->CurrentIndex
//
// AdvanceFrameBuffer()
//
// Any methods which might be called asynchronously must lock the 
// mutex before reading the above information, and you must be very 
// careful when accessing any information except for the above.
// These methods include the following:
//
// InternalGrab()
//
// Finally, when Execute() is reading from the frame buffer it must do
// so from within a mutex lock.  Otherwise tearing artifacts might result.

vtkCxxRevisionMacro(vtkVideoSource2, "$Revision: 1.7 $");
vtkStandardNewMacro(vtkVideoSource2);

#if ( _MSC_VER >= 1300 ) // Visual studio .NET
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4312 )
#endif 

/*//----------------------------------------------------------------------------
// keep a list of all the existing vtkVideoSource2 objects, to ensure
// that proper deallocation occurs then the program exits.
static int vtkVideoSourcesLen = 0;
static vtkVideoSource2 **vtkVideoSources = NULL;
static int vtkVideoSourceExitFuncRegistered = 0;

// this function is called at exit to release the system resources of all
// of the video sources and to free memory
static void vtkVideoSourceExitFunc()
{
int i = 0;

for (i = 0; i < vtkVideoSourcesLen; i++)
{
vtkVideoSources[i]->ReleaseSystemResources();
}

free((void *)vtkVideoSources);
vtkVideoSources = 0;
vtkVideoSourcesLen = 0;
}

// this function is called to add a source to the list
static void vtkVideoSourceAdd(vtkVideoSource2 *o)
{
vtkVideoSource2 **newlist = 0;
int i = 0;

if (!vtkVideoSourceExitFuncRegistered)
{
vtkVideoSourceExitFuncRegistered = atexit(vtkVideoSourceExitFunc);
}

newlist = (vtkVideoSource2 **)malloc((vtkVideoSourcesLen+1)*sizeof(void *));
for (i = 0; i < vtkVideoSourcesLen; i++)
{
newlist[i] = vtkVideoSources[i];
}
newlist[vtkVideoSourcesLen++] = o;

if (vtkVideoSources)
{
free((void *)vtkVideoSources);
}
vtkVideoSources = newlist;
}

// this function is called to remove a source from the list 
static void vtkVideoSourceRemove(vtkVideoSource2 *o)
{
int i,j,n;

n = vtkVideoSourcesLen;
for (i = 0, j = 0; i < n; i++, j++)
{
vtkVideoSources[j] = vtkVideoSources[i];
if (vtkVideoSources[i] == o)
{
vtkVideoSourcesLen--;
j--;
}
}

if (vtkVideoSourcesLen == 0 && vtkVideoSources != 0)
{
free((void *)vtkVideoSources);
vtkVideoSources = 0;
}
}*/   

//----------------------------------------------------------------------------
vtkVideoSource2::vtkVideoSource2()
{
	int i;

	this->Initialized = 0;

	this->AutoAdvance = 1;

	this->Playing = 0;
	this->Recording = 0;

	this->FrameRate = 30;

	this->FrameCount = 0;
	this->FrameIndex = -1;
	this->FrameNumber = 0; 

	this->StartTimeStamp = 0;
	this->FrameTimeStamp = 0;

	this->LastTimeStamp = 0;
	this->LastUnfilteredTimeStamp = 0;
	this->LastFrameCount = 0;
	this->EstimatedFramePeriod = 0;
	this->MaximumFramePeriodJitter = 0.100; 
	this->SmoothingFactor = 0.001; 

	this->OutputNeedsInitialization = 1;

	this->NumberOfScalarComponents = 1;

	this->NumberOfOutputFrames = 1;

	for (i = 0; i < 3; i++)
	{
		this->ClipRegion[i*2] = 0;
		this->ClipRegion[i*2+1] = VTK_INT_MAX;
		this->OutputWholeExtent[i*2] = 0;
		this->OutputWholeExtent[i*2+1] = -1;
		this->DataSpacing[i] = 1.0;
		this->DataOrigin[i] = 0.0;
	}

	for (i = 0; i < 6; i++)
	{
		this->LastOutputExtent[i] = 0;
	}
	this->LastNumberOfScalarComponents = 0;

	this->PlayerThreader = vtkMultiThreader::New();
	this->PlayerThreadId = -1;

	this->Buffer = vtkVideoBuffer2::New();

	this->UpdateWithDesiredTimestamp = 0;
	this->DesiredTimestamp = -1;
	this->TimestampClosestToDesired = -1;

	// add ourselves to the global list
	//vtkVideoSourceAdd(this);

	this->SetNumberOfInputPorts(0);

#ifdef PLUS_PRINT_VIDEO_TIMESTAMP_DEBUG_INFO 
	this->DebugInfoStream.open ("VideoBufferTimestamps.txt", ios::out);
	this->DebugInfoStream << "FrameNumber\tFrameNumberDifference\tUnfilteredTimestamp\tFilteredTimestamp\tUnfilteredTimeDifference\tSamplingPeriod\tEstimatedFramePeriod" << std::endl; 
	this->DebugInfoStream.flush(); 
#endif
}

//----------------------------------------------------------------------------
vtkVideoSource2::~vtkVideoSource2()
{ 
	// we certainly don't want to access a virtual 
	// function after the subclass has destructed!!
	this->vtkVideoSource2::ReleaseSystemResources();

	this->SetFrameBufferSize(0);
	this->Buffer->Delete();
	if ( this->PlayerThreader != NULL )
	{
		this->PlayerThreader->Delete();
	}

#ifdef PLUS_PRINT_VIDEO_TIMESTAMP_DEBUG_INFO
	this->DebugInfoStream.close(); 
#endif

	// remove ourselves from the global list
	// vtkVideoSourceRemove(this);
}

//----------------------------------------------------------------------------
void vtkVideoSource2::PrintSelf(ostream& os, vtkIndent indent)
{
	int idx;

	this->Superclass::PrintSelf(os,indent);

	os << indent << "ClipRegion: (" << this->ClipRegion[0];
	for (idx = 1; idx < 6; ++idx)
	{
		os << ", " << this->ClipRegion[idx];
	}
	os << ")\n";

	os << indent << "DataSpacing: (" << this->DataSpacing[0];
	for (idx = 1; idx < 3; ++idx)
	{
		os << ", " << this->DataSpacing[idx];
	}
	os << ")\n";

	os << indent << "DataOrigin: (" << this->DataOrigin[0];
	for (idx = 1; idx < 3; ++idx)
	{
		os << ", " << this->DataOrigin[idx];
	}
	os << ")\n";

	os << indent << "OutputWholeExtent: (" << this->OutputWholeExtent[0];
	for (idx = 1; idx < 6; ++idx)
	{
		os << ", " << this->OutputWholeExtent[idx];
	}
	os << ")\n";

	os << indent << "FrameRate: " << this->FrameRate << "\n";

	os << indent << "FrameCount: " << this->FrameCount << "\n";

	os << indent << "Recording: " << (this->Recording ? "On\n" : "Off\n");

	os << indent << "Playing: " << (this->Playing ? "On\n" : "Off\n");

	os << indent << "NumberOfOutputFrames: " << this->NumberOfOutputFrames << "\n";

	os << indent << "AutoAdvance: " << (this->AutoAdvance ? "On\n" : "Off\n");

	os << indent << "Buffer:\n";
	this->Buffer->PrintSelf(os, indent.GetNextIndent());
}

//----------------------------------------------------------------------------
void vtkVideoSource2::SetFrameSize(int x, int y, int z)
{
	int frameSize[3];
	this->Buffer->GetFrameFormat()->GetFrameSize(frameSize);

	if (x == frameSize[0] && 
		y == frameSize[1] && 
		z == frameSize[2])
	{
		return;
	}

	if (x < 1 || y < 1 || z < 1) 
	{
		vtkErrorMacro(<< "SetFrameSize: Illegal frame size");
		return;
	}

	if (this->Initialized) 
	{
		this->Buffer->Lock();
		this->Buffer->GetFrameFormat()->SetFrameSize(x,y,z);
		this->UpdateFrameBuffer();
		this->Buffer->Unlock();
	}
	else
	{
		this->Buffer->GetFrameFormat()->SetFrameSize(x,y,z);
	}

	this->Modified();
}

//----------------------------------------------------------------------------
int* vtkVideoSource2::GetFrameSize()
{
	return this->Buffer->GetFrameFormat()->GetFrameSize();
}

//----------------------------------------------------------------------------
void vtkVideoSource2::GetFrameSize(int &x, int &y, int &z)
{
	int *dim = new int[3];
	this->GetFrameSize(dim);
	x = dim[0];
	y = dim[1];
	z = dim[2];
	delete[] dim;
}

//----------------------------------------------------------------------------
void vtkVideoSource2::GetFrameSize(int dim[3])
{
	this->Buffer->GetFrameFormat()->GetFrameSize(dim);
}

//----------------------------------------------------------------------------
void vtkVideoSource2::SetFrameRate(float rate)
{
	if (this->FrameRate == rate)
	{
		return;
	}

	this->FrameRate = rate;
	this->Modified();
}

//----------------------------------------------------------------------------
void vtkVideoSource2::SetClipRegion(int x0, int x1, int y0, int y1, 
									int z0, int z1)
{
	if (this->ClipRegion[0] == x0 && this->ClipRegion[1] == x1 &&
		this->ClipRegion[2] == y0 && this->ClipRegion[3] == y1 &&
		this->ClipRegion[4] == z0 && this->ClipRegion[5] == z1)
	{
		return;
	}

	if (this->Initialized) 
	{
		this->Buffer->Lock();
		this->ClipRegion[0] = x0; this->ClipRegion[1] = x1;
		this->ClipRegion[2] = y0; this->ClipRegion[3] = y1;
		this->ClipRegion[4] = z0; this->ClipRegion[5] = z1;
		// modifies the buffer's frame extent
		this->UpdateFrameBuffer();
		this->Buffer->Unlock();
	}
	else
	{
		this->ClipRegion[0] = x0; this->ClipRegion[1] = x1;
		this->ClipRegion[2] = y0; this->ClipRegion[3] = y1;
		this->ClipRegion[4] = z0; this->ClipRegion[5] = z1;
	}

	this->Modified();
}

//----------------------------------------------------------------------------
int vtkVideoSource2::GetFrameBufferSize()
{
	return this->Buffer->GetBufferSize();
}

//----------------------------------------------------------------------------
// Override this and provide checks to ensure an appropriate number
// of components was asked for (i.e. 1 for greyscale, 3 for RGB,
// or 4 for RGBA)
void vtkVideoSource2::SetOutputFormat(int format)
{
	if (format == (int)(this->Buffer->GetFrameFormat()->GetPixelFormat()))
	{
		return;
	}

	this->Buffer->GetFrameFormat()->SetPixelFormat((unsigned int)format);

	// convert color format to number of scalar components
	int numComponents = 1;
	switch (format)
	{
	case VTK_RGBA:
		numComponents = 4;
		break;
	case VTK_RGB:
		numComponents = 3;
		break;
	case VTK_LUMINANCE_ALPHA:
		numComponents = 2;
		break;
	case VTK_LUMINANCE:
		numComponents = 1;
		break;
	default:
		vtkErrorMacro(<< "SetOutputFormat: Unrecognized color format.");
		break;
	}
	this->NumberOfScalarComponents = numComponents;

	// make sure the buffer's BitsPerPixel matches the pixel format
	this->Buffer->Lock();
	this->Buffer->GetFrameFormat()->SetBitsPerPixel(numComponents*8);
	if (this->Initialized)
	{
		this->UpdateFrameBuffer();
	}
	this->Buffer->Unlock();

	this->Modified();
}

//----------------------------------------------------------------------------
int vtkVideoSource2::GetOutputFormat()
{
	return (int) (this->Buffer->GetFrameFormat()->GetPixelFormat());
}

//----------------------------------------------------------------------------
// set or change the circular buffer size
// you will have to override this if you want the buffers 
// to be device-specific (i.e. something other than vtkDataArray)
void vtkVideoSource2::SetFrameBufferSize(int bufsize)
{
	if (bufsize < 0)
	{
		vtkErrorMacro(<< "SetFrameBufferSize: There must be at least one framebuffer");
	}

	this->Buffer->Lock();

	// update FrameIndex
	if (this->Buffer == 0)
	{
		if (bufsize > 0)
		{
			this->FrameIndex = -1;
		}
	}
	else
	{
		if (bufsize > 0)
		{
			if (this->FrameIndex >= bufsize)
			{
				this->FrameIndex = bufsize - 1;
			}
		}
		else
		{
			this->FrameIndex = -1;
		}
	}

	// update the buffer size
	this->Buffer->SetBufferSize(bufsize);
	if (this->Initialized)
	{
		this->UpdateFrameBuffer();
	}
	this->Buffer->Unlock();
}

//----------------------------------------------------------------------------
void vtkVideoSource2::SetOpacity(float alpha)
{
	if (this->Initialized)
	{
		this->Buffer->Lock();
		this->Buffer->GetFrameFormat()->SetOpacity(alpha);
		this->UpdateFrameBuffer();
		this->Buffer->Unlock();
	}
	else
	{
		this->Buffer->GetFrameFormat()->SetOpacity(alpha);
	}
}

//----------------------------------------------------------------------------
float vtkVideoSource2::GetOpacity()
{
	return this->Buffer->GetFrameFormat()->GetOpacity();
}

//----------------------------------------------------------------------------
double vtkVideoSource2::GetFrameTimeStamp(int frame)
{
	return this->Buffer->GetTimeStamp(frame);
}

//----------------------------------------------------------------------------
// Updates the frame buffer according to any changes in the frame buffer's
// information.  This function should always be called from within the buffer's
// mutex lock and should never be called asynchronously.  It ensures that all
// frames within the buffer have the correct formatting and allocates space
// for all of the frames, with the side effect that any image data in the frames
// will be erased.
void vtkVideoSource2::UpdateFrameBuffer()
{
	int i, oldExt;
	int ext;

	// clip the ClipRegion with the FrameSize and allocate memory for
	// the buffer's frame format
	int frameSize[3];
	int frameBufferExtent[6];
	this->Buffer->GetFrameFormat()->GetFrameExtent(frameBufferExtent);
	this->Buffer->GetFrameFormat()->GetFrameSize(frameSize);
	for (i = 0; i < 3; i++)
	{
		oldExt = frameBufferExtent[2*i+1] - frameBufferExtent[2*i] + 1;
		frameBufferExtent[2*i] = ((this->ClipRegion[2*i] > 0) 
			? this->ClipRegion[2*i] : 0);
		frameBufferExtent[2*i+1] = ((this->ClipRegion[2*i+1] < frameSize[i]-1) 
			? this->ClipRegion[2*i+1] : frameSize[i]-1);

		ext = frameBufferExtent[2*i+1] - frameBufferExtent[2*i] +1;
		if (ext < 0)
		{
			frameBufferExtent[2*i] = 0;
			frameBufferExtent[2*i+1] = -1;
			ext = 0;
		}

		if (oldExt > ext)
		{
			this->OutputNeedsInitialization = 1;
		}
	}
	this->Buffer->GetFrameFormat()->SetFrameExtent(frameBufferExtent);
	this->Buffer->GetFrameFormat()->Allocate();

	// for each of the frames in the buffer, make sure that its format matches
	// the buffer's FrameFormat and allocate
	vtkVideoFrame2 *currFrame;
	vtkVideoFrame2 *frameFormat;
	int currFrameSize[3];
	int formatFrameSize[3];
	int currFrameExtent[6];
	int formatFrameExtent[6];
	int currBitsPerPixel, formatBitsPerPixel;
	int currRowAlignment, formatRowAlignment;
	int currTopDown, formatTopDown;
	float currOpacity, formatOpacity;
	int currPixelFormat, formatPixelFormat;
	int currCompression, formatCompression;
	int currFrameGrabberType, formatFrameGrabberType;

	frameFormat = this->Buffer->GetFrameFormat();
	frameFormat->GetFrameSize(formatFrameSize);
	frameFormat->GetFrameExtent(formatFrameExtent);
	formatBitsPerPixel = frameFormat->GetBitsPerPixel();
	formatRowAlignment = frameFormat->GetRowAlignment();
	formatTopDown = frameFormat->GetTopDown();
	formatOpacity = frameFormat->GetOpacity();
	formatPixelFormat = frameFormat->GetPixelFormat();
	formatCompression = frameFormat->GetCompression();
	formatFrameGrabberType = frameFormat->GetFrameGrabberType();

	i = this->Buffer->GetBufferSize();
	while (--i >= 0)
	{
		currFrame = this->Buffer->GetFrame(i);
		currFrame->GetFrameSize(currFrameSize);
		currFrame->GetFrameExtent(currFrameExtent);
		currBitsPerPixel = currFrame->GetBitsPerPixel();
		currRowAlignment = currFrame->GetRowAlignment();
		currTopDown = currFrame->GetTopDown();
		currOpacity = currFrame->GetOpacity();
		currPixelFormat = currFrame->GetPixelFormat();
		currCompression = currFrame->GetCompression();
		currFrameGrabberType = currFrame->GetFrameGrabberType();

		if (currFrameSize[0] != formatFrameSize[0] ||
			currFrameSize[1] != formatFrameSize[1] ||
			currFrameSize[2] != formatFrameSize[2])
		{
			currFrame->SetFrameSize(formatFrameSize);
		}

		if (currFrameExtent[0] != formatFrameExtent[0] ||
			currFrameExtent[1] != formatFrameExtent[1] ||
			currFrameExtent[2] != formatFrameExtent[2] ||
			currFrameExtent[3] != formatFrameExtent[3] ||
			currFrameExtent[4] != formatFrameExtent[4] ||
			currFrameExtent[5] != formatFrameExtent[5])
		{
			currFrame->SetFrameExtent(formatFrameExtent);
		}

		if (currBitsPerPixel != formatBitsPerPixel)
		{
			currFrame->SetBitsPerPixel(formatBitsPerPixel);
		}

		if (currRowAlignment != formatRowAlignment)
		{
			currFrame->SetRowAlignment(formatRowAlignment);
		}

		if (currTopDown != formatTopDown)
		{
			currFrame->SetTopDown(formatTopDown);
		}

		if (currOpacity != formatOpacity)
		{
			currFrame->SetOpacity(formatOpacity);
		}

		if (currPixelFormat != formatPixelFormat)
		{
			currFrame->SetPixelFormat(formatPixelFormat);
		}

		if (currCompression != formatCompression)
		{
			currFrame->SetCompression(formatCompression);
		}

		if (currFrameGrabberType != formatFrameGrabberType)
		{
			currFrame->SetFrameGrabberType(formatFrameGrabberType);
		}

		currFrame->Allocate();
	}
}

//----------------------------------------------------------------------------
// Initialize() should be overridden to initialize the hardware frame grabber
void vtkVideoSource2::Initialize()
{
	if (this->Initialized)
	{
		return;
	}
	this->Initialized = 1;

	this->UpdateFrameBuffer();
}

//----------------------------------------------------------------------------
// Connect() should be overridden to connect to the hardware 
int vtkVideoSource2::Connect()
{
	return 1; 
}

//----------------------------------------------------------------------------
// Disconnect() should be overridden to disconnect from the hardware 
void vtkVideoSource2::Disconnect()
{

}

//----------------------------------------------------------------------------
// ReleaseSystemResources() should be overridden to release the hardware
void vtkVideoSource2::ReleaseSystemResources()
{
	if (this->Playing || this->Recording)
	{
		this->Stop();
	}

	this->Initialized = 0;
}

//----------------------------------------------------------------------------
// Copy pseudo-random noise into the frames.  This function may be called
// asynchronously.
void vtkVideoSource2::InternalGrab()
{
	int i;
	static int randsave = 0;
	int randNum;
	unsigned char *ptr;
	int *lptr;

	// get a thread lock on the frame buffer
	this->Buffer->Lock();

	// advance the buffer so that we don't copy over old data
	if (this->AutoAdvance)
	{
		this->AdvanceFrameBuffer(1);
		if (this->FrameIndex + 1 < this->Buffer->GetBufferSize())
		{
			this->FrameIndex++;
		}
	}

	// get the number of bytes needed to store one frame
	int totalSize = this->Buffer->GetFrameFormat()->GetBytesInFrame();

	randNum = randsave;

	// get the pointer to the first pixel of the current frame
	ptr = reinterpret_cast<unsigned char*>(this->Buffer->GetFrame(0)->GetVoidPointer(0));

	// Somebody should check this:
	lptr = (int *)(((((long)ptr) + 3)/4)*4);
	i = totalSize/4;

	while (--i >= 0)
	{
		randNum = 1664525*randNum + 1013904223;
		*lptr++ = randNum;
	}
	unsigned char *ptr1 = ptr + 4;
	i = (totalSize-4)/16;
	while (--i >= 0)
	{
		randNum = 1664525*randNum + 1013904223;
		*ptr1 = randNum;
		ptr1 += 16;
	}
	randsave = randNum;

	// add the new frame and the current time to the buffer
	double timestamp = vtkAccurateTimer::GetSystemTime(); 
	this->Buffer->AddItem(this->Buffer->GetFrame(0), timestamp, timestamp, ++this->FrameNumber);

	if (this->FrameCount++ == 0)
	{
		this->StartTimeStamp = this->Buffer->GetTimeStamp(0);
	}

	this->Modified();

	this->Buffer->Unlock();
}

//----------------------------------------------------------------------------
// Sleep until the specified absolute time has arrived.
// You must pass a handle to the current thread.  
// If '0' is returned, then the thread was aborted before or during the wait.
static int vtkThreadSleep(vtkMultiThreader::ThreadInfo *data, double time)
{
	// loop either until the time has arrived or until the thread is ended
	for (int i = 0;; i++)
	{
		double remaining = time - vtkAccurateTimer::GetSystemTime();

		// check to see if we have reached the specified time
		if (remaining <= 0)
		{
			if (i == 0)
			{
				//vtkGenericWarningMacro("Dropped a video frame.");
				//std::cout << "dropped a video frame" << std::endl; // TODO put back
			}
			return 1;
		}
		// check the ActiveFlag at least every 0.1 seconds
		if (remaining > 0.1)
		{
			remaining = 0.1;
		}

		// check to see if we are being told to quit 
		data->ActiveFlagLock->Lock();
		int activeFlag = *(data->ActiveFlag);
		data->ActiveFlagLock->Unlock();

		if (activeFlag == 0)
		{
			break;
		}

		vtkAccurateTimer::Delay(remaining);
	}

	return 0;
}

//----------------------------------------------------------------------------
// this function runs in an alternate thread to asyncronously grab frames
static void *vtkVideoSourceRecordThread(vtkMultiThreader::ThreadInfo *data)
{
	vtkVideoSource2 *self = (vtkVideoSource2 *)(data->UserData);

	double startTime = vtkAccurateTimer::GetSystemTime();
	double rate = self->GetFrameRate();
	int frame = 0;

	do
	{
		self->InternalGrab();
		frame++;
	}
	while (vtkThreadSleep(data, startTime + frame/rate));

	return NULL;
}

//----------------------------------------------------------------------------
// Set the source to grab frames continuously.
// You should override this as appropriate for your device.  
void vtkVideoSource2::Record()
{
	if (this->Playing)
	{
		this->Stop();
	}

	if (!this->Recording)
	{
		this->Initialize();
		this->Recording = 1;
		this->FrameCount = 0;
		this->Modified();
		this->PlayerThreadId = 
			this->PlayerThreader->SpawnThread((vtkThreadFunctionType)\
			&vtkVideoSourceRecordThread,this);
	}
}

//----------------------------------------------------------------------------
// this function runs in an alternate thread to 'play the tape' at the
// specified frame rate.
static void *vtkVideoSourcePlayThread(vtkMultiThreader::ThreadInfo *data)
{
	vtkVideoSource2 *self = (vtkVideoSource2 *)(data->UserData);

	double startTime = vtkAccurateTimer::GetSystemTime();
	double rate = self->GetFrameRate();
	int frame = 0;

	do
	{
		self->Seek(1);
		frame++;
	}
	while (vtkThreadSleep(data, startTime + frame/rate));

	return NULL;
}

//----------------------------------------------------------------------------
// Set the source to play back recorded frames.
// You should override this as appropriate for your device.  
void vtkVideoSource2::Play()
{
	if (this->Recording)
	{
		this->Stop();
	}

	if (!this->Playing)
	{
		this->Initialize();

		this->Playing = 1;
		this->Modified();
		this->PlayerThreadId = 
			this->PlayerThreader->SpawnThread((vtkThreadFunctionType)\
			&vtkVideoSourcePlayThread,this);
	}
}

//----------------------------------------------------------------------------
// Stop continuous grabbing or playback.  You will have to override this
// if your class overrides Play() and Record()
void vtkVideoSource2::Stop()
{
	if (this->Playing || this->Recording)
	{
		this->PlayerThreader->TerminateThread(this->PlayerThreadId);
		this->PlayerThreadId = -1;
		this->Playing = 0;
		this->Recording = 0;
		this->Modified();
	}
} 

//----------------------------------------------------------------------------
// Rewind back to the frame with the earliest timestamp.
// This assumes that the times motonically increase as the index decreases.
void vtkVideoSource2::Rewind()
{
	this->Buffer->Lock();

	double lowest = 0;
	double stamp;
	int i;

	if (this->Buffer->GetBufferSize())
	{
		lowest = this->Buffer->GetTimeStamp(0);
	}

	// search for the frame with the lowest timestamp
	for (i = 0; i < this->Buffer->GetBufferSize(); i++)
	{
		stamp = this->Buffer->GetTimeStamp(-(i+1));
		if (stamp != 0.0 && stamp <= lowest)
		{
			lowest = stamp;
		}
		else
		{
			break;
		}
	}
	stamp = this->Buffer->GetTimeStamp(-i);

	// update the buffer's current index
	if (stamp != 0 && stamp < 980000000.0)
	{
		vtkWarningMacro("Rewind: bogus time stamp!");
	}
	else
	{
		this->AdvanceFrameBuffer(-i);
		this->FrameIndex = (this->FrameIndex - i) % this->Buffer->GetBufferSize();
		while (this->FrameIndex < 0)
		{
			this->FrameIndex += this->Buffer->GetBufferSize();
		}
	}

	this->Buffer->Unlock();
}  

//----------------------------------------------------------------------------
// Fast-forward to the frame with the latest timestamp.
// This assumes that the times motonically increase as the index decreases.
void vtkVideoSource2::FastForward()
{
	this->Buffer->Lock();

	double highest = 0;
	double stamp;
	int i;

	if (this->Buffer->GetBufferSize())
	{
		highest = this->Buffer->GetTimeStamp(0);
	}

	// search for the frame with the highest timestamp
	for (i = 0; i < this->Buffer->GetBufferSize(); i++)
	{
		stamp = this->Buffer->GetTimeStamp(i+1);
		if (stamp != 0.0 && stamp >= highest)
		{
			highest = stamp;
		}
		else
		{
			break;
		}
	}
	stamp = this->Buffer->GetTimeStamp(i);

	// update the buffer's current index
	if (stamp != 0 && stamp < 980000000.0)
	{
		vtkWarningMacro("Rewind: bogus time stamp!");
	}
	else
	{
		this->AdvanceFrameBuffer(i);
		this->FrameIndex = (this->FrameIndex + i) % this->Buffer->GetBufferSize();
		while (this->FrameIndex < 0)
		{
			this->FrameIndex += this->Buffer->GetBufferSize();
		}
	}

	this->Buffer->Unlock();
}  

//----------------------------------------------------------------------------
// Rotate the buffers
void vtkVideoSource2::Seek(int n)
{ 
	this->Buffer->Lock();
	this->AdvanceFrameBuffer(n);
	this->FrameIndex = (this->FrameIndex + n) % this->Buffer->GetBufferSize();
	while (this->FrameIndex < 0)
	{
		this->FrameIndex += this->Buffer->GetBufferSize();
	}
	this->Buffer->Unlock();
	this->Modified(); 
}

//----------------------------------------------------------------------------
// The grab function, which should (of course) be overridden to do
// the appropriate hardware stuff.  This function should never be
// called asynchronously.
void vtkVideoSource2::Grab()
{
	// ensure that the hardware is initialized.
	this->Initialize();

	this->InternalGrab();
}

//----------------------------------------------------------------------------
// This function must be called only from within a mutex lock
void vtkVideoSource2::AdvanceFrameBuffer(int n)
{
	this->Buffer->Seek(n);
}

//----------------------------------------------------------------------------
// This method returns the largest data that can be generated.
int vtkVideoSource2::RequestInformation(
										vtkInformation * vtkNotUsed(request),
										vtkInformationVector **vtkNotUsed(inputVector),
										vtkInformationVector *outputVector)
{
	// get the info objects
	vtkInformation* outInfo = outputVector->GetInformationObject(0);

	int i;
	int extent[6];

	// ensure that the hardware is initialized.
	this->Initialize();

	// set the whole extent
	int frameExtent[6];
	this->Buffer->GetFrameFormat()->GetFrameExtent(frameExtent);
	for (i = 0; i < 3; i++)
	{
		// initially set extent to the OutputWholeExtent
		extent[2*i] = this->OutputWholeExtent[2*i];
		extent[2*i+1] = this->OutputWholeExtent[2*i+1];
		// if 'flag' is set in output extent, use the FrameBufferExtent instead
		if (extent[2*i+1] < extent[2*i])
		{
			extent[2*i] = 0; 
			extent[2*i+1] = \
				frameExtent[2*i+1] - frameExtent[2*i];
		}
		this->FrameOutputExtent[2*i] = extent[2*i];
		this->FrameOutputExtent[2*i+1] = extent[2*i+1];
	}
	// concatenate along Z according to the number of output frames
	int numFrames = this->NumberOfOutputFrames;
	if (numFrames < 1)
	{
		numFrames = 1;
	}
	if (numFrames > this->Buffer->GetBufferSize())
	{
		numFrames = this->Buffer->GetBufferSize();
	}
	// multiply Z extent by number of frames to output
	extent[5] = extent[4] + (extent[5]-extent[4]+1) * numFrames - 1;
	outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),extent,6);

	// set the spacing
	outInfo->Set(vtkDataObject::SPACING(),this->DataSpacing,3);

	// set the origin.
	outInfo->Set(vtkDataObject::ORIGIN(),this->DataOrigin,3);

	// set default data type
	vtkDataObject::SetPointDataActiveScalarInfo(outInfo, VTK_UNSIGNED_CHAR, 
		this->NumberOfScalarComponents);

	return 1;
}

//----------------------------------------------------------------------------
// The Execute method is fairly complex, so I would not recommend overriding
// it unless you have to.  Override the UnpackRasterLine() method in
// vtkVideoFrame2 instead.
int vtkVideoSource2::RequestData(
								 vtkInformation *vtkNotUsed(request),
								 vtkInformationVector **vtkNotUsed(inputVector),
								 vtkInformationVector *vtkNotUsed(outputVector))
{
	// the output data
	vtkImageData *data = this->AllocateOutputData(this->GetOutput());
	int i;

	// extent of the output, will later be clipped in Z to a single frame
	int outputExtent[6];

	// saved version of the output extent
	int saveOutputExtent[6]; // will possibly contain multiple frames
	data->GetExtent(outputExtent);
	for (i = 0; i < 6; i++)
	{
		saveOutputExtent[i] = outputExtent[i];
	}

	// clip the output extent to the Z size of one frame  
	outputExtent[4] = this->FrameOutputExtent[4]; 
	outputExtent[5] = this->FrameOutputExtent[5]; 

	// if the output is more than a single frame,
	// then the output will cover a partial or full first frame,
	// several full frames, and a partial or full last frame

	// the output data
	unsigned char *outPtr = (unsigned char *)data->GetScalarPointer();

	// if output extent has changed, need to initialize output to black
	for (i = 0; i < 3; i++)
	{
		if (saveOutputExtent[i] != this->LastOutputExtent[i])
		{
			this->LastOutputExtent[i] = saveOutputExtent[i];
			this->OutputNeedsInitialization = 1;
		}
	}

	// ditto for number of scalar components
	if (data->GetNumberOfScalarComponents() != 
		this->LastNumberOfScalarComponents)
	{
		this->LastNumberOfScalarComponents = data->GetNumberOfScalarComponents();
		this->OutputNeedsInitialization = 1;
	}

	// initialize output to zero only when necessary
	if (this->OutputNeedsInitialization)
	{
		memset(outPtr,0,
			(saveOutputExtent[1]-saveOutputExtent[0]+1)*
			(saveOutputExtent[3]-saveOutputExtent[2]+1)*
			(saveOutputExtent[5]-saveOutputExtent[4]+1)*this->NumberOfScalarComponents);
		this->OutputNeedsInitialization = 0;
	} 

	this->Buffer->Lock();
	this->FrameTimeStamp = this->Buffer->GetTimeStamp(0);

	// get the most recent frame if we are not updating with the desired timestamp, or the
	// frame closest to the desired timestamp if we are
	int index = 0; // default index is most recent frame
	if (this->UpdateWithDesiredTimestamp && this->DesiredTimestamp != -1)
	{
		index = this->Buffer->GetIndexFromTime(this->DesiredTimestamp);

		/*double actual = abs(this->DesiredTimestamp - this->Buffer->GetTimeStamp(index));
		double before = abs(this->DesiredTimestamp - this->Buffer->GetTimeStamp(index-1));
		double after = abs(this->DesiredTimestamp - this->Buffer->GetTimeStamp(index+1));
		if ((actual > before) || (actual > after))
		{
		std::cout << "ERROR IN VIDEO SOURCE!!!" << std::endl;
		}*/

		//std::cout << setiosflags(ios::fixed);
		//std::cout << "d - " << this->DesiredTimestamp << "; a - " << this->Buffer->GetTimeStamp(index) << "; -1 - " << this->Buffer->GetTimeStamp(index-1) << "; +1 - " << this->Buffer->GetTimeStamp(index+1) << std::endl;

	}
	this->Buffer->GetFrame(index)->CopyData(outPtr, outputExtent, saveOutputExtent, this->Buffer->GetFrameFormat()->GetPixelFormat());

	// Get the frame timestamp for the index
	this->TimestampClosestToDesired = this->Buffer->GetTimeStamp(index);

	this->Buffer->Unlock();

	return 1;
}

//----------------------------------------------------------------------------
//TODO create directory if it doesn't exist yet
void vtkVideoSource2::WriteFramesToFile(vtkImageWriter *writer, const char *summaryFileName, const char *filePrefix, const char *filePattern, int numFrames)
{

	this->Buffer->Lock();

	// set up the buffer
	if (this->Recording || this->Playing)
	{
		this->Stop();
	}
	//int numItems = this->Buffer->GetNumberOfItems();
	this->Seek(-numFrames);

	// open the summary file and write the format and opacity
	ofstream outputStream;
	outputStream.open(summaryFileName);
	outputStream << setiosflags(ios::fixed);
	if (outputStream.is_open())
	{
		int format = this->Buffer->GetFrameFormat()->GetPixelFormat();
		outputStream << "OutputFormat: " <<
			(format == VTK_RGBA ? "RGBA" :
			(format == VTK_RGB ? "RGB" :
			(format == VTK_LUMINANCE_ALPHA ? "LuminanceAlpha" :
			(format == VTK_LUMINANCE ? "Luminance" : "Unknown"))))
			<< "\n";
		outputStream << "Opacity: " << this->Buffer->GetFrameFormat()->GetOpacity() << "\n" << "\n";
	}

	// write the files
	vtkImageData *image = vtkImageData::New();
	char fileName[100];
	for (int i = 0; i < numFrames; i++)
	{
		this->Seek(1);

		// write the output image
		image = this->GetOutput();
		sprintf(fileName, filePattern, filePrefix, i);
		writer->SetFileName(fileName);
		writer->SetInput(image);
		writer->Write();

		// write a line in the summary file
		if (outputStream.is_open())
		{
			outputStream << this->Buffer->GetTimeStamp(0) << " " << fileName << "\n";
		}
	}
	outputStream.close();
	image->Delete();

	this->Buffer->Unlock();
}

//----------------------------------------------------------------------------
void vtkVideoSource2::WriteFramesAsBMP(const char *summaryFileName, const char *filePrefix, int numFrames)
{
	vtkBMPWriter *writer = vtkBMPWriter::New();
	this->WriteFramesToFile(writer, summaryFileName, filePrefix, "%s%d.bmp", numFrames);
	writer->Delete();
}

//----------------------------------------------------------------------------
void vtkVideoSource2::WriteFramesAsBMP(const char *summaryFileName, const char *filePrefix)
{
	this->WriteFramesAsBMP(summaryFileName, filePrefix, this->Buffer->GetNumberOfItems());
}

//----------------------------------------------------------------------------
/*void vtkVideoSource2::WriteFramesAsMINCImage(const char *summaryFileName, const char *filePrefix, int numFrames)
{
vtkMINCImageWriter *writer = vtkMINCImageWriter::New();
this->WriteFramesToFile(writer, summaryFileName, filePrefix, "%s%d.mnc", numFrames);
writer->Delete();
}*/

/*//----------------------------------------------------------------------------
void vtkVideoSource2::WriteFramesAsMINCImage(const char *summaryFileName, const char *filePrefix)
{
this->WriteFramesAsMINCImage(summaryFileName, filePrefix, this->Buffer->GetNumberOfItems());
}*/

//----------------------------------------------------------------------------
void vtkVideoSource2::WriteFramesAsPNG(const char *summaryFileName, const char *filePrefix, int numFrames)
{
	vtkPNGWriter *writer = vtkPNGWriter::New();
	this->WriteFramesToFile(writer, summaryFileName, filePrefix, "%s%d.png", numFrames);
	writer->Delete();
}

//----------------------------------------------------------------------------
void vtkVideoSource2::WriteFramesAsPNG(const char *summaryFileName, const char *filePrefix)
{
	this->WriteFramesAsPNG(summaryFileName, filePrefix, this->Buffer->GetNumberOfItems());
}

//----------------------------------------------------------------------------
void vtkVideoSource2::WriteFramesAsTIFF(const char *summaryFileName, const char *filePrefix, int compression, int numFrames)
{
	vtkTIFFWriter *writer = vtkTIFFWriter::New();
	writer->SetCompression(compression);
	this->WriteFramesToFile(writer, summaryFileName, filePrefix, "%s%d.tiff", numFrames);
	writer->Delete();
}

//----------------------------------------------------------------------------
void vtkVideoSource2::WriteFramesAsTIFF(const char *summaryFileName, const char *filePrefix, int compression)
{
	this->WriteFramesAsTIFF(summaryFileName, filePrefix, compression, this->Buffer->GetNumberOfItems());
}

//----------------------------------------------------------------------------
// Used by ReadFramesFromFile - copies data from one array to another, meant to
// copy from an image into a frame array by performing any necessary conversion
// Supported:  BMP, MINC, PNG and TIFF in luminance, RGB, and RGBA formats
void vtkVideoSource2::CopyImageToFrame(unsigned char *outPtr, unsigned char *inPtr, int bytesInFrame, int outputFormat, int *dimensions, float opacity, int fileType)
{
	switch (outputFormat)
	{
	case VTK_LUMINANCE:
		{
			if (fileType == FILETYPE_BMP)
			{
				for (int i = 0; i < bytesInFrame; i++)
				{
					*outPtr = *inPtr;
					outPtr++;
					inPtr++;
					inPtr++;
					inPtr++;
				}
			}
			else if (/*fileType == FILETYPE_MINC ||*/ fileType == FILETYPE_PNG || fileType == FILETYPE_TIFF)
			{
				for (int i = 0; i < bytesInFrame; i++)
				{
					*outPtr = *inPtr;
					outPtr++;
					inPtr++;
				}
			}
			/*else if (fileType == FILETYPE_TIFF)
			{
			outPtr += dimensions[0]*dimensions[1] - dimensions[0];
			for (int i = 0; i < dimensions[1]; i++)
			{
			for (int j = 0; j < dimensions[0]; j++)
			{
			*outPtr = *inPtr;
			outPtr++;
			inPtr++;
			}
			outPtr -= dimensions[0]*2;
			}
			}*/
			break;
		}
	case VTK_RGB:
		{
			for (int i = 0; i < bytesInFrame/3; i++)
			{
				/*// convert from rgb to bgr
				inPtr += 2;
				*outPtr = *inPtr;
				outPtr++;
				inPtr--;
				*outPtr = *inPtr;
				outPtr++;
				inPtr--;
				*outPtr = *inPtr;
				outPtr++;
				inPtr += 3;*/
				*outPtr++ = *inPtr++;
				*outPtr++ = *inPtr++;
				*outPtr++ = *inPtr++;
			}
			/*  }
			else if (fileType == FILETYPE_TIFF)
			{
			outPtr += (dimensions[0]*dimensions[1] - dimensions[0])*3;
			for (int i = 0; i < dimensions[1]; i++)
			{
			for (int j = 0; j < dimensions[0]; j++)
			{
			// convert from rgb to bgr
			inPtr += 2;
			*outPtr = *inPtr;
			outPtr++;
			inPtr--;
			*outPtr = *inPtr;
			outPtr++;
			inPtr--;
			*outPtr = *inPtr;
			outPtr++;
			inPtr += 3;
			}
			outPtr -= dimensions[0]*2*3;
			}
			}*/
			break;
		}
	case VTK_RGBA:
		{
			if (fileType == FILETYPE_BMP)
			{
				for (int i = 0; i < bytesInFrame/4; i++)
				{
					// convert from rgb to bgr and add alpha value
					/*inPtr += 2;
					*outPtr = *inPtr;
					outPtr++;
					inPtr--;
					*outPtr = *inPtr;
					outPtr++;
					inPtr--;
					*outPtr = *inPtr;
					outPtr++;
					inPtr += 3;
					*outPtr = (unsigned char)(opacity * 255);
					outPtr++;*/
					*outPtr++ = *inPtr++;
					*outPtr++ = *inPtr++;
					*outPtr++ = *inPtr++;
					*outPtr = (unsigned char)(opacity * 255);
					outPtr++;
				}
			}
			else if (fileType == FILETYPE_PNG || /*fileType == FILETYPE_MINC ||*/ fileType == FILETYPE_TIFF)
			{
				for (int i = 0; i < bytesInFrame/4; i++)
				{
					// convert from rgb to bgr and add alpha value
					/*inPtr += 2;
					*outPtr = *inPtr;
					outPtr++;
					inPtr--;
					*outPtr = *inPtr;
					outPtr++;
					inPtr--;
					*outPtr = *inPtr;
					outPtr++;
					inPtr += 3;
					*outPtr = (unsigned char)(opacity * 255);
					outPtr++;*/
					*outPtr++ = *inPtr++;
					*outPtr++ = *inPtr++;
					*outPtr++ = *inPtr++;
					*outPtr = (unsigned char)(opacity * 255);
					outPtr++;
					inPtr++;
				}
			}
			/*else if (fileType == FILETYPE_TIFF)
			{
			outPtr += (dimensions[0]*dimensions[1] - dimensions[0])*4;
			for (int i = 0; i < dimensions[1]; i++)
			{
			for (int j = 0; j < dimensions[0]; j++)
			{
			// convert from rgb to bgr
			inPtr += 2;
			*outPtr = *inPtr;
			outPtr++;
			inPtr--;
			*outPtr = *inPtr;
			outPtr++;
			inPtr--;
			*outPtr = *inPtr;
			outPtr++;
			inPtr += 3;
			*outPtr = (unsigned char)(opacity * 255);
			outPtr++;
			inPtr++;
			}
			outPtr -= dimensions[0]*2*4;
			}
			}*/
			break;
		}
	}
}

//----------------------------------------------------------------------------
// for accurate timing of the frame: an exponential moving average
// is computed to smooth out the jitter in the times that are returned by the system clock:
// EstimatedFramePeriod[t] = EstimatedFramePeriod[t-1] * (1-SmoothingFactor) + FramePeriod[t] * SmoothingFactor
// Smaller SmoothingFactor results leads to less jitter.
void vtkVideoSource2::CreateTimeStampForFrame(unsigned long framecount, double &unfilteredTimestamp, double &filteredTimestamp)
{
	double timestamp = vtkAccurateTimer::GetSystemTime();
	unfilteredTimestamp = timestamp; 

	if ( this->LastFrameCount == 0 )
	{
		this->LastFrameCount = framecount; 
		this->LastTimeStamp = timestamp; 
		this->LastUnfilteredTimeStamp = unfilteredTimestamp; 
	}

	if ( framecount == this->LastFrameCount )
	{
		unfilteredTimestamp = this->LastUnfilteredTimeStamp; 
		filteredTimestamp = this->LastTimeStamp; 
		return; 
	}

	// maximum allowed difference of the frame delay compared to average delay, in seconds
	const unsigned int numberOfAveragedPeriods = 20; 

	unsigned long frameCountDiff = framecount - this->LastFrameCount; 
	double frameperiod = ((timestamp - this->LastTimeStamp)/ frameCountDiff);

	if ( this->AveragedFramePeriods.size() < numberOfAveragedPeriods ) 
	{
		if ( this->LastTimeStamp == 0 ) 
		{
			frameperiod = 0; 
			this->LastUnfilteredTimeStamp = timestamp;
			this->AveragedFramePeriods.clear(); 
			this->EstimatedFramePeriod = 0; 
		}
		else if ( frameCountDiff > 1 )
		{
			LOG_DEBUG("CreateTimeStampForFrame: frames lost (" << frameCountDiff - 1 << "), the previous frame period was ignored!"); 
			if ( AveragedFramePeriods.size() > 0 ) 
			{
				// we have some lost frames, ignore the previous frame period
				AveragedFramePeriods.pop_back(); 
			}
		}
		else
		{
			AveragedFramePeriods.push_back(frameperiod); 
		}

		double diffUnfilteredTimestamp = ((timestamp - this->LastUnfilteredTimeStamp)/ frameCountDiff);
		this->LastTimeStamp = timestamp;
		this->LastUnfilteredTimeStamp = timestamp; 
		this->LastFrameCount = framecount;
		filteredTimestamp = timestamp;

#ifdef PLUS_PRINT_VIDEO_TIMESTAMP_DEBUG_INFO 
		// FrameNumber  FrameNumberDifference	UnfilteredTimestamp	FilteredTimestamp	UnfilteredTimeDifference	SamplingPeriod	EstimatedFramePeriod
		this->DebugInfoStream << std::fixed << framecount << "\t" << frameCountDiff << "\t" << unfilteredTimestamp << "\t" << filteredTimestamp << "\t" 
			<< diffUnfilteredTimestamp << "\t" << frameperiod << "\t" << this->EstimatedFramePeriod << std::endl; 
		this->DebugInfoStream.flush(); 
#endif
		return; 
	}
	else if ( this->AveragedFramePeriods.size() == numberOfAveragedPeriods )
	{
		this->EstimatedFramePeriod = 0; 
		for ( int i = 0; i < numberOfAveragedPeriods; i++ )
		{
			this->EstimatedFramePeriod += this->AveragedFramePeriods[i] / numberOfAveragedPeriods; 
		}
		AveragedFramePeriods.push_back(this->EstimatedFramePeriod); 
	}

	double diffUnfilteredTimestamp = ((timestamp - this->LastUnfilteredTimeStamp)/ frameCountDiff);
	this->LastUnfilteredTimeStamp = timestamp; 

	this->LastTimeStamp += ((framecount - this->LastFrameCount)* this->EstimatedFramePeriod);
	double diffperiod = (timestamp - this->LastTimeStamp);

	if (diffperiod < -this->MaximumFramePeriodJitter || diffperiod > this->MaximumFramePeriodJitter )
	{ 
		// Frame delay is very large, most probably because the frame transfer 
		// was delayed, therefore the current time doesn't represent well the actual acquisition time.
		// Ignore this outlier and use the average delay instead.
		this->LastTimeStamp = timestamp;
		this->LastFrameCount = framecount;
		filteredTimestamp = timestamp; 

#ifdef PLUS_PRINT_VIDEO_TIMESTAMP_DEBUG_INFO 
		// FrameNumber	FrameNumberDifference	UnfilteredTimestamp	FilteredTimestamp	UnfilteredTimeDifference	FramePeriod	EstimatedFramePeriod	
		this->DebugInfoStream << std::fixed << framecount << "\t" << frameCountDiff << "\t" << unfilteredTimestamp << "\t" << filteredTimestamp << "\t" 
			<< diffUnfilteredTimestamp << "\t" << frameperiod << "\t" << this->EstimatedFramePeriod << std::endl; 
		this->DebugInfoStream.flush(); 
#endif

		return; 
	}

	this->EstimatedFramePeriod = this->EstimatedFramePeriod * (1-this->SmoothingFactor) + frameperiod * this->SmoothingFactor;
	this->LastFrameCount = framecount;
	filteredTimestamp = this->LastTimeStamp; 

#ifdef PLUS_PRINT_VIDEO_TIMESTAMP_DEBUG_INFO 
	// FrameNumber	FrameNumberDifference	UnfilteredTimestamp	FilteredTimestamp	UnfilteredTimeDifference	FramePeriod	EstimatedFramePeriod	
	this->DebugInfoStream << std::fixed << framecount << "\t" << frameCountDiff << "\t" << unfilteredTimestamp << "\t" << filteredTimestamp << "\t" 
		<< diffUnfilteredTimestamp << "\t" << frameperiod << "\t" << this->EstimatedFramePeriod << std::endl; 
	this->DebugInfoStream.flush(); 
#endif

	return;
}

//----------------------------------------------------------------------------
// side-effects- resets format to appropriate format and erases everything in buffer if
// format changes
void vtkVideoSource2::ReadFramesFromFile(vtkImageReader2 *reader, const char *summaryFileName, int fileType)
{
	this->Buffer->Lock();

	if (this->Recording || this->Playing)
	{
		this->Stop();
	}

	// set up the buffer
	this->Buffer->GetFrameFormat()->SetRowAlignment(1);
	this->Buffer->GetFrameFormat()->SetTopDown(0);

	// open the summary file
	ifstream inputStream;
	inputStream.open(summaryFileName);

	vtkImageData *image = vtkImageData::New();
	char line[200];
	char delims[] = " ";
	char *fileName = NULL;
	double timestamp;
	char *dummy = NULL;
	char *formatString = NULL;
	int format = VTK_LUMINANCE; // initialize to suppress compiler warnings
	int numComponents = 1; // initialize to suppress compiler warnings
	float opacity = 1; // initialize to suppress compiler warnings

	// get the format
	if (inputStream.is_open())
	{
		if (!inputStream.getline(line, 200).eof())
		{
			dummy = strtok(line, delims);
			formatString = strtok(NULL, delims);
			if (strcmp(formatString, "Luminance") == 0)
			{
				format = VTK_LUMINANCE;
				numComponents = 1;
			}
			else if (strcmp(formatString, "LuminanceAlpha") == 0)
			{
				format = VTK_LUMINANCE_ALPHA;
				numComponents = 2;
			}
			else if (strcmp(formatString, "RGB") == 0)
			{
				format = VTK_RGB;
				numComponents = 3;
			}
			else if (strcmp(formatString, "RGBA") == 0)
			{
				format = VTK_RGBA;
				numComponents = 4;
			}
			else
			{
				vtkErrorMacro(<< "ReadFramesFromFile: Unrecognized color format.");
			}
			// access the frame format directly instead of using vtkVideoSource2 functions
			// because if we are initialized already, we don't want to update the frame buffer
			// until we are finished setting everything.
			this->Buffer->GetFrameFormat()->SetPixelFormat((unsigned int)format);
			this->NumberOfScalarComponents = numComponents;
			this->Buffer->GetFrameFormat()->SetBitsPerPixel(numComponents*8);
			reader->SetNumberOfScalarComponents(numComponents);
		}

		// get the opacity
		if (!inputStream.getline(line, 200).eof())
		{
			dummy = strtok(line, delims);
			opacity = atof(strtok(NULL, delims));
			this->Buffer->GetFrameFormat()->SetOpacity(opacity);
		}

		// get the blank line
		inputStream.getline(line, 200);

		// get the data from all of the images (after updating the buffer according to
		// the extent of the first image)
		unsigned char *ptr;
		unsigned char *imgPtr;
		int firstImage = 1;

		while (!inputStream.getline(line, 200).eof())
		{
			timestamp = atof(strtok(line, delims));
			fileName = strtok(NULL, delims);

			reader->SetFileName(fileName);
			reader->UpdateWholeExtent();
			image = reader->GetOutput();

			// update the buffer if we are reading the first image
			if (firstImage)
			{
				this->Buffer->GetFrameFormat()->SetFrameSize(image->GetDimensions());
				this->Buffer->GetFrameFormat()->SetFrameExtent(image->GetExtent());
				this->UpdateFrameBuffer();
				this->FrameNumber = 0; 
				firstImage = 0;
			}

			// advance the buffer
			this->AdvanceFrameBuffer(1);
			if (this->FrameIndex + 1 < this->Buffer->GetBufferSize())
			{
				this->FrameIndex++;
			}

			// copy the data from the image to the frame
			ptr = reinterpret_cast<unsigned char*>(this->Buffer->GetFrame(0)->GetVoidPointer(0));
			imgPtr = reinterpret_cast<unsigned char *>(image->GetScalarPointer());
			this->CopyImageToFrame(ptr, imgPtr, this->Buffer->GetFrameFormat()->GetBytesInFrame(), this->GetOutputFormat(), image->GetDimensions(), opacity, fileType);

			// add the frame
			this->Buffer->AddItem(this->Buffer->GetFrame(0), timestamp, timestamp, ++this->FrameNumber);
			if (this->FrameCount++ == 0)
			{
				this->StartTimeStamp = this->Buffer->GetTimeStamp(0);
			}
			this->Modified();
		}
	}

	inputStream.close();
	image->Delete();

	this->Buffer->Unlock();
}

//----------------------------------------------------------------------------
void vtkVideoSource2::ReadFramesAsBMP(const char *summaryFileName)
{
	vtkBMPReader* reader = vtkBMPReader::New();
	reader->Allow8BitBMPOn();
	this->ReadFramesFromFile(reader, summaryFileName, FILETYPE_BMP);
	reader->Delete();
}

//----------------------------------------------------------------------------
/*void vtkVideoSource2::ReadFramesAsMINCImage(const char *summaryFileName)
{
vtkMINCImageReader* reader = vtkMINCImageReader::New();
this->ReadFramesFromFile(reader, summaryFileName, FILETYPE_MINC);
reader->Delete();
}*/

//----------------------------------------------------------------------------
void vtkVideoSource2::ReadFramesAsPNG(const char *summaryFileName)
{
	vtkPNGReader* reader = vtkPNGReader::New();
	this->ReadFramesFromFile(reader, summaryFileName, FILETYPE_PNG);
	reader->Delete();
}

//----------------------------------------------------------------------------
void vtkVideoSource2::ReadFramesAsTIFF(const char *summaryFileName)
{
	vtkTIFFReader* reader = vtkTIFFReader::New();
	this->ReadFramesFromFile(reader, summaryFileName, FILETYPE_TIFF);
	reader->Delete();
}

//-----------------------------------------------------------------------------
void vtkVideoSource2::WriteConfiguration(vtkXMLDataElement* config)
{

}

//-----------------------------------------------------------------------------
void vtkVideoSource2::ReadConfiguration(vtkXMLDataElement* config)
{
	LOG_TRACE("vtkVideoSource2::ReadConfiguration"); 
	if ( config == NULL )
	{
		LOG_ERROR("Unable to configure video source! (XML data element is NULL)"); 
		return; 
	}

	int frameSize[3] = {0, 0, 0}; 
	if ( config->GetVectorAttribute("FrameSize", 3, frameSize) )
	{
		this->SetFrameSize(frameSize[0], frameSize[1], frameSize[2]); 
	}

	int bufferSize = 0; 
	if ( config->GetScalarAttribute("BufferSize", bufferSize) )
	{
		this->GetBuffer()->SetBufferSize(bufferSize); 
	}

	int frameRate = 0; 
	if ( config->GetScalarAttribute("FrameRate", frameRate) )
	{
		this->SetFrameRate(frameRate); 
	}

	double smoothingFactor = 0; 
	if ( config->GetScalarAttribute("SmoothingFactor", smoothingFactor) )
	{
		this->SetSmoothingFactor(smoothingFactor); 
	}

	double localTimeOffset = 0; 
	if ( config->GetScalarAttribute("LocalTimeOffset", localTimeOffset) )
	{
		LOG_INFO("Image acqusition local time offset: " << std::fixed << 1000*localTimeOffset << "ms" ); 
		this->GetBuffer()->SetLocalTimeOffset(localTimeOffset); 
	}

}


//----------------------------------------------------------------------------
void vtkVideoSource2::GenerateVideoDataAcquisitionReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder)
{
#ifdef PLUS_PRINT_VIDEO_TIMESTAMP_DEBUG_INFO 
	if ( htmlReport == NULL || plotter == NULL )
	{
		LOG_ERROR("Caller should define HTML report generator and gnuplot plotter before report generation!"); 
		return; 
	}

	std::string reportFile = vtksys::SystemTools::GetCurrentWorkingDirectory() + std::string("/VideoBufferTimestamps.txt"); 

	if ( !vtksys::SystemTools::FileExists( reportFile.c_str(), true) )
	{
		LOG_ERROR("Unable to find video data acquisition report file at: " << reportFile); 
		return; 
	}

	std::string plotBufferTimestampScript = gnuplotScriptsFolder + std::string("/PlotBufferTimestamp.gnu"); 
	if ( !vtksys::SystemTools::FileExists( plotBufferTimestampScript.c_str(), true) )
	{
		LOG_ERROR("Unable to find gnuplot script at: " << plotBufferTimestampScript); 
		return; 
	}

	htmlReport->AddText("Video Data Acquisition Analysis", vtkHTMLGenerator::H1); 
	plotter->ClearArguments(); 
	plotter->AddArgument("-e");
	std::ostringstream videoBufferAnalysis; 
	videoBufferAnalysis << "f='" << reportFile << "'; o='VideoBufferTimestamps';" << std::ends; 
	plotter->AddArgument(videoBufferAnalysis.str().c_str()); 
	
	plotter->AddArgument(plotBufferTimestampScript.c_str());  
	plotter->Execute(); 
	plotter->ClearArguments(); 

	htmlReport->AddImage("VideoBufferTimestamps.jpg", "Video Data Acquisition Analysis"); 

	htmlReport->AddHorizontalLine(); 


#endif
}