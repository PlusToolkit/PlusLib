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
#include "rsutil.h"

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

  vtkGetStdStringMacro(RgbDataSourceName);
  vtkGetStdStringMacro(DepthDataSourceName);

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

  vtkSetStdStringMacro(RgbDataSourceName);
  vtkSetStdStringMacro(DepthDataSourceName);

  virtual PlusStatus InternalConnect() VTK_OVERRIDE;
  virtual PlusStatus InternalDisconnect() VTK_OVERRIDE;

  rs2_stream vtkPlusIntelRealSenseCamera::find_stream_to_align(const std::vector<rs2::stream_profile>& streams);
  bool vtkPlusIntelRealSenseCamera::profile_changed(const std::vector<rs2::stream_profile>& current, const std::vector<rs2::stream_profile>& prev);

  /*! Pointer to the IntelRealSenseCameraInterface class instance */
  //IntelRealSenseCameraInterface* MT;

  /*! Index of the last frame number. This is used for providing a frame number when the tracker doesn't return any transform */
  double LastFrameNumber;
  
  unsigned int FrameNumber;

  std::string RgbDataSourceName;
  std::string DepthDataSourceName;

  vtkPlusDataSource* aSourceRGB;
  vtkPlusDataSource* aSourceDEPTH;
  
  rs2::pipeline pipe;
  rs2::pipeline_profile profile;
  rs2_stream align_to;

  int num_devices = -1;

};

#endif
