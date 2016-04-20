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
#include "vtkPlusVirtualMixer.h"
#include "vtkPlusVirtualSwitcher.h"
#include "vtkPlusVirtualDiscCapture.h"
#include "vtkPlusVirtualVolumeReconstructor.h"
#include "vtkPlusImageProcessorVideoSource.h"
#include "vtkPlusGenericSerialDevice.h"

//----------------------------------------------------------------------------
// Tracker devices
#include "vtkPlusDataSource.h"
#include "vtkPlusBuffer.h"
#ifdef PLUS_USE_OpenIGTLink
#include "vtkPlusOpenIGTLinkTracker.h" 
#endif
#ifdef PLUS_USE_POLARIS
#include "vtkPlusNDITracker.h"
#endif
#ifdef PLUS_USE_CERTUS
#include "vtkPlusNDICertusTracker.h"
#endif
#ifdef PLUS_USE_MICRONTRACKER
#include "vtkPlusMicronTracker.h"
#endif
#ifdef PLUS_USE_STEALTHLINK
#include "vtkPlusStealthLinkTracker.h"
#endif
#ifdef PLUS_USE_BRACHY_TRACKER
#include "vtkPlusBrachyTracker.h"
#endif
#ifdef PLUS_USE_USDIGITALENCODERS_TRACKER
#include "vtkPlusUSDigitalEncodersTracker.h"
#endif
#ifdef PLUS_USE_Ascension3DG
#include "vtkPlusAscension3DGTracker.h"
#endif
#ifdef PLUS_USE_Ascension3DGm
#include "vtkAscension3DGmTracker.h"
#endif
#ifdef PLUS_USE_OPTIMET_CONOPROBE
#include "vtkPlusOptimetConoProbeMeasurer.h"
#endif 
#ifdef PLUS_USE_OPTITRACK
#include "vtkPlusOptiTrackTracker.h"
#endif
#ifdef PLUS_USE_PHIDGET_SPATIAL_TRACKER
#include "vtkPlusPhidgetSpatialTracker.h"
#endif
#include "vtkPlusFakeTracker.h"
#include "vtkPlusChRoboticsTracker.h"
#include "vtkPlusMicrochipTracker.h"
#ifdef PLUS_USE_3dConnexion_TRACKER
// 3dConnexion tracker is supported on Windows only
#include "vtkPlus3dConnexionTracker.h"
#endif

//----------------------------------------------------------------------------
// Video sources
#include "vtkPlusSavedDataSource.h"
#include "vtkPlusUsSimulatorVideoSource.h"

#ifdef PLUS_USE_VFW_VIDEO
#include "vtkPlusWin32VideoSource2.h"
#endif

#ifdef PLUS_USE_MMF_VIDEO
#include "vtkPlusMmfVideoSource.h"
#endif

#ifdef PLUS_USE_ULTRASONIX_VIDEO
#include "vtkPlusSonixVideoSource.h"
#include "vtkPlusSonixPortaVideoSource.h"
#endif

#ifdef PLUS_USE_BKPROFOCUS_VIDEO
#include "vtkPlusBkProFocusOemVideoSource.h"
#ifdef PLUS_USE_BKPROFOCUS_CAMERALINK
  #include "vtkPlusBkProFocusCameraLinkVideoSource.h"
#endif
#endif

#ifdef PLUS_USE_ICCAPTURING_VIDEO
#include "vtkPlusICCapturingSource.h"
#endif

#ifdef PLUS_USE_INTERSON_VIDEO
#include "vtkPlusIntersonVideoSource.h"
#endif

#ifdef PLUS_USE_INTERSONSDKCXX_VIDEO
#include "vtkPlusIntersonSDKCxxVideoSource.h"
#endif


#ifdef PLUS_USE_TELEMED_VIDEO
#include "Telemed\vtkPlusTelemedVideoSource.h"
#endif

#ifdef PLUS_USE_THORLABS_VIDEO
#include "ThorLabs\vtkPlusThorLabsVideoSource.h"
#endif

#ifdef PLUS_USE_OpenIGTLink
#include "vtkPlusOpenIGTLinkVideoSource.h"
#endif

#ifdef PLUS_USE_EPIPHAN
#include "vtkPlusEpiphanVideoSource.h"
#endif

#ifdef PLUS_USE_IntuitiveDaVinci
#include "vtkPlusIntuitiveDaVinciTracker.h"
#endif

