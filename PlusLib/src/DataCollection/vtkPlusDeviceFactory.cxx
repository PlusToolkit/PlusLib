/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtkObjectFactory.h"
#include "vtkPlusDevice.h"
#include "vtkPlusDeviceFactory.h"

//----------------------------------------------------------------------------
// Virtual devices
#include "vtkVirtualMixer.h"
#include "vtkVirtualSwitcher.h"
#include "vtkVirtualDiscCapture.h"
#include "vtkVirtualVolumeReconstructor.h"

//----------------------------------------------------------------------------
// Tracker devices
#include "vtkPlusDataSource.h"
#include "vtkPlusBuffer.h"
#ifdef PLUS_USE_OpenIGTLink
#include "vtkOpenIGTLinkTracker.h" 
#endif
#ifdef PLUS_USE_POLARIS
#include "vtkNDITracker.h"
#endif
#ifdef PLUS_USE_CERTUS
#include "vtkNDICertusTracker.h"
#endif
#ifdef PLUS_USE_MICRONTRACKER
#include "vtkMicronTracker.h"
#endif
#ifdef PLUS_USE_StealthLink2
#include "vtkStealthLinkTracker.h"
#endif
#ifdef PLUS_USE_BRACHY_TRACKER
#include "vtkBrachyTracker.h"
#endif
#ifdef PLUS_USE_Ascension3DG
#include "vtkAscension3DGTracker.h"
#endif
#ifdef PLUS_USE_Ascension3DGm
#include "vtkAscension3DGmTracker.h"
#endif
#ifdef PLUS_USE_PHIDGET_SPATIAL_TRACKER
#include "vtkPhidgetSpatialTracker.h"
#endif
#include "vtkFakeTracker.h"
#include "vtkChRoboticsTracker.h"
#ifdef PLUS_USE_3dConnexion_TRACKER
  // 3dConnexion tracker is supported on Windows only
  #include "vtk3dConnexionTracker.h"
#endif

//----------------------------------------------------------------------------
// Video sources
#include "vtkSavedDataSource.h"
#include "vtkUsSimulatorVideoSource.h"

#ifdef PLUS_USE_VFW_VIDEO
#include "vtkWin32VideoSource2.h"
#endif

#ifdef PLUS_USE_MMF_VIDEO
#include "vtkMmfVideoSource.h"
#endif

#ifdef PLUS_USE_ULTRASONIX_VIDEO
#include "vtkSonixVideoSource.h"
#include "vtkSonixPortaVideoSource.h"
#endif

#ifdef PLUS_USE_BKPROFOCUS_VIDEO
#include "vtkBkProFocusOemVideoSource.h"
#ifdef PLUS_USE_BKPROFOCUS_CAMERALINK
  #include "vtkBkProFocusCameraLinkVideoSource.h"
#endif
#endif


#ifdef PLUS_USE_ICCAPTURING_VIDEO
#include "vtkICCapturingSource.h"
#endif

#ifdef PLUS_USE_INTERSON_VIDEO
#include "vtkIntersonVideoSource.h"
#endif


#ifdef PLUS_USE_OpenIGTLink
#include "vtkOpenIGTLinkVideoSource.h"
#endif

#ifdef PLUS_USE_EPIPHAN
#include "vtkEpiphanVideoSource.h"
#endif

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkPlusDeviceFactory, "$Revision: 1.0$");
vtkStandardNewMacro(vtkPlusDeviceFactory);

//----------------------------------------------------------------------------

