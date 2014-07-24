#ifndef __vtkDaVinciTracker_h
#define __vtkDaVinciTracker_h

#include "time.h"
#include "vtkDoubleArray.h"
#include "vtkImageFlip.h"
#include "vtkImageImport.h"
#include "vtkIntArray.h"
#include "vtkLongArray.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkMicronTracker.h"
#include "vtkObjectFactory.h"
#include "vtkPlusDevice.h"
#include "vtkTimerLog.h"
#include "vtkPlusDataSource.h"
#include "vtkTransform.h"
#include "vtkUnsignedCharArray.h"
#include <ctype.h>
#include <float.h>
#include <iomanip>
#include <limits.h>
#include <math.h>

#include "daVinci.h"
#include "isi_api.h"

class daVinci;

/* 
This class talks with the da Vinci Surgical System.

*/

class VTK_EXPORT vtkDaVinciTracker : public vtkPlusDevice
{
public:

  static vtkDaVinciTracker *New();

  vtkTypeMacro(vtkDaVinciTracker,vtkPlusDevice);

  /*! Hardware device SDK version. */
  virtual std::string GetSdkVersion(); 

  virtual bool IsTracker() const { return true; }

  /*!
  Probe to see if the tracking system is present.
  */
  PlusStatus Probe();

  /*!
  Get an update from the tracking system and push the new transforms
  to the tools.  This should only be used within vtkTracker.cxx.
  */
  PlusStatus InternalUpdate(); 

  /*! Get the status of the MicronTracker (Tracking or not) */
  vtkGetMacro(IsDaVinciTrackingInitialized, int);

  /*! Read da Vinci configuration and update the tracker settings accordingly */
  virtual PlusStatus ReadConfiguration( vtkXMLDataElement* config );

  /*! Write current da Vinci configuration settings to XML */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* rootConfigElement);

  /*! Connect to the tracker hardware */
  PlusStatus InternalConnect();

  /*! Disconnect from the tracker hardware */
  PlusStatus InternalDisconnect();

  daVinci* GetDaVinci() { return this->mDaVinci; };

protected:
  vtkDaVinciTracker();
  ~vtkDaVinciTracker();

  /*!
  Start the tracking system. 
  */
  PlusStatus InternalStartRecording();

  /*! Stop the tracking system and bring it back to its initial state. */
  PlusStatus InternalStopRecording();

  ///*! Refresh the loaded markers by loading them from the Markers directory */
  //PlusStatus RefreshMarkerTemplates();

  /*! Pointer to the MicronTrackerInterface class instance */
  daVinci* mDaVinci;

  /*! Non-zero if the tracker has been initialized */
  bool IsDaVinciTrackingInitialized;

  /*! Index of the last frame number. This is used for providing a frame number when the tracker doesn't return any transform */
  double LastFrameNumber;

  unsigned int FrameNumber;

#ifdef USE_DAVINCI_TIMESTAMPS
  double TrackerTimeToSystemTimeSec; // time_System = time_Tracker + TrackerTimeToSystemTimeSec
  bool TrackerTimeToSystemTimeComputed; // the time offset is always computed when the first frame is received after start tracking
#endif

private:
  vtkDaVinciTracker(const vtkDaVinciTracker&);
  void operator=(const vtkDaVinciTracker&);  

  void ISICALLBACK streamCB(void *userdata);
  void ISICALLBACK eventCB(ISI_MANIP_INDEX mid, 
    ISI_EVENT_ID event_id, 
    ISI_INT args[ISI_NUM_EVENT_ARGS], 
    void *userdata);

  int getManipIndexFromName(std::string& toolName);

  void setVtkMatrixFromISITransform(vtkMtarix4x4* vtkMatrix, ISI_TRANSFORM* isiMatrix);

  private const int CONNECT_RETRY_DELAY_SEC = 1.0;
  private const int MAX_ATTEMPTS = 5;
};

#endif
