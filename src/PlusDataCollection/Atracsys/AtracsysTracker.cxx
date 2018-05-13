/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "AtracsysTracker.h"

// System includes
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <algorithm>

// Atracsys includes
#include "ftkErrors.h"
#include "ftkEvent.h"
#include "ftkInterface.h"
#include "ftkOptions.h"
#include "ftkPlatform.h"

// vtk includes
#include <vtkMatrix4x4.h>
#include <vtkSmartPointer.h>

class AtracsysTracker::AtracsysInternal
{
public:
  AtracsysInternal()
  {
    // populate result to string
    ResultToString[ERROR_UNABLE_TO_GET_FTK_HANDLE] = "Unable to get Atracsys library handle.";
    ResultToString[ERROR_NO_DEVICE_CONNECTED] = "No Atracsys device connected.";
    ResultToString[WARNING_CONNECTED_IN_USB2] = "Atracsys connected in USB2. Please connect Atracsys device in USB3 port.";
    ResultToString[ERROR_UNABLE_TO_LOAD_MARKER] = "Unable to load marker.";
    ResultToString[ERROR_FAILURE_TO_LOAD_INI] = "Failed to load marker's ini file.";
    ResultToString[ERROR_OPTION_AVAILABLE_ONLY_ON_FTK] = "Attempted to call fusionTrack only option with non-fusionTrack device connected.";
    ResultToString[ERROR_OPTION_AVAILABLE_ONLY_ON_STK] = "Attempted to call spryTrack only option with non-spryTrack device connected.";
    ResultToString[ERROR_FAILED_TO_CLOSE_SDK] = "Failed to close the Atracsys SDK.";
    ResultToString[ERROR_CANNOT_CREATE_FRAME_INSTANCE] = "Failed to create frame.";
    ResultToString[ERROR_CANNOT_INITIALIZE_FRAME] = "Failed to initialize frame.";
    ResultToString[ERROR_NO_FRAME_AVAILABLE] = "No frame available from tracker.";
    ResultToString[ERROR_INVALID_FRAME] = "Invalid frame received from tracker.";
    ResultToString[ERROR_TOO_MANY_MARKERS] = "Too many markers in frame.";
    ResultToString[ERROR_ENABLE_IR_STROBE] = "Failed to enable / disable the IR strobe.";
    ResultToString[ERROR_SET_USER_LED] = "Failed to set the user LED.";
    ResultToString[ERROR_ENABLE_USED_LED] = "Failed to enable / disable the user LED.";
    ResultToString[ERROR_SET_MAX_MISSING_FIDUCIALS] = "Failed to set maximum missing fiducials.";
    ResultToString[ERROR_ENABLE_ONBOARD_PROCESSING] = "Failed to enable / disable onboard image processing.";
    ResultToString[ERROR_ENABLE_IMAGE_STREAMING] = "Failed to enable / disable image streaming.";
    ResultToString[ERROR_ENABLE_WIRELESS_MARKER_PAIRING] = "Failed to enable / disable wireless marker pairing.";
    ResultToString[ERROR_ENABLE_WIRELESS_MARKER_STATUS_STREAMING] = "Failed to enable / disable wireless marker status streaming.";
    ResultToString[ERROR_ENABLE_WIRELESS_MARKER_BATTERY_STREAMING] = "Failed to enable / disable wireless marker battery streaming.";
  }

  virtual ~AtracsysInternal() {}

  ftkLibrary FtkLib = 0;
  uint64 TrackerSN = 0;
  std::map<AtracsysTracker::ATRACSYS_RESULT, std::string> ResultToString;
public:
  // Code from ATRACSYS
  class IniFile
  {
  protected:

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
      else
        if (equal != std::string::npos && _currentSection != "")
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

