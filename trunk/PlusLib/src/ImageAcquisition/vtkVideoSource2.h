/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkVideoSource2.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  Author: Danielle Pace
          Robarts Research Institute and The University of Western Ontario

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information. 

=========================================================================*/
// .NAME vtkVideoSource2 - Superclass of video input devices for VTK
// .SECTION Description
// vtkVideoSource2 is a superclass for video input interfaces for VTK.
// This base class records input from a noise source.  vtkVideoSource2 uses
// vtkVideoBuffer2 and vtkVideoFrame2 to hold its image data, and should be
// used instead of vtkVideoSource if you want to be able to access data while
// recording.
// .SECTION Caveats
// You must call the ReleaseSystemResources() method before the application
// exits.  Otherwise the application might hang while trying to exit.
// .SECTION See Also
// vtkWin32VideoSource2 vtkMILVideoSource2 vtkVideoBuffer2 vtkVideoFrame2

#ifndef __vtkVideoSource2_h
#define __vtkVideoSource2_h

#include "vtkImageAlgorithm.h"
#include "vtkXMLDataElement.h"
#include <vector>

class vtkTimerLog;
class vtkCriticalSection;
class vtkMultiThreader;
class vtkScalarsToColors;
class vtkVideoBuffer;
class vtkImageWriter;
class vtkImageReader2;
class vtkHTMLGenerator; 
class vtkGnuplotExecuter; 
class vtkVideoFrame2; 
class VideoBufferItem;


#define FILETYPE_BMP  1
//#define FILETYPE_MINC 2
#define FILETYPE_PNG  3 
#define FILETYPE_TIFF 4

// Frame grabber type is used by UnpackRasterLine in vtkVideoFrame2
#define FG_BASE    1
#define FG_MIL    2
#define FG_WIN32  3

class VTK_EXPORT vtkVideoSource2 : public vtkImageAlgorithm
{
public:
  static vtkVideoSource2 *New();
  vtkTypeRevisionMacro(vtkVideoSource2,vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);   

  // Description:
  // Read/write main configuration from/to xml data
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  // Description:
  // Connect to device
  // Should be overridden to connect to the hardware 
  virtual PlusStatus Connect();

  // Description:
  // Disconnect from device
  // Should be overridden to disconnect from the hardware 
  virtual PlusStatus Disconnect();

  // Description:
  // Record incoming video at the specified FrameRate.  The recording
  // continues indefinitely until Stop() is called. 
  virtual PlusStatus Record();

  // Description:
  // Stop recording
  virtual PlusStatus Stop();

  // Description:
  // Grab a single video frame.
  virtual PlusStatus Grab();

  // Description:
  // Are we in record mode?
  // exclusive).
  vtkGetMacro(Recording,int);

  // Description:
  // Set/Get the full-frame size.  This must be an allowed size for the device,
  // the device may either refuse a request for an illegal frame size or
  // automatically choose a new frame size.
  // The default is usually 320x240x1, but can be device specific.  
  // The 'depth' should always be 1 (unless you have a device that
  // can handle 3D acquisition).
  virtual void SetFrameSize(int x, int y, int z);
  virtual void SetFrameSize(int dim[3]) { 
    this->SetFrameSize(dim[0], dim[1], dim[2]); };
  virtual int* GetFrameSize();
  virtual void GetFrameSize(int &x, int &y, int &z);
  virtual void GetFrameSize(int dim[3]);

  // Description:
  // Set/Get a particular frame rate (default 30 frames per second).
  virtual void SetFrameRate(float rate);
  vtkGetMacro(FrameRate,float);

  // Description:
  // Set/Get the output format.  This must be appropriate for device,
  // usually only VTK_LUMINANCE, VTK_RGB, and VTK_RGBA are supported.
  virtual void SetOutputFormat(int format);
  void SetOutputFormatToLuminance() { this->SetOutputFormat(VTK_LUMINANCE); };
  void SetOutputFormatToRGB() { this->SetOutputFormat(VTK_RGB); };
  void SetOutputFormatToRGBA() { this->SetOutputFormat(VTK_RGBA); };
  virtual int GetOutputFormat();

  // Description:
  // Set/Get size of the frame buffer, i.e. the number of frames that
  // the 'tape' can store.
  virtual void SetFrameBufferSize(int FrameBufferSize);
  virtual int GetFrameBufferSize();

