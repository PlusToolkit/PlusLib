# --------------------------------------------------------------------------
# Try to find VTK and include its settings (otherwise complain)
FIND_PACKAGE(VTK NO_MODULE REQUIRED PATHS ${VTK_DIR} NO_DEFAULT_PATH)

IF(VTK_FOUND)
  INCLUDE(${VTK_USE_FILE})
ELSE()
  MESSAGE(FATAL_ERROR "This application requires VTK. One of these components is missing. Please verify configuration")
ENDIF()

# --------------------------------------------------------------------------
# Determine if rendering is enabled in VTK
SET(PLUS_RENDERING_ENABLED ON)
IF (VTK_RENDERING_BACKEND STREQUAL None)
  SET(PLUS_RENDERING_ENABLED OFF)
ENDIF()

# --------------------------------------------------------------------------
# Try to find ITK and include its settings (otherwise complain)
FIND_PACKAGE (ITK REQUIRED PATHS ${ITK_DIR} NO_DEFAULT_PATH)
IF(ITK_FOUND)
  IF(ITK_NO_IO_FACTORY_REGISTER_MANAGER)
    MESSAGE("ITK_NO_IO_FACTORY_REGISTER_MANAGER is true, disabling default I/O factory manager")
  ENDIF()
  INCLUDE(${ITK_USE_FILE})
ELSE()
  MESSAGE(FATAL_ERROR "This application requires ITK. One of these components is missing. Please verify configuration")
ENDIF()

# --------------------------------------------------------------------------
# Use OpenIGTLink protocol
OPTION(PLUS_USE_OpenIGTLink "Use the OpenIGTLink protocol" OFF)
SET(PLUS_OPENIGTLINK_VERSION)
IF(PLUS_USE_OpenIGTLink)
  FIND_PACKAGE(OpenIGTLink REQUIRED NO_MODULE)
  IF(${OpenIGTLink_PROTOCOL_VERSION} LESS 3)
    MESSAGE(FATAL_ERROR "PLUS requires a build of OpenIGTLink with v3 support enabled.")
  ENDIF()

  SET(PLUS_OPENIGTLINK_VERSION "${OpenIGTLink_VERSION_MAJOR}.${OpenIGTLink_VERSION_MINOR}.${OpenIGTLink_VERSION_PATCH}")
  SET(PLUS_OPENIGTLINK_VERSION_MAJOR "${OpenIGTLink_VERSION_MAJOR}")
  SET(PLUS_OPENIGTLINK_VERSION_MINOR "${OpenIGTLink_VERSION_MINOR}")
  SET(PLUS_OPENIGTLINK_VERSION_PATCH "${OpenIGTLink_VERSION_PATCH}")

  FIND_PACKAGE(OpenIGTLinkIO REQUIRED NO_MODULE)
ENDIF()

# Reset the list of PlusLib Qt components
SET(PLUSLIB_QT_COMPONENTS "" CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Use IGSIO
FIND_PACKAGE(IGSIO REQUIRED NO_MODULE)

# --------------------------------------------------------------------------
# Options
OPTION(BUILD_SHARED_LIBS "Build with shared libraries." ${VTK_BUILD_SHARED_LIBS} ${ITK_BUILD_SHARED_LIBS})

# --------------------------------------------------------------------------
# Use accurate or simple timer protocol
#
# DevPartnerStudio performance profiler hangs if the accurate timer
# is used. If USE_SIMPLE_TIMER is defined then the accurate timer is
# bypassed. It should only be enabled for performance profiling.

OPTION(PLUS_USE_SIMPLE_TIMER "Use simple timer (not very accurate but more compatible with performance profilers)" OFF)
MARK_AS_ADVANCED(PLUS_USE_SIMPLE_TIMER)

OPTION (PLUS_TEST_HIGH_ACCURACY_TIMING "Enable testing of high-accuracy timing. High-accuracy timing may not be available on virtual machines and so testing may be turned off to avoid false alarams." ON)
MARK_AS_ADVANCED(PLUS_TEST_HIGH_ACCURACY_TIMING)

OPTION(PLUS_USE_INTEL_MKL "Use the Intel MKL library (only for image processing)" OFF)

OPTION(PLUS_BUILD_WIDGETS "Build re-usable widgets for writing PlusLib based applications" OFF)
IF(PLUS_BUILD_WIDGETS)
  FIND_PACKAGE(Qt5 REQUIRED COMPONENTS Core Widgets Test Xml)
  SET(PLUSLIB_QT_COMPONENTS ${PLUSLIB_QT_COMPONENTS} Core Widgets Test Xml CACHE INTERNAL "" FORCE)

  # Instruct CMake to run tools automatically when needed.
  SET(CMAKE_AUTOMOC ON)
  SET(CMAKE_AUTOUIC ON)
  SET(CMAKE_AUTORCC ON)
ENDIF()

# --------------------------------------------------------------------------
# Configure output paths for libraries and executables.
IF(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  IF(DEFINED PLUS_EXECUTABLE_OUTPUT_PATH)
    SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PLUS_EXECUTABLE_OUTPUT_PATH}")
  ELSE()
    SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PlusLib_BINARY_DIR}/bin")
  ENDIF()
