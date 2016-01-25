/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*   
*     Written by: 
*			Shahram Izadyar, Robarts Research Institute - London- Ontario , www.robarts.ca
*			Claudio Gatti, Ahmad Kolahi, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2013
*
***************************************************************/
#include "MTC.h"

#include "Collection.h"

Collection::Collection(mtHandle h)
{
  // If a handle is already passed to this class, then use that handle and don't create a new one.
  if (h != 0)
  {
    this->m_handle = h;
  }
  else
  {
    this->m_handle = Collection_New();
  }

  this->ownedByMe = TRUE;
}

/****************************/
/** Destructor */
Collection::~Collection()
{
  if (this->m_handle != 0 && this->ownedByMe == true)
  {
    Collection_Free(this->m_handle);
    this->m_handle = NULL;
  }
}

/****************************/
/** Add an item to the collection */
void Collection::add(mtHandle val)
{
  Collection_Add(this->m_handle, val, 0);
}

/****************************/
/** Remove an item from the collection */
void Collection::remove(int idx)
{
  Collection_Remove(this->m_handle, idx);
}

/****************************/
/** Return the count of the items in the collection */
int Collection::count()
{
  int result = Collection_Count(m_handle);
  return result;
}

/****************************/
/** Returns the integer item  of index  idx in the collection  */
mtHandle Collection::itemI(int idx)
{
  mtHandle hResult = 0;
  double dblResult = 0;
  Collection_Item(this->m_handle, idx, &hResult, &dblResult);
  return hResult;
}

/****************************/
/** Returns the integer item  of index  idx in the collection  */
double Collection::itemD(int idx)
{
	mtHandle hResult = 0;
	double dblResult = 0;
	Collection_Item(this->m_handle, idx, &hResult, &dblResult);
	return dblResult;
}
