/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/



#include "vtkTelemedVideoSource.h"
#include "PlusConfigure.h"

#include "vtkImageData.h"
#include "vtkImageImport.h"
#include "vtkObjectFactory.h"

#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "PixelCodec.h"

vtkStandardNewMacro(vtkTelemedVideoSource); // Corresponds to the implementation of : static vtkTelemedVideoSource *New();  (in .h file)

//----------------------------------------------------------------------------
vtkTelemedVideoSource::vtkTelemedVideoSource()
: FrequencyMhz(5)
, DynRangeValue(80)
, PowerPerCent(60)
, GainPerCent(70)
, DepthMm(50)
{
  this->Device = new TelemedUltrasound();
  this->RequireImageOrientationInConfiguration = true;
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkTelemedVideoSource::~vtkTelemedVideoSource()
{
  delete this->Device;
  this->Device=NULL;
}

//----------------------------------------------------------------------------
void vtkTelemedVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//-----------------------------------------------------------------------------
PlusStatus vtkTelemedVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkTelemedVideoSource::ReadConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, DynRangeDb, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, GainPercent, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, PowerPercent, deviceConfig);  
  
  //XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, DepthMm, deviceConfig);
  //XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, FrequencyMhz, deviceConfig);

  //XML_READ_VECTOR_ATTRIBUTE_OPTIONAL(int, 2, ImageSize, deviceConfig);
  //XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, Intensity, deviceConfig);
  //XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, Contrast, deviceConfig);
  //XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, SoundVelocity, deviceConfig);  

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkTelemedVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  //XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(imageAcquisitionConfig, rootConfigElement);

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkTelemedVideoSource::FreezeDevice(bool freeze)
{
  this->Device->FreezeDevice(freeze);
  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkTelemedVideoSource::InternalConnect()
{
  if(!this->Device->Initialize())
  {
    LOG_ERROR("vtkTelemedVideoSource device initialization failed");
    return PLUS_FAIL;
  }
  this->Device->SetPowerValue(this->PowerPerCent);   //%
  this->Device->SetGainValue(this->GainPerCent);    //%
  this->Device->SetDynRangeValue(this->DynRangeValue); //dB
  this->SetFrequencyMhz(this->FrequencyMhz);
  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkTelemedVideoSource::InternalDisconnect()
{
  LOG_DEBUG("Disconnect from Telemed");
  if(this->Device->Finalize())
  {
    return PLUS_SUCCESS;
  }
  else
  {
    return PLUS_FAIL;
  }
}


//----------------------------------------------------------------------------
PlusStatus vtkTelemedVideoSource::InternalUpdate()
{
  LOG_TRACE( "vtkTelemedVideoSource::InternalUpdate" );
  if (!this->Recording)
  {
    // drop the frame, we are not recording data now
    return PLUS_SUCCESS;
  }

  // Capture one frame from the Telemed device
  unsigned char* bufferData = this->Device->CaptureFrame();
  if(bufferData == NULL)
  {
    LOG_ERROR("No frame received by the device");
    return PLUS_FAIL;
  }

  this->FrameNumber++;

  int frameWidth=this->Device->GetFrameWidth();
  int frameHeight=this->Device->GetFrameHeight();
  int bufferSize=this->Device->GetBufferSize();
  if (frameWidth*frameHeight==0)
  {
    LOG_ERROR("Failed to retrieve valid frame size (got "<<frameWidth<<"x"<<frameHeight);
    return PLUS_FAIL;
  }
  int numberOfScalarComponents=bufferSize/(frameWidth*frameHeight);
  if (numberOfScalarComponents!=3)
  {
    LOG_ERROR("Unexpected number of scalar components: "<<numberOfScalarComponents);
    return PLUS_FAIL;
  }
  PixelCodec::PixelEncoding encoding = PixelCodec::PixelEncoding_BGR24;

  int frameSizeInPix[3]={frameWidth,frameHeight,1};

  // Retrieve the video source in Telemed device
  vtkPlusDataSource* aSource=NULL;
  if( this->GetFirstActiveOutputVideoSource(aSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve the video source in the Telemed device.");
    return PLUS_FAIL;
  }

  // If the buffer is empty, set the pixel type and frame size to the first received properties
  if ( aSource->GetNumberOfItems() == 0 )
  {
    LOG_DEBUG("Set up image buffer for Telemed");
    aSource->SetPixelType(VTK_UNSIGNED_CHAR);
    aSource->SetImageType(US_IMG_BRIGHTNESS);
    aSource->SetInputFrameSize(frameSizeInPix);
    LOG_DEBUG("Frame size: " << frameSizeInPix[0] << "x" << frameSizeInPix[1]
      << ", pixel type: " << vtkImageScalarTypeNameMacro(aSource->GetPixelType())
      << ", buffer image orientation: " << PlusVideoFrame::GetStringFromUsImageOrientation(aSource->GetInputImageOrientation()));
    this->UncompressedVideoFrame.SetImageType(aSource->GetImageType());
    this->UncompressedVideoFrame.SetImageOrientation(aSource->GetInputImageOrientation());
  }

  PlusStatus decodingStatus=PLUS_FAIL;
  if( aSource->GetImageType() == US_IMG_RGB_COLOR )
  {
    this->UncompressedVideoFrame.AllocateFrame(frameSizeInPix, VTK_UNSIGNED_CHAR, 3);
    decodingStatus = PixelCodec::ConvertToBmp24(PixelCodec::ComponentOrder_RGB, encoding, frameSizeInPix[0], frameSizeInPix[1], bufferData, (unsigned char*)this->UncompressedVideoFrame.GetScalarPointer());
  }
  else
  {
    this->UncompressedVideoFrame.AllocateFrame(frameSizeInPix, VTK_UNSIGNED_CHAR, 1);
    decodingStatus = PixelCodec::ConvertToGray(encoding, frameSizeInPix[0], frameSizeInPix[1], bufferData, (unsigned char*)this->UncompressedVideoFrame.GetScalarPointer());
  }
  if (decodingStatus != PLUS_SUCCESS)
  {
    LOG_ERROR("Error while decoding the grabbed image");
    return PLUS_FAIL;
  }

  // Add the frame to the stream buffer
  PlusStatus status = aSource->AddItem(&this->UncompressedVideoFrame, this->FrameNumber);
  this->Modified();

  return status;
}


/*********** PARAMETERS *************/

//----------------------------------------------------------------------------
void vtkTelemedVideoSource::SetDepthMm(long DepthMm)
{
  this->DepthMm=DepthMm;
  //if (this->Connected)
  //  this->Device->SetDepth(DepthMm);
}

//----------------------------------------------------------------------------
void vtkTelemedVideoSource::SetGainPercent(int GainPerCent)
{
  this->GainPerCent=GainPerCent;
  if (this->Connected)
    this->Device->SetGainValue(GainPerCent);
}

//----------------------------------------------------------------------------
void vtkTelemedVideoSource::SetPowerPercent(int PowerPerCent)
{
  this->PowerPerCent=PowerPerCent;
  if (this->Connected)
    this->Device->SetPowerValue(PowerPerCent);
}

//----------------------------------------------------------------------------
void vtkTelemedVideoSource::SetDynRangeDb(int DynRangeValue)
{
  this->DynRangeValue=DynRangeValue;
  if (this->Connected)
	this->Device->SetDynRangeValue(DynRangeValue);
}

void vtkTelemedVideoSource::SetFrequencyMhz(int FrequencyMhz)
{
  this->FrequencyMhz=FrequencyMhz;
  if (this->Connected) //enumerate frequencies, then set the closest one
  {
	//this->Device->SetFreqIndex(0);
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkTelemedVideoSource::NotifyConfigured()
{
  if( this->OutputChannels.size() > 1 )
  {
    LOG_WARNING("vtkTelemedVideoSource is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
  }

  if( this->OutputChannels.empty() )
  {
    LOG_ERROR("No output channels defined for vtkTelemedVideoSource. Cannot proceed." );
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
std::string vtkTelemedVideoSource::GetSdkVersion()
{
  std::ostringstream versionString;
  versionString << "Telemed version unknown" << std::ends; 
  return versionString.str();
}
