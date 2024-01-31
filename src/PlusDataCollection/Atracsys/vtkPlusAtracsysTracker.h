/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusAtracsysTracker_h
#define __vtkPlusAtracsysTracker_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

#include <string>

/*!
\class vtkPlusAtracsysTracker
\brief Interface to the Atracsys trackers
This class talks with a Atracsys Tracker over the sTk Passive Tracking SDK.
Requires PLUS_USE_ATRACSYS option in CMake.
\ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusAtracsysTracker : public vtkPlusDevice
{
public:
  static vtkPlusAtracsysTracker* New();
  vtkTypeMacro(vtkPlusAtracsysTracker, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

  /* Get SDK version */
  std::string GetSdkVersion();

  /*! Retrieves the device calibration date in ISO format: YYYY-MM-DDTHH:MM:SSZ+XX
  Example: "2020-04-08T21:24:15Z+00" is April 8th, 2020 at 9:24:15pm UTC (+00)
  */
  std::string GetCalibrationDate();

  /* Get device type */
  std::string GetDeviceType();

  /*! Retrieves the cameras parameters :
  * leftIntrinsic = left camera focal length [0-1], optical center [2-3], lens distorsion [4-8] and skew [9]
  * rightIntrinsic = left camera focal length [0-1], optical center [2-3], lens distorsion [4-8] and skew [9]
  * rightPosition = position of the right camera in the coordinate system of the left camera
  * rightOrientation = orientation of the right camera in the coordinate system of the left camera
  */
  PlusStatus GetCamerasCalibration(
    std::array<float, 10>& leftIntrinsic, std::array<float, 10>& rightIntrinsic,
    std::array<float, 3>& rightPosition, std::array<float, 3>& rightOrientation);

  /*! Retrieves the loaded marker geometries :
  * geometries maps a marker (described by its id) to the vector of x,y,z coordinates of each fiducial
  * that composes the marker
  */
  PlusStatus GetLoadedGeometries(std::map<int, std::vector<std::array<float, 3>>>& geometries);

  /* Device is a hardware tracker. */
  virtual bool IsTracker() const { return true; }
  virtual bool IsVirtual() const;

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);

  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Connect to device */
  PlusStatus InternalConnect();

  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect();

  /*! Probe to see if the tracking system is present. */
  PlusStatus Probe();

  /*!  */
  PlusStatus InternalUpdate();

  /*! Pause/unpause virtual device. */
  PlusStatus PauseVirtualDevice();
  PlusStatus UnpauseVirtualDevice();

public:
  // Commands
  static const char* ATRACSYS_COMMAND_SET_FLAG;
  static const char* ATRACSYS_COMMAND_LED_ENABLED;
  static const char* ATRACSYS_COMMAND_LASER_ENABLED;
  static const char* ATRACSYS_COMMAND_VIDEO_ENABLED;
  static const char* ATRACSYS_COMMAND_SET_LED_RGBF; // F = LED flash frequency
  static const char* ATRACSYS_COMMAND_ENABLE_TOOL;
  static const char* ATRACSYS_COMMAND_ADD_TOOL;

  const std::map<std::string, std::string>& GetDeviceOptions() const;
  PlusStatus GetOptionValue(const std::string& optionName, std::string& optionValue);

  // Command methods
  // LED
  PlusStatus SetLedEnabled(bool enabled);
  PlusStatus SetUserLEDState(int red, int green, int blue, int frequency, bool enabled = true);
  // Tools
  PlusStatus SetToolEnabled(std::string toolId, bool enabled);
  PlusStatus AddToolGeometry(std::string toolId, std::string geomString);
  // Other
  PlusStatus SetLaserEnabled(bool enabled);
  PlusStatus SetVideoEnabled(bool enabled);

protected:
  vtkPlusAtracsysTracker();
  ~vtkPlusAtracsysTracker();

  // helper to translate option names from Plus nomenclature to Atracsys' one
  bool TranslateOptionName(const std::string& optionName, std::string& translatedOptionName);

private: // Functions
  vtkPlusAtracsysTracker(const vtkPlusAtracsysTracker&);

  /*! Start the tracking system. */
  PlusStatus InternalStartRecording();

  /*! Stop the tracking system and bring it back to its initial state. */
  PlusStatus InternalStopRecording();
  std::vector<std::string> DisabledToolIds;

  class vtkInternal;
  vtkInternal* Internal;
};

#endif
