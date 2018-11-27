/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusDataCollector_h
#define __vtkPlusDataCollector_h

// Local includes
#include "igsioCommon.h"
#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

// VTK includes
#include <vtkObject.h>

//class igsioTrackedFrame; 
class vtkPlusChannel;
class vtkPlusDeviceFactory;
//class vtkIGSIOTrackedFrameList;
class vtkXMLDataElement;

/*!
\class vtkPlusDataCollector
\brief Manages devices that record image or positional data.

Provides an interface for clients to connect to a device set, and request data to the currently active devices.

\ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusDataCollector : public vtkObject
{
public:
  static vtkPlusDataCollector* New();
  vtkTypeMacro(vtkPlusDataCollector, vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*!
  Read main configuration from xml data
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);
  PlusStatus ReadConfiguration(const std::string& fileName);
  /*!
  Write main configuration to xml data
  */
  PlusStatus WriteConfiguration(vtkXMLDataElement* aConfig);

  /*!
  Set the factory instance used to create devices
  */
  void SetDeviceFactory(vtkSmartPointer<vtkPlusDeviceFactory> factory);
  /*!
  Get the factory instance used to create devices
  */
  vtkPlusDeviceFactory& GetDeviceFactory();

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
    Compute loop times for saved datasets (time intersection of the two buffers)
    itemTimestamp = loopStartTime + (actualTimestamp - startTimestamp) % loopTime
  */
  virtual PlusStatus SetLoopTimes();

  /*!
    Add a device to the device list
    \param aDevice the device to add
  */
  PlusStatus AddDevice(vtkPlusDevice* aDevice);

  /*!
    Return the requested device
    \param aDevice the device pointer to fill
    \param aDeviceId the ID of the requested device
  */
  PlusStatus GetDevice(vtkPlusDevice*& aDevice, const std::string& aDeviceId) const;

  /*!
    Return the requested channel
    \param aChannel the device pointer to fill
    \param aChannelId the ID of the requested device
  */
  PlusStatus GetChannel(vtkPlusChannel*& aChannel, const std::string& aChannelId) const;
  PlusStatus GetFirstChannel(vtkPlusChannel*& aChannel) const;

  /*!
    Allow iteration over devices
  */
  DeviceCollectionConstIterator GetDeviceConstIteratorBegin() const;
  DeviceCollectionConstIterator GetDeviceConstIteratorEnd() const;

  /*!
    Have each device dump their buffers to disk
    \param aDirectory directory to dump to
    \param maxTimeLimitSec Maximum time spent in the function (in sec)
  */
  PlusStatus DumpBuffersToDirectory(const char* aDirectory);

  /*!
    Get tracking data in a tracked frame list since time specified
    \param aTimestamp The oldest timestamp we search for in the buffer. If -1 get all frames in the time range since the most recent timestamp. Out parameter - changed to timestamp of last added frame
    \param aTrackedFrameList Tracked frame list used to get the newly acquired frames into. The new frames are appended to the tracked frame.
  */
  PlusStatus GetTrackingData(vtkPlusChannel* aRequestedChannel, double& aTimestampFrom, vtkIGSIOTrackedFrameList* aTrackedFrameList);

  /*!
    Get video data in a tracked frame list since time specified
    \param aTimestamp The oldest timestamp we search for in the buffer. If -1 get all frames in the time range since the most recent timestamp. Out parameter - changed to timestamp of last added frame
    \param aTrackedFrameList Tracked frame list used to get the newly acquired frames into. The new frames are appended to the tracked frame.
  */
  virtual PlusStatus GetVideoData(vtkPlusChannel* aRequestedChannel, double& aTimestamp, vtkIGSIOTrackedFrameList* aTrackedFrameList);

  /*
  * Functions to manage the currently active stream mixers
  */
  PlusStatus GetDevices(DeviceCollection& OutVector) const;

  /*
    Identify if the device is started or not
  */
  bool GetStarted() const;
  bool IsStarted() const { return this->GetStarted(); }

  /*
    Is the system connected?
  */
  bool GetConnected() const;

  /*! Set startup delay in sec to give some time to the buffers for proper initialization */
  vtkSetMacro(StartupDelaySec, double);
  /*! Get startup delay in sec to give some time to the buffers for proper initialization */
  vtkGetMacro(StartupDelaySec, double);

protected:
  vtkPlusDataCollector();
  virtual ~vtkPlusDataCollector();

  /*! The timestamp filtering methods require some time to initialize. Synchronization will ignore data that are acquired during startup delay. */
  double StartupDelaySec;

  vtkSmartPointer<vtkPlusDeviceFactory> DeviceFactory;

  DeviceCollection Devices;

  bool Connected;
  bool Started;

private:
  vtkPlusDataCollector(const vtkPlusDataCollector&);
  void operator=(const vtkPlusDataCollector&);
};

#endif
