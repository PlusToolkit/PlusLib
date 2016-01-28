/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkObjectFactory.h"
#include "vtkPlusDataSource.h"
#include "vtkVirtualScreenReader.h"

#include <tesseract/baseapi.h>
#include <tesseract/strngs.h>

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkVirtualScreenReader);

const int FRAME_COUNT_BEFORE_INACTIVE = 25;

//----------------------------------------------------------------------------
vtkVirtualScreenReader::vtkVirtualScreenReader()
: vtkPlusDevice()
, Language("")
{
  // The data capture thread will be used to regularly check the input devices and generate and update the output
  this->StartThreadForInternalUpdates=true;
  this->AcquisitionRate = vtkPlusDevice::VIRTUAL_DEVICE_FRAME_RATE;
}

//----------------------------------------------------------------------------
vtkVirtualScreenReader::~vtkVirtualScreenReader()
{
}

//----------------------------------------------------------------------------
void vtkVirtualScreenReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualScreenReader::InternalUpdate()
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualScreenReader::InternalConnect()
{
  this->TesseractAPI = new tesseract::TessBaseAPI();
  this->TesseractAPI->Init(NULL, Language, tesseract::OEM_DEFAULT);
  this->TesseractAPI->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualScreenReader::InternalDisconnect()
{
  delete this->TesseractAPI;
  this->TesseractAPI = NULL;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualScreenReader::ReadConfiguration( vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_STRING_ATTRIBUTE_OPTIONAL(Language, deviceConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualScreenReader::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualScreenReader::NotifyConfigured()
{
  return PLUS_SUCCESS;
}