/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "AtracsysTracker.h"

// System includes
#include <algorithm>
#include <cmath>
#include <fstream>
#include <map>
#include <string>

// Atracsys includes
#include "ftkErrors.h"
#include "ftkEvent.h"
#include "ftkOptions.h"
#include "ftkInterface.h"
#include "ftkPlatform.h"
#include "ftkTypes.h"

#include <igsioCommon.h>

#define ATRACSYS_BUFFER_SIZE 1024
#define RESET_DROPPED_FRAME_COUNT 1

//----------------------------------------------------------------------------
bool strToInt32(const std::string& str, int& var)
{
  bool noexception = false;
  try
  {
    var = std::atoi(str.c_str());
    noexception = true;
  }
  catch (std::invalid_argument& e)
  {
    LOG_WARNING(std::string("Cannot convert ") + str + std::string(" to int32"));
  }
  catch (std::out_of_range& e)
  {
    LOG_WARNING(str + std::string(" is out of range for int32"));
  }
  return noexception;
}

//----------------------------------------------------------------------------
bool strToFloat32(const std::string& str, float& var)
{
  bool noexception = false;
  try
  {
    var = std::atof(str.c_str());
    noexception = true;
  }
  catch (std::invalid_argument& e)
  {
    LOG_WARNING(std::string("Cannot convert ") + str + std::string(" to float"));
  }
  catch (std::out_of_range& e)
  {
    LOG_WARNING(str + std::string(" is out of range for float"));
  }
  return noexception;
}

//----------------------------------------------------------------------------
class AtracsysTracker::AtracsysInternal
{
public:
  AtracsysInternal()
  {
    // populate result to string
    ResultToStringMap[ERROR_UNABLE_TO_GET_FTK_HANDLE] = "Unable to get Atracsys library handle.";
    ResultToStringMap[ERROR_NO_DEVICE_CONNECTED] = "No Atracsys device connected.";
    ResultToStringMap[ERROR_UNABLE_TO_LOAD_MARKER] = "Unable to load marker.";
    ResultToStringMap[ERROR_FAILURE_TO_LOAD_INI] = "Failed to load marker's ini file.";
    ResultToStringMap[ERROR_OPTION_AVAILABLE_ONLY_ON_FTK] = "Attempted to call fusionTrack only option with non-fusionTrack device connected.";
    ResultToStringMap[ERROR_OPTION_AVAILABLE_ONLY_ON_STK] = "Attempted to call spryTrack only option with non-spryTrack device connected.";
    ResultToStringMap[ERROR_FAILED_TO_CLOSE_SDK] = "Failed to close the Atracsys SDK.";
    ResultToStringMap[ERROR_FAILED_TO_EXPORT_CALIB] = "Failed to export cameras calibration.";
    ResultToStringMap[ERROR_FAILED_TO_EXTRACT_FRAME_INFO] = "Failed to extract frame info.";
    ResultToStringMap[ERROR_CANNOT_CREATE_FRAME_INSTANCE] = "Failed to create frame.";
    ResultToStringMap[ERROR_CANNOT_INITIALIZE_FRAME] = "Failed to initialize frame.";
    ResultToStringMap[ERROR_NO_FRAME_AVAILABLE] = "No frame available from tracker.";
    ResultToStringMap[ERROR_INVALID_FRAME] = "Invalid frame received from tracker.";
    ResultToStringMap[ERROR_TOO_MANY_MARKERS] = "Too many markers in frame.";
    ResultToStringMap[ERROR_ENABLE_LASER] = "Failed to enable laser, this is a spryTrack only option.";
    ResultToStringMap[ERROR_SET_USER_LED] = "Failed to set the user LED.";
    ResultToStringMap[ERROR_ENABLE_USER_LED] = "Failed to enable / disable the user LED.";
    ResultToStringMap[ERROR_ENABLE_IMAGE_STREAMING] = "Failed to enable / disable image streaming.";
    ResultToStringMap[ERROR_ENABLE_WIRELESS_MARKER_PAIRING] = "Failed to enable / disable wireless marker pairing.";
    ResultToStringMap[ERROR_ENABLE_WIRELESS_MARKER_STATUS_STREAMING] = "Failed to enable / disable wireless marker status streaming.";
    ResultToStringMap[ERROR_ENABLE_WIRELESS_MARKER_BATTERY_STREAMING] = "Failed to enable / disable wireless marker battery streaming.";
    ResultToStringMap[ERROR_DISCONNECT_ATTEMPT_WHEN_NOT_CONNECTED] = "Disconnect called when not connected to tracker.";
    ResultToStringMap[ERROR_CANNOT_GET_MARKER_INFO] = "Cannot get info about paired wireless markers.";
    ResultToStringMap[ERROR_FAILED_TO_SET_STK_PROCESSING_TYPE] = "Failed to set spryTrack image processing type.";
  }

  virtual ~AtracsysInternal()
  {
    FtkLib = nullptr;
    LibVersion = "";
    CalibrationDate = "";
    TrackerSN = 0;
  }

  // is virtual device or not
  bool isVirtual = false;

  // is paused or not
  bool isPaused = true;

  // handle to FtkLib library
  ftkLibrary FtkLib = nullptr;

  // library version 
  std::string LibVersion;

  // calibration date
  std::string CalibrationDate;

  // serial number of tracker
  uint64 TrackerSN = 0;

  // ftk frame data
  ftkFrameQuery* Frame = nullptr;

  // mapping error code to user readable result string
  std::map<AtracsysTracker::ATRACSYS_RESULT, std::string> ResultToStringMap;

  // helper function to load ftkGeometry from file
  ATRACSYS_RESULT LoadFtkGeometryFromFile(const std::string& filename, ftkGeometry& geom);

  // helper function to load ftkGeometry from string
  ATRACSYS_RESULT LoadFtkGeometryFromString(const std::string& geomString, ftkGeometry& geom);

