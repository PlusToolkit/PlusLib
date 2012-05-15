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

  this->LastAccelerometerToTrackerTransform=vtkMatrix4x4::New();
  this->LastGyroscopeToTrackerTransform=vtkMatrix4x4::New();
  this->LastMagnetometerToTrackerTransform=vtkMatrix4x4::New();
}

//-------------------------------------------------------------------------
vtkPhidgetSpatialTracker::~vtkPhidgetSpatialTracker() 
{
  if ( this->Tracking )
  {
    this->StopTracking();
  }
  this->LastAccelerometerToTrackerTransform->Delete();
  this->LastAccelerometerToTrackerTransform=NULL;
  this->LastGyroscopeToTrackerTransform->Delete();
  this->LastGyroscopeToTrackerTransform=NULL;
  this->LastMagnetometerToTrackerTransform->Delete();
  this->LastMagnetometerToTrackerTransform=NULL;
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
      tracker->ToolTimeStampedUpdate( tracker->AccelerometerTool->GetToolName(), tracker->LastAccelerometerToTrackerTransform, TOOL_OK, timeSystemSec, timeSystemSec);
    }  
    if (tracker->GyroscopeTool!=NULL)
    {
      ConvertVectorToTransformationMatrix(data[i]->angularRate, tracker->LastGyroscopeToTrackerTransform);
      tracker->ToolTimeStampedUpdate( tracker->GyroscopeTool->GetToolName(), tracker->LastGyroscopeToTrackerTransform, TOOL_OK, timeSystemSec, timeSystemSec);
    }  
    if (tracker->MagnetometerTool!=NULL)
    {      
      if (data[i]->magneticField[0]>1e100)
      {
        // magnetometer data is not available, use the last transform with an invalid status to not have any missing transform
        tracker->ToolTimeStampedUpdate( tracker->MagnetometerTool->GetToolName(), tracker->LastMagnetometerToTrackerTransform, TOOL_OUT_OF_VIEW, timeSystemSec, timeSystemSec);
      }
      else
      {
        // magnetometer data is valid
        ConvertVectorToTransformationMatrix(data[i]->magneticField, tracker->LastMagnetometerToTrackerTransform);
        LOG_INFO(data[i]->magneticField[0] << "    " << data[i]->magneticField[1] << "    " <<data[i]->magneticField[2]);
        tracker->ToolTimeStampedUpdate( tracker->MagnetometerTool->GetToolName(), tracker->LastMagnetometerToTrackerTransform, TOOL_OK, timeSystemSec, timeSystemSec);
      }
    }     
  }

	return 0;
}

void vtkPhidgetSpatialTracker::ConvertVectorToTransformationMatrix(double *inputVector, vtkMatrix4x4* outputMatrix)
{
  // Tracker x, y, z axes
  double trackerX[3]={inputVector[0],inputVector[1],inputVector[2]};
  vtkMath::Normalize(trackerX);   

  double trackerY[3]={0,1,0};
  double trackerZ[3]={0,0,1};

  if (trackerX[0]<0.9999) // if the vector is very close to (1,0,0) then just use it as is
  {
    double xUnitVector[3]={1,0,0};
    vtkMath::Cross(xUnitVector, trackerX, trackerZ);
    vtkMath::Normalize(trackerZ);
    vtkMath::Cross(trackerZ, trackerX, trackerY);
    vtkMath::Normalize(trackerY);
  }   
  
  outputMatrix->Identity();
  for ( int row = 0; row < 3; ++ row )
  {
    outputMatrix->SetElement( row, 0, trackerX[row]);
    outputMatrix->SetElement( row, 1, trackerY[row]);
    outputMatrix->SetElement( row, 2, trackerZ[row]);
  }
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
  int userDataRateMsec=1000/this->GetFrequency();
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
  if ( this->Tracking )
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

