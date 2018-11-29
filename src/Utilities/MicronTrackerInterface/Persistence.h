/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*
*     Written by:
*      Shahram Izadyar, Robarts Research Institute - London- Ontario , www.robarts.ca
*      Claudio Gatti, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2013
*
***************************************************************/
#ifndef __PERSISTENCE_H__
#define  __PERSISTENCE_H__

#include "MicronTrackerInterface.h"

class Persistence
{
public:
  Persistence();
  ~Persistence();
  mtHandle getHandle();

  // Set/Get path
  int setPath(const char* path);
  std::string getPath();
  int setSection(const char* section);
  int retrieveInt(const char* name, int defaultVal);
  double retrieveDouble(const char* name, double defaultVal);

  int saveInt(const char* name, int val);
  int saveDouble(const char* name, double val);

private:
  mtHandle  Handle;
};

#endif
