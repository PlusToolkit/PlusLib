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

vtkCxxRevisionMacro(vtkDataCollector, "$Revision: 1.0 $");

//----------------------------------------------------------------------------

vtkDataCollector* vtkDataCollector::New()
{
  return CreateDataCollectorAccordingToDeviceSetConfiguration();
}

//----------------------------------------------------------------------------
vtkDataCollector::vtkDataCollector()
{	
  this->ProgressBarUpdateCallbackFunction = NULL; 

  this->DataCollectionConfigVersion = 2.0; 

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
vtkDataCollector* vtkDataCollector::CreateDataCollectorAccordingToDeviceSetConfiguration()
{
  LOG_TRACE("vtkDataCollector::CreateDataCollectorAccordingToDeviceSetConfiguration");

  vtkXMLDataElement* rootElement = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData();
  if (rootElement == NULL)
  {
    LOG_ERROR("Data collector cannot be instantiated because of missing device set configuration data!");
    return NULL;
  }

  // Check plus configuration version
  double plusConfigurationVersion = 0;
  if (rootElement->GetScalarAttribute("version", plusConfigurationVersion))
  {
    double currentVersion = (double)PLUSLIB_VERSION_MAJOR + ((double)PLUSLIB_VERSION_MINOR / 10.0);

    if (plusConfigurationVersion < currentVersion)
    {
      LOG_ERROR("Data collector cannot be instantiated: This version of configuration file is no longer supported! Please update to version " << std::fixed << currentVersion); 
      return NULL;
    }
  }

  // Get data collection configuration element
  vtkXMLDataElement* dataCollectionConfig = rootElement->FindNestedElementWithName("DataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Data collector cannot be instantiated: Cannot find DataCollection element in XML tree!");
    return NULL;
  }

  // If ImageAcquisition, Tracker and Synchronization elements are present then create vtkDataCollectorHardwareDevice
  vtkXMLDataElement* imageAcquisitionConfig = dataCollectionConfig->FindNestedElementWithName("ImageAcquisition"); 
  vtkXMLDataElement* trackerConfig = dataCollectionConfig->FindNestedElementWithName("Tracker"); 
  vtkXMLDataElement* synchronizationConfig = dataCollectionConfig->FindNestedElementWithName("Synchronization"); 
  if (imageAcquisitionConfig && trackerConfig && synchronizationConfig)
  {
    // First try to create the object from the vtkObjectFactory
    vtkObject* dataCollector = vtkObjectFactory::CreateInstance("vtkDataCollectorHardwareDevice");
    if (dataCollector)
    {
      return (vtkDataCollector*)dataCollector;
    }
    // If the factory was unable to create the object, then create it here.
    return vtkDataCollectorHardwareDevice::New();
  }

  // If File element is present then create vtkDataCollectorFile
  vtkXMLDataElement* fileConfig = dataCollectionConfig->FindNestedElementWithName("File");
  if (fileConfig)
  {
    //TODO
    LOG_ERROR("vtkDataCollectorFile creation is NOT IMPLEMENTED");
    return NULL;
  }

  LOG_ERROR("Data collector cannot be instantiated: Unable to identify the data collection type from device set configuration XML data!");
  return NULL;
}
