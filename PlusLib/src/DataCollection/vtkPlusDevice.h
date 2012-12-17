/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusDevice_h
#define __vtkPlusDevice_h

#include "PlusConfigure.h"
#include "TrackedFrame.h"
#include "vtkImageAlgorithm.h"
#include "vtkMultiThreader.h"
#include "vtkPlusDeviceTypes.h"
#include "vtkTrackedFrameList.h"
#include <string>

class vtkGnuplotExecuter;
class vtkHTMLGenerator;
class vtkPlusStream;
class vtkPlusStreamBuffer;
class vtkPlusStreamTool;
class vtkRfProcessor;
class vtkXMLDataElement;

/*!
\class vtkPlusDevice 
\brief Abstract interface for tracker and video devices

vtkPlusDevice is an abstract VTK interface to real-time tracking and imaging
systems.  Derived classes should override the Connect(), Disconnect(), 
GetSdkVersion(), ReadConfiguration(), WriteConfiguration() methods.

\ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkPlusDevice : public vtkImageAlgorithm
{
public:
  static vtkPlusDevice *New();
  vtkTypeRevisionMacro(vtkPlusDevice, vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  /*! 
    Probe to see to see if the device is connected to the 
    computer.  This method should be overridden in subclasses. 
  */
  virtual PlusStatus Probe();

  /*! Hardware device SDK version. This method should be overridden in subclasses. */
  virtual std::string GetSdkVersion();

  /*! Read main configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement*);

  /*! Write main configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement*);

  /*! Connect to device. Connection is needed for recording or single frame acquisition */
  virtual PlusStatus Connect();

  /*!
    Force the hardware to device to acquire a frame
    It requires overriding of the InternalUpdate() function in the child class.
  */
  virtual PlusStatus ForceUpdate();

  /*!
    Disconnect from device.
    This method must be called before application exit, or else the
    application might hang during exit.
  */
  virtual PlusStatus Disconnect();

  /*!
    Get tracked frame containing the transform(s) or the
    image(s) acquired from the device at a specific timestamp
  */
  virtual PlusStatus GetTrackedFrame(double timestamp, TrackedFrame& trackedFrame);
  virtual PlusStatus GetTrackedFrame(TrackedFrame *trackedFrame);

  /*!
    Get the tracked frame list from devices since time specified
    \param aTimestamp The oldest timestamp we search for in the buffer. If -1 get all frames in the time range since the most recent timestamp. Out parameter - changed to timestamp of last added frame
    \param aTrackedFrameList Tracked frame list used to get the newly acquired frames into. The new frames are appended to the tracked frame.
    \param aSamplingRateSec Sampling rate for getting the frames in seconds (timestamps are in seconds too)
    \param maxTimeLimitSec Maximum time spent in the function (in sec)
  */
  virtual PlusStatus GetTrackedFrameListSampled(double& aTimestamp, vtkTrackedFrameList* aTrackedFrameList, double aSamplingRateSec, double maxTimeLimitSec=-1); 

  PlusStatus GetTrackedFrameList( double& aTimestampFrom, vtkTrackedFrameList* aTrackedFrameList, int aMaxNumberOfFramesToAdd );

  /*! 
    Get the tracked frame from devices by time with each tool transforms
    \param time The closes frame to this timestamp will be retrieved
    \param trackedFrame The output where the tracked frame information will be copied
  */
  virtual PlusStatus GetTrackedFrameByTime(double time, TrackedFrame* trackedFrame); 

  /*!
    Return whether or not the device can be reset
  */
  virtual bool IsResettable();

  bool GetTrackingDataAvailable();
  bool GetVideoDataAvailable();
  bool GetTrackingEnabled() const;
  
  /*!
    Record incoming data at the specified acquisition rate.  The recording
    continues indefinitely until StopRecording() is called. 
  */
  virtual PlusStatus StartRecording();

  /*! Stop recording */
  virtual PlusStatus StopRecording();

    /*! Get the buffer that is used to hold the data. */
  virtual vtkSmartPointer<vtkPlusStreamBuffer> GetBuffer();
  virtual vtkSmartPointer<vtkPlusStreamBuffer> GetBuffer(int port);

  /*! 
    Get the buffer that is used to hold the data
	  There are cases when multiple externally controlled buffers are needed.
	  There must always be a valid buffer in the data object, therefore
    the input parameter shall not be NULL.
  */
  //virtual PlusStatus SetBuffer(vtkPlusStreamBuffer* newBuffer);

  /*! 
    Set size of the internal frame buffer, i.e. the number of most recent frames that
    are stored in the video source class internally.
  */
  virtual PlusStatus SetBufferSize(int FrameBufferSize, const char* toolName = NULL);
  /*! Get size of the internal frame buffer. */
  virtual PlusStatus GetBufferSize(int& outVal, const char * toolName = NULL);

  /*! Set recording start time */
  virtual void SetStartTime( double startTime );

  /*! Get recording start time */
  virtual double GetStartTime();

  /*! Make a request for the latest image frame */
  vtkImageData* GetBrightnessOutput();

  /*! Return the dimensions of the brightness frame size */
  PlusStatus GetBrightnessFrameSize(int aDim[2]);

  /*!
    Reset the device
  */
  virtual PlusStatus Reset();

  /*! Clear all tool buffers */
  void ClearAllBuffers();

  /*! Dump the current state of the device to metafile (with each tools and buffers) */
  virtual PlusStatus WriteToMetafile(const char* outputFolder, const char* metaFileName, bool useCompression = false );

  /*! Make this device into a copy of another device. */
  void DeepCopy(vtkPlusDevice* device);

  /*! Get the internal update rate for this tracking system.  This is the number of buffer entry items sent by the device per second (per tool). */
  double GetInternalUpdateRate() const;

  /*! Get the tool object for the specified tool name */
  PlusStatus GetTool(const char* aToolName, vtkSmartPointer<vtkPlusStreamTool> &aTool);

  /*! Get the first active tool object */
  PlusStatus GetFirstActiveTool(vtkSmartPointer<vtkPlusStreamTool> &aTool); 

  /*! Get the tool object for the specified tool port name */
  PlusStatus GetToolByPortName( const char* aPortName, vtkSmartPointer<vtkPlusStreamTool> &aTool); 

  /*! Get the beginning of the tool iterator */
  ToolContainerConstIterator GetToolIteratorBegin() const; 

  /*! Get the end of the tool iterator */
  ToolContainerConstIterator GetToolIteratorEnd() const;

  /*! Add tool to the tracker */
  PlusStatus AddTool( vtkSmartPointer<vtkPlusStreamTool> tool ); 

  /*! Get number of tools */
  int GetNumberOfTools() const;

  /*! Convert tool status to string */
  static std::string ConvertToolStatusToString(ToolStatus status); 

  /*! Convert tool status to TrackedFrameFieldStatus */
  static TrackedFrameFieldStatus ConvertToolStatusToTrackedFrameFieldStatus(ToolStatus status); 

  /*! Convert TrackedFrameFieldStatus to tool status */
  static ToolStatus ConvertTrackedFrameFieldStatusToToolStatus(TrackedFrameFieldStatus fieldStatus); 

  /*! Set Reference name of the tools */
  vtkSetStringMacro(ToolReferenceFrameName);

  /*! Get Reference name of the tools */
  vtkGetStringMacro(ToolReferenceFrameName);

  /*! Set buffer size of all available tools */
  void SetToolsBufferSize( int aBufferSize ); 

  /*! Set local time offset of all available buffers */
  void SetLocalTimeOffsetSec( double aTimeOffsetSec );
  void SetToolLocalTimeOffsetSec( double aTimeOffsetSec );

  /*! Make the unit emit a string of audible beeps.  This is supported by the POLARIS. */
  void Beep(int n);

  /*!
    Turn one of the LEDs on the specified tool on or off.  This is supported by the POLARIS.
  */
  void SetToolLED(const char* portName, int led, int state);

  /*! 
    Add generated html report from data acquisition to the existing html report. 
    htmlReport and plotter arguments has to be defined by the caller function 
  */
  virtual PlusStatus GenerateDataAcquisitionReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter ); 

  /*! Return the most recent synchronized timestamp in the buffers */
  virtual PlusStatus GetMostRecentTimestamp(double &ts); 

  /*! Return the oldest synchronized timestamp in the buffers */
  virtual PlusStatus GetOldestTimestamp(double &ts); 

  /*! Get the closest tracked frame timestamp to the specified time */
  double GetClosestTrackedFrameTimestampByTime(double time);

  /*! 
  The subclass will do all the hardware-specific update stuff
  in this function. It should call ToolUpdate() for each tool.
  Note that vtkPlusDevice.cxx starts up a separate thread after
  InternalStartRecording() is called, and that InternalUpdate() is
  called repeatedly from within that thread.  Therefore, any code
  within InternalUpdate() must be thread safe.  You can temporarily
  pause the thread by locking this->UpdateMutex->Lock() e.g. if you
  need to communicate with the device from outside of InternalUpdate().
  A call to this->UpdateMutex->Unlock() will resume the thread.
  */
  virtual PlusStatus InternalUpdate() { return PLUS_SUCCESS; };

  //BTX
  // These are used by static functions in vtkPlusDevice.cxx, and since
  // VTK doesn't generally use 'friend' functions they are public
  // instead of protected.  Do not use them anywhere except inside
  // vtkPlusDevice.cxx.
  vtkRecursiveCriticalSection* UpdateMutex;
  vtkTimeStamp UpdateTime;
  double InternalUpdateRate;  
  //ETX

  /*! Set the acquisition rate */
  virtual double GetAcquisitionRate() const;
  PlusStatus SetAcquisitionRate(double aRate);

  /*! Get whether recording is underway */
  bool IsRecording() const { return Recording == 1; }

  /* Return the id of the device */
  virtual char* GetDeviceId() const { return this->DeviceId; }
  // Set the device Id
  vtkSetStringMacro(DeviceId);

  vtkGetMacro(Selectable, bool);

  /*! Set the native ultrasound image orientation that the device acquires */
  vtkSetMacro(DeviceImageOrientation, US_IMAGE_ORIENTATION); 
  /*! Get the native ultrasound image orientation that the device acquires */
  vtkGetMacro(DeviceImageOrientation, US_IMAGE_ORIENTATION);

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
  //virtual int* GetFrameSize();

  /*! Get the full-frame size */
  virtual PlusStatus GetFrameSize(int &x, int &y);

  /*! Get the full-frame size */
  virtual PlusStatus GetFrameSize(int dim[2]);

  /*! Set the pixel type (char, unsigned short, ...) */
  virtual PlusStatus SetPixelType(PlusCommon::ITKScalarPixelType pixelType);
  /*! Get the pixel type (char, unsigned short, ...) */
  virtual PlusCommon::ITKScalarPixelType GetPixelType();

  /*! Set the image type (B-mode, RF, ...) provided by the video source. */
  virtual PlusStatus SetImageType(US_IMAGE_TYPE imageType);
  /*! Get the image pixel type (B-mode, RF, ...) */
  virtual US_IMAGE_TYPE GetImageType();

  /*! Access the available output streams */
  PlusStatus GetStreamByName(vtkSmartPointer<vtkPlusStream>& aStream, const char * aStreamName);

  /*! Add an input stream */
  PlusStatus AddInputStream(vtkSmartPointer<vtkPlusStream> aStream);

  /*!
    Perform any completion tasks once configured
  */
  virtual PlusStatus NotifyConfigured(){ return PLUS_SUCCESS; }

  /*! 
    Return the latest or desired image frame. This method can be overridden in subclasses 
    Part of the vtkAlgorithm pipeline
  */
  virtual int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  /*! 
    Return the latest or desired image frame. This method can be overridden in subclasses 
    Part of the vtkAlgorithm pipeline
  */
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

