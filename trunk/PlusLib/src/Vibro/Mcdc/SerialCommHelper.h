/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __SerialCommHelper_h
#define __SerialCommHelper_h

#include "SerialBuffer.h"
#include <map>
#include <tchar.h>

typedef enum tagSERIAL_STATE
{
  SS_Unknown,
  SS_UnInit,
  SS_Init,
  SS_Started ,
  SS_Stopped ,

} SERIAL_STATE;

/*!
\class CSerialCommHelper
\brief Class for reading and writing data through the serial (RS-232) port

\sa SerialLine
\ingroup PlusLibVibro
*/
class CSerialCommHelper  
{
public:
  CSerialCommHelper();
  virtual ~CSerialCommHelper();

  HANDLE GetWaitForEvent() {return m_hDataRx;} 

  HANDLE GetCommHandle(void) {return m_hCommPort;}

  inline void LockThis() {EnterCriticalSection ( &m_csLock );}	
  inline void UnLockThis() {LeaveCriticalSection (&m_csLock); }
  inline void InitLock() {InitializeCriticalSection (&m_csLock );}
  inline void DelLock() {DeleteCriticalSection (&m_csLock );}
  inline bool IsInputAvailable()
  {
    LockThis (); 
    bool abData = ( !m_theSerialBuffer.IsEmpty() ) ;
    UnLockThis (); 
    return abData;
  } 
  inline bool IsConnection() {return m_abIsConnected; }
  inline void SetDataReadEvent() { SetEvent(m_hDataRx); }

  HRESULT Read_N(std::string& data,long alCount,long alTimeOut);
  HRESULT Read_Upto(std::string& data,char chTerminator ,long	* alCount,long alTimeOut);
  /*!
    Reads all the data that is available in the local buffer.. 
	  does NOT make any blocking calls in case the local buffer is empty
  */
  HRESULT ReadAvailable(std::string& data);
  HRESULT Write(const char* data);
  HRESULT Write(const char* data,DWORD dwSize);
  HRESULT Init(std::string szPortName= _T("COM1"), DWORD dwBaudRate = 9600, BYTE byParity = 0, BYTE byStopBits = 1, BYTE byByteSize  = 8);
  HRESULT Start();
  HRESULT Purge();
  HRESULT Stop();
  HRESULT UnInit();

protected:
  static unsigned __stdcall ThreadFn(void*pvParam);

  HRESULT  CanProcess();
  void OnSetDebugOption(long  iOpt,BOOL bOnOff);	

private:
  SERIAL_STATE	m_eState;
  HANDLE	m_hCommPort;
  HANDLE	m_hThreadTerm ;
  HANDLE	m_hThread;
  HANDLE	m_hThreadStarted;
  HANDLE	m_hDataRx;
  bool	m_abIsConnected;
  void	InvalidateHandle(HANDLE& hHandle);
  void	CloseAndCleanHandle(HANDLE& hHandle) ;

  CSerialBuffer m_theSerialBuffer;
  CRITICAL_SECTION m_csLock;
  SERIAL_STATE GetCurrentState() {return m_eState;}
};

#endif // __SerialCommHelper_h