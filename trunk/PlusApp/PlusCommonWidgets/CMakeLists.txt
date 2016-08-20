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

if (WIN32)
  SET (PlusCommonWidgets_SRCS
    ${PlusCommonWidgets_SRCS}
    )
endif (WIN32)

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

SET( QT_Resources
     PlusCommonWidgets.qrc
    )

# --------------------------------------------------------------------------
# Qt specific commands
FIND_PACKAGE(Qt5 COMPONENTS Xml Widgets REQUIRED)

SET(QT_LIBRARIES ${QT_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Widgets_LIBRARIES})
ADD_DEFINITIONS(${Qt5Widgets_DEFINITIONS} ${Qt5Xml_DEFINITIONS})

IF (WIN32)
  SET_TARGET_PROPERTIES(${TARGET} PROPERTIES COMPILE_FLAGS "/Zc:wchar_t-")
ENDIF (WIN32)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# --------------------------------------------------------------------------
# Build the library
SET (PlusCommonWidgets_LIBS 
    ${PlusCommonWidgets_LIBS} 
    ${QT_LIBRARIES} 
    ${VTK_LIBRARIES}
    vtkPlusCalibration
    vtkPlusCommon
    )

ADD_LIBRARY(PlusCommonWidgets STATIC ${PlusCommonWidgets_SRCS} ${PlusCommonWidgets_UI_HDRS} ${PlusCommonWidgets_UI_SRCS} ${QT_Resources})
TARGET_LINK_LIBRARIES(PlusCommonWidgets  ${VTK_LIBRARIES} ${PlusCommonWidgets_LIBS})

# --------------------------------------------------------------------------
# Testing
#
IF (BUILD_TESTING)
  ADD_SUBDIRECTORY (Testing)
ENDIF (BUILD_TESTING)
