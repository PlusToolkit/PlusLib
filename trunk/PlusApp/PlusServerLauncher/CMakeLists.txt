PROJECT(PlusServerLauncher)

# --------------------------------------------------------------------------
# Sources
SET(PlusServerLauncher_SRCS
  PlusServerLauncherMain.cxx
  PlusServerLauncherMainWindow.cxx
  )

IF(WIN32)
  SET(PlusServerLauncher_SRCS
    ${PlusServerLauncher_SRCS}
    PlusServerLauncher.rc
    )
ENDIF()

SET(PlusServerLauncher_UI_HDRS
  PlusServerLauncherMainWindow.h
  )

SET(PlusServerLauncher_UI_SRCS
  PlusServerLauncherMainWindow.ui 
  )

SET(PlusServerLauncher_QT_Resources
  PlusServerLauncher.qrc
  ${PlusApp_SOURCE_DIR}/PlusCommonWidgets/PlusCommonWidgets.qrc
  )

# --------------------------------------------------------------------------
# Build the library
SET (PlusServerLauncher_LIBS 
  Qt5::Widgets
  Qt5::Network
  vtkPlusCommon
  PlusCommonWidgets
  vtkPlusServer
  )

IF(PLUS_USE_OpenIGTLinkIO)
  find_package(OpenIGTLinkIO REQUIRED)
  SET (PlusServerLauncher_LIBS ${PlusServerLauncher_LIBS}
    igtlioLogic
    )
ENDIF()

SET (APP_MODE MACOSX_BUNDLE)
IF(WIN32)
  # Create a windowed application (do not show a console)
  SET (APP_MODE WIN32)
ENDIF()

ADD_EXECUTABLE( PlusServerLauncher ${APP_MODE} 
  ${PlusServerLauncher_SRCS} 
  ${PlusServerLauncher_UI_HDRS} 
  ${PlusServerLauncher_UI_SRCS} 
  ${PlusServerLauncher_QT_Resources}
  )
SET_TARGET_PROPERTIES(PlusServerLauncher PROPERTIES COMPILE_DEFINTIIONS 
  ${Qt5Widgets_COMPILE_DEFINITIONS} 
  ${Qt5Network_COMPILE_DEFINITIONS}
  )
target_include_directories(PlusServerLauncher PRIVATE 
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${PlusApp_BINARY_DIR}/PlusCommonWidgets
  ${PlusApp_SOURCE_DIR}/PlusCommonWidgets
  )
TARGET_LINK_LIBRARIES( PlusServerLauncher PRIVATE ${PlusServerLauncher_LIBS} )

# --------------------------------------------------------------------------
# Install
IF(PLUSAPP_INSTALL_BIN_DIR)
  INSTALL(TARGETS PlusServerLauncher 
    DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
    COMPONENT RuntimeLibraries
    )
ENDIF()