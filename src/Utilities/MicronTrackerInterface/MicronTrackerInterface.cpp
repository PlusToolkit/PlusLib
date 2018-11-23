//----------------------------------------------------------------------------
/* MicronTracker Windows API */
/* Author: SI- Robarts Research Institute */
/* Date: June 9, 2003 */
/* Modified: June 20, 2003 This file has been converted to a class to make it more compatible with OOD */
/****************************/

#include "Cameras.h"
#include "Collection.h"
#include "Facet.h"
#include "MCamera.h"
#include "Marker.h"
#include "Markers.h"
#include "MicronTrackerInterface.h"
#include "MicronTrackerLoggerMacros.h"
#include "Persistence.h"
#include "Vector.h"
#include "Xform3D.h"

#include <MTC.h>

/*
#if (MTCMajorVersion == 3) && (MTCMinorVersion < 7)
  // MTVideo.h is provided before MTC_3.7, use that
  #include "MTVideo.h"
#else
  // MTVideo.h is not included anymore in the dist directory of MicronTracker SDK 3.7.x
  // therefore use a private copy (MTVideo_private.h)
  // When we drop support of MTC_3.6 and older we can probably drop this MTVideo.h header file, too.
  //#include "MTVideo_private.h"
#endif
*/

#ifdef _WIN32
  #include <direct.h>
  #include <io.h>
#endif

#include <sstream>

//----------------------------------------------------------------------------
/** Destructor */
MicronTrackerInterface::~MicronTrackerInterface()
{
}

//----------------------------------------------------------------------------
MicronTracker_Return MicronTrackerInterface::mtInit(const std::string& iniFilePath)
{
  this->m_errorString.clear();
  this->m_isCameraAttached = false;
  this->m_pMarkers = new Markers();
  this->m_pPers = new Persistence();
  //m_pCurrTempMarker =  new Marker();
  this->m_pTempMarkerForAddingFacet = NULL; //new Marker();

  initialINIAccess(iniFilePath);

  return MT_OK;
}

//----------------------------------------------------------------------------
void MicronTrackerInterface::mtEnd()
{
  this->mtDetachCameras();
}

//----------------------------------------------------------------------------
void MicronTrackerInterface::initialINIAccess(const std::string& iniFilePath)
{
  this->m_pPers->setPath(iniFilePath.c_str());
  this->m_pPers->setSection("General");

  //Setting the FrameInterleave property in the Markers object
  int defaultFrameInterleave = 0;
  this->m_pMarkers->setPredictiveFramesInterleave(this->m_pPers->retrieveInt("PredictiveFramesInterleave", defaultFrameInterleave));

  //Setting the TemplateMatchToleranceMM property in the Markers object
  double defaultTempMatchToleranceMM = 1.0;
  this->m_pMarkers->setTemplateMatchToleranceMM(this->m_pPers->retrieveDouble("TemplateMatchToleranceMM", defaultTempMatchToleranceMM));
}

//----------------------------------------------------------------------------
void MicronTrackerInterface::mtSaveSettingsToINI()
{
  this->m_pPers->saveDouble("TemplateMatchToleranceMM", this->m_pMarkers->getTemplateMatchToleranceMM());
  this->m_pPers->saveInt("PredictiveFramesInterleave", this->m_pMarkers->getPredictiveFramesInterleave());
}