  // Description:
  // Set/Get the number of frames to copy to the output on each execute.
  // The frames will be concatenated along the Z dimension, with the 
  // most recent frame first.  The default is 1.
  vtkSetMacro(NumberOfOutputFrames,int);
  vtkGetMacro(NumberOfOutputFrames,int);

  // Description:
  // Set/Get the clip rectangle for the frames.  The video will be clipped 
  // before it is copied into the framebuffer.  Changing the ClipRegion
  // will destroy the current contents of the framebuffer.
  // The default ClipRegion is (0,VTK_INT_MAX,0,VTK_INT_MAX,0,VTK_INT_MAX).
  virtual void SetClipRegion(int r[6]) { 
    this->SetClipRegion(r[0],r[1],r[2],r[3],r[4],r[5]); };
  virtual void SetClipRegion(int x0, int x1, int y0, int y1, int z0, int z1);
  vtkGetVector6Macro(ClipRegion,int);

  // Description:
  // Set/Get the WholeExtent of the output.  This can be used to either
  // clip or pad the video frame.  This clipping/padding is done when
  // the frame is copied to the output, and does not change the contents
  // of the framebuffer.  This is useful e.g. for expanding 
  // the output size to a power of two for texture mapping.  The
  // default is (0,-1,0,-1,0,-1) which causes the entire frame to be
  // copied to the output.
  vtkSetVector6Macro(OutputWholeExtent,int);
  vtkGetVector6Macro(OutputWholeExtent,int);
  
  // Description:
  // Set/Get the pixel spacing. 
  // Default: (1.0,1.0,1.0)
  vtkSetVector3Macro(DataSpacing,double);
  vtkGetVector3Macro(DataSpacing,double);
  
  // Description:
  // Set/Get the coordinates of the lower, left corner of the frame. 
  // Default: (0.0,0.0,0.0)
  vtkSetVector3Macro(DataOrigin,double);
  vtkGetVector3Macro(DataOrigin,double);

  // Description:
  // For RGBA output only (4 scalar components), set/get the opacity.  This
  // will not modify the existing contents of the framebuffer, only
  // subsequently grabbed frames.  The default is 1.0.
  virtual void SetOpacity(float alpha);
  virtual float GetOpacity();

  // Description:
  // This value is incremented each time a frame is grabbed.
  // reset it to zero (or any other value) at any time.
  vtkGetMacro(FrameCount, int);
  vtkSetMacro(FrameCount, int);

  // Description:
  // Set/Get maximum allowed difference of the frame delay compared to average delay, in seconds
  vtkSetMacro(MaximumFramePeriodJitter, double); 
  vtkGetMacro(MaximumFramePeriodJitter, double); 

  // Description:
  // Set/Get smoothing factor for accurate timing of the frames 
  // an exponential moving average is computed to smooth out the 
  // jitter in the times that are returned by the system clock:
  // EstimatedFramePeriod[t] = EstimatedFramePeriod[t-1] * (1-SmoothingFactor) + FramePeriod[t] * SmoothingFactor
  // Smaller SmoothingFactor results leads to less jitter.
  vtkSetMacro(SmoothingFactor, double); 
  vtkGetMacro(SmoothingFactor, double); 
  
  // Description:
  // Get the frame number (some devices has frame numbering, otherwise 
  // just increment if new frame received)
  vtkGetMacro(FrameNumber, unsigned long);

  // Description:
  // Get a time stamp in seconds (resolution of milliseconds) for
  // the most recent frame.  Time began on Jan 1, 1970.  This timestamp is only
  // valid after the Output has been Updated.   Usually set to the
  // timestamp for the output if UpdateWithDesiredTimestamp is off,
  // otherwise it is the timestamp for the most recent frame, which is not
  // necessarily the output
  virtual double GetFrameTimeStamp() { return this->FrameTimeStamp; };

  // Description:
  // Get the buffer that is used to hold the video frames.
  virtual vtkVideoBuffer *GetBuffer() { return this->Buffer; };

  // Description:
  // Initialize the hardware.  This is called automatically
  // on the first Update or Grab.
  virtual PlusStatus Initialize();
  virtual int GetInitialized() { return this->Initialized; };

