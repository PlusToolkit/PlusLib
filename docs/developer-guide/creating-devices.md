# Creating New Devices

This guide explains how to add support for a new hardware device in PlusLib.

## Overview

Adding a new device involves:

1. Creating a new device class derived from `vtkPlusDevice`
2. Implementing required virtual methods
3. Adding device-specific configuration
4. Creating tests
5. Updating CMake files

## Step 1: Create Device Class

### File Structure

Create files in the appropriate module (usually `PlusDataCollection`):

```
src/PlusDataCollection/
  MyVendorDevices/
    vtkPlusMyDevice.h
    vtkPlusMyDevice.cxx
    CMakeLists.txt
```

### Header File Template

```cpp
#ifndef __vtkPlusMyDevice_h
#define __vtkPlusMyDevice_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

/*!
  \class vtkPlusMyDevice
  \brief Interface to MyVendor tracking/imaging device
  
  Detailed description of the device and its capabilities.
  
  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusMyDevice : public vtkPlusDevice
{
public:
  static vtkPlusMyDevice* New();
  vtkTypeMacro(vtkPlusMyDevice, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Read configuration from XML data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config) VTK_OVERRIDE;

  /*! Write configuration to XML data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config) VTK_OVERRIDE;

  /*! Connect to device */
  virtual PlusStatus InternalConnect() VTK_OVERRIDE;

  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect() VTK_OVERRIDE;

  /*! Probe to see if the device is connected */
  virtual PlusStatus Probe() VTK_OVERRIDE;

  /*! Get device-specific information string */
  virtual std::string GetSdkVersion() VTK_OVERRIDE;

  /*! Device-specific method */
  vtkSetMacro(DeviceParameter, int);
  vtkGetMacro(DeviceParameter, int);

protected:
  vtkPlusMyDevice();
  virtual ~vtkPlusMyDevice();

  /*! Start the tracking/imaging system */
  virtual PlusStatus InternalStartRecording() VTK_OVERRIDE;

  /*! Stop the tracking/imaging system */
  virtual PlusStatus InternalStopRecording() VTK_OVERRIDE;

  /*! The main data acquisition function */
  virtual PlusStatus InternalUpdate() VTK_OVERRIDE;

private:
  vtkPlusMyDevice(const vtkPlusMyDevice&); // Not implemented
  void operator=(const vtkPlusMyDevice&);   // Not implemented

  /*! Device-specific parameter */
  int m_DeviceParameter;
};

#endif
```

### Implementation Template

```cpp
#include "PlusConfigure.h"
#include "vtkPlusMyDevice.h"
#include "vtkPlusDataSource.h"

vtkStandardNewMacro(vtkPlusMyDevice);

//----------------------------------------------------------------------------
vtkPlusMyDevice::vtkPlusMyDevice()
  : m_DeviceParameter(0)
{
  this->RequireImageOrientationInConfiguration = true;
  this->StartThreadForInternalUpdates = true; // or false for passive devices
}

//----------------------------------------------------------------------------
vtkPlusMyDevice::~vtkPlusMyDevice()
{
}

//----------------------------------------------------------------------------
void vtkPlusMyDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "DeviceParameter: " << this->m_DeviceParameter << std::endl;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusMyDevice::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  // Read required parameters
  XML_READ_SCALAR_ATTRIBUTE_REQUIRED(int, DeviceParameter, deviceConfig);

  // Read optional parameters
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(EnableSomeFeature, deviceConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusMyDevice::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  deviceConfig->SetIntAttribute("DeviceParameter", this->m_DeviceParameter);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusMyDevice::Probe()
{
  // Try to detect if device is present
  // Return PLUS_SUCCESS if device is found
  // Return PLUS_FAIL if device is not found
  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusMyDevice::InternalConnect()
{
  // Initialize device SDK
  // Open connection to hardware
  
  if (/* connection failed */)
  {
    LOG_ERROR("Failed to connect to MyDevice");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusMyDevice::InternalDisconnect()
{
  // Clean up device SDK
  // Close connection to hardware
  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusMyDevice::InternalStartRecording()
{
  // Start data acquisition
  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusMyDevice::InternalStopRecording()
{
  // Stop data acquisition
  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusMyDevice::InternalUpdate()
{
  // This is called repeatedly to acquire new data
  
  // For imaging devices:
  vtkPlusDataSource* videoSource = NULL;
  if (this->GetFirstActiveOutputVideoSource(videoSource) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to get video source");
    return PLUS_FAIL;
  }

  // Get image from device
  // ...
  
  // Add frame to buffer
  this->FrameNumber++;
  if (videoSource->AddItem(imageData, 
                           this->FrameNumber, 
                           vtkPlusDevice::GetSystemTime()) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to add frame to buffer");
    return PLUS_FAIL;
  }
  
  // For tracking devices:
  vtkPlusDataSource* toolSource = NULL;
  if (this->GetToolByPortName("MyTool", toolSource) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to get tool source");
    return PLUS_FAIL;
  }

  // Get transform from device
  vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
  // ... populate matrix ...
  
  // Add transform to buffer
  if (toolSource->SetMatrix(matrix, vtkPlusDevice::GetSystemTime()) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to set transform");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
std::string vtkPlusMyDevice::GetSdkVersion()
{
  return "MyDevice SDK v1.0";
}
```

