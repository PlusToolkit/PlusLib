/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
  
  Program:   Heart Signal Box for VTK
  Module:    $RCSfile: vtkUSBECGBox.cxx,v $
  Creator:   Chris Wedlake <cwedlake@imaging.robarts.ca>
  Language:  C++
  Author:    $Author: dpace $
  Date:      $Date: 2009/03/30 14:18:39 $
  Version:   $Revision: 1.4 $

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
#include "vtkUSBECGBox.h"
#include "vtkMultiThreader.h"
#include "vtkMutexLock.h"
#include "vtkCriticalSection.h"
#include "vtkObjectFactory.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <io.h>

vtkUSBECGBox* vtkUSBECGBox::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkUSBECGBox");
  if(ret)
    {
    return (vtkUSBECGBox*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkUSBECGBox;
}

vtkUSBECGBox::vtkUSBECGBox()
{
  this->TotalPhases = 10;
  this->IsStarted=0;
  this->Signal = 0;
  this->Timestamp = 0;
  this->mode = 1;

  this->ECGRateBPM = -1;
  this->ECGPhase = -1;

  // for threaded capture of transformations
  this->Threader = vtkMultiThreader::New();
  this->ThreadId = -1;
  this->UpdateMutex = vtkCriticalSection::New();

  /* Variable Declarations */
  this->BoardNum = 0;
  this->ULStat = 0;
  this->Average = 1;

/* Declare UL Revision Level */                             
  float    RevLevel = (float)CURRENTREVNUM;
  this->ULStat = cbDeclareRevision(&RevLevel);

  cbErrHandling (PRINTALL, DONTSTOP);
  OPTIONS = CONVERTDATA + BACKGROUND;// + CONTINUOUS + SINGLEIO;

  this->channel[0]=0;
  this->channel[1]=0;

  this->SampleRate = 3125;
  this->Gain = 0;

  this->rise=false;
  this->inverted=true;
  this->Threshold = 7000;

  ADData = (WORD*)cbWinBufAlloc(sampleSize);
}

vtkUSBECGBox::~vtkUSBECGBox()
{
  if (this->ThreadId != -1)
    {
    this->Threader->TerminateThread(this->ThreadId);
    this->ThreadId = -1;
    }

  this->Threader->Delete();
  this->UpdateMutex->Delete();

}
  
void vtkUSBECGBox::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkObject::PrintSelf(os,indent);

}
  