ENDIF()
SET(PLUS_EXECUTABLE_OUTPUT_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

IF(NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  IF(DEFINED PLUS_LIBRARY_OUTPUT_PATH)
    SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PLUS_LIBRARY_OUTPUT_PATH}")
  ELSE()
    SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PlusLib_BINARY_DIR}/lib")
  ENDIF()
ENDIF()
SET(PLUS_LIBRARY_OUTPUT_PATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

IF(NOT DEFINED CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  IF(DEFINED PLUS_ARCHIVE_OUTPUT_PATH)
    SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PLUS_ARCHIVE_OUTPUT_PATH}")
  ELSE()
    SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PlusLib_BINARY_DIR}/lib")
  ENDIF()
ENDIF()
SET(PLUS_ARCHIVE_OUTPUT_PATH "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")

IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  SET(TEST_OUTPUT_PATH "${PLUS_EXECUTABLE_OUTPUT_PATH}/Release/Output")
ELSE()
  SET(TEST_OUTPUT_PATH "${PLUS_EXECUTABLE_OUTPUT_PATH}/Output")
ENDIF()

# --------------------------------------------------------------------------
# The project library directories.
SET(PLUSLIB_LIBRARY_DIRS ${PLUS_LIBRARY_OUTPUT_PATH} CACHE INTERNAL "Path to the Plus library directory" FORCE)

# Library directories should be added automatically by the build system, however GrabbieLib
# is not found by default (as it does not have a proper USE file), therefore we add
# the library output directory to the library search path
LINK_DIRECTORIES(${PLUS_LIBRARY_OUTPUT_PATH})

# --------------------------------------------------------------------------
# Command-line application documentation

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
      INSTALL(FILES "${PLUS_EXECUTABLE_OUTPUT_PATH}/Doc/Help/${TARGET_NAME}Help.txt"
        DESTINATION "${PLUSLIB_SHARE_INSTALL}/doc")
  ENDIF()
ENDMACRO()

# --------------------------------------------------------------------------
# Library export directive file generation

# This macro generates a ...Export.h file that specifies platform-specific DLL export directives,
# for example on Windows: __declspec( dllexport )
MACRO(GENERATE_EXPORT_DIRECTIVE_FILE LIBRARY_NAME)
  SET(MY_LIBNAME ${LIBRARY_NAME})
  SET(MY_EXPORT_HEADER_PREFIX ${MY_LIBNAME})
  SET(MY_LIBRARY_EXPORT_DIRECTIVE "${MY_LIBNAME}Export")
  CONFIGURE_FILE(
    ${PlusLib_SOURCE_DIR}/src/PlusExport.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/${MY_EXPORT_HEADER_PREFIX}Export.h
    )
  INSTALL(FILES 
    ${CMAKE_CURRENT_BINARY_DIR}/${MY_EXPORT_HEADER_PREFIX}Export.h
    DESTINATION "${PLUSLIB_INCLUDE_INSTALL}"
    )
ENDMACRO()

# --------------------------------------------------------------------------
# Initialize variables that will be filled by makefiles in subdirectories

# PLUSLIB_DEPENDENCIES contain all the exported targets that will be
# available to applications that use PlusLib
SET(PLUSLIB_DEPENDENCIES "" CACHE INTERNAL "")

