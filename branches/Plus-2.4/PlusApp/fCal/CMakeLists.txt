PROJECT(fCal)

# --------------------------------------------------------------------------
# Sources

SET (fCal_SRCS
  fCalMain.cxx
  fCalMainWindow.cxx
  Toolboxes/ConfigurationToolbox.cxx
  Toolboxes/CapturingToolbox.cxx
  Toolboxes/SpatialCalibrationToolbox.cxx
  Toolboxes/TemporalCalibrationToolbox.cxx
  Toolboxes/StylusCalibrationToolbox.cxx
  Toolboxes/PhantomRegistrationToolbox.cxx
  Toolboxes/VolumeReconstructionToolbox.cxx
  )

if (WIN32)  
  SET (fCal_SRCS
    ${fCal_SRCS}
    fCal.rc
    )
endif (WIN32)  

SET (fCal_UI_HDRS
    fCalMainWindow.h
    Toolboxes/AbstractToolbox.h
    Toolboxes/ConfigurationToolbox.h
    Toolboxes/CapturingToolbox.h
    Toolboxes/SpatialCalibrationToolbox.h
    Toolboxes/TemporalCalibrationToolbox.h
    Toolboxes/StylusCalibrationToolbox.h
    Toolboxes/PhantomRegistrationToolbox.h
    Toolboxes/VolumeReconstructionToolbox.h
  )

SET (fCal_UI_SRCS
  fCalMainWindow.ui 
  Toolboxes/ConfigurationToolbox.ui
  Toolboxes/CapturingToolbox.ui
  Toolboxes/SpatialCalibrationToolbox.ui
  Toolboxes/TemporalCalibrationToolbox.ui
  Toolboxes/StylusCalibrationToolbox.ui
  Toolboxes/PhantomRegistrationToolbox.ui
  Toolboxes/VolumeReconstructionToolbox.ui
  )

SET (fCal_QT_Resources
  fCal.qrc
  ${PlusApp_SOURCE_DIR}/PlusCommonWidgets/PlusCommonWidgets.qrc
  )
  

# --------------------------------------------------------------------------
# Qt specific commands
IF( ${QT_VERSION_MAJOR} EQUAL 5 )
  FIND_PACKAGE(Qt5Widgets REQUIRED)

  INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDES})
  ADD_DEFINITIONS(${Qt5Widgets_DEFINITIONS})

  SET(QT_LIBRARIES ${Qt5Widgets_LIBRARIES} ${Qt5Core_QTMAIN_LIBRARIES})
ELSE(${QT_VERSION_MAJOR} EQUAL 5)
  FIND_PACKAGE(Qt4 REQUIRED)
  INCLUDE(${QT_USE_FILE})
  
  INCLUDE_DIRECTORIES(${Qt4_INCLUDES})
  ADD_DEFINITIONS(${Qt4_DEFINITIONS})
ENDIF(${QT_VERSION_MAJOR} EQUAL 5)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Toolboxes)
include_directories(${PlusApp_BINARY_DIR}/PlusCommonWidgets)
include_directories(${PlusApp_SOURCE_DIR}/PlusCommonWidgets)

# --------------------------------------------------------------------------
# Build the library
SET (fCal_LIBS 
    ${fCal_LIBS} 
    ${QT_LIBRARIES} 
    PlusCommonWidgets
    vtkPlusCommon
    ${ITK_LIBRARIES}
    itkvnl 
    itkvnl_algo 
    ${VTK_LIBRARIES}
    vtkPlusCalibration 
    vtkPlusDataCollection 
    vtkPlusVolumeReconstruction
    )

IF (PLUS_USE_OpenIGTLink)
  SET (fCal_LIBS ${fCal_LIBS} OpenIGTLink)
ENDIF(PLUS_USE_OpenIGTLink)

ADD_EXECUTABLE( fCal MACOSX_BUNDLE ${fCal_SRCS} ${fCal_UI_HDRS} ${fCal_UI_SRCS} ${fCal_QT_Resources})
TARGET_LINK_LIBRARIES( fCal  ${VTK_LIBRARIES} ${fCal_LIBS} )

# --------------------------------------------------------------------------
# Install
#
IF(PLUSAPP_INSTALL_BIN_DIR)

  INSTALL(TARGETS fCal 
    DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
    COMPONENT RuntimeLibraries
    )

ENDIF(PLUSAPP_INSTALL_BIN_DIR)
