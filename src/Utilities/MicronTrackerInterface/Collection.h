/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*
*     Written by:
*     Shahram Izadyar, Robarts Research Institute - London- Ontario , www.robarts.ca
*     Claudio Gatti, Ahmad Kolahi, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2013
*
***************************************************************/
#ifndef __COLLECTION_H__
#define __COLLECTION_H__

#include "MicronTrackerInterface.h"

class Collection
{
public:
  Collection(mtHandle h = 0);
  ~Collection();
  mtHandle getHandle();;
  void setHandle(mtHandle h);;

  void add(mtHandle val);
  void remove(int idx);
  int count();
  mtHandle itemI(int idx);
  double itemD(int idx);

private:
  mtHandle  Handle;
  bool      OwnedByMe;
};

#endif
