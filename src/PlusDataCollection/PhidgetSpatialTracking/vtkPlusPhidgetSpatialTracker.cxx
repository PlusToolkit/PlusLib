/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusPhidgetSpatialTracker.h"

#include "MadgwickAhrsAlgo.h"
#include "MahonyAhrsAlgo.h"
#include "PlusMath.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkPlusDataSource.h"
#include "vtkTransform.h"
#include "vtkXMLDataElement.h"
#include "vtksys/SystemTools.hxx"
#include <math.h>
#include <sstream>

#include <phidget22.h>

static const int SERIAL_NUMBER_UNDEFINED = -1;

vtkStandardNewMacro(vtkPlusPhidgetSpatialTracker);

class PhidgetSpatialCallbackClass
{
public:

  //callback that will run if the Spatial is attached to the computer
  static void CCONV AttachHandler(PhidgetHandle spatial, void* trackerPtr)
  {
    int serialNo = 0;
    Phidget_getDeviceSerialNumber(spatial, &serialNo);
    vtkPlusPhidgetSpatialTracker* tracker = (vtkPlusPhidgetSpatialTracker*)trackerPtr;
    LOG_DEBUG("Phidget spatial sensor attached to device " << tracker->GetDeviceId() << ". Serial number: " << serialNo);
  }

  //callback that will run if the Spatial is detached from the computer
  static void CCONV DetachHandler(PhidgetHandle spatial, void* trackerPtr)
  {
    int serialNo;
    Phidget_getDeviceSerialNumber(spatial, &serialNo);
    LOG_DEBUG("Phidget spatial sensor detached: " << serialNo);
  }

  //callback that will run if the Spatial generates an error
  static void CCONV ErrorHandler(PhidgetHandle spatial, void* trackerPtr, Phidget_ErrorEventCode ErrorCode, const char* unknown)
  {
    LOG_ERROR("Phidget spatial sensor error: " << ErrorCode << " (" << unknown << ")");
  }