  // Description:
  // Release the video driver.  This method must be called before
  // application exit, or else the application might hang during
  // exit.  
  virtual void ReleaseSystemResources();

  // Description:
  // The internal function which actually does the grab.  You will
  // definitely want to override this if you develop a vtkVideoSource2
  // subclass. 
  virtual PlusStatus InternalGrab();

  // Description:
  // And internal variable which marks the beginning of a Record session.
  // These methods are for internal use only.
  /*void SetStartTimeStamp(double t) { this->StartTimeStamp = t; };
  virtual double GetStartTimeStamp() { return this->StartTimeStamp; };*/

  // Description:
  // The result of GetOutput() will be the frame closest to DesiredTimestamp
  // if it is set and if UpdateWithDesiredTimestamp is set on (default off)
  vtkSetMacro(UpdateWithDesiredTimestamp, int);
  vtkGetMacro(UpdateWithDesiredTimestamp, int);
  vtkBooleanMacro(UpdateWithDesiredTimestamp, int);

  // Description:
  // The result of GetOutput() will be the frame closest to DesiredTimestamp
  // if it is set and if UpdateWithDesiredTimestamp is set on (default off)
  vtkSetMacro(DesiredTimestamp, double);
  vtkGetMacro(DesiredTimestamp, double);

  // Description:
  // Get the timestamp for the video frame returned with Desired timestamping
  vtkGetMacro(TimestampClosestToDesired, double);

  // Description:
  // Add generated html report from video data acquisition to the existing html report
  // htmlReport and plotter arguments has to be defined by the caller function
  // Solution should build with PLUS_PRINT_VIDEO_TIMESTAMP_DEBUG_INFO to generate this report
  virtual void GenerateVideoDataAcquisitionReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder); 

protected:
  vtkVideoSource2();
  virtual ~vtkVideoSource2();
  virtual int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual void CopyImageToFrame(unsigned char *outPtr, unsigned char *inPtr, int bytesInFrame, int outputFormat, int *dimensions, float opacity, int fileType);

  // Description:
  // Method for updating the virtual clock that accurately times the
  // arrival of each frame, more accurately than is possible with
  // the system clock alone because the virtual clock averages out the
  // jitter.
  virtual void CreateTimeStampForFrame(unsigned long frameNum, double &unfilteredTimestamp, double &filteredTimestamp);

  int Initialized;

  int ClipRegion[6];
  int OutputWholeExtent[6];
  double DataSpacing[3];
  double DataOrigin[3];
  // set according to the OutputFormat
  int NumberOfScalarComponents;
  // The FrameOutputExtent is the WholeExtent for a single output frame.
  // It is initialized in ExecuteInformation. 
  int FrameOutputExtent[6];

  // if we want to update according to the frame closest to the timestamp specifified by desiredTimestamp
  double DesiredTimestamp;
  int UpdateWithDesiredTimestamp;
  double TimestampClosestToDesired;

  // save this information from the output so that we can see if the
  // output scalars have changed
  int LastNumberOfScalarComponents;
  int LastOutputExtent[6];

  int Recording;
  float FrameRate;
  int FrameCount;
  unsigned long FrameNumber; 
  //double StartTimeStamp;
  double FrameTimeStamp;

  double LastTimeStamp;
  double LastUnfilteredTimeStamp;
  unsigned long LastFrameCount;
  double EstimatedFramePeriod;
  double MaximumFramePeriodJitter; 
  double SmoothingFactor; 
  std::vector<double> AveragedFramePeriods; 

  int NumberOfOutputFrames;

  // set if output needs to be cleared to be cleared before being written
  int OutputNeedsInitialization;

    // An example of asynchrony
  vtkMultiThreader *RecordThreader;
  int RecordThreadId;

  // The buffer used to hold the frames
  vtkVideoBuffer *Buffer;
  VideoBufferItem *CurrentVideoBufferItem; 

  // Description:
  // These methods can be overridden in subclasses
  virtual void UpdateFrameBuffer();
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  std::ofstream DebugInfoStream;

private:
  vtkVideoSource2(const vtkVideoSource2&);  // Not implemented.
  void operator=(const vtkVideoSource2&);  // Not implemented.

  vtkVideoFrame2* PseudoRandomNoiseFrame; 
};

#endif
