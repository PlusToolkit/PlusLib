/*****************************************************************************
* Shared data structures and function definitions for MicronTracker classes
*
* Copyrights (c)2001-2004 by Claron Technology Inc.
******************************************************************************/
/** \file
\brief Header file for client-side MTC data structures and function declarations.

The classes and functions are documented separately in their source file.
*/

#ifndef MTC_H
#define MTC_H


#ifdef _WIN32
  #include  <windows.h>
  #include  <winbase.h>
  #define EXT 
  #define CCONV  _stdcall
#else
  #define EXT extern
  #define CCONV  
#ifndef BYTE
  typedef unsigned char BYTE;
#endif
#ifndef UINT
  typedef unsigned int UINT;
#endif
#ifndef ULONG
  typedef unsigned long ULONG;
#endif
#ifndef LONG
  typedef long LONG;
#endif
  #define TRUE 1
  #define FALSE 0
  #define VOID void
#endif

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */

/* MTC Version */
#define MTCMajorVersion 2
#define MTCMinorVersion 1
#define MTCRevision 0
#define MTCVERSION_STR "2, 1, 0, 0"

/*! Any handle to an MT object
*/
typedef int mtHandle; 

/*! \name Limits*/
/*\{*/
const int MaxCameras = 10; /*!< Maximum number of cameras that can be concurrently attached. */
const int MaxXPointsPerImage = 6000; /*!< Maximum number of XPoints concurrently detected and processed in each image. */
const int MaxVectorsPerFrame = 200; /*!< Maximum number of vectors concurrently detected and processed.  */
#define MT_MAX_STRING_LENGTH    400  /*!< Maximum number of characters in a single string buffer passed in/out in calls. */

/*\}*/

/*! \name Enums and constants*/
/*\{*/
/*! Values returned from most MT function calls
  \sa MTLastErrorString()
*/
enum mtCompletionCode 
{  mtOK=0, /*!< OK */
  mtInvalidHandle, /*!< Invalid object handle */
  mtReentrantAccess, /*!< Reentrant access - library is not thread-safe */
  mtInternalMTError, /*!< Internal MicronTracker software error */
  mtNullPointer, /*!< Null pointer parameter */
  mtOutOfMemory,  /*!< Out of memory*/
  mtParameterOutOfRange,  /*!< Parameter out of range*/
  mtStringParamTooLong,  /*!< String paramtere too long*/
  mtOutBufferTooSmall,  /*!< Insufficient space allocated by the client to the output buffer*/
  mtCameraNotInitialized,  /*!< Camera not initialized*/
  mtCameraAlreadyInitialized,  /*!< Camera already initialized - cannot be initialized twice*/
  mtCameraInitializationFailed,  /*!< Camera initialization failed*/
  mtCompatibilityError,  /*!< MTC is incompatible with a software module it calls*/
  mtDataNotAvailable, /*< Data requested is not available */
  mtUnrecognizedCameraModel,  /*!< Calibration file error: unrecognized camera model*/
  mtPathNotSet,  /*!< Path not set*/
  mtCannotAccessDirectory,  /*!< Cannot access the directory specified*/
  mtWriteToFileFailed,  /*!< Write to file failed*/
  mtInvalidIndex,  /*!< Invalid Index parameter*/
  mtInvalidSideI,  /*!< Invalid SideI parameter*/
  mtInvalidDivisor,  /*!< Invalid Divisor parameter*/
  mtEmptyCollection,  /*!< Attempting to access an item of an empty IntCollection*/
  mtInsufficientSamples,  /*!< Insufficient samples*/
  mtInsufficientSamplesWithinTolerance,   /*!< Insufficient samples that fit within the acceptance tolerance*/
  mtOddNumberOfSamples,  /*!< Odd number of vector samples*/
  mtLessThan2Vectors,  /*!< Less than 2 vectors*/
  mtMoreThanMaxVectorsPerFacet,  /*!< More than maximum vectors per facet*/
  mtErrorExceedsTolerance,  /*!< Error exceeds tolerance*/
  mtInsufficientAngleBetweenVectors,  /*!< Insufficient angle between vectors*/
  mtFirstVectorShorterThanSecond,  /*!< First vector is shorter than the second*/
  mtVectorLengthsTooSimilar,  /*!< Vector lengths are too similar*/
  mtNullTemplateVector,  /*!< Template vector has 0 length*/
  mtTemplateNotSet,  /*!< The template has not been created or loaded*/
  mtCorruptTemplateFile,  /*!< Template file is corrupt*/
  mtMaxMarkerTemplatesExceeded,  /*!< Maximum number of marker templates allowed exceeded*/
  mtDifferentFacetsGeometryTooSimilar,  /*!< Geometries of different facets are too similar*/
  mtNoncompliantFacetDefinition,  /*!< Noncompliant facet definition*/
  mtCollectionContainsNonVectorHandles,  /*!< The SampledVectorPairsCollection contains non-Vector handles*/
  mtEmptyBuffer,  /*!< Empty pixels buffer*/
  mtDimensionsDoNotMatch,  /*!< Dimensions do not match*/
  mtOpenFileFailed,  /*!< File open failed*/
  mtReadFileFailed,  /*!< File read failed*/
  mtCannotOpenCalibrationFile,  /*!< Cannot open calibration file (typically named [driver]_[ser num].calib */
  mtNotACalibrationFile,  /*!< Not a calibration file*/
  mtCalibrationFileCorrupt,  /*!< Calibration file contents corrupt*/
  mtCalibrationFileDoesNotMatchCamera,  /*!< Calibration file was not generated from this camera*/
  mtCalibrationFileNotLoaded,  /*!< Calibration file not loaded*/
  mtIncorrectFileVersion,  /*!< Incorrect file version*/
  mtLocationOutOfMeasurementBounds, /*!< Input image location is out of bounds of the measurement volume*/
  mtCannotTriangulate, /*!< Input image locations do not triangulate to a valid 3-D point*/
  mtUnknownXform, /*!< Transform between coordinate spaces is unknown*/
  mtCameraNotFound, /*!< The given camera object was not found in the cameras array*/
  mtFeatureDataUnavailable, /*!< Feature Data unavailable for the current frame*/
  mtFeatureDataCorrupt, /*!< Feature Data is corrupt or incompatible with the current version*/
  mtXYZOutOfFOV, /*!< XYZ position is outside of calibrated field of view*/
  mtGrabFrameError  /*!< Grab frame error*/
};

