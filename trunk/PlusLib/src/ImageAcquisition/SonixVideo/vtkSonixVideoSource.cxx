/*=========================================================================

  Module:    $RCSfile: vtkSonixVideoSource.cxx,v $
  Author:  Siddharth Vikal, Queens School Of Computing

Copyright (c) 2008, Queen's University, Kingston, Ontario, Canada
All rights reserved.

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

#include "vtkSonixVideoSource.h"

#include "vtkImageData.h"
#include "vtkCriticalSection.h"
#include "vtkObjectFactory.h"
#include "vtkTimerLog.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkUnsignedCharArray.h"
#include "vtksys/SystemTools.hxx"

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



vtkCxxRevisionMacro(vtkSonixVideoSource, "$Revision: 1.0$");
//vtkStandardNewMacro(vtkWin32VideoSource);
//----------------------------------------------------------------------------
// Needed when we don't use the vtkStandardNewMacro.
vtkInstantiatorNewMacro(vtkSonixVideoSource);

//----------------------------------------------------------------------------

vtkSonixVideoSource* vtkSonixVideoSource::Instance = 0;
vtkSonixVideoSourceCleanup vtkSonixVideoSource::Cleanup;


//sonic param ids
#if ULTERIUS_MAJOR_VERSION == 1
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
#endif

#if ULTERIUS_MAJOR_VERSION > 1
#define VARID_FREQ ""
#define VARID_DEPTH ""
#define VARID_GAIN  ""
#define VARID_CGAIN ""
#define VARID_PGAIN ""
#define VARID_TGC ""
#define VARID_ZOOM ""
#define VARID_CPRF ""
#define VARID_PPRF ""
#define VARID_SECTOR ""
#define VARID_BCHROMA ""
#define VARID_MCHROMA ""
#define VARID_DYNRANGE ""
#define VARID_CFOCUS ""
#define VARID_CFOCUSCOLOR ""
#define VARID_SFOCUS ""
#define VARID_DFOCUS ""
#define VARID_FRATE ""
#define VARID_MSWEEP ""
#define VARID_CLARITY ""
#define VARID_CMAP ""
#define VARID_BMAP ""
#endif 

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


vtkSonixVideoSourceCleanup::vtkSonixVideoSourceCleanup()
{
}

vtkSonixVideoSourceCleanup::~vtkSonixVideoSourceCleanup()
{
  // Destroy any remaining output window.
  vtkSonixVideoSource::SetInstance(NULL);
}
//----------------------------------------------------------------------------
vtkSonixVideoSource::vtkSonixVideoSource()
{
  this->ult = new ulterius();
  this->DataDescriptor = new uDataDesc();
  this->Initialized = 0;

  this->FrameSize[0] = 640;
  this->FrameSize[1] = 480;
  this->FrameSize[2] = 1;
  
  this->SonixIP = NULL; 
  this->FrameRate = 13; // in fps
  this->Frequency = 1; //in Mhz
  this->Depth = 150; //in mm
  this->AcquisitionDataType = udtBPost; //corresponds to type: BPost 8-bit  
  this->ImagingMode = 0; //corresponds to BMode imaging  

  //note, input data i.e. data from sonix machine is always uncompressed rgb 
  //so, by default we set the output format as rgb
  this->OutputFormat = VTK_LUMINANCE;
  this->NumberOfScalarComponents = 1;
  this->FrameBufferBitsPerPixel = 8;
  this->FlipFrames = 0;
  this->FrameBufferRowAlignment = 1;  
}

//----------------------------------------------------------------------------
vtkSonixVideoSource::~vtkSonixVideoSource()
{ 
  this->vtkSonixVideoSource::ReleaseSystemResources();
  delete this->ult;
}


// Up the reference count so it behaves like New
vtkSonixVideoSource* vtkSonixVideoSource::New()
{
  vtkSonixVideoSource* ret = vtkSonixVideoSource::GetInstance();
  ret->Register(NULL);
  return ret;
}


// Return the single instance of the vtkOutputWindow
vtkSonixVideoSource* vtkSonixVideoSource::GetInstance()
{
  if(!vtkSonixVideoSource::Instance)
    {
    // Try the factory first
	 vtkSonixVideoSource::Instance = (vtkSonixVideoSource*)vtkObjectFactory::CreateInstance("vtkSonixVideoSource");    
	 if(!vtkSonixVideoSource::Instance)
	   {
	   vtkSonixVideoSource::Instance = new vtkSonixVideoSource();	   
	   }
	 if(!vtkSonixVideoSource::Instance)
	   {
	   int error = 0;
	   }
    }
  // return the instance
  return vtkSonixVideoSource::Instance;
}

void vtkSonixVideoSource::SetInstance(vtkSonixVideoSource* instance)
{
  if (vtkSonixVideoSource::Instance==instance)
    {
    return;
    }
  // preferably this will be NULL
  if (vtkSonixVideoSource::Instance)
    {
    vtkSonixVideoSource::Instance->Delete();;
    }
  vtkSonixVideoSource::Instance = instance;
  if (!instance)
    {
    return;
    }
  // user will call ->Delete() after setting instance
  instance->Register(NULL);
}
//----------------------------------------------------------------------------
void vtkSonixVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Imaging mode: " << this->ImagingMode << "\n";
  os << indent << "Frequency: " << this->Frequency << "MHz\n";
  os << indent << "Frame rate: " << this->FrameRate << "fps\n";
  
}


// the callback function used when there is a new frame of data received
bool vtkSonixVideoSource::vtkSonixVideoSourceNewFrameCallback(void * data, int type, int sz, bool cine, int frmnum)
{    
    if(!data || !sz)
    {
        printf("Error: no actual frame data received\n");
        return false;
    }
    
    /*if(BUFFERSIZE < sz)
    {
        printf("Error: frame too large for current buffer\n");
        return false;
    }
      */  
    //printf("[Rx] type:(%d) size:(%d) cine:(%d) tag:(%d)\n", type, sz, cine, ((int*)data)[0]);

    // make sure we dont do an operation that takes longer than the acquisition frame rate
    //memcpy(gBuffer, data, sz);
	if(data)
    {
		vtkSonixVideoSource::GetInstance()->LocalInternalGrab(data, type, sz, cine, frmnum);    
    }
    return true;;
}

