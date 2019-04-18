/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusIntuitiveDaVinciTestTracker_h
#define __vtkPlusIntuitiveDaVinciTestTracker_h

// Local includes
#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

// VTK includes
#include <vtkObjectFactory.h>

// Intuitive includes
#include "IntuitiveDaVinciTest.h"

class vtkMatrix4x4;
class IntuitiveDaVinciTest;
class vtkPlusIntuitiveDaVinciTestTracker;

namespace vtkPlusIntuitiveDaVinciTestTrackerUtilities
{
  void ISICALLBACK streamCB(void* userdata);
  void ISICALLBACK eventCB(ISI_MANIP_INDEX mid, ISI_EVENT_ID event_id, ISI_INT args[ISI_NUM_EVENT_ARGS],  void* userdata);
};

/* This class talks with the da Vinci Surgical System. */
class vtkPlusDataCollectionExport vtkPlusIntuitiveDaVinciTestTracker : public vtkPlusDevice
{
public:
  static vtkPlusIntuitiveDaVinciTestTracker* New();
  vtkTypeMacro(vtkPlusIntuitiveDaVinciTestTracker, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Hardware device SDK version. */
  virtual std::string GetSdkVersion();

  virtual bool IsTracker() const { return true; }

  /*! Probe to see if the tracking system is present. */
  virtual PlusStatus Probe();

  /*! Read da Vinci configuration and update the tracker settings accordingly */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* rootConfigElement);

  /*! Write current da Vinci configuration settings to XML */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* rootConfigElement);

  IntuitiveDaVinci* GetDaVinci() const;

  vtkSetStdStringMacro(IpAddr);
  vtkGetStdStringMacro(IpAddr);

  vtkSetMacro(Port, unsigned int);
  vtkGetMacro(Port, unsigned int);

  vtkSetStdStringMacro(Password);
  vtkGetStdStringMacro(Password);

protected:
  vtkPlusIntuitiveDaVinciTestTracker();
  ~vtkPlusIntuitiveDaVinciTestTracker();

  /*! Connect to the tracker hardware */
  virtual PlusStatus InternalConnect();

  /*! Disconnect from the tracker hardware */
  virtual PlusStatus InternalDisconnect();

  /*!  Start the tracking system. */
  virtual PlusStatus InternalStartRecording();

  /*! Stop the tracking system and bring it back to its initial state. */
  virtual PlusStatus InternalStopRecording();

protected:
  /*! Pointer to the MicronTrackerInterface class instance */
  IntuitiveDaVinci*   DaVinci;

  /*! Index of the last frame number. This is used for providing a frame number when the tracker doesn't return any transform */
  double              LastFrameNumber;

  unsigned int        FrameNumber;

#ifdef USE_DAVINCI_TIMESTAMPS
  double              TrackerTimeToSystemTimeSec; // time_System = time_Tracker + TrackerTimeToSystemTimeSec
  bool                TrackerTimeToSystemTimeComputed; // the time offset is always computed when the first frame is received after start tracking
#endif

  std::string         IpAddr;
  unsigned int        Port;
  std::string         Password;

private:
  vtkPlusIntuitiveDaVinciTestTracker(const vtkPlusIntuitiveDaVinciTestTracker&);
  void operator=(const vtkPlusIntuitiveDaVinciTestTracker&);

  void StreamCallback(void);

  friend void vtkPlusIntuitiveDaVinciTestTrackerUtilities::streamCB(void* userData);
  friend void vtkPlusIntuitiveDaVinciTestTrackerUtilities::eventCB(ISI_MANIP_INDEX mid, ISI_EVENT_ID event_id, ISI_INT args[ISI_NUM_EVENT_ARGS],  void* userdata);

  ISI_MANIP_INDEX getManipIndexFromName(const std::string& toolName);
  void setVtkMatrixFromISITransform(vtkMatrix4x4& vtkMatrix, ISI_TRANSFORM* isiMatrix);

  static const int CONNECT_RETRY_DELAY_SEC = 1.0;
  static const int MAX_ATTEMPTS = 5;
};

#endif
