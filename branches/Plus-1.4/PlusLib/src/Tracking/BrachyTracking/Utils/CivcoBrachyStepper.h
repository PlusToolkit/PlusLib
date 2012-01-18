/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef _CIVCOBRACHYSTEPPER_H_
#define _CIVCOBRACHYSTEPPER_H_

#include "PlusConfigure.h"
#include "BrachyStepper.h"

/*! \class CivcoBrachyStepper 
 * \brief Interface for position tracking using Civco brachy steppers
 *
 * Communication protocol for ED2 encoder controller from US digital
 * Communication runs at 19200 baud, 8 data bits, and
 * no parity using the COM port of the PC. (settable parms)
 *
 * \ingroup PlusLibBrachyStepper
 */
class  CivcoBrachyStepper : public BrachyStepper
{
public:

  /*! Constructor */
  CivcoBrachyStepper(unsigned long COMPort=1, unsigned long BaudRate=9600);
  /*! Destructor */
  virtual ~CivcoBrachyStepper();

  /*! Connect to CIVCO stepper */
  virtual PlusStatus Connect(); 

  /*! Disconnect from CIVCO stepper */
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

protected: 

  /*! IEEE Standard 754 floating point to double conversion  */
  double ConvertFloatToDecimal( long floatPoint ); 

  /*! Returns number of devices found on the SEI bus */
  long GetNumberOfSeiDevices(); 

  /*! Get information about the SEI device */
  PlusStatus GetSeiDeviceInfo(long devnum, long &model, long &serialnum, long &version, long &addr); 

  /*! Used COM port number */
  long m_COMPort; 

  /*! Baud rate */
  long m_BaudRate; 

  /*! Device number */
  long m_DeviceNumber; 

  /*! Device address */
  long m_DeviceAddress;

};

#endif
