/*=========================================================================

  Program:   AtamaiTracking for VTK
  Module:    $RCSfile: vtkTrackerBuffer.h,v $
  Creator:   David Gobbi <dgobbi@atamai.com>
  Language:  C++
  Author:    $Author: dgobbi $
  Date:      $Date: 2005-07-01 22:52:05 $
  Version:   $Revision: 1.3 $

==========================================================================

Copyright (c) 2000-2005 Atamai, Inc.

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
// .NAME vtkTrackerBuffer - maintain a circular buffer of matrices

// .SECTION Description
// vtkTrackerBuffer maintains a list of matrices and their associated
// flags, collected from a vtkTrackerTool.  The list is circular, meaning
// that it has a set maximum size and, after the number of added entries
// is greater than that maximum size, earlier entries are overwritten
// in a first-in, first-out manner.

// .SECTION see also
// vtkTrackerTool vtkTracker

#ifndef __vtkTrackerBuffer_h
#define __vtkTrackerBuffer_h

#include "vtkObject.h"
#include "vtkTracker.h"
#include "vtkMatrix4x4.h"
#include "vtkCriticalSection.h"

class vtkDoubleArray;
class vtkIntArray;
class vtkUnsignedLongLongArray; 

class VTK_EXPORT vtkTrackerBuffer : public vtkObject
{
public:
  vtkTypeMacro(vtkTrackerBuffer,vtkObject);
  static vtkTrackerBuffer *New();

  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the size of the buffer, all new transforms are set to unity.
  void SetBufferSize(int n);
  int GetBufferSize() { return this->BufferSize; };

  // Description:
  // Get the number of items in the list (this is not the same as
  // the buffer size, but is rather the number of transforms that
  // have been added to the list).  This will never be greater than
  // the BufferSize.
  int GetNumberOfItems() { return this->NumberOfItems; };

  // Description:
  // Lock the buffer: this should be done before changing or accessing
  // the data in the buffer if the buffer is being used from multiple
  // threads.
  void Lock() { this->Mutex->Lock(); };
  void Unlock() { this->Mutex->Unlock(); };

  // Description:
  // Add a matrix plus flags to the list.  If the timestamp is
  // less than or equal to the previous timestamp, then nothing
  // will be done.
  void AddItem(vtkMatrix4x4 *matrix, long flags, unsigned long frameNumber, double unfilteredTimestamp, double filteredTimestamp);

  // Description:
  // Get a(n uncalibrated) matrix from the list, where '0' is the most recent
  // '1' is the oldest and '-1' is the second-newest.
  void GetMatrix(vtkMatrix4x4 *matrix, int i);

   // Description:
  // Get a frame number from the buffer, relative to the current index (positive is forward,
  // negative is backward). '0' is the newest, '1' is the oldest and '-1' is the second-newest.
  virtual unsigned long GetFrameNumber(int i); 

  // Description:
  // Get a calibrated matrix from the list, where '0' is the most
  // recent and (NumberOfItems-1) is the oldest. 
  void GetCalibratedMatrix(vtkMatrix4x4 *matrix, int i);

  // Description:
  // Get the flags for a particular matrix.
  long GetFlags(int i);
  int IsMissing(int i)   {return ((this->GetFlags(i) & TR_MISSING) != 0); };
  int IsOutOfView(int i) {return ((this->GetFlags(i) & TR_OUT_OF_VIEW) != 0);};
  int IsRequestTimeout(int i) {return ((this->GetFlags(i) & TR_REQ_TIMEOUT) != 0);};
  int IsOutOfVolume(int i) {return ((this->GetFlags(i)&TR_OUT_OF_VOLUME)!=0);};
  int IsSwitch1On(int i) {return ((this->GetFlags(i) & TR_SWITCH1_IS_ON)!=0);};
  int IsSwitch2On(int i) {return ((this->GetFlags(i) & TR_SWITCH2_IS_ON)!=0);};
  int IsSwitch3On(int i) {return ((this->GetFlags(i) & TR_SWITCH3_IS_ON)!=0);};

  // Description:
  // Get the timestamp (in seconds since Jan 1, 1970) for the matrix.
  double GetTimeStamp(int i) { return this->GetFilteredTimeStamp(i); } 
  double GetFilteredTimeStamp(int i); 
  double GetUnfilteredTimeStamp(int i); 

  // Description:
  // Set a calibration matrices to be applied when GetMatrix() is called.
  vtkSetObjectMacro(ToolCalibrationMatrix,vtkMatrix4x4);
  vtkGetObjectMacro(ToolCalibrationMatrix,vtkMatrix4x4);
  vtkSetObjectMacro(WorldCalibrationMatrix,vtkMatrix4x4);
  vtkGetObjectMacro(WorldCalibrationMatrix,vtkMatrix4x4);

  // Description:
  // Given a timestamp, compute the nearest index.  This assumes that
  // the times monotonically increase as the index decreases.
  int GetIndexFromTime(double time);

  // Description:
  // Get a calibrated matrix for a particular timestamp, where the timestamp is
  // in system time as returned by vtkTimerLog::GetCurrentTime().
  long GetFlagsAndCalibratedMatrixFromTime(vtkMatrix4x4 *matrix, double time);

  // Description:
  // Get a(n uncalibrated) matrix for a particular timestamp, where the
  // timestamp is in system time as returned by vtkTimerLog::GetCurrentTime().
  long GetFlagsAndMatrixFromTime(vtkMatrix4x4 *matrix, double time);

  // Description:
  // Make this buffer into a copy of another buffer.  You should
  // Lock both of the buffers before doing this.
  void DeepCopy(vtkTrackerBuffer *buffer);

  // Description:
  // Write all stored tracking information out to a file.
  void WriteToFile(const char *filename);

  // Description:
  // Read tracking information from a file.
  void ReadFromFile(const char *filename);

  // Description:
  // Get the frame rate from the buffer based on the number of frames in the buffer
  // and the elapsed time.
  // Ideal frame rate shows the mean of the frame periods in the buffer based on the frame 
  // number difference (aka the device frame rate)
  virtual double GetFrameRate(bool ideal = false);

   // Description:
  // Clear buffer (set the buffer pointer to the first element)
  virtual void Clear(); 

  // Description:	
  // Get/Set the local time offset (global = local + offset)
  vtkSetMacro(LocalTimeOffset, double); 
  vtkGetMacro(LocalTimeOffset, double);

protected:
  vtkTrackerBuffer();
  ~vtkTrackerBuffer();

  vtkDoubleArray *MatrixArray;
  vtkDoubleArray *FilteredTimeStampArray;
  vtkDoubleArray *UnfilteredTimeStampArray;
  vtkIntArray *FlagArray;
  vtkUnsignedLongLongArray *FrameNumberArray; 

  vtkMatrix4x4 *ToolCalibrationMatrix;
  vtkMatrix4x4 *WorldCalibrationMatrix;

  vtkCriticalSection *Mutex;

  int BufferSize;
  int NumberOfItems;
  int CurrentIndex;
  double CurrentTimeStamp;
  double LocalTimeOffset; 

private:
  vtkTrackerBuffer(const vtkTrackerBuffer&);
  void operator=(const vtkTrackerBuffer&);
};

#endif
