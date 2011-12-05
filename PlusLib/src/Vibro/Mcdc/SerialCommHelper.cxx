/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "SerialCommHelper.h"
#include <Process.h>
#include <string>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//----------------------------------------------------------------------------
void CSerialCommHelper::InvalidateHandle(HANDLE& hHandle )
{
  hHandle = INVALID_HANDLE_VALUE;
}

//----------------------------------------------------------------------------
void CSerialCommHelper::CloseAndCleanHandle(HANDLE& hHandle)
{

  BOOL abRet  = CloseHandle( hHandle ) ;
  InvalidateHandle ( hHandle );

}

//----------------------------------------------------------------------------
CSerialCommHelper::CSerialCommHelper()
{
  m_abIsConnected = false;

  InvalidateHandle( m_hThreadTerm );
  InvalidateHandle( m_hThread	);
  InvalidateHandle( m_hThreadStarted );
  InvalidateHandle( m_hCommPort );
  InvalidateHandle( m_hDataRx );

  InitLock();
  m_eState = SS_UnInit;
}

//----------------------------------------------------------------------------
CSerialCommHelper::~CSerialCommHelper()
{
  m_eState = SS_Unknown;
  DelLock();
}

//----------------------------------------------------------------------------
HRESULT CSerialCommHelper::Init(std::string szPortName, DWORD dwBaudRate,BYTE byParity,BYTE byStopBits,BYTE byByteSize)
{
  HRESULT hr = S_OK;
  try
  {
    m_hDataRx  = CreateEvent(0,0,0,0);

    //open the COM Port
    m_hCommPort = ::CreateFile( szPortName.c_str (),
      GENERIC_READ|GENERIC_WRITE,//access ( read and write)
      0,	//(share) 0:cannot share the COM port						
      0,	//security  (None)				
      OPEN_EXISTING,// creation : open_existing
      FILE_FLAG_OVERLAPPED,// we want overlapped operation
      0// no templates file for COM port...
      );
    if ( m_hCommPort == INVALID_HANDLE_VALUE )
    {
      return E_FAIL;
    }

    //now start to read but first we need to set the COM port settings and the timeouts
    if (! ::SetCommMask(m_hCommPort,EV_RXCHAR|EV_TXEMPTY) )
    {
      return E_FAIL;
    }

    //now we need to set baud rate etc,
    DCB dcb = {0};

    dcb.DCBlength = sizeof(DCB);

    if (!::GetCommState (m_hCommPort,&dcb))
    {
      return E_FAIL;
    }

    dcb.BaudRate	= dwBaudRate;
    dcb.ByteSize	= byByteSize;
    dcb.Parity		= byParity;
    if ( byStopBits == 1 )
      dcb.StopBits	= ONESTOPBIT;
    else if (byStopBits == 2 ) 
      dcb.StopBits	= TWOSTOPBITS;
    else 
      dcb.StopBits	= ONE5STOPBITS;

    dcb.fDsrSensitivity = 0;
    dcb.fDtrControl = DTR_CONTROL_ENABLE;
    dcb.fOutxDsrFlow = 0;

    if (!::SetCommState (m_hCommPort,&dcb))
    {
      return E_FAIL;
    }		

    //now set the timeouts ( we control the timeout overselves using WaitForXXX()
    COMMTIMEOUTS timeouts;

    timeouts.ReadIntervalTimeout					= MAXDWORD; 
    timeouts.ReadTotalTimeoutMultiplier		= 0;
    timeouts.ReadTotalTimeoutConstant			= 0;
    timeouts.WriteTotalTimeoutMultiplier	= 0;
    timeouts.WriteTotalTimeoutConstant		= 0;

    if (!SetCommTimeouts(m_hCommPort, &timeouts))
    {
      return E_FAIL;
    }
    //create thread terminator event...
    m_hThreadTerm = CreateEvent(0,0,0,0);
    m_hThreadStarted = CreateEvent(0,0,0,0);

    m_hThread			= 	(HANDLE)_beginthreadex(0,0,CSerialCommHelper::ThreadFn,(void*)this,0,0 );

    DWORD dwWait = WaitForSingleObject ( m_hThreadStarted , INFINITE );

    CloseHandle(m_hThreadStarted);

    InvalidateHandle(m_hThreadStarted );
    m_abIsConnected = true;

  }
  catch(...)
  {
    hr = E_FAIL;
  }
  if ( SUCCEEDED(hr) ) 
  {
    m_eState = SS_Init;
  }
  return hr;
}	

//----------------------------------------------------------------------------
HRESULT CSerialCommHelper:: Start()
{
  m_eState = SS_Started;
  return S_OK;
}

