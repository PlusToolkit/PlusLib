#include "PlusConfigure.h"
#include "vtkSavedDataVideoSource.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtksys/SystemTools.hxx"
#include "vtkVideoBuffer2.h"
#include "vtkVideoFrame2.h"
#include "vtkTrackedFrameList.h"


vtkCxxRevisionMacro(vtkSavedDataVideoSource, "$Revision: 1.0$");
//----------------------------------------------------------------------------
// Needed when we don't use the vtkStandardNewMacro.
vtkInstantiatorNewMacro(vtkSavedDataVideoSource);

//----------------------------------------------------------------------------

vtkSavedDataVideoSource* vtkSavedDataVideoSource::Instance = 0;
vtkSavedDataVideoSourceCleanup2 vtkSavedDataVideoSource::Cleanup;


//----------------------------------------------------------------------------
vtkSavedDataVideoSourceCleanup2::vtkSavedDataVideoSourceCleanup2()
{
}

//----------------------------------------------------------------------------
vtkSavedDataVideoSourceCleanup2::~vtkSavedDataVideoSourceCleanup2()
{
	// Destroy any remaining video source
	vtkSavedDataVideoSource::SetInstance(NULL);
}
//----------------------------------------------------------------------------
vtkSavedDataVideoSource::vtkSavedDataVideoSource()
{
	this->FrameBufferRowAlignment = 1;
	this->LocalVideoBuffer = NULL;
	this->SequenceMetafile = NULL; 
	this->StartTimestamp = 0.0; 
	this->ReplayEnabled = false; 
}

//----------------------------------------------------------------------------
vtkSavedDataVideoSource::~vtkSavedDataVideoSource()
{ 
	this->vtkSavedDataVideoSource::ReleaseSystemResources();

	if ( this->LocalVideoBuffer != NULL )
	{
		this->LocalVideoBuffer->Delete(); 
		this->LocalVideoBuffer = NULL; 
	}
}

//----------------------------------------------------------------------------
// Up the reference count so it behaves like New
vtkSavedDataVideoSource* vtkSavedDataVideoSource::New()
{
	vtkSavedDataVideoSource* ret = vtkSavedDataVideoSource::GetInstance();
	ret->Register(NULL);
	return ret;
}

//----------------------------------------------------------------------------
// Return the single instance of the vtkOutputWindow
vtkSavedDataVideoSource* vtkSavedDataVideoSource::GetInstance()
{
	if(!vtkSavedDataVideoSource::Instance)
	{
		// Try the factory first
		vtkSavedDataVideoSource::Instance = (vtkSavedDataVideoSource*)vtkObjectFactory::CreateInstance("vtkSavedDataVideoSource");    
		if(!vtkSavedDataVideoSource::Instance)
		{
			vtkSavedDataVideoSource::Instance = new vtkSavedDataVideoSource();     
		}
		if(!vtkSavedDataVideoSource::Instance)
		{
			int error = 0;
		}
	}
	// return the instance
	return vtkSavedDataVideoSource::Instance;
}

