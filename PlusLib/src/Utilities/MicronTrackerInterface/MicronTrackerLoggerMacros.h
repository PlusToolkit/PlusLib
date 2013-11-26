#ifndef __MICRONTRACKERLOGGERMACROS_H__
#define __MICRONTRACKERLOGGERMACROS_H__

#include <strstream>
#include "MicronTrackerLogger.h"

#define LOG_ERROR(msg) \
  { \
  std::ostrstream msgStream; \
  msgStream << " " << msg << std::ends; \
  MicronTrackerLogger::Instance()->LogMessage(MicronTrackerLogger::ERROR_LEVEL, msgStream.str(), __FILE__, __LINE__); \
  msgStream.rdbuf()->freeze(0); \
  }  

#define LOG_WARNING(msg) \
  { \
  std::ostrstream msgStream; \
  msgStream << " " << msg << std::ends; \
  MicronTrackerLogger::Instance()->LogMessage(MicronTrackerLogger::WARNING_LEVEL, msgStream.str(), __FILE__, __LINE__); \
  msgStream.rdbuf()->freeze(0); \
  }
    
#define LOG_DEBUG(msg) \
  { \
  std::ostrstream msgStream; \
  msgStream << " " << msg << std::ends; \
  MicronTrackerLogger::Instance()->LogMessage(MicronTrackerLogger::DEBUG_LEVEL, msgStream.str(), __FILE__, __LINE__); \
  msgStream.rdbuf()->freeze(0); \
  }  
  
#endif // __MICRONTRACKERLOGGERMACROS_H__
