/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*
*     Written by:
*      Shahram Izadyar, Robarts Research Institute - London- Ontario , www.robarts.ca
*      Claudio Gatti, Claron Technology - Toronto - Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2013
*
***************************************************************/

#include <MTC.h>

#include "Persistence.h"

#include <string>

//----------------------------------------------------------------------------
Persistence::Persistence()
{
  this->Handle = Persistence_New();
}

//----------------------------------------------------------------------------
Persistence::~Persistence()
{
  Persistence_Free(this->Handle);
}

//----------------------------------------------------------------------------
mtHandle Persistence::getHandle()
{
  return this->Handle;
}

//----------------------------------------------------------------------------
int Persistence::setPath(const char* path)
{
  return Persistence_PathSet(this->Handle, path);
}

//----------------------------------------------------------------------------
int Persistence::setSection(const char* section)
{
  return Persistence_SectionSet(this->Handle, section);
}

//----------------------------------------------------------------------------
std::string Persistence::getPath()
{
  char pathName[SHRT_MAX];
  int outLength;
  Persistence_PathGet(this->Handle, pathName, SHRT_MAX, &outLength);
  pathName[outLength] = 0;
  return std::string(pathName);
}

//----------------------------------------------------------------------------
int Persistence::retrieveInt(const char* name, int defaultVal)
{
  int intVal = 0;
  Persistence_RetrieveInt(this->Handle, name, defaultVal, &intVal);
  return intVal;
}

//----------------------------------------------------------------------------
double Persistence::retrieveDouble(const char* name, double defaultVal)
{
  double dblVal = 0;
  Persistence_RetrieveDouble(this->Handle, name, defaultVal, &dblVal);
  return dblVal;
}

//----------------------------------------------------------------------------
int Persistence::saveInt(const char* name, int val)
{
  return Persistence_SaveInt(this->Handle, name, val);
}

//----------------------------------------------------------------------------
int Persistence::saveDouble(const char* name, double val)
{
  return Persistence_SaveDouble(this->Handle, name, val);
}
