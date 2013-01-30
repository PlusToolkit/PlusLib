/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
Authors include: Chris Wedlake <cwedlake@imaging.robarts.ca>, Danielle Pace
(Robarts Research Institute and The University of Western Ontario)
=========================================================================*/  

#include "vtkHeartSignalBox.h"
#include "vtkMultiThreader.h"
#include "vtkMutexLock.h"
#include "vtkObjectFactory.h"
#include "vtkRecursiveCriticalSection.h"
#include <float.h>
#include <io.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>

vtkStandardNewMacro(vtkHeartSignalBox);

//-------------------------------------------------------------------------
vtkHeartSignalBox::vtkHeartSignalBox()
{
  this->basePort = 0x0378; // crusader // 0x3020; // no clue // 0x3CD8; // desktop // 0x378;
  this->Channel1Pin = 0x01; //0x04 (REAL ECG) // 0x01 (HEART BOX)
  this->Channel2Pin = 0x04; //0x01 (REAL ECG) // 0x04 (HEART BOX)
}

//-------------------------------------------------------------------------
vtkHeartSignalBox::~vtkHeartSignalBox()
{
}

//-------------------------------------------------------------------------
void vtkHeartSignalBox::PrintSelf(ostream& os, vtkIndent indent)
{
  // vtkObject::PrintSelf(os,indent);
}

//-------------------------------------------------------------------------
short _stdcall Inp32(short PortAddress);
void _stdcall Out32(short PortAddress, short data);

//-------------------------------------------------------------------------
int vtkHeartSignalBox::GetECG(void) 
{
  int status_reg = 0;

  status_reg = Inp32(this->basePort+1); 
  this->UpdateTimestamp();

  int tmp = this->signalChannel1;
  this->signalChannel2 = (status_reg>>4) & this->Channel2Pin;
  this->signalChannel1 = (((status_reg>>4) & this->Channel1Pin));
  if (this->Channel1Pin == 0x01) 
  {
    this->signalChannel2>>=2;
  }
  else 
  {
    this->signalChannel1>>=2;
  }

  // update the beating rate and start signal time stamp if we are starting a new cycle
  if (tmp ==0 && this->signalChannel1) 
  {
    if (this->AudibleBeep)
    {
      std::cout << "\a";
    }
    this->CalculateECGRate(this->StartSignalTimeStamp, this->Timestamp);
    this->PrevStartSignalTimeStamp = this->StartSignalTimeStamp;
    this->StartSignalTimeStamp = this->Timestamp;
  }
  // the beating rate is invalid if if we are not starting a new cycle but we have
  // waited for more than two cycles without getting a new cycle
  else if ( (this->Timestamp - this->StartSignalTimeStamp) >= ((60*2)/this->ECGRateBPM) )
  {
    this->ECGRateBPM = -1.0;
  }

  // calculate the new ECG signal 
  int signal = (this->signalChannel2<<1) | this->signalChannel1;  //bit1: sign2, bit0: sign1

  // return the signal, and update the phase if the beating rate is valid
  if (this->ECGRateBPM < 0)
  {
    return signal;
  }
  else 
  {
    this->CalculatePhase(this->StartSignalTimeStamp, this->Timestamp);
    return signal;
  }
}

//-------------------------------------------------------------------------
int vtkHeartSignalBox::GetSignal1(void) 
{
  return this->signalChannel1;
}

//-------------------------------------------------------------------------
int vtkHeartSignalBox::GetSignal2(void) 
{
  return this->signalChannel2;
}

//-------------------------------------------------------------------------
void vtkHeartSignalBox::SwapSignals()
{
  int tmpSignal;
  tmpSignal = this->Channel1Pin;
  this->Channel1Pin = this->Channel2Pin;
  this->Channel2Pin = tmpSignal;
}

//-------------------------------------------------------------------------
void vtkHeartSignalBox::SetBasePort(int value)
{
  this->basePort =value;
}

//-------------------------------------------------------------------------
int vtkHeartSignalBox::GetBasePort()
{
  return this->basePort;
}

//-------------------------------------------------------------------------
static void *vtkHeartSignalBoxThread(vtkMultiThreader::ThreadInfo *data)
{

  vtkHeartSignalBox *self = (vtkHeartSignalBox *)(data->UserData);

  for (int i = 0;; i++) 
  {

#ifdef _WIN32
    Sleep(self->GetSleepInterval());
#else
#ifdef unix
#ifdef linux
    usleep(self->GetSleepInterval() * 1000);
#endif
#endif
#endif

    // query the hardware tracker
    self->UpdateMutex->Lock();
    self->Update();
    self->UpdateTime.Modified();
    self->UpdateMutex->Unlock();

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

//-------------------------------------------------------------------------
void vtkHeartSignalBox::Start()
{

  if (this->IsStarted)
  {
    return;
  }

  if (!this->IsInitialized)
  {
    this->Initialize();
  }

  // if initialization fails
  if (!this->IsInitialized)
  {
    return;
  }

  this->UpdateMutex->Lock();

  if (this->ThreadId == -1)
  {
    this->ThreadId = this->Threader->SpawnThread((vtkThreadFunctionType)&vtkHeartSignalBoxThread,this);
  }
  this->UpdateMutex->Unlock();

  this->IsStarted=1;
}
