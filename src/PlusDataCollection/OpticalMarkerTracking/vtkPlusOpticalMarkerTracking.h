/*=Plus=header=begin======================================================
  Progra  : Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusOpticalMarkerTracking_h
#define __vtkPlusOpticalMarkerTracking_h

#include "vtkPlusDataCollectionExport.h"

#include "vtkPlusDevice.h"

class vtkMatrix4x4;
class vtkPlusTransformRepository;
class vtkPlusTrackedFrameProcessor;

/*!
  \class vtkPlusOpticalMarkerTracking
  \brief Virtual device that tracks fiducial markers on the input channel in real time.
  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusOpticalMarkerTracking : public vtkPlusDevice
{
public:

  // TODO: I can use this to indicate if the depth data was used in the processing or not
  // Should change these elements to be more descriptive.
  enum TRACKING_METHOD_TYPE
  {
    TRACKING_3D,
    TRACKING_2D
  };

  static vtkPlusOpticalMarkerTracking *New();
  vtkTypeMacro(vtkPlusOpticalMarkerTracking,vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Read main config and update the settings accordingly. */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement*);

  /*! Write current main config settings to XML. */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement*);
  /*!
    Get an update from the tracking system and push the new transforms
    to the tools.  This should only be used within vtkTracker.cxx.
  */
  virtual PlusStatus InternalUpdate();

    /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

  /*! Enables processing frames. It can be used for pausing the processing. */
  vtkGetMacro(EnableProcessing, bool);
  void SetEnableProcessing(bool aValue);

  /*! This device is a virtual tracker. */
  virtual bool IsTracker() const { return true; }
  virtual bool IsVirtual() const { return true; }

  /*!
    Probe to see if the tracking system is present.
  */
  PlusStatus Probe();

  /*!
    Get image from the camera into VTK images. If an input arguments is NULL then that image is not retrieved.
  */
  PlusStatus GetImage(vtkImageData* leftImage, vtkImageData* rightImage);


  // What are these?
  vtkSetMacro(CameraCalibrationFile, std::string);
  vtkGetMacro(CameraCalibrationFile, std::string);

  vtkSetMacro(TrackingMethod, TRACKING_METHOD_TYPE);
  vtkGetMacro(TrackingMethod, TRACKING_METHOD_TYPE);

protected:
  /*! Constructor */
  vtkPlusOpticalMarkerTracking();

  /*! Destructor */
  ~vtkPlusOpticalMarkerTracking();

  /*! Connect to the tracker hardware */
  PlusStatus InternalConnect();

  /*! Disconnect from the tracker hardware */
  PlusStatus InternalDisconnect();

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
  double LastFrameNumber;
  
  unsigned int FrameNumber;
  std::string CameraCalibrationFile;

  TRACKING_METHOD_TYPE TrackingMethod;

private:
  vtkPlusOpticalMarkerTracking(const vtkPlusOpticalMarkerTracking&);
  void operator=(const vtkPlusOpticalMarkerTracking&);  
};

#endif
