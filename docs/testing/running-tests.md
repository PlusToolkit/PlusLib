# Running Tests

Guide to building and running PlusLib tests.

## Overview

PlusLib uses CTest (part of CMake) for automated testing. Tests verify:
- Core functionality
- Device interfaces
- Calibration algorithms
- Data I/O
- Image processing
- Network communication

## Building Tests

### Enable Testing

Configure with testing enabled:

```bash
cmake -DBUILD_TESTING=ON -DPLUS_USE_testsuite=ON ..
```

**CMake Options:**
- `BUILD_TESTING`: Enable CTest framework (default: ON)
- `PLUS_USE_testsuite`: Download test data (default: OFF)

### Test Data

Test data is automatically downloaded from:
```
<http://perk-software.cs.queensu.ca/plus/testdata/>
```

Stored locally in: `PlusLibData/TestImages/`

## Running Tests

### Run All Tests

**Command line:**
```bash
ctest
```

**Or use helper scripts:**

**Windows:**
```powershell
cd PlusLib-bin
.\BuildAndTest.bat
```

**Linux/macOS:**
```bash
cd PlusLib-bin
./BuildAndTest.sh
```

### Run Specific Tests

**By name:**
```bash
ctest -R vtkProbeCalibration
```

**By label:**
```bash
ctest -L Calibration
```

**Exclude tests:**
```bash
ctest -E SlowTest
```

### Verbose Output

Show detailed test output:

```bash
ctest -V
```

Or show output only for failed tests:

```bash
ctest --output-on-failure
```

### Parallel Execution

Run tests in parallel:

```bash
ctest -j8  # Run 8 tests in parallel
```

## Test Organization

### Test Categories

Tests are organized by module:

**PlusCommon:**
- Math utilities
- Configuration reading
- Logging

**PlusDataCollection:**
- Device interfaces
- Buffer management
- Data sources

**PlusCalibration:**
- Probe calibration
- Temporal calibration
- Phantom registration
- Pivot calibration

**PlusImageProcessing:**
- Scan conversion
- Image transforms
- Bone surface detection

**PlusVolumeReconstruction:**
- Volume reconstruction algorithms
- Hole filling

### Test Labels

Common labels:
- `Calibration`: Calibration algorithms
- `Device`: Device interfaces
- `IO`: Input/output operations
- `Rendering`: Visualization
- `Slow`: Long-running tests
- `RequiresDevice`: Needs hardware

List all labels:
```bash
ctest --print-labels
```

Run by label:
```bash
ctest -L Calibration
```

## Test Types

### Unit Tests

Test individual classes/functions:

```cpp
int vtkPlusMathTest(int argc, char* argv[])
{
  // Test cross product
  double v1[3] = {1, 0, 0};
  double v2[3] = {0, 1, 0};
  double result[3];
  
  PlusMath::Cross(v1, v2, result);
  
  if (result[0] != 0 || result[1] != 0 || result[2] != 1)
  {
    LOG_ERROR("Cross product failed");
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}
```

### Integration Tests

Test component interactions:

```cpp
int vtkDataCollectorTest(int argc, char* argv[])
{
  // Read config
  vtkSmartPointer<vtkXMLDataElement> config = 
    ReadConfig("TestConfig.xml");
  
  // Create data collector
  vtkSmartPointer<vtkPlusDataCollector> collector = 
    vtkSmartPointer<vtkPlusDataCollector>::New();
  
  // Test configuration
  if (collector->ReadConfiguration(config) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to read configuration");
    return EXIT_FAILURE;
  }
  
  // Test connection
  if (collector->Connect() != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to connect");
    return EXIT_FAILURE;
  }
  
  collector->Disconnect();
  return EXIT_SUCCESS;
}
```

### Comparison Tests

Compare output to reference data:

```cpp
int vtkCalibrationTest(int argc, char* argv[])
{
  // Run calibration
  RunProbeCalibration(inputFile, outputTransform);
  
  // Load reference
  vtkSmartPointer<vtkMatrix4x4> reference = LoadReferenceTransform();
  
  // Compare
  double error = ComputeTransformDifference(outputTransform, reference);
  
  if (error > 0.5) // 0.5mm tolerance
  {
    LOG_ERROR("Calibration error too large: " << error);
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}
```

## Writing Tests

### Test Template

```cpp
#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"

int MyTest(int argc, char* argv[])
{
  // Parse arguments
  std::string inputFile;
  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;
  
  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);
  args.AddArgument("--input-file", 
    vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputFile, 
    "Input file name");
  args.AddArgument("--verbose", 
    vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, 
    "Verbose level");
  
  if (!args.Parse())
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << args.GetHelp() << std::endl;
    return EXIT_FAILURE;
  }
  
  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);
  
  // Test code here
  // ...
  
  return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
  return MyTest(argc, argv);
}
```

### Adding Tests to CMake

In `Testing/CMakeLists.txt`:

```cmake
# Add test executable
ADD_EXECUTABLE(MyTest MyTest.cxx)
TARGET_LINK_LIBRARIES(MyTest vtkPlusCommon)

# Add test
ADD_TEST(NAME MyTest
  COMMAND $<TARGET_FILE:MyTest>
    --input-file=${TestDataDir}/TestInput.mha
    --verbose=3
)

# Set test properties
SET_TESTS_PROPERTIES(MyTest PROPERTIES
  LABELS "MyModule"
  TIMEOUT 30
)
```

### Test Data

Reference test data in tests:

