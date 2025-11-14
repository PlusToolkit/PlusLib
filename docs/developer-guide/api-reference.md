# API Reference

This page provides an overview of the main PlusLib APIs and classes.

## Core Classes

### Configuration Management

#### vtkPlusConfig

Global configuration management for Plus applications.

**Key Methods:**
```cpp
static vtkPlusConfig* GetInstance();
PlusStatus ReadConfiguration(vtkXMLDataElement* configRootElement);
PlusStatus SaveConfiguration(const std::string& filename);
```

**Usage:**
```cpp
vtkPlusConfig* config = vtkPlusConfig::GetInstance();
config->ReadConfiguration(configElement);
```

#### PlusXmlUtils

XML reading and writing utilities.

**Key Macros:**
- `XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING()`
- `XML_READ_STRING_ATTRIBUTE_REQUIRED()`
- `XML_READ_SCALAR_ATTRIBUTE_OPTIONAL()`
- `XML_READ_BOOL_ATTRIBUTE_OPTIONAL()`

## Data Collection

### vtkPlusDataCollector

Main class for managing devices and data acquisition.

**Key Methods:**
```cpp
PlusStatus ReadConfiguration(vtkXMLDataElement* config);
PlusStatus Connect();
PlusStatus Start();
PlusStatus Stop();
PlusStatus Disconnect();
PlusStatus GetDevice(vtkPlusDevice*& device, const std::string& deviceId);
```

**Example:**
```cpp
vtkSmartPointer<vtkPlusDataCollector> collector = 
  vtkSmartPointer<vtkPlusDataCollector>::New();
collector->ReadConfiguration(configElement);
collector->Connect();
collector->Start();

// Get device
vtkPlusDevice* device = NULL;
collector->GetDevice(device, "VideoDevice");
```

### vtkPlusDevice

Base class for all devices.

**Key Methods:**
```cpp
virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);
virtual PlusStatus Connect();
virtual PlusStatus Disconnect();
virtual PlusStatus StartRecording();
virtual PlusStatus StopRecording();
PlusStatus GetFirstActiveOutputVideoSource(vtkPlusDataSource*& videoSource);
```

### vtkPlusDataSource

Represents a data stream (video, tracking, field data).

**Key Methods:**
```cpp
vtkPlusBuffer* GetBuffer();
PlusStatus GetLatestTrackedFrame(PlusTrackedFrame* frame);
const char* GetId();
```

### vtkPlusBuffer

Circular buffer for storing tracked frames.

**Key Methods:**
```cpp
ItemStatus GetLatestTrackedFrame(PlusTrackedFrame* trackedFrame);
ItemStatus GetTrackedFrame(double timestamp, PlusTrackedFrame* trackedFrame);
ItemStatus AddItem(PlusTrackedFrame* frame);
int GetNumberOfItems();
```

## Calibration Algorithms

### vtkPlusProbeCalibrationAlgo

Probe (stylus) calibration using phantom-based methods.

**Key Methods:**
```cpp
PlusStatus ReadConfiguration(vtkXMLDataElement* config);
PlusStatus Calibrate();
vtkMatrix4x4* GetImageToProbeTransformMatrix();
```

**Configuration:**
```xml
<ProbeCalibration>
  <Segmentation
    ClipRectangleOrigin="0 0"
    ClipRectangleSize="640 480" />
  <Optimization
    MaximumIterations="500"
    OptimizationMethod="2" />
</ProbeCalibration>
```

### vtkPlusPhantomLandmarkRegistrationAlgo

Landmark-based phantom registration.

**Key Methods:**
```cpp
PlusStatus ReadConfiguration(vtkXMLDataElement* config);
PlusStatus SetPhantomCoordinates(vtkPoints* points);
PlusStatus SetRecordedPoints(vtkPoints* points);
PlusStatus LandmarkRegister();
vtkMatrix4x4* GetPhantomToReferenceTransformMatrix();
```

### vtkPlusPhantomLinearObjectRegistrationAlgo

Linear object (e.g., wire) based phantom registration.

**Key Methods:**
```cpp
PlusStatus ReadConfiguration(vtkXMLDataElement* config);
PlusStatus Register();
```

### vtkPlusTemporalCalibrationAlgo

Temporal calibration between video and tracking streams.

**Key Methods:**
```cpp
PlusStatus ReadConfiguration(vtkXMLDataElement* config);
PlusStatus Update();
double GetVideoOffset();
```

