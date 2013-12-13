PROJECT(PlusServerLauncher)

FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_USE_QT3SUPPORT TRUE) 
INCLUDE(${QT_USE_FILE})

# --------------------------------------------------------------------------
# Sources

SET (PlusServerLauncher_SRCS
  PlusServerLauncherMain.cxx
  PlusServerLauncherMainWindow.cxx
  )

if (WIN32)
  SET (PlusServerLauncher_SRCS
    ${PlusServerLauncher_SRCS}
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

QT4_WRAP_CPP( PlusServerLauncher_QT_MOC_Files ${PlusServerLauncher_UI_HDRS} )
QT4_WRAP_UI( PlusServerLauncher_QT_UI_Headers ${PlusServerLauncher_UI_SRCS} )
QT4_WRAP_CPP( PlusServerLauncher_QT_MOC_UI_Files ${PlusServerLauncher_QT_UI_Headers} )
QT4_ADD_RESOURCES( PlusServerLauncher_QT_RCC_Files ${PlusServerLauncher_QT_Resources} )

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

ADD_EXECUTABLE( PlusServerLauncher ${PlusServerLauncher_SRCS} ${PlusServerLauncher_UI_HDRS} ${PlusServerLauncher_QT_UI_Headers} ${PlusServerLauncher_QT_UI_Headers} ${PlusServerLauncher_QT_MOC_Files} ${PlusServerLauncher_QT_MOC_UI_Files} ${PlusServerLauncher_QT_RCC_Files})

TARGET_LINK_LIBRARIES( PlusServerLauncher QVTK ${PlusServerLauncher_LIBS} )

# --------------------------------------------------------------------------
# Install
#
IF(PLUSAPP_INSTALL_BIN_DIR)
  INSTALL(TARGETS PlusServerLauncher 
    DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
    COMPONENT RuntimeLibraries
    )
ENDIF(PLUSAPP_INSTALL_BIN_DIR)

