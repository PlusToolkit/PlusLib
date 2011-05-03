PROJECT(PLUS)

cmake_minimum_required(VERSION 2.8)
if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# ------------------------------------------------------------------------
# Set project version number 
SET(PLUS_VERSION_MAJOR "1")
SET(PLUS_VERSION_MINOR "1")
SET(PLUS_VERSION_PATCH "1")
SET(PLUS_VERSION ${PLUS_VERSION_MAJOR}.${PLUS_VERSION_MINOR}.${PLUS_VERSION_PATCH})

MESSAGE(STATUS "Plus version: ${PLUS_VERSION}" )

# --------------------------------------------------------------------------
# User options
#

# Standard CMake option for testing.
OPTION(BUILD_TESTING
       "Build test programs"
       ON)
       
# --------------------------------------------------------------------------
# Testing
#

SET (PLUS_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data)

# Need subversion for the automatic update of the repository
find_package(Subversion)
IF(Subversion_FOUND)
  Subversion_WC_INFO(${CMAKE_CURRENT_SOURCE_DIR} PLUS_SVN_INFO)
  MESSAGE(STATUS "Current revision is ${PLUS_SVN_INFO_WC_REVISION}") 
  SET(PLUS_REVISION ${PLUS_SVN_INFO_WC_REVISION})
ENDIF(Subversion_FOUND)

SET( PLUS_SVN_EXECUTABLE ${Subversion_SVN_EXECUTABLE} )

INCLUDE(CTest)

# --------------------------------------------------------------------------
# Packaging
#

INCLUDE (${CMAKE_CURRENT_SOURCE_DIR}/CPackConfig.cmake)

# --------------------------------------------------------------------------
# Subdirs
#

ADD_SUBDIRECTORY(src)

#-----------------------------------------------------------------------------
# Generate convenience files for automatic build, test, and packaging
#
 
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/BuildAndTest.bat.in 
	${CMAKE_CURRENT_BINARY_DIR}/BuildAndTest.bat
	)
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
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/DevEnv_RelWithDebInfo.bat.in 
	${CMAKE_CURRENT_BINARY_DIR}/DevEnv_RelWithDebInfo.bat
	)
