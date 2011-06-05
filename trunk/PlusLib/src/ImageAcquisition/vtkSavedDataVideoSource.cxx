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
void vtkSavedDataVideoSource::InternalGrab()
{
	/*LOG_TRACE("vtkSavedDataVideoSource::InternalGrab");*/
	if ( !this->Initialized )
	{
		LOG_WARNING("Called InternalGrab() when SavedDataVideoSource was not initialized!");
		return; 	
	}
	// get a thread lock on the frame buffer
	this->Buffer->Lock();

	// Compute elapsed time since we restarted the timer
	const double elapsedTime = vtkAccurateTimer::GetSystemTime() - this->GetStartTimestamp(); 

	// Get the oldest timestamp from the buffer 
	double oldestFrameTimestamp(0); 
	vtkVideoBuffer2::FrameStatus oldestFrameStatus = this->LocalVideoBuffer->GetTimeStamp( this->LocalVideoBuffer->GetOldestFrameUidInBuffer(), oldestFrameTimestamp); 
	if ( oldestFrameStatus != vtkVideoBuffer2::FRAME_OK )
	{
		LOG_WARNING("vtkSavedDataVideoSource: Unable to get oldest frame timestamp from local buffer with the oldest frame UID!");
		this->Buffer->Unlock();
		return; 
	}

	// Get the latest timestamp from the buffer 
	double latestFrameTimestamp(0); 
	vtkVideoBuffer2::FrameStatus latestFrameStatus = this->LocalVideoBuffer->GetTimeStamp( this->LocalVideoBuffer->GetLatestFrameUidInBuffer(), latestFrameTimestamp); 
	if ( latestFrameStatus != vtkVideoBuffer2::FRAME_OK )
	{
		LOG_WARNING("vtkSavedDataVideoSource: Unable to get newest frame timestamp from local buffer with the latest frame UID!");
		this->Buffer->Unlock();
		return; 
	}

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

	// Get buffer index 
	vtkVideoBuffer2::FrameUidType frameUID(0); 
	vtkVideoBuffer2::FrameStatus frameStatus = this->LocalVideoBuffer->GetFrameUidFromTime(nextFrameTimestamp, frameUID); 
	if ( frameStatus == vtkVideoBuffer2::FRAME_NOT_AVAILABLE_YET )
	{
		LOG_WARNING("vtkSavedDataVideoSource: Unable to get frame UID from time - frame not available yet!");
		this->Buffer->Unlock();
		return; 
	}
	else if ( frameStatus == vtkVideoBuffer2::FRAME_NOT_AVAILABLE_ANYMORE )
	{
		LOG_WARNING("vtkSavedDataVideoSource: Unable to get frame UID from time - frame not available anymore!");
		this->Buffer->Unlock();
		return; 
	}

	// Get frame number 
	unsigned long frameNumber(0); 
	if ( this->LocalVideoBuffer->GetFrameNumber(frameUID, frameNumber) != vtkVideoBuffer2::FRAME_OK )
	{
		LOG_WARNING( "vtkSavedDataVideoSource: Unable to get frame number from local video buffer with UID: " << frameUID ); 
		this->Buffer->Unlock();
		return; 
	}

	// use the information about data type and frmnum to do cross checking that you are maintaining correct frame index, & receiving
	// expected data type
	this->FrameNumber = frameNumber; 
	double unfilteredTimestamp(0), filteredTimestamp(0); 
	this->CreateTimeStampForFrame(this->FrameNumber, unfilteredTimestamp, filteredTimestamp);

	vtkVideoFrame2* localFrame = NULL; 
	vtkVideoBuffer2::FrameStatus localFrameStatus = this->LocalVideoBuffer->GetFrame(frameUID, localFrame); 

	if ( localFrameStatus != vtkVideoBuffer2::FRAME_OK )
	{
		LOG_WARNING( "vtkSavedDataVideoSource: Unable to get frame from local video buffer with UID: " << frameUID ); 
		this->Buffer->Unlock();
		return; 
	}

	if ( localFrame == NULL )
	{
		LOG_WARNING( "vtkSavedDataVideoSource: Failed to grab new frame from local video buffer with UID: " << frameUID  << " - Frame pointer was NULL"); 
		this->Buffer->Unlock();
		return; 
	}

	// get the pointer to the correct location in the frame buffer, where this data needs to be copied
	vtkVideoFrame2* newFrameInBuffer = this->Buffer->GetFrameToWrite(); 
	if ( newFrameInBuffer == NULL )
	{
		LOG_WARNING( "vtkSavedDataVideoSource: Failed to get video frame pointer from the buffer for the new frame!"); 
		this->Buffer->Unlock();
		return; 
	}

	unsigned char *deviceDataPtr = reinterpret_cast<unsigned char *>(localFrame->GetVoidPointer(0)); 
	unsigned char *frameBufferPtr = reinterpret_cast<unsigned char *>(newFrameInBuffer->GetVoidPointer(0));

	int FrameBufferExtent[6];
	this->Buffer->GetFrameFormat()->GetFrameExtent(FrameBufferExtent);

	int FrameBufferBitsPerPixel = this->Buffer->GetFrameFormat()->GetBitsPerPixel();

	int outBytesPerRow = ((FrameBufferExtent[1] - FrameBufferExtent[0]+1)* FrameBufferBitsPerPixel + 7)/8;
	outBytesPerRow += outBytesPerRow % this->FrameBufferRowAlignment;

	int* frameSize = this->GetFrameSize(); 
	int inBytesPerRow = frameSize[0] * FrameBufferBitsPerPixel/8;

	int rows = FrameBufferExtent[3] - FrameBufferExtent[2]+1;

	
	deviceDataPtr += FrameBufferExtent[0]* FrameBufferBitsPerPixel/8;
	deviceDataPtr += FrameBufferExtent[2]*inBytesPerRow;

	// 4) copy data to the local vtk frame buffer
	if (outBytesPerRow == inBytesPerRow)
	{
		memcpy(frameBufferPtr,deviceDataPtr,inBytesPerRow*rows);
	}
	else
	{
		while (--rows >= 0)
		{
			memcpy(frameBufferPtr,deviceDataPtr,outBytesPerRow);
			frameBufferPtr += outBytesPerRow;
			deviceDataPtr += inBytesPerRow;
		}
	}

	// add the new frame and the current time to the buffer
	this->Buffer->AddItem(newFrameInBuffer, unfilteredTimestamp, filteredTimestamp, this->FrameNumber);

	this->Modified();

	this->Buffer->Unlock();
}

