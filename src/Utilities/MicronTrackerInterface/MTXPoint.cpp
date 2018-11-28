/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*
*     Written by:
*     Ahmad Kolahi, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*   Modified by:
*     Claudio Gatti, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2013
*
***************************************************************/

#include <MTC.h>

#include "Collection.h"
#include "Facet.h"
#include "MCamera.h"
#include "MTXPoint.h"
#include "Persistence.h"
#include "Xform3D.h"

#include <string>

//----------------------------------------------------------------------------
MTXPoint::MTXPoint(mtHandle h)
{
  // If a handle is provided to this class, don't create a new one
  if (h != 0)
  {
    this->Handle = h;
  }
  else
  {
    this->Handle = XPoint_New();
  }
  this->OwnedByMe = true;
}

//----------------------------------------------------------------------------
MTXPoint::~MTXPoint()
{
  if (this->Handle != 0 && this->OwnedByMe)
  {
    XPoint_Free(this->Handle);
  }
}

//----------------------------------------------------------------------------
mtHandle MTXPoint::getHandle()
{
  return this->Handle;
}

//----------------------------------------------------------------------------
int MTXPoint::getIndex()
{
  int Result;
  XPoint_IndexGet(this->Handle, &Result);
  return Result;
}

//----------------------------------------------------------------------------
int MTXPoint::setIndex(int index)
{
  return XPoint_IndexSet(this->Handle, index);
}

//----------------------------------------------------------------------------
int MTXPoint::position3D(double* x, double* y, double* z)
{
  return XPoint_3DPositionGet(this->Handle, x, y, z);
}

//----------------------------------------------------------------------------
int MTXPoint::position2D(double* x0, double* y0, double* x1, double* y1, double* x2, double* y2)
{
  return XPoint_2DPositionGet(this->Handle, x0, y0, x1, y1, x2, y2);
}

//----------------------------------------------------------------------------
double MTXPoint::distance()
{
  int R;
  double dist;
  R = XPoint_DistGet(this->Handle, &dist);
  return dist;
}
