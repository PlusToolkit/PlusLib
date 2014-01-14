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
#ifndef __MARKERS_H__
#define __MARKERS_H__

#include "Collection.h"
#include "Marker.h"
#include "MCamera.h"

class Markers
{
public:
  Markers();
  ~Markers();
  inline int getHandle(){ return m_handle; };

  int addTemplate(int markerHandle);
  int clearTemplates();
  int identifiedMarkers(MCamera *cam);
  int unidentifiedVectors(MCamera *cam);

  int setPredictiveFramesInterleave(int level);
  int getPredictiveFramesInterleave();
  int setTemplateMatchToleranceMM(double newval);

  // Template match tolerance
  double getTemplateMatchToleranceMM();
  double getDefaultTemplateMatchToleranceMM();

  int processFrame(MCamera *cam);
  int getTemplateCount();
  int getTemplateItem(int idx);
  char* getTemplateItemName(int idx);
  int setTemplateItemName(int idx, char* name);
  int storeTemplate( int idx, int pHandle, char* nameInP);
  int restoreTemplate(int pHandle, char* nameInP);

private:
  int m_handle;
  bool ownedByMe;
  char tempString[400];

};

#endif
