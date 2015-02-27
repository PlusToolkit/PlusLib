/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "SerialLine.h"
#include <assert.h>

SerialLine::SerialLine()
{
  m_MaxReplyTime=1000;
  m_SerialPortSpeed=9600;
  m_CommHandle=INVALID_HANDLE_VALUE;
}

SerialLine::~SerialLine()
{
  if (m_CommHandle!=INVALID_HANDLE_VALUE)
  {
    Close();
  }
}

void SerialLine::Close()
{
#ifdef _WIN32
  if (m_CommHandle!=INVALID_HANDLE_VALUE)
  {
    CloseHandle(m_CommHandle);
  }
#endif  
  m_CommHandle=INVALID_HANDLE_VALUE;
}

bool SerialLine::Open()
{
  if (m_CommHandle!=INVALID_HANDLE_VALUE)
  {
    Close();
  }

  // Open serial port
#ifdef _WIN32
  m_CommHandle = CreateFile(m_PortName.c_str(),
    GENERIC_READ|GENERIC_WRITE,
    0,  // not allowed to share ports
    0,  // child-processes don't inherit handle
    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
    NULL); /* no template file */
  if (m_CommHandle==INVALID_HANDLE_VALUE)
  {
    return false;
  }

  if (!SetupComm(m_CommHandle,16384,16384))
  {
    Close();
    return false;
  }
  DCB dcb;
  GetCommState(m_CommHandle,&dcb);
  dcb.BaudRate=m_SerialPortSpeed;
  dcb.Parity=NOPARITY;
  dcb.ByteSize=8;
  dcb.StopBits=ONESTOPBIT;
  if (!SetCommState(m_CommHandle,&dcb))
  {
    Close();
    return false;
  }

  COMMTIMEOUTS timeouts;
  GetCommTimeouts(m_CommHandle,&timeouts);
  timeouts.ReadIntervalTimeout=200;
  timeouts.ReadTotalTimeoutConstant=m_MaxReplyTime;
  timeouts.ReadTotalTimeoutMultiplier=100;
  timeouts.WriteTotalTimeoutConstant=m_MaxReplyTime;
  timeouts.WriteTotalTimeoutMultiplier=100;
  if (!SetCommTimeouts(m_CommHandle,&timeouts))
  {
    Close();
    return false;
  }

  return true;
#else
  LOG_ERROR("SerialLine::Open() is only implemented on Windows");
  m_CommHandle=INVALID_HANDLE_VALUE;
  return false;
#endif  
}

int SerialLine::Write(const BYTE* data, int numberOfBytesToWrite)
{
#ifdef _WIN32
  int numberOfBytesWrittenTotal = 0;
  while (numberOfBytesToWrite > 0)
  {
    DWORD numberOfBytesWritten = 0;
    if (WriteFile(m_CommHandle,&data[numberOfBytesWrittenTotal],numberOfBytesToWrite,&numberOfBytesWritten,NULL) == FALSE)
    {
      if (GetLastError() == ERROR_OPERATION_ABORTED)
      {
        // system error: clear error and retry
        DWORD errors=0;
        ClearCommError(m_CommHandle,&errors,NULL);
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

bool SerialLine::Write(const BYTE data)
{
  return Write(&data, 1) == 1;
}

int SerialLine::Read(BYTE *data, int maxNumberOfBytesToRead)
{
#ifdef _WIN32
  int numberOfBytesReadTotal = 0;
  while (maxNumberOfBytesToRead > 0)  
  {
    DWORD numberOfBytesRead;
    if (ReadFile(m_CommHandle,&data[numberOfBytesReadTotal],maxNumberOfBytesToRead,&numberOfBytesRead,NULL) == FALSE) 
    { 
      DWORD lastError = GetLastError();
      if (lastError == ERROR_OPERATION_ABORTED) 
      {
        // system error: clear error and retry
        DWORD errors=0;
        ClearCommError(m_CommHandle,&errors,NULL);
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

bool SerialLine::Read(BYTE &data)
{
  return Read(&data,1)==1;
}

SerialLine::DWORD SerialLine::ClearError()
{
#ifdef _WIN32
  DWORD dwErrors=0;
  COMSTAT comStat;
  ClearCommError(m_CommHandle,&dwErrors, &comStat);
  return dwErrors;
#else
  LOG_ERROR("SerialLine::ClearError() is only implemented on Windows");
  return 0;
#endif   
}

unsigned int SerialLine::GetNumberOfBytesAvailableForReading() const
{
#ifdef _WIN32
  DWORD dwErrorFlags=0;
  COMSTAT comStat;
  ClearCommError( m_CommHandle, &dwErrorFlags, &comStat );
  return( (int) comStat.cbInQue );
#else
  LOG_ERROR("SerialLine::GetNumberOfBytesAvailableForReading() is only implemented on Windows");
  return 0;
#endif     
}
