/*=Plus=header=begin======================================================
    Program: Plus
    Copyright (c) Verdure Imaging Inc, Stockton, California. All rights reserved.
    See License.txt for details.

    We would like to acknowledge Verdure Imaging Inc for generously open-sourcing
    this support for the Clarius OEM interface to the PLUS & Slicer communities.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusChannel.h"
#include "vtkPlusClariusOEM.h"
#include "PixelCodec.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusUsImagingParameters.h"

// Wifi & BLE helpers
#include "ClariusBLE.h"
#include "ClariusWifi.h"

// Clarius Includes
#include <oem.h>
#include <oem_def.h>

// VTK includes
#include <vtkImageData.h>
#include <vtkInformation.h>
#include <vtkMatrix4x4.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>

// STL includes
#include <algorithm>
#include <cctype>
#include <chrono>
#include <future>
#include <map>
#include <sstream>
#include <string>

// Clarius enable / disable values
#define CLARIUS_TRUE 1
#define CLARIUS_FALSE 0
#define CLARIUS_RUN 1
#define CLARIUS_STOP 0

// Clarius connection state values
#define CLARIUS_STATE_NOT_INITIALIZED -1
#define CLARIUS_STATE_NOT_CONNECTED 0
#define CLARIUS_STATE_CONNECTED 1

//-------------------------------------------------------------------------------------------------
// instance memory
vtkPlusClariusOEM* vtkPlusClariusOEM::instance;

//-------------------------------------------------------------------------------------------------
// free helper functions
//-------------------------------------------------------------------------------------------------
std::string to_string(ClariusAvailability avail)
{
  switch (avail)
  {
  case ClariusAvailability::AVAILABLE:
    return "Available";
  case ClariusAvailability::LISTEN:
    return "Listen";
  case ClariusAvailability::NOT_AVAILABLE:
    return "Not Available";
  default:
    return "Unknown ClariusAvailability";
  }
}

//-------------------------------------------------------------------------------------------------
std::string to_string(ClariusWifiMode mode)
{
  switch (mode)
  {
  case ClariusWifiMode::ACCESS_POINT:
    return "Access Point";
  case ClariusWifiMode::LAN:
    return "Local Area Network";
  default:
    return "Unknown ClariusWifiMode";
  }
}

//-------------------------------------------------------------------------------------------------
namespace
{
  enum class BUTTON_MODE
  {
    DISABLED = CusButtonSetting::ButtonDisabled,
    FREEZE = CusButtonSetting::ButtonFreeze,
    USER = CusButtonSetting::ButtonUser
  };

  static std::map<BUTTON_MODE, std::string> ButtonModeEnumToString{
    {BUTTON_MODE::FREEZE, "FREEZE"},
    {BUTTON_MODE::USER, "USER"},
    {BUTTON_MODE::DISABLED, "DISABLED"}
  };

  static const double CM_TO_MM = 10.0;
  static const double MM_TO_CM = 0.1;
  static const double UM_TO_MM = 0.001;

  static const int CLARIUS_SHORT_DELAY_MS = 10;
  static const int CLARIUS_LONG_DELAY_MS  = 1000;

  static const std::string BATTERY_FIELD_TAG      = "ClariusBattery";
  static const std::string TEMP_FIELD_TAG         = "ClariusTemp";
  static const std::string FRAME_RATE_FIELD_TAG   = "ClariusFrameRate";
  static const std::string BUTTON_FIELD_TAG       = "ClariusButton";
  static const std::string UP_BUTTON_TAG          = "Up";
  static const std::string DOWN_BUTTON_TAG        = "Down";
  static const std::string NO_BUTTON_TAG          = "None";
  static const std::string NUM_CLICKS_FIELD_TAG   = "ClariusNumClicks";
  static const int SEND_BUTTON_STATE_FOR_N_FRAMES = 20;

  static const FrameSizeType DEFAULT_FRAME_SIZE = { 512, 512, 1 };

  static const bool DEFAULT_ENABLE_AUTO_GAIN = false;

  static const bool DEFAULT_ENABLE_5V_RAIL = false;

  static const bool DEFAULT_FREEZE_ON_POOR_WIFI_SIGNAL = true;

  static const int DEFAULT_CONTACT_DETECTION_TIMEOUT_SEC = 15;

  static const int DEFAULT_AUTO_FREEZE_TIMEOUT_SEC = 60;

  static const int DEFAULT_KEEP_AWAKE_TIMEOUT_SEC = 60;

  static const BUTTON_MODE DEFAULT_UP_BUTTON_MODE = BUTTON_MODE::DISABLED;

  static const BUTTON_MODE DEFAULT_DOWN_BUTTON_MODE = BUTTON_MODE::FREEZE;

  static const double DEFAULT_DEPTH_MM = 100.0;

  static const double DEFAULT_GAIN_PERCENT = 80.0;

  static const double DEFAULT_DYNRANGE_PERCENT = 80.0;

  static const std::vector<double> DEFAULT_TGC_DB = { 5, 5, 5 };

  static std::map<int, std::string> ConnectEnumToString {
    {CusConnection::ProbeConnected, "CONNECT_SUCCESS"},
    {CusConnection::ProbeDisconnected, "CONNECT_DISCONNECT"},
    {CusConnection::ConnectionFailed, "CONNECT_FAILED"},
    {CusConnection::SwUpdateRequired, "CONNECT_SWUPDATE"},
    {CusConnection::ConnectionError, "CONNECT_ERROR"},
  };

  static const std::string TRANSD_PORT_NAME = "Transd";
}

//-------------------------------------------------------------------------------------------------
// vtkInternal
//-------------------------------------------------------------------------------------------------

class vtkPlusClariusOEM::vtkInternal
{
public:

  vtkInternal(vtkPlusClariusOEM* ext);

  virtual ~vtkInternal()
  {
  }

protected:

  friend class vtkPlusClariusOEM;

  // Clarius callbacks
  static void ListFn(const char* list, int sz);

  static void ConnectFn(CusConnection ret, int port, const char* status);

  static void CertFn(int daysValid);

  static void PowerDownFn(CusPowerDown ret, int tm);

  static void SwUpdateFn(CusSwUpdate ret);

  static void RawImageFn(const void* newImage, const CusRawImageInfo* nfo, int npos, const CusPosInfo* pos);

  static void ProcessedImageFn(const void* newImage, const CusProcessedImageInfo* nfo, int npos, const CusPosInfo* pos);

  static void SpectralImageFn(const void* newImage, const CusSpectralImageInfo* nfo);

  static void ImagingFn(CusImagingState ready, int imaging);

  static void ButtonFn(CusButton btn, int clicks);

  static void ProgressFn(int progress);

  static void ErrorFn(const char* msg);

  // log user settings to console
  void LogUserSettings();

  //
  // members
  //

  // user configurable params
  std::string ProbeSerialNum;
  std::string PathToCert;
  FrameSizeType FrameSize;
  std::string ProbeType;
  std::string ImagingApplication;
  bool EnableAutoGain;
  bool Enable5v;
  bool FreezeOnPoorWifiSignal;
  int ContactDetectionTimeoutSec;
  int AutoFreezeTimeoutSec;
  int KeepAwakeTimeoutMin;
  BUTTON_MODE UpButtonMode;
  BUTTON_MODE DownButtonMode;

  // parameters retrieved from the probe over BLE
  std::string Ssid;
  std::string Password;
  std::string IpAddress;
  int TcpPort;

  // parameters retrieved from the probe using the OEM API
  std::promise<std::vector<std::string>> PromiseProbes;
  std::promise<std::vector<std::string>> PromiseApplications;
  std::promise<void> ConnectionBarrier;

  // pointers to DataSources
  vtkPlusDataSource* BModeSource;
  vtkPlusDataSource* TransdSource;

  // button press info
  std::string PressedButton;
  int ButtonNumClicks;
  int ButtonSentCount; // button state is sent for SEND_BUTTON_STATE_FOR_N_FRAMES
                       // frames after button is clicked to ensure user can process
                       // it in Slicer even if a frame is skipped

  enum class EXPECTED_LIST
  {
    PROBES,
    APPLICATIONS,
    UNKNOWN
  } ExpectedList;

private:
  vtkPlusClariusOEM* External;

  // BLE interface
  ClariusBLE BleHelper;

  // Wifi helper class
  ClariusWifi WifiHelper;
};

//-------------------------------------------------------------------------------------------------
vtkPlusClariusOEM::vtkInternal::vtkInternal(vtkPlusClariusOEM* ext)
: External(ext)
, ProbeSerialNum("")
, PathToCert("")
, FrameSize(DEFAULT_FRAME_SIZE)
, ProbeType("")
, ImagingApplication("")
, EnableAutoGain(DEFAULT_ENABLE_AUTO_GAIN)
, Enable5v(DEFAULT_ENABLE_5V_RAIL)
, FreezeOnPoorWifiSignal(DEFAULT_FREEZE_ON_POOR_WIFI_SIGNAL)
, ContactDetectionTimeoutSec(DEFAULT_CONTACT_DETECTION_TIMEOUT_SEC)
, AutoFreezeTimeoutSec(DEFAULT_AUTO_FREEZE_TIMEOUT_SEC)
, KeepAwakeTimeoutMin(DEFAULT_KEEP_AWAKE_TIMEOUT_SEC)
, UpButtonMode(DEFAULT_UP_BUTTON_MODE)
, DownButtonMode(DEFAULT_DOWN_BUTTON_MODE)
, IpAddress("")
, TcpPort(-1)
, BModeSource(nullptr)
, TransdSource(nullptr)
, PressedButton(NO_BUTTON_TAG)
, ButtonNumClicks(0)
, ButtonSentCount(0)
{
}

//-------------------------------------------------------------------------------------------------
void vtkPlusClariusOEM::vtkInternal::ListFn(const char* list, int sz)
{
  vtkPlusClariusOEM* device = vtkPlusClariusOEM::GetInstance();

  std::vector<std::string> vec;

  std::stringstream ss(list);
  while (ss.good())
  {
    std::string substr;
    getline(ss, substr, ',');
    vec.push_back(substr);
  }

  if (device->Internal->ExpectedList == EXPECTED_LIST::PROBES)
  {
    device->Internal->PromiseProbes.set_value(vec);
  }
  else if (device->Internal->ExpectedList == EXPECTED_LIST::APPLICATIONS)
  {
    device->Internal->PromiseApplications.set_value(vec);
  }
}

//-------------------------------------------------------------------------------------------------
void vtkPlusClariusOEM::vtkInternal::ConnectFn(CusConnection ret, int port, const char* status)
{
  if (ret == CusConnection::ProbeConnected)
  {
    // connection succeeded, set Internal->Connected variable to end busy wait in InternalConnect
    vtkPlusClariusOEM* device = vtkPlusClariusOEM::GetInstance();
    device->Internal->ConnectionBarrier.set_value();
  }
}

//-------------------------------------------------------------------------------------------------
void vtkPlusClariusOEM::vtkInternal::CertFn(int daysValid)
{
  if (daysValid <= 0)
  {
    LOG_ERROR("Invalid or expired certificate provided for Clarius OEM device");
  }
  else
  {
    LOG_INFO("Clarius certificate is valid for " << daysValid << " more days");
  }
}

//-------------------------------------------------------------------------------------------------
void vtkPlusClariusOEM::vtkInternal::PowerDownFn(CusPowerDown ret, int tm)
{
  std::stringstream ss;
  ss << "Clarius probe will power down ";

  if (tm == 0)
  {
    ss << "immediately as a result of ";
  }
  else
  {
    ss << "in " << tm << " seconds as a result of ";
  }

  if (ret == CusPowerDown::Idle)
  {
    ss << "being idle for an extended period of time. ";
  }
  else if (ret == CusPowerDown::TooHot)
  {
    ss << "overheating. ";
  }
  else if (ret == CusPowerDown::LowBattery)
  {
    ss << "running out of battery. ";
  }
  else if (ret == CusPowerDown::ButtonOff)
  {
    ss << "user holding the shutdown button. ";
  }
  else
  {
    ss << "an unspecified reason. ";
  }

  ss << "If Clarius probe has powered off please turn it back on and restart PLUS, if desired.";
  LOG_ERROR(ss.str());
}

//-------------------------------------------------------------------------------------------------
void vtkPlusClariusOEM::vtkInternal::SwUpdateFn(CusSwUpdate ret)
{
  LOG_ERROR("Clarius SwUpdateFn callback was called, but this feature is not supported by PLUS. Please update using the Clarius iOS/Android App");
}

//-------------------------------------------------------------------------------------------------
void vtkPlusClariusOEM::vtkInternal::RawImageFn(const void* newImage, const CusRawImageInfo* nfo, int npos, const CusPosInfo* pos)
{
  LOG_ERROR("Support for Clarius OEM raw images has not been implemented. If you desire this feature please submit an issue to request it on the PlusToolkit/PlusLib GitHub repository");
}

//-------------------------------------------------------------------------------------------------
void vtkPlusClariusOEM::vtkInternal::SpectralImageFn(const void* newImage, const CusSpectralImageInfo* nfo)
{
  LOG_ERROR("Support for Clarius OEM spectral images has not been implemented. If you desire this feature please submit an issue to request it on the PlusToolkit/PlusLib GitHub repository");
}

//-------------------------------------------------------------------------------------------------
void vtkPlusClariusOEM::vtkInternal::ProcessedImageFn(const void* oemImage, const CusProcessedImageInfo* nfo, int npos, const CusPosInfo* pos)
{
  vtkPlusClariusOEM* device = vtkPlusClariusOEM::GetInstance();
  if (device == NULL)
  {
    LOG_ERROR("vtkPlusClariusOEM::vtkInternal::ProcessedImageFn called when Clarius instance was NULL!");
    return;
  }

  // check if still connected
  if (!device->Connected)
  {
    LOG_ERROR("ClariusOEM device unexpectedly disconnected from Clarius Device. IpAddress = " << device->Internal->IpAddress
      << " Port = " << device->Internal->TcpPort);
    return;
  }

  if (oemImage == NULL)
  {
    LOG_ERROR("NULL frame received by the Clarius OEM device");
    return;
  }

  // check if source is valid, if not - nothing to do
  if (!device->Internal->BModeSource)
  {
    return;
  }

  // setup source
  const double unfilteredTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();
  device->Internal->BModeSource->SetInputFrameSize(nfo->width, nfo->height, 1);
  device->Internal->BModeSource->SetNumberOfScalarComponents(1);

  // convert Clarius RGBA to grayscale
  // format is:
  // R = ultrasound value
  // G = ultrasound value
  // B = ultrasound value
  // A = 255
  vtkSmartPointer<vtkImageData> vtkImage = vtkSmartPointer<vtkImageData>::New();
  vtkNew<vtkInformation> info;
  vtkImage->SetExtent(0, nfo->width-1, 0, nfo->height-1, 0, 1);
  vtkImage->SetOrigin(0.0, 0.0, 0.0);
  vtkImage->SetSpacing(1.0, 1.0, 1.0);
  vtkImage->AllocateScalars(VTK_UNSIGNED_CHAR, 1);

  std::vector<unsigned char> _gray_img;
  _gray_img.resize(nfo->imageSize / 4);
  PixelCodec::ConvertToGray(
    PixelCodec::PixelEncoding_RGBA32,
    nfo->width,
    nfo->height,
    (unsigned char*)oemImage,
    (unsigned char*)vtkImage->GetScalarPointer()
  );

  // custom fields (battery & button clicks)
  igsioFieldMapType customFields;
  CusStatusInfo stats;
  if (cusOemStatusInfo(&stats) != 0)
  {
    LOG_WARNING("Failed to retrieve cusOemStatusInfo");
  }
  customFields[BATTERY_FIELD_TAG].first = FRAMEFIELD_FORCE_SERVER_SEND;
  customFields[BATTERY_FIELD_TAG].second = std::to_string(stats.battery);
  customFields[TEMP_FIELD_TAG].first = FRAMEFIELD_FORCE_SERVER_SEND;
  customFields[TEMP_FIELD_TAG].second = std::to_string(stats.temperature);
  customFields[FRAME_RATE_FIELD_TAG].first = FRAMEFIELD_FORCE_SERVER_SEND;
  customFields[FRAME_RATE_FIELD_TAG].second = std::to_string(stats.frameRate);

  customFields[BUTTON_FIELD_TAG].first = FRAMEFIELD_FORCE_SERVER_SEND;
  customFields[BUTTON_FIELD_TAG].second = device->Internal->PressedButton;
  customFields[NUM_CLICKS_FIELD_TAG].first = FRAMEFIELD_FORCE_SERVER_SEND;
  customFields[NUM_CLICKS_FIELD_TAG].second = std::to_string(device->Internal->ButtonNumClicks);

  // reset button state if sent SEND_BUTTON_STATE_FOR_N_FRAMES times
  if (device->Internal->ButtonSentCount > 0)
  {
    --device->Internal->ButtonSentCount;
  }
  else
  {
    device->Internal->PressedButton = NO_BUTTON_TAG;
    device->Internal->ButtonNumClicks = 0;
  }

  // update the b-mode image
  device->Internal->BModeSource->AddItem(
    vtkImage->GetScalarPointer(),
    device->Internal->BModeSource->GetInputImageOrientation(),
    device->Internal->BModeSource->GetInputFrameSize(),
    VTK_UNSIGNED_CHAR,
    1,
    US_IMG_BRIGHTNESS,
    0,
    device->FrameNumber,
    unfilteredTimestamp,
    unfilteredTimestamp,
    &customFields
  );

  // if transd source is valid, update the ImageToTransd (or similarly named)
  // transform which localizes the image to the center of the transducer
  // and correctly sizes the image
  if (device->Internal->TransdSource)
  {
    int i = nfo->width / 2;
    double transd[2] = { static_cast<double>(i), 0.0 };
    for (int j = 0; j < nfo->height; j++)
    {
      unsigned char imval = vtkImage->GetScalarComponentAsDouble(i, j, 0, 0);
      if (imval != 0)
      {
        transd[0] = i;
        transd[1] = j;
        break;
      }
    }

    vtkSmartPointer<vtkMatrix4x4> tsfm = vtkSmartPointer<vtkMatrix4x4>::New();
    tsfm->Identity();
    double mmPerPixel = nfo->micronsPerPixel * UM_TO_MM;
    tsfm->SetElement(0, 0, mmPerPixel);
    tsfm->SetElement(1, 1, mmPerPixel);
    tsfm->SetElement(2, 2, mmPerPixel);
    tsfm->SetElement(0, 3, -transd[0] * mmPerPixel);
    tsfm->SetElement(1, 3, -transd[1] * mmPerPixel);
    device->Internal->TransdSource->AddTimeStampedItem(tsfm, TOOL_OK, device->FrameNumber, unfilteredTimestamp, unfilteredTimestamp, &customFields);
  }

  device->FrameNumber++;
}

//-------------------------------------------------------------------------------------------------
void vtkPlusClariusOEM::vtkInternal::ImagingFn(CusImagingState ready, int imaging)
{
  if (ready == CusImagingState::ImagingNotReady)
  {
    LOG_WARNING("Clarius imaging is not ready yet...");
  }
  else if (ready == CusImagingState::ImagingReady)
  {
    LOG_INFO("Clarius imaging is " << (imaging ? "running" : "stopped"));
  }
  else if (ready == CusImagingState::CertExpired)
  {
    LOG_ERROR("Clarius certificate needs to be updated prior to imaging");
  }
  else if (ready == CusImagingState::PoorWifi)
  {
    LOG_WARNING("Clarius stopped imaging as a result of a poor Wi-Fi connection");
  }
  else if (ready == CusImagingState::NoContact)
  {
    LOG_INFO("Clarius stopped imaging as a result of no patient contact for specified timeout duration");
  }
  else if (ready == CusImagingState::ChargingChanged)
  {
    LOG_WARNING("Clarius started / stopped imaging due to a change in charging status");
  }
  else
  {
    LOG_ERROR("Clarius ImagingFn called with unknown 'ready' state");
  }
}

//-------------------------------------------------------------------------------------------------
/*! callback for button clicks
 * @param[in] btn 0 = up, 1 = down
 * @param[in] clicks # of clicks performed*/
