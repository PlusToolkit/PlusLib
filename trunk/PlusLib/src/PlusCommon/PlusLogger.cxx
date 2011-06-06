#ifdef _WIN32
#include <Windows.h> // required for setting the text color on the console output
#endif

#include "PlusLogger.h"
#include "PlusConfigure.h"
#include <string>
#include <sstream>
#include "vtkCriticalSection.h"

PlusLogger* PlusLogger::m_pInstance = NULL;

//-------------------------------------------------------
PlusLogger::PlusLogger()
{
	m_CriticalSection = vtkSimpleCriticalSection::New(); 
	m_DisplayMessageCallbackFunction = NULL;
	m_LogLevel = LOG_LEVEL_WARNING;
	m_DisplayLogLevel = LOG_LEVEL_WARNING;
	std::ostringstream logfilename; 
	logfilename << vtkAccurateTimer::GetInstance()->GetDateAndTimeString() << "_PlusLog.txt"; 
	this->m_LogStream.open (logfilename.str().c_str(), ios::out);
}

//-------------------------------------------------------
PlusLogger::~PlusLogger()
{
	if ( this->m_CriticalSection != NULL ) 
	{
		this->m_CriticalSection->Delete(); 
		this->m_CriticalSection = NULL; 
	} 

	this->m_LogStream.close(); 
}

//-------------------------------------------------------
PlusLogger* PlusLogger::Instance() 
{
	if (m_pInstance==NULL)
	{
		m_pInstance = new PlusLogger;
	}
	return m_pInstance;
}

//-------------------------------------------------------
int PlusLogger::GetLogLevel() 
{ 
	return m_LogLevel; 
}

//-------------------------------------------------------
void PlusLogger::SetLogLevel(int logLevel) 
{ 
	m_CriticalSection->Lock(); 
	m_LogLevel=logLevel; 
	m_CriticalSection->Unlock(); 
}

//-------------------------------------------------------
int PlusLogger::GetDisplayLogLevel() 
{ 
	return m_DisplayLogLevel; 
}

//-------------------------------------------------------
void PlusLogger::SetDisplayLogLevel(int logLevel) 
{ 
	m_CriticalSection->Lock(); 
	m_DisplayLogLevel=logLevel; 
	m_CriticalSection->Unlock(); 
}

//-------------------------------------------------------
void PlusLogger::LogMessage(LogLevelType level, const char *msg, const char* fileName, int lineNumber)
{
  if (m_LogLevel<level && m_DisplayLogLevel<level)
  {
    // no need to log
    return;
  }

  std::string timestamp=vtkAccurateTimer::GetInstance()->GetDateAndTimeMSecString();

	std::ostringstream log; 
  switch (level)
  {
  case LOG_LEVEL_ERROR:
    log << "[ERROR]  ";
    break;
  case LOG_LEVEL_WARNING:
    log << "[WARNING]"; 
    break;
  case LOG_LEVEL_INFO:
    log << "[INFO]   "; 
    break;
    case LOG_LEVEL_DEBUG:
    log << "[DEBUG]  "; 
    break;
  case LOG_LEVEL_TRACE:
    log << "[TRACE]  "; 
    break;
  default:
    log << "[UNKNOWN]"; 
    break;
  }
  log << msg;
	
	m_CriticalSection->Lock(); 

	if (m_DisplayLogLevel>=level)
	{ 

#ifdef _WIN32

    // Set the text color to highlight error and warning messages (supported only on windows)
    switch (level)
    {
    case LOG_LEVEL_ERROR:  
      {
        HANDLE hStdout = GetStdHandle(STD_ERROR_HANDLE); 
        SetConsoleTextAttribute(hStdout, FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|BACKGROUND_RED);
      }
      break;
    case LOG_LEVEL_WARNING:
      {
        HANDLE hStdout = GetStdHandle(STD_ERROR_HANDLE); 
        SetConsoleTextAttribute(hStdout, BACKGROUND_RED|BACKGROUND_GREEN);
      }
      break;
    default:
      {
        HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
        SetConsoleTextAttribute(hStdout, FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
      }
      break;
    }    
#endif

    if (level>LOG_LEVEL_WARNING)
    {
      std::cout << log.str() << std::endl; 
    }
    else
    {
		  std::cerr << log.str() << std::endl; 
    }

#ifdef _WIN32
    // Revert the text color (supported only on windows)
    if (level==LOG_LEVEL_ERROR || level==LOG_LEVEL_WARNING)
    {
      HANDLE hStdout = GetStdHandle(STD_ERROR_HANDLE); 
      SetConsoleTextAttribute(hStdout, FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
    }
#endif

    if (m_DisplayMessageCallbackFunction != NULL)
    {
	    m_DisplayMessageCallbackFunction(msg, level);
    }
	}	

  if (m_LogLevel>=level)
  {
    this->m_LogStream << timestamp << " " << log.str() << msg << " [in " << fileName << "(" << lineNumber << ")]" << std::endl; 
	  this->m_LogStream.flush(); 
  }

	m_CriticalSection->Unlock(); 
}


//-------------------------------------------------------
void PlusLogger::PrintProgressbar( int percent )
{
	if ( PlusLogger::Instance()->GetLogLevel() != PlusLogger::LOG_LEVEL_INFO )
	{
		return; 
	}

	std::string bar;
	for(int i = 0; i < 50; i++)
	{
		if( i < (percent/2))
		{
			bar.replace(i,1,"=");
		}
		else if( i == (percent/2))
		{
			bar.replace(i,1,">");
		}
		else
		{
			bar.replace(i,1," ");
		}
	}

	std::cout << "\r" "[" << bar << "] ";
	std::cout.width( 3 );
	std::cout << percent << "%     " << std::flush;

	if ( percent == 100)
	{
		std::cout << std::endl << std::endl;
	}
}

//-------------------------------------------------------
void PlusLogger::SetDisplayMessageCallbackFunction( DisplayMessageCallbackPtr cb )
{
	m_CriticalSection->Lock(); 
	m_DisplayMessageCallbackFunction = cb;
	m_CriticalSection->Unlock(); 
}
