
PROJECT( OpenIGTLinkBroadcaster )


SET( OIGTL_SRCS
  OpenIGTLinkBroadcaster.cpp
  igtlStringMessage1.cxx  igtlStringMessage1.h
  vtkOpenIGTLinkPlusController.cxx  vtkOpenIGTLinkPlusController.h
	)

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

ADD_EXECUTABLE( OpenIGTLinkBroadcaster
  ${OIGTL_SRCS}
  )
TARGET_LINK_LIBRARIES( OpenIGTLinkBroadcaster
  ITKCommon
  vtkCommon
  vtkFiltering
  vtkPlusCommon
  vtkDataCollection
  vtkImageAcquisition 
  vtkTracking
  )

ADD_EXECUTABLE( SendCommands
  SendCommands.cxx
  igtlStringMessage1.cxx  igtlStringMessage1.h
  )
TARGET_LINK_LIBRARIES( SendCommands
  OpenIGTLink
  )

  
ADD_EXECUTABLE( PlusServer
  PlusServer.cxx
  igtlStringMessage1.cxx  igtlStringMessage1.h
  )
TARGET_LINK_LIBRARIES( PlusServer
  OpenIGTLink
  )

  
INSTALL(TARGETS OpenIGTLinkBroadcaster
  DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
  COMPONENT RuntimeLibraries
  )
