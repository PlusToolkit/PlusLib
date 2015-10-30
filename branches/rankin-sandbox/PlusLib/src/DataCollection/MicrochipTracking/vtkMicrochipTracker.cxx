/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkMicrochipTracker.h"

#include "SerialLine.h"
#include "vtkMatrix4x4.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPlusDataSource.h"

//#include <deque>

vtkStandardNewMacro(vtkMicrochipTracker);

//-------------------------------------------------------------------------
vtkMicrochipTracker::vtkMicrochipTracker()
{
  this->OrientationSensorToTracker = vtkMatrix4x4::New();
  this->OrientationSensorTool = NULL;
}

//-------------------------------------------------------------------------
vtkMicrochipTracker::~vtkMicrochipTracker()
{
  this->OrientationSensorToTracker->Delete();
  this->OrientationSensorToTracker=NULL;
}

//-------------------------------------------------------------------------
PlusStatus vtkMicrochipTracker::InternalConnect()
{
  LOG_TRACE( "vtkMicrochipTracker::Connect" ); 
  if (this->Superclass::InternalConnect()!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  this->OrientationSensorTool = NULL;
  return GetToolByPortName("OrientationSensor", this->OrientationSensorTool);
}

//-------------------------------------------------------------------------
PlusStatus vtkMicrochipTracker::InternalDisconnect()
{
  LOG_TRACE( "vtkMicrochipTracker::Disconnect" ); 
  if (this->Superclass::InternalDisconnect()!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  this->OrientationSensorTool = NULL;
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkMicrochipTracker::InternalUpdate()
{
  // Either update or send commands - but not simultaneously
  PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);

  std::string textReceived;
  double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();
  
  // Determine the maximum time to spend in the loop (acquisition time period, but maximum 1 sec)
  double maxReadTimeSec = (this->AcquisitionRate < 1.0) ? 1.0 : 1/this->AcquisitionRate;
  double startTime = vtkAccurateTimer::GetSystemTime();
  while (this->Serial->GetNumberOfBytesAvailableForReading()>0)
  {
    unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();
    ReceiveResponse(textReceived);
    //LOG_DEBUG("Received from serial device: "<<textReceived);
    if (vtkAccurateTimer::GetSystemTime()-startTime>maxReadTimeSec)
    {
      // force exit from the loop if continuously receiving data
      break;
    }
  }

  if (this->OrientationSensorTool!=NULL)
  {
    // LOG_TRACE("roll="<<this->EulerRoll.GetValue() <<", pitch="<<this->EulerPitch.GetValue() <<", yaw="<<this->EulerYaw.GetValue());

    double rotationQuat[4]= {1,0,0,0};
    
    // The hardware only provides output if the orientation is changed, therefore if no message is received then assume that
    // the transform has not changed.
    if (!textReceived.empty() && vtkMicrochipTracker::ParseMessage(textReceived, rotationQuat)==PLUS_SUCCESS)
    {
      double rotationMatrix[3][3] = {{1,0,0},{0,1,0},{0,0,1}};
      vtkMath::QuaternionToMatrix3x3(rotationQuat, rotationMatrix);
      for (int row=0; row<3; row++)
      {
        for (int col=0; col<3; col++)
        {
          this->OrientationSensorToTracker->Element[row][col] = rotationMatrix[row][col];
        }
      }
    }
    
    // This device has no frame numbering, so just auto increment tool frame number
    unsigned long frameNumber = this->OrientationSensorTool->GetFrameNumber() + 1 ; 
    ToolTimeStampedUpdate( this->OrientationSensorTool->GetSourceId(), this->OrientationSensorToTracker, TOOL_OK, frameNumber, unfilteredTimestamp); 
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkMicrochipTracker::ParseMessage(std::string& textReceived, double* rotationQuat)
{
  // Example message to parse:
  // X: 0.713 Y:-0.036 Z: 0.008 W: 0.699
  // VTK quaternion (rotationQuat)is in the form [w, x, y, z].
  if (textReceived.size()<35)
  {
    LOG_ERROR("Failed to parse message: "<<textReceived<<" (expected longer message)")
    return PLUS_FAIL;
  }
  rotationQuat[1] = atof(textReceived.substr(2,6).c_str());
  rotationQuat[2] = atof(textReceived.substr(11,6).c_str());
  rotationQuat[3] = atof(textReceived.substr(20,6).c_str());
  rotationQuat[0] = atof(textReceived.substr(29,6).c_str());
  return PLUS_SUCCESS;
}
