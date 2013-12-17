#ifndef __MfVideoCaptureLoggerMacros_H__
#define __MfVideoCaptureLoggerMacros_H__

#include <strstream>
#include "MfVideoCaptureLogger.h"

#define LOG_ERROR(msg) \
  { \
  std::ostrstream msgStream; \
  msgStream << " " << msg << std::ends; \
  MfVideoCaptureLogger::Instance()->LogMessage(MfVideoCaptureLogger::ERROR_LEVEL, msgStream.str(), __FILE__, __LINE__); \
  msgStream.rdbuf()->freeze(0); \
  }  

#define LOG_WARNING(msg) \
  { \
  std::ostrstream msgStream; \
  msgStream << " " << msg << std::ends; \
  MfVideoCaptureLogger::Instance()->LogMessage(MfVideoCaptureLogger::WARNING_LEVEL, msgStream.str(), __FILE__, __LINE__); \
  msgStream.rdbuf()->freeze(0); \
  }
    
#define LOG_DEBUG(msg) \
  { \
  std::ostrstream msgStream; \
  msgStream << " " << msg << std::ends; \
  MfVideoCaptureLogger::Instance()->LogMessage(MfVideoCaptureLogger::DEBUG_LEVEL, msgStream.str(), __FILE__, __LINE__); \
  msgStream.rdbuf()->freeze(0); \
  }

#define LOG_INFO(msg) \
  { \
  std::ostrstream msgStream; \
  msgStream << " " << msg << std::ends; \
  MfVideoCaptureLogger::Instance()->LogMessage(MfVideoCaptureLogger::INFO_LEVEL, msgStream.str(), __FILE__, __LINE__); \
  msgStream.rdbuf()->freeze(0); \
  } 
  
#endif // __MfVideoCaptureLoggerMacros_H__
