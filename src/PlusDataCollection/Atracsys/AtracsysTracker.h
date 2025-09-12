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
#include <vtkMatrix4x4.h>
#include <map>

struct ftkOptionsInfo;

namespace Atracsys
{
  // Functions to safely convert string to int32 or float
  bool strToInt32(const std::string& str, int& var);
  bool strToFloat32(const std::string& str, float& var);

  // Class to hold the 3D and 2D coordinates in mm and pixel respectively of
  // a single fiducial in the view of the camera
  class Fiducial
  {
  public:
    Fiducial() {};
    // Overloaded operators, only for 3D coordinates
    // override equality operator to make fids less than EQUALITY_DISTANCE_MM considered equal
    bool operator==(const Fiducial& f) const;
    // compare fiducials on distance from the origin
    bool operator<(const Fiducial& f) const;
    uint32_t id = 0;
    // 3D
    uint32_t Fid3dStatus = 0;
    float xMm = 0;
    float yMm = 0;
    float zMm = 0;
    float epipolarErrorPx = 0;
    float probability = -1;
    float triangulErrorMm = 0;
    // 2D left
    uint32_t Fid2dLeftStatus = 0;
    float xLeftPx = 0;
    float yLeftPx = 0;
    uint16_t heightLeftPx = 0;
    uint16_t widthLeftPx = 0;
    uint32_t pixCountLeft = 0;
    // 2D right
    uint32_t Fid2dRightStatus = 0;
    float xRightPx = 0;
    float yRightPx = 0;
    uint16_t heightRightPx = 0;
    uint16_t widthRightPx = 0;
    uint32_t pixCountRight = 0;
  };

  typedef std::vector<Fiducial> Fiducials;

  // Class to hold position and metadata of a marker in the camera's field of view
  class Marker
  {
  public:
    Marker();
    // toolToTracker is deep copied in this constructor
    Marker(int status, int trackingId, int geometryId,
      vtkMatrix4x4* toolToTracker, int geometryPresenceMask, float registrationErrorMM);
    Marker(const Marker&);
    bool AddFiducial(Fiducial fid);
    uint32_t GetMarkerStatus() { return MarkerStatus; }
    uint32_t GetTrackingID() { return TrackingId; }
    uint32_t GetGeometryID() { return GeometryId; }
    vtkMatrix4x4* GetTransformToTracker() { return this->ToolToTracker.GetPointer(); }
    uint32_t GetGeometryPresenceMask() { return GeometryPresenceMask; }
    float GetFiducialRegistrationErrorMm() { return RegistrationErrorMm; }
    const Fiducials& GetFiducials() { return fiducials; }

  private:
    uint32_t MarkerStatus;
    uint32_t TrackingId;
    uint32_t GeometryId;
    vtkNew<vtkMatrix4x4> ToolToTracker;
    uint32_t GeometryPresenceMask; // presence mask of fiducials expressed as their numerical indices
    float RegistrationErrorMm; // mean fiducial registration error (unit mm)
    Fiducials fiducials; // fiducial coordinates
  };

  class Tracker
  {
  public:
    /* Constructor & destructor */
    Tracker();
    virtual ~Tracker();

    enum RESULT
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
      ERROR_FAILED_TO_EXPORT_CALIB,
      ERROR_FAILED_TO_EXTRACT_FRAME_INFO,
      ERROR_CANNOT_CREATE_FRAME_INSTANCE,
      ERROR_CANNOT_INITIALIZE_FRAME,
      ERROR_NO_FRAME_AVAILABLE,
      ERROR_INVALID_FRAME,
      ERROR_TOO_MANY_MARKERS,
      ERROR_TOO_MANY_FIDUCIALS,
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
      SPRYTRACK_300,
      FUSIONTRACK_500,
      FUSIONTRACK_250
    };

    enum SPRYTRACK_IMAGE_PROCESSING_TYPE
    {
      PROCESSING_ONBOARD,
      PROCESSING_ON_PC
    };

    /* Is onboard/embedded processing on ?*/
    bool IsOnboardProcessing();

    /*! Is virtual ? */
    bool IsVirtual();

    /*! If virtual device, pause/unpause */
    void Pause(bool tof);

    /*! Connect to Atracsys tracker, must be called before any other function in this wrapper API. */
    RESULT Connect();

    /*! Closes connections to Atracsys tracker, must be called at end of application. */
    RESULT Disconnect();

    /*! */
    RESULT GetSDKversion(std::string& version);

