/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*
*     Written by:
*     Shahram Izadyar, Robarts Research Institute - London - Ontario , www.robarts.ca
*     Claudio Gatti, Ahmad Kolahi, Claron Technology - Toronto - Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2013
*
***************************************************************/

#include <MTC.h>

#include "Collection.h"

//----------------------------------------------------------------------------
Collection::Collection(mtHandle h)
{
  // If a handle is already passed to this class, then use that handle and don't create a new one.
  if (h != 0)
  {
    this->Handle = h;
  }
  else
  {
    this->Handle = Collection_New();
  }

  this->OwnedByMe = true;
}

//----------------------------------------------------------------------------
Collection::~Collection()
{
  if (this->Handle != 0 && this->OwnedByMe == true)
  {
    Collection_Free(this->Handle);
    this->Handle = NULL;
  }
}

//----------------------------------------------------------------------------
mtHandle Collection::getHandle()
{
  return this->Handle;
}

//----------------------------------------------------------------------------
void Collection::setHandle(mtHandle h)
{
  this->Handle = h;
}

//----------------------------------------------------------------------------
void Collection::add(mtHandle val)
{
  Collection_Add(this->Handle, val, 0);
}

//----------------------------------------------------------------------------
void Collection::remove(int idx)
{
  Collection_Remove(this->Handle, idx);
}

//----------------------------------------------------------------------------
int Collection::count()
{
  int result = Collection_Count(this->Handle);
  return result;
}

//----------------------------------------------------------------------------
mtHandle Collection::itemI(int idx)
{
  mtHandle hResult = 0;
  double dblResult = 0;
  Collection_Item(this->Handle, idx, &hResult, &dblResult);
  return hResult;
}

//----------------------------------------------------------------------------
double Collection::itemD(int idx)
{
  mtHandle hResult = 0;
  double dblResult = 0;
  Collection_Item(this->Handle, idx, &hResult, &dblResult);
  return dblResult;
}
