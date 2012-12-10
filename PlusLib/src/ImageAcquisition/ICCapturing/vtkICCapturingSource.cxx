/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtkICCapturingSource.h"
#include "ICCapturingListener.h"
#include <tisudshl.h>

#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtksys/SystemTools.hxx"
#include "vtkPlusDataBuffer.h"
#include "vtkMultiThreader.h"

#include <ctype.h>

#include <vector>
#include <string>


vtkCxxRevisionMacro(vtkICCapturingSource, "$Revision: 1.0$");

vtkICCapturingSource* vtkICCapturingSource::Instance = 0;
vtkICCapturingSourceCleanup vtkICCapturingSource::Cleanup;

vtkICCapturingSourceCleanup::vtkICCapturingSourceCleanup(){}

vtkICCapturingSourceCleanup::~vtkICCapturingSourceCleanup()
{
  // Destroy any remaining output window.
  vtkICCapturingSource::SetInstance(NULL);
}


//----------------------------------------------------------------------------
vtkICCapturingSource::vtkICCapturingSource()
{
  this->ICBufferSize = 50; 

  this->DeviceName = NULL; 
  this->VideoNorm = NULL; 
  this->VideoFormat = NULL; 
  this->InputChannel = NULL; 

  this->ClipRectangleOrigin[0]=0;
  this->ClipRectangleOrigin[1]=0;
  this->ClipRectangleSize[0]=0;
  this->ClipRectangleSize[1]=0;

  this->FrameGrabber = NULL;
  this->FrameGrabberListener = NULL; 

  this->SetFrameBufferSize(200); 
  this->Buffer->Modified();

  this->Modified();
}

//----------------------------------------------------------------------------
vtkICCapturingSource::~vtkICCapturingSource()
{ 
  this->Disconnect();

  if ( this->FrameGrabber != NULL) 
  {
    delete this->FrameGrabber; 
    this->FrameGrabber = NULL;
  }

  if ( this->FrameGrabberListener != NULL) 
  {
    delete this->FrameGrabberListener; 
    this->FrameGrabberListener = NULL;
  }
}

//----------------------------------------------------------------------------
// Up the reference count so it behaves like New
vtkICCapturingSource* vtkICCapturingSource::New()
{
  vtkICCapturingSource* ret = vtkICCapturingSource::GetInstance();
  ret->Register(NULL);
  return ret;
}

//----------------------------------------------------------------------------
// Return the single instance of the vtkOutputWindow
vtkICCapturingSource* vtkICCapturingSource::GetInstance()
{
  if(!vtkICCapturingSource::Instance)
  {
    // Try the factory first
    vtkICCapturingSource::Instance = (vtkICCapturingSource*)vtkObjectFactory::CreateInstance("vtkICCapturingSource");    
    if(!vtkICCapturingSource::Instance)
    {
      vtkICCapturingSource::Instance = new vtkICCapturingSource();     
    }
    if(!vtkICCapturingSource::Instance)
    {
      int error = 0;
    }
  }
  // return the instance
  return vtkICCapturingSource::Instance;
}

//----------------------------------------------------------------------------
void vtkICCapturingSource::SetInstance(vtkICCapturingSource* instance)
{
  if (vtkICCapturingSource::Instance==instance)
  {
    return;
  }
  // preferably this will be NULL
  if (vtkICCapturingSource::Instance)
  {
    vtkICCapturingSource::Instance->Delete();;
  }
  vtkICCapturingSource::Instance = instance;
  if (!instance)
  {
    return;
  }
  // user will call ->Delete() after setting instance
  instance->Register(NULL);
}

//----------------------------------------------------------------------------
std::string vtkICCapturingSource::GetSdkVersion()
{
  std::ostringstream version; 
  version << "The Imaging Source UDSHL-" << UDSHL_LIB_VERSION_MAJOR << "." << UDSHL_LIB_VERSION_MINOR; 
  return version.str(); 
}

//----------------------------------------------------------------------------
void vtkICCapturingSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

}

//----------------------------------------------------------------------------
// the callback function used when there is a new frame of data received
bool vtkICCapturingSource::vtkICCapturingSourceNewFrameCallback(unsigned char * data, unsigned long size, unsigned long frameNumber)
{    
  if(data==NULL || size==0)
  {
    LOG_ERROR("No actual frame data received from the framegrabber");
    return false;
  }

  vtkICCapturingSource::GetInstance()->AddFrameToBuffer(data, size, frameNumber);    

  return true;
}