/*! Measurement hazards.
  \sa Xform3D_HazardCodeGet()
*/
enum mtMeasurementHazardCode { //In increasing level of "danger"
  mtNone=0,  /*!< No hazard - safe measurement */
  mtShadowOverXP, /*!< Part of an XPoint is shadowed, possibly comproming accuracy */
  mtCameraBelowMinimumOperatingTemperature, /*!< The internal camera temperature is below operating range, comproming accuracy */
  mtCameraAboveMaximumOperatingTemperature, /*!< The internal camera temperature is above operating range, comproming accuracy */
  mtRapidInternalTemperatureChange, /*!< The camera is undergoing a rapid temperature change, compromising accuracy */
  mtOutsideCalibratedVolume,  /*!< The marker is outside calibrated volume, ie, too far from the camera */
  mtCameraWarmingUp, /*!< The camera is not yet thermally stable enough to provide accurate measurements */
};

/*! LightCoolness constants
  \sa Camera_LightCoolnessSet(), Cameras_LightCoolnessSet()
*/
const double mtDaylightCFL_7000K = 1.0; /*!< Daylight type compact fluorescent bulb (bluish-colored light) */
const double mtSunlight = 0.82;  /*!< Most indirect sunlight */
const double mtDaylightFluorescentTube_6500K = 0.8;   /*!< Daylight type tube */
const double mtCoolWhiteFluorescentTube_4100K = 0.56;  /*!< Cool-white type tube */
const double mtSoftWhiteFluorescentTube_3000K = 0.27;   /*!< Soft-white type tube */
const double mtSoftWhiteCFL_2700K = 0.16; /*!< Soft-white (common) compact fluorescent bulb */
const double mtHalogen = 0.02;  /*!< unfiltered halogen bulb */
const double mtIncandescent = 0; /*!< All types of filmaent-generated light */

/*! Class index for live object counters accessed via MTObjectsAlive()
  \sa MTObjectsAlive()
*/
enum mtObjType {
  mtCollection=0,    /*!< ;*/
  mtFacet,      /*!< ;*/
  mtPersistence,    /*!< ;*/
  mtStopWatch,    /*!< ;*/
  mtVector,      /*!< ;*/
  mtXform3D,      /*!< ;*/
  mtMarker,      /*!< ;*/
  mtCamera,      /*!< ;*/
  ObjTypesCount
};

/*\}*/

/***********************************************************/
/** \name General */
/***********************************************************/
/*\{*/

