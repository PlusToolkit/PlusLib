/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusOptiTrack.h"

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

// OptiTrack NatNet SDK
#include "NatNetTypes.h"
#include "NatNetClient.h"

vtkStandardNewMacro(vtkPlusOptiTrack);

//-----------------------------------------------------------------------
vtkPlusOptiTrack::vtkPlusOptiTrack()
{ 
  this->RequirePortNameInDeviceSetConfiguration = false;

  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates=true; 

}

//-------------------------------------------------------------------------
vtkPlusOptiTrack::~vtkPlusOptiTrack() 
{

}

//-------------------------------------------------------------------------
void vtkPlusOptiTrack::PrintSelf( ostream& os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusOptiTrack::InternalConnect()
{
  LOG_TRACE( "vtkPlusOptiTrack::Connect" ); 
  // Connect tracker
  // Load Calibration file
  // Set camera parameters read from the PLUS XML configuration file
  // Add tools from Configuration Files
  // Start tracker

  return PLUS_SUCCESS; 
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusOptiTrack::InternalDisconnect()
{
  LOG_TRACE( "vtkPlusOptiTrack::Disconnect" ); 
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusOptiTrack::InternalUpdate()
{
  LOG_TRACE( "vtkPlusOptiTrack::InternalUpdate" );
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOptiTrack::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusOptiTrack::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusOptiTrack::GetTransformMatrix(std::string toolName, vtkMatrix4x4* transformMatrix)
{

}