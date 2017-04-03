/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "PlusSerialLine.h"

//----------------------------------------------------------------------------
SerialLine::SerialLine()
  : MaxReplyTime(1000)
  , SerialPortSpeed(9600)
  , CommHandle(INVALID_HANDLE_VALUE)
{

}

//----------------------------------------------------------------------------
SerialLine::~SerialLine()
{
  if (CommHandle != INVALID_HANDLE_VALUE)
  {
    Close();
  }
}

//----------------------------------------------------------------------------
void SerialLine::Close()
{
#ifdef _WIN32
  if (CommHandle != INVALID_HANDLE_VALUE)
  {
    CloseHandle(CommHandle);
  }
#endif
  CommHandle = INVALID_HANDLE_VALUE;
}

//----------------------------------------------------------------------------
bool SerialLine::Open()
{
  if (CommHandle != INVALID_HANDLE_VALUE)
  {
    Close();
  }

  // Open serial port
#ifdef _WIN32
  CommHandle = CreateFile(this->PortName.c_str(),
                          GENERIC_READ | GENERIC_WRITE,
                          0,  // not allowed to share ports
                          0,  // child-processes don't inherit handle
                          OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                          NULL); /* no template file */
  if (CommHandle == INVALID_HANDLE_VALUE)
  {
    return false;
  }

  if (!SetupComm(CommHandle, 16384, 16384))
  {
    Close();
    return false;
  }
  DCB dcb;
  GetCommState(CommHandle, &dcb);
  dcb.BaudRate = SerialPortSpeed;
  dcb.Parity = NOPARITY;
  dcb.ByteSize = 8;
  dcb.StopBits = ONESTOPBIT;
  if (!SetCommState(CommHandle, &dcb))
  {
    Close();
    return false;
  }

  COMMTIMEOUTS timeouts;
  GetCommTimeouts(CommHandle, &timeouts);
  timeouts.ReadIntervalTimeout = 200;
  timeouts.ReadTotalTimeoutConstant = MaxReplyTime;
  timeouts.ReadTotalTimeoutMultiplier = 100;
  timeouts.WriteTotalTimeoutConstant = MaxReplyTime;
  timeouts.WriteTotalTimeoutMultiplier = 100;
  if (!SetCommTimeouts(CommHandle, &timeouts))
  {
    Close();
    return false;
  }

  return true;
#else
  LOG_ERROR("SerialLine::Open() is only implemented on Windows");
  CommHandle = INVALID_HANDLE_VALUE;
  return false;
#endif
}

//----------------------------------------------------------------------------
int SerialLine::Write(const BYTE* data, int numberOfBytesToWrite)
{
#ifdef _WIN32
  int numberOfBytesWrittenTotal = 0;
  while (numberOfBytesToWrite > 0)
  {
    DWORD numberOfBytesWritten = 0;
    if (WriteFile(CommHandle, &data[numberOfBytesWrittenTotal], numberOfBytesToWrite, &numberOfBytesWritten, NULL) == FALSE)
    {
      if (GetLastError() == ERROR_OPERATION_ABORTED)
      {
        // system error: clear error and retry
        DWORD errors = 0;
        ClearCommError(CommHandle, &errors, NULL);
      }
      else
      {
        // other error
        return numberOfBytesWritten;
      }
    }
    else if (numberOfBytesWritten == 0)
    {
      // no characters written, must have timed out
      return numberOfBytesWrittenTotal;
    }

    numberOfBytesToWrite -= numberOfBytesWritten;
    numberOfBytesWrittenTotal += numberOfBytesWritten;
  }
  return numberOfBytesWrittenTotal;
#else
  LOG_ERROR("SerialLine::Write() is only implemented on Windows");
  return 0;
#endif
}

//----------------------------------------------------------------------------
bool SerialLine::Write(const BYTE data)
{
  return Write(&data, 1) == 1;
}

//----------------------------------------------------------------------------
int SerialLine::Read(BYTE* data, int maxNumberOfBytesToRead)
{
#ifdef _WIN32
  int numberOfBytesReadTotal = 0;
  while (maxNumberOfBytesToRead > 0)
  {
    DWORD numberOfBytesRead;
    if (ReadFile(CommHandle, &data[numberOfBytesReadTotal], maxNumberOfBytesToRead, &numberOfBytesRead, NULL) == FALSE)
    {
      DWORD lastError = GetLastError();
      if (lastError == ERROR_OPERATION_ABORTED)
      {
        // system error: clear error and retry
        DWORD errors = 0;
        ClearCommError(CommHandle, &errors, NULL);
      }
      else
      {
        // other error
        return numberOfBytesReadTotal;
      }
    }
    else if (numberOfBytesRead == 0)
    {
      // no characters read, must have timed out
      return numberOfBytesReadTotal;
    }
    maxNumberOfBytesToRead -= numberOfBytesRead;
    numberOfBytesReadTotal += numberOfBytesRead;
  }
  return numberOfBytesReadTotal;
#else
  LOG_ERROR("SerialLine::Read() is only implemented on Windows");
  return 0;
#endif
}

//----------------------------------------------------------------------------
bool SerialLine::Read(BYTE& data)
{
  return Read(&data, 1) == 1;
}

//----------------------------------------------------------------------------
SerialLine::DWORD SerialLine::ClearError()
{
#ifdef _WIN32
  DWORD dwErrors = 0;
  COMSTAT comStat;
  ClearCommError(CommHandle, &dwErrors, &comStat);
  return dwErrors;
#else
  LOG_ERROR("SerialLine::ClearError() is only implemented on Windows");
  return 0;
#endif
}

//----------------------------------------------------------------------------
void SerialLine::SetSerialPortSpeed(DWORD speed)
{
  SerialPortSpeed = speed;
}

//----------------------------------------------------------------------------
void SerialLine::SetMaxReplyTime(int maxreply)
{
  MaxReplyTime = maxreply;
}

//----------------------------------------------------------------------------
int SerialLine::GetMaxReplyTime() const
{
  return MaxReplyTime;
}

//----------------------------------------------------------------------------
bool SerialLine::IsHandleAlive() const
{
  return (CommHandle != INVALID_HANDLE_VALUE);
}

//----------------------------------------------------------------------------
unsigned int SerialLine::GetNumberOfBytesAvailableForReading() const
{
#ifdef _WIN32
  DWORD dwErrorFlags = 0;
  COMSTAT comStat;
  ClearCommError(CommHandle, &dwErrorFlags, &comStat);
  return ((int) comStat.cbInQue);
#else
  LOG_ERROR("SerialLine::GetNumberOfBytesAvailableForReading() is only implemented on Windows");
  return 0;
#endif
}
