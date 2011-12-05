/*=Plus=header=begin======================================================
  Author: Ian Bell
  Date: 2010
  Program: Plus
  Copyright (c) University of British Columbia
  Modified: Siavash Khallaghi
  Contact: Hani Eskandari, hanie@ece.ubc.ca
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "SerialBuffer.h"
#include "SerialCommHelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//----------------------------------------------------------------------------
CSerialBuffer::CSerialBuffer()
{
  ::InitializeCriticalSection(&m_csLock);
  m_abLockAlways = true;
  m_iCurPos = 0;
  m_alBytesUnRead = 0;
  m_szInternalBuffer.erase();
}

//----------------------------------------------------------------------------
CSerialBuffer::~CSerialBuffer()
{
  ::DeleteCriticalSection(&m_csLock);
}

//----------------------------------------------------------------------------
void CSerialBuffer::AddData(char ch)
{
  m_szInternalBuffer += ch;
  m_alBytesUnRead += 1;
}

//----------------------------------------------------------------------------
void CSerialBuffer::AddData(std::string& szData,int iLen) 
{	
  m_szInternalBuffer.append(szData.c_str(), iLen);
  m_alBytesUnRead += iLen;
}

//----------------------------------------------------------------------------
void CSerialBuffer::AddData(char *strData, int iLen) 
{	
  m_szInternalBuffer.append(strData, iLen);
  m_alBytesUnRead += iLen;
}

//----------------------------------------------------------------------------
void CSerialBuffer::AddData(std::string &szData) 
{
  m_szInternalBuffer += szData;
  m_alBytesUnRead += (long)szData.size();
}

//----------------------------------------------------------------------------
void CSerialBuffer::Flush()
{
  LockBuffer();
  m_szInternalBuffer.erase();
  m_alBytesUnRead = 0;
  m_iCurPos = 0;
  UnLockBuffer();
}

//----------------------------------------------------------------------------
long CSerialBuffer::Read_N(std::string &szData, long alCount, HANDLE &hEventToReset)
{
  LockBuffer();
  long alTempCount = min(alCount, m_alBytesUnRead) ;
  long alActualSize	= GetSize();

  szData.append(m_szInternalBuffer, m_iCurPos, alTempCount);

  m_iCurPos += alTempCount;

  m_alBytesUnRead -= alTempCount;
  if (m_alBytesUnRead==0)
  {
    ClearAndReset(hEventToReset);
  }

  UnLockBuffer();
  return alTempCount;
}

//----------------------------------------------------------------------------
bool CSerialBuffer::Read_Available(std::string &szData, HANDLE &hEventToReset)
{
  LockBuffer();
  szData += m_szInternalBuffer ;

  ClearAndReset(hEventToReset);

  UnLockBuffer();
  return ( szData.size () > 0 );
}

//----------------------------------------------------------------------------
void CSerialBuffer::ClearAndReset(HANDLE& hEventToReset)
{
  m_szInternalBuffer.erase();
  m_alBytesUnRead = 0;
  m_iCurPos				= 0;
  ::ResetEvent(hEventToReset);
}

//----------------------------------------------------------------------------
bool CSerialBuffer::Read_Upto(std::string &szData, char chTerm, long &alBytesRead, HANDLE &hEventToReset)
{
  LockBuffer();
  alBytesRead = 0;

  bool abFound = false;
  if (m_alBytesUnRead>0) 
  {//if there are some bytes un-read...

    int iActualSize = GetSize();
    int iIncrementPos = 0;
    for (int i = m_iCurPos; i < iActualSize; ++i)
    {
      szData+=m_szInternalBuffer[i];
      m_alBytesUnRead -= 1;
      if (m_szInternalBuffer[i]==chTerm) 
      {
        iIncrementPos++;
        abFound = true;
        break;
      }
      iIncrementPos++;
    }
    m_iCurPos += iIncrementPos;
    if (m_alBytesUnRead==0) 
    {
      ClearAndReset(hEventToReset);
    }
  }
  UnLockBuffer();	
  return abFound;
}
