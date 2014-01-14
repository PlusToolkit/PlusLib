#ifndef __MICRONTRACKERLOGGER_H__
#define __MICRONTRACKERLOGGER_H__ 

#include <strstream>

class MicronTrackerLogger
{
public:
  enum LogLevel
  {
    ERROR_LEVEL,
    WARNING_LEVEL,
    DEBUG_LEVEL
  };

  /*! Error callback type for use with SetLogMessageCallback() */
  typedef void (*LogMessageCallbackType)(int level, const char *message, void *userdata);

  static MicronTrackerLogger* Instance(); 
  
  /*!
  Set a function that will be called each time an error occurs.
  \param callback function pointer
  \param userdata data to send to the callback each time it is called
  The callback can be set to NULL to erase a previous callback.
  */
  void SetLogMessageCallback(LogMessageCallbackType callback, void *userdata);  

  void LogMessage(LogLevel level, const char* message, const char* fileName, int lineNumber);

protected:
  MicronTrackerLogger();
  virtual ~MicronTrackerLogger();
  
  /*! Pointer to the singleton instance */
  static MicronTrackerLogger* m_pInstance; 
  
  LogMessageCallbackType m_LogMessageCallback;
  void* m_LogMessageCallbackUserdata;
};
  
#endif // __MICRONTRACKERLOGGER_H__
