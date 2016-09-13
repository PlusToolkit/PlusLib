/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

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
#ifdef PLUS_USE_NDI
#include "vtkPlusNDITracker.h"
#endif
#ifdef PLUS_USE_NDI_CERTUS
#include "vtkPlusNDICertusTracker.h"
#endif
#ifdef PLUS_USE_MICRONTRACKER
#include "vtkPlusMicronTracker.h"
#endif
#ifdef PLUS_USE_INTELREALSENSE
#include "vtkPlusIntelRealSenseTracker.h"
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

#ifdef PLUS_USE_NVIDIA_DVP
#if WIN32
#include "vtkPlusNVidiaDVPVideoSourceWin32.h"
#elif __linux__
#include "vtkPlusNVidiaDVPVideoSourceLinux.h"
#else
// TODO : add mac support
#endif
#endif

#ifdef PLUS_USE_OvrvisionPro
#include "vtkPlusOvrvisionProVideoSource.h"
#endif

//---------------------------------------------------------------------------
// Virtual devices
#ifdef PLUS_USE_tesseract
#include "vtkPlusVirtualTextRecognizer.h"
#endif

//----------------------------------------------------------------------------

vtkStandardNewMacro( vtkPlusDeviceFactory );

//----------------------------------------------------------------------------

vtkPlusDeviceFactory::vtkPlusDeviceFactory( void )
{
  DeviceTypes["FakeTracker"] = ( PointerToDevice )&vtkPlusFakeTracker::New;
  DeviceTypeClassNames["FakeTracker"] = "vtkPlusFakeTracker";
  DeviceTypes["ChRobotics"] = ( PointerToDevice )&vtkPlusChRoboticsTracker::New;
  DeviceTypeClassNames["ChRobotics"] = "vtkPlusChRoboticsTracker";
  DeviceTypes["Microchip"] = ( PointerToDevice )&vtkPlusMicrochipTracker::New;
  DeviceTypeClassNames["Microchip"] = "vtkPlusMicrochipTracker";

#ifdef PLUS_USE_3dConnexion_TRACKER
  // 3dConnexion tracker is supported on Windows only
  DeviceTypes["3dConnexion"] = ( PointerToDevice )&vtkPlus3dConnexionTracker::New;
  DeviceTypeClassNames["3dConnexion"] = "vtkPlus3dConnexionTracker";
#endif
#ifdef PLUS_USE_OpenIGTLink
  DeviceTypes["OpenIGTLinkTracker"] = ( PointerToDevice )&vtkPlusOpenIGTLinkTracker::New;
  DeviceTypeClassNames["OpenIGTLinkTracker"] = "vtkPlusOpenIGTLinkTracker";
#endif
#ifdef PLUS_USE_BRACHY_TRACKER
  DeviceTypes["BrachyTracker"] = ( PointerToDevice )&vtkPlusBrachyTracker::New;
  DeviceTypeClassNames["BrachyTracker"] = "vtkPlusBrachyTracker";
#endif
#ifdef PLUS_USE_USDIGITALENCODERS_TRACKER
  DeviceTypes["USDigitalEncodersTracker"] = ( PointerToDevice )&vtkPlusUSDigitalEncodersTracker::New;
  DeviceTypeClassNames["USDigitalEncodersTracker"] = "vtkPlusUSDigitalEncodersTracker";
#endif
#ifdef PLUS_USE_NDI_CERTUS
  DeviceTypes["CertusTracker"] = ( PointerToDevice )&vtkPlusNDICertusTracker::New;
  DeviceTypeClassNames["CertusTracker"] = "vtkPlusNDICertusTracker";
#endif
#ifdef PLUS_USE_NDI
  DeviceTypes["PolarisTracker"] = ( PointerToDevice )&vtkPlusNDITracker::New;
  DeviceTypeClassNames["PolarisTracker"] = "vtkPlusNDITracker";
  DeviceTypes["AuroraTracker"] = ( PointerToDevice )&vtkPlusNDITracker::New;
  DeviceTypeClassNames["AuroraTracker"] = "vtkPlusNDITracker";
#endif
#ifdef PLUS_USE_MICRONTRACKER
  DeviceTypes["MicronTracker"] = ( PointerToDevice )&vtkPlusMicronTracker::New;
  DeviceTypeClassNames["MicronTracker"] = "vtkPlusMicronTracker";
#endif
#ifdef PLUS_USE_INTELREALSENSE
  DeviceTypes["IntelRealSenseTracker"] = ( PointerToDevice )&vtkPlusIntelRealSenseTracker::New;
  DeviceTypeClassNames["IntelRealSenseTracker"] = "vtkPlusIntelRealSenseTracker";
#endif
#ifdef PLUS_USE_STEALTHLINK
  DeviceTypes["StealthLinkTracker"] = ( PointerToDevice )&vtkPlusStealthLinkTracker::New;
  DeviceTypeClassNames["StealthLinkTracker"] = "vtkPlusStealthLinkTracker";
#endif
#ifdef PLUS_USE_Ascension3DG
  DeviceTypes["Ascension3DG"] = ( PointerToDevice )&vtkAscension3DGTracker::New;
  DeviceTypeClassNames["Ascension3DG"] = "vtkAscension3DGTracker";
#endif
#ifdef PLUS_USE_Ascension3DGm
  DeviceTypes["Ascension3DGm"] = ( PointerToDevice )&vtkAscension3DGmTracker::New;
  DeviceTypeClassNames["Ascension3DGm"] = "vtkAscension3DGmTracker";
#endif
#ifdef PLUS_USE_PHIDGET_SPATIAL_TRACKER
  DeviceTypes["PhidgetSpatial"] = ( PointerToDevice )&vtkPlusPhidgetSpatialTracker::New;
  DeviceTypeClassNames["PhidgetSpatial"] = "vtkPlusPhidgetSpatialTracker";
#endif

  DeviceTypes["SavedDataSource"] = ( PointerToDevice )&vtkPlusSavedDataSource::New;
  DeviceTypeClassNames["SavedDataSource"] = "vtkPlusSavedDataSource";
  DeviceTypes["UsSimulator"] = ( PointerToDevice )&vtkPlusUsSimulatorVideoSource::New;
  DeviceTypeClassNames["UsSimulator"] = "vtkPlusUsSimulatorVideoSource";
  DeviceTypes["ImageProcessor"] = ( PointerToDevice )&vtkPlusImageProcessorVideoSource::New;
  DeviceTypeClassNames["ImageProcessor"] = "vtkPlusImageProcessorVideoSource";
  DeviceTypes["GenericSerialDevice"] = ( PointerToDevice )&vtkPlusGenericSerialDevice::New;
  DeviceTypeClassNames["GenericSerialDevice"] = "vtkPlusGenericSerialDevice";
  DeviceTypes["NoiseVideo"] = ( PointerToDevice )&vtkPlusDevice::New;
  DeviceTypeClassNames["NoiseVideo"] = "vtkPlusDevice";
#ifdef PLUS_USE_OpenIGTLink
  DeviceTypes["OpenIGTLinkVideo"] = ( PointerToDevice )&vtkPlusOpenIGTLinkVideoSource::New;
  DeviceTypeClassNames["OpenIGTLinkVideo"] = "vtkPlusOpenIGTLinkVideoSource";
#endif
#ifdef PLUS_USE_OPTIMET_CONOPROBE
  DeviceTypes["OptimetConoProbe"] = ( PointerToDevice )&vtkPlusOptimetConoProbeMeasurer::New;
#endif
#ifdef PLUS_USE_OPTITRACK
  DeviceTypes["OptiTrack"] = ( PointerToDevice )&vtkPlusOptiTrackTracker::New;
  DeviceTypeClassNames["OptiTrack"] = "vtkPlusOptiTrackTracker";
#endif
#ifdef PLUS_USE_ULTRASONIX_VIDEO
  DeviceTypes["SonixVideo"] = ( PointerToDevice )&vtkPlusSonixVideoSource::New;
  DeviceTypeClassNames["SonixVideo"] = "vtkPlusSonixVideoSource";
  DeviceTypes["SonixPortaVideo"] = ( PointerToDevice )&vtkPlusSonixPortaVideoSource::New;
  DeviceTypeClassNames["SonixPortaVideo"] = "vtkPlusSonixPortaVideoSource";
#endif
#ifdef PLUS_USE_BKPROFOCUS_VIDEO
  DeviceTypes["BkProFocusOem"] = ( PointerToDevice )&vtkPlusBkProFocusOemVideoSource::New;
  DeviceTypeClassNames["BkProFocusOem"] = "vtkPlusBkProFocusOemVideoSource";
#ifdef PLUS_USE_BKPROFOCUS_CAMERALINK
  DeviceTypes["BkProFocusCameraLink"] = ( PointerToDevice )&vtkPlusBkProFocusCameraLinkVideoSource::New;
  DeviceTypeClassNames["BkProFocusCameraLink"] = "vtkPlusBkProFocusCameraLinkVideoSource";
  DeviceTypes["BkProFocus"] = ( PointerToDevice )&vtkPlusBkProFocusCameraLinkVideoSource::New;
  DeviceTypeClassNames["BkProFocus"] = "vtkPlusBkProFocusCameraLinkVideoSource";  // for backward compatibility only
#endif
#endif
#ifdef PLUS_USE_VFW_VIDEO
  DeviceTypes["VFWVideo"] = ( PointerToDevice )&vtkPlusWin32VideoSource2::New;
  DeviceTypeClassNames["VFWVideo"] = "vtkPlusWin32VideoSource2";
#endif
#ifdef PLUS_USE_MMF_VIDEO
  DeviceTypes["MmfVideo"] = ( PointerToDevice )&vtkPlusMmfVideoSource::New;
  DeviceTypeClassNames["MmfVideo"] = "vtkPlusMmfVideoSource";
#endif
#ifdef PLUS_USE_ICCAPTURING_VIDEO
  DeviceTypes["ICCapturing"] = ( PointerToDevice )&vtkPlusICCapturingSource::New;
  DeviceTypeClassNames["ICCapturing"] = "vtkPlusICCapturingSource";
#endif
#ifdef PLUS_USE_INTERSON_VIDEO
  DeviceTypes["IntersonVideo"] = ( PointerToDevice )&vtkPlusIntersonVideoSource::New;
  DeviceTypeClassNames["IntersonVideo"] = "vtkPlusIntersonVideoSource";
#endif
#ifdef PLUS_USE_INTERSONSDKCXX_VIDEO
  DeviceTypes["IntersonSDKCxxVideo"] = ( PointerToDevice )&vtkPlusIntersonSDKCxxVideoSource::New;
  DeviceTypeClassNames["IntersonSDKCxxVideo"] = "vtkPlusIntersonSDKCxxVideoSource";
#endif
#ifdef PLUS_USE_TELEMED_VIDEO
  DeviceTypes["TelemedVideo"] = ( PointerToDevice )&vtkPlusTelemedVideoSource::New;
  DeviceTypeClassNames["TelemedVideo"] = "vtkPlusTelemedVideoSource";
#endif
#ifdef PLUS_USE_THORLABS_VIDEO
  DeviceTypes["ThorLabsVideo"] = ( PointerToDevice )&vtkPlusThorLabsVideoSource::New;
  DeviceTypeClassNames["ThorLabsVideo"] = "vtkPlusThorLabsVideoSource";
#endif
#ifdef PLUS_USE_EPIPHAN
  DeviceTypes["Epiphan"] = ( PointerToDevice )&vtkPlusEpiphanVideoSource::New;
  DeviceTypeClassNames["Epiphan"] = "vtkPlusEpiphanVideoSource";
#endif

#ifdef PLUS_USE_IntuitiveDaVinci
  DeviceTypes["IntuitiveDaVinci"] = ( PointerToDevice )&vtkPlusIntuitiveDaVinciTracker::New;
  DeviceTypeClassNames["IntuitiveDaVinci"] = "vtkPlusIntuitiveDaVinciTracker";
#endif

#ifdef PLUS_USE_PHILIPS_3D_ULTRASOUND
  DeviceTypes["iE33Video"] = ( PointerToDevice )&vtkPlusPhilips3DProbeVideoSource::New;
  DeviceTypeClassNames["iE33Video"] = "vtkPlusPhilips3DProbeVideoSource";
#endif

#ifdef PLUS_USE_CAPISTRANO_VIDEO
  DeviceTypes["CapistranoVideo"] = ( PointerToDevice )&vtkPlusCapistranoVideoSource::New;
  DeviceTypeClassNames["CapistranoVideo"] = "vtkPlusCapistranoVideoSource";
#endif
#ifdef PLUS_USE_tesseract
  DeviceTypes["VirtualTextRecognizer"] = ( PointerToDevice )&vtkPlusVirtualTextRecognizer::New;
  DeviceTypeClassNames["VirtualTextRecognizer"] = "vtkPlusVirtualTextRecognizer";
#endif
#ifdef PLUS_USE_NVIDIA_DVP
  DeviceTypes["NvidiaDVP"] = ( PointerToDevice )&vtkPlusNvidiaDVPVideoSource::New;
  DeviceTypeClassNames["NvidiaDVP"] = "vtkPlusNVidiaDVPVideoSource";
#endif

#ifdef PLUS_USE_OvrvisionPro
  DeviceTypes["OvrvisionPro"] = ( PointerToDevice )&vtkPlusOvrvisionProVideoSource::New;
  DeviceTypeClassNames["OvrvisionPro"] = "vtkPlusOvrvisionProVideoSource";
#endif

  // Virtual Devices
  DeviceTypes["VirtualMixer"] = ( PointerToDevice )&vtkPlusVirtualMixer::New;
  DeviceTypeClassNames["VirtualMixer"] = "vtkPlusVirtualMixer";
  DeviceTypes["VirtualSwitcher"] = ( PointerToDevice )&vtkPlusVirtualSwitcher::New;
  DeviceTypeClassNames["VirtualSwitcher"] = "vtkPlusVirtualSwitcher";
  DeviceTypes["VirtualDiscCapture"] = ( PointerToDevice )&vtkPlusVirtualDiscCapture::New;
  DeviceTypeClassNames["VirtualDiscCapture"] = "vtkPlusVirtualDiscCapture";
  DeviceTypes["VirtualBufferedDiscCapture"] = ( PointerToDevice )&vtkPlusVirtualDiscCapture::New;
  DeviceTypeClassNames["VirtualBufferedDiscCapture"] = "vtkPlusVirtualDiscCapture";
  DeviceTypes["VirtualVolumeReconstructor"] = ( PointerToDevice )&vtkPlusVirtualVolumeReconstructor::New;
  DeviceTypeClassNames["VirtualVolumeReconstructor"] = "vtkPlusVirtualVolumeReconstructor";
}

