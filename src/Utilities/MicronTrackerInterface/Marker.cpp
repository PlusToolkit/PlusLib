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

#include "Collection.h"
#include "Facet.h"
#include "MCamera.h"
#include "Marker.h"
#include "Persistence.h"
#include "Xform3D.h"

#include <string>

//----------------------------------------------------------------------------
Marker::Marker(mtHandle h)
{
  // If a handle is provided to this class, don't create a new one
  if (h != 0)
  {
    this->Handle = h;
  }
  else
  {
    this->Handle = Marker_New();
  }
  this->OwnedByMe = true;
}

//----------------------------------------------------------------------------
Marker::~Marker()
{
  if (this->Handle != 0 && this->OwnedByMe)
  {
    Marker_Free(this->Handle);
    this->Handle = NULL;
  }
}

//----------------------------------------------------------------------------
mtHandle Marker::getHandle()
{
  return this->Handle;
}

//----------------------------------------------------------------------------
mtHandle Marker::identifiedFacets(MCamera* cam)
{
  mtHandle camHandle = NULL;
  if (cam != NULL)
  {
    camHandle = cam->getHandle();
  }
  mtHandle identifiedHandle = Collection_New();
  Marker_IdentifiedFacetsGet(this->Handle, camHandle, true, identifiedHandle);
  return identifiedHandle;
}

//----------------------------------------------------------------------------
mtHandle Marker::getTemplateFacets()
{
  mtHandle templateFacetsColl = 0;
  Marker_TemplateFacetsGet(this->Handle, &templateFacetsColl);
  return templateFacetsColl;
}

//----------------------------------------------------------------------------
bool Marker::wasIdentified(MCamera* cam)
{
  mtHandle camHandle = NULL;
  if (cam != NULL)
  {
    camHandle = cam->getHandle();
  }
  bool result = false;
  int stat = Marker_WasIdentifiedGet(this->Handle, camHandle, &result);
  return result;
}

//----------------------------------------------------------------------------
Xform3D* Marker::marker2CameraXf(mtHandle camHandle)
{
  Xform3D* xf = new Xform3D;
  mtHandle identifyingCamHandle = 0;
  int result = Marker_Marker2CameraXfGet(this->Handle, camHandle, xf->getHandle(), &identifyingCamHandle);
  // if the result is ok then return the handle, otherwise return NULL
  if (result != mtOK)
  {
    return NULL;
  }
  return xf;
}

//----------------------------------------------------------------------------
Xform3D* Marker::tooltip2MarkerXf()
{
  Xform3D* xf = new Xform3D;
  int result = Marker_Tooltip2MarkerXfGet(this->Handle, xf->getHandle());

  // if the result is ok then return the handle, otherwise return NULL
  if (result != mtOK)
  {
    return NULL;
  }
  return xf;
}

//----------------------------------------------------------------------------
std::string Marker::getName()
{
  const int BUF_SIZE = 400;
  char buf[BUF_SIZE + 1];
  buf[BUF_SIZE] = '\0';
  int size = 0;
  mtCompletionCode st = Marker_NameGet(this->Handle, buf, BUF_SIZE, &size);
  buf[size] = '\0';
  this->MarkerName = buf;
  return this->MarkerName;
}

//----------------------------------------------------------------------------
int Marker::setName(char* name)
{
  return Marker_NameSet(this->Handle, name);
}

//----------------------------------------------------------------------------
int Marker::addTemplateFacet(Facet* newFacet, Xform3D* facet1ToNewFacetXf)
{
  return Marker_AddTemplateFacet(this->Handle, newFacet->getHandle(), facet1ToNewFacetXf->getHandle());
}

//----------------------------------------------------------------------------
int Marker::validateTemplate(double positionToleranceMM, std::string complString)
{
  Collection* facetsColl = new Collection(this->getTemplateFacets());
  int result;

  for (int k = 1; k < facetsColl->count(); k++)
  {
    Facet* f = new Facet(facetsColl->itemI(k));
    result = f->validateTemplate(positionToleranceMM, complString);
    if (result != mtOK)
    {
      return result;
    }
  }

  std::vector<Vector*> vs;
  for (int fi = 1; fi < facetsColl->count() - 1; fi++)
  {
    Facet* Fti = new Facet(fi);
    for (int fj = fi + 1; fj < facetsColl->count(); fj++)
    {
      Facet* Ftj = new Facet(fj);
      vs = Ftj->TemplateVectors();
      if (Fti->identify(NULL, vs, 2 * positionToleranceMM))
      {
        return mtDifferentFacetsGeometryTooSimilar;
      }
    }
  }
  /*
    'Check that two facets cannot be confused with each other, ie
    'their positions are at least twice the tolerance
    Dim fi, fj, Fti As Facet, Ftj As Facet, Vs(2) As Vector
    For fi = 1 To TemplateFacets.Count - 1
      Set Fti = TemplateFacets(fi)
      For fj = fi + 1 To TemplateFacets.Count
        Set Ftj = TemplateFacets(fj)
        Set Vs(0) = Ftj.TemplateVectors(1)
        Set Vs(1) = Ftj.TemplateVectors(2)
        If Fti.Identify(Nothing, Vs, 2, 2 * PositionToleranceMM) Then
          ValidateTemplate = False 'mtDifferentFacetsGeometryTooSimilar
          If Not IsMissing(OutCompletionString) Then
            OutCompletionString = "geometry of facets " & fi & " and " & fj & " too similar"
          End If
          Exit Function
        End If
      Next
    Next
    ValidateTemplate = True
  End Function
    */
  return mtOK;
}

//----------------------------------------------------------------------------
int Marker::storeTemplate(Persistence* p, const char* name)
{
  return Marker_StoreTemplate(this->Handle, p->getHandle(), name);
}
