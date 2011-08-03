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
#ifndef __FACET_H__
#define __FACET_H__

#include "MTC.h"
#include "MCamera.h"
#include "Vector.h"
#include "Collection.h"
#include "Xform3D.h"
#include <iostream>

class Facet
{
public:
  Facet(int h =0);
  ~Facet();
  inline int getHandle(){ return m_handle; };
  int getXpoints( MCamera *cam, double *result2x2x2x2); //[LV/SV][L/R][base/head][X/Y]
  vector<Vector *> IdentifiedVectors();
  vector<Vector *> TemplateVectors();
  bool setVectorsFromSample(vector<Collection*> &sampledVectorSets, string &outCompletionExplanation , double maxSampleErrorAllowedMM = 2.0);
  bool validateTemplate(double positionToleranceMM, string outCompletionString = NULL);
  bool identify(MCamera* cam, vector<Vector*> vectorSet, double positionToleranceMM);
  Xform3D* getFacet2CameraXf(MCamera* cam);


private:
  int m_handle;
  bool ownedByMe;
};

#endif