# PLUSLIB_INCLUDE_DIRS contain all the include directories that will be
# available to applications that use PlusLib
SET(PLUSLIB_INCLUDE_DIRS "" CACHE INTERNAL "")

# PLUSLIB_INSTALL_INCLUDE_FILES contains the include files to install with the build
SET(PLUSLIB_INSTALL_INCLUDE_FILES )

# PLUSLIB_INSTALL_CMAKE_FILES contains the cmake files to install
SET(PLUSLIB_INSTALL_CMAKE_FILES "")

# CONFIG CODE FOR OPENIGTLINKIO LIBRARY
SET(OPENIGTLINKIO_CONFIG_CODE "")

# --------------------------------------------------------------------------
# Current directory

SET(BASIC_INCLUDE_DIRS
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_BINARY_DIR}"
  )
INCLUDE_DIRECTORIES(${BASIC_INCLUDE_DIRS})
SET(PLUSLIB_INCLUDE_DIRS ${PLUSLIB_INCLUDE_DIRS} ${BASIC_INCLUDE_DIRS} CACHE INTERNAL "")

# --------------------------------------------------------------------------
# Subdirectories

# Variables sent back from subdirectories (through CACHE):
# ..._INCLUDE_DIRS
# ..._INSTALL_INCLUDE_FILES
# ..._INSTALL_CMAKE_FILES
# vcProj_...
# PLUSLIB_DEPENDENCIES

IF(PLUS_USE_SYSTEM_ZLIB) # setting PlusZLib needs to come before processing subdirectories
  SET(PlusZLib ${ZLIB_LIBRARY})
  INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})
  LIST(APPEND PLUSLIB_INCLUDE_DIRS ${ZLIB_INCLUDE_DIR} CACHE INTERNAL "")
ELSE()
  SET(PlusZLib vtkzlib)
ENDIF()

IF(PLUS_BUILD_WIDGETS)
  ADD_SUBDIRECTORY(PlusWidgets)
  INCLUDE_DIRECTORIES(${PlusWidgets_INCLUDE_DIRS})
  LIST(APPEND PLUSLIB_INCLUDE_DIRS ${PlusWidgets_INCLUDE_DIRS} CACHE INTERNAL "")
ENDIF()

ADD_SUBDIRECTORY(Utilities)
INCLUDE_DIRECTORIES(${Utilities_INCLUDE_DIRS})
LIST(APPEND PLUSLIB_INCLUDE_DIRS ${Utilities_INCLUDE_DIRS} CACHE INTERNAL "")

ADD_SUBDIRECTORY(PlusCommon)
INCLUDE_DIRECTORIES(${PlusCommon_INCLUDE_DIRS})
LIST(APPEND PLUSLIB_INCLUDE_DIRS ${PlusCommon_INCLUDE_DIRS} CACHE INTERNAL "")

IF (PLUS_RENDERING_ENABLED)
  ADD_SUBDIRECTORY(PlusRendering)
  INCLUDE_DIRECTORIES(${PlusRendering_INCLUDE_DIRS})
  LIST(APPEND PLUSLIB_INCLUDE_DIRS ${PlusRendering_INCLUDE_DIRS} CACHE INTERNAL "")
ENDIF()

IF(PLUS_USE_OpenIGTLink)
  ADD_SUBDIRECTORY(PlusOpenIGTLink)
  INCLUDE_DIRECTORIES(${PlusOpenIGTLink_INCLUDE_DIRS})
  LIST(APPEND PLUSLIB_INCLUDE_DIRS ${PlusOpenIGTLink_INCLUDE_DIRS} CACHE INTERNAL "")
ENDIF()

ADD_SUBDIRECTORY(PlusImageProcessing)
INCLUDE_DIRECTORIES(${PlusImageProcessing_INCLUDE_DIRS})
LIST(APPEND PLUSLIB_INCLUDE_DIRS ${PlusImageProcessing_INCLUDE_DIRS} CACHE INTERNAL "")

ADD_SUBDIRECTORY(PlusUsSimulator)
INCLUDE_DIRECTORIES(${PlusUsSimulator_INCLUDE_DIRS})
LIST(APPEND PLUSLIB_INCLUDE_DIRS ${PlusUsSimulator_INCLUDE_DIRS} CACHE INTERNAL "")

