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
OPTION(PLUSBUILD_USE_3DSlicer "Use 3D Slicer and it's external projects (like ITK, VTK, OpenIGTLink)" OFF)
OPTION(PLUSBUILD_USE_OpenIGTLink "Use OpenIGTLink" ON)
OPTION(PLUSBUILD_BUILD_SHARED_LIBS "Build shared libs" ON)
OPTION(PLUSBUILD_BUILD_PLUSAPP "Build PlusApp applications" ON)

OPTION(PLUS_USE_ULTRASONIX_VIDEO "Provide support for the Ultrasonix ultrasound systems" OFF)
SET (PLUS_ULTRASONIX_SDK_MAJOR_VERSION 5 CACHE STRING "Set Ultrasonix SDK major version (version: [major].[minor].[patch])")
SET (PLUS_ULTRASONIX_SDK_MINOR_VERSION 7 CACHE STRING "Set Ultrasonix SDK minor version (version: [major].[minor].[patch])")
SET (PLUS_ULTRASONIX_SDK_PATCH_VERSION 1 CACHE STRING "Set Ultrasonix SDK patch version (version: [major].[minor].[patch])")

OPTION(PLUS_USE_ICCAPTURING_VIDEO "Provide support for the IC framegrabber device" ON)
OPTION(PLUS_USE_VFW_VIDEO "Provide support for the Video-for-Windows video digitizer" OFF)

OPTION(PLUS_USE_POLARIS "Provide support for the NDI POLARIS and AURORA" OFF)
OPTION(PLUS_USE_CERTUS "Provide support for the NDI Certus" OFF)
OPTION(PLUS_USE_MICRONTRACKER "Provide support for the Claron MicronTracker" OFF)
OPTION(PLUS_USE_BRACHY_TRACKER "Provide support for the Brachy Steppers" ON)
OPTION(PLUS_USE_Ascension3DG "Provide support for the Ascension 3DG Tracker" ON)

OPTION(PLUS_USE_HEARTSIGNALBOX "Enable Heart Signal Box" OFF)
#OPTION(PLUS_USE_USBECGBOX "Enable USB ECG Signal Box" OFF) #Not yet supported 

OPTION (PLUSAPP_TEST_GUI "Enable Sikuli GUI tests" ON)

#-----------------------------------------------------------------------------
# Warnings for work in progress features
#-----------------------------------------------------------------------------
IF (PLUS_USE_MICRONTRACKER)
  MESSAGE("Provide support for the Claron MicronTracker option was selected. This is a work in progress feature. See more details at https://www.assembla.com/spaces/plus/tickets/348")
ENDIF()
IF (PLUS_USE_VFW_VIDEO)
  MESSAGE("Provide support for Video-for-Windows video digitizer option was selected. This is a work in progress feature. See more details at https://www.assembla.com/spaces/plus/tickets/364")
ENDIF()
IF (PLUS_USE_POLARIS)
  MESSAGE("Provide support for the NDI POLARIS and AURORA option was selected. This is a work in progress feature. See more details at https://www.assembla.com/spaces/plus/tickets/336")
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
IF(PLUSBUILD_BUILD_PLUSAPP) 
    FIND_FILE(QT_QMAKE_EXECUTABLE qmake${CMAKE_EXECUTABLE_SUFFIX} 
      PATH
        $ENV{QTDIR}
        "C:/Qt/4.7.3/bin"
        "../Qt/4.7.3/bin"
        "../../Qt/4.7.3/bin"
      )
    FIND_PACKAGE(Qt4 REQUIRED)
    SET(QT_USE_QT3SUPPORT TRUE) 
    INCLUDE(${QT_USE_FILE})
ENDIF(PLUSBUILD_BUILD_PLUSAPP) 