//----------------------------------------------------------------------------
// copy the Device Independent Bitmap from the VFW framebuffer into the
// vtkVideoSource framebuffer (don't do the unpacking yet)
void vtkSonixVideoSource::LocalInternalGrab(void* dataPtr, int type, int sz, bool cine, int frmnum)
{
	//to do
	// 1) Do frame buffer indices maintenance
	// 2) Do time stamping
	// 3) decode the data according to type
	// 4) copy data to the local vtk frame buffer
    
  
  // get the pointer to data
  // use the information about data type and frmnum to do cross checking that you are maintaining correct frame index, & receiving
  // expected data type


  // get a thread lock on the frame buffer
  this->FrameBufferMutex->Lock();
 
  //error check for data type, size
  if ((uData)type!= (uData)this->AcquisitionDataType)
    {
	vtkErrorMacro(<< "Received data type is different than expected");
    }
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

  // error check, if the frame indices mismatch, then it indicates, we have missed the frame?
  if ( frmnum != index+1)
    {
	// error ??
	// std::cout << "Frame goes missing"<< std::endl;
	// what is to be done in this case??
    }

  // 2) Do the time stamping
  this->FrameBufferTimeStamps[index] = vtkAccurateTimer::GetSystemTime();

  if (this->FrameCount++ == 0)
    {
    this->StartTimeStamp = this->FrameBufferTimeStamps[index];
    }

  
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
	unsigned char *frameBufferPtr = (unsigned char *)((reinterpret_cast<vtkUnsignedCharArray*>(this->FrameBuffer[index]))->GetPointer(0));

   
	int outBytesPerRow = ((this->FrameBufferExtent[1]- this->FrameBufferExtent[0]+1)* this->FrameBufferBitsPerPixel + 7)/8;
	outBytesPerRow += outBytesPerRow % this->FrameBufferRowAlignment;

	int inBytesPerRow = this->FrameSize[0] * this->FrameBufferBitsPerPixel/8;
  
	int rows = this->FrameBufferExtent[3]-this->FrameBufferExtent[2]+1;

	//check if the data received has the same size in bytes as expected
	if (sz != inBytesPerRow*rows)
	  {
	  //error; data discrepancy!
	  //what to do?
	  }

	// for frame containing FC (frame count) in the beginning for data coming from cine, jump 2 bytes
	if(		(type == udtBPre) || (type == udtRF) 
		||	(type == udtMPre) || (type == udtPWRF)
		||  (type == udtColorRF)
	  )
	  {
	  deviceDataPtr +=4;
	  }


	deviceDataPtr += this->FrameBufferExtent[0]* this->FrameBufferBitsPerPixel/8;
	deviceDataPtr += this->FrameBufferExtent[2]*inBytesPerRow;

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
 
  this->Modified();

  this->FrameBufferMutex->Unlock();
}

