/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef _INTUITIVE_DAVINCI_H_
#define _INTUITIVE_DAVINCI_H_

#include "PlusConfigure.h"
#include "IntuitiveDaVinciManipulator.h"

#include "isi_api_types.h"

class IntuitiveDaVinci
{
public:
  /*! Constructor. */
  IntuitiveDaVinci();

  /*! Destructor. */
  ~IntuitiveDaVinci();

  /*! Start streaming from the da Vinci API system */
  ISI_STATUS Start();
  ISI_STATUS StartDebugSineWaveMode();

  /*! Stop streaming */
  void Stop();

  /*! Make a request to connect to the da Vinci */
  ISI_STATUS Connect();
  ISI_STATUS ConnectDebugSineWaveMode();

  /*! Make a request to disconnect from the da Vinci */
  void Disconnect();

  /*! Accessor for connected state. */
  bool IsConnected() const;

  /*! Accessor for streaming state. */
  bool IsStreaming() const;
  
  /*! Accessor for each of the manipulators. */
  IntuitiveDaVinciManipulator* GetPsm1() const;
  IntuitiveDaVinciManipulator* GetPsm2() const;
  IntuitiveDaVinciManipulator* GetEcm() const;

  /*! Accessor for each of the manipulator base transforms. */
  ISI_TRANSFORM* GetPsm1BaseToWorld() const;
  ISI_TRANSFORM* GetPsm2BaseToWorld() const;
  ISI_TRANSFORM* GetEcmBaseToWorld() const;

  /*! Update joint values using the da Vinci API for all of the manipulators. */
  ISI_STATUS UpdateAllJointValues();

  /*! Update joint values using sine functions for debugging purposes. */
  ISI_STATUS UpdateAllJointValuesSineWave();

  /*! Print all of the joint values for all of the manipulators. */
  void PrintAllJointValues() const;

  /*! Get all of the joint values as a string. */
  std::string GetAllJointValuesAsString() const;

  /*! Print all of the kinematics transforms for all of the manipulators. */
  void PrintAllKinematicsTransforms() const;

  /*! Update all of the base frames of the manipulators using the da Vinci API. */
  void UpdateBaseToWorldTransforms();

  /*! Update every transform for each DH row in the kinematic chain. */
  ISI_STATUS UpdateAllKinematicsTransforms();

  /*! Copy data from one ISI_TRANSFORM to another. */
  static void CopyIsiTransform(ISI_TRANSFORM* srcTransform, ISI_TRANSFORM* destTransform);

protected:
  /*! Variables for storing the state of the da Vinci API. */
  ISI_STATUS mStatus; // An integer, error if != 0
  bool mConnected;
  bool mStreaming;
  unsigned int mRateHz; // Rate of data streaming from the da Vinci system

  /*! The intuitive da Vinci will have three manipulators: two PSMs and one ECM. */
  IntuitiveDaVinciManipulator* mPsm1;
  IntuitiveDaVinciManipulator* mPsm2;
  IntuitiveDaVinciManipulator* mEcm;

  /*! We also want to track all of the manipulator base frames. */
  ISI_TRANSFORM* mPsm1BaseToWorld;
  ISI_TRANSFORM* mPsm2BaseToWorld;
  ISI_TRANSFORM* mEcmBaseToWorld;

  /*! These are some intermediate variables needed for computation of the base frames poses. */
  ISI_TRANSFORM* mViewToWorld;
  ISI_TRANSFORM* mPsm1BaseToView;
  ISI_TRANSFORM* mPsm2BaseToView;
};

#endif
