# Test Dashboards

PlusLib uses CDash for continuous integration and testing visualization.

## Dashboard Overview

CDash provides web-based dashboards showing:
- Build status across platforms
- Test results
- Code coverage
- Memory leak detection
- Performance metrics

## PlusLib Dashboard

**URL**: [http://perkdata.cs.queensu.ca/CDash/index.php?project=PlusLib](http://perkdata.cs.queensu.ca/CDash/index.php?project=PlusLib)

### Build Tracks

**Nightly**
- Automated builds every night
- Full test suite execution
- Code coverage analysis (Linux)
- Memory checking (Windows)

**Continuous**
- Triggered by commits
- Quick feedback on changes
- Essential tests only

**Experimental**
- Developer submissions
- Testing local changes
- Manual submissions

## PlusApp Dashboard

**URL**: [http://perkdata.cs.queensu.ca/CDash/index.php?project=PlusApp](http://perkdata.cs.queensu.ca/CDash/index.php?project=PlusApp)

Shows build and test status for Plus applications.

## Dashboard Information

### Build Results

**Build Status Colors:**
- 🟢 **Green**: Build succeeded, all tests passed
- 🟡 **Yellow**: Build succeeded, some tests failed
- 🔴 **Red**: Build failed

### Viewing Build Details

Click on a build to see:
- Full build log
- Compiler warnings and errors
- Test results
- Coverage data
- Memory check results

### Test Results

**Test Status:**
- **Passed**: Test completed successfully
- **Failed**: Test failed (assertion, wrong output)
- **Not Run**: Test was not executed
- **Timeout**: Test exceeded time limit

### Coverage

Code coverage shows which lines of code are executed by tests:

```
Coverage Summary:
  Lines Tested:   12,456 of 15,234 (81.8%)
  Functions:      1,234 of 1,456 (84.7%)
```

**Color coding:**
- 🟢 Green: >80% coverage (good)
- 🟡 Yellow: 60-80% coverage (acceptable)
- 🔴 Red: <60% coverage (needs improvement)

### Memory Checking

Valgrind (Linux) and DrMemory (Windows) detect:
- Memory leaks
- Invalid memory access
- Use of uninitialized memory
- Double frees

## Submitting to Dashboard

### Automatic Submission

Use the provided scripts:

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

These scripts:
1. Update code from repository
2. Configure build
3. Build all targets
4. Run all tests
5. Submit results to CDash

### Manual Submission

Using CTest directly:

```bash
# Configure
cd PlusLib-bin
cmake ..

# Build
cmake --build . --config Release

# Test and submit
ctest -D Experimental
```

**CTest Dashboard Modes:**
- `Experimental`: One-time submission
- `Nightly`: Scheduled nightly build
- `Continuous`: Triggered by code changes

### Build Name

Dashboard uses build names to identify submissions:

```
Win10-VS2019-x64-Release
Linux-Ubuntu2004-gcc9-Debug
macOS-Catalina-Clang-Release
```

Configure in `CTestConfig.cmake`:

```cmake
set(CTEST_BUILD_NAME 
  "${CMAKE_SYSTEM_NAME}-${COMPILER}-${CMAKE_BUILD_TYPE}")
```

## Monitoring Dashboard

### Email Notifications

Register on CDash to receive notifications:

1. Create CDash account
2. Subscribe to PlusLib project
3. Set notification preferences:
   - Build errors
   - Test failures
   - Warnings
   - Coverage drops

### RSS Feeds

Subscribe to RSS feeds for:
- Build results
- Test failures
- Coverage changes

## Dashboard Best Practices

### For Developers

**Before Committing:**
1. Run local tests: `ctest`
2. Fix all failing tests
3. Check for new warnings
4. Commit only when clean

**After Committing:**
1. Monitor dashboard within 30 minutes
2. Check your commit didn't break builds
3. Fix any new failures immediately
4. Respond to email notifications

### For Reviewers

**Code Review:**
1. Check dashboard before merging
2. Verify all tests pass
3. Review coverage impact
4. Check for new warnings

**Continuous Monitoring:**
1. Check dashboard daily
2. Track trends over time
3. Investigate persistent failures
4. Address coverage gaps

## Platform-Specific Builds

### Windows Builds

**Tested Platforms:**
- Windows 10 x64
- Windows 11 x64
- Visual Studio 2019, 2022

**Common Issues:**
- Path length limitations
- Permission issues
- Missing runtime libraries

### Linux Builds

**Tested Distributions:**
- Ubuntu 20.04 LTS
- Ubuntu 22.04 LTS
- Debian 11

**Common Issues:**
- Missing development packages
- Qt version conflicts
- OpenGL driver issues

### macOS Builds

**Tested Versions:**
- macOS 11 (Big Sur)
- macOS 12 (Monterey)

**Common Issues:**
- Xcode version compatibility
- Code signing requirements
- Qt framework paths

## Troubleshooting Dashboard Issues

### Build Not Appearing

**Possible causes:**
- Network issues during submission
- Incorrect project name
- CTestConfig.cmake errors

**Solutions:**
```bash
# Verify CTest configuration
ctest -D ExperimentalSubmit --debug

# Check CDash connection
curl http://perkdata.cs.queensu.ca/CDash/submit.php
```

### Tests Timeout

**Possible causes:**
- Slow hardware
- Deadlocks
- Infinite loops

**Solutions:**
```cmake
# Increase timeout in CMakeLists.txt
set_tests_properties(MyTest PROPERTIES TIMEOUT 300)
```

### Coverage Not Generated

**Possible causes:**
- Coverage not enabled
- Debug build required
- gcov/lcov not installed

**Solutions:**
```bash
# Linux: enable coverage
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DCOVERAGE=ON ..

# Build and test
make -j4
ctest -D ExperimentalCoverage
```

## Advanced Features

### Custom Test Groups

Group related tests:

```cmake
set_tests_properties(
  Test1 Test2 Test3
  PROPERTIES LABELS "Calibration")
```

Run specific groups:
```bash
ctest -L Calibration
```

### Test Dependencies

Specify test order:

```cmake
set_tests_properties(TestB 
  PROPERTIES DEPENDS TestA)
```

### Test Fixtures

Setup and cleanup:

```cmake
set_tests_properties(SetupTest 
  PROPERTIES FIXTURES_SETUP MyFixture)
  
set_tests_properties(MainTest
  PROPERTIES FIXTURES_REQUIRED MyFixture)
  
set_tests_properties(CleanupTest
  PROPERTIES FIXTURES_CLEANUP MyFixture)
```

## Dashboard Metrics

### Build Time

Track build performance:
- Total build time
- Per-target build time
- Trends over time

**Optimization tips:**
- Use parallel builds: `cmake --build . -j8`
- Enable ccache: `set(CMAKE_CXX_COMPILER_LAUNCHER ccache)`
- Reduce template instantiations

### Test Execution Time

Monitor test performance:
- Individual test duration
- Total test suite time
- Slowest tests

**Optimization tips:**
- Parallelize tests: `ctest -j8`
- Reduce test data size
- Mock expensive operations

## See Also

- [Running Tests](running-tests.md)
- [Contributing Guide](../developer-guide/contributing.md)
- [Build Instructions](../getting-started/build-instructions.md)
