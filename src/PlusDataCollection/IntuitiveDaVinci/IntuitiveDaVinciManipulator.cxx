/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "IntuitiveDaVinciManipulator.h"

IntuitiveDaVinciManipulator::IntuitiveDaVinciManipulator(ISI_MANIP_INDEX manipIndex)
  : mManipIndex(manipIndex)
{
  if(mManipIndex == ISI_PSM1 || mManipIndex == ISI_PSM2) 
  {
    mNumJoints = (int)ISI_NUM_PSM_JOINTS;
  }
  else mNumJoints = (int)ISI_NUM_ECM_JOINTS;
  
  mDhTable = new ISI_DH_ROW[7];
  mBaseToWorld = new ISI_TRANSFORM;
  mTransforms = new ISI_TRANSFORM[7];
  mJointValues = new ISI_FLOAT[mNumJoints];
}

IntuitiveDaVinciManipulator::~IntuitiveDaVinciManipulator()
{
  delete[] mDhTable;
  delete mBaseToWorld;
  delete[] mTransforms;
  delete[] mJointValues;

  mDhTable = nullptr; mBaseToWorld = nullptr; mTransforms = nullptr; mJointValues = nullptr;
}

ISI_STATUS IntuitiveDaVinciManipulator::UpdateJointValues()
{
  ISI_STREAM_FIELD streamData;
  ISI_STATUS status;
  status = dv_get_stream_field(mManipIndex, ISI_JOINT_VALUES, &(streamData));

  if (status == ISI_SUCCESS)
  {
    for (int iii = 0; iii < mNumJoints; iii++)
    {
      mJointValues[iii] = streamData.data[iii];
    }
  }

  return status;
}

std::string IntuitiveDaVinciManipulator::GetJointValuesAsString()
{
  std::stringstream str;
  for (int iii = 0; iii < mNumJoints; iii++)
  {
    str << mJointValues[iii];
  }

  return str.str();
}

std::string IntuitiveDaVinciManipulator::GetDhTableAsString()
{
  std::stringstream str;
  for (int iii = 0; iii < 7; iii++)
  {
    str << mDhTable[iii].type << ' ';
    str << mDhTable[iii].l << ' ';
    str << mDhTable[iii].sina << ' ';
    str << mDhTable[iii].cosa << ' ';
    str << mDhTable[iii].d << ' ';
    str << mDhTable[iii].sinq << ' ';
    str << mDhTable[iii].cosq << ' ';
    str << '\n';
  }

  return str.str();
}

ISI_STATUS IntuitiveDaVinciManipulator::SetDhTable(ISI_DH_ROW* srcDhTable)
{
  CopyDhTable(srcDhTable, mDhTable);
  return ISI_SUCCESS;
}

void IntuitiveDaVinciManipulator::CopyDhTable(ISI_DH_ROW* srcDhTable, ISI_DH_ROW* destDhTable)
{
  for (int iii = 0; iii < 7; iii++)
  {
    destDhTable[iii].cosa = srcDhTable[iii].cosa;
    destDhTable[iii].cosq = srcDhTable[iii].cosq;
    destDhTable[iii].d = srcDhTable[iii].d;
    destDhTable[iii].l = srcDhTable[iii].l;
    destDhTable[iii].sina = srcDhTable[iii].sina;
    destDhTable[iii].sinq = srcDhTable[iii].sinq;
    destDhTable[iii].type = srcDhTable[iii].type;
  }
}

ISI_STATUS IntuitiveDaVinciManipulator::UpdateKinematicsTransforms()
{
  ISI_FLOAT garbageFloat[8*6]; // We don't really care about Jacobians
  ISI_TRANSFORM base = dv_identity_transform(); // Compute relative to identity

  ISI_STATUS status = ISI_SUCCESS;
  for (int iii = 0; iii < 7; iii++)
  {
    status += dv_dh_forward_kinematics(
      &(base), iii + 1, mDhTable, mJointValues, 
      &(mTransforms[iii]), garbageFloat);
  }

  if (status != ISI_SUCCESS)
  {
    LOG_ERROR("Could not run DH forward kinematics.");
    return ISI_FAIL;
  }

  return ISI_SUCCESS;
}