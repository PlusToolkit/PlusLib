/*=========================================================================

  Program:   Signal Box for VTK
  Module:    $RCSfile: vtkSignalBox.cxx,v $ 
  Creator:   Chris Wedlake <cwedlake@imaging.robarts.ca>
             Danielle Pace <dpace@imaging.robarts.ca>
  Language:  C++
  Author:    $Author: dpace $
  Date:      $Date: 2009/05/19 03:59:56 $
  Version:   $Revision: 1.5 $
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
// Parent class for signal boxes - children should implement
// * GetECG()
// * Initialize()
// * Start() (at least to call the new vtkSignalBoxThread)
// * Stop()
// * vtkSignalBoxThread (to cast self to the child type)
// (see vtkHeartSignalBox for an example)
// This class returns a signal every this->SignalPeriod seconds

#include "PlusConfigure.h"

#include "vtkSignalBox.h"
#include "vtkObjectFactory.h"

#include "vtkMultiThreader.h"
#include "vtkMutexLock.h"
#include <sys/timeb.h>

////-------------------------------------------------------------------------
vtkSignalBox* vtkSignalBox::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkSignalBox");
  if(ret)
    {
    return (vtkSignalBox*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkSignalBox;
}

//-------------------------------------------------------------------------
vtkSignalBox::vtkSignalBox()
{

  this->Signal = 0;
  this->SignalPrev = 0;
  this->Timestamp = 0;
  this->TotalPhases = 10;
  this->ECGPhase = -1;
  this->ECGRateBPM = -1;
  this->IsStarted=0;
  this->IsInitialized = 0;
  this->SleepInterval = 5;
  this->AudibleBeep = 0;
  this->SignalPeriod = 1.0;

  // for threaded capture of transformations
  this->Threader = vtkMultiThreader::New();
  this->ThreadId = -1;
  this->UpdateMutex = vtkCriticalSection::New();
}

//-------------------------------------------------------------------------
vtkSignalBox::~vtkSignalBox()
{
  if (this->IsStarted)
    {
    this->Stop();
    }
  else
    {
    if (this->ThreadId != -1)
      {
      this->Threader->TerminateThread(this->ThreadId);
      this->ThreadId = -1;
      }
    }

  this->Threader->Delete();
  this->UpdateMutex->Delete();
}

//-------------------------------------------------------------------------
void vtkSignalBox::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "TotalPhases: " << this->TotalPhases << "\n";
  os << indent << "IsInitialized: " << (this->IsInitialized ? "On\n":"Off\n");
  os << indent << "IsStarted: " << (this->IsStarted ? "On\n":"Off\n");
  os << indent << "SleepInterval: " << this->SleepInterval << "\n";
  os << indent << "AudibleBeep: " << this->AudibleBeep << "\n";
  os << indent << "SignalPeriod: " << this->SignalPeriod << "\n";
}
  
//-------------------------------------------------------------------------
static void *vtkSignalBoxThread(vtkMultiThreader::ThreadInfo *data)
{

  vtkSignalBox *self = (vtkSignalBox *)(data->UserData);

  for (int i = 0;; i++) {
	  
	vtkAccurateTimer::Delay(self->GetSleepInterval() * 0.001); 

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
void vtkSignalBox::Initialize()
  {
  this->IsInitialized = 1;
  }

//-------------------------------------------------------------------------
void vtkSignalBox::Start()
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

  if (this->ThreadId == -1){
    this->ThreadId = this->Threader->SpawnThread((vtkThreadFunctionType)&vtkSignalBoxThread,this);
  }
  this->UpdateMutex->Unlock();

  this->IsStarted=1;
}

//-------------------------------------------------------------------------
void vtkSignalBox::Stop()
{

  if (this->IsStarted && this->ThreadId != -1)
    {
    this->Threader->TerminateThread(this->ThreadId);
    this->ThreadId = -1;
    }
  this->IsStarted=0;
  this->IsInitialized = 0;
}

//-------------------------------------------------------------------------
void vtkSignalBox::Update()
{
  if (!this->IsStarted){
    LOG_WARNING("called Update() before you started reading Signal.  Starting Signal Reading for you");
    this->Start();
    return;
  }

  int signal = this->GetECG();

  this->SignalPrev = this->Signal;
  this->Signal = signal;
}

//-------------------------------------------------------------------------
// Get the current ECG signal, and update the ECG phase, beating rate,
// and start signal time stamp as necessary, and return the current
// ECG signal.  Should be reimplemented in child classes.
int vtkSignalBox::GetECG(void)
  {
  this->UpdateTimestamp();
  int signal;

  // update the beating rate and start signal time stamp if we are starting a new cycle
  if (this->Timestamp - this->StartSignalTimeStamp >= this->SignalPeriod)
    {
    if (this->AudibleBeep)
      {
      std::cout << "\a";
      }
    this->CalculateECGRate(this->StartSignalTimeStamp, this->Timestamp);
  this->PrevStartSignalTimeStamp = this->StartSignalTimeStamp;
    this->StartSignalTimeStamp = this->Timestamp;
    signal = 1;
    }
  // the beating rate is invalid if if we are not starting a new cycle but we have
  // waited for more than two cycles without getting a new cycle
  else if ( (this->Timestamp - this->StartSignalTimeStamp) >= ((60*2)/this->ECGRateBPM) ){
    this->ECGRateBPM = -1.0;
    signal = 0;
  }
  else
    {
    signal = 0;
    }

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
void vtkSignalBox::CalculateECGRate(double StartSignal, double current) {
  this->ECGRateBPM = float((1.0/(current-StartSignal))*60.0);
}

//-------------------------------------------------------------------------
void vtkSignalBox::CalculatePhase(double StartSignal, double current) {
  this->ECGPhase = int(((this->ECGRateBPM/60)*this->TotalPhases)*(current-StartSignal))%this->TotalPhases;
}

//-------------------------------------------------------------------------
double vtkSignalBox::CalculateRetrospectiveTimestamp(int phase) {
  // ignore illegal phases
  if (phase >= this->TotalPhases)
  {
  return -1;
  }

  return (this->PrevStartSignalTimeStamp + ((this->StartSignalTimeStamp - this->PrevStartSignalTimeStamp) / (double)this->TotalPhases * (double)phase));
}

//-------------------------------------------------------------------------
void vtkSignalBox::UpdateTimestamp() {
  struct _timeb timeTmp;
  _ftime(&timeTmp);  // seconds from Jan. 1, 1970
  this->Timestamp = timeTmp.time + 0.001*timeTmp.millitm;
}
