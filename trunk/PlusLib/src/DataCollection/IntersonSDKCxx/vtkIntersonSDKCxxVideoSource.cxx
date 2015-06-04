/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkIntersonSDKCxxVideoSource.h"

#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkImageImport.h"

#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"

#include "vtkUSImagingParameters.h"

#include "vtkRfProcessor.h"
#include "vtkUsScanConvertCurvilinear.h"
#include "vtkRfToBrightnessConvert.h"

#include "IntersonCxxImagingScan2DClass.h"
#include "IntersonCxxControlsHWControls.h"
#include "IntersonCxxIntersonClass.h"
#include "IntersonCxxImagingScanConverter.h"

//----------------------------------------------------------------------------
typedef IntersonCxx::Imaging::Scan2DClass Scan2DClassType;
typedef IntersonCxx::Controls::HWControls HWControlsType;
typedef IntersonCxx::IntersonClass        IntersonClassType;

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkIntersonSDKCxxVideoSource);

//----------------------------------------------------------------------------
struct BmodeCallbackClientData
{
  vtkIntersonSDKCxxVideoSource *  ActiveIntersonDevice;
};
struct RfCallbackClientData
{
  vtkIntersonSDKCxxVideoSource *  ActiveIntersonDevice;
};

//----------------------------------------------------------------------------
void vtkIntersonSDKCxxVideoSource::NewBmodeImageCallback( BmodePixelType * buffer, void * clientData )
{
  if( buffer == NULL )
    {
    LOG_ERROR("No actual frame data received");
    return;
    }

  BmodeCallbackClientData * callbackClientData = static_cast< BmodeCallbackClientData * >( clientData );
  vtkIntersonSDKCxxVideoSource * activeIntersonDevice = callbackClientData->ActiveIntersonDevice;

  if( activeIntersonDevice != NULL )
    {
    activeIntersonDevice->AddBmodeFrameToBuffer( buffer, clientData );
    }
  else
    {
    LOG_ERROR("vtkIntersonSDKCxxVideoSource B-mode callback but the ActiveIntersonDevice is NULL.  Disconnect between the device and SDK.");
    return;
    }
}

//----------------------------------------------------------------------------
void vtkIntersonSDKCxxVideoSource::NewRfImageCallback( RfPixelType * buffer, void * clientData )
{
  if( buffer == NULL )
    {
    LOG_ERROR("No actual frame data received");
    return;
    }

  RfCallbackClientData * callbackClientData = static_cast< RfCallbackClientData * >( clientData );
  vtkIntersonSDKCxxVideoSource * activeIntersonDevice = callbackClientData->ActiveIntersonDevice;

  if( activeIntersonDevice != NULL )
    {
    activeIntersonDevice->AddRfFrameToBuffer( buffer, clientData );
    }
  else
    {
    LOG_ERROR("vtkIntersonSDKCxxVideoSource B-mode callback but the ActiveIntersonDevice is NULL.  Disconnect between the device and SDK.");
    return;
    }
}

class vtkIntersonSDKCxxVideoSource::vtkInternal
{
public:
  vtkIntersonSDKCxxVideoSource *External;

  //----------------------------------------------------------------------------
  vtkIntersonSDKCxxVideoSource::vtkInternal::vtkInternal(vtkIntersonSDKCxxVideoSource* external) 
    : External(external)
  {	
    this->HWControls = new HWControlsType();
    this->Scan2DClass = new Scan2DClassType();
    this->IntersonClass = new IntersonClassType();

    this->BmodeClientData.ActiveIntersonDevice = this->External;
    this->RfClientData.ActiveIntersonDevice = this->External;

    this->BModeBufferToVtkImage = vtkImageImport::New();
    this->BModeBufferToVtkImage->SetDataScalarType( VTK_UNSIGNED_CHAR );
    this->BModeBufferToVtkImage->SetDataExtent( 0, Scan2DClassType::MAX_SAMPLES - 1, 0, Scan2DClassType::MAX_VECTORS - 1, 0, 0 );
    this->BModeBufferToVtkImage->SetWholeExtent( 0, Scan2DClassType::MAX_SAMPLES - 1, 0, Scan2DClassType::MAX_VECTORS - 1, 0, 0 );

    this->RfBufferToVtkImage = vtkImageImport::New();
    this->RfBufferToVtkImage->SetDataScalarType( VTK_SHORT );
    this->RfBufferToVtkImage->SetDataExtent( 0, Scan2DClassType::MAX_RFSAMPLES - 1, 0, Scan2DClassType::MAX_VECTORS - 1, 0, 0 );
    this->RfBufferToVtkImage->SetWholeExtent( 0, Scan2DClassType::MAX_RFSAMPLES - 1, 0, Scan2DClassType::MAX_VECTORS - 1, 0, 0 );
  }

