/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusCameraControlParameters.h"

#include <iterator>

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusCameraControlParameters);

//----------------------------------------------------------------------------

const char* vtkPlusCameraControlParameters::CAMERA_CONTROL_XML_ELEMENT_TAG   = "CameraControlParameters";
const char* vtkPlusCameraControlParameters::KEY_PAN_DEGREES = "PanDegrees";
const char* vtkPlusCameraControlParameters::KEY_TILT_DEGREES = "TiltDegrees";
const char* vtkPlusCameraControlParameters::KEY_ROLL_DEGREES = "RollDegrees";
const char* vtkPlusCameraControlParameters::KEY_ZOOM_MM = "ZoomMm";
const char* vtkPlusCameraControlParameters::KEY_IRIS_FSTOP = "IrisFStop";
const char* vtkPlusCameraControlParameters::KEY_EXPOSURE_LOG2SECONDS = "ExposureLog2Seconds";
const char* vtkPlusCameraControlParameters::KEY_AUTO_EXPOSURE = "AutoExposure";
const char* vtkPlusCameraControlParameters::KEY_FOCUS_MM = "FocusMm";
const char* vtkPlusCameraControlParameters::KEY_AUTO_FOCUS = "AutoFocus";

//----------------------------------------------------------------------------
vtkPlusCameraControlParameters::vtkPlusCameraControlParameters()
  : vtkPlusParameters()
{
  this->Parameters[KEY_PAN_DEGREES] = ParameterInfo("-1");
  this->Parameters[KEY_TILT_DEGREES] = ParameterInfo("-1");
  this->Parameters[KEY_ROLL_DEGREES] = ParameterInfo("-1");
  this->Parameters[KEY_ZOOM_MM] = ParameterInfo("-1");
  this->Parameters[KEY_IRIS_FSTOP] = ParameterInfo("-1");
  this->Parameters[KEY_EXPOSURE_LOG2SECONDS] = ParameterInfo("-1");
  this->Parameters[KEY_AUTO_EXPOSURE] = ParameterInfo("-1");
  this->Parameters[KEY_FOCUS_MM] = ParameterInfo("-1");
  this->Parameters[KEY_AUTO_FOCUS] = ParameterInfo("-1");
}

//----------------------------------------------------------------------------
vtkPlusCameraControlParameters::~vtkPlusCameraControlParameters()
{
  LOG_TRACE("vtkPlusCameraControlParameters::~vtkPlusCameraControlParameters()");
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCameraControlParameters::SetPanDegrees(double panDegrees)
{
  return this->SetValue<double>(KEY_PAN_DEGREES, panDegrees);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCameraControlParameters::GetPanDegrees(double& panDegrees) const
{
  return this->GetValue<double>(KEY_PAN_DEGREES, panDegrees);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCameraControlParameters::SetTiltDegrees(double tiltDegrees)
{
  return this->SetValue<double>(KEY_TILT_DEGREES, tiltDegrees);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCameraControlParameters::GetTiltDegrees(double& tiltDegrees) const
{
  return this->GetValue<double>(KEY_TILT_DEGREES, tiltDegrees);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCameraControlParameters::SetRollDegrees(double rollDegrees)
{
  return this->SetValue<double>(KEY_ROLL_DEGREES, rollDegrees);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCameraControlParameters::GetRollDegrees(double& rollDegrees) const
{
  return this->GetValue<double>(KEY_ROLL_DEGREES, rollDegrees);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCameraControlParameters::SetZoomMm(double zoomMm)
{
  return this->SetValue<double>(KEY_ZOOM_MM, zoomMm);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCameraControlParameters::GetZoomMm(double& zoomMm) const
{
  return this->GetValue<double>(KEY_ZOOM_MM, zoomMm);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCameraControlParameters::SetIrisFStop(int irisFStop)
{
  return this->SetValue<int>(KEY_IRIS_FSTOP, irisFStop);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCameraControlParameters::GetIrisFStop(int& irisFStop) const
{
  return this->GetValue<int>(KEY_IRIS_FSTOP, irisFStop);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCameraControlParameters::SetExposureLog2Seconds(int exposureLog2Seconds)
{
  return this->SetValue<int>(KEY_EXPOSURE_LOG2SECONDS, exposureLog2Seconds);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCameraControlParameters::GetExposureLog2Seconds(int& exposureLog2Seconds) const
{
  return this->GetValue<int>(KEY_EXPOSURE_LOG2SECONDS, exposureLog2Seconds);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCameraControlParameters::SetAutoExposure(bool autoExposure)
{
  return this->SetValue<std::string>(KEY_AUTO_EXPOSURE, autoExposure ? "True" : "False");
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCameraControlParameters::GetAutoExposure(bool& autoExposure) const
{
  std::string autoExposureString = "";
  if (!this->GetValue<std::string>(KEY_AUTO_EXPOSURE, autoExposureString))
  {
    return PLUS_FAIL;
  }

  autoExposure = STRCASECMP(autoExposureString.c_str(), "true") == 0;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCameraControlParameters::SetFocusMm(double focusMm)
{
  return this->SetValue<double>(KEY_FOCUS_MM, focusMm);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCameraControlParameters::GetFocusMm(double& focusMm) const
{
  return this->GetValue<double>(KEY_FOCUS_MM, focusMm);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCameraControlParameters::SetAutoFocus(bool autoFocus)
{
  return this->SetValue<std::string>(KEY_AUTO_FOCUS, autoFocus ? "True" : "False");
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCameraControlParameters::GetAutoFocus(bool& autoFocus) const
{
  std::string autoFocusString = "";
  if (!this->GetValue<std::string>(KEY_AUTO_FOCUS, autoFocusString))
  {
    return PLUS_FAIL;
  }

  autoFocus = STRCASECMP(autoFocusString.c_str(), "true") == 0;
  return PLUS_SUCCESS;
}
