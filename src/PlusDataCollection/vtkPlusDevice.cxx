/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusBuffer.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusDevice.h"
#include "vtkIGSIORecursiveCriticalSection.h"
#include "vtkPlusSequenceIO.h"
#include "vtkIGSIOTrackedFrameList.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkMatrix4x4.h>
#include <vtkMultiThreader.h>
#include <vtkObjectFactory.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkWindows.h>
#include <vtksys/SystemTools.hxx>

// STD includes
#include <set>

// System includes
#include <ctype.h>
#include <time.h>

#if ( _MSC_VER >= 1300 ) // Visual studio .NET
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4312 )
#endif

#define LOCAL_LOG_ERROR(msg) \
{ \
  std::ostringstream msgStream; \
  if( this->DeviceId.empty() ) \
  { \
    msgStream << " " << msg << std::ends; \
  } \
  else \
  { \
    msgStream << this->DeviceId << ": " << msg << std::ends; \
  } \
  std::string finalStr(msgStream.str()); \
  LOG_ERROR(finalStr); \
}
#define LOCAL_LOG_WARNING(msg) \
{ \
  std::ostringstream msgStream; \
  if( this->DeviceId.empty() ) \
{ \
  msgStream << " " << msg << std::ends; \
} \
  else \
{ \
  msgStream << this->DeviceId << ": " << msg << std::ends; \
} \
  std::string finalStr(msgStream.str()); \
  LOG_WARNING(finalStr); \
}
#define LOCAL_LOG_INFO(msg) \
{ \
  std::ostringstream msgStream; \
  if( this->DeviceId.empty() ) \
{ \
  msgStream << " " << msg << std::ends; \
} \
  else \
{ \
  msgStream << this->DeviceId << ": " << msg << std::ends; \
} \
  std::string finalStr(msgStream.str()); \
  LOG_INFO(finalStr); \
}
#define LOCAL_LOG_DEBUG(msg) \
{ \
  std::ostringstream msgStream; \
  if( this->DeviceId.empty() ) \
{ \
  msgStream << " " << msg << std::ends; \
} \
  else \
{ \
  msgStream << this->DeviceId << ": " << msg << std::ends; \
} \
  std::string finalStr(msgStream.str()); \
  LOG_DEBUG(finalStr); \
}
#define LOCAL_LOG_TRACE(msg) \
{ \
  std::ostringstream msgStream; \
  if( this->DeviceId.empty() ) \
{ \
  msgStream << " " << msg << std::ends; \
} \
  else \
{ \
  msgStream << this->DeviceId << ": " << msg << std::ends; \
} \
  std::string finalStr(msgStream.str()); \
  LOG_TRACE(finalStr); \
}

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusDevice);

const int vtkPlusDevice::VIRTUAL_DEVICE_FRAME_RATE = 50;
static const int FRAME_RATE_AVERAGING = 10;
const std::string vtkPlusDevice::BMODE_PORT_NAME = "B";
const std::string vtkPlusDevice::RFMODE_PORT_NAME = "Rf";
const std::string vtkPlusDevice::PARAMETERS_XML_ELEMENT_TAG = "Parameters";
const std::string vtkPlusDevice::PARAMETER_XML_ELEMENT_TAG = "Parameter";

//----------------------------------------------------------------------------
vtkPlusDevice::vtkPlusDevice()
  : ThreadAlive(false)
  , Connected(0)
  , Threader(vtkMultiThreader::New())
  , ThreadId(-1)
  , CurrentStreamBufferItem(new StreamBufferItem())
  , ToolReferenceFrameName("")
  , DeviceId("")
  , DataCollector(NULL)
  , AcquisitionRate(30)
  , Recording(0)
  , DesiredTimestamp(-1)
  , UpdateWithDesiredTimestamp(0)
  , TimestampClosestToDesired(-1)
  , FrameNumber(0)
  , FrameTimeStamp(0)
  , OutputNeedsInitialization(1)
  , CorrectlyConfigured(true)
  , StartThreadForInternalUpdates(false)
  , LocalTimeOffsetSec(0.0)
  , MissingInputGracePeriodSec(0.0)
  , RequireImageOrientationInConfiguration(false)
  , RequirePortNameInDeviceSetConfiguration(false)
{
  this->SetNumberOfInputPorts(0);

  // For threaded capture of transformations
  this->UpdateMutex = vtkIGSIORecursiveCriticalSection::New();
}

//----------------------------------------------------------------------------
vtkPlusDevice::~vtkPlusDevice()
{
  LOCAL_LOG_TRACE("vtkPlusDevice::~vtkPlusDevice()");
  if (this->Connected)
  {
    Disconnect();
  }

  for (ChannelContainerIterator it = this->OutputChannels.begin(); it != this->OutputChannels.end(); ++it)
  {
    (*it)->UnRegister(this);
  }
  this->InputChannels.clear();
  this->OutputChannels.clear();
  for (DataSourceContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    it->second->UnRegister(this);
  }
  this->Tools.clear();
  for (DataSourceContainerIterator it = this->Fields.begin(); it != this->Fields.end(); ++it)
  {
    it->second->UnRegister(this);
  }
  this->Fields.clear();
  for (DataSourceContainerIterator it = this->VideoSources.begin(); it != this->VideoSources.end(); ++it)
  {
    it->second->UnRegister(this);
  }
  this->VideoSources.clear();

  delete this->CurrentStreamBufferItem;
  this->CurrentStreamBufferItem = NULL;

  DELETE_IF_NOT_NULL(this->Threader);

  DELETE_IF_NOT_NULL(this->UpdateMutex);

  LOCAL_LOG_TRACE("vtkPlusDevice::~vtkPlusDevice() completed");
}

//----------------------------------------------------------------------------
void vtkPlusDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "Connected: " << (this->Connected ? "Yes\n" : "No\n");
  os << indent << "SDK version: " << this->GetSdkVersion() << std::endl;
  os << indent << "AcquisitionRate: " << this->AcquisitionRate << std::endl;
  os << indent << "Recording: " << (this->Recording ? "On\n" : "Off\n");

  for (ChannelContainerConstIterator it = this->OutputChannels.begin(); it != this->OutputChannels.end(); ++it)
  {
    vtkPlusChannel* str = *it;
    str->PrintSelf(os, indent);
  }

  if (!this->ToolReferenceFrameName.empty())
  {
    os << indent << "ToolReferenceFrameName: " << this->ToolReferenceFrameName << "\n";
  }

  for (DataSourceContainerConstIterator it = this->VideoSources.begin(); it != this->VideoSources.end(); ++it)
  {
    vtkPlusDataSource* dataSource = it->second;
    dataSource->PrintSelf(os, indent);
  }

  for (DataSourceContainerConstIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    vtkPlusDataSource* dataSource = it->second;
    dataSource->PrintSelf(os, indent);
  }

  for (DataSourceContainerConstIterator it = this->Fields.begin(); it != this->Fields.end(); ++it)
  {
    vtkPlusDataSource* dataSource = it->second;
    dataSource->PrintSelf(os, indent);
  }

  os << indent << PARAMETERS_XML_ELEMENT_TAG << std::endl;
  for (std::map<std::string, std::string>::iterator it = this->Parameters.begin(); it != this->Parameters.end(); ++it)
  {
    os << indent << indent << it->first << ": " << it->second << std::endl;
  }
}

//----------------------------------------------------------------------------
std::string vtkPlusDevice::GetSdkVersion()
{
  // Base class version is the same as the Plus library version
  std::string ver = std::string("Plus-") + std::string(PLUSLIB_VERSION);
  return ver;
}

