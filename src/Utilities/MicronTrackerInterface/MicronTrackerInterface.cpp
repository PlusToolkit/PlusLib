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
int MicronTrackerInterface::mtInit(const std::string& iniFilePath)
{
  this->ErrorString.clear();
  this->IsCameraAttached = false;
  this->MarkerList = new Markers();
  this->Settings = new Persistence();
  //m_pCurrTempMarker =  new Marker();
  this->TempMarkerForAddingFacet = NULL; //new Marker();

  initialINIAccess(iniFilePath);

  return mtOK;
}

//----------------------------------------------------------------------------
void MicronTrackerInterface::mtEnd()
{
  this->mtDetachCameras();
}

//----------------------------------------------------------------------------
void MicronTrackerInterface::initialINIAccess(const std::string& iniFilePath)
{
  this->Settings->setPath(iniFilePath.c_str());
  this->Settings->setSection("General");

  //Setting the FrameInterleave property in the Markers object
  int defaultFrameInterleave = 0;
  this->MarkerList->setPredictiveFramesInterleave(this->Settings->retrieveInt("PredictiveFramesInterleave", defaultFrameInterleave));

  //Setting the TemplateMatchToleranceMM property in the Markers object
  double defaultTempMatchToleranceMM = 1.0;
  this->MarkerList->setTemplateMatchToleranceMM(this->Settings->retrieveDouble("TemplateMatchToleranceMM", defaultTempMatchToleranceMM));
}

//----------------------------------------------------------------------------
void MicronTrackerInterface::mtSaveSettingsToINI()
{
  this->Settings->saveDouble("TemplateMatchToleranceMM", this->MarkerList->getTemplateMatchToleranceMM());
  this->Settings->saveInt("PredictiveFramesInterleave", this->MarkerList->getPredictiveFramesInterleave());
}

