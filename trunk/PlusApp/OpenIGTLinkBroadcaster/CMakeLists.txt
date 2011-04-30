
PROJECT( OpenIGTLinkBroadcaster )

FIND_PACKAGE( Qt4 REQUIRED )
INCLUDE(${QT_USE_FILE})


SET( OIGTL_SRCS
  OpenIGTLinkBroadcaster.cpp
	MainWidget.h   MainWidget.cpp
	)

SET( OIGTL_UI_HDRS
  MainWidget.h
	)

SET( OIGTL_UI_SRCS
  MainWidget.ui 
  )
	
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )
  
QT4_WRAP_UI( QT_UI_Headers ${OIGTL_UI_SRCS} )
QT4_WRAP_CPP( QT_MOC_Files ${OIGTL_UI_HDRS} )			  

CONFIGURE_FILE( "BroadcasterConfig_Simulation.xml.in"
                "${CMAKE_CURRENT_BINARY_DIR}/BroadcasterConfig_Simulation.xml" )

ADD_EXECUTABLE( OpenIGTLinkBroadcaster
  ${OIGTL_SRCS}
  ${QT_UI_Headers}
  ${QT_MOC_Files}
  )

TARGET_LINK_LIBRARIES( OpenIGTLinkBroadcaster
  ITKCommon
  vtkCommon
  vtkFiltering
  vtkPlusCommon
  ${QT_LIBRARIES}
  vtkDataCollection
  vtkImageAcquisition 
  vtkTracking
  )