//----------------------------------------------------------------------------
void vtkSavedDataVideoSource::SetInstance(vtkSavedDataVideoSource* instance)
{
	if (vtkSavedDataVideoSource::Instance==instance)
	{
		return;
	}
	// preferably this will be NULL
	if (vtkSavedDataVideoSource::Instance)
	{
		vtkSavedDataVideoSource::Instance->Delete();;
	}
	vtkSavedDataVideoSource::Instance = instance;
	if (!instance)
	{
		return;
	}
	// user will call ->Delete() after setting instance
	instance->Register(NULL);
}
//----------------------------------------------------------------------------
void vtkSavedDataVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkSavedDataVideoSource::InternalGrab()
{
	/*LOG_TRACE("vtkSavedDataVideoSource::InternalGrab");*/
	if ( !this->Initialized )
	{
		LOG_ERROR("Called InternalGrab() when SavedDataVideoSource was not initialized!");
		return PLUS_FAIL; 	
	}

	// Compute elapsed time since we restarted the timer
	const double elapsedTime = vtkAccurateTimer::GetSystemTime() - this->GetStartTimestamp(); 

	VideoBufferItem oldestVideoBufferItem; 
	ItemStatus oldestItemStatus = this->LocalVideoBuffer->GetOldestVideoBufferItem(&oldestVideoBufferItem); 
	if ( oldestItemStatus != ITEM_OK )
	{
		LOG_ERROR("vtkSavedDataVideoSource: Unable to get oldest item from local buffer with the oldest frame UID!");
		return PLUS_FAIL; 
	}

	VideoBufferItem latestVideoBufferItem; 
	ItemStatus latestItemStatus = this->LocalVideoBuffer->GetLatestVideoBufferItem(&latestVideoBufferItem); 

	if ( latestItemStatus != ITEM_OK )
	{
		LOG_ERROR("vtkSavedDataVideoSource: Unable to get newest item from local buffer with the latest frame UID!");
		return PLUS_FAIL; 
	}

	const double oldestFrameTimestamp = oldestVideoBufferItem.GetFilteredTimestamp(this->LocalVideoBuffer->GetLocalTimeOffset()) ; 
	const double latestFrameTimestamp = latestVideoBufferItem.GetFilteredTimestamp(this->LocalVideoBuffer->GetLocalTimeOffset()); 
	
	
	// Compute the next timestamp 
	double nextFrameTimestamp = oldestFrameTimestamp + elapsedTime; 
	if ( nextFrameTimestamp > latestFrameTimestamp )
	{
		if ( this->ReplayEnabled )
		{
			// Start again from the oldest frame
			nextFrameTimestamp = oldestFrameTimestamp;
			this->SetStartTimestamp(vtkAccurateTimer::GetSystemTime()); 
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

	// use the information about data type and frmnum to do cross checking that you are maintaining correct frame index, & receiving
	// expected data type
	this->FrameNumber = nextVideoBufferItem.GetIndex(); 
	double unfilteredTimestamp(0), filteredTimestamp(0); 
	this->CreateTimeStampForFrame(this->FrameNumber, unfilteredTimestamp, filteredTimestamp);

	unsigned char *deviceDataPtr = reinterpret_cast<unsigned char *>(nextVideoBufferItem.GetFrame()->GetVoidPointer(0)); 
	PlusStatus status = this->Buffer->AddItem(deviceDataPtr, nextVideoBufferItem.GetFrame()->GetFrameSize(), nextVideoBufferItem.GetFrame()->GetBitsPerPixel(), 0, unfilteredTimestamp, filteredTimestamp, this->FrameNumber); 
	this->Modified();
	return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkSavedDataVideoSource::Initialize()
{
	LOG_TRACE("vtkSavedDataVideoSource::Initialize"); 
	if (this->Initialized)
	{
		return PLUS_SUCCESS;
	}

	// Connect to device
	if ( !this->Connect() ) 
	{
		LOG_ERROR("Unable to connect to saved data video device!"); 
		return PLUS_FAIL; 
	}

	this->Initialized = 1;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSavedDataVideoSource::Connect()
{
	LOG_TRACE("vtkSavedDataVideoSource::Connect"); 

	if ( !vtksys::SystemTools::FileExists(this->GetSequenceMetafile(), true) )
	{
		LOG_ERROR("Unable to connect to saved data video source: Unable to read sequence metafile!"); 
		return PLUS_FAIL; 
	}

	vtkSmartPointer<vtkTrackedFrameList> savedDataBuffer = vtkSmartPointer<vtkTrackedFrameList>::New(); 
		
	// Update framebuffer 
	this->UpdateFrameBuffer();

	// Read metafile
	savedDataBuffer->ReadFromSequenceMetafile(this->GetSequenceMetafile()); 
	
	// Set buffer size 
	this->SetFrameBufferSize( savedDataBuffer->GetNumberOfTrackedFrames() ); 

	// Set local buffer 
	if ( this->LocalVideoBuffer == NULL )
	{
		this->LocalVideoBuffer = vtkVideoBuffer::New(); 
	}

	this->LocalVideoBuffer->SetFrameFormat(this->Buffer->GetFrameFormat()); 
	this->LocalVideoBuffer->SetBufferSize(savedDataBuffer->GetNumberOfTrackedFrames()); 
	this->LocalVideoBuffer->UpdateBuffer(); 

	// Fill local video buffers 
	for ( unsigned int frame = 0; frame < savedDataBuffer->GetNumberOfTrackedFrames(); ++frame)
	{
		TrackedFrame* trackedFrame = savedDataBuffer->GetTrackedFrame(frame); 

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
			timestamp = frame / 10.0;  // Just to make sure its increasing. This is not a normal case.
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

		TrackedFrame::PixelType *deviceDataPtr = trackedFrame->ImageData->GetBufferPointer(); 
		const int frameSize[3] = {trackedFrame->ImageData->GetLargestPossibleRegion().GetSize()[0], trackedFrame->ImageData->GetLargestPossibleRegion().GetSize()[1], 1}; 
		const int numberOfBitsPerPixel = trackedFrame->ImageData->GetNumberOfComponentsPerPixel() * sizeof(TrackedFrame::PixelType)*8; 

		if ( this->LocalVideoBuffer->AddItem(deviceDataPtr, frameSize, numberOfBitsPerPixel, 0, unfilteredTimestamp, timestamp, frameNumber) != PLUS_SUCCESS )
		{
			LOG_WARNING("vtkSavedDataVideoSource: Failed to add video frame to buffer from sequence metafile with frame #" << frame ); 
		}
	}

	return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkSavedDataVideoSource::Disconnect()
{
	LOG_TRACE("vtkSavedDataVideoSource::Disconnect"); 
	return this->Stop();
}

//----------------------------------------------------------------------------
void vtkSavedDataVideoSource::ReleaseSystemResources()
{
	LOG_TRACE("vtkSavedDataVideoSource::ReleaseSystemResources"); 
	this->Disconnect(); 
	this->Initialized = 0;
}

//----------------------------------------------------------------------------
PlusStatus vtkSavedDataVideoSource::Grab()
{
  LOG_ERROR("Grab is not implemented for this video source");
	return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkSavedDataVideoSource::Record()
{
	LOG_TRACE("vtkSavedDataVideoSource::Record"); 

	if (!this->Initialized)
	{
		LOG_ERROR("Unable to start recording: initialize the video device first!"); 
		return PLUS_FAIL;
	}

	if (!this->Recording)
	{
		this->SetStartTimestamp(vtkAccurateTimer::GetSystemTime()); 
		this->vtkVideoSource2::Record(); 
	}

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSavedDataVideoSource::Stop()
{
	LOG_TRACE("vtkSavedDataVideoSource::Stop"); 
	if (this->Recording)
	{
		this->Recording = 0;
		this->Modified();
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

//-----------------------------------------------------------------------------
PlusStatus vtkSavedDataVideoSource::WriteConfiguration(vtkXMLDataElement* config)
{
	LOG_TRACE("vtkSavedDataVideoSource::WriteConfiguration"); 
	Superclass::WriteConfiguration(config); 
  LOG_ERROR("Not implemented");
  return PLUS_FAIL;
}

