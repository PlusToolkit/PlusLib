/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "MadgwickAhrsAlgo.h"
#include "MahonyAhrsAlgo.h"
#include "PlusConfigure.h"
#include "PlusConfigure.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkPhidgetSpatialTracker.h"
#include "vtkPlusBuffer.h"
#include "vtkPlusDataSource.h"
#include "vtkTransform.h"
#include "vtkXMLDataElement.h"
#include "vtksys/SystemTools.hxx"
#include <math.h>
#include <sstream>

vtkStandardNewMacro(vtkPhidgetSpatialTracker);

//-------------------------------------------------------------------------
vtkPhidgetSpatialTracker::vtkPhidgetSpatialTracker()
{ 
  this->SpatialDeviceHandle = 0;
  this->TrackerTimeToSystemTimeSec = 0;
  this->TrackerTimeToSystemTimeComputed = false;
  this->ZeroGyroscopeOnConnect = false;

  this->AccelerometerTool = NULL;
  this->GyroscopeTool = NULL;
  this->MagnetometerTool = NULL;
  this->TiltSensorTool = NULL;
  this->FilteredTiltSensorTool = NULL;
  this->OrientationSensorTool = NULL;

  this->LastAccelerometerToTrackerTransform=vtkMatrix4x4::New();
  this->LastGyroscopeToTrackerTransform=vtkMatrix4x4::New();
  this->LastMagnetometerToTrackerTransform=vtkMatrix4x4::New();
  this->LastTiltSensorToTrackerTransform=vtkMatrix4x4::New();
  this->LastFilteredTiltSensorToTrackerTransform=vtkMatrix4x4::New();
  this->LastOrientationSensorToTrackerTransform=vtkMatrix4x4::New();
  this->FilteredTiltSensorWestAxisIndex=1;
  this->TiltSensorWestAxisIndex=1; // the sensor plane is horizontal (axis 2 points down, axis 1 points West)

  // Set up the AHRS algorithm used by the orientation sensor tool
  this->AhrsAlgo=new MadgwickAhrsAlgo; 
  this->AhrsUseMagnetometer=true;
  this->AhrsAlgorithmGain[0]=1.5; // proportional
  this->AhrsAlgorithmGain[1]=0.0; // integral
  this->AhrsLastUpdateTime=-1;
  
  // set up the AHRS algorithm used by the filteredTilt sensor tool
  this->FilteredTiltAhrsAlgo=new MadgwickAhrsAlgo; 
  this->FilteredTiltAhrsAlgorithmGain[0]=1.5; // proportional
  this->FilteredTiltAhrsAlgorithmGain[1]=0.0; // integral
  this->FilteredTiltAhrsLastUpdateTime=-1;	
	
	
  this->RequireImageOrientationInConfiguration = false;
  this->RequireFrameBufferSizeInDeviceSetConfiguration = false;
  this->RequireAcquisitionRateInDeviceSetConfiguration = false;
  this->RequireToolAveragedItemsForFilteringInDeviceSetConfiguration = true;
  this->RequireAveragedItemsForFilteringInDeviceSetConfiguration = false;
  this->RequireLocalTimeOffsetSecInDeviceSetConfiguration = false;
  this->RequireUsImageOrientationInDeviceSetConfiguration = false;
  this->RequireRfElementInDeviceSetConfiguration = false;

  // No need for StartThreadForInternalUpdates, as we are notified about each new frame through a callback function
}

//-------------------------------------------------------------------------
vtkPhidgetSpatialTracker::~vtkPhidgetSpatialTracker() 
{
  if ( this->Recording )
  {
    this->StopRecording();
  }
  this->LastAccelerometerToTrackerTransform->Delete();
  this->LastAccelerometerToTrackerTransform=NULL;
  this->LastGyroscopeToTrackerTransform->Delete();
  this->LastGyroscopeToTrackerTransform=NULL;
  this->LastMagnetometerToTrackerTransform->Delete();
  this->LastMagnetometerToTrackerTransform=NULL;
  this->LastTiltSensorToTrackerTransform->Delete();
  this->LastTiltSensorToTrackerTransform=NULL;
  this->LastFilteredTiltSensorToTrackerTransform->Delete();
  this->LastFilteredTiltSensorToTrackerTransform=NULL;
  this->LastOrientationSensorToTrackerTransform->Delete();
  this->LastOrientationSensorToTrackerTransform=NULL;
}

