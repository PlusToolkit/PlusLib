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



  // matches tool id to .ini geometry file names/paths
  std::map<std::string, std::string> IdMappedToGeometryFilename;

  // matches tool id to ftkGeometry for updating tools
  std::map<std::string, ftkGeometry> IdMappedToFtkGeometry;

  // main library handle for Atracsys sTk Passive Tracking SDK
  ftkLibrary ftkLib;

  // tracker serial number
  uint64 TrackerSN;

  std::string GetFtkErrorString();

  //DeviceData Device;
  //void DeviceEnumCallback(uint64 sn, void* user, ftkDeviceType type);
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