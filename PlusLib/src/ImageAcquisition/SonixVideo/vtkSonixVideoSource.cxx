/*=========================================================================

Module:    $RCSfile: vtkSonixVideoSource.cxx,v $
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
#include "vtkVideoBuffer.h"
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



vtkCxxRevisionMacro(vtkSonixVideoSource, "$Revision: 1.0$");
//vtkStandardNewMacro(vtkWin32VideoSource);
//----------------------------------------------------------------------------
// Needed when we don't use the vtkStandardNewMacro.
vtkInstantiatorNewMacro(vtkSonixVideoSource);

//----------------------------------------------------------------------------

vtkSonixVideoSource* vtkSonixVideoSource::Instance = 0;
vtkSonixVideoSourceCleanup vtkSonixVideoSource::Cleanup;


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

static const int CONNECT_RETRY=5;
static const int CONNECT_RETRY_DELAY_SEC=1.0;

//----------------------------------------------------------------------------
vtkSonixVideoSourceCleanup::vtkSonixVideoSourceCleanup()
{
}

//----------------------------------------------------------------------------
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

    this->SonixHostIP = "130.15.7.212";
    this->FrameRate = -1; // in fps
    this->FrameCount = 0;

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

    this->NumberOfOutputFrames = 1;

    this->SetFrameBufferSize(200); 
}

//----------------------------------------------------------------------------
vtkSonixVideoSource::~vtkSonixVideoSource()
{ 

    this->vtkSonixVideoSource::ReleaseSystemResources();

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
vtkSonixVideoSource* vtkSonixVideoSource::New()
{
    vtkSonixVideoSource* ret = vtkSonixVideoSource::GetInstance();
    ret->Register(NULL);
    return ret;
}

//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
// the callback function used when there is a new frame of data received
bool vtkSonixVideoSource::vtkSonixVideoSourceNewFrameCallback(void * data, int type, int sz, bool cine, int frmnum)
{    
    if(!data || !sz)
    {
        LOG_WARNING("Error: no actual frame data received"); 
        return false;
    }

    if(data)
    {
        vtkSonixVideoSource::GetInstance()->LocalInternalGrab(data, type, sz, cine, frmnum);    
    }
    return true;;
}

//----------------------------------------------------------------------------
// copy the Device Independent Bitmap from the VFW framebuffer into the
// vtkVideoSource framebuffer (don't do the unpacking yet)
PlusStatus vtkSonixVideoSource::LocalInternalGrab(void* dataPtr, int type, int sz, bool cine, int frmnum)
{
    if (this->Recording==0)
    {
      // drop the frame, we are not recording data now
      return PLUS_SUCCESS;
    }
    if ( !this->Initialized )
    {
        //LOG_ERROR("Cannot grab, the video source has not been initialized yet");
        return PLUS_FAIL; 	
    }
    //error check for data type, size
    if ((uData)type!= (uData)this->AcquisitionDataType)
    {
        LOG_ERROR( "Received data type is different than expected");
        return PLUS_FAIL;
    }

    // use the information about data type and frmnum to do cross checking that you are maintaining correct frame index, & receiving
    // expected data type
    this->FrameNumber = frmnum; 

    const int* frameSize = this->GetFrameSize(); 
    int frameBufferBitsPerPixel = this->Buffer->GetNumberOfBitsPerPixel(); 
    const int frameSizeInBytes = frameSize[0] * frameSize[1] * sizeof( PixelType ); 

    // for frame containing FC (frame count) in the beginning for data coming from cine, jump 2 bytes
    int numberOfBytesToSkip = 0; 
    if(    (type == udtBPre) || (type == udtRF) 
        ||  (type == udtMPre) || (type == udtPWRF)
        ||  (type == udtColorRF)
        )
    {
        numberOfBytesToSkip = 4;
    }

    if ( sz != frameSizeInBytes + numberOfBytesToSkip )
    {
        LOG_ERROR("Received frame size (" << sz << " bytes) doesn't match the buffer size (" << frameSizeInBytes + numberOfBytesToSkip << " bytes)!"); 
        return PLUS_FAIL; 
    }
    
    // get the pointer to actual incoming data on to a local pointer
    unsigned char *deviceDataPtr = static_cast<unsigned char*>(dataPtr);

    PlusStatus status = this->Buffer->AddItem(deviceDataPtr, this->GetUsImageOrientation(), frameSize, frameBufferBitsPerPixel, numberOfBytesToSkip, this->FrameNumber); 
    this->Modified(); 

    return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::Initialize()
{
    if (this->Initialized)
    {
        return PLUS_SUCCESS;
    }

    // Connect to device. Sometimes it just fails so try to make it more robust by retrying
    // the connection a few times.
    int connectionRetried=0;
    while (!this->Connect())
    {
      if (connectionRetried>=CONNECT_RETRY)
      {
        LOG_ERROR("Failed to connect to video device");
        return PLUS_FAIL;
      }
      connectionRetried++;
      LOG_DEBUG("Failed to connect to video device, retry ("<<connectionRetried<<")");      
      vtkAccurateTimer::Delay(CONNECT_RETRY_DELAY_SEC);
    }
    LOG_DEBUG("Successfully connected to video device");

    // update the frame buffer now just in case there is an error
    this->UpdateFrameBuffer();

    this->Initialized = 1;

    return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::Connect()
{
    // 1) connect to sonix machine.
    if(!this->ult->connect(this->SonixHostIP))
    {
        char *err = new char[256]; 
        int sz = 256;
        this->ult->getLastError(err,sz);
        LOG_ERROR("Initialize: couldn't connect to Ultrasonix at " << this->SonixHostIP << " address (error message: " << err << ")");
        this->ReleaseSystemResources();
        return PLUS_FAIL;
    }

    // 2) set the imaging mode
    if (!this->ult->selectMode(this->ImagingMode))
    {
        char *err = new char[256]; 
        int sz = 256;
        this->ult->getLastError(err,sz);
        LOG_ERROR("Initialize: couldn't select imaging mode (" << err << ")");
        this->ReleaseSystemResources();
        return PLUS_FAIL;
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
        return PLUS_FAIL;
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
        return PLUS_FAIL;
    }
    // actually request data, now that its available
    if (!this->ult->setDataToAcquire(AcquisitionDataType))
    {
        char *err = new char[256];  
        int sz = 256;
        this->ult->getLastError(err,sz);
        LOG_ERROR("Initialize: couldn't request the data aquisition type (" << err << ")");
        this->ReleaseSystemResources();
        return PLUS_FAIL;
    }

    // 4) get the data descriptor
    if (!this->ult->getDataDescriptor((uData)AcquisitionDataType, *this->DataDescriptor))
    {
        char *err = new char[256];  
        int sz = 256;
        this->ult->getLastError(err,sz);
        LOG_ERROR("Initialize: couldn't retrieve data descriptor (" << err << ")");
        this->ReleaseSystemResources();
        return PLUS_FAIL;
    }

    this->SetFrameSize( this->DataDescriptor->w, this->DataDescriptor->h); 
    this->Buffer->SetNumberOfBitsPerPixel( this->DataDescriptor->ss );


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
        return PLUS_FAIL; 
    }
    if (this->Frequency >= 0 && !this->ult->setParamValue(prmFrequency.id, this->Frequency))
#endif
    {
        char *err = new char[256];  
        int sz = 256;
        this->ult->getLastError(err,sz);
        LOG_ERROR("Initialize: couldn't set desired frequency (" << err << ")");
        this->ReleaseSystemResources();
        return PLUS_FAIL;
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
        return PLUS_FAIL; 
    }
    if (this->Depth >= 0 && !this->ult->setParamValue(prmDepth.id, this->Depth))
#endif
    {
        char *err = new char[256];  
        int sz = 256;
        this->ult->getLastError(err,sz);
        LOG_ERROR("Initialize: couldn't set desired depth (" << err << ")");
        this->ReleaseSystemResources();
        return PLUS_FAIL;
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
        return PLUS_FAIL; 
    }
    if (this->Sector >= 0 && !this->ult->setParamValue(prmSector.id, this->Sector))
#endif
    {
        char *err = new char[256];  
        int sz = 256;
        this->ult->getLastError(err,sz);
        LOG_ERROR("Initialize: couldn't set desired sector (" << err << ")");
        this->ReleaseSystemResources();
        return PLUS_FAIL;
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
        return PLUS_FAIL; 
    }
    if (this->Gain >= 0 && !this->ult->setParamValue(prmGain.id, this->Gain))
#endif 
    {
        char *err = new char[256];  
        int sz = 256;
        this->ult->getLastError(err,sz);
        LOG_ERROR("Initialize: couldn't set desired gain (" << err << ")");
        this->ReleaseSystemResources();
        return PLUS_FAIL;
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
        return PLUS_FAIL; 
    }
    if (this->DynRange >= 0 && !this->ult->setParamValue(prmDynRange.id, this->DynRange))
#endif 
    {
        char *err = new char[256];  
        int sz = 256;
        this->ult->getLastError(err,sz);
        LOG_ERROR("Initialize: couldn't set desired dyn range (" << err << ")");
        this->ReleaseSystemResources();
        return PLUS_FAIL;
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
        return PLUS_FAIL; 
    }
    if (this->Zoom >= 0 && !this->ult->setParamValue(prmZoom.id, this->Zoom))
#endif 
    {
        char *err = new char[256];  
        int sz = 256;
        this->ult->getLastError(err,sz);
        LOG_ERROR("Initialize: couldn't set desired zoom (" << err << ")");
        this->ReleaseSystemResources();
        return PLUS_FAIL;
    }

    if (!this->ult->setCompressionStatus(this->CompressionStatus))
    {
        char *err = new char[256];  
        int sz = 256;
        this->ult->getLastError(err,sz);
        LOG_ERROR("Initialize: couldn't set compression status (" << err << ")");
        this->ReleaseSystemResources();
        return PLUS_FAIL;
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

    return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::Disconnect()
{
    this->StopRecording();
    this->ult->disconnect();
    return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkSonixVideoSource::ReleaseSystemResources()
{
    this->Disconnect(); 

    this->Initialized = 0;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::Grab()
{
    LOG_ERROR("Grab is not implemented for this video source");
    return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::StartRecording()
{
    if (!this->Initialized)
    {
        LOG_ERROR("Unable to start recording: initialize the video device first!"); 
        return PLUS_FAIL;
    }

    if (!this->Recording)
    {
        this->Recording = 1;
        this->Modified();
        if(this->ult->getFreezeState())
            this->ult->toggleFreeze();
    }

    return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::StopRecording()
{
    if (this->Recording)
    {
        this->Recording = 0;
        this->Modified();

        if (!this->ult->getFreezeState())
            this->ult->toggleFreeze();
    }

    return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkSonixVideoSource::SetSonixIP(const char *SonixIP)
{
    if (SonixIP)
    {
        this->SonixHostIP = new char[256];
        sprintf(this->SonixHostIP, "%s", SonixIP);    
    }
}

//-----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::ReadConfiguration(vtkXMLDataElement* config)
{
    LOG_TRACE("vtkSonixVideoSource::ReadConfiguration"); 
    if ( config == NULL )
    {
        LOG_ERROR("Unable to configure Sonix video source! (XML data element is NULL)"); 
        return PLUS_FAIL; 
    }

    Superclass::ReadConfiguration(config); 

    const char* ipAddress = config->GetAttribute("IP"); 
    if ( ipAddress != NULL) 
    {
        this->SetSonixIP(ipAddress); 
    }
    LOG_DEBUG("Sonix Video IP: " << ipAddress); 

    int imagingMode = 0; 
    if ( config->GetScalarAttribute("ImagingMode", imagingMode)) 
    {
        this->SetImagingMode(imagingMode); 
    }

    int acquisitionDataType = 0; 
    if ( config->GetScalarAttribute("AcquisitionDataType", acquisitionDataType)) 
    {
        this->SetAcquisitionDataType(acquisitionDataType); 
    }

    int depth = -1; 
    if ( config->GetScalarAttribute("Depth", depth)) 
    {
        this->SetDepth(depth); 
    }

    int sector = -1; 
    if ( config->GetScalarAttribute("Sector", sector)) 
    {
        this->SetSector(sector); 
    }

    int gain = -1; 
    if ( config->GetScalarAttribute("Gain", gain)) 
    {
        this->SetGain(gain); 
    }

    int dynRange = -1; 
    if ( config->GetScalarAttribute("DynRange", dynRange)) 
    {
        this->SetDynRange(dynRange); 
    }

    int zoom = -1; 
    if ( config->GetScalarAttribute("Zoom", zoom)) 
    {
        this->SetZoom(zoom); 
    }

    int frequency = -1; 
    if ( config->GetScalarAttribute("Frequency", frequency)) 
    {
        this->SetFrequency(frequency); 
    }

    int compressionStatus = 0; 
    if ( config->GetScalarAttribute("CompressionStatus", compressionStatus)) 
    {
        this->SetCompressionStatus(compressionStatus); 
    }

    int timeout = 0; 
    if ( config->GetScalarAttribute("Timeout", timeout)) 
    {
        this->SetTimeout(timeout); 
    }

    return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::WriteConfiguration(vtkXMLDataElement* config)
{
    Superclass::WriteConfiguration(config); 
    LOG_ERROR("Not implemented");
    return PLUS_FAIL;
}