protected:
  vtkSetMacro(Selectable, bool);

  static void *vtkDataCaptureThread(vtkMultiThreader::ThreadInfo *data);

  /*! Get number of tracked frames between two given timestamps (inclusive) */
  int GetNumberOfFramesBetweenTimestamps(double aTimestampFrom, double aTimestampTo);

  /*! 
    This method should be overridden for devices that have one or more LEDs on the tracked tools. 
  */
  virtual PlusStatus InternalSetToolLED(const char* portName, int led, int state) { return PLUS_SUCCESS; };

  /*! This method should be overridden in derived classes that can make an audible beep. */
  virtual PlusStatus InternalBeep(int n) { return PLUS_SUCCESS; };

  /*! Should be overridden to connect to the hardware */
  virtual PlusStatus InternalConnect() { return PLUS_SUCCESS; }

  /*! Release the video driver. Should be overridden to disconnect from the hardware. */ 
  virtual PlusStatus InternalDisconnect() { return PLUS_SUCCESS; };

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

  /*! 
  This function is called by InternalUpdate() so that the subclasses
  can communicate information back to the vtkTracker base class, which
  will in turn relay the information to the appropriate vtkPlusStreamTool.
  */
  PlusStatus ToolTimeStampedUpdate(const char* aToolName, vtkMatrix4x4 *matrix, ToolStatus status, unsigned long frameNumber, double unfilteredtimestamp);

  /*! 
  This function is called by InternalUpdate() so that the subclasses
  can communicate information back to the vtkTracker base class, which
  will in turn relay the information to the appropriate vtkPlusStreamTool.
  This function is for devices has no frame numbering, just auto increment tool frame number if new frame received
  */
  PlusStatus ToolTimeStampedUpdateWithoutFiltering(const char* aToolName, vtkMatrix4x4 *matrix, ToolStatus status, double unfilteredtimestamp, double filteredtimestamp);

  /*!
    Helper function used during configuration to locate the correct XML element for a device
  */
  vtkXMLDataElement* FindThisDeviceElement(vtkXMLDataElement* rootXMLElement);
  /*!
    Helper function used during configuration to locate the correct XML element for an output stream
  */
  vtkXMLDataElement* FindOutputStreamElement(vtkXMLDataElement* rootXMLElement, const char* aStreamId);
  /*!
    Helper function used during configuration to locate the correct XML element for an input stream
  */
  vtkXMLDataElement* FindInputStreamElement(vtkXMLDataElement* rootXMLElement, const char* aStreamId);
  /*!
    Method that writes output streams to XML
  */
  virtual void InternalWriteOutputStreams(vtkXMLDataElement* rootXMLElement);
  /*!
    Method that writes output streams to XML
  */
  virtual void InternalWriteInputStreams(vtkXMLDataElement* rootXMLElement);

  vtkPlusDevice();
  virtual ~vtkPlusDevice();