void vtkPlusClariusOEM::vtkInternal::ButtonFn(CusButton btn, int clicks)
{
  vtkPlusClariusOEM* device = vtkPlusClariusOEM::GetInstance();
  if (device == NULL)
  {
    LOG_ERROR("vtkPlusClariusOEM::vtkInternal::ProcessedImageFn called when Clarius instance was NULL!");
    return;
  }

  // check if still connected
  if (!device->Connected)
  {
    LOG_ERROR("ClariusOEM device unexpectedly disconnected from Clarius Device. IpAddress = " << device->Internal->IpAddress
      << " Port = " << device->Internal->TcpPort);
    return;
  }

  // set button state
  device->Internal->ButtonNumClicks = clicks;
  if (btn == CusButton::ButtonDown)
  {
    device->Internal->PressedButton = DOWN_BUTTON_TAG;
  }
  else if (btn == CusButton::ButtonUp)
  {
    device->Internal->PressedButton = UP_BUTTON_TAG;
  }
  else
  {
    LOG_WARNING("Unexpected Clarius button value of " << btn << " received in vtkPlusClariusOEM::vtkInternal::ButtonFn");
  }

  // reset number of frames to send new button state for
  device->Internal->ButtonSentCount = SEND_BUTTON_STATE_FOR_N_FRAMES;
}

