#include "PlusConfigure.h"
#include "vtkBufferedVideoSource.h"

#include "vtkImageData.h"
#include "vtkCriticalSection.h"
#include "vtkObjectFactory.h"
#include "vtkTimerLog.h"
#include "vtkVideoBuffer.h"
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
PlusStatus vtkBufferedVideoSource::AddFrameToBuffer( ImageType::Pointer image, US_IMAGE_ORIENTATION usImageOrientation, double timestamp )
{
	// We don't have information about the unfiltered timestamp, use the filtered one
	const long frameNumber = this->FrameNumber + 1; 
  
  int frameSize[2] = {image->GetLargestPossibleRegion().GetSize()[0], image->GetLargestPossibleRegion().GetSize()[1]}; 
  int pixelSizeInBits = sizeof(PixelType)*8;

	PlusStatus status = this->Buffer->AddTimeStampedItem(image->GetBufferPointer(), usImageOrientation, frameSize, pixelSizeInBits, 0, timestamp, frameNumber); 

	if ( status == PLUS_SUCCESS )
	{
		this->FrameNumber = frameNumber; 
	}
  else
  {
    LOG_ERROR("Failed to add frame to buffer (timestamp: " << std::fixed << timestamp << ")!"); 
  }

  this->Modified();
  return status; 
}
