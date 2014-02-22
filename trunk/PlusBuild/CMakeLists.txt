PROJECT(PlusBuild)
#
# Configure the build to work (although with limited functionalities) if only
# src directory of the repository is available
# 
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
IF(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

SET(CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}/Modules
  ${CMAKE_MODULE_PATH}
  )
 
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

#-----------------------------------------------------------------------------
# Options to control build process
#-----------------------------------------------------------------------------

# Determine the operating system to set default values accordingly
SET (ENABLED_BY_DEFAULT_ON_WINDOWS_ONLY OFF)
SET (ENABLED_BY_DEFAULT_ON_WINDOWS32_ONLY OFF)
if (CMAKE_HOST_WIN32)	
	SET (ENABLED_BY_DEFAULT_ON_WINDOWS_ONLY ON)
  if (NOT CMAKE_CL_64)
    SET (ENABLED_BY_DEFAULT_ON_WINDOWS32_ONLY ON)
  endif ()
endif (CMAKE_HOST_WIN32)	  

OPTION(PLUSBUILD_USE_3DSlicer "Use 3D Slicer and it's external projects (like ITK, VTK, OpenIGTLink)" OFF)
OPTION(PLUSBUILD_USE_OpenIGTLink "Use OpenIGTLink" ON)
OPTION(PLUSBUILD_BUILD_SHARED_LIBS "Build shared libs" ON)
OPTION(PLUSBUILD_BUILD_PLUSAPP "Build PlusApp applications" ON)

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
SET (PLUS_ULTRASONIX_SDK_PATCH_VERSION 1 CACHE STRING "Set Ultrasonix SDK patch version (version: [major].[minor].[patch])")
OPTION(PLUS_TEST_ULTRASONIX "Enable testing of acquisition from Ultrasonix ultrasound systems. Enable this only if an Ultrasonix device accessible from this computer. " OFF)  
IF (PLUS_TEST_ULTRASONIX)
  SET (PLUS_TEST_ULTRASONIX_IP_ADDRESS "130.15.7.24" CACHE STRING "IP address of the Ultrasonix scanner that is used during testing")
ENDIF()

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
OPTION(PLUS_USE_EPIPHAN "Provide support for the Epiphan framegrabber device" OFF)
OPTION(PLUS_USE_INTERSON_VIDEO "Provide support for the Interson USB ultrasound probes" OFF)

# Tracking hardware

OPTION(PLUS_USE_POLARIS "Provide support for the NDI POLARIS and AURORA" OFF)
OPTION(PLUS_USE_CERTUS "Provide support for the NDI Certus" OFF)
OPTION(PLUS_USE_MICRONTRACKER "Provide support for the Claron MicronTracker" OFF)
OPTION(PLUS_USE_BRACHY_TRACKER "Provide support for the Brachy Steppers" ${ENABLED_BY_DEFAULT_ON_WINDOWS32_ONLY})
OPTION(PLUS_USE_Ascension3DG "Provide support for the Ascension 3DG Tracker" ${ENABLED_BY_DEFAULT_ON_WINDOWS32_ONLY})
OPTION(PLUS_USE_Ascension3DGm "Provide support for the Ascension MedSafe Tracker" OFF)
OPTION(PLUS_USE_PHIDGET_SPATIAL_TRACKER "Provide support for the Phidget Spatial accelerometer" OFF)
OPTION(PLUS_USE_3dConnexion_TRACKER "Provide support for the 3dConnexion 3d mouse" OFF)
OPTION(PLUS_USE_StealthLink2 "Provide support for the Medtronick StealthLink Server" OFF)

# Other

OPTION(PLUS_USE_HEARTSIGNALBOX "Enable Heart Signal Box" OFF)
OPTION(PLUS_USE_USBECGBOX "Enable USB ECG Signal Box" OFF) 

OPTION (PLUSAPP_TEST_GUI "Enable Sikuli GUI tests" ON)

#-----------------------------------------------------------------------------
# Warnings for incompatible build options
#-----------------------------------------------------------------------------
IF (PLUS_USE_Ascension3DG AND PLUS_USE_Ascension3DGm)
  MESSAGE(FATAL_ERROR "PLUS_USE_Ascension3DGm and PLUS_USE_Ascension3DGm options cannot be enabled at the same time. See more details at https://www.assembla.com/spaces/plus/tickets/851")
ENDIF()

#-----------------------------------------------------------------------------
# Warnings for work in progress features
#-----------------------------------------------------------------------------
IF (PLUS_USE_USBECGBOX)
  MESSAGE(WARNING "PLUS_USE_USBECGBOX option is enabled, which may cause build errors if the necessary Universal Library is not installed. This is an experimental feature for supporting ECG signal acquisition through an USB digitizer, see more details at https://www.assembla.com/spaces/plus/tickets/396")
ENDIF()

