/*=========================================================================

  File: vtkEpiphanVideoSource.cxx
  Author: Chris Wedlake <cwedlake@robarts.ca>

  Language: C++
  Description: 
     
=========================================================================

  Copyright (c) Chris Wedlake, cwedlake@robarts.ca

  Use, modification and redistribution of the software, in source or
  binary forms, are permitted provided that the following terms and
  conditions are met:

  1) Redistribution of the source code, in verbatim or modified
  form, must retain the above copyright notice, this license,
  the following disclaimer, and any notices that refer to this
  license and/or the following disclaimer.  

  2) Redistribution in binary form must include the above copyright
  notice, a copy of this license and the following disclaimer
  in the documentation or with other materials provided with the
  distribution.

  3) Modified copies of the source code must be clearly marked as such,
  and must not be misrepresented as verbatim copies of the source code.

  THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
  WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
  MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
  OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
  THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGES.


  =========================================================================*/

#include "vtkEpiphanVideoSource.h"
#include "vtkTimerLog.h"
#include "vtkObjectFactory.h"
#include "vtkCriticalSection.h"
#include "vtkUnsignedCharArray.h"
#include "vtkMutexLock.h"

#include <vtkstd/string> 

vtkEpiphanVideoSource* vtkEpiphanVideoSource::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkEpiphanVideoSource");
  if(ret)
    {
    return (vtkEpiphanVideoSource*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkEpiphanVideoSource;
}

//----------------------------------------------------------------------------
vtkEpiphanVideoSource::vtkEpiphanVideoSource()
{

  this->Initialized = 0;
  this->pauseFeed = 0;
  this->status = V2U_GRABFRAME_STATUS_OK;
  this->fg = NULL;
  this->cropRect = new V2URect;

  this->FrameBufferBitsPerPixel = 24;
  this->vtkVideoSource::SetOutputFormat(VTK_RGB);
  this->vtkVideoSource::SetFrameBufferSize( 100 );
  this->vtkVideoSource::SetFrameRate( 25.0f );

  for (unsigned int i =0; i < 15; i++){
	  this->serialNumber[i] ='\0';
  }
  
}

//----------------------------------------------------------------------------
vtkEpiphanVideoSource::~vtkEpiphanVideoSource()
{
  this->vtkEpiphanVideoSource::ReleaseSystemResources();
  if (this->fg) {
	  FrmGrab_Deinit(); // not sure if this stops all devices??
  }
}  

//----------------------------------------------------------------------------
void vtkEpiphanVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent); 
  os << indent << "SerialNumber: "<< this->serialNumber << "\n";
  os << indent << "Status: "<< this->status << "\n";
  os << indent << "Grabber: " << this->fg << "\n";
}

//----------------------------------------------------------------------------
void vtkEpiphanVideoSource::Initialize()
{
  if (this->Initialized) 
  {
    return;
  }
  
  FrmGrabNet_Init();

  char input[15];
  strncpy_s(input, "sn:", 15);
  strncat_s(input, this->serialNumber, 15);

  strncpy_s(this->serialNumber,15,"V2U109999",15);

  this->fg = FrmGrab_Open(this->serialNumber);
  
  //if (!this->fg) {
//	this->fg = FrmGrabLocal_Open();
//	vtkErrorMacro(<<"Epiphan Device with set serial number not found, looking for any available device instead"<<this->serialNumber);
  //}

  if (this->fg == NULL) {
	  vtkErrorMacro(<<"Epiphan Device Not found");
	  return;
  }

  V2U_VideoMode vm;
  if (FrmGrab_DetectVideoMode(this->fg,&vm) && vm.width && vm.height) {
	  this->SetFrameSize(vm.width,vm.height,1);
	  //this->SetFrameRate((vm.vfreq+50)/1000);
  } else {
	vtkErrorMacro(<<"No signal detected");
  }

  FrmGrab_SetMaxFps(this->fg, 25.0);

  // Initialization worked
  this->Initialized = 1;
  
  // Update frame buffer  to reflect any changes
  this->UpdateFrameBuffer();
}  

//----------------------------------------------------------------------------
void vtkEpiphanVideoSource::ReleaseSystemResources()
{
  this->Initialized = 0;
  if (this->fg != NULL) {
	FrmGrab_Close(this->fg);
  }
}

void vtkEpiphanVideoSource::InternalGrab()
{

  // get a thread lock on the frame buffer
  this->FrameBufferMutex->Lock();

  // Get pointer to data from the network source
  
  this->cropRect->x = this->FrameBufferExtent[0];
  this->cropRect->width = this->FrameBufferExtent[1]-this->FrameBufferExtent[0]+1;
  this->cropRect->y = this->FrameBufferExtent[2];
  this->cropRect->height = this->FrameBufferExtent[3]-this->FrameBufferExtent[2]+1;
  
  //imgu *IA;
  V2U_GrabFrame2 * frame = NULL;

  V2U_UINT32 format = V2U_GRABFRAME_BOTTOM_UP_FLAG; // seems to be needed to orientate correctly.

  if (this->OutputFormat == VTK_LUMINANCE) {
	format |= V2U_GRABFRAME_FORMAT_YUY2;
  } else if (this->OutputFormat == VTK_RGB) {
	format |= V2U_GRABFRAME_FORMAT_RGB24;
  } else if (this->OutputFormat == VTK_RGBA) {
	format |= V2U_GRABFRAME_FORMAT_ARGB32;
  } else {
	  // no clue what format to grab, you can add more.
	  return;
  }

  frame= FrmGrab_Frame(this->fg, format, cropRect);

  if (frame == NULL || frame->imagelen <= 0) {
	  this->FrameBufferMutex->Unlock();
	  this->Stop();
	  return;
  }
  if (frame->retcode != V2UERROR_OK) { 
	  cout << "Error: " << frame->retcode << endl;
	  this->FrameBufferMutex->Unlock();
	  this->Stop();
	  return;
  } 


  if (this->AutoAdvance)
    {
    this->AdvanceFrameBuffer(1);
    if (this->FrameIndex + 1 < this->FrameBufferSize)
      {
      this->FrameIndex++;
      }
    }

  int index = this->FrameBufferIndex % this->FrameBufferSize;
  while (index < 0)
    {
    index += this->FrameBufferSize;
    }
  
  char *buffer = (char *)frame->pixbuf;
  
  // Get a pointer to the location of the frame buffer
  char *ptr = (char *) reinterpret_cast<vtkUnsignedCharArray *>(this->FrameBuffer[index])->GetPointer(0);
  
  // Copy image into frame buffer
  memcpy(ptr, buffer, frame->imagelen);

  FrmGrab_Release(this->fg, frame);
  this->FrameBufferTimeStamps[index] = vtkTimerLog::GetUniversalTime();

  if (this->FrameCount++ == 0)
    {
    this->StartTimeStamp = this->FrameBufferTimeStamps[index];
    }

  this->Modified();

  this->FrameBufferMutex->Unlock();
}

