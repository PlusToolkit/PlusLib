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
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "MicronTrackerInterface.h"

#include <vector>

class MCamera
{
public:
  MCamera(mtHandle handle = 0);
  ~MCamera();
  mtHandle getHandle();

  int getXRes();
  int getYRes();
  int getSerialNum();
  int getBitsPerPixel();
  double getFrameTime();
  int getNumOfFramesGrabbed();

  double getShutterTime();
  int setShutterTime(double sh);
  double getMaxShutterTime();
  double getMinShutterTime();

  double getShutterTimeLimit();
  double setShutterTimeLimit(double limit);

  double getGain();
  int setGain(double g);
  double getMinGain();
  double getMaxGain();

  double getGainLimit();
  double setGainLimit(double limit);

  int getThermalHazard();

  double getDBGain();

  double getExposure();
  int setExposure(double e);
  double getMinExposure();
  double getMaxExposure();

  int getSensorsNum();

  int getAutoExposure();
  int setAutoExposure(int ae);

  double getLightCoolness();
  int setLightCoolness(double value);
  int adjustCoolnessFromColorVector(mtHandle ColorVectorHandle);

  int getImages(unsigned char** *li, unsigned char** *ri);
  int getHalfSizeImages(unsigned char** *li, unsigned char** *ri, int xRes, int yRes);
  int getImages3(unsigned char** *li, unsigned char** *ri, unsigned char** *mi);
  int getHalfSizeImages3(unsigned char** *li, unsigned char** *ri, unsigned char** *mi, int xRes, int yRes);

  int getProjectionOnImage(int image, double XYZ[], double* x, double* y);

  int grabFrame(); // returns true for success
  bool getHdrModeEnabled();
  int setHdrModeEnabled(bool NewVal);

private:
  mtHandle        Handle;
  bool            OwnedByMe;
  unsigned char*  LeftImage;
  unsigned char*  RightImage;
  unsigned char*  MiddleImage;

};

#endif