//----------------------------------------------------------------------------

vtkPlusDeviceFactory::~vtkPlusDeviceFactory( void )
{
}

//----------------------------------------------------------------------------

void vtkPlusDeviceFactory::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
  this->PrintAvailableDevices( os, indent );
}

//----------------------------------------------------------------------------

void vtkPlusDeviceFactory::PrintAvailableDevices( ostream& os, vtkIndent indent )
{
  os << indent << "Supported devices: " << std::endl;
  std::map<std::string, PointerToDevice>::iterator it;
  for ( it = DeviceTypes.begin(); it != DeviceTypes.end(); ++it )
  {
    if ( it->second != NULL )
    {
      vtkPlusDevice* device = ( *it->second )();
      os << indent.GetNextIndent() << "- " << it->first << " (ver: " << device->GetSdkVersion() << ")" << std::endl;
      device->Delete();
      device = NULL;
    }
  }
}

//----------------------------------------------------------------------------

PlusStatus vtkPlusDeviceFactory::CreateInstance( const char* aDeviceType, vtkPlusDevice*& aDevice, const std::string& aDeviceId )
{
  if ( aDevice != NULL )
  {
    aDevice->Delete();
    aDevice = NULL;
  }

  if ( aDeviceType == NULL )
  {
    LOG_ERROR( "Device type is undefined" );
    return PLUS_FAIL;
  }

  if ( DeviceTypes.find( aDeviceType ) == DeviceTypes.end() )
  {
    std::string listOfSupportedDevices;
    std::map<std::string, PointerToDevice>::iterator it;
    for ( it = DeviceTypes.begin(); it != DeviceTypes.end(); ++it )
    {
      if ( it->second != NULL )
      {
        vtkPlusDevice* device = ( *it->second )();
        if ( !listOfSupportedDevices.empty() )
        {
          listOfSupportedDevices.append( ", " );
        }
        listOfSupportedDevices.append( it->first );
        device->Delete();
        device = NULL;
      }
    }
    LOG_ERROR( "Unknown device type: " << aDeviceType << ". Supported devices: " << listOfSupportedDevices );
    return PLUS_FAIL;
  }

  if ( DeviceTypes[aDeviceType] == NULL )
  {
    LOG_ERROR( "Invalid factory method for device type: " << aDeviceType );
    return PLUS_FAIL;
  }

  // Call device New() function
  aDevice = ( *DeviceTypes[aDeviceType] )();
  if ( aDevice == NULL )
  {
    LOG_ERROR( "Invalid device created for device type: " << aDeviceType );
    return PLUS_FAIL;
  }

  aDevice->SetDeviceId( aDeviceId.c_str() );

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDeviceFactory::GetDeviceClassName( const std::string& deviceTypeName, std::string& classNameOut ) const
{
  std::map<std::string, std::string>::const_iterator it = DeviceTypeClassNames.find( deviceTypeName );
  if( it != DeviceTypeClassNames.end() )
  {
    classNameOut = it->second;
    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDeviceFactory::GetDeviceTypeName( const std::string& deviceClassName, std::string& typeNameOut ) const
{
  for( std::map<std::string, std::string>::const_iterator it = DeviceTypeClassNames.begin(); it != DeviceTypeClassNames.end(); ++it )
  {
    if( it->second == deviceClassName )
    {
      typeNameOut = it->first;
      return PLUS_SUCCESS;
    }
  }

  return PLUS_FAIL;
}