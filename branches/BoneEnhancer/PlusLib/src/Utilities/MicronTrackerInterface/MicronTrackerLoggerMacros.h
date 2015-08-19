#ifndef __MICRONTRACKERLOGGERMACROS_H__
#define __MICRONTRACKERLOGGERMACROS_H__

#include <sstream>
#include "MicronTrackerLogger.h"

#define LOG_ERROR(msg) \
  { \
  std::ostringstream msgStream; \
  msgStream << " " << msg << std::ends; \
  MicronTrackerLogger::Instance()->LogMessage(MicronTrackerLogger::ERROR_LEVEL, msgStream.str().c_str(), __FILE__, __LINE__); \
  }  

#define LOG_WARNING(msg) \
  { \
  std::ostringstream msgStream; \
  msgStream << " " << msg << std::ends; \
  MicronTrackerLogger::Instance()->LogMessage(MicronTrackerLogger::WARNING_LEVEL, msgStream.str().c_str(), __FILE__, __LINE__); \
  }
    
#define LOG_DEBUG(msg) \
  { \
  std::ostringstream msgStream; \
  msgStream << " " << msg << std::ends; \
  MicronTrackerLogger::Instance()->LogMessage(MicronTrackerLogger::DEBUG_LEVEL, msgStream.str().c_str(), __FILE__, __LINE__); \
  }  
  
#endif // __MICRONTRACKERLOGGERMACROS_H__
