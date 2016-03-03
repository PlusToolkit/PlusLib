/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Chris Wedlake <cwedlake@imaging.robarts.ca>. All rights reserved.
See License.txt for details.

Date:      $Date: 2007/04/19 12:48:53 $
Version:   $Revision: 1.1 $
=========================================================Plus=header=end*/

#include "vtkFreedom6SDevice.h"

#include "vtkForceFeedback.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkTransform.h"

#include <vector>

#include "f6s.h"
#include "freedom6s.h"

namespace
{
static const double aHeart = 0.25;
static const double MAX_FORCE = 0.6;
static const int POS2MM = 1000;
}

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkFreedom6SDevice);

//----------------------------------------------------------------------------
vtkFreedom6SDevice::vtkFreedom6SDevice()
  : FreedomAPI(new Freedom6S())
  , TimerPeriod(1000)
  , ForceModel(vtkHapticForce::New())
  , SendMatrix(vtkMatrix4x4::New())
  , UseForce(true)
  , FrameCount(0)
  , CurrentFrame(0)
  , NextFrame(1)
  , Scale(1.0)
{
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkFreedom6SDevice::~vtkFreedom6SDevice()
{
  this->ForceModel->Delete();
  this->SendMatrix->Delete();
  delete FreedomAPI;
  FreedomAPI = NULL;
}

//----------------------------------------------------------------------------
void vtkFreedom6SDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent.GetNextIndent());

  os << indent << "Force Model: ";
  ForceModel->PrintSelf(os,indent.GetNextIndent());

  if (this->SerialNumber != NULL)
  {
    os << indent << "Serial Number: " << this->SerialNumber << endl;
  }
  else
  {
    os << indent << "Serial Number: (NULL)" << endl;
  }

  if (this->LeftHanded != NULL)
  {
    os << indent << "Is Left Handed: " << this->LeftHanded << endl;
  }
  else
  {
    os << indent << "Is Left Handed: (NULL)" << endl;
  }

  if (this->Version != NULL)
  {
    os << indent << "Device Version: " << this->Version << endl;
  }
  else
  {
    os << indent << "Device Version: (NULL)" << endl;
  }

  if (this->VersionDate != NULL)
  {
    os << indent << "Device Version Date: " << this->VersionDate << endl;
  }
  else
  {
    os << indent << "Device Version Date: (NULL)" << endl;
  }

  os << indent << "Use Force: " << (this->UseForce ? "Yes" : "No") << endl;

}

