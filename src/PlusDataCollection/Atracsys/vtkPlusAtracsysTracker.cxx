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

#ifdef LINUX
#include <unistd.h>
#endif
#ifdef WIN32
#include <windows.h>
#endif

// Atracsys includes
#include "ftkErrors.h"
#include "ftkEvent.h"
#include "ftkInterface.h"
#include "ftkOptions.h"
#include "ftkPlatform.h"
#include "ftkTypes.h"

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

  int MaxMissingFiducials = 0;
  float MaxMeanRegistrationErrorMm = 2.0;
  int ActiveMarkerPairingTimeSec = 0;

  // type of tracker connected
  Atracsys::DEVICE_TYPE DeviceType = Atracsys::UNKNOWN;

  // matches plus tool id to .ini geometry file names/paths
  std::map<std::string, std::string> PlusIdMappedToGeometryFilename;

  // matches fusionTrack internal tool geometry ID to Plus tool ID for updating tools
  std::map<int, std::string> FtkGeometryIdMappedToToolId;

  // Atracsys API wrapper class handle
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
    LOG_WARNING("Invalid maximum number of missing fiducials provided. Must be between 0 and 3 inclusive. Maximum missing fiducials has been set to its default value of 0.");
    this->Internal->MaxMissingFiducials = 0;
  }

  XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_OPTIONAL(float, MaxMeanRegistrationErrorMm, this->Internal->MaxMeanRegistrationErrorMm, deviceConfig);
  if (this->Internal->MaxMeanRegistrationErrorMm < 0.1 || this->Internal->MaxMeanRegistrationErrorMm > 5.0)
  {
    LOG_WARNING("Invalid maximum mean registration error provided. Must be between 0.1 and 5 mm inclusive. Maximum mean registration error has been set to its default value of 5.0mm.");
    this->Internal->MaxMeanRegistrationErrorMm = 2.0;
  }

  XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, ActiveMarkerPairingTimeSec, this->Internal->ActiveMarkerPairingTimeSec, deviceConfig);
  if (this->Internal->ActiveMarkerPairingTimeSec > 60)
  {
    LOG_WARNING("Are you sure you want to leave active marker pairing enabled for more than 60 seconds?");
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

  this->Internal->Tracker->Connect();

  // get device type
  this->Internal->Tracker->GetDeviceType(this->Internal->DeviceType);

  // TODO: add onboard processing and image streaming to config file and add error checking
  this->Internal->Tracker->EnableOnboardProcessing();
  this->Internal->Tracker->DisableImageStreaming();
  this->Internal->Tracker->DisableWirelessMarkerStatusStreaming();
  this->Internal->Tracker->DisableWirelessMarkerBatteryStreaming();

  // load passive geometries onto Atracsys
  std::map<std::string, std::string>::iterator it;
  for (it = begin(this->Internal->PlusIdMappedToGeometryFilename); it != end(this->Internal->PlusIdMappedToGeometryFilename); it++)
  {
    // load user defined geometry file
    // TODO: add check for conflicting marker IDs
    std::string geomFilePath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(it->second);
    int geometryId;
    this->Internal->Tracker->LoadMarkerGeometry(geomFilePath, geometryId);
    std::pair<int, std::string> newTool(geometryId, it->first); 
    this->Internal->FtkGeometryIdMappedToToolId.insert(newTool);
  }

  // TODO: add ability to set number of missing fiducials for a validly tracked marker
  /*if (ftkSetInt32(Internal->ftkLib, Internal->TrackerSN, MAXIMUM_MISSING_POINTS_OPTION, this->Internal->MaxMissingFiducials) != FTK_OK)
  {
    std::cout << "Cannot enable markers with hidden fiducials to be tracked.";
  }*/

  // make LED blue during pairing
  this->Internal->Tracker->SetUserLEDState(0, 0, 255, 0);

  // pair active markers
  this->Internal->Tracker->EnableWirelessMarkerPairing();
  LOG_INFO(endl << " *** Put marker in front of the device to pair ***" << endl);

  // sleep while waiting for tracker to pair active markers
  #ifdef LINUX
    usleep(1000*this->Internal->ActiveMarkerPairingTimeSec);
  #endif
  #ifdef WIN32
    Sleep(1000*this->Internal->ActiveMarkerPairingTimeSec);
  #endif
  
  LOG_INFO(" *** End of wireless pairing window ***" << endl);
  this->Internal->Tracker->DisableWirelessMarkerPairing();

  // make LED green, pairing is complete
  this->Internal->Tracker->SetUserLEDState(0, 255, 0, 0);

  // TODO: check number of active markers paired
  // TODO: print info about paired markers here
  
  cout << "Additional info about paired markers:" << this->Internal->Tracker->GetMarkerInfo() << std::endl;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusAtracsysTracker::InternalDisconnect()
{
  LOG_TRACE("vtkPlusAtracsysTracker::InternalDisconnect");
  this->Internal->Tracker->DisableUserLED();
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
  LOG_TRACE("vtkPlusAtracsysTracker::InternalUpdate");
  const double unfilteredTimestamp = vtkPlusAccurateTimer::GetSystemTime();

  std::vector<Atracsys::Marker> markers;
  this->Internal->Tracker->GetMarkersInFrame(markers);

  std::map<int, std::string>::iterator it;
  for (it = begin(this->Internal->FtkGeometryIdMappedToToolId); it != end(this->Internal->FtkGeometryIdMappedToToolId); it++)
  {
    bool toolUpdated = false;

    std::vector<Atracsys::Marker>::iterator mit;
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
      PlusTransformName toolTransformName(it->second, this->GetToolReferenceFrameName());
      std::string toolSourceId = toolTransformName.GetTransformName();
      ToolTimeStampedUpdate(toolSourceId, mit->GetTransformToTracker(), TOOL_OK, this->FrameNumber, unfilteredTimestamp);
    }

    if (!toolUpdated)
    {
      // tool is not seen in this frame
      vtkSmartPointer<vtkMatrix4x4> emptyTransform = vtkSmartPointer<vtkMatrix4x4>::New();
      emptyTransform->Identity();
      PlusTransformName toolTransformName(it->second, this->GetToolReferenceFrameName());
      std::string toolSourceId = toolTransformName.GetTransformName();
      ToolTimeStampedUpdate(toolSourceId, emptyTransform, TOOL_OUT_OF_VIEW, this->FrameNumber, unfilteredTimestamp);
    }

  }

  this->FrameNumber++;
 
  return PLUS_SUCCESS;
}