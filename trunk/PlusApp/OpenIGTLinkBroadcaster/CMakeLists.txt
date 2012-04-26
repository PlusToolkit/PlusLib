
PROJECT( OpenIGTLinkBroadcaster )

FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_USE_QT3SUPPORT TRUE) 
INCLUDE(${QT_USE_FILE})


# --------------------------------------------------------------------------
# Sources

SET( OIGTL_SRCS
  OpenIGTLinkBroadcasterMain.cxx
  OpenIGTLinkBroadcasterWidget.cxx  
  vtkOpenIGTLinkPlusController.cxx
  )

if (WIN32)	
	SET (fCal_SRCS
		${fCal_SRCS}
		)
endif (WIN32)	

SET( OIGTL_UI_HDRS
  OpenIGTLinkBroadcasterWidget.h
  )
  
SET( OIGTL_UI_SRCS
  OpenIGTLinkBroadcaster.ui
  )
  

# --------------------------------------------------------------------------
# Qt specific commands

QT4_WRAP_CPP( OIGTL_QT_MOC_Files ${OIGTL_UI_HDRS} )			  
QT4_WRAP_UI( OIGTL_QT_UI_Headers ${OIGTL_UI_SRCS} )
QT4_WRAP_CPP( OIGTL_QT_MOC_UI_Files ${OIGTL_QT_UI_Headers} )

SOURCE_GROUP("Generated Files" FILES ${OIGTL_QT_MOC_Files} ${OIGTL_QT_UI_Headers} ${OIGTL_QT_MOC_UI_Files})
SOURCE_GROUP("UI Files" FILES ${OIGTL_UI_SRCS})

  
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})


# --------------------------------------------------------------------------
# Build the library

SET (OIGTL_LIBS
  ITKCommon
  vtkCommon
  vtkFiltering
  vtkPlusCommon
  vtkDataCollection
  vtkImageAcquisition 
  vtkTracking
  ${QT_LIBRARIES}
  OpenIGTLink
  )


ADD_EXECUTABLE( OpenIGTLinkBroadcaster
  ${OIGTL_SRCS}
  ${OIGTL_UI_HDRS}
  ${OIGTL_QT_UI_Headers}
  ${OIGTL_QT_MOC_Files}
  ${OIGTL_QT_MOC_UI_Files}
  )
TARGET_LINK_LIBRARIES( OpenIGTLinkBroadcaster ${OIGTL_LIBS} )


# --------------------------------------------------------------------------
# Install
#

INSTALL(TARGETS OpenIGTLinkBroadcaster
  DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
  COMPONENT RuntimeLibraries
  )
