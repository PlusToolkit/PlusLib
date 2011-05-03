/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*   
*     Written by: 
*      Shahram Izadyar, Robarts Research Institute - London- Ontario , www.robarts.ca
*      Claudio Gatti, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2003
*
***************************************************************/
#include "Persistence.h"

#include <string>
/****************************/
/** Constructor */
Persistence::Persistence()
{
  this->m_handle = Persistence_New();
  //this->ownedByMe = TRUE;
}

/****************************/
/** Destructor */
Persistence::~Persistence()
{
  this->ownedByMe = false;
  Persistence_Free(this->m_handle);
}

/****************************/
/** Set the path of the .ini file */
void Persistence::setPath(char* path)
{

  Persistence_PathSet(this->m_handle, path); 
}

/****************************/
/** Set the path of the .ini file */
void Persistence::setSection(char* section)
{
  Persistence_SectionSet(this->m_handle, section); 
}


#if 0
/** Get the path of the .ini file */
char* Persistence::getPath()
{
  char* pathName;
  // Claudio: need to allocate string, send it down, with the size and then reset based on retrieved lenght
  Persistence_PathGet(this->m_handle, pathName);
  return pathName;
}
#endif
  
/****************************/
/** Retrieve an int from the persistence */
int Persistence::retrieveInt(char* name, int defaultVal)
{
  int intVal = 0;
  Persistence_RetrieveInt(this->m_handle, name, defaultVal, &intVal);
  return intVal;
}

/****************************/
/** Retrieve a double from the persistence. */
double Persistence::retrieveDouble(char* name, double defaultVal)
{
  double dblVal = 0;
  Persistence_RetrieveDouble(this->m_handle, name, defaultVal, &dblVal);
  return dblVal;
}

/****************************/
/** Save an integer value in the persistence. */
void Persistence::saveInt(char* name, int val)
{
  Persistence_SaveInt(this->m_handle, name, val);
}

/****************************/
/** Save a double value in the persistnce */
void Persistence::saveDouble(char* name, double val)
{
  Persistence_SaveDouble(this->m_handle, name, val);
}
