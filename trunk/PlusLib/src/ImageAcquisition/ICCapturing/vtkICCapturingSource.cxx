#include "PlusConfigure.h"

#include "vtkICCapturingSource.h"
#include "ICCapturingListener.h"
#include <tisudshl.h>

#include "vtkImageData.h"
#include "vtkCriticalSection.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtksys/SystemTools.hxx"
#include "vtkVideoBuffer.h"
#include "vtkMultiThreader.h"

#include <ctype.h>

#include <vector>
#include <string>


vtkCxxRevisionMacro(vtkICCapturingSource, "$Revision: 1.0$");

vtkICCapturingSource* vtkICCapturingSource::Instance = 0;
vtkICCapturingSourceCleanup vtkICCapturingSource::Cleanup;

vtkICCapturingSourceCleanup::vtkICCapturingSourceCleanup(){}

vtkICCapturingSourceCleanup::~vtkICCapturingSourceCleanup()
{
	// Destroy any remaining output window.
	vtkICCapturingSource::SetInstance(NULL);
}


//----------------------------------------------------------------------------
vtkICCapturingSource::vtkICCapturingSource()
{
	this->Initialized = 0;

	this->ICBufferSize = 50; 

	this->LicenceKey = NULL; 
	this->DeviceName = NULL; 
	this->VideoNorm = NULL; 
	this->VideoFormat = NULL; 
	this->InputChannel = NULL; 

	this->SetFrameBufferSize(200); 
	this->Buffer->Modified();

	this->Modified();
	this->UpdateFrameBuffer();

	this->FrameGrabber = NULL;

}

//----------------------------------------------------------------------------
vtkICCapturingSource::~vtkICCapturingSource()
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
		vtkICCapturingSource::GetInstance()->LocalInternalGrab(data, size, frameNumber);    
	}
	return true;
}

//----------------------------------------------------------------------------
// copy the Device Independent Bitmap from the VFW framebuffer into the
// vtkVideoSource framebuffer (don't do the unpacking yet)
PlusStatus vtkICCapturingSource::LocalInternalGrab(unsigned char * dataPtr, unsigned long size, unsigned long frameNumber)
{
	this->FrameNumber = frameNumber; 

  const int frameSize[2] = {static_cast<DShowLib::Grabber*>(FrameGrabber)->getAcqSizeMaxX(), static_cast<DShowLib::Grabber*>(FrameGrabber)->getAcqSizeMaxY()}; 
	int frameBufferBitsPerPixel = static_cast<DShowLib::Grabber*>(FrameGrabber)->getVideoFormat().getBitsPerPixel(); 
  PlusStatus status = this->Buffer->AddItem(dataPtr, this->GetUsImageOrientation(), frameSize, frameBufferBitsPerPixel, 0, this->FrameNumber); 

  this->Modified();

	return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkICCapturingSource::Connect()
{
	if ( this->FrameGrabber == NULL ) 
	{
		this->FrameGrabber = new DShowLib::Grabber; 
	}

	if( !DShowLib::InitLibrary( this->GetLicenceKey() ) )
	{
		LOG_ERROR("The IC capturing library could not be initialized - invalid license key: " << this->GetLicenceKey() );
		return PLUS_FAIL;
	}

	atexit( DShowLib::ExitLibrary );

	// Set the device name (e.g. DFG/USB2-lt)
	if ( this->GetDeviceName() == NULL || !static_cast<DShowLib::Grabber*>(FrameGrabber)->openDev(this->GetDeviceName() ) ) 
	{
		LOG_ERROR("The IC capturing library could not be initialized - invalid device name: " << this->GetDeviceName() ); 
    return PLUS_FAIL;
  }

	// Set the video norm (e.g. PAL_B or NTSC_M)
	if ( this->GetVideoNorm() == NULL || !static_cast<DShowLib::Grabber*>(FrameGrabber)->setVideoNorm( this->GetVideoNorm() ) ) 
	{
		LOG_ERROR("The IC capturing library could not be initialized - invalid video norm: " << this->GetVideoNorm() ); 
		return PLUS_FAIL;
	}

	// The Y800 color format is an 8 bit monochrome format. 
	if ( this->GetVideoFormat() == NULL || !static_cast<DShowLib::Grabber*>(FrameGrabber)->setVideoFormat( this->GetVideoFormat() ) )
	{
		LOG_ERROR("The IC capturing library could not be initialized - invalid video format: " << this->GetVideoFormat() ); 
		return PLUS_FAIL;
	}

    this->GetBuffer()->SetNumberOfBitsPerPixel(static_cast<DShowLib::Grabber*>(FrameGrabber)->getVideoFormat().getBitsPerPixel() );  

    this->GetBuffer()->SetFrameSize( static_cast<DShowLib::Grabber*>(FrameGrabber)->getAcqSizeMaxX(), static_cast<DShowLib::Grabber*>(FrameGrabber)->getAcqSizeMaxY() ); 

	if ( this->GetInputChannel() == NULL || !static_cast<DShowLib::Grabber*>(FrameGrabber)->setInputChannel( this->GetInputChannel() ) ) 
	{
		LOG_ERROR("The IC capturing library could not be initialized - invalid input channel: " << this->GetInputChannel() ); 
		return PLUS_FAIL;
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
	this->FrameGrabberListener->SetICCapturingSourceNewFrameCallback(vtkICCapturingSource::vtkICCapturingSourceNewFrameCallback); 

	// 8)update framebuffer 
	this->UpdateFrameBuffer();

	return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkICCapturingSource::Disconnect()
{
	this->ReleaseSystemResources(); 
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkICCapturingSource::Initialize()
{
	if (this->Initialized)
	{
		return PLUS_SUCCESS;
	}

	// Connect to device
	if (this->Connect()!=PLUS_SUCCESS)
	{
		LOG_ERROR("Unable to connect video device!"); 
		return PLUS_FAIL;
	}
	
	this->Initialized = 1;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkICCapturingSource::ReleaseSystemResources()
{
	if (this->Recording)
	{
		this->StopRecording();
	}

	DShowLib::ExitLibrary(); 

	this->Initialized = 0;
}

//----------------------------------------------------------------------------
PlusStatus vtkICCapturingSource::Grab()
{
  LOG_ERROR("Grab is not implemented for this video source");
	return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkICCapturingSource::StartRecording()
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
PlusStatus vtkICCapturingSource::StopRecording()
{
	if (this->Recording)
	{
		this->Recording = 0;
		this->Modified();
	}
  
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkICCapturingSource::ReadConfiguration(vtkXMLDataElement* config)
{
	LOG_TRACE("vtkICCapturingSource::ReadConfiguration"); 
	if ( config == NULL )
	{
		LOG_ERROR("Unable to configure IC Capturing video source! (XML data element is NULL)"); 
		return PLUS_FAIL; 
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

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkICCapturingSource::WriteConfiguration(vtkXMLDataElement* config)
{
	Superclass::WriteConfiguration(config); 
  LOG_ERROR("Not implemented");
  return PLUS_FAIL;
}
