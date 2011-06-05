#include "vtkICCapturingSource2.h"
#include "ICCapturingListener.h"
#include "PlusConfigure.h"
#include <tisudshl.h>

#include "vtkImageData.h"
#include "vtkCriticalSection.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtksys/SystemTools.hxx"
#include "vtkVideoBuffer2.h"
#include "vtkVideoFrame2.h"
#include "vtkMultiThreader.h"

#include <ctype.h>

#include <vector>
#include <string>


vtkCxxRevisionMacro(vtkICCapturingSource2, "$Revision: 1.0$");

vtkICCapturingSource2* vtkICCapturingSource2::Instance = 0;
vtkICCapturingSourceCleanup2 vtkICCapturingSource2::Cleanup;

vtkICCapturingSourceCleanup2::vtkICCapturingSourceCleanup2(){}

vtkICCapturingSourceCleanup2::~vtkICCapturingSourceCleanup2()
{
	// Destroy any remaining output window.
	vtkICCapturingSource2::SetInstance(NULL);
}


//----------------------------------------------------------------------------
vtkICCapturingSource2::vtkICCapturingSource2()
{
	this->Initialized = 0;

	this->SetFrameSize(640, 480, 1);

	this->ICBufferSize = 50; 

	this->LicenceKey = NULL; 
	this->DeviceName = NULL; 
	this->VideoNorm = NULL; 
	this->VideoFormat = NULL; 
	this->InputChannel = NULL; 

	this->FrameBufferRowAlignment = 1;

	this->SetOutputFormatToLuminance(); 

	this->Buffer->GetFrameFormat()->SetFrameGrabberType(FG_BASE); 
	for (int i = 0; i < this->Buffer->GetBufferSize(); i++)
	{
		this->Buffer->GetFrameByBufferIndex(i)->SetFrameGrabberType(FG_BASE);
	}

	this->SetFrameBufferSize(200); 
	this->Buffer->Modified();

	this->Modified();
	this->UpdateFrameBuffer();

	this->FrameGrabber = NULL;

}

//----------------------------------------------------------------------------
vtkICCapturingSource2::~vtkICCapturingSource2()
{ 

	this->Disconnect();

	if ( this->FrameGrabber != NULL) 
	{
		delete this->FrameGrabber; 
		this->FrameGrabber = NULL;
	}
}

//----------------------------------------------------------------------------
// Up the reference count so it behaves like New
vtkICCapturingSource2* vtkICCapturingSource2::New()
{
	vtkICCapturingSource2* ret = vtkICCapturingSource2::GetInstance();
	ret->Register(NULL);
	return ret;
}

//----------------------------------------------------------------------------
// Return the single instance of the vtkOutputWindow
vtkICCapturingSource2* vtkICCapturingSource2::GetInstance()
{
	if(!vtkICCapturingSource2::Instance)
	{
		// Try the factory first
		vtkICCapturingSource2::Instance = (vtkICCapturingSource2*)vtkObjectFactory::CreateInstance("vtkICCapturingSource2");    
		if(!vtkICCapturingSource2::Instance)
		{
			vtkICCapturingSource2::Instance = new vtkICCapturingSource2();     
		}
		if(!vtkICCapturingSource2::Instance)
		{
			int error = 0;
		}
	}
	// return the instance
	return vtkICCapturingSource2::Instance;
}

//----------------------------------------------------------------------------
void vtkICCapturingSource2::SetInstance(vtkICCapturingSource2* instance)
{
	if (vtkICCapturingSource2::Instance==instance)
	{
		return;
	}
	// preferably this will be NULL
	if (vtkICCapturingSource2::Instance)
	{
		vtkICCapturingSource2::Instance->Delete();;
	}
	vtkICCapturingSource2::Instance = instance;
	if (!instance)
	{
		return;
	}
	// user will call ->Delete() after setting instance
	instance->Register(NULL);
}

//----------------------------------------------------------------------------
void vtkICCapturingSource2::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);

}

//----------------------------------------------------------------------------
// the callback function used when there is a new frame of data received
bool vtkICCapturingSource2::vtkICCapturingSource2NewFrameCallback(unsigned char * data, unsigned long size, unsigned long frameNumber)
{    
	if(!data || !size)
	{
		//printf("Error: no actual frame data received\n");
		return false;
	}

	if(data)
	{
		vtkICCapturingSource2::GetInstance()->LocalInternalGrab(data, size, frameNumber);    
	}
	return true;
}