IF (PLUS_USE_HEARTSIGNALBOX)
  MESSAGE(WARNING "PLUS_USE_HEARTSIGNALBOX option is enabled, which may cause build errors if the necessary library from http://www.logix4u.net/ is not installed. This is an experimental feature, see more details at https://www.assembla.com/spaces/plus/tickets/396")
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
IF(PLUSBUILD_BUILD_PLUSAPP AND NOT PLUSBUILD_USE_3DSlicer) 
  FIND_FILE(QT_QMAKE_EXECUTABLE qmake${CMAKE_EXECUTABLE_SUFFIX} 
    PATH
      $ENV{QTDIR}
      "C:/Qt/4.7.4/bin"
      "../Qt/4.7.4/bin"
      "../../Qt/4.7.4/bin"
    )
  FIND_PACKAGE(Qt4)
  IF ( NOT QT4_FOUND)
      MESSAGE( FATAL_ERROR "This project requires Qt4 for building PlusApp. One of these components is missing. Please verify configuration by selecting QT_QMAKE_EXECUTABLE or turn off PLUSBUILD_BUILD_PLUSAPP option.")
  ENDIF()  
  
  SET(QT_USE_QT3SUPPORT FALSE) 
  INCLUDE(${QT_USE_FILE})
ENDIF(PLUSBUILD_BUILD_PLUSAPP AND NOT PLUSBUILD_USE_3DSlicer) 

#-----------------------------------------------------------------------------
# Specify common external project properties
#-----------------------------------------------------------------------------
INCLUDE(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
INCLUDE(CTest)

SET(ep_base "${CMAKE_BINARY_DIR}")

SET(ep_common_args
  #-DCMAKE_INSTALL_PREFIX:PATH=${ep_install_dir}
  -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
  -DBUILD_TESTING:BOOL=OFF
  )

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
    ENDIF(PLUSBUILD_USE_OpenIGTLink)      
  ENDIF( Slicer_FOUND )
  
  ## Try to find Slicer3
  FIND_PACKAGE( Slicer3 PATHS ${PLUSBUILD_SLICER_BIN_DIRECTORY} NO_DEFAULT_PATH QUIET )
  IF ( Slicer3_FOUND )
    INCLUDE( ${Slicer3_USE_FILE} )
    IF(PLUSBUILD_USE_OpenIGTLink)
      FIND_PACKAGE(OpenIGTLink REQUIRED PATHS "${Slicer3_HOME}/../Slicer3-lib/OpenIGTLink-build" NO_DEFAULT_PATH )
    ENDIF(PLUSBUILD_USE_OpenIGTLink)
  ENDIF( Slicer3_FOUND )
  
  IF ( NOT Slicer_FOUND AND NOT Slicer3_FOUND)
    MESSAGE( FATAL_ERROR "Unable to find Slicer at ${PLUSBUILD_SLICER_BIN_DIRECTORY} directory. Please verify configuration")
  ENDIF()
  
  # Qt might have been found earlier, run find_package to reset all the QT_... CMake variables
  FIND_PACKAGE(Qt4)
  
  IF(PLUSBUILD_BUILD_PLUSAPP) 
    IF (NOT VTK_USE_QT)
      MESSAGE( SEND_ERROR "You have to build VTK with VTK_USE_QT flag ON if you need to use PLUSBUILD_BUILD_PLUSAPP.")
    ENDIF (NOT VTK_USE_QT)
  ENDIF(PLUSBUILD_BUILD_PLUSAPP)    
  
ENDIF ( PLUSBUILD_USE_3DSlicer )

INCLUDE(External_VTK.cmake)
INCLUDE(External_ITK.cmake)
IF(PLUSBUILD_USE_OpenIGTLink)
  INCLUDE(External_OpenIGTLink.cmake)
ENDIF(PLUSBUILD_USE_OpenIGTLink)
  
IF ( PLUS_USE_BKPROFOCUS_VIDEO )
  INCLUDE(External_GrabbieLib.cmake)
ENDIF()

IF ( PLUS_USE_CERTUS )
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

IF ( PLUS_USE_ICCAPTURING_VIDEO )
 FIND_PACKAGE (ICCAPTURING)
  IF ( NOT ICCAPTURING_FOUND)
    MESSAGE( FATAL_ERROR "This project requires IC Capturing SDK for supporting the Imaging Source USB frame grabber. One of the components is missing. Please verify configuration or turn off PLUS_USE_ICCAPTURING_VIDEO.")
  ENDIF()
ENDIF()

<<<<<<< .mine
IF ( PLUS_USE_StealthLink2 )
 FIND_PACKAGE (Stealthlink2)
  IF ( NOT Stealthlink2_FOUND)
    MESSAGE( FATAL_ERROR "This project requires Staelthlink2 SDK for supporting communication with Medtronic StealthStation. Please verify configuration or turn off PLUS_USE_StealthLink2.")
  ENDIF()
ENDIF()


=======
IF ( PLUS_USE_INTERSON_VIDEO )
 FIND_PACKAGE (INTERSON)
  IF ( NOT INTERSON_FOUND)
    MESSAGE( FATAL_ERROR "This project requires Interson iSDK for supporting the Interson USB ultrasound probes. One of the components is missing. Please verify configuration or turn off PLUS_USE_INTERSON_VIDEO.")
  ENDIF()
ENDIF()

>>>>>>> .r3182
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

SET(PlusApp_DEPENDENCIES PlusLib)

#------------------------------------------------------------------------------
# Set up project for PlusLib and PlusApp
#------------------------------------------------------------------------------

INCLUDE(External_PlusLib.cmake)

IF(PLUSBUILD_BUILD_PLUSAPP) 
  INCLUDE(External_PlusApp.cmake)
ENDIF(PLUSBUILD_BUILD_PLUSAPP)    

#-----------------------------------------------------------------------------
# Generate convenience files for automatic update build and test
# 
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
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/PlusBuildAndTest.sh.in 
        ${CMAKE_CURRENT_BINARY_DIR}/PlusBuildAndTest.sh
        )