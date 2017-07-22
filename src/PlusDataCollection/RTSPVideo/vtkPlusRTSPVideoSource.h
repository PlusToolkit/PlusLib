/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusRTSPVideoSource_h
#define __vtkPlusRTSPVideoSource_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

/*!
  \class vtkPlusRTSPVideoSource
  \brief Class for interfacing an RTSP stream (rtsp://127.0.0.1/...) and capturing frames into a Plus buffer

  Requires the PLUS_USE_RTSP_VIDEO option in CMake.
  Requires OpenCV with FFMPEG built (RTSP support)

  \ingroup PlusLibDataCollection
*/

namespace cv
{
  class VideoCapture;
  class Mat;
}

class vtkPlusDataCollectionExport vtkPlusRTSPVideoSource : public vtkPlusDevice
{
public:
  static vtkPlusRTSPVideoSource* New();
  vtkTypeMacro(vtkPlusRTSPVideoSource, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Read configuration from xml data */
  PlusStatus ReadConfiguration(vtkXMLDataElement* config);
  /*! Write configuration to xml data */
  PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Manage device frozen state */
  PlusStatus FreezeDevice(bool freeze);

  /*! Is this device a tracker */
  bool IsTracker() const {return false;}

  /*! Get an update from the tracking system and push the new transforms to the tools. This function is called by the tracker thread.*/
  PlusStatus InternalUpdate();

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

  vtkGetStdStringMacro(StreamURL);
  vtkSetStdStringMacro(StreamURL);

protected:
  vtkPlusRTSPVideoSource();
  ~vtkPlusRTSPVideoSource();

  PlusStatus InternalConnect();
  PlusStatus InternalDisconnect();

protected:
  std::string                       StreamURL;
  std::shared_ptr<cv::VideoCapture> Capture;
  std::shared_ptr<cv::Mat>          Frame;
};

#endif // __vtkPlusRTSPVideoSource_h