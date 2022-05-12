/*=Plus=header=begin======================================================
    Program: Plus
    Copyright (c) Verdure Imaging Inc, Stockton, California. All rights reserved.
    See License.txt for details.
    We would like to acknowledge Verdure Imaging Inc for generously open-sourcing
    this support for the Clarius OEM interface to the PLUS & Slicer communities.
=========================================================Plus=header=end*/

#ifndef _CLARIUSBLE_H
#define _CLARIUSBLE_H

// local includes
#include "PlusConfigure.h"

// STL includes
#include <memory>
#include <string>
#include <utility>
#include <vector>

class ClariusBLEPrivate;

//-----------------------------------------------------------------------------
enum class ClariusAvailability
{
  AVAILABLE,    // available for control over BLE
  LISTEN,       // available for casting
  NOT_AVAILABLE
};

//-----------------------------------------------------------------------------
enum class ClariusWifiMode
{
  ACCESS_POINT, // probe is configured to be connected to directly 
                // via the DIRECT-**SN** network
  LAN           // probe is configured to connect to a local area network
};

//-----------------------------------------------------------------------------
struct ClariusWifiInfo
{
  bool Ready;
  ClariusAvailability Available;
  ClariusWifiMode WifiMode;
  std::string SSID;
  std::string Password;
  std::string IPv4;
  std::string MacAddress;
  int ControlPort;
  int CastPort;
  int Channel;

  ClariusWifiInfo()
    : Ready(false)
    , Available(ClariusAvailability::NOT_AVAILABLE)
    , WifiMode(ClariusWifiMode::ACCESS_POINT)
    , SSID("")
    , Password("")
    , IPv4("")
    , MacAddress("")
    , ControlPort(-1)
    , CastPort(-1)
    , Channel(-1)
  {
  }

  ClariusWifiInfo(const ClariusWifiInfo&) = default;
  ClariusWifiInfo& operator=(const ClariusWifiInfo&) = default;
};

//-----------------------------------------------------------------------------
class ClariusBLE
{
public:

  ClariusBLE();

  ~ClariusBLE();

  /*!
   * Search for probe BLE connection by serial number
   * \return PLUS_SUCCESS if probe BLE was found
   */
  PlusStatus ClariusBLE::FindBySerial(std::string serialNum);

  /*!
   * If FindBySerial returns false, this method can be used to retrieve a list
   * of serial numbers for available Clarius probes
   * \return list of nearby probe serial numbers
   */
  std::vector<std::string> RetrieveFoundProbeIds();

  /*!
   * Is there a currently connected probe?
   */
  bool IsProbeConnected();

  /*!
   * Connect to probe found by FindBySerial
   */
  PlusStatus Connect();

  /*!
   * Disconnect from the currently connected probe BLE
   */
  PlusStatus Disconnect();

  /*!
   * Check if currently connected probe is powered
   */
  bool IsProbePowered();

  /*!
   * Initiate the power on process for the currently connected probe
   */
  PlusStatus RequestProbeOn();

  /*!
   * Initiate the power off process for the currently connected probe
   */
  PlusStatus RequestProbeOff();

  /*!
   * Synchronously wait for probe wifi information to be available.
   * Useful for blocking execution while the probe is booting.
   * 
   * WARNING: This function can block for ~10 seconds, and should
   * therefore be called on a thread.
   * 
   * \return true if info available, false if timeout
   */
  bool AwaitWifiInfoReady();

  /*!
   * Configure the probe wifi to access-point mode, where the user will
   * directly connect to the probe's wifi network (DIRECT-**SN** SSID).
   */
  PlusStatus ConfigureWifiAP();

  /*!
   * Configure the probe wifi to LAN mode, where the probe will connect
   * to a local wifi network
   * \param ssid
   * \param password
   */
  PlusStatus ConfigureWifiLAN(std::string ssid, std::string password);

  /*!
   * Get the wifi information to connect to the probe over, this is only
   * useful when the probe is configured in ACCESS_POINT mode
   * 
   * \return std::pair as C++ 11 compatible equivalent of std::optional
   *         first: status of function call
   *         second: if successful, probe wifi info
   */
  std::pair<PlusStatus, ClariusWifiInfo> GetWifiInfo();

  /*!
   * Get the last error message
   */
  std::string GetLastError();

private:

  std::unique_ptr<ClariusBLEPrivate> _impl;

};

#endif // _CLARIUSBLE_H