#-----------------------------------------------------------------------------
# Enable and setup External project global properties
#-----------------------------------------------------------------------------
INCLUDE(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

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
# Establish Target Dependencies based on Selected Options
#------------------------------------------------------------------------------

SET(PlusLib_DEPENDENCIES)
IF ( NOT PLUSBUILD_USE_3DSlicer )
    SET(VTK_DEPENDENCIES)
    SET(ITK_DEPENDENCIES)
    LIST(APPEND PlusLib_DEPENDENCIES vtk itk)
    
    IF(PLUSBUILD_USE_OpenIGTLink)
        SET(OpenIGTLink_DEPENDENCIES)
        LIST(APPEND PlusLib_DEPENDENCIES OpenIGTLink)
    ENDIF(PLUSBUILD_USE_OpenIGTLink)

ENDIF ( NOT PLUSBUILD_USE_3DSlicer )

LIST(APPEND PlusLib_DEPENDENCIES )

SET(PlusApp_DEPENDENCIES PlusLib)

#------------------------------------------------------------------------------
# Conditionnaly include ExternalProject Target
#------------------------------------------------------------------------------

IF ( PLUSBUILD_USE_3DSlicer )

    SET(PLUSBUILD_SLICER_BIN_DIRECTORY "PLUSBUILD_SLICER_BIN_DIRECTORY-NOT-FOUND" CACHE PATH "Path to 3D Slicer binary directory" )

    IF("${PLUSBUILD_SLICER_BIN_DIRECTORY}" STREQUAL "PLUSBUILD_SLICER_BIN_DIRECTORY-NOT-FOUND")
        MESSAGE(FATAL_ERROR "Cannot use 3D Slicer if PLUSBUILD_SLICER_BIN_DIRECTORY is not defined.")
    ENDIF() 

    ## Try to find Slicer4
    FIND_PACKAGE( Slicer PATHS ${PLUSBUILD_SLICER_BIN_DIRECTORY} NO_DEFAULT_PATH QUIET )
    IF ( Slicer_FOUND )
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
    
    IF(PLUSBUILD_BUILD_PLUSAPP) 
        IF (NOT VTK_USE_QT)
            MESSAGE( SEND_ERROR "You have to build VTK with VTK_USE_QT flag ON if you need to use PLUSBUILD_BUILD_PLUSAPP.")
        ENDIF (NOT VTK_USE_QT)
    ENDIF(PLUSBUILD_BUILD_PLUSAPP)    
    
    # Copy libraries to PLUS_EXECUTABLE_OUTPUT_PATH
    IF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio" )
        FILE(COPY 
            ${VTK_LIBRARY_DIRS}/Release/
            ${ITK_LIBRARY_DIRS}/Release/
            ${OpenIGTLink_LIBRARY_DIRS}/Release/
            DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Release
            FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
            )
        FILE(COPY 
            ${VTK_LIBRARY_DIRS}/Debug/
            ${ITK_LIBRARY_DIRS}/Debug/
            ${OpenIGTLink_LIBRARY_DIRS}/Debug/
            DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Debug
            FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
            )    
    ELSE()
        FILE(COPY 
            ${VTK_LIBRARY_DIRS}/
            ${ITK_LIBRARY_DIRS}/
            ${OpenIGTLink_LIBRARY_DIRS}/
            DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}
            FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
            )
    ENDIF()

ELSE ( PLUSBUILD_USE_3DSlicer )
    INCLUDE(External_VTK.cmake)
    INCLUDE(External_ITK.cmake)
    
    IF(PLUSBUILD_USE_OpenIGTLink)
        INCLUDE(External_OpenIGTLink.cmake)
    ENDIF(PLUSBUILD_USE_OpenIGTLink)
ENDIF ( PLUSBUILD_USE_3DSlicer )

IF ( PLUS_USE_CERTUS )
    FIND_PACKAGE (NDIOAPI)
    IF ( NOT NDIOAPI_FOUND)
        MESSAGE( FATAL_ERROR "This project requires NDI Oapi for CERTUS tracking. One of these components is missing. Please verify configuration or turn off PLUS_USE_CERTUS.")
    ENDIF()
ENDIF() 

IF ( PLUS_USE_ULTRASONIX_VIDEO )
    FIND_PACKAGE (ULTRASONIX_SDK)
    IF ( NOT ULTRASONIX_SDK_FOUND)
        MESSAGE( FATAL_ERROR "This project requires Ultrasonix SDK ${ULTRASONIX_SDK_VERSION} for Ultrasonix video. One of these components is missing. Please verify configuration or turn off PLUS_USE_ULTRASONIX_VIDEO.")
    ENDIF()
ENDIF() 

IF ( PLUS_USE_MICRONTRACKER )
    FIND_PACKAGE (MicronTracker)
    IF ( NOT MICRONTRACKER_FOUND)
        MESSAGE( FATAL_ERROR "This project requires Claron MicronTracker SDK for supporting the MicronTracker tracking device. One of these components is missing. Please verify configuration or turn off PLUS_USE_MICRONTRACKER.")
    ENDIF()
ENDIF() 


INCLUDE(External_PlusLib.cmake)

IF(PLUSBUILD_BUILD_PLUSAPP) 
    INCLUDE(External_PlusApp.cmake)
ENDIF(PLUSBUILD_BUILD_PLUSAPP)    