EXT double CCONV MTTimeSecs();
EXT  void CCONV ResetMTTime();
EXT int CCONV MTObjectsAlive(mtObjType MTClass);
EXT void CCONV MTVersionGet(int *p_outMajor, int *p_outMinor, int *p_outRevision);
EXT  mtCompletionCode CCONV MTLastError();
EXT char* CCONV MTLastErrorString();
EXT  char* CCONV MTHazardCodeString(mtMeasurementHazardCode Code);
EXT  int CCONV MTCopyString2Client(char *p_string, char *p_ClientMessageBuffer, int MessageBufferBytes);
/*\}*/

/***********************************************************/
/** \name Collection */
/***********************************************************/
EXT mtHandle CCONV Collection_New();
EXT mtCompletionCode CCONV Collection_Free(mtHandle handle);
EXT mtCompletionCode CCONV Collection_Add(mtHandle handle, int intVal, double doubleVal);
EXT mtCompletionCode CCONV Collection_Remove(mtHandle handle, int index);
EXT mtCompletionCode CCONV Collection_Clear(mtHandle handle);
EXT int CCONV Collection_Count(mtHandle handle);
EXT mtCompletionCode CCONV Collection_Item(mtHandle handle, int index, 
               int *p_outIntVal, double *p_outDoubleVal);
EXT mtCompletionCode CCONV Collection_LastItem(mtHandle handle, 
               int *p_outIntVal, double *p_outDoubleVal);
EXT int CCONV Collection_Int(mtHandle handle, int index);
EXT double CCONV Collection_Double(mtHandle handle, int index);
/*\}*/

/***********************************************************/
/** \name Persistence */
/***********************************************************/
EXT mtHandle CCONV Persistence_New();
EXT mtCompletionCode CCONV Persistence_Free(mtHandle handle);
EXT mtCompletionCode CCONV Persistence_PathSet(mtHandle handle, char *PathString);
EXT  mtCompletionCode CCONV Persistence_PathGet(mtHandle handle, 
            char *p_outBuffer, UINT BytesAllocated, int *p_outStrLen);
EXT mtCompletionCode CCONV Persistence_SectionSet(mtHandle handle, char *SectionString);
EXT mtCompletionCode CCONV Persistence_SectionGet(mtHandle handle,
            char *p_outBuffer, UINT BytesAllocated, int *p_outStrLen);
EXT mtCompletionCode CCONV Persistence_SaveString(mtHandle handle, char *Name, char *Value);
EXT mtCompletionCode CCONV Persistence_RetrieveString(mtHandle handle, char *Name, char *p_DefaultValue, 
            char *p_outBuffer, int BytesAllocated, int *p_outStrLen);
EXT mtCompletionCode CCONV Persistence_SaveDouble(mtHandle handle, char *Name, double Value);
EXT mtCompletionCode CCONV Persistence_RetrieveDouble(mtHandle handle, char *Name, 
            double DefaultValue, double *p_outValue);
EXT mtCompletionCode CCONV Persistence_SaveInt(mtHandle handle, char *Name, int Value);
mtCompletionCode CCONV Persistence_RetrieveInt(mtHandle handle, char* Name,
            int DefaultValue, int *p_outValue);

/*\}*/

/***********************************************************/
/** \name StopWatch */
/***********************************************************/
EXT mtHandle CCONV StopWatch_New();
EXT mtCompletionCode CCONV StopWatch_Free(mtHandle handle);
EXT mtCompletionCode CCONV StopWatch_ResetAndGo(mtHandle handle);
EXT mtCompletionCode CCONV StopWatch_ResetAndPause(mtHandle handle);
EXT mtCompletionCode CCONV StopWatch_Pause(mtHandle handle);
EXT mtCompletionCode CCONV StopWatch_Go(mtHandle handle);
EXT mtCompletionCode CCONV StopWatch_ElapsedMicroSecsGet(mtHandle handle, double *p_out);
EXT mtCompletionCode CCONV StopWatch_ElapsedMilliSecsGet(mtHandle handle, double *p_out);
EXT mtCompletionCode CCONV StopWatch_ElapsedSecsGet(mtHandle handle, double *p_out);
/*\}*/

