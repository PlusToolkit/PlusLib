/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtkEpiphanVideoSource.h"
//#include "ICCapturingListener.h"
//#include <tisudshl.h>

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

#include "vtkTimerLog.h"
#include "vtkCriticalSection.h"
#include "vtkUnsignedCharArray.h"
#include "vtkMutexLock.h"

#include <vtkstd/string> 

vtkCxxRevisionMacro(vtkEpiphanVideoSource, "$Revision: 1.0$");

vtkEpiphanVideoSource* vtkEpiphanVideoSource::Instance = 0;
vtkEpiphanVideoSourceCleanup vtkEpiphanVideoSource::Cleanup;

vtkEpiphanVideoSourceCleanup::vtkEpiphanVideoSourceCleanup(){}

vtkEpiphanVideoSourceCleanup::~vtkEpiphanVideoSourceCleanup()
{
	// Destroy any remaining output window.
	vtkEpiphanVideoSource::SetInstance(NULL);
}


//----------------------------------------------------------------------------
vtkEpiphanVideoSource::vtkEpiphanVideoSource()
{
	this->ICBufferSize = 50; 

	this->DeviceName = NULL; 
	this->VideoNorm = NULL; 
	this->VideoFormat = NULL; 
	this->InputChannel = NULL; 

	this->SetFrameBufferSize(200); 
  this->Buffer->Modified();

  this->Modified();

	this->FrameGrabber = NULL;

	this->status = V2U_GRABFRAME_STATUS_OK;
	this->cropRect = new V2URect;
	this->SpawnThreadForRecording = true;
//  this->FrameGrabberListener = NULL; 

}

//----------------------------------------------------------------------------
vtkEpiphanVideoSource::~vtkEpiphanVideoSource()
{ 
	this->Disconnect();

	if ( this->fg != NULL) 
	{
		FrmGrab_Deinit();
		//delete this->FrameGrabber; 
		this->fg = NULL;
	}

 /* if ( this->FrameGrabberListener != NULL) 
	{
		delete this->FrameGrabberListener; 
		this->FrameGrabberListener = NULL;
	}*/
}


//----------------------------------------------------------------------------
// Up the reference count so it behaves like New
vtkEpiphanVideoSource* vtkEpiphanVideoSource::New()
{
	vtkEpiphanVideoSource* ret = vtkEpiphanVideoSource::GetInstance();
	ret->Register(NULL);
	return ret;
}

//----------------------------------------------------------------------------
// Return the single instance of the vtkOutputWindow
vtkEpiphanVideoSource* vtkEpiphanVideoSource::GetInstance()
{
	if(!vtkEpiphanVideoSource::Instance)
	{
		// Try the factory first
		vtkEpiphanVideoSource::Instance = (vtkEpiphanVideoSource*)vtkObjectFactory::CreateInstance("vtkEpiphanVideoSource");    
		if(!vtkEpiphanVideoSource::Instance)
		{
			vtkEpiphanVideoSource::Instance = new vtkEpiphanVideoSource();     
		}
		if(!vtkEpiphanVideoSource::Instance)
		{
			int error = 0;
		}
	}
	// return the instance
	return vtkEpiphanVideoSource::Instance;
}

//----------------------------------------------------------------------------
void vtkEpiphanVideoSource::SetInstance(vtkEpiphanVideoSource* instance)
{
	if (vtkEpiphanVideoSource::Instance==instance)
	{
		return;
	}
	// preferably this will be NULL
	if (vtkEpiphanVideoSource::Instance)
	{
		vtkEpiphanVideoSource::Instance->Delete();;
	}
	vtkEpiphanVideoSource::Instance = instance;
	if (!instance)
	{
		return;
	}
	// user will call ->Delete() after setting instance
	instance->Register(NULL);
}

//----------------------------------------------------------------------------
/////
std::string vtkEpiphanVideoSource::GetSdkVersion()
{

  std::ostringstream version; 
/**
  version << "The Imaging Source UDSHL-" << UDSHL_LIB_VERSION_MAJOR << "." << UDSHL_LIB_VERSION_MINOR; 
  */
  return version.str(); 

}

//----------------------------------------------------------------------------
void vtkEpiphanVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);

}

//----------------------------------------------------------------------------
// the callback function used when there is a new frame of data received
/////
bool vtkEpiphanVideoSource::vtkEpiphanVideoSourceNewFrameCallback(unsigned char * data, unsigned long size, unsigned long frameNumber)
{   
/**
	if(data==NULL || size==0)
	{
		LOG_ERROR("No actual frame data received from the framegrabber");
		return false;
	}

  vtkEpiphanVideoSource::GetInstance()->AddFrameToBuffer(data, size, frameNumber);    
*/
	return true;

	}

