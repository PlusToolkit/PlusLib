PROJECT(PlusLib)

CMAKE_MINIMUM_REQUIRED(VERSION 3.3.0)

SET(CMAKE_CXX_STANDARD 11)

INCLUDE(CMake/PlusLibMacros.cmake)

# ------------------------------------------------------------------------
# Set project version number
SET(PLUSLIB_VERSION_MAJOR "2")
SET(PLUSLIB_VERSION_MINOR "7")
SET(PLUSLIB_VERSION_PATCH "0")
SET(PLUSLIB_VERSION ${PLUSLIB_VERSION_MAJOR}.${PLUSLIB_VERSION_MINOR}.${PLUSLIB_VERSION_PATCH})

# Set project install folders, these paths should be relative to ${CMAKE_INTALL_PREFIX}, absolute paths will break things
SET(PLUSLIB_INSTALL_FOLDERNAME "PlusLib-${PLUSLIB_VERSION_MAJOR}.${PLUSLIB_VERSION_MINOR}")
SET(PLUSLIB_BINARY_INSTALL "bin")
SET(PLUSLIB_INCLUDE_INSTALL "include/${PLUSLIB_INSTALL_FOLDERNAME}")
SET(PLUSLIB_LIBRARY_INSTALL "lib")
SET(PLUSLIB_ARCHIVE_INSTALL "lib")
SET(PLUSLIB_CMAKE_INSTALL "lib/cmake/${PLUSLIB_INSTALL_FOLDERNAME}")
SET(PLUSLIB_SHARE_INSTALL "share/${PLUSLIB_INSTALL_FOLDERNAME}")

# Use solution folders.
IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
  SET_PROPERTY(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake Targets")
  SET_PROPERTY(GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER "AutoGen (Qt) Targets")
ENDIF()

MESSAGE(STATUS "Configuring PLUS toolkit version ${PLUSLIB_VERSION}.")

IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  IF( ${CMAKE_GENERATOR} MATCHES "Win64")
    SET(PLUSLIB_PLATFORM "Win64" CACHE STRING "Name of the platform this instance of PlusLib is built on")
  ELSE()
    SET(PLUSLIB_PLATFORM "Win32" CACHE STRING "Name of the platform this instance of PlusLib is built on")
  ENDIF()
ELSE()
  IF(APPLE)
    SET(PLUSLIB_PLATFORM "OSX" CACHE STRING "Name of the platform this instance of PlusLib is built on")
  ELSE()
    SET(PLUSLIB_PLATFORM "Linux" CACHE STRING "Name of the platform this instance of PlusLib is built on")
  ENDIF()
ENDIF()

MESSAGE(STATUS "Detected platform: ${PLUSLIB_PLATFORM}.")

# --------------------------------------------------------------------------
# System Settings
IF(WIN32)
  # This method adds the necessary compiler flag
  SET(RUNTIME_MINIMUM_WINDOWS_VERSION "0x0602")
  ADD_DEFINITIONS(-D_MSC_PLATFORM_TOOLSET_$(PlatformToolset))
ENDIF()

# --------------------------------------------------------------------------
# Testing
#
OPTION(BUILD_TESTING "Build test programs" ON)

INCLUDE(CTest)

# Clean up the IDE folder name for CTest dashboard targets
IF(BUILD_TESTING)
  FOREACH(mode Experimental Nightly Continuous NightlyMemoryCheck)
    IF(TARGET ${mode})
      SET_PROPERTY(TARGET ${mode} PROPERTY FOLDER "CTest Targets")
    ENDIF()
  ENDFOREACH()
ENDIF()

# --------------------------------------------------------------------------
# Repository
#
IF(PLUS_OFFLINE_BUILD)
  SET(PLUSLIB_REVISION "NA")
  SET(PLUSLIB_SHORT_REVISION "NA")
  SET(PLUSLIB_COMMIT_DATE_NO_DASHES "NA")
  SET(PLUSLIB_COMMIT_DATE "NA")
ELSE()
  # Need git for the automatic update of the repository
  FIND_PACKAGE(Git)
  IF(Git_FOUND)
    EXECUTE_PROCESS(COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD 
      OUTPUT_VARIABLE PLUSLIB_REVISION 
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      )
    STRING(REGEX REPLACE "\n$" "" PLUSLIB_REVISION "${PLUSLIB_REVISION}")
    STRING(REGEX REPLACE "\r$" "" PLUSLIB_REVISION "${PLUSLIB_REVISION}")
    
    MESSAGE(STATUS "Current git hash is ${PLUSLIB_REVISION}")

    EXECUTE_PROCESS(COMMAND "${GIT_EXECUTABLE}" rev-parse --short HEAD 
      OUTPUT_VARIABLE PLUSLIB_SHORT_REVISION 
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      )
    STRING(REGEX REPLACE "\n$" "" PLUSLIB_SHORT_REVISION "${PLUSLIB_SHORT_REVISION}")
    STRING(REGEX REPLACE "\r$" "" PLUSLIB_SHORT_REVISION "${PLUSLIB_SHORT_REVISION}")

    EXECUTE_PROCESS(COMMAND ${GIT_EXECUTABLE} show ${PLUSLIB_REVISION} --date=iso8601 
      OUTPUT_VARIABLE _git_show_output
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      )
    STRING(REGEX MATCH "([0-9])+-([0-9]+)-([0-9]+)" PLUSLIB_COMMIT_DATE ${_git_show_output})
    STRING(REPLACE "-" "" PLUSLIB_COMMIT_DATE_NO_DASHES ${PLUSLIB_COMMIT_DATE})

  ELSE()
    MESSAGE(WARNING "Git tool not found. Cannot retrieve revision information from repository")
    SET(PLUSLIB_REVISION "NA")
    SET(PLUSLIB_SHORT_REVISION "NA")
  ENDIF()
ENDIF()

# --------------------------------------------------------------------------
# Developer setup
#
IF(Git_FOUND)
  GET_FILENAME_COMPONENT(_git_directory ${GIT_EXECUTABLE} DIRECTORY)
ENDIF()
SET(_x86env "ProgramFiles(x86)")
FIND_PROGRAM(BASH_EXECUTABLE bash
  HINTS 
    ${_git_directory}
    $ENV{ProgramFiles}/Git/bin
    $ENV{${_x86env}}/Git/bin
  )
MARK_AS_ADVANCED(BASH_EXECUTABLE)
IF(BASH_EXECUTABLE)
  EXECUTE_PROCESS(COMMAND "${BASH_EXECUTABLE}" -c "cd ${CMAKE_SOURCE_DIR} && ${CMAKE_SOURCE_DIR}/Utilities/SetupForDevelopment.sh copyOnly"
    OUTPUT_VARIABLE _bashOutput
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}}
  )
