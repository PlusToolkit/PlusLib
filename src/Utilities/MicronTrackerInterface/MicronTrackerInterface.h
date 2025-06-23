/***************************

  MicronTracker Windows API

  Author: SI - Robarts Research Institute

  Date: June 9, 2003

  Modified: June 20, 2003 This file has been converted to a class to make it more compatible with OOD
  Modified: November 2018 Interface modified to be more consistent

 ***************************/

#ifndef __MICRONTRACKERINTERFACE_H__
#define __MICRONTRACKERINTERFACE_H__

#include <string>
#include <vector>

class Cameras;
class Collection;
class MCamera;
class Marker;
class Markers;
class Persistence;
class Xform3D;

// Any handle to an MT object
#ifdef _WIN64  // Defined for applications for Win64.
  typedef long long mtHandle;
#else
  #ifdef _LINUX64
    typedef long long mtHandle;
  #else
    typedef int mtHandle; // for WIN32
  #endif
#endif

class MicronTrackerInterface
{
protected:
  enum mti_Error
  {
    MTI_FAIL = 0,
    MTI_SUCCESS = 1,
    MTI_CAMERA_NOT_FOUND = 2,
    MTI_GRAB_FRAME_ERROR = 3,
    MTI_MARKER_CAPTURED = 4,
    MTI_NO_MARKER_CAPTURED = 5,
    MTI_CAMERA_INDEX_OUT_OF_RANGE = 6
  };

public:
  MicronTrackerInterface() {};
  ~MicronTrackerInterface();

  //----------------------------
  //
  // Initiation and termination
  //
  //----------------------------


  /** Set some preliminary variables. This function should be called
  before calling any other functions. */
  int mtInit(const std::string& iniFilePath);

  /** Checks whether a status int idicates that an error occured, or rather status is not mtOK*/
  static bool mtIsErrorStatus(int status);

  /** No other function should be called after calling this function unless the mtInit() is called again. */
  void mtEnd();

  //-----------------------------
  //
  // Set up and detach the cameras.
  //
  //------------------------------

  /** Instantiates an object of type CurrentCameras and calls the setupCameras method on
  that object. Checks for the presence of any cameras. */
  int mtSetupCameras();

  /** Detaches the already attached cameras. It is safe to call this method even if
  no camera(s) is attached as it checks the status of the camera(s) first. */
  void mtDetachCameras();

  //------------------------------
  //
  // Cameras' property access (serial number, resolution, number of cameras).
  //
  //------------------------------

  /** Returns the serial number of the camera. If index is -1 it returns
  the serial number of the current camera, otherwise it returns the serial
  number of the index_th camera. If no parameter is passed, the default
  parameter would be -1. The first camera has a index of 0. If index is out
  of camera index range, returns -1. */
  int mtGetSerialNum(int index = -1);

  /** Returns the XResolution of the camera. If index is -1 it returns
  the resolution of the current camera, otherwise it returns the resolution
  of the index_th camera. If no parameter is passed, the default parameter
  would be -1. The first camera has a index of 0. if index is out
  of camera index range, returns -1. */
  int mtGetXResolution(int index = -1);

  /** Returns the YResolution of the camera. If index is -1 it returns
  the resolution of the current camera, otherwise it returns the resolution
  of the index_th camera. If no parameter is passed, the default parameter
  would be -1. The first camera has a index of 0. if index is out
  of camera index range, returns -1. */
  int mtGetYResolution(int index = -1);

  /** Returns the number of sensors of the camera (usually 2). If index is -1 it returns
  the number of sensors of the current camera, otherwise it returns the information
  of the index_th camera. If no parameter is passed, the default parameter
  would be -1. The first camera has a index of 0. if index is out
  of camera index range, returns -1. */

  /** Since there is no longer a library call to get this info, it just returns 2 */
  int mtGetNumOfSensors(int index = -1);

  /** Returns the number of attached cameras.*/
  int mtGetNumOfCameras();

  //--------------------------
  //
  // Set and get the shutter time and preference of the cameras.
  //
  //--------------------------

  /** Sets the shutter preference of the cameras. */
  int mtSetShutterPreference(double n);

  /** Gets the shutter preference of the cameras. */
  double mtGetShutterPreference();

