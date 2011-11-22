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
#include "Markers.h"
#include "MTC.h"
#include "Marker.h"
#include "MCamera.h"

/****************************/
/** Constructor */
Markers::Markers()
{
  //this->m_handle = Markers_New();
  this->ownedByMe = TRUE;
}

/****************************/
/** Destructor */
Markers::~Markers()
{
  //if(this->m_handle != 0 && this->ownedByMe)
    //Markers_Free(this->m_handle);
}

/****************************/
/** Restore a Marker template and add it to the know Markers. If successful returns 0. Otherwise non-zero value is returned. */
int Markers::restoreTemplate( int pHandle, char* nameInP)
{
  int result;
  result = Marker_RestoreTemplate(this->m_handle, pHandle, nameInP);
  return result;
}

/****************************/
/** Store a Marker template If successful returns 0. Otherwise non-zero value is returned. */
int Markers::storeTemplate( int idx, int pHandle, char* nameInP)
{
  int result, tHandle;
  result = Markers_TemplateItemGet(idx, &tHandle);
  if (result != mtOK) return result;
  result = Marker_StoreTemplate(tHandle, pHandle, nameInP);
  return result;
}


/****************************/
/** Add a template to the markers. If successful returns 0. Otherwise non-zero value is returned. */
int Markers::addTemplate(int markerHandle)
{
  int result;
  result = Markers_AddTemplate( markerHandle);
  return result;
}

/****************************/
/** Clears the templates of this marker. Returns 0 if successful. Otherwise a non_zero value is returned. */
int Markers::clearTemplates()
{
  int result;
  result = Markers_ClearTemplates();
  return result;
}


/****************************/
/** Return the handle to a collection of identified markers by the most recent processed frame. ?? */
int Markers::identifiedMarkers(MCamera *cam)
{
  int identifiedHandle = Collection_New();
  int camHandle;
  if (cam == NULL) {
    camHandle = NULL;
  } else {
    camHandle = cam->Handle();
  }
  Markers_IdentifiedMarkersGet(camHandle, identifiedHandle );
  return identifiedHandle;
}

/****************************/
/** Return the handle to a collection of unidentified vestors by the most recent processed frame. */
int Markers::unidentifiedVectors(MCamera *cam)
{
  int unidentifiedHandle = Collection_New();
  int camHandle;
  if (cam == NULL) {
    camHandle = NULL;
  } else {
    camHandle = cam->Handle();
  }
  Markers_UnidentifiedVectorsGet(camHandle, unidentifiedHandle);
  return unidentifiedHandle;
}

/****************************/
/** Reutrn the value for the predictive frames interleave */
int Markers::getPredictiveFramesInterleave()
{
  int level ;
  Markers_PredictiveFramesInterleaveGet( &level); // Claudio
  return level;
}

/****************************/
/** Set the value of the predictive frames interleave. Returns 0 if successful, -1 if not. */
int Markers::setPredictiveFramesInterleave(int level)
{
  int result = Markers_PredictiveFramesInterleaveSet(level);
  return (result == mtOK ? result : -1);

}

/****************************/
/** Return the value of the templateMatchToleranceMM */
double Markers::getTemplateMatchToleranceMM()
{
  double toleranceVal = 0;
  int result = Markers_TemplateMatchToleranceMMGet(&toleranceVal);
  return (result == mtOK ? toleranceVal : -1);
}

/****************************/
/** Return the value of the templateMatchToleranceMM. */
double Markers::getDefaultTemplateMatchToleranceMM()
{
  double defToleranceVal = 0;
  Markers_TemplateMatchToleranceMMDefaultGet(&defToleranceVal);
  return defToleranceVal;
}

/****************************/
/** Set the value of the templateMatchToleranceMM. Returns 0 if successful, -1 if not. */

int Markers::setTemplateMatchToleranceMM(double newVal)
{
  int result = Markers_TemplateMatchToleranceMMSet( newVal);
  return (result == mtOK ? result : -1);
}

/****************************/
/** Returns the number of templates. */
int Markers::getTemplateCount()
{
  return Markers_TemplatesCount();
}

/****************************/
/** Returns the handle to the template item with the index number of idx . */
int Markers::getTemplateItem(int idx)
{
  int result = 0;
  Markers_TemplateItemGet( idx, &result);
  return result;
}

/****************************/
/** Returns the name of the template item with the index number of idx. */
char* Markers::getTemplateItemName(int idx)
{
  int markerHandle = this->getTemplateItem(idx);
  
  memset((void *)tempString, 0 , sizeof(tempString));
  int b;
  mtCompletionCode status = Marker_NameGet( markerHandle, tempString, sizeof(tempString), &b);
  if ( status == mtOK) {
    tempString[b] = '\0';
  }

  return tempString;
}

/****************************/
/** */
int Markers::setTemplateItemName(int idx, char* name)
{
  int markerHandle = this->getTemplateItem(idx);
  return Marker_NameSet( markerHandle, name); 
}

/****************************/
/** Processes the current frame of the default camera. */
int Markers::processFrame(MCamera *cam)
{
  int camHandle;
  if (cam == NULL) {
    camHandle = NULL;
  } else {
    camHandle = cam->Handle();
  }

  return Markers_ProcessFrame( camHandle);
}
