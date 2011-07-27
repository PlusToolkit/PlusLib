PROJECT(FreehandCalibration)

FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_USE_QT3SUPPORT TRUE) 
INCLUDE(${QT_USE_FILE})

# --------------------------------------------------------------------------
# Sources

SET (FreehandCalibration_SRCS
	FreehandMain.cxx
	FreehandMainWindow.cxx
	Toolboxes/ConfigurationToolbox.cxx
	Toolboxes/ConfigurationController.cxx
	Toolboxes/FreehandCalibrationToolbox.cxx
	Toolboxes/StylusCalibrationToolbox.cxx
	Toolboxes/PhantomRegistrationToolbox.cxx
	Toolboxes/VolumeReconstructionToolbox.cxx
	)

if (WIN32)	
	SET (FreehandCalibration_SRCS
		${FreehandCalibration_SRCS}
		)
endif (WIN32)	

SET (FreehandCalibration_UI_HDRS
		FreehandMainWindow.h
		Toolboxes/ConfigurationToolbox.h
		Toolboxes/ConfigurationController.h
		Toolboxes/FreehandCalibrationToolbox.h
		Toolboxes/StylusCalibrationToolbox.h
		Toolboxes/PhantomRegistrationToolbox.h
		Toolboxes/VolumeReconstructionToolbox.h
	)

SET (FreehandCalibration_UI_SRCS
	FreehandMainWindow.ui 
	Toolboxes/ConfigurationToolbox.ui
	Toolboxes/FreehandCalibrationToolbox.ui
	Toolboxes/StylusCalibrationToolbox.ui
	Toolboxes/PhantomRegistrationToolbox.ui
	Toolboxes/VolumeReconstructionToolbox.ui
	)

SET (QT_Resources
	FreehandCalibration.qrc
	${PlusApp_SOURCE_DIR}/CommonWidgets/CommonWidgets.qrc
	)
	

# --------------------------------------------------------------------------
# Qt specific commands

QT4_WRAP_CPP( QT_MOC_Files ${FreehandCalibration_UI_HDRS} )			  
QT4_WRAP_UI( QT_UI_Headers ${FreehandCalibration_UI_SRCS} )
QT4_WRAP_CPP( QT_MOC_UI_Files ${QT_UI_Headers} )
QT4_ADD_RESOURCES( QT_RCC_Files ${QT_Resources} )

SOURCE_GROUP("Generated Files" FILES ${QT_MOC_Files} ${QT_UI_Headers} ${QT_MOC_UI_Files} ${QT_RCC_Files})
SOURCE_GROUP("UI Files" FILES ${FreehandCalibration_UI_SRCS})

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Toolboxes)
include_directories(${PlusApp_BINARY_DIR}/CommonWidgets)
include_directories(${PlusApp_SOURCE_DIR}/CommonWidgets)

# --------------------------------------------------------------------------
# Build the library

SET (FreehandCalibration_LIBS 
    ${FreehandCalibration_LIBS} 
    ${QT_LIBRARIES} 
	CommonWidgets
    vtkImageAcquisition 
	vtkPlusCommon
    ITKIO
    itkvnl 
    itkvnl_algo 
    vtkCommon 
    vtkRendering 
    vtkHybrid 
    vtkTrusCalibration 
    vtkTracking 
    vtkDataCollection 
    vtkVolumeReconstruction
    )

IF (PLUS_USE_OpenIGTLink)
    SET (FreehandCalibration_LIBS ${FreehandCalibration_LIBS} OpenIGTLink)
ENDIF(PLUS_USE_OpenIGTLink)

ADD_EXECUTABLE( FreehandCalibration MACOSX_BUNDLE ${FreehandCalibration_SRCS} ${FreehandCalibration_UI_HDRS} ${QT_UI_Sources} ${QT_UI_Headers} ${QT_MOC_Files} ${QT_MOC_UI_Files} ${QT_RCC_Files})

TARGET_LINK_LIBRARIES( FreehandCalibration QVTK ${FreehandCalibration_LIBS} )

# --------------------------------------------------------------------------
# Install
#
IF(PLUSAPP_INSTALL_BIN_DIR)
# show up in the Start Menu in Windows
if( WIN32 AND NOT UNIX )
    SET(CPACK_PACKAGE_EXECUTABLES "FreehandCalibration" "FreehandCalibration")
endif()

INSTALL(FILES 
	${QT_BINARY_DIR}/QtCore4${CMAKE_SHARED_LIBRARY_SUFFIX}
	${QT_BINARY_DIR}/QtGui4${CMAKE_SHARED_LIBRARY_SUFFIX}
	${QT_BINARY_DIR}/QtNetwork4${CMAKE_SHARED_LIBRARY_SUFFIX}
	${QT_BINARY_DIR}/QtSql4${CMAKE_SHARED_LIBRARY_SUFFIX}
	${QT_BINARY_DIR}/QtWebKit4${CMAKE_SHARED_LIBRARY_SUFFIX}
	${QT_BINARY_DIR}/QtXmlPatterns4${CMAKE_SHARED_LIBRARY_SUFFIX}
	${QT_BINARY_DIR}/phonon4${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${QT_BINARY_DIR}/QtOpenGL4${CMAKE_SHARED_LIBRARY_SUFFIX}
	DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
	COMPONENT RuntimeLibraries
  )

INSTALL(TARGETS FreehandCalibration 
  DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
  COMPONENT RuntimeLibraries
  )
  
ENDIF(PLUSAPP_INSTALL_BIN_DIR)

 

  
	 
	 
