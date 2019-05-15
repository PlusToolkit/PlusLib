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
  //mPsm1 = IntuitiveDaVinciManipulator(ISI_PSM1);
  //mPsm2 = IntuitiveDaVinciManipulator(ISI_PSM2);
  //mEcm  = IntuitiveDaVinciManipulator(ISI_ECM);
}

//----------------------------------------------------------------------------
IntuitiveDaVinci::~IntuitiveDaVinci()
{
  stop();
  disconnect();
}

//----------------------------------------------------------------------------
bool IntuitiveDaVinci::start()
{
  if (isConnected()) 
    return true;

  LOG_ERROR("Not connected. Cannot start streaming.");
  return false;
}

//----------------------------------------------------------------------------
void IntuitiveDaVinci::stop()
{
  if (isConnected())
    mConnected = false;
}

//----------------------------------------------------------------------------
ISI_STATUS IntuitiveDaVinci::connect()
{
  mStatus = dv_connect();
  // mStatus = DV_SUCCESS;

  if(mStatus == ISI_SUCCESS)
  {
    mConnected = true;
  }
  return mStatus;
}

//----------------------------------------------------------------------------
ISI_STATUS IntuitiveDaVinci::disconnect()
{
  // check if system is connected
  if (!this->mConnected)
  {
    LOG_ERROR("disconnect: not connected.");
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

bool IntuitiveDaVinci::isStreaming()
{
  return mStreaming;
}
