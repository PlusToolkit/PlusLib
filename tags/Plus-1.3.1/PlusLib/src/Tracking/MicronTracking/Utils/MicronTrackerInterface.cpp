//------------------------------------------
/* MicronTracker Windows API */
/* Author: SI- Robarts Research Institute */
/* Date: June 9, 2003 */
/* Modified: June 20, 2003 This file has been converted to a class to make it more compatible with OOD */
/****************************/

#define __MICRONTRACKERINTERFACE_CPP__
#include "MicronTrackerInterface.h"
#include "UtilityFunctions.h"
#include "Cameras.h"
//------------------------------------------
/** Destructor */
MicronTrackerInterface::~MicronTrackerInterface()
{
}

//------------------------------------------
int MicronTrackerInterface::mtInit()
{
  this->m_errorString = "No error!";
  this->isCameraAttached = false;
  this->m_pMarkers = new Markers();
  this->m_pPers = new Persistence();
  this->initialINIAccess();
  //m_pCurrTempMarker =  new Marker();
  this->m_pTempMarkerForAddingFacet = NULL; //new Marker();
  
  return 1;
}

//------------------------------------------
void MicronTrackerInterface::mtEnd()
{
  this->mtDetachCameras();  
}

//------------------------------------------
std::string MicronTrackerInterface::mtGetCurrDir()
{
  char currDir[255];
  mtUtils::getCurrPath(currDir);
  return currDir;
}

//------------------------------------------
void MicronTrackerInterface::initialINIAccess()
{
  const int currDirSize=255;
  char currDir[currDirSize+1];
  currDir[currDirSize]=0;
  mtUtils::getCurrPath(currDir);
  strcat_s(currDir,currDirSize,"\\MicronTracker Demo.ini");
  this->m_pPers->setPath(currDir);
  this->m_pPers->setSection ("General");
  
  //Setting the FrameInterleave property in the Markers object
  int defaultFrameInterleave = 0;
  this->m_pMarkers->setPredictiveFramesInterleave(this->m_pPers->retrieveInt("PredictiveFramesInterleave", defaultFrameInterleave) );
  
  //Setting the TemplateMatchToleranceMM property in the Markers object
  double defaultTempMatchToleranceMM = 1.0;
  this->m_pMarkers->setTemplateMatchToleranceMM( this->m_pPers->retrieveDouble("TemplateMatchToleranceMM", defaultTempMatchToleranceMM) );
}

//------------------------------------------
void MicronTrackerInterface::mtUpdateINI()
{
  this->m_pPers->saveDouble( "TemplateMatchToleranceMM", this->m_pMarkers->getTemplateMatchToleranceMM() );
  this->m_pPers->saveInt( "PredictiveFramesInterleave", this->m_pMarkers->getPredictiveFramesInterleave() );
}

//------------------------------------------
int MicronTrackerInterface::mtSetupCameras()
{
  this->m_pCameras = new Cameras();
  int result = m_pCameras->AttachAvailableCameras();
  
  if (result == mtOK &&  m_pCameras->getCount() >= 1 )
    {
      // Set the current camera to the first in the list. 
      this->m_currCamIndex = 0;
      this->m_pCurrCam = this->m_pCameras->getCamera(this->m_currCamIndex);
      this->isCameraAttached = true;
      
      return 1;
    }
  else
    {
      this->m_errorString = this->handleErrors(result);
      return 0;
    }
}

//------------------------------------------
void MicronTrackerInterface::mtDetachCameras()
{
  if ( isCameraAttached )
    {
      isCameraAttached = false;
      MTexit();     
    }
}

//------------------------------------------
// Checks to see if the passed argument is within the camera index, i.e. it is not
//    less than 0 or more than the number of attached camera (minus 1, of course). 

bool MicronTrackerInterface::checkCamIndex(int id)
{
  if (id > -1 && id < this->m_pCameras->getCount())
    {
      return true;
    }
  else
    {
      this->m_errorString = "Camera index out of bound.";
      return false;
    }
}

//------------------------------------------
int MicronTrackerInterface::mtGetSerialNum(int index)
{
  if (index == -1)
    {
      return this->m_pCurrCam->getSerialNum();
    }
  
  else if (!this->checkCamIndex(index))
    {
      return -1;
    }
  else
    {
      return this->m_pCameras->getCamera(index)->getSerialNum();
    }
}

//------------------------------------------
int MicronTrackerInterface::mtGetXResolution(int index)
{
  if (index == -1)
    {
      return this->m_pCurrCam->getXRes();
    }
  else if (!this->checkCamIndex(index))
    {
      return -1;
    }
  else
    {
      return this->m_pCameras->getCamera(index)->getXRes();
    }
}

//------------------------------------------
int MicronTrackerInterface::mtGetYResolution(int index)
{
  if (index == -1)
    {
      return this->m_pCurrCam->getYRes();
    }
  else if (!this->checkCamIndex(index))
    {
      return -1;
    }
  else
    {
      return this->m_pCameras->getCamera(index)->getYRes();
    }
}

