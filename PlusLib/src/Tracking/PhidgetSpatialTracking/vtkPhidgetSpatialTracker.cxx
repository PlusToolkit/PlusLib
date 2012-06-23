/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtkPhidgetSpatialTracker.h"

#include <sstream>

#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtksys/SystemTools.hxx"
#include "vtkTransform.h"
#include "vtkXMLDataElement.h"

#include "PlusConfigure.h"
#include "vtkTracker.h"
#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"

#include "MadgwickAHRS.h"
#include "MahonyAHRS.h"
 









//=====================================================================================================
// MahonyAHRS.c
//=====================================================================================================
//
// Madgwick's implementation of Mayhony's AHRS algorithm.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			Author			Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
//
//=====================================================================================================

//---------------------------------------------------------------------------------------------------
// Header files

#include "MahonyAHRS.h"
#include <math.h>

vtkStandardNewMacro(vtkPhidgetSpatialTracker);

//-------------------------------------------------------------------------
vtkPhidgetSpatialTracker::vtkPhidgetSpatialTracker()
{ 
  this->SpatialDeviceHandle = 0;
  this->TrackerTimeToSystemTimeSec = 0;
  this->TrackerTimeToSystemTimeComputed = false;

  this->AccelerometerTool = NULL;
  this->GyroscopeTool = NULL;
  this->MagnetometerTool = NULL;
  this->OrientationSensorTool = NULL;

  this->LastAccelerometerToTrackerTransform=vtkMatrix4x4::New();
  this->LastGyroscopeToTrackerTransform=vtkMatrix4x4::New();
  this->LastMagnetometerToTrackerTransform=vtkMatrix4x4::New();
  this->LastOrientationSensorToTrackerTransform=vtkMatrix4x4::New();

  // Orientation sensor fusion parameters
  sampleFreq=8000.0f; //8000.0f //512.0f			// sample frequency in Hz
  // 2 * proportional gain (Kp)
  twoKp = (2.0f * 0.005f); //(2.0f * 0.5f)	// 2 * proportional gain
  // 2 * integral gain (Ki)
  twoKi = (2.0f * 0.0f);	// 2 * integral gain											
  // quaternion of sensor frame relative to auxiliary frame
  q0 = 1.0f;
  q1 = 0.0f;
  q2 = 0.0f;
  q3 = 0.0f;					
}

//-------------------------------------------------------------------------
vtkPhidgetSpatialTracker::~vtkPhidgetSpatialTracker() 
{
  if ( this->Recording )
  {
    this->StopTracking();
  }
  this->LastAccelerometerToTrackerTransform->Delete();
  this->LastAccelerometerToTrackerTransform=NULL;
  this->LastGyroscopeToTrackerTransform->Delete();
  this->LastGyroscopeToTrackerTransform=NULL;
  this->LastMagnetometerToTrackerTransform->Delete();
  this->LastMagnetometerToTrackerTransform=NULL;
  this->LastOrientationSensorToTrackerTransform->Delete();
  this->LastOrientationSensorToTrackerTransform=NULL;
}

//-------------------------------------------------------------------------
void vtkPhidgetSpatialTracker::PrintSelf( ostream& os, vtkIndent indent )
{
  vtkTracker::PrintSelf( os, indent );

  if (this->SpatialDeviceHandle!=NULL)
  {
    const char* deviceType=NULL;
    CPhidget_getDeviceType((CPhidgetHandle)this->SpatialDeviceHandle, &deviceType);
    os << "Device type: " << deviceType << std::endl;
    int serialNo=0;
    CPhidget_getSerialNumber((CPhidgetHandle)this->SpatialDeviceHandle, &serialNo);
    os << "Serial Number: " << serialNo << std::endl;
    int version=0;
    CPhidget_getDeviceVersion((CPhidgetHandle)this->SpatialDeviceHandle, &version);
    os << "Version: " << version << std::endl;
    int numAccelAxes=0;
    CPhidgetSpatial_getAccelerationAxisCount(this->SpatialDeviceHandle, &numAccelAxes);
    os << "Number of Accel Axes: " << numAccelAxes << std::endl;
    int numGyroAxes=0;
    CPhidgetSpatial_getGyroAxisCount(this->SpatialDeviceHandle, &numGyroAxes);
    os << "Number of Gyro Axes: " << numGyroAxes << std::endl;
    int numCompassAxes=0;
    CPhidgetSpatial_getCompassAxisCount(this->SpatialDeviceHandle, &numCompassAxes);
    os << "Number of Compass Axes: " << numCompassAxes << std::endl;
    int dataRateMax=0;
    CPhidgetSpatial_getDataRateMax(this->SpatialDeviceHandle, &dataRateMax);
    os << "Maximum data rate: " << dataRateMax << std::endl;
    int dataRateMin=0;    
    CPhidgetSpatial_getDataRateMin(this->SpatialDeviceHandle, &dataRateMin);
    os << "Minimum data rate: " << dataRateMin << std::endl;
    int dataRate=0;
    CPhidgetSpatial_getDataRate(this->SpatialDeviceHandle, &dataRate);    
    os << "Current data rate: " << dataRate << std::endl;
  }
  else
  {
    os << "Spatial device is not available" << std::endl;
  }

}

