/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================

  Program:   Signal Box for VTK
  Module:    $RCSfile: vtkSignalBox.h,v $ 
  Creator:   Chris Wedlake <cwedlake@imaging.robarts.ca>
             Danielle Pace <dpace@imaging.robarts.ca>
  Language:  C++
  Author:    $Author: dpace $
  Date:      $Date: 2009/05/19 03:59:56 $
  Version:   $Revision: 1.8 $
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
vBE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

=========================================================================*/
// .NAME vtkSignalBox - parent class for signal boxes

#ifndef __vtkSignalBox_h
#define __vtkSignalBox_h

#include "vtkObject.h"
#include "vtkCriticalSection.h"

class vtkMultiThreader;

class VTK_EXPORT vtkSignalBox : public vtkObject
{
public:
  static vtkSignalBox *New();
  vtkTypeMacro(vtkSignalBox, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Initialize the system
  virtual void Initialize();

  // Description:
  // Start the system - may be reimplemented in child classes
  virtual void Start();

  // Description:
  // Stop the system - may be reimplemented in child classes
  virtual void Stop();

  // Description:
  // Update by calling GetECG()
  virtual void Update();

//BTX
  // These are used by static functions in vtkSignalBox.cxx, and since
  // VTK doesn't generally use 'friend' functions they are public
  // instead of protected.  Do not use them anywhere except inside
  // vtkSignalBox.cxx.
  vtkCriticalSection *UpdateMutex;
  vtkTimeStamp UpdateTime;
  double InternalUpdateRate;  
//ETX

  // Description:
  // Get the current phase
  virtual int GetPhase() { return this->ECGPhase; }

  // Description:
  // Get the current beating rate (beats per minute)
  virtual float GetBPMRate() { return this->ECGRateBPM; }

  // Description:
  // Get the current timestamp
  vtkGetMacro(Timestamp,double);

  // Description:
  // Get the current ECG signal
  virtual int GetECGSignal() { return this->Signal; };

  // Description:
  // Set/Get the number of phases in one cycle
  virtual void SetNumberOfPhases(double phases) { this->TotalPhases=phases; }
  virtual int GetNumberOfPhases() { return this->TotalPhases; }

  // Description:
  // Set/Get the period at which the system updates the ECG signal (in seconds)
  vtkSetMacro(SleepInterval, double);
  vtkGetMacro(SleepInterval, double);

  // Description:
  // Beep on new cycle
  vtkSetMacro(AudibleBeep, int);
  vtkGetMacro(AudibleBeep, int);
  vtkBooleanMacro(AudibleBeep, int);

  // Description:
  // Get if we are started
  vtkGetMacro(IsStarted, int);

  // Description:
  // Period between signals, in seconds (default 1 second)
  vtkSetMacro(SignalPeriod,double);
  vtkGetMacro(SignalPeriod,double);

  // Description:
  // Get time of a phase for the previous cycle for retrospective gating
  virtual double CalculateRetrospectiveTimestamp(int phase);

protected:
  vtkSignalBox();
  ~vtkSignalBox();

  virtual void CalculateECGRate(double StartSignal, double current);
  virtual void CalculatePhase(double StartSignal, double current);
  virtual void UpdateTimestamp();

  // Description:
  // Get the current ECG signal, and update the ECG phase, beating rate,
  // and start signal time stamp as necessary, and return the current
  // ECG signal.  Should be reimplemented in child classes.
  virtual int GetECG(void);

  vtkMultiThreader *Threader;
  int ThreadId;

  int Signal; // current ECG signal
  int SignalPrev; // previous ECG signal
  double Timestamp; // current timestamp
  double StartSignalTimeStamp; // timestamp at the beginning of the current cycle
  double PrevStartSignalTimeStamp; // time stamp at the beginning of the previous cycle
  int TotalPhases; // number of phases per cycle
  float ECGPhase; // current phase
  float ECGRateBPM; // beating rate: beats per minute
  int IsStarted; // is system on?
  int IsInitialized;
  double SleepInterval; // in seconds
  int AudibleBeep; // beep on new cycle
  double SignalPeriod; // for the signal box, how many seconds should pass between each signal

private:
  vtkSignalBox(const vtkSignalBox&);
  void operator=(const vtkSignalBox&);  
  
};

#endif

