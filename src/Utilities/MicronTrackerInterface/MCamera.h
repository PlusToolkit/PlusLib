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
  MicronTracker_Return setShutterTime(double sh);
  double getMaxShutterTime();
  double getMinShutterTime();

  double getShutterTimeLimit();
  double setShutterTimeLimit(double limit);

  double getGain();
  MicronTracker_Return setGain(double g);
  double getMinGain();
  double getMaxGain();

  double getGainLimit();
  double setGainLimit(double limit);

  MicronTracker_MeasurementHazardCode getThermalHazard();

  double getDBGain();

  double getExposure();
  MicronTracker_Return setExposure(double e);
  double getMinExposure();
  double getMaxExposure();

  int getSensorsNum();

  int getAutoExposure();
  MicronTracker_Return setAutoExposure(int ae);

  double getLightCoolness();
  MicronTracker_Return setLightCoolness(double value);
  MicronTracker_Return adjustCoolnessFromColorVector(mtHandle ColorVectorHandle);

  MicronTracker_Return getImages(unsigned char** *li, unsigned char** *ri);
  MicronTracker_Return getHalfSizeImages(unsigned char** *li, unsigned char** *ri, int xRes, int yRes);
  MicronTracker_Return getImages3(unsigned char** *li, unsigned char** *ri, unsigned char** *mi);
  MicronTracker_Return getHalfSizeImages3(unsigned char** *li, unsigned char** *ri, unsigned char** *mi, int xRes, int yRes);

  MicronTracker_Return getProjectionOnImage(int image, double XYZ[], double* x, double* y);

  MicronTracker_Return grabFrame(); // returns true for success
  bool getHdrModeEnabled();
  MicronTracker_Return setHdrModeEnabled(bool NewVal);

private:
  mtHandle        Handle;
  bool            OwnedByMe;
  unsigned char*  LeftImage;
  unsigned char*  RightImage;
  unsigned char*  MiddleImage;

};

#endif
