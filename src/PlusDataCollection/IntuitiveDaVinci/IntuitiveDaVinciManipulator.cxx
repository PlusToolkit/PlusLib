/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/
#include <sstream>

#include "PlusConfigure.h"

#include "dv_api.h"
#include "dv_api_math.h"

#include "IntuitiveDaVinciManipulator.h"

//----------------------------------------------------------------------------
IntuitiveDaVinciManipulator::IntuitiveDaVinciManipulator(ISI_MANIP_INDEX manipIndex)
  : mManipIndex(manipIndex)
{
  if(mManipIndex == ISI_PSM1 || mManipIndex == ISI_PSM2) 
  {
    mNumJoints = (int)ISI_NUM_PSM_JOINTS;
  }
  else mNumJoints = (int)ISI_NUM_ECM_JOINTS;
  
  mDhTable = new ISI_DH_ROW[7];
  mTransforms = new ISI_TRANSFORM[7];
  mJointValues = new ISI_FLOAT[mNumJoints];

  LOG_DEBUG("Created da Vinci manipulator.");
}

//----------------------------------------------------------------------------
IntuitiveDaVinciManipulator::~IntuitiveDaVinciManipulator()
{
  delete[] mDhTable;
  delete[] mTransforms;
  delete[] mJointValues;

  mDhTable = nullptr; mTransforms = nullptr; mJointValues = nullptr;

  LOG_DEBUG("Destroyed da Vinci manipulator.");
}

//----------------------------------------------------------------------------
ISI_STATUS IntuitiveDaVinciManipulator::UpdateJointValues()
{
  LOG_DEBUG("Updating manipulator joint values.");

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
  else
    LOG_ERROR("Could not update the manipulator joint values.");

  return status;
}

//----------------------------------------------------------------------------
std::string IntuitiveDaVinciManipulator::GetJointValuesAsString() const
{
  std::stringstream str;
  for (int iii = 0; iii < mNumJoints; iii++)
  {
    str << mJointValues[iii];
  }

  return str.str();
}

//----------------------------------------------------------------------------
std::string IntuitiveDaVinciManipulator::GetDhTableAsString() const
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

//----------------------------------------------------------------------------
void IntuitiveDaVinciManipulator::SetDhTable(ISI_DH_ROW* srcDhTable)
{
  CopyDhTable(srcDhTable, mDhTable);
}

//----------------------------------------------------------------------------
void IntuitiveDaVinciManipulator::SetJointValues(ISI_FLOAT* jointValues)
{
  for (int iii = 0; iii < mNumJoints; iii++)
  {
    mJointValues[iii] = jointValues[iii];
  }
}

//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
ISI_STATUS IntuitiveDaVinciManipulator::UpdateAllManipulatorTransforms()
{
  ISI_TRANSFORM base = dv_identity_transform(); // Compute relative to identity

  ISI_STATUS status = ISI_SUCCESS;
  for (int iii = 0; iii < 7; iii++)
  {
    status += dv_dh_forward_kinematics(
      &(base), iii + 1, mDhTable, mJointValues, 
      &(mTransforms[iii]), NULL);
  }

  if (status != ISI_SUCCESS)
  {
    LOG_ERROR("Could not run DH forward kinematics.");
  }

  return status;
}

//----------------------------------------------------------------------------
ISI_STATUS IntuitiveDaVinciManipulator::UpdateMinimalManipulatorTransforms()
{
  ISI_TRANSFORM base = dv_identity_transform(); // Compute relative to identity

  ISI_STATUS status = ISI_SUCCESS;

  int numMinimalTransforms;
  int minimalLinkIndices[4] = { 7, 4, 5, 6 };

  if (mManipIndex == ISI_ECM)
    numMinimalTransforms = 1; // Just the tip transform for the ECM
  else
    numMinimalTransforms = 4; // Tip transform and the required link transforms for rendering model motion

  int transformIndexToUpdate;

  // This will only update the first transform if its an ECM, therefore only 7 will be updated.
  for (int iii = 0; iii < numMinimalTransforms; iii++)
  {
    transformIndexToUpdate = minimalLinkIndices[iii];
    status += dv_dh_forward_kinematics(
      &(base), transformIndexToUpdate, mDhTable, mJointValues,
      &(mTransforms[transformIndexToUpdate - 1]), NULL);
  }

  if (status != ISI_SUCCESS)
  {
    LOG_ERROR("Could not run DH forward kinematics.");
  }

  return status;
}

//----------------------------------------------------------------------------
static std::string GetTransformAsString(const ISI_TRANSFORM& t)
{
  std::stringstream str;
  str << t.rot.row0.x << ' ' << t.rot.row0.y << ' '
    << t.rot.row0.z << ' ' << t.pos.x << '\n';
  str << t.rot.row1.x << ' ' << t.rot.row1.y << ' '
    << t.rot.row1.z << ' ' << t.pos.y << '\n';
  str << t.rot.row2.x << ' ' << t.rot.row2.y << ' '
    << t.rot.row2.z << ' ' << t.pos.z << '\n';
  str << "0 0 0 1";

  return str.str();
}

//----------------------------------------------------------------------------
std::string IntuitiveDaVinciManipulator::GetTransformsAsString() const
{
  std::stringstream str;

  for (int iii = 0; iii < 7; iii++)
  {
    str << "Frame" << iii + 1 << "ToBase\n";
    str << GetTransformAsString(mTransforms[iii])<< '\n';
  }

  return str.str();
}

//----------------------------------------------------------------------------
ISI_TRANSFORM* IntuitiveDaVinciManipulator::GetTransforms() const
{
  return mTransforms;
}

//----------------------------------------------------------------------------
ISI_FLOAT* IntuitiveDaVinciManipulator::GetJointValues() const
{
  return mJointValues;
}