//----------------------------------------------------------------------------
void vtkSonixVideoSource::Initialize()
{
	//to do:
	//1) connect to sonix machine using the ip address provided earlier
	//2) set the imaging mode
	//3) set the data acquisition type
	//4) get the data descriptor corresponding to the data type requested
	//5) set up the frame buffer accordingly
	//6) set parameters like: frequency, frame rate, depth
	//7) set the callback function which gets invoked upon arrival of new frame
	//8) update frame buffer
  if (this->Initialized)
    {
    return;
    }

   
  // 1) connect to sonix machine.
  if(!this->ult->connect(this->SonixIP))
    {
	char *err = new char[256]; 
	int sz = 256;
	this->ult->getLastError(err,sz);
	vtkErrorMacro(<< "Initialize: couldn't connect to Sonix RP"<<" (" << err << ")");
    this->ReleaseSystemResources();
    return;
    }

  // 2) set the imaging mode
  if (!this->ult->selectMode(this->ImagingMode))
    {
	char *err = new char[256]; 
	int sz = 256;
	this->ult->getLastError(err,sz);
    vtkErrorMacro(<< "Initialize: couldn't select imaging mode (" << err << ")");
    this->ReleaseSystemResources();
    return;
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
    vtkErrorMacro(<< "Initialize: Requested imaging mode could not be selected(" << err << ")");
    this->ReleaseSystemResources();
    return;
	}

  // 3) set the data acquisition type
  // check if the desired acquisition type is actually available on desired imaging mode
  if (!this->ult->isDataAvailable((uData)(AcquisitionDataType)))
    {
	char *err = new char[256];  
	int sz = 256;
	this->ult->getLastError(err,sz);
    vtkErrorMacro(<< "Initialize: Requested the data aquisition type not available for selected imaging mode(" << err << ")");
    this->ReleaseSystemResources();
    return;
    }
  // actually request data, now that its available
  if (!this->ult->setDataToAcquire(AcquisitionDataType))
    {
	char *err = new char[256];  
	int sz = 256;
	this->ult->getLastError(err,sz);
    vtkErrorMacro(<< "Initialize: couldn't request the data aquisition type (" << err << ")");
    this->ReleaseSystemResources();
    return;
    }

  // 4) get the data descriptor
  if (!this->ult->getDataDescriptor((uData)AcquisitionDataType, *this->DataDescriptor))
    {
	char *err = new char[256];  
	int sz = 256;
	this->ult->getLastError(err,sz);
    vtkErrorMacro(<< "Initialize: couldn't retrieve data descriptor (" << err << ")");
    this->ReleaseSystemResources();
    return;
    }

  // 5) set up the frame buffer
  this->FrameBufferMutex->Lock();
  this->DoFormatSetup();
  this->FrameBufferMutex->Unlock();



  // 6) set parameters, currently: frequency, frame rate, depth
  if (!this->ult->setParamValue(VARID_FREQ, Frequency))
    {
	char *err = new char[256];  
	int sz = 256;
	this->ult->getLastError(err,sz);
    vtkErrorMacro(<< "Initialize: couldn't set desired frequency (" << err << ")");
    this->ReleaseSystemResources();
    return;
    }

  if (!this->ult->setParamValue(VARID_FRATE, FrameRate))
    {
	char *err = new char[256]; 
	int sz = 256;
	this->ult->getLastError(err,sz);
    vtkErrorMacro(<< "Initialize: couldn't set desired frame rate (" << err << ")");
    this->ReleaseSystemResources();
    return;
    }

  if (!this->ult->setParamValue(VARID_DEPTH, Depth))
    {
	char *err = new char[256];  
	int sz = 256;
	this->ult->getLastError(err,sz);
    vtkErrorMacro(<< "Initialize: couldn't set desired depth (" << err << ")");
    this->ReleaseSystemResources();
    return;
    }

  // 7) set callback for receiving new frames
  this->ult->setCallback(vtkSonixVideoSourceNewFrameCallback);

  // 8)update framebuffer 
  this->UpdateFrameBuffer();

  this->Initialized = 1;
}

