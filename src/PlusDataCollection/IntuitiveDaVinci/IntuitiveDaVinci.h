/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef _INTUITIVE_DAVINCI_H_
#define _INTUITIVE_DAVINCI_H_

#ifdef WIN32
  #define _CRT_SECURE_NO_WARNINGS
  #include <windows.h>
#endif

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

class IntuitiveDaVinci
{
public:
  // Constructor
  IntuitiveDaVinci();

  // Destructor
  ~IntuitiveDaVinci();

  // Start streaming from the da Vinci
  bool start();

  // Stop streaming from the da Vinci
  void stop();

  // Make a request to connect to the da Vinci
  ISI_STATUS connect();

  // Make a request to disconnect from the da Vinci
  ISI_STATUS disconnect();

  // Print out the 6DOF from the given transform.
  void printTransform(const ISI_TRANSFORM* T);

  // Added. Accessor for connected state.
  bool isConnected();
  bool isStreaming();
  
  //IntuitiveDaVinciManipulator* getPsm1();
  //IntuitiveDaVinciManipulator* getPsm2();
  //IntuitiveDaVinciManipulator* getEcm();

protected:
  void copyTransform(ISI_TRANSFORM* in, ISI_TRANSFORM* out);

  // Update joints
  //ISI_STATUS updateAllJointValues();

  // Run kinematics
  //ISI_STATUS updateAllKinematicsTransforms();

protected:
  ISI_STATUS        mStatus;
  bool              mConnected;
  bool              mStreaming;

  unsigned int      mRateHz;

  //IntuitiveDaVinciManipulator mPsm1;
  //IntuitiveDaVinciManipulator mPsm2;
  //IntuitiveDaVinciManipulator mEcm;
};

#endif
