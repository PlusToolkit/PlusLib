//=====================================================================================================
// MadgwickAHRS.h
//=====================================================================================================
//
// Implementation of Madgwick's IMU and AHRS algorithms.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			Author          Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
//
//=====================================================================================================
#ifndef MadgwickAHRS_h
#define MadgwickAHRS_h

class MadgwickAHRS
{
public:

  MadgwickAHRS()
  {
    sampleFreq=512.0f; // Hz
    beta=0.1f;
    q0=1.0f;
    q1=0.0f;
    q2=0.0f;
    q3=0.0f;
      
    integralFBx = 0.0f;
    integralFBy = 0.0f;
    integralFBz = 0.0f;
  }
  
  void Update(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
  void UpdateIMU(float gx, float gy, float gz, float ax, float ay, float az);
  
  void SetSampleFreqHz(float asampleFreq) { sampleFreq=asampleFreq; };
  void SetGain(float aBeta) { beta=aBeta; };
  void SetOrientation(float aq0, float aq1, float aq2, float aq3) { q0=aq0; q1=aq1; q2=aq2; q3=aq3; };
  void GetOrientation(float &aq0, float &aq1, float &aq2, float &aq3) { aq0=q0; aq1=q1; aq2=q2; aq3=q3; };
  void GetOrientation(double &aq0, double &aq1, double &aq2, double &aq3) { aq0=q0; aq1=q1; aq2=q2; aq3=q3; };

protected:  

  float InvSqrt(float x);

  float sampleFreq; // sample frequency in Hz
  volatile float beta; // algorithm gain
  float q0, q1, q2, q3;	// quaternion of sensor frame relative to auxiliary frame
  
  // integral error terms scaled by Ki
  float integralFBx;
  float integralFBy;
  float integralFBz;
};


#endif
//=====================================================================================================
// End of file
//=====================================================================================================
