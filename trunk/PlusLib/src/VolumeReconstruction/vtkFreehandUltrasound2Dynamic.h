/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkFreehandUltrasound2Dynamic.h,v $
  Language:  C++
  Date:      $Date: 2009/07/20 18:48:07 $
  Version:   $Revision: 1.18 $
  Thanks:    Thanks to David G. Gobbi who developed this class. 
  Thanks:    Thanks to Danielle Pace who developed this class.

==========================================================================

Copyright (c) 2000-2007 Atamai, Inc.
Copyright (c) 2008-2009 Danielle Pace

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
// .NAME vtkFreehandUltrasound2Dynamic - real-time freehand ultrasound reconstruction
// with ECG gating
// .SECTION Description
// vtkFreehandUltrasound2 will incrementally compound ultrasound images into a
// reconstruction volume, given a transform which specifies the location of
// each ultrasound slice.  An alpha component is appended to the output to
// specify the coverage of each pixel in the output volume (i.e. whether or
// not a voxel has been touched by the reconstruction)
// .SECTION see also
// vtkFreehandUltrasound2, vtkVideoSource2, vtkTracker, vtkTrackerTool

#ifndef __vtkFreehandUltrasound2Dynamic_h
#define __vtkFreehandUltrasound2Dynamic_h

#include "vtkFreehandUltrasound2.h"

class vtkSignalBox;
class vtkBMPWriter;

class VTK_EXPORT vtkFreehandUltrasound2Dynamic : public vtkFreehandUltrasound2
{
public:
  static vtkFreehandUltrasound2Dynamic *New();
  vtkTypeRevisionMacro(vtkFreehandUltrasound2Dynamic, vtkFreehandUltrasound2);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get the accumulation buffer, for use with triggering
  // accumulation buffer is for compounding, there is a voxel in
  // the accumulation buffer for each voxel in the output
  virtual vtkImageData *GetAccumulationBuffer();
  virtual vtkImageData *GetAccumulationBufferFromPhase(int phase);
  virtual vtkImageData *GetAccumulationBufferFromPort(int port);

  // Description:
  // Get the output reconstructed 3D ultrasound volume, for use with
  // triggering
  // (the output is the reconstruction volume, the second component
  // is the alpha component that stores whether or not a voxel has
  // been touched by the reconstruction)
  virtual vtkImageData *GetOutput();
  virtual vtkImageData *GetOutputFromPhase(int phase);
  virtual vtkImageData *GetOutputFromPort(int port);

  // Description:
  // Fill holes in the output by using the weighted average of the
  // surrounding voxels.  If Compounding is off, then all hit voxels
  // are weighted equally. 
  virtual void FillHolesInOutput();

  // Description:
  // Configure the freehand ultrasound reconstruction according to a summary file
  virtual int ReadSummaryFile(const char *filename);

  // Description:
  // Cause the slice to be inserted into the first reconstruction volume
  //void InsertSlice();
  virtual void InsertSlice(vtkImageData* outData, vtkImageData *accData);
  virtual void InsertSlice(vtkImageData* outData, vtkImageData* accData, int phase);

  // Description:
  // Insert the slice into the ith volume, with optimization
  void OptimizedInsertSlice(vtkImageData* outData, vtkImageData* accData);
  void OptimizedInsertSlice(vtkImageData* outData, vtkImageData* accData, int phase);

  // Description:
  // Clear the data volume.
  void ClearOutput();

  // Description:
  // Set compounding
  void SetCompounding(int c);

  // Description:
  // Option to turn triggering for ECG-gated acquisition on or off (default off)
  // Note that setting triggering from on to off has side effect of deleting all
  // except the first one, and setting triggering from off to on has the side effect
  // of deleting all volumes
  virtual void SetTriggering(int);
  vtkGetMacro(Triggering,int);
  vtkBooleanMacro(Triggering, int);

  // Description:
  // Signal box for the triggering (must have at least two phases)
  virtual void SetSignalBox(vtkSignalBox *);
  vtkGetObjectMacro(SignalBox, vtkSignalBox);

  // Description:
  // Turn on or off retrospective gating (default on), if off
  // uses prospective gating
  vtkGetMacro(Retrospective, int);
  void SetRetrospective(int retro);
  void RetrospectiveOn() {this->SetRetrospective(1);};
  void RetrospectiveOff() {this->SetRetrospective(0);};

