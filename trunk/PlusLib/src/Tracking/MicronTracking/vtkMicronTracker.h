/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkMicronTracker_h
#define __vtkMicronTracker_h

#include <limits.h>
#include <float.h>
#include <math.h>
#include <ctype.h>
#include <iomanip>

#include "vtkUnsignedCharArray.h"
#include "vtkImageFlip.h"

#include "vtkMath.h"
#include "vtkTimerLog.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkMicronTracker.h"
#include "vtkTrackerTool.h"
#include "vtkObjectFactory.h"
#include "vtkImageImport.h"
#include "vtkTracker.h"
#include "vtkIntArray.h"
#include "vtkLongArray.h"
#include "vtkDoubleArray.h"
#include "time.h"

class MicronTrackerInterface;

/*!
  \class vtkMicronTracker
  \brief Interface class to Claron MicronTracker optical trackers
  \ingroup PlusLibTracking
*/
class VTK_EXPORT vtkMicronTracker : public vtkTracker
{
public:

  static vtkMicronTracker *New();
  vtkTypeMacro(vtkMicronTracker,vtkTracker);

  /*! Hardware device SDK version. */
  virtual std::string GetSdkVersion(); 
 
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
  PlusStatus ReadConfiguration( vtkXMLDataElement* config );

  /*! Write current MicronTracker configuration settings to XML */
  PlusStatus WriteConfiguration(vtkXMLDataElement* rootConfigElement);

  /*! Connect to the tracker hardware */
  PlusStatus Connect();
  /*! Disconnect from the tracker hardware */
  PlusStatus Disconnect();

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
  PlusStatus InternalStartTracking();

  /*! Stop the tracking system and bring it back to its initial state. */
  PlusStatus InternalStopTracking();

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
