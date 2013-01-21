/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusStreamBuffer.h"
#include "vtkPlusStreamImage.h"

vtkStandardNewMacro(vtkPlusStreamImage);

//----------------------------------------------------------------------------
vtkPlusStreamImage::vtkPlusStreamImage()
: Device(NULL)
, ImageName(NULL)
, Buffer(vtkSmartPointer<vtkPlusStreamBuffer>::New())
, FrameNumber(0)
{
}

//----------------------------------------------------------------------------
vtkPlusStreamImage::~vtkPlusStreamImage()
{
  if ( this->ImageName != NULL )
  {
    delete [] this->ImageName; 
    this->ImageName = NULL; 
  }
}

//----------------------------------------------------------------------------
void vtkPlusStreamImage::PrintSelf(ostream& os, vtkIndent indent)
{
	vtkObject::PrintSelf(os,indent);

  if ( this->Device )
  {
	  os << indent << "Tracker: " << this->Device << "\n";
  }
  if ( this->ImageName )
  {
    os << indent << "ImageName: " << this->GetImageName() << "\n";
  }
  if ( this->Buffer )
  {
    os << indent << "Buffer: " << this->Buffer << "\n";
    this->Buffer->PrintSelf(os,indent.GetNextIndent());
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStreamImage::SetImageName(const char* ImageName)
{
  if ( this->ImageName == NULL && ImageName == NULL) 
  { 
    return PLUS_SUCCESS;
  } 

  if ( this->ImageName && ImageName && ( STRCASECMP(this->ImageName, ImageName) == 0 ) ) 
  { 
    return PLUS_SUCCESS;
  } 

  if ( this->ImageName != NULL )
  {
    LOG_ERROR("Tool name change is not allowed for tool '" << this->ImageName << "'" ); 
    return PLUS_FAIL; 
  }

  // Copy string 
  size_t n = strlen(ImageName) + 1; 
  char *cp1 =  new char[n]; 
  const char *cp2 = (ImageName); 
  this->ImageName = cp1;
  do { *cp1++ = *cp2++; } while ( --n ); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
void vtkPlusStreamImage::SetDevice(vtkPlusDevice *device)
{
	// The device is not reference counted, since that would cause a reference loop
	if (device == this->Device)
	{
		return;
	}

	if (this->Device)
	{
		this->Device = NULL;
	}

	if (device)
	{
		this->Device = device;
	}
	else
	{
		this->Device = NULL;
	}

	this->Modified();
}

//----------------------------------------------------------------------------
void vtkPlusStreamImage::DeepCopy(vtkPlusStreamImage *image)
{
	LOG_TRACE("vtkPlusStreamImage::DeepCopy"); 

	this->SetImageName( image->GetImageName() ); 

	this->Buffer->DeepCopy( image->GetBuffer() );

	this->SetFrameNumber( image->GetFrameNumber() );
}


//-----------------------------------------------------------------------------
PlusStatus vtkPlusStreamImage::ReadConfiguration(vtkSmartPointer<vtkXMLDataElement> imageElement, bool RequireAveragedItemsForFilteringInDeviceSetConfiguration)
{
	LOG_TRACE("vtkPlusStreamTool::ReadConfiguration"); 

	if ( imageElement == NULL )
	{
		LOG_ERROR("Unable to configure stream tool! (XML data element is NULL)"); 
		return PLUS_FAIL; 
	}

	const char* ImageName = imageElement->GetAttribute("Id"); 
	if ( ImageName != NULL ) 
	{
		this->SetImageName(ImageName); 
	}
	else
	{
		LOG_ERROR("Unable to find image name! \"Id\" attribute is mandatory in tool definition."); 
		return PLUS_FAIL; 
	}

  int bufferSize = 0; 
  if ( imageElement->GetScalarAttribute("BufferSize", bufferSize) ) 
  {
    this->GetBuffer()->SetBufferSize(bufferSize);
  }
  else
  {
    LOG_ERROR("Unable to find image \"" << this->GetImageName() << "\" buffer size in device element when it is required.");
    return PLUS_FAIL;
  }

  int averagedItemsForFiltering = 0;
  if ( imageElement->GetScalarAttribute("AveragedItemsForFiltering", averagedItemsForFiltering) )
  {
    this->GetBuffer()->SetAveragedItemsForFiltering(averagedItemsForFiltering);
  }
  else if ( RequireAveragedItemsForFilteringInDeviceSetConfiguration )
  {
    LOG_ERROR("Unable to find averaged items for filtering in image element when it is required.");
    return PLUS_FAIL;
  }
  else
  {
    LOG_DEBUG("Unable to find AveragedItemsForFiltering attribute in image element. Using default value.");
  }

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusStreamImage::WriteConfiguration( vtkXMLDataElement* toolElement )
{
  LOG_TRACE("vtkPlusStreamTool::WriteConfiguration"); 

  if ( toolElement == NULL )
  {
    LOG_ERROR("Unable to configure stream image! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  toolElement->SetAttribute("Id", this->GetImageName());
  toolElement->SetIntAttribute("BufferSize", this->GetBuffer()->GetBufferSize());

  if( toolElement->GetAttribute("AveragedItemsForFiltering") != NULL )
  {
    toolElement->SetIntAttribute("AveragedItemsForFiltering", this->GetBuffer()->GetAveragedItemsForFiltering());
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusStreamImage::WriteCompactConfiguration( vtkXMLDataElement* toolElement )
{
  LOG_TRACE("vtkPlusStreamTool::WriteConfiguration"); 

  if ( toolElement == NULL )
  {
    LOG_ERROR("Unable to configure stream image! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  toolElement->SetAttribute("Id", this->GetImageName());

  return PLUS_SUCCESS;
}