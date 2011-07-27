/*=========================================================================

  Program:   USB Data ECG Box for VTK
  Module:    $RCSfile: vtkUSBECGBox.h,v $
  Creator:   Chris Wedlake <cwedlake@imaging.robarts.ca>
  Language:  C++
  Author:    $Author: dpace $
  Date:      $Date: 2009/03/30 14:18:39 $
  Version:   $Revision: 1.3 $

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
// .NAME vtkUSBECGBox - interfaces VTK with real-time ECG signal

#ifndef __vtkUSBECGBox_h
#define __vtkUSBECGBox_h

#include "vtkObject.h"
#include "vtkCriticalSection.h"

#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkSignalBox.h"

class vtkDoubleArray;
class vtkIntArray;
class vtkMultiThreader;

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include ".\cbw\cbw.h"
#include <vector>

class VTK_EXPORT vtkUSBECGBox : public vtkSignalBox
{
public:
  static vtkUSBECGBox *New();
  vtkTypeMacro(vtkUSBECGBox,vtkSignalBox);
  void PrintSelf(ostream& os, vtkIndent indent);

//  int GetRawECG(void);
//  int GetRawPressure(void);

  // Description:
  // Start the tracking system.  The tracking system is brought from
  // its ground state (i.e. on but not necessarily initialized) into
  // full tracking mode.  This method calls InternalStartTracking()
  // after doing a bit of housekeeping.
  void Start();

  // Description:
  // Stop the tracking system and bring it back to its ground state.
  // This method calls InternalStopTracking().
  void Stop();

  void Update();

//BTX
  // These are used by static functions in vtkUSBECGBox.cxx, and since
  // VTK doesn't generally use 'friend' functions they are public
  // instead of protected.  Do not use them anywhere except inside
  // vtkUSBECGBox.cxx.
  //vtkCriticalSection *UpdateMutex;
 // vtkTimeStamp UpdateTime;
  //double InternalUpdateRate;  
//ETX

  void SetModeToBasic() { this->mode=0; }
  void SetModeToAdvanced() { this->mode=1; }

  //int GetPhase() { return this->ECGPhase; }
  //float GetBPMRate() { return this->ECGRateBPM; }
  //vtkGetMacro(Timestamp,double);
  //void SetNumberOfPhases(double phases) { this->TotalPhases=phases; }
  //int GetNumberOfPhases() { return this->TotalPhases; }
  // Get the ECG Signal
  //int GetECGSignal() { return this->Signal; };

  void ShowSystemDevices();

  void SetInvertedOn();
  void SetInvertedOff();
  void InvertThreshold();
  unsigned int GetThreshold() { return this->Threshold; }
  void SetThreshold(unsigned int value) { this->Threshold=value; }

protected:
  vtkUSBECGBox();
  ~vtkUSBECGBox();

  void UpdateTimestamp();
  void CalculateECGRate();
  void CalculatePhase();

  int GetECG(void);
  vtkMultiThreader *Threader;
  int ThreadId;

//BTX
//  int Signal;  //ECG signal
//  double Timestamp;
//  double StartSignalTimeStamp;
  double ExpectedSignalTimeStamp;

  // BHP cardiac phase
//  int TotalPhases;
//  float ECGPhase;
//  float ECGRateBPM; // beating rate: beats per minute

//  int IsStarted;
  int mode;
  int BoardNum;
  int ULStat;
  int channel[2];
  //int FlashStart;
  //int FlashDuration;
  int OPTIONS;
  int Average;

  int Gain;      // Gain range
      /*
      1 = -10 to +10 Volts
      0 = -5 to +5 Volts
        2 = -2.5 to +2.5 Volts
      3 = -1.25 to +1.25 Volts
      4 = -1 to +1 Volts
      5 = -.625 to +.625 Volts
      6 = -.5 to +.5 Volts
      7 = -.1 to +.1 Volts
      100 = 0 to 10 Volts
      101 = 0 to 5 Volts 
      114 = 0 to 4 Volts 
      102 = 0 to 2.5 Volts 
      103 = 0 to 2 Volts 
      109 = 0 to 1.67 Volts 
      104 = 0 to 1.25 Volts 
      105 = 0 to 1 Volt 
      110 = 0 to .5 Volt 
      */

  long SampleRate;
#define sampleSize 100000
  WORD * ADData;
  unsigned int VoltageValue;
  unsigned int Threshold;
  bool rise;
  bool inverted;

  std::vector<float > ECGRateBPMArray;
//ETX


private:
  vtkUSBECGBox(const vtkUSBECGBox&);
  void operator=(const vtkUSBECGBox&);  
};

#endif
