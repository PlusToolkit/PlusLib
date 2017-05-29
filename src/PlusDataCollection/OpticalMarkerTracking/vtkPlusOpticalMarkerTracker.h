/*=Plus=header=begin======================================================
  Progra  : Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusOpticalMarkerTracker_h
#define __vtkPlusOpticalMarkerTracker_h

#include "vtkPlusDataCollectionExport.h"

#include "vtkPlusDevice.h"

// class vtkMatrix4x4; //could be used for creating transform matrix out of aruco data?
class vtkPlusTransformRepository;
class vtkPlusTrackedFrameProcessor;


//----------------------------------------------------------------------------

/*!
  \class vtkPlusOpticalMarkerTracker
  \brief Virtual device that tracks fiducial markers on the input channel in real time.
  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusOpticalMarkerTracker : public vtkPlusDevice
{
public:

  // TODO: I can use this to indicate if the depth data was used in the processing or not
  // Should change these elements to be more descriptive.
  enum TRACKING_METHOD_TYPE
  {
    TRACKING_3D,
    TRACKING_2D
  };

  static vtkPlusOpticalMarkerTracker *New();
  vtkTypeMacro(vtkPlusOpticalMarkerTracker,vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Read main config and update the settings accordingly. */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement*);

  /*! Write current main config settings to XML. */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement*);

  /*!
  Probe to see if the tracking system is present.
  */
  PlusStatus Probe();

  /*! Connect to the tracker hardware */
  PlusStatus InternalConnect();

  /*! Disconnect from the tracker hardware */
  PlusStatus InternalDisconnect();

  /*!
  Get an update from the tracking system and push the new transforms
  to the tools.  This should only be used within vtkTracker.cxx.
  */
  virtual PlusStatus InternalUpdate();

  // TODO: Do I need an enable processing member?
  /*! Enables processing frames. It can be used for pausing the processing. */
  //vtkGetMacro(EnableProcessing, bool);
  //void SetEnableProcessing(bool aValue);

  /*! This device is a virtual tracker. */
  virtual bool IsTracker() const { return true; }
  virtual bool IsVirtual() const { return true; }

  /*!
    Get image from the camera into VTK images. If an input arguments is NULL then that image is not retrieved.
  */
  PlusStatus GetImage(vtkImageData* leftImage, vtkImageData* rightImage);

  vtkGetMacro(TrackingMethod, TRACKING_METHOD_TYPE);
  vtkGetMacro(CalibrationFile, std::string);
  vtkGetMacro(EnableProcessing, bool);
protected:
  /*! Constructor */
  vtkPlusOpticalMarkerTracker();

  /*! Destructor */
  ~vtkPlusOpticalMarkerTracker();

  vtkSetMacro(TrackingMethod, TRACKING_METHOD_TYPE);
  vtkSetMacro(CalibrationFile, std::string);
  vtkSetMacro(EnableProcessing, bool);
  /*!
    Start the tracking system.  The tracking system is brought from
    its ground state into full tracking mode.  The POLARIS will
    only be reset if communication cannot be established without
    a reset.
  */
  PlusStatus InternalStartRecording();

  /*! Stop the tracking system and bring it back to its initial state. */
  PlusStatus InternalStopRecording();

  class vtkInternal;
  vtkInternal* Internal;

  /*! Refresh the loaded markers by loading them from the Markers directory */
  PlusStatus RefreshMarkerTemplates();

  /*! Returns the transformation matrix of the index_th marker */
  void GetTransformMatrix(int markerIndex, vtkMatrix4x4* transformMatrix);

  /*! Non-zero if the tracker has been initialized */
  int IsTrackingInitialized;

  /*! Index of the last frame number. This is used for providing a frame number when the tracker doesn't return any transform */
  unsigned int LastFrameNumber;

  unsigned int FrameNumber;
  double LastProcessedInputDataTimestamp;
  std::string CameraCalibrationFile;

  TRACKING_METHOD_TYPE TrackingMethod;

private:
  void PrintConfig();
  vtkPlusOpticalMarkerTracker(const vtkPlusOpticalMarkerTracker&);
  void operator=(const vtkPlusOpticalMarkerTracker&); 

  /*! Aruco generated calibration file for optical camera being used.*/
  std::string CalibrationFile;

  bool EnableProcessing;
};

#endif
