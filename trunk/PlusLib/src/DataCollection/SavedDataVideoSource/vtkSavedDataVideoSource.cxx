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
  this->LoopStartTime = 0.0; 
  this->LoopTime = 0.0; 
  this->UseAllFrameFields = false;
  this->UseOriginalTimestamps = false;
  this->LastAddedFrameTimestamp=0;
  this->LastAddedFrameLoopIndex=0;

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

  // Compute elapsed time since we restarted the timer
  double elapsedTime = vtkAccurateTimer::GetSystemTime() - this->GetBuffer()->GetStartTime();

  double latestFrameTimestamp(0); 
  if ( this->LocalVideoBuffer->GetLatestTimeStamp(latestFrameTimestamp) != ITEM_OK )
  {
    LOG_ERROR("vtkSavedDataVideoSource: Unable to get latest timestamp from local buffer!");
    return PLUS_FAIL; 
  }

  // Compute the next timestamp 
  double currentFrameTimestamp = this->LoopStartTime + elapsedTime; 
  int currentFrameLoopIndex=0;
  if ( currentFrameTimestamp > latestFrameTimestamp )
  {
    if ( this->RepeatEnabled )
    {
      currentFrameTimestamp = this->LoopStartTime + fmod(elapsedTime, this->LoopTime); 
      currentFrameLoopIndex=floor(elapsedTime/this->LoopTime); // how many loops have we completed so far?
    }
    else
    {
      // Use the latest frame always
      currentFrameTimestamp = latestFrameTimestamp; 
    }
  }

  BufferItemUidType currentFrameUid=0;
  this->LocalVideoBuffer->GetItemUidFromTime(currentFrameTimestamp,currentFrameUid);
  if (currentFrameTimestamp<=this->LastAddedFrameTimestamp && currentFrameLoopIndex<=this->LastAddedFrameLoopIndex)
  {
    // this frame has been already added in this period, so nothing new to grab now
    return PLUS_SUCCESS;
  }

  // Get first and last frame index to be added
  unsigned long bufferIndexOfFirstFrameToBeAdded=0;
  if (this->GetBuffer()->GetNumberOfItems()>0)
  {
    // frames have been added already
    int bufferIndexOfLastAddedFrame = 0;  
    if (this->LocalVideoBuffer->GetBufferIndexFromTime(this->LastAddedFrameTimestamp, bufferIndexOfLastAddedFrame) != ITEM_OK)
    {
      LOG_ERROR("Unable to find the latest added frame, the local buffer is corrupted " << this->LastAddedFrameTimestamp);
      return PLUS_FAIL;
    }
    bufferIndexOfFirstFrameToBeAdded=bufferIndexOfLastAddedFrame+1;
    if (bufferIndexOfFirstFrameToBeAdded>=this->LocalVideoBuffer->GetNumberOfItems())
    {
      bufferIndexOfFirstFrameToBeAdded=0;
    }
  }
  int bufferIndexOfLastFrameToBeAdded=0;
  if (this->LocalVideoBuffer->GetBufferIndexFromTime(currentFrameTimestamp, bufferIndexOfLastFrameToBeAdded) != ITEM_OK)
  {
    LOG_ERROR("Unable to find the latest added frame, the local buffer is corrupted " << currentFrameUid);
    return PLUS_FAIL;
  }

  int numberOfFramesToBeAdded=(bufferIndexOfLastFrameToBeAdded-bufferIndexOfFirstFrameToBeAdded+1)+
    (currentFrameLoopIndex-this->LastAddedFrameLoopIndex)*this->LocalVideoBuffer->GetNumberOfItems();
 
  PlusStatus status=PLUS_SUCCESS;

  // add at least one frame
  int bufferIndexOfFrameToBeAdded=bufferIndexOfFirstFrameToBeAdded;
  int loopIndexOfFrameToBeAdded=this->LastAddedFrameLoopIndex;
  for (int addedFrames=0; addedFrames<numberOfFramesToBeAdded; addedFrames++)
  {

    // The sampling rate is constant, so to have a constant frame rate we have to increase the FrameNumber by a constant.
    // For simplicity, we increase it always by 1.
    this->FrameNumber++;
       
    BufferItemUidType uidOfFrameToBeAdded=-1;
    if (this->LocalVideoBuffer->GetItemUidFromBufferIndex(bufferIndexOfFrameToBeAdded, uidOfFrameToBeAdded)!=ITEM_OK)
    {
      LOG_ERROR("vtkSavedDataVideoSource: Failed to retrieve item from the buffer " << bufferIndexOfFrameToBeAdded);
      status=PLUS_FAIL;
      continue;
    }
    StreamBufferItem DataBufferItemToBeAdded; 
    if (this->LocalVideoBuffer->GetStreamBufferItem( uidOfFrameToBeAdded, &DataBufferItemToBeAdded) != ITEM_OK )
    {
      LOG_ERROR("vtkSavedDataVideoSource: Failed to retrieve item from the buffer " << bufferIndexOfFrameToBeAdded);
      status=PLUS_FAIL;
      continue;
    }

    double filteredTimestamp=UNDEFINED_TIMESTAMP;
    double unfilteredTimestamp=UNDEFINED_TIMESTAMP;
    if (this->UseOriginalTimestamps)
    {    
      // Read the timestamp without any local time offset. Offset will be applied when it is copied to the video buffer.
      filteredTimestamp=DataBufferItemToBeAdded.GetFilteredTimestamp(0.0)+loopIndexOfFrameToBeAdded*this->LoopTime;
      unfilteredTimestamp=DataBufferItemToBeAdded.GetUnfilteredTimestamp(0.0)+loopIndexOfFrameToBeAdded*this->LoopTime;    
    }

    StreamBufferItem::FieldMapType fieldMap;
    if (this->UseAllFrameFields)
    {
      fieldMap = DataBufferItemToBeAdded.GetCustomFrameFieldMap();    
    }

    if (this->GetBuffer()->AddItem(&(DataBufferItemToBeAdded.GetFrame()), this->FrameNumber, unfilteredTimestamp, filteredTimestamp, &fieldMap)==PLUS_FAIL)
    {
      status=PLUS_FAIL;
    }  

    bufferIndexOfFrameToBeAdded++;
    if (bufferIndexOfFrameToBeAdded>=this->LocalVideoBuffer->GetNumberOfItems())
    {
      bufferIndexOfFrameToBeAdded=0;
      loopIndexOfFrameToBeAdded++;
    }
  }

  this->LastAddedFrameTimestamp=currentFrameTimestamp;
  this->LastAddedFrameLoopIndex=currentFrameLoopIndex;

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

  this->LocalVideoBuffer->GetOldestTimeStamp(this->LastAddedFrameTimestamp);

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
