/*=========================================================================

  Program:   Haptics for VTK
  Module:    $RCSfile: vtkHapticsDevice.cxx,v $
  Creator:   Chris Wedlake <cwedlake@imaging.robarts.ca>
  Language:  C++
  Author:    $Author: cwedlake $
  Date:      $Date: 2007/04/19 12:48:52 $
  Version:   $Revision: 1.1 $

==========================================================================

Copyright (c) 2000-2005

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
//----------------------------------------------------------------------------

#include "vtkHapticsDevice.h"
#include "vtkTimerLog.h"

vtkHapticsDevice* vtkHapticsDevice::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkHapticsDevice");
  if(ret)
    {
    return (vtkHapticsDevice*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkHapticsDevice;
}

//----------------------------------------------------------------------------
vtkHapticsDevice::vtkHapticsDevice()
{

  this->LastUpdateTime = 0;
  this->Initialized = 0;
  this->Transform = vtkTransform::New();
  this->Position = vtkMatrix4x4::New();
  this->Transform->SetMatrix(this->Position);
  this->Position = this->Transform->GetMatrix();

  // for threaded capture of transformations
  this->Threader = vtkMultiThreader::New();
  this->ThreadId = -1;
  this->UpdateMutex = vtkCriticalSection::New();
  this->RequestUpdateMutex = vtkCriticalSection::New();
  this->forceModel = vtkHapticForce::New();

}

//----------------------------------------------------------------------------
vtkHapticsDevice::~vtkHapticsDevice()
{
  // The thread should have been stopped before the
  // subclass destructor was called, but just in case
  // se stop it here.
  if (this->ThreadId != -1)
      {
    this->Threader->TerminateThread(this->ThreadId);
    this->ThreadId = -1;
    }

  this->Threader->Delete();
  this->UpdateMutex->Delete();
}


//----------------------------------------------------------------------------
// this thread is run whenever the tracker is tracking
static void *vtkHapticsThread(vtkMultiThreader::ThreadInfo *data)
{
  vtkHapticsDevice *self = (vtkHapticsDevice *)(data->UserData);

  double currtime[10];

  // loop until canceled
  for (int i = 0;; i++)
    {
    // get current rate over last 10 updates
    double newtime = vtkTimerLog::GetUniversalTime();
    double difftime = newtime - currtime[i%10];
    currtime[i%10] = newtime;
    if (i > 10 && difftime != 0)
      {
      self->InternalUpdateRate = (10.0/difftime);
      }

    // query the hardware device
    self->UpdateMutex->Lock();
    self->InternalUpdate();
    self->UpdateTime.Modified();
    self->UpdateMutex->Unlock();

    // check to see if main thread wants to lock the UpdateMutex
    self->RequestUpdateMutex->Lock();
    self->RequestUpdateMutex->Unlock();
    
    // check to see if we are being told to quit 
    data->ActiveFlagLock->Lock();
    int activeFlag = *(data->ActiveFlag);
    data->ActiveFlagLock->Unlock();

    if (activeFlag == 0)
      {
      return NULL;
      }
    }
}


//----------------------------------------------------------------------------
void vtkHapticsDevice::StartDevice()
{
  int initialized = this->Initialized;

  this->Initialized = this->InternalStartDevice();

  // start the tracking thread
  if (!(this->Initialized && !initialized && this->ThreadId == -1))
    {
    return;
    }

  // this will block the tracking thread until we're ready
  this->UpdateMutex->Lock();

  // start the tracking thread
  this->ThreadId = this->Threader->SpawnThread((vtkThreadFunctionType)\
                                               &vtkHapticsThread,this);
  this->LastUpdateTime = this->UpdateTime.GetMTime();

  // allow the tracking thread to proceed
  this->UpdateMutex->Unlock();

  // wait until the first update has occurred before returning
  int timechanged = 0;
  while (!timechanged)
    {
    this->RequestUpdateMutex->Lock();
    this->UpdateMutex->Lock();
    this->RequestUpdateMutex->Unlock();
    timechanged = (this->LastUpdateTime != this->UpdateTime.GetMTime());
    this->UpdateMutex->Unlock();
#ifdef _WIN32
    Sleep((int)(100));
#elif defined(__FreeBSD__) || defined(__linux__) || defined(sgi) || defined(__APPLE__)
    struct timespec sleep_time, dummy;
    sleep_time.tv_sec = 0;
    sleep_time.tv_nsec = 100000000;
    nanosleep(&sleep_time,&dummy);
#endif
    }
}

//----------------------------------------------------------------------------
void vtkHapticsDevice::StopDevice()
{
  if (this->Initialized && this->ThreadId != -1)
    {
    this->Threader->TerminateThread(this->ThreadId);
    this->ThreadId = -1;
    }



  this->InternalStopDevice();
  this->Initialized = 0;
}

//----------------------------------------------------------------------------
void vtkHapticsDevice::Update()
{
  if (!this->Initialized)
    { 
    return; 
    }

  this->LastUpdateTime = this->UpdateTime.GetMTime();
}


void vtkHapticsDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  os << indent << "Update Mutex: "  <<endl;
  this->UpdateMutex->PrintSelf(os,indent.GetNextIndent());
  os << indent << "Request Update Mutex: " <<endl;
  this->RequestUpdateMutex->PrintSelf(os,indent.GetNextIndent());
  os << indent << "Position: " <<endl;
  this->Position->PrintSelf(os,indent.GetNextIndent());
  os << indent << "Initialized: " <<  this->Initialized << endl;
  os << indent << "Last Update Time: " <<  this->LastUpdateTime << endl;
  this->Threader->PrintSelf(os,indent.GetNextIndent());
}

