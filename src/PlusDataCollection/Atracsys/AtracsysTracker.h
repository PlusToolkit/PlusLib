/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __AtracsysTracker_h
#define __AtracsysTracker_h

#include <string>
#include <vector>

// Atracsys includes
#include "ftkErrors.h"
#include "ftkEvent.h"
#include "ftkInterface.h"
#include "ftkOptions.h"
#include "ftkPlatform.h"
#include "ftkTypes.h"

#include <vtkSmartPointer.h>

class vtkMatrix4x4;

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
    ERROR_ENABLE_IR_STROBE,
    ERROR_SET_USER_LED,
    ERROR_ENABLE_USED_LED,
    ERROR_SET_MAX_MISSING_FIDUCIALS,
    ERROR_ENABLE_ONBOARD_PROCESSING,
    ERROR_ENABLE_IMAGE_STREAMING,
    ERROR_ENABLE_WIRELESS_MARKER_PAIRING,
    ERROR_ENABLE_WIRELESS_MARKER_STATUS_STREAMING,
    ERROR_ENABLE_WIRELESS_MARKER_BATTERY_STREAMING
  };

  enum DEVICE_TYPE
  {
    UNKNOWN_DEVICE = 0,
    SPRYTRACK_180 = 1,
    FUSIONTRACK_500 = 2,
    FUSIONTRACK_250 = 3
  };

  class Marker
  {
  public:
    Marker(int geometryId, vtkSmartPointer<vtkMatrix4x4> toolToTracker, int gpm, float freMm);
    int GetGeometryID();
    int GetGeometryPrecsenceMask();
    vtkSmartPointer<vtkMatrix4x4> GetTransformToTracker();
    float GetFiducialRegistrationErrorMm();
  private:
    int Id; /*!< Tracking id */
    int GeometryId;
    vtkSmartPointer<vtkMatrix4x4> ToolToTracker;
    int GeometryPresenceMask; /*!< Presence mask of fiducials expressed as
                              * their geometrical indexes */
    float FreMm; /*!< Registration mean ATRACSYS_ERROR (unit mm) */
  };

  /*! Connect to Atracsys tracker, must be called before any other function in this wrapper API. */
  ATRACSYS_RESULT Connect();
  /*! Closes connections to Atracsys tracker, must be called at end of application. */
  ATRACSYS_RESULT Disconnect();

  /*! */
  ATRACSYS_RESULT GetDeviceType(DEVICE_TYPE& deviceType);

  /*! */
  ATRACSYS_RESULT LoadMarkerGeometry(std::string filePath, int& geometryId);

  /*! */
  std::string GetMarkerInfo();

  /*! */
  std::string ResultToString(ATRACSYS_RESULT result);

  /*! */
  ATRACSYS_RESULT GetMarkersInFrame(std::vector<Marker>& markers);

  /*! */
  std::string GetLastErrorString();

  /*! */
  ATRACSYS_RESULT EnableIRStrobe(bool enabled);

  /*! */
  ATRACSYS_RESULT SetUserLEDState(int red, int green, int blue, int frequency);

  /*! */
  ATRACSYS_RESULT EnableUserLED(bool enabled);

  /*! */
  ATRACSYS_RESULT SetMaxMissingFiducials(int maxMissingFids);

  // ------------------------------------------
  // spryTrack only options
  // ------------------------------------------

  /*! */
  ATRACSYS_RESULT EnableOnboardProcessing(bool enabled);

  /*! */
  ATRACSYS_RESULT EnableImageStreaming(bool enabled);

  /*! */
  ATRACSYS_RESULT EnableWirelessMarkerPairing(bool enabled);

  /*! */
  ATRACSYS_RESULT EnableWirelessMarkerStatusStreaming(bool enabled);

  /*! */
  ATRACSYS_RESULT EnableWirelessMarkerBatteryStreaming(bool enabled);

  // ------------------------------------------
  // fusionTrack only options
  // ------------------------------------------

  /*! */
  ATRACSYS_RESULT GetDroppedFrameCount(int& droppedFrameCount);

  /*! */
  ATRACSYS_RESULT ResetLostFrameCount();

  enum OPTIONS
  {
    OPTION_DATA_SENDING = 20,
    OPTION_WIRELESS_PAIRING_ENABLE = 40,
    OPTION_IR_STROBE = 50,
    OPTION_LOST_FRAME_COUNT = 60,
    OPTION_RESET_LOST_FRAME_COUNT = 69,
    OPTION_LED_RED_COMPONENT = 90,
    OPTION_LED_GREEN_COMPONENT = 91,
    OPTION_LED_BLUE_COMPONENT = 92,
    OPTION_LED_FREQUENCY = 93,
    OPTION_LED_ENABLE = 94,
    OPTION_EPIPOLAR_MIN_DISTANCE = 2001,
    OPTION_MATCHING_TOLERANCE = 3002,
    OPTION_MAX_MEAN_REGISTRATION_ERROR = 3003,
    OPTION_MAX_MISSING_POINTS = 3004,
    OPTION_MAX_TRACKING_RANGE = 3005,
    OPTION_ONBOARD_PROCESSING = 6000,
    OPTION_IMAGE_STREAMING = 6003,
    OPTION_WIRELESS_MARKER_PAIRING = 7000,
    OPTION_WIRELESS_MARKER_STATUS_STREAMING = 7001,
    OPTION_WIRELESS_MARKER_BATTERY_STREAMING = 7002,
    OPTION_DEV_MARKERS_INFO = 7005,
  };

  enum IMAGE_PROCESSING_TYPE
  {
    PROCESSING_ONBOARD,
    PROCESSING_ON_PC
  };

private:
  DEVICE_TYPE DeviceType = UNKNOWN_DEVICE;
  
  class AtracsysInternal;
  AtracsysInternal* Internal;

  // load Atracsys marker geometry ini file
  //bool LoadIniFile(std::ifstream& is, ftkGeometry& geometry);

  // helper function to load ftkGeometry
  ATRACSYS_RESULT LoadFtkGeometry(const std::string& filename, ftkGeometry& geom);

  // helper function to set spryTrack only options
  ATRACSYS_RESULT SetSpryTrackOnlyOption(int option, int value, ATRACSYS_RESULT errorResult);

  // helper function to set fusionTrack only options
  ATRACSYS_RESULT SetFusionTrackOnlyOption(int option, int value, ATRACSYS_RESULT errorResult);
};

#endif