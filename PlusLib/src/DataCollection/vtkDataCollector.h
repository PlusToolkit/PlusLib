/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkDataCollector_h
#define __vtkDataCollector_h

#include "PlusConfigure.h"
#include "vtkImageAlgorithm.h" 

class vtkXMLDataElement; 
class vtkImageData; 
class vtkVideoBuffer; 
class vtkTrackerBuffer; 
class vtkTrackedFrameList; 
class TrackedFrame; 
class vtkDataCollectorSynchronizer; 
class vtkTracker; 
class vtkMatrix4x4; 
class vtkPlusVideoSource; 
class PlusVideoFrame; 

/*! Acquisition types */ 
enum ACQUISITION_TYPE 
{
  SYNCHRO_VIDEO_NONE=0, 
  SYNCHRO_VIDEO_SAVEDDATASET,
  SYNCHRO_VIDEO_NOISE,
  SYNCHRO_VIDEO_MIL,
  SYNCHRO_VIDEO_WIN32,
  SYNCHRO_VIDEO_LINUX,
  SYNCHRO_VIDEO_SONIX,
  SYNCHRO_VIDEO_ICCAPTURING
}; 

/*! Tracker types */
enum TRACKER_TYPE
{
  TRACKER_NONE=0, 
  TRACKER_SAVEDDATASET,
  TRACKER_BRACHY, 
  TRACKER_CERTUS, 
  TRACKER_POLARIS, 
  TRACKER_AURORA, 
  TRACKER_FLOCK, 
  TRACKER_MICRON,
  TRACKER_FAKE,
  TRACKER_ASCENSION3DG
}; 

/*! Synchronization types */
enum SYNC_TYPE
{
  SYNC_NONE=0,
  SYNC_CHANGE_DETECTION
};

/*!
  \class vtkDataCollector 
  \brief Collects tracked ultrasound data (images synchronized with tracking information)

  This class collects ultrasound images synchronized with pose tracking information.

  \ingroup PlusLibDataCollection
*/ 
class VTK_EXPORT vtkDataCollector: public vtkImageAlgorithm
{
public:

  static vtkDataCollector *New();
  vtkTypeRevisionMacro(vtkDataCollector,vtkImageAlgorithm);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  /*! Read the configuration file in XML format and set up the devices */
  virtual PlusStatus ReadConfiguration( vtkXMLDataElement* aDataCollectionConfig ); 

  /*! Disconnect from devices */
  virtual PlusStatus Disconnect(); 

  /*! Connect to devices */
  virtual PlusStatus Connect(); 

  /*! Stop data collection */
  virtual PlusStatus Stop(); 

  /*! Start data collection  */
  virtual PlusStatus Start(); 

  /*! Synchronize the connected devices */
  virtual PlusStatus Synchronize( const char* bufferOutputFolder = NULL, bool acquireDataOnly = false ); 

  /*! Copy the current state of the tracker buffer  */
  virtual PlusStatus CopyTrackerBuffer( vtkTrackerBuffer* trackerBuffer, int toolNumber ); 

  /*! Copy the current state of the tracker (with each tools and buffers) */
  virtual PlusStatus CopyTracker( vtkTracker* tracker); 

  /*! Dump the current state of the tracker to metafile (with each tools and buffers) */
  static PlusStatus WriteTrackerToMetafile( vtkTracker* tracker, const char* outputFolder, const char* metaFileName, bool useCompression = false ); 

  /*! Copy the current state of the video buffer  */
  virtual PlusStatus CopyVideoBuffer( vtkVideoBuffer* videoBuffer ); 

  /*! Dump the current state of the video buffer to metafile */
  static PlusStatus WriteVideoBufferToMetafile( vtkVideoBuffer* videoBuffer, const char* outputFolder, const char* metaFileName, bool useCompression = false ); 

  /*! Return the most recent synchronized timestamp in the buffers */
  virtual PlusStatus GetMostRecentTimestamp(double &ts); 

  /*! Return the oldest synchronized timestamp in the buffers */
  virtual PlusStatus GetOldestTimestamp(double &ts); 

  /*! Return the tool status at a given time */
  virtual PlusStatus GetToolStatus( double time, int toolNumber, TrackerStatus &status ); 

  /*! Get the number of available tools */
  int GetNumberOfTools();

  /*! Get the most recent tracked frame from devices  */
  virtual PlusStatus GetTrackedFrame(vtkImageData* frame, vtkMatrix4x4* toolTransMatrix, TrackerStatus& status, double& synchronizedTime, int toolNumber = 0, bool calibratedTransform = false); 

  /*! Get the most recent tracked frame from devices with each tool transforms */
  virtual PlusStatus GetTrackedFrame(TrackedFrame* trackedFrame, bool calibratedTransform = false); 

