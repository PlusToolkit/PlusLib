/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
Authors include: Chris Wedlake <cwedlake@imaging.robarts.ca>, Danielle Pace
(Robarts Research Institute and The University of Western Ontario)
=========================================================================*/  

#include "PlusConfigure.h"

#include "vtkSignalBox.h"
#include "vtkObjectFactory.h"

#include "vtkMultiThreader.h"
#include "vtkMutexLock.h"
#include <sys/timeb.h>

vtkStandardNewMacro(vtkSignalBox);

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
  this->UpdateMutex = vtkRecursiveCriticalSection::New();
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

  for (int i = 0;; i++) 
  {
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

  if (this->ThreadId == -1)
  {
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
  if (!this->IsStarted)
  {
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
  else if ( (this->Timestamp - this->StartSignalTimeStamp) >= ((60*2)/this->ECGRateBPM) )
  {
    this->ECGRateBPM = -1.0;
    signal = 0;
  }
  else
  {
    signal = 0;
  }

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
void vtkSignalBox::CalculateECGRate(double StartSignal, double current) 
{
  this->ECGRateBPM = float((1.0/(current-StartSignal))*60.0);
}

//-------------------------------------------------------------------------
void vtkSignalBox::CalculatePhase(double StartSignal, double current) 
{
  this->ECGPhase = int(((this->ECGRateBPM/60)*this->TotalPhases)*(current-StartSignal))%this->TotalPhases;
}

//-------------------------------------------------------------------------
double vtkSignalBox::CalculateRetrospectiveTimestamp(int phase) 
{
  // ignore illegal phases
  if (phase >= this->TotalPhases)
  {
    return -1;
  }

  return (this->PrevStartSignalTimeStamp + ((this->StartSignalTimeStamp - this->PrevStartSignalTimeStamp) / (double)this->TotalPhases * (double)phase));
}

//-------------------------------------------------------------------------
void vtkSignalBox::UpdateTimestamp() 
{
/* TODO: update this
  struct _timeb timeTmp;
  _ftime(&timeTmp);  // seconds from Jan. 1, 1970
  this->Timestamp = timeTmp.time + 0.001*timeTmp.millitm;
  */
}
