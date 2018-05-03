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

using namespace Atracsys;


namespace
{
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
      std::cout <<"Cannot find section \"" << section << "\"";
      return false;
    }
    return true;
  }

  // ----------------------------------------------------------------------------

  bool checkKey(IniFile& p, const std::string& section, const std::string& key)
  {
    if (p.sections[section].find(key) == p.sections[section].end())
    {
      std::cout << "Cannot find key \"" << key << "\" in section \"" << section << "\"";
      return false;
    }

    return true;
  }

  // ----------------------------------------------------------------------------

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

  // ----------------------------------------------------------------------------

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
}


//----------------------------------------------------------------------------
std::string Tracker::GetFtkLastErrorString()
{
  char message[1024u];
  
  ftkError err(ftkGetLastErrorString(this->FtkLib, 1024u, message));
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

struct DeviceData
{
  uint64 SerialNumber;
  ftkDeviceType Type;
};


//----------------------------------------------------------------------------
ATRACSYS_ERROR Tracker::LoadFtkGeometry(const std::string& filename, ftkGeometry& geom)
{
  std::ifstream input;
  input.open(filename.c_str());

  if (!input.fail() && this->LoadIniFile(input, geom))
  {
    return ERROR_FAILURE_TO_LOAD;
  }
  else
  {
    ftkBuffer buffer;
    buffer.reset();
    if (ftkGetData(this->FtkLib, this->TrackerSN, FTK_OPT_DATA_DIR, &buffer) != FTK_OK || buffer.size < 1u)
    {
      return ERROR_FAILURE_TO_LOAD;
    }

    std::string fullFile(reinterpret_cast< char* >(buffer.data));
    fullFile += "\\" + filename;

    input.open(fullFile.c_str());

    if (!input.fail() && this->LoadIniFile(input, geom))
    {
      return SUCCESS;
    }
  }

  return ERROR_FAILURE_TO_LOAD;
}

//----------------------------------------------------------------------------
bool Tracker::LoadIniFile(std::ifstream& is, ftkGeometry& geometry)
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

  std::cout << "Loading geometry " << geometry.geometryId << ", composed of "
    << geometry.pointsCount << " fiducials";

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

    std::cout << "Loaded fiducial " << i << " ("
      << geometry.positions[i].x << ", "
      << geometry.positions[i].y << ", "
      << geometry.positions[i].z << ")";
  }

  return true;
}

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

Tracker::Tracker()
{

}

Tracker::~Tracker()
{

}

ATRACSYS_ERROR Tracker::Connect()
{
  // initialize SDK
  this->FtkLib = ftkInit();

  if (this->FtkLib == NULL)
  {
    return ERROR_UNABLE_TO_GET_FTK_HANDLE;
  }

  DeviceData device;
  device.SerialNumber = 0uLL;

  // scan for devices
  ftkError err(FTK_OK);
  std::cout << ftkEnumerateDevices(this->FtkLib, FusionTrackEnumerator, &device);
  std::cout << this->GetFtkLastErrorString();
  
  if (device.SerialNumber == 0uLL)
  {
    std::cout << "No Atracsys device connected.";
    ftkClose(&FtkLib);
    return ERROR_NO_DEVICE_CONNECTED;
  }

  this->TrackerSN = device.SerialNumber;

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
    this->DeviceType = UNKNOWN;
  }


  // testing
  std::cout << "connected to tracker" << std::endl;
  return SUCCESS;
}

ATRACSYS_ERROR Tracker::Disconnect()
{
  ftkError err(FTK_OK);
  err = ftkClose(&this->FtkLib);
  if (err != FTK_OK)
  {
    return ERROR_FAILED_TO_CLOSE_SDK;
  }
  return SUCCESS;
}

//----------------------------------------------------------------------------
// Some spryTrack 180 only options

ATRACSYS_ERROR Tracker::SetSTKOnlyOption(OPTIONS option, int value)
{
  // this option is only available on spryTrack
  if (this->DeviceType == SPRYTRACK_180)
  {
    if (ftkSetInt32(this->FtkLib, this->TrackerSN, option, value) != FTK_OK)
    {
      return ERROR_FAILED_TO_SET_OPTION;
    }
  }
  return ERROR_OPTION_NOT_AVAILABLE_ON_FTK;
}

ATRACSYS_ERROR Tracker::EnableOnboardProcessing()
{
  return this->SetSTKOnlyOption(OPTION_ONBOARD_PROCESSING, 1);
}

ATRACSYS_ERROR Tracker::DisableOnboardProcessing()
{
  return this->SetSTKOnlyOption(OPTION_ONBOARD_PROCESSING, 0);
}

ATRACSYS_ERROR Tracker::EnableImageStreaming()
{
  return this->SetSTKOnlyOption(OPTION_IMAGE_STREAMING, 1);
}

ATRACSYS_ERROR Tracker::DisableImageStreaming()
{
  return this->SetSTKOnlyOption(OPTION_IMAGE_STREAMING, 0);
}

ATRACSYS_ERROR Tracker::EnableWirelessMarkerPairing()
{
  return this->SetSTKOnlyOption(OPTION_WIRELESS_MARKER_PAIRING, 1);
}

ATRACSYS_ERROR Tracker::DisableWirelessMarkerPairing()
{
  return this->SetSTKOnlyOption(OPTION_WIRELESS_MARKER_PAIRING, 0);
}

//----------------------------------------------------------------------------
ATRACSYS_ERROR Tracker::EnableIRStrobe()
{
  return SUCCESS;
}

ATRACSYS_ERROR Tracker::DisableIRStrobe()
{
  return SUCCESS;
}

ATRACSYS_ERROR Tracker::GetDroppedFrameCount()
{
  return SUCCESS;
}

ATRACSYS_ERROR Tracker::ResetLostFrameCount()
{
  return SUCCESS;
}

ATRACSYS_ERROR Tracker::SetUserLEDState(int red, int green, int blue, int frequency)
{
  ftkSetInt32(this->FtkLib, this->TrackerSN, OPTION_LED_FREQUENCY, frequency);
  ftkSetInt32(this->FtkLib, this->TrackerSN, OPTION_LED_RED_COMPONENT, red);
  ftkSetInt32(this->FtkLib, this->TrackerSN, OPTION_LED_GREEN_COMPONENT, green);
  ftkSetInt32(this->FtkLib, this->TrackerSN, OPTION_LED_BLUE_COMPONENT, blue);
  ftkSetInt32(this->FtkLib, this->TrackerSN, OPTION_LED_ENABLE, 1);
  return SUCCESS;
}

ATRACSYS_ERROR Tracker::GetUserLEDState(int& red, int& green, int& blue, int& frequency)
{
  return SUCCESS;
}

ATRACSYS_ERROR Tracker::EnableUserLED()
{
  return SUCCESS;
}

ATRACSYS_ERROR Tracker::DisableUserLED()
{
  return SUCCESS;
}

ATRACSYS_ERROR Tracker::LoadMarkerGeometry(std::string filePath, int& geometryId)
{
  ftkGeometry geom;
  this->LoadFtkGeometry(filePath, geom);
  if (ftkSetGeometry(this->FtkLib, this->TrackerSN, &geom) != FTK_OK)
  {
    return ERROR_FAILED_TO_LOAD_GEOMETRY;
  }
  geometryId = geom.geometryId;
  return SUCCESS;
}

ATRACSYS_ERROR Tracker::GetMarkersInFrame(std::vector<Marker>& markers)
{
  return SUCCESS;
}