  //callback that is called when a data sample is acquired
  static void CCONV SpatialDataHandler(PhidgetSpatialHandle spatial, void* trackerPtr, 
    const double acceleration[3], const double angularRate[3], const double magneticField[3], double timeTrackerMsec)
  {
    vtkPlusPhidgetSpatialTracker* tracker = (vtkPlusPhidgetSpatialTracker*)trackerPtr;
    if (! tracker->IsRecording())
    {
      // Received phidget tracking data when not tracking
      return;
    }

    if (!tracker->TrackerTimeToSystemTimeComputed)
    {
      const double timeSystemSec = vtkIGSIOAccurateTimer::GetSystemTime();
      tracker->TrackerTimeToSystemTimeSec = timeSystemSec - timeTrackerMsec/1000.0;
      tracker->TrackerTimeToSystemTimeComputed = true;
    }

    const double timeSystemSec = timeTrackerMsec/1000.0 + tracker->TrackerTimeToSystemTimeSec;
    if (tracker->AccelerometerTool != NULL)
    {
      tracker->LastAccelerometerToTrackerTransform->Identity();
      tracker->LastAccelerometerToTrackerTransform->SetElement(0, 3, acceleration[0]);
      tracker->LastAccelerometerToTrackerTransform->SetElement(1, 3, acceleration[1]);
      tracker->LastAccelerometerToTrackerTransform->SetElement(2, 3, acceleration[2]);
      tracker->ToolTimeStampedUpdateWithoutFiltering(tracker->AccelerometerTool->GetId(), tracker->LastAccelerometerToTrackerTransform, TOOL_OK, timeSystemSec, timeSystemSec);
    }
    if (tracker->GyroscopeTool != NULL)
    {
      tracker->LastGyroscopeToTrackerTransform->Identity();
      tracker->LastGyroscopeToTrackerTransform->SetElement(0, 3, angularRate[0]);
      tracker->LastGyroscopeToTrackerTransform->SetElement(1, 3, angularRate[1]);
      tracker->LastGyroscopeToTrackerTransform->SetElement(2, 3, angularRate[2]);
      tracker->ToolTimeStampedUpdateWithoutFiltering(tracker->GyroscopeTool->GetId(), tracker->LastGyroscopeToTrackerTransform, TOOL_OK, timeSystemSec, timeSystemSec);
    }
    if (tracker->MagnetometerTool != NULL)
    {
      if (magneticField[0] > 1e100)
      {
        // magnetometer data is not available, use the last transform with an invalid status to not have any missing transform
        tracker->ToolTimeStampedUpdateWithoutFiltering(tracker->MagnetometerTool->GetId(), tracker->LastMagnetometerToTrackerTransform, TOOL_INVALID, timeSystemSec, timeSystemSec);
      }
      else
      {
        // magnetometer data is valid
        tracker->LastMagnetometerToTrackerTransform->Identity();
        tracker->LastMagnetometerToTrackerTransform->SetElement(0, 3, magneticField[0]);
        tracker->LastMagnetometerToTrackerTransform->SetElement(1, 3, magneticField[1]);
        tracker->LastMagnetometerToTrackerTransform->SetElement(2, 3, magneticField[2]);
        tracker->ToolTimeStampedUpdateWithoutFiltering(tracker->MagnetometerTool->GetId(), tracker->LastMagnetometerToTrackerTransform, TOOL_OK, timeSystemSec, timeSystemSec);
      }
    }

    if (tracker->TiltSensorTool != NULL)
    {
      // Compose matrix that transforms the x axis to the input vector by rotations around two orthogonal axes
      vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();

      double downVector_Sensor[4] = {acceleration[0], acceleration[1], acceleration[2], 0}; // provided by the sensor
      vtkMath::Normalize(downVector_Sensor);

      PlusMath::ConstrainRotationToTwoAxes(downVector_Sensor, tracker->TiltSensorWestAxisIndex, tracker->LastTiltSensorToTrackerTransform);

      tracker->ToolTimeStampedUpdateWithoutFiltering(tracker->TiltSensorTool->GetId(), tracker->LastTiltSensorToTrackerTransform, TOOL_OK, timeSystemSec, timeSystemSec);
    }

    if (tracker->OrientationSensorTool != NULL)
    {
      if (magneticField[0] > 1e100)
      {
        // magnetometer data is not available, use the last transform with an invalid status to not have any missing transform
        tracker->ToolTimeStampedUpdateWithoutFiltering(tracker->OrientationSensorTool->GetId(), tracker->LastOrientationSensorToTrackerTransform, TOOL_INVALID, timeSystemSec, timeSystemSec);
      }
      else
      {
        // magnetometer data is valid

        //LOG_TRACE("samplingTime(msec)="<<1000.0*timeSinceLastAhrsUpdateSec<<", packetCount="<<count);
        //LOG_TRACE("gyroX="<<std::fixed<<std::setprecision(2)<<std::setw(6)<<angularRate[0]<<", gyroY="<<angularRate[1]<<", gyroZ="<<angularRate[2]);
        //LOG_TRACE("magX="<<std::fixed<<std::setprecision(2)<<std::setw(6)<<magneticField[0]<<", magY="<<magneticField[1]<<", magZ="<<magneticField[2]);

        if (tracker->AhrsUseMagnetometer)
        {
          tracker->AhrsAlgo->UpdateWithTimestamp(
            vtkMath::RadiansFromDegrees(angularRate[0]), vtkMath::RadiansFromDegrees(angularRate[1]), vtkMath::RadiansFromDegrees(angularRate[2]),
            acceleration[0], acceleration[1], acceleration[2],
            magneticField[0], magneticField[1], magneticField[2], timeSystemSec);
        }
        else
        {
          tracker->AhrsAlgo->UpdateIMUWithTimestamp(
            vtkMath::RadiansFromDegrees(angularRate[0]), vtkMath::RadiansFromDegrees(angularRate[1]), vtkMath::RadiansFromDegrees(angularRate[2]),
            acceleration[0], acceleration[1], acceleration[2], timeSystemSec);
        }


        double rotQuat[4] = { 0 };
        tracker->AhrsAlgo->GetOrientation(rotQuat[0], rotQuat[1], rotQuat[2], rotQuat[3]);

        double rotMatrix[3][3] = { 0 };
        vtkMath::QuaternionToMatrix3x3(rotQuat, rotMatrix);

        for (int c = 0; c < 3; c++)
        {
          for (int r = 0; r < 3; r++)
          {
            tracker->LastOrientationSensorToTrackerTransform->SetElement(r, c, rotMatrix[r][c]);
          }
        }

        tracker->ToolTimeStampedUpdateWithoutFiltering(tracker->OrientationSensorTool->GetId(), tracker->LastOrientationSensorToTrackerTransform, TOOL_OK, timeSystemSec, timeSystemSec);
      }
    }
    if (tracker->FilteredTiltSensorTool != NULL)
    {
      tracker->FilteredTiltSensorAhrsAlgo->UpdateIMUWithTimestamp(
        vtkMath::RadiansFromDegrees(angularRate[0]), vtkMath::RadiansFromDegrees(angularRate[1]), vtkMath::RadiansFromDegrees(angularRate[2]),
        acceleration[0], acceleration[1], acceleration[2], timeSystemSec);

      double rotQuat[4] = { 0 };
      tracker->AhrsAlgo->GetOrientation(rotQuat[0], rotQuat[1], rotQuat[2], rotQuat[3]);

      double rotMatrix[3][3] = { 0 };
      vtkMath::QuaternionToMatrix3x3(rotQuat, rotMatrix);

      double filteredDownVector_Sensor[4] = { rotMatrix[2][0], rotMatrix[2][1], rotMatrix[2][2], 0 };
      vtkMath::Normalize(filteredDownVector_Sensor);

      PlusMath::ConstrainRotationToTwoAxes(filteredDownVector_Sensor, tracker->FilteredTiltSensorWestAxisIndex, tracker->LastFilteredTiltSensorToTrackerTransform);

      tracker->ToolTimeStampedUpdateWithoutFiltering(tracker->FilteredTiltSensorTool->GetId(), tracker->LastFilteredTiltSensorToTrackerTransform, TOOL_OK, timeSystemSec, timeSystemSec);

      // write back the results to the FilteredTiltSensor_AHRS algorithm
      for (int c = 0; c < 3; c++)
      {
        for (int r = 0; r < 3; r++)
        {
          rotMatrix[r][c] = tracker->LastFilteredTiltSensorToTrackerTransform->GetElement(r, c);
        }
      }
      double filteredTiltSensorRotQuat[4] = { 0 };
      vtkMath::Matrix3x3ToQuaternion(rotMatrix, filteredTiltSensorRotQuat);
      tracker->FilteredTiltSensorAhrsAlgo->SetOrientation(filteredTiltSensorRotQuat[0], filteredTiltSensorRotQuat[1], filteredTiltSensorRotQuat[2], filteredTiltSensorRotQuat[3]);
    }

  } // SpatialDataHandler

}; // PhidgetSpatialCallbackClass

