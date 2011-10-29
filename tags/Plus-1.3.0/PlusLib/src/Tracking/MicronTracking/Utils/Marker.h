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
#ifndef __MARKER_H__
#define __MARKER_H__

#include "MTC.h"
#include "Collection.h"
#include "MCamera.h"
#include "Facet.h"
#include "Xform3D.h"
#include "Persistence.h"

class Marker
{
public:
  Marker(int h=0);
  ~Marker();
  inline int getHandle(){ return m_handle; };
  inline void setHandle(int h){m_handle = h;};

  int getTemplateFacets();
  int restoreTemplate(int persistenceHandle, char* name);
  
  char* getName();
  void setName(char* name);
  bool wasIdentified(MCamera *cam);
  Xform3D* marker2CameraXf(int camHandle);
  int identifiedFacets (MCamera *cam);
  int addTemplateFacet(Facet* newFacet, Xform3D* facet1ToNewFacetXf);
  bool validateTemplate(double positionToleranceMM, string complString);
  int storeTemplate(Persistence* p, char* name);

private:
  int m_handle;
  bool ownedByMe;
  char m_MarkerName[400];

};

#endif
