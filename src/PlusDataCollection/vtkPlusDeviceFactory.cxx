/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusDevice.h"
#include "vtkPlusDeviceFactory.h"

// VTK includes
#include <vtkObjectFactory.h>

//----------------------------------------------------------------------------
// Virtual devices
#include "vtkPlusVirtualMixer.h"
#include "vtkPlusVirtualSwitcher.h"
#include "vtkPlusVirtualCapture.h"
#include "vtkPlusVirtualVolumeReconstructor.h"
#include "vtkPlusVirtualDeinterlacer.h"
#include "vtkPlusImageProcessorVideoSource.h"
#include "vtkPlusGenericSerialDevice.h"
#ifdef PLUS_USE_TextRecognizer
#include "vtkPlusVirtualTextRecognizer.h"
#endif

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
#ifdef PLUS_USE_WITMOTIONTRACKER
#include "vtkPlusWitMotionTracker.h"
#endif
#ifdef PLUS_USE_INTELREALSENSE
#include "vtkPlusIntelRealSense.h"
#endif
#ifdef PLUS_USE_OPTICAL_MARKER_TRACKER
#include "vtkPlusOpticalMarkerTracker.h"
#endif
#ifdef PLUS_USE_ATRACSYS
#include "vtkPlusAtracsysTracker.h"
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
#include "vtkPlusOptiTrack.h"
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
#ifdef PLUS_USE_AGILENT
#include "vtkPlusAgilentScopeTracker.h"
#endif
#ifdef PLUS_USE_LEAPMOTION
#include "vtkPlusLeapMotion.h"
#endif
#ifdef PLUS_USE_STEAMVR
#include "vtkPlusSteamVRTracker.h"
#endif
#ifdef PLUS_USE_PICOSCOPE
  #include "vtkPlusPicoScopeDataSource.h"
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

#ifdef PLUS_USE_INTERSONARRAYSDKCXX_VIDEO
#include "vtkPlusIntersonArraySDKCxxVideoSource.h"
#endif

#ifdef PLUS_USE_TELEMED_VIDEO
#include "Telemed\vtkPlusTelemedVideoSource.h"
#endif

#ifdef PLUS_USE_SPINNAKER_VIDEO
#include "vtkPlusSpinnakerVideoSource.h"
#endif

#ifdef PLUS_USE_BLACKMAGIC_DECKLINK
#include "BlackMagic/vtkPlusDeckLinkVideoSource.h"
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

#ifdef PLUS_USE_WINPROBE_VIDEO
#include "WinProbe\vtkPlusWinProbeVideoSource.h"
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

#ifdef PLUS_USE_OpenCV_VIDEO
#include "vtkPlusOpenCVCaptureVideoSource.h"
#endif

#ifdef PLUS_USE_OPENHAPTICS
#include "vtkPlusOpenHapticsDevice.h"
#endif

#ifdef PLUS_USE_V4L2
#include "vtkPlusV4L2VideoSource.h"
#endif

#ifdef PLUS_USE_INFRARED_SEEK_CAM
#include "vtkInfraredSeekCam.h"
#endif

#ifdef PLUS_USE_ANDOR_CAMERA
#include "vtkPlusAndorVideoSource.h"
#endif

#ifdef PLUS_USE_INFRARED_TEQ1_CAM
#include "vtkInfraredTEQ1Cam.h"
#endif

#ifdef PLUS_USE_CLARIUS
#include "vtkPlusClarius.h"
#endif

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusDeviceFactory);