//----------------------------------------------------------------------------
void vtkSavedDataVideoSource::Initialize()
{
	LOG_TRACE("vtkSavedDataVideoSource::Initialize"); 
	if (this->Initialized)
	{
		return;
	}

	// Connect to device
	if ( !this->Connect() ) 
	{
		LOG_ERROR("Unable to connect to saved data video device!"); 
		return; 
	}

	this->Initialized = 1;
}

//----------------------------------------------------------------------------
int vtkSavedDataVideoSource::Connect()
{
	LOG_TRACE("vtkSavedDataVideoSource::Connect"); 

	if ( !vtksys::SystemTools::FileExists(this->GetSequenceMetafile(), true) )
	{
		LOG_ERROR("Unable to connect to saved data video source: Unable to read sequence metafile!"); 
		return 0; 
	}

	vtkSmartPointer<vtkTrackedFrameList> savedDataBuffer = vtkSmartPointer<vtkTrackedFrameList>::New(); 
		
	// Update framebuffer 
	this->Buffer->Lock();
	this->UpdateFrameBuffer();
	this->Buffer->Unlock();

	// Read metafile
	savedDataBuffer->ReadFromSequenceMetafile(this->GetSequenceMetafile()); 
	
	// Set buffer size 
	this->SetFrameBufferSize( savedDataBuffer->GetNumberOfTrackedFrames() ); 

	// Set local buffer 
	if ( this->LocalVideoBuffer == NULL )
	{
		this->LocalVideoBuffer = vtkVideoBuffer2::New(); 
	}

	this->LocalVideoBuffer->SetFrameFormat(this->Buffer->GetFrameFormat()); 
	this->LocalVideoBuffer->SetBufferSize(savedDataBuffer->GetNumberOfTrackedFrames()); 

	// Allocate local video buffer frames
	for ( int i = 0; i < this->LocalVideoBuffer->GetBufferSize(); i++ )
	{
		this->LocalVideoBuffer->GetFrameByBufferIndex(i)->Allocate(); 
	}

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
		
		// get the pointer to the correct location in the frame buffer, where this data needs to be copied
		vtkVideoFrame2* newFrameInBuffer = this->LocalVideoBuffer->GetFrameToWrite(); 
		if ( newFrameInBuffer == NULL )
		{
			LOG_WARNING( "vtkSavedDataVideoSource: Failed to get video frame pointer from the local buffer for the new frame (frame number: " << frame << ")!"); 
			continue; 
		}

		unsigned char *frameBufferPtr = reinterpret_cast<unsigned char *>(newFrameInBuffer->GetVoidPointer(0));
		int frameBufferExtent[6];
		this->LocalVideoBuffer->GetFrameFormat()->GetFrameExtent(frameBufferExtent);

		int frameBufferBitsPerPixel = this->LocalVideoBuffer->GetFrameFormat()->GetBitsPerPixel();

		int outBytesPerRow = ((frameBufferExtent[1] - frameBufferExtent[0]+1)* frameBufferBitsPerPixel + 7)/8;

		int* frameSize = this->LocalVideoBuffer->GetFrameFormat()->GetFrameSize(); 
		int inBytesPerRow = frameSize[0] * frameBufferBitsPerPixel/8;
		int rows = frameBufferExtent[3] - frameBufferExtent[2]+1;

		// 4) copy data to the local vtk frame buffer
		// Note: there is an itkData to vtkData conversion here (vertical flip)
		while (--rows >= 0)
		{
			TrackedFrame::PixelType *dataPtr = deviceDataPtr + rows*inBytesPerRow; 
			memcpy(frameBufferPtr,dataPtr,outBytesPerRow);
			frameBufferPtr += outBytesPerRow;
		}
		
		// add the new frame and the current time to the buffer
		this->LocalVideoBuffer->AddItem(newFrameInBuffer, timestamp, unfilteredTimestamp, frameNumber );
	}

	return 1; 
}

