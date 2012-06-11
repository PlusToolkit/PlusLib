# --------------------------------------------------------------------------
# Sources
SET (DataCollection_SRCS
  vtkDataCollector.cxx 
  vtkDataCollectorSynchronizer.cxx 
  vtkDataCollectorHardwareDevice.cxx
  vtkDataCollectorFile.cxx
	)

SET (DataCollection_HDRS)
if (WIN32)	
	SET (DataCollection_HDRS
		vtkDataCollector.h 
		vtkDataCollectorSynchronizer.h
		vtkDataCollectorHardwareDevice.h
		vtkDataCollectorFile.h
		)
endif (WIN32)	

IF (PLUS_USE_OpenIGTLink)
	SET (DataCollection_SRCS ${DataCollection_SRCS}
		vtkOpenIGTLinkBroadcaster.cxx
	  vtkPlusOpenIGTLinkServer.cxx
	  vtkPlusOpenIGTLinkClient.cxx
		OpenIGTLinkCommands/vtkPlusCommand.cxx
		OpenIGTLinkCommands/vtkPlusCommandCollection.cxx
		OpenIGTLinkCommands/vtkPlusCommandFactory.cxx
		OpenIGTLinkCommands/vtkPlusStartDataCollectionCommand.cxx
		OpenIGTLinkCommands/vtkPlusStopDataCollectionCommand.cxx
		)
		
	IF (WIN32)	
		SET (DataCollection_HDRS ${DataCollection_HDRS}
			vtkOpenIGTLinkBroadcaster.h
			vtkPlusOpenIGTLinkServer.h
			vtkPlusOpenIGTLinkClient.h
			OpenIGTLinkCommands/vtkPlusCommand.h
			OpenIGTLinkCommands/vtkPlusCommandCollection.h
			OpenIGTLinkCommands/vtkPlusCommandFactory.h			
			OpenIGTLinkCommands/vtkPlusStartDataCollectionCommand.h
			OpenIGTLinkCommands/vtkPlusStopDataCollectionCommand.h
			)
	ENDIF (WIN32)
	SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/OpenIGTLinkCommands CACHE INTERNAL "" FORCE)	
ENDIF(PLUS_USE_OpenIGTLink)


SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "" FORCE)

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
    OpenIGTLink vtkPlusOpenIGTLink
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
SET(vcProj_vtkDataCollection vtkDataCollection;"${PlusLib_BINARY_DIR}/src/DataCollection/vtkDataCollection.vcproj";vtkPlusCommon;vtkImageAcquisition;vtkTracking CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Install
#
INSTALL(TARGETS vtkDataCollection
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION bin
    CONFIGURATIONS Release
    COMPONENT RuntimeLibraries)

# --------------------------------------------------------------------------
# Testing
#

IF (BUILD_TESTING)
	SUBDIRS (Testing)
ENDIF (BUILD_TESTING)
