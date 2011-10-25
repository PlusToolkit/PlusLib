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

// .NAME vtkPlusVideoSource - Superclass of video input devices for VTK
// .SECTION Description
// vtkPlusVideoSource is a superclass for video input interfaces for VTK.
// This base class records input from a noise source.  vtkPlusVideoSource uses
// vtkVideoBuffer to hold its image data. The output can be accessed while
// recording.
// .SECTION Caveats
// You must call Disconnect() method before the application
// exits.  Otherwise the application might hang while trying to exit.
// .SECTION See Also
// vtkWin32VideoSource2 vtkMILVideoSource2 vtkVideoBuffer2

#ifndef __vtkPlusVideoSource_h
#define __vtkPlusVideoSource_h

#include "PlusConfigure.h"
#include <vector>
#include "vtkImageAlgorithm.h"
#include "vtkXMLDataElement.h"
#include "vtkMultiThreader.h"
#include "UsImageConverterCommon.h"
#include "PlusVideoFrame.h"

class vtkTimerLog;
class vtkCriticalSection;
class vtkMultiThreader;
class vtkScalarsToColors;
class vtkVideoBuffer;
class vtkImageWriter;
class vtkImageReader2;
class vtkHTMLGenerator; 
class vtkGnuplotExecuter; 
class VideoBufferItem;

class VTK_EXPORT vtkPlusVideoSource : public vtkImageAlgorithm
{
public:

    static vtkPlusVideoSource *New();
    vtkTypeRevisionMacro(vtkPlusVideoSource,vtkImageAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent);   

    // Description:
    // Read/write main configuration from/to xml data
    virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
    virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

    // Description:
    // Connect to device. Connection is needed for recording or single frame grabbing
    virtual PlusStatus Connect();

    // Description:
    // Disconnect from device
    // This method must be called before application exit, or else the
    // application might hang during exit.
    virtual PlusStatus Disconnect();

    // Description:
    // Record incoming video at the specified FrameRate.  The recording
    // continues indefinitely until Stop() is called. 
    virtual PlusStatus StartRecording();

    // Description:
    // Stop recording
    virtual PlusStatus StopRecording();

    // Description:
    // Grab a single video frame.
    // It requires overriding of the InternalGrab() function in the child class.
    virtual PlusStatus Grab();

    // Description:
    // Are we in record mode?
    vtkGetMacro(Recording,int);

    // Description:
    // Are we connected?
    vtkGetMacro(Connected, int);


    // Description:
    // Set/Get the full-frame size.  This must be an allowed size for the device,
    // the device may either refuse a request for an illegal frame size or
    // automatically choose a new frame size.
    // The default is usually 320x240, but can be device specific.  
    // The 'depth' should always be 1 (unless you have a device that
    // can handle 3D acquisition).
    virtual PlusStatus SetFrameSize(int x, int y);
    virtual PlusStatus SetFrameSize(int dim[2]) { return this->SetFrameSize(dim[0], dim[1]); };
    virtual int* GetFrameSize();
    virtual void GetFrameSize(int &x, int &y);
    virtual void GetFrameSize(int dim[2]);

    // Description:
    // Set/Get number of bits per pixel (by default: 8 bits/pixel)
    virtual PlusStatus SetPixelType(PlusCommon::ITKScalarPixelType pixelType);
    virtual PlusCommon::ITKScalarPixelType GetPixelType();
    //virtual unsigned int GetNumberOfBitsPerPixel();

    // Description:
    // Set/Get a particular frame rate (default 30 frames per second).
    virtual void SetFrameRate(float rate);
    vtkGetMacro(FrameRate,float);

    // Description:
    // Set/Get size of the frame buffer, i.e. the number of frames that
    // the 'tape' can store.
    virtual PlusStatus SetFrameBufferSize(int FrameBufferSize);
    virtual int GetFrameBufferSize();

    // Description:
    // Set/Get the number of frames to copy to the output on each execute.
    // The frames will be concatenated along the Z dimension, with the 
    // most recent frame first.  The default is 1.
    vtkSetMacro(NumberOfOutputFrames,int);
    vtkGetMacro(NumberOfOutputFrames,int);

    // Description:
    // This value is incremented each time a frame is grabbed.
    // reset it to zero (or any other value) at any time.
    vtkGetMacro(FrameCount, int);
    vtkSetMacro(FrameCount, int);

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
    // Set/get ultrasound image orientation of the device set
    vtkSetMacro(UsImageOrientation, US_IMAGE_ORIENTATION); 
    vtkGetMacro(UsImageOrientation, US_IMAGE_ORIENTATION); 

    // Description:
    // Add generated html report from video data acquisition to the existing html report
    // htmlReport and plotter arguments has to be defined by the caller function
    virtual PlusStatus GenerateVideoDataAcquisitionReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder); 

protected:
    vtkPlusVideoSource();
    virtual ~vtkPlusVideoSource();
    virtual int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

    static void *vtkVideoSourceRecordThread(vtkMultiThreader::ThreadInfo *data);

    // Description:
    // Should be overridden to connect to the hardware
    virtual PlusStatus InternalConnect() { return PLUS_SUCCESS; }

    // Description:
    // Should be overridden to disconnect from the hardware
    // Release the video driver.
    virtual PlusStatus InternalDisconnect() { return PLUS_SUCCESS; };

    // Description:
    // The internal function which actually grabs one frame. 
    // This must be overridden in the class if SpawnThreadForRecording is enabled
    // or the single frame grabbing (Grab()) functionality is needed.
    virtual PlusStatus InternalGrab();

    // Description:
    // Called at the end of StartRecording to allow hardware-specific
    // actions for starting the recording
    virtual PlusStatus InternalStartRecording() { return PLUS_SUCCESS; };

    // Description:
    // Called at the beginning of StopRecording to allow hardware-specific
    // actions for stopping the recording
    virtual PlusStatus InternalStopRecording() { return PLUS_SUCCESS; };

    // Description:
    // These methods can be overridden in subclasses
    virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

    ///////////////////////////

    int Connected;

    // Description:
    // If SpawnThreadForRecording is true then when recording is started
    // a new thread is created, which grabs frame in regular intervals.
    // SpawnThreadForRecording shall be set to false if the driver notifies
    // the application when a new frame is available (then the notification
    // triggers the frame grabbing)
    int SpawnThreadForRecording;

    // if we want to update according to the frame closest to the timestamp specifified by desiredTimestamp
    double DesiredTimestamp;
    int UpdateWithDesiredTimestamp;
    double TimestampClosestToDesired;

    int Recording;
    float FrameRate;
    int FrameCount;
    unsigned long FrameNumber; 
    double FrameTimeStamp;

    int NumberOfOutputFrames;

    // set if output needs to be cleared to be cleared before being written
    int OutputNeedsInitialization;

    // An example of asynchrony
    vtkMultiThreader *RecordThreader;
    int RecordThreadId;

    // The buffer used to hold the last N frames
    vtkVideoBuffer *Buffer;
    VideoBufferItem *CurrentVideoBufferItem; 

    US_IMAGE_ORIENTATION UsImageOrientation; 

private:
    vtkPlusVideoSource(const vtkPlusVideoSource&);  // Not implemented.
    void operator=(const vtkPlusVideoSource&);  // Not implemented.
};

#endif
