PROJECT(CaptureClient)

FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

# --------------------------------------------------------------------------
# Sources

SET (CaptureClient_SRCS
  CaptureClientMain.cxx 
  CaptureClientWindow.cxx 
  )

SET (CaptureClient_UI_HDRS
    CaptureClientWindow.h 
  )

SET (CaptureClient_UI_SRCS
  CaptureClientWindow.ui 
  )

SET (CaptureClient_QT_Resources
  CaptureClient.qrc
  ${PlusApp_SOURCE_DIR}/CommonWidgets/CommonWidgets.qrc
  )
  

# --------------------------------------------------------------------------
# Qt specific commands

QT4_WRAP_CPP( CaptureClient_QT_MOC_Files ${CaptureClient_UI_HDRS} )        
QT4_WRAP_UI( CaptureClient_QT_UI_Headers ${CaptureClient_UI_SRCS} )
QT4_WRAP_CPP( CaptureClient_QT_MOC_UI_Files ${CaptureClient_QT_UI_Headers} )
QT4_ADD_RESOURCES( CaptureClient_QT_RCC_Files ${CaptureClient_QT_Resources} )

SOURCE_GROUP("Generated Files" FILES ${CaptureClient_QT_MOC_Files} ${CaptureClient_QT_UI_Headers} ${CaptureClient_QT_MOC_UI_Files} ${CaptureClient_QT_RCC_Files})
SOURCE_GROUP("UI Files" FILES ${CaptureClient_UI_SRCS})

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Toolboxes)
include_directories(${PlusApp_BINARY_DIR}/CommonWidgets)
include_directories(${PlusApp_SOURCE_DIR}/CommonWidgets)

# --------------------------------------------------------------------------
# Build the library

SET (CaptureClient_LIBS 
    ${CaptureClient_LIBS} 
    ${QT_LIBRARIES} 
    CommonWidgets
    vtkPlusCommon
    vtkDataCollection 
    )

IF (PLUS_USE_OpenIGTLink)
    SET (CaptureClient_LIBS ${CaptureClient_LIBS} OpenIGTLink)
ENDIF(PLUS_USE_OpenIGTLink)

ADD_EXECUTABLE( CaptureClient MACOSX_BUNDLE ${CaptureClient_SRCS} ${CaptureClient_UI_HDRS} ${CaptureClient_QT_UI_Headers} ${CaptureClient_QT_UI_Headers} ${CaptureClient_QT_MOC_Files} ${CaptureClient_QT_MOC_UI_Files} ${CaptureClient_QT_RCC_Files})

TARGET_LINK_LIBRARIES( CaptureClient QVTK ${CaptureClient_LIBS} )

# --------------------------------------------------------------------------
# Install
#
IF(PLUSAPP_INSTALL_BIN_DIR)
# show up in the Start Menu in Windows
if( WIN32 AND NOT UNIX )
    SET(CPACK_PACKAGE_EXECUTABLES "CaptureClient" "CaptureClient")
endif()

INSTALL(TARGETS CaptureClient 
  DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
  COMPONENT RuntimeLibraries
  )
  
ENDIF(PLUSAPP_INSTALL_BIN_DIR)
