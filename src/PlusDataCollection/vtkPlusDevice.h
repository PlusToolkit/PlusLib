/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusDevice_h
#define __vtkPlusDevice_h

// Local includes
#include "igsioCommon.h"
#include "PlusConfigure.h"
#include "PlusStreamBufferItem.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataCollectionExport.h"

// VTK includes
#include <vtkImageAlgorithm.h>
#include <vtkMultiThreader.h>
#include <vtkStdString.h>

#include <set>

// STL includes
#include <string>

class vtkPlusBuffer;
class vtkPlusDataCollector;
class vtkPlusDataSource;
class vtkPlusDevice;
class vtkPlusHTMLGenerator;
class vtkXMLDataElement;

typedef std::vector<vtkPlusChannel*> ChannelContainer;
typedef ChannelContainer::const_iterator ChannelContainerConstIterator;
typedef ChannelContainer::iterator ChannelContainerIterator;

typedef std::vector<vtkPlusBuffer*> StreamBufferContainer;
typedef StreamBufferContainer::const_iterator StreamBufferContainerConstIterator;
typedef StreamBufferContainer::iterator StreamBufferContainerIterator;

typedef std::map<int, vtkPlusBuffer*> StreamBufferMapContainer;
typedef StreamBufferMapContainer::const_iterator StreamBufferMapContainerConstIterator;
typedef StreamBufferMapContainer::iterator StreamBufferMapContainerIterator;

typedef std::vector<vtkPlusDevice*> DeviceCollection;
typedef std::vector<vtkPlusDevice*>::iterator DeviceCollectionIterator;
typedef std::vector<vtkPlusDevice*>::const_iterator DeviceCollectionConstIterator;

