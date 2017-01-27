#ifndef __MfVideoCaptureLoggerMacros_H__
#define __MfVideoCaptureLoggerMacros_H__

#include <sstream>
#include "MfVideoCaptureLogger.h"

#define LOG_ERROR(msg) \
  { \
  std::ostringstream msgStream; \
  msgStream << " " << msg << std::ends; \
  MfVideoCaptureLogger::Instance()->LogMessage(MfVideoCaptureLogger::ERROR_LEVEL, msgStream.str().c_str(), __FILE__, __LINE__); \
  }  

#define LOG_WARNING(msg) \
  { \
  std::ostringstream msgStream; \
  msgStream << " " << msg << std::ends; \
  MfVideoCaptureLogger::Instance()->LogMessage(MfVideoCaptureLogger::WARNING_LEVEL, msgStream.str().c_str(), __FILE__, __LINE__); \
  }
    
#define LOG_DEBUG(msg) \
  { \
  std::ostringstream msgStream; \
  msgStream << " " << msg << std::ends; \
  MfVideoCaptureLogger::Instance()->LogMessage(MfVideoCaptureLogger::DEBUG_LEVEL, msgStream.str().c_str(), __FILE__, __LINE__); \
  }

#define LOG_INFO(msg) \
  { \
  std::ostringstream msgStream; \
  msgStream << " " << msg << std::ends; \
  MfVideoCaptureLogger::Instance()->LogMessage(MfVideoCaptureLogger::INFO_LEVEL, msgStream.str().c_str(), __FILE__, __LINE__); \
  } 
  
#endif // __MfVideoCaptureLoggerMacros_H__