## Step 2: Update CMakeLists.txt

Add device to `src/PlusDataCollection/CMakeLists.txt`:

```cmake
OPTION(PLUS_USE_MYDEVICE "Provide support for MyDevice" OFF)

IF(PLUS_USE_MYDEVICE)
  SET(MyDevice_DIR CACHE PATH "Path to MyDevice SDK")
  
  FIND_PACKAGE(MyDevice REQUIRED)
  
  SET(PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/MyVendorDevices
    ${MyDevice_INCLUDE_DIRS}
    CACHE INTERNAL "" FORCE)
  
  SET(PlusDataCollection_LIBS ${PlusDataCollection_LIBS}
    ${MyDevice_LIBRARIES}
    CACHE INTERNAL "" FORCE)
  
  SET(PlusDataCollection_SRCS ${PlusDataCollection_SRCS}
    MyVendorDevices/vtkPlusMyDevice.cxx
    CACHE INTERNAL "" FORCE)
ENDIF()
```

## Step 3: Register Device Factory

In `src/PlusDataCollection/vtkPlusDeviceFactory.cxx`, add:

```cpp
#ifdef PLUS_USE_MYDEVICE
  #include "vtkPlusMyDevice.h"
#endif

// In PointerToDevice():
#ifdef PLUS_USE_MYDEVICE
  if (STRCASECMP(deviceType, "MyDevice") == 0)
  {
    device = vtkPlusMyDevice::New();
    return device;
  }
#endif
```

## Step 4: Configuration XML

Create example configuration:

```xml
<Device
  Id="MyTracker"
  Type="MyDevice"
  DeviceParameter="42" >
  
  <DataSources>
    <DataSource Type="Tool" Id="Tool_0" PortName="0" />
    <DataSource Type="Tool" Id="Tool_1" PortName="1" />
  </DataSources>
  
  <OutputChannels>
    <OutputChannel Id="MyTrackerStream">
      <DataSource Id="Tool_0" />
      <DataSource Id="Tool_1" />
    </OutputChannel>
  </OutputChannels>
</Device>
```

## Step 5: Create Tests

Create `Testing/vtkPlusMyDeviceTest.cxx`:

```cpp
#include "PlusConfigure.h"
#include "vtkPlusMyDevice.h"
#include "vtkXMLUtilities.h"

int main(int argc, char* argv[])
{
  // Read config file
  std::string configFile = "MyDeviceTestConfig.xml";
  vtkSmartPointer<vtkXMLDataElement> configRoot = 
    vtkSmartPointer<vtkXMLDataElement>::Take(
      vtkXMLUtilities::ReadElementFromFile(configFile.c_str()));
  
  if (configRoot == NULL)
  {
    LOG_ERROR("Unable to read configuration file");
    return EXIT_FAILURE;
  }

  // Create device
  vtkSmartPointer<vtkPlusMyDevice> device = 
    vtkSmartPointer<vtkPlusMyDevice>::New();
  
  // Configure
  if (device->ReadConfiguration(configRoot) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to read configuration");
    return EXIT_FAILURE;
  }

  // Connect
  if (device->Connect() != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to connect");
    return EXIT_FAILURE;
  }

  // Start recording
  if (device->StartRecording() != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to start recording");
    return EXIT_FAILURE;
  }

  // Acquire some frames
  for (int i = 0; i < 100; i++)
  {
    device->Update();
  }

  // Stop
  device->StopRecording();
  device->Disconnect();

  LOG_INFO("Test completed successfully");
  return EXIT_SUCCESS;
}
```

Add test to `Testing/CMakeLists.txt`:

```cmake
IF(PLUS_USE_MYDEVICE)
  ADD_EXECUTABLE(vtkPlusMyDeviceTest vtkPlusMyDeviceTest.cxx)
  TARGET_LINK_LIBRARIES(vtkPlusMyDeviceTest vtkPlusDataCollection)
  ADD_TEST(NAME vtkPlusMyDeviceTest
    COMMAND $<TARGET_FILE:vtkPlusMyDeviceTest>
    --config-file=${TestDataDir}/MyDeviceTestConfig.xml)
ENDIF()
```

## Best Practices

1. **Thread Safety**: Use mutexes for shared data
2. **Error Handling**: Always return PLUS_FAIL and log errors
3. **Resource Cleanup**: Properly cleanup in destructor and disconnect
4. **Configuration**: Validate all configuration parameters
5. **Testing**: Test with and without hardware present
6. **Documentation**: Document all configuration parameters

## See Also

- [Coding Conventions](coding-conventions.md)
- [API Reference](api-reference.md)
- Example devices in `src/PlusDataCollection/`
