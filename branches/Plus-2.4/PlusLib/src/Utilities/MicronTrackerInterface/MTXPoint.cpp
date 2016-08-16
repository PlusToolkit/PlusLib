/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*   
*     Written by: 
*			Ahmad Kolahi, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*	  Modified by:
*			Claudio Gatti, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2013
*
***************************************************************/
#include "MTXPoint.h"

#include <string>

/****************************/
/** Constructor */
MTXPoint::MTXPoint(mtHandle h)
{
	// If a handle is provided to this class, don't create a new one
	if (h != 0)
		this->m_handle = h;
	else
		this->m_handle = XPoint_New();
	this->ownedByMe = TRUE;
}

/****************************/
/** Destructor */
MTXPoint::~MTXPoint()
{
	if (this->m_handle != 0 && this->ownedByMe)
		XPoint_Free(this->m_handle);
}

/****************************/
int MTXPoint::getIndex()
{
	int Result;
	XPoint_IndexGet(this->m_handle, &Result);
	return Result;
}

/****************************/
int MTXPoint::setIndex(int index)
{
	int result = mtOK;
	result = XPoint_IndexSet(this->m_handle, index);
	return result;
}

/****************************/
int MTXPoint::Position3D(double* x, double* y, double* z)
{
  int R;
  R = XPoint_3DPositionGet(this->m_handle, x, y, z);
  return R;
}

/****************************/
int MTXPoint::Position2D(double* x0, double* y0, double* x1, double* y1, double* x2, double* y2)
{
  int R;
  R = XPoint_2DPositionGet(this->m_handle, x0, y0, x1, y1, x2, y2);
  return R;
}

/****************************/
double MTXPoint::Distance()
{
  int R;
  double dist;
  R = XPoint_DistGet(this->m_handle, &dist);
  return dist;
}

