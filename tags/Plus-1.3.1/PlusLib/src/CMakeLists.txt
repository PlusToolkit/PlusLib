

# --------------------------------------------------------------------------
# Try to find VTK and include its settings (otherwise complain)
# 
FIND_PACKAGE(VTK REQUIRED PATHS ${VTK_DIR} NO_DEFAULT_PATH)
IF ( VTK_FOUND )
INCLUDE(${VTK_USE_FILE})
ELSE ( VTK_FOUND )
    MESSAGE( FATAL_ERROR "This application requires VTK. One of these components is missing. Please verify configuration")
ENDIF ( VTK_FOUND )

#
# Try to find ITK and include its settings (otherwise complain)
# 
FIND_PACKAGE (ITK REQUIRED PATHS ${ITK_DIR} NO_DEFAULT_PATH)
IF ( ITK_FOUND )
	INCLUDE( ${ITK_USE_FILE} )
ELSE ( ITK_FOUND )
    MESSAGE( FATAL_ERROR "This application requires ITK. One of these components is missing. Please verify configuration")
ENDIF( ITK_FOUND )


# --------------------------------------------------------------------------
# Use OpenIGTLink protocol
# 
OPTION(PLUS_USE_OpenIGTLink "Use the OpenIGTLink protocol" OFF)
IF(PLUS_USE_OpenIGTLink)
  FIND_PACKAGE(OpenIGTLink PATHS ${OpenIGTLink_DIR} NO_DEFAULT_PATH)
  IF (OpenIGTLink_USE_FILE)
    INCLUDE (${OpenIGTLink_USE_FILE})
  ELSE (OpenIGTLink_USE_FILE)
    MESSAGE( FATAL_ERROR "This application requires OpenIGTLink. One of these components is missing. Please verify configuration")
  ENDIF (OpenIGTLink_USE_FILE)
ENDIF(PLUS_USE_OpenIGTLink)


# --------------------------------------------------------------------------
# Options
OPTION(BUILD_SHARED_LIBS "Build with shared libraries." ${VTK_BUILD_SHARED_LIBS} ${ITK_BUILD_SHARED_LIBS} )     
# OPTION (BUILD_SLICER_MODULE "Build project as Slicer module" OFF)

# --------------------------------------------------------------------------
# Configure output paths for libraries and executables.
# 
SET(LIBRARY_OUTPUT_PATH  ${PLUS_EXECUTABLE_OUTPUT_PATH} CACHE PATH "Single output directory for building all libraries." FORCE)
SET(EXECUTABLE_OUTPUT_PATH ${PLUS_EXECUTABLE_OUTPUT_PATH} CACHE PATH "Single output directory for building all executables." FORCE)
SET(INCLUDES_OUTPUT_PATH ${PLUS_EXECUTABLE_OUTPUT_PATH} CACHE PATH "Single output directory for all include files" FORCE)
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH INCLUDES_OUTPUT_PATH)

IF (WIN32)
  SET(TEST_EXECUTABLE_OUTPUT_PATH "${PLUS_EXECUTABLE_OUTPUT_PATH}/Release")
ELSE()
  SET(TEST_EXECUTABLE_OUTPUT_PATH "${PLUS_EXECUTABLE_OUTPUT_PATH}")
ENDIF()

# --------------------------------------------------------------------------
# gnuplot binary directory path.
# 
SET(GNUPLOT_BIN_DIR ${PLUSLIB_TOOLS_DIR}/Gnuplot/binary/ CACHE PATH "Path to the Gnuplot binary directory." FORCE)

# --------------------------------------------------------------------------
# Sikuli binary directory path.
# 
SET(SIKULI_BIN_DIR ${PLUSLIB_TOOLS_DIR}/Sikuli/ CACHE PATH "Path to the Sikuli binary directory." FORCE)

# --------------------------------------------------------------------------
# The project library directories.
SET(PLUSLIB_LIBRARY_DIRS ${LIBRARY_OUTPUT_PATH} CACHE INTERNAL "Path to the Plus library directory" FORCE)

