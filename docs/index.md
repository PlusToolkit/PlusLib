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
- [Quick Start Guide](getting-started/quick-start.md) - Get up and running quickly
- [Build Instructions](getting-started/build-instructions.md) - How to build PlusLib
- [Overview](getting-started/overview.md) - Learn about PlusLib features

### Devices & Hardware
- **[All Devices](devices/index.md)** - Complete list of 58+ supported devices
- [Tracking Devices](devices/index.md#tracking-devices) - NDI, Ascension, Claron, Atracsys
- [Ultrasound Devices](devices/index.md#ultrasound-devices) - BK, Clarius, Philips, Sonix
- [Video Capture](devices/index.md#video-capture) - Epiphan, OpenCV, BlackMagic
- [Cameras](devices/index.md#cameras) - Azure Kinect, RealSense, Infrared

### Algorithms
- [Probe Calibration](algorithms/AlgorithmProbeCalibration.md) - Spatial calibration of tracked ultrasound
- [Phantom Registration](algorithms/AlgorithmPhantomRegistration.md) - Register physical phantoms
- [Temporal Calibration](algorithms/AlgorithmTemporalCalibration.md) - Synchronize tracking and imaging
- [Volume Reconstruction](algorithms/AlgorithmVolumeReconstruction.md) - 3D reconstruction from 2D images

### Applications
- [Probe Calibration Tool](applications/ApplicationProbeCalibration.md) - Calibrate ultrasound probes
- [Volume Reconstructor](applications/ApplicationVolumeReconstructor.md) - Build 3D volumes
- [Tracking Test](applications/ApplicationTrackingTest.md) - Test tracking devices
- [View Sequence File](applications/ApplicationViewSequenceFile.md) - Review captured data

### Configuration
- [Configuration File Format](file-formats/FileApplicationConfiguration.md) - XML configuration reference
- [Common Coordinate Systems](CommonCoordinateSystems.md) - Coordinate system conventions
- [Coordinate System Definitions](CoordinateSystemDefinitions.md) - Detailed definitions
- [Plus Server Commands](PlusServerCommands.md) - Remote control via OpenIGTLink

### Developer Resources
- [API Reference](developer-guide/api-reference.md) - Code documentation
- [Contributing Guide](developer-guide/contributing.md) - How to contribute
- [Creating Devices](developer-guide/creating-devices.md) - Add new device support
- [Coding Conventions](developer-guide/coding-conventions.md) - Code style guide

## Documentation Structure

This documentation is organized into the following sections:

- **Getting Started**: Installation, building, and first steps
- **Devices**: Individual pages for each of the 58+ supported devices
- **Algorithms**: Calibration and processing algorithms
- **Applications**: Command-line tools and utilities (14 applications)
- **File Formats**: Configuration and data file formats
- **Developer Guide**: API reference and contribution guidelines
- **Testing**: How to run tests and view dashboards
- **About**: License and citation information

## Device Categories

PlusLib supports **58+ devices** across multiple categories:

| Category | Count | Examples |
|----------|-------|----------|
| **Tracking Devices** | 13 | NDI Polaris/Aurora/Vega/Certus, Ascension 3DG, MicronTracker, Atracsys |
| **Ultrasound Devices** | 10 | BK ProFocus, Clarius, Philips, Telemed, Interson, Sonix |
| **Video Capture** | 8 | Epiphan, BlackMagic DeckLink, OpenCV, Microsoft Media Foundation |
| **Cameras** | 8 | Azure Kinect, Intel RealSense, Infrared cameras (Seek, TEEV2) |
| **Sensors** | 7 | Phidget Spatial, CHRobotics UM6, LeapMotion, 3dConnexion |
| **Virtual Devices** | 12 | Saved data source, virtual mixer, US simulator, volume reconstructor |

→ **[Browse all devices](devices/index.md)**

## Key Components

### PlusCommon
Core utilities and base classes used throughout the library, including logging, configuration management, and mathematical utilities.

### PlusDataCollection
Device interfaces and data acquisition framework for:
- 13 tracking devices (optical and electromagnetic)
- 10 ultrasound devices
- 8 video capture devices
- 8 camera devices
- 7 sensor devices
- Serial line communication
- Data buffering and synchronization

### PlusCalibration
Advanced calibration algorithms:
- Probe calibration (freehand, phantom-based)
- Phantom landmark registration (FCAL, N-wire phantoms)
- Temporal calibration (line and point-based)
- Pivot calibration
- Spatial calibration

### PlusImageProcessing
Image processing algorithms for ultrasound-guided interventions:
- RF processing and scan conversion
- US image enhancement
- Bone surface detection
- Transducer model fitting

### PlusOpenIGTLink
OpenIGTLink communication support for real-time data streaming to:
- 3D Slicer
- MITK
- CustusX
- Other IGT applications

### PlusVolumeReconstruction
High-performance volume reconstruction from tracked ultrasound frames:
- Paste slice (voxel-based)
- Pixel nearest neighbor
- Hole filling algorithms

## Support & Community

- **GitHub**: [PlusToolkit/PlusLib](https://github.com/PlusToolkit/PlusLib)
- **Issues**: [Report bugs or request features](https://github.com/PlusToolkit/PlusLib/issues)
- **Discussions**: [Join our community](https://github.com/PlusToolkit/PlusLib/discussions)
- **Website**: [PlusToolkit.org](http://www.plustoolkit.org)
- **Test Dashboard**: [View test results](testing/dashboards.md)

## License

PlusLib is licensed under the BSD-3-Clause License. See [License](about/license.md) for details.

## Citation

If you use PlusLib in your research, please cite:

Andras Lasso, Tamas Heffter, Adam Rankin, Csaba Pinter, Tamas Ungi, and Gabor Fichtinger, "PLUS: Open-source toolkit for ultrasound-guided intervention systems", IEEE Trans Biomed Eng. 2014 Oct;61(10):2527-37. doi: 10.1109/TBME.2014.2322864

→ See [Citation](about/citation.md) for more details

---

!!! tip "New to PlusLib?"
    Start with the [Quick Start Guide](getting-started/quick-start.md) to get up and running in minutes!

!!! info "Looking for a specific device?"
    Check out the [Devices Index](devices/index.md) for a complete categorized list of all 58+ supported devices.