//----------------------------------------------------------------------------
void vtkSavedDataVideoSource::Disconnect()
{
	LOG_TRACE("vtkSavedDataVideoSource::Disconnect"); 
	this->Stop();
}

//----------------------------------------------------------------------------
void vtkSavedDataVideoSource::ReleaseSystemResources()
{
	LOG_TRACE("vtkSavedDataVideoSource::ReleaseSystemResources"); 
	this->Disconnect(); 
	this->Initialized = 0;
}

//----------------------------------------------------------------------------
void vtkSavedDataVideoSource::Grab()
{
	LOG_TRACE("vtkSavedDataVideoSource::Grab"); 
	if (this->Recording)
	{
		return;
	}

	// ensure that the frame buffer is properly initialized
	this->Initialize();
	if (!this->Initialized)
	{
		return;
	}
}

//----------------------------------------------------------------------------
void vtkSavedDataVideoSource::Record()
{
	LOG_TRACE("vtkSavedDataVideoSource::Record"); 

	if (!this->Initialized)
	{
		LOG_ERROR("Unable to start recording: initialize the video device first!"); 
		return;
	}

	if (!this->Recording)
	{
		this->SetStartTimestamp(vtkAccurateTimer::GetSystemTime()); 
		this->vtkVideoSource2::Record(); 
	}
}

//----------------------------------------------------------------------------
void vtkSavedDataVideoSource::Stop()
{
	LOG_TRACE("vtkSavedDataVideoSource::Stop"); 
	if (this->Recording)
	{
		this->Recording = 0;
		this->Modified();
	}
}

//-----------------------------------------------------------------------------
void vtkSavedDataVideoSource::ReadConfiguration(vtkXMLDataElement* config)
{
	LOG_TRACE("vtkSavedDataVideoSource::ReadConfiguration"); 
	if ( config == NULL )
	{
		LOG_ERROR("Unable to configure Saved Data video source! (XML data element is NULL)"); 
		return; 
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
	
}

//-----------------------------------------------------------------------------
void vtkSavedDataVideoSource::WriteConfiguration(vtkXMLDataElement* config)
{
	LOG_TRACE("vtkSavedDataVideoSource::WriteConfiguration"); 
	Superclass::WriteConfiguration(config); 
}

