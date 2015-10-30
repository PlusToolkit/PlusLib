/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtkTelemedVideoSource.h"

#include "vtkImageData.h"
#include "vtkImageImport.h"
#include "vtkObjectFactory.h"

#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "PixelCodec.h"

vtkStandardNewMacro(vtkTelemedVideoSource); // Corresponds to the implementation of : static vtkTelemedVideoSource *New();  (in .h file)

//----------------------------------------------------------------------------
vtkTelemedVideoSource::vtkTelemedVideoSource()
: FrequencyMhz(-1)
, DepthMm(-1)
, GainPercent(-1)
, DynRangeDb(-1)
, PowerPercent(-1)
, ConnectedToDevice(false)
{
  this->FrameSize[0]=512;
  this->FrameSize[1]=512;
  this->FrameSize[2]=1; // just in case if the frame size is passed to a method that expects a 3D frame size

  this->Device = NULL;
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
  XML_READ_VECTOR_ATTRIBUTE_OPTIONAL(int, 2, FrameSize, deviceConfig);

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, DepthMm, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, FrequencyMhz, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, DynRangeDb, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, GainPercent, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, PowerPercent, deviceConfig);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkTelemedVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
  deviceConfig->SetDoubleAttribute("DepthMm", this->DepthMm);
  deviceConfig->SetDoubleAttribute("FrequencyMhz", this->FrequencyMhz);
  deviceConfig->SetDoubleAttribute("DynRangeDb", this->DynRangeDb);
  deviceConfig->SetDoubleAttribute("GainPercent", this->GainPercent);
  deviceConfig->SetDoubleAttribute("PowerPercent", this->PowerPercent);
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
  if (this->Device==NULL)
  {
    this->Device = new TelemedUltrasound();
  }
  this->Device->SetMaximumFrameSize(this->FrameSize);
  if(this->Device->Connect()!=PLUS_SUCCESS)
  {
    LOG_ERROR("vtkTelemedVideoSource device initialization failed");
    this->ConnectedToDevice = false;
    return PLUS_FAIL;
  }
  this->ConnectedToDevice = true;

  if (this->FrequencyMhz>0) {SetFrequencyMhz(this->FrequencyMhz); }
  if (this->DepthMm>0) {SetDepthMm(this->DepthMm); }
  if (this->GainPercent>=0) {SetGainPercent(this->GainPercent); }
  if (this->DynRangeDb>0) {SetDynRangeDb(this->DynRangeDb); }
  if (this->PowerPercent>=0) {SetPowerPercent(this->PowerPercent); }

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkTelemedVideoSource::InternalDisconnect()
{
  LOG_DEBUG("Disconnect from Telemed");
  this->Device->Disconnect();
  delete this->Device;
  this->Device = NULL;
  this->ConnectedToDevice = false;
  return PLUS_SUCCESS;
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

  int frameSizeInPix[3]={0,0,1};
  this->Device->GetFrameSize(frameSizeInPix);
  int bufferSize=this->Device->GetBufferSize();
  if (frameSizeInPix[0]*frameSizeInPix[1]==0)
  {
    LOG_ERROR("Failed to retrieve valid frame size (got "<<frameSizeInPix[0]<<"x"<<frameSizeInPix[1]);
    return PLUS_FAIL;
  }
  int numberOfScalarComponents=bufferSize/(frameSizeInPix[0]*frameSizeInPix[1]);

  PixelCodec::PixelEncoding encoding = PixelCodec::PixelEncoding_BGR24;
  PixelCodec::ComponentOrdering componentOrdering = PixelCodec::ComponentOrder_RGB;
  if (numberOfScalarComponents==3)
  {
    encoding = PixelCodec::PixelEncoding_BGR24;
    componentOrdering = PixelCodec::ComponentOrder_RGB;
  }
  else if (numberOfScalarComponents==4)
  {
    encoding = PixelCodec::PixelEncoding_RGBA32;
    componentOrdering = PixelCodec::ComponentOrder_RGBA;
  }
  else
  {
    LOG_ERROR("Unexpected number of scalar components: "<<numberOfScalarComponents);
    return PLUS_FAIL;
  }

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
    decodingStatus = PixelCodec::ConvertToBmp24(componentOrdering, encoding, frameSizeInPix[0], frameSizeInPix[1], bufferData, (unsigned char*)this->UncompressedVideoFrame.GetScalarPointer());
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

#define IMAGING_PARAMETER_SET(parameterName) \
PlusStatus vtkTelemedVideoSource::Set##parameterName(double a##parameterName) \
{ \
  if (this->Device==NULL) \
  { \
    /* Connection has not been established yet. Parameter value will be set upon connection. */ \
    this->parameterName=a##parameterName; \
    return PLUS_SUCCESS; \
  } \
  if (this->Device->Set##parameterName(this->parameterName)!=PLUS_SUCCESS) \
  { \
    LOG_ERROR("vtkTelemedVideoSource parameter setting failed: "<<parameterName<<"="<<a##parameterName); \
    return PLUS_FAIL; \
  } \
  this->parameterName=a##parameterName; \
  return PLUS_SUCCESS; \
}

#define IMAGING_PARAMETER_GET(parameterName) \
PlusStatus vtkTelemedVideoSource::Get##parameterName(double &a##parameterName) \
{ \
  if (this->Device==NULL) \
  { \
    /* Connection has not been established yet. Return cached parameter value. */ \
    a##parameterName=this->parameterName; \
    return PLUS_SUCCESS; \
  } \
  if (this->Device->Get##parameterName(this->parameterName)!=PLUS_SUCCESS) \
  { \
    LOG_ERROR("vtkTelemedVideoSource parameter setting failed: "<<parameterName<<"="<<a##parameterName); \
    return PLUS_FAIL; \
  } \
  a##parameterName=this->parameterName; \
  return PLUS_SUCCESS; \
}

IMAGING_PARAMETER_GET(FrequencyMhz);
IMAGING_PARAMETER_GET(DepthMm);
IMAGING_PARAMETER_GET(GainPercent);
IMAGING_PARAMETER_GET(DynRangeDb);
IMAGING_PARAMETER_GET(PowerPercent);

IMAGING_PARAMETER_SET(FrequencyMhz);
IMAGING_PARAMETER_SET(DepthMm);
IMAGING_PARAMETER_SET(GainPercent);
IMAGING_PARAMETER_SET(DynRangeDb);
IMAGING_PARAMETER_SET(PowerPercent);

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

//----------------------------------------------------------------------------
PlusStatus vtkTelemedVideoSource::SetFrameSize(int frameSize[2])
{
  this->FrameSize[0]=frameSize[0];
  this->FrameSize[1]=frameSize[1];
  return PLUS_SUCCESS;
}
