# --------------------------------------------------------------------------
# Sources

SET (UsFidSegAlgo_SRCS
	drawing.cpp 
	kphantseg.cpp 
	morph.cpp 
	)

SET (UsFidSegAlgo_HDRS)	
IF (WIN32)	
	SET (UsFidSegAlgo_HDRS
		binsearch.h 
		kphantseg.h 
		segimpl.h 
		sort.h 
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
  