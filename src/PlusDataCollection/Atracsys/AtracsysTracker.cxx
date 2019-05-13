/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "AtracsysTracker.h"

// System includes
#include <algorithm>
#include <fstream>
#include <map>
#include <string>

// Atracsys includes
#include "ftkErrors.h"
#include "ftkEvent.h"
#include "ftkInterface.h"
#include "ftkOptions.h"
#include "ftkPlatform.h"
#include "ftkTypes.h"

// vtk includes
#include <vtkMatrix4x4.h>
#include <vtkNew.h>

#define ATRACSYS_BUFFER_SIZE 1024
#define RESET_DROPPED_FRAME_COUNT 1

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
    ResultToStringMap[ERROR_CANNOT_CREATE_FRAME_INSTANCE] = "Failed to create frame.";
    ResultToStringMap[ERROR_CANNOT_INITIALIZE_FRAME] = "Failed to initialize frame.";
    ResultToStringMap[ERROR_NO_FRAME_AVAILABLE] = "No frame available from tracker.";
    ResultToStringMap[ERROR_INVALID_FRAME] = "Invalid frame received from tracker.";
    ResultToStringMap[ERROR_TOO_MANY_MARKERS] = "Too many markers in frame.";
    ResultToStringMap[ERROR_ENABLE_IR_STROBE] = "Failed to enable / disable the IR strobe.";
    ResultToStringMap[ERROR_ENABLE_LASER] = "Failed to enable laser, this is a spryTrack only option.";
    ResultToStringMap[ERROR_SET_USER_LED] = "Failed to set the user LED.";
    ResultToStringMap[ERROR_ENABLE_USER_LED] = "Failed to enable / disable the user LED.";
    ResultToStringMap[ERROR_SET_MAX_MISSING_FIDUCIALS] = "Failed to set maximum missing fiducials.";
    ResultToStringMap[ERROR_ENABLE_ONBOARD_PROCESSING] = "Failed to enable / disable onboard image processing.";
    ResultToStringMap[ERROR_ENABLE_IMAGE_STREAMING] = "Failed to enable / disable image streaming.";
    ResultToStringMap[ERROR_ENABLE_WIRELESS_MARKER_PAIRING] = "Failed to enable / disable wireless marker pairing.";
    ResultToStringMap[ERROR_ENABLE_WIRELESS_MARKER_STATUS_STREAMING] = "Failed to enable / disable wireless marker status streaming.";
    ResultToStringMap[ERROR_ENABLE_WIRELESS_MARKER_BATTERY_STREAMING] = "Failed to enable / disable wireless marker battery streaming.";
    ResultToStringMap[ERROR_DISCONNECT_ATTEMPT_WHEN_NOT_CONNECTED] = "Disconnect called when not connected to tracker.";
    ResultToStringMap[ERROR_CANNOT_GET_MARKER_INFO] = "Cannot get info about paired wireless markers.";
    ResultToStringMap[ERROR_FAILED_TO_SET_STK_PROCESSING_TYPE] = "Failed to set spryTrack image processing type.";
    ResultToStringMap[ERROR_FAILED_TO_SET_MAX_MISSING_FIDS] = "Failed to set maximum number of missing fiducials.";
  }

  virtual ~AtracsysInternal()
  {
    FtkLib = nullptr;
    TrackerSN = 0;
  }

  // handle to FtkLib library
  ftkLibrary FtkLib = nullptr;

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

  bool ParseIniFile(std::string fileContent, ftkGeometry& geometry)
  {
    stringFindAndReplaceAll(fileContent, "\\n", "\n");

    IniFile parser;

    if (!parser.parse(const_cast< char* >(fileContent.c_str()),
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
    if (ftkGetData(this->FtkLib, this->TrackerSN, FTK_OPT_DATA_DIR, &buffer) != ftkError::FTK_OK || buffer.size < 1u)
    {
      return ERROR_FAILURE_TO_LOAD_INI;
    }

    std::string fullFile(reinterpret_cast< char* >(buffer.data));
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

AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::AtracsysInternal::LoadFtkGeometryFromString(const std::string& geomString, ftkGeometry& geom)
{
  if (this->ParseIniFile(geomString, geom))
  {
    return SUCCESS;
  }
  return ERROR_FAILURE_TO_LOAD_INI;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::SetSpryTrackOnlyOption(int option, int value, AtracsysTracker::ATRACSYS_RESULT errorResult)
{
  // this option is only available on spryTrack
  if (this->DeviceType == SPRYTRACK_180)
  {
    if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, option, value) != ftkError::FTK_OK)
    {
      return errorResult;
    }
    return SUCCESS;
  }
  return ERROR_OPTION_AVAILABLE_ONLY_ON_STK;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::SetFusionTrackOnlyOption(int option, int value, AtracsysTracker::ATRACSYS_RESULT errorResult)
{
  // this option is only available on spryTrack
  if (this->DeviceType == FUSIONTRACK_250 || this->DeviceType == FUSIONTRACK_500)
  {
    if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, option, value) != ftkError::FTK_OK)
    {
      return errorResult;
    }
    return SUCCESS;
  }
  return ERROR_OPTION_AVAILABLE_ONLY_ON_FTK;
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

  switch (device.Type)
  {
  case ftkDeviceType::DEV_SPRYTRACK_180:
    this->DeviceType = SPRYTRACK_180;
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

  if (ftkSetFrameOptions(false, false, 128u, 128u, 4u * FTK_MAX_FIDUCIALS, 4u, this->Internal->Frame) != ftkError::FTK_OK)
  {
    ftkDeleteFrame(this->Internal->Frame);
    this->Internal->Frame = nullptr;
    return ERROR_CANNOT_INITIALIZE_FRAME;
  }

  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::Disconnect()
{
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
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::GetDeviceType(DEVICE_TYPE& deviceType)
{
  deviceType = this->DeviceType;
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::LoadMarkerGeometryFromFile(std::string filePath, int& geometryId)
{
  ftkGeometry geom;
  this->Internal->LoadFtkGeometryFromFile(filePath, geom);
  if (ftkSetGeometry(this->Internal->FtkLib, this->Internal->TrackerSN, &geom) != ftkError::FTK_OK)
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
  if (ftkSetGeometry(this->Internal->FtkLib, this->Internal->TrackerSN, &geom) != ftkError::FTK_OK)
  {
    return ERROR_UNABLE_TO_LOAD_MARKER;
  }
    geometryId = geom.geometryId;
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::GetMarkerInfo(std::string& markerInfo)
{
  ftkBuffer buffer;
  if (ftkGetData(this->Internal->FtkLib, this->Internal->TrackerSN, OPTION_DEV_MARKERS_INFO, &buffer) != ftkError::FTK_OK)
  {
    return ERROR_CANNOT_GET_MARKER_INFO;
  }
  markerInfo = std::string(buffer.data, buffer.data + buffer.size);
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
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::GetFiducialsInFrame(std::vector<Fiducial3D>& fiducials)
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

  // make sure fiducials vector is empty before populating
  fiducials.clear();

  for (size_t m = 0; m < this->Internal->Frame->threeDFiducialsCount; m++)
  {
    ftk3DFiducial& ftkFiducial = this->Internal->Frame->threeDFiducials[m];
    Fiducial3D fiducial(ftkFiducial.positionMM.x, ftkFiducial.positionMM.y, ftkFiducial.positionMM.z, ftkFiducial.probability);
    fiducials.push_back(fiducial);
  }
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::GetMarkersInFrame(std::vector<Marker>& markers)
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
    ftkMarker& marker = this->Internal->Frame->markers[m];
    vtkNew<vtkMatrix4x4> toolToTracker;

    for (int row = 0; row < 3; row++)
    {
      toolToTracker->SetElement(row, 3, marker.translationMM[row]);
      for (int column = 0; column < 3; column++)
      {
        toolToTracker->SetElement(row, column, marker.rotation[row][column]);
      }
    }

    Marker atracsysMarker((int)marker.geometryId, toolToTracker.GetPointer(), marker.geometryPresenceMask, marker.registrationErrorMM);
    markers.push_back(atracsysMarker);
  }
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::EnableIRStrobe(bool enabled)
{
  if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, OPTION_IR_STROBE, enabled) != ftkError::FTK_OK)
  {
    return ERROR_ENABLE_IR_STROBE;
  }
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::SetUserLEDState(int red, int green, int blue, int frequency, bool enabled /* = true */)
{
  if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, OPTION_LED_FREQUENCY, frequency) != ftkError::FTK_OK)
  {
    return ERROR_SET_USER_LED;
  }
  if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, OPTION_LED_RED_COMPONENT, red) != ftkError::FTK_OK)
  {
    return ERROR_SET_USER_LED;
  }
  if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, OPTION_LED_GREEN_COMPONENT, green) != ftkError::FTK_OK)
  {
    return ERROR_SET_USER_LED;
  }
  if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, OPTION_LED_BLUE_COMPONENT, blue) != ftkError::FTK_OK)
  {
    return ERROR_SET_USER_LED;
  }
  // enable user LED
  return this->EnableUserLED(enabled);
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::EnableUserLED(bool enabled)
{
  if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, OPTION_LED_ENABLE, enabled) != ftkError::FTK_OK)
  {
    return ERROR_ENABLE_USER_LED;
  }
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::SetMaxMissingFiducials(int maxMissingFids)
{
  if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, OPTION_MAX_MISSING_POINTS, maxMissingFids) != ftkError::FTK_OK)
  {
    return ERROR_FAILED_TO_SET_MAX_MISSING_FIDS;
  }
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::EnableWirelessMarkerPairing(bool enabled)
{
  if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, OPTION_WIRELESS_MARKER_PAIRING, enabled) != ftkError::FTK_OK)
  {
    return ERROR_ENABLE_WIRELESS_MARKER_PAIRING;
  }
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::EnableWirelessMarkerStatusStreaming(bool enabled)
{
  if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, OPTION_WIRELESS_MARKER_PAIRING, enabled) != ftkError::FTK_OK)
  {
    return ERROR_ENABLE_WIRELESS_MARKER_STATUS_STREAMING;
  }
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::EnableWirelessMarkerBatteryStreaming(bool enabled)
{
  if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, OPTION_WIRELESS_MARKER_BATTERY_STREAMING, enabled) != ftkError::FTK_OK)
  {
    return ERROR_ENABLE_WIRELESS_MARKER_BATTERY_STREAMING;
  }
  return SUCCESS;
}

