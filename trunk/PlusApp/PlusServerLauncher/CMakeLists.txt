PROJECT(PlusServerLauncher)

# --------------------------------------------------------------------------
# Sources

SET (PlusServerLauncher_SRCS
  PlusServerLauncherMain.cxx
  PlusServerLauncherMainWindow.cxx
  )

if (WIN32)
  SET (PlusServerLauncher_SRCS
    ${PlusServerLauncher_SRCS}
    PlusServerLauncher.rc
    )
endif (WIN32)

SET (PlusServerLauncher_UI_HDRS
  PlusServerLauncherMainWindow.h
  )

SET (PlusServerLauncher_UI_SRCS
  PlusServerLauncherMainWindow.ui 
  )

SET (PlusServerLauncher_QT_Resources
  PlusServerLauncher.qrc
  ${PlusApp_SOURCE_DIR}/PlusCommonWidgets/PlusCommonWidgets.qrc
  )

# --------------------------------------------------------------------------
# Qt specific commands
IF( ${QT_VERSION_MAJOR} EQUAL 5 )
  FIND_PACKAGE(Qt5 COMPONENTS Network Widgets REQUIRED)

  SET(QT_LIBRARIES ${QT_LIBRARIES} ${Qt5Network_LIBRARIES})
  ADD_DEFINITIONS(${Qt5Widgets_DEFINITIONS} ${Qt5Network_DEFINITIONS})
ELSE(${QT_VERSION_MAJOR} EQUAL 5)
  FIND_PACKAGE(Qt4 REQUIRED)
  INCLUDE(${QT_USE_FILE})

  INCLUDE_DIRECTORIES(${QT_INCLUDES})
  ADD_DEFINITIONS(${QT_DEFINITIONS})
ENDIF(${QT_VERSION_MAJOR} EQUAL 5)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${PlusApp_BINARY_DIR}/PlusCommonWidgets)
INCLUDE_DIRECTORIES(${PlusApp_SOURCE_DIR}/PlusCommonWidgets)

# --------------------------------------------------------------------------
# Build the library

SET (PlusServerLauncher_LIBS 
    ${PlusServerLauncher_LIBS} 
    ${QT_LIBRARIES} 
    PlusCommonWidgets
    vtkPlusServer    
    )

SET (APP_MODE "")
IF(${WIN32})
  # Create a windowed application (do not show a console)
  SET (APP_MODE "WIN32")
ENDIF()

ADD_EXECUTABLE( PlusServerLauncher ${APP_MODE} ${PlusServerLauncher_SRCS} ${PlusServerLauncher_UI_HDRS} ${PlusServerLauncher_UI_SRCS} ${PlusServerLauncher_QT_Resources})
TARGET_LINK_LIBRARIES( PlusServerLauncher ${VTK_LIBRARIES} ${PlusServerLauncher_LIBS} )

# --------------------------------------------------------------------------
# Install
#
IF(PLUSAPP_INSTALL_BIN_DIR)
  INSTALL(TARGETS PlusServerLauncher 
    DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
    COMPONENT RuntimeLibraries
    )
ENDIF(PLUSAPP_INSTALL_BIN_DIR)

