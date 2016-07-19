PROJECT(PlusBuild)
#
# Configure the build to work (although with limited functionalities) if only
# src directory of the repository is available
#
CMAKE_MINIMUM_REQUIRED(VERSION 3.3.0)

IF(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

SET(CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}/Modules
  ${CMAKE_MODULE_PATH}
  )

OPTION(PLUSBUILD_OFFLINE_BUILD "Build Plus without an internet connection. All libraries must be downloaded and updated manually." OFF)
MARK_AS_ADVANCED(PLUSBUILD_OFFLINE_BUILD)

IF (PLUSBUILD_OFFLINE_BUILD)

  # Set an empty download and update command for external projects
  SET(PLUSBUILD_EXTERNAL_PROJECT_CUSTOM_COMMANDS DOWNLOAD_COMMAND "" UPDATE_COMMAND "" )

ELSE (PLUSBUILD_OFFLINE_BUILD)

  # Don't change the default download command
  # This argument will be used for ExternalProject_Add macro, which does its
  # very strict parsing, which throws a warning if an empty argument is passed.
  # Therefore, we need to pass a harmless parameter to prevent warnings
  # (TIMEOUT parameter is chosen, as no download operation should be performed
  # for offline builds, therefore the timeout parameter is not used anyway).
  SET(PLUSBUILD_EXTERNAL_PROJECT_CUSTOM_COMMANDS TIMEOUT 1000)

  #-----------------------------------------------------------------------------
  # SVN - Let's check if a valid version of SVN is available
  #-----------------------------------------------------------------------------
  # Need subversion for the automatic update of the repository
  MARK_AS_ADVANCED(CLEAR Subversion_SVN_EXECUTABLE)
  FIND_FILE(Subversion_SVN_EXECUTABLE svn${CMAKE_EXECUTABLE_SUFFIX}
    PATHS
    "c:/Program Files/TortoiseSVN/bin/"
    "c:/Program Files/SlikSvn/bin/"
    )
  FIND_PACKAGE(Subversion REQUIRED)

  #-----------------------------------------------------------------------------
  # ASSEMBLA - Set username and password for assembla
  # Anonymous user name for Plus assembla server: https://subversion.assembla.com/svn/plus/
  #-----------------------------------------------------------------------------
  SET(PLUSBUILD_ASSEMBLA_USERNAME "perklab_anonymous" )
  SET(PLUSBUILD_ASSEMBLA_PASSWORD "anonymous" )

  #-----------------------------------------------------------------------------
  # GIT - Let's check if a valid version of GIT is available
  #-----------------------------------------------------------------------------
  # Need Git for the automatic update of the repository

  OPTION(PLUSBUILD_USE_GIT_PROTOCOL "If behind a firewall turn this off to use http instead." ON)

  SET(GIT_PROTOCOL "git")
  IF(NOT PLUSBUILD_USE_GIT_PROTOCOL)
    SET(GIT_PROTOCOL "http")
  ENDIF()

  MARK_AS_ADVANCED(CLEAR GIT_EXECUTABLE)
  FIND_FILE(GIT_EXECUTABLE git${CMAKE_EXECUTABLE_SUFFIX}
    PATHS
      "c:/Program Files/Git/bin/"
      "c:/Program Files (x86)/Git/bin/"
    )
  FIND_PACKAGE(Git)
  IF(NOT GIT_FOUND)
    MESSAGE(FATAL_ERROR "error: Install Git and try to re-configure")
  ENDIF()

ENDIF (PLUSBUILD_OFFLINE_BUILD)

#-----------------------------------------------------------------------------
# Options to control build process
#-----------------------------------------------------------------------------

# Determine the operating system to set default values accordingly
SET (ENABLED_BY_DEFAULT_ON_WINDOWS_ONLY OFF)
SET (ENABLED_BY_DEFAULT_ON_WINDOWS32_ONLY OFF)
SET (ENABLED_BY_DEFAULT_ON_WINDOWS64_ONLY OFF)
if (CMAKE_HOST_WIN32)
  SET (ENABLED_BY_DEFAULT_ON_WINDOWS_ONLY ON)
  if (NOT CMAKE_CL_64)
    SET (ENABLED_BY_DEFAULT_ON_WINDOWS32_ONLY ON)
  else()
    SET (ENABLED_BY_DEFAULT_ON_WINDOWS64_ONLY ON)
  endif()
endif (CMAKE_HOST_WIN32)

OPTION(PLUSBUILD_USE_3DSlicer "Instead of building ITK, VTK, OpenIGTLink, etc., get them from a 3D Slicer build tree." OFF)
MARK_AS_ADVANCED(PLUSBUILD_USE_3DSlicer)
OPTION(PLUSBUILD_USE_OpenIGTLink "Use OpenIGTLink" ON)
OPTION(PLUSBUILD_BUILD_SHARED_LIBS "Build shared libraries instead of statically-linked libraries" ON)
OPTION(PLUSBUILD_BUILD_PLUSAPP "Build PlusApp applications" ON)
OPTION(PLUSBUILD_BUILD_PlusLib_TOOLS "Build the PlusLib tools (PlusServer, PlusServerRemoteControl...)" ON)
OPTION(PLUSBUILD_BUILD_PlusLib_SHARED_LIBS "Build the PlusLib as shared libraries" ON)
OPTION(PLUSBUILD_BUILD_PLUSMODELCATALOG "Build Plus 3D printable model catalog" OFF)
OPTION(PLUSBUILD_USE_Tesseract "Use OCR in PlusLib for recognizing ultrasound imaging parameters by reading text from grabbed images." OFF)
OPTION(PLUSBUILD_DOWNLOAD_PlusDATA "Download sample and test data. Required for automatic tests." ON)
IF(PLUSBUILD_DOWNLOAD_PlusDATA)
  SET(PLUSBUILD_PlusDATA_SVNREVISION "HEAD" CACHE STRING "Revision of data to download." FORCE)
  MARK_AS_ADVANCED(PLUSBUILD_PlusDATA_SVNREVISION)
