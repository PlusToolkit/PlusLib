PROJECT(PLUSAPP)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

# ------------------------------------------------------------------------
# Find Plus use file
FIND_PATH( PLUS_BUILD_DIR UsePlus.cmake)
IF ( PLUS_BUILD_DIR )
	INCLUDE (${PLUS_BUILD_DIR}/src/UsePlus.cmake)
ELSE ( PLUS_BUILD_DIR )
	MESSAGE(FATAL_ERROR "Please set Plus BUILD directory.")
ENDIF ( PLUS_BUILD_DIR )

SET(PLUSAPP_VERSION_MAJOR ${PLUS_VERSION_MAJOR})
SET(PLUSAPP_VERSION_MINOR ${PLUS_VERSION_MINOR})
SET(PLUSAPP_VERSION_PATCH ${PLUS_VERSION_PATCH})

MESSAGE(STATUS "PlusApp version: ${PLUSAPP_VERSION_MAJOR}.${PLUSAPP_VERSION_MINOR}.${PLUSAPP_VERSION_PATCH}" )

# --------------------------------------------------------------------------
# Find SVN for revision information status
#
find_package(Subversion)
if(Subversion_FOUND)
  Subversion_WC_INFO(${CMAKE_CURRENT_SOURCE_DIR} PLUSAPP_SVN_INFO)
  MESSAGE(STATUS "Current revision is ${PLUSAPP_SVN_INFO_WC_REVISION}") 
  set(PLUSAPP_REVISION ${PLUSAPP_SVN_INFO_WC_REVISION})
endif(Subversion_FOUND) 

# --------------------------------------------------------------------------
# Configure output paths for libraries and executables.
#
SET(LIBRARY_OUTPUT_PATH ${PLUSAPP_BINARY_DIR}/bin CACHE PATH
    "Single output directory for building all libraries.")
SET(EXECUTABLE_OUTPUT_PATH ${PLUSAPP_BINARY_DIR}/bin CACHE PATH
    "Single output directory for building all executables.")
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)

SET(CMAKE_INSTALL_PREFIX ${EXECUTABLE_OUTPUT_PATH} )

SET( INSTALL_DESTINATION_DIR "PlusApp_v${PLUSAPP_VERSION_MAJOR}.${PLUSAPP_VERSION_MINOR}.${PLUSAPP_VERSION_PATCH}" )

# --------------------------------------------------------------------------
# Packaging
#
INCLUDE (${CMAKE_CURRENT_SOURCE_DIR}/CPackConfig.cmake)

OPTION (PLUSAPP_BUILD_4DUltrasound "Build the 4DUltrasound application" ON)
OPTION (PLUSAPP_BUILD_VolumeReconstructor "Build the VolumeReconstructor application" ON)
OPTION (PLUSAPP_BUILD_DiagnosticTools "Build the DiagnosticTools" ON)
OPTION (PLUSAPP_BUILD_OpenIGTLinkBroadcaster "Build the OpenIGTLinkBroadcaster application" OFF)
OPTION (PLUSAPP_BUILD_SequenceMetaFileConverter "Build the SequenceMetaFileConverter application" ON)
OPTION (PLUSAPP_BUILD_TrackedUltrasoundCapturing "Build the TrackedUltrasoundCapturing application (Qt version)" ON)
OPTION (PLUSAPP_BUILD_FreehandCalibration "Build the Freehand Calibration application (Qt)" ON)


# --------------------------------------------------------------------------
# Subdirs
#

# --------------------------------------------------------------------------
# Build the VolumeReconstructor application
IF (PLUSAPP_BUILD_VolumeReconstructor)
    ADD_SUBDIRECTORY(VolumeReconstructor)
ENDIF (PLUSAPP_BUILD_VolumeReconstructor)

# --------------------------------------------------------------------------
# Build the SequenceMetaFileConverter application 
IF (PLUSAPP_BUILD_SequenceMetaFileConverter)
    ADD_SUBDIRECTORY(SequenceMetaFileConverter)
ENDIF (PLUSAPP_BUILD_SequenceMetaFileConverter)

# --------------------------------------------------------------------------
# Build the DiagnosticTools 
IF (PLUSAPP_BUILD_DiagnosticTools)
    ADD_SUBDIRECTORY(DiagnosticTools)
ENDIF (PLUSAPP_BUILD_DiagnosticTools)

# --------------------------------------------------------------------------
# Build the TrackedUltrasoundCapturing application (Qt version)
IF (PLUSAPP_BUILD_TrackedUltrasoundCapturing)
	ADD_SUBDIRECTORY(TrackedUltrasoundCapturing)
ENDIF (PLUSAPP_BUILD_TrackedUltrasoundCapturing)

# --------------------------------------------------------------------------
# Build the 4DUltrasound application
IF (PLUSAPP_BUILD_4DUltrasound)
	ADD_SUBDIRECTORY(4DUltrasound)
ENDIF (PLUSAPP_BUILD_4DUltrasound)

# --------------------------------------------------------------------------
# Build the Freehand Calibration application (Qt)
IF (PLUSAPP_BUILD_FreehandCalibration)
	ADD_SUBDIRECTORY(FreehandCalibration)
ENDIF (PLUSAPP_BUILD_FreehandCalibration)

IF(PLUSAPP_BUILD_OpenIGTLinkBroadcaster)
    IF (PLUS_USE_OpenIGTLink)
        ADD_SUBDIRECTORY(OpenIGTLinkBroadcaster)
    ELSE (PLUS_USE_OpenIGTLink)
        MESSAGE(SEND_ERROR "OpenIGTLinkBroadcaster: PLUS has to be configured with PLUS_USE_OpenIGTLink flag ON")
    ENDIF (PLUS_USE_OpenIGTLink)
ENDIF(PLUSAPP_BUILD_OpenIGTLinkBroadcaster)

#-----------------------------------------------------------------------------
# Generate convenience files for automatic build, test, and packaging
#
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/CreatePackage.bat.in
	${CMAKE_CURRENT_BINARY_DIR}/CreatePackage.bat
	)	
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/DevEnv_Debug.bat.in 
	${CMAKE_CURRENT_BINARY_DIR}/DevEnv_Debug.bat
	)
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/DevEnv_Release.bat.in 
	${CMAKE_CURRENT_BINARY_DIR}/DevEnv_Release.bat
	)
