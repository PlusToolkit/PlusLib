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


  CmsBrachyStepper();

  CmsBrachyStepper(const char* COMPort, unsigned long BaudRate);

  virtual ~CmsBrachyStepper();

  
  virtual PlusStatus StartTracking(); 

  virtual PlusStatus StopTracking(); 

  virtual PlusStatus GetProbePositions(double &PPosition, 
    double &GPosition, double &RPosition, unsigned long &PositionRequestNumber);

  virtual PlusStatus GetVersionInfo(int &iVerHi, int &iVerLo, int &iModelNum, int &iSerialNum);

  virtual PlusStatus ResetStepper();

  virtual PlusStatus CalibrateStepper(std::string &CalibMsg);



  PlusStatus GetStatusInfo(unsigned int &Status);

  PlusStatus GetProbeReferenceData(double &count, double &dist, double &scale);

  PlusStatus GetGridReferenceData(double &count, double &dist, double &scale);

  PlusStatus GetRotationReferenceData(double &count, double &dist, double &scale);

  

  PlusStatus GetCalibrationState(int &PState, int &GState, int &RState);

  PlusStatus GetRotateState(int &State);

  void SetBaudRate(unsigned long BaudRate) { this->m_StepperCOMPort->SetSerialPortSpeed(BaudRate); }

  void SetCOMPort(const char* COMPort) { this->m_StepperCOMPort->SetPortName(COMPort); }

  bool IsStepperCalibrated();

  bool IsStepperAlive();

  

  PlusStatus TurnMotorOn(); 

  PlusStatus TurnMotorOff(); 

  PlusStatus GetMotorizationCode(int &MotorizationCode); 

  bool IsStepperMotorized(); 

  PlusStatus MoveProbeToPosition(double PositionInMm, int &ReturnCode); 

  PlusStatus StepperButtonEnable();

  PlusStatus StepperButtonDisable();

  PlusStatus StepperRotateCalibrationEnable();

  PlusStatus StepperRotateCalibrationDisable();

  void SetScalingParameters();

  void ClearBuffer(); 

 

private:	
  unsigned int AsciiToBin(unsigned int i, unsigned int j);

  void BinToAscii(unsigned int n, unsigned char *c1, unsigned char *c2);

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
  unsigned long m_PositionRequestNumber; 
  // Sometimes connection reset is needed on Win7 when position error query fails repeatedly.
  // When repeated error count reaches the defined maximum the connection is reset (close/reopen).
  int m_RepeatedPositionErrorCount; 

  CRITICAL_SECTION m_CriticalSection; 

};

#endif