
# --------------------------------------------------------------------------
# Options 
OPTION (PLUS_PRINT_SYNC_DEBUG_INFO "Print synchronizer debugging info to file in gnuplot data format" ON)

# --------------------------------------------------------------------------
# Sources
SET (DataCollection_SRCS
	vtkDataCollector.cxx 
	vtkDataCollectorSynchronizer.cxx 
	)

SET (DataCollection_HDRS)
if (WIN32)	
	SET (DataCollection_HDRS
		vtkDataCollector.h 
		vtkDataCollectorSynchronizer.h
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

INCLUDE_DIRECTORIES( ${PLUS_INCLUDE_DIRS} )
ADD_LIBRARY(vtkDataCollection ${DataCollection_SRCS} ${DataCollection_HDRS})
TARGET_LINK_LIBRARIES(vtkDataCollection ${DataCollection_LIBS})

# --------------------------------------------------------------------------
# Install
#
INSTALL(TARGETS vtkDataCollection
  DESTINATION bin
  COMPONENT Runtime)


