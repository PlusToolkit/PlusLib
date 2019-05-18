/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "IntuitiveDaVinci.h"

//----------------------------------------------------------------------------
IntuitiveDaVinci::IntuitiveDaVinci()
  : mStatus(ISI_SUCCESS)
  , mConnected(false)
  , mStreaming(false)
  , mRateHz(60)
{
  mPsm1 = new IntuitiveDaVinciManipulator(ISI_PSM1);
  mPsm2 = new IntuitiveDaVinciManipulator(ISI_PSM2);
  mEcm  = new IntuitiveDaVinciManipulator(ISI_ECM);

  LOG_DEBUG("Created da Vinci.");
}

//----------------------------------------------------------------------------
IntuitiveDaVinci::~IntuitiveDaVinci()
{
  delete mPsm1, mPsm2, mEcm;
  mPsm1 = nullptr; mPsm2 = nullptr; mEcm = nullptr;

  stop();
  disconnect();

  LOG_DEBUG("Destroyed da Vinci.");
}

//----------------------------------------------------------------------------
bool IntuitiveDaVinci::start()
{
  LOG_DEBUG("Starting da Vinci.");
  if (isConnected()) 
    return true;

  LOG_ERROR("Not connected. Cannot start streaming.");
  return false;
}

//----------------------------------------------------------------------------
void IntuitiveDaVinci::stop()
{
  LOG_DEBUG("Stopping da Vinci.");
  if (isConnected())
    mConnected = false;
}

//----------------------------------------------------------------------------
ISI_STATUS IntuitiveDaVinci::connect()
{
  LOG_DEBUG("IntuiviveDaVinci::connect()");
  mStatus = dv_connect();
  mStatus = dv_subscribe_all_stream_fields();
  // mStatus = dv_start_stream(10);

  mStatus = ISI_SUCCESS;

  if(mStatus == ISI_SUCCESS)
  {
    mConnected = true;
	  LOG_INFO("Connected to da Vinci system.");
	  return mStatus;
  }
  LOG_ERROR("Could not connect to da Vinci system.");
  return mStatus;
}

//----------------------------------------------------------------------------
ISI_STATUS IntuitiveDaVinci::disconnect()
{
  // check if system is connected
  if (!this->mConnected)
  {
    LOG_ERROR("Cannot disconnect because not connected.");
    return ISI_FAIL;
  }

  stop();
  mConnected = false;
  return ISI_SUCCESS;
}

//----------------------------------------------------------------------------
void IntuitiveDaVinci::copyTransform(ISI_TRANSFORM* in, ISI_TRANSFORM* out)
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
void IntuitiveDaVinci::printTransform(const ISI_TRANSFORM* T)
{
  LOG_INFO("Position: " << T->pos.x << " " << T->pos.y << " "  << T->pos.z);
  LOG_INFO("X Axis Rotation: " << T->rot.row0.x << " " << T->rot.row1.x << " "  << T->rot.row2.x);
  LOG_INFO("Y Axis Rotation: " << T->rot.row0.y << " " << T->rot.row1.y << " "  << T->rot.row2.y);
  LOG_INFO("Z Axis Rotation: " << T->rot.row0.z << " " << T->rot.row1.z << " " << T->rot.row2.z);
}

//----------------------------------------------------------------------------
bool IntuitiveDaVinci::isConnected()
{
  return mConnected;
}

//----------------------------------------------------------------------------
bool IntuitiveDaVinci::isStreaming()
{
  return mStreaming;
}

//----------------------------------------------------------------------------
IntuitiveDaVinciManipulator* IntuitiveDaVinci::GetPsm1() 
{ 
  return this->mPsm1; 
}

//----------------------------------------------------------------------------
IntuitiveDaVinciManipulator* IntuitiveDaVinci::GetPsm2()
{
  return this->mPsm2;
}

//----------------------------------------------------------------------------
IntuitiveDaVinciManipulator* IntuitiveDaVinci::GetEcm()
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

void IntuitiveDaVinci::PrintAllJointValues()
{
  std::string tmp0 = this->mPsm1->GetJointValuesAsString();
  LOG_DEBUG("PSM1 Joint Values: " << tmp0);
  std::string tmp1 = this->mPsm2->GetJointValuesAsString();
  LOG_DEBUG("PSM2 Joint Values: " << tmp1);
  std::string tmp2 = this->mEcm->GetJointValuesAsString();
  LOG_DEBUG("ECM Joint Values: " << tmp2);
}

ISI_STATUS IntuitiveDaVinci::UpdateAllKinematicsTransforms()
{
  mStatus = this->mPsm1->UpdateKinematicsTransforms();
  if (mStatus != ISI_SUCCESS) return mStatus;

  mStatus = this->mPsm2->UpdateKinematicsTransforms();
  if (mStatus != ISI_SUCCESS) return mStatus;

  mStatus = this->mEcm->UpdateKinematicsTransforms();
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
