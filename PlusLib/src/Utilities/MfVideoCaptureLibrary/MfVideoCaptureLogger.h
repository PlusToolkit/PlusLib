#ifndef __MfVideoCaptureLogger_H__
#define __MfVideoCaptureLogger_H__ 

#include <strstream>

class MfVideoCaptureLogger
{
public:
  enum LogLevel
  {
    ERROR_LEVEL,
    WARNING_LEVEL,
    INFO_LEVEL,
    DEBUG_LEVEL
  };

  /*! Error callback type for use with SetLogMessageCallback() */
  typedef void (*LogMessageCallbackType)(int level, const char *message, void *userdata);

  static MfVideoCaptureLogger* Instance(); 
  
  /*!
  Set a function that will be called each time an error occurs.
  \param callback function pointer
  \param userdata data to send to the callback each time it is called
  The callback can be set to NULL to erase a previous callback.
  */
  void SetLogMessageCallback(LogMessageCallbackType callback, void *userdata);  

  void LogMessage(LogLevel level, const char* message, const char* fileName, int lineNumber);

protected:
  MfVideoCaptureLogger();
  virtual ~MfVideoCaptureLogger();
  
  /*! Pointer to the singleton instance */
  static MfVideoCaptureLogger* m_pInstance; 
  
  LogMessageCallbackType m_LogMessageCallback;
  void* m_LogMessageCallbackUserdata;
};
  
#endif // __MfVideoCaptureLogger_H__
