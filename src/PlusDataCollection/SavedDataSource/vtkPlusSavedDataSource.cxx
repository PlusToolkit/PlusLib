/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkImageData.h"
#include "vtkMatrix4x4.h"
#include "vtkPlusSequenceIO.h"
#include "vtkObjectFactory.h"
#include "vtkPlusBuffer.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusSavedDataSource.h"
#include "vtkPlusTrackedFrameList.h"
#include "vtksys/SystemTools.hxx"

vtkStandardNewMacro(vtkPlusSavedDataSource);

//----------------------------------------------------------------------------
vtkPlusSavedDataSource::vtkPlusSavedDataSource()
  : FrameBufferRowAlignment(1)
  , SequenceFile(NULL)
  , RepeatEnabled(false)
  , LoopStartTime_Local(0.0)
  , LoopStopTime_Local(0.0)
  , LocalVideoBuffer(NULL)
  , UseAllFrameFields(false)
  , UseOriginalTimestamps(false)
  , LastAddedFrameUid(0)
  , LastAddedLoopIndex(0)
  , SimulatedStream(VIDEO_STREAM)
{
  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates = true;
  this->AcquisitionRate = 10;
}

//----------------------------------------------------------------------------
vtkPlusSavedDataSource::~vtkPlusSavedDataSource()
{
  if (this->Connected)
  {
    this->Disconnect();
  }
  DeleteLocalBuffers();
}

