/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "PlusSerialLine.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusGenericSerialDevice.h"

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkXMLDataElement.h>
#include <vtkXMLUtilities.h>
#include <vtksys/SystemTools.hxx>

// STL includes
#include <deque>

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusGenericSerialDevice);

//----------------------------------------------------------------------------
void bin2hex(const std::string& inputBinary, std::string& outputHexEncoded)
{
  std::stringstream ss;
  ss << std::hex << std::setfill('0');
  for (unsigned int i = 0; i < inputBinary.size(); i++)
  {
    if (i > 0)
    {
      ss << " ";
    }
    ss << std::setw(2) << int(inputBinary[i]);
  }
  outputHexEncoded = ss.str();
}

//----------------------------------------------------------------------------
void hex2bin(const std::string& inputHexEncoded, std::string& outputBinary)
{
  outputBinary.clear();
  int i;
  std::stringstream ss(inputHexEncoded);
  while (ss >> std::hex >> i)
  {
    outputBinary += static_cast<char>(i);
  }
}

//----------------------------------------------------------------------------
vtkPlusGenericSerialDevice::vtkPlusGenericSerialDevice()
  : Serial(new SerialLine())
  , SerialPort(1)
  , BaudRate(9600)
  , DTR(false)
  , RTS(false)
  , MaximumReplyDelaySec(0.100)
  , MaximumReplyDurationSec(0.300)
  , Mutex(vtkSmartPointer<vtkIGSIORecursiveCriticalSection>::New())
  , FrameNumber(0)
  , FieldDataSource(nullptr)
{
  // By default use CR as line ending (13, 0x0D)
  this->SetLineEnding("0d");

  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates = true;
  this->AcquisitionRate = 10;
}

