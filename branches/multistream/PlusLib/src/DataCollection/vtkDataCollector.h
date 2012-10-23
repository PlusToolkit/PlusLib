/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkDataCollector_h
#define __vtkDataCollector_h

#include "PlusCommon.h"
#include "vtkObject.h"
#include "vtkPlusDevice.h"
#include <vector>

class TrackedFrame;
class vtkTrackedFrameList;
class vtkVirtualStreamMixer;
class vtkXMLDataElement;

/*!
\class vtkDataCollector 
\brief Manages devices that record image or positional data.

Provides an interface for clients to connect to a device set, and request data to the currently active devices.

\ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkDataCollector : public vtkObject
{
public:
  static vtkDataCollector *New();
  vtkTypeRevisionMacro(vtkDataCollector, vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  /*! 
  Read main configuration from xml data 
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);
  /*! 
  Write main configuration to xml data 
  */
  PlusStatus WriteConfiguration(vtkXMLDataElement* aConfig);

  /*! 
  Start the devices. The device is brought from
  its ground state (i.e. on but not necessarily initialized) into
  full active mode.  This method calls start on the current connected device(s)
  */
  PlusStatus Start();

  /*! 
  Stop the tracking system and bring it back to its ground state. This method calls Stop on the current connected device(s)
  */
  PlusStatus Stop();

  /*! 
  Connect to device(s). Connection is needed for recording or single frame grabbing 
  */
  PlusStatus Connect();

  /*!
  Disconnect from active device(s).
  This method must be called before application exit, or else the
  application might hang during exit.
  */
  PlusStatus Disconnect();

  /*! 
  Return the most recent synchronized timestamp in the buffers 
  */
  PlusStatus GetMostRecentTimestamp(double &ts) const;

  /*
    Does the system have a tracker connected
  */
  bool GetTrackingEnabled() const;

  /*
    Is the system connected?
  */
  bool GetConnected() const;

  /*!
  Get the tracked frame list from devices since time specified
  \param aTimestamp The oldest timestamp we search for in the buffer. If -1 get all frames in the time range since the most recent timestamp. Out parameter - changed to timestamp of last added frame
  \param aTrackedFrameList Tracked frame list used to get the newly acquired frames into. The new frames are appended to the tracked frame.
  \param aMaxNumberOfFramesToAdd The maximum number of latest frames acquired from the buffers (till most recent timestamp). If -1 get all frames in the time range since aTimestamp
  */
  PlusStatus GetTrackedFrameList(double& aTimestamp, vtkTrackedFrameList* aTrackedFrameList, int aMaxNumberOfFramesToAdd = -1) const;

  /*!
    Return the requested device
    \param aDevice the device pointer to fill
    \param aDeviceId the ID of the requested device
  */
  PlusStatus GetDevice(vtkPlusDevice* &aDevice, const std::string &aDeviceId) const;

  /*!
    Very important function, requests the latest tracked frame from the data collector
    The data collector can only process this if a device exists that can output tracked frames.
    So far, we define this as a virtual device to perform this function specifically.
    \param trackedFrame the frame to fill
    */
  PlusStatus GetTrackedFrame( TrackedFrame* trackedFrame );

  /*! 
    Get the tracked frame from devices by time with each tool transforms
    \param time The closes frame to this timestamp will be retrieved
    \param trackedFrame The output where the tracked frame information will be copied
  */
  virtual PlusStatus GetTrackedFrameByTime(double time, TrackedFrame* trackedFrame); 

  /*!
    Have each device dump their buffers to disk
    \param aDirectory directory to dump to
    \param maxTimeLimitSec Maximum time spent in the function (in sec)
  */
  PlusStatus DumpBuffersToDirectory( const char * aDirectory );

  /*
  * Functions to manage the currently active tracked frame producer
  */
  PlusStatus GetTrackedFrameProducers( std::vector<vtkVirtualStreamMixer*> &OutVector ) const;
  PlusStatus SetSelectedTrackedFrameProducer( const std::string &aDeviceId );
  PlusStatus GetSelectedTrackedFrameProducer( vtkPlusDevice* &aDevice );

  /*
  * Functions to pass on to the active tracked frame producer
  */
  bool GetTrackingDataAvailable() const;
  bool GetVideoDataAvailable() const;

  /*! Set startup delay in sec to give some time to the buffers for proper initialization */
  vtkSetMacro(StartupDelaySec, double); 
  /*! Get startup delay in sec to give some time to the buffers for proper initialization */
  vtkGetMacro(StartupDelaySec, double);

  /*! Get the Tracking only flag */
  //vtkGetMacro(TrackingEnabled,bool);
  /*! Set the Tracking only flag */
  //void SetTrackingOnly(bool);

  /*! Get the Video only flag */
  //vtkGetMacro(VideoEnabled,bool);
  /*! Set the Video only flag */
  //void SetVideoOnly(bool);

protected:
  vtkDataCollector();
  virtual ~vtkDataCollector();

  /*! The timestamp filtering methods require some time to initialize. Synchronization will ignore data that are acquired during startup delay. */
  double StartupDelaySec; 

  DeviceCollection Devices;

  vtkVirtualStreamMixer* SelectedStreamMixer;

  bool Connected;
  bool Started;
  bool TrackingEnabled;
  bool VideoEnabled;

private:
  vtkDataCollector(const vtkDataCollector&);
  void operator=(const vtkDataCollector&);
};

#endif
