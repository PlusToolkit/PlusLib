/*=Plus=header=begin======================================================
    Program: Plus
    Copyright (c) Verdure Imaging Inc, Stockton, California. All rights reserved.
    See License.txt for details.
    We would like to acknowledge Verdure Imaging Inc for generously open-sourcing
    this support for the Clarius OEM interface to the PLUS & Slicer communities.
=========================================================Plus=header=end*/

// local includes
#include "ClariusBLE.h"

// WinRT includes
#include <winrt/base.h>
#include <winrt/windows.devices.bluetooth.h>
#include <winrt/windows.devices.bluetooth.genericattributeprofile.h>
#include <winrt/windows.devices.enumeration.h>
#include <winrt/windows.foundation.h>
#include <winrt/windows.foundation.collections.h>
#include <winrt/windows.storage.h>
#include <winrt/windows.storage.streams.h>

// WinRT using directives
using namespace winrt;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Bluetooth::GenericAttributeProfile;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;

// STL includes
#include <atomic>
#include <codecvt>
#include <future>
#include <mutex>
#include <iomanip>
#include <locale>
#include <sstream>
#include <stdexcept>

// link to Windows Runtime library
#pragma comment(lib, "windowsapp")

// power service uuid (8C853B6A-2297-44C1-8277-73627C8D2ABC)
static const winrt::guid POWER_SERVICE_UUID
{ 0x8c853b6a, 0x2297, 0x44c1, { 0x82, 0x77, 0x73, 0x62, 0x7c, 0x8d, 0x2a, 0xbc } };

// power published characteristic uuid (8c853b6a-2297-44c1-8277-73627c8d2abd)
static const winrt::guid POWER_PUBLISHED_CHAR_UUID
{ 0x8c853b6a, 0x2297, 0x44c1, { 0x82, 0x77, 0x73, 0x62, 0x7c, 0x8d, 0x2a, 0xbd } };

// power request characteristic uuid (8c853b6a-2297-44c1-8277-73627c8d2abe)
static const winrt::guid POWER_REQUEST_CHAR_UUID
{ 0x8c853b6a, 0x2297, 0x44c1, { 0x82, 0x77, 0x73, 0x62, 0x7c, 0x8d, 0x2a, 0xbe } };

// wifi service uuid (f9eb3fae-947a-4e5b-ab7c-c799e91ed780)
static const winrt::guid WIFI_SERVICE_UUID
{ 0xf9eb3fae, 0x947a, 0x4e5b, { 0xab, 0x7c, 0xc7, 0x99, 0xe9, 0x1e, 0xd7, 0x80 } };

// wifi published characteristic uuid (f9eb3fae-947a-4e5b-ab7c-c799e91ed781)
static const winrt::guid WIFI_PUBLISHED_CHAR_UUID
{ 0xf9eb3fae, 0x947a, 0x4e5b, { 0xab, 0x7c, 0xc7, 0x99, 0xe9, 0x1e, 0xd7, 0x81 } };

// wifi request characteristic uuid (f9eb3fae-947a-4e5b-ab7c-c799e91ed782)
static const winrt::guid WIFI_REQUEST_CHAR_UUID
{ 0xf9eb3fae, 0x947a, 0x4e5b, { 0xab, 0x7c, 0xc7, 0x99, 0xe9, 0x1e, 0xd7, 0x82 } };

// max duration to wait for probe boot sequence to complete
static const uint64_t POWER_ON_TIMEOUT_SEC = 30;
static const uint64_t POWER_ON_POLL_INTERVAL_SEC = 1;

// max duration to block while waiting for a BLE operation to complete
static const uint64_t BLE_OP_TIMEOUT_SEC = 5;

static const int MAX_BLE_CONNECTION_ATTEMPTS = 20;

//-----------------------------------------------------------------------------
// free helper functions
//-----------------------------------------------------------------------------

// busy wait for IAsyncOperation to complete
template <typename TAsyncOp>
PlusStatus await_async(TAsyncOp op)
{
  std::promise<void> wait_prom;
  std::future<void> wait_future = wait_prom.get_future();

  auto busy_wait = [&](TAsyncOp op)
  {
    while (op.Status() == AsyncStatus::Started);

    // successful completion
    wait_prom.set_value();
  };

  busy_wait(op);

  std::future_status status = wait_future.wait_for(std::chrono::seconds(BLE_OP_TIMEOUT_SEC));
  switch (status)
  {
  case std::future_status::ready:
    break;
  case std::future_status::timeout:
    LOG_ERROR("Awaiting asynchronous operation timed out");
    break;
  case std::future_status::deferred:
    LOG_ERROR("Awaiting asynchronous operation deferred");
    break;
  default:
    LOG_ERROR("Unexpected error occurred while awaiting completion of C++/WinRT asynchronous operation");
    break;
  }

  // process error
  switch (op.Status())
  {
  case AsyncStatus::Completed:
    break;
  case AsyncStatus::Canceled:
    LOG_ERROR("Awaiting asynchronous operation which terminated with status AsyncStatus::Canceled");
    break;
  case AsyncStatus::Started:
    LOG_ERROR("Awaiting asynchronous operation which timed out with status AsyncStatus::Started");
    break;
  case AsyncStatus::Error:
    LOG_ERROR("Error occurred while awaiting asynchronous operation. Error code was: " << op.ErrorCode());
    break;
  default:
    LOG_ERROR("Unexpected error occurred causing timeout while awaiting completion of C++/WinRT asynchronous operation");
    break;
  }

  // async operation was successful
  return op.Status() == AsyncStatus::Completed ? PLUS_SUCCESS : PLUS_FAIL;
}

