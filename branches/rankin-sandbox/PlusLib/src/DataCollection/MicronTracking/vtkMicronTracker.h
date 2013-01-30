/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkMicronTracker_h
#define __vtkMicronTracker_h

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
#include "vtkPlusStreamTool.h"
#include "vtkTransform.h"
#include "vtkUnsignedCharArray.h"
#include <ctype.h>
#include <float.h>
#include <iomanip>
#include <limits.h>
#include <math.h>

class MicronTrackerInterface;

/*!
  \class vtkMicronTracker
  \brief Interface class to Claron MicronTracker optical trackers
  \ingroup PlusLibTracking
*/
class VTK_EXPORT vtkMicronTracker : public vtkPlusDevice
{
public:

  static vtkMicronTracker *New();
  vtkTypeMacro(vtkMicronTracker,vtkPlusDevice);

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

  /*!
    Get image from the camera into VTK images. If an input arguments is NULL then that image is not retrieved.
  */
  PlusStatus GetImage(vtkImageData* leftImage, vtkImageData* rightImage);
  
  /*! Get the status of the MicronTracker (Tracking or not) */
  vtkGetMacro(IsMicronTrackingInitialized, int);
 
  /*! Read MicronTracker configuration and update the tracker settings accordingly */
  virtual PlusStatus ReadConfiguration( vtkXMLDataElement* config );

  /*! Write current MicronTracker configuration settings to XML */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* rootConfigElement);

  /*! Connect to the tracker hardware */
  PlusStatus InternalConnect();
  /*! Disconnect from the tracker hardware */
  PlusStatus InternalDisconnect();

  MicronTrackerInterface* GetMicronTrackerInterface() { return this->MT; };

protected:
  vtkMicronTracker();
  ~vtkMicronTracker();

  /*!
    Start the tracking system.  The tracking system is brought from
    its ground state into full tracking mode.  The POLARIS will
    only be reset if communication cannot be established without
    a reset.
  */
  PlusStatus InternalStartRecording();

  /*! Stop the tracking system and bring it back to its initial state. */
  PlusStatus InternalStopRecording();

  /*! Refresh the loaded markers by loading them from the Markers directory */
  PlusStatus RefreshMarkerTemplates();

  /*! Returns the transformation matrix of the index_th marker */
  void GetTransformMatrix(int markerIndex, vtkMatrix4x4* transformMatrix);

  /*! Pointer to the MicronTrackerInterface class instance */
  MicronTrackerInterface* MT;

  /*! Non-zero if the tracker has been initialized */
  int IsMicronTrackingInitialized;

  /*! Index of the last frame number. This is used for providing a frame number when the tracker doesn't return any transform */
  double LastFrameNumber;
  
  unsigned int FrameNumber;
  std::string TemplateDirectory;
  std::string IniFile;

#ifdef USE_MICRONTRACKER_TIMESTAMPS
  double TrackerTimeToSystemTimeSec; // time_System = time_Tracker + TrackerTimeToSystemTimeSec
  bool TrackerTimeToSystemTimeComputed; // the time offset is always computed when the first frame is received after start tracking
#endif

private:
  vtkMicronTracker(const vtkMicronTracker&);
  void operator=(const vtkMicronTracker&);  
};

#endif
