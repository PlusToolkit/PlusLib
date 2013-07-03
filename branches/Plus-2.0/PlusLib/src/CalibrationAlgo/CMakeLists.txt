# --------------------------------------------------------------------------
# Sources

SET (CalibrationAlgo_SRCS
  vtkProbeCalibrationAlgo.cxx 
  vtkMeanShiftClustering.cxx 
  vtkPivotCalibrationAlgo.cxx
  vtkPhantomLandmarkRegistrationAlgo.cxx
  vtkPhantomLinearObjectRegistrationAlgo.cxx
  vtkSpacingCalibAlgo.cxx 
  vtkCenterOfRotationCalibAlgo.cxx
  vtkBrachyStepperPhantomRegistrationAlgo.cxx
  TemporalCalibrationAlgo.cxx
  vtkLineSegmentationAlgo.cxx 
  vtkPrincipalMotionDetectionAlgo.cxx 
  vtkProbeCalibrationOptimizerAlgo.cxx
  itkFiducialTransformComputation.cxx
  vtkSingleWallCalibrationAlgo.cxx
  Line.cxx
  LinearObject.cxx
  LinearObjectBuffer.cxx
  Plane.cxx
  Planes.cxx
  Point.cxx
  PointObservation.cxx
  PointObservationBuffer.cxx
  Reference.cxx
  )

SET (CalibrationAlgo_HDRS)  
IF (WIN32)  
  SET (CalibrationAlgo_HDRS
    vtkProbeCalibrationAlgo.h
    vtkMeanShiftClustering.h
    vtkPivotCalibrationAlgo.h
    vtkPhantomLandmarkRegistrationAlgo.h
    vtkPhantomLinearObjectRegistrationAlgo.h
    vtkSpacingCalibAlgo.h 
    vtkCenterOfRotationCalibAlgo.h
    vtkBrachyStepperPhantomRegistrationAlgo.h
    TemporalCalibrationAlgo.h
    vtkLineSegmentationAlgo.h
    vtkPrincipalMotionDetectionAlgo.h
    vtkProbeCalibrationOptimizerAlgo.h
    itkFiducialTransformComputation.h
    vtkSingleWallCalibrationAlgo.h
    Line.h
	LinearObject.h
	LinearObjectBuffer.h
	Plane.h
	Planes.h
	Point.h
	PointObservation.h
	PointObservationBuffer.h
	Reference.h
    )
ENDIF (WIN32)

SET (CalibrationAlgo_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "" FORCE)

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
