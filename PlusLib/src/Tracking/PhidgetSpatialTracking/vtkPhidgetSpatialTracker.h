/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPhidgetSpatialTracker_h
#define __vtkPhidgetSpatialTracker_h

#include "vtkTracker.h"

#include <phidget21.h>

#include "MadgwickAHRS.h"
#include "MahonyAHRS.h"


class vtkTrackerBuffer; 

/*!
\class vtkPhidgetSpatialTracker 
\brief Interface for the Phidget 3/3/3 tracker 

This class talks with PhidgetSpatial 3/3/3 accelerometer/magnetometer/gyroscope device.

Gyroscope zeroing is performed automatically at device connect, therefore the sensor shall not be moved
for 2 seconds after the Connect() call.

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

protected:

  vtkPhidgetSpatialTracker();
  ~vtkPhidgetSpatialTracker();

  /*! Initialize the tracking device */
  PlusStatus InitPhidgetSpatialTracker();

  /*! 
  Start the tracking system.  The tracking system is brought from its ground state into full tracking mode.
  The device will only be reset if communication cannot be established without a reset.
  */
  PlusStatus InternalStartTracking();

  /*! Stop the tracking system and bring it back to its ground state: Initialized, not tracking, at 9600 Baud. */
  PlusStatus InternalStopTracking();

  static int CCONV SpatialDataHandler(CPhidgetSpatialHandle spatial, void *trackerPtr, CPhidgetSpatial_SpatialEventDataHandle *data, int count);

  /*! Create a transformation matrix from a transformed x vector */
  static void ConvertVectorToTransformationMatrix(double *inputVector, vtkMatrix4x4* outputMatrix);

private:  // Functions.

  vtkPhidgetSpatialTracker( const vtkPhidgetSpatialTracker& );
  void operator=( const vtkPhidgetSpatialTracker& );  


private:  // Variables.

  CPhidgetSpatialHandle SpatialDeviceHandle;

  unsigned int FrameNumber;
  double TrackerTimeToSystemTimeSec; // time_System = time_Tracker + TrackerTimeToSystemTimeSec
  bool TrackerTimeToSystemTimeComputed; // the time offset is always computed when the first frame is received after start tracking
  double LastAhrsUpdateTime; // last AHRS update time (in system time)

  vtkMatrix4x4* LastAccelerometerToTrackerTransform;
  vtkMatrix4x4* LastGyroscopeToTrackerTransform;
  vtkMatrix4x4* LastMagnetometerToTrackerTransform;
  vtkMatrix4x4* LastOrientationSensorToTrackerTransform;

  vtkTrackerTool* AccelerometerTool;
  vtkTrackerTool* GyroscopeTool;
  vtkTrackerTool* MagnetometerTool;
  vtkTrackerTool* OrientationSensorTool;

  /*!
    Gain values used by the AHRS algorithm (Mahony: first parameter is proportional, second is integral gain; Madgwick: only the first parameter is used)
    Higher gain gives higher reliability to accelerometer&magnetometer data.
    Higher gain advantage: on startup the initial orientation is reached more quickly (0.1 => takes 10 sec; 0.5 => takes 2-3 sec)
    Higher gain disadvantage: tracking is less robust, e.g., when starting/ending the rotation there could be a slicght extra "swing" in the output
  */
  double AhrsAlgorithmGain[2];

  MadgwickAHRS MadgwickAhrsAlgo;
  MahonyAHRS MahonyAhrsAlgo;

};

#endif
