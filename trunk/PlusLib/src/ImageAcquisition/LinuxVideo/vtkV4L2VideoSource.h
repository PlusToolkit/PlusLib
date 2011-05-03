/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkV4L2VideoSource.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
  
  Author:  Jan Gumprecht, Harvard Medical School
  Copyright (c) 2008, Brigham and Women's Hospital, Boston, MA
  All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

  
//New-Start
// .NAME vtkV4L2VideoSource - Superclass of video input devices for VTK
// .SECTION Description
// vtkV4L2VideoSource is a superclass for video input interfaces for VTK.
// The goal is to provide an interface which is very similar to the
// interface of a VCR, where the 'tape' is an internal frame buffer
// capable of holding a preset number of video frames.  Specialized
// versions of this class record input from various video input sources.
// This base class records input from a noise source.
// .SECTION Caveats
// You must call the ReleaseSystemResources() method before the application
// exits.  Otherwise the application might hang while trying to exit.
// .SECTION See Also
// vtkWin32VideoSource vtkMILVideoSource
//New-End

#ifndef __vtkV4L2VideoSource_h
#define __vtkV4L2VideoSource_h

//New-Start
#include <linux/videodev2.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <malloc.h>

#include <getopt.h> 
//New-End

#include "vtkUltrasoundWin32Header.h"
#include "vtkVideoSource.h"

//New-Start
#include "vtkImageAlgorithm.h"

class vtkTimerLog;
class vtkCriticalSection;
class vtkMultiThreader;
class vtkScalarsToColors;
//New-End

#define BITS_PER_PIXEL 8


//Imaging mode | not used in the simualtor. Only here for consistency.
enum {BMode};

//Acquistiontypes | not used in the simualtor. Only here for consistency.
enum {udtBPost};

//BTX

//class uDataDesc;
//class ulterius;

class VTK_ULTRASOUND_EXPORT vtkV4L2VideoSource;

//class VTK_EXPORT vtkV4L2VideoSource : public vtkVideoSource
class VTK_HYBRID_EXPORT vtkV4L2VideoSource : public vtkVideoSource
{
public:

  typedef enum {
    IO_METHOD_READ,
    IO_METHOD_MMAP,
    IO_METHOD_USERPTR,
  } io_method;

  struct buffer {
        void *                  start;
        size_t                  length;
  };

  vtkTypeRevisionMacro(vtkV4L2VideoSource,vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);   
  // Description:
  // This is a singleton pattern New.  There will only be ONE
  // reference to a vtkOutputWindow object per process.  Clients that
  // call this must call Delete on the object so that the reference
  // counting will work.   The single instance will be unreferenced when
  // the program exits.
  static vtkV4L2VideoSource* New();
  
  // Description:
  // Record incoming video at the specified FrameRate.  The recording
  // continues indefinitely until Stop() is called. 
  virtual void Record();

  // Description:
  // Play through the 'tape' sequentially at the specified frame rate.
  // If you have just finished Recoding, you should call Rewind() first.
  virtual void Play();

  // Description:
  // Stop recording or playing.
  virtual void Stop();
  
//New-Start
  // Description:
  // Rewind to the frame with the earliest timestamp.  Record operations
  // will start on the following frame, therefore if you want to re-record
  // over this frame you must call Seek(-1) before calling Grab() or Record().
  void Rewind();

  // Description:
  // FastForward to the last frame that was recorded (i.e. to the frame
  // that has the most recent timestamp).
  void FastForward();

  // Description:
  // Seek forwards or backwards by the specified number of frames
  // (positive is forward, negative is backward).
  void Seek(int n); 
//New-End

  // Description:
  // Grab a single video frame.
  void Grab();

//New-Start
  // Description:
  // Are we in record mode? (record mode and play mode are mutually
  // exclusive).
  vtkGetMacro(Recording,int);

  // Description:
  // Are we in play mode? (record mode and play mode are mutually
  // exclusive).
  vtkGetMacro(Playing,int);  
  
