#include "vtkBufferedVideoSource.h"

#include "vtkImageData.h"
#include "vtkCriticalSection.h"
#include "vtkObjectFactory.h"
#include "vtkTimerLog.h"
//#include "vtkInformation.h"
//#include "vtkInformationVector.h"
//#include "vtkStreamingDemandDrivenPipeline.h"
//#include "vtkUnsignedCharArray.h"
//#include "vtksys/SystemTools.hxx"
#include "vtkVideoBuffer2.h"
#include "vtkVideoFrame2.h"
#include "vtkMultiThreader.h"

#include <ctype.h>

// because of warnings in windows header push and pop the warning level
#ifdef _MSC_VER
#pragma warning (push, 3)
#endif

#include <vector>
#include <string>

#ifdef _MSC_VER
#pragma warning (pop)
#endif



vtkCxxRevisionMacro(vtkBufferedVideoSource, "$Revision: 1.0$");
vtkInstantiatorNewMacro(vtkBufferedVideoSource);

//----------------------------------------------------------------------------

vtkBufferedVideoSource* vtkBufferedVideoSource::Instance = 0;
vtkBufferedVideoSourceCleanup vtkBufferedVideoSource::Cleanup;


vtkBufferedVideoSourceCleanup::vtkBufferedVideoSourceCleanup()
{
}

vtkBufferedVideoSourceCleanup::~vtkBufferedVideoSourceCleanup()
{
	// Destroy any remaining output window.
	vtkBufferedVideoSource::SetInstance(NULL);
}

//----------------------------------------------------------------------------
vtkBufferedVideoSource::vtkBufferedVideoSource()
{
}

//----------------------------------------------------------------------------
vtkBufferedVideoSource::~vtkBufferedVideoSource()
{ 
	this->vtkBufferedVideoSource::ReleaseSystemResources();
}


//----------------------------------------------------------------------------
// Up the reference count so it behaves like New
vtkBufferedVideoSource* vtkBufferedVideoSource::New()
{
	vtkBufferedVideoSource* ret = vtkBufferedVideoSource::GetInstance();
	ret->Register(NULL);
	return ret;
}


//----------------------------------------------------------------------------
// Return the single instance of the vtkOutputWindow
vtkBufferedVideoSource* vtkBufferedVideoSource::GetInstance()
{
	if(!vtkBufferedVideoSource::Instance)
	{
		// Try the factory first
		vtkBufferedVideoSource::Instance = (vtkBufferedVideoSource*)vtkObjectFactory::CreateInstance("vtkBufferedVideoSource");    
		if(!vtkBufferedVideoSource::Instance)
		{
			vtkBufferedVideoSource::Instance = new vtkBufferedVideoSource();     
		}
		if(!vtkBufferedVideoSource::Instance)
		{
			int error = 0;
		}
	}
	// return the instance
	return vtkBufferedVideoSource::Instance;
}


//----------------------------------------------------------------------------
void vtkBufferedVideoSource::SetInstance(vtkBufferedVideoSource* instance)
{
	if (vtkBufferedVideoSource::Instance==instance)
	{
		return;
	}
	// preferably this will be NULL
	if (vtkBufferedVideoSource::Instance)
	{
		vtkBufferedVideoSource::Instance->Delete();;
	}
	vtkBufferedVideoSource::Instance = instance;
	if (!instance)
	{
		return;
	}
	// user will call ->Delete() after setting instance
	instance->Register(NULL);
}


//----------------------------------------------------------------------------
void vtkBufferedVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkBufferedVideoSource::Initialize()
{
	if (this->Initialized)
	{
		return;
	}

	// update framebuffer 
	this->UpdateFrameBuffer();

	this->Initialized = 1;
}

//----------------------------------------------------------------------------
void vtkBufferedVideoSource::ReleaseSystemResources()
{
	if (this->Playing || this->Recording)
	{
		this->Stop();
	}

	this->Initialized = 0;
}

//----------------------------------------------------------------------------
void vtkBufferedVideoSource::Grab()
{
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
void vtkBufferedVideoSource::Record()
{
	this->Initialize();
	if (!this->Initialized)
	{
		return;
	}

	if (this->Playing)
	{
		this->Stop();
	}

	if (!this->Recording)
	{
		this->Recording = 1;
		this->Modified();
	}
}

//----------------------------------------------------------------------------
void vtkBufferedVideoSource::Play()
{
	this->vtkVideoSource2::Play();
}

//----------------------------------------------------------------------------
void vtkBufferedVideoSource::Stop()
{
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

//----------------------------------------------------------------------------
void vtkBufferedVideoSource::AddFrame( vtkImageData* image, double timestamp )
{

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

	// get the pointer to actual incoming data on to a local pointer
	unsigned char *deviceDataPtr = static_cast<unsigned char*>( image->GetScalarPointer() ); 

	// get the pointer to the correct location in the frame buffer, where this data needs to be copied
	//unsigned char *frameBufferPtr = (unsigned char *)((reinterpret_cast<vtkUnsignedCharArray*>(this->FrameBuffer[index]))->GetVoidPointer(0));
	unsigned char *frameBufferPtr = reinterpret_cast<unsigned char *>(this->Buffer->GetFrame(0)->GetVoidPointer(0));

	int FrameBufferExtent[6];
	this->Buffer->GetFrameFormat()->GetFrameExtent(FrameBufferExtent);

	int FrameBufferBitsPerPixel = this->Buffer->GetFrameFormat()->GetBitsPerPixel();

	int outBytesPerRow = ((FrameBufferExtent[1] - FrameBufferExtent[0]+1)* FrameBufferBitsPerPixel + 7)/8;

	int* frameSize = this->GetFrameSize(); 
	int inBytesPerRow = frameSize[0] * FrameBufferBitsPerPixel/8;

	int rows = FrameBufferExtent[3] - FrameBufferExtent[2]+1;

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
	this->Buffer->AddItem(this->Buffer->GetFrame(0), timestamp, timestamp, ++this->FrameNumber );

	if (this->FrameCount++ == 0)
	{
		this->StartTimeStamp = this->Buffer->GetTimeStamp(0);
	}

	this->Modified();

	this->Buffer->Unlock();
}