//----------------------------------------------------------------------------
LONG CSerialCommHelper:: Purge()
{
  HRESULT hr = S_OK;

  // Check if the device is open
  if( !m_abIsConnected )
  {
    hr = E_FAIL;
  }

  if (!::PurgeComm(m_hCommPort, PURGE_TXCLEAR | PURGE_RXCLEAR))
  {
    hr = E_FAIL;
  }

  return hr;
}

//----------------------------------------------------------------------------
HRESULT CSerialCommHelper:: Stop()
{	
  m_eState = SS_Stopped;
  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT CSerialCommHelper:: UnInit()
{
  HRESULT hr = S_OK;
  try
  {
    m_abIsConnected = false;
    SignalObjectAndWait(m_hThreadTerm,m_hThread,INFINITE,FALSE);
    CloseAndCleanHandle( m_hThreadTerm);
    CloseAndCleanHandle( m_hThread);
    CloseAndCleanHandle( m_hDataRx );
    CloseAndCleanHandle( m_hCommPort );
  }
  catch(...)
  {
    hr = E_FAIL;
  }
  if ( SUCCEEDED(hr)) 
    m_eState = SS_UnInit;
  return hr;
}

//----------------------------------------------------------------------------
unsigned __stdcall CSerialCommHelper::ThreadFn(void*pvParam)
{

  CSerialCommHelper* apThis = (CSerialCommHelper*) pvParam ;
  bool abContinue = true;
  DWORD dwEventMask=0;

  OVERLAPPED ov;
  memset(&ov,0,sizeof(ov));
  ov.hEvent = CreateEvent( 0,true,0,0);
  HANDLE arHandles[2];
  arHandles[0] = apThis->m_hThreadTerm;

  DWORD dwWait;
  SetEvent(apThis->m_hThreadStarted);
  while (  abContinue )
  {

    BOOL abRet = ::WaitCommEvent(apThis->m_hCommPort,&dwEventMask, &ov) ;


    arHandles[1] = ov.hEvent ;

    dwWait = WaitForMultipleObjects (2,arHandles,FALSE,INFINITE);
    switch ( dwWait )
    {
    case WAIT_OBJECT_0:
      {
        _endthreadex(1);
      }
      break;
    case WAIT_OBJECT_0 + 1:
      {
        DWORD dwMask;
        if (GetCommMask(apThis->m_hCommPort,&dwMask) )
        {
          if ( dwMask == EV_TXEMPTY )
          {
            ResetEvent ( ov.hEvent );
            continue;
          }

        }

        //read data here...
        int iAccum = 0;

        apThis->m_theSerialBuffer.LockBuffer();

        try 
        {
          std::string szDebug;
          BOOL abRet = true;		//Updated: Changed to true

          DWORD dwBytesRead = 0;
          OVERLAPPED ovRead;
          memset(&ovRead,0,sizeof(ovRead));
          ovRead.hEvent = CreateEvent( 0,true,0,0);

          char szTmp[10] = "\0";	//Updated: Whole line added

          //Updated: While loop and contents added
          while(abRet)
          {
            memset(szTmp, 0, sizeof szTmp);
            abRet = ::ReadFile(apThis->m_hCommPort,szTmp,sizeof(szTmp),&dwBytesRead,&ovRead);
            if(abRet && dwBytesRead > 0)
            {
              apThis->m_theSerialBuffer.AddData ( szTmp,dwBytesRead );
              iAccum += dwBytesRead;
            }else abRet=0;
            ResetEvent(ovRead.hEvent);
          }

          /*	Removed in order ro receive all the contents from the serial port without missing chars
          //From user GP's Comment @ http://www.codeproject.com/KB/system/serial_com.aspx
          do
          {
          ResetEvent( ovRead.hEvent  );
          char szTmp[1];
          int iSize  = sizeof ( szTmp );
          memset(szTmp,0,sizeof szTmp);
          abRet = ::ReadFile(apThis->m_hCommPort,szTmp,sizeof(szTmp),&dwBytesRead,&ovRead);
          if (!abRet ) 
          {
          abContinue = FALSE;
          break;
          }
          if ( dwBytesRead > 0 )
          {
          apThis->m_theSerialBuffer.AddData ( szTmp,dwBytesRead );
          iAccum += dwBytesRead;
          }
          }while (0);// dwBytesRead > 0 );
          */

          CloseHandle(ovRead.hEvent );
        }
        catch(...)
        {
        }

        //if we are not in started state then we should flush the queue...( we would still read the data)
        if (apThis->GetCurrentState() != SS_Started ) 
        {
          iAccum  = 0;
          apThis->m_theSerialBuffer.Flush ();
        }

        apThis->m_theSerialBuffer.UnLockBuffer();

        if ( iAccum > 0 )
        {
          apThis->SetDataReadEvent(); 
        }
        ResetEvent ( ov.hEvent );
      }
      break;
    }//switch
  }
  return 0;
}

//----------------------------------------------------------------------------
HRESULT  CSerialCommHelper::CanProcess ()
{

  switch ( m_eState  ) 
  {
  case SS_Unknown	:return E_FAIL;
  case SS_UnInit	:return E_FAIL;
  case SS_Started :return S_OK;
  case SS_Init		:
  case SS_Stopped :
    return E_FAIL;
  default:return E_FAIL;	

  }	
  return E_FAIL;
}

//----------------------------------------------------------------------------
HRESULT CSerialCommHelper::Write (const char* data)
{
  return Write(data, (DWORD)strlen(data));
}

//----------------------------------------------------------------------------
HRESULT CSerialCommHelper::Write (const char* data,DWORD dwSize)
{
  HRESULT hr = CanProcess();
  if ( FAILED(hr)) return hr;
  int iRet = 0 ;
  OVERLAPPED ov;
  memset(&ov,0,sizeof(ov));
  ov.hEvent = CreateEvent( 0,true,0,0);
  DWORD dwBytesWritten = 0;
  //do
  {
    iRet = WriteFile (m_hCommPort,data,dwSize,&dwBytesWritten  ,&ov);
    if ( iRet == 0 )
    {
      WaitForSingleObject(ov.hEvent ,INFINITE);
    }

  }//	while ( ov.InternalHigh != dwSize ) ;
  CloseHandle(ov.hEvent);
  std::string szData(data);

  std::string::size_type loc = szData.find("\n", 0);
  if( loc != std::string::npos ) 
    szData.erase(loc, 2);

  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT CSerialCommHelper::Read_Upto	(std::string& data,char chTerminator ,long* alCount,long alTimeOut)
{
  HRESULT hr = CanProcess();
  if ( FAILED(hr)) return hr;

  try
  {

    std::string szTmp;
    szTmp.erase ();
    long alBytesRead;

    bool abFound =  m_theSerialBuffer.Read_Upto(szTmp ,chTerminator,alBytesRead,m_hDataRx );

    if ( abFound ) 
    {
      data = szTmp ;
    }
    else
    {//there are either none or less bytes...
      long iRead = 0;
      bool abContinue =  true;
      while (  abContinue )
      {
        DWORD dwWait  = ::WaitForSingleObject ( m_hDataRx , alTimeOut ) ;

        if  ( dwWait == WAIT_TIMEOUT) 
        {
          data.erase ();
          hr = E_FAIL;
          return hr;

        }

        bool abFound =  m_theSerialBuffer.Read_Upto(szTmp ,chTerminator,alBytesRead,m_hDataRx );


        if ( abFound ) 
        {
          data = szTmp;
          return S_OK;
        }

      }
    }
  }
  catch(...)
  {
  }
  return hr;
}

//----------------------------------------------------------------------------
HRESULT CSerialCommHelper::Read_N		(std::string& data,long alCount,long  alTimeOut )
{
  HRESULT hr = CanProcess();

  if ( FAILED(hr)) 
  {
    return hr;
  }

  try
  {

    std::string szTmp ;
    szTmp.erase();


    int iLocal =  m_theSerialBuffer.Read_N(szTmp ,alCount ,m_hDataRx );

    if ( iLocal == alCount ) 
    {
      data = szTmp;
    }
    else
    {//there are either none or less bytes...
      long iRead = 0;
      int iRemaining = alCount - iLocal;
      while (  1 )
      {


        DWORD dwWait  = WaitForSingleObject ( m_hDataRx , alTimeOut ) ;

        if  ( dwWait == WAIT_TIMEOUT ) 
        {
          data.erase ();
          hr = E_FAIL;
          return hr;

        }


        iRead =  m_theSerialBuffer.Read_N(szTmp , iRemaining  ,m_hDataRx);
        iRemaining -= iRead ;



        if (  iRemaining  == 0) 
        {
          data = szTmp;
          return S_OK;
        }
      }
    }
  }
  catch(...)
  {
  }
  return hr;

}

//----------------------------------------------------------------------------
HRESULT CSerialCommHelper::ReadAvailable(std::string& data)
{	
  HRESULT hr = CanProcess();
  if ( FAILED(hr)) return hr;
  try
  {
    std::string szTemp;
    bool abRet = m_theSerialBuffer.Read_Available (szTemp,m_hDataRx);

    data = szTemp;
  }
  catch(...)
  {
    hr = E_FAIL;
  }
  return hr;
}