  /** Sets the shutter opening time in the index_th camera. if index is -1 then
  the shutter time of the current camera will be set. If no index is passed the default value is -1.
  When this method is called the AutoExposure in the camera is set to false automatically. */
  int mtSetShutterTime(double n, int index = -1);

  /** Gets the shutter opening time in the index_th camera. If index is -1 then
  the shutter time of the current camera is returned.  If no index is passed the default value is -1.*/
  double mtGetShutterTime(int index = -1);

  /** Gets the minimum shutter opening time in the index_th camera. If index is -1 then
  the minimum shutter time of the current camera is returned.  If no index is passed the default value is -1.
  If not successful returns -1. */
  double mtGetMinShutterTime(int index = -1);

  /** Gets the maximum shutter opening time in the index_th camera. If index is -1 then
  the maximum shutter time of the current camera is returned.  If no index is passed the default value is -1.
  If not successful returns -1. */
  double mtGetMaxShutterTime(int index = -1);

  //------------------------
  //
  // Set and get the gain and exposure of the cameras.
  //
  //------------------------

  // Gets the gain of the index_th camera. If index is -1 then
  // the gain of the current camera is returned.
  // If not successful returns -1.
  double mtGetGain(int index = -1);

  // Sets the gain of the index_th camera. If index is -1 then
  // the gain of the current camera is returned.
  int mtSetGain(double n, int index = -1);

  // Get the minimum gain of the index_th camera. If index is -1 then
  // the minimum gain of the current camera is returned.
  // If not successful returns -1.
  double mtGetMinGain(int index = -1);

  // Get the maximum gain of the index_th camera. If index is -1 then
  // the maximum gain of the current camera is returned.
  // If not successful returns -1.
  double mtGetMaxGain(int index = -1);

  // Get the DB gain of the the index_th camera. If index is -1 then
  // the gain of the current camera is returned.
  // If not successful returns -1.
  double mtGetDBGain(int index = -1);

  // Get the exposure of the the index_th camera. If index is -1 then
  // the exposure of the current camera is returned.
  // If not successful returns -1. */
  double mtGetExposure(int index = -1);

  // Set the exposure of the index_th camera. If index is -1 then
  // the gain of the current camera is returned.
  int mtSetExposure(double n, int index = -1);

  // Get the minimum exposure of the the index_th camera. If index is -1 then
  // the minimum exposure of the current camera is returned.
  // If not successful returns -1.
  double mtGetMinExposure(int index = -1);

  // Get the maximum exposure of the the index_th camera. If index is -1 then
  // the maximum exposure of the current camera is returned.
  // If not successful returns -1.
  double mtGetMaxExposure(int index = -1);

  // Get the color temperature ("light coolness") used by the camera.
  // This will be automatically adjusted if a CoolCard is placed in the camera's field of view. */
  double mtGetLightCoolness(int index = -1);

  //------------------------
  //
  // Set and get the shutter auto-exposure (true of false).
  //
  //------------------------

  // Sets the AutoExposure property of the index_th camera to parameter n. If index is -1 then
  // the autoExposure of the current camera will be set.
  // When set to -1, the exposure is automatically adjusted to maintain a good distribution of
  // grey levels in the image; If set to 0, not.
  int mtSetCamAutoExposure(int n, int index = -1);

  // Gets the AutoExposure property of the index_th camera. If index is -1 then
  // the AutoExposure of the current camera is returned.
  // See also mtSetCamAutoExposure().
  int mtGetCamAutoExposure(int index = -1);

  //------------------------
  //
  // Selection of a camera; Accessing the index of the current camera
  //
  //------------------------

  // Selects the index_th camera.*/
  bool mtSelectCamera(int index);

  // Returns the index of the current camera.
  int mtGetCurrCamIndex();

  //------------------------
  //
  // Grab a frame from the camera and process it.
  //
  //------------------------

  // Grabs the frame of the index_th cameras. If index is -1 then the frame from all
  // the cameras is grabbed.
  int mtGrabFrame(int index = -1);

  // Invokes processing of the most recent grabbed frame. This process involves
  // recognizing the identified vectors
  int mtProcessFrame();

  //------------------------
  //
  // Adding or changing the templates.
  //
  //------------------------