ENDIF()

# --------------------------------------------------------------------------
# Set up paths for data and SDKs
#
SET(PLUSLIB_TOOLS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Tools)

SET(ATC_TRAKSTAR_BASE_DIR ${PLUSLIB_TOOLS_DIR}/Ascension/trakSTAR_940041_RevE)
SET(ATC_TRAKSTAR_INCLUDE_DIR ${ATC_TRAKSTAR_BASE_DIR}/inc CACHE PATH "Path to the Ascension trakSTAR 3D Guidance include folder (where the ATC3DG.h file is located)" FORCE )
SET(ATC_TRAKSTAR_LIB_DIR ${ATC_TRAKSTAR_BASE_DIR}/lib CACHE PATH "Path to the Ascension trakSTAR 3D Guidance lib folder" FORCE )
SET(ATC_TRAKSTAR_BINARY_DIR ${ATC_TRAKSTAR_BASE_DIR}/bin CACHE PATH "Path to the Ascension trakSTAR 3D Guidance dll folder" FORCE )
SET(ATC_MEDSAFE_BASE_DIR ${PLUSLIB_TOOLS_DIR}/Ascension/medSAFE_940033_Rev_F)
SET(ATC_MEDSAFE_INCLUDE_DIR ${ATC_MEDSAFE_BASE_DIR}/inc CACHE PATH "Path to the Ascension medSAFE 3D Guidance include folder (where the ATC3DGm.h file is located)" FORCE )
SET(ATC_MEDSAFE_LIB_DIR ${ATC_MEDSAFE_BASE_DIR}/lib CACHE PATH "Path to the Ascension medSAFE 3D Guidance lib folder" FORCE )
SET(ATC_MEDSAFE_BINARY_DIR ${ATC_MEDSAFE_BASE_DIR}/bin CACHE PATH "Path to the Ascension medSAFE 3D Guidance dll folder" FORCE )
MARK_AS_ADVANCED(ATC_TRAKSTAR_BASE_DIR
  ATC_TRAKSTAR_INCLUDE_DIR
  ATC_TRAKSTAR_LIB_DIR
  ATC_TRAKSTAR_BINARY_DIR
  ATC_MEDSAFE_BASE_DIR
  ATC_MEDSAFE_INCLUDE_DIR
  ATC_MEDSAFE_LIB_DIR
  ATC_MEDSAFE_BINARY_DIR
  )