/*!
\class vtkPlusDevice
\brief Abstract interface for tracker and video devices

vtkPlusDevice is an abstract VTK interface to real-time tracking and imaging
systems.  Derived classes should override the InternalConnect(), InternalDisconnect(),
GetSdkVersion(), ReadConfiguration(), WriteConfiguration() methods.

\ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusDevice : public vtkImageAlgorithm
{
public:
  static vtkPlusDevice* New();
  vtkTypeMacro(vtkPlusDevice, vtkImageAlgorithm);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  static const std::string BMODE_PORT_NAME;
  static const std::string RFMODE_PORT_NAME;
  static const std::string PARAMETERS_XML_ELEMENT_TAG;
  static const std::string PARAMETER_XML_ELEMENT_TAG;

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
  Return whether or not the device can be reset
  */
  virtual bool IsResettable();

  /*!
  Record incoming data at the specified acquisition rate.  The recording
  continues indefinitely until StopRecording() is called.
  */
  virtual PlusStatus StartRecording();

  /*! Stop recording */
  virtual PlusStatus StopRecording();

  /*! Return the reference frame */
  static PlusStatus GetToolReferenceFrameFromTrackedFrame(igsioTrackedFrame& aFrame, std::string& aToolReferenceFrameName);

  /*!
  Get the buffer that is used to hold the data
  There are cases when multiple externally controlled buffers are needed.
  There must always be a valid buffer in the data object, therefore
  the input parameter shall not be NULL.
  */
  //virtual PlusStatus SetBuffer(vtkPlusBuffer* newBuffer);

  /*!
  Set size of the internal frame buffer, i.e. the number of most recent frames that
  are stored in the video source class internally.
  */
  virtual PlusStatus SetBufferSize(vtkPlusChannel& aChannel, int FrameBufferSize, const std::string& aSourceId = std::string(""));
  /*! Get size of the internal frame buffer. */
  virtual PlusStatus GetBufferSize(vtkPlusChannel& aChannel, int& outVal, const std::string& aSourceId = std::string(""));

  /*! Set recording start time */
  virtual void SetStartTime(double startTime);

  /*! Get recording start time */
  virtual double GetStartTime();

  /*! Is this device a tracker */
  virtual bool IsTracker() const;

  virtual bool IsVirtual() const;

  /*!
  Reset the device. The actual reset action is defined in subclasses. A reset is typically performed on the users request
  while the device is connected. A reset can be used for zeroing sensors, canceling an operation in progress, etc.
  */
  virtual PlusStatus Reset();

  /*! Clear all tool buffers */
  void ClearAllBuffers();

  /*! Dump the current state of the device to sequence file (with each tools and buffers) */
  virtual PlusStatus WriteToolsToSequenceFile(const std::string& filename, bool useCompression = false);

  /*! Make this device into a copy of another device. */
  void DeepCopy(const vtkPlusDevice& device);

  /*! Get the internal update rate for this tracking system.  This is the number of buffer entry items sent by the device per second (per tool). */
  double GetInternalUpdateRate() const;

  /*! Get the data source object for the specified Id name, checks both video and tools */
  PlusStatus GetDataSource(const char* aSourceId, vtkPlusDataSource*& aSource);
  PlusStatus GetDataSource(const std::string& aSourceId, vtkPlusDataSource*& aSource);

  /*! Get the tool object for the specified tool name */
  PlusStatus GetTool(const char* aToolSourceId, vtkPlusDataSource*& aTool) const;
  PlusStatus GetTool(const std::string& aToolSourceId, vtkPlusDataSource*& aTool) const;

  /*! Get the first active tool among all the source tools */
  PlusStatus GetFirstActiveTool(vtkPlusDataSource*& aTool) const;

  /*! Get the tool object for the specified tool port name */
  PlusStatus GetToolByPortName(const char* aPortName, vtkPlusDataSource*& aSource);
  PlusStatus GetToolByPortName(const std::string& aPortName, vtkPlusDataSource*& aSource);
  /*! Get the video source objects for the specified video port name */
  PlusStatus GetVideoSourcesByPortName(const char* aPortName, std::vector<vtkPlusDataSource*>& sources);
  PlusStatus GetVideoSourcesByPortName(const std::string& aPortName, std::vector<vtkPlusDataSource*>& sources);

  /*! Get the beginning of the tool iterator */
  DataSourceContainerConstIterator GetToolIteratorBegin() const;

  /*! Get the end of the tool iterator */
  DataSourceContainerConstIterator GetToolIteratorEnd() const;

  /*! Add tool to the device */
  PlusStatus AddTool(vtkPlusDataSource* tool, bool requireUniquePortName = true);

  /*! Get number of images */
  virtual int GetNumberOfTools() const;

  /*! Get the video source for the specified source name */
  PlusStatus GetVideoSource(const char* aSourceId, vtkPlusDataSource*& aVideoSource);

  /*! Get all video sources*/
  std::vector<vtkPlusDataSource*> GetVideoSources() const;

  /*! Get the video source for the specified source index */
  PlusStatus GetVideoSourceByIndex(const unsigned int index, vtkPlusDataSource*& aVideoSource);

  /*! Get the first active image object */
  PlusStatus GetFirstVideoSource(vtkPlusDataSource*& anImage);

  /*! Get the beginning of the image iterator */
  DataSourceContainerConstIterator GetVideoSourceIteratorBegin() const;

  /*! Get the end of the image iterator */
  DataSourceContainerConstIterator GetVideoSourceIteratorEnd() const;

  /*! Add image to the device */
  PlusStatus AddVideoSource(vtkPlusDataSource* anImage);

  /*! Get number of images */
  virtual int GetNumberOfVideoSources() const;

  /*! Get the field data source for the specified field data id */
  PlusStatus GetFieldDataSource(const char* aSourceId, vtkPlusDataSource*& aSource) const;
  PlusStatus GetFieldDataSource(const std::string& aSourceId, vtkPlusDataSource*& aSource) const;

  /*! Get the beginning of the field data iterator */
  DataSourceContainerConstIterator GetFieldDataSourcessIteratorBegin() const;

  /*! Get the end of the field data iterator */
  DataSourceContainerConstIterator GetFieldDataSourcessIteratorEnd() const;

  /*! Add field data to the device */
  PlusStatus AddFieldDataSource(vtkPlusDataSource* aSource);

  /*! Get number of field data sources */
  virtual int GetNumberOfFieldDataSources() const;

  /*! Set Reference name of the tools */
  void SetToolReferenceFrameName(const std::string& frameName);
  /*! Get Reference name of the tools */
  std::string GetToolReferenceFrameName() const;

  /*! Is the device correctly configured? */
  virtual bool GetCorrectlyConfigured() const;

  /*! Set the parent data collector */
  virtual void SetDataCollector(vtkPlusDataCollector* _arg);

  /*! Set buffer size of all available tools */
  void SetToolsBufferSize(int aBufferSize);

  /*! Set buffer size of all available field data sources */
  void SetFieldDataSourcesBufferSize(int aBufferSize);

  /*! Set local time offset of all available buffers */
  virtual void SetLocalTimeOffsetSec(double aTimeOffsetSec);
  virtual double GetLocalTimeOffsetSec() const;

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
  virtual PlusStatus InternalUpdate();

  /*!
  Build a list of all of the input devices directly connected to this device (if any)
  */
  PlusStatus GetInputDevices(std::vector<vtkPlusDevice*>& outDeviceList) const;
  /*!
  Recursively assemble all devices that feed into this device (if any)
  */
  PlusStatus GetInputDevicesRecursive(std::vector<vtkPlusDevice*>& outDeviceList) const;

  // Parameter interface
  virtual PlusStatus SetParameter(const std::string& key, const std::string& value);
  virtual std::string GetParameter(const std::string& key) const;
  virtual PlusStatus GetParameter(const std::string& key, std::string& outValue) const;

  //BTX
  // These are used by static functions in vtkPlusDevice.cxx, and since
  // VTK doesn't generally use 'friend' functions they are public
  // instead of protected.  Do not use them anywhere except inside
  // vtkPlusDevice.cxx.
  vtkIGSIORecursiveCriticalSection* UpdateMutex;
  vtkTimeStamp UpdateTime;
  double InternalUpdateRate;
  //ETX

  /*! Set the acquisition rate */
  virtual double GetAcquisitionRate() const;
  PlusStatus SetAcquisitionRate(double aRate);

  /*! Get whether recording is underway */
  virtual bool IsRecording() const;

  /* Return the id of the device */
  virtual std::string GetDeviceId() const;
  // Set the device Id
  void SetDeviceId(const std::string& id);

  /*!
  Get the frame number (some devices has frame numbering, otherwise
  just increment if new frame received)
  */
  virtual unsigned long GetFrameNumber() const;

  /*!
  Get a time stamp in seconds (resolution of milliseconds) for
  the most recent frame.  Time began on Jan 1, 1970.  This timestamp is only
  valid after the Output has been Updated.  Usually set to the
  timestamp for the output if UpdateWithDesiredTimestamp is off,
  otherwise it is the timestamp for the most recent frame, which is not
  necessarily the output
  */
  virtual double GetFrameTimeStamp() const;

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
  virtual int GetConnected() const;
  virtual bool IsConnected() const;

  /*!
  Set the full-frame size.  This must be an allowed size for the device,
  the device may either refuse a request for an illegal frame size or
  automatically choose a new frame size.
  */
  virtual PlusStatus SetInputFrameSize(vtkPlusDataSource& aSource, unsigned int x, unsigned int y, unsigned int z);

  /*! Get the full-frame size */
  virtual PlusStatus GetInputFrameSize(vtkPlusChannel& aChannel, unsigned int& x, unsigned int& y, unsigned int& z) const;

  /*! Get the full-frame size */
  virtual PlusStatus GetInputFrameSize(vtkPlusChannel& aChannel, FrameSizeType& dim) const;

  /*! Get the full-frame size */
  virtual PlusStatus GetOutputFrameSize(vtkPlusChannel& aChannel, unsigned int& x, unsigned int& y, unsigned int& z) const;

  /*! Get the full-frame size */
  virtual PlusStatus GetOutputFrameSize(vtkPlusChannel& aChannel, FrameSizeType& dim) const;

  /*! Set the pixel type (char, unsigned short, ...) */
  virtual PlusStatus SetPixelType(vtkPlusChannel& aChannel, igsioCommon::VTKScalarPixelType pixelType);
  /*! Get the pixel type (char, unsigned short, ...) */
  virtual igsioCommon::VTKScalarPixelType GetPixelType(vtkPlusChannel& aChannel);

  /*! Set the image type (B-mode, RF, ...) provided by the video source. */
  virtual PlusStatus SetImageType(vtkPlusChannel& aChannel, US_IMAGE_TYPE imageType);
  /*! Get the image pixel type (B-mode, RF, ...) */
  virtual US_IMAGE_TYPE GetImageType(vtkPlusChannel& aChannel);

  /*! Add an output channel */
  PlusStatus AddOutputChannel(vtkPlusChannel* aChannel);

  /*! Access the available output channels */
  PlusStatus GetFirstOutputChannel(vtkPlusChannel*& aChannel);
  PlusStatus GetOutputChannelByName(vtkPlusChannel*& aChannel, const char* aChannelId);
  PlusStatus GetOutputChannelByName(vtkPlusChannel*& aChannel, const std::string& aChannelId);

  virtual int OutputChannelCount() const;

  ChannelContainerConstIterator GetOutputChannelsStart() const;
  ChannelContainerConstIterator GetOutputChannelsEnd() const;
  ChannelContainerIterator GetOutputChannelsStart();
  ChannelContainerIterator GetOutputChannelsEnd();

  /*! Add an input channel */
  PlusStatus AddInputChannel(vtkPlusChannel* aChannel);

  /*!
  Perform any completion tasks once configured
  */
  virtual PlusStatus NotifyConfigured();

  /*!
  Return the latest or desired image frame. This method can be overridden in subclasses
  Part of the vtkAlgorithm pipeline
  */
  virtual int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*);

  /*!
  Return the latest or desired image frame. This method can be overridden in subclasses
  Part of the vtkAlgorithm pipeline
  */
  virtual int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*);

  /* Accessors for the grace period value */
  vtkSetMacro(MissingInputGracePeriodSec, double);
  double GetMissingInputGracePeriodSec() const;

  /*!
    Creates a default output channel for the device with the name channelId or "OutputChannel".
    \param addSource If true then for imaging devices a default 'Video' source is added to the output.
  */
  virtual PlusStatus CreateDefaultOutputChannel(const char* channelId = NULL, bool addSource = true);

  /*! Convenience function for getting the first available video source in the output channels */
  PlusStatus GetFirstActiveOutputVideoSource(vtkPlusDataSource*& aVideoSource);

  /*! Return a list of items that describe what image volumes this device can provide */
  virtual PlusStatus GetImageMetaData(igsioCommon::ImageMetaDataList& imageMetaDataItems);

  /*!
    Return the specified volume from the corresponding device. The assignedImageId and requestedImageId should be the same
    If requestedImageId is empty then GetImage will return the default image and set assignedImageId to the ID of this default image.
    If requestedImageId is not empty then assignedImageId is the same as the requestedImageId.
  */
  virtual PlusStatus GetImage(const std::string& requestedImageId, std::string& assignedImageId, const std::string& imageReferencFrameName, vtkImageData* imageData, vtkMatrix4x4* ijkToReferenceTransform);

  /*!
    Send text message to the device. If a non-NULL pointer is passed as textReceived
    then the device waits for a response and returns it in textReceived.
  */
  virtual PlusStatus SendText(const std::string& textToSend, std::string* textReceived = NULL);