//------------------------------------------
int MicronTrackerInterface::mtGetNumOfCameras()
{
  return this->m_pCameras->getCount();
}
//------------------------------------------
int MicronTrackerInterface::mtRefreshTemplates(std::vector<std::string> &tmplsName,
                 std::vector<std::string> &tmplsError)
{
  mtUtils::getFileNamesFromDirectory( tmplsName,"Markers", true);
  int result = this->m_pMarkers->clearTemplates();
  std::cout << "Cleared templates..." << std::endl;
  const int currentFolderPathSize=MT_MAX_STRING_LENGTH;
  char currentFolderPath[currentFolderPathSize+1];
  currentFolderPath[currentFolderPathSize]=0;
  mtCompletionCode st;
  mtUtils::getCurrPath(currentFolderPath);
  
#ifdef WIN32
  strcat_s(currentFolderPath, MT_MAX_STRING_LENGTH-1, "\\Markers\\");
#else
  strcat(currentFolderPath, "/Markers/");
#endif
  if (0 == result)
    {
      st = Markers_LoadTemplates(currentFolderPath);
    }
  return (int)st;
}
//------------------------------------------
int MicronTrackerInterface::mtGrabFrame(int index)
{
  int result = -1;
  if (index == -1) // From all the cameras
    {
      result = (true == this->m_pCameras->grabFrame()) ? 0 : -1;
    }
  else if (false == this->checkCamIndex(index)) // Camera index out of range
    {
      result = -1;
    }
  else // From one camera
    {
      result = (true == this->m_pCameras->grabFrame(this->m_pCameras->getCamera(index))) ? 0 : -1;
    }

  return result;
}

//------------------------------------------
int MicronTrackerInterface::mtProcessFrame()
{
  int result = this->m_pMarkers->processFrame(this->m_pCurrCam);
  if (result != mtOK)
    {
      result = -1;
      this->m_errorString = this->handleErrors(result);
    }
  return result;
}

//------------------------------------------
int MicronTrackerInterface::mtCollectNewSamples(int collectingAdditionalFacet)
{
  int result = 0;
  Collection* col = new Collection( this->m_pMarkers->unidentifiedVectors(this->m_pCurrCam) );
  this->m_isAddingAdditionalFacet = collectingAdditionalFacet;
  if (col->count() == 2 || collectingAdditionalFacet == 1)
    {
      this->m_sampleVectors.push_back(col);
      result = 0;
      //    this->m_collectedSampleFrames++;
      // Additional facets
      if (this->m_isAddingAdditionalFacet)
  {
    Collection* markersCollection = new Collection(this->m_pMarkers->identifiedMarkers(this->m_pCurrCam));
    this->m_pTempMarkerForAddingFacet = new Marker(markersCollection->itemI(1));
    Collection* identifiedFacetsCol = new Collection(this->m_pTempMarkerForAddingFacet->getTemplateFacets());
    if (identifiedFacetsCol->count() > 0)
      {  
        // Also compute and save the xform of facet1 to sensor
        // Note (Programming Note): The following if block has been commented out because it
        // relates to registering a marker with two cameras. As soon as the second camera is 
        // in, we can start working on it.
        //if (this->m_pCurrMarker->marker2CameraXf(this->m_pCurrCam->getHandle() != (Xform3D*)NULL)
        this->facet1ToCameraXfs.push_back(this->m_pTempMarkerForAddingFacet->marker2CameraXf(this->m_pCurrCam->getHandle()));
        
        //else // Seen by a camera not registered with this one
        //{
        //  this->collectingStatus->label("Cannot sample: unregistered camera");
        //  this->sampleVectors.pop_back();
        //  this->m_collectedSampleFrames--;
        //}
        return result;
      }
    

    else // Not seeing a known facet
      {
        //this->collectingStatus->label("Cannot sample: no known facet!");
        this->m_sampleVectors.pop_back();
        delete identifiedFacetsCol;
        return 99;        
      }
    delete identifiedFacetsCol;
    //delete markersCollection;
  }
      
      //    char buffer[255];
      //    sprintf(buffer, "% d", this->m_collectedSampleFrames);
      //    //strcat("Collected: ", buffer);
      //    this->collectingNum->label(buffer);
    }
  else if(col->count() < 2) 
    {
      result = -1;
    }
  else if(col->count() > 2)
    {
      result = 1;
    }
  
  //  delete col;
  return result;
}

