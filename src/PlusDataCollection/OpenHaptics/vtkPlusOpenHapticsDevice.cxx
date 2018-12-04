/*=Plus=header=begin======================================================
Program: Plus
Author: Sam Horvath, Kitware, Inc.
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

//----------------------------------------------------------------------------
vtkPlusOpenHapticsDevice::vtkPlusOpenHapticsDevice()
  : FrameNumber(-1)
  , DeviceHandle(-1)
  , DeviceName("PLUS")
  , toolTransform(vtkSmartPointer<vtkTransform>::New())
  , rotation(vtkSmartPointer<vtkTransform>::New())
  , velMatrix(vtkSmartPointer<vtkMatrix4x4>::New())
  , buttonMatrix(vtkSmartPointer<vtkMatrix4x4>::New())
  , toolMatrix(vtkSmartPointer<vtkMatrix4x4>::New())
{
  this->RequirePortNameInDeviceSetConfiguration = true;
  this->StartThreadForInternalUpdates = true;
  this->AcquisitionRate = 20;

  this->rasCorrection = vtkSmartPointer<vtkMatrix4x4>::New();
  this->rasCorrection->Identity();
  this->rasCorrection->SetElement(0, 0, -1);
  this->rasCorrection->SetElement(1, 1, 0);
  this->rasCorrection->SetElement(1, 2, 1);
  this->rasCorrection->SetElement(2, 2, 0);
  this->rasCorrection->SetElement(2, 1, 1);
}

//----------------------------------------------------------------------------
vtkPlusOpenHapticsDevice::~vtkPlusOpenHapticsDevice() {}

//----------------------------------------------------------------------------
std::string vtkPlusOpenHapticsDevice::GetSdkVersion()
{
  std::stringstream ss;
  ss << HD_VERSION_MAJOR_NUMBER << "." << HD_VERSION_MINOR_NUMBER << "." << HD_VERSION_BUILD_NUMBER;
  return ss.str();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenHapticsDevice::Probe()
{
  if(this->Connected)
  {
    LOG_ERROR("vtkPlusOpenHapticsDevice::Probe should not be called while the device is already initialized");
    return PLUS_FAIL;
  }
  if(this->InternalConnect())
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

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenHapticsDevice::InternalUpdate()
{
  //schedule the internal callback - must be done each time device is polled
  //the scheduler is intended to manage multiple clients requesting data from the same device
  hdScheduleSynchronous(positionCallback, this, HD_MAX_SCHEDULER_PRIORITY);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenHapticsDevice::InternalConnect()
{

  HDErrorInfo errorFlush;
  while(HD_DEVICE_ERROR(errorFlush = hdGetError())) {}

  this->DeviceHandle = hdInitDevice(this->DeviceName.c_str());

  HDErrorInfo error;
  if(HD_DEVICE_ERROR(error = hdGetError()))
  {
    LOG_ERROR("Failed to initialize Phantom Omni " << this->DeviceName);
    this->DeviceHandle = -1;
    return PLUS_FAIL;
  }

  // Enable forces
  hdEnable(HD_FORCE_OUTPUT);
  hdEnable(HD_FORCE_RAMPING);

  LOG_DEBUG("Phantom initialized: " << this->DeviceName)

  hdStartScheduler();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenHapticsDevice::InternalDisconnect()
{
  hdDisableDevice(DeviceHandle);
  hdStopScheduler();
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenHapticsDevice::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  XML_READ_STRING_ATTRIBUTE_REQUIRED(DeviceName, deviceConfig);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenHapticsDevice::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(trackerConfig, rootConfigElement);
  trackerConfig->SetAttribute("DeviceName", this->DeviceName.c_str());
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOpenHapticsDevice::NotifyConfigured()
{
  if(this->InputChannels.empty())
  {
    LOG_INFO("No force input has been provided");
  }
  else
  {
    if(this->InputChannels.size() > 1)
    {
      LOG_INFO("Multiple input channels present.  Only first channel will be checked for force data");
    }
    vtkPlusDataSource* forceInput;
    if(this->InputChannels[0]->GetToolByPortName(forceInput, "Force") != PLUS_SUCCESS)
    {
      LOG_INFO("No Force tool in input channel.  Forces will not be available");
    }
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
HDCallbackCode HDCALLBACK
vtkPlusOpenHapticsDevice::positionCallback(void* pData)
{

  vtkPlusOpenHapticsDevice* client = reinterpret_cast<vtkPlusOpenHapticsDevice*>(pData);
  HHD handle = client->DeviceHandle;

  const double unfilteredTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();
  ++client->FrameNumber;

  HDdouble force[3];
  force[0] = 0;
  force[1] = 0;
  force[2] = 0;


  //assemble force data
  vtkPlusDataSource* forceInput;
  if(!client->InputChannels.empty() && (client->InputChannels[0]->GetToolByPortName(forceInput, "Force") == PLUS_SUCCESS))
  {
    StreamBufferItem item;
    if(forceInput->GetLatestStreamBufferItem(&item) == ITEM_OK)
    {
      if(item.GetStatus() == TOOL_OK)
      {
        vtkSmartPointer<vtkMatrix4x4> forceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
        item.GetMatrix(forceMatrix);
        vtkMatrix4x4::Multiply4x4(client->rasCorrection, forceMatrix, forceMatrix);
        force[0] = forceMatrix->GetElement(0, 3);
        force[1] = forceMatrix->GetElement(1, 3);
        force[2] = forceMatrix->GetElement(2, 3);
      }
      else
      {
        LOG_ERROR("OpenHaptics Force data tool is not valid")
      }
    }
    else
    {
      LOG_ERROR("OpenHaptics Force data tool info not recevied")
    }
  }
  else
  {
    LOG_TRACE("No force data tool has been provided");
  }

  //Current position data is pulled here
  HDdouble trans[16];
  HDdouble pos[3];
  HDdouble vel[3];
  HDint buttonVals = 0;
  HDboolean inkwell = 0;
  hdBeginFrame(handle);
  hdMakeCurrentDevice(handle);
  hdGetDoublev(HD_CURRENT_TRANSFORM, trans);
  hdGetDoublev(HD_CURRENT_POSITION, pos);
  hdGetDoublev(HD_CURRENT_VELOCITY, vel);
  hdSetDoublev(HD_CURRENT_FORCE, force);
  hdGetIntegerv(HD_CURRENT_BUTTONS, &buttonVals);
  hdGetBooleanv(HD_CURRENT_INKWELL_SWITCH, &inkwell);
  vtkPlusDataSource* stylus = NULL;
  vtkPlusDataSource* velocity = NULL;
  vtkPlusDataSource* buttons = NULL;
  hdEndFrame(handle);

  double orient[3];

  //Arrange transformations in correct order
  client->toolTransform->Identity();
  client->toolTransform->Translate(pos);
  client->rotation->SetMatrix(trans);
  client->rotation->GetOrientation(orient);
  client->toolTransform->RotateX(-1 * orient[0] + 180);
  client->toolTransform->RotateY(orient[1]);
  client->toolTransform->RotateZ(orient[2]);
  client->velMatrix->SetElement(0, 3, vel[0]);
  client->velMatrix->SetElement(1, 3, vel[1]);
  client->velMatrix->SetElement(2, 3, vel[2]);

  client->toolMatrix = client->toolTransform->GetMatrix();
  vtkMatrix4x4::Multiply4x4(client->rasCorrection, client->velMatrix, client->velMatrix);
  vtkMatrix4x4::Multiply4x4(client->rasCorrection, client->toolMatrix, client->toolMatrix);


  //Setting the button values in the matrix
  //The four button occupy the 1st column
  //The inkwell switch is at the top of the second column.
  client->buttonMatrix->SetElement(0, 0, 0);
  client->buttonMatrix->SetElement(1, 1, 0);
  client->buttonMatrix->SetElement(2, 2, 0);
  client->buttonMatrix->SetElement(0, 0, (bool)(buttonVals & HD_DEVICE_BUTTON_1));
  client->buttonMatrix->SetElement(1, 0, (bool)(buttonVals & HD_DEVICE_BUTTON_2));
  client->buttonMatrix->SetElement(2, 0, (bool)(buttonVals & HD_DEVICE_BUTTON_3));
  client->buttonMatrix->SetElement(3, 0, (bool)(buttonVals & HD_DEVICE_BUTTON_4));
  client->buttonMatrix->SetElement(0, 1, (int)inkwell);

  if(client->GetToolByPortName("Stylus", stylus) == PLUS_SUCCESS)
  {
    client->ToolTimeStampedUpdate(stylus->GetId(), client->toolMatrix, TOOL_OK, client->FrameNumber, unfilteredTimestamp);
  }

  if(client->GetToolByPortName("StylusVelocity", velocity) == PLUS_SUCCESS)
  {
    client->ToolTimeStampedUpdate(velocity->GetId(), client->velMatrix, TOOL_OK, client->FrameNumber, unfilteredTimestamp);
  }

  if(client->GetToolByPortName("Buttons", buttons) == PLUS_SUCCESS)
  {
    client->ToolTimeStampedUpdate(buttons->GetId(), client->buttonMatrix, TOOL_OK, client->FrameNumber, unfilteredTimestamp);
  }

  return HD_CALLBACK_DONE;
}

