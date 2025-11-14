# Coding Conventions

This document describes the coding standards used in PlusLib development.

## General Guidelines

- **Language**: C++ following VTK-style conventions
- **Formatting**: Use the Plus AStyle configuration
- **Headers**: Always include `PlusConfigure.h` first in `.cxx` files
- **Line Endings**: Use Windows-style (CR/LF)
- **Indentation**: Use 2 spaces (do not use tabs)

## Naming Conventions

### Classes

Classes derived from VTK classes use the `vtkPlus` prefix:

```cpp
class vtkPlusMyDevice : public vtkPlusDevice
{
  // ...
};
```

### Files

File names must match the class name:
- Header: `vtkPlusMyDevice.h`
- Implementation: `vtkPlusMyDevice.cxx`
- Test: `vtkPlusMyDeviceTest.cxx`

### Variables

- **Member variables**: Use `m_` prefix (e.g., `m_DeviceName`)
  - Exception: VTK-derived classes follow VTK conventions
  - Exception: ITK-derived classes follow ITK conventions
- **Local variables**: Use camelCase (e.g., `deviceName`)
- **Constants**: Use UPPER_CASE (e.g., `MAX_BUFFER_SIZE`)

### Methods

Use VTK naming for VTK-derived classes, Qt naming for Qt-derived classes:

```cpp
// VTK style
void SetDeviceName(const char* name);
const char* GetDeviceName();

// Qt style  
void setDeviceName(const QString& name);
QString deviceName() const;
```

## Code Formatting

### Curly Brackets

Always use curly brackets on new lines, even for single statements:

```cpp
// Correct
if (condition)
{
  DoSomething();
}

// Incorrect
if (condition)
  DoSomething();

if (condition) {
  DoSomething();
}
```

Add a blank line after opening bracket:

```cpp
void MyFunction()
{
  
  int x = 5;
  // ...
}
```

### Spacing

```cpp
// Correct spacing
if (x == 5)
{
  y = x + 3;
}

for (int i = 0; i < 10; i++)
{
  // ...
}

// Function calls
MyFunction(param1, param2);
```

## Error Handling

### Return Codes

Use `PlusStatus` (PLUS_SUCCESS/PLUS_FAIL) for return codes:

```cpp
PlusStatus vtkPlusMyDevice::Connect()
{
  
  if (m_DeviceName.empty())
  {
    LOG_ERROR("Device name is not set");
    return PLUS_FAIL;
  }
  
  if (!InitializeDevice())
  {
    LOG_ERROR("Failed to initialize device");
    return PLUS_FAIL;
  }
  
  return PLUS_SUCCESS;
}
```

### No Exceptions

**PlusLib shall not throw exceptions.** All errors must be communicated via return codes.

Using exceptions internally is acceptable but not recommended. If used:
- Must be very well documented
- Limited to smallest possible scope
- Must not expose exception-unsafe code to exceptions

### Logging Errors

If any method returns `PLUS_FAIL`, it must log the cause using `LOG_ERROR()`:

```cpp
if (DoSomething() != PLUS_SUCCESS)
{
  LOG_ERROR("Failed to do something: " << detailsAboutError);
  return PLUS_FAIL;
}
```

## Logging

Never use `printf` or `std::cout` for logging. Always use Plus logging macros:

```cpp
LOG_ERROR("Critical error occurred");      // Almost always displayed
LOG_WARNING("Potential problem detected"); // Almost always displayed
LOG_INFO("Important information");         // User-facing messages
LOG_DEBUG("Debugging information");        // Developer-only, usually not displayed
LOG_TRACE("Detailed trace information");   // Very detailed debugging
```

## Configuration Reading

Use XML macros for reading configuration:

```cpp
PlusStatus ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  
  // Required parameter
  XML_READ_STRING_ATTRIBUTE_REQUIRED(DeviceName, deviceConfig);
  
  // Optional parameters with defaults
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, Framerate, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(EnableLogging, deviceConfig);
  
  return PLUS_SUCCESS;
}
```

## Memory Management

### VTK Smart Pointers

Use VTK smart pointers for VTK objects:

```cpp
vtkSmartPointer<vtkImageData> image = vtkSmartPointer<vtkImageData>::New();
vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
```

### Reference Counting

Follow VTK reference counting conventions:
- Objects created with `New()` are owned by caller
- Methods returning objects typically don't increase reference count
- Use `Register()` and `UnRegister()` when needed

### String Handling

Avoid CRT string functions (`sprintf`, `strcpy`, `sscanf`). Use STL strings instead:

```cpp
// Correct
std::string deviceName = "MyDevice";
std::stringstream ss;
ss << "Device ID: " << deviceId;

// Incorrect - don't use
char buffer[256];
sprintf(buffer, "Device ID: %d", deviceId);
```

## Image Handling

### Image Coordinate Systems

- **B-mode images**: Store in MF coordinate system
- **RF images**: Store in FM coordinate system
- If different, reflect in variable name: `image_UF = FlipH(image)`

### Image I/O

Don't use VTK IO classes directly. Use Plus functions instead:

```cpp
// Correct
PlusVideoFrame::ReadImageFromFile("image.mha", image);
PlusVideoFrame::SaveImageToFile("output.mha", image);

// Incorrect - don't use VTK IO directly
vtkMetaImageReader* reader = vtkMetaImageReader::New();
reader->SetFileName("image.mha");
reader->Update();
```

VTK and ITK use the same memory layout, so no conversion is needed.

## Documentation

Use Doxygen-style comments for all public methods:

```cpp
/*!
 * \brief Initialize the device with specified parameters
 * 
 * This method establishes connection to the hardware device
 * and configures it according to the settings in the config file.
 * 
 * \param configElement XML element containing device configuration
 * \return PLUS_SUCCESS on success, PLUS_FAIL otherwise
 */
PlusStatus Initialize(vtkXMLDataElement* configElement);
```

Document:
- Purpose of the method
- Parameters and their meaning
- Return values
- Side effects
- Thread safety (if applicable)

## File Organization

Standard module structure:

```
src/
  PlusMyModule/
    CMakeLists.txt
    vtkPlusMyClass.h
    vtkPlusMyClass.cxx
    vtkPlusMyClass.txx (template implementations)
    Testing/
      CMakeLists.txt
      vtkPlusMyClassTest.cxx
      Data/ (test data)
```

## Testing

### Test Files

- Place tests in `Testing/` subdirectory
- Name test files: `<ClassName>Test.cxx`
- Use CTest framework

### Test Structure

```cpp
#include "PlusConfigure.h"
#include "vtkPlusMyClass.h"

int main(int argc, char* argv[])
{
  // Setup
  vtkSmartPointer<vtkPlusMyClass> myObject = 
    vtkSmartPointer<vtkPlusMyClass>::New();
  
  // Test normal operation
  if (myObject->DoSomething() != PLUS_SUCCESS)
  {
    LOG_ERROR("DoSomething failed");
    return EXIT_FAILURE;
  }
  
  // Test error conditions
  if (myObject->DoInvalidOperation() == PLUS_SUCCESS)
  {
    LOG_ERROR("Expected failure but succeeded");
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}
```

## Examples

See these files as examples of proper style:

- `src/PlusCalibration/vtkSpacingCalibAlgo/vtkPlusSpacingCalibAlgo.h`
- `src/PlusCalibration/vtkSpacingCalibAlgo/vtkPlusSpacingCalibAlgo.cxx`

## Configuration File Changes

When changing configuration file structure:

1. Add entry to Configuration file structure change history
2. Refresh documentation if needed
3. For major changes:
   - Increment version number in `WriteConfiguration`
   - Update existing config file examples
