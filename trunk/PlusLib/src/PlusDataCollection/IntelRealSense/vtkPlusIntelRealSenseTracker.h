/*=Plus=header=begin======================================================
  Progra  : Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusIntelRealSenseTracker_h
#define __vtkPlusIntelRealSenseTracker_h

#include "vtkPlusDataCollectionExport.h"

#include "vtkPlusDevice.h"

class IntelRealSenseTrackerInterface;
class vtkMatrix4x4;

/*!
  \class vtkPlusIntelRealSenseTracker
  \brief Interface class to Intel RealSense cameras
  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusIntelRealSenseTracker : public vtkPlusDevice
{
public:

  static vtkPlusIntelRealSenseTracker *New();
  vtkTypeMacro(vtkPlusIntelRealSenseTracker,vtkPlusDevice);

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
  
  /*! Get the status of the IntelRealSenseTracker (Tracking or not) */
  vtkGetMacro(IsTrackingInitialized, int);
 
  /*! Read IntelRealSenseTracker configuration and update the tracker settings accordingly */
  virtual PlusStatus ReadConfiguration( vtkXMLDataElement* config );

  /*! Write current IntelRealSenseTracker configuration settings to XML */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* rootConfigElement);

  /*! Connect to the tracker hardware */
  PlusStatus InternalConnect();
  /*! Disconnect from the tracker hardware */
  PlusStatus InternalDisconnect();

  vtkSetMacro(CameraCalibrationFile, std::string);
  vtkGetMacro(CameraCalibrationFile, std::string);

  vtkSetMacro(DeviceName, std::string);
  vtkGetMacro(DeviceName, std::string);

protected:
  vtkPlusIntelRealSenseTracker();
  ~vtkPlusIntelRealSenseTracker();

  class vtkInternal;
  vtkInternal* Internal;

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

  /*! Pointer to the IntelRealSenseTrackerInterface class instance */
  IntelRealSenseTrackerInterface* MT;

  /*! Non-zero if the tracker has been initialized */
  int IsTrackingInitialized;

  /*! Index of the last frame number. This is used for providing a frame number when the tracker doesn't return any transform */
  double LastFrameNumber;
  
  unsigned int FrameNumber;
  std::string CameraCalibrationFile;
  std::string DeviceName;


#ifdef USE_INTELREALSENSE_TIMESTAMPS
  double TrackerTimeToSystemTimeSec; // time_System = time_Tracker + TrackerTimeToSystemTimeSec
  bool TrackerTimeToSystemTimeComputed; // the time offset is always computed when the first frame is received after start tracking
#endif

private:
  vtkPlusIntelRealSenseTracker(const vtkPlusIntelRealSenseTracker&);
  void operator=(const vtkPlusIntelRealSenseTracker&);  
};

#endif
