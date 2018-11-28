/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*
*     Written by:
*     Shahram Izadyar, Robarts Research Institute - London- Ontario , www.robarts.ca
*     Claudio Gatti, Ahmad Kolahi, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2013
*
***************************************************************/
#ifndef __MARKER_H__
#define __MARKER_H__

#include "MicronTrackerInterface.h"

class MCamera;
class Facet;
class Xform3D;
class Persistence;

class Marker
{
public:
  Marker(mtHandle h = 0);
  ~Marker();
  mtHandle getHandle();

  mtHandle getTemplateFacets();

  std::string getName();
  int setName(char* name);
  bool wasIdentified(MCamera* cam);
  Xform3D* marker2CameraXf(mtHandle camHandle);
  Xform3D* tooltip2MarkerXf();
  mtHandle identifiedFacets(MCamera* cam);
  int addTemplateFacet(Facet* newFacet, Xform3D* facet1ToNewFacetXf);
  int validateTemplate(double positionToleranceMM, std::string complString);
  int storeTemplate(Persistence* p, const char* name);

protected:
  mtHandle    Handle;
  bool        OwnedByMe;
  std::string MarkerName;
};

#endif
