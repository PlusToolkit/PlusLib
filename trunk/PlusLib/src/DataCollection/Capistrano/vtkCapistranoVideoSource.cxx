/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkCapistranoVideoSource.h"

#include "usbprobedll_net.h"
#include "BmodeDLL.h"

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

//----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::NotifyConfigured()
{
  if( this->OutputChannels.size() > 1 )
  {
    LOG_WARNING("vtkCapistranoVideoSource is expecting at most one output channel and there are " << this->OutputChannels.size() << " channels.");
  }

  if( this->OutputChannels.empty() )
  {
    LOG_ERROR("No output channels defined for vtkCapistranoVideoSource. Cannot proceed." );
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
std::string vtkCapistranoVideoSource::GetSdkVersion()
{
  std::ostringstream versionString;
  versionString << "Capistrano BMode DLL version: " << bmDLLVer()
                << ", USB Probe DLL version: " << usbDLLVer() << std::ends;
  return versionString.str();
}

//----------------------------------------------------------------------------
int CALLBACK ProbeAttached()
{
  LOG_INFO("Probe attached");
  return 0;
}

//----------------------------------------------------------------------------
int CALLBACK ProbeDetached()
{
  LOG_INFO("Probe detached");
  return 0;
}

//----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::InternalConnect()
{
  LOG_TRACE( "vtkCapistranoVideoSource::InternalConnect" );

  LOG_DEBUG( "Capistrano BMode DLL version: " << bmDLLVer()
              << ", USB Probe DLL version: " << usbDLLVer());

  // These are commented out of usbprobedll_net.h in cSDK2013.
  //usbSetProbeAttachCallback(&ProbeAttached);
  //usbSetProbeDetachCallback(&ProbeDetached);

  char errorStatus[256] = {0};
  errorStatus[0] = 0;
  ULONG status = usbFindProbes(errorStatus);
  LOG_DEBUG("Find USB probes: status=" << status);
  if(status != ERROR_SUCCESS)
    {
    LOG_ERROR("Capistrano finding probes failed: " << errorStatus);
    return PLUS_FAIL;
    }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::InternalDisconnect()
{
  LOG_DEBUG("Disconnect from Capistrano");
}
