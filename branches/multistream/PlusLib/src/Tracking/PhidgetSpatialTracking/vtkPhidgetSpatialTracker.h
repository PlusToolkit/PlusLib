/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPhidgetSpatialTracker_h
#define __vtkPhidgetSpatialTracker_h

#include "vtkTracker.h"

#include <phidget21.h>

class vtkTrackerBuffer; 
class AhrsAlgo;

/*!
\class vtkPhidgetSpatialTracker 
\brief Interface for the Phidget 3/3/3 tracker 

This class talks with PhidgetSpatial 3/3/3 accelerometer/magnetometer/gyroscope device.

Tracker coordinate system: South-West-Down.

Sensor coordinate system is drawn on the sensor PCB. If the PCB is laying flat on the table
with cable socket towards East then the axis directions are: South-West-Down.

Tools:
  Accelerometer, Gyroscope, Magnetometer: Raw sensor measurements.
    The values are stored in the translation part of the transformation matrix.
    The rotation part is identity.
  TiltSensor: 2-DOF sensor tilt is computed as a rotation matrix. Only the accelerometer is used.
  OrientationSensor: 3-DOF sensor orientation is computed using sensor fusion.
    With ...IMU algorithm only the accelerometer and gyroscope data are used.
    With ...AHRS algorithm accelerometer, gyroscope, and magnetometer data are used.

Gyroscope zeroing is performed automatically at device connect, therefore the sensor shall not be moved
for 2 seconds after the vtkPhidgetSpatialTracker::Connect() call if the OrientationSensor or Accelerometer tool is used.

AHRS algorithm gain values:
 The Madgwick method uses only one parameter (beta). A value of 1.0 works well at 125Hz.
 The Mahony method uses two parameters (proportional and integral). A value of proportional=15.0, integral=1.0 work well at 125Hz.

If magnetic fields nearby the sensor have non-negligible effect then compass can be ignored by choosing an ..._IMU AHRS algorithm
(as opposed to ..._MARG) or compass correction may be performed (see http://www.phidgets.com/docs/Compass_Primer). If compass correction
parameters cannot be stored in the device flash then they should be set in vtkPhidgetSpatialTracker::Connect().

The magnetometer has an internal calibration mechanism, which is running continuously. During this internal calibration no measurement
data can be retrieved from the magnetometer. When magnetometer data is not available then the associated tool status is set to INVALID.

\ingroup PlusLibTracking
*/
class
  VTK_EXPORT
vtkPhidgetSpatialTracker : public vtkTracker
{
public:

  static vtkPhidgetSpatialTracker *New();
  vtkTypeMacro( vtkPhidgetSpatialTracker,vtkTracker );
  void PrintSelf( ostream& os, vtkIndent indent );

  /*! Connect to device */
  PlusStatus Connect();

  /*! Disconnect from device */
  virtual PlusStatus Disconnect();

  /*! 
  Probe to see if the tracking system is present on the specified serial port.  
  */
  PlusStatus Probe();

  /*!
  Get an update from the tracking system and push the new transforms
  to the tools.  This should only be used within vtkTracker.cxx.
  This method is called by the tracker thread.
  */
  PlusStatus InternalUpdate();

  /*! Read configuration from xml data */
  PlusStatus ReadConfiguration(vtkXMLDataElement* config); 

  /*! Write configuration to xml data */
  PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*
  * Override the default resettable behavior of the device
  * because Phidget trackers can be reset
  */
  virtual bool IsResettable();

  /*!
    Reset the device
  */
  virtual PlusStatus Reset();

protected:

  vtkPhidgetSpatialTracker();
  ~vtkPhidgetSpatialTracker();

  /*! 
  Start the tracking system.  The tracking system is brought from its ground state into full tracking mode.
  The device will only be reset if communication cannot be established without a reset.
  */
  PlusStatus InternalStartTracking();

  /*! Stop the tracking system and bring it back to its ground state: Initialized, not tracking, at 9600 Baud. */
  PlusStatus InternalStopTracking();

  static int CCONV SpatialDataHandler(CPhidgetSpatialHandle spatial, void *trackerPtr, CPhidgetSpatial_SpatialEventDataHandle *data, int count);

private:  // Functions.

  vtkPhidgetSpatialTracker( const vtkPhidgetSpatialTracker& );
  void operator=( const vtkPhidgetSpatialTracker& );  

private:  // Variables.  

  CPhidgetSpatialHandle SpatialDeviceHandle;

  unsigned int FrameNumber;
  double TrackerTimeToSystemTimeSec; // time_System = time_Tracker + TrackerTimeToSystemTimeSec
  bool TrackerTimeToSystemTimeComputed; // the time offset is always computed when the first frame is received after start tracking  

  vtkMatrix4x4* LastAccelerometerToTrackerTransform;
  vtkMatrix4x4* LastGyroscopeToTrackerTransform;
  vtkMatrix4x4* LastMagnetometerToTrackerTransform;
  vtkMatrix4x4* LastTiltSensorToTrackerTransform;
  vtkMatrix4x4* LastOrientationSensorToTrackerTransform;

  vtkTrackerTool* AccelerometerTool;
  vtkTrackerTool* GyroscopeTool;
  vtkTrackerTool* MagnetometerTool;
  vtkTrackerTool* TiltSensorTool;
  vtkTrackerTool* OrientationSensorTool;

  enum AHRS_METHOD
  {
    AHRS_MADGWICK,
    AHRS_MAHONY
  };

  AhrsAlgo* AhrsAlgo;

  /*!
    If AhrsUseMagnetometer enabled (a ..._MARG algorithm is chosen) then heading will be estimated using magnetometer data.
    Otherwise (when a ..._IMU algorithm is chosen) only the gyroscope data will be used for getting the heading information.
    IMU may be more noisy, but not sensitive to magnetic field distortions.
  */
  bool AhrsUseMagnetometer;

  /*!
    Gain values used by the AHRS algorithm (Mahony: first parameter is proportional, second is integral gain; Madgwick: only the first parameter is used)
    Higher gain gives higher reliability to accelerometer&magnetometer data.
  */
  double AhrsAlgorithmGain[2];

  /*! last AHRS update time (in system time) */
  double AhrsLastUpdateTime;

  /*!
    In tilt sensor mode we don't use the magnetometer, so we have to provide a direction reference.
    The orientation is specified by specifying an axis that will always point to the "West" direction.
    Recommended values:
    If sensor axis 0 points down (the sensor plane is about vertical) => TiltSensorDownAxisIndex = 2.
    If sensor axis 1 points down (the sensor plane is about vertical) => TiltSensorDownAxisIndex = 0.
    If sensor axis 2 points down (the sensor plane is about horizontal) => TiltSensorDownAxisIndex = 1.
  */
  int TiltSensorWestAxisIndex;

};

#endif
