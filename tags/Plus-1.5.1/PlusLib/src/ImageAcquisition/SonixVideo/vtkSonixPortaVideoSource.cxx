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

static const int CONNECT_RETRY=5;
static const int CONNECT_RETRY_DELAY_SEC=1.0;

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

  this->SetFrameBufferSize(200); 
  

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
  bool vtkSonixPortaVideoSource::vtkSonixPortaVideoSourceNewFrameCallback( void *param, int id )
#else // SDK version 5.7.x or newer
  bool vtkSonixPortaVideoSource::vtkSonixPortaVideoSourceNewFrameCallback( void *param, int id, int header )
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
  const int* frameSize = this->GetFrameSize(); 
  int frameBufferBytesPerPixel = this->Buffer->GetNumberOfBytesPerPixel(); 
  const int frameSizeInBytes = frameSize[0] * frameSize[1] * frameBufferBytesPerPixel; 
  
  // for frame containing FC (frame count) in the beginning for data coming from cine, jump 2 bytes
  int numberOfBytesToSkip = 4; 
  
  PlusCommon::ITKScalarPixelType pixelType=itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;    
  pixelType=itk::ImageIOBase::UCHAR;

  // AR: update the error on the id
/*  if ( sz != frameSizeInBytes + numberOfBytesToSkip )
  {
    LOG_ERROR("Received frame size (" << sz << " bytes) doesn't match the buffer size (" << frameSizeInBytes + numberOfBytesToSkip << " bytes)!"); 
	return PLUS_FAIL; 
  }*/
  
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

  PlusStatus status = this->Buffer->AddItem(deviceDataPtr, this->GetUsImageOrientation(), frameSize, pixelType, numberOfBytesToSkip, this->FrameNumber, UNDEFINED_TIMESTAMP, UNDEFINED_TIMESTAMP, "MotorAngle", motorAngle.str().c_str()); 
  this->Modified();
  return status;
  // this->FrameBufferMutex->Unlock();
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

  int code;
  int size = 256;
  char err[ 256 ];


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
//    this->Porta.getLastError( err, size );
    LOG_ERROR("Initialize: Porta could not be initialized: (" << this->GetLastPortaError() << ")" );
    return PLUS_FAIL;
  }

	this->Connected = true;

	// select the probe
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
	this->Porta.getLastError( err, size );
    LOG_ERROR("Initialize: cannot initialize imagingMode (" << err << ")" );
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
  this->Porta.setDisplayCallback( 0, vtkSonixPortaVideoSourceNewFrameCallback,
    (void*)this );

//  this->Porta.setMotorActive(true);


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
  // ensure the hardware is initialized  
  /*this->Initialize();

  if ( !this->Initialized ) 
  {
    return PLUS_FAIL;
  }*/

  if ( !this->Recording ) 
  {
    this->Recording = 1;
    this->FrameCount = 0;
    this->Modified();

    if ( !this->Porta.isImaging() )
      this->Porta.runImage();
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
  /*if ( acquisitionDataType != NULL) 
  {
    if (STRCASECMP(acquisitionDataType, "BPost")==0)
    {
      LOG_DEBUG("AcquisitionDataType set: BPost"); 
      this->SetAcquisitionDataType(udtBPost); 
    }
    else
    {
      LOG_ERROR("Unsupported AcquisitionDataType requested: "<<acquisitionDataType);
    }
  }*/

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
  // TODO: implement this
    return Superclass::WriteConfiguration(config); 
}

//----------------------------------------------------------------------------
std::string vtkSonixPortaVideoSource::GetLastPortaError()
{
  const unsigned int MAX_PORTA_ERROR_MSG_LENGTH=256;
  char err[MAX_PORTA_ERROR_MSG_LENGTH+1];
  err[MAX_PORTA_ERROR_MSG_LENGTH]=0; // make sure the string is null-terminated
  this->Porta.getLastError(err,MAX_PORTA_ERROR_MSG_LENGTH);
  return err; 
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
  /*if ( !this->Porta ) 
  {
    return PLUS_FAIL;
  }*/

  bool bRunning = this->Porta.isImaging();

  if ( bRunning ) 
  {
    this->Porta.stopImage();
  }

/*  if ( !this->Porta.updateImageParams() ) 
  {
    return PLUS_FAIL;
  }*/

/*  if ( !this->Porta.uploadData() ) 
  {
    return PLUS_FAIL;
  }*/

  // update VTK FrameRate with SonixRP's hardware frame rate
  //
  // The reasons we update it here is because the SonixRP's hardware
  // frame rate is a function of several parameters, such as
  // bline density and image-depths.
  //
  this->FrameRate = (float)(this->Porta.getFrameRate() );

/*  this->Porta.updateDisplayParams();*/

  if ( bRunning ) 
  {
    this->Porta.runImage();
  }

  this->Modified();

  return PLUS_SUCCESS;
}