ENDIF()

# Documentation
option(PLUSBUILD_DOCUMENTATION "Build Plus documentation (Doxygen)." OFF)
if(PLUSBUILD_DOCUMENTATION)
  # Try to detect GraphViz path (CMake's Doxygen package finder only tries some obsolete paths on Windows)
  # Parentheses is not permitted due to CMP0053
  SET(PROGRAMFILESX86 "ProgramFiles(x86)")
  find_program(DOXYGEN_DOT_EXECUTABLE
    NAMES dot
    PATHS
      "$ENV{ProgramFiles}/Graphviz2.38/bin"
      "$ENV{${PROGRAMFILESX86}}/Graphviz2.38/bin"
      "$ENV{ProgramFiles}/Graphviz2.34/bin"
      "$ENV{${PROGRAMFILESX86}}/Graphviz2.34/bin"
    DOC "Graphviz Dot tool for using Doxygen"
    NO_SYSTEM_ENVIRONMENT_PATH
  )
  find_package( Doxygen REQUIRED)

  if( NOT DOXYGEN_FOUND )
    message( FATAL_ERROR "Documentation: Doxygen not found. Either specify location of doxygen or disable PLUSBUILD_DOCUMENTATION." )
  endif( NOT DOXYGEN_FOUND )
  if( NOT DOXYGEN_DOT_FOUND )
    message( FATAL_ERROR "Documentation: Graphviz dot tool not found (http://www.graphviz.org/Download.php, required by Doxygen for diagram generation). Either specify location of dot or disable PLUSBUILD_DOCUMENTATION." )
  endif( NOT DOXYGEN_DOT_FOUND )

  OPTION(PLUSBUILD_DOCUMENTATION_SEARCH_SERVER_INDEXED "Search index for documentation is generated by th web server. Provides full-text search but only works on web servers." OFF)
  MARK_AS_ADVANCED(PLUSBUILD_DOCUMENTATION_SEARCH_SERVER_INDEXED)

endif(PLUSBUILD_DOCUMENTATION)

# Imaging hardware

# determine current OS
if("$ENV{PROCESSOR_ARCHITEW6432}" STREQUAL "")
  if("$ENV{PROCESSOR_ARCHITECTURE}" STREQUAL "x86")
    SET( TEMP_OS_ARCH "x86" )
  else()
    SET( TEMP_OS_ARCH "x64" )
  endif()
else()
  SET( TEMP_OS_ARCH "x64" )
endif()

OPTION(PLUS_USE_ULTRASONIX_VIDEO "Provide support for the Ultrasonix ultrasound systems" OFF)
SET (PLUS_ULTRASONIX_SDK_MAJOR_VERSION 5 CACHE STRING "Set Ultrasonix SDK major version (version: [major].[minor].[patch])")
SET (PLUS_ULTRASONIX_SDK_MINOR_VERSION 7 CACHE STRING "Set Ultrasonix SDK minor version (version: [major].[minor].[patch])")
SET (PLUS_ULTRASONIX_SDK_PATCH_VERSION 4 CACHE STRING "Set Ultrasonix SDK patch version (version: [major].[minor].[patch])")
IF (PLUS_USE_ULTRASONIX_VIDEO)
  OPTION(PLUS_TEST_ULTRASONIX "Enable testing of acquisition from Ultrasonix ultrasound systems. Enable this only if an Ultrasonix device accessible from this computer. " OFF)
  IF (PLUS_TEST_ULTRASONIX)
    SET (PLUS_TEST_ULTRASONIX_IP_ADDRESS "130.15.7.24" CACHE STRING "IP address of the Ultrasonix scanner that is used during testing")
  ENDIF()
ENDIF (PLUS_USE_ULTRASONIX_VIDEO)

OPTION(PLUS_USE_BKPROFOCUS_VIDEO "Provide support for BK ProFocus ultrasound systems through the OEM (TCP/IP) interface" OFF)
IF (PLUS_USE_BKPROFOCUS_VIDEO)
  OPTION(PLUS_USE_BKPROFOCUS_CAMERALINK "Enable acquisition from BK ProFocus ultrasound systems through CameraLink interface" OFF)
  OPTION(PLUS_TEST_BKPROFOCUS "Enable testing of acquisition from BK ProFocus ultrasound systems. Enable this only if a BK ProFocus device is connected to this computer. " OFF)
ENDIF ()
IF ( (NOT ${CMAKE_GENERATOR} MATCHES "Win64") AND TEMP_OS_ARCH MATCHES "x64" AND PLUS_USE_BKPROFOCUS_CAMERALINK)
  # warning regarding cross compilation of bkprofocus
  MESSAGE( "BK ProFocus support on a 64-bit OS requires 64-bit Plus build. A 64-bit OS and a 32-bit Plus build configuration is detected. Compilation will be successful, but the resulting executables will fail to start." )
ENDIF ()
IF ( PLUS_USE_BKPROFOCUS_CAMERALINK AND (NOT PLUS_USE_BKPROFOCUS_VIDEO) )
  MESSAGE(FATAL_ERROR "error: PLUS_USE_BKPROFOCUS_VIDEO must be enabled if the PLUS_USE_BKPROFOCUS_CAMERALINK option is enabled" )