//----------------------------------------------------------------------------
vtkPlusDeviceFactory::vtkPlusDeviceFactory()
{
  RegisterDevice("FakeTracker", "vtkPlusFakeTracker", (PointerToDevice)&vtkPlusFakeTracker::New);
  RegisterDevice("ChRobotics", "vtkPlusChRoboticsTracker", (PointerToDevice)&vtkPlusChRoboticsTracker::New);
  RegisterDevice("Microchip", "vtkPlusMicrochipTracker", (PointerToDevice)&vtkPlusMicrochipTracker::New);

#ifdef PLUS_USE_3dConnexion_TRACKER
  // 3dConnexion tracker is supported on Windows only
  RegisterDevice("3dConnexion", "vtkPlus3dConnexionTracker", (PointerToDevice)&vtkPlus3dConnexionTracker::New);
#endif
#ifdef PLUS_USE_OpenIGTLink
  RegisterDevice("OpenIGTLinkTracker", "vtkPlusOpenIGTLinkTracker", (PointerToDevice)&vtkPlusOpenIGTLinkTracker::New);
#endif
#ifdef PLUS_USE_BRACHY_TRACKER
  RegisterDevice("BrachyTracker", "vtkPlusBrachyTracker", (PointerToDevice)&vtkPlusBrachyTracker::New);
#endif
#ifdef PLUS_USE_USDIGITALENCODERS_TRACKER
  RegisterDevice("USDigitalEncodersTracker", "vtkPlusUSDigitalEncodersTracker", (PointerToDevice)&vtkPlusUSDigitalEncodersTracker::New);
#endif
#ifdef PLUS_USE_NDI_CERTUS
  RegisterDevice("CertusTracker", "vtkPlusNDICertusTracker", (PointerToDevice)&vtkPlusNDICertusTracker::New);
#endif
#ifdef PLUS_USE_NDI
  RegisterDevice("NDITracker", "vtkPlusNDITracker", (PointerToDevice)&vtkPlusNDITracker::New);
  RegisterDevice("PolarisTracker", "vtkPlusNDITracker", (PointerToDevice)&vtkPlusNDITracker::New);
  RegisterDevice("AuroraTracker", "vtkPlusNDITracker", (PointerToDevice)&vtkPlusNDITracker::New);
#endif
#ifdef PLUS_USE_MICRONTRACKER
  RegisterDevice("MicronTracker", "vtkPlusMicronTracker", (PointerToDevice)&vtkPlusMicronTracker::New);
#endif
#ifdef PLUS_USE_WITMOTIONTRACKER
  RegisterDevice("WitMotionTracker", "vtkPlusWitMotionTracker", (PointerToDevice)&vtkPlusWitMotionTracker::New);
#endif
#ifdef PLUS_USE_INTELREALSENSE
  RegisterDevice("IntelRealSense", "vtkPlusIntelRealSense", (PointerToDevice)&vtkPlusIntelRealSense::New);
#endif

#ifdef PLUS_USE_OPTICAL_MARKER_TRACKER
  RegisterDevice("OpticalMarkerTracker", "vtkPlusOpticalMarkerTracker", (PointerToDevice)&vtkPlusOpticalMarkerTracker::New);
#endif
#ifdef PLUS_USE_ATRACSYS
  RegisterDevice("AtracsysTracker", "vtkPlusAtracsysTracker", (PointerToDevice)&vtkPlusAtracsysTracker::New);
#endif
#ifdef PLUS_USE_STEALTHLINK
  RegisterDevice("StealthLinkTracker", "vtkPlusStealthLinkTracker", (PointerToDevice)&vtkPlusStealthLinkTracker::New);
#endif
#ifdef PLUS_USE_Ascension3DG
  RegisterDevice("Ascension3DG", "vtkAscension3DGTracker", (PointerToDevice)&vtkAscension3DGTracker::New);
#endif
#ifdef PLUS_USE_Ascension3DGm
  RegisterDevice("Ascension3DGm", "vtkAscension3DGmTracker", (PointerToDevice)&vtkAscension3DGmTracker::New);
#endif
#ifdef PLUS_USE_PHIDGET_SPATIAL_TRACKER
  RegisterDevice("PhidgetSpatial", "vtkPlusPhidgetSpatialTracker", (PointerToDevice)&vtkPlusPhidgetSpatialTracker::New);
#endif
#ifdef PLUS_USE_LEAPMOTION
  RegisterDevice("LeapMotion", "vtkPlusLeapMotion", (PointerToDevice)&vtkPlusLeapMotion::New);
#endif

  RegisterDevice("SavedDataSource", "vtkPlusSavedDataSource", (PointerToDevice)&vtkPlusSavedDataSource::New);
  RegisterDevice("UsSimulator", "vtkPlusUsSimulatorVideoSource", (PointerToDevice)&vtkPlusUsSimulatorVideoSource::New);
  RegisterDevice("ImageProcessor", "vtkPlusImageProcessorVideoSource", (PointerToDevice)&vtkPlusImageProcessorVideoSource::New);
  RegisterDevice("GenericSerialDevice", "vtkPlusGenericSerialDevice", (PointerToDevice)&vtkPlusGenericSerialDevice::New);
  RegisterDevice("NoiseVideo", "vtkPlusDevice", (PointerToDevice)&vtkPlusDevice::New);
#ifdef PLUS_USE_OpenIGTLink
  RegisterDevice("OpenIGTLinkVideo", "vtkPlusOpenIGTLinkVideoSource", (PointerToDevice)&vtkPlusOpenIGTLinkVideoSource::New);
#endif
#ifdef PLUS_USE_OPTIMET_CONOPROBE
  RegisterDevice("OptimetConoProbe", "vtkPlusOptimetConoProbeMeasurer", (PointerToDevice)&vtkPlusOptimetConoProbeMeasurer::New);
#endif
#ifdef PLUS_USE_OPTITRACK
  RegisterDevice("OptiTrack", "vtkPlusOptiTrack", (PointerToDevice)&vtkPlusOptiTrack::New);
#endif
#ifdef PLUS_USE_ULTRASONIX_VIDEO
  RegisterDevice("SonixVideo", "vtkPlusSonixVideoSource", (PointerToDevice)&vtkPlusSonixVideoSource::New);
  RegisterDevice("SonixPortaVideo", "vtkPlusSonixPortaVideoSource", (PointerToDevice)&vtkPlusSonixPortaVideoSource::New);
#endif
#ifdef PLUS_USE_BKPROFOCUS_VIDEO
  RegisterDevice("BkProFocusOem", "vtkPlusBkProFocusOemVideoSource", (PointerToDevice)&vtkPlusBkProFocusOemVideoSource::New);
#ifdef PLUS_USE_BKPROFOCUS_CAMERALINK
  RegisterDevice("BkProFocusCameraLink", "vtkPlusBkProFocusCameraLinkVideoSource", (PointerToDevice)&vtkPlusBkProFocusCameraLinkVideoSource::New);
  RegisterDevice("BkProFocus", "vtkPlusBkProFocusCameraLinkVideoSource", (PointerToDevice)&vtkPlusBkProFocusCameraLinkVideoSource::New);
#endif
#endif
#ifdef PLUS_USE_VFW_VIDEO
  RegisterDevice("VFWVideo", "vtkPlusWin32VideoSource2", (PointerToDevice)&vtkPlusWin32VideoSource2::New);
#endif
#ifdef PLUS_USE_MMF_VIDEO
  RegisterDevice("MmfVideo", "vtkPlusMmfVideoSource", (PointerToDevice)&vtkPlusMmfVideoSource::New);
#endif
#ifdef PLUS_USE_ICCAPTURING_VIDEO
  RegisterDevice("ICCapturing", "vtkPlusICCapturingSource", (PointerToDevice)&vtkPlusICCapturingSource::New);
#endif
#ifdef PLUS_USE_INTERSON_VIDEO
  RegisterDevice("IntersonVideo", "vtkPlusIntersonVideoSource", (PointerToDevice)&vtkPlusIntersonVideoSource::New);
#endif
#ifdef PLUS_USE_INTERSONSDKCXX_VIDEO
  RegisterDevice("IntersonSDKCxxVideo", "vtkPlusIntersonSDKCxxVideoSource", (PointerToDevice)&vtkPlusIntersonSDKCxxVideoSource::New);
#endif
#ifdef PLUS_USE_INTERSONARRAYSDKCXX_VIDEO
  RegisterDevice("IntersonArraySDKCxxVideo", "vtkPlusIntersonArraySDKCxxVideoSource", (PointerToDevice)&vtkPlusIntersonArraySDKCxxVideoSource::New);
#endif
#ifdef PLUS_USE_TELEMED_VIDEO
  RegisterDevice("TelemedVideo", "vtkPlusTelemedVideoSource", (PointerToDevice)&vtkPlusTelemedVideoSource::New);
#endif
#ifdef PLUS_USE_SPINNAKER_VIDEO
  RegisterDevice("SpinnakerVideo", "vtkPlusSpinnakerVideoSource", (PointerToDevice)&vtkPlusSpinnakerVideoSource::New);
#endif
#ifdef PLUS_USE_BLACKMAGIC_DECKLINK
  RegisterDevice("DeckLinkVideo", "vtkPlusDeckLinkVideoSource", (PointerToDevice)&vtkPlusDeckLinkVideoSource::New);
#endif
#ifdef PLUS_USE_THORLABS_VIDEO
  RegisterDevice("ThorLabsVideo", "vtkPlusThorLabsVideoSource", (PointerToDevice)&vtkPlusThorLabsVideoSource::New);
#endif
#ifdef PLUS_USE_EPIPHAN
  RegisterDevice("Epiphan", "vtkPlusEpiphanVideoSource", (PointerToDevice)&vtkPlusEpiphanVideoSource::New);
#endif

#ifdef PLUS_USE_IntuitiveDaVinci
  RegisterDevice("IntuitiveDaVinci", "vtkPlusIntuitiveDaVinciTracker", (PointerToDevice)&vtkPlusIntuitiveDaVinciTracker::New);
#endif

#ifdef PLUS_USE_PHILIPS_3D_ULTRASOUND
  RegisterDevice("iE33Video", "vtkPlusPhilips3DProbeVideoSource", (PointerToDevice)&vtkPlusPhilips3DProbeVideoSource::New);
#endif

#ifdef PLUS_USE_CAPISTRANO_VIDEO
  RegisterDevice("CapistranoVideo", "vtkPlusCapistranoVideoSource", (PointerToDevice)&vtkPlusCapistranoVideoSource::New);
#endif

#ifdef PLUS_USE_WINPROBE_VIDEO
  RegisterDevice("WinProbeVideo", "vtkPlusWinProbeVideoSource", (PointerToDevice)&vtkPlusWinProbeVideoSource::New);
#endif

#ifdef PLUS_USE_TextRecognizer
  RegisterDevice("VirtualTextRecognizer", "vtkPlusVirtualTextRecognizer", (PointerToDevice)&vtkPlusVirtualTextRecognizer::New);
#endif
#ifdef PLUS_USE_NVIDIA_DVP
  RegisterDevice("NVidiaDVP", "vtkPlusNvidiaDVPVideoSource", (PointerToDevice)&vtkPlusNvidiaDVPVideoSource::New);
#endif

#ifdef PLUS_USE_OvrvisionPro
  RegisterDevice("OvrvisionPro", "vtkPlusOvrvisionProVideoSource", (PointerToDevice)&vtkPlusOvrvisionProVideoSource::New);
#endif
#ifdef PLUS_USE_AGILENT
  RegisterDevice("AgilentScope", "vtkPlusAgilentScopeTracker", (PointerToDevice)&vtkPlusAgilentScopeTracker::New);
#endif
#ifdef PLUS_USE_PICOSCOPE
  RegisterDevice("PicoScope", "vtkPlusPicoScopeDataSource", (PointerToDevice)&vtkPlusPicoScopeDataSource::New);
#endif
#ifdef PLUS_USE_OpenCV_VIDEO
  RegisterDevice("OpenCVVideo", "vtkPlusOpenCVCaptureVideoSource", (PointerToDevice)&vtkPlusOpenCVCaptureVideoSource::New);
#endif

#ifdef PLUS_USE_V4L2
  RegisterDevice("V4L2Video", "vtkPlusV4L2VideoSource", (PointerToDevice)&vtkPlusV4L2VideoSource::New);
#endif

#ifdef PLUS_USE_OPENHAPTICS
  RegisterDevice("OpenHaptics", "vtkPlusOpenHapticsDevice", (PointerToDevice)&vtkPlusOpenHapticsDevice::New);
#endif

#ifdef PLUS_USE_INFRARED_SEEK_CAM
  RegisterDevice("InfraredSeekCam", "vtkInfraredSeekCam", (PointerToDevice)&vtkInfraredSeekCam::New);
#endif

#ifdef PLUS_USE_ANDOR_CAMERA
  RegisterDevice("AndorCamera", "vtkPlusAndorVideoSource", (PointerToDevice)&vtkPlusAndorVideoSource::New);
#endif

#ifdef PLUS_USE_INFRARED_TEQ1_CAM
  RegisterDevice("InfraredTEQ1Cam", "vtkInfraredTEQ1Cam", (PointerToDevice)&vtkInfraredTEQ1Cam::New);
#endif

#ifdef PLUS_USE_CLARIUS
  RegisterDevice("Clarius", "vtkPlusClarius", (PointerToDevice)&vtkPlusClarius::New);
#endif

#ifdef PLUS_USE_STEAMVR
  RegisterDevice("SteamVRTracker", "vtkPlusSteamVRTracker", (PointerToDevice)& vtkPlusSteamVRTracker::New);
#endif

  // Virtual Devices
  RegisterDevice("VirtualMixer", "vtkPlusVirtualMixer", (PointerToDevice)&vtkPlusVirtualMixer::New);
  RegisterDevice("VirtualSwitcher", "vtkPlusVirtualSwitcher", (PointerToDevice)&vtkPlusVirtualSwitcher::New);
  RegisterDevice("VirtualCapture", "vtkPlusVirtualCapture", (PointerToDevice)&vtkPlusVirtualCapture::New);
  RegisterDevice("VirtualDiscCapture", "vtkPlusVirtualCapture", (PointerToDevice)&vtkPlusVirtualCapture::New); // for backward compatibility
  RegisterDevice("VirtualBufferedCapture", "vtkPlusVirtualCapture", (PointerToDevice)&vtkPlusVirtualCapture::New); // for backward compatibility
  RegisterDevice("VirtualVolumeReconstructor", "vtkPlusVirtualVolumeReconstructor", (PointerToDevice)&vtkPlusVirtualVolumeReconstructor::New);
  RegisterDevice("VirtualDeinterlacer", "vtkPlusVirtualDeinterlacer", (PointerToDevice)&vtkPlusVirtualDeinterlacer::New);
}

