/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/


#ifndef _RAWSERIALLINE_H_
#define _RAWSERIALLINE_H_
#include <windows.h>
#include <string>

/*!
\class SerialLine
\brief Class for reading and writing data through the serial (RS-232) port

\sa CSerialCommHelper
\ingroup PlusLibTracking
*/

class SerialLine  
{
public:
  typedef unsigned long       DWORD;
  typedef unsigned int		    UINT;
  typedef unsigned char       BYTE;

  SerialLine();
  virtual ~SerialLine();

  bool Open();
  void Close();

  int Write(const BYTE data);
  bool Read(BYTE &data);

  void SetPortName(const std::string &name) { m_PortName=name; };
  std::string GetPortName() const { return m_PortName; };

  void SetSerialPortSpeed(DWORD speed) { m_SerialPortSpeed=speed; };

  void SetMaxReplyTime(int maxreply) { m_MaxReplyTime=maxreply; };
  int GetMaxReplyTime() const { return m_MaxReplyTime; };

  bool IsHandleAlive() const { return (m_CommHandle != INVALID_HANDLE_VALUE); };

  DWORD ClearError();

private:
  HANDLE m_CommHandle;
  std::string m_PortName;
  DWORD m_SerialPortSpeed;
  int m_MaxReplyTime;
  int UpdateSerialBuffer();
  OVERLAPPED m_osReadWrite;
};

#endif 
