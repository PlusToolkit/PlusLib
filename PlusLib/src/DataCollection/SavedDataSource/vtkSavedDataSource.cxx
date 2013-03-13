/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusBuffer.h"
#include "vtkSavedDataSource.h"
#include "vtkTrackedFrameList.h"
#include "vtksys/SystemTools.hxx"

vtkCxxRevisionMacro(vtkSavedDataSource, "$Revision: 1.0$");
vtkStandardNewMacro(vtkSavedDataSource);

//----------------------------------------------------------------------------
vtkSavedDataSource::vtkSavedDataSource()
{
  this->FrameBufferRowAlignment = 1;
  this->LocalVideoBuffer = NULL;
  this->SequenceMetafile = NULL; 
  this->RepeatEnabled = false; 
  this->LoopStartTime_Local = 0.0; 
  this->LoopStopTime_Local = 0.0; 
  this->UseAllFrameFields = false;
  this->UseOriginalTimestamps = false;
  this->LastAddedFrameUid=0;
  this->LastAddedLoopIndex=0;
  this->LoopFirstFrameUid=0;
  this->LoopLastFrameUid=0;
  this->SimulatedStream=VIDEO_STREAM;

  this->RequireFrameBufferSizeInDeviceSetConfiguration = false; // was true for tracker
  this->RequireToolAveragedItemsForFilteringInDeviceSetConfiguration = false; // was true for tracker
  this->RequireAcquisitionRateInDeviceSetConfiguration = false;
  this->RequireAveragedItemsForFilteringInDeviceSetConfiguration = false;
  this->RequireLocalTimeOffsetSecInDeviceSetConfiguration = false;
  this->RequireUsImageOrientationInDeviceSetConfiguration = false; // was true for video
  this->RequireRfElementInDeviceSetConfiguration = false;
  this->RequireImageOrientationInConfiguration=false; // device image orientation is not used, we'll use MF for B-mode and FM for RF-mode

  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates=true;
}

//----------------------------------------------------------------------------
vtkSavedDataSource::~vtkSavedDataSource()
{ 
  if (!this->Connected)
  {
    this->Disconnect();
  }
  DeleteLocalBuffers();
}