//-------------------------------------------------------------------------------------------------
/*! callback for readback progress
 * @pram[in] progress the readback process*/
void vtkPlusClariusOEM::vtkInternal::ProgressFn(int progress)
{
  LOG_INFO("Downloading: " << progress << "%");
}

//-------------------------------------------------------------------------------------------------
/*! callback for error messages
 * @param[in] err the error message sent from the listener module
 * */
void vtkPlusClariusOEM::vtkInternal::ErrorFn(const char* err)
{
  LOG_ERROR("A Clarius OEM error occurred. Error text was: " << err);
}

//-------------------------------------------------------------------------------------------------
void vtkPlusClariusOEM::vtkInternal::LogUserSettings()
{
  std::stringstream ss;
  ss << "ProbeSerialNum: " << this->ProbeSerialNum << std::endl;
  ss << "PathToCert: " << this->PathToCert << std::endl;
  ss << "ProbeType: " << this->ProbeType << std::endl;
  ss << "ImagingApplication: " << this->ImagingApplication << std::endl;
  ss << "FrameSize: [" << this->FrameSize[0] << ", " << this->FrameSize[1] << ", " << this->FrameSize[2] << "]" << std::endl;
  ss << "EnableAutoGain: " << (this->EnableAutoGain ? "TRUE" : "FALSE") << std::endl;
  ss << "Enable5v: " << (this->Enable5v ? "TRUE" : "FALSE") << std::endl;
  ss << "FreezeOnPoorWifiSignal: " << (this->FreezeOnPoorWifiSignal ? "TRUE" : "FALSE") << std::endl;
  ss << "ContactDetectionTimeoutSec: " << this->ContactDetectionTimeoutSec << std::endl;
  ss << "AutoFreezeTimeoutSec: " << this->AutoFreezeTimeoutSec << std::endl;
  ss << "KeepAwakeTimeoutMin: " << this->KeepAwakeTimeoutMin << std::endl;
  ss << "UpButtonMode: " << ButtonModeEnumToString[this->UpButtonMode] << std::endl;
  ss << "DownButtonMode: " << ButtonModeEnumToString[this->DownButtonMode] << std::endl;

  LOG_INFO(std::endl << "User settings:" << std::endl << ss.str());
}

//-------------------------------------------------------------------------------------------------
// vtkPlusClariusOEM definitions
//-------------------------------------------------------------------------------------------------
vtkPlusClariusOEM* vtkPlusClariusOEM::New()
{
  if (instance == NULL)
  {
    instance = new vtkPlusClariusOEM();
  }
  return instance;
}

//-------------------------------------------------------------------------------------------------
vtkPlusClariusOEM::vtkPlusClariusOEM()
  : Internal(new vtkInternal(this))
{
  this->StartThreadForInternalUpdates = false;
  this->RequirePortNameInDeviceSetConfiguration = true;

  this->ImagingParameters->SetDepthMm(DEFAULT_DEPTH_MM);
  this->ImagingParameters->SetGainPercent(DEFAULT_GAIN_PERCENT);
  this->ImagingParameters->SetDynRangeDb(DEFAULT_DYNRANGE_PERCENT);
  this->ImagingParameters->SetTimeGainCompensation(DEFAULT_TGC_DB);

  // set dummy values to avoid endless error messages when using SlicerOpenIGTLink US Remote Control
  this->ImagingParameters->SetPowerDb(-1);
  this->ImagingParameters->SetFocusDepthPercent(-1);
  this->ImagingParameters->SetFrequencyMhz(-1);

  instance = this;
}

//-------------------------------------------------------------------------------------------------
vtkPlusClariusOEM::~vtkPlusClariusOEM()
{
  // ensure resources released
  this->InternalDisconnect();

  if (this->Internal)
  {
    delete this->Internal;
    this->Internal = nullptr;
  }

  this->instance = NULL;
}

