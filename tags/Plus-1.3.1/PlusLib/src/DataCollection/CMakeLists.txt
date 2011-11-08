# --------------------------------------------------------------------------
# Sources
SET (DataCollection_SRCS
	igtlStringMessage1.cxx
	vtkDataCollector.cxx 
	vtkDataCollectorSynchronizer.cxx 
  vtkOpenIGTLinkBroadcaster.cxx
	vtkPlusOpenIGTLinkServer.cxx
  vtkPlusOpenIGTLinkClient.cxx
  vtkPlusCommand.cxx
  vtkPlusStartDataCollectionCommand.cxx
  vtkPlusStopDataCollectionCommand.cxx
  vtkPlusCommandCollection.cxx
  vtkPlusCommandFactory.cxx
  vtkIGTLMessageQueue.cxx
	)

SET (DataCollection_HDRS)
if (WIN32)	
	SET (DataCollection_HDRS
		igtlStringMessage1.h
		vtkDataCollector.h 
		vtkDataCollectorSynchronizer.h
    vtkOpenIGTLinkBroadcaster.h
		vtkPlusOpenIGTLinkServer.h
    vtkPlusOpenIGTLinkClient.h
    vtkPlusCommand.h
    vtkPlusStartDataCollectionCommand.h
	vtkPlusStopDataCollectionCommand.h
    vtkPlusCommandCollection.h
    vtkPlusCommandFactory.h
    vtkIGTLMessageQueue.h
		)
endif (WIN32)	

SET (DataCollection_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library

SET (DataCollection_LIBS 
  vtkCommon 
  vtkHybrid 
  vtkFiltering 
  vtkImageAcquisition 
  vtkPlusCommon 
  vtkTracking 
  vtkSignaling
  )
    
IF (PLUS_USE_OpenIGTLink)
  SET (DataCollection_LIBS ${DataCollection_LIBS} 
    OpenIGTLink
    )
ENDIF(PLUS_USE_OpenIGTLink)        

INCLUDE_DIRECTORIES( ${DataCollection_INCLUDE_DIRS} )
ADD_LIBRARY(vtkDataCollection ${DataCollection_SRCS} ${DataCollection_HDRS})
TARGET_LINK_LIBRARIES(vtkDataCollection ${DataCollection_LIBS})

# --------------------------------------------------------------------------
# Set external MS project 
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkDataCollection CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkDataCollection vtkDataCollection ${PlusLib_BINARY_DIR}/src/DataCollection/vtkDataCollection.vcproj vtkPlusCommon vtkImageAcquisition vtkTracking CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Install
#
INSTALL(TARGETS vtkDataCollection
    RUNTIME
    DESTINATION bin
    CONFIGURATIONS Release
    COMPONENT RuntimeLibraries)

# --------------------------------------------------------------------------
# Testing
#

IF (BUILD_TESTING)
	SUBDIRS (Testing)
ENDIF (BUILD_TESTING)
