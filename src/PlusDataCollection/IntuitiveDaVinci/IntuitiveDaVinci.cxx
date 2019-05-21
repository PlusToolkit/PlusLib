/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "IntuitiveDaVinci.h"
#include <cmath>
#include <time.h>

//----------------------------------------------------------------------------
IntuitiveDaVinci::IntuitiveDaVinci()
  : mStatus(ISI_SUCCESS)
  , mConnected(false)
  , mStreaming(false)
  , mRateHz(60)
{
  mPsm1 = new IntuitiveDaVinciManipulator(ISI_PSM1);
  mPsm2 = new IntuitiveDaVinciManipulator(ISI_PSM2);
  mEcm = new IntuitiveDaVinciManipulator(ISI_ECM);

  mPsm1BaseToWorld = new ISI_TRANSFORM;
  mPsm2BaseToWorld = new ISI_TRANSFORM;
  mEcmBaseToWorld = new ISI_TRANSFORM;

  mViewToWorld = new ISI_TRANSFORM;
  mPsm1BaseToView = new ISI_TRANSFORM;
  mPsm2BaseToView = new ISI_TRANSFORM;

  LOG_DEBUG("Created da Vinci.");
}

//----------------------------------------------------------------------------
IntuitiveDaVinci::~IntuitiveDaVinci()
{
  delete mPsm1, mPsm2, mEcm;
  mPsm1 = nullptr; mPsm2 = nullptr; mEcm = nullptr;

  delete mPsm1BaseToWorld, mPsm2BaseToWorld, mEcmBaseToWorld;
  mPsm1BaseToWorld = nullptr; mPsm2BaseToWorld = nullptr; mEcmBaseToWorld = nullptr;

  delete mViewToWorld, mPsm1BaseToView, mPsm2BaseToView;
  mViewToWorld = nullptr; mPsm1BaseToView = nullptr; mPsm2BaseToView = nullptr;

  this->Stop();
  this->Disconnect();

  LOG_DEBUG("Destroyed da Vinci.");
}

//----------------------------------------------------------------------------
ISI_STATUS IntuitiveDaVinci::Start()
{
  LOG_DEBUG("Starting data stream from da Vinci API.");

  if (!this->IsConnected())
  {
    LOG_ERROR("Not connected. Cannot start streaming.");
    return mStatus;
  }
  
  mStatus = dv_subscribe_all_stream_fields();
  mStatus = dv_start_stream(mRateHz);

  if (mStatus != ISI_SUCCESS)
  {
    LOG_ERROR("Could not start da Vinci data stream.");
    return mStatus;
  }

  mStreaming = true;
  LOG_DEBUG("Data stream started.");
  return mStatus;
}

ISI_STATUS IntuitiveDaVinci::StartDebugSineWaveMode()
{
  LOG_DEBUG("Started sine wave debug data stream.");
  mStreaming = true;
  return mStatus;
}

//----------------------------------------------------------------------------
void IntuitiveDaVinci::Stop()
{
  LOG_DEBUG("Stopping da Vinci API.");

  if (this->IsConnected())
  {
    LOG_DEBUG("Cannot stop stream until disconnected. Disconnecting now.");
    this->Disconnect();
  }

  if (!this->IsStreaming())
  {
    LOG_DEBUG("Stop called, but da Vinci is already stopped.");
  }

  dv_stop_stream();

  mStreaming = false;
  LOG_DEBUG("Streaming from the da Vinci API stopped.");
}

//----------------------------------------------------------------------------
ISI_STATUS IntuitiveDaVinci::Connect()
{
  LOG_DEBUG("Connecting to da Vinci API.");
  mStatus = dv_connect();

  if(mStatus != ISI_SUCCESS)
  {
    LOG_ERROR("Could not connect to da Vinci system.");
	  return mStatus;
  }
  
  mConnected = true;
  LOG_INFO("Connected to da Vinci system.");
  return mStatus;
}

ISI_STATUS IntuitiveDaVinci::ConnectDebugSineWaveMode()
{
  LOG_DEBUG("Connected to a debug version of the da Vinci API.")
  mConnected = true;
  return mStatus;
}