IF(PLUS_USE_Ascension3DG)
  ADD_SUBDIRECTORY(${ATC_TRAKSTAR_BASE_DIR})
ENDIF()
IF(PLUS_USE_Ascension3DGm)
 ADD_SUBDIRECTORY(${ATC_MEDSAFE_BASE_DIR})
ENDIF()

SET(PHIDGET_SPATIAL_BASE_DIR ${PLUSLIB_TOOLS_DIR}/Phidget/Phidget22)
SET(PHIDGET_SPATIAL_INCLUDE_DIR ${PHIDGET_SPATIAL_BASE_DIR} CACHE PATH "Path to the Phidget Spatial include folder" FORCE )
IF(MSVC AND ${CMAKE_GENERATOR} MATCHES "Win64")
  SET(PHIDGET_SPATIAL_LIB_DIR ${PHIDGET_SPATIAL_BASE_DIR}/x64 CACHE PATH "Path to the Phidget Spatial 64-bit lib folder" FORCE )
  SET(PHIDGET_SPATIAL_BINARY_DIR ${PHIDGET_SPATIAL_BASE_DIR}/x64 CACHE PATH "Path to the Phidget Spatial 64-bit dll folder" FORCE )
ELSE()
  SET(PHIDGET_SPATIAL_LIB_DIR ${PHIDGET_SPATIAL_BASE_DIR}/x86 CACHE PATH "Path to the Phidget Spatial 32-bit lib folder" FORCE )
  SET(PHIDGET_SPATIAL_BINARY_DIR ${PHIDGET_SPATIAL_BASE_DIR}/x86 CACHE PATH "Path to the Phidget Spatial 32-bit dll folder" FORCE )
ENDIF()
MARK_AS_ADVANCED(PHIDGET_SPATIAL_BASE_DIR
  PHIDGET_SPATIAL_INCLUDE_DIR
  PHIDGET_SPATIAL_LIB_DIR
  PHIDGET_SPATIAL_BINARY_DIR
  )
IF(PLUS_USE_PHIDGET_SPATIAL_TRACKER)
  ADD_SUBDIRECTORY(${PLUSLIB_TOOLS_DIR}/Phidget/Phidget22)
ENDIF()

SET(EPIPHAN_SDK_BASE_DIR ${PLUSLIB_TOOLS_DIR}/Epiphan)
SET(EPIPHAN_INCLUDE_DIR ${EPIPHAN_SDK_BASE_DIR} CACHE PATH "Path to the Epiphan SDK include directory." FORCE )
IF(MSVC AND ${CMAKE_GENERATOR} MATCHES "Win64")
  SET(EPIPHAN_BINARY_DIR ${EPIPHAN_SDK_BASE_DIR}/x64 CACHE PATH "Directory containing Epiphan shared library file." FORCE)
  SET(EPIPHAN_LIB_DIR ${EPIPHAN_SDK_BASE_DIR}/x64 CACHE PATH "Directory containing Epiphan static library file." FORCE)
ELSE()
  SET(EPIPHAN_BINARY_DIR ${EPIPHAN_SDK_BASE_DIR}/Win32 CACHE PATH "Directory containing Epiphan shared library file." FORCE)
  SET(EPIPHAN_LIB_DIR ${EPIPHAN_SDK_BASE_DIR}/Win32 CACHE PATH "Directory containing Epiphan static library file." FORCE)
ENDIF()
MARK_AS_ADVANCED(EPIPHAN_SDK_BASE_DIR
  EPIPHAN_INCLUDE_DIR
  EPIPHAN_BINARY_DIR
  EPIPHAN_LIB_DIR
  )
