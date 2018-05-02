/*=Plus=header=begin======================================================
  Progra  : Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusIntelRealSenseCamera_h
#define __vtkPlusIntelRealSenseCamera_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

#include "rs.hpp"

/*!
  \class vtkPlusIntelRealSenseCamera
  \brief Interface class to Intel RealSense cameras
  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusIntelRealSenseCamera : public vtkPlusDevice
{
public:

  static vtkPlusIntelRealSenseCamera *New();
  
  vtkTypeMacro(vtkPlusIntelRealSenseCamera, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Read configuration from xml data */
  PlusStatus ReadConfiguration(vtkXMLDataElement* config);
  /*! Write configuration to xml data */
  PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Manage device frozen state */
  PlusStatus FreezeDevice(bool freeze);

  /*! Is this device a tracker */
  bool IsTracker() const { return false; }

  /*! Get an update from the tracking system and push the new transforms to the tools. This function is called by the tracker thread.*/
  PlusStatus InternalUpdate();

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

protected:
  vtkPlusIntelRealSenseCamera();
  ~vtkPlusIntelRealSenseCamera();

  virtual PlusStatus InternalConnect() VTK_OVERRIDE;
  virtual PlusStatus InternalDisconnect() VTK_OVERRIDE;

  /*! Pointer to the IntelRealSenseCameraInterface class instance */
  //IntelRealSenseCameraInterface* MT;

  /*! Non-zero if the tracker has been initialized */
  int IsTrackingInitialized;

  /*! Index of the last frame number. This is used for providing a frame number when the tracker doesn't return any transform */
  double LastFrameNumber;
  
  unsigned int FrameNumber;
  std::string CameraCalibrationFile;
  std::string DeviceName;
  rs2::pipeline pipe;

//  std::shared_ptr<cv::Mat>                  Frame;

};

#endif
