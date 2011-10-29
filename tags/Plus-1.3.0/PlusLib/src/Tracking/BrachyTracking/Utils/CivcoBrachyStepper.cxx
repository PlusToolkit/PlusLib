/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "CivcoBrachyStepper.h"
#include "SEIDrv32.h"
#include <sstream>

//----------------------------------------------------------------------------
CivcoBrachyStepper::CivcoBrachyStepper(unsigned long COMPort/*=1*/, unsigned long BaudRate/*=9600*/)
{
  this->m_COMPort = 0; 
  this->SetCOMPort(COMPort); 

  this->m_BaudRate = 0; 
  this->SetBaudRate(BaudRate); 

  this->m_DeviceNumber = 0; // Currently we assume only one device 
  this->m_DeviceAddress = 0; 
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
  if ( this->IsStepperAlive() != PLUS_SUCCESS )
  {
    if ( ::InitializeSEI(this->m_COMPort , REINITIALIZE | AUTOASSIGN | NORESET ) != 0 )
    {
      LOG_ERROR("Failed to initialize SEI! COMPort="<<this->m_COMPort); 
      return PLUS_FAIL; 
    }

    if ( this->SetBaudRate(this->m_BaudRate) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to set baud rate for SEI!"); 
      return PLUS_FAIL; 
    }
  }
  else
  {
    LOG_DEBUG("No need to initialize SEI, it's already connected!"); 
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
  else
  {
    LOG_WARNING("No device connected to ED2!"); 
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
  // Increase the m_PositionNumber on every position request
	PositionRequestNumber = ++m_PositionRequestNumber; 

  // Probe position 
  PPosition = 0.0; 
  // Grid position 
  GPosition = 0.0; // There is no optical encoder on template grid
  // Probe rotation 
  RPosition = 0.0;

  // Probe rotation encoder value
  long rotAngle(0); 
  if ( ::ED2GetPosition1(this->m_DeviceAddress, &rotAngle) != 0 )
  {
    LOG_ERROR("Failed to get position1 (rotation angle) from E2 device!"); 
    return PLUS_FAIL; 
  }

  RPosition = this->ConvertFloatToDecimal(rotAngle); 

  // Probe translation encoder value 
  long probePositionInCm(0); 
  if ( ::ED2GetPosition2(this->m_DeviceAddress, &probePositionInCm) != 0 )
  {
    LOG_ERROR("Failed to get position2 (probe position) from E2 device!"); 
    return PLUS_FAIL; 
  }

  PPosition = this->ConvertFloatToDecimal(probePositionInCm) * 10.0; 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
// IEEE Standard 754 floating point to double conversion 
// http://steve.hollasch.net/cgindex/coding/ieeefloat.html
double CivcoBrachyStepper::ConvertFloatToDecimal( long floatPoint )
{
  // Separate input number into the sign, exponent, and mantissa fields.
  int sign = ((floatPoint >> 31) == 0) ? 1 : -1;

  // Extract the exponent from the exponent field
  // The exponent field needs to represent both positive and negative exponents. 
  // To do this, a bias is added to the actual exponent in order to get the stored exponent.
  int exp = ((floatPoint >> 23) & 0xff);
  
  // Extract the mantissa from the mantissa field
  // The mantissa, also known as the significand, represents the precision bits 
  // of the number. It is composed of an implicit leading bit and the fraction bits.
  int m = (exp == 0) ? (floatPoint & 0x7fffff) << 1 : (floatPoint & 0x7fffff) | 0x800000;

  // Copy the binary numbers to char array 
  char bitset[25]; 
  _ltoa_s(m,bitset,25,2);
  
  // Compute the normalized mantissa in decimal format
  double mantissa = 0; 
  for ( int i = 0; i < 24; i++ )
  {
    const char bit = bitset[i];
    mantissa += atoi(&bit)*pow(2.0,-i); 
  }

  // subtract the bias to recover the actual exponent of two
  // the bias is 2^(k-1) - 1, where k is the number of bits in the exponent field, giving 127 for the 32-bit format
  const double bias = 127;

  // Convert the binary value to decimal
  // Set the sign of the decimal number according to the sign bit of the original floating point number: make it negative for 1; leave positive for 0.
  double decimalPoint = sign * mantissa * pow(2.0, exp - bias); 

  return decimalPoint; 
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
PlusStatus CivcoBrachyStepper::InitializeStepper(std::string &CalibMsg)
{
  // Set 0 encoder values
  CalibMsg = "Encoder values set to 0!"; 
  return this->ResetStepper(); 
}

//----------------------------------------------------------------------------
PlusStatus CivcoBrachyStepper::IsStepperAlive()
{
  if ( ::IsInitialized() != 1 )
  {
    // Device not yet initialized
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus CivcoBrachyStepper::SetBaudRate(unsigned long BaudRate)
{
  this->m_BaudRate = BaudRate; 

  if ( this->IsStepperAlive() == PLUS_SUCCESS )
  {
    if ( ::SetBaudRate(BaudRate) != 0 )
    {
      LOG_ERROR("Failed to set baud rate for SEI!"); 
      return PLUS_FAIL; 
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus CivcoBrachyStepper::SetCOMPort(unsigned long COMPort)
{
  if ( this->IsStepperAlive() == PLUS_SUCCESS )
  {
    // If the SEI server still running in the backgroud no need to change COM port
    LOG_DEBUG("Unable to set serial port number, stepper already connected!"); 
    return PLUS_FAIL; 
  }
  
  this->m_COMPort = COMPort; 
  return PLUS_SUCCESS;
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

