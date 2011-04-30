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
	m_DisplayLogLevel = LOG_LEVEL_ERROR;
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
void PlusLogger::LogError(const char *msg)
{
	std::ostringstream log; 
	log << "[ERROR  ]: " << msg; 

	this->WriteToFile(log.str().c_str()); 

	m_CriticalSection->Lock(); 
	if (m_LogLevel>=LOG_LEVEL_ERROR)
	{ 
		std::cerr << log.str() << std::endl; 
	}
	if ((m_DisplayMessageCallbackFunction != NULL) && (m_DisplayLogLevel>=LOG_LEVEL_ERROR))
	{
		m_DisplayMessageCallbackFunction(msg, LOG_LEVEL_ERROR);
	}
	m_CriticalSection->Unlock(); 
}

//-------------------------------------------------------
void PlusLogger::LogWarning(const char *msg)
{
	std::ostringstream log; 
	log << "[WARNING]: " << msg; 

	this->WriteToFile(log.str().c_str()); 

	m_CriticalSection->Lock(); 
	if (m_LogLevel>=LOG_LEVEL_WARNING)
	{ 
		std::cerr << log.str() << std::endl; 
	}
	if ((m_DisplayMessageCallbackFunction != NULL) && (m_DisplayLogLevel>=LOG_LEVEL_WARNING))
	{
		m_DisplayMessageCallbackFunction(msg, LOG_LEVEL_WARNING);
	}
	m_CriticalSection->Unlock(); 
}

//-------------------------------------------------------
void PlusLogger::LogInfo(const char *msg)
{
	std::ostringstream log; 
	log << "[INFO   ]: " << msg; 

	this->WriteToFile(log.str().c_str()); 

	m_CriticalSection->Lock(); 
	if (m_LogLevel>=LOG_LEVEL_INFO)
	{ 
		std::cerr << log.str() << std::endl; 
	}
	if ((m_DisplayMessageCallbackFunction != NULL) && (m_DisplayLogLevel>=LOG_LEVEL_INFO))
	{
		m_DisplayMessageCallbackFunction(msg, LOG_LEVEL_INFO);
	}
	m_CriticalSection->Unlock(); 
}

//-------------------------------------------------------
void PlusLogger::LogDebug(const char *msg)
{
	std::ostringstream log; 
	log << "[DEBUG  ]: " << msg; 

	this->WriteToFile(log.str().c_str()); 

	m_CriticalSection->Lock(); 
	if (m_LogLevel>=LOG_LEVEL_DEBUG)
	{ 
		std::cerr << log.str() << std::endl; 
	}
	if ((m_DisplayMessageCallbackFunction != NULL) && (m_DisplayLogLevel>=LOG_LEVEL_DEBUG))
	{
		m_DisplayMessageCallbackFunction(msg, LOG_LEVEL_DEBUG);
	}
	m_CriticalSection->Unlock(); 
}

//-------------------------------------------------------
void PlusLogger::LogTrace(const char *msg)
{
	std::ostringstream log; 
	log << "[TRACE  ]: " << msg; 

	this->WriteToFile(log.str().c_str()); 

	m_CriticalSection->Lock(); 
	if (m_LogLevel>=LOG_LEVEL_TRACE)
	{ 
		std::cerr << log.str() << std::endl; 
	}
	if ((m_DisplayMessageCallbackFunction != NULL) && (m_DisplayLogLevel>=LOG_LEVEL_TRACE))
	{
		m_DisplayMessageCallbackFunction(msg, LOG_LEVEL_TRACE);
	}
	m_CriticalSection->Unlock(); 
}

//-------------------------------------------------------
void PlusLogger::WriteToFile(const char *msg)
{
	m_CriticalSection->Lock();
	this->m_LogStream << "[" << vtkAccurateTimer::GetInstance()->GetDateAndTimeMSecString() << "\t]::" << msg << std::endl; 
	this->m_LogStream.flush(); 
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
