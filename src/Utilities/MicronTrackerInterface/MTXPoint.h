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
#ifndef __XPOINT_H__
#define __XPOINT_H__

#include "MicronTrackerInterface.h"

class Collection;
class Facet;
class MCamera;
class Persistence;
class Xform3D;

class MTXPoint
{
public:
  MTXPoint(mtHandle h = 0);
  ~MTXPoint();
  mtHandle getHandle();

  int getIndex();
  int setIndex(int Index);

  int position3D(double* x, double* y, double* z);
  int position2D(double* x0, double* y0, double* x1, double* y1, double* x2, double* y2);

  double distance();

protected:
  mtHandle  Handle;
  bool      OwnedByMe;

};

#endif