protected:
  static void* vtkDataCaptureThread(vtkMultiThreader::ThreadInfo* data);

  /*! Should be overridden to connect to the hardware */
  virtual PlusStatus InternalConnect();

  /*! Release the video driver. Should be overridden to disconnect from the hardware. */
  virtual PlusStatus InternalDisconnect();

  /*!
  Called at the end of StartRecording to allow hardware-specific
  actions for starting the recording
  */
  virtual PlusStatus InternalStartRecording();

  /*!
  Called at the beginning of StopRecording to allow hardware-specific
  actions for stopping the recording
  */
  virtual PlusStatus InternalStopRecording();

  /*!
  This function can be called to add a video item to the specified video data sources
  */
  virtual PlusStatus AddVideoItemToVideoSources(const std::vector<vtkPlusDataSource*>& videoSources, const igsioVideoFrame& frame, long frameNumber, double unfilteredTimestamp = UNDEFINED_TIMESTAMP,
      double filteredTimestamp = UNDEFINED_TIMESTAMP, const igsioFieldMapType* customFields = NULL);

  /*!
  This function can be called to add a video item to the specified video data sources
  */
  virtual PlusStatus AddVideoItemToVideoSources(const std::vector<vtkPlusDataSource*>& videoSources, void* imageDataPtr, US_IMAGE_ORIENTATION usImageOrientation, const FrameSizeType& frameSizeInPx,
      igsioCommon::VTKScalarPixelType pixelType, unsigned int numberOfScalarComponents, US_IMAGE_TYPE imageType, int numberOfBytesToSkip, long frameNumber, double unfilteredTimestamp = UNDEFINED_TIMESTAMP,
      double filteredTimestamp = UNDEFINED_TIMESTAMP, const igsioFieldMapType* customFields = NULL);

  /*!
  This function is called by InternalUpdate() so that the subclasses
  can communicate information back to the vtkPlusDevice base class, which
  will in turn relay the information to the appropriate vtkPlusDataSource.
  */
  virtual PlusStatus ToolTimeStampedUpdate(const std::string& aToolSourceId, vtkMatrix4x4* matrix, ToolStatus status, unsigned long frameNumber, double unfilteredtimestamp, const igsioFieldMapType* customFields = NULL);

  /*!
  This function is called by InternalUpdate() so that the subclasses
  can communicate information back to the vtkPlusDevice base class, which
  will in turn relay the information to the appropriate vtkPlusDataSource.
  This function is for devices has no frame numbering, just auto increment tool frame number if new frame received
  */
  virtual PlusStatus ToolTimeStampedUpdateWithoutFiltering(const std::string& aToolSourceId, vtkMatrix4x4* matrix, ToolStatus status, double unfilteredtimestamp, double filteredtimestamp, const igsioFieldMapType* customFields = NULL);

  /*!
  Helper function used during configuration to locate the correct XML element for a device
  */
  vtkXMLDataElement* FindThisDeviceElement(vtkXMLDataElement* rootXMLElement);
  /*!
  Helper function used during configuration to locate the correct XML element for an output stream
  */
  vtkXMLDataElement* FindOutputChannelElement(vtkXMLDataElement* rootXMLElement, const char* aChannelId);
  /*!
  Helper function used during configuration to locate the correct XML element for an input stream
  */
  vtkXMLDataElement* FindInputChannelElement(vtkXMLDataElement* rootXMLElement, const char* aChannelId);
  /*!
  Method that writes output streams to XML
  */
  virtual void InternalWriteOutputChannels(vtkXMLDataElement* rootXMLElement);
  /*!
  Method that writes output streams to XML
  */
  virtual void InternalWriteInputChannels(vtkXMLDataElement* rootXMLElement);

  /*! Ensure uniqueness of given ID */
  PlusStatus EnsureUniqueDataSourceId(const std::string& aSourceId);

  vtkSetMacro(CorrectlyConfigured, bool);

  vtkSetMacro(StartThreadForInternalUpdates, bool);
  bool GetStartThreadForInternalUpdates() const;

  vtkSetMacro(RecordingStartTime, double);
  double GetRecordingStartTime() const;

  virtual vtkPlusDataCollector* GetDataCollector();

  bool HasGracePeriodExpired();

  vtkPlusDevice();
  virtual ~vtkPlusDevice();

