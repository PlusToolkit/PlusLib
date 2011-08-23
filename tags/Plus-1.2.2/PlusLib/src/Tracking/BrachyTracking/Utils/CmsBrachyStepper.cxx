#include "CmsBrachyStepper.h"
#include <iostream>
#include <assert.h>
#include <math.h>
#include "vtkAccurateTimer.h" 
#include <sstream>

#define STEPPERDEBUGMSG 0

#if STEPPERDEBUGMSG
#include <stdio.h>
#endif

#define STX 2
#define ETX 3
#define MAXTRIES 5	

static const int MAX_REPEATED_POSITION_ERROR_COUNT = 15;

//----------------------------------------------------------------------------
static const char* CALIB_MSG_PROBE_TO_BASE = "Step the probe to home position. A green light should appear.";
static const char* CALIB_MSG_PROBE_TO_REF = "Step the probe to the reference position (12.5 mm). An amber light should replace the green light.";
static const char* CALIB_MSG_PROBE_TO_HOME = "Step the probe to the home position. The light should revert to green and flashes twice.";

static const char* CALIB_MSG_GRID_TO_BASE = "Step the template grid to home position. A green light should appear.";
static const char* CALIB_MSG_GRID_TO_REF = "Step the template grid to the reference position (12.5 mm). An amber light should replace the green light.";
static const char* CALIB_MSG_GRID_TO_HOME = "Step the template grid to the home position. The light should revert to green and flashes twice.";

static const char* CALIB_MSG_ROTATION_TO_BASE = "Rotate the probe to zero degree. A green light should appear.";
static const char* CALIB_MSG_ROTATION_TO_REF = "Rotate the probe to the reference position (10.5 deg or more). An amber light should replace the green light.";
static const char* CALIB_MSG_ROTATION_TO_HOME = "Rotate the probe to zero degree. The light should revert to green and flashes twice.";

static const char* CALIB_MSG_COMPLETED = "Calibration completed!";

static CmsBrachyStepper::STEPPERCOMMAND SC_VERSION_INFO = "5151";
static CmsBrachyStepper::STEPPERRESPCODE SRC_VERSION_INFO = "37";

static CmsBrachyStepper::STEPPERCOMMAND SC_STATUS_INFO = "2121";
static CmsBrachyStepper::STEPPERRESPCODE SRC_STATUS_INFO = "22";

static CmsBrachyStepper::STEPPERCOMMAND SC_PROBE_REFERENCE_DATA = "3939";
static CmsBrachyStepper::STEPPERRESPCODE SRC_PROBE_REFERENCE_DATA = "4D";

static CmsBrachyStepper::STEPPERCOMMAND SC_GRID_REFERENCE_DATA = "4141";
static CmsBrachyStepper::STEPPERRESPCODE SRC_GRID_REFERENCE_DATA = "55";

static CmsBrachyStepper::STEPPERCOMMAND SC_ROTATION_REFERENCE_DATA = "4949";
static CmsBrachyStepper::STEPPERRESPCODE SRC_ROTATION_REFERENCE_DATA = "5D";

static CmsBrachyStepper::STEPPERCOMMAND SC_CALIBRATION_STATES = "7171";
static CmsBrachyStepper::STEPPERRESPCODE SRC_CALIBRATION_STATES = "74";

static CmsBrachyStepper::STEPPERCOMMAND SC_ROTATE_STATE = "5959";
static CmsBrachyStepper::STEPPERRESPCODE SRC_ROTATE_STATE = "8A";

static CmsBrachyStepper::STEPPERCOMMAND SC_POSITION_DATA_1 = "1919" ;
static CmsBrachyStepper::STEPPERCOMMAND SC_POSITION_DATA_2 = "9999";
static CmsBrachyStepper::STEPPERRESPCODE SRC_POSITION_DATA_BUTTON_OFF = "07";
static CmsBrachyStepper::STEPPERRESPCODE SRC_POSITION_DATA_BUTTON_ON = "47";
static CmsBrachyStepper::STEPPERRESPCODE SRC_POSITION_DATA_BUTTON_LATCH = "67";

// For motorized steppers
static CmsBrachyStepper::STEPPERCOMMAND SC_MOTOR_ON = "8A018B";
static CmsBrachyStepper::STEPPERCOMMAND SC_MOTOR_OFF = "8A008A";
static CmsBrachyStepper::STEPPERCOMMAND SC_IS_MOTORIZED = "8181";
static CmsBrachyStepper::STEPPERRESPCODE SRC_MOTORIZATION_CODE = "82"; // scale factor (zero if stepper is not motorized)
static CmsBrachyStepper::STEPPERRESPCODE SRC_MOVE_COMPLETE = "7A";

static CmsBrachyStepper::STEPPERCOMMAND SC_RESET_SYSTEM = "0101";

