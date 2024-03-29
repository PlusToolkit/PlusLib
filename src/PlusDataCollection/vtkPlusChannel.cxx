/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#ifdef PLUS_RENDERING_ENABLED
#include "PlusPlotter.h"
#endif
#include "vtkPlusBuffer.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusHTMLGenerator.h"
#include "vtkIGSIOTrackedFrameList.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkTable.h>

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusChannel);

//----------------------------------------------------------------------------
// If a frame cannot be retrieved from the device buffers (because it was overwritten by new frames)
// then we skip a SAMPLING_SKIPPING_MARGIN_SEC long period to allow the application to catch up.
// This time should be long enough to comfortably retrieve a frame from the buffer.
static const double SAMPLING_SKIPPING_MARGIN_SEC = 0.1;

//----------------------------------------------------------------------------
vtkPlusChannel::vtkPlusChannel(void)
  : VideoSource(NULL)
  , OwnerDevice(NULL)
  , ChannelId(NULL)
  , RfProcessor(NULL)
  , BlankImage(vtkImageData::New())
  , SaveRfProcessingParameters(false)
{
  // Default size for brightness frame
  this->BrightnessFrameSize[0] = 640;
  this->BrightnessFrameSize[1] = 480;
  this->BrightnessFrameSize[2] = 1;

  this->TimestampMasterTool = NULL;

  // Create a blank image, it will be used as output if frames are not available
  this->BlankImage->SetExtent(0, this->BrightnessFrameSize[0] - 1, 0, this->BrightnessFrameSize[1] - 1, 0, this->BrightnessFrameSize[2] - 1);
  this->BlankImage->AllocateScalars(VTK_UNSIGNED_CHAR, 1);

  unsigned long memorysize = this->BrightnessFrameSize[0] * this->BrightnessFrameSize[1] * this->BrightnessFrameSize[2] * this->BlankImage->GetScalarSize();
  memset(this->BlankImage->GetScalarPointer(), 0, memorysize);
}