static void *vtkUSBECGBoxThread(vtkMultiThreader::ThreadInfo *data)
{
  vtkUSBECGBox *self = (vtkUSBECGBox *)(data->UserData);


  for (int i = 0;; i++) {

    #ifdef _WIN32
      Sleep(1);
    #else
    #ifdef unix
    #ifdef linux
      usleep(5000);
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


void vtkUSBECGBox::Start()
{
  int Count=sampleSize;
  if (this->IsStarted)
    return;

  /* Collect the values with cbAInScan() in BACKGROUND mode
        Parameters:
             BoardNum    :the number used by CB.CFG to describe this board
             LowChan     :low channel of the scan
             HighChan    :high channel of the scan
             Count       :the total number of A/D samples to collect
             Rate        :sample rate in samples per second
             Gain        :the gain for the board
             ADData[]    :the array for the collected data values
             Options     :data collection options 
  */

    this->ULStat = cbAInScan (this->BoardNum, this->channel[0], this->channel[1], Count, &this->SampleRate, this->Gain, ADData, this->OPTIONS);
  if (this->ULStat > 0) {
    return;
  }
  this->UpdateMutex->Lock();

  if (this->ThreadId == -1){
    this->ThreadId = this->Threader->SpawnThread((vtkThreadFunctionType)&vtkUSBECGBoxThread,this);
  }
  this->UpdateMutex->Unlock();
  this->IsStarted=1;
}

void vtkUSBECGBox::Stop()
{

  if (this->IsStarted && this->ThreadId != -1)
    {
    this->Threader->TerminateThread(this->ThreadId);
    this->ThreadId = -1;

  this->ULStat = cbStopBackground (this->BoardNum,AIFUNCTION);
    }
  this->IsStarted=0;

}


void vtkUSBECGBox::Update()
{
  if (!this->IsStarted){
    vtkWarningMacro( << "called Update() before you started reading Signal.  Starting Signal Reading for you");
    this->Start();
    return;
  }

  int signal = GetECG();

  // cause board to flash
  /* If flashtime stared.
     if (ULStat==NOERRORS) {
         ULStat = cbFlashLED(BoardNum);
          disable flash flag; 
     }*/

}

int vtkUSBECGBox::GetECG(void) {

  short Status = RUNNING;
    long CurCount;
    long CurIndex;

   /* check the status of the current background operation
        Parameters:
            BoardNum  :the number used by CB.CFG to describe this board
            Status    :current status of the operation (IDLE or RUNNING)
            CurCount  :current number of samples collected
            CurIndex  :index to the last data value transferred 
            FunctionType: A/D operation (AIFUNCTIOM)
  */
  
  this->ULStat = cbGetStatus (BoardNum, &Status, &CurCount, &CurIndex, AIFUNCTION);

  if (this->ULStat > 0) {
    this->Stop();
    return -1;
  }

  if ( CurIndex <= 0 )
    return -1;
  if ((Status == RUNNING)) {
    this->Signal = unsigned int(ADData[CurIndex-1]);
    this->UpdateTimestamp();
  }

  if (CurCount >= sampleSize*0.97) {
    ULStat = cbStopBackground (BoardNum,AIFUNCTION);
    cbWinBufFree(ADData);
    ADData = (WORD*)cbWinBufAlloc(sampleSize);
    int Count=sampleSize;
    this->ULStat = cbAInScan (this->BoardNum, this->channel[0], this->channel[1], Count, &this->SampleRate, this->Gain, ADData, this->OPTIONS);
  }

  if (this->inverted == false) {
    if (this->Signal > this->Threshold && this->rise==false) {
      this->rise=true;
      this->CalculateECGRate();
    } else if (this->rise==true && this->Signal < this->Threshold) {
      this->rise=false;
      // mark of rise of fall?
    }
  } else {
    if (this->Signal < this->Threshold && this->rise==true) {
      this->rise=false;
      this->CalculateECGRate();
    } else if (this->rise==false && this->Signal > this->Threshold) {
      this->rise=true;
      // mark of rise of fall?
    }
  }

  if (this->ECGRateBPM > 0 && this->ECGRateBPM < 200){
    this->CalculatePhase();
  }
  return this->Signal;

}

void vtkUSBECGBox::SetInvertedOn() {
  this->inverted=true;
  this->rise=false;
}


void vtkUSBECGBox::SetInvertedOff() {
  this->inverted=false;
  this->rise=true;
}

void vtkUSBECGBox::InvertThreshold() {
  this->inverted = !this->inverted; 
  this->rise = !(this->rise);
}

void vtkUSBECGBox::CalculateECGRate() {
  if (this->Timestamp <= this->ExpectedSignalTimeStamp) {
    //this->ECGRateBPM = 0;
    //this->ECGPhase = -1;
    return;
  }

  ECGRateBPMArray.push_back(float((1/(this->Timestamp-this->StartSignalTimeStamp))*60));
  if (ECGRateBPMArray.size() > this->Average) {
    int BPM_Sum = 0;
    for (int i=1; i <= this->Average; i++) {
      BPM_Sum += ECGRateBPMArray[ECGRateBPMArray.size()-i];
    }
    this->ECGRateBPM = int(BPM_Sum/this->Average);
    if ((ECGRateBPMArray.size()+10) > this->Average) {
      ECGRateBPMArray.erase( ECGRateBPMArray.begin() );
    }
    double increase = ((this->Timestamp-this->StartSignalTimeStamp)*0.4) > 1 ? 1 : ((this->Timestamp-this->StartSignalTimeStamp)*0.4) < 0.3 ? 0.3 : ((this->Timestamp-this->StartSignalTimeStamp)*0.4);
    this->ExpectedSignalTimeStamp = this->Timestamp+increase;
    this->PrevStartSignalTimeStamp = this->StartSignalTimeStamp;
    this->StartSignalTimeStamp = this->Timestamp;
  }

}

void vtkUSBECGBox::CalculatePhase() {
  this->ECGPhase = int(((this->ECGRateBPM/60)*this->TotalPhases)*(this->Timestamp-this->StartSignalTimeStamp))%this->TotalPhases;
}

void vtkUSBECGBox::UpdateTimestamp() {
  struct _timeb timeTmp;
  _ftime(&timeTmp);  // seconds from Jan. 1, 1970
  double newTimeStamp = timeTmp.time + 0.001*timeTmp.millitm;
  if (newTimeStamp <= this->Timestamp) { this->Timestamp += 0.000001; }
  else                 { this->Timestamp = newTimeStamp; }
}

void vtkUSBECGBox::ShowSystemDevices() {
  int  BoardType, NumBoards, BaseAdr, i, j,NumBits;
    char BoardNameStr[BOARDNAMELEN];
  float    RevLevel = (float)CURRENTREVNUM;
  int NumDevs=0;

  cbGetConfig (GLOBALINFO, 0, 0, GINUMBOARDS, &NumBoards);

    for (i=0; i<NumBoards; i++)
        {
         /* Get board type of each board */
        cbGetConfig (BOARDINFO, i, 0, BIBOARDTYPE, &BoardType);

        /* If a board is installed */
        if (BoardType > 0)
            {
    /* Get the board's name */
            cbGetBoardName (i, BoardNameStr);
            printf ("    Board #%d = %s\n", i, BoardNameStr);

    /* Get the board's base address */
            cbGetConfig (BOARDINFO, i, 0, BIBASEADR, &BaseAdr);
            printf ("        Base Address = 0x%x\n",BaseAdr);

      cbGetConfig (BOARDINFO, BoardNum, 0, BINUMADCHANS, &NumDevs);
      printf ("        Number of A/D channels: %d \n", NumDevs);

      cbGetConfig (BOARDINFO, BoardNum, 0, BINUMDACHANS, &NumDevs);
      printf ("        Number of D/A channels: %d \n", NumDevs);

    /* get the number of digital devices for this board */
      cbGetConfig (BOARDINFO, BoardNum, 0, BIDINUMDEVS, &NumDevs);
      for (j=0; j<NumDevs; j++) {
        /* For each digital device, get the number of bits */
        cbGetConfig (DIGITALINFO, BoardNum, j, DINUMBITS, &NumBits);
        printf ("        Digital Device #%d : %d bits\n", j, NumBits);
      }

    /* Get the number of counter devices for this board */
      cbGetConfig (BOARDINFO, BoardNum, 0, BICINUMDEVS, &NumDevs);
      printf ("        Counter Devices : %d\n", NumDevs);

      int ExtBoardType, ADChan1, ADChan2;

      cbGetConfig (BOARDINFO, BoardNum, 0, BINUMEXPS, &NumDevs);
      for (j=0; j<NumDevs; ++j) {
        cbGetConfig (EXPANSIONINFO, BoardNum, j, XIBOARDTYPE, &ExtBoardType);
        cbGetConfig (EXPANSIONINFO, BoardNum, j, XIMUX_AD_CHAN1, &ADChan1);
      
        if (ExtBoardType==770) {
          /* it's a CIO-EXP32 occupying 2 A/D channels*/
          cbGetConfig (EXPANSIONINFO, BoardNum, j, XIMUX_AD_CHAN2, &ADChan2);
          printf ("        A/D channels #%d and #%d connected to EXP(devID=%d)\n", ADChan1, ADChan2, ExtBoardType);
        } else
          printf ("        A/D chan #%d connected to EXP(devID=%d)\n", ADChan1, ExtBoardType);
      }


            printf ("\n");
            }
        }
}
