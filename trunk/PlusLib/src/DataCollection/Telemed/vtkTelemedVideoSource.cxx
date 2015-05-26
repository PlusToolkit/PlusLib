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

vtkStandardNewMacro(vtkTelemedVideoSource); // Corresponds to the implementation of : static vtkTelemedVideoSource *New();  (in .h file)

//----------------------------------------------------------------------------
vtkTelemedVideoSource::vtkTelemedVideoSource()
{
  device = new TelemedUltrasound();
  this->RequireImageOrientationInConfiguration = true;
  this->StartThreadForInternalUpdates = true;
  CreateDefaultOutputChannel(true);
  FrequencyMhz=5;
  DynRangeValue=80;
  PowerPerCent=60;
  GainPerCent=70;
  DepthMm=50;
}

//----------------------------------------------------------------------------
vtkTelemedVideoSource::~vtkTelemedVideoSource()
{
  device->~TelemedUltrasound();
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
  vtkXMLDataElement* dataCollection = rootConfigElement->FindNestedElementWithName("DataCollection");
  vtkXMLDataElement* deviceConfig = dataCollection->FindNestedElementWithNameAndAttribute("Device", "Type", "TelemedVideo");
  if (deviceConfig == NULL)
  {
    LOG_ERROR("Internal error: If TelemedVideo device node does not exist in the configuration file, why is this method invoked?");
    return PLUS_FAIL;
  }

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
/*! Connect to device. Connection is needed for recording or single frame acquisition */
PlusStatus vtkTelemedVideoSource::Connect()
{
  PlusStatus status = InternalConnect();
  //device->SetDepth(this->DepthMm);
  device->SetPowerValue(this->PowerPerCent);   //%
  device->SetGainValue(this->GainPerCent);    //%
  device->SetDynRangeValue(this->DynRangeValue); //dB
  this->SetFrequencyMhz(this->FrequencyMhz);

  return status;
}

//----------------------------------------------------------------------------
/*!  Disconnect from device.
This method must be called before application exit, or else the
application might hang during exit.  */
PlusStatus vtkTelemedVideoSource::Disconnect()
{
  return InternalDisconnect();
}


//----------------------------------------------------------------------------
PlusStatus vtkTelemedVideoSource::FreezeDevice(bool freeze)
{
  device->FreezeDevice(freeze);
  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkTelemedVideoSource::InternalConnect()
{
  if(device->Initialize())
  {
    this->Connected=1;
    return PLUS_SUCCESS;
  }
  else
  {
    this->Connected=0;
    return PLUS_FAIL;
  }
}


//----------------------------------------------------------------------------
PlusStatus vtkTelemedVideoSource::InternalDisconnect()
{
  LOG_DEBUG("Disconnect from Telemed");
  if(device->Finalize())
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
    std::cout<<"Recording variable is false"<<std::endl; // drop the frame, we are not recording data now
    return PLUS_SUCCESS;
  }

  // Capture one frame from the Telemed device
  unsigned char * rawFrame = this->device->CaptureFrame();
  int bufferSize=this->device->GetBufferSize();
  int frameWidth=this->device->GetFrameWidth();
  int frameHeight=this->device->GetFrameHeight();
  int frameDepth=bufferSize/(frameWidth*frameHeight);

  if(rawFrame == NULL)
  {
    std::cout<<"No frame received by the device"<<std::endl;
    return PLUS_FAIL;
  }

  unsigned char * mono = new unsigned char[frameWidth*frameHeight];
  for(int i=0;i<bufferSize/3;i=i++)
  {
    mono[i]=rawFrame[3*i];
  }

  this->FrameNumber++;

  // Retrieve the video source in Telemed device
  vtkPlusDataSource* aSource=NULL;
  if( this->GetFirstActiveOutputVideoSource(aSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve the video source in the Telemed device.");
    return PLUS_FAIL;
  }

  int frameSizeInPx[3]={frameWidth,frameHeight,1};

  // If the buffer is empty, set the pixel type and frame size to the first received properties
  if ( aSource->GetNumberOfItems() == 0 )
  {
    LOG_DEBUG("Set up image buffer for Telemed");
    aSource->SetPixelType(VTK_UNSIGNED_CHAR);
    aSource->SetImageType(US_IMG_BRIGHTNESS);
    aSource->SetInputFrameSize(frameSizeInPx);
    aSource->SetOutputImageOrientation(US_IMG_ORIENT_MF);
  }

  // Add the frame to the stream buffer
  PlusStatus status = aSource->AddItem(mono, aSource->GetInputImageOrientation(), frameSizeInPx,
                                                    VTK_UNSIGNED_CHAR, 1,US_IMG_BRIGHTNESS, 0, this->FrameNumber,
                                                    UNDEFINED_TIMESTAMP, UNDEFINED_TIMESTAMP, NULL);
  this->Modified();

  delete rawFrame;
  delete mono;

  return status;
}


/*********** PARAMETERS *************/

//----------------------------------------------------------------------------
void vtkTelemedVideoSource::SetDepthMm(long DepthMm)
{
  this->DepthMm=DepthMm;
  //if (this->Connected)
  //  this->device->SetDepth(DepthMm);
}

//----------------------------------------------------------------------------
void vtkTelemedVideoSource::SetGainPercent(int GainPerCent)
{
  this->GainPerCent=GainPerCent;
  if (this->Connected)
    this->device->SetGainValue(GainPerCent);
}

//----------------------------------------------------------------------------
void vtkTelemedVideoSource::SetPowerPercent(int PowerPerCent)
{
  this->PowerPerCent=PowerPerCent;
  if (this->Connected)
    this->device->SetPowerValue(PowerPerCent);
}

//----------------------------------------------------------------------------
void vtkTelemedVideoSource::SetDynRangeDb(int DynRangeValue)
{
  this->DynRangeValue=DynRangeValue;
  if (this->Connected)
	this->device->SetDynRangeValue(DynRangeValue);
}

void vtkTelemedVideoSource::SetFrequencyMhz(int FrequencyMhz)
{
  this->FrequencyMhz=FrequencyMhz;
  if (this->Connected) //enumerate frequencies, then set the closest one
  {
	//this->device->SetFreqIndex(0);
  }
}