ENDIF ()

OPTION(PLUS_USE_ICCAPTURING_VIDEO "Provide support for the IC framegrabber device" OFF)
OPTION(PLUS_USE_VFW_VIDEO "Provide support for the Video-for-Windows video digitizer (legacy, use Microsoft Media Foundation instead)" OFF)
OPTION(PLUS_USE_MMF_VIDEO "Provide support for the Microsoft Media Foundation video digitizers (requires installation of Windows Platform SDK 7.1 or later)" OFF)
IF(PLUS_USE_MMF_VIDEO)
  OPTION(PLUS_TEST_MMF_VIDEO "Enable testing of acquisition from MMF video device (webcam). Enable this only if an MMF device is connected to this computer." OFF)
ENDIF(PLUS_USE_MMF_VIDEO)
OPTION(PLUS_USE_EPIPHAN "Provide support for the Epiphan framegrabber device" OFF)
OPTION(PLUS_USE_CAPISTRANO_VIDEO "Provide support for the Capistrano Labs USB ultrasound probes" OFF)
OPTION(PLUS_USE_INTERSON_VIDEO "Provide support for the Interson USB ultrasound probes" OFF)
OPTION(PLUS_USE_INTERSONSDKCXX_VIDEO "Provide support for the Interson SDK 1.X with C++ Wrapper USB ultrasound probes" OFF)
OPTION(PLUS_USE_TELEMED_VIDEO "Provide support for the Telemed ultrasound probes" OFF)
IF (PLUS_USE_TELEMED_VIDEO)
  OPTION(PLUS_TEST_TELEMED "Enable testing of acquisition from Telemed ultrasound systems. Enable this only if a Telemed device is connected to this computer. " OFF)
ENDIF (PLUS_USE_TELEMED_VIDEO)
OPTION(PLUS_USE_THORLABS_VIDEO "Provide support for the ThorLabs Compact Spectrometers" OFF)

# Tracking hardware
OPTION(PLUS_USE_OPTITRACK "Provide support for the OptiTrack tracking system" OFF)
OPTION(PLUS_USE_OPTIMET_CONOPROBE "Provide support for the Optimet ConoProbe" OFF)

OPTION(PLUS_USE_NDI "Provide support for the NDI POLARIS and AURORA" OFF)
OPTION(PLUS_USE_NDI_CERTUS "Provide support for the NDI Certus" OFF)

OPTION(PLUS_USE_POLARIS "Provide support for the NDI POLARIS and AURORA" OFF)
MARK_AS_ADVANCED(PLUS_USE_POLARIS)
OPTION(PLUS_USE_CERTUS "Provide support for the NDI Certus" OFF)
MARK_AS_ADVANCED(PLUS_USE_CERTUS)

IF(PLUS_USE_POLARIS)
  MESSAGE("PLUS_USE_POLARIS has been deprecated. Changing to PLUS_USE_NDI instead.")
  SET(PLUS_USE_POLARIS OFF CACHE BOOL "Provide support for the NDI POLARIS and AURORA" FORCE)
  SET(PLUS_USE_NDI ON CACHE BOOL "Provide support for the NDI POLARIS and AURORA" FORCE)
ENDIF()
IF(PLUS_USE_CERTUS)
  MESSAGE("PLUS_USE_CERTUS has been deprecated. Changing to PLUS_USE_NDI_CERTUS instead.")
  SET(PLUS_USE_CERTUS OFF CACHE BOOL "Provide support for the NDI Certus" FORCE)
  SET(PLUS_USE_NDI_CERTUS ON CACHE BOOL "Provide support for the NDI Certus" FORCE)
ENDIF()

OPTION(PLUS_USE_MICRONTRACKER "Provide support for the Claron MicronTracker" OFF)
OPTION(PLUS_USE_INTELREALSENSE "Provide support for Intel RealSense cameras" OFF)
OPTION(PLUS_USE_BRACHY_TRACKER "Provide support for the Brachy Steppers" ${ENABLED_BY_DEFAULT_ON_WINDOWS32_ONLY})
OPTION(PLUS_USE_USDIGITALENCODERS_TRACKER "Provide support for multiple USDigital encoders tracking device" ${ENABLED_BY_DEFAULT_ON_WINDOWS32_ONLY})
OPTION(PLUS_USE_Ascension3DG "Provide support for the Ascension 3DG Tracker" ${ENABLED_BY_DEFAULT_ON_WINDOWS32_ONLY})
OPTION(PLUS_USE_Ascension3DGm "Provide support for the Ascension MedSafe Tracker" OFF)
OPTION(PLUS_USE_PHIDGET_SPATIAL_TRACKER "Provide support for the Phidget Spatial accelerometer" OFF)
OPTION(PLUS_USE_3dConnexion_TRACKER "Provide support for the 3dConnexion 3d mouse" OFF)
OPTION(PLUS_USE_STEALTHLINK "Provide support for the Medtronick StealthLink Server" OFF)
OPTION(PLUS_USE_IntuitiveDaVinci "Provide support for the da Vinci Surgical System" OFF)
OPTION(PLUS_USE_PHILIPS_3D_ULTRASOUND "Provide support for the Philips ie33 3D ultrasound probe" OFF)
IF(PLUS_USE_PHILIPS_3D_ULTRASOUND)
  OPTION(PLUS_TEST_PHILIPS_3D_ULTRASOUND "Enable testing of acquisition from Philips 3D ultrasound systems. Enable this only if a Philips device is accessible from this computer. " OFF)
