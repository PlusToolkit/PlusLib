
PROJECT( OpenIGTLinkBroadcaster )


SET( OIGTL_SRCS
  OpenIGTLinkBroadcaster.cpp
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
