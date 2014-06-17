/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
The following copyright notice is applicable to parts of this file:
Copyright (c) Siddharth Vikal, Elvis Chen, 2008
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
Authors include: Siddharth Vikal (Queen's University),
Elvis Chen (Queen's University), Danielle Pace (Robarts Research Institute
and The University of Western Ontario)
=========================================================================*/  

#include "TrackedFrame.h" 
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMultiThreader.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusBuffer.h"
#include "vtkSonixPortaVideoSource.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTimerLog.h"
#include "vtkUnsignedCharArray.h"
#include "vtksys/SystemTools.hxx"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"

#include <ctype.h>

// because of warnings in windows header push and pop the warning level
#ifdef _MSC_VER
#pragma warning (push, 3)
#endif

#include <vector>
#include <string>

#ifdef _MSC_VER
#pragma warning (pop)
#endif

const int MAX_BYTES_PER_BMODE_PIXEL=4;

vtkCxxRevisionMacro(vtkSonixPortaVideoSource, "$Revision: 1.0$");
//vtkStandardNewMacro(vtkWin32VideoSource);
//----------------------------------------------------------------------------
// Needed when we don't use the vtkStandardNewMacro.
vtkInstantiatorNewMacro(vtkSonixPortaVideoSource);

//----------------------------------------------------------------------------
vtkSonixPortaVideoSource* vtkSonixPortaVideoSource::Instance = 0;
vtkSonixPortaVideoSourceCleanup vtkSonixPortaVideoSource::Cleanup;

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

// The porta wrapper implementation does not have the proper #includes, so we have to include the file here
#if (PLUS_ULTRASONIX_SDK_MAJOR_VERSION >= 6) 
#include "porta_wrapper.cpp"
#endif

//----------------------------------------------------------------------------
vtkSonixPortaVideoSourceCleanup::vtkSonixPortaVideoSourceCleanup()
{
}

//----------------------------------------------------------------------------
vtkSonixPortaVideoSourceCleanup::~vtkSonixPortaVideoSourceCleanup()
{
  // Destroy any remaining output window.
  vtkSonixPortaVideoSource::SetInstance(NULL);
}

//----------------------------------------------------------------------------
vtkSonixPortaVideoSource::vtkSonixPortaVideoSource() 
{
	this->BModeFrameSize[0] = 480;
	this->BModeFrameSize[1] = 436;
	this->BModeFrameBuffer = 0;
  this->ImagingMode = (int)BMode;
  this->PortaProbeSelected = 0;
  this->PortaModeSelected = 0;
  this->PortaProbeName = 0;
  this->PortaSettingPath = 0;
  this->PortaLicensePath = 0;
  this->PortaFirmwarePath = 0;
  this->PortaLUTPath = 0;
  this->PortaCineSize = 256 * 1024 * 1024; // defaults to 245MB of Cine
  this->FirstCallToAddFrameToBuffer = true;
  this->CurrentMotorAngle = 0;
  this->StartMotorAngle = 0;
  this->VolumeIndex = 0;
	this->IncrementVolumeIndexClockwise = false;
	this->IncrementVolumeIndexCounterClockwise = true;

  this->RequireImageOrientationInConfiguration = true;

  // No need for StartThreadForInternalUpdates, as we are notified about each new frame through a callback function
}

vtkSonixPortaVideoSource::~vtkSonixPortaVideoSource() 
{
  // clean up porta related sources
  // this->vtkSonixPortaVideoSource::ReleaseSystemResources();

  // release all previously allocated memory
  SetPortaProbeName(NULL);
  SetPortaSettingPath(NULL);
  SetPortaLicensePath(NULL);
  SetPortaFirmwarePath(NULL);
  SetPortaLUTPath(NULL);

  delete [] this->BModeFrameBuffer;
  this->BModeFrameBuffer = 0;
}

//----------------------------------------------------------------------------
// up the reference count so it behaves like New
vtkSonixPortaVideoSource *vtkSonixPortaVideoSource::New() 
{
  vtkSonixPortaVideoSource *ret = vtkSonixPortaVideoSource::GetInstance();
  ret->Register( NULL );
  return( ret );
}

//----------------------------------------------------------------------------
// Return the single instance of the vtkOutputWindow
vtkSonixPortaVideoSource *vtkSonixPortaVideoSource::GetInstance() 
{

  if ( !vtkSonixPortaVideoSource::Instance ) 
  {
    // try the factory first
    vtkSonixPortaVideoSource::Instance = (vtkSonixPortaVideoSource *)vtkObjectFactory::CreateInstance( "vtkSonixPortaVideoSource" );

    if ( !vtkSonixPortaVideoSource::Instance ) 
    {
      vtkSonixPortaVideoSource::Instance = new vtkSonixPortaVideoSource();
    }

    if ( !vtkSonixPortaVideoSource::Instance ) 
    {
      int error = 0;
    }
  }

  return( vtkSonixPortaVideoSource::Instance );
}

void vtkSonixPortaVideoSource::SetInstance( vtkSonixPortaVideoSource *instance ) 
{
  if ( vtkSonixPortaVideoSource::Instance == instance ) 
  {
    return;
  }

  // preferably this will be NULL
  if ( vtkSonixPortaVideoSource::Instance ) 
  {
    vtkSonixPortaVideoSource::Instance->Delete();
    vtkSonixPortaVideoSource::Instance=NULL;
  }

  vtkSonixPortaVideoSource::Instance = instance;

  if ( !instance ) 
  {
    return;
  }

  //user will call ->Delete() after setting instance
  instance->Register( NULL );
}


//----------------------------------------------------------------------------
std::string vtkSonixPortaVideoSource::GetSdkVersion()
{
  std::ostringstream version; 
  version << "UltrasonixSDK-" << PLUS_ULTRASONIX_SDK_MAJOR_VERSION << "." << PLUS_ULTRASONIX_SDK_MINOR_VERSION << "." << PLUS_ULTRASONIX_SDK_PATCH_VERSION; 
  return version.str(); 
}

//----------------------------------------------------------------------------
void vtkSonixPortaVideoSource::PrintSelf(ostream& os, vtkIndent indent) {
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Imaging mode: " << this->ImagingMode << "\n";
  os << indent << "Frequency: " << this->Frequency << "MHz\n";
}

//----------------------------------------------------------------------------
// the callback function used when there is a new frame of data received
#if (PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 5) || (PLUS_ULTRASONIX_SDK_MAJOR_VERSION == 5 && PLUS_ULTRASONIX_SDK_MINOR_VERSION < 7)
//  SDK version < 5.7.x 
bool vtkSonixPortaVideoSource::vtkSonixPortaVideoSourceNewFrameCallback( void *param, int id )
#elif (PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 6) 
//  5.7.x <= SDK version < 6.x
bool vtkSonixPortaVideoSource::vtkSonixPortaVideoSourceNewFrameCallback( void *param, int id, int header )
#else
int vtkSonixPortaVideoSource::vtkSonixPortaVideoSourceNewFrameCallback(void* param, int id, int header)
#endif
{

  if ( id == 0 )  
  {
    // no actual data received
    return ( false );
  }

  vtkSonixPortaVideoSource::GetInstance()->AddFrameToBuffer( param, id );

  return ( true );
}

//----------------------------------------------------------------------------
// copy the Device Independent Bitmap from the VFW framebuffer into the
// vtkVideoSource framebuffer (don't do the unpacking yet)
PlusStatus vtkSonixPortaVideoSource::AddFrameToBuffer( void *param, int id ) 
{
  if (!this->Recording)
  {
    // drop the frame, we are not recording data now
    return PLUS_SUCCESS;
  }

  if( this->OutputChannels.empty() )
  {
    LOG_ERROR("No output channels defined for vtkSonixPortaVideoSource" );
    return PLUS_FAIL;
  }
  vtkPlusChannel* outputChannel=this->OutputChannels[0];

  int frameSize[2] = {0,0};
  this->GetFrameSize(*outputChannel, frameSize);
  vtkPlusDataSource* aSource(NULL);
  if( outputChannel->GetVideoSource(aSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve the video source in the SonixPorta device.");
    return PLUS_FAIL;
  }
  int frameBufferBytesPerPixel = aSource->GetBuffer()->GetNumberOfBytesPerPixel(); 
  const int frameSizeInBytes = frameSize[0] * frameSize[1] * frameBufferBytesPerPixel; 

  // for frame containing FC (frame count) in the beginning for data coming from cine, jump 2 bytes
  int numberOfBytesToSkip = 4; 

  // Aligns the motor for correct acqusition of its angle
  if ( this->FirstCallToAddFrameToBuffer )
  {
	this->Porta.setParam( prmMotorStatus, 0 );
    this->Porta.setParam( prmMotorStatus, 1 );
    this->FirstCallToAddFrameToBuffer = false;
  }

  this->Porta.getBwImage( 0, this->BModeFrameBuffer, false );

  // Compute the angle of the motor. 
  double frameIndexOneVolume =  id % this->FramePerVolume;
  double frameIndexTwoVolumes =  id % (2*this->FramePerVolume);
  if ( frameIndexOneVolume == 0 )
  {
		frameIndexOneVolume = this->FramePerVolume;
  }
  if ( frameIndexTwoVolumes == 0 )
  {
		frameIndexTwoVolumes = 2 * this->FramePerVolume;
  }
  if ( frameIndexTwoVolumes <=  this->FramePerVolume )
  {
		if ( this->IncrementVolumeIndexCounterClockwise )
		{
			++this->VolumeIndex;
			this->IncrementVolumeIndexCounterClockwise = false;
			this->IncrementVolumeIndexClockwise = true;
		}
		this->CurrentMotorAngle = this->StartMotorAngle - (frameIndexOneVolume - 1) * this->MotorRotationPerStepDeg * (double)this->StepPerFrame;
	}
  else
  {
		if ( this->IncrementVolumeIndexClockwise )
		{
			++this->VolumeIndex;
			this->IncrementVolumeIndexCounterClockwise = true;
			this->IncrementVolumeIndexClockwise = false;
		}
		this->CurrentMotorAngle = - this->StartMotorAngle + (frameIndexOneVolume - 1) * this->MotorRotationPerStepDeg * (double)this->StepPerFrame;
  }

  //std::ostringstream motorAngle;
  //motorAngle << this->CurrentMotorAngle;

  //std::ostringstream frameNumber;
  //frameNumber << id;
  
  std::ostringstream volumeIndex;
  volumeIndex << this->VolumeIndex;

  TrackedFrame::FieldMapType customFields; 
  //customFields["MotorAngle"] = motorAngle.str();
  //customFields["FrameNumber"] = frameNumber.str(); 
  customFields["MotorToMotorRotatedTransform"] = this->GetMotorToMotorRotatedTransformTransform( this->CurrentMotorAngle );
  customFields["MotorToMotorRotatedTransformStatus"] = "OK";
	customFields["DummyToVolumeIndexTransform"] =  volumeIndex.str() + " 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0";
  customFields["DummyToVolumeIndexTransformStatus"] = "OK";

  PlusStatus status = aSource->GetBuffer()->AddItem(this->BModeFrameBuffer, aSource->GetPortImageOrientation(), frameSize, VTK_UNSIGNED_CHAR, 1, US_IMG_BRIGHTNESS, numberOfBytesToSkip, id, UNDEFINED_TIMESTAMP, UNDEFINED_TIMESTAMP, &customFields); 

  this->Modified();
  return status;
}

//----------------------------------------------------------------------------
std::string vtkSonixPortaVideoSource::GetMotorToMotorRotatedTransformTransform( double angle )
{
	vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();

	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->SetMatrix( matrix );
	transform->RotateX( angle ); 
	transform->GetMatrix(matrix);
	
	std::ostringstream matrixToString;
	matrixToString << matrix->GetElement(0,0) <<" "<< matrix->GetElement(0,1) <<" "<< matrix->GetElement(0,2) <<" "<< matrix->GetElement(0,3) 
	  << " " <<
	     matrix->GetElement(1,0) <<" "<< matrix->GetElement(1,1) <<" "<< matrix->GetElement(1,2) <<" "<< matrix->GetElement(1,3) 
	  << " " <<
	     matrix->GetElement(2,0) <<" "<< matrix->GetElement(2,1) <<" "<< matrix->GetElement(2,2) <<" "<< matrix->GetElement(2,3) 
	  << " " <<
	     matrix->GetElement(3,0) <<" "<< matrix->GetElement(3,1) <<" "<< matrix->GetElement(3,2) <<" "<< matrix->GetElement(3,3) ;//volumeIndex;

	return matrixToString.str();
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixPortaVideoSource::InternalConnect() 
{
  if ( this->PortaSettingPath == 0 ||
    this->PortaFirmwarePath == 0 ||
    this->PortaLUTPath == 0 ) 
  {
    LOG_ERROR("One of the Porta paths has not been set" );
    return PLUS_FAIL;
  }

  LOG_TRACE("Some info on the initializaton" << this->Usm << " " << this->Pci );
  if ( !this->Porta.init( this->PortaCineSize,
    this->PortaFirmwarePath,
    this->PortaSettingPath,
    this->PortaLicensePath,
    this->PortaLUTPath,
    this->Usm, 
    this->Pci,
    this->HighVoltage,
    0, 
    this->Channels) )
  {
    LOG_ERROR("Initialize: Porta could not be initialized: (" << this->GetLastPortaError() << ")" );
    return PLUS_FAIL;
  }

  this->Connected = true;

  // select the probe
  int code=0;
  if ( this->Porta.isConnected() ) 
  {
    code = (char)this->Porta.getProbeID( 0 );
  }

  // select the code read and see if it is motorized
  if ( this->Porta.selectProbe( code ) &&
    this->Porta.getProbeInfo( this->ProbeInformation ) &&
    this->ProbeInformation.motorized ) 
  {
    const int MAX_NAME_LENGTH=100;
    char name[MAX_NAME_LENGTH+1];
    name[MAX_NAME_LENGTH]=0;


    // the 3D/4D probe is always connected to port 0
    this->Porta.activateProbeConnector( 0 );
    this->Porta.getProbeName( name, MAX_NAME_LENGTH, code );

    // store the probe name
    SetPortaProbeName(name);
	if ( !this->Porta.findMasterPreset( name, MAX_NAME_LENGTH, code ) ) 
    {
      LOG_ERROR("Initialize: master preset cannot be found" );
      return PLUS_FAIL;
    }

    if ( !this->Porta.loadPreset( name ) )
    {
      LOG_ERROR("Initialize: master preset could not be loaded" );
      return PLUS_FAIL;
    }

    vtkPlusDataSource* aSource(NULL);
    if( this->GetFirstActiveVideoSource(aSource) != PLUS_SUCCESS )
    {
      LOG_ERROR(this->GetDeviceId() << ": Unable to retrieve video source.");
      return PLUS_FAIL;
    }

		// Set B-mode image size
    delete [] this->BModeFrameBuffer;
		this->BModeFrameBuffer = new unsigned char [ this->BModeFrameSize[0] * this->BModeFrameSize[1] * MAX_BYTES_PER_BMODE_PIXEL ];
		if ( !this->BModeFrameBuffer ) 
		{
			LOG_ERROR("vtkSonixPortaVideoSource constructor: not enough memory for BModeFrameBuffer" );
		}

    if( !this->SetFrameSize( *aSource, this->PortaBModeWidth, this->PortaBModeHeight )  )
    {
      LOG_ERROR("Initializer: can not set the frame size" );
    }

    // now we have successfully selected the probe
    this->PortaProbeSelected = 1;
  }

  // this is from propello
  if( !this->Porta.initImagingMode( BMode ) ) 
  {    
    LOG_ERROR("Initialize: cannot initialize imagingMode (" << this->GetLastPortaError() << ")" );
    return PLUS_FAIL;
  }
  else
  {
    this->Porta.setDisplayDimensions( 0,
      this->PortaBModeWidth,
      this->PortaBModeHeight );
  }

  // successfully set to bmode
  this->PortaModeSelected = 1;

  SetFrequency(this->Frequency);
  SetDepth(this->Depth);
  SetGain(this->Gain);
  SetZoom(this->Zoom);
  SetFramePerVolume(this->FramePerVolume);
  SetStepPerFrame(this->StepPerFrame);

  // Compute the angle per step
  this->MotorRotationPerStepDeg = (double)this->ProbeInformation.motorFov / (double)this->ProbeInformation.motorSteps / 1000;
  this->StartMotorAngle = ((double)(this->FramePerVolume-1) * this->MotorRotationPerStepDeg * (double)this->StepPerFrame) / 2;

  // Turn on the motor
  this->Porta.setParam( prmMotorStatus, 1 );

  // finally, update all the parameters
  if ( !this->UpdateSonixPortaParams() ) 
  {
    LOG_ERROR("Initialize: cannot update sonix params" ); 
  }

  // set up the callback function which is invocked upon arrival
  // of a new frame
  this->Porta.setDisplayCallback( 0, vtkSonixPortaVideoSourceNewFrameCallback,(void*)this ); 

  LOG_DEBUG("Successfully connected to sonix porta video device");
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixPortaVideoSource::InternalDisconnect()
{
  this->Porta.setParam( prmMotorStatus, 0 );
  this->Porta.stopImage();
  this->Porta.shutdown();
  return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
PlusStatus vtkSonixPortaVideoSource::InternalStartRecording() 
{
  if ( !this->Recording ) 
  {
    this->Recording = 1;
    this->Modified();

    if ( !this->Porta.isImaging() )
    {
      this->Porta.runImage();
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixPortaVideoSource::InternalStopRecording() 
{
  if ( this->Recording ) 
  {
    this->Recording = 0;
    this->Modified();

    if ( this->Porta.isImaging() )
    {
      this->Porta.stopImage();
    }
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixPortaVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_ENUM1_ATTRIBUTE_OPTIONAL(ImagingMode, deviceConfig, "BMode", BMode);

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, Depth, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, Gain, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, Zoom, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, Frequency, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, Timeout, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, FramePerVolume, deviceConfig);
  XML_READ_VECTOR_ATTRIBUTE_OPTIONAL(int, 2, BModeFrameSize, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, StepPerFrame, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, Usm, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, Pci, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, HighVoltage, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, Channels, deviceConfig);

  XML_READ_STRING_ATTRIBUTE_REQUIRED(PortaLUTPath, deviceConfig);
  XML_READ_STRING_ATTRIBUTE_REQUIRED(PortaSettingPath, deviceConfig);
  XML_READ_STRING_ATTRIBUTE_REQUIRED(PortaLicensePath, deviceConfig);
  XML_READ_STRING_ATTRIBUTE_REQUIRED(PortaFirmwarePath, deviceConfig);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkSonixPortaVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfig)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(imageAcquisitionConfig, rootConfig);

  if (this->ImagingMode == BMode)
  {
    imageAcquisitionConfig->SetAttribute("ImagingMode", "BMode");
  }
  else
  {
    LOG_ERROR("Saving of unsupported ImagingMode requested!");
  }

  imageAcquisitionConfig->SetIntAttribute("Depth", this->Depth);
  imageAcquisitionConfig->SetIntAttribute("Gain", this->Gain);
  imageAcquisitionConfig->SetIntAttribute("Zoom", this->Zoom);
  imageAcquisitionConfig->SetIntAttribute("Frequency", this->Frequency);
  imageAcquisitionConfig->SetIntAttribute("Timeout", this->Timeout);
  imageAcquisitionConfig->SetIntAttribute("FramePerVolume", this->FramePerVolume);
  imageAcquisitionConfig->SetIntAttribute("StepPerFrame", this->StepPerFrame);
  imageAcquisitionConfig->SetIntAttribute("USM", this->Usm);
  imageAcquisitionConfig->SetIntAttribute("PCI", this->Pci);
  imageAcquisitionConfig->SetIntAttribute("HighVoltage", this->HighVoltage);
  imageAcquisitionConfig->SetIntAttribute("Channels", this->Channels);
  imageAcquisitionConfig->SetAttribute("PortaLUTPath", this->PortaLUTPath);
  imageAcquisitionConfig->SetAttribute("PortaSettingPath", this->PortaSettingPath);
  imageAcquisitionConfig->SetAttribute("PortaLicensePath", this->PortaLicensePath);
  imageAcquisitionConfig->SetAttribute("PortaFirmwarePath", this->PortaFirmwarePath);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
std::string vtkSonixPortaVideoSource::GetLastPortaError()
{
#if (PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 6)
  const unsigned int MAX_PORTA_ERROR_MSG_LENGTH=256;
  char err[MAX_PORTA_ERROR_MSG_LENGTH+1];
  err[MAX_PORTA_ERROR_MSG_LENGTH]=0; // make sure the string is null-terminated
  this->Porta.getLastError(err,MAX_PORTA_ERROR_MSG_LENGTH);
  return err; 
#else
  return "unknown";
#endif
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixPortaVideoSource::SetParamValue(char* paramId, int paramValue, int &validatedParamValue)
{
  if (!this->Connected)
  {
    // Connection has not been established yet. Parameter value will be set upon connection.
    validatedParamValue=paramValue;
    return PLUS_SUCCESS;
  }
  if (!this->Porta.setParam(paramId, paramValue))
  {
    LOG_ERROR("vtkSonixPortaVideoSource::SetParamValue failed (paramId="<<paramId<<", paramValue="<<paramValue<<") "<<GetLastPortaError());
    return PLUS_FAIL;
  }
  validatedParamValue=paramValue;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixPortaVideoSource::GetParamValue(char* paramId, int& paramValue, int &validatedParamValue)
{
  if (!this->Connected)
  {
    // Connection has not been established yet. Returned the cached value.
    paramValue=validatedParamValue;
    return PLUS_SUCCESS;
  }
  paramValue=-1;
  if (!this->Porta.getParam(paramId, paramValue))
  {
    LOG_ERROR("vtkSonixPortaVideoSource::GetParamValue failed (paramId="<<paramId<<", paramValue="<<paramValue<<") "<<GetLastPortaError());
    return PLUS_FAIL;
  }
  validatedParamValue=paramValue;
  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkSonixPortaVideoSource::SetFrequency(int aFrequency)
{
  return SetParamValue(prmBTxFreq, aFrequency, this->Frequency);
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixPortaVideoSource::GetFrequency(int& aFrequency)
{
  return GetParamValue(prmBTxFreq, aFrequency, this->Frequency);
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixPortaVideoSource::SetDepth(int aDepth)
{
  return SetParamValue(prmBImageDepth, aDepth, this->Depth);
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixPortaVideoSource::GetDepth(int& aDepth)
{
  return GetParamValue(prmBImageDepth, aDepth, this->Depth);
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixPortaVideoSource::SetGain(int aGain)
{
  return SetParamValue(prmBGain, aGain, this->Gain);
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixPortaVideoSource::GetGain(int& aGain)
{
  return GetParamValue(prmBGain, aGain, this->Gain);
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixPortaVideoSource::SetZoom(int aZoom)
{
  return SetParamValue(prmZoom, aZoom, this->Zoom);
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixPortaVideoSource::GetZoom(int& aZoom)
{
  return GetParamValue(prmZoom, aZoom, this->Zoom);
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixPortaVideoSource::SetFramePerVolume(int aFramePerVolume)
{
  return SetParamValue(prmMotorFrames, aFramePerVolume, this->FramePerVolume);
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixPortaVideoSource::GetFramePerVolume(int& aFramePerVolume)
{
  return GetParamValue(prmMotorFrames, aFramePerVolume, this->FramePerVolume);
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixPortaVideoSource::SetStepPerFrame(int aStepPerFrame)
{
  return SetParamValue(prmMotorSteps, aStepPerFrame, this->StepPerFrame);
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixPortaVideoSource::GetStepPerFrame(int& aStepPerFrame)
{
  return GetParamValue(prmMotorSteps, aStepPerFrame, this->StepPerFrame);
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixPortaVideoSource::SetTimeout(int aTimeout)
{
  this->Timeout = aTimeout;
  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkSonixPortaVideoSource::UpdateSonixPortaParams() 
{
  bool bRunning = this->Porta.isImaging();

  if ( bRunning ) 
  {
    this->Porta.stopImage();
  }

  // update VTK FrameRate with SonixRP's hardware frame rate
  //
  // The reasons we update it here is because the SonixRP's hardware
  // frame rate is a function of several parameters, such as
  // bline density and image-depths.
  //
  this->AcquisitionRate = (float)(this->Porta.getFrameRate() );

  if ( bRunning ) 
  {
    this->Porta.runImage();
  }

  this->Modified();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixPortaVideoSource::NotifyConfigured()
{
  if( this->OutputChannels.size() > 1 )
  {
    LOG_WARNING("vtkSonixPortaVideoSource is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
    return PLUS_FAIL;
  }

  if( this->OutputChannels.empty() )
  {
    LOG_ERROR("No output channels defined for vtkSonixPortaVideoSource. Cannot proceed." );
    this->SetCorrectlyConfigured(false);
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}