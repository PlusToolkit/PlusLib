/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkPhilips3DProbeVideoSource.h"

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPhilips3DProbeVideoSource);

//----------------------------------------------------------------------------
vtkPhilips3DProbeVideoSource::vtkPhilips3DProbeVideoSource()
{
  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates = true;
  this->AcquisitionRate = 10;
}

//----------------------------------------------------------------------------
vtkPhilips3DProbeVideoSource::~vtkPhilips3DProbeVideoSource()
{ 
  if ( this->Connected )
  {
    this->Disconnect();
  }
}

//----------------------------------------------------------------------------
void vtkPhilips3DProbeVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPhilips3DProbeVideoSource::InternalUpdate()
{
  LOG_TRACE("vtkPhilips3DProbeVideoSource::InternalUpdate");

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPhilips3DProbeVideoSource::InternalConnect()
{
  LOG_TRACE("vtkPhilips3DProbeVideoSource::InternalConnect");

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPhilips3DProbeVideoSource::InternalDisconnect()
{
  LOG_TRACE("vtkPhilips3DProbeVideoSource::InternalDisconnect");

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPhilips3DProbeVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPhilips3DProbeVideoSource::ReadConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPhilips3DProbeVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfig)
{
  LOG_TRACE("vtkPhilips3DProbeVideoSource::WriteConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfig);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPhilips3DProbeVideoSource::NotifyConfigured()
{
  if( this->OutputChannels.size() > 1 )
  {
    LOG_WARNING("vtkPhilips3DProbeVideoSource is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
    this->SetCorrectlyConfigured(false);
    return PLUS_FAIL;
  }

  if( this->OutputChannels.empty() )
  {
    LOG_ERROR("No output channels defined for vtkPhilips3DProbeVideoSource. Cannot proceed." );
    this->SetCorrectlyConfigured(false);
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
bool vtkPhilips3DProbeVideoSource::IsTracker() const
{
  return false;
}