//----------------------------------------------------------------------------
void IntuitiveDaVinci::Disconnect()
{
  // check if system is connected
  if (!this->mConnected)
  {
    LOG_WARNING("Disconnect not called because not connected.");
    return mStatus;
  }

  dv_disconnect();
  mConnected = false;

  LOG_DEBUG("Disconnected from the da Vinci API.")
}

//----------------------------------------------------------------------------
void IntuitiveDaVinci::CopyTransform(ISI_TRANSFORM* in, ISI_TRANSFORM* out)
{
  if (in == NULL || out == NULL) { return; }

  out->pos.x = in->pos.x;
  out->pos.y = in->pos.y;
  out->pos.z = in->pos.z;

  out->rot.row0 = in->rot.row0;
  out->rot.row1 = in->rot.row1;
  out->rot.row2 = in->rot.row2;
}

//----------------------------------------------------------------------------
void IntuitiveDaVinci::PrintTransform(const ISI_TRANSFORM* T)
{
  LOG_INFO("Position: " << T->pos.x << " " << T->pos.y << " "  << T->pos.z);
  LOG_INFO("X Axis Rotation: " << T->rot.row0.x << " " << T->rot.row1.x << " "  << T->rot.row2.x);
  LOG_INFO("Y Axis Rotation: " << T->rot.row0.y << " " << T->rot.row1.y << " "  << T->rot.row2.y);
  LOG_INFO("Z Axis Rotation: " << T->rot.row0.z << " " << T->rot.row1.z << " " << T->rot.row2.z);
}

//----------------------------------------------------------------------------
bool IntuitiveDaVinci::IsConnected() const
{
  return mConnected;
}

//----------------------------------------------------------------------------
bool IntuitiveDaVinci::IsStreaming() const
{
  return mStreaming;
}

//----------------------------------------------------------------------------
IntuitiveDaVinciManipulator* IntuitiveDaVinci::GetPsm1() const
{ 
  return this->mPsm1; 
}

//----------------------------------------------------------------------------
IntuitiveDaVinciManipulator* IntuitiveDaVinci::GetPsm2() const
{
  return this->mPsm2;
}

//----------------------------------------------------------------------------
IntuitiveDaVinciManipulator* IntuitiveDaVinci::GetEcm() const
{
  return this->mEcm;
}

ISI_STATUS IntuitiveDaVinci::UpdateAllJointValues()
{
  mStatus = this->mPsm1->UpdateJointValues();
  if (mStatus != ISI_SUCCESS) return mStatus;

  mStatus = this->mPsm2->UpdateJointValues();
  if (mStatus != ISI_SUCCESS) return mStatus;

  mStatus = this->mEcm->UpdateJointValues();
  return mStatus;
}

ISI_STATUS IntuitiveDaVinci::UpdateAllJointValuesSineWave()
{
  clock_t ticks = clock();
  float t = ((float)ticks) / ((float)CLOCKS_PER_SEC);

  ISI_FLOAT psm1JointValues[ISI_NUM_PSM_JOINTS] = { 0.5*sin(1.0*t), 0.5*sin(1.5*t), 50.0*sin(2.0*t) + 75.0, sin(1.7*t), sin(0.7*t), sin(0.5*t), sin(0.8*t) };
  ISI_FLOAT psm2JointValues[ISI_NUM_PSM_JOINTS] = { 0.5*sin(1.1*t), 0.5*sin(1.4*t), 50.0*sin(2.1*t) + 75.0, sin(1.6*t), sin(0.6*t), sin(0.9*t), sin(1.8*t) };
  ISI_FLOAT ecmJointValues[ISI_NUM_ECM_JOINTS] = { 0.5*sin(0.9*t), 0.5*sin(1.3*t), 50.0*sin(1.3*t) + 75.0, sin(1.1*t)};

  this->mPsm1->SetJointValues(psm1JointValues);
  this->mPsm2->SetJointValues(psm2JointValues);
  this->mEcm->SetJointValues(ecmJointValues);

  return ISI_SUCCESS;
}

