/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusOpticalMarkerTracker.h"

#include "PlusVideoFrame.h"
#include "vtkImageData.h"
#include "vtkImageImport.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkPlusDataSource.h"

#include <fstream>
#include <iostream>
#include <set>

#include "includes/aruco/markerdetector.h"
#include "includes/opencv2/highgui.hpp"

vtkStandardNewMacro(vtkPlusOpticalMarkerTracker);
//----------------------------------------------------------------------------
class TrackedTool
{
public:
  TrackedTool()
  {

  }

private:
  std::vector<int> markerIDs;
};
//----------------------------------------------------------------------------

class vtkPlusOpticalMarkerTracker::vtkInternal
{
public:
  vtkPlusOpticalMarkerTracker *External;


  vtkInternal(vtkPlusOpticalMarkerTracker* external)
    : External(external)
  {
  }

  virtual ~vtkInternal()
  {
  }
};

//----------------------------------------------------------------------------
vtkPlusOpticalMarkerTracker::vtkPlusOpticalMarkerTracker()
: vtkPlusDevice()
{
  // TODO: Update this
}

//----------------------------------------------------------------------------
vtkPlusOpticalMarkerTracker::~vtkPlusOpticalMarkerTracker() 
{

}

//----------------------------------------------------------------------------
void vtkPlusOpticalMarkerTracker::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpticalMarkerTracker::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_CSTRING_ATTRIBUTE_OPTIONAL(CalibrationFile, deviceConfig);
  XML_READ_ENUM2_ATTRIBUTE_OPTIONAL(TrackingMethod, deviceConfig, "3D", TRACKING_3D, "2D", TRACKING_2D);

  XML_FIND_NESTED_ELEMENT_REQUIRED(dataSourcesElement, deviceConfig, "DataSources");

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpticalMarkerTracker::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(trackerConfig, rootConfigElement);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpticalMarkerTracker::InternalConnect()
{
  // get calibration file path && check file exists
  std::string calibFilePath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(this->CameraCalibrationFile);
  LOG_INFO("Use aruco camera calibration file: " << calibFilePath);
  if (!vtksys::SystemTools::FileExists(calibFilePath.c_str(), true))
  {
    LOG_WARNING("Unable to findaruco camera calibration file at: " << calibFilePath);
  }

  // TODO: Setup aruco objects
  // to test aruco
  aruco::MarkerDetector MDetector;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpticalMarkerTracker::InternalDisconnect()
{

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpticalMarkerTracker::Probe()
{
 
  return PLUS_SUCCESS;
} 

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpticalMarkerTracker::InternalStartRecording()
{

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpticalMarkerTracker::InternalStopRecording()
{

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpticalMarkerTracker::InternalUpdate()
{
  // check if processing is enabled? this should probably be handled by whether or not there is camera input

  // check input channel(s) are valid - i.e. contain proper number of video streams

  // acquire latest frame

  // check if data is recent and valid?

  // process image (aruco m.t. goes here)

  // check if output stream is valid

  // place image and tracking data on output streams

  return PLUS_SUCCESS;
}


