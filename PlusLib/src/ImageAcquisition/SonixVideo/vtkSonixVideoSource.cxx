/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

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

  this->FrameCount = 0;

  this->Frequency = -1; //in Mhz
  this->Depth = -1; //in mm
  this->Sector = -1; //in %
  this->Gain = -1; //in %
  this->DynRange = -1; //in dB
  this->Zoom = -1; //in %
  this->Timeout = -1; // in ms
  this->ConnectionSetupDelayMs = 3000; // in ms
  this->CompressionStatus = 0; // no compression by default
  this->AcquisitionDataType = udtBPost; //corresponds to type: BPost 8-bit  
  this->ImagingMode = BMode; //corresponds to BMode imaging  

  this->NumberOfOutputFrames = 1;

  this->UlteriusConnected=false;

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
#if (PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 4)
  LOG_ERROR("Minimum required Ultasonix SDK version is 5.x");
  return PLUS_FAIL; 
#endif

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
  this->Ult.setMessaging(false); // don't print messages on stdout

  // Connect to device. Sometimes it just fails so try to make it more robust by retrying
  // the connection a few times.
  int connectionTried=0;
  bool initializationCompleted=false;
  while (!initializationCompleted)
  {   
    if (this->UlteriusConnected)
    {
      // a previous connection setup attempt failed after connection has been made, so
      // disconnect before trying to connect again
      this->Ult.disconnect();
      this->UlteriusConnected=false;
    }
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

    // Connect to the imaging device
    if(!this->Ult.connect(this->SonixIP))
    {        
      LOG_ERROR("Initialize: couldn't connect to Ultrasonix at " << this->SonixIP << " address (error message: " << GetLastUlteriusError() << ")");
      continue;
    }
    this->UlteriusConnected=true;
    
    // Set the imaging mode
    if (SetImagingMode(this->ImagingMode)!=PLUS_SUCCESS) { continue; }

    // We need to wait for a little while before the mode actually gets selected
    vtkAccurateTimer::Delay(0.001*this->ConnectionSetupDelayMs); 

    // Double-check to see if the mode has actually been set
    int actualImagingMode=-1;
    if (GetImagingMode(actualImagingMode)!=PLUS_SUCCESS)
    { 
      LOG_ERROR("Initialize: Cannot check actual imaging mode");
      continue; 
    }   
    if (this->ImagingMode != actualImagingMode)
    {
      LOG_ERROR("Initialize: Requested imaging mode could not be selected (requested: "<<this->ImagingMode<<", actual: "<<actualImagingMode);
      continue;
    }

    // Set the data acquisition type
    if (SetAcquisitionDataType(this->AcquisitionDataType)!=PLUS_SUCCESS) { continue; }

    // Set frame size and pixel type
    if (!this->Ult.getDataDescriptor((uData)this->AcquisitionDataType, this->DataDescriptor))
    {
      LOG_DEBUG("Initialize: couldn't retrieve data descriptor (" << GetLastUlteriusError() << ")"); // error is reported at higher level, as it often happens that this call fails but after a few attempts it succeeds
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
      continue;
    }
    this->SetFrameSize( this->DataDescriptor.w, this->DataDescriptor.h); 

    // Set up imaging parameters
    // Parameter value <0 means that the parameter should be kept unchanged
    if (this->Frequency>=0 && SetFrequency(this->Frequency)!=PLUS_SUCCESS) { continue; }
    if (this->Depth>=0 && SetDepth(this->Depth)!=PLUS_SUCCESS) { continue; }
    if (this->Sector>=0 && SetSector(this->Sector)!=PLUS_SUCCESS) { continue; }
    if (this->Gain>=0 && SetGain(this->Gain)!=PLUS_SUCCESS) { continue; }
    if (this->DynRange>=0 && SetDynRange(this->DynRange)!=PLUS_SUCCESS) { continue; }
    if (this->Zoom>=0 && SetZoom(this->Zoom)!=PLUS_SUCCESS) { continue; }
    if (this->CompressionStatus>=0 && SetCompressionStatus(this->CompressionStatus)!=PLUS_SUCCESS) { continue; }    

    // Set callback and timeout for receiving new frames
    this->Ult.setCallback(vtkSonixVideoSourceNewFrameCallback);
    if (this->Timeout>=0 && SetTimeout(this->Timeout)!=PLUS_SUCCESS) { continue; }

    initializationCompleted=true;
  } 

  LOG_DEBUG("Successfully connected to sonix video device");
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::InternalDisconnect()
{
  this->UlteriusConnected=false;
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
      this->ImagingMode=BMode; 
    }
    else if (STRCASECMP(imagingMode, "RfMode")==0)
    {
      LOG_DEBUG("Imaging mode set: RfMode"); 
      this->ImagingMode=RfMode; 
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
      this->AcquisitionDataType=udtBPost; 
    }
    else if (STRCASECMP(acquisitionDataType, "RF")==0)
    {
      LOG_DEBUG("AcquisitionDataType set: RF"); 
      this->AcquisitionDataType=udtRF; 
    }
    else
    {
      LOG_ERROR("Unsupported AcquisitionDataType requested: "<<acquisitionDataType);
    }
  }

  int depth = -1; 
  if ( imageAcquisitionConfig->GetScalarAttribute("Depth", depth)) 
  {
    this->Depth=depth; 
  }

  int sector = -1; 
  if ( imageAcquisitionConfig->GetScalarAttribute("Sector", sector)) 
  {
    this->Sector=sector; 
  }

  int gain = -1; 
  if ( imageAcquisitionConfig->GetScalarAttribute("Gain", gain)) 
  {
    this->Gain=gain; 
  }

  int dynRange = -1; 
  if ( imageAcquisitionConfig->GetScalarAttribute("DynRange", dynRange)) 
  {
    this->DynRange=dynRange; 
  }

  int zoom = -1; 
  if ( imageAcquisitionConfig->GetScalarAttribute("Zoom", zoom)) 
  {
    this->Zoom=zoom; 
  }

  int frequency = -1; 
  if ( imageAcquisitionConfig->GetScalarAttribute("Frequency", frequency)) 
  {
    this->Frequency=frequency; 
  }

  int compressionStatus = 0; 
  if ( imageAcquisitionConfig->GetScalarAttribute("CompressionStatus", compressionStatus)) 
  {
    this->CompressionStatus=compressionStatus; 
  }

  int timeout = 0; 
  if ( imageAcquisitionConfig->GetScalarAttribute("Timeout", timeout)) 
  {
    this->Timeout=timeout; 
  }

  double connectionSetupDelayMs=3.0; 
  if ( imageAcquisitionConfig->GetScalarAttribute("ConnectionSetupDelayMs", connectionSetupDelayMs)) 
  {
    this->ConnectionSetupDelayMs=connectionSetupDelayMs; 
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::WriteConfiguration(vtkXMLDataElement* config)
{
  // TODO: implement this
  return Superclass::WriteConfiguration(config); 
}

//----------------------------------------------------------------------------
std::string vtkSonixVideoSource::GetLastUlteriusError()
{
  const unsigned int MAX_ULTERIUS_ERROR_MSG_LENGTH=256;
  char err[MAX_ULTERIUS_ERROR_MSG_LENGTH+1];
  err[MAX_ULTERIUS_ERROR_MSG_LENGTH]=0; // make sure the string is null-terminated
  this->Ult.getLastError(err,MAX_ULTERIUS_ERROR_MSG_LENGTH);

  return err; 
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::SetParamValue(char* paramId, int paramValue, int &validatedParamValue)
{
  if (!this->UlteriusConnected)
  {
    // Connection has not been established yet. Parameter value will be set upon connection.
    validatedParamValue=paramValue;
    return PLUS_SUCCESS;
  }
  if (!this->Ult.setParamValue(paramId, paramValue))
  {
    LOG_ERROR("vtkSonixVideoSource::SetParamValue failed (paramId="<<paramId<<", paramValue="<<paramValue<<") "<<GetLastUlteriusError());
    return PLUS_FAIL;
  }
  validatedParamValue=paramValue;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::GetParamValue(char* paramId, int& paramValue, int &validatedParamValue)
{
  if (!this->UlteriusConnected)
  {
    // Connection has not been established yet. Returned the cached value.
    paramValue=validatedParamValue;
    return PLUS_SUCCESS;
  }
  paramValue=-1;
  if (!this->Ult.getParamValue(paramId, paramValue))
  {
    LOG_ERROR("vtkSonixVideoSource::GetParamValue failed (paramId="<<paramId<<", paramValue="<<paramValue<<") "<<GetLastUlteriusError());
    return PLUS_FAIL;
  }
  validatedParamValue=paramValue;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::SetFrequency(int aFrequency)
{
  return SetParamValue("b-freq", aFrequency, this->Frequency);
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::GetFrequency(int& aFrequency)
{
  return GetParamValue("b-freq", aFrequency, this->Frequency);
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::SetDepth(int aDepth)
{
  return SetParamValue("b-depth", aDepth, this->Depth);
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::GetDepth(int& aDepth)
{
  return GetParamValue("b-depth", aDepth, this->Depth);
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::SetGain(int aGain)
{
  return SetParamValue("b-gain", aGain, this->Gain);
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::GetGain(int& aGain)
{
  return GetParamValue("b-gain", aGain, this->Gain);
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::SetDynRange(int aDynRange)
{
  return SetParamValue("b-dynamic range", aDynRange, this->DynRange);
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::GetDynRange(int& aDynRange)
{
  return GetParamValue("b-dynamic range", aDynRange, this->DynRange);
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::SetZoom(int aZoom)
{
  return SetParamValue("b-initial zoom", aZoom, this->Zoom);
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::GetZoom(int& aZoom)
{
  return GetParamValue("b-initial zoom", aZoom, this->Zoom);
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::SetSector(int aSector)
{
  return SetParamValue("sector", aSector, this->Sector);
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::GetSector(int& aSector)
{
  return GetParamValue("sector", aSector, this->Sector);
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::SetCompressionStatus(int aCompressionStatus)
{
  if (!this->UlteriusConnected)
  {
    // Connection has not been established yet. Parameter value will be set upon connection.
    this->CompressionStatus=aCompressionStatus;
    return PLUS_SUCCESS;
  }
  if (!this->Ult.setCompressionStatus(aCompressionStatus))
  {
    LOG_ERROR("vtkSonixVideoSource::SetCompressionStatus failed: (compressionStatus="<<aCompressionStatus<<") "<< GetLastUlteriusError());
    return PLUS_FAIL;
  }
  this->CompressionStatus=aCompressionStatus;
  return PLUS_SUCCESS;   
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::GetCompressionStatus(int& aCompressionStatus)
{
  if (!this->UlteriusConnected)
  {
    // Connection has not been established yet. Parameter value will be set upon connection.
    aCompressionStatus=this->CompressionStatus;
    return PLUS_SUCCESS;
  }
  aCompressionStatus=this->Ult.getCompressionStatus();
  this->CompressionStatus=aCompressionStatus;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::SetTimeout(int aTimeout)
{
  if (!this->UlteriusConnected)
  {
    // Connection has not been established yet. Parameter value will be set upon connection.
    this->Timeout=aTimeout;
    return PLUS_SUCCESS;
  }
  if (aTimeout<0)
  {
    LOG_ERROR("vtkSonixVideoSource::setTimeout failed: (aTimeout="<<aTimeout<<") "<< GetLastUlteriusError());
    return PLUS_FAIL;
  }
  this->Ult.setTimeout(aTimeout);
  this->Timeout=aTimeout;
  return PLUS_SUCCESS;   
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::SetAcquisitionDataType(int aAcquisitionDataType)
{
  if (!this->UlteriusConnected)
  {
    // Connection has not been established yet. Parameter value will be set upon connection.
    this->AcquisitionDataType=aAcquisitionDataType;
    return PLUS_SUCCESS;
  }
  // Check if the desired acquisition type is actually available on desired imaging mode
  if (!this->Ult.isDataAvailable((uData)(aAcquisitionDataType)))
  {
    LOG_ERROR("vtkSonixVideoSource::SetAcquisitionDataType failed: Requested the data aquisition type not available for selected imaging mode(" << GetLastUlteriusError() << ")");
    return PLUS_FAIL;
  }
  // actually request data, now that its available
  if (!this->Ult.setDataToAcquire(aAcquisitionDataType))
  {
    LOG_ERROR("vtkSonixVideoSource::SetAcquisitionDataType failed: couldn't request the data aquisition type "<<aAcquisitionDataType<<", " << GetLastUlteriusError());
    return PLUS_FAIL;
  }
  this->AcquisitionDataType=aAcquisitionDataType;
  return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::GetAcquisitionDataType(int &acquisitionDataType)
{
  if (!this->UlteriusConnected)
  {
    // Connection has not been established yet. Parameter value will be set upon connection.
    acquisitionDataType=this->AcquisitionDataType;
    return PLUS_SUCCESS;
  }
  acquisitionDataType=this->Ult.getDataToAcquire();
  this->AcquisitionDataType=acquisitionDataType;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::SetImagingMode(int mode)
{
  if (!this->UlteriusConnected)
  {
    // Connection has not been established yet. Parameter value will be set upon connection.
    this->ImagingMode=mode;
    return PLUS_SUCCESS;
  }
  if (!this->Ult.selectMode(mode))
  {
    LOG_ERROR("vtkSonixVideoSource::SetImagingMode failed: couldn't select imaging mode " << mode << ", " << GetLastUlteriusError());
    return PLUS_FAIL;
  }
  this->ImagingMode=mode;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::GetImagingMode(int & mode)
{
  if (!this->UlteriusConnected)
  {
    // Connection has not been established yet. Parameter value will be set upon connection.
    mode=this->ImagingMode;
    return PLUS_SUCCESS;
  }
  mode=this->Ult.getActiveImagingMode();
  this->ImagingMode=mode;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::PrintListOfImagingParameters()
{
  if (!this->UlteriusConnected)
  {
    LOG_ERROR("vtkSonixVideoSource::PrintListOfParameters failed: not connected");
    return PLUS_FAIL;
  }
  uParam prmDescriptor; 
  LOG_INFO("Index; Id; Name; Source; Type; Unit");
  for (int paramId=0; this->Ult.getParam(paramId, prmDescriptor); paramId++)
  {
    LOG_INFO(paramId << "; " << prmDescriptor.id << "; " << prmDescriptor.name << "; " << prmDescriptor.source 
      << "; " << prmDescriptor.type << "; " << prmDescriptor.unit);
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::GetDisplayedFrameRate(int &aFrameRate)
{
  if (!this->UlteriusConnected)
  {
    LOG_ERROR("vtkSonixVideoSource::GetDisplayedFrameRate failed: not connected");
    return PLUS_FAIL;
  }
  
  if ( !this->Ult.getParamValue("frame rate", aFrameRate) )
  {
	 LOG_ERROR("vtkSonixVideoSource::GetDisplayedFrameRate failed: cannot retrieve displayed frame rate.");
	 return PLUS_FAIL;
  }
  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::SetRFDecimation(int decimation)
{
  if (!this->UlteriusConnected)
  {
    LOG_ERROR("vtkSonixVideoSource::SetRFDecimation failed: not connected");
    return PLUS_FAIL;
  }
  if ( !this->Ult.setParamValue("rf-rf decimation", decimation) )
  {
	 LOG_ERROR("vtkSonixVideoSource::SetRFDecimation failed: cannot set decimation value.");
	 return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}
