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
class vtkMatrix4x4;

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

  /*! Return the most recent synchronized timestamp in the buffers */
  virtual PlusStatus GetMostRecentTimestamp(double &ts) = 0;

  /*! Return the oldest synchronized timestamp in the buffers */
  virtual PlusStatus GetOldestTimestamp(double &ts) = 0; 

  /*! Get the most recent tracked frame from devices with each tool transforms */
  virtual PlusStatus GetTrackedFrame(TrackedFrame* trackedFrame) = 0; 

  /*!
    Get the tracked frame list from devices since time specified
    \param frameTimestamp The oldest timestamp we search for in the buffer. If -1 get all frames in the time range since the most recent timestamp. Out parameter - changed to timestamp of last added frame
    \param trackedFrameList Tracked frame list used to get the newly acquired frames into. The new frames are appended to the tracked frame.
    \param maxNumberOfFramesToAdd The maximum number of latest frames acquired from the buffers (till most recent timestamp). If -1 get all frames in the time range since frameTimestamp
    \param videoEnabled Flag whether video buffer is recorded
    \param trackingEnabled Flag whether tracker buffer is recorded
  */
  virtual PlusStatus GetTrackedFrameList(double& frameTimestamp, vtkTrackedFrameList* trackedFrameList, int maxNumberOfFramesToAdd = -1, bool videoEnabled=true, bool trackingEnabled=true) = 0;

  /*!
    Get the tracked frame list from devices since time specified
    \param frameTimestamp The oldest timestamp we search for in the buffer. If -1 get all frames in the time range since the most recent timestamp. Out parameter - changed to timestamp of last added frame
    \param trackedFrameList Tracked frame list used to get the newly acquired frames into. The new frames are appended to the tracked frame.
    \param samplingRateSec Sampling rate for getting the frames in seconds (timestamps are in seconds too)
  */
  virtual PlusStatus GetTrackedFrameListSampled(double& frameTimestamp, vtkTrackedFrameList* trackedFrameList, double samplingRateSec) = 0; 

  /*! Get the tracked frame from devices by time with each tool transforms */
  virtual PlusStatus GetTrackedFrameByTime(double time, TrackedFrame* trackedFrame) = 0; 

  /*! Set video and tracker local time offset  */
  virtual void SetLocalTimeOffsetSec(double videoOffsetSec, double trackerOffsetSec) = 0;

  /*! Get tracker tool reference frame name (eg. "Tracker")  */
  virtual PlusStatus GetTrackerToolReferenceFrame(std::string &aToolReferenceFrameName) = 0;

  /*! Get frame rate */
  virtual PlusStatus GetFrameRate(double &aFrameRate) = 0;

  /*! Set acquisition start time to the current time in relative ( vtkAccurateTimer::GetSystemTime() )
      and absolute ( vtkTimerLog::GetUniversalTime() ) format */ 
  virtual void SetAcquisitionStartTime(); 

  /*! Reset acquisition start time to 0.0 for both relative and absolute time */ 
  virtual void ResetAcquisitionStartTime(); 

  /*! Get acquisition start time in relative format ( vtkAccurateTimer::GetSystemTime() ) */ 
  virtual double GetAcquisitionStartTimeRelative(); 

  /*! Get acquisition start time in absolute ( vtkTimerLog::GetUniversalTime() ) format */
  virtual double GetAcquisitionStartTimeAbsoluteUTC(); 

public:
  /*! Get frame size */
  virtual void GetFrameSize(int aDim[2]) = 0;

  /*! Set the Tracking only flag */
  virtual void SetTrackingOnly(bool) = 0;

  /*! Set the Video only flag */
  virtual void SetVideoOnly(bool) = 0;

  /*! Get the Connected flag  */
  vtkGetMacro(Connected,bool);
  
  /*! Get the Tracking only flag */
  vtkGetMacro(TrackingEnabled,bool);

  /*! Get the Video only flag */
  vtkGetMacro(VideoEnabled,bool);

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
  
protected:
  vtkDataCollector();
  virtual ~vtkDataCollector();

private:
  /*! Decides which data collector to create and instantiates it */
  static vtkDataCollector* CreateDataCollectorAccordingToDeviceSetConfiguration();

protected:
  /*! The timestamp filtering methods require some time to initialize. Synchronization will ignore data that are acquired during startup delay. */
  double                        StartupDelaySec; 

  /*! Successfully connected to devices */
  bool                          Connected; 

  /*! Collecting tracking data is enabled */
  bool                          TrackingEnabled;

  /*! Collecting image data is enabled */
  bool                          VideoEnabled;

  /*! Acquisition start time elapsed since AccurateTimer class instantiation (relative time) 
      vtkAccurateTimer::GetSystemTime();
  */
  double StartTimeRelative;

  /*! Acquisition start time elapsed since 00:00:00 January 1, 1970, UTC (absolute time) 
      vtkTimerLog::GetUniversalTime()
  */
  double StartTimeAbsoluteUTC;

private:
  vtkDataCollector(const vtkDataCollector&);
  void operator=(const vtkDataCollector&);

};

#endif
