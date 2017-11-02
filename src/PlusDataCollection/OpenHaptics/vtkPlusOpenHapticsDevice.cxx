/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/


// Local includes
#include "PlusConfigure.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusOpenHapticsDevice.h"

//HD includes
#include <HDU/hduVector.h>
#include <HDU/hduError.h>
#include <HD/hd.h>

// VTK includes
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>

// STL includes
#include <fstream>
#include <iostream>
#include <sstream>

vtkStandardNewMacro(vtkPlusOpenHapticsDevice);

vtkPlusOpenHapticsDevice::vtkPlusOpenHapticsDevice()
{
  this->FrameNumber = 0;
  this->isHapticDeviceInitialized = false;
  this->DeviceHandle = -1;
  this->RequirePortNameInDeviceSetConfiguration = true;
  this->StartThreadForInternalUpdates = true;
  this->AcquisitionRate = 20;
  this->DeviceName = "PLUS";
}

vtkPlusOpenHapticsDevice::~vtkPlusOpenHapticsDevice()
{
  if (this->isHapticDeviceInitialized)
  {
    this->isHapticDeviceInitialized = false;
  }
}

std::string vtkPlusOpenHapticsDevice::GetSdkVersion()
{
  std::stringstream ss;
  ss << HD_VERSION_MAJOR_NUMBER << "." << HD_VERSION_MINOR_NUMBER << "." << HD_VERSION_BUILD_NUMBER;
  return ss.str();
}

PlusStatus vtkPlusOpenHapticsDevice::Probe()
{
  if (this->isHapticDeviceInitialized)
  {
    LOG_ERROR("vtkPlusOpenHapticsDevice::Probe should not be called while the device is already initialized");
    return PLUS_FAIL;
  }
  if (this->InternalConnect())
  {
    this->InternalDisconnect();
    LOG_INFO("vtkPlusOpenHapticsDevice::Probe found OpenHaptics Device");
    return PLUS_SUCCESS;
  }
  else
  {
    LOG_ERROR("vtkPlusOpenHapticsDevice::Probe failed to connect to device");
    return PLUS_FAIL;
  }

}

PlusStatus vtkPlusOpenHapticsDevice::InternalUpdate()
{
  //schedule the internal callback - must be done each time device is polled
  //the scheduler is intended to manage multiple clients requesting data from the same device
  hdScheduleSynchronous(positionCallback, this, HD_MAX_SCHEDULER_PRIORITY);

  return PLUS_SUCCESS;
}

PlusStatus vtkPlusOpenHapticsDevice::InternalConnect()
{
  if (this->isHapticDeviceInitialized)
  {
    LOG_DEBUG("Already connected - " << this->DeviceName);
    return PLUS_SUCCESS;
  }

  if (this->InputChannels.empty())
  {
    LOG_WARNING("No force input has been provided");
  }
  else
  {
    if (this->InputChannels.size() > 1)
    {
      LOG_WARNING("Multiple input channels present.  Only first channel will be checked for force data");
    }
    vtkPlusDataSource* forceInput;
    if (!this->InputChannels[0]->GetOwnerDevice()->GetToolByPortName("Force", forceInput))
    {
      LOG_WARNING("No Force tool in input channel.  Forces will not be available");
    }
  }

  HDErrorInfo errorFlush;
  while (HD_DEVICE_ERROR(errorFlush = hdGetError())) {}

  DeviceHandle = hdInitDevice(this->DeviceName.c_str());

  HDErrorInfo error;
  if (HD_DEVICE_ERROR(error = hdGetError()))
  {
    LOG_ERROR( "Failed to initialize Phantom Omni " << this->DeviceName);
    DeviceHandle = -1;
    return PLUS_FAIL;
  }

  // Enable forces
  hdEnable(HD_FORCE_OUTPUT);
  hdEnable(HD_FORCE_RAMPING);

  this->isHapticDeviceInitialized = true;
  LOG_INFO("Phantom initialized: " << this->DeviceName)

  hdStartScheduler();

  return PLUS_SUCCESS;
}

PlusStatus vtkPlusOpenHapticsDevice::InternalDisconnect()
{
  if (this->isHapticDeviceInitialized)
  {
    hdDisableDevice(DeviceHandle);
    hdStopScheduler();
    this->isHapticDeviceInitialized=false;
  }
  return PLUS_SUCCESS;
}

PlusStatus vtkPlusOpenHapticsDevice::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  XML_READ_CSTRING_ATTRIBUTE_REQUIRED(DeviceName,deviceConfig);
  return PLUS_SUCCESS;
}

PlusStatus vtkPlusOpenHapticsDevice::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(trackerConfig, rootConfigElement);
  trackerConfig->SetAttribute("DeviceName", this->DeviceName.c_str());
  return PLUS_SUCCESS;
}

