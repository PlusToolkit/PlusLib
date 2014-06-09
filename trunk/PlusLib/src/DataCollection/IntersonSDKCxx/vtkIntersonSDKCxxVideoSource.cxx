/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkIntersonSDKCxxVideoSource.h"

#include "vtkImageData.h"
#include "vtkObjectFactory.h"

#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusBuffer.h"

#include "vtkUSImagingParameters.h"

#include "IntersonCxxImagingScan2DClass.h"
#include "IntersonCxxControlsHWControls.h"
#include "IntersonCxxIntersonClass.h"

#include "itkImage.h"

typedef IntersonCxx::Imaging::Scan2DClass Scan2DClassType;
typedef IntersonCxx::Controls::HWControls HWControlsType;
typedef IntersonCxx::IntersonClass        IntersonClassType;

vtkCxxRevisionMacro(vtkIntersonSDKCxxVideoSource, "$Revision: 1.0$");
vtkStandardNewMacro(vtkIntersonSDKCxxVideoSource);

const unsigned int Dimension = 3;
typedef Scan2DClassType::BmodePixelType         BmodePixelType;
typedef itk::Image< BmodePixelType, Dimension > BmodeImageType;

struct BmodeCallbackClientData
{
  BmodeImageType *   Image;
};


//----------------------------------------------------------------------------
void __stdcall pasteIntoBmodeImage( BmodePixelType * buffer, void * clientData )
{
  BmodeCallbackClientData * callbackClientData = static_cast< BmodeCallbackClientData * >( clientData );
  BmodeImageType * image = callbackClientData->Image;

  const BmodeImageType::RegionType & largestRegion = image->GetLargestPossibleRegion();
  const BmodeImageType::SizeType imageSize = largestRegion.GetSize();

  const int maxVectors = Scan2DClassType::MAX_VECTORS;
  const int maxSamples = Scan2DClassType::MAX_SAMPLES;
  const int framePixels = maxVectors * maxSamples;

  BmodePixelType * imageBuffer = image->GetPixelContainer()->GetBufferPointer();
  std::memcpy( imageBuffer, buffer, framePixels * sizeof( BmodePixelType ) );
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

    const int maxVectors = Scan2DClassType::MAX_VECTORS;
    const int maxSamples = Scan2DClassType::MAX_SAMPLES;

    this->BmodeImage = BmodeImageType::New();
    typedef BmodeImageType::RegionType BmodeRegionType;
    BmodeRegionType bmodeRegion;
    BmodeRegionType::IndexType bmodeIndex;
    bmodeIndex.Fill( 0 );
    bmodeRegion.SetIndex( bmodeIndex );
    BmodeRegionType::SizeType bmodeSize;
    bmodeSize[0] = maxSamples;
    bmodeSize[1] = maxVectors;
    bmodeRegion.SetSize( bmodeSize );
    this->BmodeImage->SetRegions( bmodeRegion );
    this->BmodeImage->Allocate();

    this->BmodeClientData.Image = this->BmodeImage.GetPointer();
    this->Scan2DClass->SetNewBmodeImageCallback( &pasteIntoBmodeImage,
      &(this->BmodeClientData) );
  }

  //----------------------------------------------------------------------------
  vtkIntersonSDKCxxVideoSource::vtkInternal::~vtkInternal() 
  {    
    this->External = NULL;
    delete HWControls;
    delete Scan2DClass;
    delete IntersonClass;
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

  void * GetBmodeBuffer()
  {
    return this->BmodeImage->GetPixelContainer()->GetBufferPointer();
  }

private:
  HWControlsType *    HWControls;
  Scan2DClassType *   Scan2DClass;
  IntersonClassType * IntersonClass;

  BmodeImageType::Pointer BmodeImage;
  BmodeCallbackClientData BmodeClientData;
};


//----------------------------------------------------------------------------
vtkIntersonSDKCxxVideoSource::vtkIntersonSDKCxxVideoSource()
{
  this->Internal = new vtkInternal(this);

  this->RequireImageOrientationInConfiguration = true;
  this->RequireFrameBufferSizeInDeviceSetConfiguration = true;
  this->RequireAcquisitionRateInDeviceSetConfiguration = false;
  this->RequireAveragedItemsForFilteringInDeviceSetConfiguration = false;
  this->RequireLocalTimeOffsetSecInDeviceSetConfiguration = false;
  this->RequireUsImageOrientationInDeviceSetConfiguration = true;
  this->RequireRfElementInDeviceSetConfiguration = false;

  this->ImagingParameters = new vtkUsImagingParameters(this);

  this->PulseVoltage = 50;
}

//----------------------------------------------------------------------------
vtkIntersonSDKCxxVideoSource::~vtkIntersonSDKCxxVideoSource()
{ 
  if( !this->Connected )
  {
    this->Disconnect();
  }

  delete this->Internal;
  this->Internal=NULL;
}