IF(PLUS_USE_EPIPHAN)
  ADD_SUBDIRECTORY(${PLUSLIB_TOOLS_DIR}/Epiphan)
ENDIF()

SET(USDIGITAL_SEI_BASE_DIR ${PLUSLIB_TOOLS_DIR}/UsDigital/SEI_5.22)
SET(USDIGITAL_SEI_INCLUDE_DIR ${USDIGITAL_SEI_BASE_DIR}/inc CACHE PATH "Path to the US Digital SEI include directory (needed for the CIVCO stepper)." FORCE )
SET(USDIGITAL_SEI_BINARY_DIR ${USDIGITAL_SEI_BASE_DIR}/bin CACHE PATH "Path to the US Digital SEI binary directory (needed for the CIVCO stepper)." FORCE )
SET(USDIGITAL_SEI_LIB_DIR ${USDIGITAL_SEI_BASE_DIR}/lib CACHE PATH "Path to the US Digital SEI library directory (needed for the CIVCO stepper)." FORCE )
MARK_AS_ADVANCED(USDIGITAL_SEI_BASE_DIR
  USDIGITAL_SEI_INCLUDE_DIR
  USDIGITAL_SEI_BINARY_DIR
  USDIGITAL_SEI_LIB_DIR
  )
IF(PLUS_USE_BRACHY_TRACKER)
  ADD_SUBDIRECTORY(${PLUSLIB_TOOLS_DIR}/UsDigital/SEI_5.22)
ENDIF()

IF(WIN32)
  SET(NVIDIA_DVP_BASE_DIR ${PLUSLIB_TOOLS_DIR}/NVidia/dvp170)
  SET(NVIDIA_DVP_INCLUDE_DIR ${NVIDIA_DVP_BASE_DIR}/include CACHE PATH "Path to the NVidia DVP include directory." FORCE )
  IF(MSVC AND ${CMAKE_GENERATOR} MATCHES "Win64")
    SET(NVIDIA_DVP_BINARY_DIR ${NVIDIA_DVP_BASE_DIR}/bin/x64 CACHE PATH "Directory containing NVidia DVP shared library file." FORCE)
    SET(NVIDIA_DVP_LIB_DIR ${NVIDIA_DVP_BASE_DIR}/lib/x64 CACHE PATH "Directory containing NVidia DVP static library file." FORCE)
  ELSE()
    SET(NVIDIA_DVP_BINARY_DIR ${NVIDIA_DVP_BASE_DIR}/bin/win32 CACHE PATH "Directory containing NVidia DVP shared library file." FORCE)
    SET(NVIDIA_DVP_LIB_DIR ${NVIDIA_DVP_BASE_DIR}/lib/win32 CACHE PATH "Directory containing NVidia DVP static library file." FORCE)
  ENDIF()
  MARK_AS_ADVANCED(NVIDIA_DVP_BASE_DIR
    NVIDIA_DVP_INCLUDE_DIR
    NVIDIA_DVP_BINARY_DIR
    NVIDIA_DVP_LIB_DIR
    )
  IF(PLUS_USE_NVIDIA_DVP)
    ADD_SUBDIRECTORY(${PLUSLIB_TOOLS_DIR}/NVidia/dvp170)
  ENDIF()
ENDIF()

INCLUDE (CheckCXXSourceCompiles)
CHECK_CXX_SOURCE_COMPILES("#include <future>
  int main(){ return 0; }" 
  HAVE_FUTURE
  )

# --------------------------------------------------------------------------
# Subdirs
#
ADD_SUBDIRECTORY(src)

#-----------------------------------------------------------------------------
# Generate convenience files for automatic build, test, and packaging
#
IF(WIN32)
  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/BuildAndTest.bat.in
    ${CMAKE_CURRENT_BINARY_DIR}/BuildAndTest.bat
    )

  # Documentation generator
  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/CreateApiReference.bat.in
    ${CMAKE_CURRENT_BINARY_DIR}/CreateApiReference.bat
    )
ELSE()
  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/BuildAndTest.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/BuildAndTest.sh
    )
  FILE(COPY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/BuildAndTest.sh
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
    FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ WORLD_READ
    )
ENDIF()