//------------------------------------------
int MicronTrackerInterface::mtStopSampling(char* templateName, double jitterValue)
{  
  std::string errorsString;
  Facet* f = new Facet();
  std::vector<Vector*> vectorPair;
  int result = 0;
  if (f->setVectorsFromSample(this->m_sampleVectors, errorsString))
    {
      if (this->m_isAddingAdditionalFacet)
  {
    Collection* markersCollection = new Collection(this->m_pMarkers->identifiedMarkers(this->m_pCurrCam));
    //this->m_pTempMarkerForAddingFacet->setHandle(markersCollection->itemI(1));
    Collection* identifiedFacetsCol = new Collection(this->m_pTempMarkerForAddingFacet->getTemplateFacets());
    std::vector<Xform3D*> facet1ToNewFacetXfs;
    Xform3D* facet1ToNewFacetXf;// = new Xform3D();
    
    for (unsigned int i=0; i<this->m_sampleVectors.size(); i++)
      {
        vectorPair.clear();
        vectorPair.push_back(new Vector( this->m_sampleVectors[i]->itemI(0) ));
        vectorPair.push_back(new Vector( this->m_sampleVectors[i]->itemI(1) ));
        //Programming Note: The following line is crashing in the Facet.cpp when
        //it makes a call to the DLL. As a result no registration of multi-facet markers can be made at this point. June 7, 2004
        if (f->identify(this->m_pCurrCam, vectorPair, 1)) // Then the sample matches the template
    {
      // Compute the xform between the first marker facet and the new one
      facet1ToNewFacetXf = this->facet1ToCameraXfs[i]->concatenate(f->getFacet2CameraXf(this->m_pCurrCam)->inverse());
      facet1ToNewFacetXfs.push_back(facet1ToNewFacetXf);
    }
   
      }
    // Combine the transforms accumulated to a new one and save it with the facet in the marker
    facet1ToNewFacetXf = facet1ToNewFacetXfs[1];
    for (unsigned int i=2; i<facet1ToNewFacetXfs.size(); i++)
      {
        facet1ToNewFacetXf->inBetween(facet1ToNewFacetXfs[i], 1);// will result in equal contribution by all faces
      }

      this->m_pTempMarkerForAddingFacet->addTemplateFacet(f, facet1ToNewFacetXf);


  }
      else
  {
    this->m_pCurrMarker = new Marker();
    Xform3D* Xf = new Xform3D();
    this->m_pCurrMarker->setName(templateName);
    result = this->m_pCurrMarker->addTemplateFacet(f, Xf);
    if(result != mtOK)
      {
        this->m_errorString = "Error occured creating the new marker!";
        result = -1;
      }
  }
    }
  else
    {
      this->m_errorString = "Error occured creating the new marker!";
      result = -1;
    }
  return result;
}

//------------------------------------------
int MicronTrackerInterface::mtSaveMarkerTemplate(char* templName)
{
  const int currDirSize=255;
  char currDir[currDirSize+1];
  currDir[currDirSize]=0;
  mtUtils::getCurrPath(currDir);
#if(WIN32)
  strcat_s(currDir, currDirSize, "\\Markers\\");
#else
  strcat_s(currDir, currDirSize, "/Markers/");
#endif
  strcat_s(currDir, currDirSize, templName);
  Persistence* newPersistence = new Persistence();
  newPersistence->setPath(currDir);
  int storeResult = this->m_pCurrMarker->storeTemplate(newPersistence, "");//(char*)(name.c_str()) );
  if (storeResult != mtOK)
    {
      this->m_errorString = "Error in saving the current template";
      return -1;
    }
  else
    {
      mtResetSamples();
      return 0;
    }
}

//------------------------------------------
void MicronTrackerInterface::mtResetSamples()
{
  for (unsigned int i=0; i<this->m_sampleVectors.size(); i++)
    {
      delete this->m_sampleVectors[i];
    }
  this->m_sampleVectors.clear();
}

//------------------------------------------
int MicronTrackerInterface::mtGetLoadedTemplatesNum()
{
  return this->m_pMarkers->getTemplateCount();
}

//------------------------------------------
int MicronTrackerInterface::mtGetIdentifiedMarkersCount()
{
  return this->m_numOfIdentifiedMarkers;
}

//------------------------------------------
int MicronTrackerInterface::mtGetUnidentifiedMarkersCount()
{
  return this->m_numOfUnidentifiedMarkers;
}

//------------------------------------------
int MicronTrackerInterface::mtGetNumOfFacetsInMarker(int markerIndex)
{
  return this->m_vNumOfFacetsInEachMarker[markerIndex];
}

//------------------------------------------
int MicronTrackerInterface::mtGetNumOfTotalFacetsInMarker(int markerIndex)
{
  return this->m_vNumOfTotalFacetsInEachMarker[markerIndex];
}