vtkPlusDeviceFactory::vtkPlusDeviceFactory(void)
{
  DeviceTypes["FakeTracker"]=(PointerToDevice)&vtkFakeTracker::New; 
  DeviceTypes["ChRobotics"]=(PointerToDevice)&vtkChRoboticsTracker::New; 
  DeviceTypes["StealthLinkTracker"]=(PointerToDevice)&vtkStealthLinkTracker::New;
#ifdef PLUS_USE_3dConnexion_TRACKER
  // 3dConnexion tracker is supported on Windows only
  DeviceTypes["3dConnexion"]=(PointerToDevice)&vtk3dConnexionTracker::New; 
#endif
#ifdef PLUS_USE_OpenIGTLink
  DeviceTypes["OpenIGTLinkTracker"]=(PointerToDevice)&vtkOpenIGTLinkTracker::New; 
#endif
#ifdef PLUS_USE_BRACHY_TRACKER
  DeviceTypes["BrachyTracker"]=(PointerToDevice)&vtkBrachyTracker::New; 
#endif 
#ifdef PLUS_USE_CERTUS
  DeviceTypes["CertusTracker"]=(PointerToDevice)&vtkNDICertusTracker::New; 
#endif
#ifdef PLUS_USE_POLARIS
  DeviceTypes["PolarisTracker"]=(PointerToDevice)&vtkNDITracker::New; 
  DeviceTypes["AuroraTracker"]=(PointerToDevice)&vtkNDITracker::New; 
#endif
#ifdef PLUS_USE_MICRONTRACKER  
  DeviceTypes["MicronTracker"]=(PointerToDevice)&vtkMicronTracker::New; 
#endif
#ifdef PLUS_USE_StealthLink2
  //DeviceTypes["StealthLinkTracker"]=(PointerToDevice)&vtkStealthLinkTracker::New;
#endif
#ifdef PLUS_USE_Ascension3DG  
  DeviceTypes["Ascension3DG"]=(PointerToDevice)&vtkAscension3DGTracker::New; 
#endif
#ifdef PLUS_USE_Ascension3DGm  
  DeviceTypes["Ascension3DGm"]=(PointerToDevice)&vtkAscension3DGmTracker::New; 
#endif
#ifdef PLUS_USE_PHIDGET_SPATIAL_TRACKER  
  DeviceTypes["PhidgetSpatial"]=(PointerToDevice)&vtkPhidgetSpatialTracker::New; 
#endif

  DeviceTypes["SavedDataSource"]=(PointerToDevice)&vtkSavedDataSource::New; 
  DeviceTypes["UsSimulator"]=(PointerToDevice)&vtkUsSimulatorVideoSource::New; 
  DeviceTypes["NoiseVideo"]=(PointerToDevice)&vtkPlusDevice::New; 
#ifdef PLUS_USE_OpenIGTLink
  DeviceTypes["OpenIGTLinkVideo"]=(PointerToDevice)&vtkOpenIGTLinkVideoSource::New; 
#endif
#ifdef PLUS_USE_ULTRASONIX_VIDEO
  DeviceTypes["SonixVideo"]=(PointerToDevice)&vtkSonixVideoSource::New; 
  DeviceTypes["SonixPortaVideo"]=(PointerToDevice)&vtkSonixPortaVideoSource::New; 
#endif 
#ifdef PLUS_USE_BKPROFOCUS_VIDEO
  DeviceTypes["BkProFocusOem"]=(PointerToDevice)&vtkBkProFocusOemVideoSource::New; 
  #ifdef PLUS_USE_BKPROFOCUS_CAMERALINK
    DeviceTypes["BkProFocusCameraLink"]=(PointerToDevice)&vtkBkProFocusCameraLinkVideoSource::New; 
    DeviceTypes["BkProFocus"]=(PointerToDevice)&vtkBkProFocusCameraLinkVideoSource::New;  // for backward compatibility only
  #endif 
#endif 
#ifdef PLUS_USE_VFW_VIDEO
  DeviceTypes["VFWVideo"]=(PointerToDevice)&vtkWin32VideoSource2::New; 
#endif 
#ifdef PLUS_USE_MMF_VIDEO
  DeviceTypes["MmfVideo"]=(PointerToDevice)&vtkMmfVideoSource::New; 
#endif 
#ifdef PLUS_USE_ICCAPTURING_VIDEO
  DeviceTypes["ICCapturing"]=(PointerToDevice)&vtkICCapturingSource::New; 
#endif 
#ifdef PLUS_USE_INTERSON_VIDEO
  DeviceTypes["IntersonVideo"]=(PointerToDevice)&vtkIntersonVideoSource::New; 
#endif 
#ifdef PLUS_USE_EPIPHAN
  DeviceTypes["Epiphan"]=(PointerToDevice)&vtkEpiphanVideoSource::New; 
#endif 

  // Virtual Devices
  DeviceTypes["VirtualMixer"]=(PointerToDevice)&vtkVirtualMixer::New;
  DeviceTypes["VirtualSwitcher"]=(PointerToDevice)&vtkVirtualSwitcher::New;
  DeviceTypes["VirtualDiscCapture"]=(PointerToDevice)&vtkVirtualDiscCapture::New;
  DeviceTypes["VirtualBufferedDiscCapture"]=(PointerToDevice)&vtkVirtualDiscCapture::New;
  DeviceTypes["VirtualVolumeReconstructor"]=(PointerToDevice)&vtkVirtualVolumeReconstructor::New;
  
}

