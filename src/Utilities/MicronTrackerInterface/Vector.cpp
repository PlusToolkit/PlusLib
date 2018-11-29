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

//----------------------------------------------------------------------------
Vector::Vector(mtHandle handle)
{
  // If a handle is provided to this class, don't create a new one
  if (handle != 0)
  {
    this->Handle = handle;
  }
  else
  {
    this->Handle = Vector_New();
  }
  this->OwnedByMe = true;
}

//----------------------------------------------------------------------------
Vector::~Vector()
{
  if (this->Handle != 0 && this->OwnedByMe)
  {
    Vector_Free(this->Handle);
    this->Handle = NULL;
  }
}

//----------------------------------------------------------------------------
mtHandle Vector::getHandle()
{
  return this->Handle;
}

//----------------------------------------------------------------------------
int Vector::getEndPos(EndPosType_BH_XYZ result)
{
  return Vector_EndPosGet(this->Handle, (double*) result);
}

//----------------------------------------------------------------------------
int Vector::getEndXPoints(EndXPointType_LRM_BH_XY result)
{
  return Vector_EndXPointsGet(this->Handle, (double*) result);
}