//------------------------------------------
void MicronTrackerInterface::mtFindIdentifiedMarkers()
{
  Collection* markersCollection = new Collection(this->m_pMarkers->identifiedMarkers(this->m_pCurrCam));
  this->m_numOfIdentifiedMarkers = markersCollection->count();
  if (this->m_numOfIdentifiedMarkers == 0)
    {
      this->m_markerStatus = mtUtils::MTI_NO_MARKER_CAPTURED;
      delete markersCollection; 
      return;
    }
  this->m_2dvRotations.clear();
  this->m_2dvTranslations.clear();
  this->m_vIdentifiedMarkersXPoints.clear();
  for (unsigned int i=0;i<this->m_vIdentifiedMarkersName.size();i++)
    {
      this->m_vIdentifiedMarkersName[i].erase();
    }
  
  this->m_vIdentifiedMarkersName.resize(0);
  this->m_vIdentifiedMarkersName.clear();
  this->m_vNumOfFacetsInEachMarker.clear();
  this->m_vNumOfTotalFacetsInEachMarker.clear();
  
  this->m_markerStatus = mtUtils::MTI_MARKER_CAPTURED;
  int markerNum = 1;
  int facetNum = 1;
  
  for (markerNum = 1; markerNum <= markersCollection->count(); markerNum++)
    {
      //this->m_pCurrTempMarker->setHandle(markersCollection->itemI(markerNum));
      Marker* marker = new Marker (markersCollection->itemI(markerNum));
      //Collection* totalFacetsCollection = new Collection(marker->getTemplateFacets());
      Collection* totalFacetsCollection = new Collection(marker->getTemplateFacets());
      this->m_vIdentifiedMarkersName.push_back(marker->getName());
      
      if (marker->wasIdentified(this->m_pCurrCam) != 0)
  {
    Collection* facetsCollection = new Collection(marker->identifiedFacets(this->m_pCurrCam));
    
    // Adjust the colour temperature if we see a CoolCard marker
    if ((0 == strncmp(marker->getName(), "COOL", 4)) ||  
        (0 == strncmp(marker->getName(), "cool", 4)) ||
        (0 == strncmp(marker->getName(), "Cool", 4)) )
      {
        Facet* f = new Facet(facetsCollection->itemI(1));
        Vector* ColorVector = (f->IdentifiedVectors())[0];
        this->m_pCurrCam->AdjustCoolnessFromColorVector(ColorVector->Handle());
        delete f;
      }
    vXPointsTemp.clear();
    for (facetNum = 1; facetNum <= facetsCollection->count(); facetNum++)
      {
        Facet* f = new Facet(facetsCollection->itemI(facetNum));
        // get Xpoints and then draw on each image if enabled  
        f->getXpoints(this->m_pCurrCam, (double *)LS_LR_BH_XY);
        
        // Have to push_back the elements one by one. If pass the address of the whole
        // array, the points will be deleted if the facet is deleted.
              
        // Left, Long vector
        vXPointsTemp.push_back(LS_LR_BH_XY[0][0][0][0]);
        vXPointsTemp.push_back(LS_LR_BH_XY[0][0][0][1]);
        vXPointsTemp.push_back(LS_LR_BH_XY[0][0][1][0]);
        vXPointsTemp.push_back(LS_LR_BH_XY[0][0][1][1]);
                
        // Right, Long vector
        vXPointsTemp.push_back(LS_LR_BH_XY[0][1][0][0]);
        vXPointsTemp.push_back(LS_LR_BH_XY[0][1][0][1]);
        vXPointsTemp.push_back(LS_LR_BH_XY[0][1][1][0]);
        vXPointsTemp.push_back(LS_LR_BH_XY[0][1][1][1]);
        
        // Left, short vector
        vXPointsTemp.push_back(LS_LR_BH_XY[1][0][0][0]);
        vXPointsTemp.push_back(LS_LR_BH_XY[1][0][0][1]);
        vXPointsTemp.push_back(LS_LR_BH_XY[1][0][1][0]);
        vXPointsTemp.push_back(LS_LR_BH_XY[1][0][1][1]);

        // Right, Short vector
        vXPointsTemp.push_back(LS_LR_BH_XY[1][1][0][0]);
        vXPointsTemp.push_back(LS_LR_BH_XY[1][1][0][1]);
        vXPointsTemp.push_back(LS_LR_BH_XY[1][1][1][0]);
        vXPointsTemp.push_back(LS_LR_BH_XY[1][1][1][1]);
        
        delete f;
        
      } // End of the for loop for the facets.
    this->m_vIdentifiedMarkersXPoints.push_back( vXPointsTemp );
    this->m_vNumOfFacetsInEachMarker.push_back(facetsCollection->count());
    this->m_vNumOfTotalFacetsInEachMarker.push_back(totalFacetsCollection->count());
    delete facetsCollection;
    

    /***********************************/
    /*VERY IMPORTANT PROGRAMMING NOTE                                       
      Deleting the totalFacetsCollection here causes the identified facets to be deleted
      and hence not shown as identified on the GUI side. Obviously on the other hand, not deleting this
      object results in memory leak. Should be fixed soon! */
    /***********************************/
    //      delete totalFacetsCollection;
    Xform3D* Marker2CurrCameraXf = marker->marker2CameraXf(this->m_pCurrCam->getHandle());
    // Find the translations and push them in a 2 temporary vector and then push that temp vector into a 
    // 2 dimensional vector.
    std::vector<double> vTransTemp;
    vTransTemp.clear();
    for (int i = 0 ; i < 3; i++)
      {
        vTransTemp.push_back(Marker2CurrCameraXf->getShift(i));
      }
    vTransTemp.push_back(1);
    this->m_2dvTranslations.push_back(vTransTemp);
    // Find the rotations and push them in a 2 temporary vector and then push that temp vector into a 
    // 2 dimensional vector.
    
    double vR[3][3];
    
    std::vector<double> vRotTemp;
    // problem lies here !
    Xform3D_RotMatGet(Marker2CurrCameraXf->getHandle(), reinterpret_cast<double *>(vR[0]));
   //Marker2CurrCameraXf->getRotationMatrix(reinterpret_cast<double *>(vR[0]));
    for (int j = 0; j < 3; j++)
      {
    for (int k = 0; k < 3; k++)
      { 
      vRotTemp.push_back( vR[j][k] );
        }
      }
    
    this->m_2dvRotations.push_back(vRotTemp);/**/
    delete Marker2CurrCameraXf;
    
    
  } // End of if (m_pCurrTempMarker->wasIdentified...)
      

    } // End of the for loop for the markers.
  delete markersCollection;
}





//------------------------------------------


void MicronTrackerInterface::mtFindUnidentifiedMarkers()


