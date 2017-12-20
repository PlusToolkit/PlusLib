/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusOvrvisionProVideoSource_h
#define __vtkPlusOvrvisionProVideoSource_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

// OvrvisionPro SDK includes
#include <ovrvision_pro.h>

// OpenCV includes
#include <opencv2/core/mat.hpp>

/*!
  \class __vtkPlusOvrvisionProVideoSource_h
  \brief Class for providing video input from the OvrvisionPro stereo camera device
  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusOvrvisionProVideoSource : public vtkPlusDevice
{
public:
  static vtkPlusOvrvisionProVideoSource* New();
  vtkTypeMacro(vtkPlusOvrvisionProVideoSource, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual bool IsTracker() const
  {
    return false;
  }

  /// Read configuration from xml data
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);
  /// Write configuration to xml data
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /// Perform any completion tasks once configured
  virtual PlusStatus NotifyConfigured();

  vtkGetStdStringMacro(LeftEyeDataSourceName);
  vtkGetStdStringMacro(RightEyeDataSourceName);
  vtkGetStdStringMacro(ProcessingModeName);
  vtkGetStdStringMacro(Vendor);

  std::array<unsigned int, 2> GetResolution() const;
  vtkGetMacro(Framerate, int);
  vtkGetMacro(CameraSync, bool);
  vtkSetMacro(Exposure, int);

protected:
  vtkSetStdStringMacro(LeftEyeDataSourceName);
  vtkSetStdStringMacro(RightEyeDataSourceName);
  vtkSetStdStringMacro(ProcessingModeName);
  vtkSetStdStringMacro(Vendor);

  vtkSetMacro(CameraSync, bool);
  vtkGetMacro(Exposure, int);

  /// Device-specific connect
  virtual PlusStatus InternalConnect();

  /// Device-specific disconnect
  virtual PlusStatus InternalDisconnect();

  /// Device-specific on-update function
  virtual PlusStatus InternalUpdate();

  void ConfigureProcessingMode();

  static std::string CamPropToString(OVR::Camprop format);
  static OVR::Camprop StringToCamProp(const std::string& format);

protected:
  vtkPlusOvrvisionProVideoSource();
  ~vtkPlusOvrvisionProVideoSource();

protected:
  // Requested capture format
  OVR::OvrvisionPro OvrvisionProHandle;
  OVR::Camprop RequestedFormat;
  OVR::ROI RegionOfInterest;
  OVR::Camqt ProcessingMode;
  bool CameraSync;

  // Cache variables from the config file
  std::array<unsigned int, 2> Resolution;
  int Framerate;
  std::string ProcessingModeName;
  bool IsCapturingRGB;
  std::string Vendor;
  int Exposure;

  // Record which data source corresponds to which eye
  std::string LeftEyeDataSourceName;
  std::string RightEyeDataSourceName;

  vtkPlusDataSource* LeftEyeDataSource;
  vtkPlusDataSource* RightEyeDataSource;

#if defined(PLUS_USE_OPENCL)
  cv::UMat LeftImageCL;
  cv::UMat RightImageCL;
#endif

  cv::Mat LeftImage;
  cv::Mat RightImage;

private:
  static vtkPlusOvrvisionProVideoSource* ActiveDevice;
  vtkPlusOvrvisionProVideoSource(const vtkPlusOvrvisionProVideoSource&);   // Not implemented.
  void operator=(const vtkPlusOvrvisionProVideoSource&);   // Not implemented.
};

#endif