ADD_SUBDIRECTORY(PlusVolumeReconstruction)
INCLUDE_DIRECTORIES(${PlusVolumeReconstruction_INCLUDE_DIRS})
LIST(APPEND PLUSLIB_INCLUDE_DIRS ${PlusVolumeReconstruction_INCLUDE_DIRS} CACHE INTERNAL "")

IF(PLUS_USE_USDIGITALENCODERS_TRACKER)
  ADD_SUBDIRECTORY(SEIDrv)
  INCLUDE_DIRECTORIES(${SEIDrv_INCLUDE_DIRS})
  LIST(APPEND PLUSLIB_INCLUDE_DIRS ${SEIDrv_INCLUDE_DIRS} CACHE INTERNAL "")
ENDIF()

ADD_SUBDIRECTORY(PlusDataCollection)
INCLUDE_DIRECTORIES(${PlusDataCollection_INCLUDE_DIRS})
LIST(APPEND PLUSLIB_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS} CACHE INTERNAL "")

ADD_SUBDIRECTORY(PlusCalibration)
INCLUDE_DIRECTORIES(${PlusCalibration_INCLUDE_DIRS})
LIST(APPEND PLUSLIB_INCLUDE_DIRS ${PlusCalibration_INCLUDE_DIRS} CACHE INTERNAL "")

IF(PLUS_USE_OpenIGTLink)
  ADD_SUBDIRECTORY(PlusServer)
  INCLUDE_DIRECTORIES(${PlusServer_INCLUDE_DIRS})
  LIST(APPEND PLUSLIB_INCLUDE_DIRS ${PlusServer_INCLUDE_DIRS} CACHE INTERNAL "")
ENDIF()

ADD_SUBDIRECTORY(scripts)

# --------------------------------------------------------------------------
# Configure include file
CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/PlusConfigure.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/PlusConfigure.h
  )
CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/PlusConfigure.h.in
  ${CMAKE_BINARY_DIR}/PlusConfigure.h
  )
CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/PlusRevision.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/PlusRevision.h
  )

INSTALL(FILES
  ${CMAKE_BINARY_DIR}/PlusConfigure.h
  ${CMAKE_CURRENT_BINARY_DIR}/PlusRevision.h
  DESTINATION ${PLUSLIB_INCLUDE_INSTALL}
  )

# --------------------------------------------------------------------------
# Create Plus target list

# Add all targets to the build-tree export set
export(TARGETS
  ${PLUSLIB_DEPENDENCIES}
  FILE "${CMAKE_CURRENT_BINARY_DIR}/PlusLibLibraryDepends.cmake"
  )

#-----------------------------------------------------------------------------
# With regards to ticket #1075, also generate in top-level directory
export(TARGETS
  ${PLUSLIB_DEPENDENCIES}
  FILE "${CMAKE_BINARY_DIR}/PlusLibTargets.cmake"
  )

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE PlusLib)

#----------------------------------------------------------------------------
# Append entries for export
IF(PLUS_USE_EPIPHAN)
  LIST(APPEND PLUSLIB_DEPENDENCIES Epiphan)
ENDIF()
IF(PLUS_USE_Ascension3DG)
  LIST(APPEND PLUSLIB_DEPENDENCIES Ascension3DG)
ENDIF()
IF(PLUS_USE_Ascension3DGm)
  LIST(APPEND PLUSLIB_DEPENDENCIES Ascension3DGm)
ENDIF()
IF(PLUS_USE_PHIDGET_SPATIAL_TRACKER)
  LIST(APPEND PLUSLIB_DEPENDENCIES phidget)
ENDIF()
IF(PLUS_USE_BRACHY_TRACKER)
  LIST(APPEND PLUSLIB_DEPENDENCIES USDigitalEncoders)
ENDIF()
IF(PLUS_USE_NVIDIA_DVP)
  LIST(APPEND PLUSLIB_DEPENDENCIES NVidiaDVP)
  LIST(APPEND PLUSLIB_DEPENDENCIES QuadroSDI)
ENDIF()