{
  Collection* unidentifiedVectorsColl = new Collection(this->m_pMarkers->unidentifiedVectors(this->m_pCurrCam));
  // Programming note>
  this->m_numOfUnidentifiedMarkers = unidentifiedVectorsColl->count();
  
  /***********************************
    VERY IMPORTANT PROGRAMMING NOTE                                       
    This checking is temporary as it seems a bug in MT libraries is reporting the number of unidentified markers to a weird number!
  ************************************/
  if (this->m_numOfUnidentifiedMarkers > 4 )
    {
      this->m_numOfUnidentifiedMarkers = 4;
    }
  if ( m_numOfUnidentifiedMarkers == 0) // No unidentified vector
    {
      delete unidentifiedVectorsColl;
      return;
    }
  this->m_vUnidentifiedMarkersEndPoints.clear();
  for (int i=1; i<= m_numOfUnidentifiedMarkers; i++)
    {
      this->vUnidentifiedEndPointsTemp.clear();
      Vector* v = new Vector(unidentifiedVectorsColl->itemI(i));
      v->getEndXPoints((double *)LR_BH_XY);
      // Left  
      this->vUnidentifiedEndPointsTemp.push_back(LR_BH_XY[0][0][0]);
      this->vUnidentifiedEndPointsTemp.push_back(LR_BH_XY[0][0][1]);
      this->vUnidentifiedEndPointsTemp.push_back(LR_BH_XY[0][1][0]);
      this->vUnidentifiedEndPointsTemp.push_back(LR_BH_XY[0][1][1]);

      this->vUnidentifiedEndPointsTemp.push_back(LR_BH_XY[1][0][0]);
      this->vUnidentifiedEndPointsTemp.push_back(LR_BH_XY[1][0][1]);
      this->vUnidentifiedEndPointsTemp.push_back(LR_BH_XY[1][1][0]);
      this->vUnidentifiedEndPointsTemp.push_back(LR_BH_XY[1][1][1]);

      // Right
      this->m_vUnidentifiedMarkersEndPoints.push_back(this->vUnidentifiedEndPointsTemp);
      delete v;
    }
  return ;
  delete unidentifiedVectorsColl;
}

//------------------------------------------
void MicronTrackerInterface::mtGetTranslations(std::vector<double> &vTranslations, int markerIndex)
{  
  vTranslations = this->m_2dvTranslations[markerIndex];  
}

//------------------------------------------
void MicronTrackerInterface::mtGetRotations(std::vector<double> &vRotations, int markerIndex)
{
  vRotations = this->m_2dvRotations[markerIndex];
}

//------------------------------------------
int MicronTrackerInterface::mtGetStatus()
{
  return this->m_markerStatus;
}

//------------------------------------------
int MicronTrackerInterface::mtGetMarkerStatus(int loadedMarkerIndex, int* identifiedMarkerIndex)
{
  // Safety check. If the request marke index is greater than the identified markers,
  // return NO_MARKER_CAPTURED
  if (loadedMarkerIndex > this->mtGetLoadedTemplatesNum() || loadedMarkerIndex < 0)
  {
    return mtUtils::MTI_NO_MARKER_CAPTURED;
  }
  char* markerName = this->mtGetTemplateName(loadedMarkerIndex);
  for (int i=0; i< this->m_numOfIdentifiedMarkers; i++)
  {
    if (markerName == this->m_vIdentifiedMarkersName[i])
    {
      *identifiedMarkerIndex = i;
      return mtUtils::MTI_MARKER_CAPTURED;
    }
  }
  return mtUtils::MTI_NO_MARKER_CAPTURED;
}

//------------------------------------------
int MicronTrackerInterface::mtSelectCamera(int index)
{
  if (!this->checkCamIndex(index))
    {
      this->m_pCurrCam = this->m_pCameras->getCamera(index);
    }
  if (this->m_pCurrCam != NULL)
    {
      this->m_currCamIndex = index;
      return 0;
    }
  else
    {
      return -1;
    }
}

//------------------------------------------
int MicronTrackerInterface::mtGetCurrCamIndex()
{
  return this->m_currCamIndex;
}

//------------------------------------------
int MicronTrackerInterface::mtSetShutterPreference(double n)
{
  return 1;// this->m_pCameras->setShutterPreference(n);
}

//------------------------------------------
double MicronTrackerInterface::mtGetShutterPreference()
{
  //return this->m_pCameras->getShutterPreference(); The MTC.h 
  return 0;
}

//------------------------------------------
int MicronTrackerInterface::mtSetTemplMatchTolerance(double matchTolerance)
{
  this->m_pMarkers->setTemplateMatchToleranceMM(matchTolerance);
  return 1;
}

//------------------------------------------
double MicronTrackerInterface::mtGetTemplMatchTolerance()
{
  return this->m_pMarkers->getTemplateMatchToleranceMM();
}

//------------------------------------------
double MicronTrackerInterface::mtGetTemplMatchToleranceDefault()
{
  double defToleranceVal = 0;
  Markers_TemplateMatchToleranceMMDefaultGet(&defToleranceVal);
  return defToleranceVal;
  //return this->m_pMarkers->getDefaultTemplateMatchToleranceMM();
}

//------------------------------------------
int MicronTrackerInterface::mtSetPredictiveFramesInterleave(int predictiveInterleave)
{
  this->m_pMarkers->setPredictiveFramesInterleave(predictiveInterleave);
  return 1;
}

//------------------------------------------
int MicronTrackerInterface::mtGetPredictiveFramesInterleave()
{
  return this->m_pMarkers->getPredictiveFramesInterleave();
}

//------------------------------------------
void MicronTrackerInterface::mtSetPredictiveTracking(short predTracking)
{
  //m_pMarkerTempls->setPredictiveTracking(predTracking);
}

//------------------------------------------
long MicronTrackerInterface::mtGetPredictiveTracking()
{
  return 0;//m_pMarkerTempls->getPredictiveTracking();
}

