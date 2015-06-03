/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "frmgrab.h"
#include "vtkEpiphanVideoSource.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "PixelCodec.h"
#include "vtksys/SystemTools.hxx"

vtkStandardNewMacro(vtkEpiphanVideoSource);

//----------------------------------------------------------------------------
vtkEpiphanVideoSource::vtkEpiphanVideoSource()
: GrabberLocation(NULL)
, CropRectangle(NULL)
{
  this->ClipRectangleOrigin[0]=0;
  this->ClipRectangleOrigin[1]=0;
  this->ClipRectangleSize[0]=0;
  this->ClipRectangleSize[1]=0;

  this->FrameSize[0]=0;
  this->FrameSize[1]=0;
  this->FrameSize[2]=1;

  this->RequireImageOrientationInConfiguration = true;

  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates=true;
  this->AcquisitionRate = 30;
}

//----------------------------------------------------------------------------
vtkEpiphanVideoSource::~vtkEpiphanVideoSource()
{ 
  if( !this->Connected )
  {
    this->Disconnect();
  }

  if ( this->FrameGrabber != NULL) 
  {
    FrmGrab_Deinit();
    this->FrameGrabber = NULL;
  }

  if( this->CropRectangle != NULL )
  {
    delete this->CropRectangle;
    this->CropRectangle = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkEpiphanVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

}

//----------------------------------------------------------------------------
PlusStatus vtkEpiphanVideoSource::InternalConnect()
{
  LOG_TRACE( "vtkEpiphanVideoSource::InternalConnect" );

  // Initialize frmgrab library
  FrmGrabNet_Init();

  if ( this->GrabberLocation != NULL )
  {
    if ( (this->FrameGrabber = FrmGrab_Open( this->GrabberLocation )) == NULL ) 
    {      
      if ( (this->FrameGrabber = FrmGrabLocal_Open()) == NULL )
      {
        LOG_ERROR("Epiphan Device found");
        return PLUS_FAIL;
      }
      const char UNKNOWN_DEVICE[]="UNKNOWN";
      const char* connectedTo=FrmGrab_GetLocation((FrmGrabber*)this->FrameGrabber);      
      if (connectedTo==NULL)
      {
        connectedTo=UNKNOWN_DEVICE;
      }

      LOG_WARNING("Epiphan Device with the requested location '"<<this->GrabberLocation<<"' not found. Connected to " << connectedTo << " device instead.");
    }
  }
  else
  {
    LOG_DEBUG("Serial Number not specified. Looking for any available device");
    if ( (this->FrameGrabber = FrmGrabLocal_Open()) == NULL )
    {
      LOG_ERROR("Epiphan Device Not found");
      return PLUS_FAIL;
    }
  }

  V2U_VideoMode vm;
  if (!FrmGrab_DetectVideoMode((FrmGrabber*)this->FrameGrabber,&vm)) 
  {
    LOG_ERROR("No signal detected");
    return PLUS_FAIL;
  }

  double maxPossibleAcquisitionRate=vm.vfreq/1000;
  if (this->GetAcquisitionRate()>maxPossibleAcquisitionRate)
  {
    this->SetAcquisitionRate(maxPossibleAcquisitionRate);
  }
  if (vm.width==0 || vm.height==0)
  {
    LOG_ERROR("No valid signal detected. Invalid frame size is received from the framegrabber: "<<vm.width<<"x"<<vm.height);
    return PLUS_FAIL;
  }
  this->FrameSize[0] = vm.width;
  this->FrameSize[1] = vm.height;  

  if( (this->ClipRectangleSize[0] > 0) && (this->ClipRectangleSize[1] > 0) )
  {
    if (this->ClipRectangleSize[0]%4!=0)
    {
      LOG_WARNING("ClipRectangleSize[0] is not a multiple of 4. Acquired image may be skewed.");
    }
    if (this->ClipRectangleOrigin[0]+this->ClipRectangleSize[0]>vm.width || this->ClipRectangleOrigin[1]+this->ClipRectangleSize[1]>vm.height)
    {
      LOG_ERROR("Invalid clip rectangle: rectangle does not fit into the image. Clip rectangle origin: ("<<this->ClipRectangleOrigin[0]<<","<<this->ClipRectangleOrigin[1]
      <<"), size: "<<this->ClipRectangleSize[0]<<","<<this->ClipRectangleSize[1]<<". Image size: "<<vm.width<<"x"<<vm.height);
      return PLUS_FAIL;
    }
    this->FrameSize[0] = this->ClipRectangleSize[0];
    this->FrameSize[1] = this->ClipRectangleSize[1];
  }

  if( this->GetNumberOfVideoSources() == 1 )
  {
    vtkPlusDataSource* aSource(NULL);
    if( this->GetFirstVideoSource(aSource) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to retrieve the video source in the Epiphan device on channel " << (*this->OutputChannels.begin())->GetChannelId());
      return PLUS_FAIL;
    }
    else
    {
      US_IMAGE_TYPE imageType = aSource->GetImageType();
      aSource->SetPixelType(VTK_UNSIGNED_CHAR);
      aSource->SetNumberOfScalarComponents(imageType == US_IMG_RGB_COLOR ? 3 : 1);
      aSource->SetInputFrameSize(this->FrameSize);
    }
  }
  else
  {
    // Can only be 1 or 2, so we must have two video sources, most likely for biplane configuration
    vtkPlusDataSource* aSource(NULL);
    for( int i = 0; i < this->GetNumberOfVideoSources(); ++i )
    {
      if( this->GetVideoSourceByIndex(i, aSource) != PLUS_SUCCESS )
      {
        LOG_ERROR("Unable to retrieve the video source in the Epiphan device on channel " << (*this->OutputChannels.begin())->GetChannelId());
        return PLUS_FAIL;
      }

      US_IMAGE_TYPE imageType = aSource->GetImageType();
      aSource->SetPixelType(VTK_UNSIGNED_CHAR);
      aSource->SetNumberOfScalarComponents(imageType == US_IMG_RGB_COLOR ? 3 : 1);
      aSource->SetInputFrameSize(this->FrameSize);
    }
  }

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkEpiphanVideoSource::InternalDisconnect()
{
  LOG_DEBUG( "vtkEpiphanVideoSource::InternalDisconnect" );

  if( this->Recording )
  {
    if( this->StopRecording() != PLUS_SUCCESS )
    {
      LOG_WARNING(this->GetDeviceId() << ": Unable to stop recording.");
    }
  }

  if (this->FrameGrabber != NULL) {
    FrmGrab_Close((FrmGrabber*)this->FrameGrabber);
  }
  this->FrameGrabber = NULL;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkEpiphanVideoSource::InternalStartRecording()
{
  if( !FrmGrab_Start((FrmGrabber*)this->FrameGrabber) )
  {
    LOG_ERROR(this->GetDeviceId() << ": Unable to frame grabber.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkEpiphanVideoSource::InternalStopRecording()
{
  FrmGrab_Stop((FrmGrabber*)this->FrameGrabber);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkEpiphanVideoSource::InternalUpdate()
{
  if (!this->Recording)
  {
    // drop the frame, we are not recording data now
    return PLUS_SUCCESS;
  }

  V2U_GrabFrame2 * frame = NULL;


  // If someone ever wants RGB8 or YUY2 (etc...) this line will have to be changed
  // to support any future video format choices
  // ReadConfiguration will probably need a new flag to tell this line what to do
  V2U_UINT32 videoFormat = (this->CaptureImageType == US_IMG_RGB_COLOR ? V2U_GRABFRAME_FORMAT_RGB24 : V2U_GRABFRAME_FORMAT_Y8);

  frame = FrmGrab_Frame( (FrmGrabber*)this->FrameGrabber, videoFormat, this->CropRectangle );

  if (frame == NULL)
  {
    LOG_WARNING("Frame not captured for video format: " << videoFormat);
    return PLUS_FAIL;
  }

  if ( frame->crop.width != this->FrameSize[0] || frame->crop.height != this->FrameSize[1])
  {
    LOG_ERROR("Image size received from Epiphan (" << frame->crop.width << "x" << frame->crop.height << ") does not match the clip rectangle size (" <<
      this->FrameSize[0] << "x" << this->FrameSize[1] << ")");
    FrmGrab_Release( (FrmGrabber*)this->FrameGrabber, frame );
    return PLUS_FAIL;
  }

  vtkPlusDataSource* aSource(NULL);
  for( int i = 0; i < this->GetNumberOfVideoSources(); ++i )
  {
    if( this->GetVideoSourceByIndex(i, aSource) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to retrieve the video source in the Epiphan device on channel " << (*this->OutputChannels.begin())->GetChannelId());
      return PLUS_FAIL;
    }
    int numberOfScalarComponents(1);
    if( aSource->GetImageType() == US_IMG_RGB_COLOR )
    {
      numberOfScalarComponents = 3;
    }
    if( aSource->AddItem(frame->pixbuf, aSource->GetInputImageOrientation(), this->FrameSize, VTK_UNSIGNED_CHAR, numberOfScalarComponents, aSource->GetImageType(), 0, this->FrameNumber) != PLUS_SUCCESS )
    {
      LOG_ERROR("Error adding item to video source " << aSource->GetSourceId() << " on channel " << (*this->OutputChannels.begin())->GetChannelId() );
      return PLUS_FAIL;
    }
    else
    {
      this->Modified();
    }
  }

  FrmGrab_Release( (FrmGrabber*)this->FrameGrabber, frame );

  this->FrameNumber++;

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkEpiphanVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  // SerialNumber is kept for backward compatibility only. Serial number or other address should be specified in the
  // GrabberLocation attribute.
  const char* grabberLocation = deviceConfig->GetAttribute("GrabberLocation");
  const char* serialNumber = deviceConfig->GetAttribute("SerialNumber");
  if (grabberLocation!=NULL)
  {
    SetGrabberLocation(grabberLocation);
  }
  else if( serialNumber !=NULL)
  {
    std::string grabberLocationString=std::string("sn:")+serialNumber;
    SetGrabberLocation(grabberLocationString.c_str());
    LOG_WARNING("Epiphan SerialNumber is specified. This attribute is deprecated, please use GrabberLocation=\"sn:SERIAL\" attribute instead.");
  }
  else
  {
    LOG_DEBUG("Epiphan device location is not specified in the configuration");
  }

  // Epiphan hardware clipping parameters
  XML_READ_VECTOR_ATTRIBUTE_OPTIONAL(int, 2, ClipRectangleOrigin, deviceConfig);
  XML_READ_VECTOR_ATTRIBUTE_OPTIONAL(int, 2, ClipRectangleSize, deviceConfig);

  return PLUS_SUCCESS;

}

//-----------------------------------------------------------------------------
PlusStatus vtkEpiphanVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(imageAcquisitionConfig, rootConfigElement);

  XML_WRITE_STRING_ATTRIBUTE_REMOVE_IF_NULL(GrabberLocation, imageAcquisitionConfig);

  // SerialNumber is an obsolete attribute, the information is stored now in GrabberLocation
  XML_REMOVE_ATTRIBUTE(imageAcquisitionConfig, "SerialNumber");

  // Epiphan hardware clipping parameters
  imageAcquisitionConfig->SetVectorAttribute("ClipRectangleOrigin", 2, this->GetClipRectangleOrigin());
  imageAcquisitionConfig->SetVectorAttribute("ClipRectangleSize", 2, this->GetClipRectangleSize());

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkEpiphanVideoSource::NotifyConfigured()
{
  vtkPlusDataSource* videoSource(NULL);
  if( this->OutputChannels.size() == 1 && this->GetFirstVideoSource(videoSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Epiphan is incorrectly configured. Unable to access video data source.");
    return PLUS_FAIL;
  }
  else if( this->OutputChannels.size() == 2 && this->GetVideoSourceByIndex(0, videoSource) != PLUS_SUCCESS && this->GetVideoSourceByIndex(1, videoSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Epiphan is incorrectly configured. Unable to access video data sources.");
    return PLUS_FAIL;
  }

  this->CaptureImageType = videoSource->GetImageType();
  for( int i = 0; i < this->GetNumberOfVideoSources(); ++i )
  {
    if( this->GetVideoSourceByIndex(i, videoSource) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to retrieve the video source in the Epiphan device on channel " << (*this->OutputChannels.begin())->GetChannelId());
      return PLUS_FAIL;
    }
    if( videoSource->GetImageType() != this->CaptureImageType )
    {
      LOG_ERROR("Conflicting image types in data sources. Please confirm matching image types.");
      return PLUS_FAIL;
    }
  }

  if (this->ClipRectangleSize[0] > 0 && this->ClipRectangleSize[1] > 0)
  {
    this->CropRectangle = new V2URect;
    this->CropRectangle->x = this->ClipRectangleOrigin[0];
    this->CropRectangle->y = this->ClipRectangleOrigin[1];
    this->CropRectangle->width = this->ClipRectangleSize[0];
    this->CropRectangle->height = this->ClipRectangleSize[1];
  }

  return PLUS_SUCCESS;
}
