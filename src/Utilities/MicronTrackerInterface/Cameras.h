/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*
*     Written by:
*     Shi Sherebrin , Robarts Research Institute - London- Ontario , www.robarts.ca
*     Shahram Izadyar, Robarts Research Institute - London- Ontario , www.robarts.ca
*     Claudio Gatti, Ahmad Kolahi, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2013
*
***************************************************************/


#ifndef __CAMERAS_H__
#define __CAMERAS_H__

#include "MicronTrackerInterface.h"

#include <vector>

class MCamera;

class Cameras
{
public:
  Cameras();
  ~Cameras();
  mtHandle getHandle();
  int getCount();
  int setHistogramEqualizeImages(bool on_off);
  bool getHistogramEqualizeImages();
  MCamera* getCamera(int index);

  int attachAvailableCameras();
  void detach();

  int grabFrame(MCamera* camera = NULL);

protected:
  int getMTHome(std::string& homeDirectory);

protected:
  std::vector<MCamera*>   CameraList;
  mtHandle                Handle;
  bool                    OwnedByMe;
  MCamera*                CurrentCamera;
  int                     AttachedCameraCount;

  // Pointer to the camera object that the last operation failed on
  MCamera*                LastFailedCamera;
};

#endif