  //----------------------------------------------------------------------------
  vtkIntersonSDKCxxVideoSource::vtkInternal::~vtkInternal() 
  {    
    this->External = NULL;
    delete HWControls;
    delete Scan2DClass;
    delete IntersonClass;

    this->BModeBufferToVtkImage->Delete();
    this->RfBufferToVtkImage->Delete();
  }  

  std::string GetSdkVersion()
  {
    return this->IntersonClass->Version();
  }

  HWControlsType * GetHWControls()
  {
    return this->HWControls;
  }

  Scan2DClassType * GetScan2DClass()
  {
    return this->Scan2DClass;
  }

  vtkImageData * ConvertBModeBufferToVtkImage( unsigned char * pixelData )
  {
    this->BModeBufferToVtkImage->SetImportVoidPointer( pixelData );
    this->BModeBufferToVtkImage->Update();
    return this->BModeBufferToVtkImage->GetOutput();
  }

  vtkImageData * ConvertRfBufferToVtkImage( short * pixelData )
  {
    this->RfBufferToVtkImage->SetImportVoidPointer( pixelData );
    this->RfBufferToVtkImage->Update();
    return this->RfBufferToVtkImage->GetOutput();
  }

  void EnableBModeCallback()
  {
    this->Scan2DClass->SetNewBmodeImageCallback( &vtkIntersonSDKCxxVideoSource::NewBmodeImageCallback,
      &(this->BmodeClientData) );
  }

  void DisableBModeCallback()
  {
    this->Scan2DClass->SetNewBmodeImageCallback( NULL, NULL );
  }

  void EnableRfCallback()
  {
    this->Scan2DClass->SetNewRFImageCallback( &vtkIntersonSDKCxxVideoSource::NewRfImageCallback,
      &(this->RfClientData) );
  }

  void DisableRfCallback()
  {
    this->Scan2DClass->SetNewRFImageCallback( NULL, NULL );
  }


  // Find the channel that outputs the source.
  vtkPlusChannel * GetSourceChannel( vtkPlusDataSource * source )
  {
    vtkPlusChannel * channel = NULL;
    for( ChannelContainerConstIterator channelIt = this->External->OutputChannels.begin();
         channelIt != this->External->OutputChannels.end();
         ++channelIt )
      {
      if( !(*channelIt)->HasVideoSource() )
        {
        LOG_ERROR( "Output channel does not have VideoSource" );
        return channel;
        }
      vtkPlusDataSource * channelSource = NULL;
      (*channelIt)->GetVideoSource( channelSource );
      if( channelSource == source )
        {
        channel = *channelIt;
        break;
        }
      }
    return channel;
  }

private:
  HWControlsType *    HWControls;
  Scan2DClassType *   Scan2DClass;
  IntersonClassType * IntersonClass;

  BmodeCallbackClientData BmodeClientData;
  RfCallbackClientData RfClientData;

  vtkImageImport * BModeBufferToVtkImage;
  vtkImageImport * RfBufferToVtkImage;
};


//----------------------------------------------------------------------------
vtkIntersonSDKCxxVideoSource::vtkIntersonSDKCxxVideoSource():
  PulseVoltage( 30 ),
  RfDecimation( 0 )
{
  this->Internal = new vtkInternal(this);

  this->StartThreadForInternalUpdates = false;

  this->RequireImageOrientationInConfiguration = true;
  this->RequirePortNameInDeviceSetConfiguration = true;

  this->ImagingParameters = new vtkUsImagingParameters(this);
}

//----------------------------------------------------------------------------
vtkIntersonSDKCxxVideoSource::~vtkIntersonSDKCxxVideoSource()
{ 
  if( !this->Connected )
  {
    this->Disconnect();
  }

  delete this->Internal;
  this->Internal = NULL;
}

