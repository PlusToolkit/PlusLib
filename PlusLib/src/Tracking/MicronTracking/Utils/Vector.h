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
#ifndef __VECTOR_H__
#define __VECTOR_H__

class Vector
{
public:
  Vector(int h =0);
  ~Vector();
  int Handle();

  int getEndPos3x2(double* result);
  // int setEndPos3x2(double* newPos3x2); // Claudio
  int getEndXPoints(double* result);



private:
  int m_handle;
  bool ownedByMe;
};

#endif