//-------------------------------------------------------------------------
void vtkPhidgetSpatialTracker::PrintSelf( ostream& os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );

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
  if ( ! tracker->IsRecording() )
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
      tracker->LastAccelerometerToTrackerTransform->Identity();
      tracker->LastAccelerometerToTrackerTransform->SetElement(0,3,data[i]->acceleration[0]);
      tracker->LastAccelerometerToTrackerTransform->SetElement(1,3,data[i]->acceleration[1]);
      tracker->LastAccelerometerToTrackerTransform->SetElement(2,3,data[i]->acceleration[2]);
      tracker->ToolTimeStampedUpdateWithoutFiltering( tracker->AccelerometerTool->GetSourceId(), tracker->LastAccelerometerToTrackerTransform, TOOL_OK, timeSystemSec, timeSystemSec);
    }  
    if (tracker->GyroscopeTool!=NULL)
    {
      tracker->LastGyroscopeToTrackerTransform->Identity();
      tracker->LastGyroscopeToTrackerTransform->SetElement(0,3,data[i]->angularRate[0]);
      tracker->LastGyroscopeToTrackerTransform->SetElement(1,3,data[i]->angularRate[1]);
      tracker->LastGyroscopeToTrackerTransform->SetElement(2,3,data[i]->angularRate[2]);
      tracker->ToolTimeStampedUpdateWithoutFiltering( tracker->GyroscopeTool->GetSourceId(), tracker->LastGyroscopeToTrackerTransform, TOOL_OK, timeSystemSec, timeSystemSec);
    }  
    if (tracker->MagnetometerTool!=NULL)
    {      
      if (data[i]->magneticField[0]>1e100)
      {
        // magnetometer data is not available, use the last transform with an invalid status to not have any missing transform
        tracker->ToolTimeStampedUpdateWithoutFiltering( tracker->MagnetometerTool->GetSourceId(), tracker->LastMagnetometerToTrackerTransform, TOOL_INVALID, timeSystemSec, timeSystemSec);
      }
      else
      {
        // magnetometer data is valid
        tracker->LastMagnetometerToTrackerTransform->Identity();
        tracker->LastMagnetometerToTrackerTransform->SetElement(0,3,data[i]->magneticField[0]);
        tracker->LastMagnetometerToTrackerTransform->SetElement(1,3,data[i]->magneticField[1]);
        tracker->LastMagnetometerToTrackerTransform->SetElement(2,3,data[i]->magneticField[2]);
        tracker->ToolTimeStampedUpdateWithoutFiltering( tracker->MagnetometerTool->GetSourceId(), tracker->LastMagnetometerToTrackerTransform, TOOL_OK, timeSystemSec, timeSystemSec);
      }
    }     

    if (tracker->TiltSensorTool!=NULL)
    {
      // Compose matrix that transforms the x axis to the input vector by rotations around two orthogonal axes
      vtkSmartPointer<vtkTransform> transform=vtkSmartPointer<vtkTransform>::New();      

      double downVector_Sensor[4] = {data[i]->acceleration[0],data[i]->acceleration[1],data[i]->acceleration[2],0}; // provided by the sensor
      vtkMath::Normalize(downVector_Sensor);      
      // Sensor axis vector that is assumed to always point to West. This is chosen so that cross(westVector_Sensor, downVector_Sensor) = southVector_Sensor.
      double westVector_Sensor[4]={0,0,0,0};
      double southVector_Sensor[4] = {0,0,0,0};

      if (tracker->TiltSensorWestAxisIndex>=0 && tracker->TiltSensorWestAxisIndex<3)
      {
        // Align a sensor axis with the West direction.
        // Now just set the direction, the length will be corrected at the end.
        westVector_Sensor[tracker->TiltSensorWestAxisIndex]=1; 
      }
      else
      {
        LOG_ERROR("Invalid TiltSensorWestAxisIndex is specified (valid values are 0, 1, 2). Use default: 1.");
        westVector_Sensor[1]=1;
      }

      vtkMath::Cross(westVector_Sensor, downVector_Sensor, southVector_Sensor); // compute South
      vtkMath::Normalize(southVector_Sensor);
      vtkMath::Cross(downVector_Sensor, southVector_Sensor, westVector_Sensor); // compute West
      vtkMath::Normalize(westVector_Sensor);

      // row 0
      tracker->LastTiltSensorToTrackerTransform->SetElement(0,0,southVector_Sensor[0]);
      tracker->LastTiltSensorToTrackerTransform->SetElement(0,1,southVector_Sensor[1]);
      tracker->LastTiltSensorToTrackerTransform->SetElement(0,2,southVector_Sensor[2]);
      // row 1
      tracker->LastTiltSensorToTrackerTransform->SetElement(1,0,westVector_Sensor[0]);
      tracker->LastTiltSensorToTrackerTransform->SetElement(1,1,westVector_Sensor[1]);
      tracker->LastTiltSensorToTrackerTransform->SetElement(1,2,westVector_Sensor[2]);
      // row 2
      tracker->LastTiltSensorToTrackerTransform->SetElement(2,0,downVector_Sensor[0]);
      tracker->LastTiltSensorToTrackerTransform->SetElement(2,1,downVector_Sensor[1]);
      tracker->LastTiltSensorToTrackerTransform->SetElement(2,2,downVector_Sensor[2]);

      tracker->ToolTimeStampedUpdateWithoutFiltering( tracker->TiltSensorTool->GetSourceId(), tracker->LastTiltSensorToTrackerTransform, TOOL_OK, timeSystemSec, timeSystemSec);
    }  

    if (tracker->OrientationSensorTool!=NULL)
    {
      if (data[i]->magneticField[0]>1e100)
      {
        // magnetometer data is not available, use the last transform with an invalid status to not have any missing transform
        tracker->ToolTimeStampedUpdateWithoutFiltering( tracker->OrientationSensorTool->GetSourceId(), tracker->LastOrientationSensorToTrackerTransform, TOOL_INVALID, timeSystemSec, timeSystemSec);        
      }
      else
      {
        // magnetometer data is valid

        // Compute the time that passed since the last AHRS update
        if (tracker->AhrsLastUpdateTime<0)
        {
          // this is the first update
          // just use it as a reference
          tracker->AhrsLastUpdateTime=timeSystemSec;
          continue;
        }       
        double timeSinceLastAhrsUpdateSec=timeSystemSec-tracker->AhrsLastUpdateTime;
        tracker->AhrsLastUpdateTime=timeSystemSec;

        tracker->AhrsAlgo->SetSampleFreqHz(1.0/timeSinceLastAhrsUpdateSec);

        //LOG_TRACE("samplingTime(msec)="<<1000.0*timeSinceLastAhrsUpdateSec<<", packetCount="<<count);
        //LOG_TRACE("gyroX="<<std::fixed<<std::setprecision(2)<<std::setw(6)<<data[i]->angularRate[0]<<", gyroY="<<data[i]->angularRate[1]<<", gyroZ="<<data[i]->angularRate[2]);               
        //LOG_TRACE("magX="<<std::fixed<<std::setprecision(2)<<std::setw(6)<<data[i]->magneticField[0]<<", magY="<<data[i]->magneticField[1]<<", magZ="<<data[i]->magneticField[2]);               

        if (tracker->AhrsUseMagnetometer)
        {
          tracker->AhrsAlgo->Update(          
            vtkMath::RadiansFromDegrees(data[i]->angularRate[0]), vtkMath::RadiansFromDegrees(data[i]->angularRate[1]), vtkMath::RadiansFromDegrees(data[i]->angularRate[2]),
            data[i]->acceleration[0], data[i]->acceleration[1], data[i]->acceleration[2],
            data[i]->magneticField[0], data[i]->magneticField[1], data[i]->magneticField[2]);
        }
        else
        {
          tracker->AhrsAlgo->UpdateIMU(          
            vtkMath::RadiansFromDegrees(data[i]->angularRate[0]), vtkMath::RadiansFromDegrees(data[i]->angularRate[1]), vtkMath::RadiansFromDegrees(data[i]->angularRate[2]),
            data[i]->acceleration[0], data[i]->acceleration[1], data[i]->acceleration[2]);
        }
								
								
        double rotQuat[4]={0};
        tracker->AhrsAlgo->GetOrientation(rotQuat[0],rotQuat[1],rotQuat[2],rotQuat[3]);

        double rotMatrix[3][3]={0};
        vtkMath::QuaternionToMatrix3x3(rotQuat, rotMatrix); 

        for (int c=0;c<3; c++)
        {
          for (int r=0;r<3; r++)
          {
            tracker->LastOrientationSensorToTrackerTransform->SetElement(r,c,rotMatrix[r][c]);
          }
        }

        tracker->ToolTimeStampedUpdateWithoutFiltering( tracker->OrientationSensorTool->GetSourceId(), tracker->LastOrientationSensorToTrackerTransform, TOOL_OK, timeSystemSec, timeSystemSec);            
      }            
	  if(tracker->FilteredTiltSensorTool!=NULL)
	  {
			//FROM HERE:
   // Compute the time that passed since the last FilteredTilt AHRS update
    if (tracker->FilteredTiltAhrsLastUpdateTime<0)
    {
      // this is the first update
      // just use it as a reference
      tracker->FilteredTiltAhrsLastUpdateTime=timeSystemSec;
      continue;
    }       
    double timeSinceLastFilteredTiltAhrsUpdateSec=timeSystemSec-tracker->FilteredTiltAhrsLastUpdateTime;
    tracker->FilteredTiltAhrsLastUpdateTime=timeSystemSec;

    tracker->FilteredTiltAhrsAlgo->SetSampleFreqHz(1.0/timeSinceLastFilteredTiltAhrsUpdateSec);
				//TO HERE TO BE MOVED INTO AHRS ALGO

				tracker->FilteredTiltAhrsAlgo->UpdateIMU(          
    vtkMath::RadiansFromDegrees(data[i]->angularRate[0]), vtkMath::RadiansFromDegrees(data[i]->angularRate[1]), vtkMath::RadiansFromDegrees(data[i]->angularRate[2]),
    data[i]->acceleration[0], data[i]->acceleration[1], data[i]->acceleration[2]);

				//make a function that does this 
				//quaternionToRotationMatrix(&FilteredTiltAhrsAlgo,&filteredTiltRotMatrix);
				double filteredTiltRotQuat[4]={0};
    tracker->FilteredTiltAhrsAlgo->GetOrientation(filteredTiltRotQuat[0],filteredTiltRotQuat[1],filteredTiltRotQuat[2],filteredTiltRotQuat[3]);

    double filteredTiltRotMatrix[3][3]={0};
    vtkMath::QuaternionToMatrix3x3(filteredTiltRotQuat, filteredTiltRotMatrix); 
				


				// Compose matrix that transforms the x axis to the input vector by rotations around two orthogonal axes
				vtkSmartPointer<vtkTransform> transform=vtkSmartPointer<vtkTransform>::New();      

				double filteredDownVector_Sensor[4] = {0,0,0,0};
				filteredDownVector_Sensor[0] = filteredTiltRotMatrix[2][0];
				filteredDownVector_Sensor[1] = filteredTiltRotMatrix[2][1];
				filteredDownVector_Sensor[2] = filteredTiltRotMatrix[2][2];
				//vtkMath::Normalize(downVector_Sensor);      
				// Sensor axis vector that is assumed to always point to West. This is chosen so that cross(westVector_Sensor, downVector_Sensor) = southVector_Sensor.
				double filteredWestVector_Sensor[4]={0,0,0,0};
				double filteredSouthVector_Sensor[4] = {0,0,0,0};

				if (tracker->FilteredTiltSensorWestAxisIndex>=0 && tracker->FilteredTiltSensorWestAxisIndex<3)
				{
						// Align a sensor axis with the West direction.
						// Now just set the direction, the length will be corrected at the end.
						filteredWestVector_Sensor[tracker->FilteredTiltSensorWestAxisIndex]=1;
				}
				else
				{
						LOG_ERROR("Invalid FilteredTiltSensorWestAxisIndex is specified (valid values are 0, 1, 2). Use default: 1.");
						filteredWestVector_Sensor[1]=1;
				}
				//can probably make a function to do all this crossing too
				

				vtkMath::Cross(filteredWestVector_Sensor, filteredDownVector_Sensor, filteredSouthVector_Sensor); // compute South
				vtkMath::Normalize(filteredSouthVector_Sensor);
				vtkMath::Cross(filteredDownVector_Sensor, filteredSouthVector_Sensor, filteredWestVector_Sensor); // compute West
				vtkMath::Normalize(filteredWestVector_Sensor);


				// row 0
				tracker->LastFilteredTiltSensorToTrackerTransform->SetElement(0,0,filteredSouthVector_Sensor[0]);
				tracker->LastFilteredTiltSensorToTrackerTransform->SetElement(0,1,filteredSouthVector_Sensor[1]);
				tracker->LastFilteredTiltSensorToTrackerTransform->SetElement(0,2,filteredSouthVector_Sensor[2]);
				// row 1
				tracker->LastFilteredTiltSensorToTrackerTransform->SetElement(1,0,filteredWestVector_Sensor[0]);
				tracker->LastFilteredTiltSensorToTrackerTransform->SetElement(1,1,filteredWestVector_Sensor[1]);
				tracker->LastFilteredTiltSensorToTrackerTransform->SetElement(1,2,filteredWestVector_Sensor[2]);
				// row 2
				tracker->LastFilteredTiltSensorToTrackerTransform->SetElement(2,0,filteredDownVector_Sensor[0]);
				tracker->LastFilteredTiltSensorToTrackerTransform->SetElement(2,1,filteredDownVector_Sensor[1]);
				tracker->LastFilteredTiltSensorToTrackerTransform->SetElement(2,2,filteredDownVector_Sensor[2]);

	
				tracker->ToolTimeStampedUpdateWithoutFiltering( tracker->FilteredTiltSensorTool->GetSourceId(), tracker->LastFilteredTiltSensorToTrackerTransform, TOOL_OK, timeSystemSec, timeSystemSec);

				// write back the results to the filteredTilt_AHRS algorithm
				for (int c=0;c<3; c++)
				{
						for (int r=0;r<3; r++)
						{
								filteredTiltRotMatrix[r][c]=tracker->LastFilteredTiltSensorToTrackerTransform->GetElement(r,c);
						}
				}
				vtkMath::Matrix3x3ToQuaternion(filteredTiltRotMatrix,filteredTiltRotQuat);         
				tracker->FilteredTiltAhrsAlgo->SetOrientation(filteredTiltRotQuat[0],filteredTiltRotQuat[1],filteredTiltRotQuat[2],filteredTiltRotQuat[3]);

    }  
	  
	 
    }
  }

  return 0;
}

