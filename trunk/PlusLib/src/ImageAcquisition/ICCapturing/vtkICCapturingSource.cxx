#include "PlusConfigure.h"

#include "vtkICCapturingSource.h"
#include "vtkImageData.h"
#include "vtkCriticalSection.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtksys/SystemTools.hxx"

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



vtkCxxRevisionMacro(vtkICCapturingSource, "$Revision: 1.0$");
//vtkStandardNewMacro(vtkWin32VideoSource);
//----------------------------------------------------------------------------
// Needed when we don't use the vtkStandardNewMacro.
vtkInstantiatorNewMacro(vtkICCapturingSource);

//----------------------------------------------------------------------------

vtkICCapturingSource* vtkICCapturingSource::Instance = 0;
vtkICCapturingSourceCleanup vtkICCapturingSource::Cleanup;

#if ( _MSC_VER >= 1300 ) // Visual studio .NET
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4312 )
#  define vtkGetWindowLong GetWindowLongPtr
#  define vtkSetWindowLong SetWindowLongPtr
#  define vtkGWL_USERDATA GWLP_USERDATA
#else // regular Visual studio 
#  define vtkGetWindowLong GetWindowLong
#  define vtkSetWindowLong SetWindowLong
#  define vtkGWL_USERDATA GWL_USERDATA
#endif // 

vtkICCapturingSourceCleanup::vtkICCapturingSourceCleanup()
{
}

vtkICCapturingSourceCleanup::~vtkICCapturingSourceCleanup()
{
  // Destroy any remaining output window.
  vtkICCapturingSource::SetInstance(NULL);
}

//----------------------------------------------------------------------------
vtkICCapturingSource::vtkICCapturingSource()
{
	this->Initialized = 0;

	this->FrameSize[0] = 640;
	this->FrameSize[1] = 480;
	this->FrameSize[2] = 1;

	this->OutputFormat = VTK_LUMINANCE;
	this->NumberOfScalarComponents = 1;
	this->FrameBufferBitsPerPixel = 8;
	this->FlipFrames = 0;
	this->FrameBufferRowAlignment = 1;  

	this->ICBufferSize = 50; 

	this->LicenceKey = NULL; 
	this->DeviceName = NULL; 
	this->VideoNorm = NULL; 
	this->VideoFormat = NULL; 
	this->InputChannel = NULL; 

}

//----------------------------------------------------------------------------
vtkICCapturingSource::~vtkICCapturingSource()
{ 
	this->vtkICCapturingSource::ReleaseSystemResources();
}

//----------------------------------------------------------------------------
// Up the reference count so it behaves like New
vtkICCapturingSource* vtkICCapturingSource::New()
{
	vtkICCapturingSource* ret = vtkICCapturingSource::GetInstance();
	ret->Register(NULL);
	return ret;
}

//----------------------------------------------------------------------------
// Return the single instance of the vtkOutputWindow
vtkICCapturingSource* vtkICCapturingSource::GetInstance()
{
	if(!vtkICCapturingSource::Instance)
	{
		// Try the factory first
		vtkICCapturingSource::Instance = (vtkICCapturingSource*)vtkObjectFactory::CreateInstance("vtkICCapturingSource");    
		if(!vtkICCapturingSource::Instance)
		{
			vtkICCapturingSource::Instance = new vtkICCapturingSource();	   
		}
		if(!vtkICCapturingSource::Instance)
		{
			int error = 0;
		}
	}
	// return the instance
	return vtkICCapturingSource::Instance;
}

//----------------------------------------------------------------------------
void vtkICCapturingSource::SetInstance(vtkICCapturingSource* instance)
{
	if (vtkICCapturingSource::Instance==instance)
	{
		return;
	}
	// preferably this will be NULL
	if (vtkICCapturingSource::Instance)
	{
		vtkICCapturingSource::Instance->Delete();;
	}
	vtkICCapturingSource::Instance = instance;
	if (!instance)
	{
		return;
	}
	// user will call ->Delete() after setting instance
	instance->Register(NULL);
}

//----------------------------------------------------------------------------
void vtkICCapturingSource::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
// the callback function used when there is a new frame of data received
bool vtkICCapturingSource::vtkICCapturingSourceNewFrameCallback(unsigned char * data, unsigned long size, unsigned long frameNumber)
{    
	if(!data || !size)
	{
		//printf("Error: no actual frame data received\n");
		return false;
	}

	if(data)
	{
		vtkICCapturingSource::GetInstance()->LocalInternalGrab(data, size);    
	}
	return true;
}

