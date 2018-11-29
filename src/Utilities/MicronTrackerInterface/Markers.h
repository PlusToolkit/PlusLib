/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*
*     Written by:
*     Shahram Izadyar, Robarts Research Institute - London- Ontario , www.robarts.ca
*     Claudio Gatti, Ahmad Kolahi, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*   Modified by:
*     Gregory Bootsma, Princess Margaret Hospital - Toronto - Ontario
*
*     Copyright Claron Technology 2000-2013
*
***************************************************************/
#ifndef __MARKERS_H__
#define __MARKERS_H__

class Collection;
class Marker;
class MCamera;

#include "MicronTrackerInterface.h"

#include <string.h>

class Markers
{
public:
  Markers();
  ~Markers();
  mtHandle getHandle();

  int addTemplate(mtHandle markerHandle);
  int clearTemplates();
  mtHandle identifiedMarkers(MCamera* cam);
  mtHandle unidentifiedVectors(MCamera* cam);

  int setPredictiveFramesInterleave(int level);
  int getPredictiveFramesInterleave();
  int setTemplateMatchToleranceMM(double newval);

  // Template match tolerance
  double getTemplateMatchToleranceMM();
  int setExtrapolatedFrames(int newval);
  int getExtrapolatedFrames();
  void setSmallerXPFootprint(bool newval);
  bool getSmallerXPFootprint();

  int processFrame(MCamera* cam);
  int getTemplateCount();
  mtHandle getTemplateItem(int idx);
  int getTemplateItemName(int idx, std::string& templateName);
  int getTemplateItemNameByHandle(mtHandle handle, std::string& templateName);
  int setTemplateItemName(int idx, const std::string& name);
  int storeTemplate(int idx, mtHandle pHandle, char* nameInP);
  void setAutoAdjustCam2CamRegistration(bool newVal);
  bool getAutoAdjustCam2CamRegistration();
  bool getBackGroundProcess();
  void setBackGroundProcess(bool newval);
  mtHandle getIdentifiedMarkersFromBackgroundThread(MCamera* cam);

protected:
  mtHandle  Handle;
  bool      OwnedByMe;
};

#endif