//-------------------------------------------------------------------------
PlusStatus vtkPhidgetSpatialTracker::InternalConnect()
{
  LOG_TRACE( "vtkPhidgetSpatialTracker::Connect" ); 

  this->AccelerometerTool = NULL;
  GetToolByPortName("Accelerometer", this->AccelerometerTool);

  this->GyroscopeTool = NULL;
  GetToolByPortName("Gyroscope", this->GyroscopeTool);

  this->MagnetometerTool = NULL;
  GetToolByPortName("Magnetometer", this->MagnetometerTool);

  this->TiltSensorTool = NULL;
  GetToolByPortName("TiltSensor", this->TiltSensorTool);
  
  this->FilteredTiltSensorTool = NULL;
  GetToolByPortName("FilteredTiltSensor",this->FilteredTiltSensorTool);

  this->OrientationSensorTool = NULL;
  GetToolByPortName("OrientationSensor", this->OrientationSensorTool);

  // TODO: verify tool definition

  //Create the communicator object to the PhidgetSpatial device
  CPhidgetSpatial_create(&this->SpatialDeviceHandle);

  //Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
  CPhidget_set_OnAttach_Handler((CPhidgetHandle)this->SpatialDeviceHandle, AttachHandler, this);
  CPhidget_set_OnDetach_Handler((CPhidgetHandle)this->SpatialDeviceHandle, DetachHandler, this);
  CPhidget_set_OnError_Handler((CPhidgetHandle)this->SpatialDeviceHandle, ErrorHandler, this);

  //Registers a callback that will run according to the set data rate that will return the spatial data changes
  CPhidgetSpatial_set_OnSpatialData_Handler(this->SpatialDeviceHandle, vtkPhidgetSpatialTracker::SpatialDataHandler, this);

  //This will initiate the SystemTime (time reference in Plus) to TrackerTime (time reference of the internal clock of the device) offset computation
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
  // Allowed userDataRateMsec values: 8, 16, 32, 64, 128, 256, 512, 1000, set the closest one
  if (userDataRateMsec<12) { userDataRateMsec=8; }
  else if (userDataRateMsec<24) { userDataRateMsec=16; }
  else if (userDataRateMsec<48) { userDataRateMsec=32; }
  else if (userDataRateMsec<96) { userDataRateMsec=64; }
  else if (userDataRateMsec<192) { userDataRateMsec=128; }
  else if (userDataRateMsec<384) { userDataRateMsec=256; }
  else if (userDataRateMsec<756) { userDataRateMsec=512; }
  else { userDataRateMsec=1000; }
  CPhidgetSpatial_setDataRate(this->SpatialDeviceHandle, userDataRateMsec);
  LOG_DEBUG("DataRate (msec):" << userDataRateMsec);

  // To set compass correction parameters:
  //  CPhidgetSpatial_setCompassCorrectionParameters(this->SpatialDeviceHandle, 0.648435, 0.002954, -0.024140, 0.002182, 1.520509, 1.530625, 1.575390, -0.002039, 0.003182, -0.001966, -0.013848, 0.003168, -0.014385);
  // To reset compass correction parameters:
  //  CPhidgetSpatial_resetCompassCorrectionParameters(this->SpatialDeviceHandle);

  if (this->ZeroGyroscopeOnConnect)
  {
    ZeroGyroscope();
  }

  // Initialize AHRS algorithm
  this->AhrsAlgo->SetSampleFreqHz(1000.0/userDataRateMsec); // more accurate value will be set at each update step anyway
  this->AhrsAlgo->SetGain(this->AhrsAlgorithmGain[0], this->AhrsAlgorithmGain[1]);
  this->FilteredTiltAhrsAlgo->SetSampleFreqHz(1000.0/userDataRateMsec);
  this->FilteredTiltAhrsAlgo->SetGain(this->FilteredTiltAhrsAlgorithmGain[0], this->FilteredTiltAhrsAlgorithmGain[1]);

  return PLUS_SUCCESS; 
}