//----------------------------------------------------------------------------
std::string MicronTrackerInterface::ConvertReturnToString(MicronTracker_Return returnValue)
{
  switch (returnValue)
  {
    case MT_OK:
      return "OK";
    case MT_InvalidHandle:
      return "InvalidHandle";
    case MT_ReentrantAccess:
      return "ReentrantAccess";
    case MT_InternalMTError:
      return "InternalMTError";
    case MT_NullPointer:
      return "NullPointer";
    case MT_OutOfMemory:
      return "OutOfMemory";
    case MT_ParameterOutOfRange:
      return "ParameterOutOfRange";
    case MT_StringParamTooLong:
      return "StringParamTooLong";
    case MT_OutBufferTooSmall:
      return "OutBufferTooSmall";
    case MT_CameraNotInitialized:
      return "CameraNotInitialized";
    case MT_CameraAlreadyInitialized:
      return "CameraAlreadyInitialized";
    case MT_CameraInitializationFailed:
      return "CameraInitializationFailed";
    case MT_CompatibilityError:
      return "CompatibilityError";
    case MT_DataNotAvailable:
      return "DataNotAvailable";
    case MT_UnrecognizedCameraModel:
      return "UnrecognizedCameraModel";
    case MT_PathNotSet:
      return "PathNotSet";
    case MT_CannotAccessDirectory:
      return "CannotAccessDirectory";
    case MT_WriteToFileFailed:
      return "WriteToFileFailed";
    case MT_InvalidIndex:
      return "InvalidIndex";
    case MT_InvalidSideI:
      return "InvalidSideI";
    case MT_InvalidDivisor:
      return "InvalidDivisor";
    case MT_EmptyCollection:
      return "EmptyCollection";
    case MT_InsufficientSamples:
      return "InsufficientSamples";
    case MT_InsufficientSamplesWithinTolerance:
      return "InsufficientSamplesWithinTolerance";
    case MT_OddNumberOfSamples:
      return "OddNumberOfSamples";
    case MT_LessThan2Vectors:
      return "LessThan2Vectors";
    case MT_MoreThanMaxVectorsPerFacet:
      return "MoreThanMaxVectorsPerFacet";
    case MT_ErrorExceedsTolerance:
      return "ErrorExceedsTolerance";
    case MT_InsufficientAngleBetweenVectors:
      return "InsufficientAngleBetweenVectors";
    case MT_FirstVectorShorterThanSecond:
      return "FirstVectorShorterThanSecond";
    case MT_VectorLengthsTooSimilar:
      return "VectorLengthsTooSimilar";
    case MT_NullTemplateVector:
      return "NullTemplateVector";
    case MT_TemplateNotSet:
      return "TemplateNotSet";
    case MT_CorruptTemplateFile:
      return "CorruptTemplateFile";
    case MT_MaxMarkerTemplatesExceeded:
      return "MaxMarkerTemplatesExceeded";
    case MT_DifferentFacetsGeometryTooSimilar:
      return "DifferentFacetsGeometryTooSimilar";
    case MT_NoncompliantFacetDefinition:
      return "NoncompliantFacetDefinition";
    case MT_CollectionContainsNonVectorHandles:
      return "CollectionContainsNonVectorHandles";
    case MT_EmptyBuffer:
      return "EmptyBuffer";
    case MT_DimensionsDoNotMatch:
      return "DimensionsDoNotMatch";
    case MT_OpenFileFailed:
      return "OpenFileFailed";
    case MT_ReadFileFailed:
      return "ReadFileFailed";
    case MT_WriteFileFailed:
      return "WriteFileFailed";
    case MT_CannotOpenCalibrationFile:
      return "CannotOpenCalibrationFile";
    case MT_NotACalibrationFile:
      return "NotACalibrationFile";
    case MT_CalibrationFileCorrupt:
      return "CalibrationFileCorrupt";
    case MT_CalibrationFileDoesNotMatchCamera:
      return "CalibrationFileDoesNotMatchCamera";
    case MT_CalibrationFileNotLoaded:
      return "CalibrationFileNotLoaded";
    case MT_IncorrectFileVersion:
      return "IncorrectFileVersion";
    case MT_LocationOutOfMeasurementBounds:
      return "LocationOutOfMeasurementBounds";
    case MT_CannotTriangulate:
      return "CannotTriangulate";
    case MT_UnknownXform:
      return "UnknownXform";
    case MT_CameraNotFound:
      return "CameraNotFound";
    case MT_FeatureDataUnavailable:
      return "FeatureDataUnavailable";
    case MT_FeatureDataCorrupt:
      return "FeatureDataCorrupt";
    case MT_XYZOutOfFOV:
      return "XYZOutOfFOV";
    case MT_GrabFrameError:
      return "GrabFrameError";
    case MT_GrabTimeOut:
      return "GrabTimeOut";
    case MT_CannotCreateThread:
      return "CannotCreateThread";
    case MT_HdrIsNotEnabled:
      return "HdrIsNotEnabled";
    case MT_FeatureNotSupported:
      return "FeatureNotSupported";
    case MT_HDRFrameCycleNotSupported:
      return "HDRFrameCycleNotSupported";
    case MT_FeatureNotSupportedForX64:
      return "FeatureNotSupportedForX64";
    case MT_NonUniformLightOnCoolCard:
      return "NonUniformLightOnCoolCard";
    case MT_BackgroundProcessMutexError:
      return "BackgroundProcessMutexError";
    case MT_TooManyVectors:
      return "TooManyVectors";
    case MT_MethodIsObsolete:
      return "MethodIsObsolete";
    case MT_InvalidVectors:
      return "InvalidVectors";
    case MT_MismatchedVectors:
      return "MismatchedVectors";
    case MT_InsufficientNumberOfXPoints:
      return "InsufficientNumberOfXPoints";
    case MT_XPointsProcessingIsDisabled:
      return "XPointsProcessingIsDisabled";
    case MT_RemoveFailed:
      return "RemoveFailed";
    case MT_RenameFailed:
      return "RenameFailed";
    case MT_NoRegistryKey:
      return "NoRegistryKey";
    case MT_NoEnvironmentVariable:
      return "NoEnvironmentVariable";
  }

  return "Unknown";
}

//----------------------------------------------------------------------------
MicronTracker_Return MicronTrackerInterface::mtSetupCameras()
{
  this->m_pCameras = new Cameras();
  int result = m_pCameras->attachAvailableCameras();

  if (result == mtOK &&  m_pCameras->getCount() >= 1)
  {
    // Set the current camera to the first in the list.
    this->m_currCamIndex = 0;
    this->m_pCurrCam = this->m_pCameras->getCamera(this->m_currCamIndex);
    this->m_isCameraAttached = true;

    return MT_OK;
  }
  else
  {
    this->logError(result);
    return MT_CameraNotFound;
  }
}

//----------------------------------------------------------------------------
void MicronTrackerInterface::mtDetachCameras()
{
  if (m_isCameraAttached)
  {
    m_isCameraAttached = false;
    m_pCameras->detach();
  }
}

