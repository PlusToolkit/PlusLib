/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __AtracsysTracker_h
#define __AtracsysTracker_h

#include <string>
#include <vector>
#include <vtkNew.h>

struct ftkOptionsInfo;
class vtkMatrix4x4;

// Functions to safely convert string to int32 or float
bool strToInt32(const std::string& str, int& var);
bool strToFloat32(const std::string& str, float& var);

class AtracsysTracker
{
public:
  /* Constructor & destructor */
  AtracsysTracker();
  virtual ~AtracsysTracker();

  enum ATRACSYS_RESULT
  {
    SUCCESS = 0,
    ERROR_UNABLE_TO_GET_FTK_HANDLE,
    ERROR_NO_DEVICE_CONNECTED,
    WARNING_CONNECTED_IN_USB2,
    ERROR_UNABLE_TO_LOAD_MARKER,
    ERROR_FAILURE_TO_LOAD_INI,
    ERROR_OPTION_AVAILABLE_ONLY_ON_FTK,
    ERROR_OPTION_AVAILABLE_ONLY_ON_STK,
    ERROR_FAILED_TO_CLOSE_SDK,
    ERROR_CANNOT_CREATE_FRAME_INSTANCE,
    ERROR_CANNOT_INITIALIZE_FRAME,
    ERROR_NO_FRAME_AVAILABLE,
    ERROR_INVALID_FRAME,
    ERROR_TOO_MANY_MARKERS,
    ERROR_ENABLE_LASER,
    ERROR_SET_USER_LED,
    ERROR_ENABLE_USER_LED,
    ERROR_ENABLE_ONBOARD_PROCESSING,
    ERROR_ENABLE_IMAGE_STREAMING,
    ERROR_ENABLE_WIRELESS_MARKER_PAIRING,
    ERROR_ENABLE_WIRELESS_MARKER_STATUS_STREAMING,
    ERROR_ENABLE_WIRELESS_MARKER_BATTERY_STREAMING,
    ERROR_DISCONNECT_ATTEMPT_WHEN_NOT_CONNECTED,
    ERROR_CANNOT_GET_MARKER_INFO,
    ERROR_FAILED_TO_SET_STK_PROCESSING_TYPE,
    ERROR_OPTION_NOT_FOUND,
    ERROR_SET_OPTION
  };

  enum DEVICE_TYPE
  {
    UNKNOWN_DEVICE = 0,
    SPRYTRACK_180,
    FUSIONTRACK_500,
    FUSIONTRACK_250
  };

  enum SPRYTRACK_IMAGE_PROCESSING_TYPE
  {
    PROCESSING_ONBOARD,
    PROCESSING_ON_PC
  };

  // Class to hold a single fiducial in the view of the camera
  class Fiducial3D
  {
  public:
    Fiducial3D();
    Fiducial3D(float x, float y, float z, float probability);
    // override equality operator to make fids less than EQUALITY_DISTANCE_MM considered equal
    bool operator==(const Fiducial3D& f);
    bool operator<(const Fiducial3D& f) const;
    float xMm = 0;
    float yMm = 0;
    float zMm = 0;
    float probability = -1;
  };

  // Class to hold position and metadata of a marker in the camera's field of view
  class Marker
  {
  public:
    Marker();
    /*! toolToTracker is deep copied */
    Marker(int geometryId, vtkMatrix4x4* toolToTracker, int gpm, float freMm);
    Marker(const Marker&);
    int GetGeometryID();
    int GetGeometryPresenceMask();
    vtkMatrix4x4* GetTransformToTracker();
    float GetFiducialRegistrationErrorMm();
  private:
    int GeometryId;
    vtkNew<vtkMatrix4x4> ToolToTracker;
    int GeometryPresenceMask; // presence mask of fiducials expressed as their numerical indices
    float RegistrationErrorMM; // Mean fiducial registration error (unit mm)
  };