/***********************************************************/
/** \name Xform3D */
/***********************************************************/
EXT mtHandle CCONV Xform3D_New();
EXT mtHandle CCONV Xform3D_NewAs(mtHandle orgHandle);
EXT mtCompletionCode CCONV Xform3D_SetStateAs(mtHandle handle, mtHandle SourceHandle);
EXT mtCompletionCode CCONV Xform3D_Free(mtHandle handle);
EXT mtCompletionCode CCONV Xform3D_RotMatGet(mtHandle handle, double *p_out3x3);
EXT mtCompletionCode CCONV Xform3D_RotMatSet(mtHandle handle, double *p_newval3x3);
EXT mtCompletionCode CCONV Xform3D_RotQuaternionsSet(mtHandle handle, double *p_4Quaternions);
EXT mtCompletionCode CCONV Xform3D_RotQuaternionsGet(mtHandle handle, double *p_out4Quaternions);
EXT mtCompletionCode CCONV Xform3D_RotAnglesRadsSet(mtHandle handle, double RadsAroundX, double RadsAroundY, double RadsAroundZ);
EXT mtCompletionCode CCONV Xform3D_RotAnglesDegsSet(mtHandle handle, double DegsAroundX, double DegsAroundY, double DegsAroundZ);
EXT mtCompletionCode CCONV Xform3D_RotAnglesRadsGet(mtHandle handle, double *p_outRadsAroundX, double *p_outRadsAroundY, double *p_outRadsAroundZ);
EXT mtCompletionCode CCONV Xform3D_RotAnglesDegsGet(mtHandle handle, double *p_outDegsAroundX, double *p_outDegsAroundY, double *p_outDegsAroundZ);
EXT mtCompletionCode CCONV Xform3D_NormalizeRotationMatrix(mtHandle handle);
EXT mtCompletionCode CCONV Xform3D_ShiftGet(mtHandle handle, double *p_out3);
EXT mtCompletionCode CCONV Xform3D_ShiftSet(mtHandle handle, double *newval3);
EXT mtCompletionCode CCONV Xform3D_UnitScaleGet(mtHandle handle, double *p_out);
EXT mtCompletionCode CCONV Xform3D_UnitScaleSet(mtHandle handle, double newval);
EXT mtCompletionCode CCONV Xform3D_MTTimeStampSecsGet(mtHandle handle, double *p_out);
EXT mtCompletionCode CCONV Xform3D_MTTimeStampSecsSet(mtHandle handle, double newval);
EXT mtCompletionCode CCONV Xform3D_HazardCodeGet(mtHandle handle, mtMeasurementHazardCode *p_out);
EXT  mtCompletionCode CCONV Xform3D_IsIdentityGet(mtHandle handle, bool *p_out);
EXT mtCompletionCode CCONV Xform3D_XformLocation(mtHandle handle, double *p_SourceV3, double *p_outTargetV3);
EXT mtCompletionCode CCONV Xform3D_RotateLocation(mtHandle handle, double *p_SourceV3, double *p_outTargetV3, bool ScaleIt);
EXT mtCompletionCode CCONV Xform3D_Translate(mtHandle handle, double *p_TranslationV3, bool Subtract);
EXT mtCompletionCode CCONV Xform3D_Concatenate(mtHandle handle, mtHandle SubsequentXform3DHandle, mtHandle ResultHandle);
EXT mtCompletionCode CCONV Xform3D_Inverse(mtHandle handle, mtHandle ResultHandle);
EXT mtCompletionCode CCONV Xform3D_InBetween(mtHandle handle, mtHandle OtherHandle, double OtherFract0to1, int ResultHandle);
EXT mtCompletionCode CCONV Xform3D_AngularDifference(mtHandle handle, mtHandle OtherHandle, 
              double *pout_AngleRadians, double *pout_AxisUnitVector3);
EXT mtCompletionCode CCONV Xform3D_Store(mtHandle handle, mtHandle PersistenceHandle, char *SectionName);
EXT mtCompletionCode CCONV Xform3D_Restore(mtHandle handle, mtHandle PersistenceHandle, char *SectionName);
/*\}*/

