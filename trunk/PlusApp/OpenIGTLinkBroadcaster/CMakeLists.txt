
PROJECT( OpenIGTLinkBroadcaster )


SET( OIGTL_SRCS
  OpenIGTLinkBroadcaster.cpp
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
  )
TARGET_LINK_LIBRARIES( SendCommands
  OpenIGTLink
  vtkDataCollection
  )

  
ADD_EXECUTABLE( PlusServer
  PlusServer.cxx
  )
TARGET_LINK_LIBRARIES( PlusServer
  OpenIGTLink
  vtkCommon
  vtkFiltering
  vtkPlusCommon
  vtkDataCollection
  vtkImageAcquisition 
  vtkTracking
  )

  
INSTALL(TARGETS OpenIGTLinkBroadcaster
  DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
  COMPONENT RuntimeLibraries
  )