//-------------------------------------------------------------------------
vtkPlusPhidgetSpatialTracker::vtkPlusPhidgetSpatialTracker()
{
  this->SpatialDeviceHandle = NULL;
  this->SerialNumber = SERIAL_NUMBER_UNDEFINED;
  this->TrackerTimeToSystemTimeSec = 0;
  this->TrackerTimeToSystemTimeComputed = false;
  this->ZeroGyroscopeOnConnect = false;

  this->AccelerometerTool = NULL;
  this->GyroscopeTool = NULL;
  this->MagnetometerTool = NULL;
  this->TiltSensorTool = NULL;
  this->FilteredTiltSensorTool = NULL;
  this->OrientationSensorTool = NULL;

  this->LastAccelerometerToTrackerTransform = vtkMatrix4x4::New();
  this->LastGyroscopeToTrackerTransform = vtkMatrix4x4::New();
  this->LastMagnetometerToTrackerTransform = vtkMatrix4x4::New();
  this->LastTiltSensorToTrackerTransform = vtkMatrix4x4::New();
  this->LastFilteredTiltSensorToTrackerTransform = vtkMatrix4x4::New();
  this->LastOrientationSensorToTrackerTransform = vtkMatrix4x4::New();
  this->FilteredTiltSensorWestAxisIndex = 1;
  this->TiltSensorWestAxisIndex = 1; // the sensor plane is horizontal (axis 2 points down, axis 1 points West)

  // Set up the AHRS algorithm used by the orientation sensor tool
  this->AhrsAlgo = new MadgwickAhrsAlgo;
  this->AhrsUseMagnetometer = true;
  this->AhrsAlgorithmGain[0] = 1.5; // proportional
  this->AhrsAlgorithmGain[1] = 0.0; // integral
  this->AhrsLastUpdateTime = -1;

  // set up the AHRS algorithm used by the FilteredTiltSensor sensor tool
  this->FilteredTiltSensorAhrsAlgo = new MadgwickAhrsAlgo;
  this->FilteredTiltSensorAhrsAlgorithmGain[0] = 1.5; // proportional
  this->FilteredTiltSensorAhrsAlgorithmGain[1] = 0.0; // integral
  this->FilteredTiltSensorAhrsLastUpdateTime = -1;

  this->RequirePortNameInDeviceSetConfiguration = true;

  this->AcquisitionRate = 125; // set to the maximum speed by default

  for (int i = 0; i < PHIDGET_NUMBER_OF_COMPASS_CORRECTION_PARAMETERS; i++)
  {
    this->CompassCorrectionParameters[i] = 0;
  }

  // No need for StartThreadForInternalUpdates, as we are notified about each new frame through a callback function
}

