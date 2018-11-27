PROJECT(PlusCalibration)

# --------------------------------------------------------------------------
# Sources
SET(${PROJECT_NAME}_SRCS
  PatternLocAlgo/PlusFidSegmentation.cxx
  PatternLocAlgo/PlusFidLineFinder.cxx
  PatternLocAlgo/PlusFidLabeling.cxx
  PatternLocAlgo/PlusFidPatternRecognitionCommon.cxx
  PatternLocAlgo/PlusFidPatternRecognition.cxx
  PatternLocAlgo/PlusPatternLocResultFile.cxx
  vtkProbeCalibrationAlgo/vtkPlusProbeCalibrationAlgo.cxx
  vtkProbeCalibrationAlgo/vtkPlusProbeCalibrationOptimizerAlgo.cxx
  vtkPivotCalibrationAlgo/vtkPlusPivotCalibrationAlgo.cxx
  vtkPhantomLandmarkRegistrationAlgo/vtkPlusPhantomLandmarkRegistrationAlgo.cxx
  vtkPhantomLandmarkRegistrationAlgo/vtkPlusLandmarkDetectionAlgo.cxx
  vtkPhantomLandmarkRegistrationAlgo/vtkPlusReadTrackedSignals.cxx
  vtkPhantomLinearObjectRegistrationAlgo/vtkPlusPhantomLinearObjectRegistrationAlgo.cxx
  vtkSpacingCalibAlgo/vtkPlusSpacingCalibAlgo.cxx
  vtkCenterOfRotationCalibAlgo/vtkPlusCenterOfRotationCalibAlgo.cxx
  vtkBrachyStepperPhantomRegistrationAlgo/vtkPlusBrachyStepperPhantomRegistrationAlgo.cxx
  vtkTemporalCalibrationAlgo/vtkPlusTemporalCalibrationAlgo.cxx
  vtkTemporalCalibrationAlgo/vtkPlusPrincipalMotionDetectionAlgo.cxx
  vtkLineSegmentationAlgo/vtkPlusLineSegmentationAlgo.cxx
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

IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  SET(${PROJECT_NAME}_HDRS
    PatternLocAlgo/PlusFidSegmentation.h
    PatternLocAlgo/PlusFidLineFinder.h
    PatternLocAlgo/PlusFidLabeling.h
    PatternLocAlgo/PlusFidPatternRecognitionCommon.h
    PatternLocAlgo/PlusFidPatternRecognition.h
    PatternLocAlgo/PlusPatternLocResultFile.h
    vtkProbeCalibrationAlgo/vtkPlusProbeCalibrationAlgo.h
    vtkProbeCalibrationAlgo/vtkPlusProbeCalibrationOptimizerAlgo.h
    vtkPivotCalibrationAlgo/vtkPlusPivotCalibrationAlgo.h
    vtkPhantomLandmarkRegistrationAlgo/vtkPlusPhantomLandmarkRegistrationAlgo.h
    vtkPhantomLandmarkRegistrationAlgo/vtkPlusLandmarkDetectionAlgo.h
    vtkPhantomLandmarkRegistrationAlgo/vtkPlusReadTrackedSignals.h
    vtkPhantomLinearObjectRegistrationAlgo/vtkPlusPhantomLinearObjectRegistrationAlgo.h
    vtkSpacingCalibAlgo/vtkPlusSpacingCalibAlgo.h
    vtkCenterOfRotationCalibAlgo/vtkPlusCenterOfRotationCalibAlgo.h
    vtkBrachyStepperPhantomRegistrationAlgo/vtkPlusBrachyStepperPhantomRegistrationAlgo.h
    vtkTemporalCalibrationAlgo/vtkPlusTemporalCalibrationAlgo.h
    vtkTemporalCalibrationAlgo/vtkPlusPrincipalMotionDetectionAlgo.h
    vtkLineSegmentationAlgo/vtkPlusLineSegmentationAlgo.h
    vtkPhantomLinearObjectRegistrationAlgo/Line.h
    vtkPhantomLinearObjectRegistrationAlgo/LinearObject.h
    vtkPhantomLinearObjectRegistrationAlgo/LinearObjectBuffer.h
    vtkPhantomLinearObjectRegistrationAlgo/Plane.h
    vtkPhantomLinearObjectRegistrationAlgo/Planes.h
    vtkPhantomLinearObjectRegistrationAlgo/PointObservation.h
    vtkPhantomLinearObjectRegistrationAlgo/PointObservationBuffer.h
    vtkPhantomLinearObjectRegistrationAlgo/Reference.h
    vtkPhantomLinearObjectRegistrationAlgo/References.h
    )
ENDIF()

SET(${PROJECT_NAME}_INCLUDE_DIRS
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/PatternLocAlgo
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
SET(${PROJECT_NAME}_LIBS
  ITKCommon
  ITKIOImageBase
  ITKOptimizers
  ITKTransform
  vtkInteractionStyle
  vtkRenderingFreeType
  vtkFiltersStatistics
  vtkPlusRendering
  vtkPlusCommon
  vtkIGSIOCommon
  )

GENERATE_EXPORT_DIRECTIVE_FILE(vtk${PROJECT_NAME})
ADD_LIBRARY(vtk${PROJECT_NAME} ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_HDRS})
FOREACH(p IN LISTS ${PROJECT_NAME}_INCLUDE_DIRS)
  target_include_directories(vtk${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${p}>)
ENDFOREACH()
target_include_directories(vtk${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${PLUSLIB_INCLUDE_INSTALL}>)
TARGET_LINK_LIBRARIES(vtk${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_LIBS})
PlusLibAddVersionInfo(vtk${PROJECT_NAME} "Library containing various calibration algorithms. Part of the Plus toolkit." vtk${PROJECT_NAME} vtk${PROJECT_NAME})

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtk${PROJECT_NAME} CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtk${PROJECT_NAME} vtk${PROJECT_NAME};${PlusLib_BINARY_DIR}/src/${PROJECT_NAME}/vtk${PROJECT_NAME}.vcxproj;vtkPlusCommon CACHE INTERNAL "" FORCE)

IF(PLUSBUILD_BUILD_PlusLib_TOOLS)
  #--------------------------------------------------------------------------------------------
  ADD_EXECUTABLE( ConvertFcsvToXml
    Tools/ConvertFcsvToXml.cxx
    )
  SET_TARGET_PROPERTIES(ConvertFcsvToXml PROPERTIES FOLDER Tools)
  # Link the executable to the algo library.
  TARGET_LINK_LIBRARIES( ConvertFcsvToXml
    vtk${PROJECT_NAME}
    ITKCommon
    vtkPlusDataCollection
    )

  #--------------------------------------------------------------------------------------------
  ADD_EXECUTABLE( ConvertXmlToFcsv
    Tools/ConvertXmlToFcsv.cxx
    )
  SET_TARGET_PROPERTIES(ConvertXmlToFcsv PROPERTIES FOLDER Tools)
  # Link the executable to the algo library.
  TARGET_LINK_LIBRARIES( ConvertXmlToFcsv
    vtk${PROJECT_NAME}
    ITKCommon
    vtkPlusDataCollection
    )

  #--------------------------------------------------------------------------------------------
  ADD_EXECUTABLE(TemporalCalibration Tools/TemporalCalibration.cxx)
  SET_TARGET_PROPERTIES(TemporalCalibration PROPERTIES FOLDER Tools)
  TARGET_LINK_LIBRARIES(TemporalCalibration vtkPlusCommon vtk${PROJECT_NAME} vtkPlusDataCollection)
  GENERATE_HELP_DOC(TemporalCalibration)

  #--------------------------------------------------------------------------------------------
  ADD_EXECUTABLE(ProbeCalibration Tools/ProbeCalibration.cxx)
  SET_TARGET_PROPERTIES(ProbeCalibration PROPERTIES FOLDER Tools)
  TARGET_LINK_LIBRARIES(ProbeCalibration itkvnl itkvnl_algo vtk${PROJECT_NAME} vtkPlusDataCollection )
  GENERATE_HELP_DOC(ProbeCalibration)

  INSTALL(
    TARGETS
      ConvertFcsvToXml
      ConvertXmlToFcsv
      TemporalCalibration
      ProbeCalibration
    EXPORT PlusLib
    DESTINATION "${PLUSLIB_BINARY_INSTALL}"
    COMPONENT RuntimeExecutables
    )
ENDIF()

# --------------------------------------------------------------------------
# Install
#
PlusLibInstallLibrary(vtk${PROJECT_NAME} ${PROJECT_NAME})
  
# --------------------------------------------------------------------------
# Testing
#
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY (Testing)
ENDIF()