PROJECT(CommonWidgets)

FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_USE_QT3SUPPORT TRUE)
SET(QT_USE_QTXML TRUE)
INCLUDE(${QT_USE_FILE})

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
  )

SET (CommonWidgets_UI_SRCS
  DeviceSetSelectorWidget.ui
  ConfigFileSaverDialog.ui
  SegmentationParameterDialog.ui
  )	

SET (QT_Resources
	CommonWidgets.qrc
	)
	
# --------------------------------------------------------------------------
# Qt specific commands

QT4_WRAP_CPP( QT_MOC_Files ${CommonWidgets_UI_HDRS} )			  
QT4_WRAP_UI( QT_UI_Headers ${CommonWidgets_UI_SRCS} )
QT4_WRAP_CPP( QT_MOC_UI_Files ${QT_UI_Headers} )
QT4_ADD_RESOURCES( QT_RCC_Files ${QT_Resources} )

SOURCE_GROUP("Generated Files" FILES ${QT_MOC_Files} ${QT_UI_Headers} ${QT_MOC_UI_Files} ${QT_RCC_Files})
SOURCE_GROUP("UI Files" FILES ${CommonWidgets_UI_SRCS})

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Treat wchar_t as built-in type - makes shell executes possible
if (WIN32)	
  ADD_DEFINITIONS(/Zc:wchar_t-)
endif (WIN32)	

# --------------------------------------------------------------------------
# Build the library

SET (CommonWidgets_LIBS 
    ${CommonWidgets_LIBS} 
    ${QT_LIBRARIES} 
    vtkCommon
    vtkPlusCommon
    )

ADD_LIBRARY(CommonWidgets STATIC ${CommonWidgets_SRCS} ${CommonWidgets_UI_HDRS} ${QT_UI_Sources} ${QT_UI_Headers} ${QT_MOC_Files} ${QT_MOC_UI_Files} ${QT_RCC_Files})
TARGET_LINK_LIBRARIES(CommonWidgets QVTK ${CommonWidgets_LIBS})

# --------------------------------------------------------------------------
# Testing
#

IF (BUILD_TESTING)
  ADD_SUBDIRECTORY (Testing)
ENDIF (BUILD_TESTING)
