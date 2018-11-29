/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*
*     Written by:
*      Shahram Izadyar, Robarts Research Institute - London- Ontario , www.robarts.ca
*      Claudio Gatti, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*   Modified by:
*     Gregory Bootsma, Princess Margaret Hospital - Toronto - Ontario
*
*     Copyright Claron Technology 2000-2013
*
***************************************************************/
#ifndef __XFORM3D_H__
#define __XFORM3D_H__

#include "MicronTrackerInterface.h"

class Xform3D
{
public:
  Xform3D(mtHandle h = 0);
  ~Xform3D();

  mtHandle getHandle();
  Xform3D* concatenate(Xform3D* subsequentXform);
  Xform3D* inverse();
  Xform3D* inBetween(Xform3D* secondXf, double secondFract0To1);

  double getShift(int index);
  int getShiftVector(double* s);
  int getRotateVector(double* resultVector, double* inVector, bool scaleIt = false);
  double getQuaternion(int index);
  int getQuaternionVector(double* q);
  int getHazardState();

private:
  mtHandle  Handle;
  bool      OwnedByMe;
};

#endif