  std::map<int, std::vector<std::array<float, 3>>> Geometries;

  // correspondence between atracsys option name and its actual id in the sdk
  // this map is filled automatically by the sdk, DO NOT hardcode/change any id
  std::map<std::string, ftkOptionsInfo> DeviceOptionMap{};

  //----------------------------------------------------------------------------
  // callback function stores all option id
  static void DeviceOptionEnumerator(uint64_t serialNumber, void* userData, ftkOptionsInfo* option)
  {
    AtracsysTracker::AtracsysInternal* ptr =
      reinterpret_cast<AtracsysTracker::AtracsysInternal*>(userData);
    if (!ptr)
    {
      return;
    }
    ptr->DeviceOptionMap.emplace(option->name, *option);
  }

  // Code from ATRACSYS
  class IniFile
  {
  protected:
    //----------------------------------------------------------------------------
    long findEOL(char& c, char* addr, size_t size)
    {
      for (long u = 0; u < (long)size; u++)
      {
        c = addr[u];
        if (c == 0 || c == '\n') // note that MAX may only have a '\r'
        {
          return u;
        }
      }
      return -1;
    }

    //----------------------------------------------------------------------------
    bool parseLine(std::string& line)
    {
      size_t first_bracket = line.find_first_of("["),
        last_bracket = line.find_last_of("]"),
        equal = line.find_first_of("=");

      if (first_bracket != std::string::npos &&
        last_bracket != std::string::npos)
      {
        // Found section
        _currentSection = line.substr(first_bracket + 1,
          last_bracket - first_bracket - 1);
        sections[_currentSection] = KeyValues();
      }
      else if (equal != std::string::npos && _currentSection != "")
      {
        // Found property in a section
        std::string key = line.substr(0, equal),
          val = line.substr(equal + 1);
        sections[_currentSection][key] = val;
      }
      else
      {
        // If the line is empty, just skip it, if not and is a comment, just
        // skip it
        // as well, otherwise the parsing cannot be done.
        line.erase(remove_if(line.begin(),
          line.end(), isspace), line.end());
        if (!line.empty() && line.substr(0, 1) != ";")
        {
          return false;
        }
      }
      return true;
    }
    std::string _currentSection;

  public:
    typedef std::map< std::string, std::string > KeyValues;
    typedef std::map< std::string, KeyValues > Sections;

    Sections sections;

    //----------------------------------------------------------------------------
    bool parse(char* addr, size_t size)
    {
      sections.clear();
      _currentSection = "";

      std::string strLine;

      while (size)
      {
        char c;
        long lineSize = findEOL(c, addr, size);

        if (lineSize != 0)
        {
          if (lineSize > 0)
          {
            strLine = std::string(addr, lineSize);
          }
          else
          {
            strLine = std::string(addr);
          }

          strLine.erase(remove(strLine.begin(), strLine.end(), '\r'), strLine.end());
          if (!parseLine(strLine))
          {
            return false;
          }

          if (lineSize < 0)
          {
            return true; // EOF at the end of the line
          }
        }
        if (c == 0 || size == size_t(lineSize))
        {
          return true; // !!! eof not reached
        }
        addr += lineSize + 1;
        size -= lineSize + 1;
      }
      return true;
    }

    //----------------------------------------------------------------------------
    // Return false in case of syntax error
    bool save(std::string str)
    {
      FILE* file = fopen(str.c_str(), "wb");
      if (!file)
      {
        return false;
      }

      Sections::iterator iterS = sections.begin();

      while (iterS != sections.end())
      {
        fprintf(file, "[%s]\n", iterS->first.c_str());

        KeyValues& kw = iterS->second;
        KeyValues::iterator iterK = kw.begin();

        while (iterK != kw.end())
        {
          fprintf(file, "%s=%s\n",
            iterK->first.c_str(), iterK->second.c_str());
          iterK++;
        }
        iterS++;
      }
      fclose(file);
      return true;
    }

    //----------------------------------------------------------------------------
    bool toBuffer(char** out, size_t& outSize)
    {
      if (!out)
      {
        return false;
      }

      std::string buffer;
      char temp[ATRACSYS_BUFFER_SIZE];

      Sections::iterator iterS = sections.begin();

      while (iterS != sections.end())
      {
        sprintf(temp, "[%s]\n", iterS->first.c_str());
        buffer += temp;

        KeyValues& kw = iterS->second;
        KeyValues::iterator iterK = kw.begin();

        while (iterK != kw.end())
        {
          sprintf(temp, "%s=%s\n",
            iterK->first.c_str(), iterK->second.c_str());
          buffer += temp;
          iterK++;
        }
        iterS++;
      }

      outSize = buffer.length() + 1;
      char* str = new (std::nothrow) char[outSize];

      if (!str)
      {
        return false;
      }

      strncpy(str, buffer.c_str(), outSize - 1);
      str[outSize - 1] = 0;
      *out = str;

      return true;
    } // Buffer must be unallocated by user
  };

  //----------------------------------------------------------------------------
  bool checkSection(IniFile& p, const std::string& section)
  {
    if (p.sections.find(section) == p.sections.end())
    {
      return false;
    }
    return true;
  }

  //----------------------------------------------------------------------------
  bool checkKey(IniFile& p, const std::string& section, const std::string& key)
  {
    if (p.sections[section].find(key) == p.sections[section].end())
    {
      return false;
    }

    return true;
  }

  //----------------------------------------------------------------------------
  bool assignUint32(IniFile& p, const std::string& section,
    const std::string& key,
    uint32* variable)
  {
    if (!checkKey(p, section, key))
    {
      return false;
    }

    char* pEnd;
    std::string val(p.sections[section][key]);

    *variable = uint32(strtol(val.c_str(), &pEnd, 10));

    return true;
  }