//----------------------------------------------------------------------------
// copy the Device Independent Bitmap from the VFW framebuffer into the
// vtkVideoSource framebuffer (don't do the unpacking yet)
PlusStatus vtkICCapturingSource::AddFrameToBuffer(unsigned char * dataPtr, unsigned long size, unsigned long frameNumber)
{
  if (!this->Recording)
  {
    // drop the frame, we are not recording data now
    return PLUS_SUCCESS;
  }

  this->FrameNumber = frameNumber; 

  const int frameSize[2] = {static_cast<DShowLib::Grabber*>(FrameGrabber)->getAcqSizeMaxX(), static_cast<DShowLib::Grabber*>(FrameGrabber)->getAcqSizeMaxY()}; 
  int frameBufferBitsPerPixel = static_cast<DShowLib::Grabber*>(FrameGrabber)->getVideoFormat().getBitsPerPixel(); 
  if (frameBufferBitsPerPixel!=8)
  {
    LOG_ERROR("vtkICCapturingSource::AddFrameToBuffer: only 8-bit acquisition is supported, current frameBufferBitsPerPixel="<<frameBufferBitsPerPixel);
    return PLUS_FAIL;
  }

  PlusStatus status=PLUS_SUCCESS;
  if( (this->ClipRectangleSize[0] > 0) && (this->ClipRectangleSize[1] > 0)
    && (this->ClipRectangleSize[0]<frameSize[0] || this->ClipRectangleSize[1]<frameSize[1]))
  {
    // Clipping
    LimitClippingToValidRegion(frameSize);
    unsigned int bufferSize=this->ClipRectangleSize[0]*this->ClipRectangleSize[1];
    this->ClippedImageBuffer.resize(bufferSize);
    // Copy the pixels from full frame buffer to clipped frame buffer line-by-line
    unsigned char* fullFramePixelPtr=dataPtr+this->ClipRectangleOrigin[1]*frameSize[0]+this->ClipRectangleOrigin[0];
    unsigned char* clippedFramePixelPtr=&(this->ClippedImageBuffer[0]);
    for (int y=0; y<this->ClipRectangleSize[1]; y++)
    {
      memcpy(clippedFramePixelPtr,fullFramePixelPtr,this->ClipRectangleSize[0]);
      clippedFramePixelPtr+=this->ClipRectangleSize[0];
      fullFramePixelPtr+=frameSize[0];
    }
    status = this->Buffer->AddItem(&(this->ClippedImageBuffer[0]), this->GetDeviceImageOrientation(), this->ClipRectangleSize, itk::ImageIOBase::UCHAR, US_IMG_BRIGHTNESS, 0, this->FrameNumber); 
  }
  else
  {
    // No clipping
    status = this->Buffer->AddItem(dataPtr, this->GetDeviceImageOrientation(), frameSize, itk::ImageIOBase::UCHAR, US_IMG_BRIGHTNESS, 0, this->FrameNumber); 
  }
  this->Modified();

  return status;
}

