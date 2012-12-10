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


#ifndef __CAMERAS_H__
#define __CAMERAS_H__
#include <vector>
#include "MCamera.h"

class Cameras
{
public:
  Cameras();
  ~Cameras();
  inline int getHandle() {return m_handle;};
  inline int getCount() {return m_attachedCamNums;};
  MCamera* getCamera(int index);

  int getShutterPreference();
  int setShutterPreference(int val);
  int getGainPreference();
  int setGainPreference(int val);

  // return with 0 if successful
  int AttachAvailableCameras();
  void Detach();

  // returns 0 if success, -1 if failed
  int grabFrame(MCamera *cam = NULL);

  void getLeftRightImageArray(unsigned char* &lImageArray, unsigned char* &rImageArray, int camIndex);

private:
  int getMTHome(std::string &mtHomeDirectory);

  int m_handle;
  bool ownedByMe;  

  // Pointer to the camera object that the last operation failed on
  MCamera* mFailedCam;

  MCamera* mCurrCam;
  
  std::vector<MCamera *> m_vCameras;

  int m_attachedCamNums;
};

#endif
