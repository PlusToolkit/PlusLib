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
#include <iostream>

// Atracsys includes
#include "ftkErrors.h"
#include "ftkEvent.h"
#include "ftkInterface.h"
#include "ftkOptions.h"
#include "ftkPlatform.h"
#include "ftkTypes.h"

static const unsigned ENABLE_ONBOARD_PROCESSING_OPTION = 6000;
static const unsigned SENDING_IMAGES_OPTION = 6003;
static const unsigned DEV_ENABLE_PAIRING = 7000;
static const unsigned DEV_MARKERS_INFO = 7005;
static const unsigned MAXIMUM_MISSING_POINTS_OPTION = 10004;

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
      LOG_ERROR("Cannot find section \"" << section << "\"");
      return false;
    }
    return true;
  }

  // ----------------------------------------------------------------------------

  bool checkKey(IniFile& p, const std::string& section, const std::string& key)
  {
    if (p.sections[section].find(key) == p.sections[section].end())
    {
      LOG_ERROR("Cannot find key \"" << key << "\" in section \"" << section << "\"");
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
  std::map<int, std::string> FtkGeometryIdMappedToToolId;

  // main library handle for Atracsys sTk Passive Tracking SDK
  ftkLibrary ftkLib;

  // tracker serial number
  uint64 TrackerSN;

  std::string GetFtkErrorString();

  int MaxMissingFiducials = 1;
  int ActiveMarkerPairingTimeSec = 0;

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

  LOG_INFO("Loading geometry " << geometry.geometryId << ", composed of "
    << geometry.pointsCount << " fiducials");

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

    LOG_INFO("Loaded fiducial " << i << " ("
      << geometry.positions[i].x << ", "
      << geometry.positions[i].y << ", "
      << geometry.positions[i].z << ")")
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
  this->InternalUpdateRate = 60;
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

  XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, MaxMissingFiducials, this->Internal->MaxMissingFiducials, deviceConfig);
  if (this->Internal->MaxMissingFiducials < 0 || this->Internal->MaxMissingFiducials > 3)
  {
    LOG_WARNING("Invalid maximum number of missing fiducials provided. Must be between 0 and 3 inclusive. Maximum missing fiducials has been set to it's default value of 0.");
    this->Internal->MaxMissingFiducials = 0;
  }

  XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, ActiveMarkerPairingTimeSec, this->Internal->ActiveMarkerPairingTimeSec, deviceConfig);
  if (this->Internal->ActiveMarkerPairingTimeSec > 60)
  {
    LOG_WARNING("Are you sure you want to leave active marker pairing enabled for more than 60 seconds?");
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

  // scan for devices
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
  if (device.Type == DEV_SPRYTRACK_180)
  {
    LOG_INFO("Enable onboard processing");
    if (ftkSetInt32(this->Internal->ftkLib, this->Internal->TrackerSN, ENABLE_ONBOARD_PROCESSING_OPTION, 1) != FTK_OK)
    {
      LOG_ERROR("Cannot process data directly on the SpryTrack.");
      return PLUS_FAIL;
    }

    LOG_INFO("Disable images sending");
    if (ftkSetInt32(this->Internal->ftkLib, this->Internal->TrackerSN, SENDING_IMAGES_OPTION, 0) != FTK_OK)
    {
      LOG_ERROR("Cannot disable image sending on the SpryTrack.");
      return PLUS_FAIL;
    }
  }

  // load passive geometries onto Atracsys
  std::map<std::string, std::string>::iterator it;
  for (it = begin(this->Internal->IdMappedToGeometryFilename); it != end(this->Internal->IdMappedToGeometryFilename); it++)
  {
    if (it->second == "ATRACSYS_ACTIVE_BOOMERANG")
    {
      // tool is white boomerang
      std::pair<int, std::string> newTool(37100000, it->first);
      this->Internal->FtkGeometryIdMappedToToolId.insert(newTool);
    }
    else
    {
      // load user defined geometry file
      ftkGeometry geom;
      std::string geomFilePath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(it->second);
      this->Internal->LoadFtkGeometry(geomFilePath, geom);
      if (ftkSetGeometry(this->Internal->ftkLib, this->Internal->TrackerSN, &geom) != FTK_OK)
      {
        LOG_ERROR("Failed to load geometry " << it->second);
        LOG_ERROR(this->Internal->GetFtkErrorString());
      }

      std::pair<int, std::string> newTool(geom.geometryId, it->first);
      this->Internal->FtkGeometryIdMappedToToolId.insert(newTool);
    }
  }

  // ensure markers can be tracked with only 3 visible fiducials
  // TODO: make this a parameter in config file
  if (ftkSetInt32(Internal->ftkLib, Internal->TrackerSN, MAXIMUM_MISSING_POINTS_OPTION, this->Internal->MaxMissingFiducials) != FTK_OK)
  {
    std::cout << "Cannot enable markers with hidden fiducials to be tracked.";
  }

  // make LED blue during pairing
  ftkSetInt32(this->Internal->ftkLib, this->Internal->TrackerSN, 94, 1);
  ftkSetInt32(this->Internal->ftkLib, this->Internal->TrackerSN, 90, 0); // red
  ftkSetInt32(this->Internal->ftkLib, this->Internal->TrackerSN, 91, 0); // green
  ftkSetInt32(this->Internal->ftkLib, this->Internal->TrackerSN, 92, 255); // blue

  // pair active markers
  // TODO: check number of active markers paired
  cout << "Enable Pairing" << endl;
  if (ftkSetInt32(Internal->ftkLib, Internal->TrackerSN, DEV_ENABLE_PAIRING, 1) != FTK_OK)
  {
    std::cout << "Cannot enable pairing.";
  }
  cout << endl << " *** Put marker in front of the device to pair ***" << endl;

  Sleep(1000*this->Internal->ActiveMarkerPairingTimeSec);

  cout << "Disable Pairing" << endl;
  if (ftkSetInt32(Internal->ftkLib, Internal->TrackerSN, DEV_ENABLE_PAIRING, 0) != FTK_OK)
  {
    std::cout << "Cannot disable pairing.";
  }

  cout << "Additional info about paired markers:" << endl;

  ftkBuffer buffer;
  if (ftkGetData(Internal->ftkLib, Internal->TrackerSN, DEV_MARKERS_INFO, &buffer) != FTK_OK)
  {
    std::cout << "Cannot get additional infos on the markers.";
  }
  std::string info(buffer.data, buffer.data + buffer.size);
  cout << info << endl;

  // make LED green once pairing is over
  ftkSetInt32(this->Internal->ftkLib, this->Internal->TrackerSN, 94, 1);
  ftkSetInt32(this->Internal->ftkLib, this->Internal->TrackerSN, 90, 0); // red
  ftkSetInt32(this->Internal->ftkLib, this->Internal->TrackerSN, 91, 255); // green
  ftkSetInt32(this->Internal->ftkLib, this->Internal->TrackerSN, 92, 0); // blue

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
  
  const double unfilteredTimestamp = vtkPlusAccurateTimer::GetSystemTime();

  ftkFrameQuery* frame = ftkCreateFrame();

  if (frame == 0)
  {
    LOG_ERROR("Cannot create frame instance");
    LOG_ERROR(this->Internal->GetFtkErrorString());
    return PLUS_FAIL;
  }

  ftkError err(ftkSetFrameOptions(false, false, 128u, 128u,
    4u * FTK_MAX_FIDUCIALS, 4u, frame));

  if (err != FTK_OK)
  {
    ftkDeleteFrame(frame);
    LOG_ERROR("Cannot initialize frame");
    LOG_ERROR(this->Internal->GetFtkErrorString());
  }

  if (ftkGetLastFrame(this->Internal->ftkLib, this->Internal->TrackerSN, frame, 0) != FTK_OK)
  {
    // block until next frame is available from camera
    LOG_INFO("No frame avialable from camera.");
    return PLUS_SUCCESS;
  }

  switch (frame->markersStat)
  {
  case QS_WAR_SKIPPED:
    ftkDeleteFrame(frame);
    LOG_ERROR("marker fields in the frame are not set correctly");
    LOG_ERROR(this->Internal->GetFtkErrorString());
    return PLUS_FAIL;

  case QS_ERR_INVALID_RESERVED_SIZE:
    ftkDeleteFrame(frame);
    LOG_ERROR("frame -> markersVersionSize is invalid");
    LOG_ERROR(this->Internal->GetFtkErrorString());
    return PLUS_FAIL;
    
  default:
    ftkDeleteFrame(frame);
    LOG_ERROR("invalid status");
    LOG_ERROR(this->Internal->GetFtkErrorString());
    return PLUS_FAIL;

  case QS_OK:
    break;
  }

  if (frame->markersStat == QS_ERR_OVERFLOW)
  {
    LOG_ERROR("Marker overflow. Too many markers in frame.");
    return PLUS_FAIL;
  }

  std::map<int, std::string>::iterator it;
  for (it = begin(this->Internal->FtkGeometryIdMappedToToolId); it != end(this->Internal->FtkGeometryIdMappedToToolId); it++)
  {
    ftkMarker* marker;
    bool toolUpdated = false;

    for (size_t m = 0; m < frame->markersCount; m++)
    {
      if (it->first != frame->markers[m].geometryId)
      {
        continue;
      }
      marker = &(frame->markers[m]);
      toolUpdated = true;
      vtkSmartPointer<vtkMatrix4x4> toolToTracker = vtkSmartPointer<vtkMatrix4x4>::New();
      toolToTracker->Identity();
      for (int i = 0; i < 3; i++)
      {
        toolToTracker->SetElement(i, 3, marker->translationMM[i]);
        for (int j = 0; j < 3; j++)
        {
          toolToTracker->SetElement(i, j, marker->rotation[i][j]);
        }
      }
      PlusTransformName toolTransformName(it->second, this->GetToolReferenceFrameName());
      std::string toolSourceId = toolTransformName.GetTransformName();

      ToolTimeStampedUpdate(toolSourceId, toolToTracker, TOOL_OK, this->FrameNumber, unfilteredTimestamp);
    }

    if (!toolUpdated)
    {
      vtkSmartPointer<vtkMatrix4x4> emptyTransform = vtkSmartPointer<vtkMatrix4x4>::New();
      emptyTransform->Identity();
      PlusTransformName toolTransformName(it->second, this->GetToolReferenceFrameName());
      std::string toolSourceId = toolTransformName.GetTransformName();
      ToolTimeStampedUpdate(toolSourceId, emptyTransform, TOOL_OUT_OF_VIEW, this->FrameNumber, unfilteredTimestamp);
    }
  }

  this->FrameNumber++;
  
  // close frame
  ftkDeleteFrame(frame);

  return PLUS_SUCCESS;
}