/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusCameraControlParameters_h
#define __vtkPlusCameraControlParameters_h

#include "vtkPlusDataCollectionExport.h"

#include "vtkPlusParameters.h"

#include <string>
#include <map>

/*!
\class vtkPlusCameraControlParameters
\brief This class is used to store a configuration of the imaging parameters of an Microsoft Media Foundation camera device.
This class exists mainly for two reasons:
* Provide a standard interface for accessing camera parameters
* Enable standardized API for operating on camera parameters
\ingroup PlusLibDataCollection

Currently contains the following items
* CameraControlParameters
* PanDegrees
* TiltDegrees
* RollDegrees
* ZoomMm
* IrisFStop
* ExposureLog2Seconds
* AutoExposure
* FocusMm
* AutoFocus
*/

class vtkPlusDataCollectionExport vtkPlusCameraControlParameters : public vtkPlusParameters
{
public:
  static const char* CAMERA_CONTROL_XML_ELEMENT_TAG;
  static const char* KEY_PAN_DEGREES;
  static const char* KEY_TILT_DEGREES;
  static const char* KEY_ROLL_DEGREES;
  static const char* KEY_ZOOM_MM;
  static const char* KEY_IRIS_FSTOP;
  static const char* KEY_EXPOSURE_LOG2SECONDS;
  static const char* KEY_AUTO_EXPOSURE;
  static const char* KEY_FOCUS_MM;
  static const char* KEY_AUTO_FOCUS;

public:
  static vtkPlusCameraControlParameters* New();
  vtkTypeMacro(vtkPlusCameraControlParameters, vtkPlusParameters);

  /*! Set camera pan in degrees from -180 to +180. */
  PlusStatus SetPanDegrees(double aPanDegrees);
  /*! Get camera pan in degrees from -180 to +180. */
  PlusStatus GetPanDegrees(double& aPanDegrees) const;
  /*! Set camera tilt in degrees from -180 to +180. */
  PlusStatus SetTiltDegrees(double aTiltDegrees);
  /*! Get camera tilt in degrees from -180 to +180. */
  PlusStatus GetTiltDegrees(double& aTiltDegrees) const;
  /*! Set camera roll in degrees from -180 to +180. */
  PlusStatus SetRollDegrees(double aRollDegrees);
  /*! Get camera roll in degrees from -180 to +180. */
  PlusStatus GetRollDegrees(double& aRollDegrees) const;
  /*! Set camera zoom in mm. Values from 10 to 600. */
  PlusStatus SetZoomMm(double aZoomMm);
  /*! Get camera zoom in mm. Values from 10 to 600. */
  PlusStatus GetZoomMm(double& aZoomMm) const;
  /*! Set camera iris in f-stops. Values in units of fstop * 10. */
  PlusStatus SetIrisFStop(int aIrisFStop);
  /*! Get camera iris in f-stops. Values in units of fstop * 10. */
  PlusStatus GetIrisFStop(int& aIrisFStop) const;
  /*! Set camera exposure in log2 seconds.
      Value   Seconds
      -3      1/8
      -2      1/4
      -1      1/2
       0      1
       1      2
       2      4 */
  PlusStatus SetExposureLog2Seconds(int aExposureLog2Seconds);
  /*! Get camera exposure in log2 seconds.
       Value   Seconds
       -3      1/8
       -2      1/4
       -1      1/2
        0      1
        1      2
        2      4 */
  PlusStatus GetExposureLog2Seconds(int& aExposureLog2Seconds) const;
  /*! Set camera auto exposure enabled. */
  PlusStatus SetAutoExposure(bool aAutoExposure);
  /*! Get camera auto exposure enabled. */
  PlusStatus GetAutoExposure(bool& aAutoExposure) const;
  /*! Set camera focus in mm. */
  PlusStatus SetFocusMm(double aFocusMm);
  /*! Get camera focus in mm. */
  PlusStatus GetFocusMm(double& aFocusMm) const;
  /*! Set camera auto focus enabled. */
  PlusStatus SetAutoFocus(bool aAutoFocus);
  /*! Get camera auto focus enabled. */
  PlusStatus GetAutoFocus(bool& aAutoFocus) const;

protected:
  vtkPlusCameraControlParameters();
  virtual ~vtkPlusCameraControlParameters();

  const char* GetXMLElementName() override { return vtkPlusCameraControlParameters::CAMERA_CONTROL_XML_ELEMENT_TAG; }
};

#endif
