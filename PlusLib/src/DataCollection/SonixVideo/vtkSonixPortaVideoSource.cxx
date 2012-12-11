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

#include "vtkSonixPortaVideoSource.h"

#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkTimerLog.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkUnsignedCharArray.h"
#include "vtksys/SystemTools.hxx"
#include "vtkPlusStreamBuffer.h"
#include "vtkMultiThreader.h"
#include "TrackedFrame.h" 

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

vtkCxxRevisionMacro(vtkSonixPortaVideoSource, "$Revision: 1.0$");
//vtkStandardNewMacro(vtkWin32VideoSource);
//----------------------------------------------------------------------------
// Needed when we don't use the vtkStandardNewMacro.
vtkInstantiatorNewMacro(vtkSonixPortaVideoSource);

//----------------------------------------------------------------------------
vtkSonixPortaVideoSource* vtkSonixPortaVideoSource::Instance = 0;
vtkSonixPortaVideoSourceCleanup vtkSonixPortaVideoSource::Cleanup;


// AR: Copied from vtkSonixVideoSource by Abtin, not sure if these values are valid for porta as well
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
  // porta instantiation
  this->PortaBModeWidth = 640;       // defaults to BMode, 640x480
  this->PortaBModeHeight = 480;
  this->ImageBuffer = 0;
  this->ImageBuffer = new unsigned char [ this->PortaBModeWidth *
    this->PortaBModeHeight * 4 ];
  if ( !this->ImageBuffer ) 
  {
    LOG_ERROR("vtkSonixPortaVideoSource constructor: not enough emory for ImageBuffer" );
  }

  this->ImagingMode = (int)BMode;
  this->PortaMotorStartPosition = 0;
  this->PortaMotorPosition = 0;
  this->PortaProbeSelected = 0;
  this->PortaModeSelected = 0;
  this->PortaProbeName = 0;
  this->PortaSettingPath = 0;
  this->PortaLicensePath = 0;
  this->PortaFirmwarePath = 0;
  this->PortaLUTPath = 0;
  this->PortaCineSize = 256 * 1024 * 1024; // defaults to 245MB of Cine

  //initialize the frame number
  this->FrameNumber = 0;

  this->RequireDeviceImageOrientationInDeviceSetConfiguration = true;
  this->RequireFrameBufferSizeInDeviceSetConfiguration = true;
  this->RequireAcquisitionRateInDeviceSetConfiguration = false;
  this->RequireAveragedItemsForFilteringInDeviceSetConfiguration = false;
  this->RequireLocalTimeOffsetSecInDeviceSetConfiguration = false;
  this->RequireUsImageOrientationInDeviceSetConfiguration = true;
  this->RequireRfElementInDeviceSetConfiguration = false;
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

  delete [] this->ImageBuffer;
  this->ImageBuffer = 0;

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
  os << indent << "Motor position: " << this->PortaMotorPosition << "MHz\n";
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
 
  // AR: update the number of frames.
  this->FrameNumber++;
  // AR: borrowed from sonixvideo
  int frameSize[2] = {0,0};
  this->GetFrameSize(frameSize);
  int frameBufferBytesPerPixel = this->GetBuffer()->GetNumberOfBytesPerPixel(); 
  const int frameSizeInBytes = frameSize[0] * frameSize[1] * frameBufferBytesPerPixel; 
  
  // for frame containing FC (frame count) in the beginning for data coming from cine, jump 2 bytes
  int numberOfBytesToSkip = 4; 
  
  PlusCommon::ITKScalarPixelType pixelType=itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;    
  pixelType=itk::ImageIOBase::UCHAR;
  
  this->Porta.getBwImage( 0, this->ImageBuffer, false );
  
  // get the pointer to the actual incoming data onto a local pointer
  unsigned char *deviceDataPtr = static_cast<unsigned char*>( this->ImageBuffer );

	// Compute the angle of the motor. 
	// since the motor is sweeping back and forth, the frame index is found for size of two volumes
	double frameIndexInTwoVolumes = (FrameNumber % (this->FramePerVolume * 2));
	double currentMotorAngle;
	if (frameIndexInTwoVolumes <= FramePerVolume) {
		currentMotorAngle = (frameIndexInTwoVolumes - this->FramePerVolume / 2) * this->MotorRotationPerStepDeg * this->StepPerFrame;
	} else {
		currentMotorAngle = - (frameIndexInTwoVolumes - this->FramePerVolume *3 / 2) * this->MotorRotationPerStepDeg * this->StepPerFrame;
	}
	std::ostringstream motorAngle;
	motorAngle << currentMotorAngle;

  TrackedFrame::FieldMapType customFields; 
  customFields["MotorAngle"] = motorAngle.str(); 

  PlusStatus status = this->GetBuffer()->AddItem(deviceDataPtr, this->GetDeviceImageOrientation(), frameSize, pixelType, US_IMG_BRIGHTNESS, numberOfBytesToSkip, this->FrameNumber, UNDEFINED_TIMESTAMP, UNDEFINED_TIMESTAMP, &customFields); 
  this->Modified();
  return status;
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

		std::string preset = "D:/t/devel/PLTools/Ultrasonix/sdk-5.6.4/Porta/dat/presets/imaging/GEN-General (4DC7-3 40mm).xml";
		if ( !this->Porta.loadPreset( name ) )
    {
      LOG_ERROR("Initialize: master preset could not be loaded" );
      return PLUS_FAIL;
    }

	if( !this->SetFrameSize( this->PortaBModeWidth, this->PortaBModeHeight )	)
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
PlusStatus vtkSonixPortaVideoSource::ReadConfiguration(vtkXMLDataElement* config)
{
  LOG_TRACE("vtkSonixPortaVideoSource::ReadConfiguration"); 
  if ( config == NULL )
  {
      LOG_ERROR("Unable to configure Sonix Porta video source! (XML data element is NULL)"); 
      return PLUS_FAIL; 
  }

  Superclass::ReadConfiguration(config); 

  vtkXMLDataElement* imageAcquisitionConfig = this->FindThisDeviceElement(config);
  if (imageAcquisitionConfig == NULL) 
  {
    LOG_ERROR("Unable to find ImageAcquisition element in configuration XML structure!");
    return PLUS_FAIL;
  }

  const char* imagingMode = imageAcquisitionConfig->GetAttribute("ImagingMode"); 
  if ( imagingMode != NULL) 
  {
    if (STRCASECMP(imagingMode, "BMode")==0)
    {
      LOG_DEBUG("Imaging mode set: BMode"); 
      this->SetImagingMode(BMode); 
    }
    else
    {
      LOG_ERROR("Unsupported ImagingMode requested: "<<imagingMode);
    }
  }  
  const char* acquisitionDataType = imageAcquisitionConfig->GetAttribute("AcquisitionDataType"); 

  int depth = -1; 
  if ( imageAcquisitionConfig->GetScalarAttribute("Depth", depth)) 
  {
    this->Depth = depth; 
  }

  int gain = -1; 
  if ( imageAcquisitionConfig->GetScalarAttribute("Gain", gain)) 
  {
    this->Gain = gain; 
  }

  int zoom = -1; 
  if ( imageAcquisitionConfig->GetScalarAttribute("Zoom", zoom)) 
  {
    this->Zoom = zoom; 
  }

  int frequency = -1; 
  if ( imageAcquisitionConfig->GetScalarAttribute("Frequency", frequency)) 
  {
    this->Frequency = frequency;
  }

  int timeout = 0; 
  if ( imageAcquisitionConfig->GetScalarAttribute("Timeout", timeout)) 
  {
    this->Timeout = timeout; 
  }
  
  int framePerVolume = 0; 
  if ( imageAcquisitionConfig->GetScalarAttribute("FramePerVolume", framePerVolume)) 
  {
    this->FramePerVolume = framePerVolume;
  }  

  int stepPerFrame = 0; 
  if ( imageAcquisitionConfig->GetScalarAttribute("StepPerFrame", stepPerFrame)) 
  {
    this->StepPerFrame = stepPerFrame; 
  }  

  int usm = 0; 
  if ( imageAcquisitionConfig->GetScalarAttribute("USM", usm)) 
  {
    this->Usm = usm; 
  }  

  int pci = 0; 
  if ( imageAcquisitionConfig->GetScalarAttribute("PCI", pci)) 
  {
    this->Pci = pci; 
  }  

	int highVoltage = 0; 
  if ( imageAcquisitionConfig->GetScalarAttribute("HighVoltage", highVoltage)) 
  {
    this->HighVoltage = highVoltage; 
  } 

	int channels = 0; 
  if ( imageAcquisitionConfig->GetScalarAttribute("Channels", channels)) 
  {
    this->Channels = channels; 
  } 

	const char* portaLUTpath = imageAcquisitionConfig->GetAttribute("PortaLUTPath"); 
  if ( portaLUTpath != NULL) 
  {
    this->SetPortaLUTPath(portaLUTpath); 
  }
  else
  {
	  LOG_ERROR("Porta LUT path is not defined: "<<portaLUTpath);
  }

  const char* portaSettingPath = imageAcquisitionConfig->GetAttribute("PortaSettingPath"); 
  if ( portaSettingPath != NULL) 
  {
    this->SetPortaSettingPath(portaSettingPath); 
  }
  else
  {
	  LOG_ERROR("Porta Setting path is not defined: "<<portaSettingPath);
  }

  const char* portaLicensePath = imageAcquisitionConfig->GetAttribute("PortaLicensePath"); 
  if ( portaLicensePath != NULL) 
  {
    this->SetPortaLicensePath(portaLicensePath); 
  }
  else
  {
	  LOG_ERROR("Porta License path is not defined: "<<portaLicensePath);
  }
  
  const char* portaFirmwarePath = imageAcquisitionConfig->GetAttribute("PortaFirmwarePath"); 
  if ( portaFirmwarePath != NULL) 
  {
    this->SetPortaFirmwarePath(portaFirmwarePath); 
  }
  else
  {
	  LOG_ERROR("Porta Firmware path is not defined: "<<portaFirmwarePath);
  }

  LOG_DEBUG("Porta read the XML configuration");
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkSonixPortaVideoSource::WriteConfiguration(vtkXMLDataElement* config)
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
