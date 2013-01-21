/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkSavedDataVideoSource.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtksys/SystemTools.hxx"
#include "vtkPlusStreamBuffer.h"
#include "vtkTrackedFrameList.h"

vtkCxxRevisionMacro(vtkSavedDataVideoSource, "$Revision: 1.0$");
vtkStandardNewMacro(vtkSavedDataVideoSource);

//----------------------------------------------------------------------------
vtkSavedDataVideoSource::vtkSavedDataVideoSource()
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

  this->RequireFrameBufferSizeInDeviceSetConfiguration = true;
  this->RequireAcquisitionRateInDeviceSetConfiguration = false;
  this->RequireAveragedItemsForFilteringInDeviceSetConfiguration = false;
  this->RequireLocalTimeOffsetSecInDeviceSetConfiguration = false;
  this->RequireUsImageOrientationInDeviceSetConfiguration = true;
  this->RequireRfElementInDeviceSetConfiguration = false;
  this->RequireDeviceImageOrientationInDeviceSetConfiguration=false; // device image orientation is not used, we'll use MF for B-mode and FM for RF-mode
}

//----------------------------------------------------------------------------
vtkSavedDataVideoSource::~vtkSavedDataVideoSource()
{ 
  if (!this->Connected)
  {
    this->Disconnect();
  }

  if ( this->LocalVideoBuffer != NULL )
  {
    this->LocalVideoBuffer->Delete(); 
    this->LocalVideoBuffer = NULL; 
  }
}

