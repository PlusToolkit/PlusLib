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

// VTK includes
#include <vtkMath.h>
#include <vtkNew.h>
#include <vtkMatrix4x4.h>

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
const char* vtkPlusAtracsysTracker::ATRACSYS_COMMAND_SET_FLAG        = "SetFlag";
const char* vtkPlusAtracsysTracker::ATRACSYS_COMMAND_LED_ENABLED     = "LedEnabled";
const char* vtkPlusAtracsysTracker::ATRACSYS_COMMAND_LASER_ENABLED   = "LaserEnabled";
const char* vtkPlusAtracsysTracker::ATRACSYS_COMMAND_VIDEO_ENABLED   = "VideoEnabled";
const char* vtkPlusAtracsysTracker::ATRACSYS_COMMAND_SET_LED_RGBF    = "SetLED";
const char* vtkPlusAtracsysTracker::ATRACSYS_COMMAND_ENABLE_TOOL     = "EnableTool";
const char* vtkPlusAtracsysTracker::ATRACSYS_COMMAND_ADD_TOOL        = "AddTool";


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

  int MaxMissingFiducials = 0;
  float MaxMeanRegistrationErrorMm = 2.0;
  int ActiveMarkerPairingTimeSec = 0;

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
PlusStatus vtkPlusAtracsysTracker::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusAtracsysTracker::ReadConfiguration");

  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, MaxMissingFiducials, this->Internal->MaxMissingFiducials, deviceConfig);
  if (this->Internal->MaxMissingFiducials < 0 || this->Internal->MaxMissingFiducials > 3)
  {
    LOG_WARNING("Invalid maximum number of missing fiducials provided. Must be between 0 and 3 inclusive. Maximum missing fiducials has been set to its default value of 0.");
    this->Internal->MaxMissingFiducials = 0;
  }

  XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_OPTIONAL(float, MaxMeanRegistrationErrorMm, this->Internal->MaxMeanRegistrationErrorMm, deviceConfig);
  if (this->Internal->MaxMeanRegistrationErrorMm < 0.1 || this->Internal->MaxMeanRegistrationErrorMm > 5.0)
  {
    LOG_WARNING("Invalid maximum mean registration error provided. Must be between 0.1 and 5 mm inclusive. Maximum mean registration error has been set to its default value of 2.0mm.");
    this->Internal->MaxMeanRegistrationErrorMm = 2.0;
  }

  XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, ActiveMarkerPairingTimeSec, this->Internal->ActiveMarkerPairingTimeSec, deviceConfig);
  if (this->Internal->ActiveMarkerPairingTimeSec > 15)
  {
    LOG_WARNING("Marker pairing time is set to " << this->Internal->ActiveMarkerPairingTimeSec << "seconds, tracking will not start until this period is over.");
  }

  enum TRACKING_TYPE
  {
    ACTIVE,
    PASSIVE
  };

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
PlusStatus vtkPlusAtracsysTracker::InternalConnect()
{
  LOG_TRACE("vtkPlusAtracsysTracker::InternalConnect");

  // Connect to tracker
  ATRACSYS_RESULT result = this->Internal->Tracker.Connect();
  if (result != ATR_SUCCESS && result != AtracsysTracker::ATRACSYS_RESULT::WARNING_CONNECTED_IN_USB2)
  {
    LOG_ERROR(this->Internal->Tracker.ResultToString(result));
    return PLUS_FAIL;
  }
  else if (result == AtracsysTracker::ATRACSYS_RESULT::WARNING_CONNECTED_IN_USB2)
  {
    LOG_WARNING(this->Internal->Tracker.ResultToString(result));
  }

  // get device type
  this->Internal->Tracker.GetDeviceType(this->Internal->DeviceType);

  // if spryTrack, setup for onboard processing and disable extraneous marker info streaming
  if (this->Internal->DeviceType == AtracsysTracker::DEVICE_TYPE::SPRYTRACK_180)
  {
    this->Internal->Tracker.SetSpryTrackProcessingType(AtracsysTracker::SPRYTRACK_IMAGE_PROCESSING_TYPE::PROCESSING_ONBOARD);
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

  if ((result = this->Internal->Tracker.SetMaxMissingFiducials(this->Internal->MaxMissingFiducials)) != ATR_SUCCESS)
  {
    LOG_WARNING(this->Internal->Tracker.ResultToString(result));
  }

  // make LED blue during pairing
  this->Internal->Tracker.SetUserLEDState(0, 0, 255, 0);

  // pair active markers
  if ((result = this->Internal->Tracker.EnableWirelessMarkerPairing(true)) != ATR_SUCCESS)
  {
    LOG_ERROR(this->Internal->Tracker.ResultToString(result));
    return PLUS_FAIL;
  }
  LOG_INFO("Active marker pairing period started.");

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
  LOG_INFO("Additional info about paired markers:" << markerInfo << std::endl);

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
PlusStatus vtkPlusAtracsysTracker::InternalUpdate()
{
  LOG_TRACE("vtkPlusAtracsysTracker::InternalUpdate");
  const double unfilteredTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();

  std::vector<AtracsysTracker::Marker> markers;

  ATRACSYS_RESULT result = result = this->Internal->Tracker.GetMarkersInFrame(markers);
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

  std::map<int, std::string>::iterator it;
  for (it = begin(this->Internal->FtkGeometryIdMappedToToolId); it != end(this->Internal->FtkGeometryIdMappedToToolId); it++)
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
    for (mit = begin(markers); mit != end(markers); mit++)
    {
      if (it->first != mit->GetGeometryID())
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
      igsioTransformName toolTransformName(it->second, this->GetToolReferenceFrameName());
      std::string toolSourceId = toolTransformName.GetTransformName();
      ToolTimeStampedUpdate(toolSourceId, mit->GetTransformToTracker(), TOOL_OK, this->FrameNumber, unfilteredTimestamp);
    }

    if (!toolUpdated)
    {
      // tool is not seen in this frame
      vtkNew<vtkMatrix4x4> emptyTransform;
      igsioTransformName toolTransformName(it->second, this->GetToolReferenceFrameName());
      std::string toolSourceId = toolTransformName.GetTransformName();
      ToolTimeStampedUpdate(toolSourceId, emptyTransform.GetPointer(), TOOL_OUT_OF_VIEW, this->FrameNumber, unfilteredTimestamp);
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
  }
  else
  {
    // tool is disabled, add to disabled list
    this->DisabledToolIds.push_back(toolId);
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAtracsysTracker::AddGeometry(std::string toolId, std::string geomString)
{
  int geometryId;
  ATRACSYS_RESULT result;
  if ((result = this->Internal->Tracker.LoadMarkerGeometryFromString(geomString, geometryId)) != ATR_SUCCESS)
  {
    LOG_ERROR(this->Internal->Tracker.ResultToString(result) << " This error occurred when trying to load the following geometry information: " << geomString);
    return PLUS_FAIL;
  }
  std::pair<int, std::string> newTool(geometryId, toolId);
  this->Internal->FtkGeometryIdMappedToToolId.insert(newTool);
  return PLUS_FAIL;
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