# for installing PLUSLIB_CONFIG_CODE is used
SET(PLUSLIB_CONFIG_CODE "SET(PlusLib_TARGETS_FILE \"${CMAKE_BINARY_DIR}/PlusLibTargets.cmake\")\n")
SET(VTK_CONFIG_DIR ${VTK_DIR})
SET(ITK_CONFIG_DIR ${ITK_DIR})
SET(PLUSLIB_CONFIG_USEFILE "${CMAKE_BINARY_DIR}/UsePlusLib.cmake")

# Create a PlusConfig.cmake and PlusConfigVersion.cmake file for the use from the build tree
#-----------------------------------------------------------------------------
INCLUDE(CMakePackageConfigHelpers)
# Re: #1075 eventually remove this section after people are used to the new location
CONFIGURE_FILE(PlusLibConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/PlusLibConfig.cmake" @ONLY)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/PlusLibConfigVersion.cmake"
  VERSION ${PLUSLIB_VERSION}
  COMPATIBILITY ExactVersion)
LIST(APPEND PLUSLIB_INSTALL_CMAKE_FILES} "${CMAKE_CURRENT_BINARY_DIR}/PlusLibConfigVersion.cmake")

#-----------------------------------------------------------------------------
# With regards to ticket #1075, also generate in top-level directory
CONFIGURE_FILE(PlusLibConfig.cmake.in
  "${CMAKE_BINARY_DIR}/PlusLibConfig.cmake" @ONLY)
write_basic_package_version_file(
  "${CMAKE_BINARY_DIR}/PlusLibConfigVersion.cmake"
  VERSION ${PLUSLIB_VERSION}
  COMPATIBILITY ExactVersion)

#-----------------------------------------------------------------------------
# Generate default application configuration files in the non-installed executable directories (Release and Debug too)
SET(PLUSCONFIG_DEVICESET_CONFIG_DIR "${PLUSLIB_DATA_DIR}/ConfigFiles")
SET(PLUSCONFIG_IMAGE_DIR "${PLUSLIB_DATA_DIR}/TestImages")
SET(PLUSCONFIG_MODEL_DIR "${PLUSLIB_DATA_DIR}/CADModels")
SET(PLUSCONFIG_SCRIPTS_DIR "${PLUSLIB_SCRIPTS_DIR}")

IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  # PlusConfig.xml generation for Windows or Mac (separate directories for different build types)
  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/PlusConfig.xml.in
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/Release/PlusConfig.xml
    )
  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/PlusConfig.xml.in
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/Debug/PlusConfig.xml
    )
  INSTALL(FILES ${PLUS_EXECUTABLE_OUTPUT_PATH}/Release/PlusConfig.xml
    DESTINATION ${PLUSLIB_SHARE_INSTALL}
    )
ELSE()
  # PlusConfig.xml generation for other OS (only one build type is supported)
  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/PlusConfig.xml.in
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/PlusConfig.xml
    )
  INSTALL(FILES ${PLUS_EXECUTABLE_OUTPUT_PATH}/PlusConfig.xml
    DESTINATION ${PLUSLIB_SHARE_INSTALL}
    )
ENDIF()

#-----------------------------------------------------------------------------
# Documentation
OPTION(BUILD_DOCUMENTATION "Build the documentation (Doxygen)." OFF)
MARK_AS_ADVANCED(BUILD_DOCUMENTATION)

IF(BUILD_DOCUMENTATION)
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
# Generate include file for projects that use this library
# this file currently contains only binary paths
CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/UsePlusLib.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/UsePlusLib.cmake @ONLY)

#-----------------------------------------------------------------------------
# With regards to ticket #1075, also generate in top-level directory
CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/UsePlusLib.cmake.in
  ${CMAKE_BINARY_DIR}/UsePlusLib.cmake @ONLY)

#-----------------------------------------------------------------------------
# Starting here, all operations are exclusively for INSTALL
#-----------------------------------------------------------------------------
### That means some variables are filled with bogus data in other cases.

#-----------------------------------------------------------------------------
# Install UseFile/Config
INSTALL(FILES ${PLUSLIB_INSTALL_INCLUDE_FILES}
  DESTINATION ${PLUSLIB_INCLUDE_INSTALL} COMPONENT Development
  )

INSTALL(FILES ${PLUSLIB_INSTALL_CMAKE_FILES}
  DESTINATION ${PLUSLIB_CMAKE_INSTALL} COMPONENT Development
  )