//----------------------------------------------------------------------------
std::string MicronTrackerInterface::ConvertReturnToString(int returnValue)
{
  switch (returnValue)
  {
    case mtOK:
      return "OK";
    case mtInvalidHandle:
      return "InvalidHandle";
    case mtReentrantAccess:
      return "ReentrantAccess";
    case mtInternalMTError:
      return "InternalMTError";
    case mtNullPointer:
      return "NullPointer";
    case mtOutOfMemory:
      return "OutOfMemory";
    case mtParameterOutOfRange:
      return "ParameterOutOfRange";
    case mtStringParamTooLong:
      return "StringParamTooLong";
    case mtOutBufferTooSmall:
      return "OutBufferTooSmall";
    case mtCameraNotInitialized:
      return "CameraNotInitialized";
    case mtCameraAlreadyInitialized:
      return "CameraAlreadyInitialized";
    case mtCameraInitializationFailed:
      return "CameraInitializationFailed";
    case mtCompatibilityError:
      return "CompatibilityError";
    case mtDataNotAvailable:
      return "DataNotAvailable";
    case mtUnrecognizedCameraModel:
      return "UnrecognizedCameraModel";
    case mtPathNotSet:
      return "PathNotSet";
    case mtCannotAccessDirectory:
      return "CannotAccessDirectory";
    case mtWriteToFileFailed:
      return "WriteToFileFailed";
    case mtInvalidIndex:
      return "InvalidIndex";
    case mtInvalidSideI:
      return "InvalidSideI";
    case mtInvalidDivisor:
      return "InvalidDivisor";
    case mtEmptyCollection:
      return "EmptyCollection";
    case mtInsufficientSamples:
      return "InsufficientSamples";
    case mtInsufficientSamplesWithinTolerance:
      return "InsufficientSamplesWithinTolerance";
    case mtOddNumberOfSamples:
      return "OddNumberOfSamples";
    case mtLessThan2Vectors:
      return "LessThan2Vectors";
    case mtMoreThanMaxVectorsPerFacet:
      return "MoreThanMaxVectorsPerFacet";
    case mtErrorExceedsTolerance:
      return "ErrorExceedsTolerance";
    case mtInsufficientAngleBetweenVectors:
      return "InsufficientAngleBetweenVectors";
    case mtFirstVectorShorterThanSecond:
      return "FirstVectorShorterThanSecond";
    case mtVectorLengthsTooSimilar:
      return "VectorLengthsTooSimilar";
    case mtNullTemplateVector:
      return "NullTemplateVector";
    case mtTemplateNotSet:
      return "TemplateNotSet";
    case mtCorruptTemplateFile:
      return "CorruptTemplateFile";
    case mtMaxMarkerTemplatesExceeded:
      return "MaxMarkerTemplatesExceeded";
    case mtDifferentFacetsGeometryTooSimilar:
      return "DifferentFacetsGeometryTooSimilar";
    case mtNoncompliantFacetDefinition:
      return "NoncompliantFacetDefinition";
    case mtCollectionContainsNonVectorHandles:
      return "CollectionContainsNonVectorHandles";
    case mtEmptyBuffer:
      return "EmptyBuffer";
    case mtDimensionsDoNotMatch:
      return "DimensionsDoNotMatch";
    case mtOpenFileFailed:
      return "OpenFileFailed";
    case mtReadFileFailed:
      return "ReadFileFailed";
    case mtWriteFileFailed:
      return "WriteFileFailed";
    case mtCannotOpenCalibrationFile:
      return "CannotOpenCalibrationFile";
    case mtNotACalibrationFile:
      return "NotACalibrationFile";
    case mtCalibrationFileCorrupt:
      return "CalibrationFileCorrupt";
    case mtCalibrationFileDoesNotMatchCamera:
      return "CalibrationFileDoesNotMatchCamera";
    case mtCalibrationFileNotLoaded:
      return "CalibrationFileNotLoaded";
    case mtIncorrectFileVersion:
      return "IncorrectFileVersion";
    case mtLocationOutOfMeasurementBounds:
      return "LocationOutOfMeasurementBounds";
    case mtCannotTriangulate:
      return "CannotTriangulate";
    case mtUnknownXform:
      return "UnknownXform";
    case mtCameraNotFound:
      return "CameraNotFound";
    case mtFeatureDataUnavailable:
      return "FeatureDataUnavailable";
    case mtFeatureDataCorrupt:
      return "FeatureDataCorrupt";
    case mtXYZOutOfFOV:
      return "XYZOutOfFOV";
    case mtGrabFrameError:
      return "GrabFrameError";
    case mtGrabTimeOut:
      return "GrabTimeOut";
    case mtCannotCreateThread:
      return "CannotCreateThread";
    case mtHdrIsNotEnabled:
      return "HdrIsNotEnabled";
    case mtFeatureNotSupported:
      return "FeatureNotSupported";
    case mtHDRFrameCycleNotSupported:
      return "HDRFrameCycleNotSupported";
    case mtFeatureNotSupportedForX64:
      return "FeatureNotSupportedForX64";
    case mtNonUniformLightOnCoolCard:
      return "NonUniformLightOnCoolCard";
    case mtBackgroundProcessMutexError:
      return "BackgroundProcessMutexError";
    case mtTooManyVectors:
      return "TooManyVectors";
    case mtMethodIsObsolete:
      return "MethodIsObsolete";
    case utInvalidVectors:
      return "InvalidVectors";
    case utMismatchedVectors:
      return "MismatchedVectors";
    case mtInsufficientNumberOfXPoints:
      return "InsufficientNumberOfXPoints";
    case mtXPointsProcessingIsDisabled:
      return "XPointsProcessingIsDisabled";
  }

  return "Unknown";
}

//----------------------------------------------------------------------------
std::string MicronTrackerInterface::ConvertHazardToString(int hazardCode)
{
  switch (hazardCode)
  {
    case mtNone:
      return "None";
    case mtCustomCoolnessLoaded:
      return "CustomCoolnessLoaded";
    case mtShadowOverXP:
      return "ShadowOverXP";
    case mtCameraBelowMinimumOperatingTemperature:
      return "CameraBelowMinimumOperatingTemperature";
    case mtCameraAboveMaximumOperatingTemperature:
      return "CameraAboveMaximumOperatingTemperature";
    case mtRapidInternalTemperatureChange:
      return "RapidInternalTemperatureChange";
    case mtOutsideCalibratedVolume:
      return "OutsideCalibratedVolume";
    case mtOutsideExtendedCalibratedVolume:
      return "OutsideExtendedCalibratedVolume";
    case mtCameraWarmingUp:
      return "CameraWarmingUp";
    case mtDeficientMarkerTemplateForWarmupCorrection:
      return "DeficientMarkerTemplateForWarmupCorrection";
  }

  return "Unknown";
}

