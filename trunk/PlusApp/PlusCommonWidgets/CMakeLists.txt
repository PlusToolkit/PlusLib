PROJECT(PlusCommonWidgets)

# --------------------------------------------------------------------------
# Sources
SET (PlusCommonWidgets_SRCS
  PlusDeviceSetSelectorWidget.cxx
  PlusStatusIcon.cxx
  PlusToolStateDisplayWidget.cxx
  PlusConfigFileSaverDialog.cxx
  PlusSegmentationParameterDialog.cxx
  vtkPlusVisualizationController.cxx
  vtkPlusDisplayableObject.cxx
  vtkPlusImageVisualizer.cxx
  vtkPlus3DObjectVisualizer.cxx
  PlusCaptureControlWidget.cxx 
  QCustomAction.cxx 
  )

SET (PlusCommonWidgets_UI_HDRS
  PlusDeviceSetSelectorWidget.h
  PlusStatusIcon.h
  PlusToolStateDisplayWidget.h
  PlusConfigFileSaverDialog.h
  PlusSegmentationParameterDialog.h
  vtkPlusVisualizationController.h
  vtkPlusDisplayableObject.h
  vtkPlusImageVisualizer.h
  vtkPlus3DObjectVisualizer.h
  PlusCaptureControlWidget.h 
  QCustomAction.h 
  )

SET (PlusCommonWidgets_UI_SRCS
  PlusDeviceSetSelectorWidget.ui
  PlusConfigFileSaverDialog.ui
  PlusSegmentationParameterDialog.ui
  PlusCaptureControlWidget.ui 
  )

SET(QT_Resources PlusCommonWidgets.qrc)

# --------------------------------------------------------------------------
# Build the library
SET (PlusCommonWidgets_LIBS 
  Qt5::Xml
  Qt5::Widgets
  vtkPlusCalibration
  vtkPlusCommon
  vtkPlusDataCollection
  )

ADD_LIBRARY(PlusCommonWidgets STATIC
  ${PlusCommonWidgets_SRCS} 
  ${PlusCommonWidgets_UI_HDRS} 
  ${PlusCommonWidgets_UI_SRCS} 
  ${QT_Resources}
  )
SET_TARGET_PROPERTIES(PlusCommonWidgets PROPERTIES 
  COMPILE_DEFINTIIONS 
    ${Qt5Widgets_COMPILE_DEFINITIONS} 
    ${Qt5Xml_COMPILE_DEFINITIONS}
  )
TARGET_LINK_LIBRARIES(PlusCommonWidgets PUBLIC ${PlusCommonWidgets_LIBS})
target_include_directories(PlusCommonWidgets PUBLIC 
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

# --------------------------------------------------------------------------
# Testing
IF (BUILD_TESTING)
  ADD_SUBDIRECTORY (Testing)
ENDIF()