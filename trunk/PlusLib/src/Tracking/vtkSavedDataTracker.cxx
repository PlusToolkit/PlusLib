/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkSavedDataTracker.h"
#include "vtkObjectFactory.h"
#include <sstream>
#include "vtkTracker.h"
#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtksys/SystemTools.hxx"
#include "vtkXMLDataElement.h"
#include "vtkTrackedFrameList.h"

const int MAX_NUMBER_OF_TOOLS=12;

//----------------------------------------------------------------------------
vtkSavedDataTracker* vtkSavedDataTracker::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkSavedDataTracker");
  if(ret)
  {
    return (vtkSavedDataTracker*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkSavedDataTracker;
}

//----------------------------------------------------------------------------
vtkSavedDataTracker::vtkSavedDataTracker()
{
  for (int i=0; i<MAX_NUMBER_OF_TOOLS; i++)
  {
    vtkTrackerBuffer* buffer=vtkTrackerBuffer::New();
    this->LocalTrackerBuffers.push_back(buffer);
  }
  this->SequenceMetafile = NULL; 
  this->Initialized = false;
  this->ReplayEnabled = false; 
  this->LoopStartTime = 0.0; 
  this->LoopTime = 0.0; 
  this->FrameNumber = 0; 

  this->SetNumberOfTools(MAX_NUMBER_OF_TOOLS);
}

//----------------------------------------------------------------------------
vtkSavedDataTracker::~vtkSavedDataTracker() 
{
  if (this->Tracking)
  {
    this->StopTracking();
  }

  for (std::vector<vtkTrackerBuffer*>::iterator it=this->LocalTrackerBuffers.begin(); it!=this->LocalTrackerBuffers.end(); ++it)
  {    
    if ( (*it) != NULL )
    {
      (*it)->Delete(); 
      (*it) = NULL; 
    }
  }
  
}

//----------------------------------------------------------------------------
void vtkSavedDataTracker::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkTracker::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkSavedDataTracker::Connect()
{
  LOG_TRACE("vtkSavedDataTracker::Connect"); 
  vtkSmartPointer<vtkTrackedFrameList> savedDataBuffer = vtkSmartPointer<vtkTrackedFrameList>::New(); 

  if ( this->Probe()!=PLUS_SUCCESS )
  {
    return PLUS_FAIL; 
  }

  if ( this->Initialized )
  {
    return PLUS_SUCCESS;
  }

  // Read metafile
  if ( savedDataBuffer->ReadFromSequenceMetafile(this->GetSequenceMetafile()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read tracker buffer from sequence metafile!"); 
    return PLUS_FAIL; 
  }

  TrackedFrame* frame=savedDataBuffer->GetTrackedFrame(0);
  if (frame==NULL)
  {
    LOG_ERROR("The tracked frame buffer doesn't seem to contain any frames");
    return PLUS_FAIL;
  }

  // Enable tools that have a matching transform name in the savedDataBuffer
  double transformMatrix[16]={0};
  for ( int toolNumber = 0; toolNumber < this->GetNumberOfTools() && toolNumber < MAX_NUMBER_OF_TOOLS; toolNumber++ )
  {
    vtkTrackerTool* tool=this->GetTool(toolNumber);
    tool->EnabledOff();
    if (tool->GetToolName()==NULL)
    {
      // no tool name is available, don't connect it to any transform in the savedDataBuffer
      continue;
    }        
    if (!frame->IsCustomFrameFieldDefined(tool->GetToolName()))
    {
      // this tool has no matching transform in the file
      continue;
    }
    if (frame->GetCustomFrameTransform(tool->GetToolName(), transformMatrix)!=PLUS_SUCCESS)
    {
      LOG_WARNING("Cannot convert the custom frame field ("<<tool->GetToolName()<<", for tool "<<toolNumber<<") to a transform");
      continue;
    }
    // a transform with the same name as the tool name has been found in the savedDataBuffer
    tool->GetBuffer()->SetBufferSize( savedDataBuffer->GetNumberOfTrackedFrames() ); 
    tool->EnabledOn(); 	

    this->LocalTrackerBuffers[toolNumber]->Clear();
    // Copy all the settings from the default tool buffer 
    this->LocalTrackerBuffers[toolNumber]->DeepCopy( tool->GetBuffer() ); 

    if (this->LocalTrackerBuffers[toolNumber]->CopyTransformFromTrackedFrameList(savedDataBuffer, vtkTrackerBuffer::READ_FILTERED_IGNORE_UNFILTERED_TIMESTAMPS, tool->GetToolName())!=PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to retrieve tracking data from tracked frame list for tool "<<toolNumber);
      return PLUS_FAIL;
    }
  }

  savedDataBuffer->Clear();

  ClearAllBuffers();

  this->Initialized = true;
  return PLUS_SUCCESS; 
}

//-----------------------------------------------------------------------------
PlusStatus vtkSavedDataTracker::Disconnect()
{
  LOG_TRACE("vtkSavedDataTracker::Disconnect"); 
  return this->StopTracking(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkSavedDataTracker::Probe()
{
  LOG_TRACE("vtkSavedDataTracker::Probe"); 
  if ( !vtksys::SystemTools::FileExists(this->GetSequenceMetafile(), true) )
  {
    LOG_ERROR("SavedDataTracker Probe failed: Unable to find sequence metafile!"); 
    return PLUS_FAIL; 
  }
  return PLUS_SUCCESS; 
} 

//----------------------------------------------------------------------------
PlusStatus vtkSavedDataTracker::InternalStartTracking()
{
  LOG_TRACE("vtkSavedDataTracker::InternalStartTracking"); 
  if (this->Tracking)
  {
    return PLUS_SUCCESS;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSavedDataTracker::InternalStopTracking()
{
  LOG_TRACE("vtkSavedDataTracker::InternalStopTracking"); 
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSavedDataTracker::InternalUpdate()
{
  LOG_TRACE("vtkSavedDataTracker::InternalUpdate"); 
  if (!this->Tracking)
  {
    //not tracking (probably tracker is being started/stopped), ignore the received data
    return PLUS_SUCCESS;
  }

  int firstActiveToolNumber=-1; // the first active tool determines the timestamp
  if (this->GetFirstActiveTool(firstActiveToolNumber)!=PLUS_SUCCESS || firstActiveToolNumber<0)
  {
    LOG_ERROR("There is no active tool");
    return PLUS_FAIL;
  }

  double elapsedTime = vtkAccurateTimer::GetSystemTime() - this->GetStartTime(); 

  double latestFrameTimestamp(0); 
  if ( this->LocalTrackerBuffers[firstActiveToolNumber]->GetLatestTimeStamp(latestFrameTimestamp) != ITEM_OK )
  {
    LOG_ERROR("vtkSavedDataTracker: Unable to get latest timestamp from local buffer!");
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

  // The sampling rate is constant, so to have a constant frame rate we have to increase the FrameNumber by a constant.
  // For simplicity, we increase it always by 1.
  this->FrameNumber++;

  const double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();
  int numOfErrors=0;
  for ( int toolNumber = 0; toolNumber < this->GetNumberOfTools() && toolNumber < MAX_NUMBER_OF_TOOLS; toolNumber++ )
  {
    vtkTrackerTool* tool=this->GetTool(toolNumber);
    if (!tool->GetEnabled())
    {
      continue;
    }

    TrackerBufferItem bufferItem;  
    ItemStatus itemStatus = this->LocalTrackerBuffers[toolNumber]->GetTrackerBufferItemFromTime(nextFrameTimestamp, &bufferItem, vtkTrackerBuffer::INTERPOLATED); 
    if ( itemStatus != ITEM_OK )
    {
      if ( itemStatus == ITEM_NOT_AVAILABLE_YET )
      {
        LOG_ERROR("vtkSavedDataTracker: Unable to get next item from local buffer from time for tool "<<toolNumber<<" - frame not available yet!");
      }
      else if ( itemStatus == ITEM_NOT_AVAILABLE_ANYMORE )
      {
        LOG_ERROR("vtkSavedDataTracker: Unable to get next item from local buffer from time for tool "<<toolNumber<<" - frame not available anymore!");
      }
      else
      {
        LOG_ERROR("vtkSavedDataTracker: Unable to get next item from local buffer from time for tool "<<toolNumber<<"!");
      }
      numOfErrors++;
      continue;
    }

    // Get default transfom	
    vtkSmartPointer<vtkMatrix4x4> toolTransMatrix=vtkSmartPointer<vtkMatrix4x4>::New();
    if (bufferItem.GetMatrix(toolTransMatrix)!=PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to get toolTransMatrix for tool "<<toolNumber); 
      numOfErrors++;
      continue;
    }

    // Get flags
    TrackerStatus trackerStatus = bufferItem.GetStatus(); 

    // send the transformation matrix and flags to the tool
    if (this->ToolTimeStampedUpdate(toolNumber, toolTransMatrix, trackerStatus, this->FrameNumber, unfilteredTimestamp)!=PLUS_SUCCESS)
    {
      numOfErrors++;
    }
  }

  return (numOfErrors==0) ? PLUS_SUCCESS : PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkSavedDataTracker::ReadConfiguration(vtkXMLDataElement* config)
{
  LOG_TRACE("vtkSavedDataTracker::ReadConfiguration"); 
  if ( config == NULL ) 
  {
    LOG_ERROR("Unable to find SavedDataset XML data element");
    return PLUS_FAIL; 
  }

  Superclass::ReadConfiguration(config); 

  vtkXMLDataElement* dataCollectionConfig = config->FindNestedElementWithName("USDataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find USDataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* trackerConfig = dataCollectionConfig->FindNestedElementWithName("Tracker"); 
  if (trackerConfig == NULL) 
  {
    LOG_ERROR("Cannot find Tracker element in XML tree!");
    return PLUS_FAIL;
  }

  const char* sequenceMetafile = trackerConfig->GetAttribute("SequenceMetafile"); 
  if ( sequenceMetafile != NULL ) 
  {
    this->SetSequenceMetafile(vtkPlusConfig::GetAbsoluteImagePath(sequenceMetafile).c_str());
  }

  const char* replayEnabled = trackerConfig->GetAttribute("ReplayEnabled"); 
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

//----------------------------------------------------------------------------
PlusStatus vtkSavedDataTracker::WriteConfiguration(vtkXMLDataElement* config)
{
  LOG_TRACE("vtkSavedDataTracker::WriteConfiguration"); 

  if ( config == NULL )
  {
    LOG_ERROR("Config is invalid");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* dataCollectionConfig = config->FindNestedElementWithName("USDataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find USDataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* trackerConfig = dataCollectionConfig->FindNestedElementWithName("Tracker"); 
  if (trackerConfig == NULL) 
  {
    LOG_ERROR("Cannot find Tracker element in XML tree!");
    return PLUS_FAIL;
  }

  trackerConfig->SetName("SavedDataset");  
  trackerConfig->SetAttribute( "SequenceMetafile", this->GetSequenceMetafile() ); 

  return PLUS_SUCCESS;
}

vtkTrackerBuffer* vtkSavedDataTracker::GetLocalTrackerBuffer()
{
  int firstActiveToolNumber=-1; // the first active tool determines the timestamp
  if (this->GetFirstActiveTool(firstActiveToolNumber)!=PLUS_SUCCESS || firstActiveToolNumber<0)
  {
    LOG_ERROR("There is no active tool");
    return NULL;
  }
  if (firstActiveToolNumber>=MAX_NUMBER_OF_TOOLS)
  {
    LOG_ERROR("The active tool has no matching tool in the vtkSavedDataTracker");
    return NULL;
  }
  return this->LocalTrackerBuffers[firstActiveToolNumber];
}
