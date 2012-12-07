/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Madgwick's implementation of Mahony's AHRS algorithm.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			Author			Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load

#ifndef MahonyAhrsAlgo_h
#define MahonyAhrsAlgo_h

#include "AhrsAlgo.h"

class MahonyAhrsAlgo : public AhrsAlgo
{
public:

  MahonyAhrsAlgo()
  {
    twoKp=2.0f * 0.5f;
    twoKi=2.0f * 0.0f;
    
    integralFBx = 0.0f;
    integralFBy = 0.0f;
    integralFBz = 0.0f;
  }
  
  virtual void Update(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
  virtual void UpdateIMU(float gx, float gy, float gz, float ax, float ay, float az);
  
  virtual void SetGain(float proportional, float integral) { twoKp=2*proportional; twoKi=2*integral; };

protected:  

  float twoKp;			// 2 * proportional gain (Kp)
  float twoKi;			// 2 * integral gain (Ki)
  
  // integral error terms scaled by Ki
  float integralFBx;
  float integralFBy;
  float integralFBz;
};

#endif