          strLine.erase(remove(strLine.begin(),
            strLine.end(), '\r'), strLine.end());
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
    } // Return false in case of syntax error
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
    bool toBuffer(char** out, size_t& outSize)
    {
      if (!out)
      {
        return false;
      }

      std::string buffer;
      char temp[1024];

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

  bool checkSection(IniFile& p, const std::string& section)
  {
    if (p.sections.find(section) == p.sections.end())
    {
      //std::cout << "Cannot find section \"" << section << "\"";
      return false;
    }
    return true;
  }

  //----------------------------------------------------------------------------
  bool checkKey(IniFile& p, const std::string& section, const std::string& key)
  {
    if (p.sections[section].find(key) == p.sections[section].end())
    {
      //std::cout << "Cannot find key \"" << key << "\" in section \"" << section << "\"";
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

  bool LoadIniFile(std::ifstream& is, ftkGeometry& geometry)
  {
    std::string line, fileContent("");

    while (!is.eof())
    {
      getline(is, line);
      fileContent += line + "\n";
    }

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

// END CODE FROM ATRACSYS

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::LoadFtkGeometry(const std::string& filename, ftkGeometry& geom)
{
  std::ifstream input;
  input.open(filename.c_str());

  if (!input.fail() && this->Internal->LoadIniFile(input, geom))
  {
    return ERROR_FAILURE_TO_LOAD_INI;
  }
  else
  {
    ftkBuffer buffer;
    buffer.reset();
    if (ftkGetData(this->Internal->FtkLib, this->Internal->TrackerSN, FTK_OPT_DATA_DIR, &buffer) != FTK_OK || buffer.size < 1u)
    {
      return ERROR_FAILURE_TO_LOAD_INI;
    }

    std::string fullFile(reinterpret_cast< char* >(buffer.data));
    fullFile += "\\" + filename;

    input.open(fullFile.c_str());

    if (!input.fail() && this->Internal->LoadIniFile(input, geom))
    {
      return SUCCESS;
    }
  }

  return ERROR_FAILURE_TO_LOAD_INI;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::SetSpryTrackOnlyOption(int option, int value, AtracsysTracker::ATRACSYS_RESULT errorResult)
{
  // this option is only available on spryTrack
  if (this->DeviceType == SPRYTRACK_180)
  {
    if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, option, value) != FTK_OK)
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
    if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, option, value) != FTK_OK)
    {
      return errorResult;
    }
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
  // initialize SDK
  this->Internal->FtkLib = ftkInit();

  if (this->Internal->FtkLib == NULL)
  {
    return ERROR_UNABLE_TO_GET_FTK_HANDLE;
  }

  DeviceData device;
  device.SerialNumber = 0uLL;

  // scan for devices
  ftkError err;
  err = ftkEnumerateDevices(this->Internal->FtkLib, FusionTrackEnumerator, &device);
  if (err != FTK_OK && err != FTK_WAR_USB_TOO_SLOW)
  {
    ftkClose(&this->Internal->FtkLib);
    return ERROR_NO_DEVICE_CONNECTED;
  }
  
  if (device.SerialNumber == 0uLL)
  {
    ftkClose(&this->Internal->FtkLib);
    return ERROR_NO_DEVICE_CONNECTED;
  }

  this->Internal->TrackerSN = device.SerialNumber;

  switch (device.Type)
  {
  case DEV_SPRYTRACK_180:
    this->DeviceType = SPRYTRACK_180;
    break;
  case DEV_FUSIONTRACK_500:
    this->DeviceType = FUSIONTRACK_500;
    break;
  case DEV_FUSIONTRACK_250:
    this->DeviceType = FUSIONTRACK_250;
    break;
  default:
    this->DeviceType = UNKNOWN_DEVICE;
  }

  if (err = FTK_WAR_USB_TOO_SLOW)
  {
    return WARNING_CONNECTED_IN_USB2;
  }
  else
  {
    return SUCCESS;
  }
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::Disconnect()
{
  ftkError err(FTK_OK);
  err = ftkClose(&this->Internal->FtkLib);
  if (err != FTK_OK)
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
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::LoadMarkerGeometry(std::string filePath, int& geometryId)
{
  ftkGeometry geom;
  this->LoadFtkGeometry(filePath, geom);
  if (ftkSetGeometry(this->Internal->FtkLib, this->Internal->TrackerSN, &geom) != FTK_OK)
  {
    return ERROR_UNABLE_TO_LOAD_MARKER;
  }
  geometryId = geom.geometryId;
  return SUCCESS;
}

//----------------------------------------------------------------------------
std::string AtracsysTracker::GetMarkerInfo()
{
  ftkBuffer buffer;
  if (ftkGetData(this->Internal->FtkLib, this->Internal->TrackerSN, OPTION_DEV_MARKERS_INFO, &buffer) != FTK_OK)
  {
    return "Cannot get additional infos on the markers.";
  }
  std::string info(buffer.data, buffer.data + buffer.size);
  return info;
}

//----------------------------------------------------------------------------
std::string AtracsysTracker::ResultToString(AtracsysTracker::ATRACSYS_RESULT result)
{
  std::map<AtracsysTracker::ATRACSYS_RESULT, std::string>::iterator it;
  for (it = begin(this->Internal->ResultToString); it != end(this->Internal->ResultToString); it++)
  {
    if (result == it->first) return it->second;
  }
  return "String value for error code not defined.";
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::GetMarkersInFrame(std::vector<Marker>& markers)
{
  ftkFrameQuery* frame = ftkCreateFrame();

  if (frame == 0)
  {
    ftkDeleteFrame(frame);
    return ERROR_CANNOT_CREATE_FRAME_INSTANCE;
  }

  if (ftkSetFrameOptions(false, false, 128u, 128u, 4u * FTK_MAX_FIDUCIALS, 4u, frame) != FTK_OK)
  {
    ftkDeleteFrame(frame);
    return ERROR_CANNOT_INITIALIZE_FRAME;
  }

  if (ftkGetLastFrame(this->Internal->FtkLib, this->Internal->TrackerSN, frame, 0) != FTK_OK)
  {
    // block until next frame is available from camera
    ftkDeleteFrame(frame);
    return ERROR_NO_FRAME_AVAILABLE;
  }

  switch (frame->markersStat)
  {
  case QS_WAR_SKIPPED:
    ftkDeleteFrame(frame);
    return ERROR_INVALID_FRAME;

  case QS_ERR_INVALID_RESERVED_SIZE:
    ftkDeleteFrame(frame);
    return ERROR_INVALID_FRAME;

  default:
    ftkDeleteFrame(frame);
    return ERROR_INVALID_FRAME;

  case QS_OK:
    break;
  }

  if (frame->markersStat == QS_ERR_OVERFLOW)
  {
    ftkDeleteFrame(frame);
    return ERROR_TOO_MANY_MARKERS;
  }

  ftkMarker marker;

  for (size_t m = 0; m < frame->markersCount; m++)
  {
    marker = frame->markers[m];
    int geometryId = marker.geometryId;
    vtkSmartPointer<vtkMatrix4x4> toolToTracker = vtkSmartPointer<vtkMatrix4x4>::New();
    toolToTracker->Identity();

    for (int i = 0; i < 3; i++)
    {
      toolToTracker->SetElement(i, 3, marker.translationMM[i]);
      for (int j = 0; j < 3; j++)
      {
        toolToTracker->SetElement(i, j, marker.rotation[i][j]);
      }
    }

    int gpm = marker.geometryPresenceMask;
    float freMm = marker.registrationErrorMM;

    Marker atracsysMarker(geometryId, toolToTracker, gpm, freMm);
    markers.push_back(atracsysMarker);
  }

  // close frame
  ftkDeleteFrame(frame);

  return SUCCESS;
}

//----------------------------------------------------------------------------
std::string AtracsysTracker::GetLastErrorString()
{
  char message[1024u];

  ftkError err(ftkGetLastErrorString(this->Internal->FtkLib, 1024u, message));
  if (err == FTK_OK)
  {
    return std::string(message);
  }
  else
  {
    return std::string("ftkLib is uninitialized.");
  }
  return std::string(message);
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::EnableIRStrobe(bool enabled)
{
  if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, OPTION_IR_STROBE, enabled) != FTK_OK)
  {
    return ERROR_ENABLE_IR_STROBE;
  }
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::SetUserLEDState(int red, int green, int blue, int frequency)
{
  if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, OPTION_LED_FREQUENCY, frequency) != FTK_OK)
  {
    return ERROR_SET_USER_LED;
  }
  if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, OPTION_LED_RED_COMPONENT, red) != FTK_OK)
  {
    return ERROR_SET_USER_LED;
  }
  if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, OPTION_LED_GREEN_COMPONENT, green) != FTK_OK)
  {
    return ERROR_SET_USER_LED;
  }
  if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, OPTION_LED_BLUE_COMPONENT, blue) != FTK_OK)
  {
    return ERROR_SET_USER_LED;
  }
  // make sure LED is enabled
  return this->EnableUserLED(true);
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::EnableUserLED(bool enabled)
{
  if (ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, OPTION_LED_ENABLE, enabled) != FTK_OK)
  {
    return ERROR_ENABLE_USED_LED;
  }
  return SUCCESS;
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::SetMaxMissingFiducials(int maxMissingFids)
{
  ftkSetInt32(this->Internal->FtkLib, this->Internal->TrackerSN, OPTION_MAX_MISSING_POINTS, maxMissingFids);
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
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::EnableWirelessMarkerPairing(bool enabled)
{
  return this->SetSpryTrackOnlyOption(OPTION_WIRELESS_MARKER_PAIRING, enabled, ERROR_ENABLE_WIRELESS_MARKER_PAIRING);
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::EnableWirelessMarkerStatusStreaming(bool enabled)
{
  return this->SetSpryTrackOnlyOption(OPTION_WIRELESS_MARKER_PAIRING, enabled, ERROR_ENABLE_WIRELESS_MARKER_STATUS_STREAMING);
}

//----------------------------------------------------------------------------
AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::EnableWirelessMarkerBatteryStreaming(bool enabled)
{
  return this->SetSpryTrackOnlyOption(OPTION_WIRELESS_MARKER_BATTERY_STREAMING, enabled, ERROR_ENABLE_WIRELESS_MARKER_BATTERY_STREAMING);
}

// ------------------------------------------
// fusionTrack only options
// ------------------------------------------

AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::GetDroppedFrameCount(int& droppedFrameCount)
{
  return SUCCESS;
}

AtracsysTracker::ATRACSYS_RESULT AtracsysTracker::ResetLostFrameCount()
{
  return SUCCESS;
}

// ------------------------------------------
// Marker methods
// ------------------------------------------

AtracsysTracker::Marker::Marker(int geometryId, vtkSmartPointer<vtkMatrix4x4> toolToTracker, int gpm, float freMm)
{
  this->GeometryId = geometryId;
  this->ToolToTracker = toolToTracker;
  this->GeometryPresenceMask = gpm;
  this->FreMm = freMm;
}

int AtracsysTracker::Marker::GetGeometryID()
{
  return this->GeometryId;
}

int AtracsysTracker::Marker::GetGeometryPrecsenceMask()
{
  return this->GeometryPresenceMask;
}

vtkSmartPointer<vtkMatrix4x4> AtracsysTracker::Marker::GetTransformToTracker()
{
  return this->ToolToTracker;
}

float AtracsysTracker::Marker::GetFiducialRegistrationErrorMm()
{
  return this->FreMm;
}