  //----------------------------------------------------------------------------
  bool assignFloatXX(IniFile& p, const std::string& section,
    const std::string& key,
    floatXX* variable)
  {
    if (!checkKey(p, section, key))
    {
      return false;
    }

    char* pEnd;

    *variable =
      floatXX(strtod(p.sections[section][key].c_str(), &pEnd));

    return true;
  }

  //----------------------------------------------------------------------------
  bool LoadIniFile(std::ifstream& is, ftkGeometry& geometry)
  {
    std::string line, fileContent("");

    while (!is.eof())
    {
      getline(is, line);
      fileContent += line + "\n";
    }

    return ParseIniFile(fileContent, geometry);
  }

  //----------------------------------------------------------------------------
  // this method from https://thispointer.com/find-and-replace-all-occurrences-of-a-sub-string-in-c/
  void stringFindAndReplaceAll(std::string& data, std::string toSearch, std::string replaceStr)
  {
    // Get the first occurrence
    size_t pos = data.find(toSearch);

    // Repeat till end is reached
    while (pos != std::string::npos)
    {
      // Replace this occurrence of Sub String
      data.replace(pos, toSearch.size(), replaceStr);
      // Get the next occurrence from the current position
      pos = data.find(toSearch, pos + replaceStr.size());
    }
  }

  //----------------------------------------------------------------------------
  bool ParseIniFile(std::string fileContent, ftkGeometry& geometry)
  {
    stringFindAndReplaceAll(fileContent, "\\n", "\n");

    IniFile parser;

    if (!parser.parse(const_cast<char*>(fileContent.c_str()),
      fileContent.size()))
    {
      return false;
    }

    if (!checkSection(parser, "geometry"))
    {
      return false;
    }

    uint32 tmp;

    if (!assignUint32(parser, "geometry", "count", &tmp))
    {
      return false;
    }
    geometry.version = 0u;
    geometry.pointsCount = tmp;
    if (!assignUint32(parser, "geometry", "id", &geometry.geometryId))
    {
      return false;
    }

    char sectionName[10u];

    for (uint32 i(0u); i < geometry.pointsCount; ++i)
    {
      sprintf(sectionName, "fiducial%u", i);

      if (!checkSection(parser, sectionName))
      {
        return false;
      }

      if (!assignFloatXX(parser, sectionName, "x",
        &geometry.positions[i].x))
      {
        return false;
      }
      if (!assignFloatXX(parser, sectionName, "y",
        &geometry.positions[i].y))
      {
        return false;
      }
      if (!assignFloatXX(parser, sectionName, "z",
        &geometry.positions[i].z))
      {
        return false;
      }
    }

    std::vector<std::array<float, 3>> pts;
    for (uint32 i(0u); i < geometry.pointsCount; ++i)
    {
      std::array<float, 3> pt = { geometry.positions[i].x, geometry.positions[i].y, geometry.positions[i].z };
      pts.push_back(pt);
    }
    this->Geometries[geometry.geometryId] = pts;

    return true;
  }
};

//----------------------------------------------------------------------------
struct DeviceData
{
  uint64 SerialNumber;
  ftkDeviceType Type;
};

//----------------------------------------------------------------------------
void FusionTrackEnumerator(uint64 sn, void* user, ftkDeviceType devType)
{
  if (user != 0)
  {
    DeviceData* ptr = reinterpret_cast<DeviceData*>(user);
    ptr->SerialNumber = sn;
    ptr->Type = devType;
  }
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::AtracsysInternal::LoadFtkGeometryFromFile(const std::string& filename, ftkGeometry& geom)
{
  std::ifstream input;
  input.open(filename.c_str());

  if (!input.fail() && this->LoadIniFile(input, geom))
  {
    return ERROR_FAILURE_TO_LOAD_INI;
  }
  else
  {
    ftkBuffer buffer;
    buffer.reset();
    if (ftkGetData(this->FtkLib, this->TrackerSN, this->DeviceOptionMap["Data Directory"].id, &buffer) != ftkError::FTK_OK || buffer.size < 1u)
    {
      return ERROR_FAILURE_TO_LOAD_INI;
    }

    std::string fullFile(reinterpret_cast<char*>(buffer.data));
    fullFile += "\\" + filename;

    input.open(fullFile.c_str());

    if (!input.fail() && this->LoadIniFile(input, geom))
    {
      return SUCCESS;
    }
  }

  return ERROR_FAILURE_TO_LOAD_INI;
}
// END CODE FROM ATRACSYS

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::AtracsysInternal::LoadFtkGeometryFromString(const std::string& geomString, ftkGeometry& geom)
{
  if (this->ParseIniFile(geomString, geom))
  {
    return SUCCESS;
  }
  return ERROR_FAILURE_TO_LOAD_INI;
}

//----------------------------------------------------------------------------
// provided an option name with Atracsys' nomenclature, this method returns the pointer
// to the corresponding ftkOptionsInfo which contains various information about the option
// (notably its id and value type)
bool AtracsysTracker::GetOptionInfo(const std::string& optionName, const ftkOptionsInfo*& info)
{
  std::map<std::string, ftkOptionsInfo>::const_iterator it = this->Internal->DeviceOptionMap.find(optionName);
  if (it == this->Internal->DeviceOptionMap.cend())
  {
    return false;
  }
  else
  {
    info = &(it->second);
    return true;
  }
}

//----------------------------------------------------------------------------
// this method sets a value to an option in the device. The option name follows Atracsys' nomenclature.
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::SetOption(const std::string& optionName, const std::string& attributeValue)
{
  if (this->Internal->isVirtual)
  {
    return SUCCESS;
  }

  std::string optionStr{ optionName };
  // if Embedded processing is on and the option has an Embedded variant, add the prefix
  if (isOnboardProcessing && this->Internal->DeviceOptionMap.find("Embedded " + optionName) != this->Internal->DeviceOptionMap.end())
  {
    optionStr = "Embedded " + optionStr;
  }

  const ftkOptionsInfo* info;

  if (!this->GetOptionInfo(optionStr, info))
  {
    LOG_WARNING(std::string("Info for option \"") + optionStr + std::string("\" not found."));
    return ERROR_OPTION_NOT_FOUND;
  }

  if (info->type == ftkOptionType::FTK_INT32)
  {
    int32_t val;
    if (!strToInt32(attributeValue, val))
    {
      return ERROR_SET_OPTION;
    }

    if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, info->id, val) != ftkError::FTK_OK)
    {
      ftkBuffer buffer{};
      if (ftkGetLastErrorString(this->Internal->FtkLib, sizeof(buffer.data), buffer.data) == ftkError::FTK_OK)
      {
        LOG_WARNING(std::string(buffer.data));
      }
      else
      {
        LOG_WARNING(std::string("Unknown error setting option ") + optionStr);
      }
    }
  }
  else if (info->type == ftkOptionType::FTK_FLOAT32)
  {
    float_t val;
    if (!strToFloat32(attributeValue, val))
    {
      return ERROR_SET_OPTION;
    }

    if (ftkSetFloat32(this->Internal->FtkLib, this->Internal->TrackerSN, info->id, val) != ftkError::FTK_OK)
    {
      ftkBuffer buffer{};
      if (ftkGetLastErrorString(this->Internal->FtkLib, sizeof(buffer.data), buffer.data) == ftkError::FTK_OK)
      {
        LOG_WARNING(std::string(buffer.data));
      }
      else
      {
        LOG_WARNING(std::string("Unknown error setting option ") + optionStr);
      }
    }
  }
  else if (info->type == ftkOptionType::FTK_DATA)
  {
    LOG_WARNING(std::string("Option of type \"data\" not supported yet"));
  }
  LOG_INFO(std::string("Option \"") + optionStr + std::string("\" successfully set at value ") + attributeValue);

  return SUCCESS;
}