void IntuitiveDaVinci::PrintAllJointValues()
{
  std::string tmp0 = this->mPsm1->GetJointValuesAsString();
  LOG_DEBUG("PSM1 Joint Values: " << tmp0);
  std::string tmp1 = this->mPsm2->GetJointValuesAsString();
  LOG_DEBUG("PSM2 Joint Values: " << tmp1);
  std::string tmp2 = this->mEcm->GetJointValuesAsString();
  LOG_DEBUG("ECM Joint Values: " << tmp2);
}

ISI_STATUS IntuitiveDaVinci::UpdateBaseToWorldTransforms()
{
  ISI_STATUS status;

  // Update the ECM base frame first
  status = dv_get_reference_frame(ISI_ECM, ISI_BASE_FRAME, mEcmBaseToWorld);
  mEcmBaseToWorld->pos.x *= 1000.0;
  mEcmBaseToWorld->pos.y *= 1000.0;
  mEcmBaseToWorld->pos.z *= 1000.0;

  // Get endoscope view to base transform
  ISI_TRANSFORM* viewToEcmBase = GetEcm()->GetTransforms() + 6;

  // Multiply to get the endoscope view to world transform
  dv_mult_transforms(mEcmBaseToWorld, viewToEcmBase, mViewToWorld);

  // Get the psm1basetoview transforms
  status += dv_get_reference_frame(ISI_PSM1, ISI_BASE_FRAME, mPsm1BaseToView);
  mPsm1BaseToView->pos.x *= 1000.0;
  mPsm1BaseToView->pos.y *= 1000.0;
  mPsm1BaseToView->pos.z *= 1000.0;

  status += dv_get_reference_frame(ISI_PSM2, ISI_BASE_FRAME, mPsm2BaseToView);
  mPsm2BaseToView->pos.x *= 1000.0;
  mPsm2BaseToView->pos.y *= 1000.0;
  mPsm2BaseToView->pos.z *= 1000.0;

  // Compute them relative to world
  dv_mult_transforms(mViewToWorld, mPsm1BaseToView, mPsm1BaseToWorld);
  dv_mult_transforms(mViewToWorld, mPsm2BaseToView, mPsm2BaseToWorld);

  if (status != ISI_SUCCESS)
  {
    LOG_ERROR("Error getting the base frames from the ISI API.");
  }

  return status;
}

ISI_STATUS IntuitiveDaVinci::UpdateAllKinematicsTransforms()
{
  mStatus = UpdateBaseToWorldTransforms();

  mStatus += this->mPsm1->UpdateAllManipulatorTransforms();
  mStatus += this->mPsm2->UpdateAllManipulatorTransforms();
  mStatus += this->mEcm->UpdateAllManipulatorTransforms();

  if (mStatus != ISI_SUCCESS)
    LOG_ERROR("Could not update the manipulator transforms.");

  return mStatus;
}

ISI_STATUS IntuitiveDaVinci::UpdateMinimalKinematicsTransforms()
{
  mStatus = UpdateBaseToWorldTransforms();

  mStatus += this->mPsm1->UpdateMinimalManipulatorTransforms();
  mStatus += this->mPsm2->UpdateMinimalManipulatorTransforms();
  mStatus += this->mEcm->UpdateMinimalManipulatorTransforms();

  if (mStatus != ISI_SUCCESS)
    LOG_ERROR("Could not update minimal manipulator transforms.");

  return mStatus;
}

void IntuitiveDaVinci::PrintAllKinematicsTransforms()
{
  std::string tmp0 = this->mPsm1->GetKinematicsTransformsAsString();
  LOG_DEBUG("PSM1 Kinematics Transforms: " << tmp0);
  std::string tmp1 = this->mPsm2->GetKinematicsTransformsAsString();
  LOG_DEBUG("PSM2 Kinematics Transforms: " << tmp1);
  std::string tmp2 = this->mEcm->GetKinematicsTransformsAsString();
  LOG_DEBUG("ECM Kinematics Transforms: " << tmp2);
}

ISI_TRANSFORM* IntuitiveDaVinci::GetPsm1BaseToWorld()
{
  return mPsm1BaseToWorld;
}

ISI_TRANSFORM* IntuitiveDaVinci::GetPsm2BaseToWorld()
{
  return mPsm2BaseToWorld;
}

ISI_TRANSFORM* IntuitiveDaVinci::GetEcmBaseToWorld()
{
  return mEcmBaseToWorld;
}