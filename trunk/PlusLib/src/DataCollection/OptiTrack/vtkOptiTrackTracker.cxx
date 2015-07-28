/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
Date: 
July 2015
Authors include:
* Mikael Brudfors (Laboratorio de Imagen Médica, Unidad de Medicina y Cirugía Experimental, Hospital General Universitario)
* Laura Sanz (Laboratorio de Imagen Médica, Unidad de Medicina y Cirugía Experimental, Hospital General Universitario)
=========================================================================*/  

#include "PlusConfigure.h"
#include "vtkOptiTrackTracker.h"

#include "PlusMath.h"

#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkPlusDataSource.h"
#include "vtkTransform.h"
#include "vtkXMLDataElement.h"
#include "vtksys/SystemTools.hxx"

#include <math.h>
#include <sstream>

#include "NPTrackingTools.h"

vtkStandardNewMacro(vtkOptiTrackTracker);

//-----------------------------------------------------------------------
vtkOptiTrackTracker::vtkOptiTrackTracker()
{ 
  this->RequirePortNameInDeviceSetConfiguration = true;

  this->AcquisitionRate = 100;

  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates=true; 
 
}

//-------------------------------------------------------------------------
vtkOptiTrackTracker::~vtkOptiTrackTracker() 
{
}

//-------------------------------------------------------------------------
void vtkOptiTrackTracker::PrintSelf( ostream& os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );
}

//-------------------------------------------------------------------------
PlusStatus vtkOptiTrackTracker::InternalConnect()
{
  LOG_TRACE( "vtkOptiTrackTracker::Connect" ); 

  return PLUS_SUCCESS; 
}

//-------------------------------------------------------------------------
PlusStatus vtkOptiTrackTracker::InternalDisconnect()
{
  LOG_TRACE( "vtkOptiTrackTracker::Disconnect" ); 
  return this->StopRecording();
}

//-------------------------------------------------------------------------
PlusStatus vtkOptiTrackTracker::InternalUpdate()
{
  LOG_TRACE( "vtkOptiTrackTracker::InternalUpdate" ); 

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkOptiTrackTracker::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkOptiTrackTracker::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  return PLUS_SUCCESS;
}