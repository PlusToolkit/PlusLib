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
	this->LocalTrackerBuffer = NULL;
	this->SequenceMetafile = NULL; 
	this->Tracking = 0;
	this->Initialized = false;
	this->ReplayEnabled = false; 
  this->LoopStartTime = 0.0; 
  this->LoopTime = 0.0; 
  this->FrameNumber = 0; 

	// TODO: Do we need other tools than the default tool? 
	this->SetNumberOfTools(1);
}

//----------------------------------------------------------------------------
vtkSavedDataTracker::~vtkSavedDataTracker() 
{
	if (this->Tracking)
	{
		this->StopTracking();
	}

	if ( this->LocalTrackerBuffer != NULL )
	{
		this->LocalTrackerBuffer->Delete(); 
		this->LocalTrackerBuffer = NULL; 
	}
}

//----------------------------------------------------------------------------
void vtkSavedDataTracker::PrintSelf(ostream& os, vtkIndent indent)
{
	vtkTracker::PrintSelf(os,indent);
}


/**
 * @returns 1 on success, 0 on failure.
 */
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

	// Enable tools
	for ( int tool = 0; tool < this->GetNumberOfTools(); tool++ )
	{
		this->GetTool(tool)->GetBuffer()->SetBufferSize( savedDataBuffer->GetNumberOfTrackedFrames() ); 
		this->GetTool(tool)->EnabledOn(); 
		
	}
  
	// Set tool names
	this->SetToolName(0, savedDataBuffer->GetDefaultFrameTransformName().c_str()); 
	
	// Set default tool name
	this->SetDefaultToolName(savedDataBuffer->GetDefaultFrameTransformName().c_str()); 
	
	// TODO: Read this from the config file.
	this->GetTool( this->GetDefaultTool() )->SetSendToLink( "localhost:18944" );
	
	if ( this->LocalTrackerBuffer == NULL )
	{
		this->LocalTrackerBuffer = vtkTrackerBuffer::New(); 
	}

	this->LocalTrackerBuffer->SetBufferSize(savedDataBuffer->GetNumberOfTrackedFrames()); 

	// Fill local buffers 
	for ( unsigned int frame = 0; frame < savedDataBuffer->GetNumberOfTrackedFrames(); ++frame)
	{
		TrackedFrame* trackedFrame = savedDataBuffer->GetTrackedFrame(frame); 

		// Get default transform 
		double defaultTransform[16]; 
		if ( !trackedFrame->GetDefaultFrameTransform(defaultTransform) )
		{
			LOG_WARNING("Unable to get default frame transform for frame #" << frame ); 
			continue; 
		}
		vtkSmartPointer<vtkMatrix4x4> defaultTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
		defaultTransformMatrix->DeepCopy(defaultTransform); 
		
		// Get frame number
		const char* strFrameNumber = trackedFrame->GetCustomFrameField("FrameNumber"); 
		long frameNumber = -1;
		if ( strFrameNumber == NULL ) 
		{
			frameNumber = frame;
		}
		else
		{
		  frameNumber = atol(strFrameNumber); 
		}

		// Get Timestamp
		const char* strTimestamp = trackedFrame->GetCustomFrameField("Timestamp"); 
		double timestamp = -1;
		if ( strTimestamp == NULL ) 
		{
			LOG_WARNING("Timestamp is missing, simulate a timestamp value with 10fps frame rate");
			timestamp = frameNumber / 10.0;  // This is not a normal behaviour.
		}
		else
		{
		  timestamp = atof(strTimestamp); 
		}

		// Get UnfilteredTimestamp
		const char* strUnfilteredTimestamp = trackedFrame->GetCustomFrameField("UnfilteredTimestamp"); 
		double unfilteredTimestamp = -1;
		if ( strUnfilteredTimestamp == NULL ) 
		{
			unfilteredTimestamp = timestamp;
		}
		else
		{
		  unfilteredTimestamp = atof(strUnfilteredTimestamp); 
		}

		// Get Status
		const char* strStatus = trackedFrame->GetCustomFrameField("Status"); 
		TrackerStatus status = TR_OK; 
		if ( strStatus == NULL ) 
		{
			LOG_DEBUG("Unable to get Status for frame #" << frame ); 
		}
		else
		{
		  if ( STRCASECMP(strStatus, "OK") != 0 )
		  {
			  status = TR_MISSING; 
		  }
		}
		
		LocalTrackerBuffer->AddTimeStampedItem(defaultTransformMatrix, status, frameNumber, unfilteredTimestamp); 
	}

    savedDataBuffer->Clear(); 
  
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

	if (!this->InitSavedDataTracker())
	{
		LOG_ERROR("Couldn't initialize SavedDataTracker");
		this->Tracking = 0;
		return PLUS_FAIL;
	} 

	this->Tracking = 1;

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

	double elapsedTime = vtkAccurateTimer::GetSystemTime() - this->GetStartTime(); 

	double latestFrameTimestamp(0); 
  if ( this->LocalTrackerBuffer->GetLatestTimeStamp(latestFrameTimestamp) != ITEM_OK )
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

  TrackerBufferItem bufferItem;  
  ItemStatus itemStatus = this->LocalTrackerBuffer->GetTrackerBufferItemFromTime(nextFrameTimestamp, &bufferItem); 
  if ( itemStatus != ITEM_OK )
	{
		if ( itemStatus == ITEM_NOT_AVAILABLE_YET )
		{
			LOG_ERROR("vtkSavedDataTracker: Unable to get next item from local buffer from time - frame not available yet!");
		}
		else if ( itemStatus == ITEM_NOT_AVAILABLE_ANYMORE )
		{
			LOG_ERROR("vtkSavedDataTracker: Unable to get next item from local buffer from time - frame not available anymore!");
		}
		else
		{
			LOG_ERROR("vtkSavedDataTracker: Unable to get next item from local buffer from time!");
		}
		return PLUS_FAIL; 
	}

	// Get frame number 
  if ( this->FrameNumber < bufferItem.GetIndex() )
  {
	  this->FrameNumber = bufferItem.GetIndex(); 
  }
  else
  {
    this->FrameNumber++; 
  }

	// Get default transfom	
  vtkSmartPointer<vtkMatrix4x4> defaultTransMatrix=vtkSmartPointer<vtkMatrix4x4>::New();
  if (bufferItem.GetMatrix(defaultTransMatrix)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to get defaultTransMatrix"); 
    return PLUS_FAIL;
  }

	// Get flags
	TrackerStatus trackerStatus = bufferItem.GetStatus(); 

  double unfilteredtimestamp = bufferItem.GetUnfilteredTimestamp( this->LocalTrackerBuffer->GetLocalTimeOffset() ); 

	// send the transformation matrix and flags to the tool
	PlusStatus updateStatus = this->ToolUpdate(0, defaultTransMatrix, trackerStatus, this->FrameNumber);   
  
  return updateStatus;
}

//----------------------------------------------------------------------------
PlusStatus vtkSavedDataTracker::InitSavedDataTracker()
{
	LOG_TRACE("vtkSavedDataTracker::InitSavedDataTracker"); 
	// Connect to device 
	return this->Connect(); 
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

	const char* sequenceMetafile = config->GetAttribute("SequenceMetafile"); 
	if ( sequenceMetafile != NULL ) 
	{
		this->SetSequenceMetafile(sequenceMetafile);
	}

	const char* replayEnabled = config->GetAttribute("ReplayEnabled"); 
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

	config->SetName("SavedDataset");  
	config->SetAttribute( "SequenceMetafile", this->GetSequenceMetafile() ); 
  
  return PLUS_SUCCESS;
}
