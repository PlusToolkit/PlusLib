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
  inline mtHandle getHandle()
  {
    return Handle;
  };

  MicronTracker_Return addTemplate(mtHandle markerHandle);
  MicronTracker_Return clearTemplates();
  mtHandle identifiedMarkers(MCamera* cam);
  mtHandle unidentifiedVectors(MCamera* cam);

  MicronTracker_Return setPredictiveFramesInterleave(int level);
  int getPredictiveFramesInterleave();
  MicronTracker_Return setTemplateMatchToleranceMM(double newval);

  // Template match tolerance
  double getTemplateMatchToleranceMM();
  MicronTracker_Return setExtrapolatedFrames(int newval);
  int getExtrapolatedFrames();
  void setSmallerXPFootprint(bool newval);
  bool getSmallerXPFootprint();

  MicronTracker_Return processFrame(MCamera* cam);
  int getTemplateCount();
  mtHandle getTemplateItem(int idx);
  MicronTracker_Return getTemplateItemName(int idx, std::string& templateName);
  MicronTracker_Return getTemplateItemNameByHandle(mtHandle handle, std::string& templateName);
  MicronTracker_Return setTemplateItemName(int idx, const std::string& name);
  MicronTracker_Return storeTemplate(int idx, mtHandle pHandle, char* nameInP);
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
