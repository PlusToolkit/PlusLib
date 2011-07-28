#include "CivcoBrachyStepper.h"


//----------------------------------------------------------------------------
CivcoBrachyStepper::CivcoBrachyStepper()
{

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
PlusStatus CivcoBrachyStepper::GetDeviceInfo( std::string& version, std::string& model, std::string& serial )
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

}