  /*! Get number of tracked frames between two given timestamps (inclusive) */
  int GetNumberOfFramesBetweenTimestamps(double frameTimestampFrom, double frameTimestampTo);

  /*!
    Get the tracked frame list from devices since time specified
    \param frameTimestamp The oldest timestamp we search for in the buffer. If -1 get all frames in the time range since the most recent timestamp. Out parameter - changed to timestamp of last added frame
    \param trackedFrameList Tracked frame list used to get the newly acquired frames into. The new frames are appended to the tracked frame.
    \param maxNumberOfFramesToAdd The maximum number of latest frames acquired from the buffers (till most recent timestamp). If -1 get all frames in the time range since frameTimestamp
  */
  virtual PlusStatus GetTrackedFrameList(double& frameTimestamp, vtkTrackedFrameList* trackedFrameList, int maxNumberOfFramesToAdd = -1); 

  /*!
    Get the tracked frame list from devices since time specified
    \param frameTimestamp The oldest timestamp we search for in the buffer. If -1 get all frames in the time range since the most recent timestamp. Out parameter - changed to timestamp of last added frame
    \param trackedFrameList Tracked frame list used to get the newly acquired frames into. The new frames are appended to the tracked frame.
    \param samplingRateSec Sampling rate for getting the frames in seconds (timestamps are in seconds too)
  */
  virtual PlusStatus GetTrackedFrameListSampled(double& frameTimestamp, vtkTrackedFrameList* trackedFrameList, double samplingRateMs); 

  const char* vtkDataCollector::GetDefaultFrameTransformName(int toolNumber);

  /*! Get the tracked frame from devices by time with each tool transforms */
  virtual PlusStatus GetTrackedFrameByTime(double time, TrackedFrame* trackedFrame, bool calibratedTransform = false); 

  /*! Get the tracked frame from devices by time with each tool transforms */
  virtual PlusStatus GetTrackedFrameByTime(double time, vtkImageData* frame, std::vector<vtkMatrix4x4*> &toolTransforms, std::vector<std::string> &toolTransformNames, std::vector<TrackerStatus> &status, double& synchronizedTime, bool calibratedTransform = false); 

  /*! Get transformation with timestamp from tracker  */
  virtual PlusStatus GetTransformWithTimestamp(vtkMatrix4x4* toolTransMatrix, double& transformTimestamp, TrackerStatus& status, int toolNumber = 0, bool calibratedTransform = false); 

  /*! Get transformation by timestamp from tracker  */
  virtual PlusStatus GetTransformByTimestamp(vtkMatrix4x4* toolTransMatrix, TrackerStatus& status, double synchronizedTime, int toolNumber = 0, bool calibratedTransform = false); 

  /*! Get transformations by timestamp range from tracker. The first returned transform is the one after the startTime, except if startTime is -1, then it refers to the oldest one. '-1' for end time means the latest transform. Returns the timestamp of the requested transform (makes sense if endTime is -1) */
  virtual double GetTransformsByTimeInterval(std::vector<vtkMatrix4x4*> &toolTransMatrixVector, std::vector<TrackerStatus> &statusVector, double startTime, double endTime, int toolNumber = 0, bool calibratedTransform = false);

  /*! Get frame data by time  */
  virtual PlusStatus GetFrameByTime(double time, vtkImageData* frame, double& frameTimestamp); 
  /*! Get frame data by time  */
  virtual PlusStatus GetFrameByTime(double time, PlusVideoFrame& frame, double& frameTimestamp); 

  /*! Set the acquisition type  */
  void SetAcquisitionType(ACQUISITION_TYPE type) { AcquisitionType = type; }
  /*! Get the acquisition type  */
  ACQUISITION_TYPE GetAcquisitionType() { return this->AcquisitionType; }

  /*! Set the tracker type  */
  void SetTrackerType(TRACKER_TYPE type) { TrackerType = type; }
  /*! Get the tracker type  */
  TRACKER_TYPE GetTrackerType() { return this->TrackerType; }

  /*! Set the synchronization type  */
  void SetSyncType(SYNC_TYPE type) { SyncType = type; }
  /*! Get the synchronization type  */
  SYNC_TYPE GetSyncType() { return this->SyncType; }

  /*! Set video and tracker local time offset  */
  virtual void SetLocalTimeOffset(double videoOffset, double trackerOffset); 

  /*! Set the device set name */
  vtkSetStringMacro(DeviceSetName); 
  /*! Get the device set name */
  vtkGetStringMacro(DeviceSetName); 

  /*! Set the device set description  */
  vtkSetStringMacro(DeviceSetDescription); 
  /*! Get the device set description  */
  vtkGetStringMacro(DeviceSetDescription); 

  /*! Set the video source of ultrasound */
  virtual void SetVideoSource(vtkPlusVideoSource* videoSource); 
  /*! Get the video source of ultrasound */
  vtkGetObjectMacro(VideoSource,vtkPlusVideoSource);

