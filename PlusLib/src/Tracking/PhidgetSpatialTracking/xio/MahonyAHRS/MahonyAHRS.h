//=====================================================================================================
// MahonyAHRS.h
//=====================================================================================================
//
// Madgwick's implementation of Mayhony's AHRS algorithm.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			Author			Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
//
//=====================================================================================================
#ifndef MahonyAHRS_h
#define MahonyAHRS_h

class MahonyAHRS
{
public:

  MahonyAHRS()
  {
    sampleFreq=512.0; // Hz
    twoKp=2.0f * 0.5f;
    twoKi=2.0f * 0.0f;
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
  void SetGain(float proportional, float integral) { twoKp=2*proportional; twoKi=2*integral; };
  void SetOrientation(float aq0, float aq1, float aq2, float aq3) { q0=aq0; q1=aq1; q2=aq2; q3=aq3; };
  void GetOrientation(float &aq0, float &aq1, float &aq2, float &aq3) { aq0=q0; aq1=q1; aq2=q2; aq3=q3; };
  void GetOrientation(double &aq0, double &aq1, double &aq2, double &aq3) { aq0=q0; aq1=q1; aq2=q2; aq3=q3; };

protected:  

  float InvSqrt(float x);

  float sampleFreq; // sample frequency in Hz
  float twoKp;			// 2 * proportional gain (Kp)
  float twoKi;			// 2 * integral gain (Ki)
  float q0, q1, q2, q3;	// quaternion of sensor frame relative to auxiliary frame
  
  // integral error terms scaled by Ki
  float integralFBx;
  float integralFBy;
  float integralFBz;
};

#endif
