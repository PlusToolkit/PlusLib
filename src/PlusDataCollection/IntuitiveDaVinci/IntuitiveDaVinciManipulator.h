/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef _INTUITIVE_DAVINCI_MANIPULATOR_H_
#define _INTUITIVE_DAVINCI_MANIPULATOR_H_

#include "PlusConfigure.h"

// OS includes
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

// STL includes
#include <iostream>
#include <sstream>
#include <vector>

// Intuitive includes
#include <isi_api_types.h>
#include <dv_api.h>
#include <dv_api_math.h>

#define ISI_FAIL 0x0001 // IntuitiveDaVinci expects this for some reason, not provided by isi_types.h

class IntuitiveDaVinciManipulator
{
public:
  // Constructor
  IntuitiveDaVinciManipulator(ISI_MANIP_INDEX manipIndex);

  // Destructor
  ~IntuitiveDaVinciManipulator();

  ISI_TRANSFORM* GetTransforms();
  ISI_TRANSFORM* GetBaseToWorldTransform();
  ISI_FLOAT* GetJointValues();
  std::string GetJointValuesAsString();
  std::string GetDhTableAsString();
  std::string GetKinematicsTransformsAsString();

protected:
  ISI_MANIP_INDEX mManipIndex;
  int mNumJoints;
  ISI_DH_ROW* mDhTable;
  ISI_TRANSFORM* mBaseToWorld;
  ISI_TRANSFORM* mTransforms;
  ISI_FLOAT* mJointValues;

public:
  ISI_STATUS UpdateJointValues(); // Update just the joints
  ISI_STATUS UpdateKinematicsTransforms(); // Do forward kinematics

  ISI_STATUS SetDhTable(ISI_DH_ROW* srcDhTable);
  void CopyDhTable(ISI_DH_ROW* srcDhTable, ISI_DH_ROW* destDhTable);
};

#endif