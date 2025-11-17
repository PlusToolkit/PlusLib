# Overview

PlusLib is a comprehensive software library designed for navigated image-guided interventions, providing essential tools for:

- Data acquisition from various tracking and imaging devices
- Real-time data pre-processing
- Calibration algorithms
- Transform management
- Volume reconstruction

## Architecture

PlusLib is organized into several modules:

### PlusCommon

Core utilities and base classes:

- Logging framework (`vtkPlusLogger`)
- Configuration management (`vtkPlusConfig`)
- XML utilities (`PlusXmlUtils`)
- Mathematical utilities (`PlusMath`)
- Sequence I/O operations

### PlusDataCollection

Device interfaces and data acquisition:

- Base classes for devices and data sources
- Support for 50+ tracking and imaging devices
- Data buffering and synchronization
- Serial line communication
- Video frame management

### PlusCalibration

Calibration algorithms:

- **Probe Calibration**: `vtkPlusProbeCalibrationAlgo`
- **Phantom Registration**: `vtkPlusPhantomLandmarkRegistrationAlgo`, `vtkPlusPhantomLinearObjectRegistrationAlgo`
- **Temporal Calibration**: `vtkPlusTemporalCalibrationAlgo`
- **Pivot Calibration**: `vtkPlusPivotCalibrationAlgo`
- **Spacing Calibration**: `vtkPlusSpacingCalibAlgo`

### PlusImageProcessing

Image processing algorithms:

- Bone surface detection
- Transducer modeling
- Scan conversion
- Drawing utilities

### PlusOpenIGTLink

OpenIGTLink communication:

- Real-time data streaming
- Command execution
- Network protocol implementation

### PlusVolumeReconstruction

Volume reconstruction from tracked ultrasound:

- Paste slice algorithm
- Hole filling
- Kernel-based reconstruction

### PlusRendering

Visualization components for real-time display.

### PlusWidgets

Qt-based UI components for applications.

## Supported Platforms

- **Windows 10/11** (x64) - Primary platform
- **Linux** (Ubuntu 20.04+) - Fully supported
- **macOS**: - Limited support

## Dependencies

PlusLib builds upon well-established open-source libraries:

- **VTK** (Visualization Toolkit) - 3D graphics and visualization
- **ITK** (Insight Toolkit) - Image processing
- **OpenIGTLink**: - Real-time communication protocol
- **Qt5/Qt6** - GUI framework (optional)
- **IGSTK**: - Image-guided surgery toolkit
## Design Principles

1. **Modularity**: Each component can be used independently
2. **Extensibility**: Easy to add new devices and algorithms
3. **Real-time Performance**: Optimized for low-latency operation
4. **Configuration-driven**: XML-based configuration for flexibility
5. **Cross-platform**: Platform-independent core with OS-specific device drivers

## Use Cases

PlusLib is used in:

- Ultrasound-guided interventions
- Surgical navigation systems
- Medical imaging research
- Educational training systems
- Real-time image processing applications
