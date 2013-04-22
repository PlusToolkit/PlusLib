/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
The following copyright notice is applicable to parts of this file:
Copyright (c) 2008, Queen's University, Kingston, Ontario, Canada
All rights reserved.
Authors include: Danielle Pace
(Robarts Research Institute and The University of Western Ontario)
Siddharth Vikal (Queen's University, Kingston, Ontario, Canada)
=========================================================================*/  

#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMultiThreader.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusBuffer.h"
#include "vtkSonixVideoSource.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTimerLog.h"
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
: SonixIP(NULL)
{
  this->Reset();
}

//----------------------------------------------------------------------------
vtkSonixVideoSource::~vtkSonixVideoSource()
{ 
  this->SetSonixIP(NULL);
}

//----------------------------------------------------------------------------
// Up the reference count so it behaves like New
vtkSonixVideoSource* vtkSonixVideoSource::New()
{
  vtkSonixVideoSource* ret = vtkSonixVideoSource::GetInstance();
  ret->Reset();
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
std::string vtkSonixVideoSource::GetSdkVersion()
{
  std::ostringstream version; 
  version << "UltrasonixSDK-" << PLUS_ULTRASONIX_SDK_MAJOR_VERSION << "." << PLUS_ULTRASONIX_SDK_MINOR_VERSION << "." << PLUS_ULTRASONIX_SDK_PATCH_VERSION; 
  return version.str(); 
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
  if(data == NULL || sz == 0)
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

  vtkPlusDataSource* aSource;
  PlusCommon::ITKScalarPixelType pixelType = itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;    
  US_IMAGE_TYPE imgType = US_IMG_TYPE_XX;    

  if( (uData)type == udtBPost && this->GetVideoSourceByPortName(vtkPlusDevice::BMODE_PORT_NAME, aSource) == PLUS_SUCCESS )
  {
    pixelType = itk::ImageIOBase::UCHAR;
    imgType = US_IMG_BRIGHTNESS;
  }
  else if( (uData)type == udtRF && this->GetVideoSourceByPortName(vtkPlusDevice::RFMODE_PORT_NAME, aSource) == PLUS_SUCCESS )
  {
    pixelType = itk::ImageIOBase::SHORT;
    imgType = US_IMG_RF_I_LINE_Q_LINE;
  }
  else
  {
    LOG_ERROR("Received data type \'" << type << "\' is unsupported. Please report this to the PLUS team.");
    return PLUS_FAIL;
  }

  // use the information about data type and frmnum to do cross checking that you are maintaining correct frame index, & receiving
  // expected data type
  this->FrameNumber = frmnum; 

  int frameSize[2] = {0,0};
  aSource->GetBuffer()->GetFrameSize(frameSize);
  int frameBufferBytesPerPixel = aSource->GetBuffer()->GetNumberOfBytesPerPixel(); 
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

  if ( sz != frameSizeInBytes + numberOfBytesToSkip )
  {
    LOG_ERROR("Received frame size (" << sz << " bytes) doesn't match the buffer size (" << frameSizeInBytes + numberOfBytesToSkip << " bytes)!"); 
    return PLUS_FAIL; 
  }

  // get the pointer to actual incoming data on to a local pointer
  unsigned char *deviceDataPtr = static_cast<unsigned char*>(dataPtr);

  PlusStatus status = aSource->GetBuffer()->AddItem(deviceDataPtr, aSource->GetPortImageOrientation(), frameSize, pixelType, imgType, numberOfBytesToSkip, this->FrameNumber); 
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
      this->Ult.setDataToAcquire(0); // without this Ulterius 5.x may crash
      this->Ult.disconnect();
      this->UlteriusConnected=false;
    }
    if (connectionTried > 0)
    {
      // this is a connection retry attempt
      if (connectionTried >= CONNECT_RETRY)
      {
        LOG_ERROR("Failed to connect to sonix video device");
        return PLUS_FAIL;
      }  
      LOG_DEBUG("Failed to connect to sonix video device, retry ("<<connectionTried<<")");      
      vtkAccurateTimer::Delay(CONNECT_RETRY_DELAY_SEC);
    }
    connectionTried++;

    if (this->SonixIP == NULL)
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
    if (SetImagingMode(this->ImagingMode) != PLUS_SUCCESS) { continue; }

    // We need to wait for a little while before the mode actually gets selected
    vtkAccurateTimer::Delay(0.001*this->ConnectionSetupDelayMs); 

    // Double-check to see if the mode has actually been set
    int actualImagingMode=-1;
    if (GetImagingMode(actualImagingMode) != PLUS_SUCCESS)
    { 
      LOG_ERROR("Initialize: Cannot check actual imaging mode");
      continue; 
    }   
    if (this->ImagingMode != actualImagingMode)
    {
      LOG_ERROR("Initialize: Requested imaging mode could not be selected (requested: "<<this->ImagingMode<<", actual: "<<actualImagingMode);
      continue;
    }

#if (PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 6) 
    if ( this->ImagingMode == RfMode )
    {
      if ( this->SetRfAcquisitionMode(this->RfAcquisitionMode) != PLUS_SUCCESS )
      {
        LOG_ERROR("Initialize: Cannot set RF acquisition mode");
        continue; 
      }  
      RfAcquisitionModeType actualRfAcquisitionMode = RF_UNKNOWN; 
      if (GetRfAcquisitionMode(actualRfAcquisitionMode)!=PLUS_SUCCESS)
      { 
        LOG_ERROR("Initialize: Cannot check actual RF acquisition mode");
        continue; 
      }  
      if (this->RfAcquisitionMode != actualRfAcquisitionMode)
      {
        LOG_ERROR("Initialize: Requested RF acquisition mode could not be selected (requested: "<<this->RfAcquisitionMode<<", actual: "<<actualRfAcquisitionMode);
        continue;
      }
    }
#else
    LOG_ERROR("RF acquisition mode is not supported on Ultrasonix SDK 6.x and above"); // see https://www.assembla.com/spaces/plus/tickets/489-add-rf-image-acquisition-support-on-ulterius-6-x
    continue;
#endif

    if( this->HasDataType(udtBPost) )
    {
      if( this->ConfigureVideoSource(udtBPost) != PLUS_SUCCESS )
      {
        LOG_ERROR("Unable to configure B-mode video source.");
        continue;
      }
    }

    if( this->HasDataType(udtRF) )
    {
      if( this->ConfigureVideoSource(udtRF) != PLUS_SUCCESS )
      {
        LOG_ERROR("Unable to configure Rf-mode video source.");
        continue;
      }
    }

    // Actually request data, now that its available
    if ( !this->Ult.setDataToAcquire(this->AcquisitionDataType) )
    {
      LOG_ERROR("Setting AcquisitionDataType failed: couldn't request the data aquisition type " << this->AcquisitionDataType << ", " << GetLastUlteriusError());
      return PLUS_FAIL;
    }

    // Set up imaging parameters
    // Parameter value <0 means that the parameter should be kept unchanged
    if (this->Frequency >= 0 && SetFrequency(this->Frequency) != PLUS_SUCCESS) { continue; }
    if (this->Depth >= 0 && SetDepth(this->Depth) != PLUS_SUCCESS) { continue; }
    if (this->Sector >= 0 && SetSector(this->Sector) != PLUS_SUCCESS) { continue; }
    if (this->Gain >= 0 && SetGain(this->Gain) != PLUS_SUCCESS) { continue; }
    if (this->DynRange >= 0 && SetDynRange(this->DynRange) != PLUS_SUCCESS) { continue; }
    if (this->Zoom >= 0 && SetZoom(this->Zoom) != PLUS_SUCCESS) { continue; }
    if (this->CompressionStatus >= 0 && SetCompressionStatus(this->CompressionStatus) != PLUS_SUCCESS) { continue; }    
    if ( this->SoundVelocity > 0 && this->SetParamValue( "soundvelocity", this->SoundVelocity, this->SoundVelocity ) != PLUS_SUCCESS )
    {
      continue;
    }

    if (this->AcquisitionRate<=0)
    {
      // AcquisitionRate has not been specified, set it to match the frame rate
      int aFrameRate=10;
      if ( this->Ult.getParamValue("frame rate", aFrameRate) )
      {
        this->AcquisitionRate=aFrameRate;        
      }
    }

    Ult.setSharedMemoryStatus( this->SharedMemoryStatus );

    // Set callback and timeout for receiving new frames
    this->Ult.setCallback(vtkSonixVideoSourceNewFrameCallback);
    if (this->Timeout >= 0 && SetTimeout(this->Timeout) != PLUS_SUCCESS)
    {
      continue;
    }

    initializationCompleted = true;
  } 

  LOG_DEBUG("Successfully connected to sonix video device");
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::InternalDisconnect()
{
  this->UlteriusConnected=false;
  this->Ult.setDataToAcquire(0); // without this Ulterius 5.x may crash
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

  vtkXMLDataElement* deviceConfig = this->FindThisDeviceElement(config);
  if (deviceConfig == NULL) 
  {
    LOG_ERROR("Unable to find ImageAcquisition element in configuration XML structure!");
    return PLUS_FAIL;
  }

  const char* ipAddress = deviceConfig->GetAttribute("IP"); 
  if ( ipAddress != NULL) 
  {
    this->SetSonixIP(ipAddress); 
    LOG_DEBUG("Sonix Video IP: " << ipAddress); 
  }
  else
  {
    LOG_WARNING("Ultrasonix IP address is not defined. Defaulting to 127.0.0.1");
  }  

  vtkPlusDataSource* bSource(NULL);
  vtkPlusDataSource* rfSource(NULL);
  bool bMode = this->GetVideoSourceByPortName(vtkPlusDevice::BMODE_PORT_NAME, bSource) == PLUS_SUCCESS;
  bool rfMode = this->GetVideoSourceByPortName(vtkPlusDevice::RFMODE_PORT_NAME, bSource) == PLUS_SUCCESS;

  if (bMode && !rfMode)
  {
    LOG_DEBUG("Imaging mode set: BMode"); 
    this->ImagingMode = BMode; 
    this->RfAcquisitionMode = RF_ACQ_B_ONLY; // Not necessary, setting it just in case
    this->AcquisitionDataType = udtBPost;
  } 
  else if ( rfMode && !bMode )
  {
#if (PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 6)
    LOG_DEBUG("Imaging mode set: RfMode"); 
    this->ImagingMode = RfMode;
    this->RfAcquisitionMode = RF_ACQ_RF_ONLY;
    this->AcquisitionDataType = udtRF;
#else
    LOG_ERROR("RF acquisition mode is not supported on Ultrasonix SDK 6.x and above. ImagingMode is set to BMode."); // see https://www.assembla.com/spaces/plus/tickets/489-add-rf-image-acquisition-support-on-ulterius-6-x      
    this->ImagingMode = BMode; 
    this->AcquisitionDataType = udtBPost;
#endif
  }
  else if ( bMode && rfMode )
  {
#if (PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 6)
    LOG_DEBUG("Imaging mode set: BAndRfMode");
    this->ImagingMode = RfMode;
    this->RfAcquisitionMode = RF_ACQ_B_AND_RF;
    this->AcquisitionDataType = udtBPost | udtRF;
#else
    LOG_ERROR("RF acquisition mode is not supported on Ultrasonix SDK 6.x and above. ImagingMode is set to BMode."); // see https://www.assembla.com/spaces/plus/tickets/489-add-rf-image-acquisition-support-on-ulterius-6-x      
    this->ImagingMode = BMode; 
    this->AcquisitionDataType = udtBPost;
#endif
  }

  int depth = -1; 
  if ( deviceConfig->GetScalarAttribute("Depth", depth)) 
  {
    this->Depth=depth; 
  }

  int sector = -1; 
  if ( deviceConfig->GetScalarAttribute("Sector", sector)) 
  {
    this->Sector=sector; 
  }

  int gain = -1; 
  if ( deviceConfig->GetScalarAttribute("Gain", gain)) 
  {
    this->Gain=gain; 
  }

  int dynRange = -1; 
  if ( deviceConfig->GetScalarAttribute("DynRange", dynRange)) 
  {
    this->DynRange=dynRange; 
  }

  int zoom = -1; 
  if ( deviceConfig->GetScalarAttribute("Zoom", zoom)) 
  {
    this->Zoom=zoom; 
  }

  int frequency = -1; 
  if ( deviceConfig->GetScalarAttribute("Frequency", frequency)) 
  {
    this->Frequency=frequency; 
  }

  int compressionStatus = 0; 
  if ( deviceConfig->GetScalarAttribute("CompressionStatus", compressionStatus)) 
  {
    this->CompressionStatus=compressionStatus; 
  }

  int sharedMemoryStatus = 0; 
  if ( deviceConfig->GetScalarAttribute("SharedMemoryStatus", sharedMemoryStatus)) 
  {
    this->SharedMemoryStatus=sharedMemoryStatus; 
  }

  int timeout = 0; 
  if ( deviceConfig->GetScalarAttribute("Timeout", timeout)) 
  {
    this->Timeout=timeout; 
  }

  int soundVelocity = 1540; // Default value.
  if ( deviceConfig->GetScalarAttribute("SoundVelocity", soundVelocity ) ) 
  {
    this->SoundVelocity = soundVelocity; 
  }

  double connectionSetupDelayMs=3.0; 
  if ( deviceConfig->GetScalarAttribute("ConnectionSetupDelayMs", connectionSetupDelayMs)) 
  {
    this->ConnectionSetupDelayMs=connectionSetupDelayMs; 
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::WriteConfiguration(vtkXMLDataElement* config)
{
  // Write superclass configuration
  Superclass::WriteConfiguration(config); 

  if ( config == NULL )
  {
    LOG_ERROR("Config is invalid");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* imageAcquisitionConfig = this->FindThisDeviceElement(config);
  if (imageAcquisitionConfig == NULL) 
  {
    LOG_ERROR("Cannot find ImageAcquisition element in XML tree!");
    return PLUS_FAIL;
  }

  if (this->ImagingMode == BMode)
  {
    imageAcquisitionConfig->SetAttribute("ImagingMode", "BMode");
  }
#if (PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 6) // RF acquisition mode is not supported on Ultrasonix SDK 6.x and above - see https://www.assembla.com/spaces/plus/tickets/489-add-rf-image-acquisition-support-on-ulterius-6-x
  else if (this->ImagingMode == RfMode)
  {
    imageAcquisitionConfig->SetAttribute("ImagingMode", "RfMode");
  }
#endif
  else
  {
    LOG_ERROR("Saving of unsupported ImagingMode requested!");
  }

  if (this->RfAcquisitionMode == RF_ACQ_B_ONLY)
  {
    imageAcquisitionConfig->SetAttribute("RfAcquisitionMode", "BOnly");
  }
  else if (this->RfAcquisitionMode == RF_ACQ_RF_ONLY)
  {
    imageAcquisitionConfig->SetAttribute("RfAcquisitionMode", "RfOnly");
  }
  else if (this->RfAcquisitionMode == RF_ACQ_B_AND_RF)
  {
    imageAcquisitionConfig->SetAttribute("RfAcquisitionMode", "BAndRf");
  }
  else if (this->RfAcquisitionMode == RF_ACQ_CHRF_ONLY)
  {
    imageAcquisitionConfig->SetAttribute("RfAcquisitionMode", "ChRfOnly");
  }
  else if (this->RfAcquisitionMode == RF_ACQ_B_AND_CHRF)
  {
    imageAcquisitionConfig->SetAttribute("RfAcquisitionMode", "BAndChRf");
  }
  else
  {
    LOG_ERROR("Saving of unsupported RfAcquisitionMode requested!");
  }

  if (this->AcquisitionDataType == udtBPost)
  {
    imageAcquisitionConfig->SetAttribute("AcquisitionDataType", "BPost");
  }
  else if (this->AcquisitionDataType == udtRF)
  {
    imageAcquisitionConfig->SetAttribute("AcquisitionDataType", "RF");
  }
  else
  {
    LOG_ERROR("Saving of unsupported AcquisitionDataType requested!");
  }

  imageAcquisitionConfig->SetAttribute("IP", this->SonixIP);
  imageAcquisitionConfig->SetIntAttribute("Depth", this->Depth);
  imageAcquisitionConfig->SetIntAttribute("Sector", this->Sector);
  imageAcquisitionConfig->SetIntAttribute("Gain", this->Gain);
  imageAcquisitionConfig->SetIntAttribute("DynRange", this->DynRange);
  imageAcquisitionConfig->SetIntAttribute("Zoom", this->Zoom);
  imageAcquisitionConfig->SetIntAttribute("Frequency", this->Frequency);
  imageAcquisitionConfig->SetIntAttribute("CompressionStatus", this->CompressionStatus);
  imageAcquisitionConfig->SetIntAttribute("Timeout", this->Timeout);
  imageAcquisitionConfig->SetDoubleAttribute("ConnectionSetupDelayMs", this->ConnectionSetupDelayMs);

  return PLUS_SUCCESS;
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
//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::SetPPFilter(int filterIndex)
{
  if (!this->UlteriusConnected)
  {
    LOG_ERROR("vtkSonixVideoSource::SetPPFilter failed: not connected");
    return PLUS_FAIL;
  }
  if ( !this->Ult.setParamValue("b-filter type", filterIndex) )
  {
    LOG_ERROR("vtkSonixVideoSource::SetPPFilter failed: cannot set filter value.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::SetFrameRateLimit(int frLimit)
{
  if (!this->UlteriusConnected)
  {
    LOG_ERROR("vtkSonixVideoSource::SetFrameRateLimit failed: not connected");
    return PLUS_FAIL;
  }
  if ( !this->Ult.setParamValue("max fr", frLimit) )
  {
    LOG_ERROR("vtkSonixVideoSource::SetFrameRateLimit failed: cannot set maximum frame rate limit value.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::SetRfAcquisitionMode(RfAcquisitionModeType mode)
{
  if (!this->UlteriusConnected)
  {
    // Connection has not been established yet. Parameter value will be set upon connection.
    this->RfAcquisitionMode = mode; 
    return PLUS_SUCCESS;
  }
  if (!this->Ult.setParamValue("rf-mode", mode))
  {
    LOG_ERROR("vtkSonixVideoSource::SetRfAcquisitionMode failed (paramId=rf-mode, paramValue="<<mode<<") "<<GetLastUlteriusError());
    return PLUS_FAIL;
  }
  this->RfAcquisitionMode = mode; 
  return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::GetRfAcquisitionMode(RfAcquisitionModeType & mode)
{
  int iMode = this->RfAcquisitionMode; 
  if (this->UlteriusConnected)
  {
    if (!this->Ult.getParamValue("rf-mode", iMode))
    {
      LOG_ERROR("vtkSonixVideoSource::GetRfAcquisitionMode failed. "<<GetLastUlteriusError());
      return PLUS_FAIL;
    }
  }

  switch (iMode)
  {
  case 0:
    mode = RF_ACQ_B_ONLY; 
    break;
  case 1:
    mode = RF_ACQ_RF_ONLY; 
    break;
  case 2:
    mode = RF_ACQ_B_AND_RF; 
    break;
  case 3:
    mode = RF_ACQ_CHRF_ONLY; 
    break;
  case 4:
    mode = RF_ACQ_B_AND_CHRF; 
    break;
  default: 
    mode = RF_UNKNOWN; 
    LOG_WARNING("Unknown RF acquisition mode type: " << iMode ); 
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::InternalUpdate()
{
  if( Superclass::InternalUpdate() != PLUS_SUCCESS )
  {
    return PLUS_FAIL;
  }

  // TODO : future fix, make this smart to detect changed mode, activate appropriate stream
  if( this->UlteriusConnected )
  {
    int mode;
    if( this->GetImagingMode(mode) != PLUS_SUCCESS )
    {
      return PLUS_SUCCESS;
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::NotifyConfigured()
{
  if( this->OutputChannels.size() > 2 )
  {
    LOG_WARNING("vtkSonixVideoSource is expecting at most two output channels and their are " << this->OutputChannels.size() << " channels. First output channel will be used.");
  }

  if( this->OutputChannels.size() == 0 )
  {
    LOG_ERROR("No output channels defined for vtkSonixVideoSource. Cannot proceed." );
    this->SetCorrectlyConfigured(false);
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkSonixVideoSource::Reset()
{
  for( ChannelContainerIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it )
  {
    (*it)->Delete();
  }
  this->InputChannels.clear();
  for( ChannelContainerIterator it = this->OutputChannels.begin(); it != this->OutputChannels.end(); ++it )
  {
    (*it)->Delete();
  }
  this->OutputChannels.clear();
  for( DataSourceContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it )
  {
    it->second->Delete();
  }
  this->Tools.clear();
  for( DataSourceContainerIterator it = this->VideoSources.begin(); it != this->VideoSources.end(); ++it )
  {
    it->second->Delete();
  }
  this->VideoSources.clear();

  this->SetSonixIP("127.0.0.1");
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
  this->RfAcquisitionMode=RF_ACQ_RF_ONLY; // get RF data only in RfMode 
  this->NumberOfOutputFrames = 1;
  this->UlteriusConnected = false;
  this->SharedMemoryStatus = 0; //0 corresponds to always use TCP
  this->RequireImageOrientationInConfiguration = true;
  this->RequireFrameBufferSizeInDeviceSetConfiguration = true;
  this->RequireAcquisitionRateInDeviceSetConfiguration = false;
  this->RequireAveragedItemsForFilteringInDeviceSetConfiguration = false;
  this->RequireLocalTimeOffsetSecInDeviceSetConfiguration = false;
  this->RequireUsImageOrientationInDeviceSetConfiguration = true;
  this->RequireRfElementInDeviceSetConfiguration = false;

  // No need for StartThreadForInternalUpdates, as we are notified about each new frame through a callback function

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
bool vtkSonixVideoSource::HasDataType( uData aValue )
{
  uDataDesc someVal;
  return this->Ult.getDataDescriptor( aValue, someVal ) ? PLUS_SUCCESS : PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::ConfigureVideoSource( uData aValue )
{
  vtkPlusDataSource* aSource(NULL);
  if( (aValue & udtBPost) > 0)
  {
    this->GetVideoSourceByPortName(vtkPlusDevice::BMODE_PORT_NAME, aSource);
  }
  else if( (aValue & udtRF) > 0)
  {
    this->GetVideoSourceByPortName(vtkPlusDevice::RFMODE_PORT_NAME, aSource);
  }
  else
  {
    LOG_ERROR("Unsupported uData requested of Sonix video source.");
    return PLUS_FAIL;
  }

  if( aSource == NULL )
  {
    LOG_ERROR("Unable to locate the video source for uData: " << aValue);
    return PLUS_FAIL;
  }

  if (!this->Ult.isDataAvailable(aValue))
  {
    LOG_ERROR("Data type is not available. " << aValue);
    return PLUS_FAIL;
  }

  // Set frame size and pixel type
  uDataDesc aDataDescriptor;
  if ( !this->Ult.getDataDescriptor(aValue, aDataDescriptor) )
  {
    LOG_WARNING("Initialize: couldn't retrieve data descriptor (" << GetLastUlteriusError() << ")"); // error is reported at higher level, as it often happens that this call fails but after a few attempts it succeeds
    return PLUS_FAIL;
  }

  switch (aDataDescriptor.ss)
  {
  case 8:
    aSource->GetBuffer()->SetPixelType( itk::ImageIOBase::UCHAR );
    aSource->GetBuffer()->SetImageType( US_IMG_BRIGHTNESS );
    aSource->GetBuffer()->SetImageOrientation(US_IMG_ORIENT_MF);
    break;
  case 16:
    aSource->GetBuffer()->SetPixelType( itk::ImageIOBase::SHORT );
    aSource->GetBuffer()->SetImageType( US_IMG_RF_I_LINE_Q_LINE );
    // RF data is stored line-by-line, therefore set the storage buffer to FM orientation
    aSource->GetBuffer()->SetImageOrientation(US_IMG_ORIENT_FM);
    // Swap w/h: in case of RF image acquisition the DataDescriptor.h is the width and the DataDescriptor.w is the height
    {
      int tmp = aDataDescriptor.h;
      aDataDescriptor.h = aDataDescriptor.w;
      aDataDescriptor.w = tmp;
    }
    break;
  default:
    LOG_ERROR("Unsupported Ulterius bit depth: " << aDataDescriptor.ss);
    return PLUS_FAIL;
  }
  this->SetFrameSize( *aSource, aDataDescriptor.w, aDataDescriptor.h);

  return PLUS_SUCCESS;
}