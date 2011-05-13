FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_USE_QT3SUPPORT TRUE) 
INCLUDE(${QT_USE_FILE})

# --------------------------------------------------------------------------
# Sources

SET (TrackedUltrasoundCapturing_SRCS
	TrackedUltrasoundCapturing.cxx
	TrackedUltrasoundCapturingMain.cxx 
	TrackedUltrasoundCapturingGUI.cxx
	)

if (WIN32)	
	SET (TrackedUltrasoundCapturing_SRCS
		${TrackedUltrasoundCapturing_SRCS}
		TrackedUltrasoundCapturing.h
		)
endif (WIN32)	

SET (TrackedUltrasoundCapturing_UI_HDRS
	TrackedUltrasoundCapturingGUI.h  
	)

SET (TrackedUltrasoundCapturing_UI_SRCS
	TrackedUltrasoundCapturing.ui 
	)
	
# --------------------------------------------------------------------------
# Qt specific commands

QT4_WRAP_CPP( QT_MOC_Files ${TrackedUltrasoundCapturing_UI_HDRS} )			  
QT4_WRAP_UI( QT_UI_Headers ${TrackedUltrasoundCapturing_UI_SRCS} )
QT4_WRAP_CPP( QT_MOC_UI_Files ${QT_UI_Headers} )	

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# --------------------------------------------------------------------------
# Build the library

SET (TrackedUltrasoundCapturing_LIBS 
    ${TrackedUltrasoundCapturing_LIBS} 
    ${QT_LIBRARIES} 
    vtkCommon
    vtkFiltering
    vtkHybrid
    vtkImageAcquisition
    ITKIO
    vtkTracking
    vtkDataCollection
    vtkPlusCommon
    )

IF (PLUS_USE_OpenIGTLink)
    SET (TrackedUltrasoundCapturing_LIBS ${TrackedUltrasoundCapturing_LIBS} OpenIGTLink)
ENDIF(PLUS_USE_OpenIGTLink)
    
ADD_EXECUTABLE( TrackedUltrasoundCapturing MACOSX_BUNDLE ${TrackedUltrasoundCapturing_SRCS} ${TrackedUltrasoundCapturing_UI_HDRS} ${QT_UI_Sources} ${QT_UI_Headers} ${QT_MOC_Files} ${QT_MOC_UI_Files})

TARGET_LINK_LIBRARIES( TrackedUltrasoundCapturing QVTK ${TrackedUltrasoundCapturing_LIBS} )

# --------------------------------------------------------------------------
# Install
#

# show up in the Start Menu in Windows
if( WIN32 AND NOT UNIX )
    SET(CPACK_PACKAGE_EXECUTABLES "TrackedUltrasoundCapturing" "TrackedUltrasoundCapturing")
endif()

INSTALL(FILES 
	${QT_BINARY_DIR}/QtCore4${CMAKE_SHARED_LIBRARY_SUFFIX}
	${QT_BINARY_DIR}/QtGui4${CMAKE_SHARED_LIBRARY_SUFFIX}
	${QT_BINARY_DIR}/QtNetwork4${CMAKE_SHARED_LIBRARY_SUFFIX}
	${QT_BINARY_DIR}/QtSql4${CMAKE_SHARED_LIBRARY_SUFFIX}
	${QT_BINARY_DIR}/QtWebKit4${CMAKE_SHARED_LIBRARY_SUFFIX}
	${QT_BINARY_DIR}/QtXmlPatterns4${CMAKE_SHARED_LIBRARY_SUFFIX}
	${QT_BINARY_DIR}/phonon4${CMAKE_SHARED_LIBRARY_SUFFIX}
	DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
	COMPONENT RuntimeLibraries
  )

INSTALL(FILES 
	${PLUSLIB_DATA_DIR}/ConfigFiles/USDataCollectionConfig.xml  
	DESTINATION ${PLUSAPP_INSTALL_CONFIG_DIR}
	COMPONENT RuntimeLibraries
  )

INSTALL(TARGETS TrackedUltrasoundCapturing
  DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
  COMPONENT RuntimeLibraries
  )
