/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkCapistranoVideoSource.h"

vtkStandardNewMacro(vtkCapistranoVideoSource);

//----------------------------------------------------------------------------
vtkCapistranoVideoSource::vtkCapistranoVideoSource()
{
}

//----------------------------------------------------------------------------
vtkCapistranoVideoSource::~vtkCapistranoVideoSource()
{
}

//----------------------------------------------------------------------------
void vtkCapistranoVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//-----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::ReadConfiguration(vtkXMLDataElement* config)
{
  LOG_TRACE("vtkCapistranoVideoSource::ReadConfiguration"); 
  if ( config == NULL )
  {
    LOG_ERROR("Unable to configure Capistrano video source! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  Superclass::ReadConfiguration(config); 

  vtkXMLDataElement* deviceConfig = this->FindThisDeviceElement(config);
  if (deviceConfig == NULL) 
  {
    LOG_ERROR("Unable to find ImageAcquisition element in configuration XML structure!");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  return PLUS_SUCCESS;
}