/***********************************************************/
/** \name Vector */
/***********************************************************/
EXT mtHandle CCONV Vector_New();
EXT mtHandle CCONV Vector_NewAs(mtHandle SourceHandle);
EXT mtCompletionCode CCONV Vector_SetStateAs(mtHandle handle, mtHandle SourceHandle);
EXT mtCompletionCode CCONV Vector_Free(mtHandle handle);
EXT mtCompletionCode CCONV Vector_EndPosGet(mtHandle handle, double *p_out2x3);
EXT mtCompletionCode CCONV Vector_EndXPointsGet(mtHandle handle, double *p_out2x2x2);
EXT mtCompletionCode CCONV Vector_IdentifyingCameraHandleGet(mtHandle handle, int *p_out);
EXT mtCompletionCode CCONV Vector_PixelValueRangeGet(mtHandle handle, short *p_outMinLevel, short *p_outMaxLevel);
EXT mtCompletionCode CCONV Vector_LengthGet(mtHandle handle, double *p_out);
EXT mtCompletionCode CCONV Vector_UnitVGet(mtHandle handle, double *p_out3);
EXT mtCompletionCode CCONV Vector_Store(mtHandle handle, int PersistenceHandle, char *SectionName);
EXT mtCompletionCode CCONV Vector_Restore(mtHandle handle, int PersistenceHandle, char *SectionName);
EXT mtCompletionCode CCONV Vector_MaxPosDist(mtHandle handle, mtHandle OtherVectorHandle, double *p_out);
EXT  mtCompletionCode CCONV Vector_Xform(mtHandle handle, mtHandle Xform3DHandle);
/*\}*/

/***********************************************************/
/** \name Facet */
/***********************************************************/
EXT mtHandle CCONV Facet_New();
EXT mtHandle CCONV Facet_NewAs(mtHandle orgHandle);
EXT mtCompletionCode CCONV Facet_Free(mtHandle handle);
EXT mtCompletionCode CCONV Facet_TemplateVectorsGet(mtHandle handle, int LongVectorHandle, int ShortVectorHandle);
EXT mtCompletionCode CCONV Facet_TemplateVectorsSet(mtHandle handle, int LongVectorHandle, int ShortVectorHandle);
EXT  mtCompletionCode CCONV Facet_TemplatePositionsGet(mtHandle handle, double *p_outPositions4x3);
EXT mtCompletionCode CCONV Facet_IdentifiedVectorsGet(mtHandle handle, int LongVectorHandle, int ShortVectorHandle);
EXT  mtCompletionCode CCONV Facet_MeasuredPositionsGet(mtHandle handle, double *p_outPositions4x3);
EXT mtCompletionCode CCONV Facet_IdentifiedMatchRMSEGet(mtHandle handle, int CameraHandle, double *p_out);
EXT mtCompletionCode CCONV Facet_WasIdentifiedGet(mtHandle handle, int CameraHandle, bool *p_out);
EXT mtCompletionCode CCONV Facet_IdentifiedXPointsGet(mtHandle handle, mtHandle CameraHandle, double *p_outXPCoords2x2x2x2);
EXT mtCompletionCode CCONV Facet_Facet2CameraXfGet(mtHandle handle, mtHandle CameraHandle, mtHandle ResultXfHandle);
EXT mtCompletionCode CCONV Facet_Identify(mtHandle handle, int CameraHandle,  int *VectorsHandles, int VectorsNum, bool *p_out);
EXT mtCompletionCode CCONV Facet_ValidateTemplateVectors(mtHandle handle);
EXT mtCompletionCode CCONV Facet_SetTemplateVectorsFromSamples(mtHandle handle, int SampledVectorPairsCollection, double MaxSampleErrorAllowedMM);
EXT mtCompletionCode CCONV Facet_StoreTemplate(mtHandle handle, int PersistenceHandle, char *SectionName);
EXT mtCompletionCode CCONV Facet_RestoreTemplate(mtHandle handle, int PersistenceHandle, char *SectionName);
/*\}*/

/***********************************************************/
/** \name Marker */
/***********************************************************/
EXT mtHandle CCONV Marker_New();
EXT mtHandle CCONV Marker_NewAs( int orgHandle);
EXT mtCompletionCode CCONV Marker_Free(mtHandle handle);

EXT mtCompletionCode CCONV Marker_WasIdentifiedGet(mtHandle handle, mtHandle CameraHandle, bool *p_out);
EXT  mtCompletionCode CCONV Marker_Marker2CameraXfGet(mtHandle handle, mtHandle CameraHandle, mtHandle outXfHandle, mtHandle *p_outIdentifyingCameraHandle);
EXT  mtCompletionCode CCONV Marker_Tooltip2MarkerXfGet(mtHandle handle, mtHandle XfHandle);
EXT  mtCompletionCode CCONV Marker_Tooltip2MarkerXfSet(mtHandle handle, mtHandle XfHandle);
EXT mtCompletionCode CCONV Marker_IdentifiedFacetsGet(mtHandle handle, mtHandle CameraHandle, bool copy, mtHandle outCollectionHandle);
EXT mtCompletionCode CCONV Marker_NameSet(mtHandle handle, char *pName);
EXT mtCompletionCode CCONV Marker_NameGet(mtHandle handle, char *p_outNameBuffer, int NameBufferChars, int *p_outActualChars);
EXT mtCompletionCode CCONV Marker_TemplateFacetsGet(mtHandle handle, mtHandle *FacetsCollHandle);
EXT  mtCompletionCode CCONV Marker_FacetIndexGet(mtHandle handle, mtHandle FacetHandle, int *p_out1based);