//----------------------------------------------------------------------------
// Checks to see if the passed argument is within the camera index, i.e. it is not
//    less than 0 or more than the number of attached camera (minus 1, of course).

MicronTracker_Return MicronTrackerInterface::checkCamIndex(int id)
{
  if (id > -1 && id < this->m_pCameras->getCount())
  {
    return MT_OK;
  }
  else
  {
    logError(-1, "Camera index out of bound");
    return MT_CameraNotFound;
  }
}

//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetNumOfCameras()
{
  return this->m_pCameras->getCount();
}
//----------------------------------------------------------------------------
MicronTracker_Return MicronTrackerInterface::mtRefreshTemplates(std::vector<std::string>& tmplsName,
    std::vector<std::string>& tmplsError,
    const std::string& tmplsPath)
{
  getFileNamesFromDirectory(tmplsName, tmplsPath, true);
  int status = this->m_pMarkers->clearTemplates();
  if (status != mtOK)
  {
    logError(status, "Clear templates failed");
    return (MicronTracker_Return)status;
  }
  LOG_DEBUG("Cleared templates");
  status = Markers_LoadTemplates((char*)tmplsPath.c_str()); // need casting to non-const char because of the improper MTC interface definition
  if (status != mtOK)
  {
    std::string msg = "Load templates failed from directory: " + tmplsPath;
    logError(status, msg.c_str());
    return (MicronTracker_Return)status;
  }
  return MT_OK;
}
//----------------------------------------------------------------------------
MicronTracker_Return MicronTrackerInterface::mtGrabFrame(int index)
{
  int result = mtOK;
  if (index == -1) // From all the cameras
  {
    result = this->m_pCameras->grabFrame();
  }
  else if (!this->checkCamIndex(index)) // Camera index out of range
  {
    result = false;
  }
  else // From one camera
  {
    result = this->m_pCameras->grabFrame(this->m_pCameras->getCamera(index));
  }

  return (MicronTracker_Return)result;
}

//----------------------------------------------------------------------------
MicronTracker_Return MicronTrackerInterface::mtProcessFrame()
{
  int result = this->m_pMarkers->processFrame(this->m_pCurrCam);
  if (result != mtOK)
  {
    this->logError(result);
  }
  return (MicronTracker_Return)result;
}

//----------------------------------------------------------------------------
MicronTrackerInterface::NewSampleReturnValue MicronTrackerInterface::mtCollectNewSamples(int collectingAdditionalFacet)
{
  NewSampleReturnValue result = NewSample_2Vectors;
  Collection* col = new Collection(this->m_pMarkers->unidentifiedVectors(this->m_pCurrCam));
  this->m_isAddingAdditionalFacet = collectingAdditionalFacet;
  if (col->count() == 2 || collectingAdditionalFacet == 1)
  {
    this->m_sampleVectors.push_back(col);
    result = NewSample_2Vectors;

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
        this->m_facet1ToCameraXfs.push_back(this->m_pTempMarkerForAddingFacet->marker2CameraXf(this->m_pCurrCam->getHandle()));

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
        this->m_sampleVectors.pop_back();
        delete identifiedFacetsCol;
        return NewSample_NoKnownFacetBeingDetected;
      }
      delete identifiedFacetsCol;
      //delete markersCollection;
    }
  }
  else if (col->count() < 2)
  {
    result = NewSample_LessThan2Vectors;
  }
  else if (col->count() > 2)
  {
    result = NewSample_MoreThan2Vectors;
  }

  //  delete col;
  return result;
}

//----------------------------------------------------------------------------
MicronTracker_Return MicronTrackerInterface::mtStopSampling(char* templateName, double jitterValue)
{
  std::string errorsString;
  Facet* f = new Facet();
  std::vector<Vector*> vectorPair;
  int result = mtOK;
  if (f->setVectorsFromSample(this->m_sampleVectors, errorsString))
  {
    if (this->m_isAddingAdditionalFacet)
    {
      Collection* markersCollection = new Collection(this->m_pMarkers->identifiedMarkers(this->m_pCurrCam));
      //this->m_pTempMarkerForAddingFacet->setHandle(markersCollection->itemI(1));
      Collection* identifiedFacetsCol = new Collection(this->m_pTempMarkerForAddingFacet->getTemplateFacets());
      std::vector<Xform3D*> facet1ToNewFacetXfs;
      Xform3D* facet1ToNewFacetXf;// = new Xform3D();

      for (unsigned int i = 0; i < this->m_sampleVectors.size(); i++)
      {
        vectorPair.clear();
        vectorPair.push_back(new Vector(this->m_sampleVectors[i]->itemI(0)));
        vectorPair.push_back(new Vector(this->m_sampleVectors[i]->itemI(1)));
        //Programming Note: The following line is crashing in the Facet.cpp when
        //it makes a call to the DLL. As a result no registration of multi-facet markers can be made at this point. June 7, 2004
        if (f->identify(this->m_pCurrCam, vectorPair, 1)) // Then the sample matches the template
        {
          // Compute the xform between the first marker facet and the new one
          facet1ToNewFacetXf = this->m_facet1ToCameraXfs[i]->concatenate(f->getFacet2CameraXf(this->m_pCurrCam)->inverse());
          facet1ToNewFacetXfs.push_back(facet1ToNewFacetXf);
        }

      }
      // Combine the transforms accumulated to a new one and save it with the facet in the marker
      facet1ToNewFacetXf = facet1ToNewFacetXfs[1];
      for (unsigned int i = 2; i < facet1ToNewFacetXfs.size(); i++)
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
      if (result != mtOK)
      {
        std::string msg = "Error occurred creating the new marker (" + errorsString + ")";
        logError(result, msg.c_str());
      }
    }
  }
  else
  {
    std::string msg = "Error occurred creating the new marker (" + errorsString + ")";
    logError(-1, msg.c_str());
    result = false;
  }
  return (MicronTracker_Return)result;
}