  // Initiates collecting new samples.
  // if successfully detects two new vectors return NewSample_2Vectors.
  // if more than two vectors are detected returns NewSample_MoreThan2Vectors.
  // if less than two vectors are detected returns NewSample_LessThan2Vectors.
  // In the case of adding an additional facet to a known facet:
  // Returns NewSample_NoKnownFacetBeingDetected if no known facet is being detected. */
  enum NewSampleReturnValue
  {
    NewSample_2Vectors,
    NewSample_MoreThan2Vectors,
    NewSample_LessThan2Vectors,
    NewSample_NoKnownFacetBeingDetected
  };
  NewSampleReturnValue mtCollectNewSamples(int collectingAdditionalFacet);

  // Stops collecting the samples from the new marker and creates a new marker with
  // the name of 'templateName' and jitter value of 'jitterValue.
  int mtStopSampling(char* templateName, double jitterValue);

  // Saves the template with name 'templName' in the Marker folder in the
  // current directory. */
  int mtSaveMarkerTemplate(const std::string& templName, const std::string& dir);

  // Deletes the previous samples received.
  void mtResetSamples();

  //------------------------
  //
  // Get the latest frame (or general frame) information.
  //
  //------------------------

  // Returns the time of the latest frame of the index_th camera. If index is -1 then
  // returns the time of the latest frame of the current camera.
  // The time is the time elapsed in msecs since the MicronTracker has started. If not successful, returns -1. */
  double mtGetLatestFrameTime(int index = -1);

  // Gets the histogram of the pixel values of the latest frame of the index_th camera.
  // If index is -1 then the histogram of the pixel values of the current camera will be calculated.
  // The histogram array is copied into the aPixHist array.
  // If subSampleRate is set to values > 1 then the speed of computation increases. */
  int mtGetLatestFramePixHistogram(long*& aPixHist, int subSampleRate, int index = -1);

  // Returns the number of frames grabbed by the index_th camera. Increments by one every time a
  // frame is grabbed. If index is -1 then returns the number of frames grabbed by the default camera.
  // Returns -1 if not successful.*/
  int mtGetNumOfFramesGrabbed(int index = -1);

  // Returns the BitsPerPixel of the index_th camera. If index is -1 then it returns
  // the BitsPerPixel of the current camera.
  // Returns -1 if not successful.
  int mtGetBitsPerPixel(int index = -1);

  // Check for hazard codes on the latest frame (e.g. thermal instability) */
  int mtGetLatestFrameHazard();

  //------------------------
  //
  // Load templates;
  // Set and get the template match tolerance. This is the maximum allowed distance (in mm)
  // between the ends of vectors in each facet template and its matched measure vectors during identification.
  // Get the number of loaded templates.
  //
  //------------------------

  // Loads the marker templates and inserts the result in the passed arguments:
  //  \param tmplsName Holds the name of the templates.
  //  \param tmplsError Holds the error strings (if any) during the templates loading process.
  //  \param tmplsWarn Holds the warning strings (if any) during the templates loading process.
  //  \param templsPath Path of the directory containing the template files.
  int mtRefreshTemplates(std::vector<std::string>& tmplsName, std::vector<std::string>& tmplsError, const std::string& tmplsPath);

  // Sets the match tolerance of the marker templates to \param matchTolerance in mm.
  int mtSetTemplMatchTolerance(double matchTolerance);

  // Gets the match tolerance of the marker templates.
  double mtGetTemplMatchTolerance();

  // Gets the default value for match tolerance of the marker templates.
  double mtGetTemplMatchToleranceDefault();

  //------------------------
  //
  // Set and get some properties of the loaded templates.
  // IMPORTANT NOTE: All the following methods should be called only after
  // the templates have been loaded (i.e. after the mtRefreshTemplates ) has
  // been called by the client.
  //
  //------------------------

  // Get the number of all the loaded templates in the markers list.
  // Important: This method should not be called before calling the mtRefreshTemplates method (see above).
  int mtGetLoadedTemplatesNum();

  // Set the predictive tracking to true or false.
  void mtSetPredictiveTracking(bool predTracking);
  // Get the predictive tracking of the markers.
  int mtGetPredictiveTracking();

  // If set to true, all the camera settings will be updated after each call to
  // the processFrame. If set to false this does not happen.
  void mtSetAdjustCamAfterEveryProcess(bool autoCamExp);

