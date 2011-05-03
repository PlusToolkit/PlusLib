/*=========================================================================

Module:    $RCSfile: vtkSonixVideoSource2.cxx,v $
Author:  Siddharth Vikal, Queens School Of Computing

Copyright (c) 2008, Queen's University, Kingston, Ontario, Canada
All rights reserved.

Author: Danielle Pace
Robarts Research Institute and The University of Western Ontario

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in
the documentation and/or other materials provided with the
distribution.

* Neither the name of Queen's University nor the names of any
contributors may be used to endorse or promote products derived
from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

#include "vtkSonixVideoSource2.h"

#include "vtkImageData.h"
#include "vtkCriticalSection.h"
#include "vtkObjectFactory.h"
#include "vtkTimerLog.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkUnsignedCharArray.h"
#include "vtksys/SystemTools.hxx"
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
#include "ulterius.h"
#include "ulterius_def.h"
#if ULTERIUS_MAJOR_VERSION == 1
#include "utx_imaging_modes.h"
#endif

#ifdef _MSC_VER
#pragma warning (pop)
#endif



vtkCxxRevisionMacro(vtkSonixVideoSource2, "$Revision: 1.0$");
//vtkStandardNewMacro(vtkWin32VideoSource);
//----------------------------------------------------------------------------
// Needed when we don't use the vtkStandardNewMacro.
vtkInstantiatorNewMacro(vtkSonixVideoSource2);

//----------------------------------------------------------------------------

vtkSonixVideoSource2* vtkSonixVideoSource2::Instance = 0;
vtkSonixVideoSourceCleanup2 vtkSonixVideoSource2::Cleanup;


//sonic param indices
#define VARID_FREQ 414
#define VARID_DEPTH 206
#define VARID_GAIN  15
#define VARID_CGAIN 313
#define VARID_PGAIN 274
#define VARID_TGC 327
#define VARID_ZOOM 1176
#define VARID_CPRF 303
#define VARID_PPRF 275
#define VARID_SECTOR 1116
#define VARID_BCHROMA 1087
#define VARID_MCHROMA 1179
#define VARID_DYNRANGE 361
#define VARID_CFOCUS 157
#define VARID_CFOCUSCOLOR 904
#define VARID_SFOCUS 1255
#define VARID_DFOCUS 1254
#define VARID_FRATE 584
#define VARID_MSWEEP 101
#define VARID_CLARITY 1112
#define VARID_CMAP 1082
#define VARID_BMAP 601

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

//----------------------------------------------------------------------------
vtkSonixVideoSourceCleanup2::vtkSonixVideoSourceCleanup2()
{
}

//----------------------------------------------------------------------------
vtkSonixVideoSourceCleanup2::~vtkSonixVideoSourceCleanup2()
{
	// Destroy any remaining output window.
	vtkSonixVideoSource2::SetInstance(NULL);
}
//----------------------------------------------------------------------------
vtkSonixVideoSource2::vtkSonixVideoSource2()
{
	this->ult = new ulterius();
	this->DataDescriptor = new uDataDesc();

	this->SetFrameSize(640, 480, 1);

	this->SonixHostIP = "130.15.7.212";
	this->FrameRate = -1; // in fps
	this->FrameCount = 0;
	this->FrameIndex = -1;

	this->Frequency = -1; //in Mhz
	this->Depth = -1; //in mm
	this->Sector = -1; //in %
	this->Gain = -1; //in %
	this->DynRange = -1; //in dB
	this->Zoom = -1; //in %
	this->Timeout = -1; // in ms
	this->CompressionStatus = 0; // no compression by default
	this->AcquisitionDataType = 0x00000004; //corresponds to type: BPost 8-bit  
	this->ImagingMode = 0; //corresponds to BMode imaging  

	this->FlipFrames = 0;

	this->FrameBufferRowAlignment = 1;

	//note, input data i.e. data from sonix machine is always uncompressed rgb 
	//so, by default we set the output format as rgb
	this->SetOutputFormatToLuminance(); 

	this->NumberOfScalarComponents = 1;
	this->NumberOfOutputFrames = 1;

	this->Opacity = 1.0;

	//// for accurate timing
	this->LastTimeStamp = 0;
	this->LastFrameCount = 0;
	this->EstimatedFramePeriod = 0;

	// apply vertical flip to each frame
	//this->Buffer->GetFrameFormat()->SetTopDown(1);

	this->Buffer->GetFrameFormat()->SetFrameGrabberType(FG_BASE); 
	for (int i = 0; i < this->Buffer->GetBufferSize(); i++)
	{
		this->Buffer->GetFrame(i)->SetFrameGrabberType(FG_BASE);
	}
}

//----------------------------------------------------------------------------
vtkSonixVideoSource2::~vtkSonixVideoSource2()
{ 

	this->vtkSonixVideoSource2::ReleaseSystemResources();

	if ( this->DataDescriptor != NULL ) 
	{
		delete this->DataDescriptor; 
		this->DataDescriptor = NULL; 
	}

	this->ult->disconnect(); 
	delete this->ult;
}

//----------------------------------------------------------------------------
// Up the reference count so it behaves like New
vtkSonixVideoSource2* vtkSonixVideoSource2::New()
{
	vtkSonixVideoSource2* ret = vtkSonixVideoSource2::GetInstance();
	ret->Register(NULL);
	return ret;
}

//----------------------------------------------------------------------------
// Return the single instance of the vtkOutputWindow
vtkSonixVideoSource2* vtkSonixVideoSource2::GetInstance()
{
	if(!vtkSonixVideoSource2::Instance)
	{
		// Try the factory first
		vtkSonixVideoSource2::Instance = (vtkSonixVideoSource2*)vtkObjectFactory::CreateInstance("vtkSonixVideoSource2");    
		if(!vtkSonixVideoSource2::Instance)
		{
			vtkSonixVideoSource2::Instance = new vtkSonixVideoSource2();     
		}
		if(!vtkSonixVideoSource2::Instance)
		{
			int error = 0;
		}
	}
	// return the instance
	return vtkSonixVideoSource2::Instance;
}

//----------------------------------------------------------------------------
void vtkSonixVideoSource2::SetInstance(vtkSonixVideoSource2* instance)
{
	if (vtkSonixVideoSource2::Instance==instance)
	{
		return;
	}
	// preferably this will be NULL
	if (vtkSonixVideoSource2::Instance)
	{
		vtkSonixVideoSource2::Instance->Delete();;
	}
	vtkSonixVideoSource2::Instance = instance;
	if (!instance)
	{
		return;
	}
	// user will call ->Delete() after setting instance
	instance->Register(NULL);
}
//----------------------------------------------------------------------------
void vtkSonixVideoSource2::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);

	os << indent << "Imaging mode: " << this->ImagingMode << "\n";
	os << indent << "Frequency: " << this->Frequency << "MHz\n";
	os << indent << "Frame rate: " << this->FrameRate << "fps\n";

}

//----------------------------------------------------------------------------
// the callback function used when there is a new frame of data received
bool vtkSonixVideoSource2::vtkSonixVideoSourceNewFrameCallback(void * data, int type, int sz, bool cine, int frmnum)
{    
	if(!data || !sz)
	{
		//printf("Error: no actual frame data received\n");
		return false;
	}

	if(data)
	{
		vtkSonixVideoSource2::GetInstance()->LocalInternalGrab(data, type, sz, cine, frmnum);    
	}
	return true;;
}

//----------------------------------------------------------------------------
// copy the Device Independent Bitmap from the VFW framebuffer into the
// vtkVideoSource framebuffer (don't do the unpacking yet)
void vtkSonixVideoSource2::LocalInternalGrab(void* dataPtr, int type, int sz, bool cine, int frmnum)
{
	if ( !this->Initialized )
	{
		return; 	
	}
	// get a thread lock on the frame buffer
	this->Buffer->Lock();

	//error check for data type, size
	if ((uData)type!= (uData)this->AcquisitionDataType)
	{
		LOG_ERROR( "Received data type is different than expected");
	}

	// 1) Do the frame buffer indices maintenance
	if (this->AutoAdvance)
	{
		this->AdvanceFrameBuffer(1);
		if (this->FrameIndex + 1 < this->Buffer->GetBufferSize())
		{
			this->FrameIndex++;
		}
	}

	// use the information about data type and frmnum to do cross checking that you are maintaining correct frame index, & receiving
	// expected data type
	this->FrameNumber = frmnum; 
	double unfilteredTimestamp(0), filteredTimestamp(0); 
	this->CreateTimeStampForFrame(this->FrameNumber, unfilteredTimestamp, filteredTimestamp);

	// 3) read the data, based on the data type and clip region information, which is reflected in frame buffer extents
	// this is necessary as there would be cases when there is a clip region defined i.e. only data from the desired extents should be copied 
	// to the local buffer, which demands necessary advancement of deviceDataPtr

	// first do the error check that whether the type arrived is same as the type requested?
	if (type != this->AcquisitionDataType)
	{
		// error: data being acquired is not the same as requested
		// do what?
	}

	// get the pointer to actual incoming data on to a local pointer
	unsigned char *deviceDataPtr = static_cast<unsigned char*>(dataPtr);

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

	//check if the data received has the same size in bytes as expected
	if (sz != inBytesPerRow*rows)
	{
		//error; data discrepancy!
		//what to do?
	}

	// for frame containing FC (frame count) in the beginning for data coming from cine, jump 2 bytes
	if(    (type == udtBPre) || (type == udtRF) 
		||  (type == udtMPre) || (type == udtPWRF)
		||  (type == udtColorRF)
		)
	{
		deviceDataPtr +=4;
	}


	deviceDataPtr += FrameBufferExtent[0]* FrameBufferBitsPerPixel/8;
	deviceDataPtr += FrameBufferExtent[2]*inBytesPerRow;

	// 4) copy data to the local vtk frame buffer
	while (--rows >= 0)
	{
		deviceDataPtr += inBytesPerRow; 
		for ( int c = 0; c < frameSize[0]; c++)
		{
			deviceDataPtr -= FrameBufferBitsPerPixel/8; 
			memcpy(frameBufferPtr,deviceDataPtr,FrameBufferBitsPerPixel/8);
			frameBufferPtr += FrameBufferBitsPerPixel/8; 
		}
		deviceDataPtr += inBytesPerRow;
	}

	// add the new frame and the current time to the buffer
	this->Buffer->AddItem(this->Buffer->GetFrame(0), unfilteredTimestamp, filteredTimestamp, this->FrameNumber);

	if (this->FrameCount++ == 0)
	{
		this->StartTimeStamp = this->Buffer->GetTimeStamp(0);
	}

	this->Modified();

	this->Buffer->Unlock();
}

//----------------------------------------------------------------------------
void vtkSonixVideoSource2::Initialize()
{
	if (this->Initialized)
	{
		return;
	}

	// Connect to device
	if ( !this->Connect() ) 
	{
		LOG_ERROR("Unable to connect to video device!"); 
		return; 
	}

	// Set up the frame buffer
	this->Buffer->Lock();
	// update the frame buffer now just in case there is an error
	this->UpdateFrameBuffer();
	this->DoFormatSetup();
	this->Buffer->Unlock();

	// update framebuffer 
	this->UpdateFrameBuffer();

	this->Initialized = 1;
}

//----------------------------------------------------------------------------
int vtkSonixVideoSource2::Connect()
{
	// 1) connect to sonix machine.
	if(!this->ult->connect(this->SonixHostIP))
	{
		char *err = new char[256]; 
		int sz = 256;
		this->ult->getLastError(err,sz);
		LOG_ERROR("Initialize: couldn't connect to Ultrasonix "<<" (" << err << ")");
		this->ReleaseSystemResources();
		return 0;
	}

	// 2) set the imaging mode
	if (!this->ult->selectMode(this->ImagingMode))
	{
		char *err = new char[256]; 
		int sz = 256;
		this->ult->getLastError(err,sz);
		LOG_ERROR("Initialize: couldn't select imaging mode (" << err << ")");
		this->ReleaseSystemResources();
		return 0;
	}

	// do we need to wait for a little while before the mode actually gets selected??
	// like a thread sleep or something??
	vtkAccurateTimer::Delay(2); 

	// double-check to see if the mode has actually been set
	if (this->ImagingMode != this->ult->getActiveImagingMode())
	{
		char *err = new char[256];  
		int sz = 256;
		this->ult->getLastError(err,sz);
		LOG_ERROR("Initialize: Requested imaging mode could not be selected(" << err << ")");
		this->ReleaseSystemResources();
		return 0;
	}

	// 3) set the data acquisition type
	// check if the desired acquisition type is actually available on desired imaging mode
	if (!this->ult->isDataAvailable((uData)(AcquisitionDataType)))
	{
		char *err = new char[256];  
		int sz = 256;
		this->ult->getLastError(err,sz);
		LOG_ERROR("Initialize: Requested the data aquisition type not available for selected imaging mode(" << err << ")");
		this->ReleaseSystemResources();
		return 0;
	}
	// actually request data, now that its available
	if (!this->ult->setDataToAcquire(AcquisitionDataType))
	{
		char *err = new char[256];  
		int sz = 256;
		this->ult->getLastError(err,sz);
		LOG_ERROR("Initialize: couldn't request the data aquisition type (" << err << ")");
		this->ReleaseSystemResources();
		return 0;
	}

	// 4) get the data descriptor
	if (!this->ult->getDataDescriptor((uData)AcquisitionDataType, *this->DataDescriptor))
	{
		char *err = new char[256];  
		int sz = 256;
		this->ult->getLastError(err,sz);
		LOG_ERROR("Initialize: couldn't retrieve data descriptor (" << err << ")");
		this->ReleaseSystemResources();
		return 0;
	}

	// Parameter setting doesn't work with Ulterius-2.x
#if ULTERIUS_MAJOR_VERSION != 2
	// 6) set parameters, currently: frequency, frame rate, depth

#if ULTERIUS_MAJOR_VERSION < 2
	if (this->Frequency >= 0 && !this->ult->setParamValue(VARID_FREQ, this->Frequency))
#else 
	uParam prmFrequency; 
	if ( !this->ult->getParam(VARID_FREQ, prmFrequency) )
	{
		char *err = new char[256];  
		int sz = 256;
		this->ult->getLastError(err,sz);
		LOG_ERROR("Unable to get parameter: " << err); 
		return 0; 
	}
	if (this->Frequency >= 0 && !this->ult->setParamValue(prmFrequency.id, this->Frequency))
#endif
	{
		char *err = new char[256];  
		int sz = 256;
		this->ult->getLastError(err,sz);
		LOG_ERROR("Initialize: couldn't set desired frequency (" << err << ")");
		this->ReleaseSystemResources();
		return 0;
	}

#if ULTERIUS_MAJOR_VERSION < 2
	if (this->Depth >= 0 && !this->ult->setParamValue(VARID_DEPTH, this->Depth))
#else
	uParam prmDepth; 
	if ( ! this->ult->getParam(VARID_DEPTH, prmDepth) )
	{
		char *err = new char[256];  
		int sz = 256;
		this->ult->getLastError(err,sz);
		LOG_ERROR("Unable to get parameter: " << err); 
		return 0; 
	}
	if (this->Depth >= 0 && !this->ult->setParamValue(prmDepth.id, this->Depth))
#endif
	{
		char *err = new char[256];  
		int sz = 256;
		this->ult->getLastError(err,sz);
		LOG_ERROR("Initialize: couldn't set desired depth (" << err << ")");
		this->ReleaseSystemResources();
		return 0;
	}

#if ULTERIUS_MAJOR_VERSION < 2
	if (this->Sector >= 0 && !this->ult->setParamValue(VARID_SECTOR, this->Sector))
#else
	uParam prmSector; 
	if ( !this->ult->getParam(VARID_SECTOR, prmSector) )
	{
		char *err = new char[256];  
		int sz = 256;
		this->ult->getLastError(err,sz);
		LOG_ERROR("Unable to get parameter: " << err); 
		return 0; 
	}
	if (this->Sector >= 0 && !this->ult->setParamValue(prmSector.id, this->Sector))
#endif
	{
		char *err = new char[256];  
		int sz = 256;
		this->ult->getLastError(err,sz);
		LOG_ERROR("Initialize: couldn't set desired sector (" << err << ")");
		this->ReleaseSystemResources();
		return 0;
	}

#if ULTERIUS_MAJOR_VERSION < 2
	if (this->Gain >= 0 && !this->ult->setParamValue(VARID_GAIN, this->Gain))
#else
	uParam prmGain; 
	if ( !this->ult->getParam(VARID_GAIN, prmGain) )
	{
		char *err = new char[256];  
		int sz = 256;
		this->ult->getLastError(err,sz);
		LOG_ERROR("Unable to get parameter: " << err); 
		return 0; 
	}
	if (this->Gain >= 0 && !this->ult->setParamValue(prmGain.id, this->Gain))
#endif 
	{
		char *err = new char[256];  
		int sz = 256;
		this->ult->getLastError(err,sz);
		LOG_ERROR("Initialize: couldn't set desired gain (" << err << ")");
		this->ReleaseSystemResources();
		return 0;
	}

#if ULTERIUS_MAJOR_VERSION < 2 
	if (this->DynRange >= 0 && !this->ult->setParamValue(VARID_DYNRANGE, this->DynRange))
#else
	uParam prmDynRange; 
	if ( !this->ult->getParam(VARID_DYNRANGE, prmDynRange) )
	{
		char *err = new char[256];  
		int sz = 256;
		this->ult->getLastError(err,sz);
		LOG_ERROR("Unable to get parameter: " << err); 
		return 0; 
	}
	if (this->DynRange >= 0 && !this->ult->setParamValue(prmDynRange.id, this->DynRange))
#endif 
	{
		char *err = new char[256];  
		int sz = 256;
		this->ult->getLastError(err,sz);
		LOG_ERROR("Initialize: couldn't set desired dyn range (" << err << ")");
		this->ReleaseSystemResources();
		return 0;
	}

#if ULTERIUS_MAJOR_VERSION < 2 
	if (this->Zoom >= 0 && !this->ult->setParamValue(VARID_ZOOM, this->Zoom))
#else
	uParam prmZoom; 
	if ( !this->ult->getParam(VARID_ZOOM, prmZoom) )
	{
		char *err = new char[256];  
		int sz = 256;
		this->ult->getLastError(err,sz);
		LOG_ERROR("Unable to get parameter: " << err); 
		return 0; 
	}
	if (this->Zoom >= 0 && !this->ult->setParamValue(prmZoom.id, this->Zoom))
#endif 
	{
		char *err = new char[256];  
		int sz = 256;
		this->ult->getLastError(err,sz);
		LOG_ERROR("Initialize: couldn't set desired zoom (" << err << ")");
		this->ReleaseSystemResources();
		return 0;
	}

	if (!this->ult->setCompressionStatus(this->CompressionStatus))
	{
		char *err = new char[256];  
		int sz = 256;
		this->ult->getLastError(err,sz);
		LOG_ERROR("Initialize: couldn't set compression status (" << err << ")");
		this->ReleaseSystemResources();
		return 0;
	}

	// set callback for receiving new frames
	this->ult->setCallback(vtkSonixVideoSourceNewFrameCallback);

	// Do not change the current settings if it's not set 
	if ( this->Timeout > 0 )
	{
		this->ult->setTimeout(this->Timeout); 
	}
#else
	LOG_WARNING("Ultrasound imaging parameter setting is not supported with Ulterius-2.x");
#endif

	return 1; 
}

//----------------------------------------------------------------------------
void vtkSonixVideoSource2::Disconnect()
{
	this->Stop();
	this->ult->disconnect();
}

//----------------------------------------------------------------------------
void vtkSonixVideoSource2::ReleaseSystemResources()
{
	this->Disconnect(); 

	this->Initialized = 0;
}

//----------------------------------------------------------------------------
void vtkSonixVideoSource2::Grab()
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
void vtkSonixVideoSource2::Record()
{
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
		this->Recording = 1;
		this->Modified();
		if(this->ult->getFreezeState())
			this->ult->toggleFreeze();
	}
}

//----------------------------------------------------------------------------
void vtkSonixVideoSource2::Play()
{
	this->vtkVideoSource2::Play();
}

//----------------------------------------------------------------------------
void vtkSonixVideoSource2::Stop()
{
	if (this->Recording)
	{
		this->Recording = 0;
		this->Modified();

		if (!this->ult->getFreezeState())
			this->ult->toggleFreeze();
	}
	else if (this->Playing)
	{
		this->vtkVideoSource2::Stop();
	}
}


//----------------------------------------------------------------------------
int vtkSonixVideoSource2::RequestInformation(
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

	// set the whole extent
	int FrameBufferExtent[6];
	this->Buffer->GetFrameFormat()->GetFrameExtent(FrameBufferExtent);
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
				FrameBufferExtent[2*i+1] - FrameBufferExtent[2*i];
		}
		this->FrameOutputExtent[2*i] = extent[2*i];
		this->FrameOutputExtent[2*i+1] = extent[2*i+1];
	}

	int numFrames = this->NumberOfOutputFrames;
	if (numFrames < 1)
	{
		numFrames = 1;
	}
	if (numFrames > this->Buffer->GetBufferSize())
	{
		numFrames = this->Buffer->GetBufferSize();
	}

	// multiply Z extent by number of frames to output
	extent[5] = extent[4] + (extent[5]-extent[4]+1) * numFrames - 1;

	outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),extent,6);

	outInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),extent,6);
	// set the spacing
	outInfo->Set(vtkDataObject::SPACING(),this->DataSpacing,3);

	// set the origin.
	outInfo->Set(vtkDataObject::ORIGIN(),this->DataOrigin,3);

	if((this->AcquisitionDataType == udtRF) || (this->AcquisitionDataType == udtColorRF) || (this->AcquisitionDataType == udtPWRF))
	{
		vtkDataObject::SetPointDataActiveScalarInfo(outInfo, VTK_SHORT, 
			this->NumberOfScalarComponents);
	}
	else
	{
		// set default data type (8 bit greyscale)  
		vtkDataObject::SetPointDataActiveScalarInfo(outInfo, VTK_UNSIGNED_CHAR, 
			this->NumberOfScalarComponents);
	}
	return 1;
}

//----------------------------------------------------------------------------
void vtkSonixVideoSource2::UnpackRasterLine(char *outptr, char *inptr, 
											int start, int count)
{
	char alpha = (char)(this->Opacity*255);

	switch (this->AcquisitionDataType)
	{
		// all these data types have 8-bit greyscale raster data
	case udtBPost:
	case udtMPost:
	case udtPWSpectrum:
	case udtElastoOverlay:
		{
			inptr += start;
			memcpy(outptr,inptr,count);
		}
		break;

		//these data types give vector data 8-bit, with FC at the start
	case udtBPre:
	case udtMPre:
	case udtElastoPre: //this data type does not have a FC at the start
		{
			inptr += start;
			memcpy(outptr,inptr,count);
		}
		break;

		//these data types give 16-bit vector data, to be read into int, just one component
	case udtColorRF:
	case udtPWRF:
	case udtRF:
		{
			inptr += 2*start;
			//unsigned short rawWord;
			//unsigned short *shInPtr = (unsigned short *)inptr;
			//unsigned short *shOutPtr = (unsigned short *)outptr;
			outptr += 2;
			while (--count >= 0)
			{
				*--outptr = *inptr++;
				*--outptr = *inptr++;
				outptr += 4;
			}
			//*shOutPtr++ = *shInPtr++;
			//*outptr++ = *inptr++;


			//memcpy(outptr,inptr,2*count);

		}
		break;

		// 16-bit vector data, but two components
		// don't know how to handle it as yet
	case udtColorVelocityVariance:
		this->OutputFormat = VTK_RGB;
		this->NumberOfScalarComponents = 2;
		break;

		//32-bit data
	case udtScreen:
	case udtBPost32:
#if ULTERIUS_MAJOR_VERSION == 1
	case udtColorPost:
#endif
	case udtElastoCombined:
		inptr += 4*start;
		{ // must do BGRX to RGBA conversion
			outptr += 4;
			while (--count >= 0)
			{
				*--outptr = alpha;
				*--outptr = *inptr++;
				*--outptr = *inptr++;
				*--outptr = *inptr++;
				inptr++;
				outptr += 8;
			}
		}
		break;

	default:
		break;

	}


}


//----------------------------------------------------------------------------
void vtkSonixVideoSource2::SetOutputFormat(int format)
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

	if (this->Buffer->GetFrameFormat()->GetBitsPerPixel() != numComponents*8)
	{
		this->Buffer->Lock();
		this->Buffer->GetFrameFormat()->SetBitsPerPixel(numComponents*8);
		if (this->Initialized)
		{
			this->UpdateFrameBuffer();    
			this->DoFormatSetup();
		}
		this->Buffer->Unlock();
	}

	this->Modified();
}

//----------------------------------------------------------------------------
// check the current video format and set up the VTK video framebuffer to match
void vtkSonixVideoSource2::DoFormatSetup()
{
	//set the frame size from the data descriptor, 
	this->SetFrameSize(this->DataDescriptor->w, this->DataDescriptor->h, 1);
	this->Buffer->GetFrameFormat()->SetBitsPerPixel(this->DataDescriptor->ss);
	switch (this->AcquisitionDataType)
	{
		// all these data types have 8-bit greyscale raster data
	case udtBPost:
	case udtMPost:
	case udtPWSpectrum:
	case udtElastoOverlay:
		this->OutputFormat = VTK_LUMINANCE;
		this->NumberOfScalarComponents = 1;
		break;
		//these data types give vector data 8-bit, with FC at the start
	case udtBPre:
	case udtMPre:
	case udtElastoPre: //this data type does not have a FC at the start
		this->SetFrameSize(this->DataDescriptor->h, this->DataDescriptor->w, 1);
		this->OutputFormat = VTK_LUMINANCE;
		this->NumberOfScalarComponents = 1;
		break;

		//these data types give 16-bit vector data, to be read into int, just one component
	case udtColorRF:
	case udtPWRF:
	case udtRF:
		this->SetFrameSize(this->DataDescriptor->h, this->DataDescriptor->w, 1);
		this->OutputFormat = VTK_LUMINANCE;
		this->NumberOfScalarComponents = 1;
		break;

		// 16-bit vector data, but two components
		// don't know how to handle it as yet
	case udtColorVelocityVariance:
		this->OutputFormat = VTK_RGB;
		this->NumberOfScalarComponents = 2;
		break;

		//32-bit data
	case udtScreen:
	case udtBPost32:
#if ULTERIUS_MAJOR_VERSION == 1
	case udtColorPost:
#endif
	case udtElastoCombined:
		this->OutputFormat = VTK_RGBA;
		this->NumberOfScalarComponents = 4;        
		break;
	}

	this->Modified();
	this->UpdateFrameBuffer();

}

//----------------------------------------------------------------------------
void vtkSonixVideoSource2::SetSonixIP(const char *SonixIP)
{
	if (SonixIP)
	{
		this->SonixHostIP = new char[256];
		sprintf(this->SonixHostIP, "%s", SonixIP);    
	}
}