//----------------------------------------------------------------------------
// copy the Device Independent Bitmap from the VFW framebuffer into the
// vtkVideoSource framebuffer (don't do the unpacking yet)
PlusStatus vtkICCapturingSource::LocalInternalGrab(unsigned char * dataPtr, unsigned long size)
{

	// get a thread lock on the frame buffer
	this->FrameBufferMutex->Lock();

	// 1) Do the frame buffer indices maintenance
	if (this->AutoAdvance)
	{
		this->AdvanceFrameBuffer(1);
		if (this->FrameIndex + 1 < this->FrameBufferSize)
		{
			this->FrameIndex++;
		}
	}
	int index = this->FrameBufferIndex;


	// 2) Do the time stamping
	this->FrameBufferTimeStamps[index] = vtkAccurateTimer::GetSystemTime();

	if (this->FrameCount++ == 0)
	{
		this->StartTimeStamp = this->FrameBufferTimeStamps[index];
	}

	// get the pointer to the correct location in the frame buffer, where this data needs to be copied
  unsigned char *frameBufferPtr = static_cast<unsigned char*>(this->FrameBuffer[index]);

	int outBytesPerRow = ((this->FrameBufferExtent[1]- this->FrameBufferExtent[0]+1)* this->FrameBufferBitsPerPixel + 7)/8;
	outBytesPerRow += outBytesPerRow % this->FrameBufferRowAlignment;

	int inBytesPerRow = this->FrameSize[0] * this->FrameBufferBitsPerPixel/8;

	int rows = this->FrameBufferExtent[3]-this->FrameBufferExtent[2]+1;

	//check if the data received has the same size in bytes as expected
	if (size != inBytesPerRow*rows)
	{
		//error; data discrepancy!
		//what to do?
	}

	dataPtr += this->FrameBufferExtent[0]* this->FrameBufferBitsPerPixel/8;
	dataPtr += this->FrameBufferExtent[2]*inBytesPerRow;

	// 4) copy data to the local vtk frame buffer
	if (outBytesPerRow == inBytesPerRow)
	{
		memcpy(frameBufferPtr,dataPtr,inBytesPerRow*rows);
	}
	else
	{
		while (--rows >= 0)
		{
			memcpy(frameBufferPtr,dataPtr,outBytesPerRow);
			frameBufferPtr += outBytesPerRow;
			dataPtr += inBytesPerRow;
		}
	}

	this->Modified();

	this->FrameBufferMutex->Unlock();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkICCapturingSource::Initialize()
{
	if (this->Initialized)
	{
		return PLUS_SUCCESS;
	}

	if( !DShowLib::InitLibrary( this->GetLicenceKey() ) )
	{
		LOG_ERROR("The IC capturing library could not be initialized - invalid license key: " << this->GetLicenceKey() ); 
		return PLUS_FAIL;
	}

	//atexit( DShowLib::ExitLibrary );

	// Set the device name (e.g. DFG/USB2-lt)
	if ( this->GetDeviceName() == NULL || !FrameGrabber.openDev(this->GetDeviceName() ) ) 
	{
		LOG_ERROR("The IC capturing library could not be initialized - invalid device name: " << this->GetDeviceName() ); 
		return PLUS_FAIL;
	}

	// Set the video norm (e.g. PAL_B or NTSC_M)
	if ( this->GetVideoNorm() == NULL || !FrameGrabber.setVideoNorm( this->GetVideoNorm() ) ) 
	{
		LOG_ERROR("The IC capturing library could not be initialized - invalid video norm: " << this->GetVideoNorm() ); 
		return PLUS_FAIL;
	}

	// The Y800 color format is an 8 bit monochrome format. 
	if ( this->GetVideoFormat() == NULL || !FrameGrabber.setVideoFormat( this->GetVideoFormat() ) )
	{
		LOG_ERROR("The IC capturing library could not be initialized - invalid video format: " << this->GetVideoFormat() ); 
		return PLUS_FAIL;
	}
	
	if ( this->GetInputChannel() == NULL || !FrameGrabber.setInputChannel( this->GetInputChannel() ) ) 
	{
		LOG_ERROR("The IC capturing library could not be initialized - invalid input channel: " << this->GetInputChannel() ); 
		return PLUS_FAIL;
	}

	FrameGrabberListener = new ICCapturingListener(); 

	// Assign the number of buffers to the cListener object.
	FrameGrabberListener->setBufferSize( this->GetICBufferSize() );

	// Register the FrameGrabberListener object for the frame ready 
	FrameGrabber.addListener( FrameGrabberListener, DShowLib::GrabberListener::eFRAMEREADY );

	// Create the frame handler sink: 8 bit monochrome format. 
	smart_ptr<DShowLib::FrameHandlerSink> pSink = DShowLib::FrameHandlerSink::create( DShowLib::eRGB8, 1);

	// Disable snap mode: In grab mode, frames are copied continuously to the MemBufferCollection, as soon as Grabber::startLive is called.
	pSink->setSnapMode( false );

	// Apply the sink to the grabber.
	FrameGrabber.setSinkType( pSink );

	FrameGrabber.startLive(false);				// Start the grabber.

	// 7) set callback for receiving new frames
	this->FrameGrabberListener->SetICCapturingSourceNewFrameCallback(vtkICCapturingSourceNewFrameCallback); 

	// 8)update framebuffer 
	this->UpdateFrameBuffer();

	this->Initialized = 1;
  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkICCapturingSource::ReleaseSystemResources()
{
	DShowLib::ExitLibrary(); 
	//delete FrameGrabberListener; 
}

//----------------------------------------------------------------------------
PlusStatus vtkICCapturingSource::Grab()
{
  LOG_ERROR("Grab is not implemented for this video source");
	return PLUS_FAIL;

	// just do the grab, the callback does the rest
	//this->SetStartTimeStamp(vtkAccurateTimer::GetSystemTime());
	//  capGrabFrameNoStop(this->Internal->CapWnd);
}

//----------------------------------------------------------------------------
void vtkICCapturingSource::Record()
{
	this->Initialize();
	if (!this->Initialized)
	{
		return;
	}

	if (!this->Recording)
	{
		this->Recording = 1;
		this->Modified();
	}
}

//----------------------------------------------------------------------------
void vtkICCapturingSource::Stop()
{
	if (this->Recording)
	{
		this->Recording = 0;
		this->Modified();
	}

}


//----------------------------------------------------------------------------
int vtkICCapturingSource::RequestInformation(
	vtkInformation * vtkNotUsed(request),
	vtkInformationVector **vtkNotUsed(inputVector),
	vtkInformationVector *outputVector)
{
	// get the info objects
	vtkInformation* outInfo = outputVector->GetInformationObject(0);

	int i;
	int extent[6];

	// ensure that the hardware is initialized.
	this->Initialize();

	for (i = 0; i < 3; i++)
	{
		// initially set extent to the OutputWholeExtent
		extent[2*i] = this->OutputWholeExtent[2*i];
		extent[2*i+1] = this->OutputWholeExtent[2*i+1];
		// if 'flag' is set in output extent, use the FrameBufferExtent instead
		if (extent[2*i+1] < extent[2*i])
		{
			extent[2*i] = 0; 
			extent[2*i+1] = \
				this->FrameBufferExtent[2*i+1] - this->FrameBufferExtent[2*i];
		}
		this->FrameOutputExtent[2*i] = extent[2*i];
		this->FrameOutputExtent[2*i+1] = extent[2*i+1];
	}

	int numFrames = this->NumberOfOutputFrames;
	if (numFrames < 1)
	{
		numFrames = 1;
	}
	if (numFrames > this->FrameBufferSize)
	{
		numFrames = this->FrameBufferSize;
	}

	// multiply Z extent by number of frames to output
	extent[5] = extent[4] + (extent[5]-extent[4]+1) * numFrames - 1;

	outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),extent,6);

	outInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),extent,6);
	// set the spacing
	outInfo->Set(vtkDataObject::SPACING(),this->DataSpacing,3);

	// set the origin.
	outInfo->Set(vtkDataObject::ORIGIN(),this->DataOrigin,3);

	// set default data type (8 bit greyscale)  
	vtkDataObject::SetPointDataActiveScalarInfo(outInfo, VTK_UNSIGNED_CHAR, 
		this->NumberOfScalarComponents);

	return 1;
}


//----------------------------------------------------------------------------
void vtkICCapturingSource::SetOutputFormat(int format)
{
	if (format == this->OutputFormat)
	{
		return;
	}

	this->OutputFormat = format;

	// convert color format to number of scalar components
	int numComponents;

	switch (this->OutputFormat)
	{
	case VTK_RGBA:
		numComponents = 4;
		break;
	case VTK_RGB:
		numComponents = 3;
		break;
	case VTK_LUMINANCE:
		numComponents = 1;
		break;
	default:
		numComponents = 0;
		LOG_ERROR("SetOutputFormat: Unrecognized color format.");
		break;
	}
	this->NumberOfScalarComponents = numComponents;

	if (this->FrameBufferBitsPerPixel != numComponents*8)
	{
		this->FrameBufferMutex->Lock();
		this->FrameBufferBitsPerPixel = numComponents*8;
		if (this->Initialized)
		{
			this->UpdateFrameBuffer();    
			//this->DoFormatSetup();
		}
		this->FrameBufferMutex->Unlock();
	}

	this->Modified();
}