//-------------------------------------------------------------------------
vtkPlusPhidgetSpatialTracker::~vtkPlusPhidgetSpatialTracker()
{
  if (this->Recording)
  {
    this->StopRecording();
  }
  this->LastAccelerometerToTrackerTransform->Delete();
  this->LastAccelerometerToTrackerTransform = NULL;
  this->LastGyroscopeToTrackerTransform->Delete();
  this->LastGyroscopeToTrackerTransform = NULL;
  this->LastMagnetometerToTrackerTransform->Delete();
  this->LastMagnetometerToTrackerTransform = NULL;
  this->LastTiltSensorToTrackerTransform->Delete();
  this->LastTiltSensorToTrackerTransform = NULL;
  this->LastFilteredTiltSensorToTrackerTransform->Delete();
  this->LastFilteredTiltSensorToTrackerTransform = NULL;
  this->LastOrientationSensorToTrackerTransform->Delete();
  this->LastOrientationSensorToTrackerTransform = NULL;
}

//-------------------------------------------------------------------------
void vtkPlusPhidgetSpatialTracker::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);

  if (this->SpatialDeviceHandle != NULL)
  {
    const char* deviceType = NULL;
    Phidget_getDeviceClassName((PhidgetHandle)this->SpatialDeviceHandle, &deviceType);
    os << "Device type: " << deviceType << std::endl;
    int serialNo = 0;
    Phidget_getDeviceSerialNumber((PhidgetHandle)this->SpatialDeviceHandle, &serialNo);
    os << "Serial Number: " << serialNo << std::endl;
    int version = 0;
    Phidget_getDeviceVersion((PhidgetHandle)this->SpatialDeviceHandle, &version);
    os << "Version: " << version << std::endl;
    int numAccelAxes = 0;
    PhidgetAccelerometer_getAxisCount((PhidgetAccelerometerHandle)this->SpatialDeviceHandle, &numAccelAxes);
    os << "Number of Accel Axes: " << numAccelAxes << std::endl;
    int numGyroAxes = 0;
    PhidgetGyroscope_getAxisCount((PhidgetGyroscopeHandle)this->SpatialDeviceHandle, &numGyroAxes);
    os << "Number of Gyro Axes: " << numGyroAxes << std::endl;
    int numCompassAxes = 0;
    PhidgetMagnetometer_getAxisCount((PhidgetMagnetometerHandle)this->SpatialDeviceHandle, &numCompassAxes);
    os << "Number of Compass Axes: " << numCompassAxes << std::endl;
    uint32_t dataRateMax = 0;
    PhidgetSpatial_getMaxDataInterval((PhidgetSpatialHandle)this->SpatialDeviceHandle, &dataRateMax);
    os << "Maximum data rate: " << dataRateMax << std::endl;
    uint32_t dataRateMin = 0;
    PhidgetSpatial_getMinDataInterval((PhidgetSpatialHandle)this->SpatialDeviceHandle, &dataRateMin);
    os << "Minimum data rate: " << dataRateMin << std::endl;
    uint32_t dataRate = 0;
    PhidgetSpatial_getDataInterval((PhidgetSpatialHandle)this->SpatialDeviceHandle, &dataRate);
    os << "Current data rate: " << dataRate << std::endl;
  }
  else
  {
    os << "Spatial device is not available" << std::endl;
  }

}