//----------------------------------------------------------------------------
bool vtkPlusDevice::IsResettable()
{
  // By default, devices cannot be reset.
  return false;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::Reset()
{
  // By default, Reset has no effect.
  return PLUS_SUCCESS;
  this->Parameters.clear();
}

//----------------------------------------------------------------------------
DataSourceContainerConstIterator vtkPlusDevice::GetToolIteratorBegin() const
{
  return this->Tools.begin();
}

//----------------------------------------------------------------------------
DataSourceContainerConstIterator vtkPlusDevice::GetToolIteratorEnd() const
{
  return this->Tools.end();
}

//----------------------------------------------------------------------------
int vtkPlusDevice::GetNumberOfTools() const
{
  return this->Tools.size();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::AddTool(vtkPlusDataSource* tool, bool requireUniquePortName)
{
  if (tool == NULL)
  {
    LOCAL_LOG_ERROR("Failed to add tool to tracker, tool is NULL!");
    return PLUS_FAIL;
  }

  if (tool->GetId().empty())
  {
    LOCAL_LOG_ERROR("Failed to add tool to tracker, tool source ID must be defined!");
    return PLUS_FAIL;
  }

  if (this->Tools.find(tool->GetId()) != this->GetToolIteratorEnd())
  {
    LOCAL_LOG_ERROR("Tool with ID '" << tool->GetId() << "' is already in the tool container!");
    return PLUS_FAIL;
  }

  if (requireUniquePortName && !tool->GetPortName().empty())
  {
    // Check tool port names, it should be unique too
    for (DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
    {
      if (it->second->GetPortName().empty())
      {
        continue;
      }
      if (tool->GetPortName() == it->second->GetPortName())
      {
        LOCAL_LOG_ERROR("Failed to add '" << tool->GetId() << "' tool to container: tool with name '" << it->second->GetId()
                        << "' is already defined on port '" << tool->GetPortName() << "'!");
        return PLUS_FAIL;
      }
    }
  }

  tool->Register(this);
  tool->SetDevice(this);
  this->Tools[tool->GetId()] = tool;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetFieldDataSource(const char* aSourceId, vtkPlusDataSource*& aSource) const
{
  if (aSourceId == NULL)
  {
    LOCAL_LOG_ERROR("vtkPlusDevice::GetTool failed: aToolSourceId is invalid");
    return PLUS_FAIL;
  }

  return this->GetFieldDataSource(std::string(aSourceId), aSource);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetFieldDataSource(const std::string& aSourceId, vtkPlusDataSource*& aSource) const
{
  if (this->Fields.find(aSourceId) != this->Fields.end())
  {
    aSource = this->Fields.find(aSourceId)->second;
    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
DataSourceContainerConstIterator vtkPlusDevice::GetFieldDataSourcessIteratorBegin() const
{
  return this->Fields.begin();
}

//----------------------------------------------------------------------------
DataSourceContainerConstIterator vtkPlusDevice::GetFieldDataSourcessIteratorEnd() const
{
  return this->Fields.end();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::AddFieldDataSource(vtkPlusDataSource* aSource)
{
  if (aSource == NULL)
  {
    LOCAL_LOG_ERROR("Failed to add field data source to device, source is NULL!");
    return PLUS_FAIL;
  }

  if (aSource->GetId().empty())
  {
    LOCAL_LOG_ERROR("Failed to add field data to device, field data source ID must be defined!");
    return PLUS_FAIL;
  }

  if (this->Fields.find(aSource->GetId()) != this->GetFieldDataSourcessIteratorEnd())
  {
    LOCAL_LOG_ERROR("Field data with ID '" << aSource->GetId() << "' is already in the field data container!");
    return PLUS_FAIL;
  }

  aSource->Register(this);
  aSource->SetDevice(this);
  this->Fields[aSource->GetId()] = aSource;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
int vtkPlusDevice::GetNumberOfFieldDataSources() const
{
  return this->Fields.size();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetFirstActiveTool(vtkPlusDataSource*& aTool) const
{
  if (this->Tools.size() == 0)
  {
    LOCAL_LOG_ERROR("Failed to get first active tool - there are no tools!");
    return PLUS_FAIL;
  }

  // Get the first tool
  aTool = this->Tools.begin()->second;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetTool(const char* aToolSourceId, vtkPlusDataSource*& aTool) const
{
  if (aToolSourceId == NULL)
  {
    LOCAL_LOG_ERROR("vtkPlusDevice::GetTool failed: aToolSourceId is invalid");
    return PLUS_FAIL;
  }

  if (this->Tools.find(aToolSourceId) != this->Tools.end())
  {
    aTool = this->Tools.find(aToolSourceId)->second;
    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetTool(const std::string& aToolSourceId, vtkPlusDataSource*& aTool) const
{
  return this->GetTool(aToolSourceId.c_str(), aTool);
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetToolByPortName(const char* portName, vtkPlusDataSource*& aTool)
{
  if (portName == NULL)
  {
    LOCAL_LOG_ERROR("Failed to get tool - port name is NULL!");
    return PLUS_FAIL;
  }

  return this->GetToolByPortName(std::string(portName), aTool);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetToolByPortName(const std::string& portName, vtkPlusDataSource*& aSource)
{
  for (DataSourceContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    if (portName == it->second->GetPortName())
    {
      aSource = it->second;
      return PLUS_SUCCESS;
    }
  }

  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetVideoSourcesByPortName(const char* aPortName, std::vector<vtkPlusDataSource*>& sources)
{
  if (aPortName == NULL)
  {
    LOCAL_LOG_ERROR("Failed to get video source - port name is NULL!");
    return PLUS_FAIL;
  }

  return this->GetVideoSourcesByPortName(std::string(aPortName), sources);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetVideoSourcesByPortName(const std::string& aPortName, std::vector<vtkPlusDataSource*>& sources)
{
  sources.clear();

  for (DataSourceContainerIterator it = this->VideoSources.begin(); it != this->VideoSources.end(); ++it)
  {
    if (it->second->GetPortName().empty())
    {
      LOCAL_LOG_DEBUG("Port name is not defined for video source " << (!it->second->GetId().empty() ? it->second->GetId() : "unknown"));
      continue;
    }
    if (aPortName == it->second->GetPortName())
    {
      sources.push_back(it->second);
    }
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkPlusDevice::SetToolsBufferSize(int aBufferSize)
{
  LOCAL_LOG_TRACE("vtkPlusDevice::SetToolsBufferSize(" << aBufferSize << ")");
  for (DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    it->second->SetBufferSize(aBufferSize);
  }
}

//----------------------------------------------------------------------------
void vtkPlusDevice::SetFieldDataSourcesBufferSize(int aBufferSize)
{
  LOCAL_LOG_TRACE("vtkPlusDevice::SetFieldDataSourcesBufferSize(" << aBufferSize << ")");
  for (DataSourceContainerConstIterator it = this->GetFieldDataSourcessIteratorBegin(); it != this->GetFieldDataSourcessIteratorEnd(); ++it)
  {
    it->second->SetBufferSize(aBufferSize);
  }
}

//----------------------------------------------------------------------------
void vtkPlusDevice::SetLocalTimeOffsetSec(double aTimeOffsetSec)
{
  for (DataSourceContainerIterator it = this->VideoSources.begin(); it != this->VideoSources.end(); ++it)
  {
    vtkPlusDataSource* image = it->second;
    image->SetLocalTimeOffsetSec(aTimeOffsetSec);
  }
  // local tools
  for (DataSourceContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    vtkPlusDataSource* tool = it->second;
    tool->SetLocalTimeOffsetSec(aTimeOffsetSec);
  }
  this->LocalTimeOffsetSec = aTimeOffsetSec;
}

//----------------------------------------------------------------------------
double vtkPlusDevice::GetLocalTimeOffsetSec() const
{
  return this->LocalTimeOffsetSec;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::InternalUpdate()
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetInputDevices(std::vector<vtkPlusDevice*>& outDeviceList) const
{
  for (ChannelContainerConstIterator iter = this->InputChannels.cbegin(); iter != this->InputChannels.cend(); ++iter)
  {
    const vtkPlusChannel* channel = *iter;
    outDeviceList.push_back(channel->GetOwnerDevice());
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetInputDevicesRecursive(std::vector<vtkPlusDevice*>& outDeviceList) const
{
  for (auto it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it)
  {
    (*it)->GetOwnerDevice()->GetInputDevicesRecursive(outDeviceList);
    outDeviceList.push_back((*it)->GetOwnerDevice());
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::SetParameter(const std::string& key, const std::string& value)
{
  this->Parameters[key] = value;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
std::string vtkPlusDevice::GetParameter(const std::string& key) const
{
  if (this->Parameters.find(key) != this->Parameters.end())
  {
    return this->Parameters.find(key)->second;
  }

  return "";
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetParameter(const std::string& key, std::string& outValue) const
{
  if (this->Parameters.find(key) != this->Parameters.end())
  {
    outValue = this->Parameters.find(key)->second;
    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------
void vtkPlusDevice::DeepCopy(const vtkPlusDevice& device)
{
  this->ThreadAlive = false;
  this->Connected = device.GetConnected();
  this->ToolReferenceFrameName = device.GetToolReferenceFrameName();
  this->DeviceId = device.GetDeviceId() + "_Copy";
  this->CorrectlyConfigured = device.GetCorrectlyConfigured();
  this->LocalTimeOffsetSec = device.GetLocalTimeOffsetSec();
  this->MissingInputGracePeriodSec = device.GetMissingInputGracePeriodSec();
  this->RequireImageOrientationInConfiguration = device.RequireImageOrientationInConfiguration;
  this->RequirePortNameInDeviceSetConfiguration = device.RequirePortNameInDeviceSetConfiguration;
  this->Parameters = device.Parameters;
  // Don't set data collector, because that will be done if the copied device is added to a data collector

  // VTK functions aren't const clean, this is necessary =/
  vtkPlusDevice* ewwwwDevice = const_cast<vtkPlusDevice*>(&device);
  this->SetNumberOfInputPorts(ewwwwDevice->GetNumberOfInputPorts());

  LOCAL_LOG_TRACE("vtkPlusDevice::DeepCopy");
  for (DataSourceContainerConstIterator it = device.Tools.begin(); it != device.Tools.end(); ++it)
  {
    LOCAL_LOG_DEBUG("Copy the buffer of tracker tool: " << it->first);
    if (this->AddTool(it->second, false) != PLUS_SUCCESS)
    {
      LOCAL_LOG_ERROR("Copy of tool '" << it->first << "' failed - unable to add tool to the container!");
      continue;
    }

    vtkPlusDataSource* tool = NULL;
    if (this->GetTool(it->first.c_str(), tool) != PLUS_SUCCESS)
    {
      LOCAL_LOG_ERROR("Copy of tool '" << it->first << "' failed - unable to get tool from container!");
      continue;
    }

    tool->DeepCopy(*it->second);
  }

  for (DataSourceContainerConstIterator it = device.Fields.begin(); it != device.Fields.end(); ++it)
  {
    LOCAL_LOG_DEBUG("Copy the buffer of field data source: " << it->first);
    if (this->AddFieldDataSource(it->second) != PLUS_SUCCESS)
    {
      LOCAL_LOG_ERROR("Copy of field data source '" << it->first << "' failed - unable to add field data source to the container!");
      continue;
    }

    vtkPlusDataSource* aSource = NULL;
    if (this->GetFieldDataSource(it->first.c_str(), aSource) != PLUS_SUCCESS)
    {
      LOCAL_LOG_ERROR("Copy of field data source '" << it->first << "' failed - unable to get field data source from container!");
      continue;
    }

    aSource->DeepCopy(*it->second);
  }

  this->InternalUpdateRate = device.GetInternalUpdateRate();
  this->SetAcquisitionRate(device.GetAcquisitionRate());
}

//-----------------------------------------------------------------------------
double vtkPlusDevice::GetInternalUpdateRate() const
{
  return this->InternalUpdateRate;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::SetAcquisitionRate(double aRate)
{
  LOCAL_LOG_TRACE("vtkPlusDevice::SetAcquisitionRate(" << aRate << ")");

  if (this->AcquisitionRate == aRate)
  {
    return PLUS_SUCCESS;
  }

  this->AcquisitionRate = aRate;
  this->Modified();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
bool vtkPlusDevice::IsRecording() const
{
  return (this->Recording != 0);
}

//----------------------------------------------------------------------------
std::string vtkPlusDevice::GetDeviceId() const
{
  return this->DeviceId;
}

//----------------------------------------------------------------------------
void vtkPlusDevice::SetDeviceId(const std::string& id)
{
  this->DeviceId = id;
}

//----------------------------------------------------------------------------
unsigned long vtkPlusDevice::GetFrameNumber() const
{
  return this->FrameNumber;
}

//----------------------------------------------------------------------------
double vtkPlusDevice::GetFrameTimeStamp() const
{
  return this->FrameTimeStamp;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::WriteToolsToSequenceFile(const std::string& filename, bool useCompression /*= false*/)
{
  LOCAL_LOG_TRACE("vtkPlusDevice::WriteToolsToSequenceFile: " << filename);

  if (this->GetNumberOfTools() == 0)
  {
    LOCAL_LOG_ERROR("Failed to write tracker to metafile - there are no active tools!");
    return PLUS_FAIL;
  }

  // Get the number of items from buffers and use the lowest
  int numberOfItems(-1);
  for (DataSourceContainerConstIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    if (numberOfItems < 0 || numberOfItems > it->second->GetNumberOfItems())
    {
      numberOfItems = it->second->GetNumberOfItems();
    }
  }

  vtkSmartPointer<vtkIGSIOTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkIGSIOTrackedFrameList>::New();

  PlusStatus status = PLUS_SUCCESS;

  // Get the first source
  vtkPlusDataSource* firstActiveTool = this->Tools.begin()->second;

  for (int i = 0 ; i < numberOfItems; i++)
  {
    // Create fake image
    igsioTrackedFrame trackedFrame;
    igsioVideoFrame videoFrame;
    FrameSizeType frameSize = {1, 1, 1};
    // Don't waste space, create a greyscale image
    videoFrame.AllocateFrame(frameSize, VTK_UNSIGNED_CHAR, 1);
    trackedFrame.SetImageData(videoFrame);

    StreamBufferItem bufferItem;
    BufferItemUidType uid = firstActiveTool->GetOldestItemUidInBuffer() + i;

    if (firstActiveTool->GetStreamBufferItem(uid, &bufferItem) != ITEM_OK)
    {
      LOCAL_LOG_ERROR("Failed to get tracker buffer item with UID: " << uid);
      continue;
    }

    const double frameTimestamp = bufferItem.GetFilteredTimestamp(firstActiveTool->GetLocalTimeOffsetSec());

    // Add main source timestamp
    std::ostringstream timestampFieldValue;
    timestampFieldValue << std::fixed << frameTimestamp;
    trackedFrame.SetFrameField("Timestamp", timestampFieldValue.str());

    // Add main source unfiltered timestamp
    std::ostringstream unfilteredtimestampFieldValue;
    unfilteredtimestampFieldValue << std::fixed << bufferItem.GetUnfilteredTimestamp(firstActiveTool->GetLocalTimeOffsetSec());
    trackedFrame.SetFrameField("UnfilteredTimestamp", unfilteredtimestampFieldValue.str());

    // Add main source frameNumber
    std::ostringstream frameNumberFieldValue;
    frameNumberFieldValue << std::fixed << bufferItem.GetIndex();
    trackedFrame.SetFrameField("FrameNumber", frameNumberFieldValue.str());

    // Add transforms
    for (DataSourceContainerConstIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
    {
      StreamBufferItem toolBufferItem;
      if (it->second->GetStreamBufferItemFromTime(frameTimestamp, &toolBufferItem, vtkPlusBuffer::EXACT_TIME) != ITEM_OK)
      {
        LOCAL_LOG_ERROR("Failed to get tracker buffer item from time: " << std::fixed << frameTimestamp);
        continue;
      }

      vtkSmartPointer<vtkMatrix4x4> toolMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
      if (toolBufferItem.GetMatrix(toolMatrix) != PLUS_SUCCESS)
      {
        LOCAL_LOG_ERROR("Failed to get toolMatrix");
        return PLUS_FAIL;
      }

      igsioTransformName toolToTrackerTransform(it->second->GetId(), this->ToolReferenceFrameName);
      trackedFrame.SetFrameTransform(toolToTrackerTransform, toolMatrix);

      // Add source status
      trackedFrame.SetFrameTransformStatus(toolToTrackerTransform, toolBufferItem.GetStatus());
    }

    // Add tracked frame to the list
    // This is a debugging/test function, so the additional copying in AddTrackedFrame compared to TakeTrackedFrame is not relevant.
    trackedFrameList->AddTrackedFrame(&trackedFrame);
  }

  // Save tracked frames to metafile
  if (vtkPlusSequenceIO::Write(filename, trackedFrameList, trackedFrameList->GetImageOrientation(), useCompression) != PLUS_SUCCESS)
  {
    LOCAL_LOG_ERROR("Failed to save tracked frames to sequence metafile!");
    return PLUS_FAIL;
  }

  return status;
}

//----------------------------------------------------------------------------
void vtkPlusDevice::SetToolReferenceFrameName(const std::string& frameName)
{
  this->ToolReferenceFrameName = frameName;
}

//----------------------------------------------------------------------------
std::string vtkPlusDevice::GetToolReferenceFrameName() const
{
  return this->ToolReferenceFrameName;
}

//----------------------------------------------------------------------------
bool vtkPlusDevice::GetCorrectlyConfigured() const
{
  return this->CorrectlyConfigured;
}

//----------------------------------------------------------------------------
vtkXMLDataElement* vtkPlusDevice::FindThisDeviceElement(vtkXMLDataElement* rootXMLElement)
{
  if (rootXMLElement == NULL)
  {
    LOCAL_LOG_ERROR("Unable to find device XML element for this device.");
    return NULL;
  }

  vtkXMLDataElement* dataCollectionElement = rootXMLElement->FindNestedElementWithName("DataCollection");
  if (dataCollectionElement == NULL)
  {
    LOCAL_LOG_ERROR("Unable to find data collection element in XML tree!");
    return NULL;
  }

  vtkXMLDataElement* deviceXMLElement = NULL;
  for (int i = 0; i < dataCollectionElement->GetNumberOfNestedElements(); ++i)
  {
    deviceXMLElement = dataCollectionElement->GetNestedElement(i);

    if (deviceXMLElement->GetName() != NULL &&
        deviceXMLElement->GetAttribute("Id") != NULL &&
        std::string(deviceXMLElement->GetName()) == "Device" &&
        std::string(deviceXMLElement->GetAttribute("Id")) == this->GetDeviceId())
    {
      return deviceXMLElement;
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------
vtkXMLDataElement* vtkPlusDevice::FindOutputChannelElement(vtkXMLDataElement* rootXMLElement, const char* aChannelId)
{
  if (rootXMLElement == NULL || aChannelId == NULL)
  {
    LOCAL_LOG_ERROR("Unable to find XML element for this channel. Bad inputs.");
    return NULL;
  }

  vtkXMLDataElement* deviceXMLElement = this->FindThisDeviceElement(rootXMLElement);

  vtkXMLDataElement* outputChannelsElement = deviceXMLElement->FindNestedElementWithName("OutputChannels");
  if (outputChannelsElement != NULL)
  {
    for (int i = 0; i < outputChannelsElement->GetNumberOfNestedElements(); ++i)
    {
      vtkXMLDataElement* anXMLElement = outputChannelsElement->GetNestedElement(i);

      if (anXMLElement->GetName() != NULL &&
          anXMLElement->GetAttribute("Id") != NULL &&
          STRCASECMP(anXMLElement->GetName(), "OutputChannel") == 0 &&
          STRCASECMP(anXMLElement->GetAttribute("Id"), aChannelId) == 0)
      {
        return anXMLElement;
      }
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------
vtkXMLDataElement* vtkPlusDevice::FindInputChannelElement(vtkXMLDataElement* rootXMLElement, const char* aChannelId)
{
  if (rootXMLElement == NULL || aChannelId == NULL)
  {
    LOCAL_LOG_ERROR("Unable to find XML element for this channel. Bad inputs.");
    return NULL;
  }

  vtkXMLDataElement* deviceXMLElement = this->FindThisDeviceElement(rootXMLElement);

  vtkXMLDataElement* inputChannelsElement = deviceXMLElement->FindNestedElementWithName("InputChannels");
  if (inputChannelsElement != NULL)
  {
    for (int i = 0; i < inputChannelsElement->GetNumberOfNestedElements(); ++i)
    {
      vtkXMLDataElement* anXMLElement = inputChannelsElement->GetNestedElement(i);

      if (anXMLElement->GetName() != NULL && anXMLElement->GetAttribute("Id") != NULL &&
          STRCASECMP(anXMLElement->GetName(), "InputChannel") == 0 && STRCASECMP(anXMLElement->GetAttribute("Id"), aChannelId) == 0)
      {
        return anXMLElement;
      }
    }
  }

  return NULL;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::ReadConfiguration(vtkXMLDataElement* rootXMLElement)
{
  LOCAL_LOG_TRACE("vtkPlusDevice::ReadConfiguration");

  if (rootXMLElement == NULL)
  {
    LOCAL_LOG_ERROR("Unable to find device XML element for this device.");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* deviceXMLElement = this->FindThisDeviceElement(rootXMLElement);

  if (deviceXMLElement == NULL)
  {
    LOCAL_LOG_ERROR("Unable to find device XML element for device");
    return PLUS_FAIL;
  }

  // Continue with device configuration
  const char* referenceName = deviceXMLElement->GetAttribute("ToolReferenceFrame");
  if (referenceName != NULL)
  {
    this->SetToolReferenceFrameName(referenceName);
  }
  else
  {
    this->SetToolReferenceFrameName(this->GetDeviceId());
    if (this->IsTracker())
    {
      LOCAL_LOG_WARNING("ToolReferenceFrame is undefined. Default \"" << (this->GetToolReferenceFrameName().empty() ? this->GetToolReferenceFrameName() : "(undefined)") << "\" will be used.");
    }
  }

  if (deviceXMLElement->GetAttribute("MissingInputGracePeriodSec") != NULL)
  {
    deviceXMLElement->GetScalarAttribute("MissingInputGracePeriodSec", this->MissingInputGracePeriodSec);
  }

  vtkXMLDataElement* dataSourcesElement = deviceXMLElement->FindNestedElementWithName("DataSources");
  if (dataSourcesElement != NULL)
  {
    // Read source configurations
    for (int source = 0; source < dataSourcesElement->GetNumberOfNestedElements(); source++)
    {
      vtkXMLDataElement* dataSourceElement = dataSourcesElement->GetNestedElement(source);

      if (STRCASECMP(dataSourceElement->GetName(), "DataSource") != 0)
      {
        // if this is not a Tool element, skip it
        continue;
      }

      std::string sourceId = dataSourceElement->GetAttribute("Id") != NULL ? dataSourceElement->GetAttribute("Id") : "";
      if (sourceId.empty())
      {
        LOG_ERROR("Unable to find attribute \"Id\"! \"Id\" attribute is mandatory in source definition. Skipping.");
        continue;
      }

      if (this->IsTracker() && sourceId == this->ToolReferenceFrameName)
      {
        LOG_ERROR("Data source \"" << this->ToolReferenceFrameName << "\" cannot have same name as the ToolReferenceFrameName. Skipping.");
        continue;
      }

      if (this->EnsureUniqueDataSourceId(sourceId) == PLUS_FAIL)
      {
        LOG_ERROR("Data source name \"" << sourceId << "\" is not unique. Skipping.");
        continue;
      }

      vtkSmartPointer<vtkPlusDataSource> aDataSource = vtkSmartPointer<vtkPlusDataSource>::New();
      bool isEqual(false);
      if (igsioCommon::XML::SafeCheckAttributeValueInsensitive(*dataSourceElement, "Type", vtkPlusDataSource::DATA_SOURCE_TYPE_TOOL_TAG, isEqual) == PLUS_SUCCESS && isEqual)
      {
        aDataSource->SetReferenceCoordinateFrameName(this->ToolReferenceFrameName);

        if (aDataSource->ReadConfiguration(dataSourceElement, this->RequirePortNameInDeviceSetConfiguration, this->RequireImageOrientationInConfiguration, this->GetDeviceId()) != PLUS_SUCCESS)
        {
          LOCAL_LOG_ERROR("Unable to add tool to tracker - failed to read tool configuration");
          continue;
        }

        if (this->AddTool(aDataSource) != PLUS_SUCCESS)
        {
          LOCAL_LOG_ERROR("Failed to add tool '" << (!aDataSource->GetId().empty() ? aDataSource->GetId() : "(unspecified)") << "' to device on port " << (!aDataSource->GetPortName().empty() ? aDataSource->GetPortName() : "(unspecified)"));
        }
      }
      else if (igsioCommon::XML::SafeCheckAttributeValueInsensitive(*dataSourceElement, "Type", vtkPlusDataSource::DATA_SOURCE_TYPE_FIELDDATA_TAG, isEqual) == PLUS_SUCCESS && isEqual)
      {
        if (aDataSource->ReadConfiguration(dataSourceElement, this->RequirePortNameInDeviceSetConfiguration, this->RequireImageOrientationInConfiguration, this->GetDeviceId()) != PLUS_SUCCESS)
        {
          LOCAL_LOG_ERROR("Unable to add field data source to device - failed to read field data source configuration");
          continue;
        }

        if (this->AddFieldDataSource(aDataSource) != PLUS_SUCCESS)
        {
          LOCAL_LOG_ERROR("Failed to add field data source '" << (!aDataSource->GetId().empty() ? aDataSource->GetId() : "(unspecified)") << "' to device.");
        }
      }
      else if (igsioCommon::XML::SafeCheckAttributeValueInsensitive(*dataSourceElement, "Type", vtkPlusDataSource::DATA_SOURCE_TYPE_VIDEO_TAG, isEqual) == PLUS_SUCCESS && isEqual)
      {
        aDataSource->ReadConfiguration(dataSourceElement, this->RequirePortNameInDeviceSetConfiguration, this->RequireImageOrientationInConfiguration, this->GetDeviceId());

        if (this->AddVideoSource(aDataSource) != PLUS_SUCCESS)
        {
          LOCAL_LOG_ERROR("Failed to add video source '" << (!aDataSource->GetId().empty() ? aDataSource->GetId() : "(unspecified)") << "' to device.");
        }
      }
    }
  }

  // Continue with device configuration
  double acquisitionRate = 0;
  if (deviceXMLElement->GetScalarAttribute("AcquisitionRate", acquisitionRate))
  {
    this->SetAcquisitionRate(acquisitionRate);
  }
  else
  {
    LOCAL_LOG_DEBUG("Unable to find acquisition rate in device element when it is required, using default " << this->GetAcquisitionRate());
  }

  vtkXMLDataElement* outputChannelsElement = deviceXMLElement->FindNestedElementWithName("OutputChannels");
  if (outputChannelsElement != NULL)
  {
    // Now that we have the tools and images, we can create the output streams and connect things as necessary
    for (int channel = 0; channel < outputChannelsElement->GetNumberOfNestedElements(); channel++)
    {
      vtkXMLDataElement* channelElement = outputChannelsElement->GetNestedElement(channel);
      if (STRCASECMP(channelElement->GetName(), "OutputChannel") != 0)
      {
        // if this is not a channel element, skip it
        continue;
      }

      if (channelElement->GetAttribute("Id") == NULL)
      {
        LOCAL_LOG_ERROR("No channel Id present. Skipping channel configuration.");
        continue;
      }

      bool skip(false);
      for (ChannelContainerIterator it = this->OutputChannels.begin(); it != this->OutputChannels.end(); ++it)
      {
        if (STRCASECMP((*it)->GetChannelId(), channelElement->GetAttribute("Id")) == 0)
        {
          LOCAL_LOG_ERROR("Channel with duplicate channel Id \'" << (*it)->GetChannelId() << "\'. Skipping channel configuration.");
          skip = true;
          break;
        }
      }
      if (skip)
      {
        continue;
      }

      vtkSmartPointer<vtkPlusChannel> aChannel = vtkSmartPointer<vtkPlusChannel>::New();
      aChannel->SetOwnerDevice(this);
      aChannel->ReadConfiguration(channelElement, this->RequireImageOrientationInConfiguration);
      AddOutputChannel(aChannel);
    }
  }

  double localTimeOffsetSec = 0;
  if (deviceXMLElement->GetScalarAttribute("LocalTimeOffsetSec", localTimeOffsetSec))
  {
    // DeviceId is never null, if it is, there is memory stomping occurring elsewhere
    LOCAL_LOG_INFO("Local time offset: " << 1000 * localTimeOffsetSec << "ms");
    this->SetLocalTimeOffsetSec(localTimeOffsetSec);
  }
  else
  {
    LOCAL_LOG_DEBUG("Local time offset was not defined in device configuration");
  }

  // Parameter reading
  XML_FIND_NESTED_ELEMENT_OPTIONAL(parametersElem, deviceXMLElement, vtkPlusDevice::PARAMETERS_XML_ELEMENT_TAG.c_str());
  if (parametersElem)
  {
    for (int i = 0; i < parametersElem->GetNumberOfNestedElements(); ++i)
    {
      vtkXMLDataElement* element = parametersElem->GetNestedElement(i);
      if (!igsioCommon::IsEqualInsensitive(element->GetName(), PARAMETER_XML_ELEMENT_TAG))
      {
        // Not a Parameter tag, skip
        continue;
      }

      std::string name = element->GetAttribute("name") ? element->GetAttribute("name") : "";
      std::string value = element->GetAttribute("value") ? element->GetAttribute("value") : "";
      if (name.empty())
      {
        continue;
      }

      this->Parameters[name] = value;
    }

    return PLUS_SUCCESS;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::WriteConfiguration(vtkXMLDataElement* config)
{
  if (config == NULL)
  {
    LOCAL_LOG_ERROR("Unable to write configuration: xml data element is NULL!");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* deviceDataElement = this->FindThisDeviceElement(config);

  if (deviceDataElement == NULL)
  {
    LOCAL_LOG_ERROR("Unable to write configuration: xml data element is NULL!");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* dataSourcesElement = deviceDataElement->FindNestedElementWithName("DataSources");
  if (dataSourcesElement != NULL)
  {
    for (int i = 0; i < dataSourcesElement->GetNumberOfNestedElements(); i++)
    {
      vtkXMLDataElement* dataSourceElement = dataSourcesElement->GetNestedElement(i);
      if (STRCASECMP(dataSourceElement->GetName(), "DataSource") != 0)
      {
        // if this is not a data source element, skip it
        continue;
      }
      vtkPlusDataSource* aDataSource = NULL;
      bool isEqual(false);
      if (igsioCommon::XML::SafeCheckAttributeValueInsensitive(*dataSourceElement, "Type", vtkPlusDataSource::DATA_SOURCE_TYPE_TOOL_TAG, isEqual) == PLUS_SUCCESS && isEqual)
      {
        igsioTransformName toolId(dataSourceElement->GetAttribute("Id"), this->GetToolReferenceFrameName());
        if (dataSourceElement->GetAttribute("Id") == NULL || this->GetTool(toolId.GetTransformName(), aDataSource) != PLUS_SUCCESS)
        {
          LOCAL_LOG_ERROR("Unable to retrieve tool when saving config.");
          return PLUS_FAIL;
        }
        aDataSource->WriteConfiguration(dataSourceElement);
      }
      else if (igsioCommon::XML::SafeCheckAttributeValueInsensitive(*dataSourceElement, "Type", vtkPlusDataSource::DATA_SOURCE_TYPE_FIELDDATA_TAG, isEqual) == PLUS_SUCCESS && isEqual)
      {
        if (dataSourceElement->GetAttribute("Id") == NULL || this->GetFieldDataSource(dataSourceElement->GetAttribute("Id"), aDataSource) != PLUS_SUCCESS)
        {
          LOCAL_LOG_ERROR("Unable to retrieve field data source when saving config.");
          return PLUS_FAIL;
        }
        aDataSource->WriteConfiguration(dataSourceElement);
      }
      else if (igsioCommon::XML::SafeCheckAttributeValueInsensitive(*dataSourceElement, "Type", vtkPlusDataSource::DATA_SOURCE_TYPE_VIDEO_TAG, isEqual) == PLUS_SUCCESS && isEqual)
      {
        if (dataSourceElement->GetAttribute("Id") == NULL || this->GetVideoSource(dataSourceElement->GetAttribute("Id"), aDataSource) != PLUS_SUCCESS)
        {
          LOCAL_LOG_ERROR("Unable to retrieve video source when saving config.");
          return PLUS_FAIL;
        }
        aDataSource->WriteConfiguration(dataSourceElement);
      }
    }
  }

  this->InternalWriteOutputChannels(config);
  this->InternalWriteInputChannels(config);

  if (this->GetLocalTimeOffsetSec() != 0.0)
  {
    deviceDataElement->SetDoubleAttribute("LocalTimeOffsetSec", this->GetLocalTimeOffsetSec());
  }

  // Parameters writing
  XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING(parameterList, deviceDataElement, PARAMETERS_XML_ELEMENT_TAG.c_str());

  // Clear the list before writing new elements
  parameterList->RemoveAllNestedElements();

  for (std::map<std::string, std::string>::iterator it = this->Parameters.begin(); it != this->Parameters.end(); ++it)
  {
    vtkSmartPointer<vtkXMLDataElement> parameter = vtkSmartPointer<vtkXMLDataElement>::New();
    parameter->SetName(PARAMETER_XML_ELEMENT_TAG.c_str());
    parameter->SetAttribute("name", it->first.c_str());
    parameter->SetAttribute("value", it->second.c_str());

    parameterList->AddNestedElement(parameter);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::Connect()
{
  LOCAL_LOG_DEBUG("vtkPlusDevice::Connect: " << (!this->DeviceId.empty() ? this->DeviceId : "undefined"));

  if (this->Connected)
  {
    LOCAL_LOG_DEBUG("Already connected to the data source");
    return PLUS_SUCCESS;
  }

  // We will report unknown tools after each Connect
  this->ReportedUnknownTools.clear();

  if (this->InternalConnect() != PLUS_SUCCESS)
  {
    LOCAL_LOG_ERROR("Cannot connect to device, ConnectInternal failed");
    return PLUS_FAIL;
  }

  this->Connected = 1;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::Disconnect()
{
  LOCAL_LOG_DEBUG("vtkPlusDevice::Disconnect");

  if (!this->Connected)
  {
    LOCAL_LOG_DEBUG("Data source is already disconnected");
    return PLUS_SUCCESS;
  }
  if (this->Recording)
  {
    this->StopRecording();
  }

  this->Connected = 0;

  InternalDisconnect();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// Set the source to acquire data continuously.
// You should override this as appropriate for your device.
PlusStatus vtkPlusDevice::StartRecording()
{
  LOCAL_LOG_DEBUG("vtkPlusDevice::StartRecording");

  if (this->Recording)
  {
    LOCAL_LOG_DEBUG("Recording is already active");
    return PLUS_SUCCESS;
  }

  if (!this->Connected)
  {
    if (this->Connect() != PLUS_SUCCESS)
    {
      LOCAL_LOG_ERROR("Cannot start recording, connection failed");
      return PLUS_FAIL;
    }
  }

  if (this->InternalStartRecording() != PLUS_SUCCESS)
  {
    LOCAL_LOG_ERROR("Cannot start recording, internal StartRecording failed");
    return PLUS_FAIL;
  }

  this->RecordingStartTime = vtkIGSIOAccurateTimer::GetSystemTime();
  this->Recording = 1;

  if (this->StartThreadForInternalUpdates)
  {
    this->ThreadId =
      this->Threader->SpawnThread((vtkThreadFunctionType)\
                                  &vtkDataCaptureThread, this);
  }

  this->Modified();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// Stop continuous acquisition.  You will have to override this
// if your class overrides Record()
PlusStatus vtkPlusDevice::StopRecording()
{
  LOCAL_LOG_DEBUG("vtkPlusDevice::StopRecording");

  if (!this->Recording)
  {
    LOCAL_LOG_DEBUG("Recording is already inactive");
    return PLUS_SUCCESS;
  }

  this->ThreadId = -1;
  this->Recording = 0;

  if (this->GetStartThreadForInternalUpdates())
  {
    LOCAL_LOG_DEBUG("Wait for internal update thread to terminate");
    // Let's give a chance to the thread to stop before we kill the connection
    while (this->ThreadAlive)
    {
      vtkIGSIOAccurateTimer::Delay(0.1);
    }
    this->ThreadId = -1;
    LOCAL_LOG_DEBUG("Internal update thread terminated");
  }

  if (this->InternalStopRecording() != PLUS_SUCCESS)
  {
    LOCAL_LOG_ERROR("Failed to stop tracking thread!");
    return PLUS_FAIL;
  }

  this->Modified();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// this function runs in an alternate thread to asynchronously acquire data
void* vtkPlusDevice::vtkDataCaptureThread(vtkMultiThreader::ThreadInfo* data)
{
  vtkPlusDevice* self = (vtkPlusDevice*)(data->UserData);

  double rate = self->GetAcquisitionRate();
  double currtime[FRAME_RATE_AVERAGING] = {0};
  unsigned long updatecount = 0;
  self->ThreadAlive = true;

  while (self->IsRecording() && self->GetCorrectlyConfigured())
  {
    double newtime = vtkIGSIOAccurateTimer::GetSystemTime();
    // get current tracking rate over last few updates
    double difftime = newtime - currtime[updatecount % FRAME_RATE_AVERAGING];
    currtime[updatecount % FRAME_RATE_AVERAGING] = newtime;
    if (updatecount > FRAME_RATE_AVERAGING && difftime != 0)
    {
      self->InternalUpdateRate = (FRAME_RATE_AVERAGING / difftime);
    }

    {
      // Lock before update
      igsioLockGuard<vtkIGSIORecursiveCriticalSection> updateMutexGuardedLock(self->UpdateMutex);
      if (!self->Recording)
      {
        // recording has been stopped
        break;
      }
      self->InternalUpdate();
      self->UpdateTime.Modified();
    }

    double delay = (newtime + 1.0 / rate - vtkIGSIOAccurateTimer::GetSystemTime());
    if (delay > 0)
    {
      vtkIGSIOAccurateTimer::Delay(delay);
    }

    updatecount++;
  }

  self->ThreadAlive = false;
  return NULL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::InternalConnect()
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::InternalDisconnect()
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::InternalStartRecording()
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::InternalStopRecording()
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetBufferSize(vtkPlusChannel& aChannel, int& outVal, const std::string& aSourceId)
{
  LOCAL_LOG_TRACE("vtkPlusDeviceg::GetBufferSize");

  if (aSourceId.empty())
  {
    vtkPlusDataSource* aSource(NULL);
    if (aChannel.GetVideoSource(aSource) != PLUS_SUCCESS)
    {
      LOCAL_LOG_ERROR("Unable to retrieve the buffer size of the video source in channel: " << aChannel.GetChannelId() << ".");
      return PLUS_FAIL;
    }

    outVal = aSource->GetBufferSize();
    return PLUS_SUCCESS;
  }

  for (DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    if (it->second->GetId() == aSourceId)
    {
      outVal = it->second->GetBufferSize();
      return PLUS_SUCCESS;
    }
  }

  for (DataSourceContainerConstIterator it = this->GetFieldDataSourcessIteratorBegin(); it != this->GetFieldDataSourcessIteratorEnd(); ++it)
  {
    if (it->second->GetId() == aSourceId)
    {
      outVal = it->second->GetBufferSize();
      return PLUS_SUCCESS;
    }
  }

  LOCAL_LOG_ERROR("Unable to find tool " << aSourceId << " when requesting its buffer size.");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
// set or change the circular buffer size
// you will have to override this if you want the buffers
// to be device-specific (i.e. something other than vtkDataArray)
PlusStatus vtkPlusDevice::SetBufferSize(vtkPlusChannel& aChannel, int FrameBufferSize, const std::string& aSourceId)
{
  LOCAL_LOG_TRACE("vtkPlusDevice::SetBufferSize(" << FrameBufferSize << ")");

  if (FrameBufferSize < 0)
  {
    LOCAL_LOG_ERROR("vtkPlusDevice::SetBufferSize: There must be at least one framebuffer");
    return PLUS_FAIL;
  }

  if (aSourceId.empty())
  {
    vtkPlusDataSource* aSource(NULL);
    if (aChannel.GetVideoSource(aSource) != PLUS_SUCCESS)
    {
      LOCAL_LOG_ERROR("Unable to retrieve the video source.");
      return PLUS_FAIL;
    }

    // update the buffer size
    if (aSource->SetBufferSize(FrameBufferSize) != PLUS_SUCCESS)
    {
      LOCAL_LOG_ERROR("Failed to set buffer size!");
      return PLUS_FAIL;
    }
    return PLUS_SUCCESS;
  }

  for (DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    if (it->second->GetId() == aSourceId)
    {
      it->second->SetBufferSize(FrameBufferSize);
    }
  }

  for (DataSourceContainerConstIterator it = this->GetFieldDataSourcessIteratorBegin(); it != this->GetFieldDataSourcessIteratorEnd(); ++it)
  {
    if (it->second->GetId() == aSourceId)
    {
      it->second->SetBufferSize(FrameBufferSize);
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusDevice::SetStartTime(double startTime)
{
  for (DataSourceContainerConstIterator it = this->GetVideoSourceIteratorBegin(); it != this->GetVideoSourceIteratorEnd(); ++it)
  {
    it->second->SetStartTime(startTime);
  }

  for (DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    it->second->SetStartTime(startTime);
  }

  for (DataSourceContainerConstIterator it = this->GetFieldDataSourcessIteratorBegin(); it != this->GetFieldDataSourcessIteratorEnd(); ++it)
  {
    it->second->SetStartTime(startTime);
  }
}

//----------------------------------------------------------------------------
double vtkPlusDevice::GetStartTime()
{
  // TODO : should this return the buffer start time or should the buffer start time
  //        simply be included in the average
  double sumStartTime = 0.0;
  double numberOfBuffers(0);
  for (DataSourceContainerConstIterator it = this->GetVideoSourceIteratorBegin(); it != this->GetVideoSourceIteratorEnd(); ++it)
  {
    sumStartTime += it->second->GetStartTime();
    numberOfBuffers++;
  }

  for (DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    sumStartTime += it->second->GetStartTime();
    numberOfBuffers++;
  }

  for (DataSourceContainerConstIterator it = this->GetFieldDataSourcessIteratorBegin(); it != this->GetFieldDataSourcessIteratorEnd(); ++it)
  {
    sumStartTime += it->second->GetStartTime();
    numberOfBuffers++;
  }

  return sumStartTime / numberOfBuffers;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::Probe()
{
  igsioLockGuard<vtkIGSIORecursiveCriticalSection> updateMutexGuardedLock(this->UpdateMutex);

  if (this->InternalStartRecording() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  if (this->InternalStopRecording() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkPlusDevice::ClearAllBuffers()
{
  for (DataSourceContainerConstIterator it = this->GetVideoSourceIteratorBegin(); it != this->GetVideoSourceIteratorEnd(); ++it)
  {
    it->second->Clear();
  }
  for (DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    it->second->Clear();
  }
  for (DataSourceContainerConstIterator it = this->GetFieldDataSourcessIteratorBegin(); it != this->GetFieldDataSourcessIteratorEnd(); ++it)
  {
    it->second->Clear();
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::ForceUpdate()
{
  LOCAL_LOG_TRACE("vtkPlusDevice::ForceUpdate");

  if (!this->Connected)
  {
    if (this->Connect() != PLUS_SUCCESS)
    {
      LOCAL_LOG_ERROR("Cannot acquire a single frame, connection failed");
      return PLUS_FAIL;
    }
  }

  {
    igsioLockGuard<vtkIGSIORecursiveCriticalSection> updateMutexGuardedLock(this->UpdateMutex);
    this->InternalUpdate();
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::ToolTimeStampedUpdateWithoutFiltering(const std::string& aToolSourceId, vtkMatrix4x4* matrix, ToolStatus status, double unfilteredtimestamp, double filteredtimestamp, const igsioFieldMapType* customFields /* = NULL */)
{
  if (aToolSourceId.empty())
  {
    LOCAL_LOG_ERROR("Failed to update tool - tool source ID is empty!");
    return PLUS_FAIL;
  }

  vtkPlusDataSource* tool = NULL;
  if (this->GetTool(aToolSourceId, tool) != PLUS_SUCCESS)
  {
    if (this->ReportedUnknownTools.find(aToolSourceId) == this->ReportedUnknownTools.end())
    {
      // We have not reported yet that this tool is unknown
      LOCAL_LOG_ERROR("Failed to update tool - unable to find tool: " << aToolSourceId);
      this->ReportedUnknownTools.insert(std::string(aToolSourceId));
    }
    return PLUS_FAIL;
  }

  // This function is for devices has no frame numbering, just auto increment tool frame number if new frame received
  unsigned long frameNumber = tool->GetFrameNumber() + 1 ;
  PlusStatus bufferStatus = tool->AddTimeStampedItem(matrix, status, frameNumber, unfilteredtimestamp, filteredtimestamp, customFields);
  tool->SetFrameNumber(frameNumber);

  return bufferStatus;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::AddVideoItemToVideoSources(const std::vector<vtkPlusDataSource*>& videoSources, const igsioVideoFrame& frame, long frameNumber, double unfilteredTimestamp/*=UNDEFINED_TIMESTAMP*/, double filteredTimestamp/*=UNDEFINED_TIMESTAMP*/, const igsioFieldMapType* customFields /*= NULL*/)
{
  PlusStatus result(PLUS_SUCCESS);
  for (std::vector<vtkPlusDataSource*>::const_iterator it = videoSources.begin(); it != videoSources.end(); ++it)
  {
    vtkPlusDataSource* source = *it;
    if (source->AddItem(&frame, frameNumber, unfilteredTimestamp, filteredTimestamp, customFields) != PLUS_SUCCESS)
    {
      result = PLUS_FAIL;
    }
  }
  return result;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::AddVideoItemToVideoSources(const std::vector<vtkPlusDataSource*>& videoSources, void* imageDataPtr, US_IMAGE_ORIENTATION usImageOrientation, const FrameSizeType& frameSizeInPx, igsioCommon::VTKScalarPixelType pixelType, unsigned int numberOfScalarComponents, US_IMAGE_TYPE imageType, int numberOfBytesToSkip, long frameNumber, double unfilteredTimestamp /*= UNDEFINED_TIMESTAMP*/, double filteredTimestamp /*= UNDEFINED_TIMESTAMP*/, const igsioFieldMapType* customFields /*= NULL*/)
{
  PlusStatus result(PLUS_SUCCESS);
  for (std::vector<vtkPlusDataSource*>::const_iterator it = videoSources.begin(); it != videoSources.end(); ++it)
  {
    vtkPlusDataSource* source = *it;
    if (source->AddItem(imageDataPtr, usImageOrientation, frameSizeInPx, pixelType, numberOfScalarComponents, imageType, numberOfBytesToSkip, frameNumber, unfilteredTimestamp, filteredTimestamp, customFields) != PLUS_SUCCESS)
    {
      result = PLUS_FAIL;
    }
  }
  return result;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::ToolTimeStampedUpdate(const std::string& aToolSourceId, vtkMatrix4x4* matrix, ToolStatus status, unsigned long frameNumber, double unfilteredtimestamp, const igsioFieldMapType* customFields/*= NULL*/)
{
  if (aToolSourceId.empty())
  {
    LOCAL_LOG_ERROR("Failed to update tool - tool source ID is empty!");
    return PLUS_FAIL;
  }

  vtkPlusDataSource* tool = NULL;
  if (this->GetTool(aToolSourceId, tool) != PLUS_SUCCESS)
  {
    if (this->ReportedUnknownTools.find(aToolSourceId) == this->ReportedUnknownTools.end())
    {
      // We have not reported yet that this tool is unknown
      LOCAL_LOG_ERROR("Failed to update tool - unable to find tool: " << aToolSourceId);
      this->ReportedUnknownTools.insert(std::string(aToolSourceId));
    }
    return PLUS_FAIL;
  }

  PlusStatus bufferStatus = tool->AddTimeStampedItem(matrix, status, frameNumber, unfilteredtimestamp, UNDEFINED_TIMESTAMP, customFields);
  tool->SetFrameNumber(frameNumber);

  return bufferStatus;
}

//----------------------------------------------------------------------------
// This method returns the largest data that can be generated.
int vtkPlusDevice::RequestInformation(vtkInformation* vtkNotUsed(request), vtkInformationVector** vtkNotUsed(inputVector), vtkInformationVector* outputVector)
{
  LOCAL_LOG_TRACE("vtkPlusDevice::RequestInformation");

  if (!this->Connected)
  {
    this->Connect();
  }

  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // Find a video source and set extent
  vtkPlusDataSource* aSource(NULL);
  for (ChannelContainerIterator it = this->OutputChannels.begin(); it != this->OutputChannels.end(); ++it)
  {
    if ((*it)->GetVideoSource(aSource) == PLUS_SUCCESS)
    {
      break;
    }
  }
  if (aSource == NULL)
  {
    return 0;
  }

  FrameSizeType frameSize = aSource->GetOutputFrameSize();
  // TODO : determine another mechanism to see if device has data yet or not

  int extent[6] = { 0, static_cast<int>(frameSize[0]) - 1, 0, static_cast<int>(frameSize[1]) - 1, 0, static_cast<int>(frameSize[2]) - 1 };
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent, 6);

  // Set the origin and spacing. The video source provides raw pixel output,
  // therefore the spacing is (1,1,1) and the origin is (0,0,0)
  double spacing[3] = { 1, 1, 1 };
  outInfo->Set(vtkDataObject::SPACING(), spacing, 3);
  double origin[3] = { 0, 0, 0 };
  outInfo->Set(vtkDataObject::ORIGIN(), origin, 3);

  // set default data type - unsigned char and number of components 1
  vtkDataObject::SetPointDataActiveScalarInfo(outInfo, aSource->GetPixelType(), 1);

  return 1;
}

//----------------------------------------------------------------------------
// The Execute method is fairly complex, so I would not recommend overriding
// it unless you have to.  Override the UnpackRasterLine() method instead.
int vtkPlusDevice::RequestData(vtkInformation* vtkNotUsed(request), vtkInformationVector** vtkNotUsed(inputVector), vtkInformationVector* vtkNotUsed(outputVector))
{
  LOCAL_LOG_TRACE("vtkPlusDevice::RequestData");

  // Find a video source and set extent
  vtkPlusDataSource* aSource(NULL);
  for (ChannelContainerIterator it = this->OutputChannels.begin(); it != this->OutputChannels.end(); ++it)
  {
    if ((*it)->GetVideoSource(aSource) == PLUS_SUCCESS)
    {
      break;
    }
  }
  if (aSource == NULL)
  {
    return 1;
  }

  if (aSource->GetNumberOfItems() < 1)
  {
    LOCAL_LOG_DEBUG("Cannot request data from video source, the video buffer is empty or does not exist!");
    vtkImageData* data = vtkImageData::SafeDownCast(this->GetOutputDataObject(0));
    FrameSizeType frameSize = aSource->GetOutputFrameSize();
    data->SetExtent(0, frameSize[0] - 1, 0, frameSize[1] - 1, 0, frameSize[2] - 1);
    data->AllocateScalars(aSource->GetPixelType(), aSource->GetNumberOfScalarComponents());

    return 1;
  }

  if (this->UpdateWithDesiredTimestamp && this->DesiredTimestamp != -1)
  {
    ItemStatus itemStatus = aSource->GetStreamBufferItemFromTime(this->DesiredTimestamp, this->CurrentStreamBufferItem, vtkPlusBuffer::EXACT_TIME);
    if (itemStatus != ITEM_OK)
    {
      LOCAL_LOG_ERROR("Unable to copy video data to the requested output!");
      return 1;
    }
  }
  else
  {
    // get the most recent frame if we are not updating with the desired timestamp
    ItemStatus itemStatus = aSource->GetLatestStreamBufferItem(this->CurrentStreamBufferItem);
    if (itemStatus != ITEM_OK)
    {
      LOCAL_LOG_ERROR("Unable to copy video data to the requested output!");
      return 1;
    }
  }

  this->FrameTimeStamp = this->CurrentStreamBufferItem->GetTimestamp(aSource->GetLocalTimeOffsetSec());
  this->TimestampClosestToDesired = this->CurrentStreamBufferItem->GetTimestamp(aSource->GetLocalTimeOffsetSec());

  void* sourcePtr = this->CurrentStreamBufferItem->GetFrame().GetScalarPointer();
  int bytesToCopy = this->CurrentStreamBufferItem->GetFrame().GetFrameSizeInBytes();

  // The whole image buffer is copied, regardless of the UPDATE_EXTENT value to make the copying implementation simpler
  // For a more efficient implementation, we should only update the requested part of the image.
  vtkImageData* data = vtkImageData::SafeDownCast(this->GetOutputDataObject(0));
  FrameSizeType frameSize = { 0, 0, 0 };
  this->CurrentStreamBufferItem->GetFrame().GetFrameSize(frameSize);
  data->SetExtent(0, frameSize[0] - 1, 0, frameSize[1] - 1, 0, frameSize[2] - 1);
  data->AllocateScalars(aSource->GetPixelType(), aSource->GetNumberOfScalarComponents());

  unsigned char* outPtr = (unsigned char*)data->GetScalarPointer();
  memcpy(outPtr, sourcePtr, bytesToCopy);

  return 1;
}

//----------------------------------------------------------------------------
int vtkPlusDevice::GetConnected() const
{
  return this->Connected;
}

//----------------------------------------------------------------------------
bool vtkPlusDevice::IsConnected() const
{
  return this->Connected == 1;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::SetInputFrameSize(vtkPlusDataSource& aSource, unsigned int x, unsigned int y, unsigned int z)
{
  FrameSizeType frameSize = aSource.GetInputFrameSize();
  if (x == frameSize[0] && y == frameSize[1] && z == frameSize[2])
  {
    return PLUS_SUCCESS;
  }

  if (x != 0 && y != 0 && z == 0)
  {
    LOCAL_LOG_WARNING("Single slice images should have a dimension of z=1");
    z = 1;
  }

  if (x < 1 || y < 1 || z < 1)
  {
    LOCAL_LOG_ERROR("SetInputFrameSize: Illegal frame size " << x << "x" << y << "x" << z);
    return PLUS_FAIL;
  }

  aSource.SetInputFrameSize(x, y, z);
  aSource.Modified();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetOutputFrameSize(vtkPlusChannel& aChannel, unsigned int& x, unsigned int& y, unsigned int& z) const
{
  LOCAL_LOG_TRACE("vtkPlusDevice::GetOutputFrameSize");

  FrameSizeType dim = {0, 0, 0};
  if (this->GetOutputFrameSize(aChannel, dim) != PLUS_SUCCESS)
  {
    LOCAL_LOG_ERROR("Unable to get frame size from the device.");
    return PLUS_FAIL;
  }
  x = dim[0];
  y = dim[1];
  z = dim[2];

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetOutputFrameSize(vtkPlusChannel& aChannel, FrameSizeType& dim) const
{
  LOCAL_LOG_TRACE("vtkPlusDevice::GetOutputFrameSize");

  vtkPlusDataSource* aSource(NULL);
  if (aChannel.GetVideoSource(aSource) != PLUS_SUCCESS)
  {
    LOCAL_LOG_ERROR("Unable to retrieve the video source.");
    return PLUS_FAIL;
  }

  dim = aSource->GetOutputFrameSize();
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetInputFrameSize(vtkPlusChannel& aChannel, unsigned int& x, unsigned int& y, unsigned int& z) const
{
  LOCAL_LOG_TRACE("vtkPlusDevice::GetInputFrameSize");

  FrameSizeType dim = {0, 0, 0};
  if (this->GetInputFrameSize(aChannel, dim) != PLUS_SUCCESS)
  {
    LOCAL_LOG_ERROR("Unable to get frame size from the device.");
    return PLUS_FAIL;
  }
  x = dim[0];
  y = dim[1];
  z = dim[2];

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetInputFrameSize(vtkPlusChannel& aChannel, FrameSizeType& dim) const
{
  LOCAL_LOG_TRACE("vtkPlusDevice::GetInputFrameSize");

  vtkPlusDataSource* aSource(NULL);
  if (aChannel.GetVideoSource(aSource) != PLUS_SUCCESS)
  {
    LOCAL_LOG_ERROR("Unable to retrieve the video source.");
    return PLUS_FAIL;
  }

  dim = aSource->GetInputFrameSize();
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::SetPixelType(vtkPlusChannel& aChannel, igsioCommon::VTKScalarPixelType pixelType)
{
  LOCAL_LOG_TRACE("vtkPlusDevice::SetPixelType");

  vtkPlusDataSource* aSource(NULL);
  if (aChannel.GetVideoSource(aSource) != PLUS_SUCCESS)
  {
    LOCAL_LOG_ERROR("Unable to retrieve the video source.");
    return PLUS_FAIL;
  }

  return aSource->SetPixelType(pixelType);
}

//----------------------------------------------------------------------------
igsioCommon::VTKScalarPixelType vtkPlusDevice::GetPixelType(vtkPlusChannel& aChannel)
{
  LOCAL_LOG_TRACE("vtkPlusDevice::GetPixelType");

  vtkPlusDataSource* aSource(NULL);
  if (aChannel.GetVideoSource(aSource) != PLUS_SUCCESS)
  {
    LOCAL_LOG_ERROR("Unable to retrieve the video source.");
    return VTK_VOID;
  }

  return aSource->GetPixelType();
}

//----------------------------------------------------------------------------
US_IMAGE_TYPE vtkPlusDevice::GetImageType(vtkPlusChannel& aChannel)
{
  vtkPlusDataSource* aSource(NULL);
  if (aChannel.GetVideoSource(aSource) != PLUS_SUCCESS)
  {
    LOCAL_LOG_ERROR("Unable to retrieve the video source.");
    return US_IMG_TYPE_XX;
  }

  return aSource->GetImageType();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::SetImageType(vtkPlusChannel& aChannel, US_IMAGE_TYPE imageType)
{
  vtkPlusDataSource* aSource(NULL);
  if (aChannel.GetVideoSource(aSource) != PLUS_SUCCESS)
  {
    LOCAL_LOG_ERROR("Unable to retrieve the video source.");
    return PLUS_FAIL;
  }
  return aSource->SetImageType(imageType);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetFirstOutputChannel(vtkPlusChannel*& aChannel)
{
  if (this->OutputChannels.size() == 0)
  {
    LOCAL_LOG_ERROR("Failed to get first output channel - there are no output channels!");
    return PLUS_FAIL;
  }

  aChannel = (* this->OutputChannels.begin());

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetOutputChannelByName(vtkPlusChannel*& aChannel, const char* aChannelId)
{
  if (aChannelId == NULL)
  {
    LOCAL_LOG_ERROR("Null channel name sent to GetOutputChannelByName.");
    return PLUS_FAIL;
  }

  for (ChannelContainerIterator it = this->OutputChannels.begin(); it != this->OutputChannels.end(); ++it)
  {
    vtkPlusChannel* channel = (*it);
    if (STRCASECMP(channel->GetChannelId(), aChannelId) == 0)
    {
      aChannel = channel;
      return PLUS_SUCCESS;
    }
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetOutputChannelByName(vtkPlusChannel*& aChannel, const std::string& aChannelId)
{
  return GetOutputChannelByName(aChannel, aChannelId.c_str());
}

//----------------------------------------------------------------------------
int vtkPlusDevice::OutputChannelCount() const
{
  return OutputChannels.size();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::AddInputChannel(vtkPlusChannel* aChannel)
{
  if (aChannel == NULL)
  {
    LOCAL_LOG_ERROR("vtkPlusDevice::AddInputChannel failed: input channel is invalid");
    return PLUS_FAIL;
  }
  for (ChannelContainerIterator it = InputChannels.begin(); it != InputChannels.end(); ++it)
  {
    if (STRCASECMP((*it)->GetChannelId(), aChannel->GetChannelId()) == 0)
    {
      LOCAL_LOG_WARNING("Duplicate addition of an input aChannel.");
      return PLUS_SUCCESS;
    }
  }

  // aChannel remains valid, as it is owned by another device (which provides it as an output)
  // TODO: it could be safer to increase the reference counter of aChannel here (and decrease the ref counter when aChannel is removed from InputChannels)
  this->InputChannels.push_back(aChannel);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::NotifyConfigured()
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
double vtkPlusDevice::GetAcquisitionRate() const
{
  return this->AcquisitionRate;
}

//----------------------------------------------------------------------------
void vtkPlusDevice::InternalWriteOutputChannels(vtkXMLDataElement* rootXMLElement)
{
  LOCAL_LOG_TRACE("vtkPlusDevice::InternalWriteOutputChannels( " << rootXMLElement->GetName() << ")");

  for (ChannelContainerConstIterator it = this->OutputChannels.begin(); it != this->OutputChannels.end(); ++it)
  {
    vtkPlusChannel* aStream = *it;
    vtkXMLDataElement* streamElement = this->FindOutputChannelElement(rootXMLElement, aStream->GetChannelId());
    aStream->WriteConfiguration(streamElement);
  }
}

//----------------------------------------------------------------------------
void vtkPlusDevice::InternalWriteInputChannels(vtkXMLDataElement* rootXMLElement)
{
  LOCAL_LOG_TRACE("vtkPlusDevice::InternalWriteInputChannels( " << rootXMLElement->GetName() << ")");

  for (ChannelContainerConstIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it)
  {
    vtkPlusChannel* aStream = *it;
    vtkXMLDataElement* streamElement = this->FindInputChannelElement(rootXMLElement, aStream->GetChannelId());
    aStream->WriteConfiguration(streamElement);
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::EnsureUniqueDataSourceId(const std::string& aSourceId)
{
  for (DataSourceContainer::iterator it = begin(this->Tools); it != end(this->Tools); ++it)
  {
    if (it->second->GetId() == aSourceId)
    {
      return PLUS_FAIL;
    }
  }
  for (DataSourceContainer::iterator it = begin(this->VideoSources); it != end(this->VideoSources); ++it)
  {
    if (it->second->GetId() == aSourceId)
    {
      return PLUS_FAIL;
    }
  }
  for (DataSourceContainer::iterator it = begin(this->Fields); it != end(this->Fields); ++it)
  {
    if (it->second->GetId() == aSourceId)
    {
      return PLUS_FAIL;
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
bool vtkPlusDevice::GetStartThreadForInternalUpdates() const
{
  return this->StartThreadForInternalUpdates;
}

//----------------------------------------------------------------------------
double vtkPlusDevice::GetRecordingStartTime() const
{
  return this->RecordingStartTime;
}

//----------------------------------------------------------------------------
vtkPlusDataCollector* vtkPlusDevice::GetDataCollector()
{
  return this->DataCollector;
}

//----------------------------------------------------------------------------
DataSourceContainerConstIterator vtkPlusDevice::GetVideoSourceIteratorBegin() const
{
  return this->VideoSources.begin();
}

//----------------------------------------------------------------------------
DataSourceContainerConstIterator vtkPlusDevice::GetVideoSourceIteratorEnd() const
{
  return this->VideoSources.end();
}

//----------------------------------------------------------------------------
int vtkPlusDevice::GetNumberOfVideoSources() const
{
  return this->VideoSources.size();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::AddVideoSource(vtkPlusDataSource* aVideo)
{
  if (aVideo == NULL)
  {
    LOCAL_LOG_ERROR("Failed to add video to device, video is NULL!");
    return PLUS_FAIL;
  }

  if (aVideo->GetId().empty())
  {
    LOCAL_LOG_ERROR("Failed to add video to device, image Id must be defined!");
    return PLUS_FAIL;
  }

  if (this->VideoSources.find(aVideo->GetId()) == this->GetVideoSourceIteratorEnd())
  {
    // Check image port names, it should be unique too
    for (DataSourceContainerConstIterator it = this->GetVideoSourceIteratorBegin(); it != this->GetVideoSourceIteratorEnd(); ++it)
    {
      if (aVideo->GetId() == it->second->GetId())
      {
        LOCAL_LOG_ERROR("Failed to add '" << aVideo->GetId() << "' video to container: video with Id '" << it->second->GetId()
                        << "' is already defined'!");
        return PLUS_FAIL;
      }
    }

    aVideo->Register(this);
    aVideo->SetDevice(this);
    this->VideoSources[aVideo->GetId()] = aVideo;
  }
  else
  {
    LOCAL_LOG_ERROR("Image with Id '" << aVideo->GetId() << "' is already in the image container!");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetFirstVideoSource(vtkPlusDataSource*& aVideoSource)
{
  if (this->VideoSources.size() == 0)
  {
    LOCAL_LOG_ERROR("Failed to get first active video source - there are no video sources!");
    return PLUS_FAIL;
  }

  aVideoSource = this->VideoSources.begin()->second;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetVideoSource(const char* aSourceId, vtkPlusDataSource*& aVideoSource)
{
  if (aSourceId == NULL)
  {
    LOCAL_LOG_ERROR("Failed to get image, image name is NULL");
    return PLUS_FAIL;
  }

  if (this->VideoSources.find(aSourceId) != this->VideoSources.end())
  {
    aVideoSource = this->VideoSources.find(aSourceId)->second;
    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
std::vector<vtkPlusDataSource*> vtkPlusDevice::GetVideoSources() const
{
  std::vector<vtkPlusDataSource*> results;
  for (DataSourceContainerConstIterator it = this->VideoSources.begin(); it != this->VideoSources.end(); ++it)
  {
    results.push_back(it->second);
  }
  return results;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetVideoSourceByIndex(const unsigned int index, vtkPlusDataSource*& aVideoSource)
{
  if (index > this->VideoSources.size())
  {
    LOCAL_LOG_ERROR("Failed to get video source, index is outside of range.");
    aVideoSource = NULL;
    return PLUS_FAIL;
  }

  DataSourceContainerIterator it = this->VideoSources.begin();
  for (unsigned int i = 0 ; i < index; ++i)
  {
    ++it;
  }
  aVideoSource = it->second;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetDataSource(const char* aSourceId, vtkPlusDataSource*& aSource)
{
  if (this->GetVideoSource(aSourceId, aSource) == PLUS_SUCCESS)
  {
    return PLUS_SUCCESS;
  }
  if (this->GetTool(aSourceId, aSource) == PLUS_SUCCESS)
  {
    return PLUS_SUCCESS;
  }
  if (this->GetFieldDataSource(aSourceId, aSource) == PLUS_SUCCESS)
  {
    return PLUS_SUCCESS;
  }
  else
  {
    return PLUS_FAIL;
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetDataSource(const std::string& aSourceId, vtkPlusDataSource*& aSource)
{
  return GetDataSource(aSourceId.c_str(), aSource);
}

//----------------------------------------------------------------------------
ChannelContainerConstIterator vtkPlusDevice::GetOutputChannelsStart() const
{
  return this->OutputChannels.begin();
}

//----------------------------------------------------------------------------
ChannelContainerIterator vtkPlusDevice::GetOutputChannelsStart()
{
  return this->OutputChannels.begin();
}

//----------------------------------------------------------------------------
ChannelContainerConstIterator vtkPlusDevice::GetOutputChannelsEnd() const
{
  return this->OutputChannels.end();
}

//----------------------------------------------------------------------------
ChannelContainerIterator vtkPlusDevice::GetOutputChannelsEnd()
{
  return this->OutputChannels.end();
}

//------------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetToolReferenceFrameFromTrackedFrame(igsioTrackedFrame& aFrame, std::string& aToolReferenceFrameName)
{
  LOG_TRACE("vtkPlusDataCollectorFile::GetTrackerToolReferenceFrame");

  // Try to find it out from the custom transforms that are stored in the tracked frame
  std::vector<igsioTransformName> transformNames;
  aFrame.GetFrameTransformNameList(transformNames);

  if (transformNames.size() == 0)
  {
    LOG_ERROR("No transforms found in tracked frame!");
    return PLUS_FAIL;
  }

  std::string frameName = "";
  for (std::vector<igsioTransformName>::iterator it = transformNames.begin(); it != transformNames.end(); ++it)
  {
    if (frameName == "")
    {
      frameName = it->To();
    }
    else if (frameName != it->To())
    {
      LOG_ERROR("Destination coordinate frame names are not the same!");
      return PLUS_FAIL;
    }
  }

  aToolReferenceFrameName = frameName;

  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
bool vtkPlusDevice::IsTracker() const
{
  LOCAL_LOG_ERROR("Calling base IsTracker. Override in the derived classes.");
  return false;
}

//----------------------------------------------------------------------------
bool vtkPlusDevice::IsVirtual() const
{
  return false;
}

//------------------------------------------------------------------------------
PlusStatus vtkPlusDevice::AddOutputChannel(vtkPlusChannel* aChannel)
{
  if (aChannel == NULL)
  {
    LOCAL_LOG_ERROR("Cannot add device, aChannel is invalid");
    return PLUS_FAIL;
  }
  if (aChannel->GetOwnerDevice() == NULL)
  {
    aChannel->SetOwnerDevice(this);
  }
  this->OutputChannels.push_back(aChannel);
  aChannel->Register(this);
  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
void vtkPlusDevice::SetDataCollector(vtkPlusDataCollector* _arg)
{
  this->DataCollector = _arg;
}

//------------------------------------------------------------------------------
bool vtkPlusDevice::HasGracePeriodExpired()
{
  return (vtkIGSIOAccurateTimer::GetSystemTime() - this->RecordingStartTime) > this->MissingInputGracePeriodSec;
}

//----------------------------------------------------------------------------
double vtkPlusDevice::GetMissingInputGracePeriodSec() const
{
  return this->MissingInputGracePeriodSec;
}

//------------------------------------------------------------------------------
PlusStatus vtkPlusDevice::CreateDefaultOutputChannel(const char* channelId /*=NULL*/, bool addSource/*=true*/)
{
  // Create output channel
  vtkSmartPointer<vtkPlusChannel> aChannel = vtkSmartPointer<vtkPlusChannel>::New();
  aChannel->SetOwnerDevice(this);
  aChannel->SetChannelId(channelId != NULL ? channelId : "OutputChannel");
  if (this->AddOutputChannel(aChannel) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  if (!addSource)
  {
    // no need to add sources, so we are done
    return PLUS_SUCCESS;
  }

  if (this->IsTracker())
  {
    // TODO: add default tools
  }
  else
  {
    // Create an output video stream for this channel
    vtkPlusDataSource* aDataSource = vtkPlusDataSource::New();
    if (aDataSource->SetBufferSize(30) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set video buffer size!");
      return PLUS_FAIL;
    }
    aDataSource->SetInputImageOrientation(US_IMG_ORIENT_MF);
    if (aDataSource->SetId("Video") != PLUS_SUCCESS)
    {
      return PLUS_FAIL;
    }
    if (this->AddVideoSource(aDataSource) != PLUS_SUCCESS)
    {
      return PLUS_FAIL;
    }
    aChannel->SetVideoSource(aDataSource);
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetFirstActiveOutputVideoSource(vtkPlusDataSource*& aVideoSource)
{
  aVideoSource = NULL;
  if (this->OutputChannels.empty())
  {
    LOCAL_LOG_ERROR("Failed to get first active output video source - there are no output channels");
    return PLUS_FAIL;
  }
  for (ChannelContainerIterator it = this->OutputChannels.begin(); it != this->OutputChannels.end(); ++it)
  {
    if ((*it)->GetVideoSource(aVideoSource) == PLUS_SUCCESS && aVideoSource != NULL)
    {
      // found a video source in the output channel
      return PLUS_SUCCESS;
    }
  }

  LOG_ERROR("There is no active video sources in the output channel(s)");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetImageMetaData(igsioCommon::ImageMetaDataList& imageMetaDataItems)
{
  LOCAL_LOG_DEBUG("No ImageMetaData is available");
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::GetImage(const std::string& requestedImageId, std::string& assignedImageId, const std::string& imageReferencFrameName, vtkImageData* imageData, vtkMatrix4x4* ijkToReferenceTransform)
{
  LOCAL_LOG_ERROR("vtkPlusDevice::GetImage is not implemented");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::SendText(const std::string& textToSend, std::string* textReceived/*=NULL*/)
{
  LOCAL_LOG_ERROR("vtkPlusDevice::SendText is not implemented");
  return PLUS_FAIL;
}
