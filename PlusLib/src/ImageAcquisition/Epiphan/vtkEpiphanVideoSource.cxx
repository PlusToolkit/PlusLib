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
vtkStandardNewMacro(vtkEpiphanVideoSource);
//vtkEpiphanVideoSource* vtkEpiphanVideoSource::Instance = 0;
//vtkEpiphanVideoSourceCleanup vtkEpiphanVideoSource::Cleanup;

//vtkEpiphanVideoSourceCleanup::vtkEpiphanVideoSourceCleanup(){}

/*
vtkEpiphanVideoSourceCleanup::~vtkEpiphanVideoSourceCleanup()
{
	// Destroy any remaining output window.
	vtkEpiphanVideoSource::SetInstance(NULL);
}
*/

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
	if( !this->Connected )
	{
		this->Disconnect();
	}

	if ( this->fg != NULL) 
	{
		FrmGrab_Deinit();
		//delete this->FrameGrabber; 
		this->fg = NULL;
	}
}


//----------------------------------------------------------------------------
// Up the reference count so it behaves like New
/*
vtkEpiphanVideoSource* vtkEpiphanVideoSource::New()
{
	vtkEpiphanVideoSource* ret = vtkEpiphanVideoSource::GetInstance();
	ret->Register(NULL);
	return ret;
}
*/

//----------------------------------------------------------------------------
// Return the single instance of the vtkOutputWindow
/*
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
*/
//----------------------------------------------------------------------------
/*
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
*/
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
PlusStatus vtkEpiphanVideoSource::InternalConnect()
{

  LOG_TRACE( "vtkEpiphanVideoSource::InternalConnect" );

  // Clear buffer on connect 
  this->GetBuffer()->Clear();

  // Initialize frmgrab library
  FrmGrabNet_Init();

  strncpy_s(this->serialNumber,15,"V2U109999",9);

  if ( (this->fg = FrmGrab_Open( this->serialNumber )) == NULL ) 
  {
    LOG_ERROR("Epiphan Device with set serial number not found, looking for any available device instead");
	if ( (this->fg = FrmGrabLocal_Open()) == NULL )
	{
	  LOG_ERROR("Epiphan Device Not found");
	  return PLUS_FAIL;
	}
  }
  
  V2U_VideoMode vm;

  if (FrmGrab_DetectVideoMode(this->fg,&vm) && vm.width && vm.height) {
	 //this->SetFrameSize(vm.width,vm.height);
	 this->SetFrameRate((vm.vfreq+50)/1000);
  } else {
	LOG_ERROR("No signal detected");
  }

  this->GetBuffer()->SetPixelType(itk::ImageIOBase::UCHAR);

  this->cropRect->x = 0;
  this->cropRect->y = 0;
  this->cropRect->width = 400;
  this->cropRect->height = 400;

  this->FrameSize[0] = cropRect->width;
  this->FrameSize[1] = cropRect->height;

  //FrmGrab_SetMaxFps(this->fg, 25.0);

  // Initialization worked
  //this->Initialized = 1;
  
  // Update frame buffer  to reflect any changes
  //this->UpdateFrameBuffer();

  return PLUS_SUCCESS;

  }  

//----------------------------------------------------------------------------
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


PlusStatus vtkEpiphanVideoSource::InternalGrab()
{

  if (!this->Recording)
  {
    // drop the frame, we are not recording data now
    return PLUS_SUCCESS;
  }

  V2U_GrabFrame2 * frame = NULL;
  //V2U_UINT32 format = V2U_GRABFRAME_BOTTOM_UP_FLAG; // seems to be needed to orientate correctly.

  V2U_UINT32 format = V2U_GRABFRAME_FORMAT_RGB8;
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
  


  frame= FrmGrab_Frame(this->fg, format, cropRect);

  if (frame == NULL)
  {
	  return PLUS_SUCCESS;
  }

  this->FrameNumber++;

  // Set unfiltered and filtered timestamp
  double unfilteredTimestamp = vtkTimerLog::GetUniversalTime();//v2u_computeTime();  
  double filteredTimestamp = vtkTimerLog::GetUniversalTime();//v2u_computeTime();  

  // If the buffer is empty, set the pixel type and frame size to the first received properties 
  if ( this->GetBuffer()->GetNumberOfItems() == 0 )
  {
    this->GetBuffer()->SetPixelType(itk::ImageIOBase::UCHAR);  
    this->GetBuffer()->SetFrameSize( FrameSize );
  }

  PlusStatus status = this->Buffer->AddItem(frame->pixbuf ,this->GetUsImageOrientation(), FrameSize, 
	  itk::ImageIOBase::UCHAR,0,this->FrameNumber);//,unfilteredTimestamp, filteredTimestamp); 
  this->Modified();
  FrmGrab_Release(this->fg, frame);
  return status;
}

V2U_TIME vtkEpiphanVideoSource::v2u_computeTime()
{
    V2U_TIME now;
    SYSTEMTIME sysTime;
    FILETIME   fileTime;
    GetSystemTime(&sysTime);
    SystemTimeToFileTime(&sysTime, &fileTime);
    now = ((V2U_TIME)fileTime.dwHighDateTime << 32) 
         + (V2U_TIME)fileTime.dwLowDateTime;
    now /= 10000;  /* convert from 100-ns ticks to milliseconds */
    now -= 11644473600000i64;   /* Jan 1st 1601 to Jan 1st 1970 */
    return now;
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


