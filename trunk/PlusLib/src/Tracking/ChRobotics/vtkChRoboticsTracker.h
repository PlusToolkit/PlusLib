/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkChRoboticsTracker_h
#define __vtkChRoboticsTracker_h

#include "vtkTracker.h"

class vtkTrackerBuffer;
class SerialLine;
class ChrSerialPacket;

/*!
\class vtkChRoboticsTracker 
\brief Interface for the CH Robotics CHR-UM6 tracker 

This class talks with CH Robotics CHR-UM6 accelerometer/magnetometer/gyroscope device

\ingroup PlusLibTracking
*/
class VTK_EXPORT vtkChRoboticsTracker : public vtkTracker
{
public:

  static vtkChRoboticsTracker *New();
  vtkTypeMacro( vtkChRoboticsTracker,vtkTracker );
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

  vtkChRoboticsTracker();
  ~vtkChRoboticsTracker();

  /*! 
  Start the tracking system.  The tracking system is brought from its ground state into full tracking mode.
  The device will only be reset if communication cannot be established without a reset.
  */
  PlusStatus InternalStartTracking();

  /*! Stop the tracking system and bring it back to its ground state: Initialized, not tracking, at 9600 Baud. */
  PlusStatus InternalStopTracking();

  /*! Get the firmware version identification string */
  PlusStatus GetFirmwareVersion(std::string &firmwareVersion);

  PlusStatus SendPacket( ChrSerialPacket& packet );
  PlusStatus ReceivePacket( ChrSerialPacket& packet );

private:  // Functions.

  vtkChRoboticsTracker( const vtkChRoboticsTracker& );
  void operator=( const vtkChRoboticsTracker& );  


private:  // Variables.

  /*! Serial (RS232) line connection */
  SerialLine* Serial;

  /*! Used COM port number */
  long ComPort; 

  /*! Baud rate */
  long BaudRate; 

  unsigned int FrameNumber;
  double TrackerTimeToSystemTimeSec; // time_System = time_Tracker + TrackerTimeToSystemTimeSec
  bool TrackerTimeToSystemTimeComputed; // the time offset is always computed when the first frame is received after start tracking

  vtkMatrix4x4* LastAccelerometerToTrackerTransform;
  vtkMatrix4x4* LastGyroscopeToTrackerTransform;
  vtkMatrix4x4* LastMagnetometerToTrackerTransform;
  vtkMatrix4x4* LastOrientationSensorToTrackerTransform;

  vtkTrackerTool* AccelerometerTool;
  vtkTrackerTool* GyroscopeTool;
  vtkTrackerTool* MagnetometerTool;
  vtkTrackerTool* OrientationSensorTool;
};

#endif
