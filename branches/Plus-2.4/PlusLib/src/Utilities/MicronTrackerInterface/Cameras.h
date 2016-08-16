/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*   
*     Written by: 
*			Shi Sherebrin , Robarts Research Institute - London- Ontario , www.robarts.ca
*			Shahram Izadyar, Robarts Research Institute - London- Ontario , www.robarts.ca
*			Claudio Gatti, Ahmad Kolahi, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2013
*
***************************************************************/


#ifndef __CAMERAS_H__
#define __CAMERAS_H__
#include <vector>
#include "MCamera.h"

class Cameras
{
public:
  Cameras();
  ~Cameras();
  inline mtHandle getHandle() {return m_handle;};
  inline int getCount() {return m_attachedCamNums;};
  int setHistogramEqualizeImages(bool on_off);
  bool getHistogramEqualizeImages();
  MCamera* getCamera(int index);

  int getShutterPreference();
  int setShutterPreference(int val);
  int getGainPreference();
  int setGainPreference(int val);

  // return with mtOK if successful
  int AttachAvailableCameras();
  void Detach();

  // returns mtOK if success, -1 if failed
  int grabFrame(MCamera *cam = NULL);

private:
  std::vector<MCamera *> m_vCameras;

  int getMTHome(std::string &mtHomeDirectory);

  mtHandle m_handle;
  bool ownedByMe;  

  MCamera* mCurrCam;

  // Pointer to the camera object that the last operation failed on
  MCamera* mFailedCam;

  int m_attachedCamNums;
};

#endif
