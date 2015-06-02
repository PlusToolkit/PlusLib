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
#include "vtkSonixVideoSource.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTimerLog.h"
#include "vtkUnsignedCharArray.h"
#include "vtkUsImagingParameters.h"
#include "vtksys/SystemTools.hxx"

#include "ulterius.h"
#include "ulterius_def.h"
#include "ImagingModes.h" // Ulterius imaging modes

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


//----------------------------------------------------------------------------

vtkSonixVideoSource* vtkSonixVideoSource::ActiveSonixDevice = NULL;

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkSonixVideoSource);

//----------------------------------------------------------------------------

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
vtkSonixVideoSource::vtkSonixVideoSource()
: SonixIP(NULL)
, Frequency(-1)
, Depth(-1)
, Sector(-1)
, Gain(-1)
, DynRange(-1)
, Zoom(-1)
, Timeout(-1)
, SoundVelocity(-1)
, ConnectionSetupDelayMs(3000)
, CompressionStatus(0)
, AcquisitionDataType(udtBPost)
, ImagingMode(BMode)
, RfAcquisitionMode(RF_ACQ_RF_ONLY)
, UlteriusConnected(false)
, SharedMemoryStatus(0)
, DetectDepthSwitching(false)
, DetectPlaneSwitching(false)
, AutoClipEnabled(false)
, ImageGeometryOutputEnabled(false)
, CurrentDepthMm(-1)
, ImageGeometryChanged(false)
, ImageToTransducerTransformName(NULL)
{
  this->Ult = new ulterius;
  this->SetSonixIP("127.0.0.1");
  this->StartThreadForInternalUpdates = false;

  this->CurrentTransducerOriginPixels[0]=-1;
  this->CurrentTransducerOriginPixels[1]=-1;

  this->CurrentPixelSpacingMm[0]=-1;
  this->CurrentPixelSpacingMm[1]=-1;

  this->RequireImageOrientationInConfiguration = true;
  
  // This effectively forces only one sonixvideosource at a time, but it paves the way
  // for a non-singleton architecture when the SDK supports it
  if( vtkSonixVideoSource::ActiveSonixDevice != NULL )
  {
    LOG_WARNING("There is already an active vtkSonixVideoSource device. Ultrasonix SDK only supports one connection at a time, so the existing device is now deactivated and the newly created class is activated instead.");
  }
  vtkSonixVideoSource::ActiveSonixDevice = this;
}

//----------------------------------------------------------------------------
vtkSonixVideoSource::~vtkSonixVideoSource()
{
  vtkSonixVideoSource::ActiveSonixDevice = NULL;

  this->SetSonixIP(NULL);
  delete this->Ult;
  this->Ult = NULL;
  this->SetImageToTransducerTransformName(NULL);
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
    LOG_DEBUG("vtkSonixVideoSourceNewFrameCallback: called without data");
    return false;
  }

  if( vtkSonixVideoSource::ActiveSonixDevice != NULL )
  {
    vtkSonixVideoSource::ActiveSonixDevice->AddFrameToBuffer(data, type, sz, cine, frmnum);
  }
  else
  {
    LOG_ERROR("vtkSonixVideoSource data callback but the ActiveSonixDevice is NULL. Disconnect between device and SDK.");
    return false;
  }

  return true;;
}