//callback that will run if the Spatial is attached to the computer
int CCONV AttachHandler(CPhidgetHandle spatial, void *trackerPtr)
{
	int serialNo;
	CPhidget_getSerialNumber(spatial, &serialNo);
  LOG_DEBUG("Phidget spatial sensor attached: " << serialNo);
	return 0;
}

//callback that will run if the Spatial is detached from the computer
int CCONV DetachHandler(CPhidgetHandle spatial, void *trackerPtr)
{
	int serialNo;
	CPhidget_getSerialNumber(spatial, &serialNo);
	LOG_DEBUG("Phidget spatial sensor detached: " << serialNo);
	return 0;
}

//callback that will run if the Spatial generates an error
int CCONV ErrorHandler(CPhidgetHandle spatial, void *trackerPtr, int ErrorCode, const char *unknown)
{
  LOG_ERROR("Phidget spatial sensor error: "<<ErrorCode<<" ("<<unknown<<")");
	return 0;
}

//callback that will run at datarate
//data - array of spatial event data structures that holds the spatial data packets that were sent in this event
//count - the number of spatial data event packets included in this event
int CCONV vtkPhidgetSpatialTracker::SpatialDataHandler(CPhidgetSpatialHandle spatial, void *trackerPtr, CPhidgetSpatial_SpatialEventDataHandle *data, int count)
{
  vtkPhidgetSpatialTracker* tracker=(vtkPhidgetSpatialTracker*)trackerPtr;
  if ( ! tracker->IsTracking() )
  {
    // Received phidget tracking data when not tracking
    return 0;
  }

  if ( count<1 )
  {
    LOG_WARNING("No phidget data received in data handler" );
    return 0;
  }

  if (!tracker->TrackerTimeToSystemTimeComputed)
  {
    const double timeSystemSec = vtkAccurateTimer::GetSystemTime();
    const double timeTrackerSec = data[count-1]->timestamp.seconds+data[count-1]->timestamp.microseconds*1e-6;
    tracker->TrackerTimeToSystemTimeSec = timeSystemSec-timeTrackerSec;
    tracker->TrackerTimeToSystemTimeComputed = true;
  }
  	
	LOG_TRACE("Number of phidget data packets received in the current event: " << count);

  for(int i = 0; i < count; i++)
  { 
    const double timeTrackerSec = data[i]->timestamp.seconds+data[i]->timestamp.microseconds*1e-6;
    const double timeSystemSec = timeTrackerSec + tracker->TrackerTimeToSystemTimeSec;        

    if (tracker->AccelerometerTool!=NULL)
    {
      ConvertVectorToTransformationMatrix(data[i]->acceleration, tracker->LastAccelerometerToTrackerTransform);
      tracker->ToolTimeStampedUpdateWithoutFiltering( tracker->AccelerometerTool->GetToolName(), tracker->LastAccelerometerToTrackerTransform, TOOL_OK, timeSystemSec, timeSystemSec);
    }  
    if (tracker->GyroscopeTool!=NULL)
    {
      vtkSmartPointer<vtkTransform> transform=vtkSmartPointer<vtkTransform>::New();
      transform->RotateX(data[i]->angularRate[0]/10.0);
      transform->RotateY(data[i]->angularRate[1]/10.0);  
      transform->RotateY(data[i]->angularRate[2]/10.0);  
      transform->GetMatrix(tracker->LastGyroscopeToTrackerTransform);
      tracker->ToolTimeStampedUpdateWithoutFiltering( tracker->GyroscopeTool->GetToolName(), tracker->LastGyroscopeToTrackerTransform, TOOL_OK, timeSystemSec, timeSystemSec);
    }  
    if (tracker->MagnetometerTool!=NULL)
    {      
      if (data[i]->magneticField[0]>1e100)
      {
        // magnetometer data is not available, use the last transform with an invalid status to not have any missing transform
        tracker->ToolTimeStampedUpdateWithoutFiltering( tracker->MagnetometerTool->GetToolName(), tracker->LastMagnetometerToTrackerTransform, TOOL_INVALID, timeSystemSec, timeSystemSec);
      }
      else
      {
        // magnetometer data is valid
        ConvertVectorToTransformationMatrix(data[i]->magneticField, tracker->LastMagnetometerToTrackerTransform);        
        tracker->ToolTimeStampedUpdateWithoutFiltering( tracker->MagnetometerTool->GetToolName(), tracker->LastMagnetometerToTrackerTransform, TOOL_OK, timeSystemSec, timeSystemSec);
      }
    }     

    if (tracker->OrientationSensorTool!=NULL)
    {
      //if (data[i]->magneticField[0]<1e100)
      {
        // data valid
        //MadgwickAHRSupdate( // gyro, accel, magn
        //MahonyAHRSupdate(
        MahonyAHRSupdateIMU(
          data[i]->angularRate[0], data[i]->angularRate[1], data[i]->angularRate[2]
          ,data[i]->acceleration[0], data[i]->acceleration[1], data[i]->acceleration[2]
        //  ,data[i]->magneticField[0], data[i]->magneticField[1], data[i]->magneticField[2]
        );

        //extern volatile float beta;				// algorithm gain
        //extern volatile float q0, q1, q2, q3;	// quaternion of sensor frame relative to auxiliary frame
        double rotQuat[4]= {q0,q1,q2,q3};
        double rotMatrix[3][3] = {0};

        vtkMath::QuaternionToMatrix3x3(rotQuat, rotMatrix); 

        for (int c=0;c<3; c++)
        {
          for (int r=0;r<3; r++)
          {
            tracker->LastOrientationSensorToTrackerTransform->SetElement(r,c,rotMatrix[r][c]);
          }
        }
      }
      
      /*
      vtkSmartPointer<vtkTransform> transform=vtkSmartPointer<vtkTransform>::New();
      transform->RotateWXYZ(vtkMath::DegreesFromRadians(q0),q1,q2,q3);
      transform->GetMatrix(tracker->LastGyroscopeToTrackerTransform);
      */

      tracker->ToolTimeStampedUpdateWithoutFiltering( tracker->OrientationSensorTool->GetToolName(), tracker->LastOrientationSensorToTrackerTransform, TOOL_OK, timeSystemSec, timeSystemSec);
    }
    else
    {
      //tracker->ToolTimeStampedUpdateWithoutFiltering( tracker->OrientationSensorTool->GetToolName(), tracker->LastOrientationSensorToTrackerTransform, TOOL_INVALID, timeSystemSec, timeSystemSec);
    }

  }

	return 0;
}

