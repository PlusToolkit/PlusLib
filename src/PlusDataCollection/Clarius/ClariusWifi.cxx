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

// max time ClariusWifi will wait for the Clarius network to become
// connected during call to CONNECT
static const float MAX_NETWORK_CONNECTED_WAIT_TIME_SEC = 10.0;

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

      std::string candidate_ssid = (char*)nw_info->dot11Ssid.ucSSID;
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
PlusStatus ClariusWifi::IsClariusNetworkConnected()
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

  WLAN_INTERFACE_STATE state = wlan_interface_state_not_ready;
  for (int i = 0; i < interfaces->dwNumberOfItems; i++)
  {
    if_info = (WLAN_INTERFACE_INFO*)&interfaces->InterfaceInfo[i];
    if (if_info->InterfaceGuid == this->InterfaceGuid)
    {
      // not the interface we are looking for
      state = if_info->isState;
      break;
    }
  }

  // free interfaces memory
  if (interfaces != NULL)
  {
    WlanFreeMemory(interfaces);
    interfaces = NULL;
  }

  switch (state)
  {
  case wlan_interface_state_not_ready:
    LOG_DEBUG("WiFi state: Not ready");
    break;
  case wlan_interface_state_connected:
    LOG_DEBUG("WiFi state: Connected");
    break;
  case wlan_interface_state_ad_hoc_network_formed:
    LOG_DEBUG("WiFi state: Ad hoc network formed");
    break;
  case wlan_interface_state_disconnecting:
    LOG_DEBUG("WiFi state: Disconnecting");
    break;
  case wlan_interface_state_disconnected:
    LOG_DEBUG("WiFi state: Disconnected");
    break;
  case wlan_interface_state_associating:
    LOG_DEBUG("WiFi state: Associating");
    break;
  case wlan_interface_state_discovering:
    LOG_DEBUG("WiFi state: Discovering");
    break;
  case wlan_interface_state_authenticating:
    LOG_DEBUG("WiFi state: Authenticating");
    break;
  default:
    LOG_DEBUG("WiFi state: Unknown state");
    break;
  }

  return state == wlan_interface_state_connected ? PLUS_SUCCESS : PLUS_FAIL;
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

  if (this->UpdateClariusWifiProfile(ssid, password) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to update Clarius wifi profile");
    return PLUS_FAIL;
  }

  // Convert ssid string to DOT11_SSID
  DOT11_SSID dot11_ssid = { 0 };
  if (this->StringToSsid(ssid, &dot11_ssid) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to convert ssid string to DOT11_SSID type");
    return PLUS_FAIL;
  }

  // Connect
  std::wstring w_ssid = std::wstring(ssid.begin(), ssid.end());
  WLAN_CONNECTION_PARAMETERS wlan_params;
  wlan_params.pDot11Ssid = NULL;
  wlan_params.strProfile = w_ssid.c_str();
  wlan_params.wlanConnectionMode = wlan_connection_mode_profile;
  wlan_params.pDesiredBssidList = NULL;
  wlan_params.dot11BssType = this->BssType;
  wlan_params.dwFlags = 0;

  DWORD res = WlanConnect(this->HClient, &this->InterfaceGuid, &wlan_params, NULL);
  if (res != ERROR_SUCCESS)
  {
    LOG_ERROR("Failed to connect to Clarius wifi network: Error code " << res);
    return PLUS_FAIL;
  }

  start_time = std::chrono::steady_clock::now();
  PlusStatus network_connected = PLUS_FAIL;
  while (network_connected != PLUS_SUCCESS)
  {
    // check we haven't exceeded the maximum wait time for Clarius network
    // to be connected
    std::chrono::steady_clock::time_point t = std::chrono::steady_clock::now();
    std::chrono::duration<double> dur = t - start_time;
    if (dur.count() > MAX_NETWORK_CONNECTED_WAIT_TIME_SEC)
    {
      LOG_ERROR("Waiting for Clarius wifi network to be ready timed out.");
      return PLUS_FAIL;
    }

    // pause for 1000ms, to avoid overhead of checking wifi availability at high
    // refresh rate
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // refresh network_connected state
    network_connected = this->IsClariusNetworkConnected();
  }

  this->Connected = true;
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus ClariusWifi::UpdateClariusWifiProfile(std::string ssid, std::string password)
{
  bool profileUpdated = false;

  PWLAN_AVAILABLE_NETWORK_LIST networks = NULL;
  PWLAN_AVAILABLE_NETWORK nw_info = NULL;
  DWORD res = WlanGetAvailableNetworkList(this->HClient, &this->InterfaceGuid,
    0, NULL, &networks);
  if (res != ERROR_SUCCESS)
  {
    LOG_WARNING("Failed to list available networks");
    return PLUS_FAIL;
  }

  for (int j = 0; j < networks->dwNumberOfItems; j++)
  {
    nw_info = (WLAN_AVAILABLE_NETWORK*)&networks->Network[j];

    std::string candidate_ssid = (char*)nw_info->dot11Ssid.ucSSID;
    if (ssid.compare(candidate_ssid) != 0)
    {
      continue;
    }

    std::wstring authenticationAlgorithm;
    switch (nw_info->dot11DefaultAuthAlgorithm)
    {
    case DOT11_AUTH_ALGO_80211_OPEN:
      authenticationAlgorithm = L"OPEN";
      break;
    case DOT11_AUTH_ALGO_80211_SHARED_KEY:
      authenticationAlgorithm = L"SHARED";
      break;
    case DOT11_AUTH_ALGO_WPA:
    case DOT11_AUTH_ALGO_WPA_PSK:
    case DOT11_AUTH_ALGO_WPA_NONE:
      authenticationAlgorithm = L"WPAPSK";
      break;
    case DOT11_AUTH_ALGO_RSNA:
    case DOT11_AUTH_ALGO_RSNA_PSK:
      authenticationAlgorithm = L"WPA2PSK";
      break;
    default:
      authenticationAlgorithm = L"UNKNOWN";
    }

    std::wstring encryptionAlgorithm;
    std::wstring keyType = L"passPhrase";
    switch (nw_info->dot11DefaultCipherAlgorithm)
    {
    case DOT11_CIPHER_ALGO_NONE:
      encryptionAlgorithm = L"NONE";
      break;
    case DOT11_CIPHER_ALGO_TKIP:
      encryptionAlgorithm = L"TKIP";
      break;

    case DOT11_CIPHER_ALGO_CCMP:
      encryptionAlgorithm = L"AES";
      break;
    default:
      encryptionAlgorithm = L"WEP";
      keyType = L"networkKey";
    }

    std::wstringstream ss_w_clarius_profile_xml;
    ss_w_clarius_profile_xml << "<?xml version=\"1.0\" encoding=\"US-ASCII\"?>" << std::endl;
    ss_w_clarius_profile_xml << "<WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\">" << std::endl;
    ss_w_clarius_profile_xml << "    <name>" << std::wstring(ssid.begin(), ssid.end()) << "</name>" << std::endl;
    ss_w_clarius_profile_xml << "    <SSIDConfig>" << std::endl;
    ss_w_clarius_profile_xml << "        <SSID>" << std::endl;
    ss_w_clarius_profile_xml << "            <name>" << std::wstring(ssid.begin(), ssid.end()) << "</name>" << std::endl;
    ss_w_clarius_profile_xml << "        </SSID>" << std::endl;
    ss_w_clarius_profile_xml << "    </SSIDConfig>" << std::endl;
    ss_w_clarius_profile_xml << "    <connectionType>ESS</connectionType>" << std::endl;
    ss_w_clarius_profile_xml << "    <connectionMode>auto</connectionMode>" << std::endl;
    ss_w_clarius_profile_xml << "    <autoSwitch>false</autoSwitch>" << std::endl;
    ss_w_clarius_profile_xml << "    <MSM>" << std::endl;
    ss_w_clarius_profile_xml << "        <security>" << std::endl;
    ss_w_clarius_profile_xml << "            <authEncryption>" << std::endl;
    ss_w_clarius_profile_xml << "                <authentication>" << authenticationAlgorithm << "</authentication>" << std::endl;
    ss_w_clarius_profile_xml << "                <encryption>" << encryptionAlgorithm << "</encryption>" << std::endl;
    ss_w_clarius_profile_xml << "                <useOneX>false</useOneX>" << std::endl;
    ss_w_clarius_profile_xml << "            </authEncryption>" << std::endl;
    if (nw_info->dot11DefaultCipherAlgorithm != DOT11_CIPHER_ALGO_NONE)
    {
      ss_w_clarius_profile_xml << "          <sharedKey>" << std::endl;
      ss_w_clarius_profile_xml << "              <keyType>" << keyType << "</keyType>" << std::endl;
      ss_w_clarius_profile_xml << "              <protected>false</protected>" << std::endl;
      ss_w_clarius_profile_xml << "              <keyMaterial>" << std::wstring(password.begin(), password.end()) << "</keyMaterial>" << std::endl;
      ss_w_clarius_profile_xml << "          </sharedKey>" << std::endl;
    }
    ss_w_clarius_profile_xml << "        </security>" << std::endl;
    ss_w_clarius_profile_xml << "    </MSM>" << std::endl;
    ss_w_clarius_profile_xml << "</WLANProfile>";
    std::wstring w_clarius_profile_xml = ss_w_clarius_profile_xml.str();
    LOG_DEBUG_W(L"WLAN Profile: \n" << ss_w_clarius_profile_xml.str());

    // Create/update the Wi-Fi profile
    DWORD reasonCode = 0;
    res = WlanSetProfile(this->HClient, &this->InterfaceGuid, 0, w_clarius_profile_xml.c_str(), NULL, TRUE, NULL, &reasonCode);
    if (res != ERROR_SUCCESS)
    {
      WCHAR reasonCodeStr[256] = L"\0";
      WlanReasonCodeToString(reasonCode, 256, reasonCodeStr, NULL);
      LOG_ERROR("Failed to set Wi-Fi profile: " << nw_info->strProfileName);
      LOG_ERROR("Error code: " << res);
      LOG_ERROR_W(L"Reason: (" << reasonCode << ") " << reasonCodeStr);
      continue;
    }

    this->BssType = nw_info->dot11BssType;
    profileUpdated = true;
    break;
  }

  // free networks memory
  if (networks != NULL)
  {
    WlanFreeMemory(networks);
    networks = NULL;
  }

  if (!profileUpdated)
  {
    LOG_ERROR("Failed to find Clarius Wi-Fi network");
    return PLUS_FAIL;
  }
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