protected:
  /*! Flag to store recording thread state */
  bool ThreadAlive;

  /* Is device connected */
  int Connected;

  /*! Thread used for acquisition */
  vtkMultiThreader* Threader;

  /*! Recording thread id */
  int ThreadId;

  ChannelContainer  OutputChannels;
  ChannelContainer  InputChannels;

  /*! A stream buffer item to use as a temporary staging point */
  StreamBufferItem*   CurrentStreamBufferItem;
  DataSourceContainer Tools;
  DataSourceContainer VideoSources;
  DataSourceContainer Fields;

  /*! Reference name of the tools */
  std::string ToolReferenceFrameName;

  /*! Id of the device */
  std::string DeviceId;

  vtkPlusDataCollector* DataCollector;

  /*! Acquisition rate */
  double AcquisitionRate;

  /* Flag whether the device is recording */
  int Recording;

  /*! if we want to update according to the frame closest to the timestamp specified by desiredTimestamp */
  double DesiredTimestamp;
  int UpdateWithDesiredTimestamp;
  double TimestampClosestToDesired;

  /*! Sequential number of frame, possibly since last re-initialization. */
  unsigned long FrameNumber;
  double FrameTimeStamp;

  /*! Set if output needs to be cleared to be cleared before being written */
  int OutputNeedsInitialization;

  /*! Is this device correctly configured? */
  bool CorrectlyConfigured;

  /*!
  If enabled, then a data capture thread is created when the device is connected that regularly calls InternalUpdate.
  This update mechanism is useful for devices that don't provide callback functions but require polling.
  */
  bool StartThreadForInternalUpdates;

  /*! Value to use when mixing data with another temporally calibrated device*/
  double LocalTimeOffsetSec;

  /*! Adjust the device reporting behaviour depending on whether or not a grace period has expired */
  double MissingInputGracePeriodSec;
  /*! Adjust the device reporting behaviour depending on whether or not a grace period has expired */
  double RecordingStartTime;

  /*!
    The list contains the IDs of the tools that have been already reported to be unknown.
    This list is used to only report an unknown tool once (after the connection has been established), not at each
    attempt to access it.
  */
  std::set<std::string> ReportedUnknownTools;

  /*! Map to store general purpose device parameters  */
  std::map<std::string, std::string> Parameters;

  static const int VIRTUAL_DEVICE_FRAME_RATE;