# --------------------------------------------------------------------------
# Configure include file
# 
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/PlusConfigure.h.in 
	${CMAKE_CURRENT_BINARY_DIR}/PlusConfigure.h
	)

# --------------------------------------------------------------------------
# Initialize variables that will be filled by makefiles in subdirectories
#

# PLUSLIB_DEPENDENCIES contain all the exported targets that will be
# available to applications that use PlusLib
SET(PLUSLIB_DEPENDENCIES "" CACHE INTERNAL "" )

# PLUSLIB_INCLUDE_DIRS contain all the include directories that will be
# available to applications that use PlusLib
SET(PLUSLIB_INCLUDE_DIRS "" CACHE INTERNAL "" )

# --------------------------------------------------------------------------
# Current directory

SET (BASIC_INCLUDE_DIRS 	
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	)
INCLUDE_DIRECTORIES( ${BASIC_INCLUDE_DIRS} )
SET(PLUSLIB_INCLUDE_DIRS ${PLUSLIB_INCLUDE_DIRS} ${BASIC_INCLUDE_DIRS} CACHE INTERNAL "" )

# --------------------------------------------------------------------------
# Subdirectories

# Variables sent back from subdirectories (through CACHE):
# ..._INCLUDE_DIRS
# vcProj_...
# PLUSLIB_DEPENDENCIES

ADD_SUBDIRECTORY(PlusCommon)
INCLUDE_DIRECTORIES( ${PlusCommon_INCLUDE_DIRS} )
SET(PLUSLIB_INCLUDE_DIRS ${PLUSLIB_INCLUDE_DIRS} ${PlusCommon_INCLUDE_DIRS} CACHE INTERNAL "" )

ADD_SUBDIRECTORY(ImageAcquisition)
INCLUDE_DIRECTORIES( ${ImageAcquisition_INCLUDE_DIRS} )
SET(PLUSLIB_INCLUDE_DIRS ${PLUSLIB_INCLUDE_DIRS} ${ImageAcquisition_INCLUDE_DIRS} CACHE INTERNAL "" )

# Tracking uses vtkTrackedFrameList class from ImageAcquisition
ADD_SUBDIRECTORY(Tracking)
INCLUDE_DIRECTORIES( ${Tracking_INCLUDE_DIRS} )
SET(PLUSLIB_INCLUDE_DIRS ${PLUSLIB_INCLUDE_DIRS} ${Tracking_INCLUDE_DIRS} CACHE INTERNAL "" )

ADD_SUBDIRECTORY(Signaling)
INCLUDE_DIRECTORIES( ${Signaling_INCLUDE_DIRS} )
SET(PLUSLIB_INCLUDE_DIRS ${PLUSLIB_INCLUDE_DIRS} ${Signaling_INCLUDE_DIRS} CACHE INTERNAL "" )

# DataCollection uses ImageAcquisition and Tracking
ADD_SUBDIRECTORY(DataCollection)
INCLUDE_DIRECTORIES( ${DataCollection_INCLUDE_DIRS} )
SET(PLUSLIB_INCLUDE_DIRS ${PLUSLIB_INCLUDE_DIRS} ${DataCollection_INCLUDE_DIRS} CACHE INTERNAL "" )

ADD_SUBDIRECTORY(UsFidSegAlgo)
INCLUDE_DIRECTORIES( ${UsFidSegAlgo_INCLUDE_DIRS} )
SET(PLUSLIB_INCLUDE_DIRS ${PLUSLIB_INCLUDE_DIRS} ${UsFidSegAlgo_INCLUDE_DIRS} CACHE INTERNAL "" )

ADD_SUBDIRECTORY(VolumeReconstruction)
INCLUDE_DIRECTORIES( ${VolumeReconstruction_INCLUDE_DIRS} )
SET(PLUSLIB_INCLUDE_DIRS ${PLUSLIB_INCLUDE_DIRS} ${VolumeReconstruction_INCLUDE_DIRS} CACHE INTERNAL "" )