void vtkPhidgetSpatialTracker::ConvertVectorToTransformationMatrix(double *inputVector, vtkMatrix4x4* outputMatrix)
{
  // Compose matrix that transforms the x axis to the input vector by rotations around two orthogonal axes
  double primaryRotationAngleDeg = atan2( inputVector[2],sqrt(inputVector[1]*inputVector[1]+inputVector[0]*inputVector[0]) )*180.0/vtkMath::Pi();
  double secondaryRotationAngleDeg = atan2( inputVector[1],sqrt( inputVector[2]*inputVector[2] + inputVector[0]*inputVector[0]) )*180.0/vtkMath::Pi();
  //LOG_TRACE("Pri="<<primaryRotationAngleDeg<<"  Sec="<<secondaryRotationAngleDeg<<"    x="<<inputVector[0]<<"    y="<<inputVector[1]<<"    z="<<inputVector[2]);

  vtkSmartPointer<vtkTransform> transform=vtkSmartPointer<vtkTransform>::New();
  transform->RotateX(primaryRotationAngleDeg);
  transform->RotateY(secondaryRotationAngleDeg);  
  transform->GetMatrix(outputMatrix);
}

//-------------------------------------------------------------------------
PlusStatus vtkPhidgetSpatialTracker::Connect()
{
  LOG_TRACE( "vtkPhidgetSpatialTracker::Connect" ); 

  this->AccelerometerTool = NULL;
  GetToolByPortName("Accelerometer", this->AccelerometerTool);

  this->GyroscopeTool = NULL;
  GetToolByPortName("Gyroscope", this->GyroscopeTool);

  this->MagnetometerTool = NULL;
  GetToolByPortName("Magnetometer", this->MagnetometerTool);

  this->OrientationSensorTool = NULL;
  GetToolByPortName("OrientationSensor", this->OrientationSensorTool);

	//Create the spatial object  
	CPhidgetSpatial_create(&SpatialDeviceHandle);

	//Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
	CPhidget_set_OnAttach_Handler((CPhidgetHandle)this->SpatialDeviceHandle, AttachHandler, this);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle)this->SpatialDeviceHandle, DetachHandler, this);
	CPhidget_set_OnError_Handler((CPhidgetHandle)this->SpatialDeviceHandle, ErrorHandler, this);

	//Registers a callback that will run according to the set data rate that will return the spatial data changes
	//Requires the handle for the Spatial, the callback handler function that will be called, 
	//and an arbitrary pointer that will be supplied to the callback function (may be NULL)

	CPhidgetSpatial_set_OnSpatialData_Handler(this->SpatialDeviceHandle, vtkPhidgetSpatialTracker::SpatialDataHandler, this);

  // TODO: verify tool definition

  this->TrackerTimeToSystemTimeSec = 0;
  this->TrackerTimeToSystemTimeComputed = false;

  //open the spatial object for device connections
	CPhidget_open((CPhidgetHandle)this->SpatialDeviceHandle, -1);

	//get the program to wait for a spatial device to be attached
	LOG_DEBUG("Waiting for phidget spatial device to be attached...");
  int result=0;
	if((result = CPhidget_waitForAttachment((CPhidgetHandle)this->SpatialDeviceHandle, 10000)))
	{
    const char *err=NULL;
		CPhidget_getErrorDescription(result, &err);
    LOG_ERROR( "Couldn't initialize vtkPhidgetSpatialTracker: Problem waiting for attachment (" << err << ")");
    return PLUS_FAIL;
	}

	//Set the data rate for the spatial events
  int userDataRateMsec=1000/this->GetAcquisitionRate();
	CPhidgetSpatial_setDataRate(this->SpatialDeviceHandle, userDataRateMsec);
	LOG_DEBUG("DataRate (msec):" << userDataRateMsec);

  return PLUS_SUCCESS; 
}