//----------------------------------------------------------------------------
MicronTracker_Return MicronTrackerInterface::mtSaveMarkerTemplate(const std::string& templName, const std::string& dir)
{
  std::string templFullPath = dir + "/" + templName;
  Persistence* newPersistence = new Persistence();
  newPersistence->setPath(templFullPath.c_str());
  int storeResult = this->m_pCurrMarker->storeTemplate(newPersistence, "");
  if (storeResult != mtOK)
  {
    logError(storeResult, "Error in saving the current template");
    return (MicronTracker_Return)storeResult;
  }
  mtResetSamples();
  return MT_OK;
}

//----------------------------------------------------------------------------
void MicronTrackerInterface::mtResetSamples()
{
  for (unsigned int i = 0; i < this->m_sampleVectors.size(); i++)
  {
    delete this->m_sampleVectors[i];
  }
  this->m_sampleVectors.clear();
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetLoadedTemplatesNum()
{
  return this->m_pMarkers->getTemplateCount();
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetIdentifiedMarkersCount()
{
  return this->m_numOfIdentifiedMarkers;
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetUnidentifiedMarkersCount()
{
  return this->m_numOfUnidentifiedMarkers;
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetNumOfFacetsInMarker(int markerIndex)
{
  return this->m_vNumOfFacetsInEachMarker[markerIndex];
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetNumOfTotalFacetsInMarker(int markerIndex)
{
  return this->m_vNumOfTotalFacetsInEachMarker[markerIndex];
}

//----------------------------------------------------------------------------
void MicronTrackerInterface::mtFindIdentifiedMarkers()
{
  Collection* markersCollection = new Collection(this->m_pMarkers->identifiedMarkers(this->m_pCurrCam));
  this->m_numOfIdentifiedMarkers = markersCollection->count();
  if (this->m_numOfIdentifiedMarkers == 0)
  {
    this->m_markerStatus = MTI_NO_MARKER_CAPTURED;
    delete markersCollection;
    return;
  }
  this->m_2dvRotations.clear();
  this->m_2dvTranslations.clear();
  this->m_vIdentifiedMarkersXPoints.clear();
  for (unsigned int i = 0; i < this->m_vIdentifiedMarkersName.size(); i++)
  {
    this->m_vIdentifiedMarkersName[i].erase();
  }

  this->m_vIdentifiedMarkersName.resize(0);
  this->m_vIdentifiedMarkersName.clear();
  this->m_vNumOfFacetsInEachMarker.clear();
  this->m_vNumOfTotalFacetsInEachMarker.clear();

  this->m_markerStatus = MTI_MARKER_CAPTURED;
  int markerNum = 1;
  int facetNum = 1;

  for (markerNum = 1; markerNum <= markersCollection->count(); markerNum++)
  {
    Marker* marker = new Marker(markersCollection->itemI(markerNum));
    Collection* totalFacetsCollection = new Collection(marker->getTemplateFacets());
    this->m_vIdentifiedMarkersName.push_back(marker->getName());

    if (marker->wasIdentified(this->m_pCurrCam) != 0)
    {
      Collection* facetsCollection = new Collection(marker->identifiedFacets(this->m_pCurrCam));

      // Adjust the color temperature if we see a CoolCard marker
      std::string markerName = marker->getName();
      if ((0 == strncmp(markerName.c_str(), "COOL", 4)) ||
          (0 == strncmp(markerName.c_str(), "cool", 4)) ||
          (0 == strncmp(markerName.c_str(), "Cool", 4)))
      {
        Facet* f = new Facet(facetsCollection->itemI(1));
        Vector* colorVector = (f->IdentifiedVectors())[0];
        this->m_pCurrCam->adjustCoolnessFromColorVector(colorVector->getHandle());
        delete f;
      }
      std::vector<double> vXPointsTemp;
      for (facetNum = 1; facetNum <= facetsCollection->count(); facetNum++)
      {
        Facet* f = new Facet(facetsCollection->itemI(facetNum));
        // get Xpoints and then draw on each image if enabled
        Facet::XPointsType_LS_LRM_BH_XY xPoints_LS_LRM_BH_XY = {0};
        f->getXpoints(this->m_pCurrCam, xPoints_LS_LRM_BH_XY);

        // Have to push_back the elements one by one. If pass the address of the whole
        // array, the points will be deleted if the facet is deleted.

        // Left, Long vector
        vXPointsTemp.push_back(xPoints_LS_LRM_BH_XY[0][0][0][0]);
        vXPointsTemp.push_back(xPoints_LS_LRM_BH_XY[0][0][0][1]);
        vXPointsTemp.push_back(xPoints_LS_LRM_BH_XY[0][0][1][0]);
        vXPointsTemp.push_back(xPoints_LS_LRM_BH_XY[0][0][1][1]);

        // Right, Long vector
        vXPointsTemp.push_back(xPoints_LS_LRM_BH_XY[0][1][0][0]);
        vXPointsTemp.push_back(xPoints_LS_LRM_BH_XY[0][1][0][1]);
        vXPointsTemp.push_back(xPoints_LS_LRM_BH_XY[0][1][1][0]);
        vXPointsTemp.push_back(xPoints_LS_LRM_BH_XY[0][1][1][1]);

        // Left, short vector
        vXPointsTemp.push_back(xPoints_LS_LRM_BH_XY[1][0][0][0]);
        vXPointsTemp.push_back(xPoints_LS_LRM_BH_XY[1][0][0][1]);
        vXPointsTemp.push_back(xPoints_LS_LRM_BH_XY[1][0][1][0]);
        vXPointsTemp.push_back(xPoints_LS_LRM_BH_XY[1][0][1][1]);

        // Right, Short vector
        vXPointsTemp.push_back(xPoints_LS_LRM_BH_XY[1][1][0][0]);
        vXPointsTemp.push_back(xPoints_LS_LRM_BH_XY[1][1][0][1]);
        vXPointsTemp.push_back(xPoints_LS_LRM_BH_XY[1][1][1][0]);
        vXPointsTemp.push_back(xPoints_LS_LRM_BH_XY[1][1][1][1]);

        delete f;

      } // End of the for loop for the facets.
      this->m_vIdentifiedMarkersXPoints.push_back(vXPointsTemp);
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

      // problem lies here !
      double vR[3][3] = {0};
      Xform3D_RotMatGet(Marker2CurrCameraXf->getHandle(), reinterpret_cast<double*>(vR[0]));
      std::vector<double> vRotTemp;
      for (int j = 0; j < 3; j++)
      {
        for (int k = 0; k < 3; k++)
        {
          vRotTemp.push_back(vR[j][k]);
        }
      }

      this->m_2dvRotations.push_back(vRotTemp);
      delete Marker2CurrCameraXf;
    }
  }
  delete markersCollection;
}

//----------------------------------------------------------------------------
void MicronTrackerInterface::mtFindUnidentifiedMarkers()
{
  Collection* unidentifiedVectorsColl = new Collection(this->m_pMarkers->unidentifiedVectors(this->m_pCurrCam));
  this->m_numOfUnidentifiedMarkers = unidentifiedVectorsColl->count();

  /***********************************
  VERY IMPORTANT PROGRAMMING NOTE
  This checking is temporary as it seems a bug in MT libraries is reporting the number of unidentified markers to a weird number!
  ************************************/
  if (this->m_numOfUnidentifiedMarkers > 4)
  {
    this->m_numOfUnidentifiedMarkers = 4;
  }
  if (m_numOfUnidentifiedMarkers == 0)  // No unidentified vector
  {
    delete unidentifiedVectorsColl;
    return;
  }
  this->m_vUnidentifiedMarkersEndPoints.clear();
  for (int i = 1; i <= m_numOfUnidentifiedMarkers; i++)
  {
    std::vector<double> vUnidentifiedEndPointsTemp;
    Vector* v = new Vector(unidentifiedVectorsColl->itemI(i));
    Vector::EndXPointType_LRM_BH_XY endXPoints_LRM_BH_XY = {0};
    v->getEndXPoints(endXPoints_LRM_BH_XY);
    // Left
    vUnidentifiedEndPointsTemp.push_back(endXPoints_LRM_BH_XY[0][0][0]);
    vUnidentifiedEndPointsTemp.push_back(endXPoints_LRM_BH_XY[0][0][1]);
    vUnidentifiedEndPointsTemp.push_back(endXPoints_LRM_BH_XY[0][1][0]);
    vUnidentifiedEndPointsTemp.push_back(endXPoints_LRM_BH_XY[0][1][1]);

    vUnidentifiedEndPointsTemp.push_back(endXPoints_LRM_BH_XY[1][0][0]);
    vUnidentifiedEndPointsTemp.push_back(endXPoints_LRM_BH_XY[1][0][1]);
    vUnidentifiedEndPointsTemp.push_back(endXPoints_LRM_BH_XY[1][1][0]);
    vUnidentifiedEndPointsTemp.push_back(endXPoints_LRM_BH_XY[1][1][1]);

    // Right
    this->m_vUnidentifiedMarkersEndPoints.push_back(vUnidentifiedEndPointsTemp);
    delete v;
  }
  return ;
  delete unidentifiedVectorsColl;
}

//----------------------------------------------------------------------------
void MicronTrackerInterface::mtGetTranslations(std::vector<double>& vTranslations, int markerIndex)
{
  vTranslations = this->m_2dvTranslations[markerIndex];
}

//----------------------------------------------------------------------------
void MicronTrackerInterface::mtGetRotations(std::vector<double>& vRotations, int markerIndex)
{
  vRotations = this->m_2dvRotations[markerIndex];
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetStatus()
{
  return this->m_markerStatus;
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetMarkerStatus(int loadedMarkerIndex, int* identifiedMarkerIndex)
{
  // Safety check. If the request marker index is greater than the identified markers, return NO_MARKER_CAPTURED
  if (loadedMarkerIndex > this->mtGetLoadedTemplatesNum() || loadedMarkerIndex < 0)
  {
    return MTI_NO_MARKER_CAPTURED;
  }

  std::string markerName = this->mtGetTemplateName(loadedMarkerIndex);
  for (int i = 0; i < this->m_numOfIdentifiedMarkers; i++)
  {
    if (markerName == this->m_vIdentifiedMarkersName[i])
    {
      *identifiedMarkerIndex = i;
      return MTI_MARKER_CAPTURED;
    }
  }
  return MTI_NO_MARKER_CAPTURED;
}

//----------------------------------------------------------------------------
bool MicronTrackerInterface::mtSelectCamera(int index)
{
  if (!this->checkCamIndex(index))
  {
    this->m_pCurrCam = this->m_pCameras->getCamera(index);
  }
  if (this->m_pCurrCam != NULL)
  {
    this->m_currCamIndex = index;
    return true;
  }
  else
  {
    return false;
  }
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetCurrCamIndex()
{
  return this->m_currCamIndex;
}

//----------------------------------------------------------------------------
MicronTracker_Return MicronTrackerInterface::mtSetShutterPreference(double n)
{
  return MT_OK;// this->m_pCameras->setShutterPreference(n);
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetShutterPreference()
{
  //return this->m_pCameras->getShutterPreference(); The MTC.h
  return 0.0;
}

//----------------------------------------------------------------------------
MicronTracker_Return MicronTrackerInterface::mtSetTemplMatchTolerance(double matchTolerance)
{
  return (MicronTracker_Return)this->m_pMarkers->setTemplateMatchToleranceMM(matchTolerance);
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetTemplMatchTolerance()
{
  return this->m_pMarkers->getTemplateMatchToleranceMM();
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetTemplMatchToleranceDefault()
{
  double defToleranceVal = 0;
  Markers_TemplateMatchToleranceMMDefaultGet(&defToleranceVal);
  return defToleranceVal;
  //return this->m_pMarkers->getDefaultTemplateMatchToleranceMM();
}

//----------------------------------------------------------------------------
MicronTracker_Return MicronTrackerInterface::mtSetPredictiveFramesInterleave(int predictiveInterleave)
{
  return (MicronTracker_Return)this->m_pMarkers->setPredictiveFramesInterleave(predictiveInterleave);
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetPredictiveFramesInterleave()
{
  return this->m_pMarkers->getPredictiveFramesInterleave();
}

//----------------------------------------------------------------------------
void MicronTrackerInterface::mtSetPredictiveTracking(bool predTracking)
{
  //m_pMarkerTempls->setPredictiveTracking(predTracking);
}

//----------------------------------------------------------------------------
MicronTracker_Return MicronTrackerInterface::mtGetPredictiveTracking()
{
  return MT_InternalMTError;//m_pMarkerTempls->getPredictiveTracking();
}

//----------------------------------------------------------------------------
void MicronTrackerInterface::mtSetAdjustCamAfterEveryProcess(bool autoCamExp)
{
  //m_pMarkerTempls->setAdjustCamAfterEveryProcess(autoCamExp);
}

//----------------------------------------------------------------------------
short MicronTrackerInterface::mtGetAdjustCamAfterEveryProcess()
{
  return 0;//m_pMarkerTempls->getAdjustCamAfterEveryProcess();
}

//----------------------------------------------------------------------------
MicronTracker_Return MicronTrackerInterface::mtSetShutterTime(double n, int index)
{
  if (index == -1)
  {
    return (MicronTracker_Return)this->m_pCurrCam->setShutterTime(n);
  }
  else if (!this->checkCamIndex(index))
  {
    return MT_CameraNotFound;
  }

  else
  {
    return (MicronTracker_Return)this->m_pCameras->getCamera(index)->setShutterTime(n);
  }
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetShutterTime(int index)
{
  if (index == -1)
  {
    return this->m_pCurrCam->getShutterTime();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1.0;
  }
  else
  {
    return this->m_pCameras->getCamera(index)->getShutterTime();
  }
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetMinShutterTime(int index)
{
  if (index == -1)
  {
    return this->m_pCurrCam->getMinShutterTime();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1.0;
  }
  else
  {
    return this->m_pCameras->getCamera(index)->getMinShutterTime();
  }
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetMaxShutterTime(int index)
{
  if (index == -1)
  {
    return this->m_pCurrCam->getMaxShutterTime();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1.0;
  }

  else
  {
    return this->m_pCameras->getCamera(index)->getMaxShutterTime();
  }
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetGain(int index)
{
  if (index == -1)
  {
    return this->m_pCurrCam->getGain();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1.0;
  }
  else
  {
    return this->m_pCameras->getCamera(index)->getGain();
  }
}

//----------------------------------------------------------------------------
MicronTracker_Return MicronTrackerInterface::mtSetGain(double n, int index)
{
  if (index == -1)
  {
    return (MicronTracker_Return)this->m_pCurrCam->setGain(n);
  }
  else if (!this->checkCamIndex(index))
  {
    return MT_CameraNotFound;
  }
  else
  {
    return (MicronTracker_Return)this->m_pCameras->getCamera(index)->setGain(n);
  }
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetMinGain(int index)
{
  if (index == -1)
  {
    return this->m_pCurrCam->getMinGain();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1.0;
  }
  else
  {
    return this->m_pCameras->getCamera(index)->getMinGain();
  }
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetMaxGain(int index)
{
  if (index == -1)
  {
    return this->m_pCurrCam->getMaxGain();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1.0;
  }
  else
  {
    return this->m_pCameras->getCamera(index)->getMaxGain();
  }
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetDBGain(int index)
{
  if (index == -1)
  {
    return this->m_pCurrCam->getDBGain();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1.0;
  }
  else
  {
    return this->m_pCameras->getCamera(index)->getDBGain();
  }
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetExposure(int index)
{
  if (index == -1)
  {
    return this->m_pCurrCam->getExposure();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1.0;
  }
  else
  {
    return this->m_pCameras->getCamera(index)->getExposure();
  }
}

//----------------------------------------------------------------------------
MicronTracker_Return MicronTrackerInterface::mtSetExposure(double n, int index)
{
  if (index == -1)
  {
    return this->m_pCurrCam->setExposure(n);
  }
  else if (!this->checkCamIndex(index))
  {
    return MT_CameraNotFound;
  }
  else
  {
    return this->m_pCameras->getCamera(index)->setExposure(n);
  }
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetMinExposure(int index)
{
  if (index == -1)
  {
    return this->m_pCurrCam->getMinExposure();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1.0;
  }
  else
  {
    return this->m_pCameras->getCamera(index)->getMinExposure();
  }
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetMaxExposure(int index)
{
  if (index == -1)
  {
    return this->m_pCurrCam->getMaxExposure();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1.0;
  }
  else
  {
    return this->m_pCameras->getCamera(index)->getMaxExposure();
  }
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetLightCoolness(int index)
{
  if (index == -1)
  {
    return this->m_pCurrCam->getLightCoolness();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1.0;
  }
  else
  {
    return this->m_pCameras->getCamera(index)->getLightCoolness();
  }
}

//----------------------------------------------------------------------------
MicronTracker_Return MicronTrackerInterface::mtSetCamAutoExposure(int n, int index)
{
  if (index == -1)
  {
    return (MicronTracker_Return)this->m_pCurrCam->setAutoExposure(n);
  }
  else if (!this->checkCamIndex(index))
  {
    return MT_CameraNotFound;
  }
  else
  {
    return (MicronTracker_Return)this->m_pCameras->getCamera(index)->setAutoExposure(n);
  }
}

//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetLatestFrameTime(int index)
{
  if (index == -1)
  {
    return this->m_pCurrCam->getFrameTime();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1.0;
  }
  else
  {
    return this->m_pCameras->getCamera(index)->getFrameTime();
  }
}

//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
MicronTracker_Return MicronTrackerInterface::mtGetLatestFramePixHistogram(long*& aPixHist, int subSampleRate, int index)
{
  return MT_InternalMTError;//m_pCurrCams->getLatestFramePixHistogram(aPixHist, subSampleRate, index);
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetLatestFrameHazard()
{
  return Camera_LastFrameThermalHazard(this->m_pCurrCam->getHandle());
}

//----------------------------------------------------------------------------
MicronTracker_Return MicronTrackerInterface::mtSetTemplateName(int index, const std::string& templName)
{
  int result = mtOK;
  // Check the index
  if (index < 0 || index > this->m_pMarkers->getTemplateCount())
  {
    logError(-1, "Marker index out of range");
    return MT_InvalidIndex;
  }
  std::string oldN = this->mtGetTemplateName(index);
  this->m_pMarkers->setTemplateItemName(index, templName);
  this->m_pMarkers->storeTemplate(index, this->m_pPers->getHandle(), NULL);
  result = this->renameFile(oldN, templName, "Markers");
  if (result != mtOK)
  {
    logError(result, "Changing the name of the template failed");
  }
  return (MicronTracker_Return)result;
}

//----------------------------------------------------------------------------
std::string MicronTrackerInterface::mtGetTemplateName(int index)
{
  std::string s;
  this->m_pMarkers->getTemplateItemName(index, s);
  return s;
}

//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
MicronTracker_Return MicronTrackerInterface::mtDeleteTemplate(int index)
{
  int result = mtOK;
  if (index > -1 && index < this->m_pMarkers->getTemplateCount())
  {
    std::string templateName = this->mtGetTemplateName(index);//this->m_pMarkers->getTemplateItemName(index);
    result = this->removeFile(templateName, "Markers");
    if (result != 0)
    {
      logError(result, "Could not delete the template");
    }
  }
  else
  {
    logError(-1, "Marker index out of range");
  }
  return (MicronTracker_Return)result;
}

//----------------------------------------------------------------------------
MicronTracker_Return MicronTrackerInterface::mtGetLeftRightImageArray(unsigned char**& leftImageArray, unsigned char**& rightImageArray, int index)
{
  int result = mtOK;
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

  return (MicronTracker_Return)result;
}

//----------------------------------------------------------------------------
MicronTracker_Return MicronTrackerInterface::mtGetLeftRightImageArrayHalfSize(unsigned char**& leftImageArray,
    unsigned char**& rightImageArray,
    int xResolution, int yResolution, int index)
{
  int result = mtOK;
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
  return (MicronTracker_Return)result;
}

//----------------------------------------------------------------------------
void MicronTrackerInterface::mtGetIdentifiedMarkersXPoints(double*& xPoints, int markerIndex)
{
  //Check the marker index
  if (markerIndex >= this->m_numOfIdentifiedMarkers - 1)
  {
    markerIndex = this->m_numOfIdentifiedMarkers - 1;
  }
  if (markerIndex < 0)
  {
    markerIndex = 0;
  }
  xPoints = &this->m_vIdentifiedMarkersXPoints[markerIndex][0];
}

//----------------------------------------------------------------------------
void MicronTrackerInterface::mtGetUnidentifiedMarkersEnds(double*& endPoints, int vectorIndex)
{
  //Check the vector index
  if (vectorIndex >= this->m_numOfUnidentifiedMarkers - 1)
  {
    vectorIndex = this->m_numOfUnidentifiedMarkers - 1;
  }
  if (vectorIndex < 0)
  {
    vectorIndex = 0;
  }
  endPoints = &this->m_vUnidentifiedMarkersEndPoints[vectorIndex][0];
}

//----------------------------------------------------------------------------
MicronTracker_Return MicronTrackerInterface::removeFile(const std::string& fileName, const std::string& dir)
{
  std::string fullPath = dir + "/" + fileName;
  int result = remove(fullPath.c_str());
  if (result != 0)
  {
    // failed
    return MT_RemoveFailed;
  }
  return MT_OK;
}

//-------------------------------------------------------------------------------
MicronTracker_Return MicronTrackerInterface::renameFile(const std::string& oldName, const std::string& newName, const std::string& dir)
{
  std::string fullPathOld = dir + "/" + oldName;
  std::string fullPathNew = dir + "/" + newName;

  int result = rename(fullPathOld.c_str(), fullPathNew.c_str());
  if (result != 0)
  {
    // failed
    return MT_RenameFailed;
  }
  return MT_OK;
}

//----------------------------------------------------------------------------
void MicronTrackerInterface::logError(int errorNum, const char* description/*=NULL*/)
{
  if (errorNum == mtOK)
  {
    // everything is OK, no error to report
    return;
  }
  std::ostringstream msg;
  msg << "MicronTracker error: " << errorNum;
  msg << " (" << ConvertReturnToString((MicronTracker_Return)errorNum) << ")";

  if (description)
  {
    msg << ": " << description;
  }
  msg << std::ends;

  m_errorString = msg.str();

  LOG_ERROR(m_errorString.c_str());
}

//----------------------------------------------------------------------------
std::string MicronTrackerInterface::GetSdkVersion()
{
  std::ostringstream version;
  version << "MTC-" << MTCMajorVersion << "." << MTCMinorVersion << "." << MTCBuild << "." << MTCRevision;
  return version.str();
}

//----------------------------------------------------------------------------
void MicronTrackerInterface::getFileNamesFromDirectory(std::vector<std::string>& fileNames, const std::string& dir, bool returnCompletePath)
{
#ifdef _WIN32 // Windows
  _finddata_t file;
  std::string findPath = dir + "\\*.*";
  intptr_t currentPosition = _findfirst(findPath.c_str(), &file); //find the first file in directory
  if (currentPosition == -1L)
  {
    return ; //end the procedure if no file is found
  }
  do
  {
    std::string fileName = file.name;
    //ignore . and ..
    if (strcmp(fileName.c_str(), ".") != 0 && strcmp(fileName.c_str(), "..") != 0)
    {
      //If not subdirectory
      if (!(file.attrib & _A_SUBDIR))
      {
        //If returnCompletePath is true return the full path of the files, otherwise return just the file names.
        if (returnCompletePath)
        {
          fileNames.push_back(std::string(dir) + "/" +  fileName);
        }
        else
        {
          fileNames.push_back(fileName);
        }
      }
    }
  }
  while (_findnext(currentPosition, &file) == 0);
  _findclose(currentPosition); //close search
#else // Linux
  DIR* d;
  struct dirent* ent;

  if ((d = opendir(dir)) != NULL)
  {
    while ((ent = readdir(d)) != NULL)
    {
      string entry(ent->d_name);
      if (entry != "." && entry != "..")
        //If returnCompletePath is true, return the full path of the files, otherwise return just the file names.
      {
        returnCompletePath ? fileNames.push_back(currentFolderPath + entry) : fileNames.push_back(entry);
      }
    }
  }
  closedir(d);
#endif
}
