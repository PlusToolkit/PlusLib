#ifndef _CLARIUSWIFI_H
#define _CLARIUSWIFI_H

// local includes
#include "PlusConfigure.h"

// Windows includes
#include <windows.h>
#include <wlanapi.h>
#include <objbase.h>
#include <wtypes.h>

// STL includes
#include <iostream>
#include <string>

class ClariusWifi
{
public:
  ClariusWifi();
  ~ClariusWifi();

  /*! Set up WLAN api. Must be called before any other function in this class.
      Sets the HClient and CurApiVersion members */
  PlusStatus Initialize();
  /*! Releases handle and resources from WLAN api */
  PlusStatus DeInitialize();

  /*! Check if the Clarius network (SSID: DIRECT-***SN***) is available yet.
      Sets the InterfaceGuid member if Clarius network found */
  PlusStatus IsClariusNetworkReady(std::string ssid);

  PlusStatus IsClariusNetworkConnected();

  /*! Connect to the Clarius probe wifi network */
  PlusStatus ConnectToClariusWifi(std::string ssid, std::string password);

  /*! Disconnect from the Clarius probe wifi network */
  PlusStatus DisconnectFromClariusWifi();

  /*! Update the contents of the profile for the Clarius Wi-Fi network */
  PlusStatus UpdateClariusWifiProfile(std::string ssid, std::string password);

private:

  /*! Helper function to convert a std::string to WLAN api SSID */
  PlusStatus StringToSsid(std::string ssid, PDOT11_SSID pdot11_ssid);

  bool Connected; // are we currently connected to Clarius Wifi network
  HANDLE HClient; // client handle
  DWORD CurApiVersion; // negotiated WLAN api version
  GUID InterfaceGuid; // WLAN interface to connect to the Clarius over
  DOT11_BSS_TYPE BssType; // BSS type to connect to
};

#endif // _CLARIUSWIFI_H