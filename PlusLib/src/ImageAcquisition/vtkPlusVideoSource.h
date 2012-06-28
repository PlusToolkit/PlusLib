/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
The following copyright notice is applicable to parts of this file:
Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
Authors include: Danielle Pace
(Robarts Research Institute and The University of Western Ontario)
=========================================================================*/ 

#ifndef __vtkPlusVideoSource_h
#define __vtkPlusVideoSource_h

#include "PlusConfigure.h"
#include "vtkPlusDevice.h"
#include "vtkImageAlgorithm.h"
#include "vtkXMLDataElement.h"
#include "vtkMultiThreader.h"
#include "PlusVideoFrame.h"

class vtkVideoBuffer;
class vtkHTMLGenerator;
class vtkGnuplotExecuter;
class VideoBufferItem;

/*!
\class vtkPlusVideoSource
\brief Abstract base class for video sources

vtkPlusVideoSource is a superclass for video input interfaces for VTK.
This base class records input from a noise source.  vtkPlusVideoSource uses
vtkVideoBuffer to hold its image data. The output can be accessed while
recording.
\sa vtkWin32VideoSource2 vtkMILVideoSource2 vtkVideoBuffer2
\ingroup PlusLibImageAcquisition
*/

class VTK_EXPORT vtkPlusVideoSource : public vtkPlusDevice
{
public:

  static vtkPlusVideoSource *New();
  void PrintSelf(ostream& os, vtkIndent indent);   

  /*! Read main configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 

  /*! Write main configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Connect to device. Connection is needed for recording or single frame grabbing */
  virtual PlusStatus Connect();

  /*!
    Disconnect from device.
    This method must be called before application exit, or else the
    application might hang during exit.
  */
  virtual PlusStatus Disconnect();

  /*!
    Record incoming video at the specified FrameRate.  The recording
    continues indefinitely until Stop() is called. 
  */
  virtual PlusStatus StartRecording();

  /*! Stop recording */
  virtual PlusStatus StopRecording();

  /*!
    Grab a single video frame.
    It requires overriding of the InternalGrab() function in the child class.
  */
  virtual PlusStatus Grab();

  /*! Get tracked frame containing the image acquired from the device at a specific timestamp */
  virtual PlusStatus GetTrackedFrame(double timestamp, TrackedFrame *trackedFrame);

  /*!
    Add generated html report from video data acquisition to the existing html report
    htmlReport and plotter arguments has to be defined by the caller function
  */
  virtual PlusStatus GenerateVideoDataAcquisitionReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter); 

  /*! Set recording start time */
  virtual void SetStartTime( double startTime );  
  
