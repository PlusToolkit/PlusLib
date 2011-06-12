#include "PlusConfigure.h"
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
#include "vtkVideoBuffer.h"
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
PlusStatus vtkBufferedVideoSource::Initialize()
{
	if (this->Initialized)
	{
		return PLUS_SUCCESS;
	}

	// update framebuffer 
	this->UpdateFrameBuffer();

	this->Initialized = 1;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkBufferedVideoSource::ReleaseSystemResources()
{
	if (this->Recording)
	{
		this->Stop();
	}

	this->Initialized = 0;
}

//----------------------------------------------------------------------------
PlusStatus vtkBufferedVideoSource::Grab()
{
  LOG_ERROR("Grab is not implemented for this video source");
	return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkBufferedVideoSource::Record()
{
	this->Initialize();
	if (!this->Initialized)
	{
		return PLUS_FAIL;
	}

	if (!this->Recording)
	{
		this->Recording = 1;
		this->Modified();
	}
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkBufferedVideoSource::Stop()
{
	if (this->Recording)
	{
		this->Recording = 0;
		this->Modified();
	}

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkBufferedVideoSource::AddFrame( vtkImageData* image, double timestamp )
{
	// We don't have information about the unfiltered timestamp, use the filtered one
	const double unfilteredTimestamp = timestamp; 
	const long frameNumber = this->FrameNumber + 1; 
	PlusStatus status = this->Buffer->AddItem(image, unfilteredTimestamp, timestamp, frameNumber); 

	if ( status == PLUS_SUCCESS )
	{
		this->FrameNumber = frameNumber; 
	}

	this->Modified();
	return status; 
}
