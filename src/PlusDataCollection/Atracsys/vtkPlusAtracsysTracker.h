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
  static vtkPlusAtracsysTracker *New();
  vtkTypeMacro(vtkPlusAtracsysTracker, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

  /* Device is a hardware tracker. */
  virtual bool IsTracker() const { return true; }
  virtual bool IsVirtual() const { return false; }

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

public:
   // Commands
  static const char* ATRACSYS_COMMAND_SET_FLAG;
  static const char* ATRACSYS_COMMAND_LED_ENABLED;
  static const char* ATRACSYS_COMMAND_LASER_ENABLED;
  static const char* ATRACSYS_COMMAND_VIDEO_ENABLED;
  static const char* ATRACSYS_COMMAND_SET_LED_RGBF; // F = LED flash frequency
  static const char* ATRACSYS_COMMAND_ENABLE_TOOL;
  static const char* ATRACSYS_COMMAND_ADD_TOOL;

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

private: // Functions
  vtkPlusAtracsysTracker(const vtkPlusAtracsysTracker&);
  void operator=(const vtkPlusAtracsysTracker&);

  /*! Start the tracking system. */
  PlusStatus InternalStartRecording();

  /*! Stop the tracking system and bring it back to its initial state. */
  PlusStatus InternalStopRecording();

  std::vector<std::string> DisabledToolIds;

  class vtkInternal;
  vtkInternal* Internal;
};

#endif