EXT mtCompletionCode CCONV Marker_AddTemplateFacet(mtHandle handle, mtHandle NewFacetHandle, mtHandle Facet1ToNewFacetXfHandle );
EXT mtCompletionCode CCONV Marker_RemoveTemplateFacet(mtHandle handle, mtHandle FacetToRemoveHandle);
EXT mtCompletionCode CCONV Marker_ResetIdentifiedFacets(mtHandle handle, mtHandle CamHandle);
EXT  mtCompletionCode CCONV Marker_Identify(mtHandle handle, mtHandle CameraHandle, mtHandle VectorsHandles[], int VectorsNum, mtHandle outCollectionHandle);
EXT mtCompletionCode CCONV Marker_ValidateTemplate(mtHandle handle);
EXT mtCompletionCode CCONV Marker_RestoreTemplate(mtHandle handle,  mtHandle PersistenceHandle, char *NameInP);
EXT mtCompletionCode CCONV Marker_StoreTemplate(mtHandle handle, mtHandle PersistenceHandle, char *NameInP);
/*\}*/

/***********************************************************/
/** \name Markers */
/***********************************************************/
EXT int CCONV Markers_TemplatesCount();
EXT mtCompletionCode CCONV Markers_TemplateItemGet(int index, mtHandle *p_outHandle );
EXT mtCompletionCode CCONV Markers_IdentifiedMarkersGet(mtHandle CameraHandle, mtHandle outIdentifiedCollectionHandle );
EXT mtCompletionCode CCONV Markers_UnidentifiedVectorsGet(mtHandle CameraHandle, mtHandle outUnidentifiedCollectionHandle );
EXT mtCompletionCode CCONV Markers_TemplateMatchToleranceMMGet(double *p_out);
EXT mtCompletionCode CCONV Markers_TemplateMatchToleranceMMDefaultGet(double *p_out);
EXT mtCompletionCode CCONV Markers_TemplateMatchToleranceMMSet(double newval);
EXT mtCompletionCode CCONV Markers_PredictiveFramesInterleaveGet(int *p_out);
EXT mtCompletionCode CCONV Markers_PredictiveFramesInterleaveSet(int newval);
EXT mtCompletionCode CCONV Markers_OverExposureControlInterleaveGet(int *p_out);
EXT mtCompletionCode CCONV Markers_OverExposureControlInterleaveSet(int newval);
EXT mtCompletionCode CCONV Markers_AutoAdjustCameraExposureGet(bool *p_out);
EXT mtCompletionCode CCONV Markers_AutoAdjustCameraExposureSet(bool newval);
EXT void CCONV Markers_JitterFilterEnabledSet(bool newval);
EXT bool CCONV Markers_JitterFilterEnabled();
EXT mtCompletionCode CCONV Markers_AutoAdjustCam2CamRegistrationGet(mtHandle handle, int *p_out);
EXT mtCompletionCode CCONV Markers_AutoAdjustCam2CamRegistrationSet(mtHandle handle, int newval);
EXT mtCompletionCode CCONV Markers_LoadTemplates(char *DirPath);
EXT mtCompletionCode CCONV Markers_ClearTemplates();
EXT mtCompletionCode CCONV Markers_AddTemplate(mtHandle MarkerHandle);
EXT mtCompletionCode CCONV Markers_Validate(char outTextLines[][120], int LinesCount, int *p_outLinesFilled);
EXT mtCompletionCode CCONV Markers_ProcessFrame(mtHandle CameraHandle);
/*\}*/

/***********************************************************/
/** \name Camera */
/***********************************************************/
EXT int CCONV Camera_DriverNameGet(mtHandle handle,  char *p_out, int max_size);
EXT int CCONV Camera_ModelNameGet(mtHandle handle,  char *p_out, int max_size);
EXT mtCompletionCode CCONV Camera_BitsPerPixelGet(mtHandle handle, int *p_out);
EXT mtCompletionCode CCONV Camera_FramesGrabbedGet(mtHandle handle, int *p_out);
EXT mtCompletionCode CCONV Camera_ResolutionGet(mtHandle handle, int *p_outXres, int *p_outYres);
EXT mtCompletionCode CCONV Camera_SerialNumberGet(mtHandle handle, int *p_out);
EXT mtCompletionCode CCONV Camera_OpticalCenterGet(mtHandle handle, double *pLeftX, double *pLeftY, 
                        double *pRightX, double *pRightY);