  // See the above description.
  short mtGetAdjustCamAfterEveryProcess();

  // Set the name of the template 'index' in the templates list. The new name is 'templName'.
  int mtSetTemplateName(int index, const std::string& templName);

  // Get the name of the template 'index' in the template list.
  std::string mtGetTemplateName(int index);

  // Get the name of the template 'index' in the list of the identified markers.
  // If index is out of range returns an empty string.
  char* mtGetIdentifiedTemplateName(int index);

  // Delete the template 'index' in the list of the loaded templates.
  int mtDeleteTemplate(int index);

  //------------------------
  //
  // Set and get the predictive frames interleave. This is the number of predictive-only frames
  // between the pairs of comprehensive frames.
  //
  //------------------------

  // Sets the predictiveInterleave.
  int mtSetPredictiveFramesInterleave(int predictiveInterleave);

  // Returns the number of predictiveInterleave. Returns -1 if not successful.
  int mtGetPredictiveFramesInterleave();

  //------------------------
  //
  // Get the markers information (number of markers, number of facets in each marker)
  //
  //------------------------

  // Returns the number of loaded marker templates.
  int mtGetIdentifiedMarkersCount();

  // Inserts the number of identified facets in each marker in the numOfFacets vector. So the
  // first element of this vector would be the number of identified facets in the first marker, the
  // second element of the vector would be the number of identified facets in the second marker and so on.
  int mtGetNumOfFacetsInMarker(int markerIndex);

  // Inserts the number of total facets in each marker in the numOfFacets vector. So the
  // first element of this vector would be the number of total facets in the first marker, the
  // second element of the vector would be the number of total facets in the second marker and so on.
  int mtGetNumOfTotalFacetsInMarker(int markerIndex);

  // Returns the number of unidentified vectors.
  int mtGetUnidentifiedMarkersCount();

  //------------------------
  //
  // Get the rotation and translation matrix of a marker.
  //
  //------------------------

  // Inserts the Rotation Matrix for markerIndex_th marker into \param vRotation.
  // The resulted vector  has nine elements with this order:
  // R11, R12, R13, R21, R22, R23, R31, R32, R33
  // where the first digit after R represents row and the second digit represents column.
  // This rotation matrix represents the center of the long vector of the marker.
  // Markers' index start at 0.
  void mtGetRotations(std::vector<double>& vRotations, int markerIndex);

  // Inserts the Translation (offset) of the markerIndex_th marker into \param vTranslation.
  // This offset  is in fact the offset of the center of the long vector of the marker.
  // Markers' index start at 0.
  void mtGetTranslations(std::vector<double>& vTranslations, int markerIndex);

  //------------------------
  //
  // Get the status of the tracked tool
  //
  //------------------------
  int mtGetStatus();

  // Finds the status of the loaded marker with the index of \param loadedMarkerIndex. The status of the marker
  // can be one of the following two values:
  // 1. If the marker is identified in the current camera frame the value will be mti_utils::MTI_MARKER_CAPTURED.
  // 2. If the makrer is not identified in the current camera frame the value will be mti_utils::MTI_NO_MARKER_CAPTURED.
  // If the requested marker if found in the current camera frame, then it finds out what the index of the identified marker
  // is in the list of the identified markers. Obviously this is different from the index of the marker in the
  // list of the loaded markers.
  // This index will be returned via the second parameter \param identifiedMarkerIndex.
  int mtGetMarkerStatus(int loadedMarkerIndex, int* identifiedMarkerIndex);

  //------------------------
  //
  // Find the xpoints of the identified markers and retrieve them.
  //
  //------------------------
  // Note: The elements in the returned  array are as follow (the array starts at 0):
  // Index 0 and 1: X and Y of the Long Vector's Base of the Left Image.
  // Index 2 and 3: X and Y of the Long Vector's Head of the Left Image.
  // Index 4 and 5: X and Y of the Long Vector's Base of the Right Image.
  // Index 6 and 7: X and Y of the Long Vector's Head of the Right Image.
  // Index 8 and 9: X and Y of the Short Vector's Base of the Left Image.
  // Index 10 and 11: X and Y of the Short Vector's Head of the Left Image.
  // Index 12 and 13: X and Y of the Short Vector's Base of the Right Image.
  // Index 14 and 15: X and Y of the Short Vector's Head of the Right Image.
  void mtGetIdentifiedMarkersXPoints(double*& xPoints, int markerIndex);
  void mtFindIdentifiedMarkers();

