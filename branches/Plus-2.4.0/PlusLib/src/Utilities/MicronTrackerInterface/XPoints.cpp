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

#include "MTC.h"

#include "XPoints.h"
#include "MTXPoint.h"
#include "MCamera.h"

/****************************/
/** Constructor */
XPoints::XPoints()
{
	//this->m_handle = XPoints_New();
	this->ownedByMe = TRUE;
}

/****************************/
/** Destructor */
XPoints::~XPoints()
{
	//if(this->m_handle != 0 && this->ownedByMe)
		//XPoints_Free(this->m_handle);
}

/****************************/
/** Return the handle to a collection of identified xpoints by the most recent processed frame. ?? */
mtHandle XPoints::detectedXPoints(MCamera *cam)
{
	mtHandle identifiedHandle = Collection_New();
	mtHandle camHandle = NULL;
	if (cam != NULL)
  {
		camHandle = cam->Handle();
	}
	XPoints_DetectedXPointsGet(camHandle, identifiedHandle );
	return identifiedHandle;
}

/****************************/
/** Return the value for the number of detected XPs*/
int XPoints::countXPoints(MCamera *cam)
{
	mtHandle camHandle = NULL;
	if (cam != NULL)
  {
		camHandle = cam->Handle();
	}
	return XPoints_Counter(camHandle);
}

/****************************/
/** Processes the current frame of the default camera. */
int XPoints::processFrame(MCamera *cam)
{
	mtHandle camHandle = NULL;
	if (cam != NULL)
  {
		camHandle = cam->Handle();
	}
	return XPoints_ProcessFrame(camHandle);
}
