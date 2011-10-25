/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __PLUSLOGGER_H
#define __PLUSLOGGER_H

#include "vtkObject.h"

#include <fstream>
#include <sstream>

class vtkSimpleCriticalSection; 

// This is a special output class for VTK logs that enforces VTK to
// log to the console instead of displaying them in a pop-up window.
class vtkConsoleOutputWindow : public vtkOutputWindow 
{ 
public: 
	static vtkConsoleOutputWindow* New() 
	{ return new vtkConsoleOutputWindow; } 
};

// This is a special output class for VTK logs that logs
// VTK error with the PlusLogger instead of displaying them in a pop-up window.
class vtkPlusLoggerOutputWindow : public vtkOutputWindow
{
public:
  vtkTypeMacro(vtkPlusLoggerOutputWindow, vtkOutputWindow);

  static vtkPlusLoggerOutputWindow* New();

  virtual void PrintSelf(ostream& os, vtkIndent indent);

  virtual void DisplayText(const char* text);
  virtual void DisplayErrorText(const char* text);
  virtual void DisplayWarningText(const char* text);
  virtual void DisplayGenericWarningText(const char* text); 
  virtual void DisplayDebugText(const char* text);

protected:
  vtkPlusLoggerOutputWindow(); 
  virtual ~vtkPlusLoggerOutputWindow(); 

  // VTK error messages contain multiple lines. This method replaces newline characters by another separator character
  void ReplaceNewlineBySeparator(std::string &str);

private:
  vtkPlusLoggerOutputWindow(const vtkPlusLoggerOutputWindow&);  // Not implemented.
  void operator=(const vtkPlusLoggerOutputWindow&);  // Not implemented.
};

class VTK_EXPORT vtkPlusLogger : public vtkObject
{

public:
	enum LogLevelType
	{
		LOG_LEVEL_ERROR=1,
		LOG_LEVEL_WARNING=2,
		LOG_LEVEL_INFO=3,
		LOG_LEVEL_DEBUG=4,
		LOG_LEVEL_TRACE=5
	};

	static vtkPlusLogger* Instance(); 

	void LogMessage(LogLevelType level, const char *msg, const char* fileName, int lineNumber); 
	
	int GetLogLevel();	
	void SetLogLevel(int logLevel);

	int GetDisplayLogLevel();
	
	void SetDisplayLogLevel(int logLevel);

	static void PrintProgressbar( int percent ); 

  void SetLogFileName( const char* logfilename);
  std::string GetLogFileName(); 

protected:
	vtkPlusLogger(); 
	~vtkPlusLogger();

	void WriteToFile(const char *msg); 

  void Flush(); 

private: 
	vtkPlusLogger(vtkPlusLogger const&);
	vtkPlusLogger& operator=(vtkPlusLogger const&);
	
	static vtkPlusLogger* m_pInstance;
	int m_LogLevel;
	int m_DisplayLogLevel;
  std::ostringstream m_LogStream;
  std::ofstream m_FileStream; 
  std::string m_LogFileName; 

	vtkSimpleCriticalSection* m_CriticalSection; 
};

#endif
