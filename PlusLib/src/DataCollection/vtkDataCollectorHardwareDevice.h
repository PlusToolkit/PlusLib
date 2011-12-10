/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkDataCollectorHardwareDevice_h
#define __vtkDataCollectorHardwareDevice_h

#include "vtkDataCollector.h" 

class vtkImageData; 
class vtkTracker;
class vtkDataCollectorSynchronizer;
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
  \class vtkDataCollectorHardwareDevice 
  \brief Collects tracked ultrasound data (images synchronized with tracking information) from hardware devices

  This class collects ultrasound images synchronized with pose tracking information.

  \ingroup PlusLibDataCollection
*/ 
class VTK_EXPORT vtkDataCollectorHardwareDevice: public vtkDataCollector
{
public:

  static vtkDataCollectorHardwareDevice *New();
  vtkTypeRevisionMacro(vtkDataCollectorHardwareDevice, vtkDataCollector);
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
  virtual PlusStatus Synchronize(const char* bufferOutputFolder = NULL, bool acquireDataOnly = false); 

  /*! Return the most recent synchronized timestamp in the buffers */
  virtual PlusStatus GetMostRecentTimestamp(double &ts); 

  /*! Return the oldest synchronized timestamp in the buffers */
  virtual PlusStatus GetOldestTimestamp(double &ts); 

  /*! Get the most recent tracked frame from devices with each tool transforms */
  virtual PlusStatus GetTrackedFrame(TrackedFrame* trackedFrame); 

  /*!
    Get the tracked frame list from devices since time specified
    \param aTimestamp The oldest timestamp we search for in the buffer. If -1 get all frames in the time range since the most recent timestamp. Out parameter - changed to timestamp of last added frame
    \param aTrackedFrameList Tracked frame list used to get the newly acquired frames into. The new frames are appended to the tracked frame.
    \param aMaxNumberOfFramesToAdd The maximum number of latest frames acquired from the buffers (till most recent timestamp). If -1 get all frames in the time range since aTimestamp
  */
  virtual PlusStatus GetTrackedFrameList(double& aTimestamp, vtkTrackedFrameList* aTrackedFrameList, int aMaxNumberOfFramesToAdd = -1); 

  /*!
    Get the tracked frame list from devices since time specified
    \param aTimestamp The oldest timestamp we search for in the buffer. If -1 get all frames in the time range since the most recent timestamp. Out parameter - changed to timestamp of last added frame
    \param aTrackedFrameList Tracked frame list used to get the newly acquired frames into. The new frames are appended to the tracked frame.
    \param aSamplingRateSec Sampling rate for getting the frames in seconds (timestamps are in seconds too)
  */
  virtual PlusStatus GetTrackedFrameListSampled(double& aTimestamp, vtkTrackedFrameList* aTrackedFrameList, double aSamplingRateSec); 

  /*! Get the tracked frame from devices by time with each tool transforms */
  virtual PlusStatus GetTrackedFrameByTime(double time, TrackedFrame* trackedFrame); 

  /*! Set video and tracker local time offset  */
  virtual void SetLocalTimeOffset(double videoOffset, double trackerOffset); 

  /*! Get tracker tool reference frame name (eg. "Tracker")  */
  virtual PlusStatus GetTrackerToolReferenceFrame(std::string &aToolReferenceFrameName);

  /*! Get frame rate - from video if enabled, else from tracker */
  virtual PlusStatus GetFrameRate(double &aFrameRate);

public:
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

  /*! Set video source of ultrasound */
  virtual void SetVideoSource(vtkPlusVideoSource* videoSource); 
  /*! Get video source of ultrasound */
  vtkGetObjectMacro(VideoSource,vtkPlusVideoSource);

  /*! Get frame size */
  virtual void GetFrameSize(int aDim[2]);

  /*! Set synchronizer */
  virtual void SetSynchronizer(vtkDataCollectorSynchronizer* synchronizer);
  /*! Get synchronizer */
  vtkGetObjectMacro(Synchronizer,vtkDataCollectorSynchronizer);

  /*! Set tracker  */
  virtual void SetTracker(vtkTracker* tracker); 
  /*! Get tracker  */
  vtkGetObjectMacro(Tracker,vtkTracker);
  
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

  /*! Callback function for progress bar refreshing */  
  void SetProgressBarUpdateCallbackFunction(ProgressBarUpdatePtr cb) { ProgressBarUpdateCallbackFunction = cb; } 

protected:

  /*! Get number of tracked frames between two given timestamps (inclusive) */
  int GetNumberOfFramesBetweenTimestamps(double aTimestampFrom, double aTimestampTo);

  /*! Get frame data by time  */
  virtual PlusStatus GetFrameByTime(double time, PlusVideoFrame& frame, double& aTimestamp); 

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

protected:
  vtkDataCollectorHardwareDevice();
  virtual ~vtkDataCollectorHardwareDevice();

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

  /*! The user requested the canceling of a synchronization procedure */
  bool                          CancelSyncRequest; 

private:
  vtkDataCollectorHardwareDevice(const vtkDataCollectorHardwareDevice&);
  void operator=(const vtkDataCollectorHardwareDevice&);

};

#endif