/*#include "vtkSonixPortaVideoSource.h"

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
vtkSonixPortaVideoSourceCleanup::vtkSonixPortaVideoSourceCleanup()
{
}

//----------------------------------------------------------------------------
vtkSonixPortaVideoSourceCleanup::~vtkSonixPortaVideoSourceCleanup()
{
  // Destroy any remaining output window.
  //vtkSonixPortaVideoSource::SetInstance(NULL);
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

  this->PortaImageMode = (int)BMode;
  this->PortaMotorStartPosition = 0;
  this->PortaMotorPosition = 0;
  this->PortaProbeSelected = 0;
  this->PortaModeSelected = 0;
  this->PortaProbeName = 0;
  this->PortaSettingPath = 0;
  this->PortaFirmwarePath = 0;
  this->PortaLUTPath = 0;
  this->PortaCineSize = 256 * 1024 * 1024; // defaults to 245MB of Cine
  this->bIsPortaSettingPathSet = 0;
  this->bIsPortaLUTPathSet = 0;

  // VTK
  this->FrameSize[0] = this->PortaBModeWidth;
  this->FrameSize[1] = this->PortaBModeHeight;
  this->FrameSize[2] = 1;

  // video output format
  this->OutputFormat = VTK_RGBA;
  this->NumberOfScalarComponents = 4;
  this->FrameBufferBitsPerPixel = 32;
  this->FlipFrames = 1;
  this->FrameBufferRowAlignment = 1;

}

vtkSonixPortaVideoSource::~vtkSonixPortaVideoSource() 
{
  // clean up porta related sources
  this->vtkSonixPortaVideoSource::ReleaseSystemResources();

  // release all previously allocated memory
  SetPortaProbeName(NULL);
  SetPortaSettingPath(NULL);
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
// the callback function used when there is a new frame of data received
bool vtkSonixPortaVideoSource::vtkSonixPortaVideoSourceNewFrameCallback( void *param, unsigned char *data, int cineBlock, int reserved ) {

  if ( !data ) 
  {
    // no actual data received
    return ( false );
  }

  vtkSonixPortaVideoSource::GetInstance()->AddFrameToBuffer( param,
    data,
    cineBlock,
    reserved );

  return ( true );
}

//----------------------------------------------------------------------------
// copy the Device Independent Bitmap from the VFW framebuffer into the
// vtkVideoSource framebuffer (don't do the unpacking yet)
void vtkSonixPortaVideoSource::AddFrameToBuffer( void *param,
                                                unsigned char *data,
                                                int cineBlock, int reserved ) 
{
  this->FrameBufferMutex->Lock();

  // frame buffer indices maintaince
  if ( this->AutoAdvance ) 
  {
    this->AdvanceFrameBuffer( 1 );

    if ( this->FrameIndex + 1 < this->FrameBufferSize ) 
    {
      this->FrameIndex++;
    }
  }

  // time stamping
  int index = this->FrameBufferIndex % this->FrameBufferSize;
  while ( index < 0 )
  {
    index += this->FrameBufferSize;
  }

  this->FrameBufferTimeStamps[index] = vtkTimerLog::GetUniversalTime();

  if ( this->FrameCount++ == 0 ) 
  {
    this->StartTimeStamp = this->FrameBufferTimeStamps[ index ];
  }

  // decode the data according to the imaging parameters
  //
  // get the pointer to the actual incoming data onto a local pointer
  unsigned char *deviceDataPtr = static_cast<unsigned char*>( data );

  // get the pointer to the correct location of the frame buffer,
  // where this data needs to be copied
  unsigned char *frameBufferPtr = (unsigned char *)((reinterpret_cast<vtkUnsignedCharArray*>(this->FrameBuffer[index]))->GetPointer(0));

  int outBytesPerRow = ((this->FrameBufferExtent[1]- this->FrameBufferExtent[0]+1)* this->FrameBufferBitsPerPixel + 7)/8;
  outBytesPerRow += outBytesPerRow % this->FrameBufferRowAlignment;

  int inBytesPerRow = this->FrameSize[0] * this->FrameBufferBitsPerPixel/8;

  int rows = this->FrameBufferExtent[3]-this->FrameBufferExtent[2]+1;

  // acquire an post-processed image
  this->PtrPorta.getBwImage( 0, this->ImageBuffer, true );

  // 4) copy the data to the local vtk frame buffer
  if ( outBytesPerRow == inBytesPerRow ) 
  {
    memcpy( frameBufferPtr,
      this->ImageBuffer,
      inBytesPerRow*rows );
  }
  else
  {
    while( --rows >= 0 ) 
    {
      memcpy( frameBufferPtr,
        deviceDataPtr,
        outBytesPerRow );
      frameBufferPtr += outBytesPerRow;
      deviceDataPtr += inBytesPerRow;
    }
  }

  this->Modified();
  this->FrameBufferMutex->Unlock();
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

  int code;
  probeInfo nfo;
  int size = 256;
  char err[ 256 ];

  if ( !this->PtrPorta.init( this->PortaCineSize,
    this->PortaFirmwarePath,
    this->PortaSettingPath,
    this->PortaLUTPath ) ) 
  {
    this->PtrPorta.getLastError( err, size );
    LOG_ERROR("Initialize: Porta could not be initialized: (" << err << ")" );
    return PLUS_FAIL;
  }

  // test if the nameing scheme matches
  if ( this->Porta.testParameterNames() != -1 ) 
  {
    LOG_ERROR("Porta naming scheme does not match" );
    return PLUS_FAIL;
  }

  // select the probe
  if ( this->Porta.isConnected() ) 
  {
    code = (char)this->Porta.getProbeID( 0 );
  }

  // select the code read and see if it is motorized
  if ( this->Porta.selectProbe( code ) &&
    this->Porta.getProbeInfo( nfo ) &&
    nfo.motorized ) 
  {
    const int MAX_NAME_LENGTH=80;
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

    // now we have successfully selected the probe
    this->PortaProbeSelected = 1;
  }

  // this is from propello
  if( !this->Porta.initImagingMode( BMode ) ) 
  {
    LOG_ERROR("Initialize: cannot initialize imagingMode" );
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

  // manual acquisition
  this->Porta.setParam( prmMotorStatus, 0 );

  // finally, update all the parameters
  if ( !this->UpdateSonixPortaParams() ) 
  {
    LOG_ERROR("Initialize: cannot update sonix params" ); 
  }


  // set up the frame buffer
  this->FrameBufferMutex->Lock();
  // Frame buffer is also updated within DoFormatSetup
  this->DoFormatSetup();
  this->FrameBufferMutex->Unlock();

  // set up the callback function which is invocked upon arrival
  // of a new frame

  this->Porta.setRawDataCallback( vtkSonixPortaVideoSourceNewFrameCallback,
    (void*)this );

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkSonixVideoSource::InternalDisconnect()
{
  this->Porta.stopImage();
  this->Porta.shutdown();
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
int vtkSonixPortaVideoSource::UpdateSonixPortaParams() 
{
  if ( !this->PtrPorta ) 
  {
    return( 0 );
  }

  bool bRunning = this->Porta.isImaging();

  if ( bRunning ) 
  {
    this->Porta.stopImage();
  }

  if ( !this->Porta.updateImageParams() ) 
  {
    return( 0 );
  }

  if ( !this->Porta.uploadData() ) 
  {
    return( 0 );
  }

  // update VTK FrameRate with SonixRP's hardware frame rate
  //
  // The reasons we update it here is because the SonixRP's hardware
  // frame rate is a function of several parameters, such as
  // bline density and image-depths.
  //
  this->FrameRate = (float)(this->Porta.getFrameRate() );

  this->Porta.updateDisplayParams();

  if ( bRunning ) 
  {
    this->Porta.runImage();
  }

  this->Modified();

  return ( 1 );
}

//----------------------------------------------------------------------------
// check the current video format and set up the VTK video framebuffer to match
void vtkSonixPortaVideoSource::DoFormatSetup()
{
  // for now, assume bmode
  this->FrameSize[0] = this->PortaBModeWidth;
  this->FrameSize[1] = this->PortaBModeHeight;
  this->FrameSize[2] = 1;

  this->FrameBufferBitsPerPixel = 32;
  this->OutputFormat = VTK_RGBA;
  this->NumberOfScalarComponents = 4;

  this->UpdateFrameBuffer();
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkSonixPortaVideoSource::Grab() 
{
  // check this function:  why did Sid not do anything here?

  if ( this->Recording ) 
  {
    return;
  }

  // insure that the frame buffer is properly initialized
  this->Initialize();

  if ( !this->Initialized ) 
  {
    return;
  }
}


//----------------------------------------------------------------------------
void vtkSonixPortaVideoSource::Record() 
{
  // ensure the hardware is initialized  
  this->Initialize();

  if ( !this->Initialized ) 
  {
    return;
  }

  if ( !this->Recording ) 
  {
    this->Recording = 1;
    this->FrameCount = 0;
    this->Modified();

    if ( !this->Porta.isImaging() )
      this->Porta.runImage();
  }
}

//----------------------------------------------------------------------------
void vtkSonixPortaVideoSource::Stop() 
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
}


//----------------------------------------------------------------------------
int vtkSonixPortaVideoSource::RequestInformation(
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

  for ( i = 0; i < 3; i++ ) 
  {
    // initially set extent to the OutputWholeExtent
    extent[2*i] = this->OutputWholeExtent[2*i];
    extent[2*i+1] = this->OutputWholeExtent[2*i+1];
    // if 'flag' is set in output extent, use the FrameBufferExtent instead
    if ( extent[2*i+1] < extent[2*i] )
    {
      extent[2*i] = 0;
      extent[2*i+1] =						\
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

  // set the spacing
  outInfo->Set(vtkDataObject::SPACING(),this->DataSpacing,3);

  // set the origin.
  outInfo->Set(vtkDataObject::ORIGIN(),this->DataOrigin,3);


  
   // FIXME
  
  // set the default data type
  vtkDataObject::SetPointDataActiveScalarInfo(outInfo, VTK_UNSIGNED_CHAR,
    this->NumberOfScalarComponents);

  return( 1 );
}

//----------------------------------------------------------------------------
int vtkSonixPortaVideoSource::RequestData( vtkInformation *vtkNotUsed(request),
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

  int outIncX = this->NumberOfScalarComponents;
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
void vtkSonixPortaVideoSource::UnpackRasterLine(char *outptr, char *inptr, 
                                                int start, int count) 
{
  // assume bpost mode for now
  char alpha = (char)(this->Opacity*255);

  inptr += start;

  memcpy( outptr, inptr, count*4 );
}


//----------------------------------------------------------------------------
void vtkSonixPortaVideoSource::SetOutputFormat(int format)
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

  if (this->FrameBufferBitsPerPixel != numComponents*8) 
  {
    this->FrameBufferMutex->Lock();
    this->FrameBufferBitsPerPixel = numComponents*8;
    if (this->Initialized) 
    {
      // shouldn't have to call UpdateFrameBuffer
      // as it is called within DoFormatSetup
      // this->UpdateFrameBuffer();    
      this->DoFormatSetup();
    }
    this->FrameBufferMutex->Unlock();
  }

  this->Modified();
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
void vtkSonixPortaVideoSource::SetPortaCineSize( int size ) 
{
  this->PortaCineSize = size;
}

//----------------------------------------------------------------------------
void vtkSonixPortaVideoSource::PrintSelf(ostream& os, vtkIndent indent) {

  this->Superclass::PrintSelf(os,indent);

}

//----------------------------------------------------------------------------
int vtkSonixPortaVideoSource::SetImageDepth( int myDepth ) 
{
  if ( !this->Porta.setParam( prmImageDepth, myDepth ) ) 
  {
    this->Depth = myDepth;
    LOG_ERROR("Problem setting depth manually" );
    return( 0 );
  }

  return( this->UpdateSonixPortaParams() );
}


//----------------------------------------------------------------------------
int vtkSonixPortaVideoSource::SetZoom( int percentage ) 
{
  // 100% means full height of the display area
  if ( !this->Porta.setParam( prmZoom, percentage ) ) 
  {
    LOG_ERROR("SetZoom:  cannot set the zoom value" );
    return( 0 );
  }

  return( this->UpdateSonixPortaParams() );
}

//----------------------------------------------------------------------------
int vtkSonixPortaVideoSource::SetBLineDensity( int density ) {
  if ( !this->Porta.setParam( prmBLineDensity, density ) ) 
  {
    LOG_ERROR("SetBLineDensity: cannot set the line density" );
    return( 0 );
  }

  return( this->UpdateSonixPortaParams() );
}

//----------------------------------------------------------------------------
void vtkSonixPortaVideoSource::SetPortaMotorStartPosition( double v )
{
  this->PortaMotorStartPosition = v;
}

//----------------------------------------------------------------------------
double vtkSonixPortaVideoSource::GetPortaMotorStartPosition()
{
  return( this->PortaMotorStartPosition );
}

//----------------------------------------------------------------------------
// The actual angle being set may not be the same as the specified angle
// thus we return the actual angle that is being set.
double vtkSonixPortaVideoSource::GoToPortaMotorStartPosition()
{

  bool bRunning = this->Porta.isImaging();

  if ( bRunning ) 
  {
    this->Porta.stopImage();
  }

  this->PortaMotorPosition = this->Porta.goToPosition( this->PortaMotorStartPosition );

  if ( bRunning ) 
  {
    this->Porta.runImage();
  }

  return ( this->PortaMotorPosition );
}

//----------------------------------------------------------------------------
// The actual angle being set may not be the same as the specified angle
// thus we return the actual angle that is being set.
double vtkSonixPortaVideoSource::GoToPosition( double angle ) 
{
  bool bRunning = this->Porta.isImaging();

  if ( bRunning ) 
  {
    this->Porta.stopImage();
  }

  this->PortaMotorPosition = this->Porta.goToPosition( angle );

  if ( bRunning ) 
  {
    this->Porta.runImage();
  }

  return ( this->PortaMotorPosition );
}
//----------------------------------------------------------------------------
// The actual angle being set may not be the same as the specified angle
// thus we return the actual angle that is being set.
double vtkSonixPortaVideoSource::StepPortaMotor( bool cw, int steps )
{
  bool bRunning = this->Porta.isImaging();

  if ( bRunning ) 
  {
    this->Porta.stopImage();
  }

  this->PortaMotorPosition = this->Porta.stepMotor( cw, steps );

  if ( bRunning ) 
  {
    this->Porta.runImage();
  }

  return ( this->PortaMotorPosition );

}

//----------------------------------------------------------------------------
double vtkSonixPortaVideoSource::GetPortaMotorPosition()
{
  return( this->PortaMotorPosition );
}

//----------------------------------------------------------------------------
int vtkSonixPortaVideoSource::GetPortaFrameRate()
{
  return( this->Porta.getFrameRate() );
}

//----------------------------------------------------------------------------
int vtkSonixPortaVideoSource::SetBModeFrameSize( int width, int height ) 
{
  this->PortaBModeWidth = width;
  this->PortaBModeHeight = height;

  // need to resize the ImageBuffer
  delete [] this->ImageBuffer;
  this->ImageBuffer = 0;

  this->ImageBuffer = new unsigned char [ this->PortaBModeWidth *
    this->PortaBModeHeight * 4 ];

  if ( !this->ImageBuffer ) 
  {
    LOG_ERROR("SetBModeFrameSize: Not enough memory" );
    return( 0 );
  }


  // VTK
  this->FrameSize[0] = this->PortaBModeWidth;
  this->FrameSize[1] = this->PortaBModeHeight;
  this->FrameSize[2] = 1;

  if ( this->Initialized ) 
  {
    this->FrameBufferMutex->Lock();
    this->DoFormatSetup();
    this->FrameBufferMutex->Unlock();

    this->Porta.setDisplayDimensions( 0,
      this->PortaBModeWidth,
      this->PortaBModeHeight );

    //
    // not sure if needed, but won't hurt regardless
    //
    if ( !this->UpdateSonixPortaParams() ) 
    {
      LOG_ERROR("SetBModeFrameSize:  unable to update param" );
      return( 0 );
    }
  }

  return( 1 );
}
*/
