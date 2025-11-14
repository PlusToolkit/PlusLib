# Supported Devices

PlusLib supports **58+ devices** across multiple categories for data acquisition in image-guided intervention systems.

## Quick Jump

- [Tracking Devices](#tracking-devices) (13 devices)
- [Ultrasound Devices](#ultrasound-devices) (10 devices)
- [Video Capture Devices](#video-capture) (8 devices)
- [Camera Devices](#cameras) (8 devices)
- [Sensor Devices](#sensors) (7 devices)
- [Other Devices](#other-devices) (5 devices)
- [Virtual Devices](#virtual-devices) (7 devices)

---

## Tracking Devices

Optical and electromagnetic tracking systems for tool and patient tracking.

### Optical Tracking

| Device | Description | Page |
|--------|-------------|------|
| **NDI Polaris** | Optical tracker with passive or active markers | [DeviceNDI](DeviceNDI.md) |
| **NDI Aurora** | Electromagnetic tracker with 5DOF/6DOF sensors | [DeviceNDI](DeviceNDI.md) |
| **NDI Vega** | High-performance optical tracker | [DeviceNDI](DeviceNDI.md) |
| **NDI Certus** | Next-generation optical tracking | [DeviceNDI](DeviceNDI.md) |
| **Claron MicronTracker** | Marker-based optical tracker | [DeviceMicronTracker](DeviceMicronTracker.md) |
| **Atracsys** | High-speed optical tracking | [DeviceAtracsys](DeviceAtracsys.md) |
| **OptiTrack** | Motion capture tracking system | [DeviceOptiTrack](DeviceOptiTrack.md) |
| **Optical Marker Tracker** | Generic optical marker tracking | [DeviceOpticalMarkerTracker](DeviceOpticalMarkerTracker.md) |

### Electromagnetic Tracking

| Device | Description | Page |
|--------|-------------|------|
| **Ascension 3DG** | 3D Guidance electromagnetic tracker | [DeviceAscension3DG](DeviceAscension3DG.md) |
| **NDI Aurora** | EM tracker (also listed above) | [DeviceNDI](DeviceNDI.md) |

### Other Tracking

| Device | Description | Page |
|--------|-------------|------|
| **OpenIGTLink Tracker** | Network-based tracking via OpenIGTLink | [DeviceOpenIGTLinkTracker](DeviceOpenIGTLinkTracker.md) |
| **Fake Tracker** | Simulated tracking for testing | [DeviceFakeTracker](DeviceFakeTracker.md) |
| **Generic Sensor** | Custom sensor integration | [DeviceGenericSensor](DeviceGenericSensor.md) |
| **StealthLink** | Medtronic StealthStation integration | [DeviceStealthLink](DeviceStealthLink.md) |
| **WitMotion** | IMU-based orientation tracking | [DeviceWitMotion](DeviceWitMotion.md) |

---

## Ultrasound Devices

Real-time ultrasound image acquisition from various manufacturers.

| Device | Description | Page |
|--------|-------------|------|
| **BK ProFocus** | BK Medical ultrasound systems | [DeviceBkProFocus](DeviceBkProFocus.md) |
| **Clarius (CAST)** | Clarius wireless ultrasound (CAST API) | [DeviceClarius](DeviceClarius.md) |
| **Clarius (OEM)** | Clarius wireless ultrasound (OEM API) | [DeviceClariusOEM](DeviceClariusOEM.md) |
| **Interson (Old SDK)** | Interson USB ultrasound (legacy) | [DeviceIntersonSDKCxx](DeviceIntersonSDKCxx.md) |
| **Interson (New SDK)** | Interson USB ultrasound (current) | [DeviceIntersonArray](DeviceIntersonArray.md) |
| **Philips** | Philips ultrasound systems | [DevicePhilips](DevicePhilips.md) |
| **Sonix/Ultrasonix** | Analogic/BK Sonix ultrasound | [DeviceSonixVideo](DeviceSonixVideo.md) |
| **Telemed** | Telemed ultrasound systems | [DeviceTelemed](DeviceTelemed.md) |
| **Capistrano Labs** | USB ultrasound devices | [DeviceCapistranoLabs](DeviceCapistranoLabs.md) |
| **US Simulator** | Simulated ultrasound for testing | [DeviceUsSimulator](DeviceUsSimulator.md) |

---

## Video Capture

Video capture from frame grabbers and video sources.

| Device | Description | Page |
|--------|-------------|------|
| **Epiphan** | Epiphan frame grabbers (DVI2PCIe, etc.) | [DeviceEpiphan](DeviceEpiphan.md) |
| **IC Capturing** | Imaging Source cameras via IC Imaging Control | [DeviceICCapturing](DeviceICCapturing.md) |
| **Microsoft Media Foundation** | Windows Media Foundation API | [DeviceMmfVideo](DeviceMmfVideo.md) |
| **OpenCV Video** | Video via OpenCV (files, cameras) | [DeviceOpenCVVideo](DeviceOpenCVVideo.md) |
| **Video for Windows** | Legacy VFW capture | [DeviceVFWVideo](DeviceVFWVideo.md) |
| **Spinnaker Video** | FLIR Spinnaker SDK cameras | [DeviceSpinnakerVideo](DeviceSpinnakerVideo.md) |
| **BlackMagic DeckLink** | BlackMagic Design capture cards | [DeviceBlackMagicDeckLink](DeviceBlackMagicDeckLink.md) |
| **DAQ VideoSource** | Data acquisition board video | [DeviceDAQVideoSource](DeviceDAQVideoSource.md) |

---

## Cameras

3D cameras and depth sensors.

| Device | Description | Page |
|--------|-------------|------|
| **Azure Kinect** | Microsoft Azure Kinect depth camera | [DeviceAzureKinect](DeviceAzureKinect.md) |
| **Intel RealSense** | Intel RealSense depth cameras | [DeviceIntelRealSense](DeviceIntelRealSense.md) |
| **Ovrvision Pro** | Stereo camera for VR | [DeviceOvrvisionPro](DeviceOvrvisionPro.md) |
| **Revopoint 3D Camera** | Structured light 3D scanner | [DeviceRevopoint3DCamera](DeviceRevopoint3DCamera.md) |
| **Infrared Seek** | FLIR/Seek thermal camera | [DeviceInfraredSeekCam](DeviceInfraredSeekCam.md) |
| **Infrared TEEV2** | Thermal Expert EV2 camera | [DeviceInfraredTEEV2Cam](DeviceInfraredTEEV2Cam.md) |
| **Infrared TEQ1** | Thermal Expert Q1 camera | [DeviceInfraredTEQ1Cam](DeviceInfraredTEQ1Cam.md) |
| **Ultraviolet PCO** | PCO UV camera | [DeviceUltravioletPCOUVCam](DeviceUltravioletPCOUVCam.md) |

---

## Sensors

Specialized sensors and input devices.

| Device | Description | Page |
|--------|-------------|------|
| **Phidget Spatial** | Phidget IMU/spatial sensors | [DevicePhidgetSpatial](DevicePhidgetSpatial.md) |
| **CHRobotics UM6** | UM6 IMU orientation sensor | [DeviceChRobotics](DeviceChRobotics.md) |
| **Microchip MM7150** | MM7150 IMU sensor | [DeviceMicrochip](DeviceMicrochip.md) |
| **LeapMotion** | Hand tracking sensor | [DeviceLeapMotion](DeviceLeapMotion.md) |
| **3dConnexion Mouse** | 3D navigation mouse | [Device3dConnexion](Device3dConnexion.md) |
| **Optimet ConoProbe** | Optical distance sensor | [DeviceOptimetConoProbeDist](DeviceOptimetConoProbeDist.md) |
| **ThorLabs Spectrometer** | Compact spectrometer | [DeviceThorLabsVideoSource](DeviceThorLabsVideoSource.md) |
| **Agilent Oscilloscope** | Oscilloscope data acquisition | [DeviceAgilent](DeviceAgilent.md) |

---

## Other Devices

Miscellaneous hardware interfaces.

| Device | Description | Page |
|--------|-------------|------|
| **Brachy Stepper** | Stepper motor control for brachytherapy | [DeviceBrachyStepper](DeviceBrachyStepper.md) |
| **Generic Serial** | Generic serial port communication | [DeviceGenericSerial](DeviceGenericSerial.md) |
| **OpenHaptics** | Haptic device integration | [DeviceOpenHaptics](DeviceOpenHaptics.md) |
| **OpenIGTLink Video** | Network video via OpenIGTLink | [DeviceOpenIGTLinkVideo](DeviceOpenIGTLinkVideo.md) |
| **Saved Data Source** | Replay saved sequence files | [DeviceSavedDataSource](DeviceSavedDataSource.md) |

---

## Virtual Devices

Software-based virtual devices for processing and testing.

| Device | Description | Page |
|--------|-------------|------|
| **Virtual Capture** | Virtual video capture device | [DeviceVirtualCapture](DeviceVirtualCapture.md) |
| **Virtual Mixer** | Mix multiple video streams | [DeviceVirtualMixer](DeviceVirtualMixer.md) |
| **Virtual Switcher** | Switch between video sources | [DeviceVirtualSwitcher](DeviceVirtualSwitcher.md) |
| **Virtual Text Recognizer** | OCR for video streams | [DeviceVirtualTextRecognizer](DeviceVirtualTextRecognizer.md) |
| **Virtual Volume Reconstructor** | Real-time volume reconstruction | [DeviceVirtualVolumeReconstructor](DeviceVirtualVolumeReconstructor.md) |
| **Virtual Discrepancy** | Track calibration discrepancies | [DeviceVirtualDiscCapture](DeviceVirtualDiscCapture.md) |
| **Enhance US Sequence** | Enhance ultrasound image sequences | [DeviceEnhanceUsTrpSequence](DeviceEnhanceUsTrpSequence.md) |

---

## Device Configuration

All devices are configured via XML configuration files. See:

- [Configuration File Format](../file-formats/FileApplicationConfiguration.md) - Complete configuration reference
- [Common Coordinate Systems](../CommonCoordinateSystems.md) - Coordinate system conventions
- [Example Configurations](../file-formats/FileApplicationConfiguration.md#device-examples) - Sample device setups

## Device Development

Want to add support for a new device?

- [Creating Devices](../developer-guide/creating-devices.md) - Developer guide for adding devices
- [Coding Conventions](../developer-guide/coding-conventions.md) - Code style requirements
- [Contributing Guide](../developer-guide/contributing.md) - How to contribute

---

## Device Categories Summary

| Category | Device Count |
|----------|--------------|
| Tracking Devices | 13 |
| Ultrasound Devices | 10 |
| Video Capture | 8 |
| Cameras | 8 |
| Sensors | 7 |
| Other Devices | 5 |
| Virtual Devices | 7 |
| **Total** | **58+** |
