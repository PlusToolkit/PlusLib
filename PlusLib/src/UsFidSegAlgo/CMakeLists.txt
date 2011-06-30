# --------------------------------------------------------------------------
# Sources

SET (UsFidSegAlgo_SRCS
		UltraSoundFiducialSegmentation.cpp
        UsFidSegResultFile.cpp 
	)

SET (UsFidSegAlgo_HDRS)	
IF (WIN32)	
	SET (UsFidSegAlgo_HDRS
		UltraSoundFiducialSegmentation.h
		UsFidSegResultFile.h
		)
ENDIF (WIN32)

SET(ITK_LIBRARIES
	ITKIO
	ITKCommon
	ITKBasicFilters
	ITKAlgorithms
	ITKNumerics
	ITKSpatialObject
	ITKStatistics
	)

IF( BUILD_TESTING )
  SUBDIRS( Testing )
ENDIF( BUILD_TESTING )

# --------------------------------------------------------------------------
# Build the library

ADD_LIBRARY( UsFidSegAlgo STATIC ${UsFidSegAlgo_SRCS} ${UsFidSegAlgo_HDRS} )

TARGET_LINK_LIBRARIES( UsFidSegAlgo itkvnl_algo ${ITK_LIBRARIES} vtkPlusCommon)

# --------------------------------------------------------------------------
# Install
#

# INSTALL(TARGETS UsFidSegAlgo
  # DESTINATION bin
  # COMPONENT RuntimeLibraries)
  