ENDIF(PLUS_USE_PHILIPS_3D_ULTRASOUND)
IF (PLUS_TEST_PHILIPS_3D_ULTRASOUND)
  SET (PLUS_TEST_PHILIPS_3D_ULTRASOUND_IP_ADDRESS "129.100.44.8" CACHE STRING "IP address of the Philips scanner that is used during testing")
ENDIF(PLUS_TEST_PHILIPS_3D_ULTRASOUND)

# Other

OPTION (PLUSAPP_TEST_GUI "Enable Sikuli GUI tests" ON)

OPTION (PLUS_TEST_HIGH_ACCURACY_TIMING "Enable testing of high-accuracy timing. High-accuracy timing may not be available on virtual machines and so testing may be turned off to avoid false alarams." ON)
MARK_AS_ADVANCED(PLUS_TEST_HIGH_ACCURACY_TIMING)

SET(PLUSAPP_PACKAGE_EDITION "" CACHE STRING "Specifies a name that refers to the combination of hardware devices the created install package supports. The name is added to the package filename.")

#-----------------------------------------------------------------------------
# Warnings for incompatible build options
#-----------------------------------------------------------------------------
IF (PLUS_USE_Ascension3DG AND PLUS_USE_Ascension3DGm)
  MESSAGE(FATAL_ERROR "PLUS_USE_Ascension3DG and PLUS_USE_Ascension3DGm options cannot be enabled at the same time. See more details at https://www.assembla.com/spaces/plus/tickets/851")
ENDIF()

#-----------------------------------------------------------------------------
# Plus revision - Set Plus stable relase revision (0 means latest)
#-----------------------------------------------------------------------------
SET (PLUS_SVN_REVISION 0 CACHE STRING "Set Plus desired SVN revision number (0 means latest)")

