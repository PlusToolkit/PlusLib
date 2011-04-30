#
# Configure the build to work (although with limited functionalities) if only
# src directory of the repository is available
# 
IF(NOT PLUS_INSTALL_BIN_DIR)

  cmake_minimum_required(VERSION 2.6)
  if(COMMAND cmake_policy)
	 cmake_policy(SET CMP0003 NEW)
  endif(COMMAND cmake_policy)

  MESSAGE("The library is not configured using the root CMakeList.txt file, installation and automatic testing is not supported.")
 
  # SET(PLUS_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../data)
  
ENDIF(NOT PLUS_INSTALL_BIN_DIR)

#
# Try to find VTK and include its settings (otherwise complain)
#

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

#
# Try to find ITK and include its settings (otherwise complain)
#

FIND_PACKAGE (ITK)
IF ( ITK_FOUND)
	INCLUDE( ${USE_ITK_FILE} )
ENDIF( ITK_FOUND)


#
# Use OpenIGTLink protocol
# 
OPTION(PLUS_USE_OpenIGTLink "Use the OpenIGTLink protocol" OFF)
IF(PLUS_USE_OpenIGTLink)
  FIND_PACKAGE(OpenIGTLink)
  IF (OpenIGTLink_USE_FILE)
    INCLUDE (${OpenIGTLink_USE_FILE})
  ELSE (OpenIGTLink_USE_FILE)
    MESSAGE( FATAL_ERROR "This application requires OpenIGTLink. One of these components is missing. Please verify configuration")
  ENDIF (OpenIGTLink_USE_FILE)
ENDIF(PLUS_USE_OpenIGTLink)


# --------------------------------------------------------------------------
# Options

#
# Configure output paths for libraries and executables.
#
 
SET( LIBRARY_OUTPUT_PATH  ${CMAKE_BINARY_DIR}/bin CACHE PATH 
    "Single output directory for building all libraries." FORCE)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin CACHE PATH 
    "Single output directory for building all executables." FORCE)
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)

#
# gnuplot binary directory path.
#

FIND_PATH(GNUPLOT_BIN_DIR "Gnuplot/binary/gnuplot.exe")


# The project library directories.
SET(PLUS_LIBRARY_DIRS ${LIBRARY_OUTPUT_PATH})

#
# Build shared libs ?
#
# Defaults to the same VTK setting.
#

# Standard CMake option for building libraries shared or static by default.
OPTION(BUILD_SHARED_LIBS
       "Build with shared libraries."
       ${VTK_BUILD_SHARED_LIBS}
	   ${ITK_BUILD_SHARED_LIBS})     
# Copy the CMake option to a setting with SYNCHROGRAB4D_ prefix for use in
# our project.  This name is used in vtkmyConfigure.h.in.
# SET(SYNCHROGRAB4D_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})


OPTION (BUILD_SLICER_MODULE "Build project as Slicer module" OFF)
#
# Configure include file
#

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/PlusConfigure.h.in 
	${CMAKE_CURRENT_BINARY_DIR}/PlusConfigure.h
	)


# --------------------------------------------------------------------------
# Include dirs

SET (PLUS_INCLUDE_DIRS 	
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/DataCollection 
	${CMAKE_CURRENT_SOURCE_DIR}/Signaling
	${CMAKE_CURRENT_SOURCE_DIR}/ImageAcquisition 
	${CMAKE_CURRENT_SOURCE_DIR}/ImageAcquisition/ICCapturing 
	${CMAKE_CURRENT_SOURCE_DIR}/ImageAcquisition/LinuxVideo 
	${CMAKE_CURRENT_SOURCE_DIR}/ImageAcquisition/MatroxImaging 
	${CMAKE_CURRENT_SOURCE_DIR}/ImageAcquisition/SonixVideo 
	${CMAKE_CURRENT_SOURCE_DIR}/ImageAcquisition/VideoForWindows 
	${CMAKE_CURRENT_SOURCE_DIR}/PlusCommon
	${CMAKE_CURRENT_SOURCE_DIR}/Tracking
    ${CMAKE_CURRENT_SOURCE_DIR}/Tracking/Ascension3DGTracking
	${CMAKE_CURRENT_SOURCE_DIR}/Tracking/AMSTracking
	${CMAKE_CURRENT_SOURCE_DIR}/Tracking/AMSTracking/Utils	
	${CMAKE_CURRENT_SOURCE_DIR}/Tracking/FakeTracking 
	${CMAKE_CURRENT_SOURCE_DIR}/Tracking/FlockTracking 
	${CMAKE_CURRENT_SOURCE_DIR}/Tracking/FlockTracking/Utils
	${CMAKE_CURRENT_SOURCE_DIR}/Tracking/MicronTracking 
	${CMAKE_CURRENT_SOURCE_DIR}/Tracking/MicronTracking/Utils
	${CMAKE_CURRENT_SOURCE_DIR}/Tracking/ndicapi 
	${CMAKE_CURRENT_SOURCE_DIR}/Tracking/NDICertusTracking 
	${CMAKE_CURRENT_SOURCE_DIR}/Tracking/PolarisTracking 
	${CMAKE_CURRENT_SOURCE_DIR}/TrusCalibration
	${CMAKE_CURRENT_SOURCE_DIR}/TrusCalibration/TrusCalibrationCommon  
	${CMAKE_CURRENT_SOURCE_DIR}/UsFidSegAlgo
	${CMAKE_CURRENT_SOURCE_DIR}/VolumeReconstruction 
	CACHE INTERNAL ""
	)


