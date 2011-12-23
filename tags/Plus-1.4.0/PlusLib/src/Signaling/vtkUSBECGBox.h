/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
Authors include: Chris Wedlake <cwedlake@imaging.robarts.ca>, Danielle Pace
(Robarts Research Institute and The University of Western Ontario)
=========================================================================*/  

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

/*!
\class vtkUSBECGBox
\brief Interfaces Plus with real-time ECG signal
\ingroup PlusLibSignaling
*/ 
class VTK_EXPORT vtkUSBECGBox : public vtkSignalBox
{
public:
  static vtkUSBECGBox *New();
  vtkTypeMacro(vtkUSBECGBox,vtkSignalBox);
  void PrintSelf(ostream& os, vtkIndent indent);

//  int GetRawECG(void);
//  int GetRawPressure(void);

  /*!
    Start the tracking system.  The tracking system is brought from
    its ground state (i.e. on but not necessarily initialized) into
    full tracking mode.  This method calls InternalStartTracking()
    after doing a bit of housekeeping.
  */
  void Start();

  /*!
    Stop the tracking system and bring it back to its ground state.
    This method calls InternalStopTracking().
  */
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