//----------------------------------------------------------------------------
// copy the Device Independent Bitmap from the VFW framebuffer into the
// vtkVideoSource framebuffer (don't do the unpacking yet)
void vtkICCapturingSource2::LocalInternalGrab(unsigned char * dataPtr, unsigned long size, unsigned long frameNumber)
{
	this->Buffer->Lock();

	this->FrameNumber = frameNumber; 
	double unfilteredTimestamp(0), filteredTimestamp(0); 
	this->CreateTimeStampForFrame(this->FrameNumber, unfilteredTimestamp, filteredTimestamp);

	// 3) read the data, based on the data type and clip region information, which is reflected in frame buffer extents
	// this is necessary as there would be cases when there is a clip region defined i.e. only data from the desired extents should be copied 
	// to the local buffer, which demands necessary advancement of dataPtr

	// get the pointer to the correct location in the frame buffer, where this data needs to be copied
	unsigned char *frameBufferPtr = reinterpret_cast<unsigned char *>(this->Buffer->GetFrameToWrite()->GetVoidPointer(0));

	int FrameBufferExtent[6];
	this->Buffer->GetFrameFormat()->GetFrameExtent(FrameBufferExtent);

	int FrameBufferBitsPerPixel = this->Buffer->GetFrameFormat()->GetBitsPerPixel();

	int outBytesPerRow = ((FrameBufferExtent[1] - FrameBufferExtent[0]+1)* FrameBufferBitsPerPixel + 7)/8;
	outBytesPerRow += outBytesPerRow % this->FrameBufferRowAlignment;

	int* frameSize = this->GetFrameSize(); 
	int inBytesPerRow = frameSize[0] * FrameBufferBitsPerPixel/8;

	int rows = FrameBufferExtent[3] - FrameBufferExtent[2]+1;

	//check if the data received has the same size in bytes as expected
	if (size != inBytesPerRow*rows)
	{
		//error; data discrepancy!
		//what to do?
	}

	dataPtr += FrameBufferExtent[0]* FrameBufferBitsPerPixel/8;
	dataPtr += FrameBufferExtent[2]*inBytesPerRow;

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

	// add the new frame and the current time to the buffer
	this->Buffer->AddItem(this->Buffer->GetFrameToWrite(), unfilteredTimestamp, filteredTimestamp, this->FrameNumber);

	if (this->FrameCount++ == 0)
	{
		double timestamp(0); 
		if ( this->Buffer->GetLatestTimeStamp(timestamp) == vtkVideoBuffer2::FRAME_OK )
		{
			this->StartTimeStamp = timestamp;
		}
	}

	this->Modified();

	this->Buffer->Unlock();
}

//----------------------------------------------------------------------------
int vtkICCapturingSource2::Connect()
{
	if ( this->FrameGrabber == NULL ) 
	{
		this->FrameGrabber = new DShowLib::Grabber; 
	}

	if( !DShowLib::InitLibrary( this->GetLicenceKey() ) )
	{
		vtkErrorMacro( << "The IC capturing library could not be initialized - invalid license key: " << this->GetLicenceKey() ); 
		exit(EXIT_FAILURE);
	}

	atexit( DShowLib::ExitLibrary );

	// Set the device name (e.g. DFG/USB2-lt)
	if ( this->GetDeviceName() == NULL || !static_cast<DShowLib::Grabber*>(FrameGrabber)->openDev(this->GetDeviceName() ) ) 
	{
		vtkErrorMacro( << "The IC capturing library could not be initialized - invalid device name: " << this->GetDeviceName() ); 
		exit(EXIT_FAILURE);
	}

	// Set the video norm (e.g. PAL_B or NTSC_M)
	if ( this->GetVideoNorm() == NULL || !static_cast<DShowLib::Grabber*>(FrameGrabber)->setVideoNorm( this->GetVideoNorm() ) ) 
	{
		vtkErrorMacro( << "The IC capturing library could not be initialized - invalid video norm: " << this->GetVideoNorm() ); 
		exit(EXIT_FAILURE);
	}

	// The Y800 color format is an 8 bit monochrome format. 
	if ( this->GetVideoFormat() == NULL || !static_cast<DShowLib::Grabber*>(FrameGrabber)->setVideoFormat( this->GetVideoFormat() ) )
	{
		vtkErrorMacro( << "The IC capturing library could not be initialized - invalid video format: " << this->GetVideoFormat() ); 
		exit(EXIT_FAILURE);
	}

	if ( this->GetInputChannel() == NULL || !static_cast<DShowLib::Grabber*>(FrameGrabber)->setInputChannel( this->GetInputChannel() ) ) 
	{
		vtkErrorMacro( << "The IC capturing library could not be initialized - invalid input channel: " << this->GetInputChannel() ); 
		exit(EXIT_FAILURE);
	}

	FrameGrabberListener = new ICCapturingListener(); 

	// Assign the number of buffers to the cListener object.
	FrameGrabberListener->setBufferSize( this->GetICBufferSize() );

	// Register the FrameGrabberListener object for the frame ready 
	static_cast<DShowLib::Grabber*>(FrameGrabber)->addListener( FrameGrabberListener, DShowLib::GrabberListener::eFRAMEREADY );

	// Create a FrameTypeInfoArray data structure describing the allowed color formats.
	DShowLib::FrameTypeInfoArray acceptedTypes = DShowLib::FrameTypeInfoArray::createRGBArray();

	// Create the frame handler sink: 8 bit monochrome format. 
	smart_ptr<DShowLib::FrameHandlerSink> pSink = DShowLib::FrameHandlerSink::create( DShowLib::eRGB8, 1);
	//smart_ptr<DShowLib::FrameHandlerSink> pSink = DShowLib::FrameHandlerSink::create( DShowLib::eY800, 1);

	// Disable snap mode.
	pSink->setSnapMode( false );

	// Apply the sink to the grabber.
	static_cast<DShowLib::Grabber*>(FrameGrabber)->setSinkType( pSink );

	static_cast<DShowLib::Grabber*>(FrameGrabber)->startLive(false);				// Start the grabber.

	// 7) set callback for receiving new frames
	this->FrameGrabberListener->SetICCapturingSourceNewFrameCallback(vtkICCapturingSource2::vtkICCapturingSource2NewFrameCallback); 

	// 8)update framebuffer 
	this->Buffer->Lock();
	this->UpdateFrameBuffer();
	this->Buffer->Unlock();

	return 1; 
}