EXT mtCompletionCode CCONV Camera_FirmwareVersionGet(mtHandle handle, int *major, int *minor, int *type, int *rev);
EXT mtCompletionCode CCONV Camera_LastFrameTemperatureGet(mtHandle handle, double *p_outCelsius);
EXT mtCompletionCode CCONV Camera_LastFrameTemperatureSet(mtHandle handle, double Celsius);
EXT mtCompletionCode CCONV Camera_SecondsFromPowerupGet(mtHandle handle, unsigned long *p_out);
EXT mtCompletionCode CCONV Camera_GainFGet(mtHandle handle, double *p_out);
EXT mtCompletionCode CCONV Camera_GainFSet(mtHandle handle, double newval);
EXT mtCompletionCode CCONV Camera_GainFMinGet(mtHandle handle, double *p_out);
EXT mtCompletionCode CCONV Camera_GainFMaxGet(mtHandle handle, double *p_out);
EXT mtCompletionCode CCONV Camera_GainDBGet(mtHandle handle, double *p_out);
EXT mtCompletionCode CCONV Camera_ShutterMsecsGet(mtHandle handle, double *p_out);
EXT mtCompletionCode CCONV Camera_ShutterMsecsSet(mtHandle handle, double newval);
EXT mtCompletionCode CCONV Camera_ShutterMsecsMinGet(mtHandle handle, double *p_out);
EXT mtCompletionCode CCONV Camera_ShutterMsecsMaxGet(mtHandle handle, double *p_out);
EXT mtCompletionCode CCONV Camera_ExposureMinGet(mtHandle handle, double *p_out);
EXT mtCompletionCode CCONV Camera_ExposureMaxGet(mtHandle handle, double *p_out);
EXT mtCompletionCode CCONV Camera_ExposureGet(mtHandle handle, double *p_out);
EXT mtCompletionCode CCONV Camera_ExposureSet(mtHandle handle, double newval);
EXT mtCompletionCode CCONV Camera_ShutterMsecsLimiterSet(mtHandle handle, double newval);
EXT mtCompletionCode CCONV Camera_ShutterMsecsLimiterGet(mtHandle handle, double *p_out);
EXT mtCompletionCode CCONV Camera_GainFLimiterGet(mtHandle handle, double *p_out);
EXT mtCompletionCode CCONV Camera_GainFLimiterSet(mtHandle handle, double newval);
EXT mtCompletionCode CCONV Camera_AutoExposureGet(mtHandle handle, int *p_out);
EXT mtCompletionCode CCONV Camera_AutoExposureSet(mtHandle handle, int newval);
EXT mtCompletionCode CCONV Camera_LightCoolnessGet(mtHandle handle, double *p_out);
EXT mtCompletionCode CCONV Camera_LightCoolnessSet(mtHandle handle, double newval);
EXT  mtCompletionCode CCONV Camera_LightCoolnessAdjustFromColorVector(mtHandle handle, mtHandle VectorHandle, int ColorProfile);
EXT  mtCompletionCode CCONV Camera_HistogramEqualizeImagesGet(mtHandle handle, bool *p_out);
EXT  mtCompletionCode CCONV Camera_HistogramEqualizeImagesSet(mtHandle handle, bool newval);
EXT mtCompletionCode CCONV Camera_ImagesGet(mtHandle handle, unsigned char *p_LeftImageBuffer, unsigned char *p_RightImageImageBuffer);
EXT mtCompletionCode CCONV Camera_HalfSizeImagesGet(mtHandle handle, unsigned char *LeftImage, unsigned char *RightImage);
EXT mtCompletionCode CCONV Camera_PixelsGet(mtHandle handle, int SideI, int left_x, int top_y, 
            int width, int height, unsigned char *p_outValues_WxH);