//----------------------------------------------------------------------------
void vtkPlusSavedDataSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSavedDataSource::InternalUpdate()
{
  //LOG_TRACE("vtkPlusSavedDataSource::InternalUpdate");
  const int numberOfFramesInTheLoop = this->LoopLastFrameUid - this->LoopFirstFrameUid + 1;

  // Determine the UID and loop index of the next frame that will be added
  BufferItemUidType frameToBeAddedUid = this->LastAddedFrameUid + 1;
  int frameToBeAddedLoopIndex = this->LastAddedLoopIndex;
  if (frameToBeAddedUid > this->LoopLastFrameUid)
  {
    frameToBeAddedLoopIndex++;
    frameToBeAddedUid -= numberOfFramesInTheLoop;
  }

  PlusStatus status = PLUS_FAIL;
  if (this->UseOriginalTimestamps)
  {
    status = InternalUpdateOriginalTimestamp(frameToBeAddedUid, frameToBeAddedLoopIndex);
  }
  else
  {
    status = InternalUpdateCurrentTimestamp(frameToBeAddedUid, frameToBeAddedLoopIndex);
  }

  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSavedDataSource::InternalUpdateOriginalTimestamp(BufferItemUidType frameToBeAddedUid, int frameToBeAddedLoopIndex)
{
  // Compute elapsed time since we started the acquisition
  double elapsedTime = vtkPlusAccurateTimer::GetSystemTime() - this->GetOutputDataSource()->GetStartTime();
  double loopTime = this->LoopStopTime_Local - this->LoopStartTime_Local;

  const int numberOfFramesInTheLoop = this->LoopLastFrameUid - this->LoopFirstFrameUid + 1;
  int currentLoopIndex = 0; // how many loops have we completed so far?
  BufferItemUidType currentFrameUid = 0; // uid of the frame that has been acquired most recently (uid of the last frame that has to be added in this update)
  {
    double currentFrameTime_Local = 0; // current time in the Local buffer time reference
    if (!this->RepeatEnabled || loopTime == 0)
    {
      if (elapsedTime >= loopTime)
      {
        // reached the end of the loop, nothing to add
        return PLUS_SUCCESS;
      }
      currentLoopIndex = 0;
      currentFrameTime_Local = this->LoopStartTime_Local + elapsedTime;
    }
    else
    {
      currentLoopIndex = floor(elapsedTime / loopTime);
      currentFrameTime_Local = this->LoopStartTime_Local + elapsedTime - loopTime * currentLoopIndex;
      double latestTimestamp_Local = 0;
      GetLocalBuffer()->GetLatestTimeStamp(latestTimestamp_Local);
      if (currentFrameTime_Local > latestTimestamp_Local)
      {
        // hold the last frame after the end of the buffer
        // (one frame period was added at the end of the buffer for displaying the last frame)
        currentFrameTime_Local = latestTimestamp_Local;
      }
    }

    // Get the uid of the frame that has been most recently acquired
    BufferItemUidType closestFrameUid = 0;
    GetLocalBuffer()->GetItemUidFromTime(currentFrameTime_Local, closestFrameUid);
    double closestFrameTime_Local = 0;
    GetLocalBuffer()->GetTimeStamp(closestFrameUid, closestFrameTime_Local);
    if (closestFrameTime_Local > currentFrameTime_Local)
    {
      // the closest frame is newer than the current time, so don't use this item but the one before
      currentFrameUid = closestFrameUid - 1;
    }
    else
    {
      currentFrameUid = closestFrameUid;
    }
    if (currentFrameUid < this->LoopFirstFrameUid)
    {
      currentLoopIndex--;
      currentFrameUid += numberOfFramesInTheLoop;
    }
    if (currentFrameUid > this->LoopLastFrameUid)
    {
      currentLoopIndex++;
      currentFrameUid -= numberOfFramesInTheLoop;
    }
  }

  int numberOfFramesToBeAdded = (currentFrameUid - this->LastAddedFrameUid) +
                                (currentLoopIndex - this->LastAddedLoopIndex) * numberOfFramesInTheLoop;

  PlusStatus status(PLUS_SUCCESS);
  for (int addedFrames = 0; addedFrames < numberOfFramesToBeAdded; addedFrames++)
  {

    // The sampling rate is constant, so to have a constant frame rate we have to increase the FrameNumber by a constant.
    // For simplicity, we increase it always by 1.
    // TODO: use the UID difference as increment
    this->FrameNumber++;

    StreamBufferItem dataBufferItemToBeAdded;
    if (GetLocalBuffer()->GetStreamBufferItem(frameToBeAddedUid, &dataBufferItemToBeAdded) != ITEM_OK)
    {
      LOG_ERROR("vtkPlusSavedDataSource: Failed to retrieve item from the buffer, UID=" << frameToBeAddedUid);
      status = PLUS_FAIL;
      continue;
    }

    // Compute the system time corresponding to this frame
    // Get the filtered timestamp from the buffer without any local time offset. Offset will be applied when it is copied to the output stream's buffer.
    double filteredTimestamp = dataBufferItemToBeAdded.GetFilteredTimestamp(0.0) + frameToBeAddedLoopIndex * loopTime -
                               this->LoopStartTime_Local + this->GetOutputDataSource()->GetStartTime();
    double unfilteredTimestamp = filteredTimestamp; // we ignore unfiltered timestamps

    switch (this->SimulatedStream)
    {
      case VIDEO_STREAM:
      {
        StreamBufferItem::FieldMapType fieldMap;
        if (this->UseAllFrameFields)
        {
          fieldMap = dataBufferItemToBeAdded.GetFrameFieldMap();
        }
        if (this->AddVideoItemToVideoSources(this->GetVideoSources(), dataBufferItemToBeAdded.GetFrame(), this->FrameNumber, unfilteredTimestamp, filteredTimestamp, &fieldMap) != PLUS_SUCCESS)
        {
          status = PLUS_FAIL;
        }
        break;
      }
      case TRACKER_STREAM:
      {
        // retrieve timestamp from the first active tool and add all the tool matrices corresponding to that timestamp
        double nextFrameTimestamp = dataBufferItemToBeAdded.GetFilteredTimestamp(0.0);

        for (DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
        {
          vtkPlusDataSource* tool = it->second;
          StreamBufferItem bufferItem;
          ItemStatus itemStatus = this->LocalTrackerBuffers[tool->GetId()]->GetStreamBufferItemFromTime(nextFrameTimestamp, &bufferItem, vtkPlusBuffer::INTERPOLATED);
          if (itemStatus != ITEM_OK)
          {
            if (itemStatus == ITEM_NOT_AVAILABLE_YET)
            {
              LOG_ERROR("vtkPlusSavedDataSource: Unable to get next item from local buffer from time for tool " << tool->GetId() << " - frame not available yet!");
            }
            else if (itemStatus == ITEM_NOT_AVAILABLE_ANYMORE)
            {
              LOG_ERROR("vtkPlusSavedDataSource: Unable to get next item from local buffer from time for tool " << tool->GetId() << " - frame not available anymore!");
            }
            else
            {
              LOG_ERROR("vtkPlusSavedDataSource: Unable to get next item from local buffer from time for tool " << tool->GetId() << "!");
            }
            status = PLUS_FAIL;
            continue;
          }
          // Get default transform
          vtkSmartPointer<vtkMatrix4x4> toolTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
          if (bufferItem.GetMatrix(toolTransMatrix) != PLUS_SUCCESS)
          {
            LOG_ERROR("Failed to get toolTransMatrix for tool " << tool->GetId());
            status = PLUS_FAIL;
            continue;
          }
          // Get flags
          ToolStatus toolStatus = bufferItem.GetStatus();
          // This device has no frame numbering, just auto increment tool frame number if new frame received
          // send the transformation matrix and flags to the tool
          if (this->ToolTimeStampedUpdateWithoutFiltering(tool->GetId(), toolTransMatrix, toolStatus, unfilteredTimestamp, filteredTimestamp) != PLUS_SUCCESS)
          {
            status = PLUS_FAIL;
          }
        }
      }
      break;
      default:
        LOG_ERROR("Unknown stream type: " << this->SimulatedStream);
        return PLUS_FAIL;
    }

    this->LastAddedFrameUid = frameToBeAddedUid;
    this->LastAddedLoopIndex = frameToBeAddedLoopIndex;

    frameToBeAddedUid++;
    if (frameToBeAddedUid > this->LoopLastFrameUid)
    {
      frameToBeAddedLoopIndex++;
      frameToBeAddedUid -= numberOfFramesInTheLoop;
    }
  }

  this->Modified();
  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSavedDataSource::InternalUpdateCurrentTimestamp(BufferItemUidType frameToBeAddedUid, int frameToBeAddedLoopIndex)
{
  // Don't use the original timestamps, just replay with one frame at each update

  if (!this->RepeatEnabled && frameToBeAddedLoopIndex > 0)
  {
    // there is no repeat and we already played the loop once, so don't add any more frames
    return PLUS_SUCCESS;
  }

  this->FrameNumber++;
  StreamBufferItem dataBufferItemToBeAdded;
  if (GetLocalBuffer()->GetStreamBufferItem(frameToBeAddedUid, &dataBufferItemToBeAdded) != ITEM_OK)
  {
    LOG_ERROR("vtkPlusSavedDataSource: Failed to retrieve item from the buffer, UID=" << frameToBeAddedUid);
    return PLUS_FAIL;
  }

  PlusStatus status = PLUS_SUCCESS;
  switch (this->SimulatedStream)
  {
    case VIDEO_STREAM:
    {
      StreamBufferItem::FieldMapType fieldMap;
      if (this->UseAllFrameFields)
      {
        fieldMap = dataBufferItemToBeAdded.GetFrameFieldMap();
      }
      if (this->AddVideoItemToVideoSources(this->GetVideoSources(), dataBufferItemToBeAdded.GetFrame(), this->FrameNumber, UNDEFINED_TIMESTAMP, UNDEFINED_TIMESTAMP, &fieldMap) != PLUS_SUCCESS)
      {
        // UNDEFINED_TIMESTAMP => use current timestamp
        status = PLUS_FAIL;
      }
      break;
    }
    case TRACKER_STREAM:
    {
      // retrieve timestamp from the first active tool and add all the tool matrices corresponding to that timestamp
      double nextFrameTimestamp = dataBufferItemToBeAdded.GetTimestamp(0);

      for (DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
      {
        vtkPlusDataSource* tool(it->second);
        StreamBufferItem bufferItem;
        vtkPlusBuffer* localTrackerBuffer = this->LocalTrackerBuffers[tool->GetId()];
        ItemStatus itemStatus = ITEM_UNKNOWN_ERROR;
        if (localTrackerBuffer)
        {
          itemStatus = localTrackerBuffer->GetStreamBufferItemFromTime(nextFrameTimestamp, &bufferItem, vtkPlusBuffer::INTERPOLATED);
        }
        if (itemStatus != ITEM_OK)
        {
          if (itemStatus == ITEM_NOT_AVAILABLE_YET)
          {
            LOG_ERROR("vtkPlusSavedDataSource: Unable to get next item from local buffer from time for tool " << tool->GetId() << " - frame not available yet!");
          }
          else if (itemStatus == ITEM_NOT_AVAILABLE_ANYMORE)
          {
            LOG_ERROR("vtkPlusSavedDataSource: Unable to get next item from local buffer from time for tool " << tool->GetId() << " - frame not available anymore!");
          }
          else
          {
            LOG_ERROR("vtkPlusSavedDataSource: Unable to get next item from local buffer from time for tool " << tool->GetId() << "!");
          }
          status = PLUS_FAIL;
          continue;
        }
        // Get default transform
        vtkSmartPointer<vtkMatrix4x4> toolTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
        if (bufferItem.GetMatrix(toolTransMatrix) != PLUS_SUCCESS)
        {
          LOG_ERROR("Failed to get toolTransMatrix for tool " << tool->GetId());
          status = PLUS_FAIL;
          continue;
        }
        // Get flags
        ToolStatus toolStatus = bufferItem.GetStatus();
        // This device has no frame numbering, just auto increment tool frame number if new frame received
        unsigned long frameNumber = tool->GetFrameNumber() + 1 ;
        // send the transformation matrix and flags to the tool
        if (this->ToolTimeStampedUpdate(tool->GetId(), toolTransMatrix, toolStatus, frameNumber, UNDEFINED_TIMESTAMP) != PLUS_SUCCESS)
        {
          status = PLUS_FAIL;
        }
      }
    }
    break;
    default:
      LOG_ERROR("Unknown stream type: " << this->SimulatedStream);
      return PLUS_FAIL;
  }

  this->LastAddedFrameUid = frameToBeAddedUid;
  this->LastAddedLoopIndex = frameToBeAddedLoopIndex;

  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSavedDataSource::Probe()
{
  LOG_TRACE("vtkPlusSavedDataSource::Probe");
  if (!vtksys::SystemTools::FileExists(this->GetSequenceFile(), true))
  {
    LOG_ERROR("vtkPlusSavedDataSource Probe failed: Unable to find sequence file!");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSavedDataSource::InternalConnect()
{
  LOG_TRACE("vtkPlusSavedDataSource::InternalConnect");

  if (this->SequenceFile == NULL)
  {
    LOG_ERROR("Unable to connect to saved data video source: Unable to read sequence metafile. No filename is specified.");
    return PLUS_FAIL;
  }
  std::string foundAbsoluteImagePath;
  vtkPlusConfig::GetInstance()->FindImagePath(this->SequenceFile, foundAbsoluteImagePath);
  if (!vtksys::SystemTools::FileExists(foundAbsoluteImagePath, true))
  {
    LOG_ERROR("Unable to connect to saved data video source: Unable to read sequence metafile: " << this->SequenceFile);
    return PLUS_FAIL;
  }

  vtkSmartPointer<vtkPlusTrackedFrameList> savedDataBuffer = vtkSmartPointer<vtkPlusTrackedFrameList>::New();

  // Read sequence file into tracked frame list
  vtkPlusSequenceIO::Read(foundAbsoluteImagePath, savedDataBuffer);

  if (savedDataBuffer->GetNumberOfTrackedFrames() < 1)
  {
    LOG_ERROR("Failed to connect to saved dataset - there is no frame in the sequence metafile!");
    return PLUS_FAIL;
  }

  PlusStatus status = PLUS_FAIL;
  switch (this->SimulatedStream)
  {
    case VIDEO_STREAM:
      status = InternalConnectVideo(savedDataBuffer);
      break;
    case TRACKER_STREAM:
      status = InternalConnectTracker(savedDataBuffer);
      break;
    default:
      LOG_ERROR("Unknown stream type: " << this->SimulatedStream);
  }

  if (status != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  if (GetLocalBuffer() == NULL)
  {
    LOG_ERROR("Local buffer is invalid");
    return PLUS_FAIL;
  }

  double oldestTimestamp_Local = 0;
  GetLocalBuffer()->GetOldestTimeStamp(oldestTimestamp_Local);
  double latestTimestamp_Local = 0;
  GetLocalBuffer()->GetLatestTimeStamp(latestTimestamp_Local);

  // Set the default loop start time and length to match the video buffer start time and length

  this->LoopFirstFrameUid = GetLocalBuffer()->GetOldestItemUidInBuffer();
  this->LoopLastFrameUid = GetLocalBuffer()->GetLatestItemUidInBuffer();

  this->LoopStartTime_Local = oldestTimestamp_Local;

  // When we reach the last frame we have to wait one frame period before
  // playing the first frame, so we have to add one frame period to the loop length (loopTime)
  double framePeriodSec = 0;
  double frameRate = GetLocalBuffer()->GetFrameRate();
  if (frameRate != 0.0)
  {
    framePeriodSec = 1.0 / frameRate;
  }
  else
  {
    // There is probably only one frame in the buffer, so use the AcquisitionRate
    // (instead of trying to find out the frame period from the frame rate in the file)
    if (this->AcquisitionRate != 0.0)
    {
      framePeriodSec = 1.0 / this->AcquisitionRate;
    }
    else
    {
      LOG_ERROR("Invalid AcquisitionRate: " << this->AcquisitionRate);
      framePeriodSec = 1.0;
    }
  }
  this->LoopStopTime_Local = latestTimestamp_Local + framePeriodSec;

  this->LastAddedFrameUid = this->LoopFirstFrameUid - 1;
  this->LastAddedLoopIndex = 0;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSavedDataSource::InternalConnectVideo(vtkPlusTrackedFrameList* savedDataBuffer)
{
  // Set buffer parameters based on the input tracked frame list
  vtkPlusDataSource* outputDataSource = this->GetOutputDataSource();
  if (outputDataSource == NULL)
  {
    return PLUS_FAIL;
  }
  if (outputDataSource->SetImageType(savedDataBuffer->GetImageType()) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to set video buffer image type");
    return PLUS_FAIL;
  }

  // Saved data buffer contains data read directly from file, set up a new local buffer
  DeleteLocalBuffers();
  this->LocalVideoBuffer = vtkPlusBuffer::New();
  this->LocalVideoBuffer->SetImageOrientation(savedDataBuffer->GetImageOrientation());
  this->LocalVideoBuffer->SetImageType(savedDataBuffer->GetImageType());
  FrameSizeType frameSize;
  if (savedDataBuffer->GetFrameSize(frameSize) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to retrieve frame size.");
    return PLUS_FAIL;
  }
  this->LocalVideoBuffer->SetFrameSize(frameSize);
  unsigned int numberOfScalarComponents;
  if (savedDataBuffer->GetTrackedFrame(0)->GetNumberOfScalarComponents(numberOfScalarComponents) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to retrieve number of scalar components.");
    return PLUS_FAIL;
  }
  this->LocalVideoBuffer->SetNumberOfScalarComponents(numberOfScalarComponents);
  this->LocalVideoBuffer->SetPixelType(savedDataBuffer->GetTrackedFrame(0)->GetImageData()->GetVTKScalarPixelType());
  this->LocalVideoBuffer->SetBufferSize(savedDataBuffer->GetNumberOfTrackedFrames());
  this->LocalVideoBuffer->SetLocalTimeOffsetSec(0.0);   // the time offset is copied from the output, so reset it to 0
  this->LocalVideoBuffer->CopyImagesFromTrackedFrameList(savedDataBuffer, vtkPlusBuffer::READ_FILTERED_IGNORE_UNFILTERED_TIMESTAMPS, this->UseAllFrameFields);
  savedDataBuffer->Clear();

  PlusStatus result(PLUS_SUCCESS);
  for (DataSourceContainerIterator it = this->VideoSources.begin(); it != this->VideoSources.end(); ++it)
  {
    vtkPlusDataSource* source(it->second);

    if (source->SetInputImageOrientation(this->LocalVideoBuffer->GetImageOrientation()) != PLUS_SUCCESS)
    {
      LOG_ERROR(source->GetId() << ": Failed to set video image orientation");
      result = PLUS_FAIL;
      continue;
    }

    if (source->SetInputFrameSize(this->LocalVideoBuffer->GetFrameSize()) != PLUS_SUCCESS)
    {
      LOG_ERROR(source->GetId() << ": Failed to set video image orientation");
      result = PLUS_FAIL;
      continue;
    }

    if (source->SetNumberOfScalarComponents(this->LocalVideoBuffer->GetNumberOfScalarComponents()) != PLUS_SUCCESS)
    {
      LOG_ERROR(source->GetId() << ": Failed to set video image orientation");
      result = PLUS_FAIL;
      continue;
    }

    source->Clear();

    if (source->SetInputFrameSize(this->LocalVideoBuffer->GetFrameSize()) != PLUS_SUCCESS)
    {
      LOG_ERROR(source->GetId() << ": Failed to set video image orientation");
      result = PLUS_FAIL;
      continue;
    }

    if (source->SetPixelType(this->LocalVideoBuffer->GetPixelType()) != PLUS_SUCCESS)
    {
      LOG_ERROR(source->GetId() << ": Failed to set video image orientation");
      result = PLUS_FAIL;
      continue;
    }
  }

  return result;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSavedDataSource::InternalConnectTracker(vtkPlusTrackedFrameList* savedDataBuffer)
{
  PlusTrackedFrame* frame = savedDataBuffer->GetTrackedFrame(0);
  if (frame == NULL)
  {
    LOG_ERROR("The tracked frame buffer doesn't seem to contain any frames");
    return PLUS_FAIL;
  }

  // Clear local buffers before connect
  this->DeleteLocalBuffers();

  // Enable tools that have a matching transform name in the savedDataBuffer
  double transformMatrix[16] = {0};
  for (DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    vtkPlusDataSource* tool = it->second;
    if (tool->GetId().empty())
    {
      // no tool name is available, don't connect it to any transform in the savedDataBuffer
      continue;
    }

    PlusTransformName toolTransformName(tool->GetId());
    if (!frame->IsFrameTransformNameDefined(toolTransformName))
    {
      std::string strTransformName;
      toolTransformName.GetTransformName(strTransformName);
      LOG_WARNING("Tool '" << tool->GetId() << "' has no matching transform in the file with name: " << strTransformName);
      continue;
    }

    if (frame->GetFrameTransform(toolTransformName, transformMatrix) != PLUS_SUCCESS)
    {
      LOG_WARNING("Cannot convert the frame field ( for tool " << tool->GetId() << ") to a transform");
      continue;
    }
    // a transform with the same name as the tool name has been found in the savedDataBuffer
    tool->SetBufferSize(savedDataBuffer->GetNumberOfTrackedFrames());

    vtkSmartPointer<vtkPlusBuffer> buffer = vtkSmartPointer<vtkPlusBuffer>::New();
    tool->DeepCopyBufferTo(*buffer);
    // Copy all the settings from the default tool buffer
    buffer->SetLocalTimeOffsetSec(0.0);   // the time offset is copied from the output, so reset it to 0
    if (buffer->CopyTransformFromTrackedFrameList(savedDataBuffer, vtkPlusBuffer::READ_FILTERED_IGNORE_UNFILTERED_TIMESTAMPS, toolTransformName) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to retrieve tracking data from tracked frame list for tool " << tool->GetId());
      return PLUS_FAIL;
    }

    buffer->Register(this);
    this->LocalTrackerBuffers[tool->GetId()] = buffer;
  }

  savedDataBuffer->Clear();

  ClearAllBuffers();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSavedDataSource::InternalDisconnect()
{
  DeleteLocalBuffers();
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusSavedDataSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusSavedDataSource::ReadConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  // SequenceMetafile attribute is deprecated and kept for backward compatibility only.
  // SequenceFile attribute should be used instead.
  if (deviceConfig->GetAttribute("SequenceMetafile"))
  {
    if (deviceConfig->GetAttribute("SequenceFile"))
    {
      LOG_WARNING("SavedDataSource SequenceMetafile and SequenceFile attributes are specified. Please remove the deprecated SequenceMetaFile attribute.");
      XML_READ_CSTRING_ATTRIBUTE_REQUIRED(SequenceFile, deviceConfig);
    }
    else
    {
      LOG_WARNING("Deprecated SequenceMetafile attribute is defined for SavedDataSource device. Please rename the SequenceMetafile attribute to SequenceFile.");
      SetSequenceFile(deviceConfig->GetAttribute("SequenceMetafile"));
    }
  }
  else
  {
    // Nominal case. The other branch should be removed if support for SequenceMetafile attribute is completely removed.
    XML_READ_CSTRING_ATTRIBUTE_REQUIRED(SequenceFile, deviceConfig);
  }

  if (this->SequenceFile == NULL)
  {
    LOG_ERROR("Sequence file attribute not read correctly. Check log.");
    return PLUS_FAIL;
  }

  std::string foundAbsoluteImagePath;
  if (vtkPlusConfig::GetInstance()->FindImagePath(this->SequenceFile, foundAbsoluteImagePath) == PLUS_FAIL)
  {
    std::string sequenceFileStr(this->SequenceFile);
    std::string seqFileTrim = PlusCommon::Trim(sequenceFileStr);
    std::string foundAbsoluteImagePath;
    if (vtkPlusConfig::GetInstance()->FindImagePath(seqFileTrim, foundAbsoluteImagePath) == PLUS_SUCCESS)
    {
      this->SetSequenceFile(seqFileTrim.c_str());
      LOG_WARNING("Filename contains unexpected characters at beginning or end of string. Please correct. Filename: " << this->SequenceFile);
    }
    else
    {
      LOG_ERROR("Unable to locate file: " << this->SequenceFile << ". Please verify location on disk.");
      return PLUS_FAIL;
    }
  }

  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(RepeatEnabled, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(UseOriginalTimestamps, deviceConfig);

  const char* useData = deviceConfig->GetAttribute("UseData");
  if (useData != NULL)
  {
    if (STRCASECMP("IMAGE", useData) == 0)
    {
      this->UseAllFrameFields = false;
      this->SimulatedStream = VIDEO_STREAM;
    }
    else if (STRCASECMP("IMAGE_AND_TRANSFORM", useData) == 0)
    {
      this->UseAllFrameFields = true;
      this->SimulatedStream = VIDEO_STREAM;
    }
    else if (STRCASECMP("TRANSFORM", useData) == 0)
    {
      this->UseAllFrameFields = true;
      this->SimulatedStream = TRACKER_STREAM;
    }
    else
    {
      LOG_WARNING("Unable to recognize UseData attribute: " << useData << " - changed to IMAGE by default!");
      this->UseAllFrameFields = false;
    }
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusSavedDataSource::WriteConfiguration(vtkXMLDataElement* rootConfig)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(imageAcquisitionConfig, rootConfig);

  XML_WRITE_CSTRING_ATTRIBUTE_IF_NOT_NULL(SequenceFile, imageAcquisitionConfig);
  XML_WRITE_BOOL_ATTRIBUTE(RepeatEnabled, imageAcquisitionConfig);
  XML_WRITE_BOOL_ATTRIBUTE(UseOriginalTimestamps, imageAcquisitionConfig);

  if (this->UseAllFrameFields)
  {
    if (this->SimulatedStream == VIDEO_STREAM)
    {
      imageAcquisitionConfig->SetAttribute("UseData", "IMAGE_AND_TRANSFORM");
    }
    else if (this->SimulatedStream == TRACKER_STREAM)
    {
      imageAcquisitionConfig->SetAttribute("UseData", "TRANSFORM");
    }
  }
  else
  {
    imageAcquisitionConfig->SetAttribute("UseData", "IMAGE");
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusSavedDataSource::NotifyConfigured()
{
  if (this->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined for vtkPlusSavedDataSource. Cannot proceed.");
    this->SetCorrectlyConfigured(false);
    return PLUS_FAIL;
  }

  switch (this->SimulatedStream)
  {
    case VIDEO_STREAM:
      if (this->GetOutputDataSource() == NULL)
      {
        LOG_ERROR("Buffer not created for vtkPlusSavedDataSource but it is required. Check configuration.");
        this->SetCorrectlyConfigured(false);
        return PLUS_FAIL;
      }
      break;
    case TRACKER_STREAM:
      // nothing to check for
      break;
    default:
      LOG_ERROR("Unknown stream type: " << this->SimulatedStream);
      this->SetCorrectlyConfigured(false);
      return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkPlusSavedDataSource::GetLoopTimeRange(double& loopStartTime, double& loopStopTime)
{
  loopStartTime = this->LoopStartTime_Local;
  loopStopTime = this->LoopStopTime_Local;
}

//-----------------------------------------------------------------------------
void vtkPlusSavedDataSource::SetLoopTimeRange(double loopStartTime, double loopStopTime)
{
  this->LoopStartTime_Local = loopStartTime;
  this->LoopStopTime_Local = loopStopTime;

  this->LoopFirstFrameUid = GetClosestFrameUidWithinTimeRange(this->LoopStartTime_Local, this->LoopStartTime_Local, this->LoopStopTime_Local);
  this->LoopLastFrameUid = GetClosestFrameUidWithinTimeRange(this->LoopStopTime_Local, this->LoopStartTime_Local, this->LoopStopTime_Local);

  this->LastAddedFrameUid = this->LoopFirstFrameUid - 1;
  this->LastAddedLoopIndex = 0;
}

//----------------------------------------------------------------------------
BufferItemUidType vtkPlusSavedDataSource::GetClosestFrameUidWithinTimeRange(double time_Local, double startTime_Local, double stopTime_Local)
{
  // time_Local should be within the specified interval
  if (time_Local < startTime_Local)
  {
    time_Local = startTime_Local;
  }
  else if (time_Local > stopTime_Local)
  {
    time_Local = stopTime_Local;
  }
  // time_Local should be also within the local buffer time range
  double oldestTimestamp_Local = 0;
  GetLocalBuffer()->GetOldestTimeStamp(oldestTimestamp_Local);
  double latestTimestamp_Local = 0;
  GetLocalBuffer()->GetLatestTimeStamp(latestTimestamp_Local);

  // if the asked time is outside of the loop range then return the closest element in the range
  if (time_Local < oldestTimestamp_Local)
  {
    time_Local = oldestTimestamp_Local;
  }
  else if (time_Local > latestTimestamp_Local)
  {
    time_Local = latestTimestamp_Local;
  }

  // Get the uid of the frame that has been most recently acquired
  BufferItemUidType closestFrameUid = 0;
  GetLocalBuffer()->GetItemUidFromTime(time_Local, closestFrameUid);
  double closestFrameTime_Local = 0;
  GetLocalBuffer()->GetTimeStamp(closestFrameUid, closestFrameTime_Local);

  // The closest frame is at the boundary, but it may be just outside the range:
  // use the next/previous frame if the closest frame is on the wrong side of the boundary
  if (closestFrameTime_Local < startTime_Local)
  {
    return closestFrameUid + 1;
  }
  else if (closestFrameTime_Local > stopTime_Local)
  {
    return  closestFrameUid - 1;
  }
  else
  {
    // the found closest frame is already within the specified range
    return closestFrameUid;
  }
}

//----------------------------------------------------------------------------
vtkPlusBuffer* vtkPlusSavedDataSource::GetLocalTrackerBuffer()
{
  // Get the first tool - the first active tool determines the timestamp
  vtkPlusDataSource* firstActiveTool = NULL;
  if (this->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to get local tracker buffer - there is no active tool!");
    return NULL;
  }
  return this->LocalTrackerBuffers[firstActiveTool->GetId()];
}

//----------------------------------------------------------------------------
void vtkPlusSavedDataSource::DeleteLocalBuffers()
{
  if (this->LocalVideoBuffer != NULL)
  {
    this->LocalVideoBuffer->Delete();
    this->LocalVideoBuffer = NULL;
  }

  for (std::map<std::string, vtkPlusBuffer*>::iterator it = this->LocalTrackerBuffers.begin(); it != this->LocalTrackerBuffers.end(); ++it)
  {
    if ((*it).second != NULL)
    {
      (*it).second->Delete();
      (*it).second = NULL;
    }
  }

  this->LocalTrackerBuffers.clear();
}

//----------------------------------------------------------------------------
vtkPlusBuffer* vtkPlusSavedDataSource::GetLocalBuffer()
{
  vtkPlusBuffer* buff = NULL;
  switch (this->SimulatedStream)
  {
    case VIDEO_STREAM:
      buff = LocalVideoBuffer;
      break;
    case TRACKER_STREAM:
      buff = GetLocalTrackerBuffer();
      break;
    default:
      LOG_ERROR("Unknown stream type: " << this->SimulatedStream);
  }
  if (buff == NULL)
  {
    LOG_WARNING("vtkPlusSavedDataSource LocalBuffer is invalid");
  }
  return buff;
}

//----------------------------------------------------------------------------
vtkPlusDataSource* vtkPlusSavedDataSource::GetOutputDataSource()
{
  vtkPlusDataSource* aSource(NULL);

  if (this->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined");
    return NULL;
  }
  vtkPlusChannel* outputChannel = this->OutputChannels[0];

  switch (this->SimulatedStream)
  {
    case VIDEO_STREAM:
    {
      if (outputChannel->GetVideoSource(aSource) != PLUS_SUCCESS)
      {
        LOG_ERROR("Unable to retrieve the video source in the SavedDataSource device.");
        return NULL;
      }
      break;
    }
    case TRACKER_STREAM:
    {
      if (this->GetFirstActiveTool(aSource) != PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to get local tracker buffer - there is no active tool!");
        return NULL;
      }
    }
    break;
    default:
      LOG_ERROR("Unknown stream type: " << this->SimulatedStream);
  }
  if (aSource == NULL)
  {
    LOG_WARNING("vtkPlusSavedDataSource OutputBuffer is invalid");
  }
  return aSource;
}

//----------------------------------------------------------------------------
bool vtkPlusSavedDataSource::IsTracker() const
{
  if (this->Tools.size() > 0)
  {
    return true;
  }

  for (DataSourceContainerConstIterator it = this->VideoSources.begin(); it != this->VideoSources.end(); ++it)
  {
    vtkPlusDataSource* aSource = it->second;
    StreamBufferItem item;
    if (aSource->GetNumberOfItems() > 0 && aSource->GetOldestStreamBufferItem(&item) == ITEM_OK)
    {
      if (item.HasValidTransformData())
      {
        return true;
      }
    }
  }

  return false;
}