//----------------------------------------------------------------------------
void vtkICCapturingSource2::Disconnect()
{
	this->ReleaseSystemResources(); 
}

//----------------------------------------------------------------------------
void vtkICCapturingSource2::Initialize()
{
	if (this->Initialized)
	{
		return;
	}

	// Connect to device
	if (! this->Connect() )
	{
		LOG_ERROR("Unable to connect video device!"); 
		return; 
	}
	
	this->Initialized = 1;
}

//----------------------------------------------------------------------------
void vtkICCapturingSource2::ReleaseSystemResources()
{
	if (this->Recording)
	{
		this->Stop();
	}

	DShowLib::ExitLibrary(); 

	this->Initialized = 0;
}

//----------------------------------------------------------------------------
void vtkICCapturingSource2::Grab()
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
void vtkICCapturingSource2::Record()
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
void vtkICCapturingSource2::Stop()
{
	if (this->Recording)
	{
		this->Recording = 0;
		this->Modified();
	}
}

//-----------------------------------------------------------------------------
void vtkICCapturingSource2::ReadConfiguration(vtkXMLDataElement* config)
{
	LOG_TRACE("vtkICCapturingSource2::ReadConfiguration"); 
	if ( config == NULL )
	{
		LOG_ERROR("Unable to configure IC Capturing video source! (XML data element is NULL)"); 
		return; 
	}

	Superclass::ReadConfiguration(config); 

	const char* deviceName = config->GetAttribute("DeviceName"); 
	if ( deviceName != NULL) 
	{
		this->SetDeviceName(deviceName); 
	}

	const char* videoNorm = config->GetAttribute("VideoNorm"); 
	if ( videoNorm != NULL) 
	{
		this->SetVideoNorm(videoNorm); 
	}

	const char* videoFormat = config->GetAttribute("VideoFormat"); 
	if ( videoFormat != NULL) 
	{
		this->SetVideoFormat(videoFormat); 
	}

	const char* inputChannel = config->GetAttribute("InputChannel"); 
	if ( inputChannel != NULL) 
	{
		this->SetInputChannel(inputChannel); 
	}

	const char* licenseKey = config->GetAttribute("LicenseKey"); 
	if ( licenseKey != NULL) 
	{
		this->SetLicenceKey(licenseKey); 
	}

	int icBufferSize = 0; 
	if ( config->GetScalarAttribute("ICBufferSize", icBufferSize) ) 
	{
		this->SetICBufferSize(icBufferSize); 
	}
}

//-----------------------------------------------------------------------------
void vtkICCapturingSource2::WriteConfiguration(vtkXMLDataElement* config)
{
	Superclass::WriteConfiguration(config); 
}