// ------------------------------------------
// spryTrack only options
// ------------------------------------------

AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::EnableOnboardProcessing(bool enabled)
{
  return this->SetSpryTrackOnlyOption(OPTION_ONBOARD_PROCESSING, enabled, ERROR_ENABLE_ONBOARD_PROCESSING);
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::EnableImageStreaming(bool enabled)
{
  return this->SetSpryTrackOnlyOption(OPTION_IMAGE_STREAMING, enabled, ERROR_ENABLE_IMAGE_STREAMING);
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::SetLaserEnabled(bool enabled)
{
  return this->SetSpryTrackOnlyOption(OPTION_LASER_ENABLE, enabled, ERROR_ENABLE_LASER);
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::SetSpryTrackProcessingType(AtracsysTracker::SPRYTRACK_IMAGE_PROCESSING_TYPE processingType)
{
  if (this->DeviceType != SPRYTRACK_180)
  {
    return ERROR_OPTION_AVAILABLE_ONLY_ON_STK;
  }
  bool succeeded = true;
  if (processingType == PROCESSING_ONBOARD)
  {
    succeeded = succeeded && (this->EnableOnboardProcessing(true) == SUCCESS);
    succeeded = succeeded && (this->EnableImageStreaming(false) == SUCCESS);
  }
  else if (processingType == PROCESSING_ON_PC)
  {
    succeeded = succeeded && (this->EnableOnboardProcessing(false) == SUCCESS);
    succeeded = succeeded && (this->EnableImageStreaming(true) == SUCCESS);
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
  if (this->DeviceType == FUSIONTRACK_250 || this->DeviceType == FUSIONTRACK_500)
  {
    int32 lost = 0, corrupted = 0;
    ftkGetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, OPTION_LOST_FRAME_COUNT, &lost, ftkOptionGetter::FTK_VALUE);
    ftkGetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, OPTION_CORRUPTED_FRAME_COUNT, &corrupted, ftkOptionGetter::FTK_VALUE);
    droppedFrameCount = lost + corrupted;
    return SUCCESS;
  }
  return ERROR_OPTION_AVAILABLE_ONLY_ON_FTK;
}

AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::ResetLostFrameCount()
{
  if (this->DeviceType == FUSIONTRACK_250 || this->DeviceType == FUSIONTRACK_500)
  {
    ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, OPTION_RESET_LOST_FRAME_COUNT, RESET_DROPPED_FRAME_COUNT);
    return SUCCESS;
  }
  return ERROR_OPTION_AVAILABLE_ONLY_ON_FTK;
}

// ------------------------------------------
// Fiducial3D methods
// ------------------------------------------
AtracsysTracker::Fiducial3D::Fiducial3D()
{
  this->xMm = 0.0;
  this->yMm = 0.0;
  this->zMm = 0.0;
  this->probability = -1;
}

AtracsysTracker::Fiducial3D::Fiducial3D(float x, float y, float z, float probability)
{
  this->xMm = x;
  this->yMm = y;
  this->zMm = z;
  this->probability = probability;
}

// any 2 fiducials within this 3D distance will be considered equal
const float EQUALITY_DISTANCE_MM = 2.0;

bool AtracsysTracker::Fiducial3D::operator==(const Fiducial3D& f)
{
  // pow is much slower than just x*x for squaring numbers
  float dist2 = (this->xMm - f.xMm) * (this->xMm - f.xMm) + (this->yMm - f.yMm) * (this->yMm - f.yMm) + (this->zMm - f.zMm) * (this->zMm - f.zMm);
  return sqrt(dist2) < EQUALITY_DISTANCE_MM;
}

// compare fiducials on distance from the origin
bool AtracsysTracker::Fiducial3D::operator<(const Fiducial3D& f) const
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
  this->GeometryId = -1;
  this->GeometryPresenceMask = -1;
  this->RegistrationErrorMM = 0.0;
}
AtracsysTracker::Marker::Marker(int geometryId, vtkMatrix4x4* toolToTracker, int geometryPresenceMask, float registrationErrorMM)
{
  this->GeometryId = geometryId;
  this->ToolToTracker->DeepCopy(toolToTracker);
  this->GeometryPresenceMask = geometryPresenceMask;
  this->RegistrationErrorMM = registrationErrorMM;
}

AtracsysTracker::Marker::Marker(const AtracsysTracker::Marker& obj)
{
  this->GeometryId = obj.GeometryId;
  this->ToolToTracker->DeepCopy(obj.ToolToTracker.GetPointer());
  this->GeometryPresenceMask = obj.GeometryPresenceMask;
  this->RegistrationErrorMM = obj.RegistrationErrorMM;
}

int AtracsysTracker::Marker::GetGeometryID()
{
  return this->GeometryId;
}

int AtracsysTracker::Marker::GetGeometryPresenceMask()
{
  return this->GeometryPresenceMask;
}

vtkMatrix4x4* AtracsysTracker::Marker::GetTransformToTracker()
{
  return this->ToolToTracker.GetPointer();
}

float AtracsysTracker::Marker::GetFiducialRegistrationErrorMm()
{
  return this->RegistrationErrorMM;
}