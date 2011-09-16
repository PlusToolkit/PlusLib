FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_USE_QT3SUPPORT TRUE) 
INCLUDE(${QT_USE_FILE})

# --------------------------------------------------------------------------
# Sources

SET (ProstateBiopsyGuidance_SRCS
	ProstateBiopsyGuidance.cxx
	ProstateBiopsyGuidanceMain.cxx 
	ProstateBiopsyGuidanceGUI.cxx
	)

if (WIN32)	
	SET (ProstateBiopsyGuidance_SRCS
		${ProstateBiopsyGuidance_SRCS}
		ProstateBiopsyGuidance.h
		)
endif (WIN32)	

SET (ProstateBiopsyGuidance_UI_HDRS
	ProstateBiopsyGuidanceGUI.h  
	)

SET (ProstateBiopsyGuidance_UI_SRCS
	ProstateBiopsyGuidance.ui 
	)

SET (QT_Resources
	${PlusApp_SOURCE_DIR}/CommonWidgets/CommonWidgets.qrc
	)
	
# --------------------------------------------------------------------------
# Qt specific commands

QT4_WRAP_CPP( QT_MOC_Files ${ProstateBiopsyGuidance_UI_HDRS} )			  
QT4_WRAP_UI( QT_UI_Headers ${ProstateBiopsyGuidance_UI_SRCS} )
QT4_WRAP_CPP( QT_MOC_UI_Files ${QT_UI_Headers} )	
QT4_ADD_RESOURCES( QT_RCC_Files ${QT_Resources} )


SOURCE_GROUP("Generated Files" FILES ${QT_MOC_Files} ${QT_UI_Headers} ${QT_MOC_UI_Files} ${QT_RCC_Files})
SOURCE_GROUP("UI Files" FILES ${ProstateBiopsyGuidance_UI_SRCS})

SET( Temp C:/Users/Admin/devel/PlusExperimental-bin/PlusLib/src/Vibro/AudioCard )
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${PlusApp_BINARY_DIR}/CommonWidgets)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories( Temp )

# --------------------------------------------------------------------------
# Build the library

SET (ProstateBiopsyGuidance_LIBS 
    ${ProstateBiopsyGuidance_LIBS} 
    ${QT_LIBRARIES} 
    CommonWidgets
    vtkCommon
    vtkFiltering
    vtkHybrid
    vtkImageAcquisition
    ITKIO
    vtkTracking
    vtkDataCollection
    vtkPlusCommon
	VibroAudioCard 
    )

IF (PLUS_USE_OpenIGTLink)
    SET (ProstateBiopsyGuidance_LIBS ${ProstateBiopsyGuidance_LIBS} OpenIGTLink)
ENDIF(PLUS_USE_OpenIGTLink)
    
ADD_EXECUTABLE( ProstateBiopsyGuidance MACOSX_BUNDLE ${ProstateBiopsyGuidance_SRCS} ${ProstateBiopsyGuidance_UI_HDRS} ${QT_UI_Sources} ${QT_UI_Headers} ${QT_MOC_Files} ${QT_MOC_UI_Files} ${QT_RCC_Files})

TARGET_LINK_LIBRARIES( ProstateBiopsyGuidance QVTK ${ProstateBiopsyGuidance_LIBS} )

# --------------------------------------------------------------------------
# Install
#

# show up in the Start Menu in Windows
if( WIN32 AND NOT UNIX )
    SET(CPACK_PACKAGE_EXECUTABLES "ProstateBiopsyGuidance" "ProstateBiopsyGuidance")
endif()

INSTALL(TARGETS ProstateBiopsyGuidance
  DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
  COMPONENT RuntimeLibraries
  )