//----------------------------------------------------------------------------
void vtkIntersonSDKCxxVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
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

  double depth = -1;
  this->ImagingParameters->GetDepthMm( depth );
  this->SetDepthMm( depth );

  double frequency = -1;
  if( this->SetProbeFrequency( frequency ) == PLUS_FAIL )
    {
    return PLUS_FAIL;
    }

  if( !hwControls->SendHighVoltage( this->PulseVoltage ) )
    {
    LOG_ERROR( "Could not set the high voltage." );
    return PLUS_FAIL;
    }
  if( !hwControls->EnableHighVoltage() )
    {
    LOG_ERROR( "Could not enable high voltage." );
    return PLUS_FAIL;
    }

  vtkPlusDataSource* source = NULL;
  if( this->GetFirstActiveOutputVideoSource( source ) != PLUS_SUCCESS )
    {
    LOG_ERROR("Unable to retrieve the video source in the IntersonVideo device.");
    return PLUS_FAIL;
    }
  
  // Clear buffer on connect because the new frames that we will acquire might have a different size 
  vtkPlusBuffer * plusBuffer = source->GetBuffer();
  plusBuffer->Clear();
  plusBuffer->SetPixelType( VTK_UNSIGNED_CHAR );  
  plusBuffer->SetFrameSize( Scan2DClassType::MAX_SAMPLES, Scan2DClassType::MAX_VECTORS ); 

  double dynamicRangeDb = -1;
  this->ImagingParameters->GetDynRangeDb( dynamicRangeDb );
  if( this->SetDynRangeDb( dynamicRangeDb ) == PLUS_FAIL )
    {
    return PLUS_FAIL;
    }

  // TODO: use hardware button
  hwControls->DisableHardButton();

  Scan2DClassType * scan2D = this->Internal->GetScan2DClass();
  scan2D->SetRFData( false );

  LOG_DEBUG( "Interson SDK version " << this->Internal->GetSdkVersion() <<
             ", USB probe FPGA version " << hwControls->ReadFPGAVersion() );

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkIntersonSDKCxxVideoSource::InternalDisconnect()
{
  LOG_DEBUG("Disconnect from Interson");

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

  scan2D->StartReadScan();
  Sleep( 100 ); // "time to start"

  if( !hwControls->StartBmode() )
    {
    LOG_ERROR( "Could not start B-mode collection." );
    return PLUS_FAIL;
    };

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

  this->FrameNumber++; 

  vtkPlusDataSource* source = NULL;
  if( this->GetFirstActiveOutputVideoSource( source ) != PLUS_SUCCESS )
    {
    LOG_ERROR("Unable to retrieve the video source in the ICCapturing device.");
    return PLUS_FAIL;
    }

  int frameSizeInPx[2] = { Scan2DClassType::MAX_SAMPLES,
                                 Scan2DClassType::MAX_VECTORS };

  vtkPlusBuffer * plusBuffer = source->GetBuffer();
  // If the buffer is empty, set the pixel type and frame size to the first received properties 
  if ( plusBuffer->GetNumberOfItems() == 0 )
    {
    LOG_DEBUG("Set up Plus image buffer");
    plusBuffer->SetPixelType( VTK_UNSIGNED_CHAR );
    plusBuffer->SetImageType( US_IMG_BRIGHTNESS );
    plusBuffer->SetFrameSize( frameSizeInPx );
    plusBuffer->SetImageOrientation( US_IMG_ORIENT_FU );

    LOG_INFO("Frame size: " << frameSizeInPx[0] << "x" << frameSizeInPx[1]
          << ", pixel type: " << vtkImageScalarTypeNameMacro( plusBuffer->GetPixelType() )
          << ", device image orientation: "
            << PlusVideoFrame::GetStringFromUsImageOrientation( source->GetPortImageOrientation() )
          << ", buffer image orientation: "
            << PlusVideoFrame::GetStringFromUsImageOrientation( plusBuffer->GetImageOrientation() ));
    }
  
  if( plusBuffer->AddItem(  this->Internal->GetBmodeBuffer(),
                            source->GetPortImageOrientation(),
                            frameSizeInPx,
                            VTK_UNSIGNED_CHAR,
                            1,
                            US_IMG_BRIGHTNESS,
                            0,
                            this->FrameNumber ) != PLUS_SUCCESS )
    {
    LOG_ERROR( "Error adding item to video source " << source->GetSourceId() );
    return PLUS_FAIL;
    }

  this->Modified();
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

  double depthMm = -1; 
  if ( deviceConfig->GetScalarAttribute("DepthMm", depthMm)) 
  {
    this->ImagingParameters->SetDepthMm(depthMm); 
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkIntersonSDKCxxVideoSource::WriteConfiguration(vtkXMLDataElement* config)
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

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkIntersonSDKCxxVideoSource::NotifyConfigured()
{
  if( this->OutputChannels.size() > 1 )
  {
    LOG_WARNING("vtkIntersonSDKCxxVideoSource is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
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
PlusStatus vtkIntersonSDKCxxVideoSource::SetProbeFrequency(double freq)
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
  LOG_TRACE( "Current frequency is " << frequency / 1.0e6 ); 

  if( !hwControls->SetFrequency( frequency ) )
    {
    LOG_ERROR( "Could not set the frequency." );
    return PLUS_FAIL;
    }

  this->ImagingParameters->SetFrequencyMhz( static_cast< double >( frequency / 1.0e6 ) );

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonSDKCxxVideoSource::SetDepthMm(double depthMm)
{
  int depth = static_cast< int >( depthMm );

  HWControlsType * hwControls = this->Internal->GetHWControls();
  // Since we are not using their scan converter, this does not have much effect
  // other than limiting to the maximum valid depth.
  depth = hwControls->ValidDepth( depth );

  this->ImagingParameters->SetDepthMm( static_cast< double >( depth ) );

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonSDKCxxVideoSource::SetFrequencyMhz(double freq)
{
  return this->SetProbeFrequency( freq );
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonSDKCxxVideoSource::SetDynRangeDb(double dynRangeDb)
{
  if( dynRangeDb > 0.0 )
    {
    const unsigned char usedGain = static_cast< unsigned char >( 255 * dynRangeDb );
    HWControlsType * hwControls = this->Internal->GetHWControls();
    if( !hwControls->SendDynamic( usedGain ) )
      {
      LOG_ERROR( "Could not set dynamic gain." );
      return PLUS_FAIL;
      }
    }
  return PLUS_SUCCESS;
}
