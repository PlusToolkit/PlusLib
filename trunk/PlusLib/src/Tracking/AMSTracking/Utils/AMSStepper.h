#ifndef _STEPPER_H_
#define _STEPPER_H_
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

#include "vtkStepperWin32Header.h"
#include "SerialLine.h"
#include <vector>
#include <string>

	typedef const char* STEPPERCOMMAND;
	typedef const char* STEPPERRESPCODE;
	class  AMSStepper 
	{
	public:
		
		enum BRACHY_STEPPER_TYPE
		{
			BURDETTE_MEDICAL_SYSTEMS_DIGITAL_STEPPER,
			BURDETTE_MEDICAL_SYSTEMS_DIGITAL_MOTORIZED_STEPPER,
			CMS_ACCUSEED_DS300
		};

		AMSStepper();

		AMSStepper(const char* COMPort, unsigned long BaudRate);

		~AMSStepper();

		bool GetVersionInfo(int &iVerHi, int &iVerLo, int &iModelNum, int &iSerialNum);

		bool GetStatusInfo(unsigned int &Status);

		bool GetProbeReferenceData(double &count, double &dist, double &scale);

		bool GetGridReferenceData(double &count, double &dist, double &scale);

		bool GetRotationReferenceData(double &count, double &dist, double &scale);

		bool GetProbePositions(double &PPosition, double &GPosition, double &RPosition, unsigned long &PositionRequestNumber);

		bool GetCalibrationState(int &PState, int &GState, int &RState);

		bool GetRotateState(int &State);

		void SetBaudRate(unsigned long BaudRate) { this->m_StepperCOMPort->SetSerialPortSpeed(BaudRate); }

		void SetCOMPort(const char* COMPort) { this->m_StepperCOMPort->SetPortName(COMPort); }

		bool IsStepperCalibrated();

		bool IsStepperAlive();

		bool StartTracking(); 

		void StopTracking(); 

		bool CalibrateStepper(std::string &CalibMsg);

		bool ResetStepper();

		bool TurnMotorOn(); 

		bool TurnMotorOff(); 

		bool GetMotorizationCode(int &MotorizationCode); 

		bool IsStepperMotorized(); 

		bool MoveProbeToPosition(double PositionInMm, int &ReturnCode); 

		bool StepperButtonEnable();

		bool StepperButtonDisable();

		bool StepperRotateCalibrationEnable();

		bool StepperRotateCalibrationDisable();

		void SetScalingParameters();

		void ClearBuffer(); 

		void SetBrachyStepperType( BRACHY_STEPPER_TYPE type ) { m_BarchyStepperType = type; }
		BRACHY_STEPPER_TYPE GetBrachyStepperType() { return m_BarchyStepperType; }

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

		bool GetReferenceData(STEPPERCOMMAND command, STEPPERRESPCODE respcode, double &count, double &dist, double &scale);

	private:
		BRACHY_STEPPER_TYPE m_BarchyStepperType; 
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