//------------------------------------------
void MicronTrackerInterface::mtSetAdjustCamAfterEveryProcess(short autoCamExp)
{
  //m_pMarkerTempls->setAdjustCamAfterEveryProcess(autoCamExp);
}

//------------------------------------------
short MicronTrackerInterface::mtGetAdjustCamAfterEveryProcess()
{
  return 0;//m_pMarkerTempls->getAdjustCamAfterEveryProcess();
}

//------------------------------------------
int MicronTrackerInterface::mtSetShutterTime(double n, int index)
{
  if (index == -1)
    {
      return this->m_pCurrCam->setShutterTime(n);
    }
  else if(!this->checkCamIndex(index))
    {
      return -1;
    }

  else
    {
      return this->m_pCameras->getCamera(index)->setShutterTime(n);
    }
}

//------------------------------------------
double MicronTrackerInterface::mtGetShutterTime(int index)
{
  if (index == -1)
    {
      return this->m_pCurrCam->getShutterTime();
    }
  else if(!this->checkCamIndex(index))
    {
      return -1;
    }
  else
    {
      return this->m_pCameras->getCamera(index)->getShutterTime();
    }
}

//------------------------------------------
double MicronTrackerInterface::mtGetMinShutterTime(int index)
{
  if (index == -1)
    {
      return this->m_pCurrCam->getMinShutterTime();
    }
  else if(!this->checkCamIndex(index))
    {
      return -1;
    }
  else
    {
      return this->m_pCameras->getCamera(index)->getMinShutterTime();
    }
}

//------------------------------------------
double MicronTrackerInterface::mtGetMaxShutterTime(int index)
{
  if (index == -1)
    {
      return this->m_pCurrCam->getMaxShutterTime();
    }
  else if(!this->checkCamIndex(index))
    {
      return -1.0;
    }

  else
    {
      return this->m_pCameras->getCamera(index)->getMaxShutterTime();
    }
}

//------------------------------------------
double MicronTrackerInterface::mtGetGain(int index)
{
  if (index == -1)
    {
      return this->m_pCurrCam->getGain();
    }
  else if(!this->checkCamIndex(index))
    {
      return -1;
    }
  else
    {
      return this->m_pCameras->getCamera(index)->getGain();
    }
}

//------------------------------------------
int MicronTrackerInterface::mtSetGain(double n, int index)
{
  if (index == -1)
    {
      return this->m_pCurrCam->setGain(n);
    }
  else if(!this->checkCamIndex(index))
    {
      return -1;
    }
  else
    {
      return this->m_pCameras->getCamera(index)->setGain(n);
    }
}

//------------------------------------------
double MicronTrackerInterface::mtGetMinGain(int index)
{
  if (index == -1)
    {
      return this->m_pCurrCam->getMinGain();
    }
  else if(!this->checkCamIndex(index))
    {
      return -1;
    }
  else
    {
      return this->m_pCameras->getCamera(index)->getMinGain();
    }
}

//------------------------------------------
double MicronTrackerInterface::mtGetMaxGain(int index)
{
  if (index == -1)
    {
      return this->m_pCurrCam->getMaxGain();
    }
  else if(!this->checkCamIndex(index))
    {
      return -1;
    }
  else
    {
      return this->m_pCameras->getCamera(index)->getMaxGain();
    }
}

//------------------------------------------
double MicronTrackerInterface::mtGetDBGain(int index)
{
  if (index == -1)
    {
      return this->m_pCurrCam->getDBGain();
    }
  else if(!this->checkCamIndex(index))
    {
      return -1;
    }
  else
    {
      return this->m_pCameras->getCamera(index)->getDBGain();
    }
}

//------------------------------------------
double MicronTrackerInterface::mtGetExposure(int index)
{
  if (index == -1)
    {
      return this->m_pCurrCam->getExposure();
    }
  else if(!this->checkCamIndex(index))
    {
      return -1;
    }
  else
    {
      return this->m_pCameras->getCamera(index)->getExposure();
    }
}

//------------------------------------------
int MicronTrackerInterface::mtSetExposure(double n, int index)
{
  if (index == -1)
    {
      return this->m_pCurrCam->setExposure(n);
    }
  else if(!this->checkCamIndex(index))
    {
      return -1;
    }
  else
    {
      return this->m_pCameras->getCamera(index)->setExposure(n);
    }
}

//------------------------------------------
double MicronTrackerInterface::mtGetMinExposure(int index)
{
  if (index == -1)
    {
      return this->m_pCurrCam->getMinExposure();
    }
  else if(!this->checkCamIndex(index))
    {
      return -1;
    }
  else
    {
      return this->m_pCameras->getCamera(index)->getMinExposure();
    }
}

//------------------------------------------
double MicronTrackerInterface::mtGetMaxExposure(int index)
{
  if (index == -1)
    {
      return this->m_pCurrCam->getMaxExposure();
    }
  else if(!this->checkCamIndex(index))
    {
      return -1.0;
    }
  else
    {
      return this->m_pCameras->getCamera(index)->getMaxExposure();
    }
}

//------------------------------------------
double MicronTrackerInterface::mtGetLightCoolness(int index)
{
  if (index == -1)
    {
      return this->m_pCurrCam->getLightCoolness();
    }
  else if(!this->checkCamIndex(index))
    {
      return -1;
    }
  else
    {
      return this->m_pCameras->getCamera(index)->getLightCoolness();
    }
}

