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
class vtkTrackedFrameList; 
class TrackedFrame; 
class vtkTracker; 
class vtkMatrix4x4; 
class vtkPlusVideoSource; 

/*!
  \class vtkDataCollector 
  \brief Abstract base class of data collectors that collect tracked ultrasound data (images synchronized with tracking information)

  This class is an abstract base class of data collectors that collect ultrasound images synchronized with pose tracking information.

  \ingroup PlusLibDataCollection
*/ 
class VTK_EXPORT vtkDataCollector: public vtkImageAlgorithm
{
public:
  static vtkDataCollector *New();
  vtkTypeRevisionMacro(vtkDataCollector,vtkImageAlgorithm);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  /*! Read the configuration file in XML format and set up the devices */
  virtual PlusStatus ReadConfiguration( vtkXMLDataElement* aDataCollectionConfig ) = 0;

  /*! Disconnect from devices */
  virtual PlusStatus Disconnect() = 0;

  /*! Connect to devices */
  virtual PlusStatus Connect() = 0;

  /*! Stop data collection */
  virtual PlusStatus Stop() = 0;

  /*! Start data collection  */
  virtual PlusStatus Start() = 0;

  /*! Synchronize the connected devices */
  virtual PlusStatus Synchronize( const char* bufferOutputFolder = NULL, bool acquireDataOnly = false ) = 0;

  /*! Return the most recent synchronized timestamp in the buffers */
  virtual PlusStatus GetMostRecentTimestamp(double &ts) = 0;

  /*! Return the oldest synchronized timestamp in the buffers */
  virtual PlusStatus GetOldestTimestamp(double &ts) = 0; 

  /*! Get the most recent tracked frame from devices with each tool transforms */
  virtual PlusStatus GetTrackedFrame(TrackedFrame* trackedFrame, bool calibratedTransform = false) = 0; 

  /*!
    Get the tracked frame list from devices since time specified
    \param frameTimestamp The oldest timestamp we search for in the buffer. If -1 get all frames in the time range since the most recent timestamp. Out parameter - changed to timestamp of last added frame
    \param trackedFrameList Tracked frame list used to get the newly acquired frames into. The new frames are appended to the tracked frame.
    \param maxNumberOfFramesToAdd The maximum number of latest frames acquired from the buffers (till most recent timestamp). If -1 get all frames in the time range since frameTimestamp
  */
  virtual PlusStatus GetTrackedFrameList(double& frameTimestamp, vtkTrackedFrameList* trackedFrameList, int maxNumberOfFramesToAdd = -1) = 0; 

  /*!
    Get the tracked frame list from devices since time specified
    \param frameTimestamp The oldest timestamp we search for in the buffer. If -1 get all frames in the time range since the most recent timestamp. Out parameter - changed to timestamp of last added frame
    \param trackedFrameList Tracked frame list used to get the newly acquired frames into. The new frames are appended to the tracked frame.
    \param samplingRateSec Sampling rate for getting the frames in seconds (timestamps are in seconds too)
  */
  virtual PlusStatus GetTrackedFrameListSampled(double& frameTimestamp, vtkTrackedFrameList* trackedFrameList, double samplingRateMs) = 0; 

  /*! Get the tracked frame from devices by time with each tool transforms */
  virtual PlusStatus GetTrackedFrameByTime(double time, TrackedFrame* trackedFrame, bool calibratedTransform = false) = 0; 

  /*! Get transformation with timestamp from tracker  */
  virtual PlusStatus GetTransformWithTimestamp(vtkMatrix4x4* toolTransMatrix, double& transformTimestamp, TrackerStatus& status, const char* aToolName, bool calibratedTransform = false) = 0; 

  /*! Set video and tracker local time offset  */
  virtual void SetLocalTimeOffset(double videoOffset, double trackerOffset) = 0; 

  /*! Set the Tracking only flag */
  virtual void SetTrackingOnly(bool) = 0;

  /*! Set the Video only flag */
  virtual void SetVideoOnly(bool) = 0;

public:
  /*! Get video source of ultrasound */
  virtual vtkPlusVideoSource* GetVideoSource() = 0;

  /*! Get tracker  */
  virtual vtkTracker* GetTracker() = 0;
  
  /*! Get frame size */
  virtual void GetFrameSize(int aDim[2]) = 0;

  /*! Get the Connected flag  */
  vtkGetMacro(Connected,bool);
  
  /*! Get the Tracking only flag */
  vtkGetMacro(TrackingEnabled,bool);

  /*! Get the Video only flag */
  vtkGetMacro(VideoEnabled,bool);

  typedef void (*ProgressBarUpdatePtr)(int percent);
  /*! Callback function for progress bar refreshing */  
  virtual void SetProgressBarUpdateCallbackFunction(ProgressBarUpdatePtr cb) = 0;

protected:
  /*! Set the Connected flag  */
  vtkSetMacro(Connected,bool);  
  /*! Set the Connected flag  */
  vtkBooleanMacro(Connected, bool); 

protected:
  vtkDataCollector();
  virtual ~vtkDataCollector();

private:
  /*! Decides which data collector to create and instantiates it */
  static vtkDataCollector* CreateDataCollectorAccordingToDeviceSetConfiguration();

protected:
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

  /*! Pointer to the progress bar update callback function */ 
  ProgressBarUpdatePtr          ProgressBarUpdateCallbackFunction; 

private:
  vtkDataCollector(const vtkDataCollector&);
  void operator=(const vtkDataCollector&);

};

#endif
