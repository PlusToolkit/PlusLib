/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef AhrsAlgo_h
#define AhrsAlgo_h

/*!
\class AhrsAlgo 
\brief Common base class for AHRS algorithms

\ingroup PlusLibDataCollection
*/

class AhrsAlgo
{
public:
    
  AhrsAlgo()
  {
    minTimestampDifferenceSec = 1E-6;
    sampleFreq=512.0; // Hz
    lastUpdateTime = -1;
    
    q0=1.0f;
    q1=0.0f;
    q2=0.0f;
    q3=0.0f;    
  }
  
  virtual void Update(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz)=0;
  virtual void UpdateIMU(float gx, float gy, float gz, float ax, float ay, float az)=0;
  
  //combines updating with timestamping
  void UpdateWithTimestamp(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz, double timestamp)
  {
    UpdateSampleFreqFromSystemTimeSec(timestamp);
    Update(gx, gy, gz, ax, ay, az, mx, my, mz);
  }
  
  //combines updating with timestamping
  void UpdateIMUWithTimestamp(float gx, float gy, float gz, float ax, float ay, float az, double timestamp)
  {
    UpdateSampleFreqFromSystemTimeSec(timestamp);
    UpdateIMU(gx, gy, gz, ax, ay, az);
  }
  

  virtual void SetGain(float proportional, float integral)=0;
  
  void SetSampleFreqHz(float asampleFreq) { sampleFreq=asampleFreq; };  
  void SetOrientation(float aq0, float aq1, float aq2, float aq3) { q0=aq0; q1=aq1; q2=aq2; q3=aq3; };
  void GetOrientation(float &aq0, float &aq1, float &aq2, float &aq3) { aq0=q0; aq1=q1; aq2=q2; aq3=q3; };
  void GetOrientation(double &aq0, double &aq1, double &aq2, double &aq3) { aq0=q0; aq1=q1; aq2=q2; aq3=q3; };
  double GetLastUpdateTime() {return lastUpdateTime; };
  
  //updates the sampling frequency from a given timestamp
  void UpdateSampleFreqFromSystemTimeSec(double timeSystemSec)
  {
    if (timeSystemSec<0.0)
    { 
      sampleFreq = 125;
      return;
    }
    //if first update, use as reference time and assume sample frequency is the maximum normal sample freq
    if (lastUpdateTime<0)
    {
      lastUpdateTime=timeSystemSec;
      sampleFreq=125;
    }
    else
    {
      double timeSinceLastAhrsUpdateSec=timeSystemSec-lastUpdateTime;
      if(timeSinceLastAhrsUpdateSec < minTimestampDifferenceSec)
      {
        sampleFreq = 125;
        return;
      }
      lastUpdateTime=timeSystemSec;
      sampleFreq = static_cast<float>(1.0/timeSinceLastAhrsUpdateSec);
    }
  }

protected:  
  
  /*!
    Fast inverse square-root
    See: http://en.wikipedia.org/wiki/Fast_inverse_square_root
    */
  
  float InvSqrt(float x) 
  {
    float halfx = 0.5f * x;
    float y = x;
    long i = *(long*)&y;
    i = 0x5f3759df - (i>>1);
    y = *(float*)&i;
    y = y * (1.5f - (halfx * y * y));
    return y;
  }

  float sampleFreq; // sample frequency in Hz
  double lastUpdateTime; //system time at last update

  // quaternion of sensor frame relative to auxiliary frame  
  float q0;
  float q1;
  float q2;
  float q3;

  double minTimestampDifferenceSec;
};

#endif