//----------------------------------------------------------------------------
void vtkIntersonSDKCxxVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Pulse voltage: " << this->PulseVoltage << "\n";
  os << indent << "RF decimation: " << this->RfDecimation << "\n";
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonSDKCxxVideoSource::InternalConnect()
{
  LOG_TRACE( "vtkIntersonSDKCxxVideoSource::InternalConnect" );

  HWControlsType * hwControls = this->Internal->GetHWControls();

  typedef HWControlsType::FoundProbesType FoundProbesType;
  FoundProbesType foundProbes;
  hwControls->FindAllProbes( foundProbes );
  if( foundProbes.empty() )
    {
    LOG_ERROR( "Interson SDK Cxx could not find the probe." );
    return PLUS_FAIL;
    }
  if( foundProbes.size() > 1 )
    {
    LOG_WARNING("Multiple Interson probes are attached, using the first one");
    }
  hwControls->FindMyProbe( 0 );
  const unsigned int probeId = hwControls->GetProbeID();
  LOG_DEBUG( "Found probe ID: " << probeId );
  if( probeId == 0 )
    {
    LOG_ERROR( "Interson SDK Cxx could not find the probe." );
    return PLUS_FAIL;
    }

  double frequency = -1;
  this->ImagingParameters->GetFrequencyMhz( frequency );
  if( this->SetProbeFrequencyMhz( frequency ) == PLUS_FAIL )
    {
    return PLUS_FAIL;
    }

  if( !hwControls->SendHighVoltage( this->PulseVoltage ) )
    {
    LOG_ERROR( "Could not set the pulse voltage." );
    return PLUS_FAIL;
    }
  if( !hwControls->EnableHighVoltage() )
    {
    LOG_ERROR( "Could not enable high voltage." );
    return PLUS_FAIL;
    }

  double dynamicRangeDb = -1;
  this->ImagingParameters->GetDynRangeDb( dynamicRangeDb );
  if( this->SetDynRangeDb( dynamicRangeDb ) == PLUS_FAIL )
    {
    return PLUS_FAIL;
    }

  // TODO: use hardware button
  hwControls->DisableHardButton();

  Scan2DClassType * scan2D = this->Internal->GetScan2DClass();
  std::vector<vtkPlusDataSource *> rfSources;
  vtkPlusDataSource * source = NULL;

  this->GetVideoSourcesByPortName(vtkPlusDevice::RFMODE_PORT_NAME, rfSources);
  if( !rfSources.empty() )
    {
    scan2D->SetRFData( true );
    }
  else
    {
    scan2D->SetRFData( false );
    }

  LOG_DEBUG( "Interson SDK version " << this->Internal->GetSdkVersion() <<
             ", USB probe FPGA version " << hwControls->ReadFPGAVersion() );

  // Even if we do not use their SDK scan converter, we have to initialize the
  // scan converter to get the probe fully initialized.
  const bool upDown = false;
  const bool leftRight = false;
  const int width = 1000;
  const int height = 650;
  const int scanConvertDepth = 10;
  typedef IntersonCxx::Imaging::ScanConverter ScanConverterType;
  ScanConverterType scanConverter;
  ScanConverterType::ScanConverterError converterError =
    scanConverter.HardInitScanConverter( scanConvertDepth,
                                         upDown,
                                         leftRight,
                                         width,
                                         height );

  std::vector<vtkPlusDataSource *> bmodeSources;
  this->GetVideoSourcesByPortName(vtkPlusDevice::BMODE_PORT_NAME, bmodeSources);

  if( !rfSources.empty() )
    {
    this->Internal->EnableRfCallback();
    this->Internal->DisableBModeCallback();

    if( this->RfDecimation )
      {
      hwControls->EnableRFDecimator();
      }
    else
      {
      hwControls->DisableRFDecimator();
      }

    source = rfSources[0];
    // Clear buffer on connect because the new frames that we will acquire might have a different size 
    source->Clear();

    vtkPlusChannel * channel = this->Internal->GetSourceChannel( source );
    if( channel == NULL )
      {
      LOG_ERROR( "Could not find channel for source" );
      return PLUS_FAIL;
      }
    else
      {
      source->SetPixelType( VTK_SHORT );  
      source->SetImageType( US_IMG_RF_REAL );
      source->SetOutputImageOrientation( US_IMG_ORIENT_FM );
      source->SetInputFrameSize( Scan2DClassType::MAX_RFSAMPLES,
                                 Scan2DClassType::MAX_VECTORS,
                                 1 ); 
      LOG_INFO("RF Pixel type: " << vtkImageScalarTypeNameMacro( source->GetPixelType() )
            << ", device image orientation: "
              << PlusVideoFrame::GetStringFromUsImageOrientation( source->GetInputImageOrientation() )
            << ", buffer image orientation: "
              << PlusVideoFrame::GetStringFromUsImageOrientation( source->GetOutputImageOrientation() ));
      }

    if( !bmodeSources.empty() )
      {
      LOG_INFO("BMode souces are not empty!!");
      source = bmodeSources[0];
      channel = this->Internal->GetSourceChannel( source );
      if( channel == NULL )
        {
        LOG_ERROR( "Could not find channel for source" );
        return PLUS_FAIL;
        }

      // Clear buffer on connect because the new frames that we will acquire might have a different size 
      source->Clear();
      source->SetPixelType( VTK_UNSIGNED_CHAR );  
      source->SetImageType( US_IMG_BRIGHTNESS ) ;
      vtkRfProcessor * rfProcessor = channel->GetRfProcessor();
      if( rfProcessor != NULL )
        {
        channel->SetSaveRfProcessingParameters(true); // RF processing parameters were used, make sure they will be saved into the config file
        source->SetOutputImageOrientation( US_IMG_ORIENT_MF );
        vtkUsScanConvert * scanConverter = rfProcessor->GetScanConverter();
        if( scanConverter != NULL )
          {
          int outputExtent[6];
          scanConverter->GetOutputImageExtent( outputExtent );
          source->SetInputFrameSize( outputExtent[1] - outputExtent[0] + 1,
                                     outputExtent[3] - outputExtent[2] + 1,
                                     1 );
          }
        }
      else
        {
        source->SetOutputImageOrientation( US_IMG_ORIENT_FM );
        source->SetInputFrameSize( Scan2DClassType::MAX_RFSAMPLES,
                                   Scan2DClassType::MAX_VECTORS,
                                   1 ); 
        }
      LOG_INFO("Pixel type: " << vtkImageScalarTypeNameMacro( source->GetPixelType() )
            << ", device image orientation: "
              << PlusVideoFrame::GetStringFromUsImageOrientation( source->GetInputImageOrientation() )
            << ", buffer image orientation: "
              << PlusVideoFrame::GetStringFromUsImageOrientation( source->GetOutputImageOrientation() ));
      }
    }
  else if( !bmodeSources.empty() )
    {
    this->Internal->EnableBModeCallback();
    this->Internal->DisableRfCallback();

    source = bmodeSources[0];
    // Clear buffer on connect because the new frames that we will acquire might have a different size 
    source->Clear();
    source->SetPixelType( VTK_UNSIGNED_CHAR );  
    source->SetImageType( US_IMG_BRIGHTNESS );

    vtkPlusChannel * channel = this->Internal->GetSourceChannel( source );
    if( channel == NULL )
      {
      LOG_ERROR( "Could not find channel for source" );
      return PLUS_FAIL;
      }
    else
      {
      vtkRfProcessor * rfProcessor = channel->GetRfProcessor();
      if( rfProcessor != NULL )
        {
        vtkUsScanConvert * scanConverter = rfProcessor->GetScanConverter();
        if( scanConverter != NULL )
          {
          channel->SetSaveRfProcessingParameters(true); // RF processing parameters were used, make sure they will be saved into the config file
          source->SetOutputImageOrientation( US_IMG_ORIENT_MF );
          int outputExtent[6];
          scanConverter->GetOutputImageExtent( outputExtent );
          source->SetInputFrameSize( outputExtent[1] - outputExtent[0] + 1,
                                     outputExtent[3] - outputExtent[2] + 1,
                                     1 );
          }
        else
          {
          LOG_ERROR( "Did not find expected scan converter parameters." );
          return PLUS_FAIL;
          }
        }
      else
        {
        source->SetOutputImageOrientation( US_IMG_ORIENT_MF );
        source->SetInputFrameSize( Scan2DClassType::MAX_SAMPLES,
                                   Scan2DClassType::MAX_VECTORS,
                                   1 ); 
        }
      }
  
    LOG_INFO("BMode Pixel type: " << vtkImageScalarTypeNameMacro( source->GetPixelType() )
          << ", device image orientation: "
            << PlusVideoFrame::GetStringFromUsImageOrientation( source->GetInputImageOrientation() )
          << ", buffer image orientation: "
            << PlusVideoFrame::GetStringFromUsImageOrientation( source->GetOutputImageOrientation() ));
    }
  else
    {
    LOG_ERROR( "Expected an RF or BMode port not found" );
    return PLUS_FAIL;
    }

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkIntersonSDKCxxVideoSource::InternalDisconnect()
{
  LOG_DEBUG("Disconnect from Interson");

  HWControlsType * hwControls = this->Internal->GetHWControls();
  hwControls->DisableHighVoltage();

  this->StopRecording();
  Scan2DClassType * scan2D = this->Internal->GetScan2DClass();
  scan2D->AbortScan();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonSDKCxxVideoSource::InternalStartRecording()
{
  Scan2DClassType * scan2D = this->Internal->GetScan2DClass();
  scan2D->AbortScan();

  HWControlsType * hwControls = this->Internal->GetHWControls();
  if( !hwControls->StartMotor() )
    {
    LOG_ERROR( "Could not start motor." );
    return PLUS_FAIL;
    }

  std::vector<vtkPlusDataSource *> bmodeSources;
  std::vector<vtkPlusDataSource *> rfSources;
  this->GetVideoSourcesByPortName(vtkPlusDevice::BMODE_PORT_NAME, bmodeSources);
  this->GetVideoSourcesByPortName(vtkPlusDevice::RFMODE_PORT_NAME, rfSources);

  if( !rfSources.empty() )
    {
    scan2D->StartRFReadScan();
    }
  else if( !bmodeSources.empty() )
    {
    scan2D->StartReadScan();
    }
  Sleep( 100 ); // "time to start"

  if( !rfSources.empty() && !hwControls->StartRFmode() )
    {
    LOG_ERROR( "Could not start RF collection." );
    return PLUS_FAIL;
    }
  else if( !bmodeSources.empty() && !hwControls->StartBmode() )
    {
    LOG_ERROR( "Could not start B-mode collection." );
    return PLUS_FAIL;
    }
  Sleep( 750 ); // "time to start"

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonSDKCxxVideoSource::InternalStopRecording()
{ 
  HWControlsType * hwControls = this->Internal->GetHWControls();
  if( !hwControls->StopAcquisition() )
    {
    LOG_ERROR( "Could not stop acquisition." );
    return PLUS_FAIL;
    }

  Scan2DClassType * scan2D = this->Internal->GetScan2DClass();
  scan2D->StopReadScan();
  Sleep( 100 ); // "time to stop"
  scan2D->DisposeScan();
  if( !hwControls->StopMotor() )
    {
    LOG_ERROR( "Could not stop motor." );
    return PLUS_FAIL;
    }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonSDKCxxVideoSource::InternalUpdate()
{
  if( Superclass::InternalUpdate() != PLUS_SUCCESS )
  {
    return PLUS_FAIL;
  }
  
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkIntersonSDKCxxVideoSource::ReadConfiguration(vtkXMLDataElement* config)
{
  LOG_TRACE("vtkIntersonSDKCxxVideoSource::ReadConfiguration"); 
  if ( config == NULL )
  {
    LOG_ERROR("Unable to configure Interson video source! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  Superclass::ReadConfiguration(config); 

  vtkXMLDataElement* deviceConfig = this->FindThisDeviceElement(config);
  if (deviceConfig == NULL) 
  {
    LOG_ERROR("Unable to find ImageAcquisition element in configuration XML structure!");
    return PLUS_FAIL;
  }

  double dynRange = -1; 
  if ( deviceConfig->GetScalarAttribute("DynRangeDb", dynRange)) 
  {
    this->ImagingParameters->SetDynRangeDb(dynRange); 
  }

  double frequency = -1; 
  if ( deviceConfig->GetScalarAttribute("FrequencyMhz", frequency)) 
  {
    this->ImagingParameters->SetFrequencyMhz(frequency); 
  }

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, PulseVoltage, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, RfDecimation, deviceConfig);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkIntersonSDKCxxVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
  deviceConfig->SetIntAttribute("PulseVoltage", this->GetPulseVoltage());
  deviceConfig->SetIntAttribute("RfDecimation", this->RfDecimation);

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkIntersonSDKCxxVideoSource::NotifyConfigured()
{
  if( this->OutputChannels.size() > 2 )
  {
    LOG_WARNING("vtkIntersonSDKCxxVideoSource is expecting at most two output channels and there are " << this->OutputChannels.size() << " channels.");
  }

  if( this->OutputChannels.empty() )
  {
    LOG_ERROR("No output channels defined for vtkIntersonSDKCxxVideoSource. Cannot proceed." );
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
std::string vtkIntersonSDKCxxVideoSource::GetSdkVersion()
{
  return this->Internal->GetSdkVersion();
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonSDKCxxVideoSource::SetPulseVoltage(unsigned char voltage)
{
  if( voltage != this->PulseVoltage )
    {
    this->PulseVoltage = voltage;
    this->Modified();
    }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonSDKCxxVideoSource::SetRfDecimation(int enableDecimation)
{
  if( enableDecimation != this->RfDecimation )
    {
    this->RfDecimation = enableDecimation;
    this->Modified();
    }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonSDKCxxVideoSource::SetProbeFrequencyMhz(double freq)
{
  int frequency = static_cast< int >( freq * 1e6 );

  HWControlsType * hwControls = this->Internal->GetHWControls();
  HWControlsType::FrequenciesType supportedFrequencies;
  hwControls->GetFrequency( supportedFrequencies );
  // Set to the closest frequency value.
  unsigned int frequencyIndex = 0;
  if( frequency <= supportedFrequencies[0] )
    {
    frequencyIndex = 0;
    }
  const size_t numSupportedFrequencies = supportedFrequencies.size();
  if( frequency >= supportedFrequencies[numSupportedFrequencies - 1] )
    {
    frequencyIndex = numSupportedFrequencies - 1;
    }
  for( size_t ii = 1; ii < numSupportedFrequencies - 1; ++ii )
    {
    const int lower = supportedFrequencies[ii - 1] + 
                  ( supportedFrequencies[ii] - supportedFrequencies[ii - 1] ) / 2;
    if( frequency <= lower )
      {
      frequencyIndex = ii - 1;
      break;
      }
    const int upper = supportedFrequencies[ii] + 
                  ( supportedFrequencies[ii + 1] - supportedFrequencies[ii] ) / 2;
    if( frequency < upper )
      {
      frequencyIndex = ii;
      break;
      }
    frequencyIndex = ii + 1;
    }
  frequency = supportedFrequencies[frequencyIndex];
  LOG_DEBUG( "Current frequency is " << frequency / 1.0e6 ); 

  if( !hwControls->SetFrequency( frequency ) )
    {
    LOG_ERROR( "Could not set the frequency." );
    return PLUS_FAIL;
    }

  this->ImagingParameters->SetFrequencyMhz( static_cast< double >( frequency / 1.0e6 ) );

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonSDKCxxVideoSource::SetDynRangeDb(double dynRangeDb)
{
  unsigned char usedGain = 100;
  if( dynRangeDb > 0.0 )
    {
    usedGain = static_cast< unsigned char >( 255 * dynRangeDb );
    }
  HWControlsType * hwControls = this->Internal->GetHWControls();
  if( !hwControls->SendDynamic( usedGain ) )
    {
    LOG_ERROR( "Could not set dynamic gain." );
    return PLUS_FAIL;
    }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonSDKCxxVideoSource::AddBmodeFrameToBuffer( BmodePixelType * buffer, void * clientData )
{
  if( !this->Recording )
    {
    // drop the frame, we are not recording data now
    return PLUS_SUCCESS;
    }

  HWControlsType * hwControls = this->Internal->GetHWControls();
  if( hwControls->ReadHardButton() )
    {
    // TODO: add support for sending the button press info through OpenIGTLink
    }

  vtkImageData * bufferVtkImageData = NULL;
  ++this->FrameNumber;

  std::vector<vtkPlusDataSource *> sources;
  vtkPlusDataSource * source = NULL;
  this->GetVideoSourcesByPortName(vtkPlusDevice::BMODE_PORT_NAME, sources);
  if( !sources.empty() )
    {
    source = sources[0];
    }
  else
    {
    LOG_ERROR( "Expected Bmode port not found" );
    return PLUS_FAIL;
    }

  vtkPlusChannel * channel = this->Internal->GetSourceChannel( source );

  vtkRfProcessor * rfProcessor = channel->GetRfProcessor();
  if( rfProcessor != NULL )
    {
    /*
    // currently does not work.
    rfProcessor->SetRfFrame( this->Internal->ConvertBModeBufferToVtkImage(buffer),
                             US_IMG_BRIGHTNESS );
    bufferVtkImageData = rfProcessor->GetBrightnessScanConvertedImage();
    */
    vtkUsScanConvert * scanConverter = rfProcessor->GetScanConverter();
    if( scanConverter != NULL )
      {
      scanConverter->SetInputData_vtk5compatible( this->Internal->ConvertBModeBufferToVtkImage( buffer ) );
      scanConverter->Modified();
      scanConverter->Update();
      bufferVtkImageData = scanConverter->GetOutput();
      }
    }
  else
    {
    bufferVtkImageData = this->Internal->ConvertBModeBufferToVtkImage( buffer );
    }

  const PlusStatus status = source->AddItem( bufferVtkImageData,
    source->GetInputImageOrientation(),
    US_IMG_BRIGHTNESS,
    this->FrameNumber );

  this->Modified();

  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonSDKCxxVideoSource::AddRfFrameToBuffer( RfPixelType * buffer, void * clientData )
{
  if( !this->Recording )
    {
    // drop the frame, we are not recording data now
    return PLUS_SUCCESS;
    }

  HWControlsType * hwControls = this->Internal->GetHWControls();
  if( hwControls->ReadHardButton() )
    {
    // TODO: add support for sending the button press info through OpenIGTLink
    }

  ++this->FrameNumber;

  std::vector<vtkPlusDataSource *> rfSources;
  vtkPlusDataSource * source = NULL;
  this->GetVideoSourcesByPortName(vtkPlusDevice::RFMODE_PORT_NAME, rfSources);
  if( !rfSources.empty() )
    {
    source = rfSources[0];
    }
  else
    {
    LOG_ERROR( "Expected RF port not found" );
    return PLUS_FAIL;
    }

  vtkPlusChannel * channel = this->Internal->GetSourceChannel( source );

  vtkImageData * rfBufferVtkImageData = this->Internal->ConvertRfBufferToVtkImage( buffer );
  if( source->AddItem( rfBufferVtkImageData,
                           source->GetInputImageOrientation(),
                           US_IMG_RF_REAL,
                           this->FrameNumber ) == PLUS_FAIL )
    {
    LOG_ERROR( "Failed to add RF frame to buffer" );
    return PLUS_FAIL;
    }

  std::vector<vtkPlusDataSource *> bmodeSources;
  this->GetVideoSourcesByPortName(vtkPlusDevice::BMODE_PORT_NAME, bmodeSources);
  if( !bmodeSources.empty() )
    {
    source = bmodeSources[0];
    channel = this->Internal->GetSourceChannel( source );
    vtkRfProcessor * rfProcessor = channel->GetRfProcessor();
    if( rfProcessor != NULL )
      {
      rfProcessor->SetRfFrame( rfBufferVtkImageData,
                               US_IMG_RF_REAL );
      rfProcessor->GetRfToBrightnessConverter()->Modified();
      vtkUsScanConvert * scanConverter = rfProcessor->GetScanConverter();
      vtkImageData * bmodeBufferVtkImageData = NULL;
      if( scanConverter != NULL )
        {
        bmodeBufferVtkImageData = rfProcessor->GetBrightnessScanConvertedImage();
        }
      else
        {
        bmodeBufferVtkImageData = rfProcessor->GetBrightnessConvertedImage();
        }
      if( source->AddItem( bmodeBufferVtkImageData,
                               source->GetInputImageOrientation(),
                               US_IMG_BRIGHTNESS,
                               this->FrameNumber ) == PLUS_FAIL )
        {
        LOG_ERROR( "Failed to add BMode frame to buffer." );
        return PLUS_FAIL;
        }
      }
    else
      {
      LOG_ERROR( "Expected RfProcessor not found." );
      return PLUS_FAIL;
      }
    }

  this->Modified();

  return PLUS_SUCCESS;
}