  //------------------------
  //
  // Find the end points of the unidentified markers and retrieve them.
  //
  //------------------------
  // Note: The elements in the returned  array are as follow (the array starts at 0):
  // Index 0 and 4: X and Y of the vector's Base of the Left Image.
  // Index 2 and 6: X and Y of the vector's Head of the left Image.
  // Index 1 and 5: X and Y of the vector's Base of the right Image.
  // Index 3 and 7: X and Y of the vector's Head of the right Image.
  void mtGetUnidentifiedMarkersEnds(double*& endPoints, int vectorIndex);

  //------------------------
  // Gets the images captured by the index_th camera. Index of -1 means the current camera's images will be considered.
  int mtGetLeftRightImageArray(unsigned char**& leftImageArray, unsigned char**& rightImageArray, int index = -1);
  int mtGetLeftRightImageArrayHalfSize(unsigned char**& leftImageArray, unsigned char**& rightImageArray, int xResolution, int yResolution, int index = -1);

  /*! Get the MTC library version (major.minor.build.revision) */
  std::string GetSdkVersion();

  void mtSaveSettingsToINI();

  std::string ConvertReturnToString(int returnValue);
  std::string ConvertHazardToString(int hazardCode);

  std::string GetLastErrorString();

protected:
  void mtFindUnidentifiedMarkers();

  int removeFile(const std::string& fileName, const std::string& dir);
  int renameFile(const std::string& oldName, const std::string& newName, const std::string& dir);
  void logError(int errorNum, const char* description = NULL);

  static void getFileNamesFromDirectory(std::vector<std::string>& fileNames, const std::string& dir, bool returnCompletePath);

  // Accesses the "MicronTracker.ini" file to retrieve some values from it and then
  // set those values in the Markers interface object.
  void initialINIAccess(const std::string& iniFilePath);
  int checkCamIndex(int id);

protected:
  // A two dimensional vector for storing the rotation matrices. The first dimension is the marker number.
  // The second dimension is the vector of rotation matrices
  std::vector<std::vector<double>>  Rotations2D;

  // A two dimensional vector for storing the translations.
  std::vector<std::vector<double>>  Translations2D;

  // A two dimensional vector for storing the xpoints of the identified vectors. The first vector is the index
  // of the marker. The second dimension is the vector of xpoints. The latter vector is divided into group of 16s,
  // i.e. the first 16 elements belong to the first facet in the marker, if there are more than one facet in that
  // marker then the next 16 elements represent the xpoints of that facet and so on.
  std::vector<std::vector<double>>  IdentifiedMarkersXPoints;

  // A vector for storing the name of the identified markers
  std::vector<std::string>          IdentifiedMarkersName;

  // A two dimensional vector for storing the xpoints of the unidentified vectors.
  std::vector<std::vector<double>>  UnidentifiedMarkersEndPoints;

  // A vector for storing new sample vectors.
  std::vector<Collection*>          SampleVectors;

  Marker*                           CurrentMarker;

  // A vector for storing the number of facets for each identified marker. So the first element
  // is the number of facets in the first marker and so on.

  std::vector<int>                  NumOfFacetsInEachMarker;

  // A vector for storing the total number of facets for each identified marker. So the first element
  // is the number of total facets in the first marker and so on.

  std::vector<int>                  NumOfTotalFacetsInEachMarker;

  bool                              IsCameraAttached;
  Cameras*                          CameraList;
  MCamera*                          CurrentCamera;
  int                               CurrentCameraIndex;
  Markers*                          MarkerList;
  Persistence*                      Settings;
  Marker*                           TempMarkerForAddingFacet;

  int                               MarkerStatus;
  int                               NumOfIdentifiedMarkers;
  int                               NumOfUnidentifiedMarkers;

  // In the process of registering a marker, this variable indicates whether the registering facet is
  // a new facet or it is a new facet being added to an already existing marker. 0 = false; 1 = true

  int                               IsAddingAdditionalFacet;
  std::vector<Xform3D*>             Facet1ToCameraXfs;
  std::string                       ErrorString;
};

#endif
