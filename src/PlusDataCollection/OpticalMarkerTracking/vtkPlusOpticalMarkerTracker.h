/*=Plus=header=begin======================================================
  Progra  : Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusOpticalMarkerTracker_h
#define __vtkPlusOpticalMarkerTracker_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

// aruco headers
// TODO: move these to cxx files (use PIMPL - vtkInternal - if needed)
#include "markerdetector.h"
#include "cameraparameters.h"
#include "posetracker.h"

class vtkPlusDataSource;
class vtkMatrix4x4;

/*!
  \class vtkPlusOpticalMarkerTracker
  \brief Virtual device that tracks fiducial markers on the input channel in real time.
  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusOpticalMarkerTracker : public vtkPlusDevice
{
public:
  /*! Defines whether or not depth stream is used. */
  enum TRACKING_METHOD
  {
    OPTICAL_ONLY,
    OPTICAL_DEPTH
  };

  static vtkPlusOpticalMarkerTracker *New();
  vtkTypeMacro(vtkPlusOpticalMarkerTracker,vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Read main config settings from XML. */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement*);

  /*! Write current main config settings to XML. */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement*);

  /*! Probe to see if the tracking system is present. */
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

  /* This device is a virtual tracker. */
  virtual bool IsTracker() const { return true; }
  virtual bool IsVirtual() const { return true; }

  /*!
    Get image from the camera into VTK images. If an input arguments is NULL then that image is not retrieved.
  */
  PlusStatus GetImage(vtkImageData* leftImage, vtkImageData* rightImage);

  vtkGetMacro(TrackingMethod, TRACKING_METHOD);
  vtkGetMacro(CameraCalibrationFile, std::string);
  vtkGetMacro(MarkerDictionary, std::string);
protected:
  /*! Constructor */
  vtkPlusOpticalMarkerTracker();

  /*! Destructor */
  ~vtkPlusOpticalMarkerTracker();

  vtkSetMacro(TrackingMethod, TRACKING_METHOD);
  vtkSetMacro(CameraCalibrationFile, std::string);
  vtkSetMacro(MarkerDictionary, std::string);

  /*! */
  PlusStatus InternalStartRecording();

  /*! Stop the tracking system and bring it back to its initial state. */
  PlusStatus InternalStopRecording();

  class vtkInternal;
  vtkInternal* Internal;

  unsigned int FrameNumber;
  double LastProcessedInputDataTimestamp;

private:
  vtkPlusOpticalMarkerTracker(const vtkPlusOpticalMarkerTracker&);
  void operator=(const vtkPlusOpticalMarkerTracker&);

  /*!  */
  class TrackedTool
  {
  public:
    TrackedTool(int MarkerId, float MarkerSizeMm, std::string ToolSourceId);
    TrackedTool(std::string MarkerMapFile, string ToolSourceId);
    enum TOOL_MARKER_TYPE
    {
      SINGLE_MARKER,
      MARKER_MAP
    };
    int MarkerId;
    TOOL_MARKER_TYPE ToolMarkerType;
    float MarkerSizeMm;
    std::string MarkerMapFile;
    std::string ToolSourceId;
    std::string ToolName;
    vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  };

  // TODO: add error checking
  void BuildTransformMatrix(vtkSmartPointer<vtkMatrix4x4> transformMatrix, cv::Mat Rvec, cv::Mat Tvec);

  /*!  */
  std::string CameraCalibrationFile;

  /*!  */
  TRACKING_METHOD TrackingMethod;

  /*!  */
  std::string MarkerDictionary;

  /*!  */
  std::vector<TrackedTool> Tools;

  /*! Pointer to main aruco objects */
  aruco::MarkerDetector MDetector;
  aruco::CameraParameters CP;
  aruco::MarkerPoseTracker MarkerPoseTracker;
  vector<aruco::Marker> markers;
};

#endif