//----------------------------------------------------------------------------
vtkPlusChannel::~vtkPlusChannel(void)
{
  this->VideoSource = NULL;
  this->Tools.clear();
  this->FieldDataSources.clear();

  this->SetOwnerDevice(NULL);

  this->SetChannelId(nullptr);

  DELETE_IF_NOT_NULL(this->BlankImage);

  DELETE_IF_NOT_NULL(this->RfProcessor);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::ReadConfiguration(vtkXMLDataElement* aChannelElement, bool RequireImageOrientationInChannelConfiguration)
{
  // Read the stream element, build the stream
  // If there are references to tools, request them from the owner device and keep a reference to them here
  const char* id = aChannelElement->GetAttribute("Id");
  if (id == NULL)
  {
    LOG_ERROR("No stream id defined. It is required for all streams.");
    return PLUS_FAIL;
  }
  this->SetChannelId(id);

  if (this->OwnerDevice == NULL)
  {
    LOG_ERROR("Channel does not know about its parent device. Unable to configure.");
    return PLUS_FAIL;
  }

  vtkPlusDataSource* aSource = NULL;
  for (int i = 0; i < aChannelElement->GetNumberOfNestedElements(); i++)
  {
    vtkXMLDataElement* aSourceElement = aChannelElement->GetNestedElement(i);
    if (STRCASECMP(aSourceElement->GetName(), "DataSource") != 0)
    {
      // if this is not an data source element, skip it
      continue;
    }

    const char* id = aSourceElement->GetAttribute("Id");
    if (id == NULL)
    {
      LOG_ERROR("No field \"Id\" defined in the source element " << this->GetChannelId() << ". Unable to add it to the channel.");
      continue;
    }

    igsioTransformName idName(id, this->OwnerDevice->GetToolReferenceFrameName());
    if (this->OwnerDevice->GetDataSource(id, aSource) == PLUS_SUCCESS)
    {
      if (aSource->GetType() == DATA_SOURCE_TYPE_TOOL)
      {
        this->Tools[aSource->GetId()] = aSource;
      }
      else
      {
        this->FieldDataSources[aSource->GetId()] = aSource;
      }
    }
    else if (this->OwnerDevice->GetDataSource(idName.GetTransformName().c_str(), aSource) == PLUS_SUCCESS)
    {
      this->Tools[aSource->GetId()] = aSource;
    }
    else
    {
      LOG_ERROR("Unable to find data source with Id=\'" << id << "\'.");
      return PLUS_FAIL;
    }
  }

  if (aChannelElement->GetAttribute("VideoDataSourceId") != NULL && this->OwnerDevice->GetVideoSource(aChannelElement->GetAttribute("VideoDataSourceId"), aSource) == PLUS_SUCCESS)
  {
    this->VideoSource = aSource;
  }
  else if (aChannelElement->GetAttribute("VideoDataSourceId") != NULL)
  {
    LOG_ERROR("Unable to find video data source that matches Id: " << aChannelElement->GetAttribute("VideoDataSourceId"));
    return PLUS_FAIL;
  }

  vtkXMLDataElement* rfElement = aChannelElement->FindNestedElementWithName(vtkPlusRfProcessor::GetRfProcessorTagName());
  if (rfElement != NULL)
  {
    this->RfProcessor = vtkPlusRfProcessor::New();
    this->RfProcessor->ReadConfiguration(rfElement);
    this->SaveRfProcessingParameters = true;
  }
  else
  {
    if (this->RfProcessor)
    {
      this->RfProcessor->Delete();
      this->RfProcessor = NULL;
    }
  }

  this->CustomAttributes.clear();
  for (int i = 0; i < aChannelElement->GetNumberOfNestedElements(); i++)
  {
    vtkXMLDataElement* attrElement = aChannelElement->GetNestedElement(i);
    if (STRCASECMP(attrElement->GetName(), "Attribute") != 0)
    {
      continue;
    }
    const char* id = attrElement->GetAttribute("Id");
    if (id == NULL)
    {
      LOG_ERROR("No idea in channel attribute for channel " << this->GetChannelId());
      continue;
    }
    const char* value = attrElement->GetCharacterData();
    if (value == NULL)
    {
      LOG_ERROR("No value set for attribute: " << id);
      continue;
    }
    std::string strId(id);
    this->CustomAttributes[strId] = std::string(value);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::WriteConfiguration(vtkXMLDataElement* aChannelElement)
{
  aChannelElement->SetAttribute("Id", this->GetChannelId());

  for (int i = 0; i < aChannelElement->GetNumberOfNestedElements(); i++)
  {
    vtkXMLDataElement* element = aChannelElement->GetNestedElement(i);
    if (STRCASECMP(element->GetName(), "DataSource") != 0)
    {
      continue;
    }
    bool isEqual(false);
    if (igsioCommon::XML::SafeCheckAttributeValueInsensitive(*element, "Type", vtkPlusDataSource::DATA_SOURCE_TYPE_VIDEO_TAG, isEqual) == PLUS_SUCCESS && isEqual)
    {
      if (this->HasVideoSource())
      {
        this->VideoSource->WriteConfiguration(element);
      }
    }
    else if (igsioCommon::XML::SafeCheckAttributeValueInsensitive(*element, "Type", vtkPlusDataSource::DATA_SOURCE_TYPE_TOOL_TAG, isEqual) == PLUS_SUCCESS && isEqual)
    {
      vtkPlusDataSource* aTool = NULL;
      if (element->GetAttribute("Id") == NULL || this->GetTool(aTool, element->GetAttribute("Id")) != PLUS_SUCCESS)
      {
        LOG_ERROR("Unable to retrieve tool when saving config.");
        return PLUS_FAIL;
      }
      aTool->WriteCompactConfiguration(element);
    }
    else if (igsioCommon::XML::SafeCheckAttributeValueInsensitive(*element, "Type", vtkPlusDataSource::DATA_SOURCE_TYPE_FIELDDATA_TAG, isEqual) == PLUS_SUCCESS && isEqual)
    {
      vtkPlusDataSource* aSource = NULL;
      if (element->GetAttribute("Id") == NULL || this->GetFieldDataSource(aSource, element->GetAttribute("Id")) != PLUS_SUCCESS)
      {
        LOG_ERROR("Unable to retrieve field data source when saving config.");
        return PLUS_FAIL;
      }
      aSource->WriteCompactConfiguration(element);
    }
  }

  if (this->SaveRfProcessingParameters)
  {
    vtkXMLDataElement* rfElement = aChannelElement->FindNestedElementWithName("RfProcessing");
    this->RfProcessor->WriteConfiguration(rfElement);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetTool(vtkPlusDataSource*& aTool, const std::string& toolSourceId)
{
  if (toolSourceId.empty())
  {
    LOG_ERROR("vtkPlusChannel::GetTool failed: toolSourceId is invalid");
    return PLUS_FAIL;
  }

  for (DataSourceContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    if (toolSourceId == it->second->GetId())
    {
      aTool = it->second;
      return PLUS_SUCCESS;
    }
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetToolByPortName(vtkPlusDataSource*& aTool, const std::string& portName)
{
  if (portName.empty())
  {
    LOG_ERROR("vtkPlusChannel::GetToolByPortName failed: portName is invalid");
    return PLUS_FAIL;
  }

  for (DataSourceContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    if (portName == it->second->GetPortName())
    {
      aTool = it->second;
      return PLUS_SUCCESS;
    }
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::AddTool(vtkPlusDataSource* aTool)
{
  if (aTool == NULL)
  {
    LOG_ERROR("Trying to add null tool to channel.");
    return PLUS_FAIL;
  }

  for (DataSourceContainerConstIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    if (it->second == aTool)
    {
      // tool has been already added
      return PLUS_SUCCESS;
    }
  }

  this->Tools[aTool->GetId()] = aTool;
  this->Tools[aTool->GetId()]->Register(this);

  if (this->TimestampMasterTool == NULL)
  {
    // the first added tool will be used as master tool
    // (the first item in the std::map is not the first added tool but depends on the source ID)
    this->TimestampMasterTool = aTool;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::RemoveTool(const std::string& toolSourceId)
{
  if (toolSourceId.empty())
  {
    LOG_ERROR("vtkPlusChannel::RemoveTool failed: toolSourceId is invalid");
    return PLUS_FAIL;
  }

  for (DataSourceContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    if (it->second->GetId() == toolSourceId)
    {
      this->Tools.erase(it);
      if (this->TimestampMasterTool == it->second)
      {
        // the master tool has been deleted
        this->TimestampMasterTool = NULL;
      }
      return PLUS_SUCCESS;
    }
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::RemoveTools()
{
  this->Tools.clear();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::AddFieldDataSource(vtkPlusDataSource* aSource)
{
  if (aSource == NULL)
  {
    LOG_ERROR("Trying to add null field data source to channel.");
    return PLUS_FAIL;
  }

  for (DataSourceContainerConstIterator it = this->FieldDataSources.begin(); it != this->FieldDataSources.end(); ++it)
  {
    if (it->second == aSource)
    {
      // source has been already added
      return PLUS_SUCCESS;
    }
  }

  this->FieldDataSources[aSource->GetId()] = aSource;
  this->FieldDataSources[aSource->GetId()]->Register(this);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::RemoveFieldDataSource(const std::string& sourceId)
{
  if (sourceId.empty())
  {
    LOG_ERROR("vtkPlusChannel::RemoveFieldDataSource failed: sourceId is invalid");
    return PLUS_FAIL;
  }

  for (DataSourceContainerIterator it = this->FieldDataSources.begin(); it != this->FieldDataSources.end(); ++it)
  {
    if (it->second->GetId() == sourceId)
    {
      this->FieldDataSources.erase(it);
      return PLUS_SUCCESS;
    }
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetFieldDataSource(vtkPlusDataSource*& aSource, const std::string& sourceId)
{
  if (sourceId.empty())
  {
    LOG_ERROR("vtkPlusChannel::GetFieldDataSource failed: sourceId is invalid");
    return PLUS_FAIL;
  }

  for (DataSourceContainerIterator it = this->FieldDataSources.begin(); it != this->FieldDataSources.end(); ++it)
  {
    if (sourceId == it->second->GetId())
    {
      aSource = it->second;
      return PLUS_SUCCESS;
    }
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::RemoveFieldDataSources()
{
  this->FieldDataSources.clear();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::Clear()
{
  for (DataSourceContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    it->second->Clear();
  }
  this->TimestampMasterTool = NULL;
  if (this->VideoSource != NULL)
  {
    this->VideoSource->Clear();
  }
  for (DataSourceContainerIterator it = this->FieldDataSources.begin(); it != this->FieldDataSources.end(); ++it)
  {
    it->second->Clear();
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetLatestTimestamp(double& aTimestamp) const
{
  aTimestamp = 0;

  if (this->HasVideoSource())
  {
    if (this->VideoSource->GetLatestTimeStamp(aTimestamp) != ITEM_OK)
    {
      LOG_ERROR("Unable to retrieve latest timestamp from the video source buffer.");
    }
  }

  for (DataSourceContainerConstIterator it = this->GetToolsStartConstIterator(); it != this->GetToolsEndConstIterator(); ++it)
  {
    vtkPlusDataSource* aTool = it->second;
    double timestamp;
    if (aTool->GetLatestTimeStamp(timestamp) == ITEM_OK)
    {
      if (timestamp > aTimestamp)
      {
        aTimestamp = timestamp;
      }
    }
  }

  for (DataSourceContainerConstIterator it = this->GetFieldDataSourcesStartConstIterator(); it != this->GetFieldDataSourcesEndConstIterator(); ++it)
  {
    vtkPlusDataSource* aSource = it->second;
    double timestamp;
    if (aSource->GetLatestTimeStamp(timestamp) == ITEM_OK)
    {
      if (timestamp > aTimestamp)
      {
        aTimestamp = timestamp;
      }
    }
  }

  return aTimestamp != 0 ? PLUS_SUCCESS : PLUS_FAIL;
}


//----------------------------------------------------------------------------
void vtkPlusChannel::ShallowCopy(vtkDataObject* otherObject)
{
  if (otherObject == NULL)
  {
    return;
  }

  vtkPlusChannel* channel = dynamic_cast<vtkPlusChannel*>(otherObject);
  if (channel == NULL)
  {
    return;
  }

  return this->ShallowCopy(*channel);
}

//----------------------------------------------------------------------------
void vtkPlusChannel::ShallowCopy(const vtkPlusChannel& aChannel)
{
  this->Clear();

  vtkPlusDataSource* aSource = NULL;
  if (aChannel.HasVideoSource() && aChannel.GetVideoSource(aSource))
  {
    this->VideoSource = aSource;
  }
  for (DataSourceContainerConstIterator it = aChannel.GetToolsStartConstIterator(); it != aChannel.GetToolsEndConstIterator(); ++it)
  {
    if (this->AddTool(it->second) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to add a tool when shallow copying a channel.");
      continue;
    }
  }
  for (DataSourceContainerConstIterator it = aChannel.GetFieldDataSourcesStartConstIterator(); it != aChannel.GetFieldDataSourcesEndConstIterator(); ++it)
  {
    if (this->AddFieldDataSource(it->second) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to add a field data source when shallow copying a channel.");
      continue;
    }
  }
}

//----------------------------------------------------------------------------
void vtkPlusChannel::SetVideoSource(vtkPlusDataSource* aSource)
{
  this->VideoSource = aSource;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetTrackedFrame(double timestamp, igsioTrackedFrame& aTrackedFrame, bool enableImageData/*=true*/)
{
  int numberOfErrors(0);
  double synchronizedTimestamp(0);

  // Get frame UID
  if (this->HasVideoSource() && enableImageData)
  {
    if (this->VideoSource->GetNumberOfItems() < 1)
    {
      LOG_ERROR("Couldn't get tracked frame from video source, frames are not available yet");
      return PLUS_FAIL;
    }
    BufferItemUidType frameUID = 0;
    ItemStatus status = this->VideoSource->GetItemUidFromTime(timestamp, frameUID);
    if (status != ITEM_OK)
    {
      if (status == ITEM_NOT_AVAILABLE_ANYMORE)
      {
        LOG_ERROR("Couldn't get frame UID from time (" << std::fixed << timestamp <<
                  ") - item not available anymore!");
      }
      else if (status == ITEM_NOT_AVAILABLE_YET)
      {
        LOG_ERROR("Couldn't get frame UID from time (" << std::fixed << timestamp <<
                  ") - item not available yet!");
      }
      else
      {
        LOG_ERROR("Couldn't get frame UID from time (" << std::fixed << timestamp << ")!");
      }

      return PLUS_FAIL;
    }

    StreamBufferItem CurrentStreamBufferItem;
    if (this->VideoSource->GetStreamBufferItem(frameUID, &CurrentStreamBufferItem) != ITEM_OK)
    {
      LOG_ERROR("Couldn't get video buffer item by frame UID: " << frameUID);
      return PLUS_FAIL;
    }

    // Copy frame
    igsioVideoFrame frame = CurrentStreamBufferItem.GetFrame();
    aTrackedFrame.SetImageData(frame);

    // Copy all custom fields
    igsioFieldMapType fieldMap = CurrentStreamBufferItem.GetFrameFieldMap();
    for (igsioFieldMapType::const_iterator fieldIterator = fieldMap.begin(); fieldIterator != fieldMap.end(); fieldIterator++)
    {
      aTrackedFrame.SetFrameField((*fieldIterator).first, (*fieldIterator).second.second, fieldIterator->second.first);
    }

    synchronizedTimestamp = CurrentStreamBufferItem.GetTimestamp(this->VideoSource->GetLocalTimeOffsetSec());
  }

  if (synchronizedTimestamp == 0)
  {
    synchronizedTimestamp = timestamp;
  }

  // Add main tool timestamp
  aTrackedFrame.SetTimestamp(synchronizedTimestamp);

  for (DataSourceContainerConstIterator it = this->GetToolsStartIterator(); it != this->GetToolsEndIterator(); ++it)
  {
    vtkPlusDataSource* aTool = it->second;
    igsioTransformName toolTransformName(aTool->GetId());
    if (!toolTransformName.IsValid())
    {
      LOG_ERROR("Tool transform name is invalid!");
      numberOfErrors++;
      continue;
    }

    StreamBufferItem bufferItem;
    ItemStatus result = aTool->GetStreamBufferItemFromTime(synchronizedTimestamp, &bufferItem, vtkPlusBuffer::INTERPOLATED);
    if (result != ITEM_OK)
    {
      double latestTimestamp(0);
      if (aTool->GetLatestTimeStamp(latestTimestamp) != ITEM_OK)
      {
        LOG_ERROR("Failed to get latest timestamp!");
        numberOfErrors++;
      }

      double oldestTimestamp(0);
      if (aTool->GetOldestTimeStamp(oldestTimestamp) != ITEM_OK)
      {
        LOG_ERROR("Failed to get oldest timestamp!");
        numberOfErrors++;
      }

      LOG_ERROR(aTool->GetId() << ": Failed to get tracker item from buffer by time: " << std::fixed << synchronizedTimestamp << " (Latest timestamp: " << latestTimestamp << "   Oldest timestamp: " << oldestTimestamp << ").");
      numberOfErrors++;
      continue;
    }

    vtkSmartPointer<vtkMatrix4x4> dMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    if (bufferItem.GetMatrix(dMatrix) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to get matrix from buffer item for tool " << aTool->GetId());
      numberOfErrors++;
      continue;
    }

    if (aTrackedFrame.SetFrameTransform(toolTransformName, dMatrix) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set transform for tool " << aTool->GetId());
      numberOfErrors++;
      continue;
    }

    if (aTrackedFrame.SetFrameTransformStatus(toolTransformName, bufferItem.GetStatus()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set transform status for tool " << aTool->GetId());
      numberOfErrors++;
      continue;
    }

    // Copy all custom fields
    igsioFieldMapType fieldMap = bufferItem.GetFrameFieldMap();
    igsioFieldMapType::iterator fieldIterator;
    for (igsioFieldMapType::const_iterator fieldIterator = fieldMap.begin(); fieldIterator != fieldMap.end(); fieldIterator++)
    {
      aTrackedFrame.SetFrameField(fieldIterator->first, fieldIterator->second.second, fieldIterator->second.first);
    }

    synchronizedTimestamp = bufferItem.GetTimestamp(aTool->GetLocalTimeOffsetSec());
  }

  for (DataSourceContainerConstIterator it = this->GetFieldDataSourcesStartIterator(); it != this->GetFieldDataSourcesEndIterator(); ++it)
  {
    vtkPlusDataSource* aSource = it->second;

    StreamBufferItem bufferItem;
    ItemStatus result = aSource->GetStreamBufferItemFromTime(synchronizedTimestamp, &bufferItem, vtkPlusBuffer::CLOSEST_TIME);
    if (result != ITEM_OK)
    {
      double latestTimestamp(0);
      if (aSource->GetLatestTimeStamp(latestTimestamp) != ITEM_OK)
      {
        LOG_ERROR("Failed to get latest timestamp!");
        numberOfErrors++;
      }

      double oldestTimestamp(0);
      if (aSource->GetOldestTimeStamp(oldestTimestamp) != ITEM_OK)
      {
        LOG_ERROR("Failed to get oldest timestamp!");
        numberOfErrors++;
      }

      LOG_ERROR(aSource->GetId() << ": Failed to get tracker item from buffer by time: " << std::fixed << synchronizedTimestamp << " (Latest timestamp: " << latestTimestamp << "   Oldest timestamp: " << oldestTimestamp << ").");
      numberOfErrors++;
      continue;
    }

    // Copy all custom fields
    igsioFieldMapType fieldMap = bufferItem.GetFrameFieldMap();
    for (igsioFieldMapType::const_iterator fieldIterator = fieldMap.begin(); fieldIterator != fieldMap.end(); fieldIterator++)
    {
      aTrackedFrame.SetFrameField(fieldIterator->first, fieldIterator->second.second, fieldIterator->second.first);
    }

    synchronizedTimestamp = bufferItem.GetTimestamp(aSource->GetLocalTimeOffsetSec());
  }

  // Copy frame timestamp
  aTrackedFrame.SetTimestamp(synchronizedTimestamp);

  return (numberOfErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetTrackedFrame(igsioTrackedFrame& trackedFrame)
{
  double mostRecentFrameTimestamp(0);
  RETURN_WITH_FAIL_IF(this->GetMostRecentTimestamp(mostRecentFrameTimestamp) != PLUS_SUCCESS,
                      "Failed to get most recent timestamp from the buffer!");

  return this->GetTrackedFrame(mostRecentFrameTimestamp, trackedFrame);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetTrackedFrameList(double& aTimestampOfLastFrameAlreadyGot, vtkIGSIOTrackedFrameList* aTrackedFrameList, int aMaxNumberOfFramesToAdd)
{
  LOG_TRACE("vtkPlusDevice::GetTrackedFrameList(" << aTimestampOfLastFrameAlreadyGot << ", " << aMaxNumberOfFramesToAdd << ")");

  if (aTrackedFrameList == NULL)
  {
    LOG_ERROR("Unable to get tracked frame list - output tracked frame list is NULL!");
    return PLUS_FAIL;
  }

  // If the buffer is empty then don't display an error just return without adding any items to the output tracked frame list
  if (this->GetVideoDataAvailable())
  {
    if (this->VideoSource->GetNumberOfItems() == 0)
    {
      LOG_DEBUG("vtkPlusDataCollector::GetTrackedFrameList: the video buffer is empty, no items will be returned");
      return PLUS_SUCCESS;
    }
  }

  if (this->GetTrackingEnabled())
  {
    vtkPlusDataSource* masterTool = NULL;
    if (this->GetTimestampMasterTool(masterTool) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to get timestamp master tool");
      return PLUS_FAIL;
    }
    if (masterTool->GetNumberOfItems() == 0)
    {
      LOG_DEBUG("vtkPlusDataCollector::GetTrackedFrameList: the tracker buffer is empty, no items will be returned");
      return PLUS_SUCCESS;
    }
  }

  // Get latest and oldest timestamp
  double mostRecentTimestamp(0);
  static vtkIGSIOLogHelper logHelper(60.0, 500000);
  CUSTOM_RETURN_WITH_FAIL_IF(this->GetMostRecentTimestamp(mostRecentTimestamp) != PLUS_SUCCESS,
                             "Unable to get most recent timestamp!");

  PlusStatus status = PLUS_SUCCESS;
  double oldestTimestamp(0);
  if (this->GetOldestTimestamp(oldestTimestamp) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to get oldest timestamp from buffer!");
    return PLUS_FAIL;
  }

  // If undefined starting timestamp is specified we assume that the acquisition starts from the most recent frame
  double timestampFrom = aTimestampOfLastFrameAlreadyGot;
  if (aTimestampOfLastFrameAlreadyGot == UNDEFINED_TIMESTAMP)
  {
    timestampFrom = mostRecentTimestamp;
  }

  if (timestampFrom < oldestTimestamp)
  {
    LOG_ERROR("Items are requested from " << timestampFrom << ", but the oldest available data is acquired at " << oldestTimestamp);
    return PLUS_FAIL;
  }

  if (aMaxNumberOfFramesToAdd > 0)
  {
    if (this->GetVideoDataAvailable())
    {
      BufferItemUidType mostRecentVideoUid = 0;
      if (this->VideoSource->GetItemUidFromTime(mostRecentTimestamp, mostRecentVideoUid) != ITEM_OK)
      {
        LOG_ERROR("Failed to get video buffer item by timestamp " << mostRecentTimestamp);
        return PLUS_FAIL;
      }
      BufferItemUidType videoUidFrom = 0;
      if (this->VideoSource->GetItemUidFromTime(timestampFrom, videoUidFrom) != ITEM_OK)
      {
        LOG_ERROR("Failed to get video buffer item by timestamp " << timestampFrom);
        return PLUS_FAIL;
      }

      BufferItemUidType firstVideoUidToAdd = videoUidFrom;
      if (mostRecentVideoUid - videoUidFrom + 1 > aMaxNumberOfFramesToAdd)
      {
        // More frames are requested than the maximum allowed frames to add
        firstVideoUidToAdd = mostRecentVideoUid - aMaxNumberOfFramesToAdd + 1; // +1: because most recent is needed too
      }
      else
      {
        LOG_TRACE("Number of frames in the video buffer is less than maxNumberOfFramesToAdd (more data is allowed to be recorded than it was provided by the data sources)");
      }

      if (this->VideoSource->GetTimeStamp(firstVideoUidToAdd, timestampFrom) != ITEM_OK)
      {
        LOG_ERROR("Failed to get video buffer timestamp from UID: " << firstVideoUidToAdd);
        return PLUS_FAIL;
      }
    }
    else if (this->GetTrackingEnabled())
    {
      vtkPlusDataSource* masterTool = NULL;
      if (this->GetTimestampMasterTool(masterTool) != PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to get tracked frame list - there is no active tool!");
        return PLUS_FAIL;
      }

      BufferItemUidType mostRecentTrackerUid = 0;
      if (masterTool->GetItemUidFromTime(mostRecentTimestamp, mostRecentTrackerUid) != ITEM_OK)
      {
        LOG_ERROR("Failed to get tracked buffer item by timestamp " << mostRecentTimestamp);
        return PLUS_FAIL;
      }
      BufferItemUidType trackerUidFrom = 0;
      ItemStatus status = masterTool->GetItemUidFromTime(timestampFrom, trackerUidFrom);
      if (status != ITEM_OK)
      {
        switch (status)
        {
          case ITEM_NOT_AVAILABLE_YET:
            LOG_ERROR("Failed to get tracker buffer item by timestamp " << timestampFrom << ". Item not available yet.");
            break;
          case ITEM_NOT_AVAILABLE_ANYMORE:
            LOG_ERROR("Failed to get tracker buffer item by timestamp " << timestampFrom << ". Item not available anymore.");
            break;
          case ITEM_UNKNOWN_ERROR:
          default:
            LOG_ERROR("Failed to get tracker buffer item by timestamp " << timestampFrom);
            break;
        }
        return PLUS_FAIL;
      }

      BufferItemUidType firstTrackerUidToAdd = trackerUidFrom;
      if (mostRecentTrackerUid - trackerUidFrom + 1 > aMaxNumberOfFramesToAdd)
      {
        // More frames are requested than the maximum allowed frames to add
        firstTrackerUidToAdd = mostRecentTrackerUid - aMaxNumberOfFramesToAdd + 1; // +1: because most recent is needed too
      }
      else
      {
        LOG_TRACE("Number of frames in the tracker buffer is less than maxNumberOfFramesToAdd (more data is allowed to be recorded than it was provided by the data sources)");
      }

      if (masterTool->GetTimeStamp(firstTrackerUidToAdd, timestampFrom) != ITEM_OK)
      {
        LOG_ERROR("Failed to get tracker buffer timestamp from UID: " << firstTrackerUidToAdd);
        return PLUS_FAIL;
      }
    }
    else if (this->GetFieldDataEnabled())
    {
      vtkPlusDataSource* aSource = this->FieldDataSources.begin()->second;

      BufferItemUidType mostRecentSourceUid = 0;
      if (aSource->GetItemUidFromTime(mostRecentTimestamp, mostRecentSourceUid) != ITEM_OK)
      {
        LOG_ERROR("Failed to get tracked buffer item by timestamp " << mostRecentTimestamp);
        return PLUS_FAIL;
      }
      BufferItemUidType sourceUidFrom = 0;
      ItemStatus status = aSource->GetItemUidFromTime(timestampFrom, sourceUidFrom);
      if (status != ITEM_OK)
      {
        switch (status)
        {
          case ITEM_NOT_AVAILABLE_YET:
            LOG_ERROR("Failed to get field buffer item by timestamp " << timestampFrom << ". Item not available yet.");
            break;
          case ITEM_NOT_AVAILABLE_ANYMORE:
            LOG_ERROR("Failed to get field buffer item by timestamp " << timestampFrom << ". Item not available anymore.");
            break;
          case ITEM_UNKNOWN_ERROR:
          default:
            LOG_ERROR("Failed to get field buffer item by timestamp " << timestampFrom);
            break;
        }
        return PLUS_FAIL;
      }

      BufferItemUidType firstSourceUidToAdd = sourceUidFrom;
      if (mostRecentSourceUid - sourceUidFrom + 1 > aMaxNumberOfFramesToAdd)
      {
        // More frames are requested than the maximum allowed frames to add
        firstSourceUidToAdd = mostRecentSourceUid - aMaxNumberOfFramesToAdd + 1; // +1: because most recent is needed too
      }
      else
      {
        LOG_TRACE("Number of frames in the field buffer is less than maxNumberOfFramesToAdd (more data is allowed to be recorded than it was provided by the data sources)");
      }

      if (aSource->GetTimeStamp(firstSourceUidToAdd, timestampFrom) != ITEM_OK)
      {
        LOG_ERROR("Failed to get field buffer timestamp from UID: " << firstSourceUidToAdd);
        return PLUS_FAIL;
      }
    }
  }

  // Check input frameTimestamp to be in a valid range
  if (timestampFrom > mostRecentTimestamp)
  {
    timestampFrom = mostRecentTimestamp;
  }
  else if (timestampFrom < oldestTimestamp)
  {
    timestampFrom = oldestTimestamp;
  }

  // Determine how many frames to add
  int numberOfFramesSinceTimestamp = GetNumberOfFramesBetweenTimestamps(timestampFrom, mostRecentTimestamp);

  int numberOfFramesToAdd = 0;
  if (aMaxNumberOfFramesToAdd > 0)
  {
    numberOfFramesToAdd = std::min(aMaxNumberOfFramesToAdd, numberOfFramesSinceTimestamp);
  }
  else
  {
    numberOfFramesToAdd = numberOfFramesSinceTimestamp;
  }

  LOG_TRACE("Number of added frames: " << numberOfFramesToAdd << " out of " << numberOfFramesSinceTimestamp);

  // If we couldn't find any frames (or one of the items were invalid)
  // set the timestamp to the most recent one
  if (numberOfFramesToAdd == 0)
  {
    timestampFrom = mostRecentTimestamp;
  }

  for (int i = 0; i < numberOfFramesToAdd; ++i)
  {
    // Only add this frame if it has not been already added
    if (timestampFrom > aTimestampOfLastFrameAlreadyGot || aTimestampOfLastFrameAlreadyGot == UNDEFINED_TIMESTAMP)
    {
      // Get tracked frame from buffer
      igsioTrackedFrame* trackedFrame = new igsioTrackedFrame;

      if (this->GetTrackedFrame(timestampFrom, *trackedFrame) != PLUS_SUCCESS)
      {
        delete trackedFrame;
        LOG_ERROR("Unable to get tracked frame by time: " << std::fixed << timestampFrom);
        return PLUS_FAIL;
      }

      // Add tracked frame to the list
      aTimestampOfLastFrameAlreadyGot = trackedFrame->GetTimestamp();
      if (aTrackedFrameList->TakeTrackedFrame(trackedFrame, vtkIGSIOTrackedFrameList::SKIP_INVALID_FRAME) != PLUS_SUCCESS)
      {
        LOG_ERROR("Unable to add tracked frame to the list!");
        return PLUS_FAIL;
      }
    }

    // Get next timestamp
    if (this->GetVideoDataAvailable() && i < numberOfFramesToAdd - 1)
    {
      BufferItemUidType videoUid(0);
      if (this->VideoSource->GetItemUidFromTime(timestampFrom, videoUid) != ITEM_OK)
      {
        LOG_ERROR("Failed to get video buffer item UID from time: " << std::fixed << timestampFrom);
        return PLUS_FAIL;
      }

      if (videoUid >= this->VideoSource->GetLatestItemUidInBuffer())
      {
        LOG_WARNING("Requested video uid (" << videoUid + 1 << ") is not in the buffer yet!");
        break;
      }

      // Get the timestamp of the next item in the buffer
      if (this->VideoSource->GetTimeStamp(++videoUid, timestampFrom) != ITEM_OK)
      {
        LOG_ERROR("Unable to get timestamp from video buffer by UID: " << videoUid);
        return PLUS_FAIL;
      }
    }
    else if (this->GetTrackingEnabled() && i < numberOfFramesToAdd - 1)
    {
      vtkPlusDataSource* masterTool = NULL;
      if (this->GetTimestampMasterTool(masterTool) != PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to get tracked frame list - there is no active tool!");
        return PLUS_FAIL;
      }

      BufferItemUidType trackerUid(0);
      if (masterTool->GetItemUidFromTime(timestampFrom, trackerUid) != ITEM_OK)
      {
        LOG_ERROR("Failed to get tracker buffer item UID from time: " << std::fixed << timestampFrom);
        return PLUS_FAIL;
      }

      if (trackerUid >= masterTool->GetLatestItemUidInBuffer())
      {
        LOG_ERROR("Requested tracker uid (" << trackerUid + 1 << ") is not in the buffer yet!");
        break;
      }

      // Get the timestamp of the next item in the buffer
      if (masterTool->GetTimeStamp(++trackerUid, timestampFrom) != ITEM_OK)
      {
        LOG_WARNING("Unable to get timestamp from tracker buffer by UID: " << trackerUid);
        return PLUS_FAIL;
      }
    }
    else if (this->GetFieldDataAvailable() && i < numberOfFramesToAdd - 1)
    {
      vtkPlusDataSource* firstFieldDataSource = this->FieldDataSources.begin()->second;

      BufferItemUidType fieldUid(0);
      if (firstFieldDataSource->GetItemUidFromTime(timestampFrom, fieldUid) != ITEM_OK)
      {
        LOG_ERROR("Failed to get tracker buffer item UID from time: " << std::fixed << timestampFrom);
        return PLUS_FAIL;
      }

      if (fieldUid >= firstFieldDataSource->GetLatestItemUidInBuffer())
      {
        LOG_ERROR("Requested tracker uid (" << fieldUid + 1 << ") is not in the buffer yet!");
        break;
      }

      // Get the timestamp of the next item in the buffer
      if (firstFieldDataSource->GetTimeStamp(++fieldUid, timestampFrom) != ITEM_OK)
      {
        LOG_WARNING("Unable to get timestamp from field data buffer by UID: " << fieldUid);
        return PLUS_FAIL;
      }
    }
  }

  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetTrackedFrameListSampled(double& aTimestampOfLastFrameAlreadyGot, double& aTimestampOfNextFrameToBeAdded, vtkIGSIOTrackedFrameList* aTrackedFrameList, double aSamplingPeriodSec, double maxTimeLimitSec/*=-1*/)
{
  LOG_TRACE("vtkPlusDataCollector::GetTrackedFrameListSampled: aTimestampOfLastFrameAlreadyGot=" << aTimestampOfLastFrameAlreadyGot << ", aTimestampOfNextFrameToBeAdded=" << aTimestampOfNextFrameToBeAdded << ", aSamplingPeriodSec=" << aSamplingPeriodSec);

  if (aTrackedFrameList == NULL)
  {
    LOG_ERROR("vtkPlusChannel::GetTrackedFrameListSampled failed: unable to get tracked frame list. Output tracked frame list is NULL.");
    return PLUS_FAIL;
  }

  double startTimeSec = vtkIGSIOAccurateTimer::GetSystemTime();

  double mostRecentTimestamp(0);
  RETURN_WITH_FAIL_IF(this->GetMostRecentTimestamp(mostRecentTimestamp) != PLUS_SUCCESS,
                      "vtkPlusChannel::GetTrackedFrameListSampled failed: unable to get most recent timestamp. Probably no frames have been acquired yet.");

  PlusStatus status = PLUS_SUCCESS;
  // Add frames to input trackedFrameList
  for (; aTimestampOfNextFrameToBeAdded <= mostRecentTimestamp; aTimestampOfNextFrameToBeAdded += aSamplingPeriodSec)
  {
    // If the time that is allowed for adding of frames is expired then stop the processing now
    if (maxTimeLimitSec > 0 && vtkIGSIOAccurateTimer::GetSystemTime() - startTimeSec > maxTimeLimitSec)
    {
      LOG_DEBUG("Reached maximum time that is allowed for sampling frames");
      break;
    }

    // Make sure the next frame to be added is still in the buffer:
    // If the frame will be removed from the buffer really soon, then jump ahead in time (and skip some frames),
    // instead of trying to retrieve from the buffer (and then fail because the frame is not available anymore).
    double oldestTimestamp = 0;
    if (this->GetOldestTimestamp(oldestTimestamp) != PLUS_SUCCESS)
    {
      LOG_ERROR("vtkPlusChannel::GetTrackedFrameListSampled: Failed to get oldest timestamp from buffer. Probably no frames have been acquired yet.");
      return PLUS_FAIL;
    }
    if (aTimestampOfNextFrameToBeAdded < oldestTimestamp + SAMPLING_SKIPPING_MARGIN_SEC)
    {
      double newTimestampOfFrameToBeAdded = oldestTimestamp + SAMPLING_SKIPPING_MARGIN_SEC;
      LOG_WARNING("vtkPlusChannel::GetTrackedFrameListSampled: Frames in the buffer are not available any more at time: " << std::fixed << aTimestampOfNextFrameToBeAdded << ". Skipping " << newTimestampOfFrameToBeAdded - aTimestampOfNextFrameToBeAdded << " seconds from the recording to catch up. Increase the buffer size or decrease the acquisition rate to avoid this situation.");
      aTimestampOfNextFrameToBeAdded = newTimestampOfFrameToBeAdded;
      continue;
    }

    // Get the closest frame to the timestamp of the next frame to be added
    double closestTimestamp = GetClosestTrackedFrameTimestampByTime(aTimestampOfNextFrameToBeAdded);
    if (closestTimestamp == UNDEFINED_TIMESTAMP)
    {
      LOG_ERROR("vtkPlusChannel::GetTrackedFrameListSampled: Failed to get closest timestamp from buffer for the next frame. Probably no frames have been acquired yet.");
      return PLUS_FAIL;
    }
    if (aTimestampOfLastFrameAlreadyGot != UNDEFINED_TIMESTAMP && closestTimestamp <= aTimestampOfLastFrameAlreadyGot)
    {
      // This frame has been already added. Don't spend time with retrieving this frame, just jump to the next
      continue;
    }
    // Get tracked frame from buffer (actually copies pixel and field data)
    igsioTrackedFrame* trackedFrame = new igsioTrackedFrame;
    if (GetTrackedFrame(closestTimestamp, *trackedFrame) != PLUS_SUCCESS)
    {
      LOG_WARNING("vtkPlusChannel::GetTrackedFrameListSampled: Unable retrieve frame from the devices for time: " << std::fixed << aTimestampOfNextFrameToBeAdded << ", probably the item is not available in the buffers anymore. Frames may be lost.");
      delete trackedFrame;
      continue;
    }
    aTimestampOfLastFrameAlreadyGot = trackedFrame->GetTimestamp();
    // Add tracked frame to the list
    if (aTrackedFrameList->TakeTrackedFrame(trackedFrame, vtkIGSIOTrackedFrameList::SKIP_INVALID_FRAME) != PLUS_SUCCESS)
    {
      LOG_ERROR("vtkPlusChannel::GetTrackedFrameListSampled: Unable to add tracked frame to the list");
      status = PLUS_FAIL;
    }
  }


  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetOldestTimestamp(double& ts)
{
  //LOG_TRACE("vtkPlusChannel::GetOldestTimestamp");
  ts = 0;

  // ********************* video timestamp **********************
  double oldestVideoTimestamp(std::numeric_limits<double>::max());
  if (this->GetVideoDataAvailable())
  {
    if (this->VideoSource->GetOldestTimeStamp(oldestVideoTimestamp) != ITEM_OK)
    {
      LOG_WARNING("Failed to get oldest timestamp from video buffer!");
      return PLUS_FAIL;
    }
  }

  // ********************* tracker timestamp **********************
  double oldestTrackerTimestamp(std::numeric_limits<double>::max());
  if (this->GetTrackingEnabled())
  {
    vtkPlusDataSource* masterTool = NULL;
    if (this->GetTimestampMasterTool(masterTool) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to get oldest timestamp from tracker buffer - there is no active tool!");
      return PLUS_FAIL;
    }

    // Get the oldest valid timestamp from the tracker buffer
    if (masterTool->GetOldestTimeStamp(oldestTrackerTimestamp) != ITEM_OK)
    {
      LOG_WARNING("Unable to get timestamp from default tool tracker buffer");
      return PLUS_FAIL;
    }
  }

  // ********************* field data timestamp *******************
  double oldestFieldDataTimestamp(std::numeric_limits<double>::max());
  if (this->GetFieldDataEnabled())
  {
    vtkPlusDataSource* aSource = this->FieldDataSources.begin()->second;

    // Get the oldest valid timestamp from the tracker buffer
    if (aSource->GetOldestTimeStamp(oldestFieldDataTimestamp) != ITEM_OK)
    {
      LOG_WARNING("Unable to get timestamp from default field data buffer");
      return PLUS_FAIL;
    }
  }

  double oldestTimestamp = std::min(std::min(oldestFieldDataTimestamp, oldestTrackerTimestamp), oldestVideoTimestamp);
  if (!this->GetVideoDataAvailable())
  {
    oldestVideoTimestamp = oldestTimestamp;
  }
  if (!this->GetTrackingEnabled())
  {
    oldestTrackerTimestamp = oldestTimestamp;
  }
  if (!this->GetFieldDataEnabled())
  {
    oldestFieldDataTimestamp = oldestTimestamp;
  }

  // If the video timestamp is older (smaller) than the tracker timestamp, then use the earliest video timestamp that comes after the first tracker timestamp
  if (oldestVideoTimestamp < oldestTrackerTimestamp)
  {
    // Get the video timestamp that is closest to the oldest tracker timestamp
    BufferItemUidType videoUid(0);
    if (this->VideoSource->GetItemUidFromTime(oldestTrackerTimestamp, videoUid) != ITEM_OK)
    {
      LOG_ERROR("Failed to get video buffer item UID from time: " << std::fixed << oldestVideoTimestamp);
      return PLUS_FAIL;
    }
    if (oldestVideoTimestamp < oldestTrackerTimestamp)
    {
      // the closest video timestamp is still older (smaller) than the first tracking data,
      // so we need the next video timestamp (that should have a timestamp that is larger than the first tracking data)
      if (videoUid + 1 > this->VideoSource->GetLatestItemUidInBuffer())
      {
        // the next video item does not exist, so there is no overlap between the tracking and video data
        LOG_ERROR("Failed to get oldest timestamp: no overlap between tracking and video data");
        return PLUS_FAIL;
      }
      if (this->VideoSource->GetTimeStamp(videoUid + 1, oldestVideoTimestamp) != ITEM_OK)
      {
        LOG_ERROR("Failed to get video buffer timestamp from UID: " << videoUid);
        return PLUS_FAIL;
      }
      if (oldestVideoTimestamp < oldestTrackerTimestamp)
      {
        LOG_ERROR("Failed to get oldest timestamp: no overlap between tracking and video data");
        return PLUS_FAIL;
      }
    }
  }

  // If the video timestamp is older (smaller) than the field data timestamp, then use the earliest video timestamp that comes after the first field data timestamp
  if (oldestVideoTimestamp < oldestFieldDataTimestamp)
  {
    // Get the video timestamp that is closest to the oldest field data timestamp
    BufferItemUidType videoUid(0);
    if (this->VideoSource->GetItemUidFromTime(oldestFieldDataTimestamp, videoUid) != ITEM_OK)
    {
      LOG_ERROR("Failed to get video buffer item UID from time: " << std::fixed << oldestVideoTimestamp);
      return PLUS_FAIL;
    }
    if (oldestVideoTimestamp < oldestFieldDataTimestamp)
    {
      // the closest video timestamp is still older (smaller) than the first tracking data,
      // so we need the next video timestamp (that should have a timestamp that is larger than the first tracking data)
      if (videoUid + 1 > this->VideoSource->GetLatestItemUidInBuffer())
      {
        // the next video item does not exist, so there is no overlap between the field data and video data
        LOG_ERROR("Failed to get oldest timestamp: no overlap between field data and video data");
        return PLUS_FAIL;
      }
      if (this->VideoSource->GetTimeStamp(videoUid + 1, oldestVideoTimestamp) != ITEM_OK)
      {
        LOG_ERROR("Failed to get video buffer timestamp from UID: " << videoUid);
        return PLUS_FAIL;
      }
      if (oldestVideoTimestamp < oldestFieldDataTimestamp)
      {
        LOG_ERROR("Failed to get oldest timestamp: no overlap between field data and video data");
        return PLUS_FAIL;
      }
    }
  }

  ts = oldestVideoTimestamp;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetMostRecentTimestamp(double& ts)
{
  ts = 0;

  double latestVideoTimestamp(0);
  // This can't check for data, only if there is a video source device...
  if (this->GetVideoDataAvailable())
  {
    // Get the most recent timestamp from the buffer
    RETURN_WITH_FAIL_IF(this->VideoSource->GetLatestTimeStamp(latestVideoTimestamp) != ITEM_OK,
                        "Unable to get latest timestamp from video buffer!");
  }

  double latestTrackerTimestamp(0); // the latest tracker timestamp that is available for all tools
  if (this->GetTrackingEnabled())
  {
    double latestCommonTrackerTimestamp = 0;
    bool mostRecentTrackerTimestampRetrieved = false;
    for (DataSourceContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
    {
      vtkPlusDataSource* tool = it->second;
      if (tool == NULL)
      {
        LOG_ERROR("Invalid tool " << it->first);
        continue;
      }
      // Get the most recent valid timestamp from the tracker buffer
      double latestTrackerTimestampForCurrentTool = 0;
      if (tool->GetLatestTimeStamp(latestTrackerTimestampForCurrentTool) != ITEM_OK)
      {
        LOG_WARNING("Unable to get timestamp from " << it->first << " tool tracker buffer for time: " << latestTrackerTimestampForCurrentTool);
        continue;
      }
      if (!mostRecentTrackerTimestampRetrieved)
      {
        // initialize with the first tool
        latestCommonTrackerTimestamp = latestTrackerTimestampForCurrentTool;
        mostRecentTrackerTimestampRetrieved = true;
      }
      else if (latestTrackerTimestampForCurrentTool < latestCommonTrackerTimestamp)
      {
        latestCommonTrackerTimestamp = latestTrackerTimestampForCurrentTool;
      }
    }

    if (!mostRecentTrackerTimestampRetrieved)
    {
      LOG_ERROR("Failed to get most recent timestamp from all the tracker tools");
      return PLUS_FAIL;
    }

    // The master tool determines the sampling times, the other tools are interpolated.
    vtkPlusDataSource* masterTool = NULL;
    if (this->GetTimestampMasterTool(masterTool) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to get most recent timestamp from tracker buffer - there is no active tool");
      return PLUS_FAIL;
    }

    BufferItemUidType uid = 0;
    if (masterTool->GetItemUidFromTime(latestCommonTrackerTimestamp, uid) != ITEM_OK)
    {
      LOG_ERROR("Failed to get tracker buffer item UID from time: " << std::fixed << latestCommonTrackerTimestamp);
      return PLUS_FAIL;
    }

    double latestTrackerTimestampForMasterTool = 0;
    // Get the most recent valid timestamp from the tracker buffer
    if (masterTool->GetTimeStamp(uid, latestTrackerTimestampForMasterTool) != ITEM_OK)
    {
      LOG_WARNING("Unable to get timestamp from default tool tracker buffer with UID: " << uid);
      return PLUS_FAIL;
    }

    if (latestTrackerTimestampForMasterTool > latestCommonTrackerTimestamp)
    {
      // the closest master tracking timestamp is larger than the last common tracking data timestamp,
      // so we need the previous master tracker timestamp (that should have a timestamp that is smaller than the latest common timestamp)
      if (uid - 1 < masterTool->GetOldestItemUidInBuffer())
      {
        // the tracker buffer item does not exist, so there is no overlap between the tracking tools
        LOG_ERROR("Failed to get most recent timestamp: no time overlap between tracking tools");
        return PLUS_FAIL;
      }
      if (masterTool->GetTimeStamp(uid - 1, latestTrackerTimestampForMasterTool) != ITEM_OK)
      {
        LOG_ERROR("Failed to get tracker buffer timestamp from UID: " << uid - 1);
        return PLUS_FAIL;
      }
      if (latestTrackerTimestampForMasterTool > latestCommonTrackerTimestamp)
      {
        LOG_ERROR("Failed to get most recent timestamp: no time overlap between tracking tools data");
        return PLUS_FAIL;
      }
    }

    latestTrackerTimestamp = latestTrackerTimestampForMasterTool;
  }

  double latestFieldDataTimestamp(0); // the latest field data timestamp that is available
  if (this->GetFieldDataEnabled())
  {
    double latestCommonTimestamp(0);
    bool mostRecentSourceTimestampRetrieved(false);
    for (DataSourceContainerIterator it = this->FieldDataSources.begin(); it != this->FieldDataSources.end(); ++it)
    {
      vtkPlusDataSource* aSource = it->second;
      if (aSource == NULL)
      {
        LOG_ERROR("Invalid data source " << it->first);
        continue;
      }
      // Get the most recent valid timestamp from the field data buffer
      double latestTimestampForCurrentSource(0);
      if (aSource->GetLatestTimeStamp(latestTimestampForCurrentSource) != ITEM_OK)
      {
        LOG_WARNING("Unable to get timestamp from " << it->first << " field data buffer for time: " << latestTimestampForCurrentSource);
        continue;
      }
      if (!mostRecentSourceTimestampRetrieved)
      {
        // initialize with the first field data source
        latestCommonTimestamp = latestTimestampForCurrentSource;
        mostRecentSourceTimestampRetrieved = true;
      }
      else if (latestTimestampForCurrentSource < latestCommonTimestamp)
      {
        latestCommonTimestamp = latestTimestampForCurrentSource;
      }
    }

    if (!mostRecentSourceTimestampRetrieved)
    {
      LOG_ERROR("Failed to get most recent timestamp from all the field data sources");
      return PLUS_FAIL;
    }

    latestFieldDataTimestamp = latestCommonTimestamp;
  }

  if (!this->GetVideoDataAvailable())
  {
    latestVideoTimestamp = std::max(latestTrackerTimestamp, latestFieldDataTimestamp);
  }

  if (!this->GetTrackingEnabled())
  {
    latestTrackerTimestamp = std::max(latestVideoTimestamp, latestFieldDataTimestamp);
  }

  if (!this->GetFieldDataEnabled())
  {
    latestFieldDataTimestamp = std::max(latestVideoTimestamp, latestTrackerTimestamp);;
  }

  // If the video timestamp is newer than the tracker timestamp, then use the latest video timestamp that comes before the first tracker timestamp
  if (latestVideoTimestamp > latestTrackerTimestamp)
  {
    // Get the timestamp of the video item that is closest to the latest tracker item
    BufferItemUidType videoUid(0);
    static vtkIGSIOLogHelper logHelper(60.0, 500000);
    CUSTOM_RETURN_WITH_FAIL_IF(this->VideoSource->GetItemUidFromTime(latestTrackerTimestamp, videoUid) != ITEM_OK,
                               "Failed to get video buffer item UID from time: " << std::fixed << latestVideoTimestamp);
    RETURN_WITH_FAIL_IF(this->VideoSource->GetTimeStamp(videoUid, latestVideoTimestamp) != ITEM_OK,
                        "Failed to get video buffer timestamp from UID: " << videoUid);
    if (latestVideoTimestamp > latestTrackerTimestamp)
    {
      // the closest video timestamp is still larger than the last tracking data,
      // so we need the previous video timestamp (that should have a timestamp that is smaller than the first tracking data)
      if (videoUid - 1 < this->VideoSource->GetOldestItemUidInBuffer())
      {
        // the previous video item does not exist, so there is no overlap between the tracking and video data
        LOG_ERROR("Failed to get most recent timestamp: no overlap between tracking and video data");
        return PLUS_FAIL;
      }
      if (this->VideoSource->GetTimeStamp(videoUid - 1, latestVideoTimestamp) != ITEM_OK)
      {
        LOG_ERROR("Failed to get video buffer timestamp from UID: " << videoUid);
        return PLUS_FAIL;
      }
      if (latestVideoTimestamp > latestTrackerTimestamp)
      {
        LOG_ERROR("Failed to get most recent timestamp: no overlap between tracking and video data");
        return PLUS_FAIL;
      }
    }
  }

  // If the video timestamp is newer than the field data timestamp, then use the latest video timestamp that comes before the first field data timestamp
  if (latestVideoTimestamp > latestFieldDataTimestamp)
  {
    // Get the timestamp of the video item that is closest to the latest field data item
    BufferItemUidType videoUid(0);
    RETURN_WITH_FAIL_IF(this->VideoSource->GetItemUidFromTime(latestFieldDataTimestamp, videoUid) != ITEM_OK,
                        "Failed to get video buffer item UID from time: " << std::fixed << latestVideoTimestamp);
    RETURN_WITH_FAIL_IF(this->VideoSource->GetTimeStamp(videoUid, latestVideoTimestamp) != ITEM_OK,
                        "Failed to get video buffer timestamp from UID: " << videoUid);
    if (latestVideoTimestamp > latestFieldDataTimestamp)
    {
      // the closest video timestamp is still larger than the last field data,
      // so we need the previous video timestamp (that should have a timestamp that is smaller than the first field data)
      if (videoUid - 1 < this->VideoSource->GetOldestItemUidInBuffer())
      {
        // the previous video item does not exist, so there is no overlap between the field data and video data
        LOG_ERROR("Failed to get most recent timestamp: no overlap between field and video data");
        return PLUS_FAIL;
      }
      if (this->VideoSource->GetTimeStamp(videoUid - 1, latestVideoTimestamp) != ITEM_OK)
      {
        LOG_ERROR("Failed to get video buffer timestamp from UID: " << videoUid);
        return PLUS_FAIL;
      }
      if (latestVideoTimestamp > latestFieldDataTimestamp)
      {
        LOG_ERROR("Failed to get most recent timestamp: no overlap between field and video data");
        return PLUS_FAIL;
      }
    }
  }

  ts = latestVideoTimestamp;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
bool vtkPlusChannel::GetTrackingDataAvailable()
{
  vtkPlusDataSource* aSource = NULL;
  if (this->HasVideoSource() && this->GetVideoSource(aSource) == PLUS_SUCCESS)
  {
    if (aSource->GetLatestItemHasValidTransformData())
    {
      return true;
    }
  }

  // Now check any and all tool buffers
  for (DataSourceContainerConstIterator toolIt = this->GetToolsStartConstIterator(); toolIt != this->GetToolsEndConstIterator(); ++toolIt)
  {
    vtkPlusDataSource* tool = toolIt->second;
    if (tool->GetLatestItemHasValidTransformData())
    {
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------
bool vtkPlusChannel::GetVideoDataAvailable()
{
  vtkPlusDataSource* aSource = NULL;
  if (this->GetVideoSource(aSource) != PLUS_SUCCESS)
  {
    return false;
  }
  return aSource->GetLatestItemHasValidVideoData();
}

//----------------------------------------------------------------------------
bool vtkPlusChannel::GetFieldDataAvailable()
{
  vtkPlusDataSource* aSource = NULL;
  if (this->HasVideoSource() && this->GetVideoSource(aSource) == PLUS_SUCCESS)
  {
    if (aSource->GetLatestItemHasValidFieldData())
    {
      return true;
    }
  }

  // Now check any and all field data buffers
  for (DataSourceContainerConstIterator it = this->GetFieldDataSourcesStartConstIterator(); it != this->GetFieldDataSourcesEndConstIterator(); ++it)
  {
    vtkPlusDataSource* aSource = it->second;
    if (aSource->GetLatestItemHasValidFieldData())
    {
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------
bool vtkPlusChannel::GetTrackingEnabled() const
{
  return this->ToolCount() > 0;
}

//----------------------------------------------------------------------------
bool vtkPlusChannel::GetVideoEnabled() const
{
  return this->HasVideoSource();
}

//----------------------------------------------------------------------------
bool vtkPlusChannel::GetFieldDataEnabled() const
{
  return this->FieldCount() > 0;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetTimestampMasterTool(vtkPlusDataSource*& aTool)
{
  if (this->TimestampMasterTool == NULL)
  {
    // the timestamp master tool has not been set, either no tools have been added
    // or the timestamp master tool has been deleted
    DataSourceContainerConstIterator dataSourceIt = this->GetToolsStartConstIterator();
    if (dataSourceIt == this->GetToolsEndConstIterator())
    {
      LOG_ERROR("Failed to get the timestamp master tool - there is no active tool");
      return PLUS_FAIL;
    }
    this->TimestampMasterTool = dataSourceIt->second;
    if (this->TimestampMasterTool == NULL)
    {
      LOG_ERROR("Failed to get the timestamp master tool - the first active tool is invalid");
      return PLUS_FAIL;
    }
  }
  aTool = this->TimestampMasterTool;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
int vtkPlusChannel::GetNumberOfFramesBetweenTimestamps(double aTimestampFrom, double aTimestampTo)
{
  int numberOfFrames = 0;

  if (this->GetVideoDataAvailable())
  {
    BufferItemUidType fromItemUid(0);
    if (this->VideoSource->GetItemUidFromTime(aTimestampFrom, fromItemUid) != ITEM_OK)
    {
      return 0;
    }

    BufferItemUidType toItemUid(0);
    if (this->VideoSource->GetItemUidFromTime(aTimestampTo, toItemUid) != ITEM_OK)
    {
      return 0;
    }

    numberOfFrames = abs((int)(toItemUid - fromItemUid));
  }
  else if (this->GetTrackingEnabled())
  {
    vtkPlusDataSource* masterTool = NULL;
    if (this->GetTimestampMasterTool(masterTool) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to get number of frames between timestamps - there is no active tool!");
      return PLUS_FAIL;
    }

    BufferItemUidType fromItemUid(0);
    if (masterTool->GetItemUidFromTime(aTimestampFrom, fromItemUid) != ITEM_OK)
    {
      return 0;
    }

    BufferItemUidType toItemUid(0);
    if (masterTool->GetItemUidFromTime(aTimestampTo, toItemUid) != ITEM_OK)
    {
      return 0;
    }

    numberOfFrames = abs((int)(toItemUid - fromItemUid));
  }
  else if (this->GetFieldDataEnabled())
  {
    vtkPlusDataSource* firstSource = this->FieldDataSources.begin()->second;

    BufferItemUidType fromItemUid(0);
    if (firstSource->GetItemUidFromTime(aTimestampFrom, fromItemUid) != ITEM_OK)
    {
      return 0;
    }

    BufferItemUidType toItemUid(0);
    if (firstSource->GetItemUidFromTime(aTimestampTo, toItemUid) != ITEM_OK)
    {
      return 0;
    }

    numberOfFrames = abs((int)(toItemUid - fromItemUid));
  }

  return numberOfFrames + 1;
}

//----------------------------------------------------------------------------
double vtkPlusChannel::GetClosestTrackedFrameTimestampByTime(double time)
{
  if (this->GetVideoDataAvailable())
  {
    BufferItemUidType uid = 0;
    if (this->VideoSource->GetItemUidFromTime(time, uid) != ITEM_OK)
    {
      return UNDEFINED_TIMESTAMP;
    }
    double closestTimestamp = UNDEFINED_TIMESTAMP;
    if (this->VideoSource->GetTimeStamp(uid, closestTimestamp) != ITEM_OK)
    {
      return UNDEFINED_TIMESTAMP;
    }
    return closestTimestamp;
  }

  if (this->GetTrackingEnabled())
  {
    vtkPlusDataSource* masterTool = NULL;
    if (this->GetTimestampMasterTool(masterTool) != PLUS_SUCCESS)
    {
      // there is no active tool
      return UNDEFINED_TIMESTAMP;
    }
    BufferItemUidType uid = 0;
    if (masterTool->GetItemUidFromTime(time, uid) != ITEM_OK)
    {
      return UNDEFINED_TIMESTAMP;
    }
    double closestTimestamp = UNDEFINED_TIMESTAMP;
    if (masterTool->GetTimeStamp(uid, closestTimestamp) != ITEM_OK)
    {
      return UNDEFINED_TIMESTAMP;
    }
    return closestTimestamp;
  }

  if (this->GetFieldDataEnabled())
  {
    vtkPlusDataSource* firstSource = this->FieldDataSources.begin()->second;
    BufferItemUidType uid = 0;
    if (firstSource->GetItemUidFromTime(time, uid) != ITEM_OK)
    {
      return UNDEFINED_TIMESTAMP;
    }
    double closestTimestamp = UNDEFINED_TIMESTAMP;
    if (firstSource->GetTimeStamp(uid, closestTimestamp) != ITEM_OK)
    {
      return UNDEFINED_TIMESTAMP;
    }
    return closestTimestamp;
  }

  // neither tracker, nor video, nor field data available
  return UNDEFINED_TIMESTAMP;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetBrightnessFrameSize(FrameSizeType& aDim)
{
  aDim = this->BrightnessFrameSize;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
vtkImageData* vtkPlusChannel::GetBrightnessOutput()
{
  vtkImageData* resultImage = this->BlankImage;
  if (!this->HasVideoSource())
  {
    return resultImage;
  }

  if (this->VideoSource->GetLatestStreamBufferItem(&this->BrightnessOutputTrackedFrame) != ITEM_OK)
  {
    LOG_DEBUG("No video data available yet, return blank frame");
  }
  else if (this->BrightnessOutputTrackedFrame.GetFrame().GetImageType() == US_IMG_BRIGHTNESS)
  {
    // B-mode image already, just return as is
    resultImage = this->BrightnessOutputTrackedFrame.GetFrame().GetImage();
  }
  else if (this->BrightnessOutputTrackedFrame.GetFrame().GetImageType() == US_IMG_RGB_COLOR)
  {
    // Color image (Doppler, etc.), just return as is
    resultImage = this->BrightnessOutputTrackedFrame.GetFrame().GetImage();
  }
  else if (this->RfProcessor != NULL)
  {
    // RF frame, convert to B-mode frame
    this->RfProcessor->SetRfFrame(this->BrightnessOutputTrackedFrame.GetFrame().GetImage(), this->BrightnessOutputTrackedFrame.GetFrame().GetImageType());
    resultImage = this->RfProcessor->GetBrightnessScanConvertedImage();

    // RF processing parameters were used, so save them into the config file
    this->SaveRfProcessingParameters = true;
  }
  else
  {
    return NULL;
  }

  int* resultExtent = resultImage->GetExtent();
  this->BrightnessFrameSize[0] = resultExtent[1] - resultExtent[0] + 1;
  this->BrightnessFrameSize[1] = resultExtent[3] - resultExtent[2] + 1;
  this->BrightnessFrameSize[2] = resultExtent[5] - resultExtent[4] + 1;

  return resultImage;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetCustomAttribute(const std::string& attributeId, std::string& output) const
{
  if (this->CustomAttributes.find(attributeId) != this->CustomAttributes.end())
  {
    output = this->CustomAttributes.find(attributeId)->second;
    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetCustomAttributeMap(CustomAttributeMap& output) const
{
  for (CustomAttributeMapConstIterator it = this->CustomAttributes.begin(); it != this->CustomAttributes.end(); ++it)
  {
    output[it->first] = it->second;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::SetCustomAttribute(const std::string& attributeId, const std::string& value)
{
  std::string tempValue;
  if (this->GetCustomAttribute(attributeId, tempValue) == PLUS_SUCCESS)
  {
    LOG_WARNING("Attribute: " << attributeId << " will be overwritten with value: " << value);
  }
  this->CustomAttributes[attributeId] = value;

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GenerateDataAcquisitionReport(vtkPlusHTMLGenerator* htmlReport)
{
#ifdef PLUS_RENDERING_ENABLED
  if (htmlReport == NULL)
  {
    LOG_ERROR("Caller should define HTML report generator before report generation!");
    return PLUS_FAIL;
  }

  vtkSmartPointer<vtkTable> timestampReportTable = vtkSmartPointer<vtkTable>::New();

  std::string reportText = std::string("Device: ") + this->GetOwnerDevice()->GetDeviceId() + " - Channel: " + this->GetChannelId();
  htmlReport->AddText(reportText.c_str(), vtkPlusHTMLGenerator::H1);

  std::string deviceAndChannelName = std::string(this->GetOwnerDevice()->GetDeviceId()) + "-" + this->GetChannelId();

  int imageSize[2] = {800, 400};

  // Video data
  vtkPlusDataSource* videoSource = NULL;
  if (this->GetVideoSource(videoSource) == PLUS_SUCCESS && videoSource != NULL)
  {
    if (videoSource->GetTimeStampReportTable(timestampReportTable) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to get timestamp report table from video buffer!");
      return PLUS_FAIL;
    }

    std::string reportFile = vtkPlusConfig::GetInstance()->GetOutputPath(vtkPlusConfig::GetInstance()->GetApplicationStartTimestamp() + "-" + deviceAndChannelName + "-VideoBufferTimestamps.txt");
    PlusPlotter::WriteTableToFile(*timestampReportTable, reportFile.c_str());

    htmlReport->AddText("Video data", vtkPlusHTMLGenerator::H2);
    std::string imageFilePath = htmlReport->AddImageAutoFilename(std::string(deviceAndChannelName + "VideoBufferTimestamps.png").c_str(), "Video Data Acquisition Analysis");
    PlusPlotter::WriteLineChartToFile("Frame index", "Timestamp (s)", *timestampReportTable, 0, 1, 2, imageSize, imageFilePath.c_str());

    htmlReport->AddHorizontalLine();
  }

  // Tracker tools
  for (DataSourceContainerConstIterator it = this->GetToolsStartIterator(); it != this->GetToolsEndIterator(); ++it)
  {
    vtkPlusDataSource* tool = it->second;

    if (tool->GetTimeStampReportTable(timestampReportTable) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to get timestamp report table from tool '" << tool->GetId() << "' buffer!");
      return PLUS_FAIL;
    }

    reportText =  std::string("Tracking data - ") + tool->GetId();
    htmlReport->AddText(reportText.c_str(), vtkPlusHTMLGenerator::H2);
    std::string imageFilePath = htmlReport->AddImageAutoFilename(std::string(deviceAndChannelName + "-" + tool->GetId() + "-TrackerBufferTimestamps.png").c_str(), reportText.c_str());
    PlusPlotter::WriteLineChartToFile("Frame index", "Timestamp (s)", *timestampReportTable, 0, 1, 2, imageSize, imageFilePath.c_str());

    std::string reportFile = vtkPlusConfig::GetInstance()->GetOutputPath(vtkPlusConfig::GetInstance()->GetApplicationStartTimestamp() + "-" + deviceAndChannelName + "-" + tool->GetId() + "TrackerBufferTimestamps.txt");
    PlusPlotter::WriteTableToFile(*timestampReportTable, reportFile.c_str());

    htmlReport->AddHorizontalLine();
  }

  // Field data tools
  for (DataSourceContainerConstIterator it = this->GetFieldDataSourcesStartIterator(); it != this->GetFieldDataSourcesEndIterator(); ++it)
  {
    vtkPlusDataSource* aSource = it->second;

    if (aSource->GetTimeStampReportTable(timestampReportTable) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to get timestamp report table from source '" << aSource->GetId() << "' buffer!");
      return PLUS_FAIL;
    }

    reportText =  std::string("Field data - ") + aSource->GetId();
    htmlReport->AddText(reportText.c_str(), vtkPlusHTMLGenerator::H2);
    std::string imageFilePath = htmlReport->AddImageAutoFilename(std::string(deviceAndChannelName + "-" + aSource->GetId() + "-FieldDataBufferTimestamps.png").c_str(), reportText.c_str());
    PlusPlotter::WriteLineChartToFile("Frame index", "Timestamp (s)", *timestampReportTable, 0, 1, 2, imageSize, imageFilePath.c_str());

    std::string reportFile = vtkPlusConfig::GetInstance()->GetOutputPath(vtkPlusConfig::GetInstance()->GetApplicationStartTimestamp() + "-" + deviceAndChannelName + "-" + aSource->GetId() + "FieldDataBufferTimestamps.txt");
    PlusPlotter::WriteTableToFile(*timestampReportTable, reportFile.c_str());

    htmlReport->AddHorizontalLine();
  }

  return PLUS_SUCCESS;
#else
  LOG_WARNING("Cannot generate report when VTK_RENDERING_BACKEND is None!");
  return PLUS_FAIL;
#endif
}

//-----------------------------------------------------------------------------
bool vtkPlusChannel::IsVideoSource3D() const
{
  if (this->HasVideoSource())
  {
    vtkPlusDataSource* source(NULL);
    this->GetVideoSource(source);
    if (source->GetNumberOfItems() <= 0)
    {
      return false;
    }
    StreamBufferItem item;
    source->GetLatestStreamBufferItem(&item);
    int dims[3] = {0, 0, 0};
    item.GetFrame().GetImage()->GetDimensions(dims);
    if (dims[2] > 1)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
}