#ifdef PLUS_USE_PHILIPS_3D_ULTRASOUND
#include "vtkPlusPhilips3DProbeVideoSource.h"
#endif

#ifdef PLUS_USE_CAPISTRANO_VIDEO
#include "Capistrano\vtkPlusCapistranoVideoSource.h"
#endif

//---------------------------------------------------------------------------
// Virtual devices
#ifdef PLUS_USE_tesseract
#include "vtkPlusVirtualTextRecognizer.h"
#endif

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusDeviceFactory);

//----------------------------------------------------------------------------

vtkPlusDeviceFactory::vtkPlusDeviceFactory(void)
{
  DeviceTypes["FakeTracker"]=(PointerToDevice)&vtkPlusFakeTracker::New; 
  DeviceTypes["ChRobotics"]=(PointerToDevice)&vtkPlusChRoboticsTracker::New; 
  DeviceTypes["Microchip"]=(PointerToDevice)&vtkPlusMicrochipTracker::New;
  
#ifdef PLUS_USE_3dConnexion_TRACKER
  // 3dConnexion tracker is supported on Windows only
  DeviceTypes["3dConnexion"]=(PointerToDevice)&vtkPlus3dConnexionTracker::New; 
#endif
#ifdef PLUS_USE_OpenIGTLink
  DeviceTypes["OpenIGTLinkTracker"]=(PointerToDevice)&vtkPlusOpenIGTLinkTracker::New;
#endif
#ifdef PLUS_USE_BRACHY_TRACKER
  DeviceTypes["BrachyTracker"]=(PointerToDevice)&vtkPlusBrachyTracker::New; 
#endif 
#ifdef PLUS_USE_USDIGITALENCODERS_TRACKER
  DeviceTypes["USDigitalEncodersTracker"]=(PointerToDevice)&vtkPlusUSDigitalEncodersTracker::New;
#endif
#ifdef PLUS_USE_CERTUS
  DeviceTypes["CertusTracker"]=(PointerToDevice)&vtkPlusNDICertusTracker::New; 
#endif
#ifdef PLUS_USE_POLARIS
  DeviceTypes["PolarisTracker"]=(PointerToDevice)&vtkPlusNDITracker::New; 
  DeviceTypes["AuroraTracker"]=(PointerToDevice)&vtkPlusNDITracker::New; 
#endif
#ifdef PLUS_USE_MICRONTRACKER  
  DeviceTypes["MicronTracker"]=(PointerToDevice)&vtkPlusMicronTracker::New; 
#endif
#ifdef PLUS_USE_STEALTHLINK
  DeviceTypes["StealthLinkTracker"]=(PointerToDevice)&vtkPlusStealthLinkTracker::New;
#endif
#ifdef PLUS_USE_Ascension3DG  
  DeviceTypes["Ascension3DG"]=(PointerToDevice)&vtkAscension3DGTracker::New; 
#endif
#ifdef PLUS_USE_Ascension3DGm  
  DeviceTypes["Ascension3DGm"]=(PointerToDevice)&vtkAscension3DGmTracker::New; 
#endif
#ifdef PLUS_USE_PHIDGET_SPATIAL_TRACKER  
  DeviceTypes["PhidgetSpatial"]=(PointerToDevice)&vtkPlusPhidgetSpatialTracker::New; 
#endif

  DeviceTypes["SavedDataSource"]=(PointerToDevice)&vtkPlusSavedDataSource::New; 
  DeviceTypes["UsSimulator"]=(PointerToDevice)&vtkPlusUsSimulatorVideoSource::New;
  DeviceTypes["ImageProcessor"]=(PointerToDevice)&vtkPlusImageProcessorVideoSource::New;
  DeviceTypes["GenericSerialDevice"]=(PointerToDevice)&vtkPlusGenericSerialDevice::New;
  DeviceTypes["NoiseVideo"]=(PointerToDevice)&vtkPlusDevice::New; 
#ifdef PLUS_USE_OpenIGTLink
  DeviceTypes["OpenIGTLinkVideo"]=(PointerToDevice)&vtkPlusOpenIGTLinkVideoSource::New; 
#endif
#ifdef PLUS_USE_OPTIMET_CONOPROBE
  DeviceTypes["OptimetConoProbe"] = (PointerToDevice)&vtkPlusOptimetConoProbeMeasurer::New;
#endif 
#ifdef PLUS_USE_OPTITRACK
  DeviceTypes["OptiTrack"]=(PointerToDevice)&vtkPlusOptiTrackTracker::New; 
#endif
#ifdef PLUS_USE_ULTRASONIX_VIDEO
  DeviceTypes["SonixVideo"]=(PointerToDevice)&vtkPlusSonixVideoSource::New; 
  DeviceTypes["SonixPortaVideo"]=(PointerToDevice)&vtkPlusSonixPortaVideoSource::New; 
#endif 
#ifdef PLUS_USE_BKPROFOCUS_VIDEO
  DeviceTypes["BkProFocusOem"]=(PointerToDevice)&vtkPlusBkProFocusOemVideoSource::New; 
  #ifdef PLUS_USE_BKPROFOCUS_CAMERALINK
    DeviceTypes["BkProFocusCameraLink"]=(PointerToDevice)&vtkPlusBkProFocusCameraLinkVideoSource::New; 
    DeviceTypes["BkProFocus"]=(PointerToDevice)&vtkPlusBkProFocusCameraLinkVideoSource::New;  // for backward compatibility only
  #endif 
#endif 
#ifdef PLUS_USE_VFW_VIDEO
  DeviceTypes["VFWVideo"]=(PointerToDevice)&vtkPlusWin32VideoSource2::New; 
#endif 
#ifdef PLUS_USE_MMF_VIDEO
  DeviceTypes["MmfVideo"]=(PointerToDevice)&vtkPlusMmfVideoSource::New; 
#endif 
#ifdef PLUS_USE_ICCAPTURING_VIDEO
  DeviceTypes["ICCapturing"]=(PointerToDevice)&vtkPlusICCapturingSource::New; 
#endif 
#ifdef PLUS_USE_INTERSON_VIDEO
  DeviceTypes["IntersonVideo"]=(PointerToDevice)&vtkPlusIntersonVideoSource::New; 
#endif 
#ifdef PLUS_USE_INTERSONSDKCXX_VIDEO
  DeviceTypes["IntersonSDKCxxVideo"]=(PointerToDevice)&vtkPlusIntersonSDKCxxVideoSource::New; 
#endif 
#ifdef PLUS_USE_TELEMED_VIDEO
  DeviceTypes["TelemedVideo"]=(PointerToDevice)&vtkPlusTelemedVideoSource::New;
#endif
#ifdef PLUS_USE_THORLABS_VIDEO
  DeviceTypes["ThorLabsVideo"]=(PointerToDevice)&vtkPlusThorLabsVideoSource::New;
#endif
#ifdef PLUS_USE_EPIPHAN
  DeviceTypes["Epiphan"]=(PointerToDevice)&vtkPlusEpiphanVideoSource::New; 
#endif 

#ifdef PLUS_USE_IntuitiveDaVinci
  DeviceTypes["IntuitiveDaVinci"]=(PointerToDevice)&vtkPlusIntuitiveDaVinciTracker::New;
#endif

#ifdef PLUS_USE_PHILIPS_3D_ULTRASOUND
  DeviceTypes["iE33Video"]=(PointerToDevice)&vtkPlusPhilips3DProbeVideoSource::New;
#endif

#ifdef PLUS_USE_CAPISTRANO_VIDEO
	DeviceTypes["CapistranoVideo"]=(PointerToDevice)&vtkPlusCapistranoVideoSource::New;
#endif
#ifdef PLUS_USE_tesseract
  DeviceTypes["VirtualTextRecognizer"]=(PointerToDevice)&vtkPlusVirtualTextRecognizer::New;
#endif

  // Virtual Devices
  DeviceTypes["VirtualMixer"]=(PointerToDevice)&vtkPlusVirtualMixer::New;
  DeviceTypes["VirtualSwitcher"]=(PointerToDevice)&vtkPlusVirtualSwitcher::New;
  DeviceTypes["VirtualDiscCapture"]=(PointerToDevice)&vtkPlusVirtualDiscCapture::New;
  DeviceTypes["VirtualBufferedDiscCapture"]=(PointerToDevice)&vtkPlusVirtualDiscCapture::New;
  DeviceTypes["VirtualVolumeReconstructor"]=(PointerToDevice)&vtkPlusVirtualVolumeReconstructor::New;
  
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
