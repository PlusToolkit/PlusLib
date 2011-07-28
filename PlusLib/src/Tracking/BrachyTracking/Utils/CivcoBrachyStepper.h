#ifndef _CIVCOBRACHYSTEPPER_H_
#define _CIVCOBRACHYSTEPPER_H_
/**************************************************************
*
*       Communication protocol for ED2 encoder controller from US digital
*       Communication runs at 19200 baud, 8 data bits, and
*       no parity using the COM port of the PC. (settable parms)
*
**************************************************************/

#include "PlusConfigure.h"
#include "BrachyStepper.h"

class  CivcoBrachyStepper : public BrachyStepper
{
public:

  CivcoBrachyStepper();
  CivcoBrachyStepper(unsigned long COMPort, unsigned long BaudRate);
  virtual ~CivcoBrachyStepper();

  // Description: 
  // Connect to CIVCO stepper    
  virtual PlusStatus Connect(); 

  // Description:
  // Disconnect from CIVCO stepper 
  virtual PlusStatus Disconnect(); 

  // Description:
  // Get raw encoder values from stepper 
  virtual PlusStatus GetEncoderValues(double &PPosition, 
    double &GPosition, double &RPosition, unsigned long &PositionRequestNumber);

  virtual PlusStatus GetDeviceModelInfo( std::string& version, std::string& model, std::string& serial ); 

  virtual PlusStatus ResetStepper();

  virtual PlusStatus CalibrateStepper(std::string &CalibMsg);

  virtual void SetBaudRate(unsigned long BaudRate); 

  virtual void SetCOMPort(unsigned long COMPort); 

  virtual PlusStatus IsStepperAlive();

protected: 

  long GetNumberOfSeiDevices(); 

  PlusStatus GetSeiDeviceInfo(long devnum, long &model, long &serialnum, long &version, long &addr); 

  long m_COMPort; 

};

#endif