INCLUDE_DIRECTORIES( ${PLUS_INCLUDE_DIRS} )

# --------------------------------------------------------------------------
# Subdirs
ADD_SUBDIRECTORY(ImageAcquisition)
ADD_SUBDIRECTORY(TrusCalibration)
ADD_SUBDIRECTORY(Signaling)
ADD_SUBDIRECTORY(PlusCommon)
ADD_SUBDIRECTORY(UsFidSegAlgo)
ADD_SUBDIRECTORY(Tracking)
ADD_SUBDIRECTORY(DataCollection)
ADD_SUBDIRECTORY(VolumeReconstruction)
ADD_SUBDIRECTORY(scripts)

file(GLOB headers 
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
	)

IF (BUILD_SLICER_MODULE)
	
	# --------------------------------------------------------------------------
	# Find Slicer3
	
	if(NOT Slicer3_SOURCE_DIR)
		find_package(Slicer3 REQUIRED)
		include(${Slicer3_USE_FILE})
		slicer3_set_default_install_prefix_for_external_projects()
	endif(NOT Slicer3_SOURCE_DIR)

	include_directories(
	  ${Slicer3_Libs_INCLUDE_DIRS}
	  ${Slicer3_Base_INCLUDE_DIRS}
	  )

	install(FILES 
		${headers} 
		"${CMAKE_CURRENT_BINARY_DIR}/vtkPlusConfigure.h"
		DESTINATION ${Slicer3_INSTALL_MODULES_INCLUDE_DIR}/${PROJECT_NAME} COMPONENT Development
	)
	
	# --------------------------------------------------------------------------
	# Sources

	set(PLUS_SRCS 
		vtkPlusLogic.cxx
		vtkPlusGUI.cxx
		vtkMRMLPlusNode.cxx
		)

	# --------------------------------------------------------------------------
	# Wrapping
	
	include("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
	vtk_wrap_tcl3(Plus PLUS_TCL_SRCS "${PLUS_SRCS}" "")
	
	#---------------------------------------------------------------------------
	# Add Loadable Module support
	
	generatelm(PLUS_SRCS Plus.txt)
	
	# --------------------------------------------------------------------------
	# Build and install the library
	
	set(lib_name Plus)
	add_library(${lib_name}
	${PLUS_SRCS} 
	${PLUS_TCL_SRCS}
	)
	
	slicer3_set_modules_output_path(${lib_name})
	
	#LINK_DIRECTORIES("c:/devel/Slicer3/STABLE/Slicer3-build/bin/Debug")
	
	target_link_libraries(${lib_name}
	${Slicer3_Libs_LIBRARIES}
	${Slicer3_Base_LIBRARIES}
	${KWWidgets_LIBRARIES}
	vtkDataCollection
	vtkImageAcquisition
	)
	
	install(TARGETS ${lib_name}
	RUNTIME DESTINATION ${Slicer3_INSTALL_MODULES_BIN_DIR} COMPONENT RuntimeLibraries 
	LIBRARY DESTINATION ${Slicer3_INSTALL_MODULES_LIB_DIR} COMPONENT RuntimeLibraries
	ARCHIVE DESTINATION ${Slicer3_INSTALL_MODULES_LIB_DIR} COMPONENT Development
	)
	
	slicer3_install_modules(${lib_name})

ENDIF (BUILD_SLICER_MODULE)

#-----------------------------------------------------------------------------
# Generate include file for projects that use this library 
#

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/UsePlus.cmake.in 
	${CMAKE_CURRENT_BINARY_DIR}/UsePlus.cmake
	)	

# --------------------------------------------------------------------------
# Install
#

IF(PLUS_INSTALL_SCRIPTS_DIR)

  INSTALL(
    DIRECTORY ${GNUPLOT_BIN_DIR}/
    DESTINATION ${PLUS_INSTALL_GNUPLOT_DIR}
    PATTERN ".svn" EXCLUDE
  )

ENDIF(PLUS_INSTALL_SCRIPTS_DIR)