protected:
  /*
  When defining a device, it may be a tracker or imaging device
  These variables allow a device to define which commonly used attributes it is expecting
  */
  bool RequireImageOrientationInConfiguration;
  bool RequirePortNameInDeviceSetConfiguration;

private:
  vtkPlusDevice(const vtkPlusDevice&);   // Not implemented.
  void operator=(const vtkPlusDevice&);   // Not implemented.
};

#define XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement) \
  if( Superclass::ReadConfiguration(rootConfigElement) != PLUS_SUCCESS )  \
  { \
    LOG_ERROR("Unable to continue reading configuration of "<<this->GetClassName()<<". Generic device configuration reading failed.");  \
    return PLUS_FAIL; \
  } \
  vtkXMLDataElement* deviceConfig = this->FindThisDeviceElement(rootConfigElement);  \
  if (deviceConfig == NULL)  \
  { \
    LOG_ERROR("Unable to continue configuration of "<<this->GetClassName()<<". Could not find corresponding element.");  \
    return PLUS_FAIL; \
  }

#define XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement) \
  if( Superclass::WriteConfiguration(rootConfigElement) == PLUS_FAIL ) \
  { \
    LOG_ERROR("Unable to continue writing configuration of "<<this->GetClassName()<<". Generic device configuration writing failed.");  \
    return PLUS_FAIL; \
  } \
  vtkXMLDataElement* deviceConfig = this->FindThisDeviceElement(rootConfigElement); \
  if (deviceConfig == NULL) \
  { \
    LOG_ERROR("Cannot find or add "<<this->GetClassName()<<" device in XML tree"); \
    return PLUS_FAIL; \
  }

#endif
