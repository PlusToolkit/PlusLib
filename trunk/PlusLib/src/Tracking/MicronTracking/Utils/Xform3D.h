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
#ifndef __XFORM3D_H__
#define __XFORM3D_H__

#include "MTC.h"

class Xform3D
{

public:
  Xform3D(int h=0);
  ~Xform3D();

  inline int getHandle() {return m_handle;};
  Xform3D* concatenate( Xform3D* subsequentXform);
  Xform3D* inverse();
  Xform3D* inBetween(Xform3D* secondXf, double secondFract0To1);

  double getShift(int index);
  void getShiftVector(double* s);
  void getRotateVector(double* resultVector, double* inVector, bool scaleIt = false);
    int getRotationMatrix(double *resultMatrix_3x3);

private:
  int m_handle;
  bool ownedByMe;

};
#endif