//----------------------------------------------------------------------------

vtkPlusDeviceFactory::~vtkPlusDeviceFactory(void)
{
}

//----------------------------------------------------------------------------

void vtkPlusDeviceFactory::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf(os,indent);
  this->PrintAvailableDevices(os, indent); 
}

//----------------------------------------------------------------------------

void vtkPlusDeviceFactory::PrintAvailableDevices( ostream& os, vtkIndent indent )
{
  os << indent << "Supported devices: " << std::endl; 
  std::map<std::string,PointerToDevice>::iterator it; 
  for ( it = DeviceTypes.begin(); it != DeviceTypes.end(); ++it)
  {
    if ( it->second != NULL )
    {
      vtkPlusDevice* device = (*it->second)(); 
      os << indent.GetNextIndent() << "- " << it->first << " (ver: " << device->GetSdkVersion() << ")" << std::endl; 
      device->Delete();
      device = NULL;
    }
  }
}

//----------------------------------------------------------------------------

PlusStatus vtkPlusDeviceFactory::CreateInstance( const char* aDeviceType, vtkPlusDevice* &aDevice, const std::string &aDeviceId )
{
  if ( aDevice != NULL )
  {
    aDevice->Delete(); 
    aDevice = NULL; 
  }

  if ( aDeviceType == NULL ) 
  {
    LOG_ERROR("Device type is undefined"); 
    return PLUS_FAIL;
  }

  if ( DeviceTypes.find(aDeviceType) == DeviceTypes.end() )
  {
    std::string listOfSupportedDevices;
    std::map<std::string,PointerToDevice>::iterator it; 
    for ( it = DeviceTypes.begin(); it != DeviceTypes.end(); ++it)
    {
      if ( it->second != NULL )
      {
        vtkPlusDevice* device = (*it->second)(); 
        if (!listOfSupportedDevices.empty())
        {
          listOfSupportedDevices.append(", ");
        }
        listOfSupportedDevices.append(it->first); 
        device->Delete();
        device = NULL;
      }
    }
    LOG_ERROR("Unknown device type: " << aDeviceType<<". Supported devices: "<<listOfSupportedDevices);
    return PLUS_FAIL; 
  }

  if ( DeviceTypes[aDeviceType] == NULL )
  { 
    LOG_ERROR("Invalid factory method for device type: " << aDeviceType);
    return PLUS_FAIL; 
  }

  // Call device New() function
  aDevice = (*DeviceTypes[aDeviceType])(); 
  if (aDevice==NULL)
  {
    LOG_ERROR("Invalid device created for device type: " << aDeviceType);
    return PLUS_FAIL; 
  }

  aDevice->SetDeviceId(aDeviceId.c_str());

  return PLUS_SUCCESS; 
}
