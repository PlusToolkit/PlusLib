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

#ifdef _WIN32
  memset(&m_osReadWrite,0,sizeof(m_osReadWrite));
  m_osReadWrite.hEvent = CreateEvent(NULL, true, false, NULL);
  assert(m_osReadWrite.hEvent!=NULL);  // Error creating overlapped event handle.
#endif

}

SerialLine::~SerialLine()
{
#ifdef _WIN32
  CloseHandle(m_osReadWrite.hEvent);
#endif
  if (m_CommHandle!=INVALID_HANDLE_VALUE)
    Close();
}

void SerialLine::Close()
{
#ifdef _WIN32
  if (m_CommHandle!=INVALID_HANDLE_VALUE)
    CloseHandle(m_CommHandle);
#endif  
  m_CommHandle=INVALID_HANDLE_VALUE;
}

bool SerialLine::Open()
{
  if (m_CommHandle!=INVALID_HANDLE_VALUE)
    Close();
  // Open serial port
#ifdef _WIN32
  m_CommHandle=CreateFile(m_PortName.c_str(),GENERIC_READ|GENERIC_WRITE,0,0,OPEN_EXISTING,
            FILE_FLAG_OVERLAPPED,NULL);
  if (m_CommHandle==INVALID_HANDLE_VALUE)
    return false;
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

int SerialLine::Write(const BYTE data)
{
#ifdef _WIN32
  DWORD dwWritten;
  // Create this writes OVERLAPPED structure hEvent.
  ResetEvent(m_osReadWrite.hEvent);
  // Issue write.
    if (!WriteFile(m_CommHandle, &data, 1, &dwWritten, &m_osReadWrite)) 
    {
      if (GetLastError() != ERROR_IO_PENDING) { 
        // WriteFile failed, but it isn't delayed. Report error and abort.
        return 0;
      }
      else
      {
        // Write is pending.
        if (!GetOverlappedResult(m_CommHandle, &m_osReadWrite, &dwWritten, true))
          return 0;
        else
          // Write operation completed successfully.
          return dwWritten;
      }
    }
  // WriteFile completed immediately.
  return 1;
#else
  LOG_ERROR("SerialLine::Write() is only implemented on Windows");
  return 0;
#endif  
  
}

bool SerialLine::Read(BYTE &data)
{
#ifdef _WIN32
  ResetEvent(m_osReadWrite.hEvent);
  DWORD dwRead;
  
  // Issue read operation.
    if (!ReadFile(m_CommHandle, &data, 1, &dwRead, &m_osReadWrite)) 
    {
      if (GetLastError() != ERROR_IO_PENDING)     // read not delayed?
        return false; // Error in communications; report it.
    }
    else 
      return true;

    DWORD dwRes;  
    dwRes = WaitForSingleObject(m_osReadWrite.hEvent, m_MaxReplyTime);
    if (dwRes==WAIT_OBJECT_0)
    {
      // Read completed.
      if (!GetOverlappedResult(m_CommHandle, &m_osReadWrite, &dwRead, false))
        return false; // Error in communications; report it.
      else
        return dwRead==1; // Read completed successfully if read 1 byte.
    }
    return false;
#else
  LOG_ERROR("SerialLine::Read() is only implemented on Windows");
  return false;
#endif 
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
