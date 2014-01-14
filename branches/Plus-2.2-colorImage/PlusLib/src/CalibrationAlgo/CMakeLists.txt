# --------------------------------------------------------------------------
# Sources

SET (CalibrationAlgo_SRCS
  vtkProbeCalibrationAlgo/vtkProbeCalibrationAlgo.cxx 
  vtkProbeCalibrationAlgo/vtkProbeCalibrationOptimizerAlgo.cxx
  vtkPivotCalibrationAlgo/vtkPivotCalibrationAlgo.cxx
  vtkPhantomLandmarkRegistrationAlgo/vtkPhantomLandmarkRegistrationAlgo.cxx
  vtkPhantomLinearObjectRegistrationAlgo/vtkPhantomLinearObjectRegistrationAlgo.cxx
  vtkSpacingCalibAlgo/vtkSpacingCalibAlgo.cxx 
  vtkCenterOfRotationCalibAlgo/vtkCenterOfRotationCalibAlgo.cxx
  vtkBrachyStepperPhantomRegistrationAlgo/vtkBrachyStepperPhantomRegistrationAlgo.cxx
  vtkTemporalCalibrationAlgo/vtkTemporalCalibrationAlgo.cxx
  vtkTemporalCalibrationAlgo/vtkPrincipalMotionDetectionAlgo.cxx 
  vtkLineSegmentationAlgo/vtkLineSegmentationAlgo.cxx 
  vtkPhantomLinearObjectRegistrationAlgo/Line.cxx
  vtkPhantomLinearObjectRegistrationAlgo/LinearObject.cxx
  vtkPhantomLinearObjectRegistrationAlgo/LinearObjectBuffer.cxx
  vtkPhantomLinearObjectRegistrationAlgo/Point.cxx
  vtkPhantomLinearObjectRegistrationAlgo/Plane.cxx
  vtkPhantomLinearObjectRegistrationAlgo/Planes.cxx
  vtkPhantomLinearObjectRegistrationAlgo/PointObservation.cxx
  vtkPhantomLinearObjectRegistrationAlgo/PointObservationBuffer.cxx
  vtkPhantomLinearObjectRegistrationAlgo/Reference.cxx
  vtkPhantomLinearObjectRegistrationAlgo/References.cxx
  )

SET (CalibrationAlgo_HDRS)  
IF (WIN32)  
  SET (CalibrationAlgo_HDRS
    vtkProbeCalibrationAlgo/vtkProbeCalibrationAlgo.h
    vtkProbeCalibrationAlgo/vtkProbeCalibrationOptimizerAlgo.h
    vtkPivotCalibrationAlgo/vtkPivotCalibrationAlgo.h
    vtkPhantomLandmarkRegistrationAlgo/vtkPhantomLandmarkRegistrationAlgo.h
    vtkPhantomLinearObjectRegistrationAlgo/vtkPhantomLinearObjectRegistrationAlgo.h
    vtkSpacingCalibAlgo/vtkSpacingCalibAlgo.h 
    vtkCenterOfRotationCalibAlgo/vtkCenterOfRotationCalibAlgo.h
    vtkBrachyStepperPhantomRegistrationAlgo/vtkBrachyStepperPhantomRegistrationAlgo.h
    vtkTemporalCalibrationAlgo/vtkTemporalCalibrationAlgo.h
    vtkTemporalCalibrationAlgo/vtkPrincipalMotionDetectionAlgo.h
    vtkLineSegmentationAlgo/vtkLineSegmentationAlgo.h
    vtkPhantomLinearObjectRegistrationAlgo/Line.h
    vtkPhantomLinearObjectRegistrationAlgo/LinearObject.h
    vtkPhantomLinearObjectRegistrationAlgo/LinearObjectBuffer.h
    vtkPhantomLinearObjectRegistrationAlgo/Point.cxx
    vtkPhantomLinearObjectRegistrationAlgo/Plane.h
    vtkPhantomLinearObjectRegistrationAlgo/Planes.h
    vtkPhantomLinearObjectRegistrationAlgo/PointObservation.h
    vtkPhantomLinearObjectRegistrationAlgo/PointObservationBuffer.h
    vtkPhantomLinearObjectRegistrationAlgo/Reference.h
    vtkPhantomLinearObjectRegistrationAlgo/References.h
    )
ENDIF (WIN32)

SET (CalibrationAlgo_INCLUDE_DIRS 
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkBrachyStepperPhantomRegistrationAlgo
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkCenterOfRotationCalibAlgo
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkLineSegmentationAlgo
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkPhantomLandmarkRegistrationAlgo
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkPhantomLinearObjectRegistrationAlgo
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkPivotCalibrationAlgo
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkProbeCalibrationAlgo
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkSpacingCalibAlgo
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkTemporalCalibrationAlgo 
  CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library

SET (CalibrationAlgo_LIBS 
  vtkCommon 
  vtkInfovis 
  itkvnl 
  itkvnl_algo 
  PatternLocAlgo 
  vtkCharts 
  vtkPlusCommon
  vtkDataCollection
  )

INCLUDE_DIRECTORIES( ${CalibrationAlgo_INCLUDE_DIRS} )
ADD_LIBRARY( vtkCalibrationAlgo STATIC ${CalibrationAlgo_SRCS} ${CalibrationAlgo_HDRS} )
TARGET_LINK_LIBRARIES(vtkCalibrationAlgo ${CalibrationAlgo_LIBS})

# --------------------------------------------------------------------------
# Set external MS project 
SET (PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkCalibrationAlgo CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkCalibrationAlgo vtkCalibrationAlgo;"${PlusLib_BINARY_DIR}/src/CalibrationAlgo/vtkCalibrationAlgo.vcproj";vtkPlusCommon;PatternLocAlgo CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Install
#

# INSTALL(TARGETS vtkCalibrationAlgo
  # DESTINATION bin
  # CONFIGURATIONS Release
  # COMPONENT RuntimeLibraries)
    
# --------------------------------------------------------------------------
# Testing
#

IF (BUILD_TESTING)
  ADD_SUBDIRECTORY (Testing)
ENDIF (BUILD_TESTING)
