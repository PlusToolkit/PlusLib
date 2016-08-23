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

#include "MTC.h"

#include "Vector.h"

#include "MicronTrackerLoggerMacros.h"

/****************************/
/** Constructor */
Vector::Vector(mtHandle handle)
{
  // If a handle is provided to this class, don't create a new one
  if (handle != 0)
    this->m_handle = handle;
  else
    this->m_handle = Vector_New();
  this->ownedByMe = TRUE;
}

/****************************/
/** Destructor */
Vector::~Vector()
{
  if (this->m_handle != 0 && this->ownedByMe )
  {
    Vector_Free(this->m_handle);
    this->m_handle = NULL;
  }
}
mtHandle Vector::Handle()
{
  return ( m_handle);
}

/****************************/
/** */
int Vector::getEndPos(EndPosType_BH_XYZ result)
{
  return Vector_EndPosGet(this->m_handle, (double*) result );
}

/****************************/
/** */
int Vector::getEndXPoints(EndXPointType_LRM_BH_XY result)
{
  return Vector_EndXPointsGet(this->m_handle, (double*) result);
}
