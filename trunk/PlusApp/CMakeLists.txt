PROJECT(PlusApp)

CMAKE_MINIMUM_REQUIRED(VERSION 3.3.0)

# ------------------------------------------------------------------------
# PlusLib
# --------------------------------------------------------------------------
FIND_PACKAGE(PlusLib REQUIRED NO_MODULE)
INCLUDE(${PlusLib_USE_FILE})

# ------------------------------------------------------------------------
# Set project version number: PlusApp version should identical to Plus version
# --------------------------------------------------------------------------
SET(PLUSAPP_VERSION_MAJOR ${PLUSLIB_VERSION_MAJOR})
SET(PLUSAPP_VERSION_MINOR ${PLUSLIB_VERSION_MINOR})
SET(PLUSAPP_VERSION_PATCH ${PLUSLIB_VERSION_PATCH})
SET(PLUSAPP_VERSION ${PLUSAPP_VERSION_MAJOR}.${PLUSAPP_VERSION_MINOR}.${PLUSAPP_VERSION_PATCH})
SET(PLUSAPP_REVISION ${PLUSLIB_REVISION})

# Use solution folders.
IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
  SET_PROPERTY(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake Targets")
  SET_PROPERTY(GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER "AutoGen (Qt) Targets")
ENDIF()

SET(PLUSAPP_PACKAGE_EDITION "" CACHE STRING "Specifies a name that refers to the combination of hardware devices the created install package supports. The name is added to the package filename.")

MESSAGE(STATUS "PlusApp version: ${PLUSAPP_VERSION}" )

# --------------------------------------------------------------------------
# Testing
# --------------------------------------------------------------------------

# Standard CMake option for testing.
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

# Setup file for setting custom ctest vars (for ignoring certain warnings, etc.)
CONFIGURE_FILE(
  CTestCustom.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake
  @ONLY
  )

# Documentation generator
CONFIGURE_FILE(
  CreateUserManual.bat.in
  ${CMAKE_CURRENT_BINARY_DIR}/CreateUserManual.bat
  )

# --------------------------------------------------------------------------
# Find SVN for revision information status and automatic update of the repository
# --------------------------------------------------------------------------
IF (PLUSAPP_OFFLINE_BUILD)
  SET(PLUSAPP_REVISION 0)
ELSE()
  FIND_PACKAGE(Git)
  IF(Git_FOUND)
    EXECUTE_PROCESS(COMMAND git rev-parse HEAD OUTPUT_VARIABLE PLUSAPP_REVISION WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    STRING(REGEX REPLACE "\n$" "" PLUSAPP_REVISION "${PLUSAPP_REVISION}")
    STRING(REGEX REPLACE "\r$" "" PLUSAPP_REVISION "${PLUSAPP_REVISION}")
    
    MESSAGE(STATUS "Current git hash is ${PLUSLIB_REVISION}")

    EXECUTE_PROCESS(COMMAND git rev-parse --short HEAD OUTPUT_VARIABLE PLUSAPP_SHORT_REVISION WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    STRING(REGEX REPLACE "\n$" "" PLUSAPP_SHORT_REVISION "${PLUSAPP_SHORT_REVISION}")
    STRING(REGEX REPLACE "\r$" "" PLUSAPP_SHORT_REVISION "${PLUSAPP_SHORT_REVISION}")
  ENDIF()
ENDIF()

# --------------------------------------------------------------------------
# Standard CMake option for building libraries shared or static by default.
# --------------------------------------------------------------------------
OPTION(BUILD_SHARED_LIBS
  "Build with shared libraries."
  ${VTK_BUILD_SHARED_LIBS}
  ${ITK_BUILD_SHARED_LIBS}
  )

# --------------------------------------------------------------------------
# Configure output paths for libraries and executables.
# --------------------------------------------------------------------------
IF(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin CACHE PATH "Single output directory for building all executables." FORCE)
  SET(PLUS_EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin CACHE PATH "Single output directory for building all executables." FORCE)
ENDIF()

IF(NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib CACHE PATH "Single output directory for building all libraries." FORCE)
  SET(PLUS_LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib CACHE PATH "Single output directory for building all libraries." FORCE)
ENDIF()

IF(NOT DEFINED CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib CACHE PATH "Single output directory for building all archives." FORCE)
  SET(PLUS_ARCHIVE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib CACHE PATH "Single output directory for building all archives." FORCE)
ENDIF()

IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  SET(TEST_EXECUTABLE_OUTPUT_PATH "${PLUS_EXECUTABLE_OUTPUT_PATH}/Release")
ELSE()
  SET(TEST_EXECUTABLE_OUTPUT_PATH "${PLUS_EXECUTABLE_OUTPUT_PATH}")
ENDIF()

SET(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/Install)
SET(INSTALL_DESTINATION_DIR "PlusApp_v${PLUSAPP_VERSION_MAJOR}.${PLUSAPP_VERSION_MINOR}.${PLUSAPP_VERSION_PATCH}")

OPTION(PLUSAPP_BUILD_DiagnosticTools "Build the DiagnosticTools" ON)
OPTION(PLUSAPP_BUILD_fCal "Build the fCal application (Qt)" ON)
OPTION(PLUSAPP_TEST_GUI "Enable GUI tests" ON)

# --------------------------------------------------------------------------
# Command-line application documentation
# --------------------------------------------------------------------------

# Create directory for the command-line application help files (they are generated by after build by running
# each command-line tool with the --help parameter)
IF(BUILD_DOCUMENTATION)
  FILE(MAKE_DIRECTORY ${PLUS_EXECUTABLE_OUTPUT_PATH}/Doc/Help)
ENDIF()

# This macro generates a command-line application help file
MACRO(GENERATE_HELP_DOC TARGET_NAME)
  IF(BUILD_DOCUMENTATION)
    ADD_CUSTOM_COMMAND(
      TARGET ${TARGET_NAME}
      POST_BUILD
      COMMAND $<TARGET_FILE:${TARGET_NAME}> --help > ${TARGET_NAME}Help.txt
      WORKING_DIRECTORY ${PLUS_EXECUTABLE_OUTPUT_PATH}/Doc/Help
      )
  ENDIF()
ENDMACRO()

# --------------------------------------------------------------------------
# Packaging
# --------------------------------------------------------------------------
INCLUDE (${CMAKE_CURRENT_SOURCE_DIR}/CPackConfig.cmake)

# --------------------------------------------------------------------------
# Qt
# --------------------------------------------------------------------------
# As moc files are generated in the binary dir, tell CMake
  # to always look for includes there:
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

FIND_PACKAGE(Qt5 REQUIRED COMPONENTS Core Widgets Test Xml Network)

# These variables are populated as part of our install script
SET(QT_VERSION_MAJOR ${Qt5Core_VERSION_MAJOR}) 
SET(QT_VERSION_MINOR ${Qt5Core_VERSION_MINOR})
SET(QT_VERSION_PATCH ${Qt5Core_VERSION_PATCH})
SET(QT_VERSION ${Qt5Core_VERSION})
GET_FILENAME_COMPONENT(QT_BINARY_DIR ${QT_MOC_EXECUTABLE} DIRECTORY)
GET_FILENAME_COMPONENT(QT_ROOT_DIR ${QT_BINARY_DIR} DIRECTORY)

# Instruct CMake to run tools automatically when needed.
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTORCC ON)

# --------------------------------------------------------------------------
# Build various utilities
# --------------------------------------------------------------------------
ADD_SUBDIRECTORY(PointSetExtractor)
ADD_SUBDIRECTORY(SpatialSensorFusion)

IF(PLUS_USE_OpenIGTLink)
  ADD_SUBDIRECTORY(PlusServerLauncher) #(Qt)
ENDIF()

# --------------------------------------------------------------------------
# Build the DiagnosticTools
# --------------------------------------------------------------------------
IF(PLUSAPP_BUILD_DiagnosticTools)
  ADD_SUBDIRECTORY(DiagnosticTools)
  ADD_DEPENDENCIES(DiagDataCollection ${PLUSLIB_DEPENDENCIES})
ENDIF()

# --------------------------------------------------------------------------
# Build the fCal application (Qt)
# --------------------------------------------------------------------------
IF(PLUSAPP_BUILD_fCal)
  ADD_SUBDIRECTORY(fCal)
  ADD_DEPENDENCIES(fCal ${PLUSLIB_DEPENDENCIES})
ENDIF()

#-----------------------------------------------------------------------------
# Generate documentation
# --------------------------------------------------------------------------
OPTION(BUILD_DOCUMENTATION "Build the documentation (Doxygen)." ${PLUSLIB_BUILD_DOCUMENTATION})
MARK_AS_ADVANCED(BUILD_DOCUMENTATION)

IF(NOT EXISTS ${DOXYGEN_DOT_EXECUTABLE})
  SET(PROGWIN64 "PROGRAMFILES")
  SET(PROGWIN32 "PROGRAMFILES(X86)")

  # Try to detect GraphViz path (CMake's Doxygen package finder only tries some obsolete paths on Windows)
  FIND_PROGRAM(DOXYGEN_DOT_EXECUTABLE
    NAMES dot
    PATHS
      "$ENV{${PROGWIN32}}/Graphviz2.38/bin"
      "$ENV{${PROGWIN64}}/Graphviz2.38/bin"
      "$ENV{${PROGWIN32}}/Graphviz2.34/bin"
      "$ENV{${PROGWIN64}}/Graphviz2.34/bin"
    DOC "Graphviz Dot tool for using Doxygen"
    NO_SYSTEM_ENVIRONMENT_PATH
    )

  IF(WIN32)
    SET(DOXYGEN_HHC_EXECUTABLE "${PLUSLIB_TOOLS_DIR}/HtmlHelp/hhc.exe" CACHE FILEPATH "Path to the Windows HTML Help Compiler tool." FORCE)
  ENDIF()
ENDIF()

ADD_SUBDIRECTORY(Documentation)

#-----------------------------------------------------------------------------
# Include PlusLib MS projects
# --------------------------------------------------------------------------
INCLUDE_PLUSLIB_MS_PROJECTS()
GROUP_PLUSLIB_MS_PROJECTS("PlusLib")

SET (PLUSAPP_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

# --------------------------------------------------------------------------
# Exports & Generation
# --------------------------------------------------------------------------
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE PlusApp)

# Create a PlusAppConfig.cmake file for the use from the build tree
CONFIGURE_FILE(PlusAppConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/PlusAppConfig.cmake" @ONLY)
CONFIGURE_FILE(PlusAppConfigVersion.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/PlusAppConfigVersion.cmake" @ONLY)

# Generate include file for projects that use this library
CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/UsePlusApp.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/UsePlusApp.cmake
    @ONLY
  )

# Generate default application configuration file for the install
SET(PLUSCONFIG_DEVICESET_CONFIG_DIR "../config")
SET(PLUSCONFIG_IMAGE_DIR "../data")
SET(PLUSCONFIG_MODEL_DIR "../config")
SET(PLUSCONFIG_SCRIPTS_DIR "../scripts")

CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/PlusConfig.xml.in
  ${CMAKE_CURRENT_BINARY_DIR}/PlusConfig.xml
  )

# Generate convenience files for automatic build, test, and packaging
IF(WIN32)
  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/CreatePackage.bat.in
    ${CMAKE_CURRENT_BINARY_DIR}/CreatePackage.bat
    )
  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/BuildAndTest.bat.in
    ${CMAKE_CURRENT_BINARY_DIR}/BuildAndTest.bat
    )
ELSE()
  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/BuildAndTest.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/BuildAndTest.sh
    )
  FILE(COPY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/BuildAndTest.sh
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
    FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ WORLD_READ)
ENDIF()

#-----------------------------------------------------------------------------
# Install files
# --------------------------------------------------------------------------
INCLUDE(InstallFiles.cmake)