//----------------------------------------------------------------------------
void vtkSonixVideoSource::ReleaseSystemResources()
{
  this->ult->disconnect();
}

//----------------------------------------------------------------------------
void vtkSonixVideoSource::Grab()
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

  // just do the grab, the callback does the rest
  //this->SetStartTimeStamp(vtkAccurateTimer::GetSystemTime());
//  capGrabFrameNoStop(this->Internal->CapWnd);
}

//----------------------------------------------------------------------------
void vtkSonixVideoSource::Record()
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
    if(this->ult->getFreezeState())
		this->ult->toggleFreeze();
    }
}
    
//----------------------------------------------------------------------------
void vtkSonixVideoSource::Play()
{
  this->vtkVideoSource::Play();
}
    
//----------------------------------------------------------------------------
void vtkSonixVideoSource::Stop()
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
    this->vtkVideoSource::Stop();
    }
}


//----------------------------------------------------------------------------
int vtkSonixVideoSource::RequestInformation(
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
int vtkSonixVideoSource::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **vtkNotUsed(inputVector),
  vtkInformationVector *vtkNotUsed(outputVector))
{
  vtkImageData *data = this->AllocateOutputData(this->GetOutput());
  int i,j;

  int outputExtent[6];     // will later be clipped in Z to a single frame
  int saveOutputExtent[6]; // will possibly contain multiple frames
  data->GetExtent(outputExtent);
  for (i = 0; i < 6; i++)
    {
    saveOutputExtent[i] = outputExtent[i];
    }
  // clip to extent to the Z size of one frame  
  outputExtent[4] = this->FrameOutputExtent[4]; 
  outputExtent[5] = this->FrameOutputExtent[5]; 

  int frameExtentX = this->FrameBufferExtent[1]-this->FrameBufferExtent[0]+1;
  int frameExtentY = this->FrameBufferExtent[3]-this->FrameBufferExtent[2]+1;
  int frameExtentZ = this->FrameBufferExtent[5]-this->FrameBufferExtent[4]+1;

  int extentX = outputExtent[1]-outputExtent[0]+1;
  int extentY = outputExtent[3]-outputExtent[2]+1;
  int extentZ = outputExtent[5]-outputExtent[4]+1;

  // if the output is more than a single frame,
  // then the output will cover a partial or full first frame,
  // several full frames, and a partial or full last frame

  // index and Z size of the first frame in the output extent
  int firstFrame = (saveOutputExtent[4]-outputExtent[4])/extentZ;
  int firstOutputExtent4 = saveOutputExtent[4] - extentZ*firstFrame;

  // index and Z size of the final frame in the output extent
  int finalFrame = (saveOutputExtent[5]-outputExtent[4])/extentZ;
  int finalOutputExtent5 = saveOutputExtent[5] - extentZ*finalFrame;

  char *outPtr = (char *)data->GetScalarPointer();
  char *outPtrTmp;

  int inIncY = (frameExtentX*this->FrameBufferBitsPerPixel + 7)/8;
  inIncY = ((inIncY + this->FrameBufferRowAlignment - 1)/
            this->FrameBufferRowAlignment)*this->FrameBufferRowAlignment;
  int inIncZ = inIncY*frameExtentY;

  int outIncX = this->FrameBufferBitsPerPixel/8;
  int outIncY = outIncX*extentX;
  int outIncZ = outIncY*extentY;

  int inPadX = 0;
  int inPadY = 0;
  int inPadZ; // do inPadZ later

  int outPadX = -outputExtent[0];
  int outPadY = -outputExtent[2];
  int outPadZ;  // do outPadZ later

  if (outPadX < 0)
    {
    inPadX -= outPadX;
    outPadX = 0;
    }

  if (outPadY < 0)
    {
    inPadY -= outPadY;
    outPadY = 0;
    }

  int outX = frameExtentX - inPadX; 
  int outY = frameExtentY - inPadY; 
  int outZ; // do outZ later

  if (outX > extentX - outPadX)
    {
    outX = extentX - outPadX;
    }
  if (outY > extentY - outPadY)
    {
    outY = extentY - outPadY;
    }

  // if output extent has changed, need to initialize output to black
  for (i = 0; i < 3; i++)
    {
    if (saveOutputExtent[i] != this->LastOutputExtent[i])
      {
      this->LastOutputExtent[i] = saveOutputExtent[i];
      this->OutputNeedsInitialization = 1;
      }
    }

  // ditto for number of scalar components
  if (data->GetNumberOfScalarComponents() != 
      this->LastNumberOfScalarComponents)
    {
    this->LastNumberOfScalarComponents = data->GetNumberOfScalarComponents();
    this->OutputNeedsInitialization = 1;
    }

  // initialize output to zero only when necessary
  if (this->OutputNeedsInitialization)
    {
    memset(outPtr,0,
           (saveOutputExtent[1]-saveOutputExtent[0]+1)*
           (saveOutputExtent[3]-saveOutputExtent[2]+1)*
           (saveOutputExtent[5]-saveOutputExtent[4]+1)*outIncX);
    this->OutputNeedsInitialization = 0;
    } 

  // we have to modify the outputExtent of the first frame,
  // because it might be complete (it will be restored after
  // the first frame has been copied to the output)
  int saveOutputExtent4 = outputExtent[4];
  outputExtent[4] = firstOutputExtent4;

  this->FrameBufferMutex->Lock();

  int index = this->FrameBufferIndex;
  this->FrameTimeStamp = 
    this->FrameBufferTimeStamps[index % this->FrameBufferSize];

  int frame;
  for (frame = firstFrame; frame <= finalFrame; frame++)
    {
    if (frame == finalFrame)
      {
      outputExtent[5] = finalOutputExtent5;
      } 
    
    vtkDataArray *frameBuffer = reinterpret_cast<vtkDataArray *>(this->FrameBuffer[(index + frame) % this->FrameBufferSize]);

    char *inPtr = reinterpret_cast<char*>(frameBuffer->GetVoidPointer(0));
    char *inPtrTmp ;

    extentZ = outputExtent[5]-outputExtent[4]+1;
    inPadZ = 0;
    outPadZ = -outputExtent[4];
    
    if (outPadZ < 0)
      {
      inPadZ -= outPadZ;
      outPadZ = 0;
      }

    outZ = frameExtentZ - inPadZ;

    if (outZ > extentZ - outPadZ)
      {
      outZ = extentZ - outPadZ;
      }

    if (this->FlipFrames)
      { // apply a vertical flip while copying to output
      outPtr += outIncZ*outPadZ+outIncY*outPadY+outIncX*outPadX;
      inPtr += inIncZ*inPadZ+inIncY*(frameExtentY-inPadY-outY);

      for (i = 0; i < outZ; i++)
        {
        inPtrTmp = inPtr;
        outPtrTmp = outPtr + outIncY*outY;
        for (j = 0; j < outY; j++)
          {
          outPtrTmp -= outIncY;
          if (outX > 0)
            {
            this->UnpackRasterLine(outPtrTmp,inPtrTmp,inPadX,outX);
            }
          inPtrTmp += inIncY;
          }
        outPtr += outIncZ;
        inPtr += inIncZ;
        }
      }
    else
      { // don't apply a vertical flip
      outPtr += outIncZ*outPadZ+outIncY*outPadY+outIncX*outPadX;
      inPtr += inIncZ*inPadZ+inIncY*inPadY;

      for (i = 0; i < outZ; i++)
        {
        inPtrTmp = inPtr;
        outPtrTmp = outPtr;
        for (j = 0; j < outY; j++)
          {
          if (outX > 0) 
            {
            this->UnpackRasterLine(outPtrTmp,inPtrTmp,inPadX,outX);
            }
          outPtrTmp += outIncY;
          inPtrTmp += inIncY;
          }
        outPtr += outIncZ;
        inPtr += inIncZ;
        }
      }
    // restore the output extent once the first frame is done
    outputExtent[4] = saveOutputExtent4;
    }

  this->FrameBufferMutex->Unlock();

  return 1;
}

