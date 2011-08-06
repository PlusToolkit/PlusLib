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

  BrachyStepper(){};
  virtual ~BrachyStepper(){}; 

  // Description:
  // Connect to stepper
  virtual PlusStatus Connect() = 0; 

  // Description:
  // Disconnect from stepper 
  virtual PlusStatus Disconnect() = 0; 

  // Description:
  // Get raw encoder values from stepper 
  virtual PlusStatus GetEncoderValues(double &PPosition, 
    double &GPosition, double &RPosition, unsigned long &PositionRequestNumber) = 0; 

  // Description:
  // Get stepper model specific information from device 
  virtual PlusStatus GetDeviceModelInfo( std::string& version, std::string& model, std::string& serial ) = 0; 

  // Description:
  // Reset stepper 
  virtual PlusStatus ResetStepper() = 0; 

  // Description:
  // Set baud rate for communication with the stepper
  virtual PlusStatus SetBaudRate(unsigned long BaudRate) = 0; 

  // Description:
  // Set COM port number for cummunication with the stepper 
  virtual PlusStatus SetCOMPort(unsigned long COMPort) = 0; 

  // Description:
  // Get notification from the stepper 
  virtual PlusStatus IsStepperAlive() = 0; 

  // Description:
  // Initialize stepper 
  virtual PlusStatus InitializeStepper(std::string &CalibMsg) = 0; 

  // Description:
  // Set/get bracy stepper type from BRACHY_STEPPER_TYPE
  void SetBrachyStepperType( BRACHY_STEPPER_TYPE type ) { m_BarchyStepperType = type; }
  BRACHY_STEPPER_TYPE GetBrachyStepperType() { return m_BarchyStepperType; }

  // Description:
  // Get brachy stepper type in string format
  static std::string GetBrachyStepperTypeInString(BRACHY_STEPPER_TYPE stepperType) 
  { 
    std::string strStepperType = ""; 

    switch(stepperType)
    { 
    case BURDETTE_MEDICAL_SYSTEMS_DIGITAL_STEPPER: 
      strStepperType = "Burdette Medical Systems Digital Stepper"; 
      break;
      
    case BURDETTE_MEDICAL_SYSTEMS_DIGITAL_MOTORIZED_STEPPER: 
        strStepperType = "Burdette Medical Systems Digital Motorized Stepper"; 
        break; 
    
    case CMS_ACCUSEED_DS300: 
        strStepperType = "CMS Accuseed DS300"; 
        break; 

    case CIVCO_STEPPER: 
        strStepperType = "CIVCO"; 
        break; 
    default: 
      LOG_ERROR("Unable to recognize stepper type: " << stepperType); 
      strStepperType = ""; 
    }

    return strStepperType; 
  }

protected: 

  BRACHY_STEPPER_TYPE m_BarchyStepperType;

  unsigned long m_PositionRequestNumber; 

}; 

#endif 