//----------------------------------------------------------------------------
void vtkICCapturingSource::LimitClippingToValidRegion(const int frameSize[2])
{
  if (this->ClipRectangleOrigin[0]<0 || this->ClipRectangleOrigin[1]<0
    || this->ClipRectangleOrigin[0]>=frameSize[0] || this->ClipRectangleOrigin[1]>=frameSize[0])
  {
    LOG_WARNING("ClipRectangleOrigin is invalid ("<<this->ClipRectangleOrigin[0]<<", "<<this->ClipRectangleOrigin[1]<<"). The frame size is "
      <<frameSize[0]<<"x"<<frameSize[1]<<". Using (0,0) as ClipRectangleOrigin.");
    this->ClipRectangleOrigin[0]=0;
    this->ClipRectangleOrigin[1]=0;
  }
  if (this->ClipRectangleOrigin[0]+this->ClipRectangleSize[0]>=frameSize[0])
  {
    // rectangle size is out of the framSize bounds, clip it to the available size
    LOG_WARNING("Adjusting ClipRectangleSize x to "<<this->ClipRectangleSize[0]);
    this->ClipRectangleSize[0]=frameSize[0]-this->ClipRectangleOrigin[0];
  }
  if (this->ClipRectangleOrigin[1]+this->ClipRectangleSize[1]>frameSize[1])
  {
    // rectangle size is out of the framSize bounds, clip it to the available size
    LOG_WARNING("Adjusting ClipRectangleSize y to "<<this->ClipRectangleSize[1]);
    this->ClipRectangleSize[1]=frameSize[1]-this->ClipRectangleOrigin[1];
  }    
}

  
//----------------------------------------------------------------------------
PlusStatus vtkICCapturingSource::InternalConnect()
{
  if( !DShowLib::InitLibrary() )
  {
    LOG_ERROR("The IC capturing library could not be initialized");
    return PLUS_FAIL;
  }

  // Add DShowLib::ExitLibrary to the list of functions that are called on application exit.
  // It is useful because when the application is forced to exit then the destructor may not be called.
  static bool exitFunctionAdded=false;
  if (!exitFunctionAdded)
  {
    atexit( DShowLib::ExitLibrary );
    exitFunctionAdded=true;
  }

  if ( this->FrameGrabber == NULL ) 
  {
    this->FrameGrabber = new DShowLib::Grabber; 
  }

  // Set the device name (e.g. DFG/USB2-lt)
  if ( this->GetDeviceName() == NULL || !static_cast<DShowLib::Grabber*>(FrameGrabber)->openDev(this->GetDeviceName() ) ) 
  {
    LOG_ERROR("The IC capturing library could not be initialized - invalid device name: " << this->GetDeviceName() ); 
    return PLUS_FAIL;
  }

  // Set the video norm (e.g. PAL_B or NTSC_M)
  if ( this->GetVideoNorm() == NULL || !static_cast<DShowLib::Grabber*>(FrameGrabber)->setVideoNorm( this->GetVideoNorm() ) ) 
  {
    LOG_ERROR("The IC capturing library could not be initialized - invalid video norm: " << this->GetVideoNorm() ); 
    return PLUS_FAIL;
  }

  // The Y800 color format is an 8 bit monochrome format. 
  if ( this->GetVideoFormat() == NULL || !static_cast<DShowLib::Grabber*>(FrameGrabber)->setVideoFormat( this->GetVideoFormat() ) )
  {
    LOG_ERROR("The IC capturing library could not be initialized - invalid video format: " << this->GetVideoFormat() ); 
    return PLUS_FAIL;
  }

  int bitsPerPixel=static_cast<DShowLib::Grabber*>(FrameGrabber)->getVideoFormat().getBitsPerPixel();
  if (bitsPerPixel!=8)
  {
    LOG_ERROR("The IC capturing library could not be initialized - invalid bits per pixel: " << bitsPerPixel ); 
    return PLUS_FAIL;    
  }
  this->GetBuffer()->SetPixelType(itk::ImageIOBase::UCHAR );  

  int frameSize[2]={0,0};
  frameSize[0]=static_cast<DShowLib::Grabber*>(FrameGrabber)->getAcqSizeMaxX();
  frameSize[1]=static_cast<DShowLib::Grabber*>(FrameGrabber)->getAcqSizeMaxY();

  if( (this->ClipRectangleSize[0] > 0) && (this->ClipRectangleSize[1] > 0) )
  {
    LimitClippingToValidRegion(frameSize);
    this->GetBuffer()->SetFrameSize(this->ClipRectangleSize);
  }
  else
  {
    // No clipping
    this->GetBuffer()->SetFrameSize(frameSize); 
  }

  if ( this->GetInputChannel() == NULL || !static_cast<DShowLib::Grabber*>(FrameGrabber)->setInputChannel( this->GetInputChannel() ) ) 
  {
    LOG_ERROR("The IC capturing library could not be initialized - invalid input channel: " << this->GetInputChannel() ); 
    return PLUS_FAIL;
  }

  if (this->FrameGrabberListener==NULL)
  {
    this->FrameGrabberListener = new ICCapturingListener(); 
  }

  this->FrameGrabberListener->SetICCapturingSourceNewFrameCallback(vtkICCapturingSource::vtkICCapturingSourceNewFrameCallback); 

  // Assign the number of buffers to the cListener object.
  this->FrameGrabberListener->setBufferSize( this->GetICBufferSize() );

  // Register the FrameGrabberListener object for the frame ready 
  // TODO: check if the listener should be removed (in disconnect, when reconnecting, ...)
  static_cast<DShowLib::Grabber*>(FrameGrabber)->addListener( FrameGrabberListener, DShowLib::GrabberListener::eFRAMEREADY );

  // Create a FrameTypeInfoArray data structure describing the allowed color formats.
  DShowLib::FrameTypeInfoArray acceptedTypes = DShowLib::FrameTypeInfoArray::createRGBArray();

  // Create the frame handler sink: 8 bit monochrome format. 
  smart_ptr<DShowLib::FrameHandlerSink> pSink = DShowLib::FrameHandlerSink::create( DShowLib::eRGB8, 1);
  //smart_ptr<DShowLib::FrameHandlerSink> pSink = DShowLib::FrameHandlerSink::create( DShowLib::eY800, 1);

  // Disable snap mode.
  pSink->setSnapMode( false );

  // Apply the sink to the grabber.
  static_cast<DShowLib::Grabber*>(FrameGrabber)->setSinkType( pSink );

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkICCapturingSource::InternalDisconnect()
{
  LOG_DEBUG("Disconnect from IC capturing");

  static_cast<DShowLib::Grabber*>(FrameGrabber)->removeListener( FrameGrabberListener );
  delete this->FrameGrabberListener; 
  this->FrameGrabberListener=NULL;

  delete FrameGrabber;
  this->FrameGrabber=NULL;

  DShowLib::ExitLibrary(); 

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkICCapturingSource::InternalStartRecording()
{
  if (!static_cast<DShowLib::Grabber*>(FrameGrabber)->startLive(false))
  {
    LOG_ERROR("Framegrabber startLive failed, cannot start the recording");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkICCapturingSource::InternalStopRecording()
{
  if (!static_cast<DShowLib::Grabber*>(FrameGrabber)->stopLive())
  {
    LOG_ERROR("Framegrabber stopLive failed");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkICCapturingSource::ReadConfiguration(vtkXMLDataElement* config)
{
  LOG_TRACE("vtkICCapturingSource::ReadConfiguration"); 
  if ( config == NULL )
  {
    LOG_ERROR("Unable to configure IC Capturing video source! (XML data element is NULL)"); 
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

  const char* deviceName = imageAcquisitionConfig->GetAttribute("DeviceName"); 
  if ( deviceName != NULL) 
  {
    this->SetDeviceName(deviceName); 
  }

  const char* videoNorm = imageAcquisitionConfig->GetAttribute("VideoNorm"); 
  if ( videoNorm != NULL) 
  {
    this->SetVideoNorm(videoNorm); 
  }

  const char* videoFormat = imageAcquisitionConfig->GetAttribute("VideoFormat"); 
  if ( videoFormat != NULL) 
  {
    this->SetVideoFormat(videoFormat); 
  }

  const char* inputChannel = imageAcquisitionConfig->GetAttribute("InputChannel"); 
  if ( inputChannel != NULL) 
  {
    this->SetInputChannel(inputChannel); 
  }

  int icBufferSize = 0; 
  if ( imageAcquisitionConfig->GetScalarAttribute("ICBufferSize", icBufferSize) ) 
  {
    this->SetICBufferSize(icBufferSize); 
  }

  // clipping parameters
  int clipRectangleOrigin[2]={0,0};
  if (imageAcquisitionConfig->GetVectorAttribute("ClipRectangleOrigin", 2, clipRectangleOrigin))
  {
    this->SetClipRectangleOrigin(clipRectangleOrigin);
  }
  int clipRectangleSize[2]={0,0};
  if (imageAcquisitionConfig->GetVectorAttribute("ClipRectangleSize", 2, clipRectangleSize))
  {
    this->SetClipRectangleSize(clipRectangleSize);
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkICCapturingSource::WriteConfiguration(vtkXMLDataElement* config)
{
  // Write superclass configuration
  Superclass::WriteConfiguration(config); 

  if ( config == NULL )
  {
    LOG_ERROR("Config is invalid");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* dataCollectionConfig = config->FindNestedElementWithName("DataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* imageAcquisitionConfig = dataCollectionConfig->FindNestedElementWithName("ImageAcquisition"); 
  if (imageAcquisitionConfig == NULL) 
  {
    LOG_ERROR("Cannot find ImageAcquisition element in XML tree!");
    return PLUS_FAIL;
  }

  imageAcquisitionConfig->SetAttribute("DeviceName", this->DeviceName);
  imageAcquisitionConfig->SetAttribute("VideoNorm", this->VideoNorm);
  imageAcquisitionConfig->SetAttribute("VideoFormat", this->VideoFormat);
  imageAcquisitionConfig->SetAttribute("InputChannel", this->InputChannel);
  imageAcquisitionConfig->SetIntAttribute("ICBufferSize", this->ICBufferSize);

  // clipping parameters
  imageAcquisitionConfig->SetVectorAttribute("ClipRectangleOrigin", 2, this->GetClipRectangleOrigin());
  imageAcquisitionConfig->SetVectorAttribute("ClipRectangleSize", 2, this->GetClipRectangleSize());

  return PLUS_SUCCESS;
}