PlusStatus vtkPlusOpenHapticsDevice::InternalStartRecording()
{
  if (!this->isHapticDeviceInitialized)
  {
    LOG_ERROR("InternalStartRecording failed: OpenHaptics device has not been initialized");
    return PLUS_FAIL;
  }
  LOG_TRACE("vtkPlusOpenHapticsDevice::InternalStartRecording");
  return PLUS_SUCCESS;
}

PlusStatus vtkPlusOpenHapticsDevice::InternalStopRecording()
{
  LOG_TRACE("vtkPlusOpenHapticsDevice::InternalStopRecording");
  return PLUS_SUCCESS;
}

HDCallbackCode HDCALLBACK
vtkPlusOpenHapticsDevice::positionCallback(void* pData)
{
  
  vtkPlusOpenHapticsDevice* client = reinterpret_cast<vtkPlusOpenHapticsDevice*>(pData);
  HHD handle = client->DeviceHandle;
  HD_state state = client->DeviceState;

  const double unfilteredTimestamp = vtkPlusAccurateTimer::GetSystemTime();
  ++client->FrameNumber;

  HDdouble force[3];
  force[0] = 0;
  force[1] = 0;
  force[2] = 0;

  //Transform to RAS coordinate system
  vtkSmartPointer<vtkMatrix4x4> rasCorrection = vtkSmartPointer<vtkMatrix4x4>::New();
  rasCorrection->Identity();
  rasCorrection->SetElement(0, 0, -1);
  rasCorrection->SetElement(1, 1, 0);
  rasCorrection->SetElement(1, 2, 1);
  rasCorrection->SetElement(2, 2, 0);
  rasCorrection->SetElement(2, 1, 1);

  //assemble force data
  auto inputChannels = client->InputChannels;
  vtkPlusDataSource* forceInput;
  if (!inputChannels.empty() && inputChannels[0]->GetOwnerDevice()->GetToolByPortName("Force", forceInput))
  {
    StreamBufferItem* item = new StreamBufferItem();
    if (forceInput->GetLatestItemUidInBuffer() > 0)
    {
      forceInput->GetLatestStreamBufferItem(item);
      vtkSmartPointer<vtkMatrix4x4> forceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
      item->GetMatrix(forceMatrix);
      vtkMatrix4x4::Multiply4x4(rasCorrection, forceMatrix, forceMatrix);
      force[0] = forceMatrix->GetElement(0,3);
      force[1] = forceMatrix->GetElement(1,3);
      force[2] = forceMatrix->GetElement(2,3);
    }
    delete item;
  }
  else
  {
    LOG_TRACE("No force data available");
  }

  //Current position data is pulled here
  HDdouble trans[16];
  HDdouble pos[3];  
  HDdouble vel[3];
  hdBeginFrame(handle);
  hdMakeCurrentDevice(handle);  
  hdGetDoublev(HD_CURRENT_TRANSFORM, trans);
  hdGetDoublev(HD_CURRENT_POSITION, pos);
  hdGetDoublev(HD_CURRENT_VELOCITY, vel);
  hdSetDoublev(HD_CURRENT_FORCE, force);
  hdEndFrame(handle);

  vtkPlusDataSource* stylus = NULL;
  vtkPlusDataSource* velocity = NULL;

  if (client->GetToolByPortName("Stylus", stylus) != PLUS_SUCCESS)
  {
    LOG_ERROR("Stylus not found!");    
  }

  if (client->GetToolByPortName("StylusVelocity", velocity) != PLUS_SUCCESS)
  {
    LOG_ERROR("Stylus velocity not found!");
  }

  vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
  vtkSmartPointer<vtkMatrix4x4> toolMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkSmartPointer<vtkTransform> rotation = vtkSmartPointer<vtkTransform>::New();
  vtkSmartPointer<vtkTransform> velTrans = vtkSmartPointer<vtkTransform>::New();
  vtkSmartPointer<vtkMatrix4x4> velMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  double orient[3];

  //Arrange transformations in correct order
  transform->Identity();
  transform->Translate(pos);
  rotation->SetMatrix(trans);
  rotation->GetOrientation(orient);
  transform->RotateX(-1* orient[0] + 180);
  transform->RotateY(orient[1]);
  transform->RotateZ(orient[2]);  
  transform->GetMatrix(toolMatrix);
  velTrans->Translate(vel);
  velTrans->GetMatrix(velMatrix);
  vtkMatrix4x4::Multiply4x4(rasCorrection, velMatrix, velMatrix);
  vtkMatrix4x4::Multiply4x4(rasCorrection,toolMatrix,toolMatrix);

  client->ToolTimeStampedUpdate(velocity->GetId(), velMatrix, TOOL_OK, client->FrameNumber, unfilteredTimestamp);
  client->ToolTimeStampedUpdate(stylus->GetId(), toolMatrix, TOOL_OK, client->FrameNumber,unfilteredTimestamp);
  return HD_CALLBACK_DONE;
}

