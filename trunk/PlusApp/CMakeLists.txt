PROJECT(PlusApp)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

# ------------------------------------------------------------------------
# Find Plus use file
IF ( NOT PLUSLIB_DIR )
    FIND_PACKAGE (PlusLib)
ELSE ()
    FIND_PACKAGE (PlusLib REQUIRED PATHS "${PLUSLIB_DIR}/src" NO_DEFAULT_PATH)
ENDIF() 

IF ( PlusLib_FOUND)
	INCLUDE( ${PlusLib_USE_FILE} )
ELSE( PlusLib_FOUND )
    MESSAGE( FATAL_ERROR "This project requires PlusLib. One of these components is missing. Please verify configuration")
ENDIF( PlusLib_FOUND )

# ------------------------------------------------------------------------
# Set project version number: PlusApp version should identical to Plus version 
SET(PLUSAPP_VERSION_MAJOR ${PLUSLIB_VERSION_MAJOR})
SET(PLUSAPP_VERSION_MINOR ${PLUSLIB_VERSION_MINOR})
SET(PLUSAPP_VERSION_PATCH ${PLUSLIB_VERSION_PATCH})
SET(PLUSAPP_VERSION ${PLUSAPP_VERSION_MAJOR}.${PLUSAPP_VERSION_MINOR}.${PLUSAPP_VERSION_PATCH})

MESSAGE(STATUS "PlusApp version: ${PLUSAPP_VERSION}" )
       
# --------------------------------------------------------------------------
# Testing
#

# Standard CMake option for testing.
OPTION(BUILD_TESTING
       "Build test programs"
       ON)

INCLUDE(CTest)

# Setup file for setting custom ctest vars (for ignoring certain warnings, etc.)
CONFIGURE_FILE(
  CTestCustom.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake
  @ONLY
  )

# --------------------------------------------------------------------------
# Find SVN for revision information status and automatic update of the repository
#
FIND_PACKAGE(Subversion)
IF(Subversion_FOUND)
  Subversion_WC_INFO(${CMAKE_CURRENT_SOURCE_DIR} PLUSAPP_SVN_INFO)
  MESSAGE(STATUS "Current revision is ${PLUSAPP_SVN_INFO_WC_REVISION}") 
  SET(PLUSAPP_REVISION ${PLUSAPP_SVN_INFO_WC_REVISION})
ENDIF(Subversion_FOUND) 
SET( PLUSAPP_SVN_EXECUTABLE ${Subversion_SVN_EXECUTABLE} )

# Standard CMake option for building libraries shared or static by default.
OPTION(BUILD_SHARED_LIBS
       "Build with shared libraries."
       ${VTK_BUILD_SHARED_LIBS}
	   ${ITK_BUILD_SHARED_LIBS})    

# --------------------------------------------------------------------------
# Configure output paths for libraries and executables.
#
SET(LIBRARY_OUTPUT_PATH  ${PLUS_EXECUTABLE_OUTPUT_PATH} CACHE PATH "Single output directory for building all libraries." FORCE)
SET(EXECUTABLE_OUTPUT_PATH ${PLUS_EXECUTABLE_OUTPUT_PATH} CACHE PATH "Single output directory for building all executables." FORCE)
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH )

IF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio" )
  SET(TEST_EXECUTABLE_OUTPUT_PATH "${PLUS_EXECUTABLE_OUTPUT_PATH}/Release")
ELSE()
  SET(TEST_EXECUTABLE_OUTPUT_PATH "${PLUS_EXECUTABLE_OUTPUT_PATH}")
ENDIF()

SET(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/Install )
SET( INSTALL_DESTINATION_DIR "PlusApp_v${PLUSAPP_VERSION_MAJOR}.${PLUSAPP_VERSION_MINOR}.${PLUSAPP_VERSION_PATCH}" )

OPTION (PLUSAPP_BUILD_DiagnosticTools "Build the DiagnosticTools" ON)
OPTION (PLUSAPP_BUILD_OpenIGTLinkBroadcaster "Build the OpenIGTLinkBroadcaster application" OFF)
OPTION (PLUSAPP_BUILD_fCal "Build the fCal application (Qt)" ON)