```cmake
ADD_TEST(NAME MyTest
  COMMAND $<TARGET_FILE:MyTest>
    --config-file=${TestDataDir}/TestConfig.xml
    --baseline=${TestDataDir}/BaselineImage.mha
    --input-seq=${TestDataDir}/InputSequence.mha
)
```

## Test Results

### Success Criteria

Tests pass if:
- Return `EXIT_SUCCESS` (0)
- No timeout
- No crash/segfault
- Output matches expected (for comparison tests)

Tests fail if:
- Return `EXIT_FAILURE` (1)
- Timeout exceeded
- Crash or exception
- Output differs from expected

### Understanding Failures

**Read test output:**
```bash
ctest -V -R FailingTest
```

**Check test log:**
```
Testing/Temporary/LastTest.log
```

**Run test directly:**
```bash
cd PlusLib-bin/bin
./MyTest --verbose=5
```

## Debugging Tests

### Debug Single Test

**Visual Studio:**
1. Open PlusLib.sln
2. Set test project as startup project
3. Set command line arguments in project properties
4. Press F5 to debug

**GDB (Linux):**
```bash
gdb --args ./MyTest --input-file=test.mha --verbose=5
(gdb) run
(gdb) bt  # backtrace on crash
```

**LLDB (macOS):**
```bash
lldb ./MyTest -- --input-file=test.mha
(lldb) run
(lldb) bt
```

### Memory Debugging

**Valgrind (Linux):**
```bash
valgrind --leak-check=full ./MyTest
```

**DrMemory (Windows):**
```bash
drmemory -- MyTest.exe
```

### Enable More Logging

```bash
./MyTest --verbose=5  # Maximum verbosity
```

Or in code:
```cpp
vtkPlusLogger::Instance()->SetLogLevel(vtkPlusLogger::LOG_LEVEL_TRACE);
```

## Continuous Testing

### Local Continuous Testing

Watch for changes and auto-test:

```bash
ctest -D Continuous
```

### Pre-commit Testing

Before committing code:

```bash
# Quick test
ctest -j8

# Full test including slow tests
ctest -j8 -L ""

# Memory check
ctest -D ExperimentalMemCheck
```

### Dashboard Submission

Submit to CDash:

```bash
# Experimental build
ctest -D Experimental

# Or use scripts
./BuildAndTest.sh
```

See [Test Dashboards](dashboards.md) for details.

## Performance Testing

### Timing Tests

Measure execution time:

```cpp
#include "vtkPlusAccurateTimer.h"

double startTime = vtkPlusAccurateTimer::GetSystemTime();

// Code to measure
PerformOperation();

double elapsed = vtkPlusAccurateTimer::GetSystemTime() - startTime;
LOG_INFO("Operation took " << elapsed << " seconds");

// Fail if too slow
if (elapsed > maxAllowedTime)
{
  LOG_ERROR("Operation too slow");
  return EXIT_FAILURE;
}
```

### Memory Usage

Monitor memory:

```cpp
#include "vtkPlusAccurateTimer.h"

size_t initialMemory = GetCurrentMemoryUsage();

// Code to measure
AllocateResources();

size_t finalMemory = GetCurrentMemoryUsage();
size_t memoryUsed = finalMemory - initialMemory;

LOG_INFO("Memory used: " << memoryUsed / 1024.0 / 1024.0 << " MB");
```

## Best Practices

### Test Design

1. **Independent**: Tests should not depend on each other
2. **Repeatable**: Same input → same output
3. **Fast**: Keep tests quick (<5s preferred)
4. **Focused**: Test one thing per test
5. **Clear**: Obvious what is being tested

### Test Data

1. **Minimal**: Use smallest data that tests functionality
2. **Versioned**: Include test data in version control or reference
3. **Documented**: Explain what test data represents

### Test Maintenance

1. **Update with code**: Keep tests current
2. **Fix immediately**: Don't let tests stay broken
3. **Remove obsolete**: Delete tests for removed features

## Common Test Patterns

### Configuration Test

```cpp
int TestConfiguration()
{
  vtkSmartPointer<vtkPlusConfig> config = 
    vtkSmartPointer<vtkPlusConfig>::New();
  
  if (config->ReadConfiguration(configFile) != PLUS_SUCCESS)
    return EXIT_FAILURE;
    
  // Verify configuration values
  if (config->GetDeviceCount() != expectedCount)
    return EXIT_FAILURE;
    
  return EXIT_SUCCESS;
}
```

### Device Test

```cpp
int TestDevice()
{
  vtkSmartPointer<vtkPlusMyDevice> device = 
    vtkSmartPointer<vtkPlusMyDevice>::New();
  
  if (device->Connect() != PLUS_SUCCESS)
    return EXIT_FAILURE;
    
  if (device->StartRecording() != PLUS_SUCCESS)
    return EXIT_FAILURE;
    
  // Acquire some frames
  for (int i = 0; i < 100; i++)
  {
    device->Update();
  }
  
  device->StopRecording();
  device->Disconnect();
  
  return EXIT_SUCCESS;
}
```

### Algorithm Test

```cpp
int TestAlgorithm()
{
  // Load test data
  TrackedFrameList frames = LoadTestData();
  
  // Run algorithm
  Algorithm algo;
  algo.SetInput(frames);
  algo.Update();
  
  // Check results
  double result = algo.GetResult();
  double expected = 123.456;
  double tolerance = 0.001;
  
  if (fabs(result - expected) > tolerance)
  {
    LOG_ERROR("Result " << result << " != expected " << expected);
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}
```

## See Also

- [Test Dashboards](dashboards.md)
- [Contributing Guide](../developer-guide/contributing.md)
- [Build Instructions](../getting-started/build-instructions.md)
