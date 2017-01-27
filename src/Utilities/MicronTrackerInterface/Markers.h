/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*   
*     Written by: 
*			Shahram Izadyar, Robarts Research Institute - London- Ontario , www.robarts.ca
*			Claudio Gatti, Ahmad Kolahi, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*	  Modified by:
*			Gregory Bootsma, Princess Margaret Hospital - Toronto - Ontario
*
*     Copyright Claron Technology 2000-2013
*
***************************************************************/
#ifndef __MARKERS_H__
#define __MARKERS_H__

#include "Collection.h"
#include "Marker.h"
#include "MCamera.h"
#include <string.h>

class Markers
{
public:
  Markers();
  ~Markers();
  inline mtHandle getHandle(){ return m_handle; };

  int addTemplate(mtHandle markerHandle);
  int clearTemplates();
  mtHandle identifiedMarkers(MCamera *cam);
  mtHandle unidentifiedVectors(MCamera *cam);

  int setPredictiveFramesInterleave(int level);
  int getPredictiveFramesInterleave();
  int setTemplateMatchToleranceMM(double newval);

  // Template match tolerance
  double getTemplateMatchToleranceMM();
  void setExtrapolatedFrames(int newval);
  int getExtrapolatedFrames();
  void setSmallerXPFootprint(bool newval);
  bool getSmallerXPFootprint();

  int processFrame(MCamera *cam);
  int getTemplateCount();
  mtHandle getTemplateItem(int idx);
  mtCompletionCode getTemplateItemName(int idx, std::string &templateName);
  mtCompletionCode getTemplateItemHName(mtHandle handle, std::string &templateName);
  int setTemplateItemName(int idx, char* name);
  int storeTemplate( int idx, mtHandle pHandle, char* nameInP);
  int restoreTemplate(mtHandle pHandle, char* nameInP);
  void setAutoAdjustCam2CamRegistration(bool newVal);
  bool getAutoAdjustCam2CamRegistration();
  bool getBackGroundProcess();
  void setBackGroundProcess(bool newval);
  mtHandle getIdentifiedMarkersFromBackgroundThread(MCamera *cam);

private:
  mtHandle m_handle;
  bool ownedByMe;
  char tempString[400];

};

#endif