//----------------------------------------------------------------------------
vtkPlusDeviceFactory::~vtkPlusDeviceFactory(void)
{
}

//----------------------------------------------------------------------------
void vtkPlusDeviceFactory::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  this->PrintAvailableDevices(os, indent);
}

//----------------------------------------------------------------------------
void vtkPlusDeviceFactory::PrintAvailableDevices(ostream& os, vtkIndent indent)
{
  os << indent << "Supported devices: " << std::endl;
  std::map<std::string, PointerToDevice>::iterator it;
  for (it = DeviceTypes.begin(); it != DeviceTypes.end(); ++it)
  {
    if (it->second != NULL)
    {
      vtkPlusDevice* device = (*it->second)();
      os << indent.GetNextIndent() << "- " << it->first << " (ver: " << device->GetSdkVersion() << ")" << std::endl;
      device->Delete();
      device = NULL;
    }
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDeviceFactory::CreateInstance(const char* aDeviceType, vtkPlusDevice*& aDevice, const std::string& aDeviceId)
{
  if (aDevice != NULL)
  {
    aDevice->Delete();
    aDevice = NULL;
  }

  if (aDeviceType == NULL)
  {
    LOG_ERROR("Device type is undefined");
    return PLUS_FAIL;
  }

  if (DeviceTypes.find(aDeviceType) == DeviceTypes.end())
  {
    std::string listOfSupportedDevices;
    std::map<std::string, PointerToDevice>::iterator it;
    for (it = DeviceTypes.begin(); it != DeviceTypes.end(); ++it)
    {
      if (it->second != NULL)
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
    LOG_ERROR("Unknown device type: " << aDeviceType << ". Supported devices: " << listOfSupportedDevices);
    return PLUS_FAIL;
  }

  if (DeviceTypes[aDeviceType] == NULL)
  {
    LOG_ERROR("Invalid factory method for device type: " << aDeviceType);
    return PLUS_FAIL;
  }

  // Call device New() function
  aDevice = (*DeviceTypes[aDeviceType])();
  if (aDevice == NULL)
  {
    LOG_ERROR("Invalid device created for device type: " << aDeviceType);
    return PLUS_FAIL;
  }

  aDevice->SetDeviceId(aDeviceId.c_str());

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDeviceFactory::GetDeviceClassName(const std::string& deviceTypeName, std::string& classNameOut) const
{
  std::map<std::string, std::string>::const_iterator it = DeviceTypeClassNames.find(deviceTypeName);
  if (it != DeviceTypeClassNames.end())
  {
    classNameOut = it->second;
    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDeviceFactory::GetDeviceTypeName(const std::string& deviceClassName, std::string& typeNameOut) const
{
  for (std::map<std::string, std::string>::const_iterator it = DeviceTypeClassNames.begin(); it != DeviceTypeClassNames.end(); ++it)
  {
    if (it->second == deviceClassName)
    {
      typeNameOut = it->first;
      return PLUS_SUCCESS;
    }
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
void vtkPlusDeviceFactory::RegisterDevice(const std::string& deviceTypeName, const std::string& deviceClassName, vtkPlusDeviceFactory::PointerToDevice constructionMethod)
{
  vtkPlusDeviceFactory::DeviceTypes[deviceTypeName] = constructionMethod;
  vtkPlusDeviceFactory::DeviceTypeClassNames[deviceTypeName] = deviceClassName;
}
