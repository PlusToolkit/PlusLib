/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
The following copyright notice is applicable to parts of this file:
Copyright (c) 2014, Robarts Research Institute, The University of Western Ontario, London, Ontario, Canada
All rights reserved.
Authors include:
* Adam Rankin (Robarts Research Institute and The University of Western Ontario)
=========================================================================*/

// Plus includes
#include "PlusConfigure.h"
#include "vtkPlusNvidiaDVPVideoSource.h"
#include "vtkPlusDataSource.h"

// VTK includes
#include <vtkObjectFactory.h>

// NVidia DVP includes
#include <DVPAPI.h>
#include <dvpapi_gl.h>

#include <NVCtrlLib.h>
#include <NVCtrl.h>


//----------------------------------------------------------------------------

vtkStandardNewMacro( vtkPlusNvidiaDVPVideoSource );

//----------------------------------------------------------------------------
vtkPlusNvidiaDVPVideoSource::vtkPlusNvidiaDVPVideoSource()
  : FrameNumber( 0 )
  , EnableGPUCPUCopy( false )
{
  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates = true;
  this->AcquisitionRate = 30;
}

//----------------------------------------------------------------------------
vtkPlusNvidiaDVPVideoSource::~vtkPlusNvidiaDVPVideoSource()
{
  if ( this->Connected )
  {
    this->Disconnect();
  }
}

//----------------------------------------------------------------------------
void vtkPlusNvidiaDVPVideoSource::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNvidiaDVPVideoSource::InternalConnect()
{
  LOG_TRACE( "vtkPlusPhilips3DProbeVideoSource::InternalConnect" );

  DVPStatus status = dvpInitGLContext();
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNvidiaDVPVideoSource::InternalDisconnect()
{
  LOG_TRACE( "vtkPlusNvidiaDVPVideoSource::InternalDisconnect" );

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusNvidiaDVPVideoSource::InternalUpdate()
{
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusNvidiaDVPVideoSource::ReadConfiguration( vtkXMLDataElement* rootConfigElement )
{
  LOG_TRACE( "vtkPlusNvidiaDVPVideoSource::ReadConfiguration" );
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING( deviceConfig, rootConfigElement );

  XML_READ_BOOL_ATTRIBUTE_OPTIONAL( EnableGPUCPUCopy, deviceConfig );

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusNvidiaDVPVideoSource::WriteConfiguration( vtkXMLDataElement* rootConfig )
{
  LOG_TRACE( "vtkPlusNvidiaDVPVideoSource::WriteConfiguration" );
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING( deviceConfig, rootConfig );

  XML_WRITE_BOOL_ATTRIBUTE( EnableGPUCPUCopy, deviceConfig );

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusNvidiaDVPVideoSource::NotifyConfigured()
{
  if ( this->OutputChannels.size() != 1 && this->EnableGPUCPUCopy ||
       this->OutputChannels.size() > 0 && !this->EnableGPUCPUCopy )
  {
    LOG_ERROR( "Incorrect configuration. GPU/CPU copy and OutputChannel configuration are incompatible." );
    this->SetCorrectlyConfigured( false );
    return PLUS_FAIL;
  }

  vtkPlusDataSource* videoSource( NULL );
  if( this->EnableGPUCPUCopy && this->GetFirstVideoSource( videoSource ) != PLUS_SUCCESS )
  {
    LOG_ERROR( "Unable to find video source. Device needs a video buffer to put new frames into when copying frames from the GPU." );
    this->SetCorrectlyConfigured( false );
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
bool vtkPlusNvidiaDVPVideoSource::IsTracker() const
{
  return false;
}