//----------------------------------------------------------------------------
// codecs


//----------------------------------------------------------------------------
void vtkSonixVideoSource::UnpackRasterLine(char *outptr, char *inptr, 
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
void vtkSonixVideoSource::SetOutputFormat(int format)
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
      vtkErrorMacro(<< "SetOutputFormat: Unrecognized color format.");
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
      this->DoFormatSetup();
	  }
    this->FrameBufferMutex->Unlock();
    }

  this->Modified();
}


// check the current video format and set up the VTK video framebuffer to match
void vtkSonixVideoSource::DoFormatSetup()
{

  // use the information from the data descriptor, here's how the data descriptor looks like:
  // it is defined in ulterius_def.h
  /*
    class uDataDesc
    {
	public:
		/// data type
		uData type;
		/// data width
		int w;
		/// data height
		int h;
		/// data sample size in bits
		int ss;
		/// roi of data
		uROI roi;
	};

	class uROI
	{
	public:
		/// roi - upper left (x)
		int ulx;
		/// roi - upper left (y)
		int uly;
		/// roi - upper right (x)
		int urx;
		/// roi - upper right (y)
		int ury;
		/// roi - bottom right (x)
		int brx;
		/// roi - bottom right (y)
		int bry;
		/// roi - bottom left (x)
		int blx;
		/// roi - bottom left (y)
		int bly;
	};

  */


  //set the frame size from the data descriptor, 
  this->FrameSize[0] = this->DataDescriptor->w;
  this->FrameSize[1] = this->DataDescriptor->h;
  this->FrameBufferBitsPerPixel = this->DataDescriptor->ss;
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
		this->FrameSize[0] = this->DataDescriptor->h;
		this->FrameSize[1] = this->DataDescriptor->w;  		
		this->OutputFormat = VTK_LUMINANCE;
        this->NumberOfScalarComponents = 1;
        break;

	//these data types give 16-bit vector data, to be read into int, just one component
	case udtColorRF:
	case udtPWRF:
	case udtRF:
		this->FrameSize[0] = this->DataDescriptor->h;
		this->FrameSize[1] = this->DataDescriptor->w;  
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

