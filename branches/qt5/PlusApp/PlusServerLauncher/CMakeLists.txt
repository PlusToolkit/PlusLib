PROJECT(PlusServerLauncher)

IF( ${QT_MAJOR_VERSION} EQUAL 4 )
  FIND_PACKAGE(Qt4 REQUIRED)
  INCLUDE(${QT_USE_FILE})
ENDIF()

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
  )

SET (PlusServerLauncher_QT_Resources
  ${PlusApp_SOURCE_DIR}/CommonWidgets/CommonWidgets.qrc
  )

# --------------------------------------------------------------------------
# Qt specific commands
IF( ${QT_MAJOR_VERSION} EQUAL 5 )
  qt5_wrap_cpp( PlusServerLauncher_QT_MOC_Files ${PlusServerLauncher_UI_HDRS} )
  qt5_wrap_ui( PlusServerLauncher_QT_UI_Headers ${PlusServerLauncher_UI_SRCS} )
  qt5_wrap_cpp( PlusServerLauncher_QT_MOC_UI_Files ${PlusServerLauncher_QT_UI_Headers} )
  qt5_add_resources(PlusServerLauncher_QT_RCC_Files, ${PlusServerLauncher_QT_Resources} )
  
  INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDES})
  ADD_DEFINITIONS(${Qt5Widgets_DEFINITIONS})
  
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
ELSE(${QT_MAJOR_VERSION} EQUAL 5)
  QT4_WRAP_CPP( PlusServerLauncher_QT_MOC_Files ${PlusServerLauncher_UI_HDRS} ) 
  QT4_WRAP_UI( PlusServerLauncher_QT_UI_Headers ${PlusServerLauncher_UI_SRCS} )
  QT4_WRAP_CPP( PlusServerLauncher_QT_MOC_UI_Files ${PlusServerLauncher_QT_UI_Headers} )
  QT4_ADD_RESOURCES( PlusServerLauncher_QT_RCC_Files ${PlusServerLauncher_QT_Resources} )
ENDIF(${QT_MAJOR_VERSION} EQUAL 5)

SOURCE_GROUP("Generated Files" FILES ${PlusServerLauncher_QT_MOC_Files} ${PlusServerLauncher_QT_UI_Headers} ${PlusServerLauncher_QT_MOC_UI_Files} ${PlusServerLauncher_QT_RCC_Files})
SOURCE_GROUP("UI Files" FILES ${PlusServerLauncher_UI_SRCS})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${PlusApp_BINARY_DIR}/CommonWidgets)
INCLUDE_DIRECTORIES(${PlusApp_SOURCE_DIR}/CommonWidgets)

# --------------------------------------------------------------------------
# Build the library

SET (PlusServerLauncher_LIBS 
    ${PlusServerLauncher_LIBS} 
    ${QT_LIBRARIES} 
    CommonWidgets
    vtkPlusServer    
    )

SET (APP_MODE "")
IF(${WIN32})
  # Create a windowed application (do not show a console)
  SET (APP_MODE "WIN32")
ENDIF()

ADD_EXECUTABLE( PlusServerLauncher ${APP_MODE} ${PlusServerLauncher_SRCS} ${PlusServerLauncher_UI_HDRS} ${PlusServerLauncher_QT_UI_Headers} ${PlusServerLauncher_QT_UI_Headers} ${PlusServerLauncher_QT_MOC_Files} ${PlusServerLauncher_QT_MOC_UI_Files} ${PlusServerLauncher_QT_RCC_Files})
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