# set all paths to install location paths
#relative to PlusConfig.cmake

# include files should all we copied into the include folder
SET(PLUSLIB_INCLUDE_DIRS "\${PLUSLIB_INSTALL_PATH}/${PLUSLIB_INCLUDE_INSTALL}")
SET(PLUSLIB_DATA_DIR "\${PLUSLIB_INSTALL_PATH}/${PLUSLIB_SHARE_INSTALL}")
# PlusLib_Libraries is filled with depending external libaries twice, during build/installation and inclusion in 2nd project
SET(PlusLib_LIBRARIES "")

IF(${PLUS_USE_EPIPHAN})
  SET(EPIPHAN_LIB_DIR "\${PLUSLIB_INSTALL_PATH}/${PLUSLIB_LIBRARY_INSTALL}")
  SET(EPIPHAN_BINARY_DIR "\${PLUSLIB_INSTALL_PATH}/${PLUSLIB_BINARY_INSTALL}")
ENDIF()

IF(${PLUS_USE_Ascension3DG})
  GET_FILENAME_COMPONENT(Ascension3DG_LIB_FILE ${Ascension3DG_LIB_FILE} NAME)
  SET(Ascension3DG_LIB_FILE "\${PLUSLIB_INSTALL_PATH}/${PLUSLIB_LIBRARY_INSTALL}/${Ascension3DG_LIB_FILE}")
  GET_FILENAME_COMPONENT(Ascension3DG_SHARED_LIB_FILE ${Ascension3DG_SHARED_LIB_FILE} NAME)
  SET(Ascension3DG_SHARED_LIB_FILE "\${PLUSLIB_INSTALL_PATH}/${PLUSLIB_BINARY_INSTALL}/${Ascension3DG_SHARED_LIB_FILE}")
ENDIF()

IF(${PLUS_USE_Ascension3DGm})
  SET(Ascension3DGm_BINARY_DIR "\${PLUSLIB_INSTALL_PATH}/${PLUSLIB_BINARY_INSTALL}")
  SET(Ascension3DGm_LIB_DIR "\${PLUSLIB_INSTALL_PATH}/${PLUSLIB_LIBRARY_INSTALL}")
ENDIF()

IF(${PLUS_USE_PHIDGET_SPATIAL_TRACKER})
  SET(PHIDGET_SPATIAL_BINARY_DIR "\${PLUSLIB_INSTALL_PATH}/${PLUSLIB_BINARY_INSTALL}")
  SET(PHIDGET_SPATIAL_LIB_DIR "\${PLUSLIB_INSTALL_PATH}/${PLUSLIB_LIBRARY_INSTALL}")
ENDIF()

IF(${PLUS_USE_BRACHY_TRACKER})
  SET(USDIGITAL_SEI_BINARY_DIR "\${PLUSLIB_INSTALL_PATH}/${PLUSLIB_BINARY_INSTALL}")
  SET(USDIGITAL_SEI_LIB_DIR "\${PLUSLIB_INSTALL_PATH}/${PLUSLIB_LIBRARY_INSTALL}")
ENDIF()

IF(${PLUS_USE_NVIDIA_DVP})
  # use the relative paths instead of absolute paths
  FILE(RELATIVE_PATH NVIDIA_DVP_LIB_DIR ${CMAKE_INSTALL_PREFIX} ${NVIDIA_DVP_LIB_DIR})
  FILE(RELATIVE_PATH NVIDIA_DVP_BINARY_DIR ${CMAKE_INSTALL_PREFIX} ${NVIDIA_DVP_BINARY_DIR})
  FILE(RELATIVE_PATH QuadroSDI_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX} ${QuadroSDI_INCLUDE_DIRS})
  FILE(RELATIVE_PATH QuadroSDI_LIBRARY_DIR ${CMAKE_INSTALL_PREFIX} ${QuadroSDI_LIBRARY_DIR})
ENDIF()

IF(${PLUS_USE_OpenIGTLink})
  # use the relative path instead of absolute path
  FILE(RELATIVE_PATH OpenIGTLink_DIR ${CMAKE_INSTALL_PREFIX} ${OpenIGTLink_DIR})
  SET(OpenIGTLink_DIR "\${PLUSLIB_INSTALL_PATH}/${OpenIGTLink_DIR}")
