#ifndef _BRACHYSTEPPER_H_
#define _BRACHYSTEPPER_H_
#include "PlusConfigure.h"

class  BrachyStepper 
{
public:

  enum BRACHY_STEPPER_TYPE
  {
    BURDETTE_MEDICAL_SYSTEMS_DIGITAL_STEPPER,
    BURDETTE_MEDICAL_SYSTEMS_DIGITAL_MOTORIZED_STEPPER,
    CMS_ACCUSEED_DS300, 
    CIVCO_STEPPER
  };

  BrachyStepper();
  virtual ~BrachyStepper(); 

  virtual PlusStatus StartTracking() = 0; 
  virtual PlusStatus StopTracking() = 0; 

  virtual PlusStatus GetProbePositions(double &PPosition, 
    double &GPosition, double &RPosition, unsigned long &PositionRequestNumber) = 0; 

  virtual PlusStatus GetVersionInfo(int &iVerHi, int &iVerLo, int &iModelNum, int &iSerialNum) = 0; 

  virtual PlusStatus ResetStepper() = 0; 

  virtual PlusStatus CalibrateStepper(std::string &CalibMsg) = 0; 


   void SetBrachyStepperType( BRACHY_STEPPER_TYPE type ) { m_BarchyStepperType = type; }
  BRACHY_STEPPER_TYPE GetBrachyStepperType() { return m_BarchyStepperType; }

protected: 

  BRACHY_STEPPER_TYPE m_BarchyStepperType;

}; 

#endif 
