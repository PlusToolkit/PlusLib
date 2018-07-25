/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusWitMotionTracker.h"
#include "PlusSerialLine.h"
#include "vtkPlusDataSource.h"

// VTK includes
#include <vtkMatrix4x4.h>
#include <vtkMath.h>
#include <vtkObjectFactory.h>
#include <vtkTransform.h>

//-------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusWitMotionTracker);

//-------------------------------------------------------------------------

namespace
{
  const char WIT_IMU_DATA_FRAME = 0x55;
  const char WIT_ACCELERATION_FRAME = 0x51;
  const char WIT_VELOCITY_FRAME = 0x52;
  const char WIT_ORIENTATION_FRAME = 0x53;
}

//-------------------------------------------------------------------------
vtkPlusWitMotionTracker::vtkPlusWitMotionTracker()
  : Accelerometer(nullptr)
{
}

//-------------------------------------------------------------------------
vtkPlusWitMotionTracker::~vtkPlusWitMotionTracker()
{

}

//-------------------------------------------------------------------------
PlusStatus vtkPlusWitMotionTracker::NotifyConfigured()
{
  if (this->GetToolByPortName("OrientationSensor", this->Accelerometer) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to locate tool with PortName=\"OrientationSensor\". This is required.");
    return PLUS_FAIL;
  }
  
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
// Receive Response from WIT-Motion Accelerometer
PlusStatus vtkPlusWitMotionTracker::ReceiveData()
{
  StreamBufferType::size_type usRxLength;

  // Continue reading from serial port until we get orientation data
  while (this->Serial->GetNumberOfBytesAvailableForReading() > 0)
  {
    usRxLength = this->Serial->Read(this->StreamData.data(), this->StreamData.size());
    if (this->StreamData.data()[0] == WIT_IMU_DATA_FRAME && this->StreamData.data()[1] == WIT_ORIENTATION_FRAME)
    {
      return PLUS_SUCCESS;
    }
  }
  return PLUS_FAIL;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusWitMotionTracker::InternalUpdate()
{
  if (ReceiveData() == PLUS_SUCCESS)
  {
    if (this->DecodeData() == PLUS_SUCCESS && this->StreamData.data()[1] == WIT_ORIENTATION_FRAME)
    {
      // Only do a transform update if we receive new orientation data
      vtkNew<vtkTransform> tran;
      tran->Identity();
      tran->PostMultiply();
      tran->RotateX(this->Orientation[0]);
      tran->RotateY(this->Orientation[1]);
      tran->RotateZ(this->Orientation[2]);

      // This device has no frame numbering, so use internal frame counting
      this->ToolTimeStampedUpdate(this->Accelerometer->GetId(), tran->GetMatrix(), TOOL_OK, this->FrameNumber++, UNDEFINED_TIMESTAMP);
    }
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusWitMotionTracker::DecodeData()
{
  switch (this->StreamData.data()[1])
  {
  case WIT_ACCELERATION_FRAME:
  {
    this->Acceleration[0] = (short(this->StreamData.data()[3] << 8 | this->StreamData.data()[2])) / 32768.0 * 16;
    this->Acceleration[1] = (short(this->StreamData.data()[5] << 8 | this->StreamData.data()[4])) / 32768.0 * 16;
    this->Acceleration[2] = (short(this->StreamData.data()[7] << 8 | this->StreamData.data()[6])) / 32768.0 * 16;
    this->Acceleration[3] = (short(this->StreamData.data()[9] << 8 | this->StreamData.data()[8])) / 340.0 + 36.25;

    break;
  }
  case WIT_VELOCITY_FRAME:
  {
    this->Velocity[0] = (short(this->StreamData.data()[3] << 8 | this->StreamData.data()[2])) / 32768.0 * 2000;
    this->Velocity[1] = (short(this->StreamData.data()[5] << 8 | this->StreamData.data()[4])) / 32768.0 * 2000;
    this->Velocity[2] = (short(this->StreamData.data()[7] << 8 | this->StreamData.data()[6])) / 32768.0 * 2000;
    this->Velocity[3] = (short(this->StreamData.data()[9] << 8 | this->StreamData.data()[8])) / 340.0 + 36.25;

    break;
  }
  case WIT_ORIENTATION_FRAME:
  {
    this->Orientation[0] = (short(this->StreamData.data()[3] << 8 | this->StreamData.data()[2])) / 32768.0 * 180;
    this->Orientation[1] = (short(this->StreamData.data()[5] << 8 | this->StreamData.data()[4])) / 32768.0 * 180;
    this->Orientation[2] = (short(this->StreamData.data()[7] << 8 | this->StreamData.data()[6])) / 32768.0 * 180;
    this->Temperature = (short(this->StreamData.data()[9] << 8 | this->StreamData.data()[8])) / 100;

    break;
  }
  }

  return PLUS_SUCCESS;
}