ENDIF()

IF(${PLUS_USE_OvrvisionPro})
  # use the relative path instead of absolute path
  FILE(RELATIVE_PATH OvrvisionPro_DIR ${CMAKE_INSTALL_PREFIX} ${OvrvisionPro_DIR})
  SET(OvrvisionPro_DIR "\${PLUSLIB_INSTALL_PATH}/${OvrvisionPro_DIR}")
ENDIF()

# use the relative paths instead of absolute paths
FILE(RELATIVE_PATH VTK_REL_DIR ${CMAKE_INSTALL_PREFIX} ${VTK_DIR})
FILE(RELATIVE_PATH ITK_REL_DIR ${CMAKE_INSTALL_PREFIX} ${ITK_DIR})

IF(${PLUS_USE_TextRecognizer})
  # use the relative path instead of absolute path
  FILE(RELATIVE_PATH tesseract_DIR ${CMAKE_INSTALL_PREFIX} ${tesseract_DIR})
  SET(tesseract_DIR "\${PLUSLIB_INSTALL_PATH}/${tesseract_DIR}")
ENDIF()

IF(${PLUS_USE_OpenCV})
  # use the relative path instead of absolute path
  FILE(RELATIVE_PATH OpenCV_DIR ${CMAKE_INSTALL_PREFIX} ${OpenCV_DIR})
  SET(OpenCV_DIR "\${PLUSLIB_INSTALL_PATH}/${OpenCV_DIR}")
ENDIF()

IF(${PLUS_USE_aruco})
  # use the relative path instead of absolute path
  FILE(RELATIVE_PATH aruco_DIR ${CMAKE_INSTALL_PREFIX} ${aruco_DIR})
  SET(aruco_DIR "\${PLUSLIB_INSTALL_PATH}/${aruco_DIR}")
ENDIF()

IF(${PLUS_USE_SLICER})
  # use the relative path instead of absolute path
  FILE(RELATIVE_PATH SLICER_BIN_DIRECTORY ${CMAKE_INSTALL_PREFIX} ${SLICER_BIN_DIRECTORY})
  SET(SLICER_BIN_DIRECTORY "\${PLUSLIB_INSTALL_PATH}/${SLICER_BIN_DIRECTORY}")
ENDIF()

INSTALL(EXPORT PlusLib
  DESTINATION "${PLUSLIB_CMAKE_INSTALL}" COMPONENT Development
  FILE PlusLibTargets.cmake
  )

SET(PLUSLIB_CONFIG_CODE [=[
GET_FILENAME_COMPONENT(PLUSLIB_PACKAGE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
GET_FILENAME_COMPONENT(_IMPORT_PREFIX "${PLUSLIB_PACKAGE_DIR}" PATH)
GET_FILENAME_COMPONENT(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)
GET_FILENAME_COMPONENT(PLUSLIB_INSTALL_PATH "${_IMPORT_PREFIX}" PATH)
SET(PlusLib_TARGETS_FILE "${PLUSLIB_PACKAGE_DIR}/PlusLibTargets.cmake")]=])
SET(VTK_CONFIG_DIR "\${PLUSLIB_INSTALL_PATH}/${VTK_REL_DIR}")
SET(ITK_CONFIG_DIR "\${PLUSLIB_INSTALL_PREFIX}/${ITK_REL_DIR}")
SET(PLUSLIB_CONFIG_USEFILE "\${PLUSLIB_PACKAGE_DIR}/UsePlusLib.cmake")

CONFIGURE_FILE(
  PlusLibConfig.cmake.in
  ${CMAKE_BINARY_DIR}/CMakeFiles/install/PlusLibConfig.cmake @ONLY
  )
CONFIGURE_FILE(
  UsePlusLib.cmake.in
  ${CMAKE_BINARY_DIR}/CMakeFiles/install/UsePlusLib.cmake @ONLY
  )

INSTALL(FILES 
  "${CMAKE_BINARY_DIR}/CMakeFiles/install/PlusLibConfig.cmake"
  "${CMAKE_BINARY_DIR}/CMakeFiles/install/UsePlusLib.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/PlusLibConfigVersion.cmake"
  DESTINATION "${PLUSLIB_CMAKE_INSTALL}"
  )