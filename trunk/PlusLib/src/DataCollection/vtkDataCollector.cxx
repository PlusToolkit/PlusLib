/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"

#include "vtkDataCollector.h"

#include "vtkDataCollectorHardwareDevice.h"
#include "vtkDataCollectorFile.h"

#include "vtkObjectFactory.h"
#include "vtkXMLUtilities.h"
#include "vtkTimerLog.h"

// Needed for getting the value of VTK_DEBUG_LEAKS
#include "vtkToolkits.h"
#include "vtkDebugLeaks.h"

vtkCxxRevisionMacro(vtkDataCollector, "$Revision: 1.0 $");

//----------------------------------------------------------------------------

vtkDataCollector* vtkDataCollector::New()
{
  return CreateDataCollectorAccordingToDeviceSetConfiguration();
}

//----------------------------------------------------------------------------
vtkDataCollector::vtkDataCollector()
{	
  this->StartupDelaySec = 0.0; 

  this->ConnectedOff(); 

  this->TrackingEnabled = true;
  this->VideoEnabled = true;
}

//----------------------------------------------------------------------------
vtkDataCollector::~vtkDataCollector()
{
}

//----------------------------------------------------------------------------
void vtkDataCollector::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollector::VerifyDeviceSetConfigurationData(vtkXMLDataElement* rootElement)
{
  if (rootElement == NULL)
  {
    LOG_ERROR("DeviceSetConfigurationData is invalid");
    return PLUS_FAIL;
  }
  // Check plus configuration version. There were significant changes in the configuration file structure,
  // so reject processing older ones.
  const double minimumRequiredPlusConfigurationVersion=1.4;
  double plusConfigurationVersion = 0;
  if (!rootElement->GetScalarAttribute("version", plusConfigurationVersion))
  {
    LOG_ERROR("Version is not specified in the device set configuration. Minimum required version: " 
      << std::fixed << minimumRequiredPlusConfigurationVersion << ".");
    return PLUS_FAIL;
  }
  if (plusConfigurationVersion < minimumRequiredPlusConfigurationVersion)
  {
    LOG_ERROR("This version ("<< std::fixed << plusConfigurationVersion << ") of the device set configuration is no longer supported. Minimum required version: " 
      << std::fixed << minimumRequiredPlusConfigurationVersion << ".");            
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
vtkDataCollector* vtkDataCollector::CreateDataCollectorAccordingToDeviceSetConfiguration()
{
  LOG_TRACE("vtkDataCollector::CreateDataCollectorAccordingToDeviceSetConfiguration");
  
  vtkXMLDataElement* rootElement = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData();
  if (rootElement == NULL)
  {
    LOG_ERROR("Data collector cannot be instantiated because of missing device set configuration data!");
    return NULL;
  }
  if (VerifyDeviceSetConfigurationData(rootElement)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Data collector cannot be instantiated because the device set configuration data is invalid");
    return NULL;
  }
  
  // Get data collection configuration element
  vtkXMLDataElement* dataCollectionConfig = rootElement->FindNestedElementWithName("DataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Data collector cannot be instantiated: Cannot find DataCollection element in XML tree!");
    return NULL;
  }

  // If ImageAcquisition and Tracker elements are present then create vtkDataCollectorHardwareDevice
  vtkXMLDataElement* imageAcquisitionConfig = dataCollectionConfig->FindNestedElementWithName("ImageAcquisition"); 
  vtkXMLDataElement* trackerConfig = dataCollectionConfig->FindNestedElementWithName("Tracker"); 
  if (imageAcquisitionConfig || trackerConfig)
  {
    // First try to create the object from the vtkObjectFactory
    vtkObject* dataCollector = vtkObjectFactory::CreateInstance("vtkDataCollectorHardwareDevice");
    if (dataCollector)
    {
      return (vtkDataCollector*)dataCollector;
    }
    // if the factory failed to create the object,
    // then destroy it now, as vtkDebugLeaks::ConstructClass was called
#ifdef VTK_DEBUG_LEAKS
    vtkDebugLeaks::DestructClass("vtkDataCollectorHardwareDevice");
#endif 
    // If the factory was unable to create the object, then create it here.
    return vtkDataCollectorHardwareDevice::New();
  }

  // If File element is present then create vtkDataCollectorFile
  vtkXMLDataElement* fileConfig = dataCollectionConfig->FindNestedElementWithName("File");
  if (fileConfig)
  {
    // First try to create the object from the vtkObjectFactory
    vtkObject* dataCollector = vtkObjectFactory::CreateInstance("vtkDataCollectorFile");
    if (dataCollector)
    {
      return (vtkDataCollector*)dataCollector;
    }
    // if the factory failed to create the object,
    // then destroy it now, as vtkDebugLeaks::ConstructClass was called
#ifdef VTK_DEBUG_LEAKS
    vtkDebugLeaks::DestructClass("vtkDataCollectorFile");
#endif 
    return vtkDataCollectorFile::New();
  }

  LOG_ERROR("Data collector cannot be instantiated: Unable to identify the data collection type from device set configuration XML data!");
  return NULL;
}