  // Description:
  // Whether or not to discard image slices that occur during
  // outlier heart rates (default off)
  void SetCheckHeartRate(int discard);
  vtkGetMacro(CheckHeartRate, int);
  vtkBooleanMacro(CheckHeartRate, int);

  // Description:
  // Set the amount of time (in seconds) that we will monitor the heart
  // rate for before starting the reconstruction - the mean heart rate
  // will be used as the baseline heart rate (default 10 seconds)
  vtkSetMacro(ECGMonitoringTime, double);
  vtkGetMacro(ECGMonitoringTime, double);

  // Description:
  // Set the maximum number of trials to find the heart rate measurements
  // (default 5)
  vtkSetMacro(NumECGTrials, int);
  vtkGetMacro(NumECGTrials, int);

  // Description:
  // Percentage of the heart rate that the heart rate can increase
  // by and still be considered valid (i.e. specify 10 for 10%)
  // (default 20%)
  vtkSetMacro(PercentageIncreasedHeartRateAllowed, double);
  vtkGetMacro(PercentageIncreasedHeartRateAllowed, double);

  // Description:
  // Percentage of the heart rate that the heart rate can decrease
  // by and still be considered valid (i.e. specify 10 for 10%)
  // (default 20%)
  vtkSetMacro(PercentageDecreasedHeartRateAllowed, double);
  vtkGetMacro(PercentageDecreasedHeartRateAllowed, double);

  // Description:
  // When triggering, toggle whether you will reconstruct all phases or
  // if you will choose select phases to reconstruct
  virtual void SetUseSelectPhases(int);
  vtkGetMacro(UseSelectPhases, int);
  vtkBooleanMacro(UseSelectPhases, int);

  // Description:
  // When selecting which phases to reconstruct, set whether or not you
  // want to reconstruct a phase (default on).  This must be called
  // after the signal box has been assigned.
  // Note for pipeline - when turning reconstruction for a certain phase
  // off, will always remove the last outputs, not any in the middle -
  // so make sure that you don't have filters further in the pipeline that
  // expect data at the last ports!
  virtual void SetPhaseReconstruction(int phase, int onoff);

  // Description:
  // When selecting which phases to reconstruct, get whether or not
  // a phase will be reconstructed.  If we are not triggering or don't have a
  // signal box, then all phases are reconstructed and this returns true.  If
  // we are not selecting phases to reconstruct, then all valid phases return
  // true.  If we are selecting phases to reconstruct, then all phases that
  // will be reconstructed reutrn true.
  virtual int GetPhaseReconstruction(int phase);

  // Description:
  // Calculates the phase from an output volume, for use when selecting
  // which volumes to reconstruct.  Returns 0 if not triggering or if
  // we don't have a signal box, -1 on error or if the phase is not reconstructed,
  // or the port corresponding to the phase if the phase is reconstructed
  int CalculatePhaseFromPort(int port);

  // Directory:
  // Get the number of phases we think the signal box has
  vtkGetMacro(NumSignalBoxPhases, int);

  // Description:
  // Sets the directory for saved timestamps and/or slices
  virtual void SetSaveInsertedDirectory(const char *directory);

  // Description:
  // Saves the timestamps used to insert the slices if turned on (default off)
  // The directory for saving inserted timestamps and/or slices should be set previously
  virtual void SaveInsertedTimestampsOn();
  virtual void SaveInsertedTimestampsOff();
  vtkGetMacro(SaveInsertedTimestamps, int);

  // Description:
  // Saves the inserted slices if turned on (default off).  Automatically saves
  // the inserted timestamps as well.  The directory for saving inserted timestamps
  // and/or slices should be set previously
  virtual void SaveInsertedSlicesOn();
  virtual void SaveInsertedSlicesOff();
  vtkGetMacro(SaveInsertedSlices, int);

  // Description:
  // The current phase for triggering
  vtkGetMacro(CurrentPhase,int);

  // Description:
  // The previous phase for triggering
  vtkGetMacro(PreviousPhase,int);

  // Description:
  // The ecg rate for triggering
  vtkGetMacro(ECGRate, double);