ADD_SUBDIRECTORY(TrusCalibration)
INCLUDE_DIRECTORIES( ${TrusCalibration_INCLUDE_DIRS} )
SET(PLUSLIB_INCLUDE_DIRS ${PLUSLIB_INCLUDE_DIRS} ${TrusCalibration_INCLUDE_DIRS} CACHE INTERNAL "" )

ADD_SUBDIRECTORY(Vibro)
INCLUDE_DIRECTORIES( ${Vibro_INCLUDE_DIRS} )
SET(PLUSLIB_INCLUDE_DIRS ${PLUSLIB_INCLUDE_DIRS} ${Vibro_INCLUDE_DIRS} CACHE INTERNAL "" )

ADD_SUBDIRECTORY(scripts)

file(GLOB headers 
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
	)

# --------------------------------------------------------------------------
# Create Plus target list 

# Add all targets to the build-tree export set
export(TARGETS 
  ${PLUSLIB_DEPENDENCIES}
  FILE "${CMAKE_CURRENT_BINARY_DIR}/PlusLibLibraryDepends.cmake"
  )    
 
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE PlusLib)

# Create a PlusConfig.cmake file for the use from the build tree
configure_file(PlusLibConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/PlusLibConfig.cmake" @ONLY)
configure_file(PlusLibConfigVersion.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/PlusLibConfigVersion.cmake" @ONLY)
  
#-----------------------------------------------------------------------------
# Generate include file for projects that use this library 
# 
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/UsePlusLib.cmake.in 
	${CMAKE_CURRENT_BINARY_DIR}/UsePlusLib.cmake @ONLY)	

#-----------------------------------------------------------------------------
# Generate default application configuration files in the non-installed executable directories (Release and Debug too)
# 
SET( PLUSCONFIG_DEVICESET_CONFIG_DIR "${PLUSLIB_SOURCE_DIR}/data/ConfigFiles" )
SET( PLUSCONFIG_IMAGE_DIR "${PLUSLIB_SOURCE_DIR}/data/TestImages" )
SET( PLUSCONFIG_GNUPLOT_DIR "${GNUPLOT_BIN_DIR}" )
SET( PLUSCONFIG_SCRIPTS_DIR "${PLUSLIB_SCRIPTS_DIR}" )

 IF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio" )
    #IF ( NOT EXISTS "${PLUS_EXECUTABLE_OUTPUT_PATH}/Release/PlusConfig.xml" )
        configure_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/PlusConfig.xml.in
            ${PLUS_EXECUTABLE_OUTPUT_PATH}/Release/PlusConfig.xml
            )	
    #ENDIF()
    
    #IF ( NOT EXISTS "${PLUS_EXECUTABLE_OUTPUT_PATH}/Debug/PlusConfig.xml" )
        configure_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/PlusConfig.xml.in
            ${PLUS_EXECUTABLE_OUTPUT_PATH}/Debug/PlusConfig.xml
            )
    #ENDIF()
ELSE()
     #IF ( NOT EXISTS "${PLUS_EXECUTABLE_OUTPUT_PATH}/PlusConfig.xml" )
        configure_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/PlusConfig.xml.in
            ${PLUS_EXECUTABLE_OUTPUT_PATH}/PlusConfig.xml
            )
    #ENDIF()
ENDIF()
  
#-----------------------------------------------------------------------------
# Documentation
#
option(DOCUMENTATION_TARGET_IN_ALL "Include the custom target for building documentation in 'all'" OFF)
mark_as_advanced(DOCUMENTATION_TARGET_IN_ALL)

# Attempt to discover Doxygen so that DOXYGEN_EXECUTABLE is set to an appropriate default value
SET (DOXYGEN_EXECUTABLE ${PLUSLIB_TOOLS_DIR}/Doxygen/doxygen-1.7.5.1/doxygen.exe CACHE PATH "Doxygen executable" FORCE)
SET (DOXYGEN_DOT_EXECUTABLE ${PLUSLIB_TOOLS_DIR}/Doxygen/doxygen-1.7.5.1/Graphviz-2.28/bin/dot.exe CACHE PATH "Doxygen dot executable" FORCE)
find_package(Doxygen QUIET)

add_subdirectory(Doxygen)