//-------------------------------------------------------------------------
PlusStatus vtkPhidgetSpatialTracker::InternalDisconnect()
{
  LOG_TRACE( "vtkPhidgetSpatialTracker::Disconnect" ); 
  this->StopRecording();
  CPhidget_close((CPhidgetHandle)this->SpatialDeviceHandle);
  CPhidget_delete((CPhidgetHandle)this->SpatialDeviceHandle);
  this->SpatialDeviceHandle = NULL;
  this->AccelerometerTool = NULL;
  this->GyroscopeTool = NULL;
  this->MagnetometerTool = NULL;
  this->TiltSensorTool = NULL;
  this->FilteredTiltSensorTool=NULL;
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
PlusStatus vtkPhidgetSpatialTracker::InternalStartRecording()
{
  LOG_TRACE( "vtkPhidgetSpatialTracker::InternalStartRecording" ); 
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPhidgetSpatialTracker::InternalStopRecording()
{
  LOG_TRACE( "vtkPhidgetSpatialTracker::InternalStopRecording" );   
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPhidgetSpatialTracker::ReadConfiguration(vtkXMLDataElement* config)
{
  // Read superclass configuration first
  Superclass::ReadConfiguration(config); 

  if ( config == NULL ) 
  {
    LOG_WARNING("Unable to find BrachyTracker XML data element");
    return PLUS_FAIL; 
  }

  vtkXMLDataElement* trackerConfig = this->FindThisDeviceElement(config);
  if (trackerConfig == NULL) 
  {
    LOG_ERROR("Cannot find Tracker element in XML tree!");
    return PLUS_FAIL;
  }

  const char* zeroGyroscopeOnConnect = trackerConfig->GetAttribute("ZeroGyroscopeOnConnect"); 
  if ( zeroGyroscopeOnConnect != NULL )
  {
    if ( STRCASECMP(zeroGyroscopeOnConnect, "true") == 0 )
    {
      this->ZeroGyroscopeOnConnect=true;
    }
    else
    {
      this->ZeroGyroscopeOnConnect=false;
    }
  } 

  int tiltSensorWestAxisIndex=0; 
  if ( trackerConfig->GetScalarAttribute("TiltSensorWestAxisIndex", tiltSensorWestAxisIndex ) ) 
  {
    if (tiltSensorWestAxisIndex<0 || tiltSensorWestAxisIndex>2)
    {
      LOG_ERROR("TiltSensorWestAxisIndex is invalid. Specified value: "<<tiltSensorWestAxisIndex<<". Valid values: 0, 1, 2. Keep using the default value: "
        <<this->TiltSensorWestAxisIndex);
    }
    else
    {
      this->TiltSensorWestAxisIndex=tiltSensorWestAxisIndex;
    }
  }

  int filteredTiltSensorWestAxisIndex=0; 
  if ( trackerConfig->GetScalarAttribute("FilteredTiltSensorWestAxisIndex", filteredTiltSensorWestAxisIndex ) ) 
  {
    if (filteredTiltSensorWestAxisIndex<0 || filteredTiltSensorWestAxisIndex>2)
    {
      LOG_ERROR("FilteredTiltSensorWestAxisIndex is invalid. Specified value: "<<filteredTiltSensorWestAxisIndex<<". Valid values: 0, 1, 2. Keep using the default value: "
        <<this->FilteredTiltSensorWestAxisIndex);
    }
    else
    {
      this->FilteredTiltSensorWestAxisIndex=filteredTiltSensorWestAxisIndex;
    }
  }

  double ahrsAlgorithmGain[2]={0}; 
  if ( trackerConfig->GetVectorAttribute("AhrsAlgorithmGain", 2, ahrsAlgorithmGain ) ) 
  {
    this->AhrsAlgorithmGain[0]=ahrsAlgorithmGain[0]; 
    this->AhrsAlgorithmGain[1]=ahrsAlgorithmGain[1]; 
  }

  double filteredTiltAhrsAlgorithmGain[2]={0}; 
  if ( trackerConfig->GetVectorAttribute("FitleredTiltAhrsAlgorithmGain", 2, filteredTiltAhrsAlgorithmGain ) ) 
  {
    this->FilteredTiltAhrsAlgorithmGain[0]=filteredTiltAhrsAlgorithmGain[0]; 
    this->FilteredTiltAhrsAlgorithmGain[1]=filteredTiltAhrsAlgorithmGain[1]; 
  }


  const char* ahrsAlgoName = trackerConfig->GetAttribute("AhrsAlgorithm"); 
  if ( ahrsAlgoName != NULL )
  {
    if ( STRCASECMP("MADGWICK_MARG", ahrsAlgoName)==0 || STRCASECMP("MADGWICK_IMU", ahrsAlgoName)==0)
    {
      if (dynamic_cast<MadgwickAhrsAlgo*>(this->AhrsAlgo)==0)
      {
        // not the requested type
        // delete the old algo and create a new one with the correct type
        delete this->AhrsAlgo; 
        this->AhrsAlgo=new MadgwickAhrsAlgo;
      }
      if (STRCASECMP("MADGWICK_MARG", ahrsAlgoName)==0)
      {
        this->AhrsUseMagnetometer=true;
      }
      else
      {
        this->AhrsUseMagnetometer=false;
      }
    }
    else if ( STRCASECMP("MAHONY_MARG", ahrsAlgoName)==0 || STRCASECMP("MAHONY_IMU", ahrsAlgoName)==0)
    {
      if (dynamic_cast<MahonyAhrsAlgo*>(this->AhrsAlgo)==0)
      {
        // not the requested type
        // delete the old algo and create a new one with the correct type
        delete this->AhrsAlgo; 
        this->AhrsAlgo=new MahonyAhrsAlgo;
      }
      if (STRCASECMP("MAHONY_MARG", ahrsAlgoName)==0)
      {
        this->AhrsUseMagnetometer=true;
      }
      else
      {
        this->AhrsUseMagnetometer=false;
      }
    }
    else
    {
      LOG_ERROR("Unable to recognize AHRS algorithm type: " << ahrsAlgoName <<". Supported types: MADGWICK_MARG, MAHONY_MARG, MADGWICK_IMU, MAHONY_IMU");
      return PLUS_FAIL; 
    }
  }
  const char* filteredTiltAhrsAlgoName = trackerConfig->GetAttribute("FilteredTiltAhrsAlgorithm"); 
  if ( filteredTiltAhrsAlgoName != NULL )
  {
    if (STRCASECMP("MADGWICK_IMU", filteredTiltAhrsAlgoName)==0)
    {
      if (dynamic_cast<MadgwickAhrsAlgo*>(this->FilteredTiltAhrsAlgo)==0)
      {
        // not the requested type
        // delete the old algo and create a new one with the correct type
        delete this->FilteredTiltAhrsAlgo; 
        this->FilteredTiltAhrsAlgo=new MadgwickAhrsAlgo;
      }
    }
    else if (STRCASECMP("MAHONY_IMU", filteredTiltAhrsAlgoName)==0)
    {
      if (dynamic_cast<MahonyAhrsAlgo*>(this->FilteredTiltAhrsAlgo)==0)
      {
        // not the requested type
        // delete the old algo and create a new one with the correct type
        delete this->FilteredTiltAhrsAlgo; 
        this->FilteredTiltAhrsAlgo=new MahonyAhrsAlgo;
      }
    }
    else
    {
      LOG_ERROR("Unable to recognize AHRS algorithm type for Filtered Tilt: " << filteredTiltAhrsAlgoName <<". Supported types: MADGWICK_IMU, MAHONY_IMU");
      return PLUS_FAIL; 
    }
  }



  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPhidgetSpatialTracker::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  if ( rootConfigElement == NULL )
  {
    LOG_ERROR("Configuration is invalid");
    return PLUS_FAIL;
  }

  // Write configuration 
  Superclass::WriteConfiguration(rootConfigElement); 

  vtkXMLDataElement* trackerConfig = this->FindThisDeviceElement(rootConfigElement);
  if ( trackerConfig == NULL) 
  {
    LOG_ERROR("Cannot find Tracker element in XML tree!");
    return PLUS_FAIL;
  }

  if (this->ZeroGyroscopeOnConnect)
  {
    trackerConfig->SetAttribute("ZeroGyroscopeOnConnect","TRUE");
  }

  if (this->TiltSensorTool)
  {
    trackerConfig->SetIntAttribute("TiltSensorWestAxisIndex", this->TiltSensorWestAxisIndex);  
  }

  if (this->FilteredTiltSensorTool)
  {
    trackerConfig->SetIntAttribute("FilteredTiltSensorWestAxisIndex", this->FilteredTiltSensorWestAxisIndex); 
	if (this->FilteredTiltAhrsAlgorithmGain[1]==0.0)
    {
      // if the second gain parameter is zero then just write the first value
      trackerConfig->SetDoubleAttribute( "FilteredTiltAhrsAlgorithmGain", this->FilteredTiltAhrsAlgorithmGain[0] ); 
    }
    else
    {
      trackerConfig->SetVectorAttribute( "FilteredTiltAhrsAlgorithmGain", 2, this->FilteredTiltAhrsAlgorithmGain );
    }

    if ( dynamic_cast<MadgwickAhrsAlgo*>(this->FilteredTiltAhrsAlgo)!=0)
    {
						trackerConfig->SetAttribute( "FilteredTiltAhrsAlgorithm", "MADGWICK_IMU" );
    }
    else if (dynamic_cast<MahonyAhrsAlgo*>(this->FilteredTiltAhrsAlgo)!=0)
    {
      trackerConfig->SetAttribute( "FilteredTiltAhrsAlgorithm", "MAHONY_IMU" );
    }
    else
    {
      LOG_ERROR("Unknown AHRS algorithm type for Filtered Tilt Sensor. Cannot write name to XML.");
    } 
  }

  if (this->OrientationSensorTool)
  {
    if (this->AhrsAlgorithmGain[1]==0.0)
    {
      // if the second gain parameter is zero then just write the first value
      trackerConfig->SetDoubleAttribute( "AhrsAlgorithmGain", this->AhrsAlgorithmGain[0] ); 
    }
    else
    {
      trackerConfig->SetVectorAttribute( "AhrsAlgorithmGain", 2, this->AhrsAlgorithmGain );
    }

    if ( dynamic_cast<MadgwickAhrsAlgo*>(this->AhrsAlgo)!=0)
    {
      if (this->AhrsUseMagnetometer)
      {
        trackerConfig->SetAttribute( "AhrsAlgorithm", "MADGWICK_MARG" ); 
      }
      else
      {
        trackerConfig->SetAttribute( "AhrsAlgorithm", "MADGWICK_IMU" ); 
      }
    }
    else if (dynamic_cast<MahonyAhrsAlgo*>(this->AhrsAlgo)!=0)
    {
      if (this->AhrsUseMagnetometer)
      {
        trackerConfig->SetAttribute( "AhrsAlgorithm", "MAHONY_MARG" ); 
      }
      else
      {
        trackerConfig->SetAttribute( "AhrsAlgorithm", "MAHONY_IMU" ); 
      }
    }
    else
    {
      LOG_ERROR("Unknown AHRS algorithm type. Cannot write name to XML.");
    }    
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPhidgetSpatialTracker::ZeroGyroscope()
{  
  LOG_INFO("Zeroing the gyroscope. Keep the sensor stationary for 2 seconds.");
  CPhidgetSpatial_zeroGyro(this->SpatialDeviceHandle);
}

//----------------------------------------------------------------------------
bool vtkPhidgetSpatialTracker::IsResettable()
{
  return true;
}

//----------------------------------------------------------------------------
PlusStatus vtkPhidgetSpatialTracker::Reset()
{
  if( this->IsRecording() )
  {
    ZeroGyroscope();
  }

  return PLUS_SUCCESS;
}