//-------------------------------------------------------------------------
PlusStatus vtkPhidgetSpatialTracker::Disconnect()
{
  LOG_TRACE( "vtkPhidgetSpatialTracker::Disconnect" ); 
  this->StopTracking();
  CPhidget_close((CPhidgetHandle)this->SpatialDeviceHandle);
  CPhidget_delete((CPhidgetHandle)this->SpatialDeviceHandle);
  this->SpatialDeviceHandle = NULL;
  this->AccelerometerTool = NULL;
  this->GyroscopeTool = NULL;
  this->MagnetometerTool = NULL;
  this->OrientationSensorTool = NULL;
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPhidgetSpatialTracker::Probe()
{
  LOG_TRACE( "vtkPhidgetSpatialTracker::Probe" ); 

  return PLUS_SUCCESS; 
} 

//-------------------------------------------------------------------------
PlusStatus vtkPhidgetSpatialTracker::InternalStartTracking()
{
  LOG_TRACE( "vtkPhidgetSpatialTracker::InternalStartTracking" ); 
  if ( this->Recording )
  {
    return PLUS_SUCCESS;
  }  

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPhidgetSpatialTracker::InternalStopTracking()
{
  LOG_TRACE( "vtkPhidgetSpatialTracker::InternalStopTracking" );   

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPhidgetSpatialTracker::InternalUpdate()
{
  LOG_TRACE( "vtkPhidgetSpatialTracker::InternalUpdate" ); 
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPhidgetSpatialTracker::InitPhidgetSpatialTracker()
{
  LOG_TRACE( "vtkPhidgetSpatialTracker::InitPhidgetSpatialTracker" ); 
  return this->Connect(); 
}

