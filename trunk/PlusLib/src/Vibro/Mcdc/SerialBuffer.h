/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __SerialBuffer_h
#define __SerialBuffer_h

#include <string>

/*!
\class CSerialBuffer
\brief Class for temporary data storage during serial (RS-232) communication

\sa CSerialCommHelper
\ingroup PlusLibVibro
*/
class CSerialBuffer  
{
public:
  CSerialBuffer( );
  virtual ~CSerialBuffer();

  void AddData( char ch ) ;
  void AddData( std::string& szData ) ;
  void AddData( std::string& szData,int iLen ) ;
  void AddData( char *strData,int iLen ) ;
  std::string GetData() {return m_szInternalBuffer;}

  void Flush();
  long Read_N( std::string &szData,long alCount,HANDLE& hEventToReset);
  bool Read_Upto( std::string &szData,char chTerm,long  &alBytesRead ,HANDLE& hEventToReset);
  bool Read_Upto_FIX( std::string &szData,char chTerm,long  &alBytesRead,HANDLE & hEventToReset);
  bool Read_Available( std::string &szData,HANDLE & hEventToReset);
  inline long GetSize() {return (long)m_szInternalBuffer.size(); }
  inline bool IsEmpty() {return m_szInternalBuffer.size() == 0; }
  
  void	ClearAndReset(HANDLE& hEventToReset);

  inline void LockBuffer() {::EnterCriticalSection(&m_csLock );}
  inline void UnLockBuffer() {::LeaveCriticalSection(&m_csLock );} 

private:
  std::string m_szInternalBuffer;
  CRITICAL_SECTION	m_csLock;
  bool	m_abLockAlways;
  long	m_iCurPos;
  long  m_alBytesUnRead;
};

#endif // __SerialBuffer_h
