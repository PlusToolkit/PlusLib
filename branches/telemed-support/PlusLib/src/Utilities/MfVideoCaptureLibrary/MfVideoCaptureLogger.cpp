#include "MfVideoCaptureLogger.h"
#include <strstream>

MfVideoCaptureLogger* MfVideoCaptureLogger::m_pInstance = NULL;

MfVideoCaptureLogger::MfVideoCaptureLogger()
: m_LogMessageCallback(NULL)
, m_LogMessageCallbackUserdata(NULL)
{
}

MfVideoCaptureLogger::~MfVideoCaptureLogger()
{
}

MfVideoCaptureLogger* MfVideoCaptureLogger::Instance() 
{
  if (m_pInstance == NULL)
  {
    if( m_pInstance != NULL )
    {
      return m_pInstance;
    }
    m_pInstance = new MfVideoCaptureLogger;     
  }
  return m_pInstance;
}

void MfVideoCaptureLogger::SetLogMessageCallback(LogMessageCallbackType callback, void *userdata)
{
  m_LogMessageCallback=callback;
  m_LogMessageCallbackUserdata=userdata;
}

void MfVideoCaptureLogger::LogMessage(LogLevel level, const char* description, const char* fileName, int lineNumber)
{
  if (m_LogMessageCallback==NULL)
  {
    // no error callback is specified
    return;
  }
  std::ostrstream msgStream;
  msgStream << (description?description:"unspecified message") << " in " << fileName << "(" << lineNumber << ")";
  msgStream << std::ends;
  m_LogMessageCallback(level, msgStream.str(), m_LogMessageCallbackUserdata);
}
