/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusOpenCVCaptureVideoSource_h
#define __vtkPlusOpenCVCaptureVideoSource_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

// OpenCV includes
#include <opencv2/videoio.hpp>

/*!
\class vtkPlusOpenCVCaptureVideoSource
\brief Class for interfacing an OpenCVC capture device and recording frames into a Plus buffer

Requires the PLUS_USE_OpenCVCapture_VIDEO option in CMake.
Requires OpenCV with FFMPEG built (for RTSP support)

\ingroup PlusLibDataCollection
*/

namespace cv
{
  class VideoCapture;
  class Mat;
}

class vtkPlusDataCollectionExport vtkPlusOpenCVCaptureVideoSource : public vtkPlusDevice
{
public:
  static vtkPlusOpenCVCaptureVideoSource* New();
  vtkTypeMacro(vtkPlusOpenCVCaptureVideoSource, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);
  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Manage device frozen state */
  PlusStatus FreezeDevice(bool freeze);

  /*! Is this device a tracker */
  virtual bool IsTracker() const { return false; }

  /*! Get an update from the tracking system and push the new transforms to the tools. This function is called by the tracker thread.*/
  virtual PlusStatus InternalUpdate();

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

  vtkGetStdStringMacro(VideoURL);
  vtkSetStdStringMacro(VideoURL);

  vtkGetMacro(DeviceIndex, int);
  vtkSetMacro(DeviceIndex, int);

  vtkGetMacro(AutofocusEnabled, bool);
  vtkSetMacro(AutofocusEnabled, bool);

  vtkGetMacro(AutoexposureEnabled, bool);
  vtkSetMacro(AutoexposureEnabled, bool);

  vtkGetMacro(FourCC, std::string);
  vtkSetMacro(FourCC, std::string);

  static cv::VideoCaptureAPIs CaptureAPIFromString(const std::string& apiString);
  static std::string StringFromCaptureAPI(cv::VideoCaptureAPIs api);

protected:
  vtkPlusOpenCVCaptureVideoSource();
  ~vtkPlusOpenCVCaptureVideoSource();

  virtual PlusStatus InternalConnect();
  virtual PlusStatus InternalDisconnect();

protected:
  std::string                       VideoURL;
  int                               DeviceIndex;
  std::shared_ptr<cv::VideoCapture> Capture;
  std::shared_ptr<cv::Mat>          Frame;
  std::shared_ptr<cv::Mat>          UndistortedFrame;
  cv::VideoCaptureAPIs              RequestedCaptureAPI;
  bool                              AutofocusEnabled;
  bool                              AutoexposureEnabled;
  std::string                       FourCC;

  FrameSizeType                     FrameSize;

  std::shared_ptr<cv::Mat>          CameraMatrix;
  std::shared_ptr<cv::Mat>          DistortionCoefficients;
};

#endif // __vtkPlusOpenCVCaptureVideoSource_h