// ------------------------------------------
// universally available options & methods
// ------------------------------------------

AtracsysTracker::AtracsysTracker()
  : Internal(new AtracsysInternal()) {}

AtracsysTracker::~AtracsysTracker()
{
  delete Internal;
  Internal = nullptr;
}

//----------------------------------------------------------------------------
void AtracsysTracker::Pause(bool tof)
{
  this->Internal->isPaused = tof;
}

//----------------------------------------------------------------------------
bool AtracsysTracker::IsOnboardProcessing()
{
  return isOnboardProcessing;
}

//----------------------------------------------------------------------------
bool AtracsysTracker::IsVirtual()
{
  return this->Internal->isVirtual;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::Connect()
{
  if (this->Internal->FtkLib != nullptr && this->Internal->TrackerSN != 0)
  {
    // already connected
    return SUCCESS;
  }

  // initialize SDK
  this->Internal->FtkLib = ftkInit();

  if (this->Internal->FtkLib == NULL)
  {
    return ERROR_UNABLE_TO_GET_FTK_HANDLE;
  }

  DeviceData device;
  device.SerialNumber = 0uLL;

  // scan for devices
  ftkError err = ftkEnumerateDevices(this->Internal->FtkLib, FusionTrackEnumerator, &device);
  if (err != ftkError::FTK_OK && err != ftkError::FTK_WAR_USB_TOO_SLOW)
  {
    ftkClose(&this->Internal->FtkLib);
    this->Internal->FtkLib = nullptr;
    return ERROR_NO_DEVICE_CONNECTED;
  }

  if (device.SerialNumber == 0uLL)
  {
    ftkClose(&this->Internal->FtkLib);
    this->Internal->FtkLib = nullptr;
    return ERROR_NO_DEVICE_CONNECTED;
  }

  this->Internal->TrackerSN = device.SerialNumber;

  ftkBuffer sdkVersion;
  ftkVersion(&sdkVersion);
  this->Internal->LibVersion = sdkVersion.data;

  switch (device.Type)
  {
  case ftkDeviceType::DEV_SPRYTRACK_180:
    this->DeviceType = SPRYTRACK_180;
    break;
  case ftkDeviceType::DEV_SPRYTRACK_300:
    this->DeviceType = SPRYTRACK_300;
    break;
  case ftkDeviceType::DEV_FUSIONTRACK_500:
    this->DeviceType = FUSIONTRACK_500;
    break;
  case ftkDeviceType::DEV_FUSIONTRACK_250:
    this->DeviceType = FUSIONTRACK_250;
    break;
  default:
    this->DeviceType = UNKNOWN_DEVICE;
  }

  // allocate memory for ftk frame to be used throughout life of the object
  this->Internal->Frame = ftkCreateFrame();

  if (this->Internal->Frame == nullptr)
  {
    ftkDeleteFrame(this->Internal->Frame);
    this->Internal->Frame = nullptr;
    return ERROR_CANNOT_CREATE_FRAME_INSTANCE;
  }

  if (ftkSetFrameOptions(false, this->MaxAdditionalEventsNumber,
    this->Max2dFiducialsNumber, this->Max2dFiducialsNumber,
    this->Max3dFiducialsNumber, this->MaxMarkersNumber,
    this->Internal->Frame) != ftkError::FTK_OK)
  {
    ftkDeleteFrame(this->Internal->Frame);
    this->Internal->Frame = nullptr;
    return ERROR_CANNOT_INITIALIZE_FRAME;
  }

  if (ftkEnumerateOptions(this->Internal->FtkLib, this->Internal->TrackerSN,
    &AtracsysTracker::AtracsysInternal::DeviceOptionEnumerator, this->Internal) != ftkError::FTK_OK
    || this->Internal->DeviceOptionMap.find("Data Directory") == this->Internal->DeviceOptionMap.cend())
  {
    return ERROR_OPTION_NOT_FOUND;
  }

  // Needs to be after the device option enumeration
  const ftkOptionsInfo* info;
  if (!this->GetOptionInfo("Calibration processing datetime", info))
  {
    LOG_ERROR("Option unknown: \"Calibration processing datetime\"");
    return ERROR_OPTION_NOT_FOUND;
  }
  ftkBuffer buff;
  ftkGetData(this->Internal->FtkLib, this->Internal->TrackerSN, info->id, &buff);
  this->Internal->CalibrationDate = std::string(buff.data);

  // Check whether onboard processing is off or on (spryTrack only)
  if (this->DeviceType == SPRYTRACK_180 || this->DeviceType == SPRYTRACK_300)
  {
    if (!this->GetOptionInfo("Enable embedded processing", info))
    {
      LOG_WARNING(std::string("Embedded processing not part of the option list."));
      return ERROR_OPTION_NOT_FOUND;
    }
    else
    {
      int32 val;
      ftkGetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, info->id, &val, ftkOptionGetter::FTK_VALUE);
      isOnboardProcessing = (val == 1) ? true : false;
      LOG_INFO("Embedded processing is initially " << (isOnboardProcessing ? "enabled" : "disabled"));
    }
  }

  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::Disconnect()
{
  if (this->Internal->isVirtual)
  {
    return SUCCESS;
  }

  if (this->Internal->FtkLib == nullptr && this->Internal->TrackerSN == 0)
  {
    return ERROR_DISCONNECT_ATTEMPT_WHEN_NOT_CONNECTED;
  }

  // de-allocate memory for frame
  ftkDeleteFrame(this->Internal->Frame);
  this->Internal->Frame = nullptr;

  ftkError err = ftkClose(&this->Internal->FtkLib);
  if (err != ftkError::FTK_OK)
  {
    return ERROR_FAILED_TO_CLOSE_SDK;
  }
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::GetSDKversion(std::string& version)
{
  version = this->Internal->LibVersion;
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::GetCalibrationDate(std::string& date)
{
  date = this->Internal->CalibrationDate;
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::GetDeviceType(DEVICE_TYPE& deviceType)
{
  deviceType = this->DeviceType;
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::GetCamerasCalibration(
  std::array<float, 10>& leftIntrinsic, std::array<float, 10>& rightIntrinsic,
  std::array<float, 3>& rightPosition, std::array<float, 3>& rightOrientation)
{
  if (!this->Internal->isVirtual || this->SetOption("Calibration export", "1") != SUCCESS)
  {
    LOG_ERROR("Could not export calibration.");
    return ERROR_FAILED_TO_EXPORT_CALIB;
  }
  else
  {
    if (ftkGetLastFrame(this->Internal->FtkLib, this->Internal->TrackerSN, this->Internal->Frame, 20) != ftkError::FTK_OK)
    {
      return ERROR_NO_FRAME_AVAILABLE;
    }
    else
    {
      ftkFrameInfoData info;
      info.WantedInformation = ftkInformationType::CalibrationParameters;
      if (ftkExtractFrameInfo(this->Internal->Frame, &info) != ftkError::FTK_OK)
      {
        return ERROR_FAILED_TO_EXTRACT_FRAME_INFO;
      }
      else
      {
        ftkStereoParameters sps = info.Calibration;
        leftIntrinsic = { sps.LeftCamera.FocalLength[0], sps.LeftCamera.FocalLength[1],
        sps.LeftCamera.OpticalCentre[0], sps.LeftCamera.OpticalCentre[1],
        sps.LeftCamera.Distorsions[0], sps.LeftCamera.Distorsions[1], sps.LeftCamera.Distorsions[2],
        sps.LeftCamera.Distorsions[3], sps.LeftCamera.Distorsions[4], sps.LeftCamera.Skew };
        rightIntrinsic = { sps.RightCamera.FocalLength[0], sps.RightCamera.FocalLength[1],
        sps.RightCamera.OpticalCentre[0], sps.RightCamera.OpticalCentre[1],
        sps.RightCamera.Distorsions[0], sps.RightCamera.Distorsions[1], sps.RightCamera.Distorsions[2],
        sps.RightCamera.Distorsions[3], sps.RightCamera.Distorsions[4], sps.RightCamera.Skew };
        rightPosition = { sps.Translation[0], sps.Translation[1], sps.Translation[2] };
        rightOrientation = { sps.Rotation[0], sps.Rotation[1], sps.Rotation[2] };
      }
    }
  }
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::GetDeviceId(uint64_t& id)
{
  id = this->Internal->TrackerSN;
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::LoadMarkerGeometryFromFile(std::string filePath, int& geometryId)
{
  ftkGeometry geom;
  this->Internal->LoadFtkGeometryFromFile(filePath, geom);
  if (!this->Internal->isVirtual &&
    ftkSetGeometry(this->Internal->FtkLib, this->Internal->TrackerSN, &geom) != ftkError::FTK_OK)
  {
    return ERROR_UNABLE_TO_LOAD_MARKER;
  }
  geometryId = geom.geometryId;
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::LoadMarkerGeometryFromString(std::string geomString, int& geometryId)
{
  ftkGeometry geom;
  this->Internal->LoadFtkGeometryFromString(geomString, geom);
  if (!this->Internal->isVirtual &&
    ftkSetGeometry(this->Internal->FtkLib, this->Internal->TrackerSN, &geom) != ftkError::FTK_OK)
  {
    return ERROR_UNABLE_TO_LOAD_MARKER;
  }
  geometryId = geom.geometryId;
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::GetMarkerInfo(std::string& markerInfo)
{
  if (this->Internal->isVirtual)
  {
    return ERROR_CANNOT_GET_MARKER_INFO;
  }

  // get correct device option number
  const ftkOptionsInfo* info;
  if (!this->GetOptionInfo("Active Wireless Markers info", info))
  {
    return ERROR_OPTION_NOT_FOUND;
  }

  ftkBuffer buffer;
  if (ftkGetData(this->Internal->FtkLib, this->Internal->TrackerSN, info->id, &buffer) != ftkError::FTK_OK)
  {
    return ERROR_CANNOT_GET_MARKER_INFO;
  }
  markerInfo = std::string(buffer.data, buffer.data + buffer.size);
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::GetLoadedGeometries(std::map<int, std::vector<std::array<float, 3>>>& geometries)
{
  geometries = this->Internal->Geometries;
  return SUCCESS;
}

//----------------------------------------------------------------------------
std::string AtracsysTracker::ResultToString(AtracsysTracker::ATRACSYS_RESULT result)
{
  std::map<AtracsysTracker::ATRACSYS_RESULT, std::string>::iterator it;
  it = this->Internal->ResultToStringMap.find(result);
  if (it != end(this->Internal->ResultToStringMap))
  {
    return it->second;
  }
  return std::string("Unknown error has occured. Result value: " + result);
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::GetMarkersInFrame(std::vector<Marker>& markers,
  std::map<std::string, std::string>& events, uint64_t& sdkTimestamp)
{
  ftkError err = ftkGetLastFrame(this->Internal->FtkLib, this->Internal->TrackerSN, this->Internal->Frame, 20);
  if (err != ftkError::FTK_OK)
  {
    return ERROR_NO_FRAME_AVAILABLE;
  }

  switch (this->Internal->Frame->markersStat)
  {
  case ftkQueryStatus::QS_WAR_SKIPPED:
    return ERROR_INVALID_FRAME;
  case ftkQueryStatus::QS_ERR_INVALID_RESERVED_SIZE:
    return ERROR_INVALID_FRAME;
  case ftkQueryStatus::QS_OK:
    break;
  default:
    return ERROR_INVALID_FRAME;
  }

  if (this->Internal->Frame->markersStat == ftkQueryStatus::QS_ERR_OVERFLOW)
  {
    return ERROR_TOO_MANY_MARKERS;
  }

  // make sure markers vector is empty before populating
  markers.clear();

  for (size_t m = 0; m < this->Internal->Frame->markersCount; m++)
  {
    const ftkMarker& marker = this->Internal->Frame->markers[m];

    // A marker
    vtkNew<vtkMatrix4x4> toolToTracker;
    for (int row = 0; row < 3; row++)
    {
      toolToTracker->SetElement(row, 3, marker.translationMM[row]);
      for (int column = 0; column < 3; column++)
      {
        toolToTracker->SetElement(row, column, marker.rotation[row][column]);
      }
    }

    Marker atracsysMarker(marker.status, marker.id, marker.geometryId,
      toolToTracker.GetPointer(), marker.geometryPresenceMask, marker.registrationErrorMM);

    // Add the corresponding 3D fiducials
    for (size_t f3 = 0; f3 < FTK_MAX_FIDUCIALS; f3++)
    {
      const uint32 f = marker.fiducialCorresp[f3];
      if (f == INVALID_ID) // no more 3D fids for this marker
      {
        continue;
      }
      else
      {
        Fiducial fid;
        // 3D stuff
        const ftk3DFiducial& ftkFid3d = this->Internal->Frame->threeDFiducials[f];
        fid.Fid3dStatus = ftkFid3d.status;
        fid.xMm = ftkFid3d.positionMM.x;
        fid.yMm = ftkFid3d.positionMM.y;
        fid.zMm = ftkFid3d.positionMM.z;
        fid.epipolarErrorPx = ftkFid3d.epipolarErrorPixels;
        fid.probability = ftkFid3d.probability;
        fid.triangulErrorMm = ftkFid3d.triangulationErrorMM;
        // Left 2D stuff
        const ftkRawData& leftRaw = this->Internal->Frame->rawDataLeft[ftkFid3d.leftIndex];
        fid.Fid2dLeftStatus = leftRaw.status;
        fid.xLeftPx = leftRaw.centerXPixels;
        fid.yLeftPx = leftRaw.centerYPixels;
        fid.heightLeftPx = leftRaw.height;
        fid.widthLeftPx = leftRaw.width;
        fid.pixCountLeft = leftRaw.pixelsCount;
        // Right 2D stuff
        const ftkRawData& rightRaw = this->Internal->Frame->rawDataRight[ftkFid3d.rightIndex];
        fid.Fid2dRightStatus = rightRaw.status;
        fid.xRightPx = rightRaw.centerXPixels;
        fid.yRightPx = rightRaw.centerYPixels;
        fid.heightRightPx = rightRaw.height;
        fid.widthRightPx = rightRaw.width;
        fid.pixCountRight = rightRaw.pixelsCount;

        if (!atracsysMarker.AddFiducial(fid))
        {
          return ERROR_TOO_MANY_FIDUCIALS;
        }
      }
    }
    markers.push_back(atracsysMarker);
  }

  // make sure events map is empty before populating
  events.clear();

  // Parse events
  for (size_t e = 0; e < this->Internal->Frame->eventsCount; e++)
  {
    const ftkEvent& event = *this->Internal->Frame->events[e];

    if (event.Type == FtkEventType::fetTempV4)
    {
      std::stringstream ss;
      const EvtTemperatureV4Payload* ptr = reinterpret_cast<EvtTemperatureV4Payload*>(event.Data);
      for (unsigned int i = 0; i < event.Payload / sizeof(EvtTemperatureV4Payload) - 1; i++, ++ptr)
      {
        ss << ptr->SensorId << " " << ptr->SensorValue << " ";
      }
      ss << ptr->SensorId << " " << ptr->SensorValue;
      events.emplace("tempv4", ss.str());
    }
  }

  // Save sdk timestamp
  sdkTimestamp = this->Internal->Frame->imageHeader->timestampUS;

  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::SetUserLEDState(int red, int green, int blue, int frequency, bool enabled /* = true */)
{
  if (this->Internal->isVirtual)
  {
    return SUCCESS;
  }

  // get correct device option number
  const ftkOptionsInfo* info;
  if (!this->GetOptionInfo("User-LED frequency", info))
  {
    return ERROR_OPTION_NOT_FOUND;
  }
  if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, info->id, frequency) != ftkError::FTK_OK)
  {
    return ERROR_SET_USER_LED;
  }
  if (!this->GetOptionInfo("User-LED red component", info))
  {
    return ERROR_OPTION_NOT_FOUND;
  }
  if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, info->id, red) != ftkError::FTK_OK)
  {
    return ERROR_SET_USER_LED;
  }
  if (!this->GetOptionInfo("User-LED green component", info))
  {
    return ERROR_OPTION_NOT_FOUND;
  }
  if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, info->id, green) != ftkError::FTK_OK)
  {
    return ERROR_SET_USER_LED;
  }
  if (!this->GetOptionInfo("User-LED blue component", info))
  {
    return ERROR_OPTION_NOT_FOUND;
  }
  if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, info->id, blue) != ftkError::FTK_OK)
  {
    return ERROR_SET_USER_LED;
  }
  // enable user LED
  return this->EnableUserLED(enabled);
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::EnableUserLED(bool enabled)
{
  if (this->Internal->isVirtual)
  {
    return SUCCESS;
  }

  // get correct device option number
  const ftkOptionsInfo* info;
  if (!this->GetOptionInfo("Enables the user-LED", info))
  {
    return ERROR_OPTION_NOT_FOUND;
  }
  if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, info->id, enabled) != ftkError::FTK_OK)
  {
    return ERROR_ENABLE_USER_LED;
  }
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::SetLaserEnabled(bool enabled)
{
  if (this->Internal->isVirtual)
  {
    return SUCCESS;
  }

  // get correct device option number
  const ftkOptionsInfo* info;
  if (!this->GetOptionInfo("Enables lasers", info))
  {
    return ERROR_OPTION_NOT_FOUND;
  }

  int laserEnabledValue = enabled ? 3 : 0;  // 3 = both lasers on

  if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, info->id, laserEnabledValue) != ftkError::FTK_OK)
  {
    return ERROR_ENABLE_LASER;
  }
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::EnableWirelessMarkerPairing(bool enabled)
{
  if (this->Internal->isVirtual)
  {
    return SUCCESS;
  }

  // get correct device option number
  const ftkOptionsInfo* info;
  if (!this->GetOptionInfo("Active Wireless Pairing Enable", info))
  {
    return ERROR_OPTION_NOT_FOUND;
  }

  if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, info->id, enabled) != ftkError::FTK_OK)
  {
    return ERROR_ENABLE_WIRELESS_MARKER_PAIRING;
  }
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::EnableWirelessMarkerStatusStreaming(bool enabled)
{
  if (this->Internal->isVirtual)
  {
    return SUCCESS;
  }

  // get correct device option number
  const ftkOptionsInfo* info;
  if (!this->GetOptionInfo("Active Wireless button statuses streaming", info))
  {
    return ERROR_OPTION_NOT_FOUND;
  }

  if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, info->id, enabled) != ftkError::FTK_OK)
  {
    return ERROR_ENABLE_WIRELESS_MARKER_STATUS_STREAMING;
  }
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::EnableWirelessMarkerBatteryStreaming(bool enabled)
{
  if (this->Internal->isVirtual)
  {
    return SUCCESS;
  }

  // get correct device option number
  const ftkOptionsInfo* info;
  if (!this->GetOptionInfo("Active Wireless battery state streaming", info))
  {
    return ERROR_OPTION_NOT_FOUND;
  }

  if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, info->id, enabled) != ftkError::FTK_OK)
  {
    return ERROR_ENABLE_WIRELESS_MARKER_BATTERY_STREAMING;
  }
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::SetMaxAdditionalEventsNumber(int n)
{
  if (n < 0)
  {
    return ERROR_SET_OPTION;
  }
  this->MaxAdditionalEventsNumber = n;
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::SetMax2dFiducialsNumber(int n)
{
  if (n < 0)
  {
    return ERROR_SET_OPTION;
  }
  this->Max2dFiducialsNumber = n;
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::SetMax3dFiducialsNumber(int n)
{
  if (n < 0)
  {
    return ERROR_SET_OPTION;
  }
  this->Max3dFiducialsNumber = n;
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::SetMaxMarkersNumber(int n)
{
  if (n < 0)
  {
    return ERROR_SET_OPTION;
  }
  this->MaxMarkersNumber = n;
  return SUCCESS;
}

// ------------------------------------------
// spryTrack only options
// ------------------------------------------

AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::EnableOnboardProcessing(bool enabled)
{
  if (this->Internal->isVirtual)
  {
    return SUCCESS;
  }

  // get correct device option number
  const ftkOptionsInfo* info;
  if (!this->GetOptionInfo("Enable embedded processing", info))
  {
    return ERROR_OPTION_NOT_FOUND;
  }

  if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, info->id, enabled) != ftkError::FTK_OK)
  {
    return ERROR_ENABLE_ONBOARD_PROCESSING;
  }
  LOG_INFO("Embedded processing successfully " << (enabled ? "enabled" : "disabled"));
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::EnableImageStreaming(bool enabled)
{
  if (this->Internal->isVirtual)
  {
    return SUCCESS;
  }

  // get correct device option number
  const ftkOptionsInfo* info;
  if (!this->GetOptionInfo("Enable images sending", info))
  {
    return ERROR_OPTION_NOT_FOUND;
  }

  if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, info->id, enabled) != ftkError::FTK_OK)
  {
    return ERROR_ENABLE_IMAGE_STREAMING;
  }
  LOG_INFO("Image streaming successfully " << (enabled ? "enabled" : "disabled"));
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::SetSpryTrackProcessingType(AtracsysTracker::SPRYTRACK_IMAGE_PROCESSING_TYPE processingType)
{
  if (this->DeviceType != SPRYTRACK_180 && this->DeviceType != SPRYTRACK_300)
  {
    LOG_WARNING("Embedded processing is available only on spryTracks.");
    return ERROR_OPTION_AVAILABLE_ONLY_ON_STK;
  }
  bool succeeded = true;
  if (processingType == PROCESSING_ONBOARD)
  {
    succeeded = succeeded && (this->EnableOnboardProcessing(true) == SUCCESS);
    succeeded = succeeded && (this->EnableImageStreaming(false) == SUCCESS);
    isOnboardProcessing = true;
  }
  else if (processingType == PROCESSING_ON_PC)
  {
    succeeded = succeeded && (this->EnableOnboardProcessing(false) == SUCCESS);
    succeeded = succeeded && (this->EnableImageStreaming(true) == SUCCESS);
    isOnboardProcessing = false;
  }

  if (!succeeded)
  {
    return ERROR_FAILED_TO_SET_STK_PROCESSING_TYPE;
  }
  return SUCCESS;
}

// ------------------------------------------
// fusionTrack only options
// ------------------------------------------

AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::GetDroppedFrameCount(int& droppedFrameCount)
{
  if (this->Internal->isVirtual)
  {
    droppedFrameCount = 0;
    return SUCCESS;
  }

  if (this->DeviceType == FUSIONTRACK_250 || this->DeviceType == FUSIONTRACK_500)
  {
    int32 lost = 0, corrupted = 0;
    // get correct device option number
    const ftkOptionsInfo* info;
    if (!this->GetOptionInfo("Counter of lost frames", info))
    {
      return ERROR_OPTION_NOT_FOUND;
    }
    ftkGetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, info->id, &lost, ftkOptionGetter::FTK_VALUE);
    if (!this->GetOptionInfo("Counter of corrupted frames", info))
    {
      return ERROR_OPTION_NOT_FOUND;
    }
    ftkGetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, info->id, &corrupted, ftkOptionGetter::FTK_VALUE);

    droppedFrameCount = lost + corrupted;
    return SUCCESS;
  }
  return ERROR_OPTION_AVAILABLE_ONLY_ON_FTK;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::ResetLostFrameCount()
{
  if (this->Internal->isVirtual)
  {
    return SUCCESS;
  }

  if (this->DeviceType == FUSIONTRACK_250 || this->DeviceType == FUSIONTRACK_500)
  {
    // get correct device option number
    const ftkOptionsInfo* info;
    if (!this->GetOptionInfo("Resets lost counters", info))
    {
      return ERROR_OPTION_NOT_FOUND;
    }
    ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, info->id, RESET_DROPPED_FRAME_COUNT);
    return SUCCESS;
  }
  return ERROR_OPTION_AVAILABLE_ONLY_ON_FTK;
}

