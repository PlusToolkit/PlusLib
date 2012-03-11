PROJECT(PlusBroadcasterDaemon)

FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_USE_QT3SUPPORT TRUE) 
INCLUDE(${QT_USE_FILE})

# --------------------------------------------------------------------------
# Sources

SET (PlusBroadcasterDaemon_SRCS
	PlusBroadcasterDaemonMain.cxx
	PlusBroadcasterDaemonMainWindow.cxx
  Widgets/BroadcastedToolEditorDialog.cxx
  Widgets/BroadcastedToolInfoWidget.cxx
	)

if (WIN32)	
	SET (PlusBroadcasterDaemon_SRCS
		${PlusBroadcasterDaemon_SRCS}
		)
endif (WIN32)	

SET (PlusBroadcasterDaemon_UI_HDRS
		PlusBroadcasterDaemonMainWindow.h
    Widgets/BroadcastedToolEditorDialog.h
    Widgets/BroadcastedToolInfoWidget.h
	)

SET (PlusBroadcasterDaemon_UI_SRCS
	PlusBroadcasterDaemonMainWindow.ui
  Widgets/BroadcastedToolEditorDialog.ui
  Widgets/BroadcastedToolInfoWidget.ui 
	)

SET (PlusBroadcasterDaemon_QT_Resources
	PlusBroadcasterDaemon.qrc
	${PlusApp_SOURCE_DIR}/CommonWidgets/CommonWidgets.qrc
	)
	

# --------------------------------------------------------------------------
# Qt specific commands

QT4_WRAP_CPP( PlusBroadcasterDaemon_QT_MOC_Files ${PlusBroadcasterDaemon_UI_HDRS} )			  
QT4_WRAP_UI( PlusBroadcasterDaemon_QT_UI_Headers ${PlusBroadcasterDaemon_UI_SRCS} )
QT4_WRAP_CPP( PlusBroadcasterDaemon_QT_MOC_UI_Files ${PlusBroadcasterDaemon_QT_UI_Headers} )
QT4_ADD_RESOURCES( PlusBroadcasterDaemon_QT_RCC_Files ${PlusBroadcasterDaemon_QT_Resources} )

SOURCE_GROUP("Generated Files" FILES ${PlusBroadcasterDaemon_QT_MOC_Files} ${PlusBroadcasterDaemon_QT_UI_Headers} ${PlusBroadcasterDaemon_QT_MOC_UI_Files} ${PlusBroadcasterDaemon_QT_RCC_Files})
SOURCE_GROUP("UI Files" FILES ${PlusBroadcasterDaemon_UI_SRCS})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Widgets)
INCLUDE_DIRECTORIES(${PlusApp_BINARY_DIR}/CommonWidgets)
INCLUDE_DIRECTORIES(${PlusApp_SOURCE_DIR}/CommonWidgets)

# --------------------------------------------------------------------------
# Build the library

SET (PlusBroadcasterDaemon_LIBS 
    ${PlusBroadcasterDaemon_LIBS} 
    ${QT_LIBRARIES} 
    CommonWidgets
    vtkPlusCommon
    ITKIO
    itkvnl 
    itkvnl_algo 
    vtkCommon 
    vtkRendering 
    vtkHybrid 
    OpenIGTLink
    vtkTracking 
    vtkImageAcquisition 
    vtkDataCollection 
    )

ADD_EXECUTABLE( PlusBroadcasterDaemon ${PlusBroadcasterDaemon_SRCS} ${PlusBroadcasterDaemon_UI_HDRS} ${PlusBroadcasterDaemon_QT_UI_Headers} ${PlusBroadcasterDaemon_QT_UI_Headers} ${PlusBroadcasterDaemon_QT_MOC_Files} ${PlusBroadcasterDaemon_QT_MOC_UI_Files} ${PlusBroadcasterDaemon_QT_RCC_Files})

TARGET_LINK_LIBRARIES( PlusBroadcasterDaemon QVTK ${PlusBroadcasterDaemon_LIBS} )

# --------------------------------------------------------------------------
# Install
#
IF(PLUSAPP_INSTALL_BIN_DIR)
	# show up in the Start Menu in Windows
	IF( WIN32 AND NOT UNIX )
		SET(CPACK_PACKAGE_EXECUTABLES "PlusBroadcasterDaemon" "PlusBroadcasterDaemon")
	ENDIF()

	INSTALL(TARGETS PlusBroadcasterDaemon 
	  DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
	  COMPONENT RuntimeLibraries
	  )
ENDIF(PLUSAPP_INSTALL_BIN_DIR)
