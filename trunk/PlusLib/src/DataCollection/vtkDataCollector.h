/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkDataCollectorHardwareDevice_h
#define __vtkDataCollectorHardwareDevice_h

#include "vtkImageAlgorithm.h" 
#include "TrackedFrame.h"

class vtkXMLDataElement; 
class vtkTrackedFrameList; 
class TrackedFrame; 
class vtkMatrix4x4;

class vtkImageData; 
class vtkTracker;
class vtkPlusVideoSource;
class PlusVideoFrame;

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
  vtkTypeRevisionMacro(vtkDataCollector, vtkImageAlgorithm);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

	typedef std::map<std::string, std::string> FieldMapType;

  /*! Read the configuration file in XML format and set up the devices */
  virtual PlusStatus ReadConfiguration( vtkXMLDataElement* aDataCollectionConfig ); 

  /*! Write the device set configuration to XML format */
  virtual PlusStatus WriteConfiguration( vtkXMLDataElement* aDataCollectionConfig ); 

  /*! Disconnect from devices */
  virtual PlusStatus Disconnect(); 

  /*! Connect to devices */
  virtual PlusStatus Connect(); 

  /*! Stop data collection */
  virtual PlusStatus Stop(); 

  /*! Start data collection  */
  virtual PlusStatus Start(); 

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
    Get tracking data in a tracked frame list since time specified
    \param aTimestamp The oldest timestamp we search for in the buffer. If -1 get all frames in the time range since the most recent timestamp. Out parameter - changed to timestamp of last added frame
    \param aTrackedFrameList Tracked frame list used to get the newly acquired frames into. The new frames are appended to the tracked frame.
  */
  virtual PlusStatus GetTrackingData(double& aTimestamp, vtkTrackedFrameList* aTrackedFrameList); 

  /*!
    Get video data in a tracked frame list since time specified
    \param aTimestamp The oldest timestamp we search for in the buffer. If -1 get all frames in the time range since the most recent timestamp. Out parameter - changed to timestamp of last added frame
    \param aTrackedFrameList Tracked frame list used to get the newly acquired frames into. The new frames are appended to the tracked frame.
  */
  virtual PlusStatus GetVideoData(double& aTimestamp, vtkTrackedFrameList* aTrackedFrameList); 

  /*!
    Get the tracked frame list from devices since time specified
    \param aTimestamp The oldest timestamp we search for in the buffer. If -1 get all frames in the time range since the most recent timestamp. Out parameter - changed to timestamp of last added frame
    \param aTrackedFrameList Tracked frame list used to get the newly acquired frames into. The new frames are appended to the tracked frame.
    \param aSamplingRateSec Sampling rate for getting the frames in seconds (timestamps are in seconds too)
  */
  virtual PlusStatus GetTrackedFrameListSampled(double& aTimestamp, vtkTrackedFrameList* aTrackedFrameList, double aSamplingRateSec); 

  /*! Get the tracked frame from devices by time with each tool transforms */
  virtual PlusStatus GetTrackedFrameByTime(double time, TrackedFrame* trackedFrame); 

  /*! Set video and tracker local time offset */
  virtual void SetLocalTimeOffsetSec(double videoOffsetSec, double trackerOffsetSec); 

  /*! Get tracker tool reference frame name (eg. "Tracker")  */
  virtual PlusStatus GetTrackerToolReferenceFrame(std::string &aToolReferenceFrameName);

  /*! Get frame rate - from video if enabled, else from tracker */
  virtual PlusStatus GetFrameRate(double &aFrameRate);

public:
  /*! Set video source of ultrasound */
  virtual void SetVideoSource(vtkPlusVideoSource* videoSource); 
  /*! Get video source of ultrasound */
  vtkGetObjectMacro(VideoSource,vtkPlusVideoSource);

  /*! Get frame size */
  virtual PlusStatus GetFrameSize(int aDim[2]);

  /*! Get frame size */
  virtual PlusStatus GetBrightnessFrameSize(int aDim[2]);

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

  /*! Set startup delay in sec to give some time to the buffers for proper initialization */
  vtkSetMacro(StartupDelaySec, double); 
  /*! Get startup delay in sec to give some time to the buffers for proper initialization */
  vtkGetMacro(StartupDelaySec, double);

  /*! Returns a brightness image as output. If RF data is collected then it is converted. */
  vtkImageData* GetBrightnessOutput();

    /*! Get the Connected flag  */
  vtkGetMacro(Connected,bool);
  
  /*!
    Check if tracking data is available. It may come from any device, therefore tracking data may be
    available even when a dedicated tracking device is not available.
  */
  bool GetTrackingDataAvailable();


protected:

  /*! Set the Connected flag  */
  vtkSetMacro(Connected,bool);  
  /*! Set the Connected flag  */
  vtkBooleanMacro(Connected, bool); 

  /*!
    Verifies if the specified configuration data is valid (e.g., configuration version
    is specified and supported by the current software version).
    If any of the checks fails then it logs the error and returns PLUS_FAIL.
    If all the checks pass then it returns PLUS_SUCCESS.
  */  
  static PlusStatus VerifyDeviceSetConfigurationData(vtkXMLDataElement* rootElement);

  /*! Get number of tracked frames between two given timestamps (inclusive) */
  int GetNumberOfFramesBetweenTimestamps(double aTimestampFrom, double aTimestampTo);

  /*! This is called by the superclass. */
  virtual int RequestData(vtkInformation *request, vtkInformationVector** inputVector, vtkInformationVector* outputVector);

  /*! Read image acquisition properties from xml file  */
  virtual PlusStatus ReadImageAcquisitionProperties(vtkXMLDataElement* aConfigurationData); 

  /*! Read tracker properties from xml file  */
  virtual PlusStatus ReadTrackerProperties(vtkXMLDataElement* aConfigurationData); 

  /*!
    Compute loop times for saved datasets (time intersection of the two buffers)
    itemTimestamp = loopStartTime + (actualTimestamp - startTimestamp) % loopTime
  */
  virtual PlusStatus SetLoopTimes(); 

  /*!
    Helper function to find out the Tracker system reference frame name from the transform names
    stored in the latest tracked frame. Actually the method looks for a common "To" reference frame name,
    as normally the transform names are ImageToTracker, ReferenceToTracker, etc. and returns
    that common reference frame name ("Tracker").
  */
  PlusStatus GetTrackerToolReferenceFrameFromTrackedFrame(std::string &aToolReferenceFrameName);

protected:
  vtkDataCollector();
  virtual ~vtkDataCollector();

protected:

  /*! The timestamp filtering methods require some time to initialize. Synchronization will ignore data that are acquired during startup delay. */
  double StartupDelaySec; 

  /*! Successfully connected to devices */
  bool Connected; 

  /*! Collecting tracking data is enabled */
  bool TrackingEnabled;

  /*! Collecting image data is enabled */
  bool VideoEnabled;

  /*! Ultrasound image data source */
  vtkPlusVideoSource* VideoSource; 
  /*! Tracking data source */
  vtkTracker* Tracker; 

  /*!
    A blank image, to be returned when video data is requested but not available, to avoid potential crashes due to 
    NULL pointers.
  */
  vtkImageData* BlankImage;

private:
  vtkDataCollector(const vtkDataCollector&);
  void operator=(const vtkDataCollector&);

};

#endif