    /*! */
    RESULT GetCalibrationDate(std::string& date);

    /*! */
    RESULT GetDeviceType(DEVICE_TYPE& deviceType);

    /*! */
    RESULT GetDeviceId(uint64_t& id);

    /*! Retrieves the cameras parameters :
    * leftIntrinsic = left camera focal length [0-1], optical center [2-3], lens distorsion [4-8] and skew [9]
    * rightIntrinsic = left camera focal length [0-1], optical center [2-3], lens distorsion [4-8] and skew [9]
    * rightPosition = position of the right camera in the coordinate system of the left camera
    * rightOrientation = orientation of the right camera in the coordinate system of the left camera
    */
    RESULT GetCamerasCalibration(std::array<float,10>& leftIntrinsic, std::array<float, 10>& rightIntrinsic,
      std::array<float, 3>& rightPosition, std::array<float, 3>& rightOrientation);

    /*! */
    RESULT LoadMarkerGeometryFromFile(std::string filePath, int& geometryId);

    /*! */
    RESULT LoadMarkerGeometryFromString(std::string filePath, int& geometryId);

    /*! */
    RESULT GetMarkerInfo(std::string& markerInfo);

    /*! */
    RESULT GetLoadedGeometries(std::map<int, std::vector<std::array<float,3>>>& geometries);

    /*! */
    std::string ResultToString(RESULT result);

    /*! */
    RESULT GetFiducialsInFrame(std::vector<Fiducial>& fiducials,
      std::map<std::string, std::string>& events, uint64_t& sdkTimestamp);

    /*! */
    RESULT GetMarkersInFrame(std::vector<Marker>& markers,
      std::map<std::string, std::string>& events, uint64_t& sdkTimestamp);

    /*! */
    RESULT SetUserLEDState(int red, int green, int blue, int frequency, bool enabled = true);

    /*! */
    RESULT EnableUserLED(bool enabled);

    /*! */
    RESULT SetLaserEnabled(bool enabled);

    /*! */
    RESULT EnableWirelessMarkerPairing(bool enabled);

    /*! */
    RESULT EnableWirelessMarkerStatusStreaming(bool enabled);

    /*! */
    RESULT EnableWirelessMarkerBatteryStreaming(bool enabled);

    // ------------------------------------------
    // frame options
    // ------------------------------------------
    /*! Set/get the maximum number of additional events per frame included in the device's output.
    This extends the default allocation of 20, for a total of 20 + n events allowed per frame. */
    RESULT SetMaxAdditionalEventsNumber(int n);
    int GetMaxAdditionalEventsNumber() { return MaxAdditionalEventsNumber; }

    /*! Set/get the maximum number of 2D fiducials (in either left or right frame) included in the device's output */
    RESULT SetMax2dFiducialsNumber(int n);
    int GetMax2dFiducialsNumber() { return Max2dFiducialsNumber; }

    /*! Set/get the maximum number of 3D fiducials (after triangulation) included in the device's output */
    RESULT SetMax3dFiducialsNumber(int n);
    int GetMax3dFiducialsNumber() { return Max3dFiducialsNumber; }

    /*! Set/get the maximum number of markers included in the device's output */
    RESULT SetMaxMarkersNumber(int n);
    int GetMaxMarkersNumber() { return MaxMarkersNumber; }

    // ------------------------------------------
    // spryTrack only options
    // ------------------------------------------

    /*! */
    RESULT EnableOnboardProcessing(bool enabled);

    /*! */
    RESULT EnableImageStreaming(bool enabled);

    /*! */
    RESULT SetSpryTrackProcessingType(SPRYTRACK_IMAGE_PROCESSING_TYPE processingType);

    // ------------------------------------------
    // fusionTrack only options
    // ------------------------------------------

    /*! Sum of lost and corrupted frames */
    RESULT GetDroppedFrameCount(int& droppedFrameCount);

    /*! */
    RESULT ResetLostFrameCount();

    RESULT SetOption(const std::string&, const std::string&);

  protected:
    bool GetOptionInfo(const std::string&, const ftkOptionsInfo*&);

  private:
    DEVICE_TYPE DeviceType = UNKNOWN_DEVICE;

    int MaxAdditionalEventsNumber = 0; // beyond the default allocation of 20 events
    int Max2dFiducialsNumber = 256;
    int Max3dFiducialsNumber = 256;
    int MaxMarkersNumber = 16;

    bool isOnboardProcessing = false;

    class Internal;
    Internal* InternalObj;
  };
} // namespace Atracsys
#endif
