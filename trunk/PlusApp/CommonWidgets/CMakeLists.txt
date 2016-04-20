PROJECT(CommonWidgets)

# --------------------------------------------------------------------------
# Sources

SET (CommonWidgets_SRCS
  DeviceSetSelectorWidget.cxx
  StatusIcon.cxx
  ToolStateDisplayWidget.cxx
  ConfigFileSaverDialog.cxx
  SegmentationParameterDialog.cxx
  vtkVisualizationController.cxx
  vtkDisplayableObject.cxx
  vtkImageVisualizer.cxx
  vtk3DObjectVisualizer.cxx
  CaptureControlWidget.cxx 
  QCustomAction.cxx 
  )

if (WIN32)
  SET (CommonWidgets_SRCS
    ${CommonWidgets_SRCS}
    )
endif (WIN32)

SET (CommonWidgets_UI_HDRS
    DeviceSetSelectorWidget.h
    StatusIcon.h
    ToolStateDisplayWidget.h
    ConfigFileSaverDialog.h
    SegmentationParameterDialog.h
    vtkVisualizationController.h
    vtkDisplayableObject.h
    vtkImageVisualizer.h
    vtk3DObjectVisualizer.h
    CaptureControlWidget.h 
    QCustomAction.h 
  )

SET (CommonWidgets_UI_SRCS
  DeviceSetSelectorWidget.ui
  ConfigFileSaverDialog.ui
  SegmentationParameterDialog.ui
  CaptureControlWidget.ui 
  )

SET( QT_Resources
     CommonWidgets.qrc
    )

# --------------------------------------------------------------------------
# Qt specific commands
IF( ${QT_VERSION_MAJOR} EQUAL 5 )
  FIND_PACKAGE(Qt5 COMPONENTS Xml Widgets REQUIRED)

  SET(QT_LIBRARIES ${QT_LIBRARIES} ${Qt5Xml_LIBRARIES})
  ADD_DEFINITIONS(${Qt5Widgets_DEFINITIONS} ${Qt5Xml_DEFINITIONS})

  IF (WIN32)
    SET_TARGET_PROPERTIES(${TARGET} PROPERTIES COMPILE_FLAGS "/Zc:wchar_t-")
  ENDIF (WIN32)

ELSE(${QT_VERSION_MAJOR} EQUAL 5)
  FIND_PACKAGE(Qt4 REQUIRED)
  SET(QT_USE_QTXML TRUE)
  INCLUDE(${QT_USE_FILE})

  INCLUDE_DIRECTORIES(${QT_INCLUDES})
  ADD_DEFINITIONS(${QT_DEFINITIONS})
  
  # Treat wchar_t as built-in type - makes shell executes possible
  IF (WIN32)
    ADD_DEFINITIONS(/Zc:wchar_t-)
  ENDIF (WIN32)
ENDIF(${QT_VERSION_MAJOR} EQUAL 5)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# --------------------------------------------------------------------------
# Build the library
SET (CommonWidgets_LIBS 
    ${CommonWidgets_LIBS} 
    ${QT_LIBRARIES} 
    ${VTK_LIBRARIES}
    vtkPlusCalibration
    vtkPlusCommon
    )

IF (${VTK_VERSION_MAJOR} LESS 6)
  SET (CommonWidgets_LIBS ${CommonWidgets_LIBS}
    QVTK
    )
ENDIF()

ADD_LIBRARY(CommonWidgets STATIC ${CommonWidgets_SRCS} ${CommonWidgets_UI_HDRS} ${CommonWidgets_UI_SRCS} ${QT_Resources})
TARGET_LINK_LIBRARIES(CommonWidgets  ${VTK_LIBRARIES} ${CommonWidgets_LIBS})

# --------------------------------------------------------------------------
# Testing
#
IF (BUILD_TESTING)
  ADD_SUBDIRECTORY (Testing)
ENDIF (BUILD_TESTING)
