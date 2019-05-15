/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "IntuitiveDaVinciManipulator.h"

IntuitiveDaVinciManipulator::IntuitiveDaVinciManipulator(ISI_MANIP_INDEX manipIndex, ISI_DH_ROW* dhTable)
  : mManipIndex(manipIndex)
{
  if(mManipIndex == ISI_PSM1 || mManipIndex == ISI_PSM2) 
  {
    mNumJoints = ISI_NUM_PSM_JOINTS;
  }
  else mNumJoints = ISI_NUM_ECM_JOINTS;

   
}