//----------------------------------------------------------------------------
std::string MicronTrackerInterface::GetLastErrorString()
{
  return this->ErrorString;
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtSetupCameras()
{
  this->CameraList = new Cameras();
  int result = this->CameraList->attachAvailableCameras();

  if (result == mtOK && this->CameraList->getCount() >= 1)
  {
    // Set the current camera to the first in the list.
    this->CurrentCameraIndex = 0;
    this->CurrentCamera = this->CameraList->getCamera(this->CurrentCameraIndex);
    this->IsCameraAttached = true;

    return mtOK;
  }
  else
  {
    this->logError(result);
    return mtCameraNotFound;
  }
}

//----------------------------------------------------------------------------
void MicronTrackerInterface::mtDetachCameras()
{
  if (this->IsCameraAttached)
  {
    this->IsCameraAttached = false;
    this->CameraList->detach();
  }
}

//----------------------------------------------------------------------------
// Checks to see if the passed argument is within the camera index, i.e. it is not
//    less than 0 or more than the number of attached camera (minus 1, of course).
int MicronTrackerInterface::checkCamIndex(int id)
{
  if (id > -1 && id < this->CameraList->getCount())
  {
    return mtOK;
  }
  else
  {
    logError(-1, "Camera index out of bound");
    return mtCameraNotFound;
  }
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetSerialNum(int index)
{
  if (index == -1)
  {
    return this->CurrentCamera->getSerialNum();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1;
  }
  else
  {
    return this->CameraList->getCamera(index)->getSerialNum();
  }
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetXResolution(int index)
{
  if (index == -1)
  {
    return this->CurrentCamera->getXRes();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1;
  }
  else
  {
    return this->CameraList->getCamera(index)->getXRes();
  }
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetYResolution(int index)
{
  if (index == -1)
  {
    return this->CurrentCamera->getYRes();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1;
  }
  else
  {
    return this->CameraList->getCamera(index)->getYRes();
  }
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetNumOfSensors(int index /*= -1*/)
{
  return 2;
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetNumOfCameras()
{
  return this->CameraList->getCount();
}
//----------------------------------------------------------------------------
int MicronTrackerInterface::mtRefreshTemplates(std::vector<std::string>& tmplsName,
    std::vector<std::string>& tmplsError,
    const std::string& tmplsPath)
{
  getFileNamesFromDirectory(tmplsName, tmplsPath, true);
  int status = this->MarkerList->clearTemplates();
  if (status != mtOK)
  {
    logError(status, "Clear templates failed");
    return status;
  }
  LOG_DEBUG("Cleared templates");
  status = Markers_LoadTemplates((char*)tmplsPath.c_str()); // need casting to non-const char because of the improper MTC interface definition
  if (status != mtOK)
  {
    std::string msg = "Load templates failed from directory: " + tmplsPath;
    logError(status, msg.c_str());
    return status;
  }
  return mtOK;
}
//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGrabFrame(int index)
{
  int result = mtOK;
  if (index == -1) // From all the cameras
  {
    result = this->CameraList->grabFrame();
  }
  else if (!this->checkCamIndex(index)) // Camera index out of range
  {
    result = false;
  }
  else // From one camera
  {
    result = this->CameraList->grabFrame(this->CameraList->getCamera(index));
  }

  return result;
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtProcessFrame()
{
  int result = this->MarkerList->processFrame(this->CurrentCamera);
  if (result != mtOK)
  {
    this->logError(result);
  }
  return result;
}

//----------------------------------------------------------------------------
MicronTrackerInterface::NewSampleReturnValue MicronTrackerInterface::mtCollectNewSamples(int collectingAdditionalFacet)
{
  NewSampleReturnValue result = NewSample_2Vectors;
  Collection* col = new Collection(this->MarkerList->unidentifiedVectors(this->CurrentCamera));
  this->IsAddingAdditionalFacet = collectingAdditionalFacet;
  if (col->count() == 2 || collectingAdditionalFacet == 1)
  {
    this->SampleVectors.push_back(col);
    result = NewSample_2Vectors;

    // Additional facets
    if (this->IsAddingAdditionalFacet)
    {
      Collection* markersCollection = new Collection(this->MarkerList->identifiedMarkers(this->CurrentCamera));
      this->TempMarkerForAddingFacet = new Marker(markersCollection->itemI(1));
      Collection* identifiedFacetsCol = new Collection(this->TempMarkerForAddingFacet->getTemplateFacets());
      if (identifiedFacetsCol->count() > 0)
      {
        // Also compute and save the xform of facet1 to sensor
        // Note (Programming Note): The following if block has been commented out because it
        // relates to registering a marker with two cameras. As soon as the second camera is
        // in, we can start working on it.
        //if (this->m_pCurrMarker->marker2CameraXf(this->m_pCurrCam->getHandle() != (Xform3D*)NULL)
        this->Facet1ToCameraXfs.push_back(this->TempMarkerForAddingFacet->marker2CameraXf(this->CurrentCamera->getHandle()));

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
        this->SampleVectors.pop_back();
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
int MicronTrackerInterface::mtStopSampling(char* templateName, double jitterValue)
{
  std::string errorsString;
  Facet* f = new Facet();
  std::vector<Vector*> vectorPair;
  int result = mtOK;
  if (f->setVectorsFromSample(this->SampleVectors, errorsString))
  {
    if (this->IsAddingAdditionalFacet)
    {
      Collection* markersCollection = new Collection(this->MarkerList->identifiedMarkers(this->CurrentCamera));
      //this->m_pTempMarkerForAddingFacet->setHandle(markersCollection->itemI(1));
      Collection* identifiedFacetsCol = new Collection(this->TempMarkerForAddingFacet->getTemplateFacets());
      std::vector<Xform3D*> facet1ToNewFacetXfs;
      Xform3D* facet1ToNewFacetXf;// = new Xform3D();

      for (unsigned int i = 0; i < this->SampleVectors.size(); i++)
      {
        vectorPair.clear();
        vectorPair.push_back(new Vector(this->SampleVectors[i]->itemI(0)));
        vectorPair.push_back(new Vector(this->SampleVectors[i]->itemI(1)));
        //Programming Note: The following line is crashing in the Facet.cpp when
        //it makes a call to the DLL. As a result no registration of multi-facet markers can be made at this point. June 7, 2004
        if (f->identify(this->CurrentCamera, vectorPair, 1)) // Then the sample matches the template
        {
          // Compute the xform between the first marker facet and the new one
          facet1ToNewFacetXf = this->Facet1ToCameraXfs[i]->concatenate(f->getFacet2CameraXf(this->CurrentCamera)->inverse());
          facet1ToNewFacetXfs.push_back(facet1ToNewFacetXf);
        }

      }
      // Combine the transforms accumulated to a new one and save it with the facet in the marker
      facet1ToNewFacetXf = facet1ToNewFacetXfs[1];
      for (unsigned int i = 2; i < facet1ToNewFacetXfs.size(); i++)
      {
        facet1ToNewFacetXf->inBetween(facet1ToNewFacetXfs[i], 1);// will result in equal contribution by all faces
      }

      this->TempMarkerForAddingFacet->addTemplateFacet(f, facet1ToNewFacetXf);
    }
    else
    {
      this->CurrentMarker = new Marker();
      Xform3D* Xf = new Xform3D();
      this->CurrentMarker->setName(templateName);
      result = this->CurrentMarker->addTemplateFacet(f, Xf);
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
  return result;
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtSaveMarkerTemplate(const std::string& templName, const std::string& dir)
{
  std::string templFullPath = dir + "/" + templName;
  Persistence* newPersistence = new Persistence();
  newPersistence->setPath(templFullPath.c_str());
  int storeResult = this->CurrentMarker->storeTemplate(newPersistence, "");
  if (storeResult != mtOK)
  {
    logError(storeResult, "Error in saving the current template");
    return storeResult;
  }
  mtResetSamples();
  return mtOK;
}

//----------------------------------------------------------------------------
void MicronTrackerInterface::mtResetSamples()
{
  for (unsigned int i = 0; i < this->SampleVectors.size(); i++)
  {
    delete this->SampleVectors[i];
  }
  this->SampleVectors.clear();
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetLoadedTemplatesNum()
{
  return this->MarkerList->getTemplateCount();
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetIdentifiedMarkersCount()
{
  return this->NumOfIdentifiedMarkers;
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetUnidentifiedMarkersCount()
{
  return this->NumOfUnidentifiedMarkers;
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetNumOfFacetsInMarker(int markerIndex)
{
  return this->NumOfFacetsInEachMarker[markerIndex];
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetNumOfTotalFacetsInMarker(int markerIndex)
{
  return this->NumOfTotalFacetsInEachMarker[markerIndex];
}

//----------------------------------------------------------------------------
void MicronTrackerInterface::mtFindIdentifiedMarkers()
{
  Collection* markersCollection = new Collection(this->MarkerList->identifiedMarkers(this->CurrentCamera));
  this->NumOfIdentifiedMarkers = markersCollection->count();
  if (this->NumOfIdentifiedMarkers == 0)
  {
    this->MarkerStatus = MTI_NO_MARKER_CAPTURED;
    delete markersCollection;
    return;
  }
  this->Rotations2D.clear();
  this->Translations2D.clear();
  this->IdentifiedMarkersXPoints.clear();
  for (unsigned int i = 0; i < this->IdentifiedMarkersName.size(); i++)
  {
    this->IdentifiedMarkersName[i].erase();
  }

  this->IdentifiedMarkersName.resize(0);
  this->IdentifiedMarkersName.clear();
  this->NumOfFacetsInEachMarker.clear();
  this->NumOfTotalFacetsInEachMarker.clear();

  this->MarkerStatus = MTI_MARKER_CAPTURED;
  int markerNum = 1;
  int facetNum = 1;

  for (markerNum = 1; markerNum <= markersCollection->count(); markerNum++)
  {
    Marker* marker = new Marker(markersCollection->itemI(markerNum));
    Collection* totalFacetsCollection = new Collection(marker->getTemplateFacets());
    this->IdentifiedMarkersName.push_back(marker->getName());

    if (marker->wasIdentified(this->CurrentCamera) != 0)
    {
      Collection* facetsCollection = new Collection(marker->identifiedFacets(this->CurrentCamera));

      // Adjust the color temperature if we see a CoolCard marker
      std::string markerName = marker->getName();
      if ((0 == strncmp(markerName.c_str(), "COOL", 4)) ||
          (0 == strncmp(markerName.c_str(), "cool", 4)) ||
          (0 == strncmp(markerName.c_str(), "Cool", 4)))
      {
        Facet* f = new Facet(facetsCollection->itemI(1));
        Vector* colorVector = (f->IdentifiedVectors())[0];
        this->CurrentCamera->adjustCoolnessFromColorVector(colorVector->getHandle());
        delete f;
      }
      std::vector<double> vXPointsTemp;
      for (facetNum = 1; facetNum <= facetsCollection->count(); facetNum++)
      {
        Facet* f = new Facet(facetsCollection->itemI(facetNum));
        // get Xpoints and then draw on each image if enabled
        Facet::XPointsType_LS_LRM_BH_XY xPoints_LS_LRM_BH_XY = {0};
        f->getXpoints(this->CurrentCamera, xPoints_LS_LRM_BH_XY);

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
      this->IdentifiedMarkersXPoints.push_back(vXPointsTemp);
      this->NumOfFacetsInEachMarker.push_back(facetsCollection->count());
      this->NumOfTotalFacetsInEachMarker.push_back(totalFacetsCollection->count());
      delete facetsCollection;

      /***********************************/
      /*VERY IMPORTANT PROGRAMMING NOTE
      Deleting the totalFacetsCollection here causes the identified facets to be deleted
      and hence not shown as identified on the GUI side. Obviously on the other hand, not deleting this
      object results in memory leak. Should be fixed soon! */
      /***********************************/
      //      delete totalFacetsCollection;
      Xform3D* Marker2CurrCameraXf = marker->marker2CameraXf(this->CurrentCamera->getHandle());
      // Find the translations and push them in a 2 temporary vector and then push that temp vector into a
      // 2 dimensional vector.
      std::vector<double> vTransTemp;
      vTransTemp.clear();
      for (int i = 0 ; i < 3; i++)
      {
        vTransTemp.push_back(Marker2CurrCameraXf->getShift(i));
      }
      vTransTemp.push_back(1);
      this->Translations2D.push_back(vTransTemp);
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

      this->Rotations2D.push_back(vRotTemp);
      delete Marker2CurrCameraXf;
    }
  }
  delete markersCollection;
}

//----------------------------------------------------------------------------
void MicronTrackerInterface::mtFindUnidentifiedMarkers()
{
  Collection* unidentifiedVectorsColl = new Collection(this->MarkerList->unidentifiedVectors(this->CurrentCamera));
  this->NumOfUnidentifiedMarkers = unidentifiedVectorsColl->count();

  /***********************************
  VERY IMPORTANT PROGRAMMING NOTE
  This checking is temporary as it seems a bug in MT libraries is reporting the number of unidentified markers to a weird number!
  ************************************/
  if (this->NumOfUnidentifiedMarkers > 4)
  {
    this->NumOfUnidentifiedMarkers = 4;
  }
  if (this->NumOfUnidentifiedMarkers == 0)  // No unidentified vector
  {
    delete unidentifiedVectorsColl;
    return;
  }
  this->UnidentifiedMarkersEndPoints.clear();
  for (int i = 1; i <= this->NumOfUnidentifiedMarkers; i++)
  {
    std::vector<double> unidentifiedEndPointsTemp;
    Vector* v = new Vector(unidentifiedVectorsColl->itemI(i));
    Vector::EndXPointType_LRM_BH_XY endXPoints_LRM_BH_XY = {0};
    v->getEndXPoints(endXPoints_LRM_BH_XY);

    // Left
    unidentifiedEndPointsTemp.push_back(endXPoints_LRM_BH_XY[0][0][0]);
    unidentifiedEndPointsTemp.push_back(endXPoints_LRM_BH_XY[0][0][1]);
    unidentifiedEndPointsTemp.push_back(endXPoints_LRM_BH_XY[0][1][0]);
    unidentifiedEndPointsTemp.push_back(endXPoints_LRM_BH_XY[0][1][1]);

    unidentifiedEndPointsTemp.push_back(endXPoints_LRM_BH_XY[1][0][0]);
    unidentifiedEndPointsTemp.push_back(endXPoints_LRM_BH_XY[1][0][1]);
    unidentifiedEndPointsTemp.push_back(endXPoints_LRM_BH_XY[1][1][0]);
    unidentifiedEndPointsTemp.push_back(endXPoints_LRM_BH_XY[1][1][1]);

    // Right
    this->UnidentifiedMarkersEndPoints.push_back(unidentifiedEndPointsTemp);
    delete v;
  }
  return ;
  delete unidentifiedVectorsColl;
}

//----------------------------------------------------------------------------
void MicronTrackerInterface::mtGetTranslations(std::vector<double>& vTranslations, int markerIndex)
{
  vTranslations = this->Translations2D[markerIndex];
}

//----------------------------------------------------------------------------
void MicronTrackerInterface::mtGetRotations(std::vector<double>& vRotations, int markerIndex)
{
  vRotations = this->Rotations2D[markerIndex];
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetStatus()
{
  return this->MarkerStatus;
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
  for (int i = 0; i < this->NumOfIdentifiedMarkers; i++)
  {
    if (markerName == this->IdentifiedMarkersName[i])
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
    this->CurrentCamera = this->CameraList->getCamera(index);
  }
  if (this->CurrentCamera != NULL)
  {
    this->CurrentCameraIndex = index;
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
  return this->CurrentCameraIndex;
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtSetShutterPreference(double n)
{
  return mtOK;// this->m_pCameras->setShutterPreference(n);
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetShutterPreference()
{
  //return this->m_pCameras->getShutterPreference(); The MTC.h
  return 0.0;
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtSetTemplMatchTolerance(double matchTolerance)
{
  return this->MarkerList->setTemplateMatchToleranceMM(matchTolerance);
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetTemplMatchTolerance()
{
  return this->MarkerList->getTemplateMatchToleranceMM();
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
int MicronTrackerInterface::mtSetPredictiveFramesInterleave(int predictiveInterleave)
{
  return this->MarkerList->setPredictiveFramesInterleave(predictiveInterleave);
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetPredictiveFramesInterleave()
{
  return this->MarkerList->getPredictiveFramesInterleave();
}

//----------------------------------------------------------------------------
void MicronTrackerInterface::mtSetPredictiveTracking(bool predTracking)
{
  //m_pMarkerTempls->setPredictiveTracking(predTracking);
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetPredictiveTracking()
{
  return mtInternalMTError;//m_pMarkerTempls->getPredictiveTracking();
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
int MicronTrackerInterface::mtSetShutterTime(double n, int index)
{
  if (index == -1)
  {
    return this->CurrentCamera->setShutterTime(n);
  }
  else if (!this->checkCamIndex(index))
  {
    return mtCameraNotFound;
  }

  else
  {
    return this->CameraList->getCamera(index)->setShutterTime(n);
  }
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetShutterTime(int index)
{
  if (index == -1)
  {
    return this->CurrentCamera->getShutterTime();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1.0;
  }
  else
  {
    return this->CameraList->getCamera(index)->getShutterTime();
  }
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetMinShutterTime(int index)
{
  if (index == -1)
  {
    return this->CurrentCamera->getMinShutterTime();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1.0;
  }
  else
  {
    return this->CameraList->getCamera(index)->getMinShutterTime();
  }
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetMaxShutterTime(int index)
{
  if (index == -1)
  {
    return this->CurrentCamera->getMaxShutterTime();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1.0;
  }

  else
  {
    return this->CameraList->getCamera(index)->getMaxShutterTime();
  }
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetGain(int index)
{
  if (index == -1)
  {
    return this->CurrentCamera->getGain();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1.0;
  }
  else
  {
    return this->CameraList->getCamera(index)->getGain();
  }
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtSetGain(double n, int index)
{
  if (index == -1)
  {
    return this->CurrentCamera->setGain(n);
  }
  else if (!this->checkCamIndex(index))
  {
    return mtCameraNotFound;
  }
  else
  {
    return this->CameraList->getCamera(index)->setGain(n);
  }
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetMinGain(int index)
{
  if (index == -1)
  {
    return this->CurrentCamera->getMinGain();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1.0;
  }
  else
  {
    return this->CameraList->getCamera(index)->getMinGain();
  }
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetMaxGain(int index)
{
  if (index == -1)
  {
    return this->CurrentCamera->getMaxGain();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1.0;
  }
  else
  {
    return this->CameraList->getCamera(index)->getMaxGain();
  }
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetDBGain(int index)
{
  if (index == -1)
  {
    return this->CurrentCamera->getDBGain();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1.0;
  }
  else
  {
    return this->CameraList->getCamera(index)->getDBGain();
  }
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetExposure(int index)
{
  if (index == -1)
  {
    return this->CurrentCamera->getExposure();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1.0;
  }
  else
  {
    return this->CameraList->getCamera(index)->getExposure();
  }
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtSetExposure(double n, int index)
{
  if (index == -1)
  {
    return this->CurrentCamera->setExposure(n);
  }
  else if (!this->checkCamIndex(index))
  {
    return mtCameraNotFound;
  }
  else
  {
    return this->CameraList->getCamera(index)->setExposure(n);
  }
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetMinExposure(int index)
{
  if (index == -1)
  {
    return this->CurrentCamera->getMinExposure();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1.0;
  }
  else
  {
    return this->CameraList->getCamera(index)->getMinExposure();
  }
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetMaxExposure(int index)
{
  if (index == -1)
  {
    return this->CurrentCamera->getMaxExposure();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1.0;
  }
  else
  {
    return this->CameraList->getCamera(index)->getMaxExposure();
  }
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetLightCoolness(int index)
{
  if (index == -1)
  {
    return this->CurrentCamera->getLightCoolness();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1.0;
  }
  else
  {
    return this->CameraList->getCamera(index)->getLightCoolness();
  }
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtSetCamAutoExposure(int n, int index)
{
  if (index == -1)
  {
    return this->CurrentCamera->setAutoExposure(n);
  }
  else if (!this->checkCamIndex(index))
  {
    return mtCameraNotFound;
  }
  else
  {
    return this->CameraList->getCamera(index)->setAutoExposure(n);
  }
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetCamAutoExposure(int index)
{
  if (index == -1)
  {
    return this->CurrentCamera->getAutoExposure();
  }
  else
  {
    return this->CameraList->getCamera(index)->getAutoExposure();
  }
}

//----------------------------------------------------------------------------
double MicronTrackerInterface::mtGetLatestFrameTime(int index)
{
  if (index == -1)
  {
    return this->CurrentCamera->getFrameTime();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1.0;
  }
  else
  {
    return this->CameraList->getCamera(index)->getFrameTime();
  }
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetNumOfFramesGrabbed(int index)
{
  if (index == -1)
  {
    return this->CurrentCamera->getNumOfFramesGrabbed();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1;
  }
  else
  {
    return this->CameraList->getCamera(index)->getNumOfFramesGrabbed();
  }
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetBitsPerPixel(int index)
{
  if (index == -1)
  {
    return this->CurrentCamera->getBitsPerPixel();
  }
  else if (!this->checkCamIndex(index))
  {
    return -1;
  }
  else
  {
    return this->CameraList->getCamera(index)->getBitsPerPixel();
  }
  return 0;
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetLatestFramePixHistogram(long*& aPixHist, int subSampleRate, int index)
{
  return mtInternalMTError;//m_pCurrCams->getLatestFramePixHistogram(aPixHist, subSampleRate, index);
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetLatestFrameHazard()
{
  return Camera_LastFrameThermalHazard(this->CurrentCamera->getHandle());
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtSetTemplateName(int index, const std::string& templName)
{
  int result = mtOK;
  // Check the index
  if (index < 0 || index > this->MarkerList->getTemplateCount())
  {
    logError(-1, "Marker index out of range");
    return mtInvalidIndex;
  }
  std::string oldN = this->mtGetTemplateName(index);
  this->MarkerList->setTemplateItemName(index, templName);
  this->MarkerList->storeTemplate(index, this->Settings->getHandle(), NULL);
  result = this->renameFile(oldN, templName, "Markers");
  if (result != mtOK)
  {
    logError(result, "Changing the name of the template failed");
  }
  return result;
}

//----------------------------------------------------------------------------
std::string MicronTrackerInterface::mtGetTemplateName(int index)
{
  std::string s;
  this->MarkerList->getTemplateItemName(index, s);
  return s;
}

//----------------------------------------------------------------------------
char* MicronTrackerInterface::mtGetIdentifiedTemplateName(int index)
{
  // Check the index
  if (index > -1 && index < this->NumOfIdentifiedMarkers)
  {
    return (char*)this->IdentifiedMarkersName[index].c_str();
  }
  else
  {
    return "";
  }
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtDeleteTemplate(int index)
{
  int result = mtOK;
  if (index > -1 && index < this->MarkerList->getTemplateCount())
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
  return result;
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetLeftRightImageArray(unsigned char**& leftImageArray, unsigned char**& rightImageArray, int index)
{
  int result = mtOK;
  if (index == -1)
  {
    result = this->CurrentCamera->getImages(&leftImageArray, &rightImageArray);
  }
  else if (!this->checkCamIndex(index))
  {
    result = false;
  }
  else
  {
    result = this->CameraList->getCamera(index)->getImages(&leftImageArray, &rightImageArray);
  }

  return result;
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::mtGetLeftRightImageArrayHalfSize(unsigned char**& leftImageArray,
    unsigned char**& rightImageArray,
    int xResolution, int yResolution, int index)
{
  int result = mtOK;
  if (index == -1)
  {
    result = this->CurrentCamera->getHalfSizeImages(&leftImageArray, &rightImageArray, xResolution, yResolution);
  }
  else if (!this->checkCamIndex(index))
  {
    result = false;
  }
  else
  {
    result = this->CameraList->getCamera(index)->getHalfSizeImages(&rightImageArray, &leftImageArray, xResolution, yResolution);
  }
  return result;
}

//----------------------------------------------------------------------------
void MicronTrackerInterface::mtGetIdentifiedMarkersXPoints(double*& xPoints, int markerIndex)
{
  //Check the marker index
  if (markerIndex >= this->NumOfIdentifiedMarkers - 1)
  {
    markerIndex = this->NumOfIdentifiedMarkers - 1;
  }
  if (markerIndex < 0)
  {
    markerIndex = 0;
  }
  xPoints = &this->IdentifiedMarkersXPoints[markerIndex][0];
}

//----------------------------------------------------------------------------
void MicronTrackerInterface::mtGetUnidentifiedMarkersEnds(double*& endPoints, int vectorIndex)
{
  //Check the vector index
  if (vectorIndex >= this->NumOfUnidentifiedMarkers - 1)
  {
    vectorIndex = this->NumOfUnidentifiedMarkers - 1;
  }
  if (vectorIndex < 0)
  {
    vectorIndex = 0;
  }
  endPoints = &this->UnidentifiedMarkersEndPoints[vectorIndex][0];
}

//----------------------------------------------------------------------------
int MicronTrackerInterface::removeFile(const std::string& fileName, const std::string& dir)
{
  std::string fullPath = dir + "/" + fileName;
  int result = remove(fullPath.c_str());
  if (result != 0)
  {
    // failed
    return result;
  }
  return mtOK;
}

//-------------------------------------------------------------------------------
int MicronTrackerInterface::renameFile(const std::string& oldName, const std::string& newName, const std::string& dir)
{
  std::string fullPathOld = dir + "/" + oldName;
  std::string fullPathNew = dir + "/" + newName;

  int result = rename(fullPathOld.c_str(), fullPathNew.c_str());
  if (result != 0)
  {
    // failed
    return result;
  }
  return mtOK;
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
  msg << " (" << ConvertReturnToString(errorNum) << ")";

  if (description)
  {
    msg << ": " << description;
  }
  msg << std::ends;

  this->ErrorString = msg.str();

  LOG_ERROR(this->ErrorString.c_str());
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