//----------------------------------------------------------------------------
void vtkSavedDataVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkSavedDataVideoSource::InternalUpdate()
{
  //LOG_TRACE("vtkSavedDataVideoSource::InternalUpdate");

  const int numberOfFramesInTheLoop=this->LoopLastFrameUid-this->LoopFirstFrameUid+1;

  // Determine the UID and loop index of the next frame that will be added
  BufferItemUidType frameToBeAddedUid=this->LastAddedFrameUid+1;
  int frameToBeAddedLoopIndex=this->LastAddedLoopIndex;
  if (frameToBeAddedUid>this->LoopLastFrameUid)
  {
    frameToBeAddedLoopIndex++;
    frameToBeAddedUid -= numberOfFramesInTheLoop;      
  }

  if (!this->UseOriginalTimestamps)
  {
    // Don't use the original timestamps, just replay with one frame at each update

    if (!this->RepeatEnabled && frameToBeAddedLoopIndex>0)
    {
      // there is no repeat and we already played the loop once, so don't add any more frames
      return PLUS_SUCCESS;
    }
    
    this->FrameNumber++;

    StreamBufferItem dataBufferItemToBeAdded; 
    if (this->LocalVideoBuffer->GetStreamBufferItem( frameToBeAddedUid, &dataBufferItemToBeAdded) != ITEM_OK )
    {
      LOG_ERROR("vtkSavedDataVideoSource: Failed to retrieve item from the buffer, UID=" << frameToBeAddedUid);
      return PLUS_FAIL;
    }    
    StreamBufferItem::FieldMapType fieldMap;
    if (this->UseAllFrameFields)
    {
      fieldMap = dataBufferItemToBeAdded.GetCustomFrameFieldMap();    
    }

    // UNDEFINED_TIMESTAMP => use current timestamp
    if (this->GetBuffer()->AddItem(&(dataBufferItemToBeAdded.GetFrame()), this->FrameNumber, UNDEFINED_TIMESTAMP, UNDEFINED_TIMESTAMP, &fieldMap)!=PLUS_SUCCESS)
    {
      return PLUS_FAIL;
    }

    this->LastAddedFrameUid=frameToBeAddedUid;
    this->LastAddedLoopIndex=frameToBeAddedLoopIndex;

    return PLUS_SUCCESS;
  }

  // Compute elapsed time since we started the acquisition
  double elapsedTime = vtkAccurateTimer::GetSystemTime() - this->GetBuffer()->GetStartTime();

  double loopTime=this->LoopStopTime_Local-this->LoopStartTime_Local;

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
    this->LocalVideoBuffer->GetItemUidFromTime(currentFrameTime_Local,closestFrameUid);
    double closestFrameTime_Local=0;
    this->LocalVideoBuffer->GetTimeStamp(closestFrameUid, closestFrameTime_Local);
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
    if (this->LocalVideoBuffer->GetStreamBufferItem( frameToBeAddedUid, &dataBufferItemToBeAdded) != ITEM_OK )
    {
      LOG_ERROR("vtkSavedDataVideoSource: Failed to retrieve item from the buffer, UID=" << frameToBeAddedUid);
      status=PLUS_FAIL;
      continue;
    }

    // Read the timestamp without any local time offset. Offset will be applied when it is copied to the video buffer.
    double filteredTimestamp=dataBufferItemToBeAdded.GetFilteredTimestamp(0.0)+frameToBeAddedLoopIndex*loopTime-this->LoopStartTime_Local;
    double unfilteredTimestamp=dataBufferItemToBeAdded.GetUnfilteredTimestamp(0.0)+frameToBeAddedLoopIndex*loopTime-this->LoopStartTime_Local;    // TODO: check what happens if no unfiltered timestamp is available

    StreamBufferItem::FieldMapType fieldMap;
    if (this->UseAllFrameFields)
    {
      fieldMap = dataBufferItemToBeAdded.GetCustomFrameFieldMap();    
    }

    if (this->GetBuffer()->AddItem(&(dataBufferItemToBeAdded.GetFrame()), this->FrameNumber, unfilteredTimestamp, filteredTimestamp, &fieldMap)==PLUS_FAIL)
    {
      status=PLUS_FAIL;
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
PlusStatus vtkSavedDataVideoSource::InternalConnect()
{
  LOG_TRACE("vtkSavedDataVideoSource::InternalConnect"); 

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

  // Set buffer parameters based on the input tracked frame list
  if ( this->GetBuffer()->SetImageType( savedDataBuffer->GetImageType() ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set video buffer image type"); 
    return PLUS_FAIL; 
  }
  if (savedDataBuffer->GetImageType()==US_IMG_BRIGHTNESS)
  {
    // Brightness images will be imported into MF orientation
    this->GetBuffer()->SetImageOrientation(US_IMG_ORIENT_MF);
    this->SetDeviceImageOrientation(US_IMG_ORIENT_MF);
  }
  else
  {
    // RF data is stored line-by-line, therefore set the storage buffer to FM orientation
    this->GetBuffer()->SetImageOrientation(US_IMG_ORIENT_FM);
  }
  if ( this->GetBuffer()->SetImageOrientation( savedDataBuffer->GetImageOrientation() ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set video iamge orientation"); 
    return PLUS_FAIL; 
  }

  this->GetBuffer()->SetFrameSize(savedDataBuffer->GetTrackedFrame(0)->GetFrameSize());

  // Set local buffer 
  if ( this->LocalVideoBuffer != NULL )
  {
    this->LocalVideoBuffer->Delete();
  }

  this->LocalVideoBuffer = vtkPlusStreamBuffer::New(); 
  // Copy all the settings from the video buffer 
  this->LocalVideoBuffer->DeepCopy( this->GetBuffer() );

  // Fill local video buffer

  this->LocalVideoBuffer->CopyImagesFromTrackedFrameList(savedDataBuffer, vtkPlusStreamBuffer::READ_FILTERED_IGNORE_UNFILTERED_TIMESTAMPS, this->UseAllFrameFields); 
  savedDataBuffer->Clear(); 

  double oldestTimestamp_Local=0;
  this->LocalVideoBuffer->GetOldestTimeStamp(oldestTimestamp_Local);
  double latestTimestamp_Local=0;
  this->LocalVideoBuffer->GetLatestTimeStamp(latestTimestamp_Local);

  // Set the default loop start time and length to match the video buffer start time and length

  this->LoopFirstFrameUid=this->LocalVideoBuffer->GetOldestItemUidInBuffer();
  this->LoopLastFrameUid=this->LocalVideoBuffer->GetLatestItemUidInBuffer();
  
  this->LoopStartTime_Local=oldestTimestamp_Local;  
  
  // When we reach the last frame we have to wait one frame period before
  // playing the first frame, so we have to add one frame period to the loop length (loopTime)
  double framePeriodSec=0;
  double frameRate=this->LocalVideoBuffer->GetFrameRate(true);
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
  
  this->GetBuffer()->Clear();
  this->GetBuffer()->SetFrameSize( this->LocalVideoBuffer->GetFrameSize() ); 
  this->GetBuffer()->SetPixelType( this->LocalVideoBuffer->GetPixelType() ); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkSavedDataVideoSource::InternalDisconnect()
{
  if ( this->LocalVideoBuffer != NULL )
  {
    this->LocalVideoBuffer->Delete(); 
    this->LocalVideoBuffer = NULL; 
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkSavedDataVideoSource::ReadConfiguration(vtkXMLDataElement* config)
{
  LOG_TRACE("vtkSavedDataVideoSource::ReadConfiguration"); 
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
    }
    else if ( STRCASECMP("IMAGE_AND_TRANSFORM", useData ) == 0 )
    {
      this->UseAllFrameFields = true; 
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
PlusStatus vtkSavedDataVideoSource::WriteConfiguration(vtkXMLDataElement* config)
{
  LOG_TRACE("vtkSavedDataVideoSource::WriteConfiguration"); 

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
PlusStatus vtkSavedDataVideoSource::NotifyConfigured()
{
  if( this->GetBuffer() == NULL )
  {
    LOG_ERROR("Buffer not created for vtkSavedDataVideoSource but it is required. Check configuration.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkSavedDataVideoSource::GetLoopTimeRange(double& loopStartTime, double& loopStopTime)
{
  loopStartTime=this->LoopStartTime_Local;
  loopStopTime=this->LoopStopTime_Local;
}

//-----------------------------------------------------------------------------
void vtkSavedDataVideoSource::SetLoopTimeRange(double loopStartTime, double loopStopTime)
{
  this->LoopStartTime_Local=loopStartTime;
  this->LoopStopTime_Local=loopStopTime;

  this->LoopFirstFrameUid=GetClosestFrameUidWithinTimeRange(this->LoopStartTime_Local, this->LoopStartTime_Local, this->LoopStopTime_Local);
  this->LoopLastFrameUid=GetClosestFrameUidWithinTimeRange(this->LoopStopTime_Local, this->LoopStartTime_Local, this->LoopStopTime_Local);

  this->LastAddedFrameUid=this->LoopFirstFrameUid-1;
  this->LastAddedLoopIndex=0;  
}

BufferItemUidType vtkSavedDataVideoSource::GetClosestFrameUidWithinTimeRange(double time_Local, double startTime_Local, double stopTime_Local)
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
  this->LocalVideoBuffer->GetOldestTimeStamp(oldestTimestamp_Local);
  double latestTimestamp_Local=0;
  this->LocalVideoBuffer->GetLatestTimeStamp(latestTimestamp_Local);

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
  this->LocalVideoBuffer->GetItemUidFromTime(time_Local,closestFrameUid);
  double closestFrameTime_Local=0;
  this->LocalVideoBuffer->GetTimeStamp(closestFrameUid, closestFrameTime_Local);

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
