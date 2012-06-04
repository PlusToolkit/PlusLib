/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtkEpiphanVideoSource.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtksys/SystemTools.hxx"
#include "vtkVideoBuffer.h"
#include "epiphan/frmgrab.h"

vtkCxxRevisionMacro(vtkEpiphanVideoSource, "$Revision: 1.0$");
vtkStandardNewMacro(vtkEpiphanVideoSource);

//----------------------------------------------------------------------------
vtkEpiphanVideoSource::vtkEpiphanVideoSource()
{
	this->VideoFormat = VIDEO_FORMAT_Y8; 
  this->ClipRectangleOrigin[0]=0;
  this->ClipRectangleOrigin[1]=0;
  this->ClipRectangleSize[0]=0;
  this->ClipRectangleSize[1]=0;
	this->SerialNumber = NULL;
	this->SpawnThreadForRecording = true;
	this->SetFrameBufferSize(200); 
	this->Buffer->Modified();
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

  // Clear buffer on connect 
  this->GetBuffer()->Clear();

  // Initialize frmgrab library
  FrmGrabNet_Init();

  if ( this->SerialNumber != NULL )
  {
	  if ( (this->FrameGrabber = FrmGrab_Open( this->SerialNumber )) == NULL ) 
    {      
      if ( (this->FrameGrabber = FrmGrabLocal_Open()) == NULL )
      {
        LOG_ERROR("Epiphan Device found");
        return PLUS_FAIL;
      }
      const char UNKNOWN_DEVICE[]="UNKNOWN";
      const char* connectedTo=FrmGrab_GetSN((FrmGrabber*)this->FrameGrabber);      
      if (connectedTo==NULL)
      {
        connectedTo=UNKNOWN_DEVICE;
      }
      
      LOG_WARNING("Epiphan Device with the requested serial number '"<<this->SerialNumber<<"' not found. Connected to " << connectedTo << " device instead.");
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
	  this->FrameSize[0] = this->ClipRectangleSize[0];
	  this->FrameSize[1] = this->ClipRectangleSize[1];
  }

  this->GetBuffer()->SetPixelType(itk::ImageIOBase::UCHAR);
  this->GetBuffer()->SetFrameSize(this->FrameSize); 

  return PLUS_SUCCESS;
}

    
//----------------------------------------------------------------------------
PlusStatus vtkEpiphanVideoSource::InternalDisconnect()
{

  LOG_DEBUG("Disconnect from Epiphan ");

  //this->Initialized = 0;
  if (this->FrameGrabber != NULL) {
	FrmGrab_Close((FrmGrabber*)this->FrameGrabber);
  }
  this->FrameGrabber = NULL;

  return this->StopRecording();

  return PLUS_SUCCESS;

  }

//----------------------------------------------------------------------------
PlusStatus vtkEpiphanVideoSource::InternalStartRecording()
{
  if (!this->Recording)
    {
		return PLUS_SUCCESS;
	}
  FrmGrab_Start((FrmGrabber*)this->FrameGrabber); 

  return this->Connect();
}

//----------------------------------------------------------------------------
PlusStatus vtkEpiphanVideoSource::InternalStopRecording()
{
  FrmGrab_Stop((FrmGrabber*)this->FrameGrabber);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkEpiphanVideoSource::InternalGrab()
{

  if (!this->Recording)
  {
    // drop the frame, we are not recording data now
    return PLUS_SUCCESS;
  }

  V2U_GrabFrame2 * frame = NULL;
  
  V2U_UINT32 videoFormat=V2U_GRABFRAME_FORMAT_Y8;
  switch (this->VideoFormat)
  {
  case VIDEO_FORMAT_Y8: videoFormat=V2U_GRABFRAME_FORMAT_Y8; break;
  case VIDEO_FORMAT_RGB8: videoFormat=V2U_GRABFRAME_FORMAT_RGB8; break;
  default:
    LOG_ERROR("Unkonwn video format: "<<this->VideoFormat);
    return PLUS_FAIL;
  }

  V2URect *cropRect=NULL;
  if (this->ClipRectangleSize[0]>0 && this->ClipRectangleSize[1]>0)
  {
    cropRect=new V2URect;
    cropRect->x = this->ClipRectangleOrigin[0];
    cropRect->y = this->ClipRectangleOrigin[1];
    cropRect->width = this->ClipRectangleSize[0];
    cropRect->height = this->ClipRectangleSize[1];
  }

  frame= FrmGrab_Frame((FrmGrabber*)this->FrameGrabber, videoFormat, cropRect);

  delete cropRect;
  cropRect = NULL;

  if (frame == NULL)
  {
	  LOG_WARNING("Frame not captured");
	  return PLUS_FAIL;
  }

  this->FrameNumber++; 

  // If the buffer is empty, set the pixel type and frame size to the first received properties 
  if ( this->GetBuffer()->GetNumberOfItems() == 0 )
  {
    this->GetBuffer()->SetPixelType(itk::ImageIOBase::UCHAR);  
    this->GetBuffer()->SetFrameSize( FrameSize );
  }

  PlusStatus status = this->Buffer->AddItem(frame->pixbuf ,this->GetUsImageOrientation(), FrameSize, 
	  itk::ImageIOBase::UCHAR,0,this->FrameNumber);
  this->Modified();
  FrmGrab_Release((FrmGrabber*)this->FrameGrabber, frame);
  return status;
}

//-----------------------------------------------------------------------------
PlusStatus vtkEpiphanVideoSource::ReadConfiguration(vtkXMLDataElement* config)
{

	LOG_TRACE("vtkEpiphanVideoSource::ReadConfiguration"); 
	if ( config == NULL )
	{
		LOG_ERROR("Unable to configure Epiphan video source! (XML data element is NULL)"); 
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

	const char* videoFormat = imageAcquisitionConfig->GetAttribute("VideoFormat"); 
	if ( videoFormat != NULL) 
	{
		if( !strcmp(videoFormat,"RGB8") )
		{
			this->SetVideoFormat( VIDEO_FORMAT_RGB8 );
		}
		else if ( !strcmp(videoFormat,"Y8") )
		{
			this->SetVideoFormat( VIDEO_FORMAT_Y8 );
		}
		else 
		{
			LOG_WARNING("Video Format unspecified/not supported. Using Y8"); 
			this->SetVideoFormat( VIDEO_FORMAT_Y8 );
		}
	}

	const char* serialNumber = imageAcquisitionConfig->GetAttribute("SerialNumber");
	if( serialNumber !=NULL)
	{
		this->SetSerialNumber(serialNumber);
	}
	else
	{
		LOG_DEBUG("Epiphan device SerialNumber is not specified in the configuration");
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
PlusStatus vtkEpiphanVideoSource::WriteConfiguration(vtkXMLDataElement* config)
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

  if ( this->VideoFormat == VIDEO_FORMAT_RGB8 )
  {
	  imageAcquisitionConfig->SetAttribute("VideoFormat", "RGB8");
  }
  else if ( this->VideoFormat == VIDEO_FORMAT_Y8 )
  {
	  imageAcquisitionConfig->SetAttribute("VideoFormat", "Y8");
  }
  else
  {
    LOG_ERROR("Attempted to write invalid video format into the config file: "<<this->VideoFormat<<". Written Y8 instead.");
    imageAcquisitionConfig->SetAttribute("VideoFormat", "Y8");
  }

  imageAcquisitionConfig->SetAttribute("SerialNumber", this->SerialNumber);

  // clipping parameters
  imageAcquisitionConfig->SetVectorAttribute("ClipRectangleOrigin", 2, this->GetClipRectangleOrigin());
  imageAcquisitionConfig->SetVectorAttribute("ClipRectangleSize", 2, this->GetClipRectangleSize());

  return PLUS_SUCCESS;
}