**Configuration:**
```xml
<TemporalCalibration>
  <SamplingResolutionSec>0.001</SamplingResolutionSec>
  <MaximumMovingLagSec>0.08</MaximumMovingLagSec>
  <FixedLagSec>0.05</FixedLagSec>
</TemporalCalibration>
```

### vtkPlusPivotCalibrationAlgo

Pivot calibration for tracked stylus tools.

**Key Methods:**
```cpp
PlusStatus ReadConfiguration(vtkXMLDataElement* config);
PlusStatus AddToolToReferenceTransform(vtkMatrix4x4* matrix);
PlusStatus DoPivotCalibration();
void GetToolTipToToolTransform(vtkMatrix4x4* matrix);
```

### vtkPlusSpacingCalibAlgo

Ultrasound image spacing calibration.

**Key Methods:**
```cpp
PlusStatus ReadConfiguration(vtkXMLDataElement* config);
PlusStatus Calibrate();
PlusStatus GetSpacing(double spacing[3]);
```

## Transform Management

### igsioTransformRepository

Manages coordinate system transforms.

**Key Methods:**
```cpp
PlusStatus ReadConfiguration(vtkXMLDataElement* config);
PlusStatus GetTransform(const igsioTransformName& name, vtkMatrix4x4* matrix);
PlusStatus SetTransform(const igsioTransformName& name, vtkMatrix4x4* matrix);
PlusStatus IsExistingTransform(const igsioTransformName& name);
```

**Usage:**
```cpp
vtkSmartPointer<vtkIGSIOTransformRepository> repo = 
  vtkSmartPointer<vtkIGSIOTransformRepository>::New();
repo->ReadConfiguration(configElement);

vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
repo->GetTransform(igsioTransformName("Probe", "Tracker"), matrix);
```

## Logging

### vtkPlusLogger

Singleton logger for Plus applications.

**Key Methods:**
```cpp
static vtkPlusLogger* Instance();
void SetLogLevel(int level);
int GetLogLevel();
```

**Log Levels:**
```cpp
vtkPlusLogger::LOG_LEVEL_ERROR    // 1
vtkPlusLogger::LOG_LEVEL_WARNING  // 2
vtkPlusLogger::LOG_LEVEL_INFO     // 3
vtkPlusLogger::LOG_LEVEL_DEBUG    // 4
vtkPlusLogger::LOG_LEVEL_TRACE    // 5
```

**Macros:**
```cpp
LOG_ERROR("Error message");
LOG_WARNING("Warning message");
LOG_INFO("Info message");
LOG_DEBUG("Debug message");
LOG_TRACE("Trace message");
```

## Image Processing

### PlusVideoFrame

Utilities for video frame I/O and manipulation.

**Key Methods:**
```cpp
static PlusStatus ReadImageFromFile(const std::string& filename, vtkImageData* image);
static PlusStatus SaveImageToFile(const std::string& filename, vtkImageData* image);
static PlusStatus FlipImage(vtkImageData* image, FlipInfoType flipInfo);
```

## Common Patterns

### Reading Configuration

```cpp
PlusStatus MyClass::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  
  XML_READ_STRING_ATTRIBUTE_REQUIRED(MyParameter, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, OptionalParam, deviceConfig);
  
  return PLUS_SUCCESS;
}
```

### Device Initialization Pattern

```cpp
// Create and configure device
vtkSmartPointer<vtkPlusMyDevice> device = 
  vtkSmartPointer<vtkPlusMyDevice>::New();
device->ReadConfiguration(configElement);

// Connect and start recording
device->Connect();
device->StartRecording();

// Use device...

// Cleanup
device->StopRecording();
device->Disconnect();
```

### Error Handling Pattern

```cpp
PlusStatus MyFunction()
{
  if (CheckCondition() != PLUS_SUCCESS)
  {
    LOG_ERROR("Condition check failed");
    return PLUS_FAIL;
  }
  
  if (!DoSomething())
  {
    LOG_ERROR("DoSomething failed");
    return PLUS_FAIL;
  }
  
  return PLUS_SUCCESS;
}
```

## Full API Documentation

For complete API documentation with all classes and methods, see the [generated Doxygen documentation](http://perk-software.cs.queensu.ca/plus/doc/nightly/dev/).

## Module-Specific Documentation

- **PlusCommon**: Core utilities and base classes
- **PlusDataCollection**: Device interfaces and data acquisition
- **PlusCalibration**: Calibration algorithms
- **PlusImageProcessing**: Image processing utilities
- **PlusOpenIGTLink**: OpenIGTLink communication
- **PlusVolumeReconstruction**: Volume reconstruction
- **PlusRendering**: Visualization components
- **PlusWidgets**: Qt-based UI components