//----------------------------------------------------------------------------
// copy the Device Independent Bitmap from the VFW framebuffer into the
// vtkVideoSource framebuffer (don't do the unpacking yet)
/////
PlusStatus vtkEpiphanVideoSource::AddFrameToBuffer(unsigned char * dataPtr, unsigned long size, unsigned long frameNumber)
{
/**
	if (!this->Recording)
  {
    // drop the frame, we are not recording data now
    return PLUS_SUCCESS;
  }

	this->FrameNumber = frameNumber; 

  const int frameSize[2] = {static_cast<DShowLib::Grabber*>(FrameGrabber)->getAcqSizeMaxX(), static_cast<DShowLib::Grabber*>(FrameGrabber)->getAcqSizeMaxY()}; 
	int frameBufferBitsPerPixel = static_cast<DShowLib::Grabber*>(FrameGrabber)->getVideoFormat().getBitsPerPixel(); 
  if (frameBufferBitsPerPixel!=8)
  {
    LOG_ERROR("vtkEpiphanVideoSource::AddFrameToBuffer: only 8-bit acquisition is supported, current frameBufferBitsPerPixel="<<frameBufferBitsPerPixel);
    return PLUS_FAIL;
  }

  PlusStatus status = this->Buffer->AddItem(dataPtr, this->GetUsImageOrientation(), frameSize, itk::ImageIOBase::UCHAR, 0, this->FrameNumber); 
  this->Modified();
*/
	PlusStatus status;
	return status;

	}

//----------------------------------------------------------------------------
/////
PlusStatus vtkEpiphanVideoSource::InternalConnect()
{
  // Clear buffer on connect 
  this->GetBuffer()->Clear();

  FrmGrabNet_Init();

  //char input[15];
  //strncpy_s(input, "sn:", 15);
  //strncat_s(input, this->serialNumber, 15);

  strncpy_s(this->serialNumber,15,"V2U109999",9);

  this->fg = FrmGrab_Open(this->serialNumber);
  
  if (this->fg == NULL) {
	LOG_ERROR("Epiphan Device with set serial number not found, looking for any available device instead");
	this->fg = FrmGrabLocal_Open();
	
  }

  if (this->fg == NULL) {
	  LOG_ERROR("Epiphan Device Not found");
	  return PLUS_FAIL;;
  }

  V2U_VideoMode vm;

  if (FrmGrab_DetectVideoMode(this->fg,&vm) && vm.width && vm.height) {
	 // this->SetFrameSize(vm.width,vm.height,1);
	  //this->SetFrameRate((vm.vfreq+50)/1000);
  } else {
	LOG_ERROR("No signal detected");
  }

  FrmGrab_SetMaxFps(this->fg, 25.0);

  // Initialization worked
  //this->Initialized = 1;
  
  // Update frame buffer  to reflect any changes
  //this->UpdateFrameBuffer();

  this->GetBuffer()->SetPixelType(itk::ImageIOBase::UCHAR ); 
  this->GetBuffer()->SetFrameSize(vm.width,vm.height);

  return PLUS_SUCCESS;

  }  

//----------------------------------------------------------------------------
/////
/**
void vtkEpiphanVideoSource::ReleaseSystemResources()
{

  this->Initialized = 0;
  if (this->fg != NULL) {
	FrmGrab_Close(this->fg);
  }

	return PLUS_SUCCESS; 

	}
*/
//----------------------------------------------------------------------------
/////
PlusStatus vtkEpiphanVideoSource::InternalDisconnect()
{

  LOG_DEBUG("Disconnect from Epiphan ");

  //this->Initialized = 0;
  if (this->fg != NULL) {
	FrmGrab_Close(this->fg);
  }
  //delete fg;
  this->fg = NULL;

  return this->StopRecording();

  return PLUS_SUCCESS;

  }

//----------------------------------------------------------------------------
/////
PlusStatus vtkEpiphanVideoSource::InternalStartRecording()
{

   //if (this->Playing)
   // {
   // this->Stop();
   // }

  if (!this->Recording)
    {
		return PLUS_SUCCESS;
	}

  return this->Connect();
    //this->Initialize();

    //this->Recording = 1;
    //this->FrameCount = 0;
	//this->pauseFeed = 0;
    //this->Modified();
    //this->PlayerThreadId = 
    //  this->PlayerThreader->SpawnThread((vtkThreadFunctionType)\
    //                            &vtkEpiphanVideoSourceRecordThread,this);
    //}

}


/**
static void *vtkEpiphanVideoSourceRecordThread(vtkMultiThreader::ThreadInfo *data)
{

	vtkEpiphanVideoSource *self = (vtkEpiphanVideoSource *)(data->UserData);
  
  double startTime = vtkTimerLog::GetUniversalTime();
  double rate = self->GetFrameRate();
  int frame = 0;

  do
    {
    self->InternalGrab();
    frame++;
    }
  while (vtkThreadSleep(data, startTime + frame/rate));

  return NULL;

  }
*/

/////