#-----------------------------------------------------------------------------
# Plus executable output path
#-----------------------------------------------------------------------------
SET (PLUS_EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

#-----------------------------------------------------------------------------
# Qt - Let's check if a valid version of Qt is available
#-----------------------------------------------------------------------------
SET(ep_qt_args) # Arguments that will have to be passed to external projects that use Qt
IF(PLUSBUILD_BUILD_PLUSAPP AND NOT PLUSBUILD_USE_3DSlicer)
  FIND_PACKAGE(Qt5 COMPONENTS Core Widgets Designer OpenGL Sql QUIET)

  IF ( NOT Qt5_FOUND )
    FIND_PACKAGE(Qt4 QUIET)

    IF ( NOT Qt4_FOUND)
      MESSAGE( FATAL_ERROR "This project requires Qt4 or Qt5 for building PlusApp but Qt was not found. Please specify QT_QMAKE_EXECUTABLE (for Qt4) or Qt5_DIR (for Qt5) or turn off PLUSBUILD_BUILD_PLUSAPP option.")
    ENDIF()

    GET_FILENAME_COMPONENT(QT_BINARY_DIR ${QT_QMAKE_EXECUTABLE} DIRECTORY)
    SET(ep_qt_args
      -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
      )
  ELSE()
    SET(QT_VERSION_MAJOR ${Qt5Core_VERSION_MAJOR})
    SET(QT_VERSION_MINOR ${Qt5Core_VERSION_MINOR})
    SET(QT_VERSION_PATCH ${Qt5Core_VERSION_PATCH})
    SET(QT_VERSION ${Qt5Core_VERSION})
    GET_FILENAME_COMPONENT(QT_BINARY_DIR ${QT_MOC_EXECUTABLE} DIRECTORY)
    SET(ep_qt_args
      -DQt5_DIR:PATH=${Qt5_DIR}
      -DQt5Widgets_DIR:PATH=${Qt5Widgets_DIR}
      -DQt5Designer_DIR:PATH=${Qt5Designer_DIR}
      -DQt5OpenGL_DIR:PATH=${Qt5OpenGL_DIR}
      -DQt5Sql_DIR:PATH=${Qt5Sql_DIR}
      )
    # WebKitWidgets is deprecated in Qt 5.3 and removed in 5.6.0
    find_package(Qt5 COMPONENTS WebKitWidgets QUIET)
    if(Qt5WebKitWidgets_FOUND)
      SET(ep_qt_args ${ep_qt_args}
        -DQt5WebKitWidgets_DIR:PATH=${Qt5WebKitWidgets_DIR}
        )
    endif()
  ENDIF()
ENDIF()

#-----------------------------------------------------------------------------
# Specify common external project properties
#-----------------------------------------------------------------------------
INCLUDE(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

SET(ep_base "${CMAKE_BINARY_DIR}")
SET(ep_common_args)

IF(UNIX AND NOT APPLE)
  EXECUTE_PROCESS(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                  OUTPUT_VARIABLE GCC_VERSION)
  STRING(REGEX MATCHALL "[0-9]+" GCC_VERSION_COMPONENTS ${GCC_VERSION})
  LIST(GET GCC_VERSION_COMPONENTS 0 GCC_MAJOR)
  LIST(GET GCC_VERSION_COMPONENTS 1 GCC_MINOR)

  # Allow inline namespacing to enable Qt::Orientation:: etc..
  IF(${GCC_VERSION} VERSION_GREATER 4.6)
    SET(ADDITIONAL_CXX_FLAGS "${ADDITIONAL_CXX_FLAGS} -std=c++11" CACHE STRING "Additional CXX Flags")
  ELSEIF(${GCC_VERSION} VERSION_GREATER 4.3)
    SET(ADDITIONAL_CXX_FLAGS "${ADDITIONAL_CXX_FLAGS} -std=c++0x" CACHE STRING "Additional CXX Flags")
  ELSE()
    MESSAGE(FATAL_ERROR "gcc >= 4.3 required to build RobartsVTK. gcc >= 4.7 recommended.")
  ENDIF()
ENDIF()
MARK_AS_ADVANCED(ADDITIONAL_CXX_FLAGS)
MARK_AS_ADVANCED(ADDITIONAL_C_FLAGS)

IF( UNIX AND CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
  SET(ADDITIONAL_CXX_FLAGS "${ADDITIONAL_CXX_FLAGS} -fPIC")
  SET(ADDITIONAL_C_FLAGS "${ADDITIONAL_C_FLAGS} -fPIC")
ENDIF()

IF(NOT ${CMAKE_GENERATOR} MATCHES "Visual Studio" AND NOT ${CMAKE_GENERATOR} MATCHES "Visual Studio")
  SET(ep_common_args
    ${ep_common_args}
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    )
ENDIF()

SET(ep_common_c_flags "${CMAKE_C_FLAGS_INIT} ${ADDITIONAL_C_FLAGS}")
SET(ep_common_cxx_flags "${CMAKE_CXX_FLAGS_INIT} ${ADDITIONAL_CXX_FLAGS}")

# Compute -G arg for configuring external projects with the same CMake generator:
IF(CMAKE_EXTRA_GENERATOR)
  SET(gen "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
ELSE()
  SET(gen "${CMAKE_GENERATOR}")
ENDIF()

#------------------------------------------------------------------------------
# Specify external projects
#------------------------------------------------------------------------------

IF ( PLUSBUILD_USE_3DSlicer )

  SET(PLUSBUILD_SLICER_BIN_DIRECTORY "PLUSBUILD_SLICER_BIN_DIRECTORY-NOT-FOUND" CACHE PATH "Path to 3D Slicer binary directory" )

  IF("${PLUSBUILD_SLICER_BIN_DIRECTORY}" STREQUAL "PLUSBUILD_SLICER_BIN_DIRECTORY-NOT-FOUND")
    MESSAGE(FATAL_ERROR "Cannot use 3D Slicer if PLUSBUILD_SLICER_BIN_DIRECTORY is not defined.")
  ENDIF()

  # The Slicer4 config file complains if these are set.
  UNSET (QT_QMAKE_EXECUTABLE CACHE)
  UNSET (QT_BINARY_DIR CACHE)
  UNSET (VTK_DIR CACHE)
  UNSET (ITK_DIR CACHE)
  UNSET (OpenIGTLink_DIR CACHE)

  ## Try to find Slicer4
  FIND_PACKAGE( Slicer PATHS ${PLUSBUILD_SLICER_BIN_DIRECTORY} NO_DEFAULT_PATH QUIET )
  IF ( Slicer_FOUND )
    SET(Slicer_SKIP_EXTENSION_NAME_CHECK TRUE)
    INCLUDE( ${Slicer_USE_FILE} )
    IF(PLUSBUILD_USE_OpenIGTLink)
      FIND_PACKAGE(OpenIGTLink REQUIRED PATHS "${Slicer_BINARY_DIR}/../OpenIGTLink-build" NO_DEFAULT_PATH )
    ENDIF()
  ENDIF()

  IF ( NOT Slicer_FOUND )
    MESSAGE( FATAL_ERROR "Unable to find Slicer at ${PLUSBUILD_SLICER_BIN_DIRECTORY} directory. Please verify configuration")
  ENDIF()

  # Qt might have been found earlier, run find_package to reset all the QT_... CMake variables
  FIND_PACKAGE(Qt4)
ENDIF()

INCLUDE(External_VTK.cmake)
INCLUDE(External_ITK.cmake)
IF(PLUSBUILD_USE_OpenIGTLink)
  INCLUDE(External_OpenIGTLink.cmake)
ENDIF(PLUSBUILD_USE_OpenIGTLink)

IF ( PLUS_USE_BKPROFOCUS_VIDEO )
  INCLUDE(External_GrabbieLib.cmake)
ENDIF()

IF ( PLUS_USE_OPTITRACK )
  INCLUDE(External_BiiGOptitrack.cmake)
ENDIF()

IF ( PLUS_USE_OPTIMET_CONOPROBE )
  FIND_PACKAGE (OPTIMETSMART32SDK)
  IF ( NOT OPTIMETSMART32SDK_FOUND)
    MESSAGE( FATAL_ERROR "This project requires the Optimet Smart 32 SDK for ConoProbe tracking. One of the components is missing. Please verify configuration or turn off PLUS_USE_OPTIMET_CONOPROBE.")
  ENDIF()
ENDIF()

IF ( PLUS_USE_NDI_CERTUS )
  FIND_PACKAGE (NDIOAPI)
  IF ( NOT NDIOAPI_FOUND)
    MESSAGE( FATAL_ERROR "This project requires NDI Oapi for CERTUS tracking. One of the components is missing. Please verify configuration or turn off PLUS_USE_CERTUS.")
  ENDIF()
ENDIF()

IF ( PLUS_USE_ULTRASONIX_VIDEO )
  FIND_PACKAGE (ULTRASONIX_SDK)
  IF ( NOT ULTRASONIX_SDK_FOUND)
    MESSAGE( FATAL_ERROR "This project requires Ultrasonix SDK ${ULTRASONIX_SDK_VERSION} for Ultrasonix video. One of the components is missing. Please verify configuration or turn off PLUS_USE_ULTRASONIX_VIDEO.")
  ENDIF()
ENDIF()

IF ( PLUS_USE_MICRONTRACKER )
  FIND_PACKAGE (MicronTracker)
  IF ( NOT MICRONTRACKER_FOUND)
    MESSAGE( FATAL_ERROR "This project requires Claron MicronTracker SDK for supporting the MicronTracker tracking device. One of the components is missing. Please verify configuration or turn off PLUS_USE_MICRONTRACKER.")
  ENDIF()
ENDIF()

IF ( PLUS_USE_INTELREALSENSE )
  FIND_PACKAGE (RSSDK)
  IF ( NOT RSSDK_FOUND)
    MESSAGE( FATAL_ERROR "This project requires Intel RealSense SDK for supporting the Intel RealSense device. One of the components is missing. Please verify configuration or turn off PLUS_USE_INTELREALSENSE.")
  ENDIF()
ENDIF()

IF ( PLUS_USE_ICCAPTURING_VIDEO )
 FIND_PACKAGE (ICCAPTURING)
  IF ( NOT ICCAPTURING_FOUND)
    MESSAGE( FATAL_ERROR "This project requires IC Capturing SDK for supporting the Imaging Source USB frame grabber. One of the components is missing. Please verify configuration or turn off PLUS_USE_ICCAPTURING_VIDEO.")
  ENDIF()
ENDIF()

IF ( PLUS_USE_STEALTHLINK )
  FIND_PACKAGE (STEALTHLINK)
  IF ( NOT STEALTHLINK_FOUND)
    MESSAGE( FATAL_ERROR "This project requires Stealthlink2 SDK for supporting communication with Medtronic StealthStation. Please verify configuration or turn off PLUS_USE_STEALTHLINK.")
  ENDIF()
ENDIF()

IF ( PLUS_USE_CAPISTRANO_VIDEO )
 FIND_PACKAGE (CAPISTRANO)
  IF ( NOT CAPISTRANO_FOUND)
    MESSAGE( FATAL_ERROR "This project requires Capistrano Labs cSDK for supporting the Capistrano Labs USB ultrasound probes. One of the components is missing. Please verify configuration or turn off PLUS_USE_CAPISTRANO_VIDEO.")
  ENDIF()
ENDIF()

IF ( PLUS_USE_INTERSON_VIDEO )
 FIND_PACKAGE (INTERSON)
  IF ( NOT INTERSON_FOUND)
    MESSAGE( FATAL_ERROR "This project requires Interson iSDK for supporting the Interson USB ultrasound probes. One of the components is missing. Please verify configuration or turn off PLUS_USE_INTERSON_VIDEO.")
  ENDIF()
ENDIF()

IF( PLUS_USE_IntuitiveDaVinci)
  FIND_PACKAGE (IntuitiveDaVinci)
  IF( NOT IntuitiveDaVinci_FOUND )
    MESSAGE( FATAL_ERROR "This project requires headers and library provided by Intuitive. One of the components is missing. Please verify configuration or turn off PLUS_USE_IntuitiveDaVinci.")
  ENDIF()
ENDIF()

IF ( PLUS_USE_TELEMED_VIDEO )
  FIND_PACKAGE (Telemed)
  IF ( NOT TELEMED_FOUND )
    MESSAGE( FATAL_ERROR "This project requires Telemed SDK for supporting the Telemed ultrasound probes. One of the components is missing. Please verify configuration or turn off PLUS_USE_TELEMED_VIDEO.")
  ENDIF()
ENDIF()

IF ( PLUS_USE_THORLABS_VIDEO )
  FIND_PACKAGE (ThorLabs)
  IF ( NOT THORLABS_FOUND )
    MESSAGE( FATAL_ERROR "This project requires ThorLabs CCS VISA  SDK for supporting the ThorLabs devices. One of the components is missing. Please verify configuration or turn off PLUS_USE_THORLABS_VIDEO.")
  ENDIF()
ENDIF()

IF (PLUS_USE_PHILIPS_3D_ULTRASOUND)
  IF(NOT CMAKE_HOST_WIN32)
    # Philips is windows only
    MESSAGE( FATAL_ERROR "Philips SDK is only available for Windows.")
  ENDIF(NOT CMAKE_HOST_WIN32)
  IF(PLUS_USE_ULTRASONIX_VIDEO)
    # Ultrasonix contains its own libmmd.dll which does not support all the functionality needed for the philips probe
    # They both cannot be enabled at the same time.
    MESSAGE( FATAL_ERROR "Plus cannot enable both Ultrasonix and Philips devices due to .dll conflicts in their respective SDK packages.")
  ELSE()
    SET (PLUS_Philips_MAJOR_VERSION 1 CACHE STRING "Set Philips library major version (version: [major].[minor].[patch])")
    SET (PLUS_Philips_MINOR_VERSION 0 CACHE STRING "Set Philips library minor version (version: [major].[minor].[patch])")
    SET (PLUS_Philips_PATCH_VERSION 0 CACHE STRING "Set Philips library patch version (version: [major].[minor].[patch])")
    FIND_PACKAGE(Philips)
    IF( NOT PHILIPS_FOUND )
      MESSAGE( FATAL_ERROR "In order to use the Philips ie33 ultrasound system, the requisite DLLs must be made available. Please verify configuration or turn off PLUS_USE_PHILIPS_3D_ULTRASOUND.")
    ENDIF()
      MESSAGE( STATUS "To use the Philips devices you must:" )
      MESSAGE( STATUS "1. Register '${CMAKE_CURRENT_BINARY_DIR}/bin/Debug|Release/Stream3d.dll'. If you are using a 32-bit OS, this can be done by running 'regsvr32 Stream3d.dll' in command line. If you are using a 64-bit OS, this can be done by the following procedure. Open the command line in administrator mode (Right click on cmd, choose 'Run as administrator'). Go to directory 'C:/Windows/SysWow64'. Run the regsvr32 command in SysWow64." )
  ENDIF()
ENDIF()

#------------------------------------------------------------------------------
# Specify target dependencies
#------------------------------------------------------------------------------
SET(PlusLib_DEPENDENCIES)

IF ( NOT VTK_DIR )
  # VTK_DIR is not supplied, so it is built inside Plus, therefore we need to specify dependencies to make sure it is built early enough
  SET(VTK_DEPENDENCIES)
  LIST(APPEND PlusLib_DEPENDENCIES vtk)
ENDIF()

IF ( NOT ITK_DIR )
  # ITK_DIR is not supplied, so it is built inside Plus, therefore we need to specify dependencies to make sure it is built early enough
  SET(ITK_DEPENDENCIES)
  LIST(APPEND PlusLib_DEPENDENCIES itk)
ENDIF()

IF(PLUSBUILD_USE_OpenIGTLink AND NOT OpenIGTLink_DIR)
  # OpenIGTLink_DIR is not supplied, so it is built inside Plus, therefore we need to specify dependencies to make sure it is built early enough
  SET(OpenIGTLink_DEPENDENCIES)
  LIST(APPEND PlusLib_DEPENDENCIES OpenIGTLink)
ENDIF()

IF ( PLUS_USE_BKPROFOCUS_VIDEO )
  SET(GrabbieLib_DEPENDENCIES)
  LIST(APPEND PlusLib_DEPENDENCIES GrabbieLib)
ENDIF()

IF( PLUS_USE_INTERSONSDKCXX_VIDEO AND NOT IntersonSDKCxx_DIR )
  SET(IntersonSDKCxx_DEPENDENCIES)
  LIST(APPEND PlusLib_DEPENDENCIES IntersonSDKCxx)
  INCLUDE(External_IntersonSDKCxx.cmake)
ENDIF()

IF ( PLUS_USE_OPTITRACK AND PLUS_BiiGOptitrack_DIR AND NOT BiiGOptitrack_DIR )
  SET(BiiGOptitrack_DEPENDENCIES itk OpenIGTLink)
  LIST(APPEND PlusLib_DEPENDENCIES BiiGOptitrack)
ENDIF()

SET(PlusApp_DEPENDENCIES PlusLib)
SET(PlusModelCatalog_DEPENDENCIES PlusLib)

IF(PLUSBUILD_USE_Tesseract)
  INCLUDE(External_Tesseract.cmake)
  LIST(APPEND PlusLib_DEPENDENCIES tesseract)
ENDIF(PLUSBUILD_USE_Tesseract)

#------------------------------------------------------------------------------
# Construct a descriptive build name
#------------------------------------------------------------------------------
SET(BUILD_ARCHITECTURE "x64")

IF( UNIX AND NOT APPLE )
  SET(LINUX TRUE)
ENDIF()

IF( ${VTK_GIT_TAG} MATCHES "v7" )
  SET(VTK_BUILD_NAME "vtk7")
ELSE()
  IF( PLUSBUILD_USE_VTK6 )
    SET(VTK_BUILD_NAME "vtk6")
  ELSE( PLUSBUILD_USE_VTK6 )
    SET(VTK_BUILD_NAME "vtk5")
  ENDIF(PLUSBUILD_USE_VTK6)
ENDIF()

IF( PLUSBUILD_BUILD_PLUSAPP )
  IF( Qt5Core_FOUND )
    SET(QT_BUILD_NAME "qt5")
  ELSEIF( QT4_FOUND )
    SET(QT_BUILD_NAME "qt4")
  ENDIF( Qt5Core_FOUND )
ENDIF( PLUSBUILD_BUILD_PLUSAPP )

#Build a nice system and compiler name
IF( LINUX )
  # System name
  IF( EXISTS /etc/issue.net )
    EXECUTE_PROCESS(COMMAND cat /etc/issue.net OUTPUT_VARIABLE SYSTEM_NAME_FULL)
    STRING(REGEX REPLACE "[ \n\t\r]" "" SYSTEM_NAME ${SYSTEM_NAME_FULL})
  ELSEIF( EXISTS /etc/issue )
    EXECUTE_PROCESS(COMMAND cat /etc/issue OUTPUT_VARIABLE SYSTEM_NAME_FULL)
    STRING(REGEX REPLACE "[ \n\t\r]" "" SYSTEM_NAME ${SYSTEM_NAME_FULL})
  ELSEIF( EXISTS /etc/redhat-release )
    EXECUTE_PROCESS(COMMAND cat /etc/redhat-release OUTPUT_VARIABLE SYSTEM_NAME_FULL)
    STRING(REGEX REPLACE "[ \n\t\r]" "" SYSTEM_NAME ${SYSTEM_NAME_FULL})
  ELSE()
    SET(SYSTEM_NAME ${CMAKE_SYSTEM_NAME})
  ENDIF()
  STRING(TOLOWER ${SYSTEM_NAME} SYSTEM_NAME)

  # Compiler name
  EXECUTE_PROCESS(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
  STRING(REGEX REPLACE "[ \n\t\r]" "" GCC_VERSION ${GCC_VERSION})
  SET(COMPILER_NAME "gcc-${GCC_VERSION}")
ELSEIF( APPLE )
  EXECUTE_PROCESS(COMMAND sw_vers -productName OUTPUT_VARIABLE SYSTEM_NAME_FULL)
  STRING(REGEX REPLACE "[ \n\t\r]" "" SYSTEM_NAME ${SYSTEM_NAME_FULL})
  EXECUTE_PROCESS(COMMAND sw_vers -productVersion OUTPUT_VARIABLE SYSTEM_VERSION_FULL)
  STRING(REGEX REPLACE "[ \n\t\r]" "" SYSTEM_VERSION ${SYSTEM_VERSION_FULL})
  SET(SYSTEM_NAME "${SYSTEM_NAME}-${SYSTEM_VERSION}")
  EXECUTE_PROCESS(COMMAND clang --version OUTPUT_VARIABLE CLANG_VERSION)
  STRING(REGEX MATCH "Apple LLVM version ([0-9]+.[0-9]+.[0-9]+)" CLANG_VERSION ${CLANG_VERSION})
  SET(COMPILER_NAME "clang${CMAKE_MATCH_1}")
ELSEIF( MSVC )
  # Use generator to determine name
  IF( ${CMAKE_GENERATOR} MATCHES "Visual Studio 9" )
    SET(COMPILER_NAME vs9)
  ELSEIF( ${CMAKE_GENERATOR} MATCHES "Visual Studio 10" )
    SET(COMPILER_NAME vs10)
  ELSEIF( ${CMAKE_GENERATOR} MATCHES "Visual Studio 11" )
    SET(COMPILER_NAME vs11)
  ELSEIF( ${CMAKE_GENERATOR} MATCHES "Visual Studio 12" )
    SET(COMPILER_NAME vs12)
  ELSEIF( ${CMAKE_GENERATOR} MATCHES "Visual Studio 14" )
    SET(COMPILER_NAME vs14)
  ELSE()
    SET(COMPILER_NAME vs??)
  ENDIF()
ELSEIF( MINGW )
  # Compiler name
  EXECUTE_PROCESS(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
  STRING(REGEX REPLACE "[ \n\t\r]" "" GCC_VERSION ${GCC_VERSION})
  SET(COMPILER_NAME "mingwgcc-${GCC_VERSION}")
ENDIF()

IF( WIN32 )
  # Build nice windows system name
  EXECUTE_PROCESS(COMMAND systeminfo OUTPUT_VARIABLE SYSTEM_NAME_FULL)
  IF ("${SYSTEM_NAME_FULL}" STREQUAL "")
    # On WindowsXP systeminfo does not exist, command output is empty
    SET(SYSTEM_NAME "win")
  ELSE()
    STRING(REGEX MATCH "OS Name: *Microsoft Windows ([0-9]+)" WIN_VERSION ${SYSTEM_NAME_FULL})
    SET(SYSTEM_NAME "win${CMAKE_MATCH_1}")
  ENDIF()
  IF( NOT ${CMAKE_GENERATOR} MATCHES "x64" )
    SET(BUILD_ARCHITECTURE "x86")
  ENDIF()
ENDIF()

SET(PLUSBUILD_BUILDNAME_POSTFIX "" CACHE STRING "Postfix that will be appended to the build name on the dashboard. If defined then BUILDNAME variable is always overwritten.")
MARK_AS_ADVANCED(PLUSBUILD_BUILDNAME_POSTFIX)
SET(GENERATED_BUILDNAME "${COMPILER_NAME}${BUILD_ARCHITECTURE}-${SYSTEM_NAME}-${VTK_BUILD_NAME}-${QT_BUILD_NAME}")

IF ("${PLUSBUILD_BUILDNAME_POSTFIX}" STREQUAL "")
  SET(BUILDNAME "${GENERATED_BUILDNAME}" CACHE STRING "Name of the build on the dashboard")
ELSE()
  SET(BUILDNAME "${GENERATED_BUILDNAME}-${PLUSBUILD_BUILDNAME_POSTFIX}" CACHE STRING "Name of the build on the dashboard. Value is generated automatically from build options and PLUSBUILD_BUILDNAME_POSTFIX." FORCE)
ENDIF()
MARK_AS_ADVANCED(BUILDNAME)

#------------------------------------------------------------------------------
# Set up project for PlusLib, PlusApp, and PlusModelCatalog
#------------------------------------------------------------------------------
INCLUDE(CTest)

IF(PLUSBUILD_DOWNLOAD_PlusDATA AND NOT PLUSBUILD_OFFLINE_BUILD)
  INCLUDE(External_PlusLibData.cmake)
  LIST(APPEND PlusLib_DEPENDENCIES PlusLibData)
ENDIF()

INCLUDE(External_PlusLib.cmake)

IF(PLUSBUILD_BUILD_PLUSAPP)
  INCLUDE(External_PlusApp.cmake)
ENDIF(PLUSBUILD_BUILD_PLUSAPP)

IF(PLUSBUILD_BUILD_PLUSMODELCATALOG)
  INCLUDE(External_PlusModelCatalog.cmake)
ENDIF(PLUSBUILD_BUILD_PLUSMODELCATALOG)

#-----------------------------------------------------------------------------
# Generate convenience files for automatic update build and test
#
IF(WIN32)
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/BuildAndTest.bat.in
    ${CMAKE_CURRENT_BINARY_DIR}/BuildAndTest.bat
    )
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/BuildAndTest.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/BuildAndTest.sh
    )

  STRING(REPLACE "/" "\\" PLUSBUILD_BIN_DIR_WIN "${CMAKE_BINARY_DIR}")
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/PlusBuildAndTest.bat.in
    ${CMAKE_CURRENT_BINARY_DIR}/PlusBuildAndTest.bat
    )
ELSE()
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/PlusBuildAndTest.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/PlusBuildAndTest.sh
    )
    file(COPY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/PlusBuildAndTest.sh
      DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
    FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ WORLD_READ)
ENDIF()
