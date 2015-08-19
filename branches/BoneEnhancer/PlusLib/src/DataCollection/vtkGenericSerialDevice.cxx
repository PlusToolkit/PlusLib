/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkGenericSerialDevice.h"

#include "SerialLine.h"
#include "vtkObjectFactory.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLUtilities.h"
#include "vtksys/SystemTools.hxx"

#include <deque>

vtkStandardNewMacro(vtkGenericSerialDevice);

//-------------------------------------------------------------------------
void bin2hex(const std::string& inputBinary, std::string& outputHexEncoded)
{
  std::stringstream ss;
  ss << std::hex << std::setfill('0');
  for (int i=0; i<inputBinary.size(); i++)
  {
    if (i>0)
    {
      ss << " ";
    }
    ss << std::setw(2) << int(inputBinary[i]);    
  }
  outputHexEncoded = ss.str();
}

void hex2bin(const std::string& inputHexEncoded, std::string& outputBinary)
{
  outputBinary.clear();
  int i;
  std::stringstream ss(inputHexEncoded);
  while (ss >> std::hex >> i)
  {
    outputBinary+=static_cast<char>(i);
  }
}

//-------------------------------------------------------------------------
vtkGenericSerialDevice::vtkGenericSerialDevice()
: SerialPort(1)
, BaudRate(9600)
, MaximumReplyDelaySec(0.100)
, MaximumReplyDurationSec(0.300)
, Mutex(vtkSmartPointer<vtkRecursiveCriticalSection>::New())
{ 
  this->Serial = new SerialLine();
 
  // By default use CR as line ending (13, 0x0D)
  this->SetLineEnding("0d");

  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates=true;
  this->AcquisitionRate = 10;
}

//-------------------------------------------------------------------------
vtkGenericSerialDevice::~vtkGenericSerialDevice() 
{
  if ( this->Recording )
  {
    this->StopRecording();
  }

  if (this->Serial->IsHandleAlive())
  {
    this->Serial->Close();
    delete this->Serial; 
    this->Serial = NULL; 
  }
}

//-------------------------------------------------------------------------
void vtkGenericSerialDevice::PrintSelf( ostream& os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );
}

