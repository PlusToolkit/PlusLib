/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*   
*     Written by: 
*			Shahram Izadyar, Robarts Research Institute - London- Ontario , www.robarts.ca
*			Claudio Gatti, Ahmad Kolahi, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2013
*
***************************************************************/
#ifndef __FACET_H__
#define __FACET_H__

#include "MCamera.h"
#include "Vector.h"
#include "Collection.h"
#include "Xform3D.h"

class Facet
{
public:
  // long/short vector; left/right/middle camera; base/head; x/y coordinate
  typedef double XPointsType_LS_LRM_BH_XY[2][3][2][2];

  Facet(mtHandle h =0);
  ~Facet();
  inline mtHandle getHandle(){ return m_handle; };
  int getXpoints( MCamera *cam, XPointsType_LS_LRM_BH_XY result2x3x2x2); //[LV/SV][L/R/M][base/head][X/Y]
  std::vector<Vector *> IdentifiedVectors();
  std::vector<Vector *> TemplateVectors();
  bool setVectorsFromSample(std::vector<Collection*> &sampledVectorSets, std::string &outCompletionExplanation , double maxSampleErrorAllowedMM = 2.0);
  bool validateTemplate(double positionToleranceMM, std::string outCompletionString = NULL);
  bool identify(MCamera* cam, std::vector<Vector*> vectorSet, double positionToleranceMM);
  Xform3D* getFacet2CameraXf(MCamera* cam);


private:
  mtHandle m_handle;
  bool ownedByMe;
};

#endif
