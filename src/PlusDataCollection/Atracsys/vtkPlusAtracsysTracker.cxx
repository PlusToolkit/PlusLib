/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

// Local includes
#include "AtracsysTracker.h"
#include "vtkIGSIOAccurateTimer.h"
#include "vtkPlusAtracsysTracker.h"
#include "vtkPlusDataSource.h"

// VTK includes
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>

// System includes
#include <fstream>
#include <iostream>
#include <map>
#include <string>

// Atracsys includes
#include "ftkErrors.h"
#include "ftkEvent.h"
#include "ftkInterface.h"
#include "ftkOptions.h"
#include "ftkPlatform.h"
#include "ftkTypes.h"

// for convenience
#define ATR_SUCCESS AtracsysTracker::ATRACSYS_RESULT::SUCCESS
typedef AtracsysTracker::ATRACSYS_RESULT ATRACSYS_RESULT;

vtkStandardNewMacro(vtkPlusAtracsysTracker);

//----------------------------------------------------------------------------
// Define command strings
const char* vtkPlusAtracsysTracker::ATRACSYS_COMMAND_SET_FLAG = "SetFlag";
const char* vtkPlusAtracsysTracker::ATRACSYS_COMMAND_LED_ENABLED = "LedEnabled";
const char* vtkPlusAtracsysTracker::ATRACSYS_COMMAND_LASER_ENABLED = "LaserEnabled";
const char* vtkPlusAtracsysTracker::ATRACSYS_COMMAND_VIDEO_ENABLED = "VideoEnabled";
const char* vtkPlusAtracsysTracker::ATRACSYS_COMMAND_SET_LED_RGBF = "SetLED";
const char* vtkPlusAtracsysTracker::ATRACSYS_COMMAND_ENABLE_TOOL = "EnableTool";
const char* vtkPlusAtracsysTracker::ATRACSYS_COMMAND_ADD_TOOL = "AddTool";


//----------------------------------------------------------------------------
class vtkPlusAtracsysTracker::vtkInternal
{
public:
  vtkPlusAtracsysTracker* External;

  vtkInternal(vtkPlusAtracsysTracker* external)
    : External(external)
  {
    // stores correspondences between config file option names and atracsys option names
    this->DeviceOptionTranslator["MaxMeanRegistrationErrorMm"] = "Registration Mean Error";
    this->DeviceOptionTranslator["MaxMissingFiducials"] = "Matching Maximum Missing Points";
    this->DeviceOptionTranslator["SymmetriseCoordinates"] = "Symmetrise coordinates";
    this->DeviceOptionTranslator["EnableLasers"] = "Enables lasers";
    this->DeviceOptionTranslator["EnableUserLED"] = "Enables the user-LED";
    this->DeviceOptionTranslator["EnableIRstrobe"] = "Enables IR strobe";
    this->DeviceOptionTranslator["ImageCompressionThreshold"] = "Image Compression Threshold";
    this->DeviceOptionTranslator["ImageIntegrationTime"] = "Image Integration Time";
  }

  virtual ~vtkInternal()
  {
  }

  // mapping option names as written in config file to Atracsys option names
  std::map<std::string, std::string> DeviceOptionTranslator;
  // the actual options as read in the config file
  std::map<std::string, std::string> DeviceOptions{};

  // parameters used internally and their default values
  int ActiveMarkerPairingTimeSec = 0; // pairing time in seconds
  float MaxMeanRegistrationErrorMm = 2.0; // maximum mean registration error
  int MaxMissingFiducials = 0; // maximum number of missing fiducials

  // matches plus tool id to .ini geometry file names/paths
  std::map<std::string, std::string> PlusIdMappedToGeometryFilename;

  // matches fusionTrack internal tool geometry ID to Plus tool ID for updating tools
  std::map<int, std::string> FtkGeometryIdMappedToToolId;

  // Atracsys API wrapper class handle
  AtracsysTracker Tracker;

  // type of tracker connected
  AtracsysTracker::DEVICE_TYPE DeviceType = AtracsysTracker::UNKNOWN_DEVICE;
};

//----------------------------------------------------------------------------
vtkPlusAtracsysTracker::vtkPlusAtracsysTracker()
  : vtkPlusDevice()
  , Internal(new vtkInternal(this))
{
  LOG_TRACE("vtkPlusAtracsysTracker::vtkPlusAtracsysTracker()");
  this->FrameNumber = 0;
  this->StartThreadForInternalUpdates = true;
  this->InternalUpdateRate = 300;
}

//----------------------------------------------------------------------------
vtkPlusAtracsysTracker::~vtkPlusAtracsysTracker()
{
  LOG_TRACE("vtkPlusAtracsysTracker::~vtkPlusAtracsysTracker()");

  delete Internal;
  Internal = nullptr;
}