  /*! Connect to Atracsys tracker, must be called before any other function in this wrapper API. */
  ATRACSYS_RESULT Connect();
  /*! Closes connections to Atracsys tracker, must be called at end of application. */
  ATRACSYS_RESULT Disconnect();

  /*! */
  ATRACSYS_RESULT GetDeviceType(DEVICE_TYPE& deviceType);

  /*! */
  ATRACSYS_RESULT LoadMarkerGeometryFromFile(std::string filePath, int& geometryId);

  /*! */
  ATRACSYS_RESULT LoadMarkerGeometryFromString(std::string filePath, int& geometryId);

  /*! */
  ATRACSYS_RESULT GetMarkerInfo(std::string& markerInfo);

  /*! */
  std::string ResultToString(ATRACSYS_RESULT result);

  /*! */
  ATRACSYS_RESULT GetFiducialsInFrame(std::vector<Fiducial3D>& fiducials);

  /*! */
  ATRACSYS_RESULT GetMarkersInFrame(std::vector<Marker>& markers);

  /*! */
  ATRACSYS_RESULT SetUserLEDState(int red, int green, int blue, int frequency, bool enabled = true);

  /*! */
  ATRACSYS_RESULT EnableUserLED(bool enabled);

  /*! */
  ATRACSYS_RESULT SetLaserEnabled(bool enabled);

  /*! */
  ATRACSYS_RESULT EnableWirelessMarkerPairing(bool enabled);

  /*! */
  ATRACSYS_RESULT EnableWirelessMarkerStatusStreaming(bool enabled);

  /*! */
  ATRACSYS_RESULT EnableWirelessMarkerBatteryStreaming(bool enabled);

  // ------------------------------------------
  // frame options
  // ------------------------------------------
  /*! Set/get the maximum number of events per frame included in the device's output */
  ATRACSYS_RESULT SetMaxEventsNumber(int n);
  int GetMaxEventsNumber() { return MaxEventsNumber; }

  /*! Set/get the maximum number of 2D fiducials (in either left or right frame) included in the device's output */
  ATRACSYS_RESULT SetMax2dFiducialsNumber(int n);
  int GetMax2dFiducialsNumber() { return Max2dFiducialsNumber; }

  /*! Set/get the maximum number of 3D fiducials (after triangulation) included in the device's output */
  ATRACSYS_RESULT SetMax3dFiducialsNumber(int n);
  int GetMax3dFiducialsNumber() { return Max3dFiducialsNumber; }

  /*! Set/get the maximum number of markers included in the device's output */
  ATRACSYS_RESULT SetMaxMarkersNumber(int n);
  int GetMaxMarkersNumber() { return MaxMarkersNumber; }

  // ------------------------------------------
  // spryTrack only options
  // ------------------------------------------

  /*! */
  ATRACSYS_RESULT EnableOnboardProcessing(bool enabled);

  /*! */
  ATRACSYS_RESULT EnableImageStreaming(bool enabled);

  /*! */
  ATRACSYS_RESULT SetSpryTrackProcessingType(SPRYTRACK_IMAGE_PROCESSING_TYPE processingType);

  // ------------------------------------------
  // fusionTrack only options
  // ------------------------------------------

  /*! Sum of lost and corrupted frames */
  ATRACSYS_RESULT GetDroppedFrameCount(int& droppedFrameCount);

  /*! */
  ATRACSYS_RESULT ResetLostFrameCount();

  ATRACSYS_RESULT SetOption(const std::string&, const std::string&);

protected:
  bool GetOptionInfo(const std::string&, const ftkOptionsInfo*&);

private:
  DEVICE_TYPE DeviceType = UNKNOWN_DEVICE;

  int MaxEventsNumber = 0;
  int Max2dFiducialsNumber = 0;
  int Max3dFiducialsNumber = 64;
  int MaxMarkersNumber = 16;

  class AtracsysInternal;
  AtracsysInternal* Internal;
};
#endif
