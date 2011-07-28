#include "CivcoBrachyStepper.h"
#include "SEIDrv32.h"

//----------------------------------------------------------------------------
CivcoBrachyStepper::CivcoBrachyStepper()
{
  this->m_COMPort = 0; 
}

//----------------------------------------------------------------------------
CivcoBrachyStepper::CivcoBrachyStepper(unsigned long COMPort, unsigned long BaudRate)
{

}

//----------------------------------------------------------------------------
CivcoBrachyStepper::~CivcoBrachyStepper()
{

}

//----------------------------------------------------------------------------
PlusStatus CivcoBrachyStepper::Connect()
{

  // SEI Initialization.
  // Start the SEI Server Program, and look for devices on the SEI bus
  // the zero means to look on all com ports, and the AUTOASSIGN means
  // that if there are address conflicts on the SEI bus, the device
  // addresses will automatically be reassigned so there are no conflicts
  // Initialization.
	
	if ( InitializeSEI(this->m_COMPort , REINITIALIZE | AUTOASSIGN) != 0 )
  {
    LOG_ERROR("Failed to initialize SEI!"); 
    return PLUS_FAIL; 
  }


  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus CivcoBrachyStepper::Disconnect()
{

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus CivcoBrachyStepper::GetEncoderValues(double &PPosition, 
    double &GPosition, double &RPosition, unsigned long &PositionRequestNumber)
{

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus CivcoBrachyStepper::GetDeviceModelInfo( std::string& version, std::string& model, std::string& serial )
{
  
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus CivcoBrachyStepper::ResetStepper()
{

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus CivcoBrachyStepper::CalibrateStepper(std::string &CalibMsg)
{

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus CivcoBrachyStepper::IsStepperAlive()
{

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
void CivcoBrachyStepper::SetBaudRate(unsigned long BaudRate)
{

}

//----------------------------------------------------------------------------
void CivcoBrachyStepper::SetCOMPort(unsigned long COMPort)
{
  this->m_COMPort = COMPort; 
}

//----------------------------------------------------------------------------
long CivcoBrachyStepper::GetNumberOfSeiDevices()
{
  return GetNumberOfDevices(); 
}

//----------------------------------------------------------------------------
PlusStatus CivcoBrachyStepper::GetSeiDeviceInfo(long devnum, long &model, long &serialnum, long &version, long &addr)
{
  if ( GetDeviceInfo(devnum, &model, &serialnum, &version, &addr) != 0 )
  {
    LOG_ERROR("Failed to get device info from SEI with device number: " << devnum ); 
    return PLUS_FAIL; 
  }
  return PLUS_SUCCESS; 
}

