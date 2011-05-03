/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkVideoBuffer2.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  Author: Danielle Pace
          Robarts Research Institute and The University of Western Ontario

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information. 

=========================================================================*/
// .NAME vtkVideoBuffer2 - Store a collection of video frames
// .SECTION Description
// vtkVideoBuffer2 is a structure for holding video frames that are
// captured either from a vtkRenderWindow or from some other video
// source.  The buffer can be locked, to allow some video frames to
// be written to while other frames are being read from.  Hopefully
// an additional class will be written that will take a vtkVideoBuffer2
// and compress it into a movie file.
// .SECTION See Also
// vtkVideoFrame2 vtkVideoSource2 vtkWin32VideoSource2 vtkMILVideoSource2

#ifndef __vtkVideoBuffer2_h
#define __vtkVideoBuffer2_h

#include "vtkObject.h"

class vtkCriticalSection;
class vtkVideoFrame2;
class vtkDoubleArray;
class vtkUnsignedLongLongArray; 

class VTK_EXPORT vtkVideoBuffer2 : public vtkObject
{
public:
  static vtkVideoBuffer2 *New();
  vtkTypeRevisionMacro(vtkVideoBuffer2,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set/Get the size of the buffer, i.e. the maximum number of
  // video frames that it will hold.  The default is 30.
  virtual void SetBufferSize(int n);
  vtkGetMacro(BufferSize, int);

  // Description:
  // Set/Get the number of items in the list (this is not the same as
  // the buffer size, but is rather the number of transforms that
  // have been added to the list).  This will never be greater than
  // the BufferSize.
  vtkGetMacro(NumberOfItems, int);

  // Description:
  // Set/Get the format of the video frames in the buffer (the get method
  // returns a frame that contains no data, just the format 
  // description).  If the buffer is not empty, changing the frame format has the
  // side-effect of deleting the frames within the buffer.
  virtual void SetFrameFormat(vtkVideoFrame2 *);
  vtkVideoFrame2 *GetFrameFormat() { return this->FrameFormat; };

  // Description:
  // Lock/Unlock the buffer: this should be done before changing or accessing
  // the data in the buffer if the buffer is being used from multiple
  // threads.  
  virtual void Lock();
  virtual void Unlock();

  // Description:
  // Add a frame plus a timestamp to the buffer with frame index.  If the timestamp is
  // less than or equal to the previous timestamp, or if the frame's format
  // doesn't match the buffer's frame format, then nothing will be done.
  // This method does not automatically seek forward before adding the new
  // frame and timestamp: you should call Seek(-1) first. Remember to
  // Allocate() each frame before adding it to a buffer.
  virtual void AddItem(vtkVideoFrame2* frame, double unfilteredTimestamp, double filteredTimestamp, int frameNumber);

  // Description:
  // Rotate the buffer forwards or backwards by the specified number
  // of frames, relative to the current index (positive is forward, negative is backward).
  virtual void Seek(int n); 

  // Description:
  // Get a frame from the buffer, relative to the current index (positive is forward,
  // negative is backward). '0' is the newest, '1' is the oldest and '-1' is the second-newest.
  virtual vtkVideoFrame2 *GetFrame(int i);

  // Description:
  // Get a timestamp from the buffer (in seconds since Jan 1, 1970), relative to the
  // current index (positive is forward, negative is backward). '0' is the newest,
  // '1' is the oldest and '-1' is the second-oldest.
  virtual double GetTimeStamp(int i) { return this->GetFilteredTimeStamp(i); }
  virtual double GetFilteredTimeStamp(int i); 
  virtual double GetUnfilteredTimeStamp(int i); 

  // Description:
  // Get a frame index from the buffer, relative to the current index (positive is forward,
  // negative is backward). '0' is the newest, '1' is the oldest and '-1' is the second-newest.
  virtual unsigned long GetFrameNumber(int i); 

  // Description:
  // Given a timestamp, compute the nearest index, relative to the current index.
  // This assumes that the times motonically increase as the index decreases and that
  // the current index points to the newest frame.  Remember that the current index
  // changes while recording or playing, so be sure to lock the mutex before using
  // this function.
  virtual int GetIndexFromTime(double time);

   // Description:
  // Make this buffer into a copy of another buffer.  You should
  // Lock both of the buffers before doing this.
  virtual void DeepCopy(vtkVideoBuffer2* buffer); 

  // Description:	
  // Get/Set the local time offset (global = local + offset)
  vtkSetMacro(LocalTimeOffset, double); 
  vtkGetMacro(LocalTimeOffset, double);

  // Description:
  // Get the frame rate from the buffer based on the number of frames in the buffer
  // and the elapsed time.
  // Ideal frame rate shows the mean of the frame periods in the buffer based on the frame 
  // number difference (aka the device frame rate)
  virtual double GetFrameRate(bool ideal = false);

  // Description:
  // Clear buffer (set the buffer pointer to the first element)
  virtual void Clear(); 
  
protected:
  vtkVideoBuffer2();
  ~vtkVideoBuffer2();

  vtkVideoFrame2 **FrameArray;
  vtkDoubleArray *FilteredTimeStampArray;
  vtkDoubleArray *UnfilteredTimeStampArray;
  vtkUnsignedLongLongArray *FrameNumberArray; 

  // holds the formatting information for the buffer's frames, but no data.
  vtkVideoFrame2 *FrameFormat;

  vtkCriticalSection *Mutex;

  int BufferSize;
  int NumberOfItems;
  int CurrentIndex;
  double CurrentTimeStamp;
  double LocalTimeOffset; 

private:
  vtkVideoBuffer2(const vtkVideoBuffer2&);
  void operator=(const vtkVideoBuffer2&);
};

#endif
