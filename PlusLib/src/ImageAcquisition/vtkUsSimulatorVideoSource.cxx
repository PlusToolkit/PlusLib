/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkUsSimulatorVideoSource.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkVideoBuffer.h"
#include "vtkTrackedFrameList.h"


vtkCxxRevisionMacro(vtkUsSimulatorVideoSource, "$Revision: 1.0$");
//----------------------------------------------------------------------------
// Needed when we don't use the vtkStandardNewMacro.
vtkInstantiatorNewMacro(vtkUsSimulatorVideoSource);

//----------------------------------------------------------------------------

vtkUsSimulatorVideoSource* vtkUsSimulatorVideoSource::Instance = 0;
vtkUsSimulatorVideoSourceCleanup2 vtkUsSimulatorVideoSource::Cleanup;


//----------------------------------------------------------------------------
vtkUsSimulatorVideoSourceCleanup2::vtkUsSimulatorVideoSourceCleanup2()
{
}

//----------------------------------------------------------------------------
vtkUsSimulatorVideoSourceCleanup2::~vtkUsSimulatorVideoSourceCleanup2()
{
  // Destroy any remaining video source
  vtkUsSimulatorVideoSource::SetInstance(NULL);
}
//----------------------------------------------------------------------------
vtkUsSimulatorVideoSource::vtkUsSimulatorVideoSource()
{
  this->UsSimulator = NULL; 
  this->LocalVideoBuffer = NULL;
}

//----------------------------------------------------------------------------
vtkUsSimulatorVideoSource::~vtkUsSimulatorVideoSource()
{ 
  if (!this->Connected)
  {
    this->Disconnect();
  }

  if ( this->UsSimulator != NULL )
  {
    this->UsSimulator->Delete(); 
    this->UsSimulator = NULL; 
  }
  if ( this->LocalVideoBuffer != NULL )
  {
    this->LocalVideoBuffer->Delete(); 
    this->LocalVideoBuffer = NULL; 
  }
}

//----------------------------------------------------------------------------
// Up the reference count so it behaves like New
vtkUsSimulatorVideoSource* vtkUsSimulatorVideoSource::New()
{
  vtkUsSimulatorVideoSource* ret = vtkUsSimulatorVideoSource::GetInstance();
  ret->Register(NULL);
  return ret;
}

//----------------------------------------------------------------------------
// Return the single instance of the vtkOutputWindow
vtkUsSimulatorVideoSource* vtkUsSimulatorVideoSource::GetInstance()
{
  if(!vtkUsSimulatorVideoSource::Instance)
  {
    // Try the factory first
    vtkUsSimulatorVideoSource::Instance = (vtkUsSimulatorVideoSource*)vtkObjectFactory::CreateInstance("vtkUsSimulatorVideoSource");    
    if(!vtkUsSimulatorVideoSource::Instance)
    {
      vtkUsSimulatorVideoSource::Instance = new vtkUsSimulatorVideoSource();     
    }
    if(!vtkUsSimulatorVideoSource::Instance)
    {
      int error = 0;
    }
  }
  // return the instance
  return vtkUsSimulatorVideoSource::Instance;
}

//----------------------------------------------------------------------------
void vtkUsSimulatorVideoSource::SetInstance(vtkUsSimulatorVideoSource* instance)
{
  if (vtkUsSimulatorVideoSource::Instance==instance)
  {
    return;
  }
  // preferably this will be NULL
  if (vtkUsSimulatorVideoSource::Instance)
  {
    vtkUsSimulatorVideoSource::Instance->Delete();;
  }
  vtkUsSimulatorVideoSource::Instance = instance;
  if (!instance)
  {
    return;
  }
  // user will call ->Delete() after setting instance
  instance->Register(NULL);
}
//----------------------------------------------------------------------------
void vtkUsSimulatorVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkUsSimulatorVideoSource::InternalGrab()
{/*
  //LOG_TRACE("vtkUsSimulatorVideoSource::InternalGrab");

  // Compute elapsed time since we restarted the timer
  double elapsedTime = vtkAccurateTimer::GetSystemTime() - this->GetBuffer()->GetStartTime(); 

  double latestFrameTimestamp(0); 
  if ( this->LocalVideoBuffer->GetLatestTimeStamp(latestFrameTimestamp) != ITEM_OK )
  {
    LOG_ERROR("vtkUsSimulatorVideoSource: Unable to get latest timestamp from local buffer!");
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
      LOG_ERROR("vtkUsSimulatorVideoSource: Unable to get next item from local buffer from time - frame not available yet !");
    }
    else if ( nextItemStatus == ITEM_NOT_AVAILABLE_ANYMORE )
    {
      LOG_ERROR("vtkUsSimulatorVideoSource: Unable to get next item from local buffer from time - frame not available anymore !");
    }
    else
    {
      LOG_ERROR("vtkUsSimulatorVideoSource: Unable to get next item from local buffer from time!");
    }
    return PLUS_FAIL; 
  }

  // The sampling rate is constant, so to have a constant frame rate we have to increase the FrameNumber by a constant.
  // For simplicity, we increase it always by 1.
  this->FrameNumber++;

  PlusStatus status = this->Buffer->AddItem(&(nextVideoBufferItem.GetFrame()), this->GetUsImageOrientation(), this->FrameNumber); 
  this->Modified();
  return status;*/return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsSimulatorVideoSource::InternalConnect()
{/*
  LOG_TRACE("vtkUsSimulatorVideoSource::InternalConnect"); 

  if (this->GetSequenceMetafile()==NULL)
  {
    LOG_ERROR("Unable to connect to saved data video source: Unable to read sequence metafile. No filename is specified."); 
    return PLUS_FAIL; 
  }
  if ( !vtksys::SystemTools::FileExists(this->GetSequenceMetafile(), true) )
  {
    LOG_ERROR("Unable to connect to saved data video source: Unable to read sequence metafile: "<<this->GetSequenceMetafile()); 
    return PLUS_FAIL; 
  }

  vtkSmartPointer<vtkTrackedFrameList> savedDataBuffer = vtkSmartPointer<vtkTrackedFrameList>::New(); 

  // Read metafile
  if ( savedDataBuffer->ReadFromSequenceMetafile(this->GetSequenceMetafile()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read video buffer from sequence metafile!"); 
    return PLUS_FAIL; 
  }

  if ( savedDataBuffer->GetNumberOfTrackedFrames() < 1 ) 
  {
    LOG_ERROR("Failed to connect to saved dataset - there is no frame in the sequence metafile!"); 
    return PLUS_FAIL; 
  }

  // Set to default MF internal image orientation (sequence metafile reader always converts it to MF)
  this->SetUsImageOrientation(US_IMG_ORIENT_MF); 

  // Set buffer size 
  if ( this->SetFrameBufferSize( savedDataBuffer->GetNumberOfTrackedFrames() ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set video buffer size!"); 
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

  return PLUS_SUCCESS;*/return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsSimulatorVideoSource::InternalDisconnect()
{
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkUsSimulatorVideoSource::ReadConfiguration(vtkXMLDataElement* config)
{/*
  LOG_TRACE("vtkUsSimulatorVideoSource::ReadConfiguration"); 
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
      LOG_ERROR("Cannot find input sequence metafile!");
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

  return PLUS_SUCCESS;*/return PLUS_FAIL;
}

//-----------------------------------------------------------------------------
PlusStatus vtkUsSimulatorVideoSource::WriteConfiguration(vtkXMLDataElement* config)
{/*
  LOG_TRACE("vtkUsSimulatorVideoSource::WriteConfiguration"); 

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

  return PLUS_SUCCESS;*/return PLUS_FAIL;
}

