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
#if PLUS_ULTRASONIX_SDK_MAJOR_VERSION == 1
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
    this->SonixIP = 0;

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
    this->AcquisitionDataType = udtBPost; //corresponds to type: BPost 8-bit  
    this->ImagingMode = BMode; //corresponds to BMode imaging  

    this->NumberOfOutputFrames = 1;

    this->SetFrameBufferSize(200); 
}

//----------------------------------------------------------------------------
vtkSonixVideoSource::~vtkSonixVideoSource()
{ 
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
    if(data==NULL || sz==0)
    {
        LOG_DEBUG("Error: no actual frame data received"); 
        return false;
    }

    vtkSonixVideoSource::GetInstance()->AddFrameToBuffer(data, type, sz, cine, frmnum);    

    return true;;
}

//----------------------------------------------------------------------------
// copy the Device Independent Bitmap from the VFW framebuffer into the
// vtkVideoSource framebuffer (don't do the unpacking yet)
PlusStatus vtkSonixVideoSource::AddFrameToBuffer(void* dataPtr, int type, int sz, bool cine, int frmnum)
{
    if (!this->Recording)
    {
      // drop the frame, we are not recording data now
      return PLUS_SUCCESS;
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
    int frameBufferBytesPerPixel = this->Buffer->GetNumberOfBytesPerPixel(); 
    const int frameSizeInBytes = frameSize[0] * frameSize[1] * frameBufferBytesPerPixel; 

    // for frame containing FC (frame count) in the beginning for data coming from cine, jump 2 bytes
    int numberOfBytesToSkip = 0; 
#if (PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 5) || (PLUS_ULTRASONIX_SDK_MAJOR_VERSION == 5 && PLUS_ULTRASONIX_SDK_MINOR_VERSION < 7)
    if(    (type == udtBPre) || (type == udtRF) 
        ||  (type == udtMPre) || (type == udtPWRF)
        ||  (type == udtColorRF)
        )
    {
        numberOfBytesToSkip = 4;
    }
#else 
	// It's Ulterius 5.7 or newer.
	// RF images don't have a 4-byte header. It's possible that none of the types have the 4-byte header anymore.
	if(    (type == udtBPre) 
        ||  (type == udtMPre) || (type == udtPWRF)
        ||  (type == udtColorRF)
        )
    {
        numberOfBytesToSkip = 4;
    }
#endif

    PlusCommon::ITKScalarPixelType pixelType=itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;    
    switch (type)
    {
    case udtBPost:
      pixelType=itk::ImageIOBase::UCHAR;
      break;
    case udtRF:
      pixelType=itk::ImageIOBase::SHORT;
      break;
    default:
      LOG_ERROR("Uknown pixel type");
    }

    if ( sz != frameSizeInBytes + numberOfBytesToSkip )
    {
        LOG_ERROR("Received frame size (" << sz << " bytes) doesn't match the buffer size (" << frameSizeInBytes + numberOfBytesToSkip << " bytes)!"); 
        return PLUS_FAIL; 
    }
    
    // get the pointer to actual incoming data on to a local pointer
    unsigned char *deviceDataPtr = static_cast<unsigned char*>(dataPtr);
    
    PlusStatus status = this->Buffer->AddItem(deviceDataPtr, this->GetUsImageOrientation(), frameSize, pixelType, numberOfBytesToSkip, this->FrameNumber); 
    this->Modified(); 

    return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::InternalConnect()
{

  // Connect to device. Sometimes it just fails so try to make it more robust by retrying
  // the connection a few times.
  int connectionTried=0;
  bool connected=false;
  while (!connected)
  {
    if (connectionTried>0)
    {
      // this is a connection retry attempt
      if (connectionTried>=CONNECT_RETRY)
      {
        LOG_ERROR("Failed to connect to sonix video device");
        return PLUS_FAIL;
      }  
      LOG_DEBUG("Failed to connect to sonix video device, retry ("<<connectionTried<<")");      
      vtkAccurateTimer::Delay(CONNECT_RETRY_DELAY_SEC);
    }
    connectionTried++;

    if (this->SonixIP==NULL)
    {
      LOG_ERROR("Sonix host IP address is undefined");
      continue;
    }

    // 1) connect to sonix machine.
    if(!this->Ult.connect(this->SonixIP))
    {        
      LOG_ERROR("Initialize: couldn't connect to Ultrasonix at " << this->SonixIP << " address (error message: " << GetLastUlteriusError() << ")");
      continue;
    }

    // 2) set the imaging mode
    if (!this->Ult.selectMode(this->ImagingMode))
    {
      LOG_ERROR("Initialize: couldn't select imaging mode (" << GetLastUlteriusError() << ")");
      this->Ult.disconnect();
      continue;
    }

    // do we need to wait for a little while before the mode actually gets selected??
    // like a thread sleep or something??
    vtkAccurateTimer::Delay(2); 

    // double-check to see if the mode has actually been set
    if (this->ImagingMode != this->Ult.getActiveImagingMode())
    {
      LOG_ERROR("Initialize: Requested imaging mode could not be selected(" << GetLastUlteriusError() << ")");
      this->Ult.disconnect();
      continue;
    }

    // 3) set the data acquisition type
    // check if the desired acquisition type is actually available on desired imaging mode
    if (!this->Ult.isDataAvailable((uData)(AcquisitionDataType)))
    {
      LOG_ERROR("Initialize: Requested the data aquisition type not available for selected imaging mode(" << GetLastUlteriusError() << ")");
      this->Ult.disconnect();
      continue;
    }
    // actually request data, now that its available
    if (!this->Ult.setDataToAcquire(AcquisitionDataType))
    {
      LOG_ERROR("Initialize: couldn't request the data aquisition type (" << GetLastUlteriusError() << ")");
      this->Ult.disconnect();
      continue;
    }

    // 4) get the data descriptor
    if (!this->Ult.getDataDescriptor((uData)AcquisitionDataType, this->DataDescriptor))
    {
      LOG_DEBUG("Initialize: couldn't retrieve data descriptor (" << GetLastUlteriusError() << ")"); // error is reported at higher level, as it often happens that this call fails but after a few attempts it succeeds
      this->Ult.disconnect();
      continue;
    }
    
    switch (this->DataDescriptor.ss)
    {
    case 8:
      this->SetPixelType( itk::ImageIOBase::UCHAR );
      break;
    case 16:
      this->SetPixelType( itk::ImageIOBase::SHORT );
      // Swap w/h: in case of RF image acquisition the DataDescriptor.h is the width and the DataDescriptor.w is the height
      {
        int tmp=this->DataDescriptor.h;
        this->DataDescriptor.h=this->DataDescriptor.w;
        this->DataDescriptor.w=tmp;
      }
      break;
    default:
      LOG_ERROR("Unsupported Ulterius bit depth: "<<this->DataDescriptor.ss);
      this->Ult.disconnect();
      continue;
    }
    
    this->SetFrameSize( this->DataDescriptor.w, this->DataDescriptor.h); 

    // Parameter setting doesn't work with Ulterius-2.x
#if PLUS_ULTRASONIX_SDK_MAJOR_VERSION != 2
    // 6) set parameters, currently: frequency, frame rate, depth

#if PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 2
    if (this->Frequency >= 0 && !this->Ult.setParamValue(VARID_FREQ, this->Frequency))
#else 
    uParam prmFrequency; 
    if ( !this->Ult.getParam(VARID_FREQ, prmFrequency) )
    {
      LOG_ERROR("Unable to get parameter: " << GetLastUlteriusError()); 
      this->Ult.disconnect();
      continue;
    }
    if (this->Frequency >= 0 && !this->Ult.setParamValue(prmFrequency.id, this->Frequency))
#endif
    {
      LOG_ERROR("Initialize: couldn't set desired frequency (" << GetLastUlteriusError() << ")");
      this->Ult.disconnect();
      continue;
    }

#if PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 2
    if (this->Depth >= 0 && !this->Ult.setParamValue(VARID_DEPTH, this->Depth))
#else
    uParam prmDepth; 
    if ( ! this->Ult.getParam(VARID_DEPTH, prmDepth) )
    {
      LOG_ERROR("Unable to get parameter: " << GetLastUlteriusError()); 
      this->Ult.disconnect();
      continue;
    }
    if (this->Depth >= 0 && !this->Ult.setParamValue(prmDepth.id, this->Depth))
#endif
    {
      LOG_ERROR("Initialize: couldn't set desired depth (" << GetLastUlteriusError() << ")");
      this->Ult.disconnect();
      continue;
    }

#if PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 2
    if (this->Sector >= 0 && !this->Ult.setParamValue(VARID_SECTOR, this->Sector))
#else
    uParam prmSector; 
    if ( !this->Ult.getParam(VARID_SECTOR, prmSector) )
    {
      LOG_ERROR("Unable to get parameter: " << GetLastUlteriusError()); 
      this->Ult.disconnect();
      continue;
    }
    if (this->Sector >= 0 && !this->Ult.setParamValue(prmSector.id, this->Sector))
#endif
    {
      LOG_ERROR("Initialize: couldn't set desired sector (" << GetLastUlteriusError() << ")");
      this->Ult.disconnect();
      continue;
    }

#if PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 2
    if (this->Gain >= 0 && !this->Ult.setParamValue(VARID_GAIN, this->Gain))
#else
    uParam prmGain; 
    if ( !this->Ult.getParam(VARID_GAIN, prmGain) )
    {
      LOG_ERROR("Unable to get parameter: " << GetLastUlteriusError()); 
      this->Ult.disconnect();
      continue;
    }
    if (this->Gain >= 0 && !this->Ult.setParamValue(prmGain.id, this->Gain))
#endif 
    {
      LOG_ERROR("Initialize: couldn't set desired gain (" << GetLastUlteriusError() << ")");
      this->Ult.disconnect();
      continue;
    }

#if PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 2 
    if (this->DynRange >= 0 && !this->Ult.setParamValue(VARID_DYNRANGE, this->DynRange))
#else
    uParam prmDynRange; 
    if ( !this->Ult.getParam(VARID_DYNRANGE, prmDynRange) )
    {
      LOG_ERROR("Unable to get parameter: " << GetLastUlteriusError() ); 
      this->Ult.disconnect();
      continue;
    }
    if (this->DynRange >= 0 && !this->Ult.setParamValue(prmDynRange.id, this->DynRange))
#endif 
    {
      LOG_ERROR("Initialize: couldn't set desired dyn range (" << GetLastUlteriusError() << ")");
      this->Ult.disconnect();
      continue;
    }

#if PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 2 
    if (this->Zoom >= 0 && !this->Ult.setParamValue(VARID_ZOOM, this->Zoom))
#else
    uParam prmZoom; 
    if ( !this->Ult.getParam(VARID_ZOOM, prmZoom) )
    {
      LOG_ERROR("Unable to get parameter: " << GetLastUlteriusError() ); 
      this->Ult.disconnect();
      continue;
    }
    if (this->Zoom >= 0 && !this->Ult.setParamValue(prmZoom.id, this->Zoom))
#endif 
    {
      LOG_ERROR("Initialize: couldn't set desired zoom (" << GetLastUlteriusError() << ")");
      this->Ult.disconnect();
      continue;
    }

    if (!this->Ult.setCompressionStatus(this->CompressionStatus))
    {
      LOG_ERROR("Initialize: couldn't set compression status (" << GetLastUlteriusError() << ")");
      this->Ult.disconnect();
      continue;
    }

    // set callback for receiving new frames
    this->Ult.setCallback(vtkSonixVideoSourceNewFrameCallback);

    // Do not change the current settings if it's not set 
    if ( this->Timeout > 0 )
    {
      this->Ult.setTimeout(this->Timeout); 
    }
#else
    LOG_WARNING("Ultrasound imaging parameter setting is not supported with Ulterius-2.x");
#endif

    connected=true;
  } 

  LOG_DEBUG("Successfully connected to sonix video device");
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::InternalDisconnect()
{
  this->Ult.disconnect();
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::InternalStartRecording()
{
  // Unfreeze
  if(this->Ult.getFreezeState())
  {
    this->Ult.toggleFreeze();
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::InternalStopRecording()
{
  // Freeze
  if (!this->Ult.getFreezeState())
  {
    this->Ult.toggleFreeze();
  }

  return PLUS_SUCCESS;
}

PlusStatus vtkSonixVideoSource::SetAcquisitionDataType(int acquisitionDataType)
{
  if (acquisitionDataType==this->AcquisitionDataType)
  {
    // no change
    return PLUS_SUCCESS;
  }
  switch (acquisitionDataType)
  {
  case udtBPost:
    // post-processed data, 8-bit unsigned
    GetBuffer()->SetPixelType(itk::ImageIOBase::UCHAR);
    break;
  case udtRF:
    // RF data, 16-bit
    GetBuffer()->SetPixelType(itk::ImageIOBase::SHORT);
    break;
  default:
    LOG_ERROR("Unsupported AcquisitionDataType requested: "<<acquisitionDataType);
    return PLUS_FAIL;
  }
  this->AcquisitionDataType=acquisitionDataType;
  return PLUS_SUCCESS;
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

	vtkSmartPointer<vtkXMLDataElement> dataCollectionConfig = config->FindNestedElementWithName("USDataCollection");
	if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find USDataCollection element in XML tree!");
		return PLUS_FAIL;
	}

  vtkSmartPointer<vtkXMLDataElement> imageAcquisitionConfig = dataCollectionConfig->FindNestedElementWithName("ImageAcquisition"); 
  if (imageAcquisitionConfig == NULL) 
  {
    LOG_ERROR("Unable to find ImageAcquisition element in configuration XML structure!");
    return PLUS_FAIL;
  }

  const char* ipAddress = imageAcquisitionConfig->GetAttribute("IP"); 
  if ( ipAddress != NULL) 
  {
      this->SetSonixIP(ipAddress); 
  }
  LOG_DEBUG("Sonix Video IP: " << ipAddress); 

  const char* imagingMode = imageAcquisitionConfig->GetAttribute("ImagingMode"); 
  if ( imagingMode != NULL) 
  {
    if (STRCASECMP(imagingMode, "BMode")==0)
    {
      LOG_DEBUG("Imaging mode set: BMode"); 
      this->SetImagingMode(BMode); 
    }
    else if (STRCASECMP(imagingMode, "RfMode")==0)
    {
      LOG_DEBUG("Imaging mode set: RfMode"); 
      this->SetImagingMode(RfMode); 
    }
    else
    {
      LOG_ERROR("Unsupported ImagingMode requested: "<<imagingMode);
    }
  }  
  const char* acquisitionDataType = imageAcquisitionConfig->GetAttribute("AcquisitionDataType"); 
  if ( acquisitionDataType != NULL) 
  {
    if (STRCASECMP(acquisitionDataType, "BPost")==0)
    {
      LOG_DEBUG("AcquisitionDataType set: BPost"); 
      this->SetAcquisitionDataType(udtBPost); 
    }
    else if (STRCASECMP(acquisitionDataType, "RF")==0)
    {
      LOG_DEBUG("AcquisitionDataType set: RF"); 
      this->SetAcquisitionDataType(udtRF); 
    }
    else
    {
      LOG_ERROR("Unsupported AcquisitionDataType requested: "<<acquisitionDataType);
    }
  }

  int depth = -1; 
  if ( imageAcquisitionConfig->GetScalarAttribute("Depth", depth)) 
  {
      this->SetDepth(depth); 
  }

  int sector = -1; 
  if ( imageAcquisitionConfig->GetScalarAttribute("Sector", sector)) 
  {
      this->SetSector(sector); 
  }

  int gain = -1; 
  if ( imageAcquisitionConfig->GetScalarAttribute("Gain", gain)) 
  {
      this->SetGain(gain); 
  }

  int dynRange = -1; 
  if ( imageAcquisitionConfig->GetScalarAttribute("DynRange", dynRange)) 
  {
      this->SetDynRange(dynRange); 
  }

  int zoom = -1; 
  if ( imageAcquisitionConfig->GetScalarAttribute("Zoom", zoom)) 
  {
      this->SetZoom(zoom); 
  }

  int frequency = -1; 
  if ( imageAcquisitionConfig->GetScalarAttribute("Frequency", frequency)) 
  {
      this->SetFrequency(frequency); 
  }

  int compressionStatus = 0; 
  if ( imageAcquisitionConfig->GetScalarAttribute("CompressionStatus", compressionStatus)) 
  {
      this->SetCompressionStatus(compressionStatus); 
  }

  int timeout = 0; 
  if ( imageAcquisitionConfig->GetScalarAttribute("Timeout", timeout)) 
  {
      this->SetTimeout(timeout); 
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::WriteConfiguration(vtkXMLDataElement* config)
{
  // TODO: implement this
    return Superclass::WriteConfiguration(config); 
}

std::string vtkSonixVideoSource::GetLastUlteriusError()
{
  const unsigned int MAX_ULTERIUS_ERROR_MSG_LENGTH=256;
  char err[MAX_ULTERIUS_ERROR_MSG_LENGTH+1];
  err[MAX_ULTERIUS_ERROR_MSG_LENGTH]=0; // make sure the string is null-terminated
  this->Ult.getLastError(err,MAX_ULTERIUS_ERROR_MSG_LENGTH);

  return err; 
}
