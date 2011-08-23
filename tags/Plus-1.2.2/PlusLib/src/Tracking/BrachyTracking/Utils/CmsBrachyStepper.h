#ifndef _CMSBRACHYSTEPPER_H_
#define _CMSBRACHYSTEPPER_H_
/**************************************************************
*
*       Communication protocol for Prostate encoder controller VERSION 3.X
*       Communication runs at 19200 baud, 8 data bits, and
*       no parity using the COM port of the PC. (settable parms)
*
*       The communications channel (receive) operates
*       with interrupts enabled, does checksum verification,
*       and queues up the received data bytes.
*
**************************************************************/

#include "PlusConfigure.h"
#include "BrachyStepper.h"
#include "SerialLine.h"
#include <vector>
#include <string>

class  CmsBrachyStepper : public BrachyStepper
{
public:
  typedef const char* STEPPERCOMMAND;
  typedef const char* STEPPERRESPCODE;

  CmsBrachyStepper(unsigned long COMPort=1, unsigned long BaudRate=19200);
  virtual ~CmsBrachyStepper();

  // Connect to CMS stepper    
  virtual PlusStatus Connect(); 

  // Disconnect from CMS stepper 
  virtual PlusStatus Disconnect(); 

  // Get raw encoder values from stepper 
  virtual PlusStatus GetEncoderValues(double &PPosition, 
    double &GPosition, double &RPosition, unsigned long &PositionRequestNumber);

  virtual PlusStatus GetDeviceModelInfo( std::string& version, std::string& model, std::string& serial ); 

  virtual PlusStatus ResetStepper();

  virtual PlusStatus InitializeStepper(std::string &CalibMsg);

  virtual PlusStatus SetBaudRate(unsigned long BaudRate); 

  virtual PlusStatus SetCOMPort(unsigned long COMPort); 

  virtual PlusStatus IsStepperAlive();


  //*********************************************

  PlusStatus GetStatusInfo(unsigned int &Status);

  PlusStatus GetProbeReferenceData(double &count, double &dist, double &scale);

  PlusStatus GetGridReferenceData(double &count, double &dist, double &scale);

  PlusStatus GetRotationReferenceData(double &count, double &dist, double &scale);

  PlusStatus GetCalibrationState(int &PState, int &GState, int &RState);

  PlusStatus GetRotateState(int &State);

  bool IsStepperCalibrated();

  PlusStatus GetMotorizationScaleFactor(int &scaleFactor); 

  bool IsStepperMotorized(); 

  PlusStatus MoveProbeToPosition(double PositionInMm, int &ReturnCode); 
 

protected:	

  PlusStatus GetVersionInfo(int &iVerHi, int &iVerLo, int &iModelNum, int &iSerialNum); 

  PlusStatus TurnMotorOn(); 

  PlusStatus TurnMotorOff(); 

  PlusStatus StepperButtonEnable();

  PlusStatus StepperButtonDisable();

  PlusStatus StepperRotateCalibrationEnable();

  PlusStatus StepperRotateCalibrationDisable();

  void SetScalingParameters();

  void ClearBuffer(); 
  


  static unsigned int AsciiToBin(unsigned int i, unsigned int j);

  static void BinToAscii(unsigned int n, unsigned char *c1, unsigned char *c2);

  void SendStepperCommand(STEPPERCOMMAND command, STEPPERRESPCODE Response, std::vector<BYTE> &vRawMessage);

  void SendPositionRequestCommand(STEPPERCOMMAND command, std::vector<BYTE> &vRawMessage);

  void StepperInstruction(STEPPERCOMMAND command);

  void ReadStepperAnswer(std::vector<BYTE> &stepperAnswer);

  bool IsStepperACKRecieved(std::vector<BYTE> &ackMessage, STEPPERCOMMAND command);

  void GetStepperMessage(STEPPERCOMMAND command, const char* Response, std::vector<BYTE> &DecodedMessage);

  void GetPositionMessage(std::vector<BYTE> &StepperMessage, std::vector<BYTE> &DecodedMessage);

  void CreateAckMessage( BYTE opcode, std::string *sMessage);

  void SendAckMessage(std::vector<BYTE> StepperMessage);

  void DecodeStepperMessage(std::vector<BYTE> StepperMessage, std::vector<BYTE> &DecodedMessage);

  PlusStatus GetReferenceData(STEPPERCOMMAND command, STEPPERRESPCODE respcode, double &count, double &dist, double &scale);

private:
   
  SerialLine* m_StepperCOMPort;
  double m_ProbeScale;
  double m_GridScale;
  double m_RotationScale;
  bool m_IsCalibrated; 
  
  // Sometimes connection reset is needed on Win7 when position error query fails repeatedly.
  // When repeated error count reaches the defined maximum the connection is reset (close/reopen).
  int m_RepeatedPositionErrorCount; 

  CRITICAL_SECTION m_CriticalSection; 

};

#endif