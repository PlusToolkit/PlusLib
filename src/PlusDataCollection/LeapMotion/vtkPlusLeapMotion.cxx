/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusLeapMotion.h"
#include "PlusMath.h"

// VTK includes
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkPlusDataSource.h>
#include <vtkXMLDataElement.h>
#include <vtksys/SystemTools.hxx>

// OS includes
#include <math.h>

// STL includes
#include <sstream>

//-------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusLeapMotion);

//-------------------------------------------------------------------------
vtkPlusLeapMotion::vtkPlusLeapMotion()
{
  this->RequirePortNameInDeviceSetConfiguration = true;
  this->StartThreadForInternalUpdates = true; // Polling based device
  this->AcquisitionRate = 60; // set to the maximum speed by default
}

//-------------------------------------------------------------------------
vtkPlusLeapMotion::~vtkPlusLeapMotion()
{
  if (this->Recording)
  {
    this->StopRecording();
  }
}

//-------------------------------------------------------------------------
void vtkPlusLeapMotion::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);

  os << "Device type: " << "4" << std::endl;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::InternalConnect()
{
  LOG_TRACE("vtkPlusLeapMotion::Connect");

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::InternalDisconnect()
{
  LOG_TRACE("vtkPlusLeapMotion::Disconnect");

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::InternalUpdate()
{
  LOG_TRACE("vtkPlusLeapMotion::InternalUpdate");

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::Probe()
{
  LOG_TRACE("vtkPlusLeapMotion::Probe");

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::InternalStartRecording()
{
  LOG_TRACE("vtkPlusLeapMotion::InternalStartRecording");

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::InternalStopRecording()
{
  LOG_TRACE("vtkPlusLeapMotion::InternalStopRecording");

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusLeapMotion::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);


  return PLUS_SUCCESS;
}