public:
  /*! Are we connected? */
  vtkGetMacro(Connected, int);

  /*!
    Set the full-frame size.  This must be an allowed size for the device,
    the device may either refuse a request for an illegal frame size or
    automatically choose a new frame size.
  */
  virtual PlusStatus SetFrameSize(int x, int y);

  /*!
    Set the full-frame size.  This must be an allowed size for the device,
    the device may either refuse a request for an illegal frame size or
    automatically choose a new frame size.
  */
  virtual PlusStatus SetFrameSize(int dim[2]) { return this->SetFrameSize(dim[0], dim[1]); };

  /*! Get the full-frame size */
  virtual int* GetFrameSize();

  /*! Get the full-frame size */
  virtual void GetFrameSize(int &x, int &y);

  /*! Get the full-frame size */
  virtual void GetFrameSize(int dim[2]);

  /*! Set the image pixel type */
  virtual PlusStatus SetPixelType(PlusCommon::ITKScalarPixelType pixelType);
  /*! Get the image pixel type */
  virtual PlusCommon::ITKScalarPixelType GetPixelType();
  //virtual unsigned int GetNumberOfBitsPerPixel();

  /*! Set the requested frame rate. Actual frame rate may be different. (default 30 frames per second). */
  virtual PlusStatus SetAcquisitionRate(double rate);

  /*! 
    Set size of the internal frame buffer, i.e. the number of most recent frames that
    are stored in the video source class internally.
  */
  virtual PlusStatus SetFrameBufferSize(int FrameBufferSize);
  /*! Get size of the internal frame buffer. */
  virtual int GetFrameBufferSize();

  /*! 
    Set the number of frames to copy to the output on each execute.
    The frames will be concatenated along the Z dimension, with the 
    most recent frame first.  The default is 1.
  */
  vtkSetMacro(NumberOfOutputFrames,int);
  /*! Get the number of frames to copy to the output on each execute. */
  vtkGetMacro(NumberOfOutputFrames,int);

  /*!
    Get the frame number (some devices has frame numbering, otherwise 
    just increment if new frame received)
  */
  vtkGetMacro(FrameNumber, unsigned long);

  /*!
    Get a time stamp in seconds (resolution of milliseconds) for
    the most recent frame.  Time began on Jan 1, 1970.  This timestamp is only
    valid after the Output has been Updated.  Usually set to the
    timestamp for the output if UpdateWithDesiredTimestamp is off,
    otherwise it is the timestamp for the most recent frame, which is not
    necessarily the output
  */
  virtual double GetFrameTimeStamp() { return this->FrameTimeStamp; };

  /*! Get the buffer that is used to hold the video frames. */
  virtual vtkVideoBuffer *GetBuffer() { return this->Buffer; };

  /*! 
    Get the buffer that is used to hold the video frames
	  There are cases when multiple externally controlled buffers are needed.
	  There must always be a valid buffer in the video source object, therefore
    the input parameter shall not be NULL.
  */
  virtual PlusStatus SetBuffer(vtkVideoBuffer *newBuffer);

  /*!
    The result of GetOutput() will be the frame closest to DesiredTimestamp
    if it is set and if UpdateWithDesiredTimestamp is set on (default off)
  */
  vtkSetMacro(UpdateWithDesiredTimestamp, int);
  /*!
    The result of GetOutput() will be the frame closest to DesiredTimestamp
    if it is set and if UpdateWithDesiredTimestamp is set on (default off)
  */
  vtkGetMacro(UpdateWithDesiredTimestamp, int);
  /*!
    The result of GetOutput() will be the frame closest to DesiredTimestamp
    if it is set and if UpdateWithDesiredTimestamp is set on (default off)
  */
  vtkBooleanMacro(UpdateWithDesiredTimestamp, int);

  /*!
    Set the desired timestamp. The result of GetOutput() will be the frame closest to DesiredTimestamp
    if it is set and if UpdateWithDesiredTimestamp is set on (default off)
  */
  vtkSetMacro(DesiredTimestamp, double);
  /*! Get the desired timestamp */
  vtkGetMacro(DesiredTimestamp, double);

  /*! Get the timestamp for the video frame returned with desired timestamping */
  vtkGetMacro(TimestampClosestToDesired, double);

  /*! Set ultrasound image orientation of the device set */
  vtkSetMacro(UsImageOrientation, US_IMAGE_ORIENTATION); 
  /*! Get ultrasound image orientation of the device set */
  vtkGetMacro(UsImageOrientation, US_IMAGE_ORIENTATION);

protected:
  vtkPlusVideoSource();
  virtual ~vtkPlusVideoSource();

  virtual int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  static void *vtkVideoSourceRecordThread(vtkMultiThreader::ThreadInfo *data);

  /*! Should be overridden to connect to the hardware */
  virtual PlusStatus InternalConnect() { return PLUS_SUCCESS; }

  /*! Release the video driver. Should be overridden to disconnect from the hardware. */ 
  virtual PlusStatus InternalDisconnect() { return PLUS_SUCCESS; };

  /*!
    The internal function which actually grabs one frame. 
    This must be overridden in the class if SpawnThreadForRecording is enabled
    or the single frame grabbing (Grab()) functionality is needed.
  */
  virtual PlusStatus InternalGrab();

  /*!
    Called at the end of StartRecording to allow hardware-specific
    actions for starting the recording
  */
  virtual PlusStatus InternalStartRecording() { return PLUS_SUCCESS; };

  /*! 
    Called at the beginning of StopRecording to allow hardware-specific
    actions for stopping the recording
  */
  virtual PlusStatus InternalStopRecording() { return PLUS_SUCCESS; };

  /*! Return the latest or desired image frame. This method can be overridden in subclasses */
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

protected:
  int Connected;

  /*!
    If SpawnThreadForRecording is true then when recording is started
    a new thread is created, which grabs frame in regular intervals.
    SpawnThreadForRecording shall be set to false if the driver notifies
    the application when a new frame is available (then the notification
    triggers the frame grabbing)
  */
  bool SpawnThreadForRecording;

  /*! Flag to strore recording thread state */
  bool RecordingThreadAlive; 

  /*! if we want to update according to the frame closest to the timestamp specifified by desiredTimestamp */
  double DesiredTimestamp;
  int UpdateWithDesiredTimestamp;
  double TimestampClosestToDesired;

  /*! Requested frame rate in FPS. Actual frame rate may be different. TODO: Is it really the FPS? -Csaba */
  unsigned long FrameNumber; 
  double FrameTimeStamp;

  int NumberOfOutputFrames;

  /*! Set if output needs to be cleared to be cleared before being written */
  int OutputNeedsInitialization;

  /*! The buffer used to hold the last N frames */
  vtkVideoBuffer *Buffer;
  VideoBufferItem *CurrentVideoBufferItem; 

  US_IMAGE_ORIENTATION UsImageOrientation; 

private:
  vtkPlusVideoSource(const vtkPlusVideoSource&);  // Not implemented.
  void operator=(const vtkPlusVideoSource&);  // Not implemented.
};

#endif
