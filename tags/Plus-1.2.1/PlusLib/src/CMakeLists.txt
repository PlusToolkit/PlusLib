
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
SET(LIBRARY_OUTPUT_PATH  ${CMAKE_BINARY_DIR}/bin CACHE PATH "Single output directory for building all libraries." FORCE)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin CACHE PATH "Single output directory for building all executables." FORCE)
SET(INCLUDES_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin CACHE PATH "Single output directory for all include files" FORCE)
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH INCLUDES_OUTPUT_PATH)

# --------------------------------------------------------------------------
# gnuplot binary directory path.
# 
SET(GNUPLOT_BIN_DIR ${PLTOOLS_DIR}/Gnuplot/binary/ CACHE PATH "Path to the Gnuplot binary directory." FORCE)

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
# Include dirs

SET (PLUSLIB_INCLUDE_DIRS 	
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
	${CMAKE_CURRENT_SOURCE_DIR}/Tracking/BrachyTracking
	${CMAKE_CURRENT_SOURCE_DIR}/Tracking/BrachyTracking/Utils	
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
	CACHE INTERNAL "" FORCE
	)


INCLUDE_DIRECTORIES( ${PLUSLIB_INCLUDE_DIRS} )

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

# --------------------------------------------------------------------------
# Create Plus target list 
SET( PLUSLIB_TARGETS 
        vtkImageAcquisition 
        vtkTrusCalibration 
        vtkTrusCalibrationCommon 
        vtkSignaling 
        vtkPlusCommon 
        UsFidSegAlgo 
        vtkTracking 
        vtkDataCollection 
        vtkVolumeReconstruction 
        )

IF ( PLUS_USE_BRACHY_TRACKER )
    SET( PLUSLIB_TARGETS ${PLUSLIB_TARGETS} BrachyStepper )
ENDIF ( PLUS_USE_BRACHY_TRACKER )

IF ( PLUS_USE_CERTUS OR PLUS_USE_POLARIS )
    SET( PLUSLIB_TARGETS ${PLUSLIB_TARGETS} vtkndicapi )
ENDIF ( PLUS_USE_CERTUS OR PLUS_USE_POLARIS )

IF ( PLUS_USE_FLOCK )
    SET( PLUSLIB_TARGETS ${PLUSLIB_TARGETS} vtkflock )
ENDIF ( PLUS_USE_FLOCK )

IF ( PLUS_USE_MICRONTRACKER )
    SET( PLUSLIB_TARGETS ${PLUSLIB_TARGETS} vtkMicrontrackerfiles )
ENDIF ( PLUS_USE_MICRONTRACKER )

# Add all targets to the build-tree export set
export(TARGETS ${PLUSLIB_TARGETS}
  FILE "${CMAKE_CURRENT_BINARY_DIR}/PlusLibLibraryDepends.cmake")    
 
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE PlusLib)

# Create a PlusConfig.cmake file for the use from the build tree
configure_file(PlusLibConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/PlusLibConfig.cmake" @ONLY)
configure_file(PlusLibConfigVersion.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/PlusLibConfigVersion.cmake" @ONLY)
  
# IF (BUILD_SLICER_MODULE)
	
	# --------------------------------------------------------------------------
	# Find Slicer3
	
	# if(NOT Slicer3_SOURCE_DIR)
		# find_package(Slicer3 REQUIRED)
		# include(${Slicer3_USE_FILE})
		# slicer3_set_default_install_prefix_for_external_projects()
	# endif(NOT Slicer3_SOURCE_DIR)

	# include_directories(
	  # ${Slicer3_Libs_INCLUDE_DIRS}
	  # ${Slicer3_Base_INCLUDE_DIRS}
	  # )

	# install(FILES 
		# ${headers} 
		# "${CMAKE_CURRENT_BINARY_DIR}/vtkPlusConfigure.h"
		# DESTINATION ${Slicer3_INSTALL_MODULES_INCLUDE_DIR}/${PROJECT_NAME} COMPONENT Development
	# )
	
	# --------------------------------------------------------------------------
	# Sources

	# set(PLUS_SRCS 
		# vtkPlusLogic.cxx
		# vtkPlusGUI.cxx
		# vtkMRMLPlusNode.cxx
		# )

	# --------------------------------------------------------------------------
	# Wrapping
	
	# include("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
	# vtk_wrap_tcl3(Plus PLUS_TCL_SRCS "${PLUS_SRCS}" "")
	
	#---------------------------------------------------------------------------
	# Add Loadable Module support
	
	# generatelm(PLUS_SRCS Plus.txt)
	
	# --------------------------------------------------------------------------
	# Build and install the library
	
	# set(lib_name Plus)
	# add_library(${lib_name}
	# ${PLUS_SRCS} 
	# ${PLUS_TCL_SRCS}
	# )
	
	# slicer3_set_modules_output_path(${lib_name})
	
	#LINK_DIRECTORIES("c:/devel/Slicer3/STABLE/Slicer3-build/bin/Debug")
	
	# target_link_libraries(${lib_name}
	# ${Slicer3_Libs_LIBRARIES}
	# ${Slicer3_Base_LIBRARIES}
	# ${KWWidgets_LIBRARIES}
	# vtkDataCollection
	# vtkImageAcquisition
	# )
	
	# install(TARGETS ${lib_name}
	# RUNTIME DESTINATION ${Slicer3_INSTALL_MODULES_BIN_DIR} COMPONENT RuntimeLibraries 
	# LIBRARY DESTINATION ${Slicer3_INSTALL_MODULES_LIB_DIR} COMPONENT RuntimeLibraries
	# ARCHIVE DESTINATION ${Slicer3_INSTALL_MODULES_LIB_DIR} COMPONENT Development
	# )
	
	# slicer3_install_modules(${lib_name})

# ENDIF (BUILD_SLICER_MODULE)

#-----------------------------------------------------------------------------
# Generate include file for projects that use this library 
# 
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/UsePlusLib.cmake.in 
	${CMAKE_CURRENT_BINARY_DIR}/UsePlusLib.cmake
    @ONLY
	)	
