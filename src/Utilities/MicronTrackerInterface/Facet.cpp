/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*
*     Written by:
*     Shahram Izadyar, Robarts Research Institute - London - Ontario , www.robarts.ca
*     Claudio Gatti, Ahmad Kolahi, Claron Technology - Toronto - Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2013
*
***************************************************************/

#include <MTC.h>

#include "Facet.h"
#include "MCamera.h"
#include "Vector.h"
#include "Collection.h"
#include "Xform3D.h"

//----------------------------------------------------------------------------
Facet::Facet(mtHandle h)
{
  if (h != 0)
  {
    this->Handle = h;
  }
  else
  {
    this->Handle = Facet_New();
  }

  this->OwnedByMe = true;
}

//----------------------------------------------------------------------------
Facet::~Facet()
{
  if (this->Handle != 0 && this->OwnedByMe)
  {
    Facet_Free(this->Handle);
  }
}

//----------------------------------------------------------------------------
mtHandle Facet::getHandle()
{
  return this->Handle;
}

//----------------------------------------------------------------------------
int Facet::getXpoints(MCamera* cam, XPointsType_LS_LRM_BH_XY result2x3x2x2)
{
  mtHandle camHandle = NULL;
  if (cam != NULL)
  {
    camHandle = cam->getHandle();
  }
  return Facet_IdentifiedXPointsGet(this->Handle, camHandle, (double*)result2x3x2x2);
}

//----------------------------------------------------------------------------
std::vector<Vector*> Facet::TemplateVectors()
{
  std::vector<Vector*> iv;
  int r;

  iv.push_back(new Vector);
  iv.push_back(new Vector);

  r = Facet_TemplateVectorsGet(this->Handle, iv[0]->getHandle(), iv[1]->getHandle());
  return iv;
}

//----------------------------------------------------------------------------
std::vector<Vector*> Facet::IdentifiedVectors()
{
  std::vector<Vector*> iv;
  int r;

  iv.push_back(new Vector);
  iv.push_back(new Vector);

  r = Facet_IdentifiedVectorsGet(this->Handle, iv[0]->getHandle(), iv[1]->getHandle());
  return iv;
}

//----------------------------------------------------------------------------
int Facet::setVectorsFromSample(std::vector<Collection*>& sampledVectorSets, std::string& outCompletionExplanation, double maxSampleErrorAllowedMM)
{
  Collection* handlesCollection = new Collection();
  for (unsigned int i = 0; i < sampledVectorSets.size(); i++)
  {
    if (sampledVectorSets[i]->count() == 2)
    {
      handlesCollection->add(sampledVectorSets[i]->itemI(1));
      handlesCollection->add(sampledVectorSets[i]->itemI(2));
    }
  }

  if (handlesCollection->count() == 0)
  {
    return mtEmptyCollection;
  }
  return Facet_SetTemplateVectorsFromSamples(this->Handle, handlesCollection->getHandle(), maxSampleErrorAllowedMM);
}

//----------------------------------------------------------------------------
bool Facet::identify(MCamera* cam, std::vector<Vector*> vectorSet, double positionToleranceMM)
{
  mtHandle* vectorHandles;

  vectorHandles = (mtHandle*)malloc(vectorSet.size() * sizeof(int));

  for (unsigned int i = 0; i < vectorSet.size(); i++)
  {
    vectorHandles[i] = vectorSet[i]->getHandle();
  }

  mtHandle camHandle;
  if (cam == NULL)
  {
    camHandle = 0;
  }
  else
  {
    camHandle = cam->getHandle();
  }

  bool identified;
  Facet_Identify(this->Handle, cam->getHandle(), vectorHandles, static_cast<int>(vectorSet.size()),  &identified);
  free(vectorHandles);
  return identified;
}

//----------------------------------------------------------------------------
int Facet::validateTemplate(double positionToleranceMM, std::string outCompletionString)
{
  return Facet_ValidateTemplateVectors(this->Handle);
}

//----------------------------------------------------------------------------
Xform3D* Facet::getFacet2CameraXf(MCamera* cam)
{
  Xform3D* facet2CameraXf = new Xform3D();
  Facet_Facet2CameraXfGet(this->Handle, cam->getHandle(), facet2CameraXf->getHandle());
  return facet2CameraXf;
}
