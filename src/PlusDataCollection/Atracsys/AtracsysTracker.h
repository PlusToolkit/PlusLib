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

    /*! */
    ATRACSYS_ERROR Connect();
    /*! */
    ATRACSYS_ERROR Disconnect();

    /*! */
    ATRACSYS_ERROR EnableOnboardProcessing();
    /*! */
    ATRACSYS_ERROR DisableOnboardProcessing();

    /*! */
    ATRACSYS_ERROR EnableImageStreaming();
    /*! */
    ATRACSYS_ERROR DisableImageStreaming();
    
    /*! */
    ATRACSYS_ERROR EnableIRStrobe();
    /*! */
    ATRACSYS_ERROR DisableIRStrobe();

    /*! */
    ATRACSYS_ERROR GetDroppedFrameCount();
    /*! */
    ATRACSYS_ERROR ResetLostFrameCount();

    /*! */
    ATRACSYS_ERROR SetUserLEDState(int red, int green, int blue, int frequency);
    /*! */
    ATRACSYS_ERROR GetUserLEDState(int& red, int& green, int& blue, int& frequency);
    /*! */
    ATRACSYS_ERROR EnableUserLED();
    /*! */
    ATRACSYS_ERROR DisableUserLED();

    /*! */
    ATRACSYS_ERROR LoadMarkerGeometry(std::string filePath, int& geometryId);
    /*! */
    ATRACSYS_ERROR EnableWirelessMarkerPairing();
    /*! */
    ATRACSYS_ERROR DisableWirelessMarkerPairing();

    /*! */
    ATRACSYS_ERROR GetMarkersInFrame(std::vector<Marker>& markers);

    /*! */
    std::string GetFtkLastErrorString();

    /*! */
    ATRACSYS_ERROR LoadFtkGeometry(const std::string& filename, ftkGeometry& geom);

  private:
    ftkLibrary FtkLib = 0;
    uint64 TrackerSN = 0;
    DEVICE_TYPE DeviceType = UNKNOWN;

    bool LoadIniFile(std::ifstream& is, ftkGeometry& geometry);
    //----------------------------------------------------------------------------
  
    ATRACSYS_ERROR SetSTKOnlyOption(OPTIONS option, int value);
  };
}
#endif