//-----------------------------------------------------------------------------
std::wstring to_wide_string(std::string str)
{
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  return converter.from_bytes(str);
}

//-----------------------------------------------------------------------------
std::string to_narrow_string(std::wstring wstr)
{
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  return converter.to_bytes(wstr);
}

//-----------------------------------------------------------------------------
std::string to_narrow_string(winrt::hstring hstr)
{
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  return converter.to_bytes(hstr.c_str());
}

//-----------------------------------------------------------------------------
// from https://github.com/bucienator/ble-win-cpp
std::string uuid_to_string(const winrt::guid& uuid)
{
  std::stringstream str;
  str << std::uppercase << std::hex;
  str << std::setw(8) << std::setfill('0') << uuid.Data1 << "-";
  str << std::setw(4) << std::setfill('0') << uuid.Data2 << "-";
  str << std::setw(4) << std::setfill('0') << uuid.Data3 << "-";
  str << std::setw(2) << std::setfill('0') << static_cast<short>(uuid.Data4[0])
    << std::setw(2) << std::setfill('0') << static_cast<short>(uuid.Data4[1])
    << '-'
    << std::setw(2) << std::setfill('0') << static_cast<short>(uuid.Data4[2])
    << std::setw(2) << std::setfill('0') << static_cast<short>(uuid.Data4[3])
    << std::setw(2) << std::setfill('0') << static_cast<short>(uuid.Data4[4])
    << std::setw(2) << std::setfill('0') << static_cast<short>(uuid.Data4[5])
    << std::setw(2) << std::setfill('0') << static_cast<short>(uuid.Data4[6])
    << std::setw(2) << std::setfill('0') << static_cast<short>(uuid.Data4[7]);
  str << std::nouppercase;
  return str.str();
}

//-----------------------------------------------------------------------------
// ClariusBLEPrivate declaration
//-----------------------------------------------------------------------------
class ClariusBLEPrivate
{
public:

  ClariusBLEPrivate(ClariusBLE* ext);

  virtual ~ClariusBLEPrivate() = default;

  // WinRT callback called whenever a new BLE device is discovered
  void DeviceAdded(DeviceWatcher sender, DeviceInformation deviceInfo);

  // WinRT callback called whenever the state of an existing found BLE device
  // is update
  // NOTE: it seems that without this function, the DeviceAdded callback
  // doesn't function correctly
  void DeviceUpdated(DeviceWatcher sender, DeviceInformationUpdate devInfoUpdate);

  // WinRT callback subscribed to changes in the PowerPublished characteristic
  void PowerStateChanged(GattCharacteristic sender, GattValueChangedEventArgs args);

  // WinRT callback subscribed to changes in the WifiPublished characteristic
  void WifiStateChanged(GattCharacteristic sender, GattValueChangedEventArgs args);

  // setup function for the Clarius power service and related characteristics
  PlusStatus SetupPowerService();

  // setup function for the Clarius WiFi service and related characteristics
  PlusStatus SetupWifiService();

  // call this immediately after SetupPowerService / SetupWifiService members
  // to ensure state is initialized correctly
  PlusStatus InitializeState();

  // thread safe accessor for wifi info
  ClariusWifiInfo GetWifiInfo();

  // convert GattCommunicationStatus enum to string representations
  std::string GattCommunicationStatusToString(GattCommunicationStatus status);

  // parses Clarius WiFi info string into this->WifiInfo
  void ProcessWifiInfo(std::string info);

  // members

  // pointer to ClariusBLE instance
  ClariusBLE* External;

  // desired Clarius probe ID (serial-number)
  std::wstring ProbeId;

  // list of nearby probes populated by call to FindBySerial / the DeviceAdded callback
  std::vector<std::string> FoundProbeIds;

  // last error message
  std::string LastError;

  // BLE device
  DeviceInformation DeviceInfo{ nullptr };
  std::promise<void> DeviceInfoPromise;
  BluetoothLEDevice Device{ nullptr };

  // BLE services
  GattDeviceService PowerService{ nullptr };
  GattDeviceService WifiService{ nullptr };

  // BLE characteristics
  GattCharacteristic PowerPublishedChar{ nullptr };
  GattCharacteristic PowerRequestChar{ nullptr };
  GattCharacteristic WifiPublishedChar{ nullptr };
  GattCharacteristic WifiRequestChar{ nullptr };

