#include "CivcoBrachyStepper.h"
#include "SEIDrv32.h"
#include <sstream>

//----------------------------------------------------------------------------
CivcoBrachyStepper::CivcoBrachyStepper()
{
  this->m_COMPort = 0; 
  this->m_DeviceNumber = 0; // Currently we assume only one device 
}

//----------------------------------------------------------------------------
CivcoBrachyStepper::CivcoBrachyStepper(unsigned long COMPort, unsigned long BaudRate)
{
  this->m_COMPort = COMPort; 
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
	
  if ( ::InitializeSEI(this->m_COMPort , REINITIALIZE | AUTOASSIGN) != 0 )
  {
    LOG_ERROR("Failed to initialize SEI!"); 
    return PLUS_FAIL; 
  }

  if ( this->GetNumberOfSeiDevices() > 0 )
  {
    // We assume that only one device connected to bus
    const long devNumber = 0; 
    long lModel = 0;
    long lSerialNumber = 0;
    long lVersion = 0;
    long lAddress = 0;
    if ( this->GetSeiDeviceInfo(devNumber, lModel, lSerialNumber, lVersion, lAddress) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get SEI device info for device number: " << devNumber); 
      return PLUS_FAIL; 
    }

    this->m_DeviceAddress = lAddress; 
    this->m_DeviceNumber = devNumber; 
  }


  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus CivcoBrachyStepper::Disconnect()
{
  if ( ::IsInitialized() != 1 )
  {
    // Device not yet initialized
    return PLUS_SUCCESS; 
  }

  if ( ::CloseSEI() != 0 )
  {
    LOG_ERROR("Failed to close SEI!"); 
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus CivcoBrachyStepper::GetEncoderValues(double &PPosition, 
    double &GPosition, double &RPosition, unsigned long &PositionRequestNumber)
{
  // Probe position 
  PPosition = 0; 
  // Grid position 
  GPosition = 0; 
  // Probe rotation 
  RPosition = 0; 
  long pos1(0); 
  if ( ::ED2GetPosition1(this->m_DeviceAddress, &pos1) != 0 )
  {
    LOG_ERROR("Failed to get position1 from E2 device!"); 
    return PLUS_FAIL; 
  }
  
  long pos2(0); 
  if ( ::ED2GetPosition2(this->m_DeviceAddress, &pos2) != 0 )
  {
    LOG_ERROR("Failed to get position2 from E2 device!"); 
    return PLUS_FAIL; 
  }

  long resolution(0); 
  ::ED2GetResolution(this->m_DeviceAddress, &resolution); 

  LOG_INFO("Pos1: " << pos1 << "  Pos2: " << pos2 << "  Resolution: " << resolution); 
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus CivcoBrachyStepper::GetDeviceModelInfo( std::string& version, std::string& model, std::string& serial )
{
  long lModel = 0;
	long lSerialNumber = 0;
	long lVersion = 0;
	long lAddress = 0;
  if ( this->GetSeiDeviceInfo(this->m_DeviceNumber, lModel, lSerialNumber, lVersion, lAddress) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get SEI device info for device number: " << this->m_DeviceNumber ); 
    return PLUS_FAIL; 
  }

  // Set version info
  std::ostringstream strVersion; 
  strVersion << lVersion; 
  version = strVersion.str(); 

  // Set model info
  std::ostringstream strModel; 
  strModel << lModel; 
  model = strModel.str(); 

  // Set serial info
  std::ostringstream strSerial; 
  strModel << lModel; 
  serial = strModel.str(); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus CivcoBrachyStepper::ResetStepper()
{
  if ( ::ResetSEI() != 0 )
  {
    LOG_ERROR("Failed to reset SEI bus!"); 
    return PLUS_FAIL; 
  }

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
  if ( ::IsInitialized() != 1 )
  {
    // Device not yet initialized
    return PLUS_SUCCESS; 
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
void CivcoBrachyStepper::SetBaudRate(unsigned long BaudRate)
{
  if ( ::SetBaudRate(BaudRate) != 0 )
  {
    LOG_ERROR("Failed to set baud rate for SEI!"); 
  }
}

//----------------------------------------------------------------------------
void CivcoBrachyStepper::SetCOMPort(unsigned long COMPort)
{
  this->m_COMPort = COMPort; 
}

//----------------------------------------------------------------------------
long CivcoBrachyStepper::GetNumberOfSeiDevices()
{
  return ::GetNumberOfDevices(); 
}

//----------------------------------------------------------------------------
PlusStatus CivcoBrachyStepper::GetSeiDeviceInfo(long devnum, long &model, long &serialnum, long &version, long &addr)
{
  if ( ::GetDeviceInfo(devnum, &model, &serialnum, &version, &addr) != 0 )
  {
    LOG_ERROR("Failed to get device info from SEI with device number: " << devnum ); 
    return PLUS_FAIL; 
  }
  return PLUS_SUCCESS; 
}

