PROJECT(fCal)

# --------------------------------------------------------------------------
# Sources
SET (fCal_SRCS
  fCalMain.cxx
  fCalMainWindow.cxx
  PlusSegmentationParameterDialog.cxx
  vtkPlusVisualizationController.cxx
  vtkPlusDisplayableObject.cxx
  vtkPlusImageVisualizer.cxx
  vtkPlus3DObjectVisualizer.cxx
  PlusCaptureControlWidget.cxx 
  QCustomAction.cxx 
  )

SET(fCal_Toolbox_SRCS
  Toolboxes/QConfigurationToolbox.cxx
  Toolboxes/QCapturingToolbox.cxx
  Toolboxes/QSpatialCalibrationToolbox.cxx
  Toolboxes/QTemporalCalibrationToolbox.cxx
  Toolboxes/QStylusCalibrationToolbox.cxx
  Toolboxes/QPhantomRegistrationToolbox.cxx
  Toolboxes/QVolumeReconstructionToolbox.cxx
  )

IF(WIN32)  
  LIST(APPEND fCal.rc)
ENDIF()  

SET (fCal_UI_HDRS
  fCalMainWindow.h
  PlusSegmentationParameterDialog.h
  vtkPlusVisualizationController.h
  vtkPlusDisplayableObject.h
  vtkPlusImageVisualizer.h
  vtkPlus3DObjectVisualizer.h
  PlusCaptureControlWidget.h 
  QCustomAction.h 
  )

SET (fCal_Toolbox_UI_HDRS
  Toolboxes/QAbstractToolbox.h
  Toolboxes/QConfigurationToolbox.h
  Toolboxes/QCapturingToolbox.h
  Toolboxes/QSpatialCalibrationToolbox.h
  Toolboxes/QTemporalCalibrationToolbox.h
  Toolboxes/QStylusCalibrationToolbox.h
  Toolboxes/QPhantomRegistrationToolbox.h
  Toolboxes/QVolumeReconstructionToolbox.h
  )

SET (fCal_UI_SRCS
  fCalMainWindow.ui 
  Toolboxes/QConfigurationToolbox.ui
  Toolboxes/QCapturingToolbox.ui
  Toolboxes/QSpatialCalibrationToolbox.ui
  Toolboxes/QTemporalCalibrationToolbox.ui
  Toolboxes/QStylusCalibrationToolbox.ui
  Toolboxes/QPhantomRegistrationToolbox.ui
  Toolboxes/QVolumeReconstructionToolbox.ui
  PlusSegmentationParameterDialog.ui
  PlusCaptureControlWidget.ui 
  )

SET (fCal_QT_Resources
  fCal.qrc
  )

# --------------------------------------------------------------------------
# Build the library
SET(fCal_LIBS 
  Qt5::Widgets
  Qt5::Core
  Qt5::Xml
  PlusWidgets
  vtkPlusCommon
  itkvnl 
  itkvnl_algo 
  vtkPlusCalibration 
  vtkPlusDataCollection 
  vtkPlusVolumeReconstruction
  vtkGUISupportQt
  )
IF(TARGET vtkRenderingGL2PS${VTK_RENDERING_BACKEND})
  LIST(APPEND fCal_LIBS
    vtkRenderingGL2PS${VTK_RENDERING_BACKEND}
    )
ELSEIF(TARGET vtkRenderingGL2PS)
  LIST(APPEND fCal_LIBS
    vtkRenderingGL2PS
    )
ENDIF()

IF (PLUS_USE_OpenIGTLink)
  LIST(APPEND fCal_LIBS OpenIGTLink)
ENDIF()

SET(APP_MODE MACOSX_BUNDLE)
IF(WIN32)
  SET(APP_MODE WIN32)
ENDIF()

ADD_EXECUTABLE( fCal ${APP_MODE} 
  ${fCal_SRCS} 
  ${fCal_Toolbox_SRCS}
  ${fCal_UI_HDRS} 
  ${fCal_Toolbox_UI_HDRS}
  ${fCal_UI_SRCS} 
  ${fCal_QT_Resources}
  )
SET_TARGET_PROPERTIES(fCal PROPERTIES COMPILE_DEFINTIIONS ${Qt5Widgets_DEFINITIONS})
TARGET_LINK_LIBRARIES(fCal PUBLIC ${fCal_LIBS})
target_include_directories(fCal PUBLIC 
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/Toolboxes
  )
source_group(Toolboxes FILES ${fCal_Toolbox_SRCS} ${fCal_Toolbox_UI_HDRS})
source_group("UI Files" FILES ${fCal_UI_SRCS})

# --------------------------------------------------------------------------
# Install
IF(PLUSAPP_INSTALL_BIN_DIR)
  INSTALL(TARGETS fCal 
    DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
    COMPONENT RuntimeLibraries
    )
ENDIF()

# --------------------------------------------------------------------------
# Testing
IF (BUILD_TESTING)
  ADD_SUBDIRECTORY (Testing)
ENDIF()