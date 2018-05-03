/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusAtracsysTracker.h"
#include "AtracsysTracker.h"
#include "AtracsysMarker.h"

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

  //std::string GetFtkErrorString();

  int MaxMissingFiducials = 1;
  int ActiveMarkerPairingTimeSec = 0;

  //PlusStatus LoadFtkGeometry(const std::string& filename, ftkGeometry& geom);
  //bool LoadIniFile(std::ifstream& is, ftkGeometry& geometry);

  // here begins the new interface
  Atracsys::Tracker* Tracker = new Atracsys::Tracker();
};

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

  this->Internal->Tracker->Connect();

  // TODO: add onboard processing and image streaming to config file and add error checking
  this->Internal->Tracker->EnableOnboardProcessing();
  this->Internal->Tracker->DisableImageStreaming();

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
      // TODO: add check for conflicting marker IDs
      std::string geomFilePath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(it->second);
      int geometryId;
      this->Internal->Tracker->LoadMarkerGeometry(geomFilePath, geometryId);
      std::pair<int, std::string> newTool(geometryId, it->first);
      this->Internal->FtkGeometryIdMappedToToolId.insert(newTool);
    }
  }

  // TODO: add ability to set number of missing fiducials for a validly tracked marker
  /*if (ftkSetInt32(Internal->ftkLib, Internal->TrackerSN, MAXIMUM_MISSING_POINTS_OPTION, this->Internal->MaxMissingFiducials) != FTK_OK)
  {
    std::cout << "Cannot enable markers with hidden fiducials to be tracked.";
  }*/

  // make LED blue during pairing
  this->Internal->Tracker->SetUserLEDState(0, 0, 255, 0);

  // pair active markers
  this->Internal->Tracker->EnableImageStreaming();
  LOG_INFO(endl << " *** Put marker in front of the device to pair ***" << endl);
  Sleep(1000*this->Internal->ActiveMarkerPairingTimeSec);
  LOG_INFO(" *** End of wireless pairing window ***" << endl);
  this->Internal->Tracker->DisableWirelessMarkerPairing();

  // make LED green, pairing is complete
  this->Internal->Tracker->SetUserLEDState(0, 255, 0, 0);

  // TODO: check number of active markers paired
  // TODO: print info about paired markers here
  /*
  cout << "Additional info about paired markers:" << endl;
  ftkBuffer buffer;
  if (ftkGetData(Internal->ftkLib, Internal->TrackerSN, DEV_MARKERS_INFO, &buffer) != FTK_OK)
  {
    std::cout << "Cannot get additional infos on the markers.";
  }
  std::string info(buffer.data, buffer.data + buffer.size);
  cout << info << endl;
  */

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAtracsysTracker::InternalDisconnect()
{
  LOG_TRACE("vtkPlusAtracsysTracker::InternalDisconnect");
  this->Internal->Tracker->SetUserLEDState(0, 0, 0, 0);
  this->Internal->Tracker->Disconnect();
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
  /*
  LOG_TRACE("vtkPlusAtracsysTracker::InternalUpdate");
  
  const double unfilteredTimestamp = vtkPlusAccurateTimer::GetSystemTime();

  ftkFrameQuery* frame = ftkCreateFrame();

  if (frame == 0)
  {
    LOG_ERROR("Cannot create frame instance");
    LOG_ERROR(this->Internal->Tracker->GetFtkLastErrorString());
    return PLUS_FAIL;
  }

  ftkError err(ftkSetFrameOptions(false, false, 128u, 128u,
    4u * FTK_MAX_FIDUCIALS, 4u, frame));

  if (err != FTK_OK)
  {
    ftkDeleteFrame(frame);
    LOG_ERROR("Cannot initialize frame");
    LOG_ERROR(this->Internal->Tracker->GetFtkLastErrorString());
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
    LOG_ERROR(this->Internal->Tracker->GetFtkLastErrorString());
    return PLUS_FAIL;

  case QS_ERR_INVALID_RESERVED_SIZE:
    ftkDeleteFrame(frame);
    LOG_ERROR("frame -> markersVersionSize is invalid");
    LOG_ERROR(this->Internal->Tracker->GetFtkLastErrorString());
    return PLUS_FAIL;
    
  default:
    ftkDeleteFrame(frame);
    LOG_ERROR("invalid status");
    LOG_ERROR(this->Internal->Tracker->GetFtkLastErrorString());
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
  */
  return PLUS_SUCCESS;
}