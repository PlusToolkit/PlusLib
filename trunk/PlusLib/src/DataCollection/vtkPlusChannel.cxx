/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkPlusBuffer.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkPlusChannel, "$Revision: 1.0$");
vtkStandardNewMacro(vtkPlusChannel);

//----------------------------------------------------------------------------
// If a frame cannot be retrieved from the device buffers (because it was overwritten by new frames)
// then we skip a SAMPLING_SKIPPING_MARGIN_SEC long period to allow the application to catch up.
// This time should be long enough to comfortably retrieve a frame from the buffer.
static const double SAMPLING_SKIPPING_MARGIN_SEC=0.1; 

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

  // Create a blank image, it will be used as output if frames are not available
  this->BlankImage->SetExtent( 0, this->BrightnessFrameSize[0] -1, 0, this->BrightnessFrameSize[1] - 1, 0, 0);
  this->BlankImage->SetScalarTypeToUnsignedChar();
  this->BlankImage->SetNumberOfScalarComponents(1); 
  this->BlankImage->AllocateScalars(); 
  unsigned long memorysize = this->BrightnessFrameSize[0] * this->BrightnessFrameSize[1] * this->BlankImage->GetScalarSize(); 
  memset(this->BlankImage->GetScalarPointer(), 0, memorysize);
}

