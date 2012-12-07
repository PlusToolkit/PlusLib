PROJECT(PlusLib)

cmake_minimum_required(VERSION 2.8)
if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# ------------------------------------------------------------------------
# Set project version number 
SET(PLUSLIB_VERSION_MAJOR "1")
SET(PLUSLIB_VERSION_MINOR "8")
SET(PLUSLIB_VERSION_PATCH "0")
SET(PLUSLIB_VERSION ${PLUSLIB_VERSION_MAJOR}.${PLUSLIB_VERSION_MINOR}.${PLUSLIB_VERSION_PATCH})

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

SET (PLUSLIB_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data)
SET (PLUSLIB_TOOLS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tools)
# Need subversion for the automatic update of the repository
find_package(Subversion)
IF(Subversion_FOUND)
  Subversion_WC_INFO(${CMAKE_CURRENT_SOURCE_DIR} PLUSLIB_SVN_INFO)
  MESSAGE(STATUS "Current revision is ${PLUSLIB_SVN_INFO_WC_REVISION}") 
  SET(PLUSLIB_REVISION ${PLUSLIB_SVN_INFO_WC_REVISION})
ENDIF(Subversion_FOUND)

SET( PLUSLIB_SVN_EXECUTABLE ${Subversion_SVN_EXECUTABLE} )

INCLUDE(CTest)

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
	${CMAKE_CURRENT_SOURCE_DIR}/CreateDoc.bat.in 
	${CMAKE_CURRENT_BINARY_DIR}/CreateDoc.bat
	)