//-------------------------------------------------------------------------
vtkPlusGenericSerialDevice::~vtkPlusGenericSerialDevice()
{
  if (this->Recording)
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
void vtkPlusGenericSerialDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusGenericSerialDevice::InternalConnect()
{
  LOG_TRACE("vtkPlusGenericSerialDevice::Connect");

  if (this->Serial->IsHandleAlive())
  {
    LOG_ERROR("Already connected to serial port");
    return PLUS_FAIL;
  }

  // COM port name format is different for port number under/over 10 (see Microsoft KB115831)
  // Port number<10: COMn
  // Port number>=10: \\.\COMn
  std::ostringstream strComPort;
  if (this->SerialPort < 10)
  {
    strComPort << "COM" << this->SerialPort;
  }
  else
  {
    strComPort << "\\\\.\\COM" << this->SerialPort;
  }
  this->Serial->SetPortName(strComPort.str());

  this->Serial->SetSerialPortSpeed(this->BaudRate);

  this->Serial->SetMaxReplyTime(50);   // msec

  if (!this->Serial->Open())
  {
    LOG_ERROR("Cannot open serial port " << strComPort.str());
    return PLUS_FAIL;
  }

  if (!this->Serial->IsHandleAlive())
  {
    LOG_ERROR("COM port handle is not alive " << strComPort.str());
    return PLUS_FAIL;
  }

  if (!this->SetDTR(this->DTR)) //re-set it in case we are reconnecting
  {
    LOG_ERROR("Could not re-establish DTR (data-terminal-ready) line ");
    return PLUS_FAIL;
  }

  if (!this->SetRTS(this->RTS)) //re-set it in case we are reconnecting
  {
    LOG_ERROR("Could not re-establish RTS (request-to-send) line ");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusGenericSerialDevice::InternalDisconnect()
{
  LOG_TRACE("vtkPlusGenericSerialDevice::Disconnect");
  this->StopRecording();

  this->Serial->Close();

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusGenericSerialDevice::InternalUpdate()
{
  // Either update or send commands - but not simultaneously
  igsioLockGuard<vtkIGSIORecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);

  // Determine the maximum time to spend in the loop (acquisition time period, but maximum 1 sec)
  double maxReadTimeSec = (this->AcquisitionRate < 1.0) ? 1.0 : 1 / this->AcquisitionRate;
  double startTime = vtkIGSIOAccurateTimer::GetSystemTime();
  while (this->Serial->GetNumberOfBytesAvailableForReading() > 0)
  {
    std::string textReceived;
    ReceiveResponse(textReceived);
    LOG_DEBUG("Received from serial device without request: " << textReceived);
    if (vtkIGSIOAccurateTimer::GetSystemTime() - startTime > maxReadTimeSec)
    {
      // force exit from the loop if continuously receiving data
      break;
    }
  }
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusGenericSerialDevice::SetDTR(bool onOff)
{
  // Either update or send commands - but not simultaneously
  igsioLockGuard<vtkIGSIORecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);

  PlusStatus retval;

  if (onOff == this->DTR)
  {
    return PLUS_SUCCESS; //already the desired value
  }
  else
  {
    retval = this->Serial->SetDTR(onOff);
    if (retval == PLUS_SUCCESS)
    {
      this->DTR = onOff;
    }
  }

  return retval;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusGenericSerialDevice::SetRTS(bool onOff)
{
  // Either update or send commands - but not simultaneously
  igsioLockGuard<vtkIGSIORecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);

  PlusStatus retval;

  if (onOff == this->RTS)
  {
    return PLUS_SUCCESS; //already the desired value
  }
  else
  {
    retval = this->Serial->SetRTS(onOff);
    if (retval == PLUS_SUCCESS)
    {
      this->RTS = onOff;
    }
  }

  return retval;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusGenericSerialDevice::GetDSR(bool & onOff)
{
  // Either update or send commands - but not simultaneously
  igsioLockGuard<vtkIGSIORecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);
  PlusStatus retval = this->Serial->GetDSR(onOff);
  return retval;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusGenericSerialDevice::GetCTS(bool & onOff)
{
  // Either update or send commands - but not simultaneously
  igsioLockGuard<vtkIGSIORecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);
  PlusStatus retval = this->Serial->GetCTS(onOff);
  return retval;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusGenericSerialDevice::SendText(const std::string& textToSend, std::string* textReceived/*=NULL*/,
  ReplyTermination acceptReply/*=REQUIRE_LINE_ENDING*/)
{
  LOG_DEBUG("Send to Serial device: " << textToSend);

  // Either update or send commands - but not simultaneously
  igsioLockGuard<vtkIGSIORecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);

  // Write text
  unsigned char packetLength = textToSend.size();
  for (int i = 0; i < packetLength; i++)
  {
    this->Serial->Write(textToSend[i]);
  }
  // Write line ending
  for (std::string::iterator lineEndingIt = this->LineEndingBin.begin(); lineEndingIt != this->LineEndingBin.end(); ++lineEndingIt)
  {
    this->Serial->Write(*lineEndingIt);
  }
  // Get response
  if (textReceived != NULL)
  {
    textReceived->clear();
    this->WaitForResponse();
    while (this->Serial->GetNumberOfBytesAvailableForReading() > 0)
    {
      // a response is expected
      std::string line;
      if (this->ReceiveResponse(line, acceptReply) != PLUS_SUCCESS)
      {
        *textReceived += line;
        break;
      }
      *textReceived += line;
    }
    LOG_DEBUG("Received from serial device: " << (*textReceived));
  }
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
bool vtkPlusGenericSerialDevice::WaitForResponse()
{
  const int waitPeriodSec = 0.010;

  double startTime = vtkIGSIOAccurateTimer::GetSystemTime();
  while (this->Serial->GetNumberOfBytesAvailableForReading() == 0)
  {
    if (vtkIGSIOAccurateTimer::GetSystemTime() - startTime > this->MaximumReplyDelaySec)
    {
      // waiting time expired
      return false;
    }
    vtksys::SystemTools::Delay(waitPeriodSec * 1000);
  }
  // data available for reading
  return true;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusGenericSerialDevice::ReceiveResponse(std::string& textReceived, ReplyTermination acceptReply/*=REQUIRE_LINE_ENDING*/)
{
  textReceived.clear();
  double startTime = vtkIGSIOAccurateTimer::GetSystemTime();

  // Read the the response (until line ending is found or timeout)
  unsigned int lineEndingLength = this->LineEndingBin.size();
  unsigned char d = 0;
  bool lineEndingFound = false;
  do
  {
    while (!this->Serial->Read(d))
    {
      if (vtkIGSIOAccurateTimer::GetSystemTime() - startTime > this->MaximumReplyDurationSec)
      {
        // waiting time expired
        if (acceptReply == REQUIRE_LINE_ENDING)
        {
          static vtkIGSIOLogHelper logHelper(60.0, 1e6);
          CUSTOM_RETURN_WITH_FAIL_IF(true, "Failed to get a proper response within configured time (" << this->MaximumReplyDurationSec << " sec)");
        }
        else if (acceptReply == REQUIRE_NOT_EMPTY && textReceived.empty())
        {
          RETURN_WITH_FAIL_IF(true, "Failed to read a complete line from serial device. Received: " << textReceived);
        }
        else
        {
          return PLUS_SUCCESS;
        }
      }
    }
    textReceived.push_back(d);
    lineEndingFound = textReceived.size() >= lineEndingLength
                      && (this->LineEndingBin.compare(textReceived.substr(textReceived.size() - lineEndingLength, lineEndingLength)) == 0);

  }
  while (!lineEndingFound);

  // Remove line ending
  textReceived.erase(textReceived.size() - lineEndingLength, lineEndingLength);

  // Store in frame
  if (this->FieldDataSource != nullptr)
  {
    igsioTrackedFrame::FieldMapType fieldMap;
    fieldMap[this->FieldDataSource->GetId()] = textReceived;
    this->FieldDataSource->AddItem(fieldMap, this->FrameNumber);
    this->FrameNumber++;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGenericSerialDevice::NotifyConfigured()
{
  if (this->GetFieldDataSourcessIteratorBegin() != this->GetFieldDataSourcessIteratorEnd())
  {
    this->FieldDataSource = this->Fields.begin()->second;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGenericSerialDevice::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  XML_READ_SCALAR_ATTRIBUTE_REQUIRED(unsigned long, SerialPort, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(unsigned long, BaudRate, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, MaximumReplyDelaySec, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, MaximumReplyDurationSec, deviceConfig);
  XML_READ_CSTRING_ATTRIBUTE_OPTIONAL(LineEnding, deviceConfig);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGenericSerialDevice::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
  deviceConfig->SetUnsignedLongAttribute("SerialPort", this->SerialPort);
  deviceConfig->SetUnsignedLongAttribute("BaudRate", this->BaudRate);
  deviceConfig->SetDoubleAttribute("MaximumReplyDelaySec", this->MaximumReplyDelaySec);
  deviceConfig->SetDoubleAttribute("MaximumReplyDurationSec", this->MaximumReplyDurationSec);
  deviceConfig->SetAttribute("LineEnding", this->LineEnding.c_str());
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusGenericSerialDevice::SetLineEnding(const char* lineEndingHex)
{
  this->LineEnding = lineEndingHex;
  hex2bin(this->LineEnding, this->LineEndingBin);
}
