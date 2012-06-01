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
#include "TrackedFrame.h"

vtkStandardNewMacro(vtkSavedDataTracker);

//----------------------------------------------------------------------------
vtkSavedDataTracker::vtkSavedDataTracker()
{
  this->SequenceMetafile = NULL; 
  this->Initialized = false;
  this->ReplayEnabled = false; 
  this->LoopStartTime = 0.0; 
  this->LoopTime = 0.0; 
}

//----------------------------------------------------------------------------
vtkSavedDataTracker::~vtkSavedDataTracker() 
{
  if (this->Recording)
  {
    this->StopTracking();
  }

  this->DeleteLocalTrackerBuffers(); 
}

//----------------------------------------------------------------------------
void vtkSavedDataTracker::DeleteLocalTrackerBuffers()
{
  for (std::map<std::string, vtkTrackerBuffer*>::iterator it=this->LocalTrackerBuffers.begin(); it!=this->LocalTrackerBuffers.end(); ++it)
  {    
    if ( (*it).second != NULL )
    {
      (*it).second->Delete(); 
      (*it).second = NULL; 
    }
  }

  this->LocalTrackerBuffers.clear(); 
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

  // Clear local buffers before connect 
  this->DeleteLocalTrackerBuffers(); 
  
  // Enable tools that have a matching transform name in the savedDataBuffer
  double transformMatrix[16]={0};
  for ( ToolIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    vtkTrackerTool* tool=it->second;
    if (tool->GetToolName()==NULL)
    {
      // no tool name is available, don't connect it to any transform in the savedDataBuffer
      continue;
    }        
    
    PlusTransformName toolTransformName(tool->GetToolName(), this->ToolReferenceFrameName ); 
    if (!frame->IsCustomFrameTransformNameDefined(toolTransformName) )
    {
      std::string strTransformName; 
      toolTransformName.GetTransformName(strTransformName); 
      LOG_WARNING("Tool '" << tool->GetToolName() << "' has no matching transform in the file with name: " << strTransformName ); 
      continue;
    }
    
    if (frame->GetCustomFrameTransform(toolTransformName, transformMatrix)!=PLUS_SUCCESS)
    {
      LOG_WARNING("Cannot convert the custom frame field ( for tool "<<tool->GetToolName()<<") to a transform");
      continue;
    }
    // a transform with the same name as the tool name has been found in the savedDataBuffer
    tool->GetBuffer()->SetBufferSize( savedDataBuffer->GetNumberOfTrackedFrames() ); 

    vtkTrackerBuffer* buffer=vtkTrackerBuffer::New();
    // Copy all the settings from the default tool buffer 
    buffer->DeepCopy( tool->GetBuffer() ); 
    if (buffer->CopyTransformFromTrackedFrameList(savedDataBuffer, vtkTrackerBuffer::READ_FILTERED_IGNORE_UNFILTERED_TIMESTAMPS, toolTransformName)!=PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to retrieve tracking data from tracked frame list for tool "<<tool->GetToolName());
      return PLUS_FAIL;
    }

    this->LocalTrackerBuffers[tool->GetToolName()] = buffer;
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
  if (this->Recording)
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
  if (!this->Recording)
  {
    //not tracking (probably tracker is being started/stopped), ignore the received data
    return PLUS_SUCCESS;
  }

  // Get the first tool - the first active tool determines the timestamp
  vtkTrackerTool* firstActiveTool = NULL; 
  if ( this->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
  {
    LOG_ERROR("Internal update failed - there is no active tool!"); 
    return PLUS_FAIL; 
  }

  double elapsedTime = vtkAccurateTimer::GetSystemTime() - this->GetStartTime(); 

  double latestFrameTimestamp(0); 
  if ( this->LocalTrackerBuffers[firstActiveTool->GetToolName()]->GetLatestTimeStamp(latestFrameTimestamp) != ITEM_OK )
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

  const double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();
  int numOfErrors=0;
  for ( ToolIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    vtkTrackerTool* tool=it->second;

    TrackerBufferItem bufferItem;  
    ItemStatus itemStatus = this->LocalTrackerBuffers[tool->GetToolName()]->GetTrackerBufferItemFromTime(nextFrameTimestamp, &bufferItem, vtkTrackerBuffer::INTERPOLATED); 
    if ( itemStatus != ITEM_OK )
    {
      if ( itemStatus == ITEM_NOT_AVAILABLE_YET )
      {
        LOG_ERROR("vtkSavedDataTracker: Unable to get next item from local buffer from time for tool "<<tool->GetToolName()<<" - frame not available yet!");
      }
      else if ( itemStatus == ITEM_NOT_AVAILABLE_ANYMORE )
      {
        LOG_ERROR("vtkSavedDataTracker: Unable to get next item from local buffer from time for tool "<<tool->GetToolName()<<" - frame not available anymore!");
      }
      else
      {
        LOG_ERROR("vtkSavedDataTracker: Unable to get next item from local buffer from time for tool "<<tool->GetToolName()<<"!");
      }
      numOfErrors++;
      continue;
    }

    // Get default transfom	
    vtkSmartPointer<vtkMatrix4x4> toolTransMatrix=vtkSmartPointer<vtkMatrix4x4>::New();
    if (bufferItem.GetMatrix(toolTransMatrix)!=PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to get toolTransMatrix for tool "<<tool->GetToolName()); 
      numOfErrors++;
      continue;
    }

    // Get flags
    ToolStatus toolStatus = bufferItem.GetStatus(); 

    // send the transformation matrix and flags to the tool
    if (this->ToolTimeStampedUpdate(tool->GetToolName(), toolTransMatrix, toolStatus, unfilteredTimestamp)!=PLUS_SUCCESS)
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

  // Read superclass configuration
  Superclass::ReadConfiguration(config); 

  vtkXMLDataElement* dataCollectionConfig = config->FindNestedElementWithName("DataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
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

  // Write superclass configuration
  Superclass::WriteConfiguration(config); 

  vtkXMLDataElement* dataCollectionConfig = config->FindNestedElementWithName("DataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* trackerConfig = dataCollectionConfig->FindNestedElementWithName("Tracker"); 
  if (trackerConfig == NULL) 
  {
    LOG_ERROR("Cannot find Tracker element in XML tree!");
    return PLUS_FAIL;
  }

  trackerConfig->SetAttribute("SequenceMetafile", this->SequenceMetafile);

  if (this->ReplayEnabled)
  {
    trackerConfig->SetAttribute("ReplayEnabled", "TRUE");
  }
  else
  {
    trackerConfig->SetAttribute("ReplayEnabled", "FALSE");
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
vtkTrackerBuffer* vtkSavedDataTracker::GetLocalTrackerBuffer()
{
  // Get the first tool - the first active tool determines the timestamp
  vtkTrackerTool* firstActiveTool = NULL; 
  if ( this->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get local tracker buffer - there is no active tool!"); 
    return NULL; 
  }
  return this->LocalTrackerBuffers[firstActiveTool->GetToolName()];
}
