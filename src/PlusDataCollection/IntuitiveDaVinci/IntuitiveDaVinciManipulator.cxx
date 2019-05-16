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
    mNumJoints = ISI_NUM_PSM_JOINTS;
  }
  else mNumJoints = ISI_NUM_ECM_JOINTS;
  
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

void IntuitiveDaVinciManipulator::PrintJointValues()
{
  std::stringstream str;
  for (int iii = 0; iii < mNumJoints; iii++)
  {
    str << mJointValues[iii];
  }

  std::cout << str.str() << std::endl;
}

ISI_STATUS IntuitiveDaVinciManipulator::SetDhTable(ISI_DH_ROW* srcDhTable)
{
  return ISI_SUCCESS;
}

ISI_STATUS IntuitiveDaVinciManipulator::UpdateKinematicsTransforms()
{
  return ISI_SUCCESS;
}