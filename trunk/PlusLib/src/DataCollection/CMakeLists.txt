
# --------------------------------------------------------------------------
# Options 
OPTION (PLUS_PRINT_SYNC_DEBUG_INFO "Print synchronizer debugging info to file in gnuplot data format" ON)

# --------------------------------------------------------------------------
# Sources
SET (DataCollection_SRCS
	vtkDataCollector.cxx 
	vtkDataCollectorSynchronizer.cxx 
    vtkOpenIGTLinkBroadcaster.cxx
	)

SET (DataCollection_HDRS)
if (WIN32)	
	SET (DataCollection_HDRS
		vtkDataCollector.h 
		vtkDataCollectorSynchronizer.h
        vtkOpenIGTLinkBroadcaster.h
		)
endif (WIN32)	

IF (BUILD_TESTING)
	SUBDIRS (Testing)
ENDIF (BUILD_TESTING)

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
    SET (DataCollection_LIBS ${DataCollection_LIBS} OpenIGTLink)
ENDIF(PLUS_USE_OpenIGTLink)        

INCLUDE_DIRECTORIES( ${PLUSLIB_INCLUDE_DIRS} )
ADD_LIBRARY(vtkDataCollection ${DataCollection_SRCS} ${DataCollection_HDRS})
TARGET_LINK_LIBRARIES(vtkDataCollection ${DataCollection_LIBS})

# --------------------------------------------------------------------------
# Install
#
INSTALL(TARGETS vtkDataCollection
    RUNTIME
    DESTINATION bin
    CONFIGURATIONS Release
    COMPONENT RuntimeLibraries)