  // Description:
  // Set the full-frame size.  This must be an allowed size for the device,
  // the device may either refuse a request for an illegal frame size or
  // automatically choose a new frame size.
  // The default is usually 320x240x1, but can be device specific.  
  // The 'depth' should always be 1 (unless you have a device that
  // can handle 3D acquisition).
  void SetFrameSize(int x, int y, int z);
  void SetFrameSize(int dim[3]) { 
    this->SetFrameSize(dim[0], dim[1], dim[2]); };
  vtkGetVector3Macro(FrameSize,int);
  
  // Description:
  // Request a particular frame rate (default 30 frames per second).
  void SetFrameRate(float rate);
  vtkGetMacro(FrameRate,float);
  
  // Description:
  // Set the output format.  This must be appropriate for device,
  // usually only VTK_LUMINANCE, VTK_RGB, and VTK_RGBA are supported.
  void SetOutputFormat(int format);
  void SetOutputFormatToLuminance() { this->SetOutputFormat(VTK_LUMINANCE); };
  void SetOutputFormatToRGB() { this->SetOutputFormat(VTK_RGB); };
  void SetOutputFormatToRGBA() { this->SetOutputFormat(VTK_RGBA); };
  vtkGetMacro(OutputFormat,int);
  
  // Description:
  // Set size of the frame buffer, i.e. the number of frames that
  // the 'tape' can store.
  void SetFrameBufferSize(int FrameBufferSize);
  vtkGetMacro(FrameBufferSize,int);
  
  // Description:
  // Set the number of frames to copy to the output on each execute.
  // The frames will be concatenated along the Z dimension, with the 
  // most recent frame first.
  // Default: 1
  vtkSetMacro(NumberOfOutputFrames,int);
  vtkGetMacro(NumberOfOutputFrames,int);

  // Description:
  // Set whether to automatically advance the buffer before each grab. 
  // Default: on
  vtkBooleanMacro(AutoAdvance,int);
  vtkSetMacro(AutoAdvance,int)
  vtkGetMacro(AutoAdvance,int);
  
  // Description:
  // Set the clip rectangle for the frames.  The video will be clipped 
  // before it is copied into the framebuffer.  Changing the ClipRegion
  // will destroy the current contents of the framebuffer.
  // The default ClipRegion is (0,VTK_INT_MAX,0,VTK_INT_MAX,0,VTK_INT_MAX).
  virtual void SetClipRegion(int r[6]) { 
    this->SetClipRegion(r[0],r[1],r[2],r[3],r[4],r[5]); };
  virtual void SetClipRegion(int x0, int x1, int y0, int y1, int z0, int z1);
  vtkGetVector6Macro(ClipRegion,int);
  
    // Description:
  // Get/Set the WholeExtent of the output.  This can be used to either
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
  // For RGBA output only (4 scalar components), set the opacity.  This
  // will not modify the existing contents of the framebuffer, only
  // subsequently grabbed frames.
  vtkSetMacro(Opacity,float);
  vtkGetMacro(Opacity,float);  

  // Description:
  // This value is incremented each time a frame is grabbed.
  // reset it to zero (or any other value) at any time.
  vtkGetMacro(FrameCount, int);
  vtkSetMacro(FrameCount, int);

  // Description:
  // Get the frame index relative to the 'beginning of the tape'.  This
  // value wraps back to zero if it increases past the FrameBufferSize.
  vtkGetMacro(FrameIndex, int);

  //Description:
  // This value sets the input channel of the framegrabber card. Input channel 3
  // for example is the S-Video port at a Hauppauge Impact VCB modell 558
  vtkSetMacro(VideoChannel, int);
  vtkGetMacro(VideoChannel, int);

  //Description:
  // Available VideoModes: NTSC == 1, PAL == 2
  vtkSetMacro(VideoMode, int);  
  vtkGetMacro(VideoMode, int);
  
  // Description:
  // Get a time stamp in seconds (resolution of milliseconds) for
  // a video frame.   Time began on Jan 1, 1970.  You can specify
  // a number (negative or positive) to specify the position of the
  // video frame relative to the current frame.
  double GetFrameTimeStamp(int frame);
  
  // Description:
  // Get a time stamp in seconds (resolution of milliseconds) for
  // the Output.  Time began on Jan 1, 1970.  This timestamp is only
  // valid after the Output has been Updated.
  double GetFrameTimeStamp() { return this->FrameTimeStamp; };

//New-End