  // Description:
  // Check whether a phase shift was detected
  vtkGetMacro(PhaseShift, int);

  // Description:
  // Get the number of output volumes
  vtkGetMacro(NumberOfOutputVolumes,int);

  // Description:
  // The mean heart rate over the time specified by ECGMonitoringTime,
  // as calculated at the beginning of the reconstruction
  vtkGetMacro(MeanHeartRate, double);

  // Description:
  // The maximum heart rate that is still valid
  vtkGetMacro(MaxAllowedHeartRate, double);

  // Description:
  // The minimum heart rate that is still valid
  vtkGetMacro(MinAllowedHeartRate, double);
  
  // Description:
  // Half of the video period, so that we don't have to calculate it
  // on every iteration of the reconstruction
  vtkGetMacro(HalfVideoPeriod, double);

  // Description:
  // Turn on for verbose debugging
  //vtkGetMacro(VerboseDebugging, int);
  //vtkSetMacro(VerboseDebugging, int);
  //vtkBooleanMacro(VerboseDebugging, int);

  //////////////////////////////////////////////////////////////////////////////////
  // Not protected to be available from the reconstruction thread
  // These are internal functions, don't use them!!!

//BTX
  // Description:
  // Not protected to be accessible from reconstruction thread - do not use this function
  virtual void ReconstructSlice(double timestamp, vtkImageData* inData);

  // Description:
  // Do any old inserts before inserting the current slice (not used in base class, but
  // used in derived classes), for the reconstruction thread
  virtual void ReconstructOldSlice(double timestamp, vtkImageData* inData);

  // Description:
  // Get the time to sleep, for the reconstruction thread
  virtual double GetSleepTime();

  // Description:
  // Get the timestamp for the video slice, for the reconstruction thread
  virtual double CalculateCurrentVideoTime(vtkImageData* inData);

  // Description:
  // Update the input slice, for the reconstruction thread
  virtual int UpdateSlice(vtkImageData* inData);

  // Description:
  // Update the accumulation buffer, for the reconstruction thread
  virtual void UpdateAccumulationBuffers();
//ETX
  //////////////////////////////////////////////////////////////////////////////////

protected:
  vtkFreehandUltrasound2Dynamic();
  ~vtkFreehandUltrasound2Dynamic();

  // triggering
  int Triggering;
  vtkSignalBox *SignalBox;
  int Retrospective;
  int CurrentPhase;
  int PreviousPhase;
  double ECGRate;
  int PhaseShift;
  int NumberOfOutputVolumes;
  double VideoTime; // timestamp for the current video slice
  double PreviousVideoTime; // timestamp for the previous video slice
  double DesiredTime; // ideal timestamp calculated via retrospective gating
  
  // check heart rate
  int CheckHeartRate;
  // put slice and transforms into these buffers (size of buffer
  // is number of output volumes), wait one cycle, check heart rate
  // and put them in if heart rate is ok
  vtkImageData **SliceBuffer;
  vtkMatrix4x4 **SliceAxesBuffer;
  vtkLinearTransform **SliceTransformBuffer;
  double *SliceTimestampsBuffer;
  int NumberInsertedIntoSliceBuffers;

  // deciding on expected heart rate when checking heart rate
  double ECGMonitoringTime;
  int NumECGTrials;
  double PercentageIncreasedHeartRateAllowed;
  double PercentageDecreasedHeartRateAllowed;
  double MeanHeartRate;
  double MaxAllowedHeartRate;
  double MinAllowedHeartRate;
  int PreviousCyclePassed;

  // waiting for next video frame in prospective gating
  int InsertOnNextVideoFrame;
  int PhaseToInsert;
  double HalfVideoPeriod;

  // choosing only select phases from the signal box to reconstruct
  int UseSelectPhases;
  int *MapPhaseToOutputVolume;
  int NumSignalBoxPhases;

  // saving the inserted timestamps and slices
  int SaveInsertedTimestamps;
  char *SaveInsertedTimestampsOutputFilename;
  FILE *SaveInsertedTimestampsOutputFile;
  int SaveInsertedSlices;
  vtkBMPWriter* SaveInsertedSlicesWriter;
  char *SaveInsertedDirectory;
  int SaveInsertedTimestampsCounter;

