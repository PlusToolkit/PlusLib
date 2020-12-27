// local includes
#include "ClariusWifi.h"

// STL includes
#include <chrono>
#include <thread>

// Need to link with wlanapi.lib and ole32.lib
#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")

// max time ClariusWifi will wait for the Clarius network to become
// available during call to CONNECT
static const float MAX_NETWORK_READY_WAIT_TIME_SEC = 25.0;

//-------------------------------------------------------------------------------------------------
ClariusWifi::ClariusWifi()
  : Connected(false)
  , HClient(NULL)
  , CurApiVersion(0)
{
}

//-------------------------------------------------------------------------------------------------
ClariusWifi::~ClariusWifi()
{
  // enforce disconnection & de-initialization
  this->DisconnectFromClariusWifi();
  this->DeInitialize();
}

//-------------------------------------------------------------------------------------------------
PlusStatus ClariusWifi::Initialize()
{
  // already initialized, return failure
  if (this->HClient != NULL)
  {
    LOG_ERROR("ClariusWifi already intialized, this initialization call was rejected");
    return PLUS_FAIL;
  }

  DWORD maxClientVersion = 2;
  DWORD res = WlanOpenHandle(maxClientVersion, NULL, &this->CurApiVersion, &this->HClient);
  if (res != ERROR_SUCCESS)
  {
    LOG_ERROR("Failed to open WLAN API client handle");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus ClariusWifi::DeInitialize()
{
  // already de-initialized, return success
  if (this->HClient == NULL)
  {
    return PLUS_SUCCESS;
  }

  DWORD res = WlanCloseHandle(this->HClient, NULL);
  if (res != ERROR_SUCCESS)
  {
    LOG_ERROR("Failed to close WLAN API client handle");
    return PLUS_FAIL;
  }

  this->HClient = NULL;
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus ClariusWifi::IsClariusNetworkReady(std::string ssid)
{
  // check ClariusWifi was initialized
  if (this->HClient == NULL)
  {
    LOG_ERROR("ClariusWifi must be initialized before calling IsClariusNetworkReady");
    return PLUS_FAIL;
  }

  PWLAN_INTERFACE_INFO_LIST interfaces = NULL;
  PWLAN_INTERFACE_INFO if_info = NULL;
  DWORD res = WlanEnumInterfaces(this->HClient, NULL, &interfaces);
  if (res != ERROR_SUCCESS)
  {
    LOG_ERROR("Failed to enumerate WLAN interfaces");
    return PLUS_FAIL;
  }

  for (int i = 0; i < interfaces->dwNumberOfItems; i++)
  {
    if_info = (WLAN_INTERFACE_INFO*)&interfaces->InterfaceInfo[i];

    PWLAN_RAW_DATA data;
    res = WlanScan(this->HClient, &if_info->InterfaceGuid, NULL, NULL, NULL);
    if (res != ERROR_SUCCESS)
    {
      LOG_ERROR("Failed to scan for networks");
    }

    PWLAN_AVAILABLE_NETWORK_LIST networks = NULL;
    PWLAN_AVAILABLE_NETWORK nw_info = NULL;
    res = WlanGetAvailableNetworkList(this->HClient, &if_info->InterfaceGuid,
      0, NULL, &networks);
    if (res != ERROR_SUCCESS)
    {
      LOG_WARNING("Failed to list available networks for interface: " << if_info->strInterfaceDescription);
      continue;
    }

    for (int j = 0; j < networks->dwNumberOfItems; j++)
    {
      nw_info = (WLAN_AVAILABLE_NETWORK*)&networks->Network[j];

      std::string candidate_ssid = (char*) nw_info->dot11Ssid.ucSSID;
      if (ssid.compare(candidate_ssid) == 0)
      {
        this->InterfaceGuid = if_info->InterfaceGuid;
        return PLUS_SUCCESS;
      }
    }

    // free networks memory
    if (networks != NULL)
    {
      WlanFreeMemory(networks);
      networks = NULL;
    }
  }

  // free interfaces memory
  if (interfaces != NULL)
  {
    WlanFreeMemory(interfaces);
    interfaces = NULL;
  }

  return PLUS_FAIL;
}

//-------------------------------------------------------------------------------------------------
PlusStatus ClariusWifi::ConnectToClariusWifi(std::string ssid, std::string password)
{
  // already connected, return failure
  if (this->Connected)
  {
    LOG_ERROR("ClariusWifi already connected to Clarius network, this connection call was rejected");
  }

  // check ClariusWifi was initialized
  if (this->HClient == NULL)
  {
    LOG_ERROR("ClariusWifi must be initialized before calling IsClariusNetworkReady");
    return PLUS_FAIL;
  }

  // wait for Clarius network being ready
  std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
  PlusStatus network_ready = PLUS_FAIL;
  while (network_ready != PLUS_SUCCESS)
  {
    // check we haven't exceeded the maximum wait time for Clarius network
    // to be ready
    std::chrono::steady_clock::time_point t = std::chrono::steady_clock::now();
    std::chrono::duration<double> dur = t - start_time;
    if (dur.count() > MAX_NETWORK_READY_WAIT_TIME_SEC)
    {
      LOG_ERROR("Waiting for Clarius wifi network to be ready timed out.");
      return PLUS_FAIL;
    }

    // pause for 500ms, to avoid overhead of checking wifi availability at high
    // refresh rate
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // refresh network_ready state
    network_ready = this->IsClariusNetworkReady(ssid);
  }

  std::string clarius_profile;
  PWLAN_PROFILE_INFO_LIST profiles = NULL;
  PWLAN_PROFILE_INFO pf_info = NULL;
  DWORD res = WlanGetProfileList(this->HClient, &this->InterfaceGuid, NULL, &profiles);
  if (res != ERROR_SUCCESS)
  {
    LOG_ERROR("Failed to list avilable WLAN interface profiles");
    return PLUS_FAIL;
  }

  for (int i = 0; i < profiles->dwNumberOfItems; i++)
  {
    pf_info = &profiles->ProfileInfo[i];
    std::wstring w_pf_name = pf_info->strProfileName;
    std::string pf_name(w_pf_name.begin(), w_pf_name.end());
    
    if (pf_name == ssid)
    {
      clarius_profile = pf_name;
      break;
    }
  }

  // free profiles memory
  if (profiles != NULL)
  {
    WlanFreeMemory(profiles);
    profiles = NULL;
  }

  // convert ssid string to DOT11_SSID
  DOT11_SSID dot11_ssid = { 0 };
  if (this->StringToSsid(ssid, &dot11_ssid) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to convert ssid string to DOT11_SSID type");
    return PLUS_FAIL;
  }

  // connect
  WLAN_CONNECTION_PARAMETERS wlan_params;
  wlan_params.wlanConnectionMode = wlan_connection_mode_profile;
  std::wstring w_clarius_profile(clarius_profile.begin(), clarius_profile.end());
  wlan_params.strProfile = w_clarius_profile.c_str();
  wlan_params.pDot11Ssid = &dot11_ssid;
  wlan_params.dot11BssType = dot11_BSS_type_infrastructure;
  wlan_params.pDesiredBssidList = NULL;
  wlan_params.dwFlags = 0;
  res = WlanConnect(this->HClient, &this->InterfaceGuid, &wlan_params, NULL);
  if (res != ERROR_SUCCESS)
  {
    LOG_ERROR("Failed to connect to Clarius wifi network");
    return PLUS_FAIL;
  }

  this->Connected = true;
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus ClariusWifi::DisconnectFromClariusWifi()
{
  // check if currently connected
  if (!this->Connected)
  {
    return PLUS_SUCCESS;
  }

  // check ClariusWifi was initialized
  if (this->HClient == NULL)
  {
    LOG_ERROR("ClariusWifi must be initialized before calling IsClariusNetworkReady");
    return PLUS_FAIL;
  }

  // disconnect
  DWORD res = WlanDisconnect(this->HClient, &this->InterfaceGuid, NULL);
  if (res != ERROR_SUCCESS)
  {
    LOG_ERROR("Failed to disconnect from Clarius wifi network with error: " << res);
    return PLUS_FAIL;
  }

  this->Connected = false;
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus ClariusWifi::StringToSsid(std::string ssid, PDOT11_SSID pdot11_ssid)
{
  BYTE pb_ssid[DOT11_SSID_MAX_LENGTH + 1] = { 0 };

  std::wstring w_ssid(ssid.begin(), ssid.end());
  if (ssid.empty() || pdot11_ssid == NULL)
  {
    return PLUS_FAIL;
  }
  else
  {
    pdot11_ssid->uSSIDLength = WideCharToMultiByte(CP_ACP,
      0,
      w_ssid.c_str(),
      -1,
      (LPSTR)pb_ssid,
      sizeof(pb_ssid),
      NULL,
      NULL);

    pdot11_ssid->uSSIDLength--;
    memcpy(&pdot11_ssid->ucSSID, pb_ssid, pdot11_ssid->uSSIDLength);
  }

  return PLUS_SUCCESS;
}