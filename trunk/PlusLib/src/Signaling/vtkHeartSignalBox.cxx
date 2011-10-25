/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================

  Program:   Heart Signal Box for VTK
  Module:    $RCSfile: vtkHeartSignalBox.cxx,v $
  Creator:   Chris Wedlake <cwedlake@imaging.robarts.ca>
  Language:  C++
  Author:    $Author: dpace $
  Date:      $Date: 2009/03/30 21:52:43 $
  Author:    $Author: dpace $
  Date:      $Date: 2009/03/30 21:52:43 $
  Version:   $Revision: 1.6 $
             Rearranged September 10, 2008 by dpace to have all signal
             boxes inherit from a parent class

==========================================================================

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

#include <limits.h>
#include <float.h>
#include <math.h>
#include "vtkHeartSignalBox.h"
#include "vtkObjectFactory.h"
#include "vtkMultiThreader.h"
#include "vtkMutexLock.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <io.h>

//-------------------------------------------------------------------------
vtkHeartSignalBox* vtkHeartSignalBox::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkHeartSignalBox");
  if(ret)
    {
    return (vtkHeartSignalBox*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkHeartSignalBox;
}

//-------------------------------------------------------------------------
vtkHeartSignalBox::vtkHeartSignalBox()
{
  this->basePort = 0x0378; // crusader // 0x3020; // no clue // 0x3CD8; // desktop // 0x378;
  this->Channel1Pin = 0x01; //0x04 (REAL ECG) // 0x01 (HEART BOX)
  this->Channel2Pin = 0x04; //0x01 (REAL ECG) // 0x04 (HEART BOX)
}

//-------------------------------------------------------------------------
// everything is done in the constructor
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
int vtkHeartSignalBox::GetECG(void) {
  int status_reg = 0;

  status_reg = Inp32(this->basePort+1); 
  this->UpdateTimestamp();
  
  int tmp = this->signalChannel1;
  this->signalChannel2 = (status_reg>>4) & this->Channel2Pin;
  this->signalChannel1 = (((status_reg>>4) & this->Channel1Pin));
  if (this->Channel1Pin == 0x01) {
    this->signalChannel2>>=2;
  }
  else {
    this->signalChannel1>>=2;
  }

  // update the beating rate and start signal time stamp if we are starting a new cycle
  if (tmp ==0 && this->signalChannel1) {
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
  else if ( (this->Timestamp - this->StartSignalTimeStamp) >= ((60*2)/this->ECGRateBPM) ){
    this->ECGRateBPM = -1.0;
  }

  // calculate the new ECG signal 
  int signal = (this->signalChannel2<<1) | this->signalChannel1;  //bit1: sign2, bit0: sign1

  // return the signal, and update the phase if the beating rate is valid
  if (this->ECGRateBPM < 0){
    return signal;
  }
  else {
    this->CalculatePhase(this->StartSignalTimeStamp, this->Timestamp);
    return signal;
  }
}

//-------------------------------------------------------------------------
int vtkHeartSignalBox::GetSignal1(void) {
  return this->signalChannel1;
}

//-------------------------------------------------------------------------
int vtkHeartSignalBox::GetSignal2(void) {
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

  for (int i = 0;; i++) {

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

    if (activeFlag == 0) {
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
