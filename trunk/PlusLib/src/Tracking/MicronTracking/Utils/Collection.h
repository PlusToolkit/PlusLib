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
#ifndef __COLLECTION_H__
#define __COLLECTION_H__

#include "MTC.h"

class Collection
{
public:
  Collection(int h=0);
  ~Collection();
  inline int getHandle(){ return m_handle; };
  inline void setHandle(int h){m_handle = h; };
  
  void add(int val);
  void remove(int idx);
  int count();
  int itemI(int idx);
  double itemD(int idx);

private:
  int m_handle;
  bool ownedByMe;
};

#endif