//----------------------------------------------------------------------------
PlusStatus vtkFreedom6SDevice::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkFreedom6SDevice::ReadConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, Scale, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(UseForce, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, TimerPeriod, deviceConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkFreedom6SDevice::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkFreedom6SDevice::WriteConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  deviceConfig->SetDoubleAttribute("Scale", this->Scale);
  deviceConfig->SetIntAttribute("TimerPeriod", this->TimerPeriod);
  XML_WRITE_BOOL_ATTRIBUTE(UseForce, deviceConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkFreedom6SDevice::Probe()
{
  if( this->FreedomAPI != NULL )
  {
    return this->FreedomAPI->GetPowerStatus() == 1 ? PLUS_SUCCESS : PLUS_FAIL;
  }
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkFreedom6SDevice::InternalConnect()
{
  if (!this->FreedomAPI->Initialize())
  {
    LOG_ERROR("Error initializing Freedom6S library: " << FreedomAPI->GetLastError());
    return PLUS_FAIL;
  }
  if (this->FreedomAPI->GetLastError() < 0)
  {
    LOG_ERROR("F6S Error: " << this->FreedomAPI->GetLastErrorString());
    return PLUS_FAIL;
  }
  this->FreedomAPI->ComputeJointVel(0.001,20);
  if (this->FreedomAPI->GetLastError() < 0)
  {
    LOG_ERROR("F6S Error: " << this->FreedomAPI->GetLastErrorString());
    return PLUS_FAIL;
  }
  this->FreedomAPI->SetHoldDist(0.03f);
  if (this->FreedomAPI->GetLastError() < 0)
  {
    LOG_ERROR("F6S Error: " << this->FreedomAPI->GetLastErrorString());
    return PLUS_FAIL;
  }

  F6sDeviceInfo *pDevInfo;
  this->FreedomAPI->GetDeviceInfo(&pDevInfo);
  strcpy_s(SerialNumber, 75, pDevInfo->m_strSerialNo);
  strcpy_s(LeftHanded, 20, (pDevInfo->m_bLeftDevice ? "yes" : "no"));

  this->FrameCount = 0;
  this->CurrentFrame = 0;
  this->NextFrame = 1;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkFreedom6SDevice::InternalDisconnect()
{
  this->FreedomAPI->Cleanup();
  return this->FreedomAPI->GetLastError() == F6SRC_NOERROR ? PLUS_SUCCESS : PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkFreedom6SDevice::InternalStartRecording()
{
  this->FreedomAPI->SetTimer(this->TimerPeriod);
  this->GetVersion();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkFreedom6SDevice::InternalStopRecording()
{
  this->FreedomAPI->KillTimer();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkFreedom6SDevice::InternalUpdate()
{
  if( !this->IsRecording() )
  {
    return PLUS_SUCCESS;
  }

  vtkPlusDataSource* tool;
  if( this->GetFirstActiveTool(tool) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to find tool. Is Freedom6S correctly configured?");
    return PLUS_FAIL;
  }

  // what does this do?
  this->FreedomAPI->SleepTimer();

  this->FrameCount = this->FrameCount + 1;

  this->FreedomAPI->UpdateKinematics();
  double matrix[16];
  this->FreedomAPI->GetPositionMatrixGL(matrix);  // transform from handle to GL
  matrix[12] *= POS2MM*this->Scale; //x
  matrix[13] *= POS2MM*this->Scale; //y
  matrix[14] *= POS2MM*this->Scale; //z

  this->SendMatrix->DeepCopy(matrix);
  this->SendMatrix->Transpose();

  // force-feedback
  Vector3 force(0,0,0);
  Vector3 torque(0,0,0);
  if (UseForce)
  {
    Vector3 linearVec, angularVec;
    this->FreedomAPI->GetVelocity(linearVec, angularVec);

    this->ComputeForceTorque(force, torque);
  }

  // Output to device
  this->FreedomAPI->SetForceTorque(force, torque);

  TrackedFrame::FieldMapType customFields;
  std::ostringstream ss;
  ss << force[0] << " " << force[1] << " " << force[2];
  customFields["UseForce"] = UseForce ? "True" : "False";
  customFields["Force"] = ss.str().c_str();
  ss.clear();
  ss << torque[0] << " " << torque[1] << " " << torque[2];
  customFields["Torque"] = ss.str().c_str();

  return this->ToolTimeStampedUpdate(tool->GetSourceId(), this->SendMatrix, TOOL_OK, this->FrameCount, UNDEFINED_TIMESTAMP, &customFields);
}

//----------------------------------------------------------------------------
PlusStatus vtkFreedom6SDevice::NotifyConfigured()
{
  if( this->OutputChannels.size() != 1 )
  {
    LOG_ERROR("Freedom6S device needs exactly one output channel defined.");
    return PLUS_FAIL;
  }

  if( this->OutputChannels[0]->ToolCount() != 1 )
  {
    LOG_ERROR("Freedom6S needs exactly one tool defined.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//--------------------------------------------------------------------------------------
void vtkFreedom6SDevice::ComputeForceTorque(Vector3& force, Vector3& torque)
{
  if (this->ForceModel->GetNumberOfFrames() == 0 )
  {
    return;
  }

  if ( this->SendMatrix == NULL )
  {
    return;
  }
  double forceSP[3] = {0,0,0};
  int i;
  Vector3 pHome;
  Vector3 pEE(0,0,0);
  int flag = 0;
  double forceSP1[3] = {0,0,0}, forceSP2[3]= {0,0,0};

  if (this->ForceModel->GetNumberOfFrames() <=0)
  {
    return;
  }

  // SHOULD BE THE CURRENT VOLUME INFORMATION
  int dispPeriod = ForceModel->GetNumberOfFrames();


  for(i=0; i<3; i++)
  {
    force[i] = 0;
    torque[i] = 0;
  }

  Vector3 p = FreedomAPI->TransformPointToHome(pEE);

  for(i=0; i<3; i++)
  {
    pHome[i] = p[i]*POS2MM*this->Scale;
  }

  double pointSP[3] = {pHome[0]/aHeart, pHome[1]/aHeart, pHome[2]/aHeart};

  this->CurrentFrame = (NextFrame % ForceModel->GetNumberOfFrames());
  this->NextFrame = (CurrentFrame + 1) % ForceModel->GetNumberOfFrames();

  this->ForceModel->GetForceModel(this->CurrentFrame)->GenerateForce(this->SendMatrix, forceSP1);
  flag = this->ForceModel->GetForceModel(this->NextFrame)->GenerateForce(this->SendMatrix, forceSP2);

  double alpha = CurrentFrame/dispPeriod;
  for(i=0; i<3; i++)
  {
    forceSP[i] = (1-alpha)*forceSP1[i] + alpha*forceSP2[i];
  }

  force[0] = forceSP[0];
  force[1] = -forceSP[2];
  force[2] = forceSP[1];

  double forceMag = sqrt(force[0]*force[0] + force[1]*force[1] + force[2]*force[2]);

  // Limit the maximum force
  if(forceMag >= MAX_FORCE)
  {
    force.normalize();
    force = force*MAX_FORCE;
  }
  return;
}

//----------------------------------------------------------------------------
int vtkFreedom6SDevice::GetVersion()
{
  this->FreedomAPI->GetVersion(this->Version, FREEDOM6S_STRING_SIZE, this->VersionDate, FREEDOM6S_STRING_SIZE);
  return this->FreedomAPI->GetLastError();
}