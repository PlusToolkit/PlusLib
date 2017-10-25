/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/


// Local includes
#include "PlusConfigure.h"
#include "PlusVideoFrame.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusOpenHapticsDevice.h"

//HD includes
#include <HDU/hduVector.h>
#include <HDU/hduError.h>
#include <HD/hd.h>

// VTK includes
#include <vtkImageData.h>
#include <vtkImageImport.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkTransform.h>

// STL includes
#include <fstream>
#include <iostream>
#include <set>
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

  //Current position data is pulled here
  HDdouble trans[16];
  HDdouble pos[3];  
  hdBeginFrame(handle);
  hdMakeCurrentDevice(handle);  
  hdGetDoublev(HD_CURRENT_TRANSFORM, trans);
  hdGetDoublev(HD_CURRENT_POSITION, pos);
  hdEndFrame(handle);

  vtkPlusDataSource* tool = NULL;

  if (client->GetToolByPortName("Stylus", tool) != PLUS_SUCCESS)
  {
    LOG_ERROR("Stylus not found!");    
  }

  vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
  vtkSmartPointer<vtkMatrix4x4> finalMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkSmartPointer<vtkMatrix4x4> rasCorrection = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkSmartPointer<vtkTransform> rotation = vtkSmartPointer<vtkTransform>::New();
  double orient[3];

  //Arrange transformations in correct order
  transform->Identity();
  transform->Translate(pos);
  rotation->SetMatrix(trans);
  rotation->GetOrientation(orient);
  transform->RotateX(-1* orient[0] + 180);
  transform->RotateY(orient[1]);
  transform->RotateZ(orient[2]);  
  transform->GetMatrix(finalMatrix);

  //Transform to RAS coordinate system
  rasCorrection->Identity();
  rasCorrection->SetElement(0,0,-1);
  rasCorrection->SetElement(1,1,0);
  rasCorrection->SetElement(1,2,1);
  rasCorrection->SetElement(2,2,0);
  rasCorrection->SetElement(2,1,1);
  vtkMatrix4x4::Multiply4x4(rasCorrection,finalMatrix,finalMatrix);

  
  client->ToolTimeStampedUpdate(tool->GetId(), finalMatrix, TOOL_OK, client->FrameNumber,unfilteredTimestamp);

  return HD_CALLBACK_DONE;
}

