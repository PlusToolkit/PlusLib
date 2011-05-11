PROJECT(PlusBuild)
#
# Configure the build to work (although with limited functionalities) if only
# src directory of the repository is available
# 
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
IF(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

#-----------------------------------------------------------------------------
# SVN - Let's check if a valid version of SVN is available
#-----------------------------------------------------------------------------
# Need subversion for the automatic update of the repository
MARK_AS_ADVANCED(CLEAR Subversion_SVN_EXECUTABLE)
FIND_FILE(Subversion_SVN_EXECUTABLE svn${CMAKE_EXECUTABLE_SUFFIX} 
  PATHS 
    "c:/Program Files/SlikSvn/bin/"
  )
FIND_PACKAGE(Subversion REQUIRED)

SET(PLUSBUILD_ASSEMBLA_USERNAME "PLUSBUILD_ASSEMBLA_USERNAME-NOT-FOUND" CACHE STRING "Username for Plus assembla server: https://subversion.assembla.com/svn/plus/" )
SET(PLUSBUILD_ASSEMBLA_PASSWORD "PLUSBUILD_ASSEMBLA_PASSWORD-NOT-FOUND" CACHE STRING "Password for Plus assembla server: https://subversion.assembla.com/svn/plus/" )

IF("${PLUSBUILD_ASSEMBLA_USERNAME}" STREQUAL "PLUSBUILD_ASSEMBLA_USERNAME-NOT-FOUND")
  MESSAGE(WARNING "Cannot download necessary files from the Plus repository if PLUSBUILD_ASSEMBLA_USERNAME is not defined.")
ENDIF() 

IF("${PLUSBUILD_ASSEMBLA_PASSWORD}" STREQUAL "PLUSBUILD_ASSEMBLA_PASSWORD-NOT-FOUND")
  MESSAGE(WARNING "Cannot download necessary files from the Plus repository if PLUSBUILD_ASSEMBLA_PASSWORD is not defined.")
ENDIF() 


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
# Qt - Let's check if a valid version of Qt is available
#-----------------------------------------------------------------------------
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

#-----------------------------------------------------------------------------
# Options to control build process
#-----------------------------------------------------------------------------
OPTION(PLUSBUILD_USE_PLTools "Use the PLTools repository" ON)
OPTION(PLUSBUILD_USE_OpenIGTLink "Use OpenIGTLink" ON)
OPTION(PLUSBUILD_BUILD_SHARED_LIBS "Build shared libs" ON)
OPTION(PLUSBUILD_BUILD_PLUSAPP "Build PlusApp applications" ON)

OPTION(PLUS_USE_SONIX_VIDEO "Provide support for the Sonix video source with the Ulterius SDK" ON)
OPTION(PLUS_USE_ICCAPTURING_VIDEO "Provide support for the IC framegrabber device" ON)
OPTION(PLUS_USE_VFW_VIDEO "Provide support for the Video-for-Windows video digitizer" OFF)

OPTION(PLUS_USE_POLARIS "Provide support for the NDI POLARIS and AURORA" OFF)
OPTION(PLUS_USE_CERTUS "Provide support for the NDI Certus" ON)
OPTION(PLUS_USE_FLOCK "Provide support for the Ascension Flock of Birds" OFF)
OPTION(PLUS_USE_MICRONTRACKER "Provide support for the Claron MicronTracker" OFF)
OPTION(PLUS_USE_AMS_TRACKER "Provide support for the AMS Stepper tracking device" ON)
OPTION(PLUS_USE_Ascension3DG "Provide support for the Ascension 3DG Tracker" OFF)

OPTION(PLUS_USE_HEARTSIGNALBOX "Enable Heart Signal Box" OFF)
OPTION(PLUS_USE_USBECGBOX "Enable USB ECG Signal Box" OFF)
SET (PLUS_ULTERIUS_MAJOR_VERSION 5 CACHE STRING "Set Ultrasonix Ulterius SDK major version")

#-----------------------------------------------------------------------------
# Enable and setup External project global properties
#-----------------------------------------------------------------------------
INCLUDE(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

SET(ep_base        "${CMAKE_BINARY_DIR}")

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

SET(VTK_DEPENDENCIES)

SET(ITK_DEPENDENCIES)

SET(PlusLib_DEPENDENCIES vtk itk)

IF(PLUSBUILD_USE_OpenIGTLink)
    SET(OpenIGTLink_DEPENDENCIES)
    LIST(APPEND PlusLib_DEPENDENCIES OpenIGTLink)
ENDIF(PLUSBUILD_USE_OpenIGTLink)

IF(PLUSBUILD_USE_PLTools)
    LIST(APPEND PlusLib_DEPENDENCIES PLTools)
ELSE()
    MESSAGE("TODO: Need to set third party libraries...")
ENDIF() 

SET(PlusApp_DEPENDENCIES PlusLib)

#------------------------------------------------------------------------------
# Conditionnaly include ExternalProject Target
#------------------------------------------------------------------------------

INCLUDE(External_VTK.cmake)

INCLUDE(External_ITK.cmake)

IF(PLUSBUILD_USE_OpenIGTLink)
    INCLUDE(External_OpenIGTLink.cmake)
ENDIF(PLUSBUILD_USE_OpenIGTLink)

IF(PLUSBUILD_USE_PLTools)
    INCLUDE(External_PLTools.cmake)
ENDIF(PLUSBUILD_USE_PLTools)    

INCLUDE(External_PlusLib.cmake)

IF(PLUSBUILD_BUILD_PLUSAPP) 
    INCLUDE(External_PlusApp.cmake)
ENDIF(PLUSBUILD_BUILD_PLUSAPP)    

