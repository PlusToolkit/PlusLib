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
#ifndef __VECTOR_H__
#define __VECTOR_H__

class Vector
{
public:
  // left/right/middle camera; base/head; x/y coordinate
  typedef double EndXPointType_LRM_BH_XY[3][2][2];
  // left/right/middle camera; base/head; x/y/z coordinate
  typedef double EndPosType_BH_XYZ[2][3];

  Vector(mtHandle h=0);
  ~Vector();
  mtHandle Handle();

  int getEndPos(EndPosType_BH_XYZ result);
  int getEndXPoints(EndXPointType_LRM_BH_XY result);

private:
  mtHandle m_handle;
  bool ownedByMe;
};

#endif