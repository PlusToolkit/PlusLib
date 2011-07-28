
#ifndef _RAWSERIALLINE_H_
#define _RAWSERIALLINE_H_
#include <windows.h>
#include <string>

class SerialLine  
{
public:


  typedef unsigned long       DWORD;
  typedef unsigned int		    UINT;
  typedef unsigned char       BYTE;


  DWORD ClearError();

  SerialLine();

  virtual ~SerialLine();

  bool Open();

  void Close();

  int Write(const BYTE data);

  bool Read(BYTE &data);

  void SetPortName(const std::string &name) { m_PortName=name; };

  std::string GetPortName() { return m_PortName; };

  void SetSerialPortSpeed(DWORD speed) { m_SerialPortSpeed=speed; };

  void SetMaxReplyTime(int maxreply) { m_MaxReplyTime=maxreply; };

  int GetMaxReplyTime() { return m_MaxReplyTime; };

  bool IsHandleAlive(){ return (m_CommHandle != INVALID_HANDLE_VALUE); };

private:
  HANDLE m_CommHandle;

  std::string m_PortName;

  UINT m_EolDelay;

  UINT m_ReplyTimeout;

  DWORD m_SerialPortSpeed;

  int m_MaxReplyTime;

  int UpdateSerialBuffer();

  OVERLAPPED m_osReadWrite;
};

#endif 