//------------------------------------------
int MicronTrackerInterface::mtSetCamAutoExposure(int n, int index)
{
  if (index == -1)
    {
      return this->m_pCurrCam->setAutoExposure(n);
    }
  else if(!this->checkCamIndex(index))
    {
      return -1;
    }
  else
    {
      return this->m_pCameras->getCamera(index)->setAutoExposure(n);
    }
}

//------------------------------------------
int MicronTrackerInterface::mtGetCamAutoExposure(int index)
{
  if (index == -1)
    {
      return this->m_pCurrCam->getAutoExposure();
    }
  else
    {
      return this->m_pCameras->getCamera(index)->getAutoExposure();
    }
}

//------------------------------------------
double MicronTrackerInterface::mtGetLatestFrameTime(int index)
{
  if (index == -1)
    {
      return this->m_pCurrCam->getFrameTime();
    }
  else if(!this->checkCamIndex(index))
    {
      return -1;
    }
  else
    {
      return this->m_pCameras->getCamera(index)->getFrameTime();
    }
}

//------------------------------------------
int MicronTrackerInterface::mtGetNumOfFramesGrabbed(int index)
{
  if (index == -1)
    {
      return this->m_pCurrCam->getNumOfFramesGrabbed();
    }
  else if (!this->checkCamIndex(index))
    {
      return -1;
    }
  else
    {
      return this->m_pCameras->getCamera(index)->getNumOfFramesGrabbed();
    }
}

//------------------------------------------
int MicronTrackerInterface::mtGetBitsPerPixel(int index)
{
  if (index == -1)
    {
      return this->m_pCurrCam->getBitsPerPixel();
    }
  else if (!this->checkCamIndex(index))
    {
      return -1;
    }
  else
    {
      return this->m_pCameras->getCamera(index)->getBitsPerPixel();
    }
  return 0;
}

//------------------------------------------
int MicronTrackerInterface::mtGetLatestFramePixHistogram(long* &aPixHist, int subSampleRate, int index )
{
  return  0;//m_pCurrCams->getLatestFramePixHistogram(aPixHist, subSampleRate, index);
}

//------------------------------------------
mtMeasurementHazardCode MicronTrackerInterface::mtGetLatestFrameHazard()
{
  return Camera_LastFrameThermalHazard(this->m_pCurrCam->getHandle());
  // return this->m_pCurrCam->getHazardCode();
}

//------------------------------------------
int MicronTrackerInterface::mtSetTemplateName(int index, char* templName)
{
  int result = -1;
  // Check the index
  if (index < 0 || index > this->m_pMarkers->getTemplateCount() )
    {
      this->m_errorString = "Marker index out of range!";
    }
  char* oldN = this->mtGetTemplateName(index);
  result = this->m_pMarkers->setTemplateItemName(index, templName);
  result = this->m_pMarkers->storeTemplate(index, this->m_pPers->getHandle(), NULL );
  result = this->renameFile(oldN, templName, "Markers");
  if (result != mtOK)
    {
      this->m_errorString = "Changing the name of the template was not successful.";
    }
  return result;
}

//------------------------------------------
char* MicronTrackerInterface::mtGetTemplateName(int index)
{
  std::string s;
  
  return this->m_pMarkers->getTemplateItemName(index);
}

//------------------------------------------
char* MicronTrackerInterface::mtGetIdentifiedTemplateName(int index)
{
  // Check the index
  if (index > -1 && index < this->m_numOfIdentifiedMarkers)
    {
      return (char*)this->m_vIdentifiedMarkersName[index].c_str();
    }
  else
    {
      return "";
    }
}

//------------------------------------------
int MicronTrackerInterface::mtDeleteTemplate(int index)
{  
  int result = -1;
  if (index > -1 && index < this->m_pMarkers->getTemplateCount() )
    {
      char* templateName = this->mtGetTemplateName(index);//this->m_pMarkers->getTemplateItemName(index);
      result = this->removeFile(templateName, "Markers");
      if (result != 0)
  {
    this->m_errorString = "Could not delete the template!";
  }
    }
  else
    {
      this->m_errorString = "Marker index out of range!";
    }
  return result;
}

//------------------------------------------
int MicronTrackerInterface::mtGetLeftRightImageArray(unsigned char** &leftImageArray, unsigned char** &rightImageArray, int index)
{
  bool result = 0;
  if (index == -1)
    {
      result = this->m_pCurrCam->getImages(&leftImageArray, &rightImageArray);
    }
  else if (!this->checkCamIndex(index))
    {
      result = false;
    }
  else
    {
      result = this->m_pCameras->getCamera(index)->getImages(&leftImageArray, &rightImageArray);
    }
    
  return result ? 0 : -1;
}

//------------------------------------------
int MicronTrackerInterface::mtGetLeftRightImageArrayHalfSize(unsigned char** &leftImageArray, 
                   unsigned char** &rightImageArray, 
                   int xResolution, int yResolution, int index)
{
  bool result = 0;
  if (index == -1)
    {
      result = this->m_pCurrCam->getHalfSizeImages(&leftImageArray, &rightImageArray, xResolution, yResolution);
    }
  else if (!this->checkCamIndex(index))
    {
      result = false;
    }
  else
    {
      result = this->m_pCameras->getCamera(index)->getHalfSizeImages(&rightImageArray, &leftImageArray, xResolution, yResolution);
    }
  return result ? 0 : -1;
}

