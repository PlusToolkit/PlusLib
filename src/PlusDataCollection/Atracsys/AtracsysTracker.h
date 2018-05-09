/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __AtracsysTracker_h
#define __AtracsysTracker_h

#include "AtracsysConstants.h"
#include <string>
#include <vector>

// Atracsys includes

// Atracsys includes
#include "ftkErrors.h"
#include "ftkEvent.h"
#include "ftkInterface.h"
#include "ftkOptions.h"
#include "ftkPlatform.h"
#include "ftkTypes.h"

namespace Atracsys
{
  class Marker;
  class Internal;

  class Tracker
  {
  public:
    /* Constructor & destructor */
    Tracker();
    ~Tracker();

    /*! Connect to Atracsys tracker, must be called before any other function in this wrapper API. */
    ATRACSYS_ERROR Connect();
    /*! Closes connections to Atracsys tracker, must be called at end of application. */
    ATRACSYS_ERROR Disconnect();

    /*! */
    ATRACSYS_ERROR GetDeviceType(DEVICE_TYPE& deviceType);

    /*! */
    ATRACSYS_ERROR LoadMarkerGeometry(std::string filePath, int& geometryId);

    /*! */
    std::string GetMarkerInfo();

    /*! */
    ATRACSYS_ERROR GetMarkersInFrame(std::vector<Marker>& markers);

    /*! */
    std::string GetLastErrorString();

    /*! */
    ATRACSYS_ERROR EnableIRStrobe();
    /*! */
    ATRACSYS_ERROR DisableIRStrobe();

    /*! */
    ATRACSYS_ERROR SetUserLEDState(int red, int green, int blue, int frequency);

    /*! */
    ATRACSYS_ERROR EnableUserLED();
    /*! */
    ATRACSYS_ERROR DisableUserLED();

    // ------------------------------------------
    // spryTrack only options
    // ------------------------------------------

    /*! */
    ATRACSYS_ERROR EnableOnboardProcessing();
    /*! */
    ATRACSYS_ERROR DisableOnboardProcessing();

    /*! */
    ATRACSYS_ERROR EnableImageStreaming();
    /*! */
    ATRACSYS_ERROR DisableImageStreaming();

    /*! */
    ATRACSYS_ERROR EnableWirelessMarkerPairing();
    /*! */
    ATRACSYS_ERROR DisableWirelessMarkerPairing();

    /*! */
    ATRACSYS_ERROR EnableWirelessMarkerStatusStreaming();
    /*! */
    ATRACSYS_ERROR DisableWirelessMarkerStatusStreaming();

    /*! */
    ATRACSYS_ERROR EnableWirelessMarkerBatteryStreaming();
    /*! */
    ATRACSYS_ERROR DisableWirelessMarkerBatteryStreaming();

    // ------------------------------------------
    // fusionTrack only options
    // ------------------------------------------

    /*! */
    ATRACSYS_ERROR Tracker::GetDroppedFrameCount(int& droppedFrameCount);

    /*! */
    ATRACSYS_ERROR Tracker::ResetLostFrameCount();

  private:
    ftkLibrary FtkLib = 0;
    uint64 TrackerSN = 0;
    DEVICE_TYPE DeviceType = UNKNOWN;
    std::string LastError;
    
    // load Atracsys marker geometry ini file
    bool LoadIniFile(std::ifstream& is, ftkGeometry& geometry);

    // helper function to load ftkGeometry
    ATRACSYS_ERROR LoadFtkGeometry(const std::string& filename, ftkGeometry& geom);

    // helper function to set spryTrack only options
    ATRACSYS_ERROR SetSpryTrackOnlyOption(int option, int value);

    // helper function to set fusionTrack only options
    ATRACSYS_ERROR SetFusionTrackOnlyOption(int option, int value);
  };
}
#endif