//----------------------------------------------------------------------------
void vtkPlusAtracsysTracker::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
std::string vtkPlusAtracsysTracker::GetSdkVersion()
{
  std::string v;
  this->Internal->Tracker.GetSDKversion(v);
  return v;
}

//----------------------------------------------------------------------------
std::string vtkPlusAtracsysTracker::GetCalibrationDate()
{
  std::string d;
  this->Internal->Tracker.GetCalibrationDate(d);
  return d;
}

//----------------------------------------------------------------------------
std::string vtkPlusAtracsysTracker::GetDeviceType()
{
  switch (this->Internal->DeviceType)
  {
  case AtracsysTracker::DEVICE_TYPE::FUSIONTRACK_250:
    return "fusionTrack250";
  case AtracsysTracker::DEVICE_TYPE::FUSIONTRACK_500:
    return "fusionTrack500";
  case AtracsysTracker::DEVICE_TYPE::SPRYTRACK_180:
    return "spryTrack180";
  case AtracsysTracker::DEVICE_TYPE::SPRYTRACK_300:
    return "spryTrack300";
  default:
    return "unknown";
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAtracsysTracker::GetCamerasCalibration(
  std::array<float, 10>& leftIntrinsic, std::array<float, 10>& rightIntrinsic,
  std::array<float, 3>& rightPosition, std::array<float, 3>& rightOrientation)
{
  ATRACSYS_RESULT result = this->Internal->Tracker.GetCamerasCalibration(
    leftIntrinsic, rightIntrinsic, rightPosition, rightOrientation);
  if (result != ATR_SUCCESS)
  {
    LOG_ERROR(this->Internal->Tracker.ResultToString(result));
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAtracsysTracker::GetLoadedGeometries(std::map<int, std::vector<std::array<float, 3>>>& geometries)
{
  if (this->Internal->Tracker.GetLoadedGeometries(geometries) != ATR_SUCCESS)
  {
    LOG_ERROR("Could not get loaded geometries");
    return PLUS_FAIL;
  }
  if (geometries.size() == 0)
  {
    LOG_ERROR("No loaded geometries");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
bool vtkPlusAtracsysTracker::IsVirtual() const
{
  return this->Internal->Tracker.IsVirtual();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAtracsysTracker::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  // Read and store all device options read in the xml config file
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  for (int i = 0; i < deviceConfig->GetNumberOfAttributes(); ++i)
  {
    this->Internal->DeviceOptions.emplace(deviceConfig->GetAttributeName(i), deviceConfig->GetAttributeValue(i));
    // also store parameters with vtkPlusDevice method
    this->SetParameter(deviceConfig->GetAttributeName(i), deviceConfig->GetAttributeValue(i));
  }

  XML_FIND_NESTED_ELEMENT_REQUIRED(dataSourcesElement, deviceConfig, "DataSources");
  for (int nestedElementIndex = 0; nestedElementIndex < dataSourcesElement->GetNumberOfNestedElements(); nestedElementIndex++)
  {
    vtkXMLDataElement* toolDataElement = dataSourcesElement->GetNestedElement(nestedElementIndex);
    if (STRCASECMP(toolDataElement->GetName(), "DataSource") != 0)
    {
      // if this is not a data source element, skip it
      continue;
    }
    if (toolDataElement->GetAttribute("Type") != NULL && STRCASECMP(toolDataElement->GetAttribute("Type"), "Tool") != 0)
    {
      // if this is not a Tool element, skip it
      continue;
    }
    std::string toolId(toolDataElement->GetAttribute("Id"));
    if (toolId.empty())
    {
      // tool doesn't have ID needed to generate transform
      LOG_ERROR("Failed to initialize Atracsys tool: DataSource Id is missing.");
      continue;
    }

    enum TRACKING_TYPE
    {
      ACTIVE,
      PASSIVE
    };

    TRACKING_TYPE toolTrackingType;
    XML_READ_ENUM2_ATTRIBUTE_NONMEMBER_OPTIONAL(TrackingType, toolTrackingType, toolDataElement, "ACTIVE", ACTIVE, "PASSIVE", PASSIVE);
    if (toolTrackingType == ACTIVE)
    {
      // active tool, can be loaded directly into FtkGeometryIdMappedToToolId
      int ftkGeometryId = -1;
      XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, GeometryId, ftkGeometryId, toolDataElement);
      if (ftkGeometryId != -1)
      {
        std::pair<int, std::string> thisTool(ftkGeometryId, toolId);
        this->Internal->FtkGeometryIdMappedToToolId.insert(thisTool);
      }
      else
      {
        LOG_ERROR("Active tool with Id " << toolId << " is missing a GeometryId");
        return PLUS_FAIL;
      }
    }
    else if (toolTrackingType == PASSIVE)
    {
      // passive tool, load into PlusIdMappedToGeometryFilename to have geometry loaded in InternalConnect
      const char* geometryFile;
      if ((geometryFile = toolDataElement->GetAttribute("GeometryFile")) != NULL)
      {
        std::pair<std::string, std::string> thisTool(toolId, geometryFile);
        this->Internal->PlusIdMappedToGeometryFilename.insert(thisTool);
      }
      else
      {
        LOG_ERROR("Passive tool with Id " << toolId << " is missing GeometryFile (.ini).");
        return PLUS_FAIL;
      }
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAtracsysTracker::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusAtracsysTracker::WriteConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAtracsysTracker::Probe()
{
  LOG_TRACE("vtkPlusAtracsysTracker::Probe");
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
const std::map<std::string, std::string>& vtkPlusAtracsysTracker::GetDeviceOptions() const
{
  return this->Internal->DeviceOptions;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAtracsysTracker::GetOptionValue(const std::string& optionName, std::string& optionValue)
{
  std::map<std::string, std::string>::const_iterator itd = this->Internal->DeviceOptions.find(optionName);
  if (itd != this->Internal->DeviceOptions.cend())
  {
    optionValue = this->Internal->DeviceOptions[optionName];
    return PLUS_SUCCESS;
  }
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
bool vtkPlusAtracsysTracker::TranslateOptionName(const std::string& optionName, std::string& translatedOptionName)
{
  std::map<std::string, std::string>::const_iterator itt =
    this->Internal->DeviceOptionTranslator.find(optionName);
  if (itt == this->Internal->DeviceOptionTranslator.cend())
  {
    return false;
  }
  else
  {
    translatedOptionName = itt->second;
    return true;
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAtracsysTracker::InternalConnect()
{
  LOG_TRACE("vtkPlusAtracsysTracker::InternalConnect");

  // Connect to device
  AtracsysTracker::ATRACSYS_RESULT result = this->Internal->Tracker.Connect();
  if (result != ATR_SUCCESS && result != AtracsysTracker::ATRACSYS_RESULT::WARNING_CONNECTED_IN_USB2)
  {
    LOG_ERROR(this->Internal->Tracker.ResultToString(result));
    return PLUS_FAIL;
  }
  else if (result == AtracsysTracker::ATRACSYS_RESULT::WARNING_CONNECTED_IN_USB2)
  {
    LOG_WARNING(this->Internal->Tracker.ResultToString(result));
  }

  // get device serial number in hex form
  std::ostringstream oss;
  uint64 sn;
  this->Internal->Tracker.GetDeviceId(sn);
  oss << "0x" << std::setw(16) << std::setfill('0') << std::hex << sn;
  this->DeviceId = oss.str();

  // get device type
  this->Internal->Tracker.GetDeviceType(this->Internal->DeviceType);

  std::map<std::string, std::string>::const_iterator itd;
  // handling options that are used only internally
  // ------- time allocated for activer marker pairing
  itd = this->Internal->DeviceOptions.find("ActiveMarkerPairingTimeSec");
  if (itd != this->Internal->DeviceOptions.cend())
  {
    if (strToInt32(itd->second, this->Internal->ActiveMarkerPairingTimeSec))
    {
      LOG_INFO("Marker pairing time is set to " << this->Internal->ActiveMarkerPairingTimeSec << " seconds, tracking will not start until this period is over.");
    }
  }

  // handling options that are used both internally and in the sdk
  // ------- max registration error in mm
  itd = this->Internal->DeviceOptions.find("MaxMeanRegistrationErrorMm");
  if (itd != this->Internal->DeviceOptions.cend())
  {
    if (strToFloat32(itd->second, this->Internal->MaxMeanRegistrationErrorMm))
    {
      if (this->Internal->MaxMeanRegistrationErrorMm < 0.1 || this->Internal->MaxMeanRegistrationErrorMm > 5.0)
      {
        this->Internal->MaxMeanRegistrationErrorMm = 2.0;
        LOG_WARNING("Invalid maximum mean registration error provided. Must be between 0.1 and 5 mm inclusive. Maximum mean registration error has been set to its default value of " << this->Internal->MaxMeanRegistrationErrorMm << "mm.");
      }
    }
  }
  else
  {
    this->Internal->DeviceOptions.emplace("MaxMeanRegistrationErrorMm",
      std::to_string(this->Internal->MaxMeanRegistrationErrorMm));
  }
  // ------- max number of missing fiducials
  itd = this->Internal->DeviceOptions.find("MaxMissingFiducials");
  if (itd != this->Internal->DeviceOptions.cend())
  {
    if (strToInt32(itd->second, this->Internal->MaxMissingFiducials))
    {
      if (this->Internal->MaxMissingFiducials < 0 || this->Internal->MaxMissingFiducials > 3)
      {
        this->Internal->MaxMissingFiducials = 0;
        LOG_WARNING("Invalid maximum number of missing fiducials provided. Must be between 0 and 3 inclusive. Maximum missing fiducials has been set to its default value of " << this->Internal->MaxMissingFiducials << ".");
      }
    }
  }
  else
  {
    this->Internal->DeviceOptions.emplace("MaxMissingFiducials",
      std::to_string(this->Internal->MaxMissingFiducials));
  }

  // set frame options (internally passed to sdk during connection)
  // ------- maximum number of events per frame included in the device's output
  itd = this->Internal->DeviceOptions.find("MaxAdditionalEventsNumber");
  if (itd != this->Internal->DeviceOptions.cend())
  {
    int value = -1;
    strToInt32(itd->second, value);
    if (value < 0) {
      LOG_WARNING("Invalid value for max events number per frame in output: " << itd->second
        << ". Default value used (" << this->Internal->Tracker.GetMaxAdditionalEventsNumber() << ")");
    }
    else
    {
      this->Internal->Tracker.SetMaxAdditionalEventsNumber(value);
    }
  }
  // ------- maximum number of 2D fiducials (in either left or right frame) included in the device's output
  itd = this->Internal->DeviceOptions.find("Max2dFiducialsNumber");
  if (itd != this->Internal->DeviceOptions.cend())
  {
    int value = -1;
    strToInt32(itd->second, value);
    if (value < 0)
    {
      LOG_WARNING("Invalid value for max 2D fiducials number in output: " << itd->second
        << ". Default value used (" << this->Internal->Tracker.GetMax2dFiducialsNumber() << ")");
    }
    else
    {
      this->Internal->Tracker.SetMax2dFiducialsNumber(value);
    }
  }
  // ------- maximum number of 3D fiducials (after triangulation) included in the device's output
  itd = this->Internal->DeviceOptions.find("Max3dFiducialsNumber");
  if (itd != this->Internal->DeviceOptions.cend())
  {
    int value = -1;
    strToInt32(itd->second, value);
    if (value < 0)
    {
      LOG_WARNING("Invalid value for max 3D fiducials number in output: " << itd->second
        << ". Default value used (" << this->Internal->Tracker.GetMax3dFiducialsNumber() << ")");
    }
    else
    {
      this->Internal->Tracker.SetMax3dFiducialsNumber(value);
    }
  }
  // ------- maximum number of markers included in the device's output
  itd = this->Internal->DeviceOptions.find("MaxMarkersNumber");
  if (itd != this->Internal->DeviceOptions.cend())
  {
    int value = -1;
    strToInt32(itd->second, value);
    if (value < 0)
    {
      LOG_WARNING("Invalid value for max markers number in output: " << itd->second
        << ". Default value used (" << this->Internal->Tracker.GetMaxMarkersNumber() << ")");
    }
    else
    {
      this->Internal->Tracker.SetMaxMarkersNumber(value);
    }
  }

  // Set actual device options and remove those unsuccessfully set
  // SpryTrack's Embedded processing option needs to be considered first.
  // By default, the spryTrack is in Embedded processing mode.
  // In Embedded processing mode, some other options will require the "Embedded " prefix.
  std::map<std::string, std::string>::iterator itr = this->Internal->DeviceOptions.find("Enable_embedded_processing");
  if (itr != this->Internal->DeviceOptions.end())
  {
    if (this->Internal->DeviceOptions.at("Enable_embedded_processing") == "0")
    {
      if (this->Internal->Tracker.SetSpryTrackProcessingType(AtracsysTracker::PROCESSING_ON_PC) != ATR_SUCCESS)
      {
        LOG_WARNING("Embedded processing could not be disabled.");
        this->Internal->DeviceOptions.erase(itr);
      }
    }
    else if (this->Internal->DeviceOptions.at("Enable_embedded_processing") == "1")
    {
      if (this->Internal->Tracker.SetSpryTrackProcessingType(AtracsysTracker::PROCESSING_ONBOARD) != ATR_SUCCESS)
      {
        this->Internal->DeviceOptions.erase(itr);
        LOG_WARNING("Embedded processing could not be enabled.");
      }
    }
    else
    {
      LOG_WARNING(this->Internal->DeviceOptions.at("Enable_embedded_processing") << " is not a correct value for Embedded processing.\nAccepted values are 0 (false) and 1 (true).");
      this->Internal->DeviceOptions.erase(itr);
    }
  }

  itr = this->Internal->DeviceOptions.begin();
  while (itr != this->Internal->DeviceOptions.end())
  {
    std::string translatedOptionName;

    /* Dirty hack circumventing a nomenclature discrepancy between ftk and stk API's, this may be fixed in a future API release */
    if (itr->first == "EnableIRstrobe" && (this->Internal->DeviceType == AtracsysTracker::DEVICE_TYPE::FUSIONTRACK_500
      || this->Internal->DeviceType == AtracsysTracker::DEVICE_TYPE::FUSIONTRACK_250))
    {
      // For fusiontracks, strobe on is 0 and strobe off is 1 (the opposite of sprytracks).
      // Fusiontracks also supports a mode 2 where the strobe turns on every other frame (unsupported by sprytracks at the moment)
      std::map<std::string, std::string> strobCorresp{ {"0","1"},{"1","0"}, {"2","2"} };
      if (strobCorresp.find(itr->second) != strobCorresp.end())
      {
        if (this->Internal->Tracker.SetOption("Strobe mode", strobCorresp.at(itr->second)) != ATR_SUCCESS)
        {
          itr = this->Internal->DeviceOptions.erase(itr);
          continue;
        }
      }
      else
      {
        LOG_WARNING("Unsupported strobe mode value " << itr->second);
        itr = this->Internal->DeviceOptions.erase(itr);
        continue;
      }
    }
    else/* end of hack*/ if (this->TranslateOptionName(itr->first, translatedOptionName)) // looking for the option in the translation dictionary
    {
      if (this->Internal->Tracker.SetOption(translatedOptionName, itr->second) != ATR_SUCCESS)
      {
        itr = this->Internal->DeviceOptions.erase(itr);
        continue;
      }
    }
    // option not found in dictionary, let's try to infer the option name by replacing _ by spaces
    else if (itr->first.find('_') != std::string::npos && itr->first != "Enable_embedded_processing")
    {
      std::string inferredOptionName = itr->first;
      std::replace(inferredOptionName.begin(), inferredOptionName.end(), '_', ' ');
      if (this->Internal->Tracker.SetOption(inferredOptionName, itr->second) != ATR_SUCCESS)
      {
        itr = this->Internal->DeviceOptions.erase(itr);
        continue;
      }
    }
    else if (itr->first != "AcquisitionRate" && itr->first != "Id" && itr->first != "Type"
      && itr->first != "ToolReferenceFrame" && itr->first != "Enable_embedded_processing")
    {
      LOG_WARNING("Unknown option \"" << itr->first << "\".");
      itr = this->Internal->DeviceOptions.erase(itr);
      continue;
    }
    ++itr;
  }

  // disable marker status streaming and battery charge streaming, they cause momentary pauses in tracking while sending
  if (this->Internal->DeviceType == AtracsysTracker::DEVICE_TYPE::SPRYTRACK_180)
  {
    if ((result = this->Internal->Tracker.EnableWirelessMarkerStatusStreaming(false)) != ATR_SUCCESS)
    {
      LOG_WARNING(this->Internal->Tracker.ResultToString(result));
    }
    if ((result = this->Internal->Tracker.EnableWirelessMarkerBatteryStreaming(false)) != ATR_SUCCESS)
    {
      LOG_WARNING(this->Internal->Tracker.ResultToString(result));
    }
  }

  // load passive geometries onto Atracsys
  std::map<std::string, std::string>::iterator it;
  for (it = begin(this->Internal->PlusIdMappedToGeometryFilename); it != end(this->Internal->PlusIdMappedToGeometryFilename); it++)
  {
    // load user defined geometry file
    // TODO: add check for conflicting marker IDs
    std::string geomFilePath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(it->second);
    int geometryId;
    if ((result = this->Internal->Tracker.LoadMarkerGeometryFromFile(geomFilePath, geometryId)) != ATR_SUCCESS)
    {
      LOG_ERROR(this->Internal->Tracker.ResultToString(result) << " This error occurred when trying to load geometry file at path: " << geomFilePath);
      return PLUS_FAIL;
    }
    std::pair<int, std::string> newTool(geometryId, it->first);
    this->Internal->FtkGeometryIdMappedToToolId.insert(newTool);
  }

  // if active marker pairing is desired, then its time > 0 second
  if (this->Internal->ActiveMarkerPairingTimeSec > 0)
  {
    // make LED blue during pairing
    this->Internal->Tracker.SetUserLEDState(0, 0, 255, 0);

    // pair active markers
    if ((result = this->Internal->Tracker.EnableWirelessMarkerPairing(true)) != ATR_SUCCESS)
    {
      LOG_ERROR(this->Internal->Tracker.ResultToString(result));
      return PLUS_FAIL;
    }
    LOG_INFO("Active marker pairing period started for " << this->Internal->ActiveMarkerPairingTimeSec << " seconds.");

    // sleep while waiting for tracker to pair active markers
    vtkIGSIOAccurateTimer::Delay(this->Internal->ActiveMarkerPairingTimeSec);

    LOG_INFO("Active marker pairing period ended.");

    if ((result = this->Internal->Tracker.EnableWirelessMarkerPairing(false)) != ATR_SUCCESS)
    {
      LOG_ERROR(this->Internal->Tracker.ResultToString(result));
      return PLUS_FAIL;
    }

    // make LED green, pairing is complete
    this->Internal->Tracker.SetUserLEDState(0, 255, 0, 0);

    // TODO: check number of active markers paired

    std::string markerInfo;
    this->Internal->Tracker.GetMarkerInfo(markerInfo);
    if (!markerInfo.empty())
    {
      LOG_INFO("Additional info about paired markers:" << markerInfo << std::endl);
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAtracsysTracker::InternalDisconnect()
{
  LOG_TRACE("vtkPlusAtracsysTracker::InternalDisconnect");
  this->Internal->Tracker.EnableUserLED(false);

  ATRACSYS_RESULT result;
  if ((result = this->Internal->Tracker.Disconnect()) != ATR_SUCCESS)
  {
    LOG_ERROR(this->Internal->Tracker.ResultToString(result));
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAtracsysTracker::InternalStartRecording()
{
  LOG_TRACE("vtkPlusAtracsysTracker::InternalStartRecording");
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAtracsysTracker::InternalStopRecording()
{
  LOG_TRACE("vtkPlusAtracsysTracker::InternalStopRecording");
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAtracsysTracker::PauseVirtualDevice()
{
  LOG_TRACE("vtkPlusAtracsysTracker::PauseVirtualDevice");
  this->Internal->Tracker.Pause(true);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAtracsysTracker::UnpauseVirtualDevice()
{
  LOG_TRACE("vtkPlusAtracsysTracker::UnpauseVirtualDevice");
  this->Internal->Tracker.Pause(false);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAtracsysTracker::InternalUpdate()
{
  LOG_TRACE("vtkPlusAtracsysTracker::InternalUpdate");
  double unfilteredTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();

  std::vector<AtracsysTracker::Marker> markers;
  std::map<std::string, std::string> events;

  uint64_t sdkTimestamp = 0;
  ATRACSYS_RESULT result = this->Internal->Tracker.GetMarkersInFrame(markers, events, sdkTimestamp);

  if (result == AtracsysTracker::ATRACSYS_RESULT::ERROR_NO_FRAME_AVAILABLE)
  {
    // waiting for frame
    return PLUS_SUCCESS;
  }
  else if (result != ATR_SUCCESS)
  {
    LOG_ERROR(this->Internal->Tracker.ResultToString(result));
    return PLUS_FAIL;
  }

  igsioFieldMapType customFields;

  // save sdk timestamp in customfield
  customFields["SdkTimestamp"].first = FRAMEFIELD_NONE;
  customFields["SdkTimestamp"].second = std::to_string(sdkTimestamp);

  // save event data in custom field
  for (const auto& it : events)
  {
    customFields["Event_" + it.first].first = FRAMEFIELD_NONE;
    customFields["Event_" + it.first].second = it.second;
  }

  std::map<int, std::string>::iterator it;
  for (it = this->Internal->FtkGeometryIdMappedToToolId.begin(); it != this->Internal->FtkGeometryIdMappedToToolId.end(); it++)
  {
    if (std::find(this->DisabledToolIds.begin(), this->DisabledToolIds.end(), it->second) != this->DisabledToolIds.end())
    {
      // tracking of this tool has been disabled
      vtkNew<vtkMatrix4x4> emptyTransform;
      igsioTransformName toolTransformName(it->second, this->GetToolReferenceFrameName());
      std::string toolSourceId = toolTransformName.GetTransformName();
      ToolTimeStampedUpdate(toolSourceId, emptyTransform.GetPointer(), TOOL_OUT_OF_VIEW, this->FrameNumber, unfilteredTimestamp);
      continue;
    }
    bool toolUpdated = false;

    std::vector<AtracsysTracker::Marker>::iterator mit;
    for (mit = markers.begin(); mit != markers.end(); mit++)
    {
      if (it->first != (int)mit->GetGeometryID())
      {
        continue;
      }
      // check if tool marker registration falls above maximum
      if (mit->GetFiducialRegistrationErrorMm() > this->Internal->MaxMeanRegistrationErrorMm)
      {
        LOG_WARNING("Maximum mean marker fiducial registration error exceeded for tool: " << it->second);
        continue;
      }

      // tool is seen with acceptable registration error
      toolUpdated = true;

      // save marker data in custom field
      std::ostringstream mos;
      mos.precision(3);
      mos << std::fixed << mit->GetMarkerStatus() << " " << mit->GetTrackingID() << " ";
      mos << mit->GetGeometryID() << " " << mit->GetGeometryPresenceMask() << " ";
      mos << mit->GetFiducialRegistrationErrorMm() << " ";
      mos << mit->GetFiducials().size(); // add number of fiducials
      customFields[it->second + "_info"].first = FRAMEFIELD_NONE;
      customFields[it->second + "_info"].second = mos.str();

      // save fiducial data in custom field
      const auto& fids = mit->GetFiducials();
      for (unsigned int f = 0; f < fids.size(); f++)
      {
        // 3D
        std::ostringstream f3dos;
        f3dos.precision(3);
        f3dos << std::fixed << fids[f].Fid3dStatus << " ";
        f3dos << fids[f].xMm << " " << fids[f].yMm << " " << fids[f].zMm << " ";
        f3dos << fids[f].epipolarErrorPx << " " << fids[f].probability << " " << fids[f].triangulErrorMm;
        customFields[it->second + "_fid" + std::to_string(f) + "_3D"].first = FRAMEFIELD_NONE;
        customFields[it->second + "_fid" + std::to_string(f) + "_3D"].second = f3dos.str();
        // 2D left
        std::ostringstream f2dLos;
        f2dLos.precision(3);
        f2dLos << std::fixed << fids[f].Fid2dLeftStatus << " ";
        f2dLos << fids[f].xLeftPx << " " << fids[f].yLeftPx << " ";
        f2dLos << fids[f].heightLeftPx << " " << fids[f].widthLeftPx << " ";
        f2dLos << fids[f].pixCountLeft;
        customFields[it->second + "_fid" + std::to_string(f) + "_2D_L"].first = FRAMEFIELD_NONE;
        customFields[it->second + "_fid" + std::to_string(f) + "_2D_L"].second = f2dLos.str();
        // 2D right
        std::ostringstream f2dRos;
        f2dRos.precision(3);
        f2dRos << std::fixed << fids[f].Fid2dRightStatus << " ";
        f2dRos << fids[f].xRightPx << " " << fids[f].yRightPx << " ";
        f2dRos << fids[f].heightRightPx << " " << fids[f].widthRightPx << " ";
        f2dRos << fids[f].pixCountRight;
        customFields[it->second + "_fid" + std::to_string(f) + "_2D_R"].first = FRAMEFIELD_NONE;
        customFields[it->second + "_fid" + std::to_string(f) + "_2D_R"].second = f2dRos.str();
      }

      // dump marker transform and custom data to buffer
      igsioTransformName toolTransformName(it->second, this->GetToolReferenceFrameName());
      std::string toolSourceId = toolTransformName.GetTransformName();
      ToolTimeStampedUpdate(toolSourceId, mit->GetTransformToTracker(), TOOL_OK, this->FrameNumber,
        unfilteredTimestamp, &customFields);
    }

    if (!toolUpdated)
    {
      // tool is not seen in this frame
      vtkNew<vtkMatrix4x4> emptyTransform;
      igsioTransformName toolTransformName(it->second, this->GetToolReferenceFrameName());
      std::string toolSourceId = toolTransformName.GetTransformName();
      ToolTimeStampedUpdate(toolSourceId, emptyTransform.GetPointer(), TOOL_OUT_OF_VIEW, this->FrameNumber,
        unfilteredTimestamp, &customFields);
    }
  }

  this->FrameNumber++;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// Command methods
//----------------------------------------------------------------------------

// LED
PlusStatus vtkPlusAtracsysTracker::SetLedEnabled(bool enabled)
{
  if (this->Internal->Tracker.EnableUserLED(enabled) != ATR_SUCCESS)
  {
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

PlusStatus vtkPlusAtracsysTracker::SetUserLEDState(int red, int green, int blue, int frequency, bool enabled /* = true */)
{
  if (this->Internal->Tracker.SetUserLEDState(red, green, blue, frequency, enabled) != ATR_SUCCESS)
  {
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// Tools
PlusStatus vtkPlusAtracsysTracker::SetToolEnabled(std::string toolId, bool enabled)
{
  if (enabled)
  {
    // remove any occurances of ToolId in DisabledToolIds
    this->DisabledToolIds.erase(std::remove(this->DisabledToolIds.begin(), this->DisabledToolIds.end(), toolId), this->DisabledToolIds.end());
    return PLUS_SUCCESS;
  }

  // tool should be disabled, if it exists add to disabled list
  bool toolExists = false;
  std::map<int, std::string>::iterator it;
  for (it = this->Internal->FtkGeometryIdMappedToToolId.begin(); it != this->Internal->FtkGeometryIdMappedToToolId.end(); it++)
  {
    if (igsioCommon::IsEqualInsensitive(it->second, toolId))
    {
      toolExists = true;
    }
  }

  if (!toolExists)
  {
    // trying to disable non-existant tool
    LOG_ERROR("Tried to disable non-existant tool.");
    return PLUS_FAIL;
  }

  this->DisabledToolIds.push_back(toolId);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAtracsysTracker::AddToolGeometry(std::string toolId, std::string geomString)
{
  // make sure geometry with toolId doesn't already exist
  bool toolExists = false;
  std::map<int, std::string>::iterator it;
  for (it = this->Internal->FtkGeometryIdMappedToToolId.begin(); it != this->Internal->FtkGeometryIdMappedToToolId.end(); it++)
  {
    if (igsioCommon::IsEqualInsensitive(it->second, toolId))
    {
      toolExists = true;
    }
  }

  if (toolExists)
  {
    // trying to add a tool with an already-existing ID
    LOG_ERROR("Tried to add tool with conflicting ToolId (" << toolId << ").");
    return PLUS_FAIL;
  }

  int geometryId;
  ATRACSYS_RESULT result;
  if ((result = this->Internal->Tracker.LoadMarkerGeometryFromString(geomString, geometryId)) != ATR_SUCCESS)
  {
    LOG_ERROR(this->Internal->Tracker.ResultToString(result) << " This error occurred when trying to load the following geometry information: " << geomString);
    return PLUS_FAIL;
  }

  // add datasources for this tool
  vtkSmartPointer<vtkPlusDataSource> aToolSource = vtkSmartPointer<vtkPlusDataSource>::New();
  aToolSource->SetReferenceCoordinateFrameName(this->ToolReferenceFrameName);
  aToolSource->SetType(DATA_SOURCE_TYPE_TOOL);
  aToolSource->SetId(toolId);
  this->AddTool(aToolSource);

  // TO DO: add fiducial data as separate sources ?
  //vtkSmartPointer<vtkPlusDataSource> aFids3dSource = vtkSmartPointer<vtkPlusDataSource>::New();
  //aFids3dSource->SetReferenceCoordinateFrameName(this->ToolReferenceFrameName);
  //aFids3dSource->SetType(DATA_SOURCE_TYPE_FIELDDATA);
  //aFids3dSource->SetId(toolId + "_Fids3d");
  //this->AddFieldDataSource(aFids3dSource);

  //vtkSmartPointer<vtkPlusDataSource> aFids2dLefSource = vtkSmartPointer<vtkPlusDataSource>::New();
  //aFids2dLefSource->SetReferenceCoordinateFrameName(this->ToolReferenceFrameName);
  //aFids2dLefSource->SetType(DATA_SOURCE_TYPE_FIELDDATA);
  //aFids2dLefSource->SetId(toolId + "_Fids2dLeft");
  //this->AddFieldDataSource(aFids2dLefSource);

  //vtkSmartPointer<vtkPlusDataSource> aFids2dRightSource = vtkSmartPointer<vtkPlusDataSource>::New();
  //aFids2dRightSource->SetReferenceCoordinateFrameName(this->ToolReferenceFrameName);
  //aFids2dRightSource->SetType(DATA_SOURCE_TYPE_FIELDDATA);
  //aFids2dRightSource->SetId(toolId + "_Fids2dRight");
  //this->AddFieldDataSource(aFids2dRightSource);

  // add output channel for this tool
  vtkPlusChannel* outChannel;
  if (this->GetFirstOutputChannel(outChannel) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to get output channel when adding Atracsys geometry.");
    return PLUS_FAIL;
  }
  outChannel->AddTool(aToolSource);
  //outChannel->AddFieldDataSource(aFids3dSource);
  //outChannel->AddFieldDataSource(aFids2dLefSource);
  //outChannel->AddFieldDataSource(aFids2dRightSource);

  // enable tool in Plus


  // register this tool internally
  std::pair<int, std::string> newTool(geometryId, toolId);
  this->Internal->FtkGeometryIdMappedToToolId.insert(newTool);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// Other
PlusStatus vtkPlusAtracsysTracker::SetLaserEnabled(bool enabled)
{
  if (this->Internal->Tracker.SetLaserEnabled(enabled) != ATR_SUCCESS)
  {
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

PlusStatus vtkPlusAtracsysTracker::SetVideoEnabled(bool enabled)
{
  // not implemented yet
  return PLUS_FAIL;
}
