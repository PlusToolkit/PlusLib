/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef _INTUITIVE_DAVINCI_MANIPULATOR_H_
#define _INTUITIVE_DAVINCI_MANIPULATOR_H_

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
#include <isi_api.h>
#include <isi_api_math.h>
#define ISI_FAIL 0x0001 // IntuitiveDaVinci expects this for some reason, not provided by isi_types.h

class IntuitiveDaVinciManipulator
{
public:
  // Constructor
  IntuitiveDaVinciManipulator(ISI_MANIP_INDEX manipIndex, ISI_DH_ROW* dhTable);

  // Destructor
  ~IntuitiveDaVinciManipulator();

  // Kinematics updates
  ISI_STATUS updateKinematics(); // Update everything

  ISI_TRANSFORM* getTransforms();
  ISI_FLOAT* getJointValues();

protected:
  ISI_MANIP_INDEX mManipIndex;
  ISI_FLOAT mNumJoints;
  ISI_DH_ROW* mDhTable;
  ISI_TRANSFORM* mTransforms;
  ISI_FLOAT* mJointValues;

protected:
  ISI_STATUS updateJointValues(); // Update just the joints
  ISI_STATUS updateKinematicsTransforms(); // Do forward kinematics
};

#endif