static CmsBrachyStepper::STEPPERCOMMAND SC_BUTTON_ENABLE = "2929";

static CmsBrachyStepper::STEPPERCOMMAND SC_BUTTON_DISABLE = "3131";

static CmsBrachyStepper::STEPPERCOMMAND SC_ENABLE_ROTATE_CALIBRATION = "920193";

static CmsBrachyStepper::STEPPERCOMMAND SC_DISABLE_ROTATE_CALIBRATION = "920092";

static CmsBrachyStepper::STEPPERRESPCODE SRC_COUNTERS_INVALID = "29";

//******************************************************************
//******************************************************************

//----------------------------------------------------------------------------
CmsBrachyStepper::CmsBrachyStepper( unsigned long COMPort/*=1*/, unsigned long BaudRate/*=19200*/)
{
	m_StepperCOMPort = new SerialLine(); 
  this->SetCOMPort(COMPort); 
  this->SetBaudRate(BaudRate); 
	m_StepperCOMPort->SetMaxReplyTime(1000);

  m_ProbeScale = 0; 
	m_GridScale = 0; 
	m_RotationScale = 0; 

  m_IsCalibrated = false; 
	m_RepeatedPositionErrorCount = 0; 

	m_BrachyStepperType = BURDETTE_MEDICAL_SYSTEMS_DIGITAL_STEPPER; 

	InitializeCriticalSection(&m_CriticalSection);
}

//----------------------------------------------------------------------------
CmsBrachyStepper::~CmsBrachyStepper()
{
	if (m_StepperCOMPort->IsHandleAlive())
	{
		m_StepperCOMPort->Close();
		delete m_StepperCOMPort; 
		m_StepperCOMPort = NULL; 
	}

	DeleteCriticalSection(&m_CriticalSection);
}

