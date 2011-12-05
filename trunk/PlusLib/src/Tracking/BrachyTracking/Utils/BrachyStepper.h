/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef _BRACHYSTEPPER_H_
#define _BRACHYSTEPPER_H_
#include "PlusConfigure.h"

/*! \class BrachyStepper 
 * \brief Generic interface for position tracking using brachy stepper devices
 * \ingroup PlusLibBrachyStepper
 */
class  BrachyStepper 
{
public:

  /*! Stepper type enumeration */
  enum BRACHY_STEPPER_TYPE
  {
    UNDEFINED_STEPPER,
    BURDETTE_MEDICAL_SYSTEMS_DIGITAL_STEPPER,
    BURDETTE_MEDICAL_SYSTEMS_DIGITAL_MOTORIZED_STEPPER,
    CMS_ACCUSEED_DS300, 
    CIVCO_STEPPER
  };

  /*! Constructor */
  BrachyStepper()
  {
    m_BrachyStepperType=UNDEFINED_STEPPER;
    m_PositionRequestNumber=0;
  };
  virtual ~BrachyStepper(){}; 

  /*! Destructor */
  virtual PlusStatus Connect() = 0; 

  /*! Disconnect from stepper  */
  virtual PlusStatus Disconnect() = 0; 

  /*! Get raw encoder values from stepper  */
  virtual PlusStatus GetEncoderValues(double &PPosition, 
    double &GPosition, double &RPosition, unsigned long &PositionRequestNumber) = 0; 

  /*! Get stepper model specific information from device  */
  virtual PlusStatus GetDeviceModelInfo( std::string& version, std::string& model, std::string& serial ) = 0; 

  /*! Reset stepper  */
  virtual PlusStatus ResetStepper() = 0; 

  /*! Set baud rate for communication with the stepper */
  virtual PlusStatus SetBaudRate(unsigned long BaudRate) = 0; 

  /*! Set COM port number for cummunication with the stepper  */
  virtual PlusStatus SetCOMPort(unsigned long COMPort) = 0; 

  /*! Get notification from the stepper  */
  virtual PlusStatus IsStepperAlive() = 0; 

  /*! Initialize stepper  */
  virtual PlusStatus InitializeStepper(std::string &CalibMsg) = 0; 

  /*! Set/get bracy stepper type from BRACHY_STEPPER_TYPE */
  void SetBrachyStepperType( BRACHY_STEPPER_TYPE type ) { m_BrachyStepperType = type; }
  BRACHY_STEPPER_TYPE GetBrachyStepperType() const { return m_BrachyStepperType; }

  /*! Get brachy stepper type in string format */
  static std::string GetBrachyStepperTypeInString(BRACHY_STEPPER_TYPE stepperType) 
  { 
    std::string strStepperType;

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
      strStepperType.clear(); 
    }

    return strStepperType; 
  }

protected: 

  /*! Type of the current brachy stepper */
  BRACHY_STEPPER_TYPE m_BrachyStepperType;

  /*! Number of position requests performed */
  unsigned long m_PositionRequestNumber; 

}; 

#endif 
