# --------------------------------------------------------------------------
# Sources

SET (TrusCalibration_SRCS
	vtkCalibrationController.cxx 
	vtkProbeCalibrationControllerIO.cxx 
    vtkMeanShiftClustering.cxx 
	vtkPivotCalibrationAlgo.cxx
	vtkPhantomRegistrationAlgo.cxx
	VolumeReconstructionController.cxx
    vtkSpacingCalibAlgo.cxx 
	)

SET (TrusCalibration_HDRS)	
IF (WIN32)	
	SET (TrusCalibration_HDRS
		vtkCalibrationController.h
		vtkProbeCalibrationControllerIO.h
        vtkMeanShiftClustering.h
		vtkPivotCalibrationAlgo.h
		vtkPhantomRegistrationAlgo.h
		VolumeReconstructionController.h
        vtkSpacingCalibAlgo.h 
		)
ENDIF (WIN32)

IF (PLUS_USE_BRACHY_TRACKER)
    SET (TrusCalibration_SRCS
        ${TrusCalibration_SRCS}
        vtkStepperCalibrationController.cxx
        vtkRotationAxisCalibAlgo.cxx 
        vtkTranslAxisCalibAlgo.cxx 
        vtkCenterOfRotationCalibAlgo.cxx
        )
    IF (WIN32)	
        SET (TrusCalibration_HDRS
            ${TrusCalibration_HDRS}
            vtkStepperCalibrationController.h 
            vtkRotationAxisCalibAlgo.h
            vtkTranslAxisCalibAlgo.h
            vtkCenterOfRotationCalibAlgo.h
            )
     ENDIF (WIN32)
ENDIF ()

SET (TrusCalibration_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "" FORCE)

ADD_SUBDIRECTORY (TrusCalibrationCommon)
				
# --------------------------------------------------------------------------
# Build the library

SET (TrusCalibration_LIBS 
  vtkCommon 
  vtkInfovis 
  itkvnl 
  itkvnl_algo 
  UsFidSegAlgo 
  vtkTrusCalibrationCommon 
  vtkPlusCommon
  vtkTracking
  )

INCLUDE_DIRECTORIES( ${TrusCalibration_INCLUDE_DIRS} )
ADD_LIBRARY( vtkTrusCalibration STATIC ${TrusCalibration_SRCS} ${TrusCalibration_HDRS} )
TARGET_LINK_LIBRARIES(vtkTrusCalibration ${TrusCalibration_LIBS})

# --------------------------------------------------------------------------
# Set external MS project 
SET (PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkTrusCalibration CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkTrusCalibration vtkTrusCalibration "${PlusLib_BINARY_DIR}/src/TrusCalibration/vtkTrusCalibration.vcproj" vtkPlusCommon UsFidSegAlgo CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Install
#

# INSTALL(TARGETS vtkTrusCalibration
  # DESTINATION bin
  # CONFIGURATIONS Release
  # COMPONENT RuntimeLibraries)
    
# --------------------------------------------------------------------------
# Testing
#

IF (BUILD_TESTING)
	ADD_SUBDIRECTORY (Testing)
ENDIF (BUILD_TESTING)