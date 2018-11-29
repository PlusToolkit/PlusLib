/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*
*     Written by:
*     Ahmad Kolahi, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*   Modified by:
*     Claudio Gatti, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2013
*
***************************************************************/

#include "MTC.h"

#include "XPoints.h"
#include "MTXPoint.h"
#include "MCamera.h"

//----------------------------------------------------------------------------
XPoints::XPoints()
  : Handle(mtHandleNull)
{
  this->OwnedByMe = true;
}

//----------------------------------------------------------------------------
XPoints::~XPoints()
{

}

//----------------------------------------------------------------------------
mtHandle XPoints::getHandle()
{
  return this->Handle;
}

//----------------------------------------------------------------------------
/** Return the handle to a collection of identified xpoints by the most recent processed frame. ?? */
mtHandle XPoints::detectedXPoints(MCamera* cam)
{
  mtHandle identifiedHandle = Collection_New();
  mtHandle camHandle = NULL;
  if (cam != NULL)
  {
    camHandle = cam->getHandle();
  }
  XPoints_DetectedXPointsGet(camHandle, identifiedHandle);
  return identifiedHandle;
}

//----------------------------------------------------------------------------
int XPoints::countXPoints(MCamera* cam)
{
  mtHandle camHandle = NULL;
  if (cam != NULL)
  {
    camHandle = cam->getHandle();
  }
  return XPoints_Counter(camHandle);
}

//----------------------------------------------------------------------------
int XPoints::processFrame(MCamera* cam)
{
  mtHandle camHandle = NULL;
  if (cam != NULL)
  {
    camHandle = cam->getHandle();
  }
  return XPoints_ProcessFrame(camHandle);
}