//-------------------------------------------------------------------------
PlusStatus vtkPlusPhidgetSpatialTracker::InternalConnect()
{
  LOG_TRACE("vtkPlusPhidgetSpatialTracker::Connect");

  this->AccelerometerTool = NULL;
  GetToolByPortName("Accelerometer", this->AccelerometerTool);

  this->GyroscopeTool = NULL;
  GetToolByPortName("Gyroscope", this->GyroscopeTool);

  this->MagnetometerTool = NULL;
  GetToolByPortName("Magnetometer", this->MagnetometerTool);

  this->TiltSensorTool = NULL;
  GetToolByPortName("TiltSensor", this->TiltSensorTool);

  this->FilteredTiltSensorTool = NULL;
  GetToolByPortName("FilteredTiltSensor", this->FilteredTiltSensorTool);

  this->OrientationSensorTool = NULL;
  GetToolByPortName("OrientationSensor", this->OrientationSensorTool);






  //Create the communicator object to the PhidgetSpatial device
  //PhidgetSpatial_create((PhidgetSpatialHandle*)(&this->SpatialDeviceHandle));


  PhidgetReturnCode res = PhidgetSpatial_create((PhidgetSpatialHandle*)&this->SpatialDeviceHandle);
  if (res != EPHIDGET_OK)
  {
    LOG_ERROR("Device " << this->GetDeviceId() << " failed to create Spatial channel");
    return PLUS_FAIL;
  }

  //Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
  Phidget_setOnAttachHandler((PhidgetHandle)this->SpatialDeviceHandle, PhidgetSpatialCallbackClass::AttachHandler, this);
  Phidget_setOnDetachHandler((PhidgetHandle)this->SpatialDeviceHandle, PhidgetSpatialCallbackClass::DetachHandler, this);
  Phidget_setOnErrorHandler((PhidgetHandle)this->SpatialDeviceHandle, PhidgetSpatialCallbackClass::ErrorHandler, this);

  if (this->SerialNumber != SERIAL_NUMBER_UNDEFINED)
  {
    Phidget_setDeviceSerialNumber((PhidgetHandle)this->SpatialDeviceHandle, this->SerialNumber);
  }

  //Registers a callback that will run according to the set data rate that will return the spatial data changes
  PhidgetSpatial_setOnSpatialDataHandler((PhidgetSpatialHandle)this->SpatialDeviceHandle, PhidgetSpatialCallbackClass::SpatialDataHandler, this);

  //This will initiate the SystemTime (time reference in Plus) to TrackerTime (time reference of the internal clock of the device) offset computation
  this->TrackerTimeToSystemTimeSec = 0;
  this->TrackerTimeToSystemTimeComputed = false;

  //get the program to wait for a spatial device to be attached
  LOG_DEBUG("Waiting for phidget spatial device to be attached...");
  if ((res = Phidget_openWaitForAttachment((PhidgetHandle)this->SpatialDeviceHandle, 10000)) != EPHIDGET_OK)
  {
    const char* err = NULL;
    Phidget_getErrorDescription(res, &err);
    if (this->SerialNumber == SERIAL_NUMBER_UNDEFINED)
    {
      LOG_ERROR("Device " << this->GetDeviceId() << " cannot connect to PhidgetSpatial hardware device (serial number: any). Problem waiting for attachment (" << err << ")");
    }
    else
    {
      LOG_ERROR("Device " << this->GetDeviceId() << " cannot connect to PhidgetSpatial hardware device (serial number: " << this->SerialNumber << "). Problem waiting for attachment (" << err << ")");
    }
    return PLUS_FAIL;
  }

  if (this->SerialNumber == SERIAL_NUMBER_UNDEFINED)
  {
    // The user has not specified a serial number, so let him know which device we are actually connected to
    int32_t serialNo = 0;
    Phidget_getDeviceSerialNumber((PhidgetHandle)this->SpatialDeviceHandle, &serialNo);
    LOG_INFO("Phidget spatial sensor attached to device " << this->GetDeviceId() << ". Serial number: " << serialNo);
  }

  //Set the data rate for the spatial events
  uint32_t requestedUserDataRateMsec = int(1000.0 / this->GetAcquisitionRate());
  // Allowed userDataRateMsec values: 8, 16, 32, 64, 128, 256, 512, 1000, set the closest one
  if (requestedUserDataRateMsec < 12) { requestedUserDataRateMsec = 8; }
  else if (requestedUserDataRateMsec < 24) { requestedUserDataRateMsec = 16; }
  else if (requestedUserDataRateMsec < 48) { requestedUserDataRateMsec = 32; }
  else if (requestedUserDataRateMsec < 96) { requestedUserDataRateMsec = 64; }
  else if (requestedUserDataRateMsec < 192) { requestedUserDataRateMsec = 128; }
  else if (requestedUserDataRateMsec < 384) { requestedUserDataRateMsec = 256; }
  else if (requestedUserDataRateMsec < 756) { requestedUserDataRateMsec = 512; }
  else { requestedUserDataRateMsec = 1000; }
  PhidgetSpatial_setDataInterval((PhidgetSpatialHandle)this->SpatialDeviceHandle, requestedUserDataRateMsec);

  uint32_t userDataRateMsec = 0;
  PhidgetSpatial_getDataInterval((PhidgetSpatialHandle)this->SpatialDeviceHandle, &userDataRateMsec);

  LOG_DEBUG("DataRate (msec):" << userDataRateMsec << " (requested: " << requestedUserDataRateMsec << ")");

  // Initialize AHRS algorithm
  this->AhrsAlgo->SetSampleFreqHz(1000.0 / userDataRateMsec); // more accurate value will be set at each update step anyway
  this->AhrsAlgo->SetGain(this->AhrsAlgorithmGain[0], this->AhrsAlgorithmGain[1]);
  this->FilteredTiltSensorAhrsAlgo->SetSampleFreqHz(1000.0 / userDataRateMsec);
  this->FilteredTiltSensorAhrsAlgo->SetGain(this->FilteredTiltSensorAhrsAlgorithmGain[0], this->FilteredTiltSensorAhrsAlgorithmGain[1]);

  // Set compass correction parameters
  if (this->IsCompassCorrectionParametersDefined())
  {
    if (PHIDGET_NUMBER_OF_COMPASS_CORRECTION_PARAMETERS == 13) // we expect exactly 13 parameters here
    {
      double magField = this->CompassCorrectionParameters[0];
      double offset0 = this->CompassCorrectionParameters[1];
      double offset1 = this->CompassCorrectionParameters[2];
      double offset2 = this->CompassCorrectionParameters[3];
      double gain0 = this->CompassCorrectionParameters[4];
      double gain1 = this->CompassCorrectionParameters[5];
      double gain2 = this->CompassCorrectionParameters[6];
      double T0 = this->CompassCorrectionParameters[7];
      double T1 = this->CompassCorrectionParameters[8];
      double T2 = this->CompassCorrectionParameters[9];
      double T3 = this->CompassCorrectionParameters[10];
      double T4 = this->CompassCorrectionParameters[11];
      double T5 = this->CompassCorrectionParameters[12];
      PhidgetMagnetometer_setCorrectionParameters((PhidgetMagnetometerHandle)this->SpatialDeviceHandle,
          magField, offset0, offset1, offset2, gain0, gain1, gain2, T0, T1, T2, T3, T4, T5);
    }
    else
    {
      LOG_ERROR("Failed to set compass correction parameters: 13 parameters were expected");
    }
  }
  else
  {
    LOG_DEBUG("No compass correction parameters are specified");
  }

  if (this->ZeroGyroscopeOnConnect)
  {
    ZeroGyroscope();
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusPhidgetSpatialTracker::InternalDisconnect()
{
  LOG_TRACE("vtkPlusPhidgetSpatialTracker::Disconnect");
  this->StopRecording();
  Phidget_close((PhidgetHandle)this->SpatialDeviceHandle);
  Phidget_delete((PhidgetHandle*)&this->SpatialDeviceHandle);
  this->SpatialDeviceHandle = NULL;
  this->AccelerometerTool = NULL;
  this->GyroscopeTool = NULL;
  this->MagnetometerTool = NULL;
  this->TiltSensorTool = NULL;
  this->FilteredTiltSensorTool = NULL;
  this->OrientationSensorTool = NULL;
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusPhidgetSpatialTracker::Probe()
{
  LOG_TRACE("vtkPlusPhidgetSpatialTracker::Probe");

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusPhidgetSpatialTracker::InternalStartRecording()
{
  LOG_TRACE("vtkPlusPhidgetSpatialTracker::InternalStartRecording");
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusPhidgetSpatialTracker::InternalStopRecording()
{
  LOG_TRACE("vtkPlusPhidgetSpatialTracker::InternalStopRecording");
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusPhidgetSpatialTracker::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(ZeroGyroscopeOnConnect, deviceConfig);

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, SerialNumber, deviceConfig);

  int tiltSensorWestAxisIndex = 0;
  if (deviceConfig->GetScalarAttribute("TiltSensorWestAxisIndex", tiltSensorWestAxisIndex))
  {
    if (tiltSensorWestAxisIndex < 0 || tiltSensorWestAxisIndex > 2)
    {
      LOG_ERROR("TiltSensorWestAxisIndex is invalid. Specified value: " << tiltSensorWestAxisIndex << ". Valid values: 0, 1, 2. Keep using the default value: "
                << this->TiltSensorWestAxisIndex);
    }
    else
    {
      this->TiltSensorWestAxisIndex = tiltSensorWestAxisIndex;
    }
  }

  int FilteredTiltSensorWestAxisIndex = 0;
  if (deviceConfig->GetScalarAttribute("FilteredTiltSensorWestAxisIndex", FilteredTiltSensorWestAxisIndex))
  {
    if (FilteredTiltSensorWestAxisIndex < 0 || FilteredTiltSensorWestAxisIndex > 2)
    {
      LOG_ERROR("FilteredTiltSensorWestAxisIndex is invalid. Specified value: " << FilteredTiltSensorWestAxisIndex << ". Valid values: 0, 1, 2. Keep using the default value: "
                << this->FilteredTiltSensorWestAxisIndex);
    }
    else
    {
      this->FilteredTiltSensorWestAxisIndex = FilteredTiltSensorWestAxisIndex;
    }
  }

  XML_READ_VECTOR_ATTRIBUTE_OPTIONAL(double, 2, AhrsAlgorithmGain, deviceConfig);
  XML_READ_VECTOR_ATTRIBUTE_OPTIONAL(double, 2, FilteredTiltSensorAhrsAlgorithmGain, deviceConfig);

  const char* ahrsAlgoName = deviceConfig->GetAttribute("AhrsAlgorithm");
  if (ahrsAlgoName != NULL)
  {
    if (STRCASECMP("MADGWICK_MARG", ahrsAlgoName) == 0 || STRCASECMP("MADGWICK_IMU", ahrsAlgoName) == 0)
    {
      if (dynamic_cast<MadgwickAhrsAlgo*>(this->AhrsAlgo) == 0)
      {
        // not the requested type
        // delete the old algo and create a new one with the correct type
        delete this->AhrsAlgo;
        this->AhrsAlgo = new MadgwickAhrsAlgo;
      }
      if (STRCASECMP("MADGWICK_MARG", ahrsAlgoName) == 0)
      {
        this->AhrsUseMagnetometer = true;
      }
      else
      {
        this->AhrsUseMagnetometer = false;
      }
    }
    else if (STRCASECMP("MAHONY_MARG", ahrsAlgoName) == 0 || STRCASECMP("MAHONY_IMU", ahrsAlgoName) == 0)
    {
      if (dynamic_cast<MahonyAhrsAlgo*>(this->AhrsAlgo) == 0)
      {
        // not the requested type
        // delete the old algo and create a new one with the correct type
        delete this->AhrsAlgo;
        this->AhrsAlgo = new MahonyAhrsAlgo;
      }
      if (STRCASECMP("MAHONY_MARG", ahrsAlgoName) == 0)
      {
        this->AhrsUseMagnetometer = true;
      }
      else
      {
        this->AhrsUseMagnetometer = false;
      }
    }
    else
    {
      LOG_ERROR("Unable to recognize AHRS algorithm type: " << ahrsAlgoName << ". Supported types: MADGWICK_MARG, MAHONY_MARG, MADGWICK_IMU, MAHONY_IMU");
      return PLUS_FAIL;
    }
  }
  const char* FilteredTiltSensorAhrsAlgoName = deviceConfig->GetAttribute("FilteredTiltSensorAhrsAlgorithm");
  if (FilteredTiltSensorAhrsAlgoName != NULL)
  {
    if (STRCASECMP("MADGWICK_IMU", FilteredTiltSensorAhrsAlgoName) == 0)
    {
      if (dynamic_cast<MadgwickAhrsAlgo*>(this->FilteredTiltSensorAhrsAlgo) == 0)
      {
        // not the requested type
        // delete the old algo and create a new one with the correct type
        delete this->FilteredTiltSensorAhrsAlgo;
        this->FilteredTiltSensorAhrsAlgo = new MadgwickAhrsAlgo;
      }
    }
    else if (STRCASECMP("MAHONY_IMU", FilteredTiltSensorAhrsAlgoName) == 0)
    {
      if (dynamic_cast<MahonyAhrsAlgo*>(this->FilteredTiltSensorAhrsAlgo) == 0)
      {
        // not the requested type
        // delete the old algo and create a new one with the correct type
        delete this->FilteredTiltSensorAhrsAlgo;
        this->FilteredTiltSensorAhrsAlgo = new MahonyAhrsAlgo;
      }
    }
    else
    {
      LOG_ERROR("Unable to recognize AHRS algorithm type for Filtered Tilt: " << FilteredTiltSensorAhrsAlgoName << ". Supported types: MADGWICK_IMU, MAHONY_IMU");
      return PLUS_FAIL;
    }
  }

  XML_READ_VECTOR_ATTRIBUTE_EXACT_OPTIONAL(double, PHIDGET_NUMBER_OF_COMPASS_CORRECTION_PARAMETERS, CompassCorrectionParameters, deviceConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusPhidgetSpatialTracker::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  if (this->ZeroGyroscopeOnConnect)
  {
    deviceConfig->SetAttribute("ZeroGyroscopeOnConnect", "TRUE");
  }

  if (this->SerialNumber != SERIAL_NUMBER_UNDEFINED)
  {
    deviceConfig->SetIntAttribute("SerialNumber", this->SerialNumber);
  }

  if (this->TiltSensorTool)
  {
    deviceConfig->SetIntAttribute("TiltSensorWestAxisIndex", this->TiltSensorWestAxisIndex);
  }

  if (this->FilteredTiltSensorTool)
  {
    deviceConfig->SetIntAttribute("FilteredTiltSensorWestAxisIndex", this->FilteredTiltSensorWestAxisIndex);
    if (this->FilteredTiltSensorAhrsAlgorithmGain[1] == 0.0)
    {
      // if the second gain parameter is zero then just write the first value
      deviceConfig->SetDoubleAttribute("FilteredTiltSensorAhrsAlgorithmGain", this->FilteredTiltSensorAhrsAlgorithmGain[0]);
    }
    else
    {
      deviceConfig->SetVectorAttribute("FilteredTiltSensorAhrsAlgorithmGain", 2, this->FilteredTiltSensorAhrsAlgorithmGain);
    }

    if (dynamic_cast<MadgwickAhrsAlgo*>(this->FilteredTiltSensorAhrsAlgo) != 0)
    {
      deviceConfig->SetAttribute("FilteredTiltSensorAhrsAlgorithm", "MADGWICK_IMU");
    }
    else if (dynamic_cast<MahonyAhrsAlgo*>(this->FilteredTiltSensorAhrsAlgo) != 0)
    {
      deviceConfig->SetAttribute("FilteredTiltSensorAhrsAlgorithm", "MAHONY_IMU");
    }
    else
    {
      LOG_ERROR("Unknown AHRS algorithm type for Filtered Tilt Sensor. Cannot write name to XML.");
    }
  }

  if (this->OrientationSensorTool)
  {
    if (this->AhrsAlgorithmGain[1] == 0.0)
    {
      // if the second gain parameter is zero then just write the first value
      deviceConfig->SetDoubleAttribute("AhrsAlgorithmGain", this->AhrsAlgorithmGain[0]);
    }
    else
    {
      deviceConfig->SetVectorAttribute("AhrsAlgorithmGain", 2, this->AhrsAlgorithmGain);
    }

    if (dynamic_cast<MadgwickAhrsAlgo*>(this->AhrsAlgo) != 0)
    {
      if (this->AhrsUseMagnetometer)
      {
        deviceConfig->SetAttribute("AhrsAlgorithm", "MADGWICK_MARG");
      }
      else
      {
        deviceConfig->SetAttribute("AhrsAlgorithm", "MADGWICK_IMU");
      }
    }
    else if (dynamic_cast<MahonyAhrsAlgo*>(this->AhrsAlgo) != 0)
    {
      if (this->AhrsUseMagnetometer)
      {
        deviceConfig->SetAttribute("AhrsAlgorithm", "MAHONY_MARG");
      }
      else
      {
        deviceConfig->SetAttribute("AhrsAlgorithm", "MAHONY_IMU");
      }
    }
    else
    {
      LOG_ERROR("Unknown AHRS algorithm type. Cannot write name to XML.");
    }
  }

  if (IsCompassCorrectionParametersDefined())
  {
    deviceConfig->SetVectorAttribute("CompassCorrectionParameters", PHIDGET_NUMBER_OF_COMPASS_CORRECTION_PARAMETERS, this->CompassCorrectionParameters);
  }
  else
  {
    deviceConfig->RemoveAttribute("CompassCorrectionParameters");
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusPhidgetSpatialTracker::ZeroGyroscope()
{
  LOG_INFO("Zeroing the gyroscope. Keep the sensor stationary for 2 seconds.");
  PhidgetSpatial_zeroGyro((PhidgetSpatialHandle)this->SpatialDeviceHandle);
}

//----------------------------------------------------------------------------
bool vtkPlusPhidgetSpatialTracker::IsResettable()
{
  return true;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusPhidgetSpatialTracker::Reset()
{
  if (this->IsRecording())
  {
    ZeroGyroscope();
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
/*
void vtkPlusPhidgetSpatialTracker::Get3x3RotMatrixFromIMUQuat(double rotMatrix[3][3], AhrsAlgo* AhrsAlgo)
{
  double rotQuat[4]={0};
  AhrsAlgo->GetOrientation(rotQuat[0],rotQuat[1],rotQuat[2],rotQuat[3]);
  vtkMath::QuaternionToMatrix3x3(rotQuat, rotMatrix);

  return;
}
*/

//----------------------------------------------------------------------------
bool vtkPlusPhidgetSpatialTracker::IsCompassCorrectionParametersDefined()
{
  for (int i = 0; i < PHIDGET_NUMBER_OF_COMPASS_CORRECTION_PARAMETERS; i++)
  {
    if (this->CompassCorrectionParameters[i] != 0)
    {
      return true;
    }
  }
  return false;
}