// ------------------------------------------
// Fiducial methods
// ------------------------------------------

// any 2 fiducials within this 3D distance in mm will be considered equal
const float EQUALITY_DISTANCE_MM = 2.0;

bool AtracsysTracker::Fiducial::operator==(const Fiducial& f)
{
  // pow is much slower than just x*x for squaring numbers
  float dist2 = (this->xMm - f.xMm) * (this->xMm - f.xMm) + (this->yMm - f.yMm) * (this->yMm - f.yMm) + (this->zMm - f.zMm) * (this->zMm - f.zMm);
  return sqrt(dist2) < EQUALITY_DISTANCE_MM;
}

// compare fiducials on distance from the origin
bool AtracsysTracker::Fiducial::operator<(const Fiducial& f) const
{
  float distF1 = sqrt(this->xMm * this->xMm + this->yMm * this->yMm + this->zMm * this->zMm);
  float distF2 = sqrt(f.xMm * f.xMm + f.yMm * f.yMm + f.zMm * f.zMm);
  return distF1 < distF2;
}

// ------------------------------------------
// Marker methods
// ------------------------------------------

AtracsysTracker::Marker::Marker()
{
  this->MarkerStatus = 0;
  this->TrackingId = -1;
  this->GeometryId = -1;
  this->GeometryPresenceMask = -1;
  this->RegistrationErrorMm = 0.0;
}
AtracsysTracker::Marker::Marker(int status, int trackingId, int geometryId,
  vtkMatrix4x4* toolToTracker, int geometryPresenceMask, float registrationErrorMM)
{
  this->MarkerStatus = status;
  this->TrackingId = trackingId;
  this->GeometryId = geometryId;
  this->ToolToTracker->DeepCopy(toolToTracker);
  this->GeometryPresenceMask = geometryPresenceMask;
  this->RegistrationErrorMm = registrationErrorMM;
}

AtracsysTracker::Marker::Marker(const AtracsysTracker::Marker& obj)
{
  this->MarkerStatus = obj.MarkerStatus;
  this->TrackingId = obj.TrackingId;
  this->GeometryId = obj.GeometryId;
  this->ToolToTracker->DeepCopy(obj.ToolToTracker.GetPointer());
  this->GeometryPresenceMask = obj.GeometryPresenceMask;
  this->RegistrationErrorMm = obj.RegistrationErrorMm;
  this->fiducials = obj.fiducials;
}

bool AtracsysTracker::Marker::AddFiducial(AtracsysTracker::Fiducial fid)
{
  if (fiducials.size() < FTK_MAX_FIDUCIALS)
  {
    fiducials.push_back(fid);
    return true;
  }
  else
  {
    return false;
  }
}
