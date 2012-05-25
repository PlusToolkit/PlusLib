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

vtkCxxRevisionMacro(vtkEpiphanVideoSource, "$Revision: 1.0$");
vtkStandardNewMacro(vtkEpiphanVideoSource);

//----------------------------------------------------------------------------
vtkEpiphanVideoSource::vtkEpiphanVideoSource()
{
	this->VideoFormat = V2U_GRABFRAME_FORMAT_Y8; 
	this->CropRect = new V2URect;
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

	if ( this->fg != NULL) 
	{
		FrmGrab_Deinit();
		this->fg = NULL;
	}
	delete CropRect;
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
	  if ( (this->fg = FrmGrab_Open( this->SerialNumber )) == NULL ) 
	  {
		LOG_WARNING("Epiphan Device with set serial number not found, looking for any available device instead");
		if ( (this->fg = FrmGrabLocal_Open()) == NULL )
		{
		  LOG_ERROR("Epiphan Device Not found");
		  return PLUS_FAIL;
		}
	  }
  }
  else
  {
	  LOG_WARNING("Serial Number not specified. Looking for any available device");
	  if ( (this->fg = FrmGrabLocal_Open()) == NULL )
		{
		  LOG_ERROR("Epiphan Device Not found");
		  return PLUS_FAIL;
		}
  }
  
  V2U_VideoMode vm;

  if (FrmGrab_DetectVideoMode(this->fg,&vm) && vm.width && vm.height) 
  {
	  //this->SetFrameSize(vm.width,vm.height);
	  this->SetFrameRate(vm.vfreq/1000);
  } 
  else
  {
	  LOG_ERROR("No signal detected");
  }

  this->GetBuffer()->SetPixelType(itk::ImageIOBase::UCHAR);

  if( (CropRect->height != NULL) && (CropRect->width != NULL) )
  {
	  this->FrameSize[0] = CropRect->width;
	  this->FrameSize[1] = CropRect->height;
  }
  else
  {
	  this->FrameSize[0] = vm.width;
	  this->FrameSize[1] = vm.height;
  }

  return PLUS_SUCCESS;
}

    
//----------------------------------------------------------------------------
PlusStatus vtkEpiphanVideoSource::InternalDisconnect()
{

  LOG_DEBUG("Disconnect from Epiphan ");

  //this->Initialized = 0;
  if (this->fg != NULL) {
	FrmGrab_Close(this->fg);
  }
  this->fg = NULL;

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
  FrmGrab_Start(fg); 

  return this->Connect();
}

//----------------------------------------------------------------------------
PlusStatus vtkEpiphanVideoSource::InternalStopRecording()
{
  FrmGrab_Stop(fg);
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
  frame= FrmGrab_Frame(this->fg, VideoFormat, CropRect);

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
  FrmGrab_Release(this->fg, frame);
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
			this->SetVideoFormat( V2U_GRABFRAME_FORMAT_RGB8 );
		}
		else if ( !strcmp(videoFormat,"Y8") )
		{
			this->SetVideoFormat( V2U_GRABFRAME_FORMAT_Y8 );
		}
		else 
		{
			LOG_WARNING("Video Format unspecified/not supported. Using Y8"); 
			this->SetVideoFormat( V2U_GRABFRAME_FORMAT_Y8 );
		}
	}

	const char* serialNumber = imageAcquisitionConfig->GetAttribute("SerialNumber");
	if( serialNumber !=NULL)
	{
		this->SetSerialNumber(serialNumber);
	}
	else
	{
		LOG_WARNING("Serial Number not specified. Will try detecting it automatically");
	}

	int CropRectX,CropRectY,CropRectWidth,CropRectHeight;
	if ( imageAcquisitionConfig->GetScalarAttribute("CropRectX",CropRectX) )
	{
		this->CropRect->x=CropRectX;
	}
	else
	{
		this->CropRect->x=NULL;
	}
	if ( imageAcquisitionConfig->GetScalarAttribute("CropRectY",CropRectY) )
	{
		this->CropRect->y=CropRectY;
	}
	else
	{
		this->CropRect->y=NULL;
	}
	if ( imageAcquisitionConfig->GetScalarAttribute("CropRectWidth",CropRectWidth) )
	{
		this->CropRect->width=CropRectWidth;
	}
	else
	{
		LOG_WARNING("Crop Rectangle Width not specified. Setting it to NULL");
		this->CropRect->width=NULL;
	}
	if ( imageAcquisitionConfig->GetScalarAttribute("CropRectHeight",CropRectHeight) )
	{
		this->CropRect->height=CropRectHeight;
	}
	else
	{
		LOG_WARNING("Crop Rectangle Height not specified. Setting it to NULL");
		this->CropRect->height=NULL;
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

  if ( this->VideoFormat == V2U_GRABFRAME_FORMAT_RGB8 )
  {
	  imageAcquisitionConfig->SetAttribute("VideoFormat", "RGB8");
  }
  else
  {
	  imageAcquisitionConfig->SetAttribute("VideoFormat", "Y8");
  }
  imageAcquisitionConfig->SetAttribute("SerialNumber", this->SerialNumber);
  imageAcquisitionConfig->SetIntAttribute("CropX", this->CropRect->x);
  imageAcquisitionConfig->SetIntAttribute("CropY", this->CropRect->y);
  imageAcquisitionConfig->SetIntAttribute("CropWidth", this->CropRect->width);
  imageAcquisitionConfig->SetIntAttribute("CropHeight", this->CropRect->height);

  return PLUS_SUCCESS;
}