//----------------------------------------------------------------------------
// the callback when parameters change on a device
bool vtkSonixVideoSource::vtkSonixVideoSourceParamCallback( void * paramId, int ptX, int ptY )
{
  char* paramName = (char*)paramId;

  if( vtkSonixVideoSource::ActiveSonixDevice == NULL )
  {
    LOG_ERROR("vtkSonixVideoSource data callback but the ActiveSonixDevice is NULL. Disconnect between device and SDK.");
    return false;
  }

  if( STRCASECMP(paramName, "b-depth") == 0 )
  {
    // we cannot query parameter values here, so just set a flag
    // and get the value when getting the frame
    vtkSonixVideoSource::ActiveSonixDevice->ImageGeometryChanged = true;
    return true;
  }
  else if ( STRCASECMP(paramName, "probe id") == 0 )
  {
    char probeName[200] = {0};
    if( !vtkSonixVideoSource::ActiveSonixDevice->Ult->getActiveProbe(probeName, 200) )
    {
      LOG_ERROR("Unable to retrieve probe name: " << vtkSonixVideoSource::ActiveSonixDevice->GetLastUlteriusError() );
      return false;
    }
    std::string probeString(probeName);
    return true;
  }
  else if ( STRCASECMP(paramName, "") == 0 )
  {
    return true;
  }
  return false;
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

  std::vector<vtkPlusDataSource*> sources;
  PlusCommon::VTKScalarPixelType pixelType = VTK_VOID;    
  US_IMAGE_TYPE imgType = US_IMG_TYPE_XX;    

  if( (uData)type == udtBPost && this->GetVideoSourcesByPortName(vtkPlusDevice::BMODE_PORT_NAME, sources) == PLUS_SUCCESS )
  {
    pixelType = VTK_UNSIGNED_CHAR;
    imgType = US_IMG_BRIGHTNESS;
  }
  else if( (uData)type == udtRF && this->GetVideoSourcesByPortName(vtkPlusDevice::RFMODE_PORT_NAME, sources) == PLUS_SUCCESS )
  {
    pixelType = VTK_SHORT;
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

  vtkPlusDataSource* aSource = sources[0];

  int frameSize[3] = {0,0,0};
  aSource->GetInputFrameSize(frameSize);
  int frameBufferBytesPerPixel = aSource->GetNumberOfBytesPerPixel(); 
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
    // This typically occurs when Plus with Ultrasonix SDK 5.7.x is used with Exam software 6.0.7 or later
    LOG_ERROR("Received frame size (" << sz << " bytes) doesn't match the buffer size (" << frameSizeInBytes + numberOfBytesToSkip << " bytes). Make sure the Ultrasonix SDK version used in Plus ("<<GetSdkVersion()<<") is compatible with the Exam software running on the ultrasound device."); 
    return PLUS_FAIL; 
  }

  if (this->ImageGeometryChanged)
  {
    this->ImageGeometryChanged = false;
    int currentDepth=-1;
    if (!vtkSonixVideoSource::ActiveSonixDevice->Ult->getParamValue("b-depth", currentDepth))
    {
      LOG_WARNING("Failed to retrieve b-depth parameter");
    }
    uPoint currentPixelSpacingMicron;
    currentPixelSpacingMicron.x=-1;
    currentPixelSpacingMicron.y=-1;
    if (!vtkSonixVideoSource::ActiveSonixDevice->Ult->getParamValue("microns", currentPixelSpacingMicron))
    {
      LOG_WARNING("Failed to retrieve bb-microns parameter");
    }
    uPoint currentTransducerOriginPixels;
    currentTransducerOriginPixels.x=-1;
    currentTransducerOriginPixels.y=-1;
    if (!vtkSonixVideoSource::ActiveSonixDevice->Ult->getParamValue("origin", currentTransducerOriginPixels))
    {
      LOG_WARNING("Failed to retrieve bb-origin parameter");
    }

    this->CurrentDepthMm = currentDepth;

    this->CurrentPixelSpacingMm[0] = 0.001*currentPixelSpacingMicron.x;
    this->CurrentPixelSpacingMm[1] = 0.001*currentPixelSpacingMicron.y;

    int *clipRectangleOrigin = aSource->GetClipRectangleOrigin();
    this->CurrentTransducerOriginPixels[0] = currentTransducerOriginPixels.x-clipRectangleOrigin[0];
    this->CurrentTransducerOriginPixels[1] = currentTransducerOriginPixels.y-clipRectangleOrigin[1];
  }

  TrackedFrame::FieldMapType customFields;

  if (this->ImageGeometryOutputEnabled)
  {
    std::ostringstream depthStr;
    depthStr << this->CurrentDepthMm;
    customFields["DepthMm"] = depthStr.str();

    std::ostringstream pixelSpacingStr;
    pixelSpacingStr << this->CurrentPixelSpacingMm[0] << " " << this->CurrentPixelSpacingMm[1];
    customFields["PixelSpacingMm"] = pixelSpacingStr.str();

    std::ostringstream transducerOriginStr;
    transducerOriginStr << this->CurrentTransducerOriginPixels[0] << " " << this->CurrentTransducerOriginPixels[1];
    customFields["TransducerOriginPix"] = transducerOriginStr.str(); // "TransducerOriginPixels" would be over the 20-char limit of OpenIGTLink device name
  }
  if (this->ImageToTransducerTransformName!=NULL && strlen(this->ImageToTransducerTransformName)>0)
  {
    std::ostringstream imageToTransducerTransformStr;
    double zPixelSpacingMm = (this->CurrentPixelSpacingMm[0]+this->CurrentPixelSpacingMm[1])/2.0; // set to non-zero to keep the matrix as a 3D-3D transformation
    imageToTransducerTransformStr << this->CurrentPixelSpacingMm[0] << " 0 0 " << -1.0*this->CurrentTransducerOriginPixels[0]*this->CurrentPixelSpacingMm[0];
    imageToTransducerTransformStr << " 0 " << this->CurrentPixelSpacingMm[1] << " 0 " << -1.0*this->CurrentTransducerOriginPixels[1]*this->CurrentPixelSpacingMm[1];
    imageToTransducerTransformStr << " 0 0 " << zPixelSpacingMm << " 0";
    imageToTransducerTransformStr << " 0 0 0 1";
    customFields["ImageToTransducerTransform"] = imageToTransducerTransformStr.str();
    customFields["ImageToTransducerTransformStatus"] = "OK";
  }

  // get the pointer to actual incoming data on to a local pointer
  unsigned char *deviceDataPtr = static_cast<unsigned char*>(dataPtr);

  PlusStatus status = aSource->AddItem(deviceDataPtr, aSource->GetInputImageOrientation(), frameSize, pixelType, 1, imgType, numberOfBytesToSkip, this->FrameNumber,
    UNDEFINED_TIMESTAMP, UNDEFINED_TIMESTAMP, &customFields);
  this->Modified(); 

  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::InternalConnect()
{
  this->Ult->setMessaging(false); // don't print messages on stdout

  int requestedImagingDataType = 0;
  if (GetRequestedImagingDataTypeFromSources(requestedImagingDataType)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to connect to sonix video device");
    return PLUS_FAIL;
  }

  switch (requestedImagingDataType)
  {
  case vtkUsImagingParameters::DataTypeBPost:
    LOG_DEBUG("Imaging mode set: BMode");
    this->ImagingMode = BMode;
    this->RfAcquisitionMode = RF_ACQ_B_ONLY;
    this->AcquisitionDataType = udtBPost;
    break;
  case vtkUsImagingParameters::DataTypeRF:
    LOG_DEBUG("Imaging mode set: RfMode");
#if (PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 6)
    this->ImagingMode = RfMode;
#else
    this->ImagingMode = 0; // RfMode is always enabled for SDK 6.x
#endif
    this->RfAcquisitionMode = RF_ACQ_RF_ONLY;
    this->AcquisitionDataType = udtRF;
    break;
  case (vtkUsImagingParameters::DataTypeBPost+vtkUsImagingParameters::DataTypeRF):
    LOG_DEBUG("Imaging mode set: BAndRfMode");
#if (PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 6)
    this->ImagingMode = RfMode;
#else
    this->ImagingMode = BMode; // RfMode is always enabled for SDK 6.x
#endif
    this->RfAcquisitionMode = RF_ACQ_B_AND_RF;
    this->AcquisitionDataType = udtBPost | udtRF;
    break;
  default:
    LOG_ERROR("Unsupported imaging data types are requested: "<<requestedImagingDataType);
    return PLUS_FAIL;
  }

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
      this->Ult->setDataToAcquire(0); // without this Ulterius 5.x may crash
      this->Ult->disconnect();
      vtkAccurateTimer::Delay(0.5); // without this Ulterius 6.x may crash
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
    if(!this->Ult->connect(this->SonixIP))
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
    int actualImagingMode = -1;
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

    // Set up imaging parameters
    // Parameter value <0 means that the parameter should be kept unchanged
    if (this->Frequency >= 0 && SetFrequency(this->Frequency) != PLUS_SUCCESS) { continue; }
    if (this->Depth >= 0 && SetDepth(this->Depth) != PLUS_SUCCESS) { continue; }
    if (this->Sector >= 0 && SetSector(this->Sector) != PLUS_SUCCESS) { continue; }
    if (this->Gain >= 0 && SetGain(this->Gain) != PLUS_SUCCESS) { continue; }
    if (this->DynRange >= 0 && SetDynRange(this->DynRange) != PLUS_SUCCESS) { continue; }
    if (this->Zoom >= 0 && SetZoom(this->Zoom) != PLUS_SUCCESS) { continue; }
    if (this->CompressionStatus >= 0 && SetCompressionStatus(this->CompressionStatus) != PLUS_SUCCESS) { continue; }    
    if (this->SoundVelocity > 0 && this->SetParamValue( "soundvelocity", this->SoundVelocity, this->SoundVelocity ) != PLUS_SUCCESS ) { continue; }

    if (this->AcquisitionRate<=0)
    {
      // AcquisitionRate has not been specified, set it to match the frame rate
      int aFrameRate=10;
      if ( this->Ult->getParamValue("frame rate", aFrameRate) )
      {
        this->AcquisitionRate=aFrameRate;        
      }
    }

    Ult->setSharedMemoryStatus( this->SharedMemoryStatus );

#if (PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 6)
    // RF acquisition mode is always enabled on Ultrasonix SDK 6.x and above, so we only need to change it if it's an earlier SDK version
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
#endif

    // Wait for the depth change to take effect before calling ConfigureVideoSource()
    // (it is especially important if auto clipping is enabled because then we need accurate frame size)
    Sleep(1000);

    // Configure video sources
    if( this->WantDataType(udtBPost))
    {
      if (!this->HasDataType(udtBPost))
      {
        // This typically occurs when Plus with Ultrasonix SDK 6.1.0 is used with Exam software 6.0.2
        LOG_ERROR("No B-mode data is available. Make sure the Ultrasonix SDK version used in Plus ("<<GetSdkVersion()<<") is compatible with the Exam software running on the ultrasound device.");
        continue;
      }
      if( this->ConfigureVideoSource(udtBPost) != PLUS_SUCCESS )
      {
        LOG_ERROR("Unable to configure B-mode video source");
        continue;
      }
    }
    if( this->WantDataType(udtRF))
    {
      if (!this->HasDataType(udtRF))
      {
        LOG_ERROR("No Rf-mode data is available");
        continue;
      }
      if( this->ConfigureVideoSource(udtRF) != PLUS_SUCCESS )
      {
        LOG_ERROR("Unable to configure Rf-mode video source");
        continue;
      }
    }

    // Actually request data, now that its available
    if ( !this->Ult->setDataToAcquire(this->AcquisitionDataType) )
    {
      LOG_ERROR("Setting AcquisitionDataType failed: couldn't request the data aquisition type " << this->AcquisitionDataType << ", " << GetLastUlteriusError());
      return PLUS_FAIL;
    }

    // Set callback and timeout for receiving new frames
    this->Ult->setCallback(vtkSonixVideoSourceNewFrameCallback);
    if (this->Timeout >= 0 && this->SetTimeout(this->Timeout) != PLUS_SUCCESS)
    {
      continue;
    }

    // Set the param callback so we can observe depth and plane changes
    this->Ult->setParamCallback(vtkSonixVideoSourceParamCallback);

    initializationCompleted = true;
    this->ImageGeometryChanged = true; // trigger an initial update of geometry info
  }

  LOG_DEBUG("Successfully connected to sonix video device");
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::InternalDisconnect()
{
  this->UlteriusConnected=false;
  this->Ult->setDataToAcquire(0); // without this Ulterius 5.x may crash
  this->Ult->disconnect();
  vtkAccurateTimer::Delay(0.5); // without this Ulterius 6.x may crash
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::InternalStartRecording()
{
  // Unfreeze
  if(this->Ult->getFreezeState())
  {
    this->Ult->toggleFreeze();
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::InternalStopRecording()
{
  // Freeze
  if (!this->Ult->getFreezeState())
  {
    this->Ult->toggleFreeze();
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkSonixVideoSource::ReadConfiguration"); 
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  const char* ipAddress = deviceConfig->GetAttribute("IP"); 
  if ( ipAddress != NULL) 
  {
    this->SetSonixIP(ipAddress); 
    LOG_DEBUG("Sonix Video IP: " << ipAddress); 
  }
  else
  {
    LOG_WARNING("Ultrasonix IP address is not defined. Defaulting to " << this->GetSonixIP() );
  }  

  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(DetectDepthSwitching, deviceConfig);
  if (this->DetectDepthSwitching)
  {
    // TODO : read the config for each output channel, check for Depth="x" attribute
    // with that attribute, build a lookup table depth->channel
  }
  else
  {
    XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, Depth, deviceConfig);
  }

  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(DetectPlaneSwitching, deviceConfig);

  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(AutoClipEnabled, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(ImageGeometryOutputEnabled, deviceConfig);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(ImageToTransducerTransformName, deviceConfig);

  // TODO : if depth or plane switching, build lookup table
  // if both attributes, build [plane, depth]->channel lookup table
  // if one, build [attr]->channel lookup table

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, Sector, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, Gain, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, DynRange, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, Zoom, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, Frequency, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, CompressionStatus, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, SharedMemoryStatus, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, Timeout, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, SoundVelocity, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, ConnectionSetupDelayMs, deviceConfig);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfig)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(imageAcquisitionConfig, rootConfig);

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
  
  XML_WRITE_BOOL_ATTRIBUTE(AutoClipEnabled, imageAcquisitionConfig);
  XML_WRITE_BOOL_ATTRIBUTE(ImageGeometryOutputEnabled, imageAcquisitionConfig);
  XML_WRITE_STRING_ATTRIBUTE_REMOVE_IF_NULL(ImageToTransducerTransformName, imageAcquisitionConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
std::string vtkSonixVideoSource::GetLastUlteriusError()
{
  const unsigned int MAX_ULTERIUS_ERROR_MSG_LENGTH=256;
  char err[MAX_ULTERIUS_ERROR_MSG_LENGTH+1];
  err[MAX_ULTERIUS_ERROR_MSG_LENGTH]=0; // make sure the string is null-terminated
  this->Ult->getLastError(err,MAX_ULTERIUS_ERROR_MSG_LENGTH);

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
  if (!this->Ult->setParamValue(paramId, paramValue))
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
  if (!this->Ult->getParamValue(paramId, paramValue))
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
  if (!this->Ult->setCompressionStatus(aCompressionStatus))
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
  aCompressionStatus=this->Ult->getCompressionStatus();
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
  this->Ult->setTimeout(aTimeout);
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
  if (!this->Ult->isDataAvailable((uData)(aAcquisitionDataType)))
  {
    LOG_ERROR("vtkSonixVideoSource::SetAcquisitionDataType failed: Requested the data aquisition type not available for selected imaging mode(" << GetLastUlteriusError() << ")");
    return PLUS_FAIL;
  }
  // actually request data, now that its available
  if (!this->Ult->setDataToAcquire(aAcquisitionDataType))
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
  acquisitionDataType=this->Ult->getDataToAcquire();
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
  if (!this->Ult->selectMode(mode))
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
  mode=this->Ult->getActiveImagingMode();
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
  for (int paramId=0; this->Ult->getParam(paramId, prmDescriptor); paramId++)
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

  if ( !this->Ult->getParamValue("frame rate", aFrameRate) )
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
  if ( !this->Ult->setParamValue("rf-rf decimation", decimation) )
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
  if ( !this->Ult->setParamValue("b-filter type", filterIndex) )
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
  if ( !this->Ult->setParamValue("max fr", frLimit) )
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
  if (!this->Ult->setParamValue("rf-mode", mode))
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
    if (!this->Ult->getParamValue("rf-mode", iMode))
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
bool vtkSonixVideoSource::HasDataType( uData aValue )
{
  uDataDesc someVal;
  bool success = this->Ult->getDataDescriptor( aValue, someVal );
  return success;
}

//----------------------------------------------------------------------------
bool vtkSonixVideoSource::WantDataType( uData aValue )
{
  return (this->AcquisitionDataType & aValue) > 0;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::ConfigureVideoSource( uData aValue )
{
  vtkPlusDataSource* aSource(NULL);
  std::vector<vtkPlusDataSource*> sources;
  if( (aValue & udtBPost) > 0)
  {
    this->GetVideoSourcesByPortName(vtkPlusDevice::BMODE_PORT_NAME, sources);
    if (!sources.empty())
    {
      aSource = sources[0];
    }
  }
  else if( (aValue & udtRF) > 0)
  {
    this->GetVideoSourcesByPortName(vtkPlusDevice::RFMODE_PORT_NAME, sources);
    if (!sources.empty())
    {
      aSource = sources[0];
    }
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

  if (!this->Ult->isDataAvailable(aValue))
  {
    LOG_ERROR("Data type is not available. " << aValue);
    return PLUS_FAIL;
  }

  // Set frame size and pixel type
  uDataDesc aDataDescriptor;
  if ( !this->Ult->getDataDescriptor(aValue, aDataDescriptor) )
  {
    LOG_WARNING("Initialize: couldn't retrieve data descriptor (" << GetLastUlteriusError() << ")"); // error is reported at higher level, as it often happens that this call fails but after a few attempts it succeeds
    return PLUS_FAIL;
  }

  switch (aDataDescriptor.ss)
  {
  case 8:
    aSource->SetPixelType( VTK_UNSIGNED_CHAR );
    aSource->SetImageType( US_IMG_BRIGHTNESS );
    aSource->SetOutputImageOrientation(US_IMG_ORIENT_MF);
    break;
  case 16:
    aSource->SetPixelType( VTK_SHORT );
    aSource->SetImageType( US_IMG_RF_I_LINE_Q_LINE );
    // RF data is stored line-by-line, therefore set the storage buffer to FM orientation
    aSource->SetOutputImageOrientation(US_IMG_ORIENT_FM);
    // Swap w/h: in case of RF image acquisition the DataDescriptor.h is the width and the DataDescriptor.w is the height
    std::swap(aDataDescriptor.h,aDataDescriptor.w);
    std::swap(aDataDescriptor.roi.ulx, aDataDescriptor.roi.uly);
    std::swap(aDataDescriptor.roi.urx, aDataDescriptor.roi.ury);
    std::swap(aDataDescriptor.roi.blx, aDataDescriptor.roi.bly);
    std::swap(aDataDescriptor.roi.brx, aDataDescriptor.roi.bry);
    break;
  default:
    LOG_ERROR("Unsupported Ulterius bit depth: " << aDataDescriptor.ss);
    return PLUS_FAIL;
  }

  if (this->AutoClipEnabled)
  {
    int clipRectangleOrigin[3] = {0,0,0};
    clipRectangleOrigin[0] = std::min(aDataDescriptor.roi.ulx, aDataDescriptor.roi.blx);
    clipRectangleOrigin[1] = std::min(aDataDescriptor.roi.uly, aDataDescriptor.roi.ury);
    int clipRectangleSize[3] = {0,0,1};
    clipRectangleSize[0] = std::max(aDataDescriptor.roi.urx-aDataDescriptor.roi.ulx, aDataDescriptor.roi.brx-aDataDescriptor.roi.blx);
    clipRectangleSize[1] = std::max(aDataDescriptor.roi.bly-aDataDescriptor.roi.uly, aDataDescriptor.roi.bry-aDataDescriptor.roi.ury);
    aSource->SetClipRectangleOrigin(clipRectangleOrigin);
    aSource->SetClipRectangleSize(clipRectangleSize);
  }

  this->SetInputFrameSize( *aSource, aDataDescriptor.w, aDataDescriptor.h, 1 );

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::GetRequestedImagingDataTypeFromSources(int &requestedImagingDataType)
{
  requestedImagingDataType=0;
  std::vector<vtkPlusDataSource*> sources;  
  if (this->GetVideoSourcesByPortName(vtkPlusDevice::BMODE_PORT_NAME, sources) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  if (!sources.empty())
  {
    requestedImagingDataType |= vtkUsImagingParameters::DataTypeBPost;
  }
  if (this->GetVideoSourcesByPortName(vtkPlusDevice::RFMODE_PORT_NAME, sources) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  if (!sources.empty())
  {
    requestedImagingDataType |= vtkUsImagingParameters::DataTypeRF;
  }
  return PLUS_SUCCESS;
}