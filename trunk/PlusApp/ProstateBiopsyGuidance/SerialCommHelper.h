// SerialCommHelper.h: interface for the CSerialCommHelper class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "SerialBuffer.h"
//#include "DebugDump.h"
#include <map>

#ifdef _UNICODE
#define _T(x)      L ## x
#else /* _UNICODE */
#define _T(x)      x
#endif /* _UNICODE */

typedef enum tagSERIAL_STATE
{
	SS_Unknown,
	SS_UnInit,
	SS_Init,
	SS_Started ,
	SS_Stopped ,
	
} SERIAL_STATE;

class CSerialCommHelper  
{
private:
	SERIAL_STATE	m_eState;
	HANDLE	m_hCommPort;
	HANDLE	m_hThreadTerm ;
	HANDLE	m_hThread;
	HANDLE	m_hThreadStarted;
	HANDLE	m_hDataRx;
	bool	m_abIsConnected;
	void	InvalidateHandle(HANDLE& hHandle );
	void	CloseAndCleanHandle(HANDLE& hHandle) ;
	
	CSerialBuffer m_theSerialBuffer;
	CRITICAL_SECTION m_csLock;
	SERIAL_STATE GetCurrentState() {return m_eState;}
public:
	CSerialCommHelper();
	virtual ~CSerialCommHelper();
	//void		GetEventToWaitOn(HANDLE* hEvent) {*hEvent = m_hDataRx;}
	HANDLE	GetWaitForEvent() {return m_hDataRx;} 

	HANDLE GetCommHandle(void) {return m_hCommPort;}

	inline void		LockThis()			{EnterCriticalSection ( &m_csLock );}	
	inline void		UnLockThis()		{LeaveCriticalSection (&m_csLock); }
	inline void		InitLock()			{InitializeCriticalSection (&m_csLock );}
	inline void		DelLock()				{DeleteCriticalSection (&m_csLock );}
 	inline bool		IsInputAvailable()
	{
		LockThis (); 
		bool abData = ( !m_theSerialBuffer.IsEmpty() ) ;
		UnLockThis (); 
		return abData;
	} 
	inline bool		IsConnection() {return m_abIsConnected ;}
 	inline void		SetDataReadEvent()	{		SetEvent ( m_hDataRx );	}
	

	HRESULT			Read_N		(std::string& data,long alCount,long alTimeOut);
	HRESULT			Read_Upto	(std::string& data,char chTerminator ,long	* alCount,long alTimeOut);
	HRESULT			ReadAvailable(std::string& data);
	HRESULT			Write (const char* data);
	HRESULT			Write (const char* data,DWORD dwSize);
	HRESULT			Init(std::string szPortName= _T("COM1"), DWORD dwBaudRate = 9600,BYTE byParity = 0,BYTE byStopBits = 1,BYTE byByteSize  = 8);
	HRESULT			Start();
	HRESULT			Purge();
	HRESULT			Stop();
	HRESULT			UnInit();

	static unsigned __stdcall ThreadFn(void*pvParam);
	//-- helper fn.
 	HRESULT  CanProcess();
	void OnSetDebugOption(long  iOpt,BOOL bOnOff);	
};