  // power state
  std::atomic<bool> PowerState;

  // wifi state
  std::promise<void> WifiInfoPromise; // use to await setting on power-up
  std::atomic<bool> WifiInfoSet;      // use to check if already set

private:

  // helper to retrieve a GattDeviceService by UUID
  PlusStatus RetrieveService(const guid& serviceUuid, GattDeviceService& service);

  // helper to retrieve a GattCharacteristic from a service by UUID
  PlusStatus RetrieveCharacteristic(
    const GattDeviceService& service,
    const guid charUuid,
    GattCharacteristic& characteristic
  );

  // helper for ProcessWifiInfo to split string up
  std::vector<std::string> TokenizeString(std::string str, const char delimiter);

  // wifi connection information for Clarius probe
  std::mutex WifiInfoMutex;
  ClariusWifiInfo WifiInfo;
};

//-----------------------------------------------------------------------------
// ClariusBLEPrivate method definitions
//-----------------------------------------------------------------------------
ClariusBLEPrivate::ClariusBLEPrivate(ClariusBLE* ext)
  : External(ext)
  , PowerState(false)
  , WifiInfoSet(false)
{
}

//-----------------------------------------------------------------------------
void ClariusBLEPrivate::DeviceAdded(
  DeviceWatcher sender,
  DeviceInformation info)
{
  (void)sender;

  std::wstring name{ info.Name().c_str() };
  if (name == this->ProbeId)
  {
    this->DeviceInfo = info;
    this->DeviceInfoPromise.set_value();
  }
  else if (name.find(L"CUS") != std::wstring::npos)
  {
    this->FoundProbeIds.push_back(to_narrow_string(name));
  }
}

//-----------------------------------------------------------------------------
void ClariusBLEPrivate::DeviceUpdated(
  winrt::Windows::Devices::Enumeration::DeviceWatcher sender,
  winrt::Windows::Devices::Enumeration::DeviceInformationUpdate devInfoUpdate)
{
  (void)sender;
  (void)devInfoUpdate;
}

//-----------------------------------------------------------------------------
void ClariusBLEPrivate::PowerStateChanged(GattCharacteristic sender, GattValueChangedEventArgs args)
{
  DataReader reader = DataReader::FromBuffer(args.CharacteristicValue());
  uint8_t powered = reader.ReadByte();
  this->PowerState = bool(powered);
}

//-----------------------------------------------------------------------------
void ClariusBLEPrivate::WifiStateChanged(GattCharacteristic sender, GattValueChangedEventArgs args)
{
  DataReader reader = DataReader::FromBuffer(args.CharacteristicValue());
  winrt::hstring wifiHString = reader.ReadString(reader.UnconsumedBufferLength());
  std::string wifiStr = to_narrow_string(wifiHString);
  this->ProcessWifiInfo(wifiStr);

  // signal info ready
  this->WifiInfoPromise.set_value();
}

