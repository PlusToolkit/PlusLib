/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusAtracsysTracker.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkMatrix4x4.h>
#include <vtkMath.h>

// System includes
#include <string>
#include <map>
#include <fstream>

// Atracsys includes
#include "ftkErrors.h"
#include "ftkEvent.h"
#include "ftkInterface.h"
#include "ftkOptions.h"
#include "ftkPlatform.h"
#include "ftkTypes.h"

#define ENABLE_ONBOARD_PROCESSING_OPTION 6000
#define SENDING_IMAGES_OPTION 6003


vtkStandardNewMacro(vtkPlusAtracsysTracker);

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
      std::cerr << "Cannot find section \"" << section << "\"" << std::endl;
      return false;
    }
    return true;
  }

  // ----------------------------------------------------------------------------

  bool checkKey(IniFile& p, const std::string& section, const std::string& key)
  {
    if (p.sections[section].find(key) == p.sections[section].end())
    {
      std::cerr << "Cannot find key \"" << key << "\" in section \""
        << section << "\"" << std::endl;
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
class vtkPlusAtracsysTracker::vtkInternal
{
public:
  vtkPlusAtracsysTracker* External;

  vtkInternal(vtkPlusAtracsysTracker* external)
    : External(external)
  {
  }

  virtual ~vtkInternal()
  {
  }

  // matches tool id to .ini geometry file names/paths
  std::map<std::string, std::string> IdMappedToGeometryFilename;

  // matches tool id to ftkGeometry for updating tools
  std::map<std::string, ftkGeometry> IdMappedToFtkGeometry;

  // main library handle for Atracsys sTk Passive Tracking SDK
  ftkLibrary ftkLib;

  // tracker serial number
  uint64 TrackerSN;

  std::string GetFtkErrorString();

  PlusStatus LoadFtkGeometry(const std::string& filename, ftkGeometry& geom);
  bool LoadIniFile(std::ifstream& is, ftkGeometry& geometry);
};

//----------------------------------------------------------------------------
std::string vtkPlusAtracsysTracker::vtkInternal::GetFtkErrorString()
{
  char message[1024u];
  ftkError err(ftkGetLastErrorString(this->ftkLib, 1024u, message));
  if (err == FTK_OK)
  {
    return std::string(message);
  }
  else
  {
    return std::string("ftkLib is uninitialized.");
  }
}

struct DeviceData
{
  uint64 SerialNumber;
  ftkDeviceType Type;
};

//----------------------------------------------------------------------------
PlusStatus vtkPlusAtracsysTracker::vtkInternal::LoadFtkGeometry(const std::string& filename, ftkGeometry& geom)
{
  std::ifstream input;
  input.open(filename.c_str());

  if (!input.fail() && this->LoadIniFile(input, geom))
  {
    return PLUS_FAIL;
  }
  else
  {
    ftkBuffer buffer;
    buffer.reset();
    if (ftkGetData(this->ftkLib, this->TrackerSN, FTK_OPT_DATA_DIR, &buffer) != FTK_OK || buffer.size < 1u)
    {
      return PLUS_FAIL;
    }

    std::string fullFile(reinterpret_cast< char* >(buffer.data));
    fullFile += "\\" + filename;

    input.open(fullFile.c_str());

    if (!input.fail() && this->LoadIniFile(input, geom))
    {
      return PLUS_SUCCESS;
    }
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
bool vtkPlusAtracsysTracker::vtkInternal::LoadIniFile(std::ifstream& is, ftkGeometry& geometry)
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
    << geometry.pointsCount << " fiducials" << std::endl;

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
      << geometry.positions[i].z << ")" << std::endl;
  }

  return true;
}

//----------------------------------------------------------------------------
void fusionTrackEnumerator(uint64 sn, void* user, ftkDeviceType devType)
{
  if (user != 0)
  {
    DeviceData* ptr = reinterpret_cast<DeviceData*>(user);
    ptr->SerialNumber = sn;
    ptr->Type = devType;
  }
}

//----------------------------------------------------------------------------
vtkPlusAtracsysTracker::vtkPlusAtracsysTracker()
  : vtkPlusDevice()
  , Internal(new vtkInternal(this))
{
  LOG_TRACE("vtkPlusAtracsysTracker::vtkPlusAtracsysTracker()");

  this->FrameNumber = 0;
  this->StartThreadForInternalUpdates = true;
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
PlusStatus vtkPlusAtracsysTracker::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusAtracsysTracker::ReadConfiguration");

  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
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
    const char* geometryFile;
    if ((geometryFile = toolDataElement->GetAttribute("GeometryFile")) != NULL)
    {
      std::pair<std::string, std::string> thisTool(toolId, geometryFile);
      this->Internal->IdMappedToGeometryFilename.insert(thisTool);
    }
    else
    {
LOG_ERROR("Failed to initialize Atracsys tool " << toolId << ": GeometryFile is missing.")
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
PlusStatus vtkPlusAtracsysTracker::InternalConnect()
{
  LOG_TRACE("vtkPlusAtracsysTracker::InternalConnect");

  // initialize SDK
  this->Internal->ftkLib = ftkInit();

  if (this->Internal->ftkLib == NULL)
  {
    LOG_ERROR("Failed to open Atracys sTK Passive Tracking SDK.");
  }

  DeviceData device;
  device.SerialNumber = 0uLL;

  // Scan for devices
  ftkError err(FTK_OK);
  err = ftkEnumerateDevices(this->Internal->ftkLib, fusionTrackEnumerator, &device);
  LOG_WARNING(std::endl << this->Internal->GetFtkErrorString());

  if (device.SerialNumber == 0uLL)
  {
    LOG_ERROR("No Atracsys device connected.");
    ftkClose(&this->Internal->ftkLib);
    return PLUS_FAIL;
  }

  std::string deviceType;
  switch (device.Type)
  {
  case DEV_SPRYTRACK_180:
    deviceType = "sTk 180";
    break;
  case DEV_FUSIONTRACK_500:
    deviceType = "fTk 500";
    break;
  case DEV_FUSIONTRACK_250:
    deviceType = "fTk 250";
    break;
  case DEV_SIMULATOR:
    deviceType = "fTk simulator";
    break;
  default:
    deviceType = " UNKNOWN";
  }

  this->Internal->TrackerSN = device.SerialNumber;

  LOG_INFO("Detected " << deviceType << " with serial number 0x" << std::setw(16u)
    << std::setfill('0') << std::hex << device.SerialNumber << std::dec
    << std::endl << std::setfill('\0'));

  // set spryTrack to do onboard image processing
  if (DEV_SPRYTRACK_180 == device.Type)
  {
    cout << "Enable onboard processing" << endl;
    if (ftkSetInt32(this->Internal->ftkLib, this->Internal->TrackerSN, ENABLE_ONBOARD_PROCESSING_OPTION, 1) != FTK_OK)
    {
      LOG_ERROR("Cannot process data directly on the SpryTrack.");
      return PLUS_FAIL;
    }

    cout << "Disable images sending" << endl;
    if (ftkSetInt32(this->Internal->ftkLib, this->Internal->TrackerSN, SENDING_IMAGES_OPTION, 0) != FTK_OK)
    {
      LOG_ERROR("Cannot disable images sending on the SpryTrack.");
      return PLUS_FAIL;
    }
  }

  // load geometries onto Atracsys
  std::map<std::string, std::string>::iterator it;
  for (it = begin(this->Internal->IdMappedToGeometryFilename); it != end(this->Internal->IdMappedToGeometryFilename); it++)
  {
    ftkGeometry geom;
    std::string geomFilePath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(it->second);
    this->Internal->LoadFtkGeometry(geomFilePath, geom);
    if (ftkSetGeometry(this->Internal->ftkLib, this->Internal->TrackerSN, &geom) != FTK_OK)
    {
      LOG_ERROR("Failed to load geometry " << it->second);
      LOG_ERROR(this->Internal->GetFtkErrorString());
    }
    //cout << it->first << " " << it->second;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAtracsysTracker::InternalDisconnect()
{
  LOG_TRACE("vtkPlusAtracsysTracker::InternalDisconnect");

  // turn off led
  ftkSetInt32(this->Internal->ftkLib, this->Internal->TrackerSN, 94, 0);

  ftkError err(FTK_OK);
  err = ftkClose(&this->Internal->ftkLib);
  if (err != FTK_OK)
  {
    LOG_ERROR("Failed to close Atracys sTK Passive Tracking SDK.");
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
PlusStatus vtkPlusAtracsysTracker::InternalUpdate()
{
  LOG_TRACE("vtkPlusAtracsysTracker::InternalUpdate");

  ftkSetInt32(this->Internal->ftkLib, this->Internal->TrackerSN, 94, 1);
  ftkSetInt32(this->Internal->ftkLib, this->Internal->TrackerSN, 90, 255); // red
  ftkSetInt32(this->Internal->ftkLib, this->Internal->TrackerSN, 91, 0); // green
  ftkSetInt32(this->Internal->ftkLib, this->Internal->TrackerSN, 92, 255); // blue
  return PLUS_SUCCESS;
}