//-------------------------------------------------------------------------------------------------
vtkPlusClariusOEM* vtkPlusClariusOEM::GetInstance()
{
  if (instance != NULL)
  {
    return instance;
  }
  else
  {
    // Instance is null, creating new instance
    instance = new vtkPlusClariusOEM();
    return instance;
  }
}

//-------------------------------------------------------------------------------------------------
void vtkPlusClariusOEM::PrintSelf(ostream& os, vtkIndent indent)
{
  LOG_TRACE("vtkPlusClariusOEM::PrintSelf");

  this->Superclass::PrintSelf(os, indent);
  os << indent << "ProbeSerialNum: " << this->Internal->ProbeSerialNum << std::endl;
  os << indent << "PathToCert: " << this->Internal->PathToCert << std::endl;
  os << indent << "ProbeType: " << this->Internal->ProbeType << std::endl;
  os << indent << "ImagingApplication: " << this->Internal->ImagingApplication << std::endl;
  os << indent << "FrameSize: [" << this->Internal->FrameSize[0] << ", " << this->Internal->FrameSize[1] << ", " << this->Internal->FrameSize[2] << "]" << std::endl;
  os << indent << "EnableAutoGain: " << (this->Internal->EnableAutoGain ? "TRUE" : "FALSE") << std::endl;
  os << indent << "Enable5v: " << (this->Internal->Enable5v ? "TRUE" : "FALSE") << std::endl;
  os << indent << "FreezeOnPoorWifiSignal: " << (this->Internal->FreezeOnPoorWifiSignal ? "TRUE" : "FALSE") << std::endl;
  os << indent << "ContactDetectionTimeoutSec: " << this->Internal->ContactDetectionTimeoutSec << std::endl;
  os << indent << "AutoFreezeTimeoutSec: " << this->Internal->AutoFreezeTimeoutSec << std::endl;
  os << indent << "KeepAwakeTimeoutMin: " << this->Internal->KeepAwakeTimeoutMin << std::endl;
  os << indent << "UpButtonMode: " << ButtonModeEnumToString[this->Internal->UpButtonMode] << std::endl;
  os << indent << "DownButtonMode: " << ButtonModeEnumToString[this->Internal->DownButtonMode] << std::endl;
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusClariusOEM::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusClariusOEM::ReadConfiguration");

  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  // probe serial number
  XML_READ_STRING_ATTRIBUTE_NONMEMBER_REQUIRED(
    ProbeSerialNum, this->Internal->ProbeSerialNum, deviceConfig);

  // path to Clarius certificate
  XML_READ_STRING_ATTRIBUTE_NONMEMBER_REQUIRED(
    PathToCert, this->Internal->PathToCert, deviceConfig);

  // probe type
  XML_READ_STRING_ATTRIBUTE_NONMEMBER_REQUIRED(
    ProbeType, this->Internal->ProbeType, deviceConfig);
  // force probe type string to be entirely uppercase
  std::transform(
    this->Internal->ProbeType.begin(),
    this->Internal->ProbeType.end(),
    this->Internal->ProbeType.begin(),
    [](unsigned char c) { return std::toupper(c); }
  );

  // imaging application (msk, abdomen, etc.)
  XML_READ_STRING_ATTRIBUTE_NONMEMBER_REQUIRED(
    ImagingApplication, this->Internal->ImagingApplication, deviceConfig);
  // force imaging application string to be entirely lowercase
  std::transform(
    this->Internal->ImagingApplication.begin(),
    this->Internal->ImagingApplication.end(),
    this->Internal->ImagingApplication.begin(),
    [](unsigned char c) { return std::tolower(c); }
  );

  // frame size
  int rfs[2] = { static_cast<int>(DEFAULT_FRAME_SIZE[0]), static_cast<int>(DEFAULT_FRAME_SIZE[1]) };
  if (deviceConfig->GetVectorAttribute("FrameSize", 2, rfs))
  {
    if (rfs[0] < 0 || rfs[1] < 0)
    {
      LOG_ERROR("Negative frame size defined in config file. Please define a positive frame size.");
      return PLUS_FAIL;
    }
    FrameSizeType fs = { static_cast<unsigned int>(rfs[0]), static_cast<unsigned int>(rfs[1]), 1 };
    this->Internal->FrameSize = fs;
  }

  // enable auto gain
  XML_READ_BOOL_ATTRIBUTE_NONMEMBER_OPTIONAL(EnableAutoGain,
    this->Internal->EnableAutoGain, deviceConfig);

  // enable 5v rail
  XML_READ_BOOL_ATTRIBUTE_NONMEMBER_OPTIONAL(Enable5v,
    this->Internal->Enable5v, deviceConfig);

  // freeze on poor wifi signal
  XML_READ_BOOL_ATTRIBUTE_NONMEMBER_OPTIONAL(FreezeOnPoorWifiSignal,
    this->Internal->FreezeOnPoorWifiSignal, deviceConfig);

  // contact detection timeout (seconds)
  XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, ContactDetectionTimeoutSec,
    this->Internal->ContactDetectionTimeoutSec, deviceConfig);

  // auto freeze timeout (seconds)
  XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, AutoFreezeTimeoutSec,
    this->Internal->AutoFreezeTimeoutSec, deviceConfig);

  // keep awake timeout (seconds)
  XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, KeepAwakeTimeoutMin,
    this->Internal->KeepAwakeTimeoutMin, deviceConfig);

  // up button mode
  XML_READ_ENUM3_ATTRIBUTE_NONMEMBER_OPTIONAL(UpButtonMode,
    this->Internal->UpButtonMode, deviceConfig,
    "FREEZE", BUTTON_MODE::FREEZE,
    "USER", BUTTON_MODE::USER,
    "DISABLED", BUTTON_MODE::DISABLED
  );

  // down button mode
  XML_READ_ENUM3_ATTRIBUTE_NONMEMBER_OPTIONAL(DownButtonMode,
    this->Internal->DownButtonMode, deviceConfig,
    "FREEZE", BUTTON_MODE::FREEZE,
    "USER", BUTTON_MODE::USER,
    "DISABLED", BUTTON_MODE::DISABLED
  );

  // read imaging parameters
  this->ImagingParameters->ReadConfiguration(deviceConfig);

  // validate data sources
  XML_FIND_NESTED_ELEMENT_REQUIRED(dataSourcesElement, deviceConfig, "DataSources");
  int numSources = dataSourcesElement->GetNumberOfNestedElements();
  for (int i = 0; i < numSources; ++i)
  {
    vtkXMLDataElement* element = dataSourcesElement->GetNestedElement(i);
    if (!igsioCommon::IsEqualInsensitive(element->GetName(), "DataSource"))
    {
      // if this is not a data source element, skip it
      continue;
    }

    // if video source, verify correct orientation (MF) to place the (0, 0) voxel
    // of the image in the marked corner adjacent to the probe
    const char* type = element->GetAttribute("Type");
    if (type && igsioCommon::IsEqualInsensitive(type, "Video"))
    {
      const char* orientation = element->GetAttribute("PortUsImageOrientation");
      if (orientation && !igsioCommon::IsEqualInsensitive(orientation, "UF"))
      {
        LOG_WARNING("Clarius image source has unexpected value '" << orientation << "' for PortUsImageOrientation. Please confirm your image orientation, expected value for most Clarius probes is 'UF'.");
      }
    }

    // if tool, verify port name is a valid option for Clarius OEM
    if (type && igsioCommon::IsEqualInsensitive(type, "Tool"))
    {
      const char* toolId = element->GetAttribute("Id");
      if (toolId == NULL)
      {
        LOG_ERROR("Failed to initialize Clarius OEM transform: DataSource Id is missing");
        return PLUS_FAIL;
      }

      const char* portName = element->GetAttribute("PortName");
      if (portName && !igsioCommon::IsEqualInsensitive(portName, TRANSD_PORT_NAME))
      {
        LOG_ERROR("Clarius OEM device only supports Tool items with PortName=\""
          << TRANSD_PORT_NAME << "\"");
        return PLUS_FAIL;
      }

      // set vtkInternal pointer to transd transform source
      igsioTransformName transdTransformName(toolId, this->GetToolReferenceFrameName());
      std::string transdSourceId = transdTransformName.GetTransformName();
      if (this->GetTool(transdSourceId, this->Internal->TransdSource) != PLUS_SUCCESS || this->Internal->TransdSource == NULL)
      {
        LOG_ERROR("Failed to get ClariusOEM tool: " << transdSourceId);
        return PLUS_FAIL;
      }
    }
  }

  // set vtkInternal pointer to b-mode data source
  std::vector<vtkPlusDataSource*> bModeSources;
  this->GetVideoSourcesByPortName(vtkPlusDevice::BMODE_PORT_NAME, bModeSources);
  if (!bModeSources.empty())
  {
    this->Internal->BModeSource = bModeSources[0];
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusClariusOEM::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusClariusOEM::WriteConfiguration");

  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  this->ImagingParameters->WriteConfiguration(deviceConfig);

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusClariusOEM::NotifyConfigured()
{
  LOG_TRACE("vtkPlusClariusOEM::NotifyConfigured");

  vtkPlusClariusOEM* device = vtkPlusClariusOEM::GetInstance();
  if (device->OutputChannels.size() > 1)
  {
    LOG_WARNING("vtkPlusClariusOEM is expecting one output channel and there are " <<
      this->OutputChannels.size() << " channels. First output channel will be used.");
  }

  if (device->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined for vtkPlusClariusOEM. Cannot proceed.");
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }

  std::vector<vtkPlusDataSource*> sources;
  sources = device->GetVideoSources();
  if (sources.size() > 1)
  {
    LOG_WARNING("More than one output video source found. First will be used");
  }
  if (sources.size() == 0)
  {
    LOG_ERROR("Video source required in configuration. Cannot proceed.");
    return PLUS_FAIL;
  }

  // Check if output channel has data source
  vtkPlusDataSource* aSource(NULL);
  if (device->OutputChannels[0]->GetVideoSource(aSource) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to retrieve the video source in the vtkPlusClariusOEM device.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusClariusOEM::Probe()
{
  LOG_TRACE("vtkPlusClariusOEM: Probe");
  LOG_ERROR("vtkPlusClariusOEM::Probe() is not implemented");

  return PLUS_SUCCESS;
};

//-------------------------------------------------------------------------------------------------
std::string vtkPlusClariusOEM::GetSdkVersion()
{
  LOG_TRACE("vtkPlusClariusOEM::GetSdkVersion");
  return "SDK version not available";
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusClariusOEM::InitializeBLE()
{
  // find probe by serial number
  if (!this->Internal->BleHelper.FindBySerial(this->Internal->ProbeSerialNum))
  {
    LOG_ERROR("Failed to find BLE connection for Clarius probe with serial number "
      << this->Internal->ProbeSerialNum << ". Please make sure the probe is charged"
      ", paired, and in range for BLE.");

    std::vector<std::string> probes = this->Internal->BleHelper.RetrieveFoundProbeIds();

    if (!probes.size())
    {
      // failed to connect to probe by serial number, no active probes found in BLE range
      LOG_ERROR("No active Clarius probes found within BLE range");
      return PLUS_FAIL;
    }

    // print list of nearby active probes
    std::stringstream ss;
    ss << "Clarius probes configured with Windows Bluetooth are:\n";
    for (const std::string& probe : probes)
    {
      ss << "\t" << probe << "\n";
    }
    LOG_ERROR(ss.str());
    return PLUS_FAIL;
  }

  // attempt to connect to probe ble network
  LOG_INFO("Connecting to BLE network of Clarius probe with SN: " << this->Internal->ProbeSerialNum);
  if (this->Internal->BleHelper.Connect() != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to connect to Clarius probe. Last error was: "
      << this->Internal->BleHelper.GetLastError());
    return PLUS_FAIL;
  }

  // double check probe is connected
  if (!this->Internal->BleHelper.IsProbeConnected())
  {
    LOG_ERROR("Failed to connect to Clarius probe. Last error was: "
      << this->Internal->BleHelper.GetLastError());
    return PLUS_FAIL;
  }

  // BLE connection succeeded
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusClariusOEM::InitializeProbe()
{
  // power on the probe
  if (this->Internal->BleHelper.RequestProbeOn() != PLUS_SUCCESS)
  {
    LOG_ERROR("An error occurred during RequestProbeOn. Last error was: "
      << this->Internal->BleHelper.GetLastError());
    return PLUS_FAIL;
  }

  // wait for probe to fully power on
  if (!this->Internal->BleHelper.AwaitWifiInfoReady())
  {
    LOG_ERROR("Timeout occurred while waiting for Clarius probe to power on."
      << " Last error was: " << this->Internal->BleHelper.GetLastError());
    return PLUS_FAIL;
  }

  // force wifi into AP mode
  if (this->Internal->BleHelper.ConfigureWifiAP() != PLUS_SUCCESS)
  {
    LOG_ERROR("An error occurred during ConfigureWifiAP. Last error was: "
      << this->Internal->BleHelper.GetLastError());
    return PLUS_FAIL;
  }

  // get & print wifi info
  std::pair<bool, ClariusWifiInfo> infoPair = this->Internal->BleHelper.GetWifiInfo();
  if (!infoPair.first)
  {
    LOG_ERROR("An error occurred during GetWifiInfo. Last error was: "
      << this->Internal->BleHelper.GetLastError());
    return PLUS_FAIL;
  }

  ClariusWifiInfo info = infoPair.second;
  if (!info.Ready)
  {
    LOG_ERROR("Wifi was not ready when connection info was requested");
    return PLUS_FAIL;
  }
  else
  {
    std::stringstream infoStream;
    infoStream << "\tAvailable?: " << to_string(info.Available) << "\n";
    infoStream << "\tWifi Mode : " << to_string(info.WifiMode) << "\n";
    infoStream << "\tSSID: " << info.SSID << "\n";
    infoStream << "\tPassword: " << info.Password << "\n";
    infoStream << "\tIPv4: " << info.IPv4 << "\n";
    infoStream << "\tMac Address: " << info.MacAddress << "\n";
    infoStream << "\tControl Port: " << info.ControlPort << "\n";
    infoStream << "\tCast Port: " << info.CastPort << "\n";
    infoStream << "\tChannel: " << info.Channel << "\n";

    LOG_INFO("Clarius Wifi Info: " << std::endl << infoStream.str());

    this->Internal->Ssid = info.SSID;
    this->Internal->Password = info.Password;
    this->Internal->IpAddress = info.IPv4;
    this->Internal->TcpPort = info.ControlPort;
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusClariusOEM::InitializeWifi()
{
  if (this->Internal->WifiHelper.Initialize() != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to initialize Clarius wifi helper");
    return PLUS_FAIL;
  }

  PlusStatus res = this->Internal->WifiHelper.ConnectToClariusWifi(
    this->Internal->Ssid,
    this->Internal->Password
  );
  if (res != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to connect to Clarius probe wifi");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusClariusOEM::InitializeOEM()
{
  // placeholder argc / argv arguments
  int argc = 1;
  char** argv = new char* [1];
  argv[0] = new char[4];
  strcpy(argv[0], "abc");
  const char* certPath = "/Clarius";

  // api callback functions
  CusListFn listFnPtr = static_cast<CusListFn>(&vtkPlusClariusOEM::vtkInternal::ListFn);
  CusConnectFn connectFnPtr = static_cast<CusConnectFn>(&vtkPlusClariusOEM::vtkInternal::ConnectFn);
  CusCertFn certFnPtr = static_cast<CusCertFn>(&vtkPlusClariusOEM::vtkInternal::CertFn);
  CusPowerDownFn powerDownFnPtr = static_cast<CusPowerDownFn>(&vtkPlusClariusOEM::vtkInternal::PowerDownFn);
  CusSwUpdateFn swUpdateFnPtr = static_cast<CusSwUpdateFn>(&vtkPlusClariusOEM::vtkInternal::SwUpdateFn);
  CusNewRawImageFn newRawImageFnPtr = static_cast<CusNewRawImageFn>(&vtkPlusClariusOEM::vtkInternal::RawImageFn);
  CusNewProcessedImageFn newProcessedImageFnPtr = static_cast<CusNewProcessedImageFn>(&vtkPlusClariusOEM::vtkInternal::ProcessedImageFn);
  CusNewSpectralImageFn newSpectralImageFnPtr = static_cast<CusNewSpectralImageFn>(&vtkPlusClariusOEM::vtkInternal::SpectralImageFn);
  CusImagingFn imagingFnPtr = static_cast<CusImagingFn>(&vtkPlusClariusOEM::vtkInternal::ImagingFn);
  CusButtonFn buttonFnPtr = static_cast<CusButtonFn>(&vtkPlusClariusOEM::vtkInternal::ButtonFn);
  CusProgressFn progressFnPtr = static_cast<CusProgressFn>(&vtkPlusClariusOEM::vtkInternal::ProgressFn);
  CusErrorFn errorFnPtr = static_cast<CusErrorFn>(&vtkPlusClariusOEM::vtkInternal::ErrorFn);

  // no b-mode data sources, disable b mode callback
  std::vector<vtkPlusDataSource*> bModeSources;
  this->GetVideoSourcesByPortName(vtkPlusDevice::BMODE_PORT_NAME, bModeSources);
  if (bModeSources.empty())
  {
    newProcessedImageFnPtr = nullptr;
  }

  // no RF-mode data sources, disable RF-mode callback
  std::vector<vtkPlusDataSource*> rfModeSources;
  this->GetVideoSourcesByPortName(vtkPlusDevice::RFMODE_PORT_NAME, rfModeSources);
  if (rfModeSources.empty())
  {
    newRawImageFnPtr = nullptr;
  }

  try
  {
    FrameSizeType fs = this->Internal->FrameSize;
    int result = cusOemInit(
      argc,
      argv,
      certPath,
      connectFnPtr,
      certFnPtr,
      powerDownFnPtr,
      newProcessedImageFnPtr,
      newRawImageFnPtr,
      newSpectralImageFnPtr,
      imagingFnPtr,
      buttonFnPtr,
      errorFnPtr,
      fs[0],
      fs[1]
    );
    std::this_thread::sleep_for(std::chrono::milliseconds(CLARIUS_LONG_DELAY_MS));

    if (result < 0)
    {
      LOG_ERROR("Failed to initialize Clarius OEM library");
      return PLUS_FAIL;
    }
  }
  catch (const std::runtime_error& re)
  {
    LOG_ERROR("Runtime error on cusOemInit. Error text: " << re.what());
    return PLUS_FAIL;
  }
  catch (const std::exception& ex)
  {
    LOG_ERROR("Exception on cusOemInit. Error text: " << ex.what());
    return PLUS_FAIL;
  }
  catch (...)
  {
    LOG_ERROR("Unknown failure occurred on cusOemInit");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;

}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusClariusOEM::SetClariusCert()
{
  // load the cert file
  std::string fullCertPath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(this->Internal->PathToCert);
  std::ifstream certFile(fullCertPath);
  if (!certFile.is_open())
  {
    LOG_ERROR("Failed to open Clarius cert file from " << fullCertPath << ". Please check the PathToCert path in your config.");
    return PLUS_FAIL;
  }

  std::ostringstream sstr;
  sstr << certFile.rdbuf();
  std::string certStr = sstr.str();

  // set cert in OEM API
  if (cusOemSetCert(certStr.c_str()) != 0)
  {
    LOG_ERROR("Failed to set Clarius OEM connection certificate");
    return PLUS_FAIL;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(CLARIUS_LONG_DELAY_MS));

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusClariusOEM::ConfigureProbeApplication()
{
  const char* ip = this->Internal->IpAddress.c_str();
  unsigned int port = this->Internal->TcpPort;
  LOG_INFO("Attempting connection to Clarius ultrasound on " << ip << ":" << port << " for 25 seconds:");

  std::future<void> connectionBarrierFuture = this->Internal->ConnectionBarrier.get_future();
  try
  {
    int result = cusOemConnect(ip, port);
    if (result != CusConnection::ProbeConnected)
    {
      LOG_ERROR("Failed to initiate connection to Clarius probe on " << ip << ":" << port <<
        ". Return code: " << ConnectEnumToString[result]);
      return PLUS_FAIL;
    }
  }
  catch (const std::runtime_error& re)
  {
    LOG_ERROR("Runtime error on cusOemConnect. Error text: " << re.what());
    return PLUS_FAIL;
  }
  catch (const std::exception& ex)
  {
    LOG_ERROR("Exception on cusOemConnect. Error text: " << ex.what());
    return PLUS_FAIL;
  }
  catch (...)
  {
    LOG_ERROR("Unknown failure occurred on cusOemConnect");
    return PLUS_FAIL;
  }

  // wait for cusOemConnected call to complete
  if (connectionBarrierFuture.wait_for(std::chrono::seconds(25)) != std::future_status::ready)
  {
    LOG_ERROR("Connection to Clarius device timed out");
    return PLUS_FAIL;
  }
  LOG_INFO("Connected to Clarius probe on " << ip << ":" << port);

  // get list of available probes
  CusListFn listFnPtr = static_cast<CusListFn>(&vtkPlusClariusOEM::vtkInternal::ListFn);
  this->Internal->ExpectedList = vtkPlusClariusOEM::vtkInternal::EXPECTED_LIST::PROBES;
  std::future<std::vector<std::string>> futureProbes = this->Internal->PromiseProbes.get_future();
  if (cusOemProbes(listFnPtr) != 0)
  {
    LOG_INFO("Failed to retrieve list of valid probe types");
    return PLUS_FAIL;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(CLARIUS_LONG_DELAY_MS));

  // wait for probes list to be populated
  if (futureProbes.wait_for(std::chrono::seconds(5)) != std::future_status::ready)
  {
    LOG_ERROR("Failed to retrieve list of valid Clarius probe names");
    return PLUS_FAIL;
  }
  std::vector<std::string> vProbes = futureProbes.get();

  // validate provided probe type
  std::string probeType = this->Internal->ProbeType;
  if (std::find(vProbes.begin(), vProbes.end(), probeType) == vProbes.end())
  {
    std::string vProbesStr;
    for (const auto& probe : vProbes)
    {
      vProbesStr += probe + ", ";
    }
    vProbesStr.pop_back(); vProbesStr.pop_back(); // remove trailing comma and space
    LOG_ERROR("Invalid probe type (" << probeType << ") provided, valid probe types are: " << vProbesStr);
    return PLUS_FAIL;
  }

  // list available imaging applications
  this->Internal->ExpectedList = vtkPlusClariusOEM::vtkInternal::EXPECTED_LIST::APPLICATIONS;
  std::future<std::vector<std::string>> futureApplications = this->Internal->PromiseApplications.get_future();
  if (cusOemApplications(probeType.c_str(), listFnPtr) != 0)
  {
    LOG_ERROR("Failed to retrieve list of valid imaging applications");
    return PLUS_FAIL;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(CLARIUS_LONG_DELAY_MS));

  // wait for applications list to be populated
  if (futureApplications.wait_for(std::chrono::seconds(5)) != std::future_status::ready)
  {
    LOG_ERROR("Failed to retrieve list of valid Clarius application names");
    return PLUS_FAIL;
  }
  std::vector<std::string> vApps = futureApplications.get();

  // validate provided imaging application
  std::string imagingApplication = this->Internal->ImagingApplication;
  if (std::find(vApps.begin(), vApps.end(), imagingApplication) == vApps.end())
  {
    std::string vAppsStr;
    for (const auto& app : vApps)
    {
      vAppsStr += app + ", ";
    }
    vAppsStr.pop_back(); vAppsStr.pop_back(); // remove trailing comma and space
    LOG_ERROR("Invalid imaging application (" << imagingApplication << ") provided, valid imaging applications are: " << vAppsStr);
    return PLUS_FAIL;
  }

  // configure probe mode
  if (cusOemLoadApplication(probeType.c_str(), imagingApplication.c_str()) == 0)
  {
    LOG_INFO("Attempting to load " << imagingApplication << " application on a " << probeType << " probe");
  }
  else
  {
    LOG_ERROR("An error occured on call to cusOemLoadApplication");
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(CLARIUS_LONG_DELAY_MS));

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusClariusOEM::SetInitialUsParams()
{
  // set imaging depth (mm)
  double depthMm = this->ImagingParameters->GetDepthMm();
  if (this->SetDepthMm(depthMm) != PLUS_SUCCESS)
  {
    LOG_WARNING("Failed to set requested imaging depth (mm) in Clarius OEM device, unknown depth will be used");
  }

  // set gain (%)
  double gainPercent = this->ImagingParameters->GetGainPercent();
  if (this->SetGainPercent(gainPercent) != PLUS_SUCCESS)
  {
    LOG_WARNING("Failed to set requested imaging gain (%) in Clarius OEM device, unknown gain will be used");
  }

  // set dynamic range (%)
  double dynRangePercent = this->ImagingParameters->GetDynRangeDb();
  if (this->SetDynRangePercent(dynRangePercent) != PLUS_SUCCESS)
  {
    LOG_WARNING("Failed to set requested imaging dynamic range in Clarius OEM device, unknown dynamic range will be used");
  }

  // set time gain compensation
  std::vector<double> tgcDb = this->ImagingParameters->GetTimeGainCompensation();
  if (this->SetTimeGainCompensationDb(tgcDb) != PLUS_SUCCESS)
  {
    LOG_WARNING("Failed to set requested imaging time gain compensation in Clarius OEM device, unknown time gain compensation will be used");
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusClariusOEM::InternalConnect()
{
  LOG_TRACE("vtkPlusClariusOEM::InternalConnect");

  if (this->Connected)
  {
    // Internal connect already called and completed successfully
    return PLUS_SUCCESS;
  }

  // log user settings for debugging
  this->Internal->LogUserSettings();

  // BLE
  if (this->InitializeBLE() != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to initialize BLE in Clarius OEM device");
    this->InternalDisconnect();
    return PLUS_FAIL;
  }

  // PROBE (power, etc.)
  if (this->InitializeProbe() != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to initialize probe (power, wifi settings) in Clarius OEM device");
    this->InternalDisconnect();
    return PLUS_FAIL;
  }

  // WIFI
  if (this->InitializeWifi() != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to initialize wifi in Clarius OEM device");
    this->InternalDisconnect();
    return PLUS_FAIL;
  }

  // OEM library
  if (this->InitializeOEM() != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to initialize OEM library in Clarius OEM device");
    this->InternalDisconnect();
    return PLUS_FAIL;
  }

  // SET CERTIFICATE
  if (this->SetClariusCert() != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to set Clarius certificate. Please check your PathToCert is valid, and contains the correct cert for the probe you're connecting to");
    this->InternalDisconnect();
    return PLUS_FAIL;
  }

  // CONFIGURE PROBE SETTINGS
  CusProbeSettings settings;
  settings.contactDetection = this->Internal->ContactDetectionTimeoutSec;
  settings.autoFreeze = (this->Internal->AutoFreezeTimeoutSec ? 1 : 0);
  settings.keepAwake = this->Internal->KeepAwakeTimeoutMin;
  settings.wifiOptimization = this->Internal->FreezeOnPoorWifiSignal;
  settings.up = static_cast<CusButtonSetting>(this->Internal->UpButtonMode);
  settings.down = static_cast<CusButtonSetting>(this->Internal->DownButtonMode);
  if (cusOemSetProbeSettings(&settings) != 0)
  {
    LOG_ERROR("Failed to set Clarius OEM probe settings");
    return PLUS_FAIL;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(CLARIUS_LONG_DELAY_MS));

  // CONFIGURE PROBE MODE
  if (this->ConfigureProbeApplication() != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to configure Clarius probe application");
    this->InternalDisconnect();
    return PLUS_FAIL;
  }

  // PRINT DEVICE STATS AND PROBE INFO
  CusStatusInfo stats;
  if (cusOemStatusInfo(&stats) != 0)
  {
    LOG_WARNING("Failed to retrieve cusOemStatusInfo");
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(CLARIUS_LONG_DELAY_MS));

  CusProbeInfo probeInfo;
  if (cusOemProbeInfo(&probeInfo) != 0)
  {
    LOG_WARNING("Failed to retrieve cusOemProbeInfo");
  }
  std::stringstream ss;
  ss << "Version: " << probeInfo.version << std::endl;
  ss << "Battery: " << stats.battery << "%" << std::endl;
  ss << "Temperature: " << stats.temperature << "%" << std::endl;
  ss << "Elements: " << probeInfo.elements << std::endl;
  ss << "Pitch: " << probeInfo.pitch << std::endl;
  ss << "Radius: " << probeInfo.radius << "mm" << std::endl;
  LOG_INFO(std::endl << "Probe info: " << std::endl << ss.str());

  // enable the 5v rail on the top of the Clarius probe
  int enable5v = this->Internal->Enable5v ? 1 : 0;
  if (cusOemEnable5v(enable5v) < 0)
  {
    std::string enstr = (enable5v ? "TRUE" : "FALSE");
    LOG_WARNING("Failed to set the state of the Clarius probe 5v rail, provided enable value was: " << enstr);
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(CLARIUS_LONG_DELAY_MS));

  // set imaging parameters
  this->SetInitialUsParams();

  return PLUS_SUCCESS;
};

//-------------------------------------------------------------------------------------------------
void vtkPlusClariusOEM::DeInitializeOEM()
{
  int oemState = cusOemIsConnected();

  if (oemState == CLARIUS_STATE_CONNECTED)
  {
    if (cusOemEnable5v(CLARIUS_FALSE) < 0)
    {
      LOG_WARNING("Failed to disable Clarius 5v");
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(CLARIUS_LONG_DELAY_MS));
    if (cusOemRun(CLARIUS_STOP) < 0)
    {
      LOG_WARNING("Failed to stop Clarius imaging");
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(CLARIUS_LONG_DELAY_MS));
    if (cusOemDisconnect() < 0)
    {
      LOG_WARNING("Failed to disconnect from Clarius OEM library");
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(CLARIUS_LONG_DELAY_MS));
    if (cusOemDestroy() < 0)
    {
      LOG_WARNING("Failed to destroy Clarius OEM library");
    }
  }
  else if (oemState == CLARIUS_STATE_NOT_CONNECTED)
  {
    if (cusOemDestroy() < 0)
    {
      LOG_WARNING("Failed to destroy Clarius OEM library");
    }
  }
  else if (oemState == CLARIUS_STATE_NOT_INITIALIZED)
  {
    // nothing to do here
  }
}

//-------------------------------------------------------------------------------------------------
void vtkPlusClariusOEM::DeInitializeWifi()
{
  if (!this->Internal->WifiHelper.DisconnectFromClariusWifi())
  {
    LOG_WARNING("Failed to disconnect from Clarius wifi");
  }
  if (!this->Internal->WifiHelper.DeInitialize())
  {
    LOG_WARNING("Failed to de-initialize ClariusWifi");
  }
}

//-------------------------------------------------------------------------------------------------
void vtkPlusClariusOEM::DeInitializeProbe()
{
  // power off the probe if powered / connected over BLE
  if (!this->Internal->BleHelper.IsProbeConnected())
  {
    return;
  }

  if (this->Internal->BleHelper.RequestProbeOff() != PLUS_SUCCESS)
  {
    LOG_ERROR("An error occurred during RequestProbeOff. Last error was: "
      << this->Internal->BleHelper.GetLastError())
  }
}

//-------------------------------------------------------------------------------------------------
void vtkPlusClariusOEM::DeInitializeBLE()
{
  // disconnect from probe BLE
  if (this->Internal->BleHelper.Disconnect() != PLUS_SUCCESS)
  {
    LOG_ERROR("An error occurred during Clarius BLE Disconnect. Last error was: "
      << this->Internal->BleHelper.GetLastError());
  }
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusClariusOEM::InternalDisconnect()
{
  LOG_TRACE("vtkPlusClariusOEM::InternalDisconnect");

  // inverse order to initialization
  this->DeInitializeOEM();
  this->DeInitializeWifi();
  this->DeInitializeProbe();
  this->DeInitializeBLE();

  return PLUS_SUCCESS;
};

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusClariusOEM::InternalStartRecording()
{
  LOG_TRACE("vtkPlusClariusOEM::InternalStartRecording");

  if (cusOemRun(CLARIUS_RUN) < 0)
  {
    LOG_ERROR("Failed to start Clarius imaging");
    return PLUS_FAIL;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(CLARIUS_LONG_DELAY_MS));

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusClariusOEM::InternalStopRecording()
{
  LOG_TRACE("vtkPlusClariusOEM::InternalStopRecording");

  if (cusOemRun(CLARIUS_STOP) < 0)
  {
    LOG_ERROR("Failed to stop Clarius imaging");
    return PLUS_FAIL;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(CLARIUS_LONG_DELAY_MS));

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusClariusOEM::InternalApplyImagingParameterChange()
{
  PlusStatus status = PLUS_SUCCESS;

  // depth (mm), note: Clarius uses cm
  if (this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_DEPTH)
    && this->ImagingParameters->IsPending(vtkPlusUsImagingParameters::KEY_DEPTH))
  {
    if (this->SetDepthMm(this->ImagingParameters->GetDepthMm()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set depth imaging parameter");
      status = PLUS_FAIL;
    }
    this->ImagingParameters->SetPending(vtkPlusUsImagingParameters::KEY_DEPTH, false);
  }

  // gain (percent)
  if (this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_GAIN)
    && this->ImagingParameters->IsPending(vtkPlusUsImagingParameters::KEY_GAIN))
  {
    if (this->SetGainPercent(this->ImagingParameters->GetGainPercent()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set gain imaging parameter");
      status = PLUS_FAIL;
    }
    this->ImagingParameters->SetPending(vtkPlusUsImagingParameters::KEY_GAIN, false);
  }

  // dynamic range (percent)
  if (this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_DYNRANGE)
    && this->ImagingParameters->IsPending(vtkPlusUsImagingParameters::KEY_DYNRANGE))
  {
    if (this->SetDynRangePercent(this->ImagingParameters->GetDynRangeDb()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set dynamic range imaging parameter");
      status = PLUS_FAIL;
    }
    this->ImagingParameters->SetPending(vtkPlusUsImagingParameters::KEY_DYNRANGE, false);
  }

  // TGC (time gain compensation)
  if (this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_TGC)
    && this->ImagingParameters->IsPending(vtkPlusUsImagingParameters::KEY_TGC))
  {
    std::vector<double> tgcVec;
    this->ImagingParameters->GetTimeGainCompensation(tgcVec);
    if (this->SetTimeGainCompensationDb(tgcVec) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set time gain compensation imaging parameter");
      status = PLUS_FAIL;
    }
    this->ImagingParameters->SetPending(vtkPlusUsImagingParameters::KEY_TGC, false);
  }

  return status;
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusClariusOEM::GetDepthMm(double& aDepthMm)
{
  int oemState = cusOemIsConnected();
  if (oemState != CLARIUS_STATE_CONNECTED)
  {
    // Connection has not been established yet, return cached parameter value
    return this->ImagingParameters->GetDepthMm(aDepthMm);
  }

  double oemVal = cusOemGetParam(CusParam::ImageDepth);
  if (oemVal < 0)
  {
    aDepthMm = -1;
    LOG_ERROR("Failed to get DepthMm parameter");
    return PLUS_FAIL;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(CLARIUS_SHORT_DELAY_MS));

  aDepthMm = oemVal * CM_TO_MM;

  // ensure ImagingParameters is up to date
  this->ImagingParameters->SetDepthMm(aDepthMm);

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusClariusOEM::SetDepthMm(double aDepthMm)
{
  int oemState = cusOemIsConnected();
  if (oemState != CLARIUS_STATE_CONNECTED)
  {
    // Connection has not been established yet, parameter value will be set upon connection
    this->ImagingParameters->SetDepthMm(aDepthMm);
    LOG_INFO("Cached US parameter DepthMm = " << aDepthMm);
    return PLUS_SUCCESS;
  }

  // attempt to set parameter value
  double depthCm = aDepthMm * MM_TO_CM;
  if (cusOemSetParam(CusParam::ImageDepth, depthCm) < 0)
  {
    LOG_ERROR("Failed to set DepthMm parameter");
    return PLUS_FAIL;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(CLARIUS_SHORT_DELAY_MS));

  // update imaging parameters & return successfully
  this->ImagingParameters->SetDepthMm(aDepthMm);
  LOG_INFO("Set US parameter DepthMm to " << aDepthMm);
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusClariusOEM::GetGainPercent(double& aGainPercent)
{
  int oemState = cusOemIsConnected();
  if (oemState != CLARIUS_STATE_CONNECTED)
  {
    // Connection has not been established yet, return cached parameter value
    return this->ImagingParameters->GetGainPercent(aGainPercent);
  }

  double oemVal = cusOemGetParam(CusParam::Gain);
  if (oemVal < 0)
  {
    aGainPercent = -1;
    LOG_ERROR("Failed to get GainPercent parameter");
    return PLUS_FAIL;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(CLARIUS_SHORT_DELAY_MS));

  aGainPercent = oemVal;

  // ensure ImagingParameters is up to date
  this->ImagingParameters->SetGainPercent(aGainPercent);

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusClariusOEM::SetGainPercent(double aGainPercent)
{
  int oemState = cusOemIsConnected();
  if (oemState != CLARIUS_STATE_CONNECTED)
  {
    // Connection has not been established yet, parameter value will be set upon connection
    this->ImagingParameters->SetGainPercent(aGainPercent);
    LOG_INFO("Cached US parameter GainPercent = " << aGainPercent);
    return PLUS_SUCCESS;
  }

  // attempt to set parameter value
  if (cusOemSetParam(CusParam::Gain, aGainPercent) < 0)
  {
    LOG_ERROR("Failed to set GainPercent parameter");
    return PLUS_FAIL;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(CLARIUS_SHORT_DELAY_MS));

  // update imaging parameters & return successfully
  this->ImagingParameters->SetGainPercent(aGainPercent);
  LOG_INFO("Set US parameter GainPercent to " << aGainPercent);
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusClariusOEM::GetDynRangePercent(double& aDynRangePercent)
{
  int oemState = cusOemIsConnected();
  if (oemState != CLARIUS_STATE_CONNECTED)
  {
    // Connection has not been established yet, return cached parameter value
    return this->ImagingParameters->GetDynRangeDb(aDynRangePercent);
  }

  double oemVal = cusOemGetParam(CusParam::DynamicRange);
  if (oemVal < 0)
  {
    aDynRangePercent = -1;
    LOG_ERROR("Failed to get DynRange parameter");
    return PLUS_FAIL;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(CLARIUS_SHORT_DELAY_MS));

  aDynRangePercent = oemVal;

  // ensure ImagingParameters is up to date
  this->ImagingParameters->SetDynRangeDb(aDynRangePercent);

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusClariusOEM::SetDynRangePercent(double aDynRangePercent)
{
  int oemState = cusOemIsConnected();
  if (oemState != CLARIUS_STATE_CONNECTED)
  {
    // Connection has not been established yet, parameter value will be set upon connection
    this->ImagingParameters->SetDynRangeDb(aDynRangePercent);
    LOG_INFO("Cached US parameter DynRangePercent = " << aDynRangePercent);
    return PLUS_SUCCESS;
  }

  // attempt to set parameter value
  if (cusOemSetParam(CusParam::DynamicRange, aDynRangePercent) < 0)
  {
    LOG_ERROR("Failed to set DynRange parameter");
    return PLUS_FAIL;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(CLARIUS_SHORT_DELAY_MS));

  // update imaging parameters & return successfully
  this->ImagingParameters->SetDynRangeDb(aDynRangePercent);
  LOG_INFO("Set US parameter DynRangePercent to " << aDynRangePercent);
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusClariusOEM::GetTimeGainCompensationDb(std::vector<double>& aTGC)
{
  int oemState = cusOemIsConnected();
  if (oemState != CLARIUS_STATE_CONNECTED)
  {
    // Connection has not been established yet, return cached parameter value
    return this->ImagingParameters->GetTimeGainCompensation(aTGC);
  }

  CusTgc cTGC;
  if (cusOemGetTgc(&cTGC) < 0)
  {
    LOG_ERROR("Failed to get time gain compensation parameter");
    return PLUS_FAIL;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(CLARIUS_SHORT_DELAY_MS));

  aTGC.clear();
  aTGC.resize(3);
  aTGC[0] = cTGC.top;
  aTGC[1] = cTGC.mid;
  aTGC[2] = cTGC.bottom;

  // ensure imaging parameters are up to date
  this->ImagingParameters->SetTimeGainCompensation(aTGC);

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusClariusOEM::SetTimeGainCompensationDb(const std::vector<double>& aTGC)
{
  if (aTGC.size() != 3)
  {
    LOG_ERROR("vtkPlusClariusOEM time gain compensation parameter must be provided a vector of exactly 3 doubles [top gain, mid gain, bottom gain]");
    return PLUS_FAIL;
  }

  int oemState = cusOemIsConnected();
  if (oemState != CLARIUS_STATE_CONNECTED)
  {
    // Connection has not been established yet, parameter value will be set upon connection
    this->ImagingParameters->SetTimeGainCompensation(aTGC);
    LOG_INFO("Cached US parameter TGC = [" << aTGC[0] << ", " << aTGC[1] << ", " << aTGC[2] << "]");
    return PLUS_SUCCESS;
  }

  CusTgc cTGC;
  cTGC.top = aTGC[0];
  cTGC.mid = aTGC[1];
  cTGC.bottom = aTGC[2];
  if (cusOemSetTgc(&cTGC) < 0)
  {
    LOG_ERROR("Failed to set time gain compensation parameter");
    return PLUS_FAIL;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(CLARIUS_SHORT_DELAY_MS));

  // update imaging parameters & return successfully
  this->ImagingParameters->SetTimeGainCompensation(aTGC);
  LOG_INFO("Set US parameter TGC to [" << aTGC[0] << ", " << aTGC[1] << ", " << aTGC[2] << "]");
  return PLUS_SUCCESS;
}