//----------------------------------------------------------------------------
vtkPlusChannel::~vtkPlusChannel(void)
{
  this->VideoSource = NULL;
  Tools.clear();

  this->SetOwnerDevice(NULL);

  DELETE_IF_NOT_NULL(this->BlankImage);

  DELETE_IF_NOT_NULL(this->RfProcessor);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::ReadConfiguration( vtkXMLDataElement* aChannelElement, bool RequireRfElementInDeviceSetConfiguration, bool RequireImageOrientationInChannelConfiguration )
{
  // Read the stream element, build the stream
  // If there are references to tools, request them from the owner device and keep a reference to them here
  const char * id = aChannelElement->GetAttribute("Id");
  if( id == NULL )
  {
    LOG_ERROR("No stream id defined. It is required for all streams.");
    return PLUS_FAIL;
  }
  this->SetChannelId(id);

  vtkPlusDataSource* aSource = NULL;
  for ( int i = 0; i < aChannelElement->GetNumberOfNestedElements(); i++ )
  {
    vtkXMLDataElement* aSourceElement = aChannelElement->GetNestedElement(i); 
    if ( STRCASECMP(aSourceElement->GetName(), "DataSource") != 0 )
    {
      // if this is not an data source element, skip it
      continue; 
    }

    const char* id = aSourceElement->GetAttribute("Id");
    if( id == NULL )
    {
      LOG_WARNING("No field \"Id\" defined in the source element " << this->GetChannelId() << ". Unable to add it to the channel.");
      continue;
    }

    PlusTransformName idName(id, this->OwnerDevice->GetToolReferenceFrameName());
    if( this->OwnerDevice != NULL && this->OwnerDevice->GetDataSource(id, aSource) == PLUS_SUCCESS)
    {
      this->Tools[aSource->GetSourceId()] = aSource;
    }
    else if( this->OwnerDevice != NULL && this->OwnerDevice->GetDataSource(idName.GetTransformName().c_str(), aSource) == PLUS_SUCCESS )
    {

      this->Tools[aSource->GetSourceId()] = aSource;
    }
    else
    {
      LOG_ERROR("Unable to find data source with Id=\'" << id << "\'.");
      return PLUS_FAIL;
    }
  }

  if( aChannelElement->GetAttribute("VideoDataSourceId") != NULL && this->OwnerDevice->GetVideoSource(aChannelElement->GetAttribute("VideoDataSourceId"), aSource) == PLUS_SUCCESS )
  {
    this->VideoSource = aSource;
  }
  else if( aChannelElement->GetAttribute("VideoDataSourceId") != NULL )
  {
    LOG_ERROR("Unable to find video data source that matches Id: " << aChannelElement->GetAttribute("VideoDataSourceId"));
    return PLUS_FAIL;
  }

  vtkXMLDataElement* rfElement = aChannelElement->FindNestedElementWithName(vtkRfProcessor::GetRfProcessorTagName());
  if (rfElement != NULL)
  {
    this->RfProcessor = vtkRfProcessor::New();
    this->RfProcessor->ReadConfiguration(rfElement);
    this->SaveRfProcessingParameters = true;
  }
  else if( RequireRfElementInDeviceSetConfiguration )
  {
    LOG_ERROR("Unable to find RF processing sub-element in channel \'" << this->GetChannelId() << "\' configuration when it is required.");
    return PLUS_FAIL;
  }

  this->CustomAttributes.clear();
  for ( int i = 0; i < aChannelElement->GetNumberOfNestedElements(); i++ )
  {
    vtkXMLDataElement* attrElement = aChannelElement->GetNestedElement(i); 
    if ( STRCASECMP(attrElement->GetName(), "Attribute") != 0 )
    {
      continue; 
    }
    const char* id = attrElement->GetAttribute("Id");
    if( id == NULL )
    {
      LOG_ERROR("No idea in channel attribute for channel " << this->GetChannelId() );
      continue;
    }
    const char* value = attrElement->GetCharacterData();
    if( value == NULL )
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
PlusStatus vtkPlusChannel::WriteConfiguration( vtkXMLDataElement* aChannelElement )
{
  aChannelElement->SetAttribute("Id", this->GetChannelId());

  for ( int i = 0; i < aChannelElement->GetNumberOfNestedElements(); i++ )
  {
    vtkXMLDataElement* element = aChannelElement->GetNestedElement(i); 
    if ( STRCASECMP(element->GetName(), "DataSource") != 0 )
    {
      continue; 
    }
    if( element->GetAttribute("Type") != NULL && STRCASECMP(element->GetAttribute("Type"), "Video") == 0 )
    {
      if( this->HasVideoSource() )
      {
        this->VideoSource->WriteConfiguration(element);
      }
    }
    else if( element->GetAttribute("Type") != NULL && STRCASECMP(element->GetAttribute("Type"), "Tool") == 0 )
    {
      vtkPlusDataSource* aTool = NULL;
      if( element->GetAttribute("Id") == NULL || this->GetTool(aTool, element->GetAttribute("Id")) != PLUS_SUCCESS )
      {
        LOG_ERROR("Unable to retrieve tool when saving config.");
        return PLUS_FAIL;
      }
      aTool->WriteCompactConfiguration(element);
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
PlusStatus vtkPlusChannel::GetVideoSource( vtkPlusDataSource*& aVideoSource )
{
  if( this->HasVideoSource() )
  {
    aVideoSource = this->VideoSource;
    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetVideoSource( vtkPlusDataSource*& aVideoSource ) const
{
  if( this->HasVideoSource() )
  {
    aVideoSource = this->VideoSource;
    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetTool(vtkPlusDataSource*& aTool, const char* toolName )
{
  if( toolName == NULL )
  {
    LOG_ERROR("Null toolname sent to stream tool request.");
    return PLUS_FAIL;
  }

  for( DataSourceContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    if( STRCASECMP(toolName, it->second->GetSourceId()) == 0 )
    {
      aTool = it->second;
      return PLUS_SUCCESS;
    }
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
DataSourceContainerConstIterator vtkPlusChannel::GetToolsStartConstIterator() const
{
  return this->Tools.begin();
}

//----------------------------------------------------------------------------
DataSourceContainerConstIterator vtkPlusChannel::GetToolsEndConstIterator() const
{
  return this->Tools.end();
}

//----------------------------------------------------------------------------
DataSourceContainerIterator vtkPlusChannel::GetToolsStartIterator()
{
  return this->Tools.begin();
}

//----------------------------------------------------------------------------
DataSourceContainerIterator vtkPlusChannel::GetToolsEndIterator()
{
  return this->Tools.end();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::AddTool(vtkPlusDataSource* aTool )
{
  if( aTool == NULL )
  {
    LOG_ERROR("Trying to add null tool to stream.");
    return PLUS_FAIL;
  }

  for( DataSourceContainerConstIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    if( it->second == aTool )
    {
      // Yes, compare pointers
      return PLUS_SUCCESS;
    }
  }

  this->Tools[aTool->GetSourceId()] = aTool;
  this->Tools[aTool->GetSourceId()]->Register(this);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::RemoveTool( const char* toolName )
{
  if( toolName == NULL )
  {
    LOG_ERROR("Trying to remove null toolname from stream.");
    return PLUS_FAIL;
  }

  for( DataSourceContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    if( STRCASECMP(it->second->GetSourceId(), toolName) == 0 )
    {
      this->Tools.erase(it);
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
PlusStatus vtkPlusChannel::Clear()
{
  for( DataSourceContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    (it->second)->GetBuffer()->Clear();
  }
  if( this->VideoSource != NULL )
  {
    this->VideoSource->GetBuffer()->Clear();
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetLatestTimestamp(double& aTimestamp) const
{
  aTimestamp = 0;

  if( this->HasVideoSource() )
  {
    if( this->VideoSource->GetBuffer()->GetLatestTimeStamp(aTimestamp) != ITEM_OK )
    {
      LOG_ERROR("Unable to retrieve latest timestamp from the video source buffer.");
    }
  }

  for( DataSourceContainerConstIterator it = this->GetToolsStartConstIterator(); it != this->GetToolsEndConstIterator(); ++it)
  {
    vtkPlusDataSource* aTool = it->second;
    double timestamp;
    if( aTool->GetBuffer()->GetLatestTimeStamp(timestamp) == ITEM_OK )
    {
      if( timestamp > aTimestamp )
      {
        aTimestamp = timestamp;
      }
    }
  }

  return aTimestamp != 0 ? PLUS_SUCCESS : PLUS_FAIL;
}

//----------------------------------------------------------------------------
void vtkPlusChannel::ShallowCopy( const vtkPlusChannel& aChannel )
{
  this->Clear();

  vtkPlusDataSource* aSource = NULL;
  if( aChannel.HasVideoSource() && aChannel.GetVideoSource(aSource) )
  {
    this->VideoSource = aSource;
  }
  for( DataSourceContainerConstIterator it = aChannel.GetToolsStartConstIterator(); it != aChannel.GetToolsEndConstIterator(); ++it)
  {
    if( this->AddTool(it->second) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to add a tool when shallow copying a stream.");
      continue;
    }
  }
}

//----------------------------------------------------------------------------
bool vtkPlusChannel::HasVideoSource() const
{
  return this->VideoSource != NULL;
}

//----------------------------------------------------------------------------
void vtkPlusChannel::SetVideoSource( vtkPlusDataSource* aSource )
{
  this->VideoSource = aSource;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetTrackedFrame( double timestamp, TrackedFrame& aTrackedFrame, bool enableImageData/*=true*/ )
{
  int numberOfErrors(0);
  double synchronizedTimestamp(0);

  // Get frame UID
  if( this->HasVideoSource() && enableImageData )
  {
    BufferItemUidType frameUID = 0; 
    ItemStatus status = this->VideoSource->GetBuffer()->GetItemUidFromTime(timestamp, frameUID); 
    if ( status != ITEM_OK )
    {
      if ( status == ITEM_NOT_AVAILABLE_ANYMORE )
      {
        LOG_ERROR("Couldn't get frame UID from time (" << std::fixed << timestamp <<
          ") - item not available anymore!"); 
      }
      else if ( status == ITEM_NOT_AVAILABLE_YET) 
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
    if ( this->VideoSource->GetBuffer()->GetStreamBufferItem(frameUID, &CurrentStreamBufferItem) != ITEM_OK )
    {
      LOG_ERROR("Couldn't get video buffer item by frame UID: " << frameUID); 
      return PLUS_FAIL; 
    }

    // Copy frame 
    PlusVideoFrame frame = CurrentStreamBufferItem.GetFrame(); 
    aTrackedFrame.SetImageData(frame);

    // Copy all custom fields
    StreamBufferItem::FieldMapType fieldMap = CurrentStreamBufferItem.GetCustomFrameFieldMap();
    StreamBufferItem::FieldMapType::iterator fieldIterator;
    for (fieldIterator = fieldMap.begin(); fieldIterator != fieldMap.end(); fieldIterator++)
    {
      aTrackedFrame.SetCustomFrameField((*fieldIterator).first, (*fieldIterator).second);
    }

    synchronizedTimestamp = CurrentStreamBufferItem.GetTimestamp(this->VideoSource->GetBuffer()->GetLocalTimeOffsetSec());
  }

  if( synchronizedTimestamp == 0 )
  {
    synchronizedTimestamp = timestamp;
  }

  // Add main tool timestamp
  aTrackedFrame.SetTimestamp(synchronizedTimestamp);

  for (DataSourceContainerConstIterator it = this->GetToolsStartIterator(); it != this->GetToolsEndIterator(); ++it)
  {
    vtkPlusDataSource* aTool = it->second;
    PlusTransformName toolTransformName( aTool->GetSourceId() ); 
    if ( !toolTransformName.IsValid() )
    {
      LOG_ERROR("Tool transform name is invalid!"); 
      numberOfErrors++; 
      continue; 
    }

    StreamBufferItem bufferItem;
    ItemStatus result = aTool->GetBuffer()->GetStreamBufferItemFromTime(synchronizedTimestamp, &bufferItem, vtkPlusBuffer::INTERPOLATED );
    if ( result != ITEM_OK )
    {
      double latestTimestamp(0); 
      if ( aTool->GetBuffer()->GetLatestTimeStamp(latestTimestamp) != ITEM_OK )
      {
        LOG_ERROR("Failed to get latest timestamp!");
        numberOfErrors++;
      }

      double oldestTimestamp(0); 
      if ( aTool->GetBuffer()->GetOldestTimeStamp(oldestTimestamp) != ITEM_OK )
      {
        LOG_ERROR("Failed to get oldest timestamp!");
        numberOfErrors++; 
      }

      LOG_ERROR(aTool->GetSourceId() << ": Failed to get tracker item from buffer by time: " << std::fixed << synchronizedTimestamp << " (Latest timestamp: " << latestTimestamp << "   Oldest timestamp: " << oldestTimestamp << ")."); 
      numberOfErrors++; 
      continue; 
    }

    vtkSmartPointer<vtkMatrix4x4> dMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    if (bufferItem.GetMatrix(dMatrix) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to get matrix from buffer item for tool " << aTool->GetSourceId() ); 
      numberOfErrors++; 
      continue; 
    }

    if ( aTrackedFrame.SetCustomFrameTransform(toolTransformName, dMatrix) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to set transform for tool " << aTool->GetSourceId() ); 
      numberOfErrors++; 
      continue; 
    }

    if ( aTrackedFrame.SetCustomFrameTransformStatus(toolTransformName, vtkPlusDevice::ConvertToolStatusToTrackedFrameFieldStatus(bufferItem.GetStatus()) ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to set transform status for tool " << aTool->GetSourceId() ); 
      numberOfErrors++; 
      continue; 
    }

    // Copy all custom fields
    StreamBufferItem::FieldMapType fieldMap = bufferItem.GetCustomFrameFieldMap();
    StreamBufferItem::FieldMapType::iterator fieldIterator;
    for (fieldIterator = fieldMap.begin(); fieldIterator != fieldMap.end(); fieldIterator++)
    {
      aTrackedFrame.SetCustomFrameField((*fieldIterator).first, (*fieldIterator).second);
    }

    synchronizedTimestamp = bufferItem.GetTimestamp(aTool->GetBuffer()->GetLocalTimeOffsetSec());
  }

  // Copy frame timestamp   
  aTrackedFrame.SetTimestamp(synchronizedTimestamp);

  return (numberOfErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL ); 
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetTrackedFrame(TrackedFrame* trackedFrame)
{
  LOG_TRACE("vtkPlusDevice::GetTrackedFrame - TrackedFrame"); 

  double mostRecentFrameTimestamp(0);
  if (this->GetMostRecentTimestamp(mostRecentFrameTimestamp) != PLUS_SUCCESS) 
  {
    LOG_ERROR("Failed to get most recent timestamp from the buffer!"); 
    return PLUS_FAIL;
  }

  if (trackedFrame==NULL)
  {
    LOG_ERROR("Failed to get most recent tracked frame: trackedFrame pointer is invalid"); 
    return PLUS_FAIL;
  }

  return this->GetTrackedFrame(mostRecentFrameTimestamp, *trackedFrame);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetTrackedFrameList( double& aTimestampFrom, vtkTrackedFrameList* aTrackedFrameList, int aMaxNumberOfFramesToAdd )
{
  LOG_TRACE("vtkPlusDevice::GetTrackedFrameList(" << aTimestampFrom << ", " << aMaxNumberOfFramesToAdd << ")"); 

  if ( aTrackedFrameList == NULL )
  {
    LOG_ERROR("Unable to get tracked frame list - output tracked frame list is NULL!"); 
    return PLUS_FAIL; 
  }

  // If the buffer is empty then don't display an error just return without adding any items to the output tracked frame list
  if ( this->GetVideoDataAvailable() )
  {
    if ( this->VideoSource->GetBuffer()->GetNumberOfItems() == 0 )
    {
      LOG_DEBUG("vtkDataCollector::GetTrackedFrameList: the video buffer is empty, no items will be returned"); 
      return PLUS_SUCCESS;
    }
  }

  if ( this->GetTrackingEnabled() )
  {
    // Get the first tool
    vtkPlusDataSource* firstActiveTool = NULL; 
    if ( this->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get first active tool"); 
      return PLUS_FAIL; 
    }
    if ( firstActiveTool->GetBuffer()->GetNumberOfItems() == 0 )
    {
      LOG_DEBUG("vtkDataCollector::GetTrackedFrameList: the tracker buffer is empty, no items will be returned"); 
      return PLUS_SUCCESS;
    }
  }

  // Get latest and oldest timestamp
  double mostRecentTimestamp(0); 
  if ( this->GetMostRecentTimestamp(mostRecentTimestamp) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to get most recent timestamp!"); 
    return PLUS_FAIL; 
  }

  PlusStatus status = PLUS_SUCCESS; 
  double oldestTimestamp(0); 
  if ( this->GetOldestTimestamp(oldestTimestamp) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get oldest timestamp from buffer!"); 
    return PLUS_FAIL; 
  }

  // If zero or negative starting timestamp is specified we assume that the acquisition starts from the most recent frame
  if ( aTimestampFrom <= 0.0)
  {
    aTimestampFrom = mostRecentTimestamp; 
  }

  if (aTimestampFrom<oldestTimestamp)
  {
    LOG_ERROR("Items are requested from "<<aTimestampFrom<<", but the oldest available data is acquired at "<<oldestTimestamp);
    return PLUS_FAIL;
  }

  if ( aMaxNumberOfFramesToAdd > 0 ) 
  {
    if ( this->GetVideoDataAvailable() )
    {
      BufferItemUidType mostRecentVideoUid = 0; 
      if ( this->VideoSource->GetBuffer()->GetItemUidFromTime(mostRecentTimestamp, mostRecentVideoUid) != ITEM_OK )
      {
        LOG_ERROR("Failed to get video buffer item by timestamp " << mostRecentTimestamp);
        return PLUS_FAIL;
      }
      BufferItemUidType videoUidFrom = 0; 
      if ( this->VideoSource->GetBuffer()->GetItemUidFromTime(aTimestampFrom, videoUidFrom) != ITEM_OK )
      {
        LOG_ERROR("Failed to get video buffer item by timestamp " << aTimestampFrom);
        return PLUS_FAIL;
      }

      BufferItemUidType firstVideoUidToAdd = videoUidFrom;
      if ( mostRecentVideoUid - videoUidFrom + 1 > aMaxNumberOfFramesToAdd )
      {
        // More frames are requested than the maximum allowed frames to add        
        firstVideoUidToAdd = mostRecentVideoUid - aMaxNumberOfFramesToAdd + 1; // +1: because most recent is needed too
      }
      else
      {
        LOG_TRACE("Number of frames in the video buffer is less than maxNumberOfFramesToAdd (more data is allowed to be recorded than it was provided by the data sources)"); 
      }

      if ( this->VideoSource->GetBuffer()->GetTimeStamp(firstVideoUidToAdd, aTimestampFrom ) != ITEM_OK )
      {
        LOG_ERROR("Failed to get video buffer timestamp from UID: " << firstVideoUidToAdd ); 
        return PLUS_FAIL; 
      }
    }
    else if ( this->GetTrackingEnabled() )
    {
      // Get the first tool
      vtkPlusDataSource* firstActiveTool = NULL; 
      if ( this->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get tracked frame list - there is no active tool!"); 
        return PLUS_FAIL; 
      }
      vtkPlusBuffer* trackerBuffer = firstActiveTool->GetBuffer(); 
      if ( trackerBuffer == NULL )
      {
        LOG_ERROR("Failed to get first active tool!"); 
        return PLUS_FAIL; 
      }

      BufferItemUidType mostRecentTrackerUid = 0; 
      if ( trackerBuffer->GetItemUidFromTime(mostRecentTimestamp, mostRecentTrackerUid) != ITEM_OK )
      {
        LOG_ERROR("Failed to get tracked buffer item by timestamp " << mostRecentTimestamp);
        return PLUS_FAIL;
      }
      BufferItemUidType trackerUidFrom = 0; 
      ItemStatus status = trackerBuffer->GetItemUidFromTime(aTimestampFrom, trackerUidFrom);
      if ( status != ITEM_OK )
      {
        switch(status)
        {
        case ITEM_NOT_AVAILABLE_YET:
          LOG_ERROR("Failed to get tracker buffer item by timestamp " << aTimestampFrom << ". Item not available yet.");
          break;
        case ITEM_NOT_AVAILABLE_ANYMORE:
          LOG_ERROR("Failed to get tracker buffer item by timestamp " << aTimestampFrom << ". Item not available anymore.");
          break;
        case ITEM_UNKNOWN_ERROR:
        default:
          LOG_ERROR("Failed to get tracker buffer item by timestamp " << aTimestampFrom);
          break;
        }
        return PLUS_FAIL;
      }

      BufferItemUidType firstTrackerUidToAdd = trackerUidFrom;
      if ( mostRecentTrackerUid - trackerUidFrom + 1 > aMaxNumberOfFramesToAdd )
      {
        // More frames are requested than the maximum allowed frames to add        
        firstTrackerUidToAdd = mostRecentTrackerUid - aMaxNumberOfFramesToAdd + 1; // +1: because most recent is needed too
      }
      else
      {
        LOG_TRACE("Number of frames in the tracker buffer is less than maxNumberOfFramesToAdd (more data is allowed to be recorded than it was provided by the data sources)"); 
      }

      if ( trackerBuffer->GetTimeStamp(firstTrackerUidToAdd, aTimestampFrom ) != ITEM_OK )
      {
        LOG_ERROR("Failed to get tracker buffer timestamp from UID: " << firstTrackerUidToAdd ); 
        return PLUS_FAIL; 
      }
    }
  }

  // Check input frameTimestamp to be in a valid range 
  if (aTimestampFrom > mostRecentTimestamp )
  {
    aTimestampFrom = mostRecentTimestamp; 
  }
  else if ( aTimestampFrom < oldestTimestamp )
  {
    aTimestampFrom = oldestTimestamp; 
  }

  // Determine how many frames to add
  int numberOfFramesSinceTimestamp = GetNumberOfFramesBetweenTimestamps(aTimestampFrom, mostRecentTimestamp);

  int numberOfFramesToAdd = 0;
  if (aMaxNumberOfFramesToAdd > 0)
  {
    numberOfFramesToAdd = std::min( aMaxNumberOfFramesToAdd, numberOfFramesSinceTimestamp );
  }
  else
  {
    numberOfFramesToAdd = numberOfFramesSinceTimestamp;
  }

  LOG_TRACE("Number of added frames: " << numberOfFramesToAdd << " out of " << numberOfFramesSinceTimestamp);

  // If we couldn't find any frames (or one of the items were invalid) 
  // set the timestamp to the most recent one
  if ( numberOfFramesToAdd == 0 )
  {
    aTimestampFrom = mostRecentTimestamp; 
  }

  for (int i=0; i<numberOfFramesToAdd; ++i)
  {
    // Get tracked frame from buffer
    TrackedFrame trackedFrame; 

    if ( this->GetTrackedFrame(aTimestampFrom, trackedFrame) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to get tracked frame by time: " << std::fixed << aTimestampFrom ); 
      return PLUS_FAIL;
    }

    // Add tracked frame to the list 
    if ( aTrackedFrameList->AddTrackedFrame(&trackedFrame, vtkTrackedFrameList::SKIP_INVALID_FRAME) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to add tracked frame to the list!" ); 
      return PLUS_FAIL; 
    }

    // Get next timestamp
    if ( this->GetVideoDataAvailable() && i < numberOfFramesToAdd - 1 )
    {
      BufferItemUidType videoUid(0); 
      if ( this->VideoSource->GetBuffer()->GetItemUidFromTime(aTimestampFrom, videoUid) != ITEM_OK )
      {
        LOG_ERROR("Failed to get video buffer item UID from time: " << std::fixed << aTimestampFrom ); 
        return PLUS_FAIL; 
      }

      if ( videoUid >= this->VideoSource->GetBuffer()->GetLatestItemUidInBuffer() )
      {
        LOG_WARNING("Requested video uid (" << videoUid+1 << ") is not in the buffer yet!");
        break;
      }

      // Get the timestamp of the next item in the buffer
      if ( this->VideoSource->GetBuffer()->GetTimeStamp(++videoUid, aTimestampFrom) != ITEM_OK )
      {
        LOG_ERROR("Unable to get timestamp from video buffer by UID: " << videoUid); 
        return PLUS_FAIL;
      }
    }
    else if ( this->GetTrackingEnabled() && i < numberOfFramesToAdd - 1 )
    {
      // Get the first tool
      vtkPlusDataSource* firstActiveTool = NULL; 
      if ( this->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get tracked frame list - there is no active tool!"); 
        return PLUS_FAIL; 
      }

      vtkPlusBuffer* trackerBuffer = firstActiveTool->GetBuffer(); 
      if ( trackerBuffer == NULL )
      {
        LOG_ERROR("Failed to get first active tool!"); 
        return PLUS_FAIL;
      }

      BufferItemUidType trackerUid(0); 
      if ( trackerBuffer->GetItemUidFromTime(aTimestampFrom, trackerUid) != ITEM_OK )
      {
        LOG_ERROR("Failed to get tracker buffer item UID from time: " << std::fixed << aTimestampFrom ); 
        return PLUS_FAIL; 
      }

      if ( trackerUid >= trackerBuffer->GetLatestItemUidInBuffer() )
      {
        LOG_ERROR("Requested tracker uid (" << trackerUid+1 << ") is not in the buffer yet!");
        break;
      }

      // Get the timestamp of the next item in the buffer
      if ( trackerBuffer->GetTimeStamp(++trackerUid, aTimestampFrom) != ITEM_OK )
      {
        LOG_WARNING("Unable to get timestamp from tracker buffer by UID: " << trackerUid); 
        return PLUS_FAIL;
      }
    }
  }

  return status; 
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetTrackedFrameListSampled(double &aTimestampOfLastFrameAlreadyGot, double& aTimestampOfNextFrameToBeAdded, vtkTrackedFrameList* aTrackedFrameList, double aSamplingPeriodSec, double maxTimeLimitSec/*=-1*/)
{
  LOG_TRACE("vtkDataCollector::GetTrackedFrameListSampled: aTimestampOfLastFrameAlreadyGot="<<aTimestampOfLastFrameAlreadyGot<<", aTimestampOfNextFrameToBeAdded="<<aTimestampOfNextFrameToBeAdded<<", aSamplingPeriodSec="<< aSamplingPeriodSec); 

  if ( aTrackedFrameList == NULL )
  {
    LOG_ERROR("vtkPlusChannel::GetTrackedFrameListSampled failed: unable to get tracked frame list. Output tracked frame list is NULL."); 
    return PLUS_FAIL; 
  }

  double startTimeSec = vtkAccurateTimer::GetSystemTime();

  double mostRecentTimestamp(0); 
  if ( this->GetMostRecentTimestamp(mostRecentTimestamp) != PLUS_SUCCESS )
  {
    LOG_ERROR("vtkPlusChannel::GetTrackedFrameListSampled failed: unable to get most recent timestamp. Probably no frames have been acquired yet."); 
    return PLUS_FAIL; 
  }

  PlusStatus status=PLUS_SUCCESS;
  // Add frames to input trackedFrameList
  for (; aTimestampOfNextFrameToBeAdded <= mostRecentTimestamp; aTimestampOfNextFrameToBeAdded += aSamplingPeriodSec)
  {       
    // If the time that is allowed for adding of frames is expired then stop the processing now
    if (maxTimeLimitSec>0 && vtkAccurateTimer::GetSystemTime() - startTimeSec > maxTimeLimitSec)
    {
      LOG_DEBUG("Reached maximum time that is allowed for sampling frames");
      break;
    }

    // Make sure the next frame to be added is still in the buffer:
    // If the frame will be removed from the buffer really soon, then jump ahead in time (and skip some frames),
    // instead of trying to retrieve from the buffer (and then fail because the frame is not available anymore).
    double oldestTimestamp=0;
    if ( this->GetOldestTimestamp(oldestTimestamp) != PLUS_SUCCESS )
    {
      LOG_ERROR("vtkPlusChannel::GetTrackedFrameListSampled: Failed to get oldest timestamp from buffer. Probably no frames have been acquired yet."); 
      return PLUS_FAIL; 
    }
    if (aTimestampOfNextFrameToBeAdded < oldestTimestamp+SAMPLING_SKIPPING_MARGIN_SEC)
    {      
      double newTimestampOfFrameToBeAdded = oldestTimestamp + SAMPLING_SKIPPING_MARGIN_SEC;
      LOG_WARNING("vtkPlusChannel::GetTrackedFrameListSampled: Frames in the buffer are not available any more at time: " << std::fixed << aTimestampOfNextFrameToBeAdded <<". Skipping " << newTimestampOfFrameToBeAdded-aTimestampOfNextFrameToBeAdded << " seconds from the recording to catch up. Increase the buffer size or decrease the acquisition rate to avoid this situation.");
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
    if ( aTimestampOfLastFrameAlreadyGot != UNDEFINED_TIMESTAMP && closestTimestamp <= aTimestampOfLastFrameAlreadyGot )
    {
      // This frame has been already added. Don't spend time with retrieving this frame, just jump to the next
      continue;
    }
    // Get tracked frame from buffer (actually copies pixel and field data)
    TrackedFrame trackedFrame; 
    if ( GetTrackedFrame(closestTimestamp, trackedFrame) != PLUS_SUCCESS )
    {
      LOG_WARNING("vtkPlusChannel::GetTrackedFrameListSampled: Unable retrieve frame from the devices for time: " << std::fixed << aTimestampOfNextFrameToBeAdded <<", probably the item is not available in the buffers anymore. Frames may be lost."); 
      continue;
    }
    aTimestampOfLastFrameAlreadyGot=trackedFrame.GetTimestamp();
    // Add tracked frame to the list 
    if ( aTrackedFrameList->AddTrackedFrame(&trackedFrame, vtkTrackedFrameList::SKIP_INVALID_FRAME) != PLUS_SUCCESS )
    {
      LOG_ERROR("vtkPlusChannel::GetTrackedFrameListSampled: Unable to add tracked frame to the list" ); 
      status=PLUS_FAIL; 
    }
  }


  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetOldestTimestamp(double &ts)
{
  LOG_TRACE("vtkPlusChannel::GetOldestTimestamp"); 
  ts=0;

  // ********************* video timestamp **********************
  double oldestVideoTimestamp(0); 
  if ( this->GetVideoDataAvailable() )
  {
    if ( this->VideoSource->GetBuffer()->GetOldestTimeStamp(oldestVideoTimestamp) != ITEM_OK )
    {
      LOG_WARNING("Failed to get oldest timestamp from video buffer!"); 
      return PLUS_FAIL; 
    }
  }

  // ********************* tracker timestamp **********************
  double oldestTrackerTimestamp(0);   
  if ( this->GetTrackingEnabled() )
  {    
    // Get the first tool
    vtkPlusDataSource* firstActiveTool = NULL; 
    if ( this->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get oldest timestamp from tracker buffer - there is no active tool!"); 
      return PLUS_FAIL; 
    }
    vtkPlusBuffer* trackerBuffer = firstActiveTool->GetBuffer(); 

    if ( trackerBuffer == NULL )
    {
      LOG_ERROR("Failed to get first active tool!"); 
      return PLUS_FAIL; 
    }

    BufferItemUidType uid = trackerBuffer->GetOldestItemUidInBuffer(); 
    // Get the oldest valid timestamp from the tracker buffer
    if ( trackerBuffer->GetTimeStamp(uid, oldestTrackerTimestamp ) != ITEM_OK )
    {
      LOG_WARNING("Unable to get timestamp from default tool tracker buffer with UID: " << uid); 
      return PLUS_FAIL;
    }
  }
  if ( !this->GetVideoDataAvailable() )
  {
    oldestVideoTimestamp = oldestTrackerTimestamp; 
  }

  if ( !this->GetTrackingEnabled() )
  {
    oldestTrackerTimestamp = oldestVideoTimestamp; 
  }

  // If the video timestamp is older than the tracker timestamp, then use the earliest video timestamp that comes after the first tracker timestamp
  if ( oldestVideoTimestamp < oldestTrackerTimestamp )
  {
    // Get the video timestamp that is closest to the oldest tracker timestamp 
    BufferItemUidType videoUid(0); 
    if ( this->VideoSource->GetBuffer()->GetItemUidFromTime(oldestTrackerTimestamp, videoUid) != ITEM_OK )
    {
      LOG_ERROR("Failed to get video buffer item UID from time: " << std::fixed << oldestVideoTimestamp ); 
      return PLUS_FAIL; 
    }
    if (oldestVideoTimestamp < oldestTrackerTimestamp)
    {
      // the closest video timestamp is still smaller than the first tracking data,
      // so we need the next video timestamp (that should have a timestamp that is larger than the first tracking data)
      if ( videoUid + 1 > this->VideoSource->GetBuffer()->GetLatestItemUidInBuffer() ) 
      {
        // the next video item does not exist, so there is no overlap between the tracking and video data
        LOG_ERROR("Failed to get oldest timestamp: no overlap between tracking and video data"); 
        return PLUS_FAIL; 
      }
      if ( this->VideoSource->GetBuffer()->GetTimeStamp(videoUid+1, oldestVideoTimestamp) != ITEM_OK )
      {
        LOG_ERROR("Failed to get video buffer timestamp from UID: " << videoUid); 
        return PLUS_FAIL; 
      }
      if ( oldestVideoTimestamp < oldestTrackerTimestamp )
      {
        LOG_ERROR("Failed to get oldest timestamp: no overlap between tracking and video data"); 
        return PLUS_FAIL; 
      }
    }    
  }
  
  ts = oldestVideoTimestamp; 
  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetMostRecentTimestamp(double &ts)
{
  LOG_TRACE("vtkPlusChannel::GetMostRecentTimestamp"); 
  ts=0;

  double latestVideoTimestamp(0); 
  // This can't check for data, only if there is a video source device...
  if ( this->GetVideoDataAvailable() )
  {
    // Get the most recent timestamp from the buffer
    if ( this->VideoSource->GetBuffer()->GetLatestTimeStamp(latestVideoTimestamp) != ITEM_OK )
    {
      LOG_WARNING("Unable to get latest timestamp from video buffer!"); 
      return PLUS_FAIL;
    }
  }

  double latestTrackerTimestamp(0); // the latest tracker timestamp that is available for all tools
  if ( this->GetTrackingEnabled() )
  {      
    bool mostRecentTrackerTimestampRetrieved=false;
    for( DataSourceContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
    {
      vtkPlusDataSource* tool=it->second;
      if (tool==NULL)
      {
        LOG_ERROR("Invalid tool " << it->first);
        continue;
      }
      vtkPlusBuffer* trackerBuffer = tool->GetBuffer(); 
      if ( trackerBuffer == NULL )
      {
        LOG_ERROR("Failed to get buffer of tool "<<it->first); 
        continue;
      }
      BufferItemUidType uid = trackerBuffer->GetLatestItemUidInBuffer(); 
      // Get the most recent valid timestamp from the tracker buffer
      double latestTrackerTimestampForCurrentTool=0;
      if ( trackerBuffer->GetTimeStamp(uid, latestTrackerTimestampForCurrentTool ) != ITEM_OK )
      {
        LOG_WARNING("Unable to get timestamp from default tool tracker buffer with UID: " << uid); 
        continue;
      }
      if (!mostRecentTrackerTimestampRetrieved)
      {
        // first tool
        latestTrackerTimestamp=latestTrackerTimestampForCurrentTool;
        mostRecentTrackerTimestampRetrieved=true;
      }
      else if (latestTrackerTimestampForCurrentTool<latestTrackerTimestamp)
      {
        latestTrackerTimestamp=latestTrackerTimestampForCurrentTool;
      }
    }

    if (!mostRecentTrackerTimestampRetrieved)
    {
      LOG_ERROR("Failed to get most recent timestamp from all the tracker tools"); 
      return PLUS_FAIL; 
    }

    // Get the first tool. The first tool determines the sampling times, the other tools are interpolated.
    vtkPlusDataSource* firstActiveTool = NULL; 
    if ( this->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get most recent timestamp from tracker buffer - there is no active tool!"); 
      return PLUS_FAIL; 
    }

    vtkPlusBuffer* trackerBuffer = firstActiveTool->GetBuffer(); 
    BufferItemUidType uid = 0;
    if (trackerBuffer->GetItemUidFromTime(latestTrackerTimestamp, uid) != ITEM_OK )
    {
      LOG_ERROR("Failed to get video buffer item UID from time: " << std::fixed << latestTrackerTimestamp ); 
      return PLUS_FAIL; 
    }
 
    double latestTrackerTimestampForFirstActiveTool=0;
    // Get the most recent valid timestamp from the tracker buffer
    if ( trackerBuffer->GetTimeStamp(uid, latestTrackerTimestampForFirstActiveTool ) != ITEM_OK )
    {
      LOG_WARNING("Unable to get timestamp from default tool tracker buffer with UID: " << uid); 
      return PLUS_FAIL;
    }
    latestTrackerTimestamp=latestTrackerTimestampForFirstActiveTool;
  }

  if ( !this->GetVideoDataAvailable() )
  {
    latestVideoTimestamp = latestTrackerTimestamp; 
  }

  if ( !this->GetTrackingEnabled() )
  {
    latestTrackerTimestamp = latestVideoTimestamp; 
  }
  
  // If the video timestamp is newer than the tracker timestamp, then use the latest video timestamp that comes before the first tracker timestamp
  if ( latestVideoTimestamp > latestTrackerTimestamp )
  {
    BufferItemUidType videoUid(0); 
    if ( this->VideoSource->GetBuffer()->GetItemUidFromTime(latestTrackerTimestamp, videoUid) != ITEM_OK )
    {
      LOG_ERROR("Failed to get video buffer item UID from time: " << std::fixed << latestVideoTimestamp ); 
      return PLUS_FAIL; 
    }
    if ( latestVideoTimestamp > latestTrackerTimestamp )
    {
      // the closest video timestamp is still larger than the last tracking data,
      // so we need the previous video timestamp (that should have a timestamp that is smaller than the first tracking data)
      if ( videoUid-1 < this->VideoSource->GetBuffer()->GetOldestItemUidInBuffer() ) 
      {
        // the previous video item does not exist, so there is no overlap between the tracking and video data
        LOG_ERROR("Failed to get most recent timestamp: no overlap between tracking and video data"); 
        return PLUS_FAIL; 
      }
      if ( this->VideoSource->GetBuffer()->GetTimeStamp(videoUid-1, latestVideoTimestamp) != ITEM_OK )
      {
        LOG_ERROR("Failed to get video buffer timestamp from UID: " << videoUid); 
        return PLUS_FAIL; 
      }
      if ( latestVideoTimestamp > latestTrackerTimestamp )
      {
        LOG_ERROR("Failed to get most recent timestamp: no overlap between tracking and video data"); 
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
  if( this->HasVideoSource() && this->GetVideoSource(aSource) == PLUS_SUCCESS )
  {
    StreamBufferItem item;
    if( aSource->GetBuffer()->GetLatestStreamBufferItem(&item) == ITEM_OK && item.HasValidTransformData() )
    {
      return true;
    }
  }

  // Now check any and all tool buffers
  for( DataSourceContainerConstIterator toolIt = this->GetToolsStartConstIterator(); toolIt != this->GetToolsEndConstIterator(); ++toolIt)
  {
    vtkPlusDataSource* tool = toolIt->second;
    StreamBufferItem item;
    if( tool->GetBuffer()->GetLatestStreamBufferItem(&item) != ITEM_OK )
    {
      continue;
    }
    if( item.HasValidTransformData() )
    {
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------
bool vtkPlusChannel::GetVideoDataAvailable()
{
  if( !this->HasVideoSource() )
  {
    return false;
  }
  vtkPlusDataSource* aSource = NULL;
  if (this->GetVideoSource(aSource) != PLUS_SUCCESS )
  {
    return false;
  }
  StreamBufferItem item;
  if( aSource->GetBuffer()->GetLatestStreamBufferItem(&item) != ITEM_OK)
  {
    return false;
  }
  return item.HasValidVideoData();
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
PlusStatus vtkPlusChannel::GetFirstActiveTool(vtkPlusDataSource*& aTool)
{
  if ( this->GetToolsStartConstIterator() == this->GetToolsEndConstIterator() )
  {
    LOG_ERROR("Failed to get first active tool - there is no active tool!"); 
    return PLUS_FAIL; 
  }

  // Get the first tool
  aTool = this->GetToolsStartIterator()->second; 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
int vtkPlusChannel::GetNumberOfFramesBetweenTimestamps(double aTimestampFrom, double aTimestampTo)
{
  LOG_TRACE("vtkPlusChannel::GetNumberOfFramesBetweenTimestamps(" << aTimestampFrom << ", " << aTimestampTo << ")");

  int numberOfFrames = 0;

  if ( this->GetVideoDataAvailable() )
  {
    StreamBufferItem vFromItem; 
    if (this->VideoSource->GetBuffer()->GetStreamBufferItemFromTime(aTimestampFrom, &vFromItem, vtkPlusBuffer::CLOSEST_TIME) != ITEM_OK )
    {
      return 0;
    }

    StreamBufferItem vToItem; 
    if (this->VideoSource->GetBuffer()->GetStreamBufferItemFromTime(aTimestampTo, &vToItem, vtkPlusBuffer::CLOSEST_TIME) != ITEM_OK )
    {
      return 0;
    }

    numberOfFrames = abs((int)(vToItem.GetUid() - vFromItem.GetUid()));
  }
  else if ( this->GetTrackingEnabled() )
  {
    // Get the first tool
    vtkPlusDataSource* firstActiveTool = NULL; 
    if ( this->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get number of frames between timestamps - there is no active tool!"); 
      return PLUS_FAIL; 
    }

    vtkPlusBuffer* trackerBuffer = firstActiveTool->GetBuffer(); 
    if ( trackerBuffer == NULL )
    {
      LOG_ERROR("Failed to get first active tool!"); 
      return 0; 
    }

    // vtkPlusBuffer::INTERPOLATED will give the closest item UID  
    StreamBufferItem tFromItem; 
    if (trackerBuffer->GetStreamBufferItemFromTime(aTimestampFrom, &tFromItem, vtkPlusBuffer::INTERPOLATED) != ITEM_OK )
    {
      return 0;
    } 

    // vtkPlusBuffer::INTERPOLATED will give the closest item UID 
    StreamBufferItem tToItem; 
    if (trackerBuffer->GetStreamBufferItemFromTime(aTimestampTo, &tToItem, vtkPlusBuffer::INTERPOLATED) != ITEM_OK )
    {
      return 0;
    }

    numberOfFrames = abs((int)(tToItem.GetUid() - tFromItem.GetUid())); 
  }

  return numberOfFrames + 1;
}

//----------------------------------------------------------------------------
double vtkPlusChannel::GetClosestTrackedFrameTimestampByTime(double time)
{
  if ( this->GetVideoDataAvailable() )
  {
    BufferItemUidType uid=0;
    if (this->VideoSource->GetBuffer()->GetItemUidFromTime(time, uid)!=ITEM_OK)
    {
      return UNDEFINED_TIMESTAMP;
    }    
    double closestTimestamp = UNDEFINED_TIMESTAMP; 
    if ( this->VideoSource->GetBuffer()->GetTimeStamp(uid, closestTimestamp)!=ITEM_OK)
    {
      return UNDEFINED_TIMESTAMP;
    }
    return closestTimestamp;
  }

  if ( this->GetTrackingEnabled() )
  {
    // Get the first tool
    vtkPlusDataSource* firstActiveTool = NULL; 
    if ( this->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
    {
      // there is no active tool
      return UNDEFINED_TIMESTAMP; 
    }
    vtkPlusBuffer* trackerBuffer = firstActiveTool->GetBuffer(); 
    if ( trackerBuffer == NULL )
    {
      // there is no buffer
      return UNDEFINED_TIMESTAMP;
    }
    BufferItemUidType uid=0;
    if (trackerBuffer->GetItemUidFromTime(time, uid)!=ITEM_OK)
    {
      return UNDEFINED_TIMESTAMP;
    }    
    double closestTimestamp = UNDEFINED_TIMESTAMP; 
    if (trackerBuffer->GetTimeStamp(uid, closestTimestamp)!=ITEM_OK)
    {
      return UNDEFINED_TIMESTAMP;
    }
    return closestTimestamp;
  }

  // neither tracker, nor video data available
  return UNDEFINED_TIMESTAMP;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetBrightnessFrameSize(int aDim[2])
{
  aDim[0]=this->BrightnessFrameSize[0];
  aDim[1]=this->BrightnessFrameSize[1];

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
vtkImageData* vtkPlusChannel::GetBrightnessOutput()
{  
  vtkImageData* resultImage = this->BlankImage;
  if( !this->HasVideoSource() )
  {
    return resultImage;
  }

  if ( this->VideoSource->GetBuffer() != NULL && this->VideoSource->GetBuffer()->GetLatestStreamBufferItem( &this->BrightnessOutputTrackedFrame ) != ITEM_OK )
  {
    LOG_DEBUG("No video data available yet, return blank frame");
  }
  else if (this->BrightnessOutputTrackedFrame.GetFrame().GetImageType()==US_IMG_BRIGHTNESS)
  {
    // B-mode image already, just return as is
    resultImage = this->BrightnessOutputTrackedFrame.GetFrame().GetImage();
  }
  else if (this->RfProcessor != NULL )
  {
    // RF frame, convert to B-mode frame
    this->RfProcessor->SetRfFrame(this->BrightnessOutputTrackedFrame.GetFrame().GetImage(), this->BrightnessOutputTrackedFrame.GetFrame().GetImageType());
    resultImage = this->RfProcessor->GetBrightessScanConvertedImage();

    // RF processing parameters were used, so save them into the config file
    this->SaveRfProcessingParameters=true;
  }
  else
  {
    return NULL;
  }

  int *resultExtent = resultImage->GetExtent();
  this->BrightnessFrameSize[0] = resultExtent[1] - resultExtent[0]+1;
  this->BrightnessFrameSize[1] = resultExtent[3] - resultExtent[2]+1;

  return resultImage;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetCustomAttribute( const std::string& attributeId, std::string& output ) const
{
  if( this->CustomAttributes.find(attributeId) != this->CustomAttributes.end() )
  {
    output = this->CustomAttributes.find(attributeId)->second;
    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::GetCustomAttributeMap( CustomAttributeMap& output ) const
{
  for( CustomAttributeMapConstIterator it = this->CustomAttributes.begin(); it != this->CustomAttributes.end(); ++it )
  {
    output[it->first] = it->second;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusChannel::SetCustomAttribute( const std::string& attributeId, const std::string& value )
{
  std::string tempValue;
  if( this->GetCustomAttribute(attributeId, tempValue) == PLUS_SUCCESS )
  {
    LOG_WARNING("Attribute: " << attributeId << " will be overwritten with value: " << value);
  }
  this->CustomAttributes[attributeId] = value;

  return PLUS_SUCCESS;
}
