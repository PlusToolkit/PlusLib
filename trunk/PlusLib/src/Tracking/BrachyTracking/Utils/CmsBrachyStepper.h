/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef _CMSBRACHYSTEPPER_H_
#define _CMSBRACHYSTEPPER_H_

#include "PlusConfigure.h"
#include "BrachyStepper.h"
#include "SerialLine.h"
#include <vector>
#include <string>

/*! \class CmsBrachyStepper 
 * \brief Interface for position tracking using CMS brachy steppers
 *
 * Communication protocol for Prostate encoder controller VERSION 3.X
 * Communication runs at 19200 baud, 8 data bits, and
 * no parity using the COM port of the PC. (settable parms)
 *
 * The communications channel (receive) operates
 * with interrupts enabled, does checksum verification,
 * and queues up the received data bytes.
 *
 * \ingroup PlusLibBrachyStepper
 */
class  CmsBrachyStepper : public BrachyStepper
{
public:
  typedef const char* STEPPERCOMMAND;
  typedef const char* STEPPERRESPCODE;

  /*! Constructor */
  CmsBrachyStepper(unsigned long COMPort=1, unsigned long BaudRate=19200);
  /*! Destructor */
  virtual ~CmsBrachyStepper();

  /*! Connect to CMS stepper */
  virtual PlusStatus Connect(); 

  /*! Disconnect from CMS stepper */
  virtual PlusStatus Disconnect(); 

  /*! Get raw encoder values from stepper */
  virtual PlusStatus GetEncoderValues(double &PPosition, 
    double &GPosition, double &RPosition, unsigned long &PositionRequestNumber);

  /*! Get device mode information */
  virtual PlusStatus GetDeviceModelInfo( std::string& version, std::string& model, std::string& serial ); 

  /*! Reset stepper */
  virtual PlusStatus ResetStepper();

  /*! Initialize stepper */
  virtual PlusStatus InitializeStepper(std::string &CalibMsg);

  /*! Set baud rate of communication */
  virtual PlusStatus SetBaudRate(unsigned long BaudRate); 

  /*! Set used COM port for communication */
  virtual PlusStatus SetCOMPort(unsigned long COMPort); 

  /*! Return whether stepper is alive */
  virtual PlusStatus IsStepperAlive();

public:

  /*! Get status information */
  PlusStatus GetStatusInfo(unsigned int &Status);

  /*! Get probe reference data */
  PlusStatus GetProbeReferenceData(double &count, double &dist, double &scale);

  /*! Get grid reference data */
  PlusStatus GetGridReferenceData(double &count, double &dist, double &scale);

  /*! Get rotation reference data */
  PlusStatus GetRotationReferenceData(double &count, double &dist, double &scale);

  /*! Get calibration state */
  PlusStatus GetCalibrationState(int &PState, int &GState, int &RState);

  /*! Get rotation state */
  PlusStatus GetRotateState(int &State);

  /*! Return whether stepper is calibrated */
  bool IsStepperCalibrated();

  /*! Return motorization scale factor */
  PlusStatus GetMotorizationScaleFactor(int &scaleFactor); 

  /*! Return whether stepper is motorized */
  bool IsStepperMotorized(); 

  /*! Commands the stepper to move the probe to a certain position */
  PlusStatus MoveProbeToPosition(double PositionInMm, int &ReturnCode); 
 
protected:
  /*! Get version information */
  PlusStatus GetVersionInfo(int &iVerHi, int &iVerLo, int &iModelNum, int &iSerialNum); 

  /*! Turn motor on */
  PlusStatus TurnMotorOn(); 

  /*! Turn motor off */
  PlusStatus TurnMotorOff(); 

  /*! Enable stepper button */
  PlusStatus StepperButtonEnable();

  /*! Disable stepper button */
  PlusStatus StepperButtonDisable();

  /*! Enable stepper rotation calibration */
  PlusStatus StepperRotateCalibrationEnable();

  /*! Disable stepper rotation calibration */
  PlusStatus StepperRotateCalibrationDisable();

  /*! Set scaling parameters */
  void SetScalingParameters();

  /*! Clear buffer */
  void ClearBuffer(); 
  
protected:
  /*! Convert from ASCII to binary */
  static unsigned int AsciiToBin(unsigned int i, unsigned int j);

  /*! Convert from binary to ASCII */
  static void BinToAscii(unsigned int n, unsigned char *c1, unsigned char *c2);

  /*! Send a command to the stepper */
  void SendStepperCommand(STEPPERCOMMAND command, STEPPERRESPCODE Response, std::vector<BYTE> &vRawMessage);

  /*! Send a position request command to the stepper */
  void SendPositionRequestCommand(STEPPERCOMMAND command, std::vector<BYTE> &vRawMessage);

  /*! Send instruction to the stepper */
  void StepperInstruction(STEPPERCOMMAND command);

  /*! Read stepper answer */
  void ReadStepperAnswer(std::vector<BYTE> &stepperAnswer);

  /*! Returns whether ACK received from stepper */
  bool IsStepperACKRecieved(std::vector<BYTE> &ackMessage, STEPPERCOMMAND command);

  /*! Get message from stepper */
  void GetStepperMessage(STEPPERCOMMAND command, const char* Response, std::vector<BYTE> &DecodedMessage);

  /*! Get position message from stepper */
  void GetPositionMessage(std::vector<BYTE> &StepperMessage, std::vector<BYTE> &DecodedMessage);

  /*! Create ACK message */
  void CreateAckMessage( BYTE opcode, std::string *sMessage);

  /*! Send ACK message */
  void SendAckMessage(std::vector<BYTE> StepperMessage);

  /*! Decode stepper message */
  void DecodeStepperMessage(std::vector<BYTE> StepperMessage, std::vector<BYTE> &DecodedMessage);

  /*! Get reference data */
  PlusStatus GetReferenceData(STEPPERCOMMAND command, STEPPERRESPCODE respcode, double &count, double &dist, double &scale);

private:
   
  /*! Serial COM port information */
  SerialLine* m_StepperCOMPort;

  /*! Probe scale */
  double m_ProbeScale;

  /*! Grid scale */
  double m_GridScale;

  /*! Rotation scale */
  double m_RotationScale;

  /*! Flag showing whether stepper is calibrated */
  bool m_IsCalibrated; 
  
  /*! Sometimes connection reset is needed on Win7 when position error query fails repeatedly.
   * When repeated error count reaches the defined maximum the connection is reset (close/reopen).
   */
  int m_RepeatedPositionErrorCount; 

  /*! Critical section object */
  CRITICAL_SECTION m_CriticalSection; 

};

#endif