OPTION (PLUSAPP_TEST_GUI "Enable Sikuli GUI tests" ON)

# --------------------------------------------------------------------------
# Packaging
# 
INCLUDE (${CMAKE_CURRENT_SOURCE_DIR}/CPackConfig.cmake) 

# --------------------------------------------------------------------------
# Build the CommonWidgets library
ADD_SUBDIRECTORY(CommonWidgets)

# --------------------------------------------------------------------------
# Build the DiagnosticTools 
IF (PLUSAPP_BUILD_DiagnosticTools)
    ADD_SUBDIRECTORY(DiagnosticTools)
    ADD_DEPENDENCIES( DiagDataCollection ${PLUSLIB_DEPENDENCIES} )
    ADD_DEPENDENCIES( DiagDataSync ${PLUSLIB_DEPENDENCIES} )
ENDIF (PLUSAPP_BUILD_DiagnosticTools)

# --------------------------------------------------------------------------
# Build the fCal application (Qt)
IF (PLUSAPP_BUILD_fCal)
	ADD_SUBDIRECTORY(fCal)
    ADD_DEPENDENCIES( fCal ${PLUSLIB_DEPENDENCIES} )
ENDIF (PLUSAPP_BUILD_fCal)

IF(PLUSAPP_BUILD_OpenIGTLinkBroadcaster)
    IF (PLUS_USE_OpenIGTLink)
        ADD_SUBDIRECTORY(OpenIGTLinkBroadcaster)
        ADD_DEPENDENCIES( OpenIGTLinkBroadcaster ${PLUSLIB_DEPENDENCIES} )
    ELSE (PLUS_USE_OpenIGTLink)
        MESSAGE(SEND_ERROR "OpenIGTLinkBroadcaster: PLUS has to be configured with PLUS_USE_OpenIGTLink flag ON")
    ENDIF (PLUS_USE_OpenIGTLink)
ENDIF(PLUSAPP_BUILD_OpenIGTLinkBroadcaster)

#-----------------------------------------------------------------------------
# Include PlusLib MS projects 
INCLUDE_PLUSLIB_MS_PROJECTS()

SET (PLUSAPP_INCLUDE_DIRS 	
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/CommonWidgets
    ${CMAKE_CURRENT_BINARY_DIR}/CommonWidgets
    )
# --------------------------------------------------------------------------
# Create PlusApp target list 
SET( PLUSAPP_TARGETS
    CommonWidgets
   )

# Add all targets to the build-tree export set
export(TARGETS ${PLUSAPP_TARGETS}
  FILE "${CMAKE_CURRENT_BINARY_DIR}/PlusAppLibraryDepends.cmake") 
  
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE PlusApp)

# Create a PlusAppConfig.cmake file for the use from the build tree
configure_file(PlusAppConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/PlusAppConfig.cmake" @ONLY)
configure_file(PlusAppConfigVersion.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/PlusAppConfigVersion.cmake" @ONLY)

#-----------------------------------------------------------------------------
# Generate include file for projects that use this library 
# 
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/UsePlusApp.cmake.in 
	${CMAKE_CURRENT_BINARY_DIR}/UsePlusApp.cmake
    @ONLY
	)

#-----------------------------------------------------------------------------
# Generate default application configuration file for the install
# 
SET( PLUSCONFIG_DEVICESET_CONFIG_DIR "../config" )
SET( PLUSCONFIG_IMAGE_DIR "../data" )
SET( PLUSCONFIG_GNUPLOT_DIR "../gnuplot" )
SET( PLUSCONFIG_SCRIPTS_DIR "../scripts" )

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/PlusConfig.xml.in
	${CMAKE_CURRENT_BINARY_DIR}/PlusConfig.xml
	)	

#-----------------------------------------------------------------------------
# Generate convenience files for automatic build, test, and packaging
# 
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/CreatePackage.bat.in
	${CMAKE_CURRENT_BINARY_DIR}/CreatePackage.bat
	)	
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/BuildAndTest.bat.in 
	${CMAKE_CURRENT_BINARY_DIR}/BuildAndTest.bat
	)
#-----------------------------------------------------------------------------
# Install files 
INCLUDE(InstallFiles.cmake)    