void vtkEpiphanVideoSource::SetSerialNumber(char * serial) {
	strncpy_s(this->serialNumber, serial, 15);
}

//----------------------------------------------------------------------------
// platform-independent sleep function
static inline void vtkSleep(double duration)
{
  duration = duration; // avoid warnings
  // sleep according to OS preference
#ifdef _WIN32
  Sleep((int)(1000*duration));
#elif defined(__FreeBSD__) || defined(__linux__) || defined(sgi)
  struct timespec sleep_time, dummy;
  sleep_time.tv_sec = (int)duration;
  sleep_time.tv_nsec = (int)(1000000000*(duration-sleep_time.tv_sec));
  nanosleep(&sleep_time,&dummy);
#endif
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
    double remaining = time - vtkTimerLog::GetUniversalTime();

    // check to see if we have reached the specified time
    if (remaining <= 0)
      {
      if (i == 0)
        {
        vtkGenericWarningMacro("Dropped a video frame.");
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

    vtkSleep(remaining);
    }

  return 0;
}

//----------------------------------------------------------------------------
// this function runs in an alternate thread to asyncronously grab frames
static void *vtkEpiphanVideoSourceRecordThread(vtkMultiThreader::ThreadInfo *data)
{
  vtkEpiphanVideoSource *self = (vtkEpiphanVideoSource *)(data->UserData);
  
  double startTime = vtkTimerLog::GetUniversalTime();
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
void vtkEpiphanVideoSource::Record()
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
	this->pauseFeed = 0;
    this->Modified();
    this->PlayerThreadId = 
      this->PlayerThreader->SpawnThread((vtkThreadFunctionType)\
                                &vtkEpiphanVideoSourceRecordThread,this);
    }
}

//----------------------------------------------------------------------------
// this function runs in an alternate thread to 'play the tape' at the
// specified frame rate.
static void *vtkEpiphanVideoSourcePlayThread(vtkMultiThreader::ThreadInfo *data)
{
  vtkVideoSource *self = (vtkVideoSource *)(data->UserData);
 
  double startTime = vtkTimerLog::GetUniversalTime();
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
void vtkEpiphanVideoSource::Play()
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
                                        &vtkEpiphanVideoSourcePlayThread,this);
    }
}

//----------------------------------------------------------------------------
// Stop continuous grabbing or playback.  You will have to override this
// if your class overrides Play() and Record()
void vtkEpiphanVideoSource::Stop()
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

void vtkEpiphanVideoSource::Pause() {
	this->pauseFeed = 1;
}
void vtkEpiphanVideoSource::UnPause() {
	this->pauseFeed = 0;
}

void vtkEpiphanVideoSource::SetFrameRate(float rate) {
	vtkVideoSource::SetFrameRate(rate);
	if (this->fg) {
		FrmGrab_SetMaxFps(this->fg, rate);
	}
}

void vtkEpiphanVideoSource::SetOutputFormat(int format)
{
  if (format == this->OutputFormat)
    {
    return;
    }

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
	case VTK_LUMINANCE:
	  numComponents = 2;
	  break;
    default:
      vtkErrorMacro(<< "SetOutputFormat: Unrecognized color format.");
      return;
    }

  this->OutputFormat = format;

  this->NumberOfScalarComponents = numComponents;
  if (this->FrameBufferBitsPerPixel != numComponents*8)
    {
    this->FrameBufferMutex->Lock();
    this->FrameBufferBitsPerPixel = numComponents*8;
    if (this->Initialized)
      {
      this->UpdateFrameBuffer();
      }
    this->FrameBufferMutex->Unlock();
    }

  this->Modified();
}

//----------------------------------------------------------------------------
void vtkEpiphanVideoSource::SetClipRegion(int x0, int x1, int y0, int y1, int z0, int z1)
{
  x0 = int(x0/4)*4;
  x1 = (int((x1+1)/4)*4)-1;
  y0 = int(y0/4)*4;
  y1 = (int((y1+1)/4)*4)-1;

  // Factor of 4 for clipping extents... NO clue why but device defaults.

  int difference = ((x1-x0)*(y1-y0));
  if ((difference > -200 ) && (difference < 200)) {
	vtkErrorMacro(<<"Epiphan Device must have a size of minimum 200 pixels");
	return;
  }

  vtkVideoSource::SetClipRegion(x0,x1,y0,y1,z0,z1);
}