EXT mtCompletionCode CCONV Camera_PixelSet(mtHandle handle, int SideI, int x, int y, short NewValue);
EXT mtCompletionCode CCONV Camera_ImagesSet(mtHandle handle, unsigned char *LeftImage,  unsigned char *RightImage);
EXT mtCompletionCode CCONV Camera_FeatureDataCaptureEnabledSet(mtHandle handle, bool newval);
EXT  mtCompletionCode CCONV Camera_FeatureDataCaptureEnabledGet(mtHandle handle, bool *p_out);
EXT  mtCompletionCode CCONV Camera_FeatureDataAccessGet(mtHandle handle, void **p_outFeatureDataAddr, int *p_outSize);
EXT  mtCompletionCode CCONV Camera_FeatureDataSet(mtHandle handle, void *p_FeatureData, int Size);
EXT mtCompletionCode CCONV Camera_LoadCalibration(mtHandle handle,  char *DirectoryPath); //For the current camera, given the directory
EXT mtCompletionCode CCONV Camera_LoadCalibrationFile(mtHandle handle,  char *FilePath, bool IgnoreSerialNumMismatch);  // or specific path
EXT mtCompletionCode CCONV Camera_CalibrationFileInfo(char *CalibrationFilePath, 
            char TextLines[][80], int LinesCount, int *pLinesFilled);
EXT mtCompletionCode CCONV Camera_CalibrationInfo(mtHandle handle, 
            char TextLines[][80], int LinesCount, int *pLinesFilled);
EXT mtCompletionCode CCONV Camera_CalibrationLoadedGet(mtHandle handle, bool *p_out);
EXT  mtCompletionCode CCONV Camera_Triangulate(mtHandle handle, double LeftX, double LeftY, double RightX, double RightY,
                     double *p_outXYZ, double *p_outIntersectionGapMM);
EXT  mtCompletionCode CCONV Camera_ProjectionOnImage(mtHandle handle, int SideI, double XYZ[],
                     double *p_outX, double *p_outY);
EXT mtCompletionCode CCONV Camera_GrabFrame(mtHandle handle);
EXT  mtMeasurementHazardCode CCONV Camera_LastFrameThermalHazard(mtHandle handle);
EXT mtCompletionCode CCONV Camera_FrameMTTimeSecsGet(mtHandle handle, double *p_out);
EXT mtHandle CCONV Camera_New(char *DriverName, int SerialNumber);
EXT mtCompletionCode CCONV Camera_Free(mtHandle handle);
/*\}*/

/***********************************************************/
/** \name Cameras */
/***********************************************************/
EXT mtCompletionCode CCONV Cameras_AttachAvailableCameras (char *CalibrationFilesDirectory );
EXT void CCONV Cameras_Detach();
EXT mtCompletionCode CCONV Cameras_SerialNumbersGet(char *DriverName, mtHandle outCollection);
EXT mtCompletionCode CCONV Cameras_GrabFrame (int Cam );
EXT  mtHandle CCONV Cameras_FailedCamera();
EXT mtCompletionCode CCONV Cameras_Camera2Camera0XfGet (mtHandle CameraHandle, mtHandle outXform3DHandle);
EXT  mtCompletionCode CCONV Cameras_Camera2Camera0XfSet(mtHandle CameraHandle, mtHandle Xform3DHandle);
EXT mtCompletionCode CCONV Cameras_Camera2CameraXfGet (mtHandle FromCamHandle, mtHandle ToCamHandle, mtHandle ResultXform3DHandle);
EXT int CCONV Cameras_CameraIndex(mtHandle CamHandle);
EXT int CCONV Cameras_Count();
EXT int CCONV Cameras_LastI();
EXT mtCompletionCode CCONV Cameras_ItemGet(int index, int *p_out);
EXT mtCompletionCode CCONV Cameras_ShutterMsecsLimiterGet(double *p_out);
EXT mtCompletionCode CCONV Cameras_ShutterMsecsLimiterSet(double newval);
EXT mtCompletionCode CCONV Cameras_GainFLimiterGet(double *p_out);
EXT mtCompletionCode CCONV Cameras_GainFLimiterSet(double newval);
EXT double CCONV Cameras_ShutterMsecsLimiterDefault();
EXT double CCONV Cameras_GainFLimiterDefaultdouble();
EXT double CCONV Cameras_LightCoolness();
EXT mtCompletionCode CCONV Cameras_LightCoolnessSet(double newval);
EXT  mtCompletionCode CCONV Cameras_LightCoolnessSet(double newval);
EXT  mtCompletionCode CCONV Cameras_HistogramEqualizeImagesGet(bool *p_out);
EXT  mtCompletionCode CCONV Cameras_HistogramEqualizeImagesSet(bool newval);
/*\}*/

#  ifdef __cplusplus
}
#endif

#endif // MTC_H