protected:
  static const int VIRTUAL_DEVICE_FRAME_RATE;

  /*! Flag to store recording thread state */
  bool ThreadAlive; 

  /* Is device connected */
  int Connected;

  /*! Thread used for acquisition */
  vtkMultiThreader* Threader;

  /*! Recording thread id */
  int ThreadId;

  StreamContainer OutputStreams;
  StreamContainer InputStreams;
  vtkSmartPointer<vtkPlusStream> CurrentStream;

  /*! A stream buffer item to use as a temporary staging point */
  StreamBufferItem* CurrentStreamBufferItem;
  /*! Tracker tools */
  ToolContainer Tools; 
  /*! Reference name of the tools */
  char* ToolReferenceFrameName; 

  /*! Id of the device */
  char* DeviceId;

  /*! The orientation of the image in the buffer if there is one */
  US_IMAGE_ORIENTATION DeviceImageOrientation; 

  /*! Acquisition rate */
  double AcquisitionRate;

  /* Flag whether the device is recording */
  int Recording;

  /*! If true then RF processing parameters will be saved into the config file */
  bool SaveRfProcessingParameters;

  /*! RF to brightness conversion */
  vtkRfProcessor* RfProcessor;
  vtkImageData* BlankImage;
  StreamBufferItem BrightnessOutputTrackedFrame;
  int BrightnessFrameSize[2];

  /*! if we want to update according to the frame closest to the timestamp specified by desiredTimestamp */
  double DesiredTimestamp;
  int UpdateWithDesiredTimestamp;
  double TimestampClosestToDesired;

  /*! Requested frame rate in FPS. Actual frame rate may be different. TODO: Is it really the FPS? -Csaba */
  unsigned long FrameNumber; 
  double FrameTimeStamp;

  int NumberOfOutputFrames;

  /*! Set if output needs to be cleared to be cleared before being written */
  int OutputNeedsInitialization;

  bool Selectable;

protected:
  /*
    When defining a device, it may be a tracker or imaging device
    These variables allow a device to define which section of the configuration it is expecting
  */
  bool RequireDeviceImageOrientationInDeviceSetConfiguration;
  bool RequireFrameBufferSizeInDeviceSetConfiguration;
  bool RequireAcquisitionRateInDeviceSetConfiguration;
  bool RequireAveragedItemsForFilteringInDeviceSetConfiguration;
  bool RequireToolAveragedItemsForFilteringInDeviceSetConfiguration;
  bool RequireLocalTimeOffsetSecInDeviceSetConfiguration;
  bool RequireUsImageOrientationInDeviceSetConfiguration;
  bool RequireRfElementInDeviceSetConfiguration;

private:
  vtkPlusDevice(const vtkPlusDevice&);  // Not implemented.
  void operator=(const vtkPlusDevice&);  // Not implemented. 
};

#endif