//----------------------------------------------------------------------------
void vtkSavedDataSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkSavedDataSource::InternalUpdate()
{
  //LOG_TRACE("vtkSavedDataSource::InternalUpdate");

  const int numberOfFramesInTheLoop=this->LoopLastFrameUid-this->LoopFirstFrameUid+1;

  // Determine the UID and loop index of the next frame that will be added
  BufferItemUidType frameToBeAddedUid=this->LastAddedFrameUid+1;
  int frameToBeAddedLoopIndex=this->LastAddedLoopIndex;
  if (frameToBeAddedUid>this->LoopLastFrameUid)
  {
    frameToBeAddedLoopIndex++;
    frameToBeAddedUid -= numberOfFramesInTheLoop;      
  }

  PlusStatus status=PLUS_FAIL;
  if (!this->UseOriginalTimestamps)
  {
    status=InternalUpdateCurrentTimestamp(frameToBeAddedUid, frameToBeAddedLoopIndex);
  }
  else
  {
    status=InternalUpdateOriginalTimestamp(frameToBeAddedUid, frameToBeAddedLoopIndex);
  }

  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkSavedDataSource::InternalUpdateOriginalTimestamp(BufferItemUidType frameToBeAddedUid, int frameToBeAddedLoopIndex)
{
  // Compute elapsed time since we started the acquisition
  double elapsedTime = vtkAccurateTimer::GetSystemTime() - this->GetOutputBuffer()->GetStartTime();
  double loopTime=this->LoopStopTime_Local-this->LoopStartTime_Local;

  const int numberOfFramesInTheLoop=this->LoopLastFrameUid-this->LoopFirstFrameUid+1;
  int currentLoopIndex=0; // how many loops have we completed so far?
  BufferItemUidType currentFrameUid=0; // uid of the frame that has been acquired most recently (uid of the last frame that has to be added in this update)
  {
    double currentFrameTime_Local=0; // current time in the Local buffer time reference
    if (!this->RepeatEnabled || loopTime==0 )
    {
      if ( elapsedTime >= loopTime )
      {
        // reached the end of the loop, nothing to add
        return PLUS_SUCCESS;
      }
      currentLoopIndex=0;
      currentFrameTime_Local = this->LoopStartTime_Local + elapsedTime;
    }
    else
    {
      currentLoopIndex=floor(elapsedTime/loopTime);
      currentFrameTime_Local = this->LoopStartTime_Local + elapsedTime - loopTime*currentLoopIndex;    
    }

    // Get the uid of the frame that has been most recently acquired
    BufferItemUidType closestFrameUid=0;
    GetLocalBuffer()->GetItemUidFromTime(currentFrameTime_Local,closestFrameUid);
    double closestFrameTime_Local=0;
    GetLocalBuffer()->GetTimeStamp(closestFrameUid, closestFrameTime_Local);
    if (closestFrameTime_Local>currentFrameTime_Local)
    {
      // the closest frame is newer than the current time, so don't use this item but the one before
      currentFrameUid=closestFrameUid-1;
    }
    else
    {
      currentFrameUid=closestFrameUid;
    }
    if (currentFrameUid<this->LoopFirstFrameUid)
    {
      currentLoopIndex--;
      currentFrameUid += numberOfFramesInTheLoop;      
    }
    if (currentFrameUid>this->LoopLastFrameUid)
    {
      currentLoopIndex++;
      currentFrameUid -= numberOfFramesInTheLoop;      
    }
  }

  int numberOfFramesToBeAdded=(currentFrameUid-this->LastAddedFrameUid)+
    (currentLoopIndex-this->LastAddedLoopIndex)*numberOfFramesInTheLoop;

  PlusStatus status=PLUS_SUCCESS;
  for (int addedFrames=0; addedFrames<numberOfFramesToBeAdded; addedFrames++)
  {

    // The sampling rate is constant, so to have a constant frame rate we have to increase the FrameNumber by a constant.
    // For simplicity, we increase it always by 1.
    // TODO: use the UID difference as increment
    this->FrameNumber++;

    StreamBufferItem dataBufferItemToBeAdded; 
    if (GetLocalBuffer()->GetStreamBufferItem( frameToBeAddedUid, &dataBufferItemToBeAdded) != ITEM_OK )
    {
      LOG_ERROR("vtkSavedDataSource: Failed to retrieve item from the buffer, UID=" << frameToBeAddedUid);
      status=PLUS_FAIL;
      continue;
    }

    // Compute the system time corresponding to this frame
    // Get the filtered timestamp from the buffer without any local time offset. Offset will be applied when it is copied to the output stream's buffer.
    double filteredTimestamp=dataBufferItemToBeAdded.GetFilteredTimestamp(0.0)+frameToBeAddedLoopIndex*loopTime-this->LoopStartTime_Local+this->GetOutputBuffer()->GetStartTime();
    double unfilteredTimestamp=filteredTimestamp; // we ignore unfiltered timestamps

    switch (this->SimulatedStream)
    {
    case VIDEO_STREAM:
      {            
        StreamBufferItem::FieldMapType fieldMap;
        if (this->UseAllFrameFields)
        {
          fieldMap = dataBufferItemToBeAdded.GetCustomFrameFieldMap();    
        }
        if (this->GetOutputBuffer()->AddItem(&(dataBufferItemToBeAdded.GetFrame()), this->FrameNumber, unfilteredTimestamp, filteredTimestamp, &fieldMap)==PLUS_FAIL)
        {
          status=PLUS_FAIL;
        }  
        break;
      }
    case TRACKER_STREAM:
      {
        // retrieve timestamp from the first active tool and add all the tool matrices corresponding to that timestamp
        double nextFrameTimestamp=dataBufferItemToBeAdded.GetFilteredTimestamp(0.0);

        int numOfErrors=0;
        for ( DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
        {
          vtkPlusDataSource* tool=it->second;
          StreamBufferItem bufferItem;  
          ItemStatus itemStatus = this->LocalTrackerBuffers[tool->GetSourceId()]->GetStreamBufferItemFromTime(nextFrameTimestamp, &bufferItem, vtkPlusBuffer::INTERPOLATED); 
          if ( itemStatus != ITEM_OK )
          {
            if ( itemStatus == ITEM_NOT_AVAILABLE_YET )
            {
              LOG_ERROR("vtkSavedDataSource: Unable to get next item from local buffer from time for tool "<<tool->GetSourceId()<<" - frame not available yet!");
            }
            else if ( itemStatus == ITEM_NOT_AVAILABLE_ANYMORE )
            {
              LOG_ERROR("vtkSavedDataSource: Unable to get next item from local buffer from time for tool "<<tool->GetSourceId()<<" - frame not available anymore!");
            }
            else
            {
              LOG_ERROR("vtkSavedDataSource: Unable to get next item from local buffer from time for tool "<<tool->GetSourceId()<<"!");
            }
            status=PLUS_FAIL;
            continue;
          }
          // Get default transfom  
          vtkSmartPointer<vtkMatrix4x4> toolTransMatrix=vtkSmartPointer<vtkMatrix4x4>::New();
          if (bufferItem.GetMatrix(toolTransMatrix)!=PLUS_SUCCESS)
          {
            LOG_ERROR("Failed to get toolTransMatrix for tool "<<tool->GetSourceId()); 
            status=PLUS_FAIL;
            continue;
          }
          // Get flags
          ToolStatus toolStatus = bufferItem.GetStatus(); 
          // This device has no frame numbering, just auto increment tool frame number if new frame received
          unsigned long frameNumber = tool->GetFrameNumber() + 1 ; 
          // send the transformation matrix and flags to the tool
          if (this->ToolTimeStampedUpdateWithoutFiltering(tool->GetSourceId(), toolTransMatrix, toolStatus, unfilteredTimestamp, filteredTimestamp)!=PLUS_SUCCESS)
          {
            status=PLUS_FAIL;
          }
        }
      }
      break;
    default:
      LOG_ERROR("Unkown stream type: "<<this->SimulatedStream);
      return PLUS_FAIL;
    }

    this->LastAddedFrameUid=frameToBeAddedUid;
    this->LastAddedLoopIndex=frameToBeAddedLoopIndex;

    frameToBeAddedUid++;
    if (frameToBeAddedUid>this->LoopLastFrameUid)
    {
      frameToBeAddedLoopIndex++;
      frameToBeAddedUid -= numberOfFramesInTheLoop;      
    }
  }

  this->Modified();
  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkSavedDataSource::InternalUpdateCurrentTimestamp(BufferItemUidType frameToBeAddedUid, int frameToBeAddedLoopIndex)
{
  // Don't use the original timestamps, just replay with one frame at each update

  if (!this->RepeatEnabled && frameToBeAddedLoopIndex>0)
  {
    // there is no repeat and we already played the loop once, so don't add any more frames
    return PLUS_SUCCESS;
  }

  this->FrameNumber++;
  StreamBufferItem dataBufferItemToBeAdded; 
  if (GetLocalBuffer()->GetStreamBufferItem( frameToBeAddedUid, &dataBufferItemToBeAdded) != ITEM_OK )
  {
    LOG_ERROR("vtkSavedDataSource: Failed to retrieve item from the buffer, UID=" << frameToBeAddedUid);
    return PLUS_FAIL;
  }

  PlusStatus status=PLUS_SUCCESS;
  switch (this->SimulatedStream)
  {
  case VIDEO_STREAM:
    {            
      StreamBufferItem::FieldMapType fieldMap;
      if (this->UseAllFrameFields)
      {
        fieldMap = dataBufferItemToBeAdded.GetCustomFrameFieldMap();    
      }        
      if (this->GetOutputBuffer()->AddItem(&(dataBufferItemToBeAdded.GetFrame()), this->FrameNumber, UNDEFINED_TIMESTAMP, UNDEFINED_TIMESTAMP, &fieldMap)!=PLUS_SUCCESS) // UNDEFINED_TIMESTAMP => use current timestamp
      {
        status=PLUS_FAIL;
      }
      break;
    }
  case TRACKER_STREAM:
    {
      // retrieve timestamp from the first active tool and add all the tool matrices corresponding to that timestamp
      double nextFrameTimestamp=dataBufferItemToBeAdded.GetTimestamp(0);

      for ( DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
      {
        vtkPlusDataSource* tool=it->second;
        StreamBufferItem bufferItem;  
        ItemStatus itemStatus = this->LocalTrackerBuffers[tool->GetSourceId()]->GetStreamBufferItemFromTime(nextFrameTimestamp, &bufferItem, vtkPlusBuffer::INTERPOLATED); 
        if ( itemStatus != ITEM_OK )
        {
          if ( itemStatus == ITEM_NOT_AVAILABLE_YET )
          {
            LOG_ERROR("vtkSavedDataSource: Unable to get next item from local buffer from time for tool "<<tool->GetSourceId()<<" - frame not available yet!");
          }
          else if ( itemStatus == ITEM_NOT_AVAILABLE_ANYMORE )
          {
            LOG_ERROR("vtkSavedDataSource: Unable to get next item from local buffer from time for tool "<<tool->GetSourceId()<<" - frame not available anymore!");
          }
          else
          {
            LOG_ERROR("vtkSavedDataSource: Unable to get next item from local buffer from time for tool "<<tool->GetSourceId()<<"!");
          }
          status=PLUS_FAIL;
          continue;
        }
        // Get default transfom  
        vtkSmartPointer<vtkMatrix4x4> toolTransMatrix=vtkSmartPointer<vtkMatrix4x4>::New();
        if (bufferItem.GetMatrix(toolTransMatrix)!=PLUS_SUCCESS)
        {
          LOG_ERROR("Failed to get toolTransMatrix for tool "<<tool->GetSourceId()); 
          status=PLUS_FAIL;
          continue;
        }
        // Get flags
        ToolStatus toolStatus = bufferItem.GetStatus(); 
        // This device has no frame numbering, just auto increment tool frame number if new frame received
        unsigned long frameNumber = tool->GetFrameNumber() + 1 ; 
        // send the transformation matrix and flags to the tool
        if (this->ToolTimeStampedUpdate(tool->GetSourceId(), toolTransMatrix, toolStatus, frameNumber, UNDEFINED_TIMESTAMP)!=PLUS_SUCCESS)
        {
          status=PLUS_FAIL;
        }
      }
    }
    break;
  default:
    LOG_ERROR("Unkown stream type: "<<this->SimulatedStream);
    return PLUS_FAIL;
  }

  this->LastAddedFrameUid=frameToBeAddedUid;
  this->LastAddedLoopIndex=frameToBeAddedLoopIndex;

  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkSavedDataSource::Probe()
{
  LOG_TRACE("vtkSavedDataSource::Probe"); 
  if ( !vtksys::SystemTools::FileExists(this->GetSequenceMetafile(), true) )
  {
    LOG_ERROR("vtkSavedDataSource Probe failed: Unable to find sequence metafile!"); 
    return PLUS_FAIL; 
  }
  return PLUS_SUCCESS; 
} 

//----------------------------------------------------------------------------
PlusStatus vtkSavedDataSource::InternalConnect()
{
  LOG_TRACE("vtkSavedDataSource::InternalConnect"); 

  if (this->SequenceMetafile==NULL)
  {
    LOG_ERROR("Unable to connect to saved data video source: Unable to read sequence metafile. No filename is specified."); 
    return PLUS_FAIL; 
  }
  if ( !vtksys::SystemTools::FileExists(this->SequenceMetafile, true) )
  {
    LOG_ERROR("Unable to connect to saved data video source: Unable to read sequence metafile: "<<this->SequenceMetafile); 
    return PLUS_FAIL; 
  }

  vtkSmartPointer<vtkTrackedFrameList> savedDataBuffer = vtkSmartPointer<vtkTrackedFrameList>::New(); 

  // Read metafile
  if ( savedDataBuffer->ReadFromSequenceMetafile(this->SequenceMetafile) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read video buffer from sequence metafile: "<<this->SequenceMetafile); 
    return PLUS_FAIL; 
  }

  if ( savedDataBuffer->GetNumberOfTrackedFrames() < 1 ) 
  {
    LOG_ERROR("Failed to connect to saved dataset - there is no frame in the sequence metafile!"); 
    return PLUS_FAIL; 
  }

  PlusStatus status=PLUS_FAIL;
  switch (this->SimulatedStream)
  {
  case VIDEO_STREAM:
    status=InternalConnectVideo(savedDataBuffer);
    break;
  case TRACKER_STREAM:
    status=InternalConnectTracker(savedDataBuffer);
    break;
  default:
    LOG_ERROR("Unknown stream type: "<<this->SimulatedStream);
  }

  if (status!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  if (GetLocalBuffer()==NULL)
  {
    LOG_ERROR("Local buffer is invalid");
    return PLUS_FAIL;
  }

  double oldestTimestamp_Local=0;
  GetLocalBuffer()->GetOldestTimeStamp(oldestTimestamp_Local);
  double latestTimestamp_Local=0;
  GetLocalBuffer()->GetLatestTimeStamp(latestTimestamp_Local);

  // Set the default loop start time and length to match the video buffer start time and length

  this->LoopFirstFrameUid=GetLocalBuffer()->GetOldestItemUidInBuffer();
  this->LoopLastFrameUid=GetLocalBuffer()->GetLatestItemUidInBuffer();

  this->LoopStartTime_Local=oldestTimestamp_Local;  

  // When we reach the last frame we have to wait one frame period before
  // playing the first frame, so we have to add one frame period to the loop length (loopTime)
  double framePeriodSec=0;
  double frameRate=GetLocalBuffer()->GetFrameRate();
  if (frameRate!=0.0)
  {
    framePeriodSec=1.0/frameRate;
  }
  else
  {
    // There is probably only one frame in the buffer, so use the AcquisitionRate
    // (instead of trying to find out the frame period from the frame rate in the file)
    if ( this->AcquisitionRate!=0.0 )
    {      
      framePeriodSec=1.0/this->AcquisitionRate;
    }
    else
    {
      LOG_ERROR("Invalid AcquisitionRate: "<<this->AcquisitionRate);      
      framePeriodSec=1.0;
    }
  }
  this->LoopStopTime_Local=latestTimestamp_Local+framePeriodSec;

  this->LastAddedFrameUid=this->LoopFirstFrameUid-1;
  this->LastAddedLoopIndex=0;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSavedDataSource::InternalConnectVideo(vtkTrackedFrameList* savedDataBuffer)
{
  // Set buffer parameters based on the input tracked frame list
  if ( this->GetOutputBuffer()->SetImageType( savedDataBuffer->GetImageType() ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set video buffer image type"); 
    return PLUS_FAIL; 
  }
  if (savedDataBuffer->GetImageType()==US_IMG_BRIGHTNESS)
  {
    // Brightness images will be imported into MF orientation
    this->GetOutputBuffer()->SetImageOrientation(US_IMG_ORIENT_MF);
    this->OutputChannels[0]->SetImageOrientation(US_IMG_ORIENT_MF);
  }
  else
  {
    // RF data is stored line-by-line, therefore set the storage buffer to FM orientation
    this->GetOutputBuffer()->SetImageOrientation(US_IMG_ORIENT_FM);
  }
  if ( this->GetOutputBuffer()->SetImageOrientation( savedDataBuffer->GetImageOrientation() ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set video iamge orientation"); 
    return PLUS_FAIL; 
  }

  this->GetOutputBuffer()->SetFrameSize(savedDataBuffer->GetTrackedFrame(0)->GetFrameSize());

  // Set up a new local buffer
  DeleteLocalBuffers();
  this->LocalVideoBuffer = vtkPlusBuffer::New(); 
  
  // Copy all the settings from the video buffer 
  this->LocalVideoBuffer->DeepCopy( this->GetOutputBuffer() );

  // Fill local video buffer

  this->LocalVideoBuffer->CopyImagesFromTrackedFrameList(savedDataBuffer, vtkPlusBuffer::READ_FILTERED_IGNORE_UNFILTERED_TIMESTAMPS, this->UseAllFrameFields); 
  savedDataBuffer->Clear(); 

  this->GetOutputBuffer()->Clear();
  this->GetOutputBuffer()->SetFrameSize( this->LocalVideoBuffer->GetFrameSize() ); 
  this->GetOutputBuffer()->SetPixelType( this->LocalVideoBuffer->GetPixelType() ); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkSavedDataSource::InternalConnectTracker(vtkTrackedFrameList* savedDataBuffer)
{
  TrackedFrame* frame=savedDataBuffer->GetTrackedFrame(0);
  if (frame==NULL)
  {
    LOG_ERROR("The tracked frame buffer doesn't seem to contain any frames");
    return PLUS_FAIL;
  }

  // Clear local buffers before connect 
  this->DeleteLocalBuffers(); 

  // Enable tools that have a matching transform name in the savedDataBuffer
  double transformMatrix[16]={0};
  for ( DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    vtkPlusDataSource* tool=it->second;
    if (tool->GetSourceId()==NULL)
    {
      // no tool name is available, don't connect it to any transform in the savedDataBuffer
      continue;
    }        

    PlusTransformName toolTransformName(tool->GetSourceId(), this->ToolReferenceFrameName ); 
    if (!frame->IsCustomFrameTransformNameDefined(toolTransformName) )
    {
      std::string strTransformName; 
      toolTransformName.GetTransformName(strTransformName); 
      LOG_WARNING("Tool '" << tool->GetSourceId() << "' has no matching transform in the file with name: " << strTransformName ); 
      continue;
    }

    if (frame->GetCustomFrameTransform(toolTransformName, transformMatrix)!=PLUS_SUCCESS)
    {
      LOG_WARNING("Cannot convert the custom frame field ( for tool "<<tool->GetSourceId()<<") to a transform");
      continue;
    }
    // a transform with the same name as the tool name has been found in the savedDataBuffer
    tool->GetBuffer()->SetBufferSize( savedDataBuffer->GetNumberOfTrackedFrames() ); 

    vtkSmartPointer<vtkPlusBuffer> buffer=vtkSmartPointer<vtkPlusBuffer>::New();
    // Copy all the settings from the default tool buffer 
    buffer->DeepCopy( tool->GetBuffer() ); 
    if (buffer->CopyTransformFromTrackedFrameList(savedDataBuffer, vtkPlusBuffer::READ_FILTERED_IGNORE_UNFILTERED_TIMESTAMPS, toolTransformName)!=PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to retrieve tracking data from tracked frame list for tool "<<tool->GetSourceId());
      return PLUS_FAIL;
    }

    buffer->Register(this);
    this->LocalTrackerBuffers[tool->GetSourceId()] = buffer;
  }

  savedDataBuffer->Clear();

  ClearAllBuffers();

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkSavedDataSource::InternalDisconnect()
{
  DeleteLocalBuffers();
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkSavedDataSource::ReadConfiguration(vtkXMLDataElement* config)
{
  LOG_TRACE("vtkSavedDataSource::ReadConfiguration"); 
  if ( config == NULL )
  {
    LOG_ERROR("Unable to configure Saved Data video source! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  // Read superclass configuration
  Superclass::ReadConfiguration(config); 

  vtkXMLDataElement* imageAcquisitionConfig = this->FindThisDeviceElement(config);
  if (imageAcquisitionConfig == NULL) 
  {
    LOG_ERROR("Unable to find ImageAcquisition element in configuration XML structure!");
    return PLUS_FAIL;
  }

  const char* sequenceMetafile = imageAcquisitionConfig->GetAttribute("SequenceMetafile"); 
  if ( sequenceMetafile != NULL ) 
  {
    std::string foundAbsoluteImagePath;
    if (vtkPlusConfig::GetAbsoluteImagePath(sequenceMetafile, foundAbsoluteImagePath) == PLUS_SUCCESS)
    {
      this->SetSequenceMetafile(foundAbsoluteImagePath.c_str());
    }
    else
    {
      LOG_ERROR("Unable to find SequenceMetafile element in configuration XML structure!");
      return PLUS_FAIL;
    }
  }

  const char* repeatEnabled = imageAcquisitionConfig->GetAttribute("RepeatEnabled"); 
  if ( repeatEnabled != NULL ) 
  {
    if ( STRCASECMP("TRUE", repeatEnabled ) == 0 )
    {
      this->RepeatEnabled = true; 
    }
    else if ( STRCASECMP("FALSE", repeatEnabled ) == 0 )
    {
      this->RepeatEnabled = false; 
    }
    else
    {
      LOG_WARNING("Unable to recognize RepeatEnabled attribute: " << repeatEnabled << " - changed to false by default!"); 
      this->RepeatEnabled = false; 
    }
  }

  const char* useData = imageAcquisitionConfig->GetAttribute("UseData"); 
  if ( useData != NULL ) 
  {
    if ( STRCASECMP("IMAGE", useData ) == 0 )
    {
      this->UseAllFrameFields = false; 
      this->SimulatedStream = VIDEO_STREAM;
    }
    else if ( STRCASECMP("IMAGE_AND_TRANSFORM", useData ) == 0 )
    {
      this->UseAllFrameFields = true; 
      this->SimulatedStream = VIDEO_STREAM;
    }
    else if ( STRCASECMP("TRANSFORM", useData ) == 0 )
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

  const char* useOriginalTimestamps = imageAcquisitionConfig->GetAttribute("UseOriginalTimestamps"); 
  if ( useOriginalTimestamps != NULL ) 
  {
    if ( STRCASECMP("TRUE", useOriginalTimestamps ) == 0 )
    {
      this->UseOriginalTimestamps = true; 
    }
    else if ( STRCASECMP("FALSE", useOriginalTimestamps ) == 0 )
    {
      this->UseOriginalTimestamps = false; 
    }
    else
    {
      LOG_WARNING("Unable to recognize UseOriginalTimestamps attribute: " << useOriginalTimestamps << " - changed to false by default!"); 
      this->UseOriginalTimestamps = false; 
    }
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkSavedDataSource::WriteConfiguration(vtkXMLDataElement* config)
{
  LOG_TRACE("vtkSavedDataSource::WriteConfiguration"); 

  // Write superclass configuration
  Superclass::WriteConfiguration(config); 

  if ( config == NULL )
  {
    LOG_ERROR("Config is invalid");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* imageAcquisitionConfig = this->FindThisDeviceElement(config);
  if (imageAcquisitionConfig == NULL) 
  {
    LOG_ERROR("Cannot find ImageAcquisition element in XML tree!");
    return PLUS_FAIL;
  }

  imageAcquisitionConfig->SetAttribute("SequenceMetafile", this->SequenceMetafile);

  if (this->RepeatEnabled)
  {
    imageAcquisitionConfig->SetAttribute("RepeatEnabled", "TRUE");
  }
  else
  {
    imageAcquisitionConfig->SetAttribute("RepeatEnabled", "FALSE");
  }

  if (this->UseAllFrameFields)
  {
    imageAcquisitionConfig->SetAttribute("UseData", "IMAGE_AND_TRANSFORM");
  }
  else
  {
    imageAcquisitionConfig->SetAttribute("UseData", "IMAGE");
  }

  if (this->UseOriginalTimestamps)
  {
    imageAcquisitionConfig->SetAttribute("UseOriginalTimestamps", "TRUE");
  }
  else
  {
    imageAcquisitionConfig->SetAttribute("UseOriginalTimestamps", "FALSE");
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkSavedDataSource::NotifyConfigured()
{
  if( this->OutputChannels.size() > 1 )
  {
    LOG_WARNING("vtkSavedDataSource is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
    return PLUS_FAIL;
  }

  if( this->OutputChannels.size() == 0 )
  {
    LOG_ERROR("No output channels defined for vtkSavedDataSource. Cannot proceed." );
    this->SetCorrectlyConfigured(false);
    return PLUS_FAIL;
  }

  switch (this->SimulatedStream)
  {
  case VIDEO_STREAM:
    if( this->GetOutputBuffer() == NULL )
    {
      LOG_ERROR("Buffer not created for vtkSavedDataSource but it is required. Check configuration.");
      return PLUS_FAIL;
    }
    break;
  case TRACKER_STREAM:
    // nothing to check for
    break;
  default:
    LOG_ERROR("Unknown stream type: " << this->SimulatedStream);
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkSavedDataSource::GetLoopTimeRange(double& loopStartTime, double& loopStopTime)
{
  loopStartTime=this->LoopStartTime_Local;
  loopStopTime=this->LoopStopTime_Local;
}

//-----------------------------------------------------------------------------
void vtkSavedDataSource::SetLoopTimeRange(double loopStartTime, double loopStopTime)
{
  this->LoopStartTime_Local=loopStartTime;
  this->LoopStopTime_Local=loopStopTime;

  this->LoopFirstFrameUid=GetClosestFrameUidWithinTimeRange(this->LoopStartTime_Local, this->LoopStartTime_Local, this->LoopStopTime_Local);
  this->LoopLastFrameUid=GetClosestFrameUidWithinTimeRange(this->LoopStopTime_Local, this->LoopStartTime_Local, this->LoopStopTime_Local);

  this->LastAddedFrameUid=this->LoopFirstFrameUid-1;
  this->LastAddedLoopIndex=0;  
}

//----------------------------------------------------------------------------
BufferItemUidType vtkSavedDataSource::GetClosestFrameUidWithinTimeRange(double time_Local, double startTime_Local, double stopTime_Local)
{
  // time_Local should be within the specified interval
  if (time_Local<startTime_Local)
  {
    time_Local=startTime_Local;
  }
  else if (time_Local>stopTime_Local)
  {
    time_Local=stopTime_Local;
  }
  // time_Local should be also within the local buffer time range
  double oldestTimestamp_Local=0;
  GetLocalBuffer()->GetOldestTimeStamp(oldestTimestamp_Local);
  double latestTimestamp_Local=0;
  GetLocalBuffer()->GetLatestTimeStamp(latestTimestamp_Local);

  // if the asked time is outside of the loop range then return the closest element in the range
  if (time_Local<oldestTimestamp_Local)
  {
    time_Local=oldestTimestamp_Local;
  }
  else if (time_Local>latestTimestamp_Local)
  {
    time_Local=latestTimestamp_Local;
  }

  // Get the uid of the frame that has been most recently acquired
  BufferItemUidType closestFrameUid=0;
  GetLocalBuffer()->GetItemUidFromTime(time_Local,closestFrameUid);
  double closestFrameTime_Local=0;
  GetLocalBuffer()->GetTimeStamp(closestFrameUid, closestFrameTime_Local);

  // The closest frame is at the boundary, but it may be just outside the range:
  // use the next/previous frame if the closest frame is on the wrong side of the boundary
  if (closestFrameTime_Local<startTime_Local)
  {
    return closestFrameUid + 1;
  } 
  else if (closestFrameTime_Local>stopTime_Local)
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
vtkPlusBuffer* vtkSavedDataSource::GetLocalTrackerBuffer()
{
  // Get the first tool - the first active tool determines the timestamp
  vtkPlusDataSource* firstActiveTool = NULL; 
  if ( this->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get local tracker buffer - there is no active tool!"); 
    return NULL; 
  }
  return this->LocalTrackerBuffers[firstActiveTool->GetSourceId()];
}

//----------------------------------------------------------------------------
void vtkSavedDataSource::DeleteLocalBuffers()
{
  if ( this->LocalVideoBuffer != NULL )
  {
    this->LocalVideoBuffer->Delete(); 
    this->LocalVideoBuffer = NULL; 
  }

  for (std::map<std::string, vtkPlusBuffer*>::iterator it=this->LocalTrackerBuffers.begin(); it!=this->LocalTrackerBuffers.end(); ++it)
  {    
    if ( (*it).second != NULL )
    {
      (*it).second->UnRegister(this); 
      (*it).second = NULL; 
    }
  }

  this->LocalTrackerBuffers.clear(); 
}

//----------------------------------------------------------------------------
vtkPlusBuffer* vtkSavedDataSource::GetLocalBuffer()
{
  vtkPlusBuffer* buff=NULL;
  switch (this->SimulatedStream)
  {
  case VIDEO_STREAM:
    buff=LocalVideoBuffer;
    break;
  case TRACKER_STREAM:
    buff=GetLocalTrackerBuffer();
    break;
  default:
    LOG_ERROR("Unkown stream type: "<<this->SimulatedStream);
  }
  if (buff==NULL)
  {
    LOG_WARNING("vtkSavedDataSource LocalBuffer is invalid");
  }
  return buff;
}

//----------------------------------------------------------------------------
vtkPlusBuffer* vtkSavedDataSource::GetOutputBuffer()
{
  vtkPlusBuffer* buff=NULL;

  switch (this->SimulatedStream)
  {
  case VIDEO_STREAM:
    {
      vtkPlusDataSource* aSource(NULL);
      if( this->OutputChannels[0]->GetVideoSource(aSource) != PLUS_SUCCESS )
      {
        LOG_ERROR("Unable to retrieve the video source in the SavedDataSource device.");
        return NULL;
      }
      buff = aSource->GetBuffer();
      break;
    }
  case TRACKER_STREAM:
    {
      vtkPlusDataSource* firstActiveTool = NULL; 
      if ( this->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get local tracker buffer - there is no active tool!"); 
      }
      else
      {
        buff=firstActiveTool->GetBuffer();
      }
    }
    break;
  default:
    LOG_ERROR("Unknown stream type: "<<this->SimulatedStream);
  }
  if (buff==NULL)
  {
    LOG_WARNING("vtkSavedDataSource OutputBuffer is invalid");
  }
  return buff;
}
