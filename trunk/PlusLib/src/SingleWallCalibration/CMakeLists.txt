# --------------------------------------------------------------------------
# Sources
SET (SingleWallCalibration_SRCS 
  vtkSingleWallCalibration.cxx
  SingleWallCalibrationApp.cxx
	)
	
IF (WIN32)	
	SET (SingleWallCalibration_HDRS 
    vtkSingleWallCalibration.h
		)
ENDIF (WIN32)	

SET (SingleWallCalibration_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library

SET (SingleWallCalibration_LIBS   
  vtkPlusCommon 
  vtkDataCollection
  vtkVolumeReconstruction
  )

INCLUDE_DIRECTORIES( ${SingleWallCalibration_INCLUDE_DIRS} )
ADD_EXECUTABLE(SingleWallCalibration ${SingleWallCalibration_SRCS} ${SingleWallCalibration_HDRS})
TARGET_LINK_LIBRARIES(SingleWallCalibration ${SingleWallCalibration_LIBS})

# --------------------------------------------------------------------------
# Set external MS project 
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} SingleWallCalibration CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_SingleWallCalibration SingleWallCalibration;"${PlusLib_BINARY_DIR}/src/SingleWallCalibration/SingleWallCalibration.vcproj";vtkPlusCommon CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Install
#
INSTALL(TARGETS SingleWallCalibration
  RUNTIME DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  LIBRARY DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  )

# --------------------------------------------------------------------------
# Testing
#

IF (BUILD_TESTING)
	#SUBDIRS (Testing)
ENDIF (BUILD_TESTING)