  // Description:
  // Initialize the driver (this is called automatically when the
  // first grab is done).
  void Initialize();
  //New-Start
  int GetInitialized() { return this->Initialized; };
  //New-End

  // Description:
   // Release the video driver.  This method must be called before
  // application exit, or else the application might hang during
  // exit. Automatically done at the destructor
  void ReleaseSystemResources();
  
  // Description:
  // The internal function which actually does the grab.  You will
  // definitely want to override this if you develop a vtkV4L2VideoSource
  // subclass. 
   void InternalGrab();
   
   // Description:
  // And internal variable which marks the beginning of a Record session.
  // These methods are for internal use only.
  void SetStartTimeStamp(double t) { this->StartTimeStamp = t; };
  double GetStartTimeStamp() { return this->StartTimeStamp; };
  
  // Set video device
  void SetVideoDevice(const char* device);

protected:
  vtkV4L2VideoSource();
  ~vtkV4L2VideoSource();

  int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  
  //New-Start    
  int Initialized;
  int DeviceInitialized;

  int FrameSize[3];
  int ClipRegion[6];
  int OutputWholeExtent[6];
  double DataSpacing[3];
  double DataOrigin[3];
  int OutputFormat;
  // set according to the OutputFormat
  int NumberOfScalarComponents;
  // The FrameOutputExtent is the WholeExtent for a single output frame.
  // It is initialized in ExecuteInformation. 
  int FrameOutputExtent[6];

  // save this information from the output so that we can see if the
  // output scalars have changed
  int LastNumberOfScalarComponents;
  int LastOutputExtent[6];

  int Recording;
  int Playing;
  float FrameRate;

  int FrameCount;
  int FrameIndex;
  double StartTimeStamp;
  double FrameTimeStamp;

  int AutoAdvance;
  int NumberOfOutputFrames;

  float Opacity;

  // true if Execute() must apply a vertical flip to each frame
  int FlipFrames;

  // set if output needs to be cleared to be cleared before being written
  int OutputNeedsInitialization;

  // An example of asynchrony
  vtkMultiThreader *PlayerThreader;
  int PlayerThreadId;

  // A mutex for the frame buffer: must be applied when any of the
  // below data is modified.
  vtkCriticalSection *FrameBufferMutex;

  // set according to the needs of the hardware:
  // number of bits per framebuffer pixel
  int FrameBufferBitsPerPixel;
  // byte alignment of each row in the framebuffer
  int FrameBufferRowAlignment;
  // FrameBufferExtent is the extent of frame after it has been clipped 
  // with ClipRegion.  It is initialized in CheckBuffer().
  int FrameBufferExtent[6];

  int FrameBufferSize;
  int FrameBufferIndex;
  void **FrameBuffer;
  double *FrameBufferTimeStamps;

  int VideoChannel; //S-Video is channel 3 at Hauppauge Impact VCB Modell 558
  int VideoMode; //NTSC == 1, PAL == 2

  // Description:
  // These methods can be overridden in subclasses
  void UpdateFrameBuffer();
  void AdvanceFrameBuffer(int n);
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  // if some component conversion is required, it is done here:
  void UnpackRasterLine(char *outPtr, char *rowPtr, 
  int start, int count);
  //New-End    

private:
  //New-Start    
   struct buffer *         buffers;

   static char             dev_name[256];
   static io_method        io;
   static int              fd;
   static unsigned int     n_buffers;
   static const char short_options [];
   static const struct option long_options [];
 

  vtkV4L2VideoSource(const vtkV4L2VideoSource&);  // Not implemented.
  void operator=(const vtkV4L2VideoSource&);  // Not implemented.
  
  int xioctl(int fd, int request, void *arg);
   
   void OpenDevice(void);
   void CloseDevice(void);
   void InitDevice();
   void UninitDevice(void);
   
   void InitRead(unsigned int buffer_size);
   void InitMmap(void);
   void InitUserp(unsigned int buffer_size);

   void errno_exit (const char *s);
  //New-End    
};

#endif