//----------------------------------------------------------------------------
PlusStatus CmsBrachyStepper::SetCOMPort(unsigned long COMPort) 
{ 
  std::ostringstream strComPort; 
  strComPort << "COM" << COMPort; 

  // Change only if we not yet connected
  if (this->m_StepperCOMPort->IsHandleAlive())  
  {
    LOG_ERROR("Unable to set serial port number, stepper already connected!"); 
    return PLUS_FAIL;
  }

  this->m_StepperCOMPort->SetPortName(strComPort.str()); 
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus CmsBrachyStepper::SetBaudRate(unsigned long BaudRate)
{ 
  // Change only if we not yet connected
  if (this->m_StepperCOMPort->IsHandleAlive())  
  {
    LOG_ERROR("Unable to set serial port speed, stepper already connected!"); 
    return PLUS_FAIL;
  }

  this->m_StepperCOMPort->SetSerialPortSpeed(BaudRate); 
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus CmsBrachyStepper::Connect()
{
	if (!this->m_StepperCOMPort->IsHandleAlive())
	{
		this->m_StepperCOMPort->Open(); 
	}

  if ( this->IsStepperAlive() != PLUS_SUCCESS )
  {
    LOG_ERROR("Stepper is not alive");
    return PLUS_FAIL;
  }

  if (!this->m_StepperCOMPort->IsHandleAlive())  
  {	
    LOG_ERROR("Stepper COM port handle is not alive");
    return PLUS_FAIL; 
  }
  if (!this->IsStepperCalibrated())
  {
    LOG_ERROR("Stepper is not calibrated");
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus CmsBrachyStepper::Disconnect()
{
	this->m_StepperCOMPort->Close();

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus CmsBrachyStepper::GetDeviceModelInfo( std::string& version, std::string& model, std::string& serial )
{
  int iVerHi=0; int iVerLo=0; int iModelNum=0; int iSerialNum=0;
  if ( this->GetVersionInfo(iVerHi, iVerLo, iModelNum, iSerialNum) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get version info from stepper!"); 
    return PLUS_FAIL; 
  }

  std::ostringstream strVersion; 
  strVersion << iVerHi << "." << iVerLo; 
  version = strVersion.str(); 

  std::ostringstream strModel; 
  strModel << iModelNum; 
  model = strModel.str(); 

  std::ostringstream strSerial; 
  strSerial << iSerialNum; 
  serial = strSerial.str(); 
  
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus CmsBrachyStepper::GetStatusInfo(unsigned int &Status)
{
	std::vector<BYTE> vDecodedMessage;
	PlusStatus retValue(PLUS_FAIL); 
	EnterCriticalSection(&m_CriticalSection); 
	for ( int i = 0; i < MAXTRIES; i++)
	{
		SendStepperCommand(SC_STATUS_INFO, SRC_STATUS_INFO, vDecodedMessage);

		if (vDecodedMessage.size() >= 2 )
		{
			Status = vDecodedMessage[1];
			retValue = PLUS_SUCCESS;
			break; 
		} 
	}
	LeaveCriticalSection(&m_CriticalSection); 
	return retValue;
}

//----------------------------------------------------------------------------
PlusStatus CmsBrachyStepper::GetVersionInfo(int &iVerHi, int &iVerLo, int &iModelNum, int &iSerialNum)
{
	std::vector<BYTE> vDecodedMessage; 
	PlusStatus retValue(PLUS_FAIL); 
	EnterCriticalSection(&m_CriticalSection); 
	for ( int i = 0; i < MAXTRIES; i++)
	{
		SendStepperCommand(SC_VERSION_INFO, SRC_VERSION_INFO, vDecodedMessage);

		if (vDecodedMessage.size() >= 7 )
		{
			iVerHi = vDecodedMessage[1];
			iVerLo = vDecodedMessage[2];
			iModelNum = vDecodedMessage[3] * 256 + vDecodedMessage[4];
			iSerialNum = vDecodedMessage[5] * 256 + vDecodedMessage[6];
			retValue = PLUS_SUCCESS;
			break; 
		}
	}
	LeaveCriticalSection(&m_CriticalSection); 
	return retValue;
}

//----------------------------------------------------------------------------
PlusStatus CmsBrachyStepper::GetReferenceData(STEPPERCOMMAND command, STEPPERRESPCODE respcode, double &count, double &dist, double &scale)
{
	std::vector<BYTE> vDecodedMessage; 
	PlusStatus retValue(PLUS_FAIL); 
	EnterCriticalSection(&m_CriticalSection); 
	for ( int i = 0; i < MAXTRIES; i++)
	{
		SendStepperCommand(command, respcode, vDecodedMessage);

		if (vDecodedMessage.size() >= 5) 
		{
			count = vDecodedMessage[1] * 256 + vDecodedMessage[2];
			dist = vDecodedMessage[3] * 256 + vDecodedMessage[4];
			scale = (double)(dist/100.0) / (double)count;
			retValue = PLUS_SUCCESS;
			break; 
		}
	}
	LeaveCriticalSection(&m_CriticalSection); 
	return retValue;
}

//----------------------------------------------------------------------------
PlusStatus CmsBrachyStepper::GetProbeReferenceData(double &count, double &dist, double &scale)
{	
	count = 0; dist = 0; scale = 0; 
	if (GetReferenceData(SC_PROBE_REFERENCE_DATA,SRC_PROBE_REFERENCE_DATA,count, dist, scale))
	{
		m_ProbeScale = scale;
		return PLUS_SUCCESS;
	}

	return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus CmsBrachyStepper::GetGridReferenceData(double &count, double &dist, double &scale)
{
	count = 0; dist = 0; scale = 0; 
	if (GetReferenceData(SC_GRID_REFERENCE_DATA,SRC_GRID_REFERENCE_DATA,count, dist, scale)==PLUS_SUCCESS) 
	{
		m_GridScale = scale;
		return PLUS_SUCCESS;
	}

	return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus CmsBrachyStepper::GetRotationReferenceData(double &count, double &dist, double &scale)
{
	count = 0; dist = 0; scale = 0; 
	if (GetReferenceData(SC_ROTATION_REFERENCE_DATA,SRC_ROTATION_REFERENCE_DATA,count, dist, scale)==PLUS_SUCCESS)
	{
		m_RotationScale = scale;
		return PLUS_SUCCESS;
	}

	return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus CmsBrachyStepper::GetEncoderValues(double &PPosition, double &GPosition, double &RPosition, unsigned long &PositionRequestNumber)
{
	// Increase the m_PositionNumber on every position request
	PositionRequestNumber = ++m_PositionRequestNumber; 

	if ( ! m_IsCalibrated )
	{
    LOG_ERROR("Cannot get encoder values, the stepper is not calibrated");
		return PLUS_FAIL; 
	}

	if ( m_ProbeScale == 0 || m_GridScale == 0 || m_RotationScale == 0 )
	{
		this->SetScalingParameters(); 
	}

	std::vector<BYTE> vDecodedMessage; 
	
	PlusStatus retValue(PLUS_FAIL); 

	EnterCriticalSection(&m_CriticalSection); 
	SendPositionRequestCommand(SC_POSITION_DATA_1, vDecodedMessage);
	LeaveCriticalSection(&m_CriticalSection); 

	if (vDecodedMessage.size() >= 7) 
	{
		PPosition = (static_cast<short>(vDecodedMessage[1]*256 + vDecodedMessage[2]))*m_ProbeScale;
		GPosition = (static_cast<short>(vDecodedMessage[3]*256 + vDecodedMessage[4]))*m_GridScale;
		RPosition = (static_cast<short>(vDecodedMessage[5]*256 + vDecodedMessage[6]))*m_RotationScale;
		m_RepeatedPositionErrorCount = 0; 
		retValue = PLUS_SUCCESS;
	}
	else
	{
		m_RepeatedPositionErrorCount++; 
	}

	if ( m_RepeatedPositionErrorCount >= MAX_REPEATED_POSITION_ERROR_COUNT )
	{
		m_RepeatedPositionErrorCount = 0; 
		this->Disconnect(); 
		this->Connect(); 
	}

	return retValue;
}

//----------------------------------------------------------------------------
PlusStatus CmsBrachyStepper::GetCalibrationState(int &PState, int &GState, int &RState)
{
	std::vector<BYTE> vDecodedMessage; 
	PlusStatus retValue(PLUS_FAIL); 
	EnterCriticalSection(&m_CriticalSection); 
	for ( int i = 0; i < MAXTRIES; i++)
	{
		SendStepperCommand(SC_CALIBRATION_STATES, SRC_CALIBRATION_STATES, vDecodedMessage);

		if (vDecodedMessage.size() >= 4) 
		{
			PState = vDecodedMessage[1];
			GState = vDecodedMessage[2];
			RState = vDecodedMessage[3];
			retValue = PLUS_SUCCESS;
			break; 
		}
	}
	LeaveCriticalSection(&m_CriticalSection);
	return retValue;
}

//----------------------------------------------------------------------------
PlusStatus CmsBrachyStepper::GetRotateState(int &State)
{
	std::vector<BYTE> vDecodedMessage; 
	PlusStatus retValue(PLUS_FAIL); 
	EnterCriticalSection(&m_CriticalSection);
	for ( int i = 0; i < MAXTRIES; i++)
	{
		SendStepperCommand(SC_ROTATE_STATE, SRC_ROTATE_STATE, vDecodedMessage);

		if (vDecodedMessage.size() >= 2) 
		{
			State = vDecodedMessage[1];
			retValue = PLUS_SUCCESS;
			break; 
		}
	}
	LeaveCriticalSection(&m_CriticalSection);
	return retValue;

}

//----------------------------------------------------------------------------
bool CmsBrachyStepper::IsStepperCalibrated()
{
	int PState = 0, GState = 0, RState = 0;
	this->m_IsCalibrated = false; 

	this->GetCalibrationState(PState, GState, RState);

	if( m_BrachyStepperType == BURDETTE_MEDICAL_SYSTEMS_DIGITAL_STEPPER
		&& 
		PState == 5 && GState == 5 && RState == 9)
	{
		this->m_IsCalibrated = true; 
		this->SetScalingParameters();
	}
	else if ( m_BrachyStepperType == BURDETTE_MEDICAL_SYSTEMS_DIGITAL_MOTORIZED_STEPPER
		&& 
		PState == 0 && GState == 5 && RState == 9)
	{
		this->m_IsCalibrated = true; 
		this->SetScalingParameters();
	} 
	else if ( m_BrachyStepperType == CMS_ACCUSEED_DS300 )
	{
		this->m_IsCalibrated = true; 
		this->SetScalingParameters();
	}

	return this->m_IsCalibrated;
}

//----------------------------------------------------------------------------
PlusStatus CmsBrachyStepper::IsStepperAlive()
{
	unsigned int Status;
	if (!this->GetStatusInfo(Status))
	{
		return PLUS_FAIL; 
	}
	return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus CmsBrachyStepper::InitializeStepper(std::string &CalibMsg)
{
	int PState = 0, GState = 0, RState = 0;
	this->GetCalibrationState(PState, GState, RState);
	this->m_IsCalibrated = false;

	if ((PState < 5) && (m_BrachyStepperType != BURDETTE_MEDICAL_SYSTEMS_DIGITAL_MOTORIZED_STEPPER))
	{
		switch (PState) 
		{
		case 1:
		case 2:
			CalibMsg = CALIB_MSG_PROBE_TO_REF;
			break;
		case 3:
		case 4:
			CalibMsg = CALIB_MSG_PROBE_TO_HOME;
			break;
		default:
			CalibMsg = CALIB_MSG_PROBE_TO_BASE;
		}
		return PLUS_FAIL;
	}
	else if ((GState < 5) && (m_BrachyStepperType != BURDETTE_MEDICAL_SYSTEMS_DIGITAL_MOTORIZED_STEPPER)) //TODO: does motorized stepper need this? It works without it and does not flash the green light either
	{
		switch (GState) 
		{
		case 1:
		case 2:
			CalibMsg = CALIB_MSG_GRID_TO_REF;
			break;
		case 3:
		case 4:
			CalibMsg = CALIB_MSG_GRID_TO_HOME;
			break;
		default:
			CalibMsg = CALIB_MSG_GRID_TO_BASE;
		}
		return PLUS_FAIL;
	}
	else if (RState < 9)
	{
		switch (RState) 
		{
		case 1:
		case 2:
			CalibMsg = CALIB_MSG_ROTATION_TO_REF;
			break;
		case 4:
			CalibMsg = CALIB_MSG_ROTATION_TO_HOME;
			break;
		default:
			CalibMsg = CALIB_MSG_ROTATION_TO_BASE;
		}
		return PLUS_FAIL;
	}
	CalibMsg = CALIB_MSG_COMPLETED;
	this->SetScalingParameters();
	this->m_IsCalibrated = true;
	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus CmsBrachyStepper::ResetStepper()
{
	PlusStatus retValue(PLUS_FAIL); 
	EnterCriticalSection(&m_CriticalSection);
	for ( int i = 0; i<MAXTRIES; i++)
	{
		std::vector<BYTE> ackMessage; 
		this->StepperInstruction(SC_RESET_SYSTEM);
		if(IsStepperACKRecieved(ackMessage, SC_RESET_SYSTEM))
		{
			retValue = PLUS_SUCCESS;
			break; 
		}
		ClearBuffer(); 
	}
	LeaveCriticalSection(&m_CriticalSection);
	return retValue;
}

//----------------------------------------------------------------------------
PlusStatus CmsBrachyStepper::TurnMotorOn()
{
	PlusStatus retValue(PLUS_FAIL); 
	EnterCriticalSection(&m_CriticalSection);
	for ( int i = 0; i<MAXTRIES; i++)
	{
		std::vector<BYTE> ackMessage; 
		this->StepperInstruction(SC_MOTOR_ON);
		if(IsStepperACKRecieved(ackMessage, SC_MOTOR_ON))
		{
			retValue = PLUS_SUCCESS;
			break; 
		}
		ClearBuffer(); 
	}
	LeaveCriticalSection(&m_CriticalSection);
	return retValue;
}

//----------------------------------------------------------------------------
PlusStatus CmsBrachyStepper::TurnMotorOff()
{
	PlusStatus retValue(PLUS_FAIL); 
	EnterCriticalSection(&m_CriticalSection);
	for ( int i = 0; i<MAXTRIES; i++)
	{
		std::vector<BYTE> ackMessage; 
		this->StepperInstruction(SC_MOTOR_OFF);
		if(IsStepperACKRecieved(ackMessage, SC_MOTOR_OFF))
		{
			retValue = PLUS_SUCCESS;
			break; 
		}
		ClearBuffer(); 
	}
	LeaveCriticalSection(&m_CriticalSection);
	return retValue;
}

//----------------------------------------------------------------------------
PlusStatus CmsBrachyStepper::GetMotorizationScaleFactor(int &scaleFactor)
{
	PlusStatus retValue(PLUS_FAIL); 
	EnterCriticalSection(&m_CriticalSection);
	for ( int i = 0; i<MAXTRIES; i++)
	{
		std::vector<BYTE> vDecodedMessage; 
		vDecodedMessage.reserve(50);
		SendStepperCommand(SC_IS_MOTORIZED, SRC_MOTORIZATION_CODE, vDecodedMessage);

		if (vDecodedMessage.size() >= 2) 
		{
			scaleFactor = vDecodedMessage[1];
			retValue = PLUS_SUCCESS;
			break; 
		}
		ClearBuffer(); 
	}
	LeaveCriticalSection(&m_CriticalSection);

	return retValue;
}

//----------------------------------------------------------------------------
bool CmsBrachyStepper::IsStepperMotorized()
{
	int scaleFactor(0); 
	this->GetMotorizationScaleFactor(scaleFactor); 

  // scaleFactor==0 for non-motorized steppers, scaleFactor>0 for motorized steppers
  return ( scaleFactor > 0 );
}


//----------------------------------------------------------------------------
PlusStatus CmsBrachyStepper::MoveProbeToPosition(double PositionInMm, int &ReturnCode)
{
	if ( !this->IsStepperMotorized() )
	{
		LOG_ERROR("Cannot move probe, the stepper is not motorized");
		return PLUS_FAIL; 
	}

	this->TurnMotorOn(); 

	// Get probe reference data
	double count (0), dist(0), scale(0);
	if ( !this->GetProbeReferenceData(count, dist, scale) )
	{
		LOG_ERROR("Unable to get probe reference data");
		return PLUS_FAIL; 
	}

	int scaleFactor(0); 
	this->GetMotorizationScaleFactor(scaleFactor); 

	double pPosition(0), gPosition(0), pRotation(0); 
	unsigned long positionRequestNumber(0); 
	if ( !this->GetEncoderValues(pPosition, gPosition, pRotation, positionRequestNumber) )
	{
		LOG_ERROR("Unable to get probe position");
		return PLUS_FAIL; 
	}
	
	// Calculate position distance
	double deltaPosMm = (PositionInMm - pPosition); 

	// Calculate direction 
	const int direction = (deltaPosMm >= 0.0 ? 1 : 0 );

	// Calculate steps
	const int steps = static_cast<int>(fabs(deltaPosMm*scaleFactor)+0.5); 

	// Create stepper command
	unsigned int ck;
	unsigned char c1,c2, c3, c4, c5, c6, c7, c8;
	std::string sMessage; 
	
	sMessage.push_back('7'); 
	sMessage.push_back('C'); 
	this->BinToAscii((direction & 0xff),&c1,&c2);
	sMessage.push_back(c1); 
	sMessage.push_back(c2); 
	this->BinToAscii(steps/256, &c3, &c4);
	sMessage.push_back(c3);
	sMessage.push_back(c4);
	this->BinToAscii(steps%256, &c5, &c6);
	sMessage.push_back(c5);
	sMessage.push_back(c6);
	ck = 0x7c + direction + (steps/256) + (steps%256);
    ck &= 0xff;
	this->BinToAscii(ck,&c7,&c8);
	sMessage.push_back(c7);
	sMessage.push_back(c8);

	// Send probe to desired position
	STEPPERCOMMAND moveProbe = (static_cast<STEPPERCOMMAND>(sMessage.c_str()));

	std::vector<BYTE> vDecodedMessage;
	PlusStatus retValue(PLUS_FAIL); 
	EnterCriticalSection(&m_CriticalSection); 
	ReturnCode = -1; 
	StepperInstruction(moveProbe);

	while ( ReturnCode == -1 )
	{
		GetStepperMessage(moveProbe, SRC_MOVE_COMPLETE, vDecodedMessage);

		if (vDecodedMessage.size() >= 2 )
		{
			ReturnCode = vDecodedMessage[1];
			retValue = PLUS_SUCCESS;
			break; 
		} 
		vtkAccurateTimer::GetInstance()->Delay(0.5); 
	}
	LeaveCriticalSection(&m_CriticalSection); 
	
	this->TurnMotorOff(); 
	
	return retValue;

}

//----------------------------------------------------------------------------
PlusStatus CmsBrachyStepper::StepperButtonEnable()
{
	PlusStatus retValue(PLUS_FAIL); 
	EnterCriticalSection(&m_CriticalSection);
	for ( int i = 0; i<MAXTRIES; i++)
	{
		this->StepperInstruction(SC_BUTTON_ENABLE);
		std::vector<BYTE> ackMessage; 
		if(IsStepperACKRecieved(ackMessage, SC_BUTTON_ENABLE))
		{
			retValue = PLUS_SUCCESS;
			break; 
		}
		ClearBuffer(); 
	}
	LeaveCriticalSection(&m_CriticalSection);
	return retValue;
}

//----------------------------------------------------------------------------
PlusStatus CmsBrachyStepper::StepperButtonDisable()
{
	PlusStatus retValue(PLUS_FAIL); 
	EnterCriticalSection(&m_CriticalSection);
	for ( int i = 0; i<MAXTRIES; i++)
	{
		this->StepperInstruction(SC_BUTTON_DISABLE);
		std::vector<BYTE> ackMessage; 	
		if(IsStepperACKRecieved(ackMessage, SC_BUTTON_DISABLE))
		{
			retValue = PLUS_SUCCESS;
			break; 
		}
		ClearBuffer(); 
	}
	LeaveCriticalSection(&m_CriticalSection);
	return retValue;
}

//----------------------------------------------------------------------------
PlusStatus CmsBrachyStepper::StepperRotateCalibrationEnable()
{
	PlusStatus retValue(PLUS_FAIL); 
	EnterCriticalSection(&m_CriticalSection);
	for ( int i = 0; i<MAXTRIES; i++)
	{
		this->StepperInstruction(SC_ENABLE_ROTATE_CALIBRATION);
		std::vector<BYTE> ackMessage; 
		if(IsStepperACKRecieved(ackMessage, SC_ENABLE_ROTATE_CALIBRATION))
		{
			retValue = PLUS_SUCCESS;
			break; 
		}
		ClearBuffer(); 
	}
	LeaveCriticalSection(&m_CriticalSection);
	return retValue;
}

//----------------------------------------------------------------------------
PlusStatus CmsBrachyStepper::StepperRotateCalibrationDisable()
{
	PlusStatus retValue(PLUS_FAIL); 
	EnterCriticalSection(&m_CriticalSection);
	for ( int i = 0; i<MAXTRIES; i++)
	{
		this->StepperInstruction(SC_DISABLE_ROTATE_CALIBRATION);
		std::vector<BYTE> ackMessage; 
		if(IsStepperACKRecieved(ackMessage, SC_DISABLE_ROTATE_CALIBRATION))
		{
			retValue = PLUS_SUCCESS;
			break; 
		}
		ClearBuffer(); 
	}
	LeaveCriticalSection(&m_CriticalSection);
	return retValue;
}

//----------------------------------------------------------------------------
void CmsBrachyStepper::SendStepperCommand(STEPPERCOMMAND command, STEPPERRESPCODE Response, std::vector<BYTE> &vRawMessage)
{
	vRawMessage.clear(); 

	StepperInstruction(command);

	GetStepperMessage(command, Response, vRawMessage);
}

//----------------------------------------------------------------------------
void CmsBrachyStepper::SendPositionRequestCommand(STEPPERCOMMAND command, std::vector<BYTE> &vRawMessage)
{
	vRawMessage.clear(); 

	if ( ! m_IsCalibrated )
	{
		return; 
	}

	StepperInstruction(command);
	std::vector<BYTE> StepperMessage; 
	bool ack = IsStepperACKRecieved(StepperMessage, command);
	if( ack )
	{
		StepperMessage.clear(); 
		this->ReadStepperAnswer(StepperMessage);
	}

	GetPositionMessage(StepperMessage, vRawMessage);

	if ( !ack )
	{
		IsStepperACKRecieved(StepperMessage, command);
	}

#if STEPPERDEBUGMSG
			std::cout<< "\n" << std::flush; 
#endif
}

//----------------------------------------------------------------------------
void CmsBrachyStepper::StepperInstruction(STEPPERCOMMAND command)
{
	const char* p = command; 

	m_StepperCOMPort->Write(STX);

#if STEPPERDEBUGMSG
	std::cout << "->" << std::flush; 
#endif

	while (*p != 0)
	{
		m_StepperCOMPort->Write((BYTE)*p);

#if STEPPERDEBUGMSG
		BYTE b  = (byte)*p;
		std::cout << b << std::flush; 
#endif
		p++;
}

	m_StepperCOMPort->Write(ETX);

#if STEPPERDEBUGMSG
	std::cout << " " << std::flush; 
#endif

}

//----------------------------------------------------------------------------
void CmsBrachyStepper::ReadStepperAnswer(std::vector<BYTE> &stepperAnswer)
{
	stepperAnswer.clear(); 
	BYTE buff = 0;

#if STEPPERDEBUGMSG
	std::cout << "<-" << std::flush; 
#endif

	while ( m_StepperCOMPort->Read(buff) && buff != ETX)
	{
		if (stepperAnswer.max_size() > stepperAnswer.size())
		{
			stepperAnswer.push_back(buff);
		}
#if STEPPERDEBUGMSG
		std::cout << buff << std::flush;
#endif

	}

	if (buff == ETX)
	{
		stepperAnswer.push_back(buff);
	}

#if STEPPERDEBUGMSG
	std::cout << " " << std::flush;
#endif

}

//----------------------------------------------------------------------------
bool CmsBrachyStepper::IsStepperACKRecieved(std::vector<BYTE> &ackMessage, STEPPERCOMMAND command )
{
	this->ReadStepperAnswer(ackMessage);

	if (ackMessage.size() >= 5 
		&& 
		( ackMessage[1] == 'F' && ackMessage[2] == 'A' ) )
	{
		
		if ( command[0] == ackMessage[3] && command[1] == ackMessage[4] )
		{
			// recieved the expected ack 
			return PLUS_SUCCESS;
		}
		else
		{
			// recieved another ACK read the message and respond
			std::vector<BYTE> StepperMessage; 
			this->ReadStepperAnswer(StepperMessage);
			this->SendAckMessage(StepperMessage);
			return PLUS_FAIL; 
		}
	}

	return PLUS_FAIL;
}

//----------------------------------------------------------------------------
void CmsBrachyStepper::GetStepperMessage(STEPPERCOMMAND command, const char* Response, std::vector<BYTE> &DecodedMessage )
{
	DecodedMessage.clear(); 

	std::vector<BYTE> StepperMessage;
	StepperMessage.reserve(50);

	bool ack = IsStepperACKRecieved(StepperMessage, command); 

	if ( ack )
	{
		StepperMessage.clear(); 
		this->ReadStepperAnswer(StepperMessage);
	}

	if (StepperMessage.size() >= 3 ) 
	{
		if (StepperMessage[0] == STX && 
			StepperMessage[1] == Response[0] && 
			StepperMessage[2] == Response[1])
		{
			DecodeStepperMessage(StepperMessage, DecodedMessage);
		}
		
		this->SendAckMessage(StepperMessage);
	}

	if ( !ack )
	{
		IsStepperACKRecieved(StepperMessage, command);
	}

#if STEPPERDEBUGMSG
	std::cout<< "\n" << std::flush; 
#endif

}

//----------------------------------------------------------------------------
void CmsBrachyStepper::SendAckMessage(std::vector<BYTE> StepperMessage)
{
	// Don't respond ack to an ack message
	if ( StepperMessage.size() < 2 || StepperMessage[1] == 'F' )
	{
		return; 
	}

	std::vector<BYTE> DecodedMessage;

	this->DecodeStepperMessage(StepperMessage, DecodedMessage);

	std::string message; 
	this->CreateAckMessage(DecodedMessage[0], &message);
	this->StepperInstruction(static_cast<STEPPERCOMMAND>(message.c_str()));
}

//----------------------------------------------------------------------------
void CmsBrachyStepper::GetPositionMessage(std::vector<BYTE> &StepperMessage, std::vector<BYTE> &DecodedMessage)
{
	DecodedMessage.clear(); 

	if (StepperMessage.size() >= 3 ) 
	{
		if ( StepperMessage[0] == STX && 
			(
			(StepperMessage[1] == SRC_POSITION_DATA_BUTTON_ON[0] && StepperMessage[2] == SRC_POSITION_DATA_BUTTON_ON[1])
			||
			(StepperMessage[1] == SRC_POSITION_DATA_BUTTON_OFF[0] && StepperMessage[2] == SRC_POSITION_DATA_BUTTON_OFF[1])
			||
			(StepperMessage[1] == SRC_POSITION_DATA_BUTTON_LATCH[0] && StepperMessage[2] == SRC_POSITION_DATA_BUTTON_LATCH[1])
			))
		{
			DecodeStepperMessage(StepperMessage, DecodedMessage);
			std::string message; 
			CreateAckMessage(DecodedMessage[0], &message);
			this->StepperInstruction(static_cast<STEPPERCOMMAND>(message.c_str()));
			return; 
		}
		else if (StepperMessage[0] == STX 
			&& 
			( StepperMessage[1] == SRC_COUNTERS_INVALID[0] && StepperMessage[2] == SRC_COUNTERS_INVALID[1]) )
		{
			this->SetScalingParameters(); 
			this->SendAckMessage(StepperMessage);
			return; 

		}
			// Wrong message received 
			this->SendAckMessage(StepperMessage);
	}

}

//----------------------------------------------------------------------------
void CmsBrachyStepper::DecodeStepperMessage(std::vector<BYTE> StepperMessage, std::vector<BYTE> &DecodedMessage)
{
	DecodedMessage.clear(); 
	DecodedMessage.reserve(50);

	if ( StepperMessage.size() < 3 )
	{
		return; 
	}

	unsigned int i = 1; 
	unsigned int msglen = this->AsciiToBin(StepperMessage[i], StepperMessage[i+1]);
	DecodedMessage.push_back(msglen);
	msglen = msglen & 0x7;

	for( msglen ; msglen != 0; --msglen)
	{
		i = i+2;
		if ( StepperMessage.size() > i+1)
		{
			DecodedMessage.push_back(this->AsciiToBin(StepperMessage[i], StepperMessage[i+1]));
		}
	}
}

//----------------------------------------------------------------------------
void CmsBrachyStepper::SetScalingParameters()
{
	// get reference data for scaling
	double count = 0, dist = 0, scale = 0;

	this->GetProbeReferenceData(count, dist, scale);
	this->GetGridReferenceData(count, dist, scale);
	this->GetRotationReferenceData(count, dist, scale);
}

//----------------------------------------------------------------------------
void CmsBrachyStepper::ClearBuffer()
{
	BYTE buff;
	while ( m_StepperCOMPort->Read(buff)){}
}

//----------------------------------------------------------------------------
void CmsBrachyStepper::CreateAckMessage(BYTE opcode, std::string *sMessage)
{
	unsigned int ck;
	unsigned char c1,c2, c3, c4, c5, c6;

	ck = 0xfa + static_cast<unsigned int>(opcode);
	ck &= 0xff;              
	this->BinToAscii(0xfa,&c1,&c2);
	sMessage->push_back(c1);
	sMessage->push_back(c2);

	this->BinToAscii(opcode,&c3,&c4);
	sMessage->push_back(c3);
	sMessage->push_back(c4);

	this->BinToAscii(ck,&c5,&c6);
	sMessage->push_back(c5);
	sMessage->push_back(c6);

}

//----------------------------------------------------------------------------
unsigned int CmsBrachyStepper::AsciiToBin(unsigned int i, unsigned int j)
{
	i -= 0x30;
	if (i > 9)
	{
		i -= 7;
	}

	j -= 0x30;
	if (j > 9) 
	{
		j -= 7;
	}

	return (i*16 + j);
}

//----------------------------------------------------------------------------
void CmsBrachyStepper::BinToAscii(unsigned int n, unsigned char *c1, unsigned char *c2)
{                                                                                                                                         
	unsigned int t1,t2;
	t1 = (n/16);
	t2 = (n%16);
	t1 += 0x30;

	if (t1 > 0x39)
	{
		t1 += 7;
	}

	t2 += 0x30;             

	if (t2 > 0x39)
	{
		t2 += 7;
	}

	*c1 = (char)t1;
	*c2 = (char)t2;
}                 