PlusStatus vtkEpiphanVideoSource::InternalGrab()
{

  if (!this->Recording)
  {
    // drop the frame, we are not recording data now
    return PLUS_SUCCESS;
  }

  V2U_GrabFrame2 * frame = NULL;
  V2U_UINT32 format = V2U_GRABFRAME_BOTTOM_UP_FLAG; // seems to be needed to orientate correctly.

  format |= V2U_GRABFRAME_FORMAT_RGB24;
  /*
  if (this->OutputFormat == VTK_LUMINANCE) {
	format |= V2U_GRABFRAME_FORMAT_YUY2;
  } else if (this->OutputFormat == VTK_RGB) {

	format |= V2U_GRABFRAME_FORMAT_RGB24;
  } else if (this->OutputFormat == VTK_RGBA) {
	format |= V2U_GRABFRAME_FORMAT_ARGB32;
  } else {
	  // no clue what format to grab, you can add more.
	  return;
  }
*/
  /*
  this->cropRect.x = 0;
  this->cropRect.y = 0;
  this->cropRect.width = 400;
  this->cropRect.width = 400;
*/
  frame= FrmGrab_Frame(this->fg, format, NULL);

  this->FrameNumber++;

  int FrameSize[2] = {frame->imagelen, frame->imagelen};
  
  PlusStatus status = this->Buffer->AddItem(frame->pixbuf ,this->GetUsImageOrientation(), FrameSize, 
	  itk::ImageIOBase::UCHAR,0,this->FrameNumber); 
  this->Modified();
  return status;
/*
  // If the buffer is empty, set the pixel type and frame size to the first received properties 
  if ( this->GetBuffer()->GetNumberOfItems() == 0 )
  {
    this->GetBuffer()->SetPixelType(pixelType);  
    this->GetBuffer()->SetFrameSize( imgSize[0], imgSize[1] );
  }
*/
}


//----------------------------------------------------------------------------
/////
/**
PlusStatus vtkEpiphanVideoSource::InternalStopRecording()
{

  if (!static_cast<DShowLib::Grabber*>(FrameGrabber)->stopLive())
  {
    LOG_ERROR("Framegrabber stopLive failed");
    return PLUS_FAIL;
  }



}
  */
//-----------------------------------------------------------------------------

/////
PlusStatus vtkEpiphanVideoSource::ReadConfiguration(vtkXMLDataElement* config)
{

	LOG_TRACE("vtkEpiphanVideoSource::ReadConfiguration"); 
	if ( config == NULL )
	{
		LOG_ERROR("Unable to configure IC Capturing video source! (XML data element is NULL)"); 
		return PLUS_FAIL; 
	}

	Superclass::ReadConfiguration(config); 

	vtkXMLDataElement* dataCollectionConfig = config->FindNestedElementWithName("DataCollection");
	if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
		return PLUS_FAIL;
	}

  vtkXMLDataElement* imageAcquisitionConfig = dataCollectionConfig->FindNestedElementWithName("ImageAcquisition"); 
  if (imageAcquisitionConfig == NULL) 
  {
    LOG_ERROR("Unable to find ImageAcquisition element in configuration XML structure!");
    return PLUS_FAIL;
  }

	const char* deviceName = imageAcquisitionConfig->GetAttribute("DeviceName"); 
	if ( deviceName != NULL) 
	{
		this->SetDeviceName(deviceName); 
	}

	const char* videoNorm = imageAcquisitionConfig->GetAttribute("VideoNorm"); 
	if ( videoNorm != NULL) 
	{
		this->SetVideoNorm(videoNorm); 
	}

	const char* videoFormat = imageAcquisitionConfig->GetAttribute("VideoFormat"); 
	if ( videoFormat != NULL) 
	{
		this->SetVideoFormat(videoFormat); 
	}

	const char* inputChannel = imageAcquisitionConfig->GetAttribute("InputChannel"); 
	if ( inputChannel != NULL) 
	{
		this->SetInputChannel(inputChannel); 
	}

	int icBufferSize = 0; 
	if ( imageAcquisitionConfig->GetScalarAttribute("ICBufferSize", icBufferSize) ) 
	{
		this->SetICBufferSize(icBufferSize); 
	}

  return PLUS_SUCCESS;

}

//-----------------------------------------------------------------------------

/////
PlusStatus vtkEpiphanVideoSource::WriteConfiguration(vtkXMLDataElement* config)
{
/**
  // Write superclass configuration
  Superclass::WriteConfiguration(config); 

  if ( config == NULL )
  {
    LOG_ERROR("Config is invalid");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* dataCollectionConfig = config->FindNestedElementWithName("DataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* imageAcquisitionConfig = dataCollectionConfig->FindNestedElementWithName("ImageAcquisition"); 
  if (imageAcquisitionConfig == NULL) 
  {
    LOG_ERROR("Cannot find ImageAcquisition element in XML tree!");
    return PLUS_FAIL;
  }

  imageAcquisitionConfig->SetAttribute("DeviceName", this->DeviceName);
  imageAcquisitionConfig->SetAttribute("VideoNorm", this->VideoNorm);
  imageAcquisitionConfig->SetAttribute("VideoFormat", this->VideoFormat);
  imageAcquisitionConfig->SetAttribute("InputChannel", this->InputChannel);
  imageAcquisitionConfig->SetIntAttribute("ICBufferSize", this->ICBufferSize);
*/
  return PLUS_SUCCESS;

}


