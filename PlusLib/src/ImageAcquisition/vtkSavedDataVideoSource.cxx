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
#include "vtkVideoBuffer.h"
#include "vtkTrackedFrameList.h"


vtkCxxRevisionMacro(vtkSavedDataVideoSource, "$Revision: 1.0$");
vtkStandardNewMacro(vtkSavedDataVideoSource);

//----------------------------------------------------------------------------
vtkSavedDataVideoSource::vtkSavedDataVideoSource()
{
  this->FrameBufferRowAlignment = 1;
  this->LocalVideoBuffer = NULL;
  this->SequenceMetafile = NULL; 
  this->ReplayEnabled = false; 
  this->LoopStartTime = 0.0; 
  this->LoopTime = 0.0; 
  this->SpawnThreadForRecording = true;
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
PlusStatus vtkSavedDataVideoSource::InternalGrab()
{
  //LOG_TRACE("vtkSavedDataVideoSource::InternalGrab");

  // Compute elapsed time since we restarted the timer
  double elapsedTime = vtkAccurateTimer::GetSystemTime() - this->Buffer->GetStartTime();

  double latestFrameTimestamp(0); 
  if ( this->LocalVideoBuffer->GetLatestTimeStamp(latestFrameTimestamp) != ITEM_OK )
  {
    LOG_ERROR("vtkSavedDataVideoSource: Unable to get latest timestamp from local buffer!");
    return PLUS_FAIL; 
  }

  // Compute the next timestamp 
  double nextFrameTimestamp = this->LoopStartTime + elapsedTime; 
  if ( nextFrameTimestamp > latestFrameTimestamp )
  {
    if ( this->ReplayEnabled )
    {
      nextFrameTimestamp = this->LoopStartTime + fmod(elapsedTime, this->LoopTime); 
    }
    else
    {
      // Use the latest frame always
      nextFrameTimestamp = latestFrameTimestamp; 
    }
  }

  VideoBufferItem nextVideoBufferItem; 
  ItemStatus nextItemStatus = this->LocalVideoBuffer->GetVideoBufferItemFromTime( nextFrameTimestamp, &nextVideoBufferItem); 
  if ( nextItemStatus != ITEM_OK )
  {
    if ( nextItemStatus == ITEM_NOT_AVAILABLE_YET )
    {
      LOG_ERROR("vtkSavedDataVideoSource: Unable to get next item from local buffer from time - frame not available yet !");
    }
    else if ( nextItemStatus == ITEM_NOT_AVAILABLE_ANYMORE )
    {
      LOG_ERROR("vtkSavedDataVideoSource: Unable to get next item from local buffer from time - frame not available anymore !");
    }
    else
    {
      LOG_ERROR("vtkSavedDataVideoSource: Unable to get next item from local buffer from time!");
    }
    return PLUS_FAIL; 
  }

  // The sampling rate is constant, so to have a constant frame rate we have to increase the FrameNumber by a constant.
  // For simplicity, we increase it always by 1.
  this->FrameNumber++;

  PlusStatus status = this->Buffer->AddItem(&(nextVideoBufferItem.GetFrame()), this->FrameNumber); 
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

  // Set buffer size 
  if ( this->SetFrameBufferSize( savedDataBuffer->GetNumberOfTrackedFrames() ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set video buffer size"); 
    return PLUS_FAIL; 
  }
  if ( this->Buffer->SetImageType( savedDataBuffer->GetImageType() ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set video buffer image type"); 
    return PLUS_FAIL; 
  }
  if (savedDataBuffer->GetImageType()==US_IMG_BRIGHTNESS)
  {
    // Brightness images will be imported into MF orientation
    this->Buffer->SetImageOrientation(US_IMG_ORIENT_MF);
  }
  else
  {
    // RF data is stored line-by-line, therefore set the storage buffer to FM orientation
    this->Buffer->SetImageOrientation(US_IMG_ORIENT_FM);
  }
  if ( this->Buffer->SetImageOrientation( savedDataBuffer->GetImageOrientation() ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set video iamge orientation"); 
    return PLUS_FAIL; 
  }

  // Set local buffer 
  if ( this->LocalVideoBuffer != NULL )
  {
    this->LocalVideoBuffer->Delete();
  }

  this->LocalVideoBuffer = vtkVideoBuffer::New(); 
  // Copy all the settings from the video buffer 
  this->LocalVideoBuffer->DeepCopy( this->Buffer );

  // Fill local video buffer
  this->LocalVideoBuffer->CopyImagesFromTrackedFrameList(savedDataBuffer, vtkVideoBuffer::READ_FILTERED_IGNORE_UNFILTERED_TIMESTAMPS); 
  savedDataBuffer->Clear(); 

  this->Buffer->Clear();
  this->Buffer->SetFrameSize( this->LocalVideoBuffer->GetFrameSize() ); 
  this->Buffer->SetPixelType( this->LocalVideoBuffer->GetPixelType() ); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkSavedDataVideoSource::InternalDisconnect()
{
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

	vtkXMLDataElement* dataCollectionConfig = config->FindNestedElementWithName("DataCollection");
	if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
		return PLUS_FAIL;
	}

  vtkXMLDataElement* imageAcquisitionConfig = dataCollectionConfig->FindNestedElementWithName("ImageAcquisition"); 
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

  const char* replayEnabled = imageAcquisitionConfig->GetAttribute("ReplayEnabled"); 
  if ( replayEnabled != NULL ) 
  {
    if ( STRCASECMP("TRUE", replayEnabled ) == 0 )
    {
      this->ReplayEnabled = true; 
    }
    else if ( STRCASECMP("FALSE", replayEnabled ) == 0 )
    {
      this->ReplayEnabled = false; 
    }
    else
    {
      LOG_WARNING("Unable to recognize ReplayEnabled attribute: " << replayEnabled << " - changed to false by default!"); 
      this->ReplayEnabled = false; 
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

  vtkXMLDataElement* dataCollectionConfig = config->FindNestedElementWithName("DataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* imageAcquisitionConfig = dataCollectionConfig->FindNestedElementWithName("ImageAcquisition"); 
  if (imageAcquisitionConfig == NULL) 
  {
    LOG_ERROR("Cannot find ImageAcquisition element in XML tree!");
    return PLUS_FAIL;
  }

  imageAcquisitionConfig->SetAttribute("SequenceMetafile", this->SequenceMetafile);

  if (this->ReplayEnabled)
  {
    imageAcquisitionConfig->SetAttribute("ReplayEnabled", "TRUE");
  }
  else
  {
    imageAcquisitionConfig->SetAttribute("ReplayEnabled", "FALSE");
  }

  return PLUS_SUCCESS;
}