//------------------------------------------
void MicronTrackerInterface::mtGetIdentifiedMarkersXPoints(double* &xPoints, int markerIndex)
{
  //Check the marker index
  if (markerIndex >= this->m_numOfIdentifiedMarkers-1)
    {
      markerIndex = this->m_numOfIdentifiedMarkers -1;
    }
  if(markerIndex < 0)
    {
      markerIndex = 0;
    }
  xPoints = &this->m_vIdentifiedMarkersXPoints[markerIndex][0];
}

//------------------------------------------
void MicronTrackerInterface::mtGetUnidentifiedMarkersEnds(double* &endPoints, int vectorIndex)
{  
  //Check the vector index
  if (vectorIndex >= this->m_numOfUnidentifiedMarkers-1)
    {
      vectorIndex = this->m_numOfUnidentifiedMarkers-1;
    }
  if (vectorIndex < 0)
    {
      vectorIndex = 0;
    }
  endPoints = &this->m_vUnidentifiedMarkersEndPoints[vectorIndex][0];
}

//------------------------------------------
int MicronTrackerInterface::removeFile(const std::string& fileName, char* dir)
{
  const int currentFolderPathSize=255;
  char currentFolderPath[currentFolderPathSize+1];
  currentFolderPath[currentFolderPathSize]=0;
  mtUtils::getCurrPath(currentFolderPath);
  if ( dir != NULL )
    {
#if(WIN32)
      {
  strcat_s(currentFolderPath, currentFolderPathSize, "\\");
  strcat_s(currentFolderPath, currentFolderPathSize, dir);
  strcat_s(currentFolderPath, currentFolderPathSize, "\\");
      }
#else
      {
  strcat_s(currentFolderPath, currentFolderPathSize, "/");
  strcat_s(currentFolderPath, currentFolderPathSize, dir);
  strcat_s(currentFolderPath, currentFolderPathSize, "/");
      }
#endif
    }
  if (_chdir(currentFolderPath) == 0)
    {
      int result = remove(fileName.c_str());
      // Return to the parent directory
      _chdir("..");
      if (result == 0)
  {
    return 0; // successful
  }
      else // not successful
  {
    return -1;
  }
    }
  return -1;
}

//---------------------------------------------
int MicronTrackerInterface::renameFile(const std::string& oldName, const std::string& newName, char* dir)
{
  std::cout << "old name is " << oldName << " new name " << newName << std::endl;
  const int currentFolderPathSize=255;
  char currentFolderPath[currentFolderPathSize+1];
  currentFolderPath[currentFolderPathSize]=0;
  mtUtils::getCurrPath(currentFolderPath);
  if ( dir != NULL )
    {
#if(WIN32)
      {
  strcat_s(currentFolderPath, currentFolderPathSize, "\\");
  strcat_s(currentFolderPath, currentFolderPathSize, dir);
  strcat_s(currentFolderPath, currentFolderPathSize, "\\");
      }
#else
      {
  strcat_s(currentFolderPath, currentFolderPathSize, "/");
  strcat_s(currentFolderPath, currentFolderPathSize, dir);
  strcat_s(currentFolderPath, currentFolderPathSize, "/");
      }
#endif
    }
  if (_chdir(currentFolderPath) == 0 )
    {
      int result = rename(oldName.c_str(), newName.c_str());
      
      // Return to the parent directory
      _chdir("..");
      if (result == 0 )
  {
    return 0; // successful
  }
      else // not successful
  {
    return -1;
  }

    }
  return -1;
}
//------------------------------------------
std::string MicronTrackerInterface::handleErrors(int errorNum)
{
  /*mtOK=0,
    mtError,
    mtInvalidHandle,
    mtNullPointer,
    mtOutOfMemory,
    mtStringParamTooLong,
    mtPathNotSet,
    mtWriteToFileFailed,
    mtInvalidIndex,
    mtInvalidSideI,
    mtInvalidDivisor,
    mtEmptyCollection,
    mtInsufficientSamples,
    mtOddNumberOfSamples,
    mtLessThan2Vectors,
    mtMoreThanMaxVectorsPerFacet,
    mtErrorExceedsTolerance,
    mtInsufficientAngleBetweenVectors,
    mtFirstVectorShorterThanSecond,
    mtVectorLengthsTooSimilar,
    mtNullTemplateVector,
    mtDifferentFacetsGeometryTooSimilar,
    mtNoncompliantFacetDefinition,
    mtCollectionContainsNonVectorHandles,
    mtParameterOutOfRange,
    mtEmptyBuffer,
    mtPropertyNotSet,
    mtDimensionsDoNotMatch,
    mtOpenFileFailed,
    mtReadFileFailed,
    mtNotACalibrationFile,
    mtIncorrectFileVersion,
    mtCalibrationFileIncomplete,
    mtCameraInitializeFailed,
    mtGrabFrameError*/
  std::string error = "";
  switch (errorNum)
    {
    case 1:
      error = "Unknown error!";
      break;
    case 2:
      error = "Invalid handle!";
      break;
    case 3:
      error = "Null pointer!";
      break;
    case 4:
      error = "Out of memory";
      break;
    case 5:
      error = "String parameter is too long!";
      break;
    default:
      error = "Unknown error";
      break;
    }
  return error;
}

