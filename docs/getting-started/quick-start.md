# Quick Start

This guide will help you get started with using PlusLib in your application.

## Basic Usage Example

Here's a minimal example showing how to use PlusLib to read a configuration file and access a device:

```cpp
#include "PlusConfigure.h"
#include "vtkPlusDataCollector.h"
#include "vtkPlusDevice.h"
#include "vtkXMLUtilities.h"

int main(int argc, char* argv[])
{
  // Read configuration file
  vtkSmartPointer<vtkXMLDataElement> configRootElement = 
    vtkSmartPointer<vtkXMLDataElement>::Take(
      vtkXMLUtilities::ReadElementFromFile("PlusConfig.xml"));
  
  if (configRootElement == NULL)
  {
    LOG_ERROR("Unable to read configuration file");
    return EXIT_FAILURE;
  }

  // Create data collector
  vtkSmartPointer<vtkPlusDataCollector> dataCollector = 
    vtkSmartPointer<vtkPlusDataCollector>::New();
  
  // Configure from XML
  if (dataCollector->ReadConfiguration(configRootElement) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to read configuration");
    return EXIT_FAILURE;
  }

  // Connect to devices
  if (dataCollector->Connect() != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to connect to devices");
    return EXIT_FAILURE;
  }

  // Start data acquisition
  if (dataCollector->Start() != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to start data acquisition");
    return EXIT_FAILURE;
  }

  // Your application code here...
  
  // Stop data acquisition
  dataCollector->Stop();
  dataCollector->Disconnect();

  return EXIT_SUCCESS;
}
```

## Configuration File Example

Create a `PlusConfig.xml` file:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<PlusConfiguration version="2.8">

  <DataCollection StartupDelay="1.0">
    <DeviceSet
      Name="PlusServer: Saved dataset"
      Description="Replay a recorded sequence file" />
      
    <Device
      Id="VideoDevice"
      Type="SavedDataSource"
      SequenceFile="RecordedVideo.mha" >
      <DataSources>
        <DataSource Type="Video" Id="Video" />
      </DataSources>
    </Device>

  </DataCollection>

  <CoordinateDefinitions>
    <Transform From="Image" To="Reference"
      Matrix="1 0 0 0
              0 1 0 0
              0 0 1 0
              0 0 0 1" />
  </CoordinateDefinitions>

</PlusConfiguration>
```

## Reading Data from a Device

```cpp
// Get a specific device
vtkPlusDevice* device = NULL;
if (dataCollector->GetDevice(device, "VideoDevice") != PLUS_SUCCESS)
{
  LOG_ERROR("Unable to get device");
  return EXIT_FAILURE;
}

// Get a data source from the device
vtkPlusDataSource* videoSource = NULL;
if (device->GetFirstActiveOutputVideoSource(videoSource) != PLUS_SUCCESS)
{
  LOG_ERROR("Unable to get video source");
  return EXIT_FAILURE;
}

// Get the buffer
vtkPlusBuffer* buffer = videoSource->GetBuffer();

// Get latest frame
PlusTrackedFrame frame;
if (buffer->GetLatestTrackedFrame(&frame) == ITEM_OK)
{
  // Access image data
  vtkImageData* imageData = frame.GetImageData()->GetImage();
  
  // Access transform
  vtkMatrix4x4* matrix = vtkMatrix4x4::New();
  if (frame.GetCustomFrameTransform("ProbeToTracker", matrix))
  {
    // Use the transform
  }
  matrix->Delete();
}
```

## Using Calibration Algorithms

### Probe Calibration Example

```cpp
#include "vtkPlusProbeCalibrationAlgo.h"

// Create calibration algorithm
vtkSmartPointer<vtkPlusProbeCalibrationAlgo> calibration = 
  vtkSmartPointer<vtkPlusProbeCalibrationAlgo>::New();

// Read configuration
if (calibration->ReadConfiguration(configElement) != PLUS_SUCCESS)
{
  LOG_ERROR("Failed to read calibration configuration");
  return EXIT_FAILURE;
}

// Set input data (tracked ultrasound frames)
calibration->SetInputs(trackedFrameList, transformRepository);

// Run calibration
if (calibration->Calibrate() != PLUS_SUCCESS)
{
  LOG_ERROR("Calibration failed");
  return EXIT_FAILURE;
}

// Get calibration result
vtkMatrix4x4* imageToProbeMatrix = calibration->GetImageToProbeTransformMatrix();
```

## Using Transform Repository

```cpp
#include "igsioTransformRepository.h"

// Create transform repository
vtkSmartPointer<vtkIGSIOTransformRepository> transformRepo = 
  vtkSmartPointer<vtkIGSIOTransformRepository>::New();

// Read coordinate definitions from config
if (transformRepo->ReadConfiguration(configElement) != PLUS_SUCCESS)
{
  LOG_ERROR("Failed to read coordinate definitions");
  return EXIT_FAILURE;
}

// Get a transform
vtkSmartPointer<vtkMatrix4x4> probeToTrackerMatrix = 
  vtkSmartPointer<vtkMatrix4x4>::New();
  
if (transformRepo->GetTransform(
    igsioTransformName("Probe", "Tracker"), 
    probeToTrackerMatrix) != PLUS_SUCCESS)
{
  LOG_ERROR("Failed to get transform");
  return EXIT_FAILURE;
}
```

## Logging

PlusLib provides comprehensive logging:

```cpp
// Set log level
vtkPlusLogger::Instance()->SetLogLevel(vtkPlusLogger::LOG_LEVEL_DEBUG);

// Log messages
LOG_ERROR("This is an error message");
LOG_WARNING("This is a warning");
LOG_INFO("This is an info message");
LOG_DEBUG("This is a debug message");
LOG_TRACE("This is a trace message");
```

## CMake Integration

To use PlusLib in your CMake project:

```cmake
cmake_minimum_required(VERSION 3.15)
project(MyPlusApp)

# Find PlusLib
find_package(PlusLib REQUIRED PATHS "path/to/PlusLib-bin")

# Add executable
add_executable(MyPlusApp main.cpp)

# Link against PlusLib
target_link_libraries(MyPlusApp
  vtkPlusCommon
  vtkPlusDataCollection
  vtkPlusCalibration
)
```

## Next Steps

- Read the [User Guide](../user-guide/configuration.md) for detailed configuration options
- Explore [Developer Guide](../developer-guide/api-reference.md) for API details
- Check out example applications in PlusApp repository
- See [Testing](../testing/running-tests.md) for information on running tests
