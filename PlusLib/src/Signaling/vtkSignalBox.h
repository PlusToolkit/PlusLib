/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
Authors include: Chris Wedlake <cwedlake@imaging.robarts.ca>, Danielle Pace
(Robarts Research Institute and The University of Western Ontario)
=========================================================================*/  

#ifndef __vtkSignalBox_h
#define __vtkSignalBox_h

#include "vtkObject.h"
#include "vtkRecursiveCriticalSection.h"

class vtkMultiThreader;

/*!
  \class vtkSignalBox
  \brief Parent class for signal boxes
  
  Children should implement:
    * GetECG()
    * Initialize()
    * Start() (at least to call the new vtkSignalBoxThread)
    * Stop()
    * vtkSignalBoxThread (to cast self to the child type)
    (see vtkHeartSignalBox for an example)
  
  This class returns a signal every this->SignalPeriod seconds

  \ingroup PlusLibSignaling
*/
class VTK_EXPORT vtkSignalBox : public vtkObject
{
public:
  static vtkSignalBox *New();
  vtkTypeMacro(vtkSignalBox, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  /*! Initialize the system */
  virtual void Initialize();

  /*! Start the system - may be reimplemented in child classes */
  virtual void Start();

  /*! Stop the system - may be reimplemented in child classes */
  virtual void Stop();

  /*! Update by calling GetECG() */
  virtual void Update();

//BTX
  // These are used by static functions in vtkSignalBox.cxx, and since
  // VTK doesn't generally use 'friend' functions they are public
  // instead of protected.  Do not use them anywhere except inside
  // vtkSignalBox.cxx.
  /*! Internal use only */
  vtkRecursiveCriticalSection *UpdateMutex;
  /*! Internal use only */
  vtkTimeStamp UpdateTime;
  /*! Internal use only */
  double InternalUpdateRate;  
//ETX

  /*! Get the current phase */
  virtual int GetPhase() { return this->ECGPhase; }

  /*! Get the current beating rate (beats per minute) */
  virtual float GetBPMRate() { return this->ECGRateBPM; }

  /*! Get the current timestamp */
  vtkGetMacro(Timestamp,double);

  /*! Get the current ECG signal */
  virtual int GetECGSignal() { return this->Signal; };

  /*! Set the number of phases in one cycle */
  virtual void SetNumberOfPhases(double phases) { this->TotalPhases=phases; }
  /*! Get the number of phases in one cycle */
  virtual int GetNumberOfPhases() { return this->TotalPhases; }

  /*! Set the period at which the system updates the ECG signal (in seconds) */
  vtkSetMacro(SleepInterval, double);
  /*! Get the period at which the system updates the ECG signal (in seconds) */
  vtkGetMacro(SleepInterval, double);

  /*! Enable/disable beep on new cycle */
  vtkSetMacro(AudibleBeep, int);
  /*! Enable/disable beep on new cycle */
  vtkGetMacro(AudibleBeep, int);
  /*! Enable/disable beep on new cycle */
  vtkBooleanMacro(AudibleBeep, int);

  /*! Get if we are started */
  vtkGetMacro(IsStarted, int);

  /*! Set period between signals, in seconds (default 1 second) */
  vtkSetMacro(SignalPeriod,double);
  /*! Get period between signals, in seconds */
  vtkGetMacro(SignalPeriod,double);

  /*! Get time of a phase for the previous cycle for retrospective gating */
  virtual double CalculateRetrospectiveTimestamp(int phase);

protected:
  vtkSignalBox();
  ~vtkSignalBox();

  virtual void CalculateECGRate(double StartSignal, double current);
  virtual void CalculatePhase(double StartSignal, double current);
  virtual void UpdateTimestamp();

  /*!
    Get the current ECG signal, and update the ECG phase, beating rate,
    and start signal time stamp as necessary, and return the current
    ECG signal.  Should be reimplemented in child classes.
  */
  virtual int GetECG(void);

  vtkMultiThreader *Threader;
  int ThreadId;

  /*! current ECG signal */
  int Signal;
  /*! previous ECG signal */
  int SignalPrev; 
  /*! current timestamp */
  double Timestamp; 
  /*! timestamp at the beginning of the current cycle */
  double StartSignalTimeStamp; 
  /*! time stamp at the beginning of the previous cycle */
  double PrevStartSignalTimeStamp; 
  /*! number of phases per cycle */
  int TotalPhases; 
  /*! current phase */
  float ECGPhase; 
  /*! beating rate: beats per minute */
  float ECGRateBPM; 
  /*! is system on? */
  int IsStarted; 
  int IsInitialized;
  /*! in seconds */
  double SleepInterval; 
  /*! beep on new cycle */
  int AudibleBeep; 
  /*! for the signal box, how many seconds should pass between each signal */
  double SignalPeriod; 

private:
  vtkSignalBox(const vtkSignalBox&);
  void operator=(const vtkSignalBox&);  
  
};

#endif