  // Description:
  // Setup anything that needs to be done before real-time reconstruction
  // (override in derived classes)
  virtual int InitializeRealTimeReconstruction();

  // Description:
  // Setup anything that needs to be done before non-real-time reconstruction
  // (override in derived classes)
  virtual int InitializeReconstruction();

  // Description:
  // Clean up any loose ends after real-time reconstruction
  // (overridden in derived classes)
  virtual void UninitializeRealTimeReconstruction();

  // Description:
  // Clean up any loose ends after non-real-tme reconstruction
  // (override in derived classes)
  virtual void UninitializeReconstruction();

  // Description:
  // Setup the output volumes
  virtual void SetupOutputVolumes();
  virtual void SetupOutputVolumes(int num);

  // Description:
  // Setup the accumulation buffers if we are compounding
  virtual void SetupAccumulationBuffers();
  virtual void SetupAccumulationBuffers(int num);

  // Description:
  // Deletes the accumulation buffers
  virtual void DeleteAccumulationBuffers();

  // Description:
  // Both StartReconstruction() and StartRealTimeReconstruction() need to run
  // this before reconstructing with triggering, to make sure that the signal
  // box is set up properly
  int TestBeforeReconstructingWithTriggering();

  // Description:
  // Actually clear the output volume(s), by calling InternalInternalClearOutput
  virtual void InternalClearOutput();

  // Description:
  // Getting the IndexMatrix when discarding slices based on ECG signal
  vtkMatrix4x4 *GetIndexMatrix();
  vtkMatrix4x4 *GetIndexMatrix(int phase);

  // Description:
  // Calculate the mean heart rate and allowed maximum and minimum
  // heart rates
  int CalculateHeartRateParameters();

  // Description:
  // Set/Get the image slice for a particular phase in the buffer, when
  // discarding outlier heart rates
  void SetSliceBuffer(int phase, vtkImageData* inData);
  vtkImageData* GetSliceBuffer(int phase);

  // Description:
  // Set/Get the slice axes matrix  and the slice transform for a particular phase in the buffer,
  // when discarding outlier heart rates
  void SetSliceAxesAndTransformsBuffers(int phase, vtkMatrix4x4* sliceAxesMatrix, vtkLinearTransform* sliceTransform, double timestamp);
  vtkMatrix4x4* GetSliceAxesBuffer(int phase);
  vtkLinearTransform* GetSliceTransformBuffer(int phase);
  double GetSliceTimestampsBuffer(int phase);

  // Description:
  // The number of items inserted into the slice buffers
  vtkGetMacro(NumberInsertedIntoSliceBuffers, int);
  vtkSetMacro(NumberInsertedIntoSliceBuffers, int);

  // Description:
  // Clears the slice, slice axes and slice transform buffers for you
  // Does not delete the buffers, just sets values to zero/identity as
  // required
  // For use with discarding based on ECG signals
  void ClearBuffers();

  // Description:
  // Create the slice, slice axes and slice transform buffers
  void CreateSliceBuffers(int num);

  // Description:
  // Actually deletes the slice, slice axes and slice transform buffers
  void DeleteSliceBuffers();

  // Description:
  // Calculates the output volume index for a phase, for use when selecting
  // which volumes to reconstruct.  Returns 0 if not triggering or if
  // we don't have a signal box, -1 on error or if the phase is not reconstructed,
  // or the port corresponding to the phase if the phase is reconstructed
  int CalculatePortFromPhase(int phase);

  // Description:
  // Save the slice to .bmp and save the timestamp
  void SaveSlice(vtkImageData* inData, int phase, double timestamp);

  // Description:
  // Returns an XMLDataElement describing the freehand object
  virtual vtkXMLDataElement* MakeXMLElement();

  // Description:
  // Similar to RequestInformation(), but applied to the output
  virtual void InternalExecuteInformation();

  // Description:
  // for VTK 5 pipeline
  virtual int RequestInformation(vtkInformation* request,
                                 vtkInformationVector** inputVector,
                                 vtkInformationVector* outputVector);
  
private:
  vtkFreehandUltrasound2Dynamic(const vtkFreehandUltrasound2Dynamic&);
  void operator=(const vtkFreehandUltrasound2Dynamic&);
};

#endif
