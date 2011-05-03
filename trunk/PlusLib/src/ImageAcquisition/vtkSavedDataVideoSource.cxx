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

	// 1) Do the frame buffer indices maintenance
	if (this->AutoAdvance)
	{
		this->AdvanceFrameBuffer(1);
		if (this->FrameIndex + 1 < this->Buffer->GetBufferSize())
		{
			this->FrameIndex++;
		}
	}

	const double elapsedTime = vtkAccurateTimer::GetSystemTime() - this->GetStartTimestamp(); 
	const double localStartTime = this->LocalVideoBuffer->GetTimeStamp(LocalVideoBuffer->GetNumberOfItems() - 1); 

	// Get buffer index 
	const int bufferIndex = this->LocalVideoBuffer->GetIndexFromTime(localStartTime + elapsedTime); 

	// Get frame number 
	int frameNumber = this->LocalVideoBuffer->GetFrameNumber(bufferIndex); 

	// use the information about data type and frmnum to do cross checking that you are maintaining correct frame index, & receiving
	// expected data type
	this->FrameNumber = frameNumber; 
	double unfilteredTimestamp(0), filteredTimestamp(0); 
	this->CreateTimeStampForFrame(this->FrameNumber, unfilteredTimestamp, filteredTimestamp);

	unsigned char *deviceDataPtr = reinterpret_cast<unsigned char *>(this->LocalVideoBuffer->GetFrame(bufferIndex)->GetVoidPointer(0)); 
	// get the pointer to the correct location in the frame buffer, where this data needs to be copied
	unsigned char *frameBufferPtr = reinterpret_cast<unsigned char *>(this->Buffer->GetFrame(0)->GetVoidPointer(0));

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
	this->Buffer->AddItem(this->Buffer->GetFrame(0), unfilteredTimestamp, filteredTimestamp, this->FrameNumber);

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
		LOG_ERROR("Unable to connect to saved data video source: Unable to read sequence metafile: " << this->GetSequenceMetafile()); 
		return 0; 
	}

	vtkSmartPointer<vtkTrackedFrameList> savedDataBuffer = vtkSmartPointer<vtkTrackedFrameList>::New(); 
		
	// Update framebuffer 
	this->Buffer->Lock();
	this->UpdateFrameBuffer();
	this->Buffer->Unlock();

	// Read metafile
	savedDataBuffer->ReadFromSequenceMetafile(this->GetSequenceMetafile()); 
	
	if ( this->LocalVideoBuffer == NULL )
	{
		this->LocalVideoBuffer = vtkVideoBuffer2::New(); 
	}

	this->LocalVideoBuffer->SetFrameFormat(this->Buffer->GetFrameFormat()); 
	this->LocalVideoBuffer->SetBufferSize(savedDataBuffer->GetNumberOfTrackedFrames()); 

	// Allocate local video buffer frames
	for ( int i = 0; i < this->LocalVideoBuffer->GetBufferSize(); i++ )
	{
		this->LocalVideoBuffer->GetFrame(i)->Allocate(); 
	}

	// Fill local video buffers 
	for ( unsigned int frame = 0; frame < savedDataBuffer->GetNumberOfTrackedFrames(); ++frame)
	{
		TrackedFrame* trackedFrame = savedDataBuffer->GetTrackedFrame(frame); 

		// Get frame number
		const char* strFrameNumber = trackedFrame->GetCustomFrameField("FrameNumber"); 
		if ( strFrameNumber == NULL ) 
		{
			LOG_WARNING("Unable to get frame number for frame #" << frame ); 
			continue; 
		}
		long frameNumber = atol(strFrameNumber); 

		// Get Timestamp
		const char* strTimestamp = trackedFrame->GetCustomFrameField("Timestamp"); 
		if ( strTimestamp == NULL ) 
		{
			LOG_WARNING("Unable to get timestamp for frame #" << frame ); 
			continue; 
		}
		double timestamp = atof(strTimestamp); 

		// Get UnfilteredTimestamp
		const char* strUnfilteredTimestamp = trackedFrame->GetCustomFrameField("UnfilteredTimestamp"); 
		if ( strUnfilteredTimestamp == NULL ) 
		{
			LOG_WARNING("Unable to get unfiltered timestamp for frame #" << frame ); 
			continue; 
		}
		double unfilteredTimestamp = atof(strUnfilteredTimestamp); 

		this->LocalVideoBuffer->Seek(1);
		TrackedFrame::PixelType *deviceDataPtr = trackedFrame->ImageData->GetBufferPointer(); 
		unsigned char *frameBufferPtr = reinterpret_cast<unsigned char *>(this->LocalVideoBuffer->GetFrame(0)->GetVoidPointer(0));
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
		this->LocalVideoBuffer->AddItem(this->LocalVideoBuffer->GetFrame(0), timestamp, timestamp, frameNumber );
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

	if (this->Playing)
	{
		this->Stop();
	}

	if (!this->Recording)
	{
		this->SetStartTimestamp(vtkAccurateTimer::GetSystemTime()); 
		this->vtkVideoSource2::Record(); 
	}
}

//----------------------------------------------------------------------------
void vtkSavedDataVideoSource::Play()
{
	LOG_TRACE("vtkSavedDataVideoSource::Play"); 
	this->vtkVideoSource2::Play();
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
	else if (this->Playing)
	{
		this->vtkVideoSource2::Stop();
	}
}

