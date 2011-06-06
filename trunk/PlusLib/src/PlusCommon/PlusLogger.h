#ifndef __PLUSLOGGER_H
#define __PLUSLOGGER_H

#define DllExport   __declspec( dllexport )

#include <fstream>

class vtkSimpleCriticalSection; 

class DllExport PlusLogger
{
	typedef void (*DisplayMessageCallbackPtr)(const char *msg, const int level);

public:
	enum LogLevelType
	{
		LOG_LEVEL_ERROR=1,
		LOG_LEVEL_WARNING=2,
		LOG_LEVEL_INFO=3,
		LOG_LEVEL_DEBUG=4,
		LOG_LEVEL_TRACE=5
	};

	static PlusLogger* Instance(); 

	void LogMessage(LogLevelType level, const char *msg, const char* fileName, int lineNumber); 
	
	int GetLogLevel();	
	void SetLogLevel(int logLevel);

	int GetDisplayLogLevel();
	
	void SetDisplayLogLevel(int logLevel);

	static void PrintProgressbar( int percent ); 

  void SetDisplayMessageCallbackFunction( DisplayMessageCallbackPtr cb );

protected:
	PlusLogger(); 
	~PlusLogger();

	void WriteToFile(const char *msg); 

private: 
	PlusLogger(PlusLogger const&);
	PlusLogger& operator=(PlusLogger const&);
	
	static PlusLogger* m_pInstance;
	int m_LogLevel;
	int m_DisplayLogLevel;
	std::ofstream m_LogStream;

	vtkSimpleCriticalSection* m_CriticalSection; 

	DisplayMessageCallbackPtr m_DisplayMessageCallbackFunction;
};

#endif
