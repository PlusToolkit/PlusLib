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
#include <MTC.h>

#include "Collection.h"
#include "MCamera.h"
#include "Marker.h"
#include "Markers.h"

//----------------------------------------------------------------------------
Markers::Markers()
{
  //this->m_handle = Markers_New();
  this->OwnedByMe = true;
}

//----------------------------------------------------------------------------
Markers::~Markers()
{
  //if(this->m_handle != 0 && this->ownedByMe)
  //Markers_Free(this->m_handle);
}

//----------------------------------------------------------------------------
MicronTracker_Return Markers::storeTemplate(int idx, mtHandle pHandle, char* nameInP)
{
  int result = mtOK;
  mtHandle tHandle;
  result = Markers_TemplateItemGet(idx, &tHandle);
  if (result != mtOK)
  {
    return (MicronTracker_Return)result;
  }
  return (MicronTracker_Return)Marker_StoreTemplate(tHandle, pHandle, nameInP);
}


//----------------------------------------------------------------------------
MicronTracker_Return Markers::addTemplate(mtHandle markerHandle)
{
  return (MicronTracker_Return)Markers_AddTemplate(markerHandle);
}

//----------------------------------------------------------------------------
MicronTracker_Return Markers::clearTemplates()
{
  return (MicronTracker_Return)Markers_ClearTemplates();
}

//----------------------------------------------------------------------------
mtHandle Markers::identifiedMarkers(MCamera* cam)
{
  mtHandle identifiedHandle = Collection_New();
  mtHandle camHandle;
  if (cam == NULL)
  {
    camHandle = NULL;
  }
  else
  {
    camHandle = cam->getHandle();
  }
  Markers_IdentifiedMarkersGet(camHandle, identifiedHandle);
  return identifiedHandle;
}

//----------------------------------------------------------------------------
mtHandle Markers::unidentifiedVectors(MCamera* cam)
{
  mtHandle unidentifiedHandle = Collection_New();
  mtHandle camHandle;
  if (cam == NULL)
  {
    camHandle = NULL;
  }
  else
  {
    camHandle = cam->getHandle();
  }
  Markers_UnidentifiedVectorsGet(camHandle, unidentifiedHandle);
  return unidentifiedHandle;
}

//----------------------------------------------------------------------------
int Markers::getPredictiveFramesInterleave()
{
  int level = 0;
  Markers_PredictiveFramesInterleaveGet(&level);
  return level;
}

//----------------------------------------------------------------------------
MicronTracker_Return Markers::setPredictiveFramesInterleave(int level)
{
  return (MicronTracker_Return)Markers_PredictiveFramesInterleaveSet(level);
}

//----------------------------------------------------------------------------
int Markers::getExtrapolatedFrames()
{
  int result;
  Markers_ExtrapolatedFramesGet(&result);
  return result;
}

//----------------------------------------------------------------------------
MicronTracker_Return Markers::setExtrapolatedFrames(int newval)
{
  return (MicronTracker_Return)Markers_ExtrapolatedFramesSet(newval);
}

//----------------------------------------------------------------------------
bool Markers::getSmallerXPFootprint()
{
  return Markers_SmallerXPFootprint();
}

//----------------------------------------------------------------------------
void Markers::setSmallerXPFootprint(bool newval)
{
  Markers_SmallerXPFootprintSet(newval);
}

//----------------------------------------------------------------------------
double Markers::getTemplateMatchToleranceMM()
{
  double toleranceVal = 0;
  Markers_TemplateMatchToleranceMMGet(&toleranceVal);
  return toleranceVal;
}

//----------------------------------------------------------------------------
MicronTracker_Return Markers::setTemplateMatchToleranceMM(double newVal)
{
  return (MicronTracker_Return)Markers_TemplateMatchToleranceMMSet(newVal);
}

//----------------------------------------------------------------------------
int Markers::getTemplateCount()
{
  return Markers_TemplatesCount();
}

//----------------------------------------------------------------------------
void Markers::setAutoAdjustCam2CamRegistration(bool newVal)
{
  Markers_AutoAdjustCam2CamRegistrationSet(newVal);
}

//----------------------------------------------------------------------------
bool Markers::getAutoAdjustCam2CamRegistration()
{
  bool val = false;
  Markers_AutoAdjustCam2CamRegistrationGet(&val);
  return val;
}

//----------------------------------------------------------------------------
mtHandle Markers::getTemplateItem(int idx)
{
  mtHandle result = 0;
  Markers_TemplateItemGet(idx, &result);
  return result;
}

//----------------------------------------------------------------------------
MicronTracker_Return Markers::getTemplateItemName(int idx, std::string& templateName)
{
  mtHandle markerHandle = this->getTemplateItem(idx);
  return (MicronTracker_Return)getTemplateItemNameByHandle(markerHandle, templateName);
}

//----------------------------------------------------------------------------
MicronTracker_Return Markers::getTemplateItemNameByHandle(mtHandle handle, std::string& templateName)
{
  char markerName[512];
  memset((void*)markerName, 0, sizeof(markerName));
  int b = 0;
  mtCompletionCode status = Marker_NameGet(handle, markerName, sizeof(markerName), &b);
  if (status == mtOK)
  {
    markerName[b] = '\0';
  }
  templateName = std::string(markerName);

  return (MicronTracker_Return)status;
}

//----------------------------------------------------------------------------
MicronTracker_Return Markers::setTemplateItemName(int idx, const std::string& name)
{
  mtHandle markerHandle = this->getTemplateItem(idx);
  char* cname = new char[name.length() + 1];
  memcpy((void*)cname, (void*)name.c_str(), name.length());
  cname[name.length()] = 0;
  mtCompletionCode result = Marker_NameSet(markerHandle, cname);
  delete[] cname;
  return (MicronTracker_Return)result;
}

//----------------------------------------------------------------------------
MicronTracker_Return Markers::processFrame(MCamera* cam)
{
  mtHandle camHandle = NULL;
  if (cam != NULL)
  {
    camHandle = cam->getHandle();
  }
  return (MicronTracker_Return)Markers_ProcessFrame(camHandle);
}

//----------------------------------------------------------------------------
bool Markers::getBackGroundProcess()
{
  bool result = false;
  Markers_BackGroundProcessGet(&result);
  return result;
}

//----------------------------------------------------------------------------
void Markers::setBackGroundProcess(bool newval)
{
  Markers_BackGroundProcessSet(newval);
}

//----------------------------------------------------------------------------
mtHandle Markers::getIdentifiedMarkersFromBackgroundThread(MCamera* cam)
{
  mtHandle camHandle = NULL;
  if (cam != NULL)
  {
    camHandle = cam->getHandle();
  }
  return Markers_GetIdentifiedMarkersFromBackgroundThread(camHandle);
}