  /*! Set the video source of ultrasound */
  virtual void SetSynchronizer(vtkDataCollectorSynchronizer* synchronizer);
  /*! Get the video source of ultrasound */
  vtkGetObjectMacro(Synchronizer,vtkDataCollectorSynchronizer);

  /*! Set the tracker  */
  virtual void SetTracker(vtkTracker* tracker); 
  /*! Get the tracker  */
  vtkGetObjectMacro(Tracker,vtkTracker);
  
  /*! Get the Connected flag  */
  vtkGetMacro(Connected,bool);
  
  /*! Get the Tracking only flag */
  vtkGetMacro(TrackingEnabled,bool);
  /*! Set the Tracking only flag */
  void SetTrackingOnly(bool);

  /*! Get the Video only flag */
  vtkGetMacro(VideoEnabled,bool);
  /*! Set the Video only flag */
  void SetVideoOnly(bool);

  /*! Set the cancel sync request flag to cancel the active sync job  */
  vtkSetMacro(CancelSyncRequest, bool); 
  /*! Get the cancel sync request flag to cancel the active sync job  */
  vtkGetMacro(CancelSyncRequest, bool); 
  /*! Set the cancel sync request flag to cancel the active sync job  */
  vtkBooleanMacro(CancelSyncRequest, bool); 

  /*! Set startup delay in sec to give some time to the buffers for proper initialization */
  vtkSetMacro(StartupDelaySec, double); 
  /*! Get startup delay in sec to give some time to the buffers for proper initialization */
  vtkGetMacro(StartupDelaySec, double);

  typedef void (*ProgressBarUpdatePtr)(int percent);
  /*! Callback function for progress bar refreshing */  
  void SetProgressBarUpdateCallbackFunction(ProgressBarUpdatePtr cb) { ProgressBarUpdateCallbackFunction = cb; } 

protected:
  vtkDataCollector();
  virtual ~vtkDataCollector();

  /*! Set the Connected flag  */
  vtkSetMacro(Connected,bool);  
  /*! Set the Connected flag  */
  vtkBooleanMacro(Connected, bool); 

  /*! This is called by the superclass. */
  virtual int RequestData(vtkInformation *request, vtkInformationVector** inputVector, vtkInformationVector* outputVector);

  /*! Read image acquisition properties from xml file  */
  virtual PlusStatus ReadImageAcquisitionProperties(vtkXMLDataElement* aConfigurationData); 

  /*! Read tracker properties from xml file  */
  virtual PlusStatus ReadTrackerProperties(vtkXMLDataElement* aConfigurationData); 

  /*! Read synchronization properties from xml file  */
  virtual PlusStatus ReadSynchronizationProperties(vtkXMLDataElement* aConfigurationData); 

  /*!
    Compute loop times for saved datasets (time intersection of the two buffers)
    itemTimestamp = loopStartTime + (actualTimestamp - startTimestamp) % loopTime
  */
  virtual PlusStatus SetLoopTimes(); 

  /*! Pointer to the progress bar update callback function */ 
  ProgressBarUpdatePtr ProgressBarUpdateCallbackFunction; 

protected:

  /*! Synchronizer used for temporal calibration */
  vtkDataCollectorSynchronizer* Synchronizer; 

  /*! Ultrasound image data source */
  vtkPlusVideoSource*	          VideoSource; 
  /*! Tracking data source */
  vtkTracker*		              	Tracker; 

  /*! Image acquisition device type */
  ACQUISITION_TYPE	            AcquisitionType; 
  /*! Tracking data acquisition device type */
  TRACKER_TYPE		              TrackerType; 
  /*! Synchronization algorithm type */
  SYNC_TYPE			                SyncType; 

  /*! Latest tracking data (transformation matrices) */
  std::vector<vtkMatrix4x4*>    ToolTransMatrices; 
  /*! Latest tracking data (tool status information) */
  std::vector<TrackerStatus>	  ToolStatus; 

  /*! Version of the configuration description XML data element */
  double			              	  DataCollectionConfigVersion; 

  /*! The timestamp filtering methods require some time to initialize. Synchronization will ignore data that are acquired during startup delay. */
  double                        StartupDelaySec; 

  /*! Successfully connected to devices */
  bool                          Connected; 

  /*! Collecting tracking data is enabled */
  bool                          TrackingEnabled;
  /*! Collecting image data is enabled */
  bool                          VideoEnabled;

  /*! The user requested the canceling of a synchronization procedure */
  bool                          CancelSyncRequest; 

  /*! Name of the current device set */
  char*                         DeviceSetName; 
  /*! Description of the current device set */
  char*                         DeviceSetDescription; 

private:
  vtkDataCollector(const vtkDataCollector&);
  void operator=(const vtkDataCollector&);

};

#endif
