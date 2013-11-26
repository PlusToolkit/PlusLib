#include "MicronTrackerLogger.h"
#include "MicronTrackerInterface.h"
#include <strstream>

MicronTrackerLogger* MicronTrackerLogger::m_pInstance = NULL;

MicronTrackerLogger::MicronTrackerLogger()
: m_LogMessageCallback(NULL)
, m_LogMessageCallbackUserdata(NULL)
{
}

MicronTrackerLogger::~MicronTrackerLogger()
{
}

MicronTrackerLogger* MicronTrackerLogger::Instance() 
{
  if (m_pInstance == NULL)
  {
    if( m_pInstance != NULL )
    {
      return m_pInstance;
    }
    m_pInstance = new MicronTrackerLogger;     
  }
  return m_pInstance;
}

void MicronTrackerLogger::SetLogMessageCallback(LogMessageCallbackType callback, void *userdata)
{
  m_LogMessageCallback=callback;
  m_LogMessageCallbackUserdata=userdata;
}

void MicronTrackerLogger::LogMessage(LogLevel level, const char* description, const char* fileName, int lineNumber)
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