//-----------------------------------------------------------------------------
PlusStatus ClariusBLEPrivate::SetupPowerService()
{
  LOG_DEBUG("Setting up Clarius power service");

  if (this->RetrieveService(POWER_SERVICE_UUID, this->PowerService) != PLUS_SUCCESS)
  {
    // last error already set
    return PLUS_FAIL;
  }

  if (this->RetrieveCharacteristic(this->PowerService, POWER_PUBLISHED_CHAR_UUID, this->PowerPublishedChar) != PLUS_SUCCESS)
  {
    // last error already set
    return PLUS_FAIL;
  }

  if (this->RetrieveCharacteristic(this->PowerService, POWER_REQUEST_CHAR_UUID, this->PowerRequestChar) != PLUS_SUCCESS)
  {
    // last error already set
    return PLUS_FAIL;
  }

  if (!this->PowerPublishedChar)
  {
    this->LastError = "Could not find PowerPublished characteristic";
    return PLUS_FAIL;
  }

  if (!this->PowerRequestChar)
  {
    this->LastError = "Could not find PowerRequest characteristic";
    return PLUS_FAIL;
  }

  // subscribe to power state changes
  this->PowerPublishedChar.WriteClientCharacteristicConfigurationDescriptorAsync(
    GattClientCharacteristicConfigurationDescriptorValue::Notify);
  this->PowerPublishedChar.ValueChanged({ this, &ClariusBLEPrivate::PowerStateChanged });

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus ClariusBLEPrivate::SetupWifiService()
{
  LOG_DEBUG("Setting up Clarius wifi service");

  if (this->RetrieveService(WIFI_SERVICE_UUID, this->WifiService) != PLUS_SUCCESS)
  {
    // last error already set
    return PLUS_FAIL;
  }

  if (this->RetrieveCharacteristic(this->WifiService, WIFI_PUBLISHED_CHAR_UUID, this->WifiPublishedChar) != PLUS_SUCCESS)
  {
    // last error already set
    return PLUS_FAIL;
  }

  if (this->RetrieveCharacteristic(this->WifiService, WIFI_REQUEST_CHAR_UUID, this->WifiRequestChar) != PLUS_SUCCESS)
  {
    // last error already set
    return PLUS_FAIL;
  }

  if (!this->WifiPublishedChar)
  {
    this->LastError = "Could not find WifiPublished characteristic";
    return PLUS_FAIL;
  }

  if (!this->WifiRequestChar)
  {
    this->LastError = "Could not find WifiRequest characteristic";
    return PLUS_FAIL;
  }

  // subscribe to wifi state changes
  // TODO: Even after subscribing, WifiStateChanged callback doesn't seem to be called.
  this->WifiPublishedChar.WriteClientCharacteristicConfigurationDescriptorAsync(
    GattClientCharacteristicConfigurationDescriptorValue::Notify);
  this->WifiPublishedChar.ValueChanged({ this, &ClariusBLEPrivate::WifiStateChanged });

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus ClariusBLEPrivate::InitializeState()
{
  LOG_DEBUG("Initializing Clarius BLE state");

  // initialize power state
  IAsyncOperation<GattReadResult> powerOp =
    this->PowerPublishedChar.ReadValueAsync(BluetoothCacheMode::Uncached);
  if (await_async(powerOp) != PLUS_SUCCESS)
  {
    this->LastError = "Failed to async read power state from BLE device";
    return PLUS_FAIL;
  }

  GattReadResult powerResult = powerOp.GetResults();
  if (powerResult.Status() != GattCommunicationStatus::Success)
  {
    std::stringstream msg;
    msg << "Failed to initialize power state in InitializeState with non-successful "
      "GATT communication. Status was: "
      << this->GattCommunicationStatusToString(powerResult.Status());
    this->LastError = msg.str();
    return PLUS_FAIL;
  }

  DataReader powerReader = DataReader::FromBuffer(powerResult.Value());
  this->PowerState = bool(powerReader.ReadByte());

  if (!this->PowerState)
  {
    // Clarius not powered, wifi info will be initialized correctly on power
    // up completion
    return PLUS_SUCCESS;
  }

  // initialize wifi state
  IAsyncOperation<GattReadResult> wifiOp =
    this->WifiPublishedChar.ReadValueAsync(BluetoothCacheMode::Uncached);
  if (await_async(wifiOp) != PLUS_SUCCESS)
  {
    this->LastError = "Failed to async read wifi state from BLE device";
    return PLUS_FAIL;
  }

  GattReadResult wifiResult = wifiOp.GetResults();
  if (wifiResult.Status() != GattCommunicationStatus::Success)
  {
    std::stringstream msg;
    msg << "Failed to initialize wifi state in InitializeState with non-successful "
      "GATT communication. Status was: " << this->GattCommunicationStatusToString(wifiResult.Status());
    this->LastError = msg.str();
    return PLUS_FAIL;
  }

  DataReader wifiReader = DataReader::FromBuffer(wifiResult.Value());
  winrt::hstring wifiHString = wifiReader.ReadString(wifiReader.UnconsumedBufferLength());
  std::string wifiStr = to_narrow_string(wifiHString);
  this->ProcessWifiInfo(wifiStr);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
ClariusWifiInfo ClariusBLEPrivate::GetWifiInfo()
{
  std::unique_lock<std::mutex> wifiInfoLock(this->WifiInfoMutex);
  return this->WifiInfo;
}

//-----------------------------------------------------------------------------
std::string ClariusBLEPrivate::GattCommunicationStatusToString(GattCommunicationStatus status)
{
  switch (status)
  {
  case GattCommunicationStatus::AccessDenied:
    return "AccessDenied";
  case GattCommunicationStatus::ProtocolError:
    return "ProtocolError";
  case GattCommunicationStatus::Success:
    return "Success";
  case GattCommunicationStatus::Unreachable:
    return "Unreachable";
  default:
    return "Unknown GattCommunicationStatus";
  }
}

//-----------------------------------------------------------------------------
PlusStatus ClariusBLEPrivate::RetrieveService(const guid& serviceUuid, GattDeviceService& service)
{
  IAsyncOperation<GattDeviceServicesResult> getServicesOp =
    this->Device.GetGattServicesForUuidAsync(serviceUuid, BluetoothCacheMode::Uncached);
  if (await_async(getServicesOp) != PLUS_SUCCESS)
  {
    this->LastError = "ClariusBLEPrivate::RetrieveService async operation failed";
    return PLUS_FAIL;
  }

  GattDeviceServicesResult servicesResult = getServicesOp.GetResults();

  // check that GetGattServicesForUuidAsync returned a successful status
  if (servicesResult.Status() != GattCommunicationStatus::Success)
  {
    std::stringstream msg;
    msg << "ClariusBLEPrivate::RetrieveService failed for UUID " << uuid_to_string(serviceUuid)
      << " with non-successful GATT communication. Status was: "
      << this->GattCommunicationStatusToString(servicesResult.Status());
    this->LastError = msg.str();
    return PLUS_FAIL;
  }

  // check that at least one service was found for this uuid
  if (!servicesResult.Services().Size())
  {
    std::stringstream msg;
    msg << "ClariusBLEPrivate::RetrieveService failed for UUID " << uuid_to_string(serviceUuid)
      << ". No services with this UUID were found.";
    this->LastError = msg.str();
    return PLUS_FAIL;
  }

  service = *servicesResult.Services().First();
  if (!service)
  {
    std::stringstream msg;
    msg << "ClariusBLEPrivate::RetrieveService could not find service " << uuid_to_string(serviceUuid);
    this->LastError = msg.str();
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus ClariusBLEPrivate::RetrieveCharacteristic(
  const GattDeviceService& service,
  const guid charUuid,
  GattCharacteristic& characteristic)
{
  IAsyncOperation<GattCharacteristicsResult> getCharsOp =
    service.GetCharacteristicsForUuidAsync(charUuid, BluetoothCacheMode::Uncached);
  if (await_async(getCharsOp) != PLUS_SUCCESS)
  {
    std::stringstream msg;
    msg << "ClariusBLEPrivate::RetrieveCharacteristic async operation failed for UUID";
    msg << uuid_to_string(charUuid);
    this->LastError = msg.str();
    return PLUS_FAIL;
  }

  // check that GetCharacteristicsForUuidAsync returned a successful status
  GattCharacteristicsResult charsResult = getCharsOp.GetResults();
  if (charsResult.Status() != GattCommunicationStatus::Success)
  {
    std::stringstream msg;
    msg << "ClariusBLEPrivate::RetrieveCharacteristic failed for UUID " << uuid_to_string(charUuid)
      << " with non-successful GATT communication. Status was: "
      << this->GattCommunicationStatusToString(charsResult.Status());
    this->LastError = msg.str();
    return PLUS_FAIL;
  }

  // check that at least one characteristic was found for this uuid
  if (!charsResult.Characteristics().Size())
  {
    std::stringstream msg;
    msg << "ClariusBLEPrivate::RetrieveCharacteristic failed for UUID " << uuid_to_string(charUuid)
      << ". No characteristics with this UUID were found.";
    this->LastError = msg.str();
    return PLUS_FAIL;
  }

  characteristic = *charsResult.Characteristics().First();
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void ClariusBLEPrivate::ProcessWifiInfo(std::string info)
{
  std::unique_lock<std::mutex> wifiInfoLock(this->WifiInfoMutex);
  std::vector<std::string> infoList = this->TokenizeString(info, '\n');
  if (infoList.size() == 1)
  {
    // wifi disabled, so only state returned
    if (infoList.at(0).find("state") == std::string::npos)
    {
      throw std::runtime_error("Format of ClariusWifiInfo string has changed. Please report "
        "this info the the PLUS developers");
    }

    ClariusWifiInfo newInfo;
    newInfo.Ready = false;
    this->WifiInfo = newInfo;
    return;
  }
  else if (infoList.size() != 10)
  {
    throw std::runtime_error("Format of ClariusWifiInfo string has changed. Please report "
      "this info the the PLUS developers");
  }

  ClariusWifiInfo newInfo;

  // parse IsConnected (= Clarius "state")
  std::string stateStr = infoList.at(0);
  newInfo.Ready = (stateStr.find("connected") != std::string::npos);

  // parse IsAvailable
  std::string availStr = infoList.at(7);
  std::string availTag = "avail: ";
  availStr = availStr.replace(0, availTag.length(), "");
  if (availStr == "available")
  {
    newInfo.Available = ClariusAvailability::AVAILABLE;
  }
  else if (availStr == "listen")
  {
    newInfo.Available = ClariusAvailability::LISTEN;
  }
  else
  {
    newInfo.Available = ClariusAvailability::NOT_AVAILABLE;
  }

  // parse wifi Mode
  std::string apStr = infoList.at(1);
  if (apStr.find("true") != std::string::npos)
  {
    newInfo.WifiMode = ClariusWifiMode::ACCESS_POINT;
  }
  else
  {
    newInfo.WifiMode = ClariusWifiMode::LAN;
  }

  // parse SSID
  std::string ssidStr = infoList.at(2);
  std::string ssidTag = "ssid: ";
  newInfo.SSID = ssidStr.replace(0, ssidTag.length(), "");

  // parse Password
  std::string pwStr = infoList.at(3);
  std::string pwTag = "pw: ";
  newInfo.Password = pwStr.replace(0, pwTag.length(), "");

  // parse IPv4
  std::string ipv4Str = infoList.at(4);
  std::string ipv4Tag = "ip4: ";
  newInfo.IPv4 = ipv4Str.replace(0, ipv4Tag.length(), "");

  // parse MacAddress
  std::string macStr = infoList.at(9);
  std::string macTag = "mac: ";
  newInfo.MacAddress = macStr.replace(0, macTag.length(), "");

  // parse ControlPort
  std::string ctlStr = infoList.at(5);
  std::string ctlTag = "ctl: ";
  std::string ctlStrInt = ctlStr.replace(0, ctlTag.length(), "");
  newInfo.ControlPort = std::stoi(ctlStrInt);

  // parse CastPort
  std::string castStr = infoList.at(6);
  std::string castTag = "cast: ";
  std::string castStrInt = castStr.replace(0, castTag.length(), "");
  newInfo.CastPort = std::stoi(castStrInt);

  // parse Channel
  std::string channelStr = infoList.at(8);
  std::string channelTag = "channel: ";
  std::string channelStrInt = channelStr.replace(0, channelTag.length(), "");
  newInfo.Channel = std::stoi(channelStrInt);

  // set WifiInfo member variable and set variable
  this->WifiInfo = newInfo;
  this->WifiInfoSet = true;
}

//-----------------------------------------------------------------------------
std::vector<std::string> ClariusBLEPrivate::TokenizeString(std::string str, const char delimiter)
{
  std::stringstream ss(str);
  std::vector<std::string> tokens;
  std::string tmp;

  while (getline(ss, tmp, delimiter))
  {
    tokens.push_back(tmp);
  }

  return tokens;
}

//-----------------------------------------------------------------------------
// ClariusBLE method definitions
//-----------------------------------------------------------------------------
ClariusBLE::ClariusBLE()
  : _impl(std::make_unique<ClariusBLEPrivate>(this))
{
}

//-----------------------------------------------------------------------------
ClariusBLE::~ClariusBLE()
{
  this->RequestProbeOff();
  this->Disconnect();
}

//-----------------------------------------------------------------------------
PlusStatus ClariusBLE::FindBySerial(std::string serialNum)
{
  std::string fullBleName = "CUS-" + serialNum;
  _impl->ProbeId = to_wide_string(fullBleName);

  DeviceWatcher deviceWatcher{ nullptr };
  winrt::hstring aqsFilter{ BluetoothLEDevice::GetDeviceSelectorFromPairingState(true) };
  deviceWatcher = DeviceInformation::CreateWatcher(
    aqsFilter,
    nullptr,
    DeviceInformationKind::AssociationEndpoint
  );
  deviceWatcher.Added({ _impl.get(), &ClariusBLEPrivate::DeviceAdded });
  deviceWatcher.Updated({ _impl.get(), &ClariusBLEPrivate::DeviceUpdated });

  std::future<void> deviceInfoFuture = _impl->DeviceInfoPromise.get_future();
  deviceWatcher.Start();

  if (deviceInfoFuture.wait_for(std::chrono::milliseconds(1000)) == std::future_status::ready)
  {
    deviceWatcher.Stop();
    return PLUS_SUCCESS;
  }

  deviceWatcher.Stop();
  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------
std::vector<std::string> ClariusBLE::RetrieveFoundProbeIds()
{
  return _impl->FoundProbeIds;
}

//-----------------------------------------------------------------------------
bool ClariusBLE::IsProbeConnected()
{
  return (_impl->Device && _impl->Device.ConnectionStatus() == BluetoothConnectionStatus::Connected);
}

//-----------------------------------------------------------------------------
PlusStatus ClariusBLE::Connect()
{
  if (!_impl->DeviceInfo)
  {
    _impl->LastError = "Attempted to call ClariusBLE::Connect with unset m_deviceInfo, "
      "ensure ClariusBLE::FindBySerial is called successfully before calling Connect";
    return PLUS_FAIL;
  }
  if (_impl->Device && _impl->Device.ConnectionStatus() == BluetoothConnectionStatus::Connected)
  {
    _impl->LastError = "Connect called but probe is already connected";
    return PLUS_FAIL;
  }

  // get BLE device
  PlusStatus success = PLUS_FAIL;

  int connectionAttemptCount = 0;
  int retryDelayMs = 1000;
  while (connectionAttemptCount < MAX_BLE_CONNECTION_ATTEMPTS && !success)
  {
    if (connectionAttemptCount > 0)
    {
      LOG_DEBUG("Attempt #" << connectionAttemptCount << " failed. Last error: \"" << this->GetLastError() << "\"");
      this->CloseConnection();
      std::this_thread::sleep_for(std::chrono::milliseconds(retryDelayMs));
    }

    ++connectionAttemptCount;
    LOG_DEBUG("Trying to connect: Attempt #" << connectionAttemptCount);

    IAsyncOperation<BluetoothLEDevice> deviceOp = BluetoothLEDevice::FromIdAsync(_impl->DeviceInfo.Id());
    if (await_async(deviceOp) != PLUS_SUCCESS)
    {
      _impl->LastError = "Failed to connect to device, async operation to get device failed";
      continue;
    }
    _impl->Device = deviceOp.GetResults();
    if (_impl->Device == nullptr)
    {
      _impl->LastError = "Failed to connect to device, unable to find device";
      continue;
    }

    // setup power & wifi services
    if (_impl->SetupPowerService() != PLUS_SUCCESS)
    {
      continue;
    }
    if (_impl->SetupWifiService() != PLUS_SUCCESS)
    {
      continue;
    }

    // initialize power state & wifi info correctly
    if (_impl->InitializeState() != PLUS_SUCCESS)
    {
      continue;
    }

    success = PLUS_SUCCESS;
    LOG_DEBUG("Device connected: Attempt #" << connectionAttemptCount);
  }

  return success;
}

//-----------------------------------------------------------------------------
PlusStatus ClariusBLE::CloseConnection()
{
  if (_impl->PowerService)
  {
    _impl->PowerService.Close();
  }
  if (_impl->WifiService)
  {
    _impl->WifiService.Close();
  }
  if (_impl->Device)
  {
    _impl->Device.Close();
  }

  _impl->PowerPublishedChar = nullptr;
  _impl->PowerRequestChar = nullptr;
  _impl->WifiPublishedChar = nullptr;
  _impl->WifiRequestChar = nullptr;

  _impl->PowerService = nullptr;
  _impl->WifiService = nullptr;

  _impl->Device = nullptr;

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus ClariusBLE::Disconnect()
{
  if (!IsProbeConnected())
  {
    // already disconnected, nothing to do here...
    return PLUS_SUCCESS;
  }

  this->CloseConnection();

  _impl->DeviceInfo = nullptr;

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
bool ClariusBLE::IsProbePowered()
{
  return _impl->PowerState;
}

//-----------------------------------------------------------------------------
PlusStatus ClariusBLE::RequestProbeOn()
{
  if (!IsProbeConnected())
  {
    _impl->LastError = "RequestProbeOn called but no probe is connected";
    return PLUS_FAIL;
  }

  DataWriter writer;
  writer.WriteByte(0x01); // 0x01 = request power on
  IBuffer buf = writer.DetachBuffer();

  IAsyncOperation<GattWriteResult> writeOp =
    _impl->PowerRequestChar.WriteValueWithResultAsync(buf, GattWriteOption::WriteWithResponse);
  if (await_async(writeOp) != PLUS_SUCCESS)
  {
    _impl->LastError = "RequestProbeOn Failed to async write to PowerRequestChar";
    return PLUS_FAIL;
  }

  GattWriteResult writeResult = writeOp.GetResults();
  if (writeResult.Status() != GattCommunicationStatus::Success)
  {
    std::stringstream msg;
    msg << "ClariusBLE::ProbeOn failed with non-successful GATT communication. Status was: "
      << _impl->GattCommunicationStatusToString(writeResult.Status());
    _impl->LastError = msg.str();
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus ClariusBLE::RequestProbeOff()
{
  if (!IsProbeConnected())
  {
    _impl->LastError = "RequestProbeOff called but no probe is connected";
    return PLUS_FAIL;
  }

  DataWriter writer;
  writer.WriteByte(0x00); // 0x00 = request power off
  IBuffer buf = writer.DetachBuffer();

  if (_impl->PowerRequestChar)
  {
    IAsyncOperation<GattWriteResult> writeOp =
      _impl->PowerRequestChar.WriteValueWithResultAsync(buf, GattWriteOption::WriteWithResponse);
    if (await_async(writeOp) != PLUS_SUCCESS)
    {
      _impl->LastError = "RequestProbeOff Failed to async write to PowerRequestChar";
      return PLUS_FAIL;
    }

    GattWriteResult writeResult = writeOp.GetResults();

    if (writeResult.Status() != GattCommunicationStatus::Success)
    {
      std::stringstream msg;
      msg << "ClariusBLE::ProbeOff failed with non-successful GATT communication. Status was: "
        << _impl->GattCommunicationStatusToString(writeResult.Status());
      _impl->LastError = msg.str();
      return PLUS_FAIL;
    }
  }
  else
  {
    LOG_ERROR("Unable to send probe off signal. Power request characteristic not availiable.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
bool ClariusBLE::AwaitWifiInfoReady()
{
  if (_impl->WifiInfoSet)
  {
    // wifi info has already been set
    return true;
  }

  // check to see if wifi info is already availiable in the case that the probe is already on
  // and wifi is availiable
  this->ReadWifiInfo();
  if (_impl->WifiInfoSet)
  {
    return true;
  }

  // wait for wifi info to be set
  std::future<void> wifiInfoFuture = _impl->WifiInfoPromise.get_future();

  std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
  std::chrono::duration<double> duration;
  do
  {
    // TODO: WifiStateChanged callback is not currently being called.
    // After waiting for the timeout, try once to see if WifiPublishedChar has been updated.
    this->ReadWifiInfo();
    if (_impl->WifiInfoSet || wifiInfoFuture.wait_for(std::chrono::seconds(POWER_ON_POLL_INTERVAL_SEC)) == std::future_status::ready)
    {
      duration = std::chrono::steady_clock::now() - start_time;
      LOG_DEBUG("WiFi info set: " << duration.count() << "s");
      return true;
    }

    duration = std::chrono::steady_clock::now() - start_time;

    LOG_DEBUG("Waiting for WiFi info: " << duration.count() << "s");
  } while (duration.count() < POWER_ON_TIMEOUT_SEC);

  std::stringstream msg;
  msg << "Clarius probe took longer than the maximum allowed " << POWER_ON_TIMEOUT_SEC
    << " to boot up and provide Wifi info, please try again" << std::endl;
  _impl->LastError = msg.str();
  return false;
}

//-----------------------------------------------------------------------------
PlusStatus ClariusBLE::ReadWifiInfo()
{
  IAsyncOperation<GattReadResult> wifiOp =
    _impl->WifiPublishedChar.ReadValueAsync(BluetoothCacheMode::Uncached);
  if (await_async(wifiOp) == PLUS_SUCCESS)
  {
    GattReadResult wifiResult = wifiOp.GetResults();
    if (wifiResult.Status() != GattCommunicationStatus::Success)
    {
      std::stringstream msg;
      msg << "ClariusBLE::ReadWifiInfo failed with non-successful GATT communication. Status was: "
        << _impl->GattCommunicationStatusToString(wifiResult.Status());
      _impl->LastError = msg.str();
      return PLUS_FAIL;
    }

    DataReader wifiReader = DataReader::FromBuffer(wifiResult.Value());
    winrt::hstring wifiHString = wifiReader.ReadString(wifiReader.UnconsumedBufferLength());
    std::string wifiStr = to_narrow_string(wifiHString);
    _impl->ProcessWifiInfo(wifiStr);
    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------
PlusStatus ClariusBLE::ConfigureWifiAP()
{
  if (!IsProbeConnected())
  {
    _impl->LastError = "ConfigureWifiAP called but no probe is connected";
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ss << "ap: true\n";
  ss << "channel: auto\n";

  DataWriter writer;
  writer.WriteString(to_wide_string(ss.str()));
  IBuffer buf = writer.DetachBuffer();

  IAsyncOperation<GattWriteResult> writeOp =
    _impl->WifiRequestChar.WriteValueWithResultAsync(buf, GattWriteOption::WriteWithResponse);
  if (await_async(writeOp) != PLUS_SUCCESS)
  {
    _impl->LastError = "ConfigureWifiAP Failed to write to WifiRequestChar";
    return PLUS_FAIL;
  }

  GattWriteResult writeResult = writeOp.GetResults();
  if (writeResult.Status() != GattCommunicationStatus::Success)
  {
    std::stringstream msg;
    msg << "ClariusBLE::ConfigureWifiAP failed with non-successful GATT communication. Status was: "
      << _impl->GattCommunicationStatusToString(writeResult.Status());
    _impl->LastError = msg.str();
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus ClariusBLE::ConfigureWifiLAN(std::string ssid, std::string password)
{
  if (!IsProbeConnected())
  {
    _impl->LastError = "ConfigureWifiLAN called but no probe is connected";
    return PLUS_FAIL;
  }

  if (!ssid.length())
  {
    _impl->LastError = "ClariusBLE::ConfigureWifiLAN called with ssid parameter of invalid length (0)";
    return PLUS_FAIL;
  }
  if (!password.length())
  {
    _impl->LastError = "ClariusBLE::ConfigureWifiLAN called with password parameter of invalid length (0)";
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ss << "ap: false\n";
  ss << "ssid: " << ssid << "\n";
  ss << "pw: " << password << "\n";

  DataWriter writer;
  writer.WriteString(to_wide_string(ss.str()));
  IBuffer buf = writer.DetachBuffer();

  IAsyncOperation<GattWriteResult> writeOp =
    _impl->PowerRequestChar.WriteValueWithResultAsync(buf, GattWriteOption::WriteWithResponse);
  if (await_async(writeOp) != PLUS_SUCCESS)
  {
    _impl->LastError = "ConfigureWifiLAN failed to write to WifiRequestChar";
    return PLUS_FAIL;
  }

  GattWriteResult writeResult = writeOp.GetResults();
  if (writeResult.Status() != GattCommunicationStatus::Success)
  {
    std::stringstream msg;
    msg << "ClariusBLE::ConfigureWifiLAN failed with non-successful GATT communication. Status was: "
      << _impl->GattCommunicationStatusToString(writeResult.Status());
    _impl->LastError = msg.str();
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
std::pair<PlusStatus, ClariusWifiInfo> ClariusBLE::GetWifiInfo()
{
  if (!_impl->WifiInfoSet)
  {
    _impl->LastError = "Clarius wifi info not set yet...";
    return { PLUS_FAIL, {} };
  }

  return { PLUS_SUCCESS, _impl->GetWifiInfo() };
}

//-----------------------------------------------------------------------------
std::string ClariusBLE::GetLastError()
{
  return _impl->LastError;
}
