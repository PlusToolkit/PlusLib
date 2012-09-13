/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Implementation of Madgwick's IMU and AHRS algorithms.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			Author          Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load

#ifndef MadgwickAhrsAlgo_h
#define MadgwickAhrsAlgo_h

#include "AhrsAlgo.h"

class MadgwickAhrsAlgo : public AhrsAlgo
{
public:

  MadgwickAhrsAlgo()
  {
    beta=0.1f;
  }
  
  virtual void Update(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
  virtual void UpdateIMU(float gx, float gy, float gz, float ax, float ay, float az);
  
  virtual void SetGain(float proportional, float) { beta=proportional; };

protected:  

  float beta; // algorithm gain
};

#endif
