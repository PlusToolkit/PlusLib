# PlusLib Documentation

Welcome to the PlusLib documentation! PlusLib is a software library for data acquisition, pre-processing, and calibration for navigated image-guided interventions.

## What is PlusLib?

PlusLib is an open-source library that provides a comprehensive set of tools for:

- **Data Acquisition**: Real-time capture from 58+ tracking, imaging, and sensor devices
- **Tracking**: Support for optical and electromagnetic tracking systems  
- **Calibration**: Tools for spatial and temporal calibration of imaging systems
- **Image Processing**: Algorithms for ultrasound image enhancement and analysis
- **Volume Reconstruction**: 3D volume reconstruction from tracked ultrasound images
- **Integration**: OpenIGTLink support for communication with other software (3D Slicer, MITK, etc.)

## Quick Links

### Getting Started

* [Quick Start Guide](getting-started/quick-start.md) - Get up and running quickly
* [Build Instructions](getting-started/build-instructions.md) - How to build PlusLib
* [Overview](getting-started/overview.md) - Learn about PlusLib features

### Devices & Hardware

PlusLib supports **58+ devices** across multiple categories. See [All Devices](devices/index.md) for the complete list.

## Support & Community

* **GitHub**: [PlusToolkit/PlusLib](https://github.com/PlusToolkit/PlusLib)
* **Issues**: [Report bugs or request features](https://github.com/PlusToolkit/PlusLib/issues)
* **Discussions**: [Join our community](https://github.com/PlusToolkit/PlusLib/discussions)
* **Website**: [PlusToolkit.org](http://www.plustoolkit.org)
* **Test Dashboard**: [View test results](testing/dashboards.md)

## License

PlusLib is licensed under the BSD-3-Clause License. See [License](about/license.md) for details.

## Citation

If you use PlusLib in your research, please cite:

Andras Lasso, Tamas Heffter, Adam Rankin, Csaba Pinter, Tamas Ungi, and Gabor Fichtinger, 
"PLUS: Open-source toolkit for ultrasound-guided intervention systems", 
*IEEE Trans Biomed Eng.* 2014 Oct;61(10):2527-37. doi: 10.1109/TBME.2014.2322864

See [Citation](about/citation.md) for more details.

## Contents

```{toctree}
:maxdepth: 2
:caption: Getting Started

getting-started/overview
getting-started/build-instructions
getting-started/quick-start
SupportedPlatforms
```

```{toctree}
:maxdepth: 2
:caption: Devices

devices/index
```

```{toctree}
:maxdepth: 1
:caption: Tracking Devices
:hidden:

devices/DeviceNDI
devices/DeviceNDICertus
devices/DeviceAscension3DG
devices/DeviceMicron
devices/DeviceAtracsys
devices/DeviceOptiTrack
devices/DeviceOpenIGTLinkTracker
devices/DeviceOpticalMarkerTracker
devices/DeviceFakeTracker
devices/DeviceGenericSensor
devices/DeviceStealthLink
devices/DeviceWitMotionTracker
```

```{toctree}
:maxdepth: 1
:caption: Ultrasound Devices
:hidden:

devices/DeviceBkProFocus
devices/DeviceClarius
devices/DeviceClariusOEM
devices/DeviceInterson
devices/DeviceIntersonSDKCxx
devices/DevicePhilips
devices/DeviceSonixVideo
devices/DeviceTelemed
devices/DeviceCapistrano
devices/DeviceUsSimulatorVideo
```

```{toctree}
:maxdepth: 1
:caption: Video Capture
:hidden:

devices/DeviceEpiphan
devices/DeviceICCapturing
devices/DeviceMicrosoftMediaFoundation
devices/DeviceOpenCVVideo
devices/DeviceVideoForWindows
devices/DeviceSpinnakerVideo
devices/DeviceBlackMagicDeckLink
devices/DeviceDAQVideoSource
```

```{toctree}
:maxdepth: 1
:caption: Cameras
:hidden:

devices/DeviceAzureKinect
devices/DeviceIntelRealSense
devices/DeviceOvrvisionPro
devices/DeviceRevopointSurface
devices/DeviceInfraredSeekCam
devices/DeviceInfraredTEEV2Cam
devices/DeviceInfraredTEQ1Cam
devices/DeviceUltravioletPCOUVCam
```

```{toctree}
:maxdepth: 1
:caption: Sensors
:hidden:

devices/DevicePhidgetSpatial
devices/DeviceChRobotics
devices/DeviceMicrochip
devices/DeviceLeapMotion
devices/Device3dConnexion
devices/DeviceOptimetConoProbe
devices/DeviceThorLabs
devices/DeviceAgilent
```

```{toctree}
:maxdepth: 1
:caption: Other Devices
:hidden:

devices/DeviceBrachy
devices/DeviceGenericSerial
devices/DeviceOpenHaptics
devices/DeviceOpenIGTLinkVideo
devices/DeviceSavedDataSource
```

```{toctree}
:maxdepth: 1
:caption: Virtual Devices
:hidden:

devices/DeviceVirtualCapture
devices/DeviceVirtualMixer
devices/DeviceVirtualSwitcher
devices/DeviceVirtualTextRecognizer
devices/DeviceVirtualVolumeReconstructor
devices/DeviceEnhanceUsTrpSequence
```

```{toctree}
:maxdepth: 2
:caption: Algorithms

algorithms/AlgorithmProbeCalibration
algorithms/AlgorithmPhantomRegistration
algorithms/AlgorithmPivotCalibration
algorithms/AlgorithmTemporalCalibration
algorithms/AlgorithmVolumeReconstruction
algorithms/AlgorithmRfProcessing
```

```{toctree}
:maxdepth: 2
:caption: Applications

applications/ApplicationProbeCalibration
applications/ApplicationTemporalCalibration
applications/ApplicationVolumeReconstructor
applications/ApplicationTrackingTest
applications/ApplicationViewSequenceFile
applications/ApplicationEditSequenceFile
applications/ApplicationRfProcessor
applications/ApplicationScanConvert
applications/ApplicationExtractScanLines
applications/ApplicationDrawScanlines
applications/ApplicationDrawClipRegion
applications/ApplicationCreateSliceModels
applications/ApplicationEnhanceUsTrpSequence
applications/ApplicationPlusVersion
```

```{toctree}
:maxdepth: 2
:caption: Configuration

file-formats/FileApplicationConfiguration
file-formats/FileSequenceFile
file-formats/FileLog
CoordinateSystemDefinitions
CommonCoordinateSystems
PlusServerCommands
```

```{toctree}
:maxdepth: 2
:caption: User Guide

UltrasoundImageOrientation
UsImagingParameters
```

```{toctree}
:maxdepth: 2
:caption: Developer Guide

developer-guide/contributing
developer-guide/coding-conventions
developer-guide/creating-devices
developer-guide/api-reference
```

```{toctree}
:maxdepth: 2
:caption: Testing

testing/dashboards
testing/running-tests
```

```{toctree}
:maxdepth: 2
:caption: About

about/license
about/citation
```

```{toctree}
:maxdepth: 2
:caption: 3D Model Catalog

plusmodelcatalog:index
plusmodelcatalog:catalog/tools
plusmodelcatalog:catalog/tracking-fixtures
plusmodelcatalog:catalog/fcal-phantoms
plusmodelcatalog:catalog/anatomy
plusmodelcatalog:catalog/needletutor
```