//-------------------------------------------------------------------------
PlusStatus vtkGenericSerialDevice::InternalConnect()
{
  LOG_TRACE( "vtkGenericSerialDevice::Connect" ); 

  if (this->Serial->IsHandleAlive())
  {
    LOG_ERROR("Already connected to serial port");
    return PLUS_FAIL;
  }

  // COM port name format is different for port number under/over 10 (see Microsoft KB115831)
  // Port number<10: COMn
  // Port number>=10: \\.\COMn
  std::ostringstream strComPort; 
  if (this->SerialPort<10)
  {
    strComPort << "COM" << this->SerialPort;
  }
  else
  {
    strComPort << "\\\\.\\COM" << this->SerialPort;
  }  
  this->Serial->SetPortName(strComPort.str()); 

  this->Serial->SetSerialPortSpeed(this->BaudRate); 

  this->Serial->SetMaxReplyTime(50); // msec

  if (!this->Serial->Open())
  {
    LOG_ERROR("Cannot open serial port "<<strComPort.str());
    return PLUS_FAIL;
  }

  if (!this->Serial->IsHandleAlive())  
  {  
    LOG_ERROR("COM port handle is not alive "<<strComPort.str());
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS; 
}

//-------------------------------------------------------------------------
PlusStatus vtkGenericSerialDevice::InternalDisconnect()
{
  LOG_TRACE( "vtkGenericSerialDevice::Disconnect" ); 
  this->StopRecording();

  this->Serial->Close();

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkGenericSerialDevice::Probe()
{
  LOG_ERROR("vtkGenericSerialDevice::Probe is not implemented");
  return PLUS_SUCCESS; 
} 

//-------------------------------------------------------------------------
PlusStatus vtkGenericSerialDevice::InternalStartRecording()
{
  LOG_TRACE( "vtkGenericSerialDevice::InternalStartRecording" ); 
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkGenericSerialDevice::InternalStopRecording()
{
  LOG_TRACE( "vtkGenericSerialDevice::InternalStopRecording" );
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkGenericSerialDevice::InternalUpdate()
{
  // Either update or send commands - but not simultaneously
  PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);

  // Determine the maximum time to spend in the loop (acquisition time period, but maximum 1 sec)
  double maxReadTimeSec = (this->AcquisitionRate < 1.0) ? 1.0 : 1/this->AcquisitionRate;
  double startTime = vtkAccurateTimer::GetSystemTime();
  while (this->Serial->GetNumberOfBytesAvailableForReading()>0)
  { 
    std::string textReceived;
    ReceiveResponse(textReceived);
    LOG_DEBUG("Received from serial device without request: "<<textReceived);
    if (vtkAccurateTimer::GetSystemTime()-startTime>maxReadTimeSec)
    {
      // force exit from the loop if continuously receiving data
      break;
    }
  }
  return PLUS_SUCCESS;
}


//-------------------------------------------------------------------------
PlusStatus vtkGenericSerialDevice::SendText(const std::string& textToSend, std::string* textReceived/*=NULL*/)
{
  LOG_DEBUG("Send to Serial device: "<<textToSend);
  
  // Either update or send commands - but not simultaneously
  PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);

  // Write text
  unsigned char packetLength=textToSend.size();
  for( int i = 0; i < packetLength; i++ )
  {
    this->Serial->Write(textToSend[i]);
  }
  // Write line ending
  for (std::string::iterator lineEndingIt=this->LineEndingBin.begin(); lineEndingIt!=this->LineEndingBin.end(); ++lineEndingIt)
  {
    this->Serial->Write(*lineEndingIt);
  }
  // Get response
  if (textReceived!=NULL)
  {
    textReceived->clear();
    this->WaitForResponse();
    while (this->Serial->GetNumberOfBytesAvailableForReading()>0)
    { 
      // a response is expected
      std::string line;
      if (this->ReceiveResponse(line)!=PLUS_SUCCESS)
      {
        *textReceived+=line;
        break;
      }
      *textReceived+=line;
    }
    LOG_DEBUG("Received from serial device: "<<(*textReceived));
  }
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
bool vtkGenericSerialDevice::WaitForResponse()
{
  const int waitPeriodSec = 0.010;
  
  double startTime = vtkAccurateTimer::GetSystemTime();
  while (this->Serial->GetNumberOfBytesAvailableForReading()==0)
  {
    if (vtkAccurateTimer::GetSystemTime()-startTime>this->MaximumReplyDelaySec)
    {
      // waiting time expired
      return false;
    }
    vtksys::SystemTools::Delay( waitPeriodSec * 1000 );
  }
  // data available for reading
  return true;
}

//-------------------------------------------------------------------------
PlusStatus vtkGenericSerialDevice::ReceiveResponse(std::string& textReceived)
{
  const int waitPeriodSec = 0.005;

  textReceived.clear();
  double startTime = vtkAccurateTimer::GetSystemTime();
  
  // Read the the response (until line ending is found or timeout)
  unsigned int lineEndingLength = this->LineEndingBin.size();
  unsigned char d=0;  
  bool lineEndingFound=false;
  do
  {
    while (!this->Serial->Read(d))
    {
      if (vtkAccurateTimer::GetSystemTime()-startTime>this->MaximumReplyDurationSec)
      {
        // waiting time expired
        LOG_ERROR("Failed to read complete line from serial device. Received: "<<textReceived);
        return PLUS_FAIL;
      }
    }
    textReceived.push_back(d);
    lineEndingFound = textReceived.size()>=lineEndingLength
      && (this->LineEndingBin.compare(textReceived.substr(textReceived.size()-lineEndingLength,lineEndingLength)) == 0);

  } while (!lineEndingFound);

  // Remove line ending
  textReceived.erase(textReceived.size()-lineEndingLength,lineEndingLength);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkGenericSerialDevice::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  XML_READ_SCALAR_ATTRIBUTE_REQUIRED(unsigned long, SerialPort, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(unsigned long, BaudRate, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, MaximumReplyDelaySec, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, MaximumReplyDurationSec, deviceConfig);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(LineEnding, deviceConfig);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkGenericSerialDevice::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
  deviceConfig->SetUnsignedLongAttribute( "SerialPort", this->SerialPort ); 
  deviceConfig->SetUnsignedLongAttribute( "BaudRate", this->BaudRate );
  deviceConfig->SetDoubleAttribute( "MaximumReplyDelaySec", this->MaximumReplyDelaySec );
  deviceConfig->SetDoubleAttribute( "MaximumReplyDurationSec", this->MaximumReplyDurationSec );
  deviceConfig->SetAttribute( "LineEnding", this->LineEnding.c_str() );
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkGenericSerialDevice::SetLineEnding(const char* lineEndingHex)
{
  this->LineEnding=lineEndingHex;
  hex2bin(this->LineEnding, this->LineEndingBin);
}
