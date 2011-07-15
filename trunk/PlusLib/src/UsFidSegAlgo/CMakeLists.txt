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

SET(UsFidSegAlgo_LIBS itkvnl_algo ${ITK_LIBRARIES} vtkPlusCommon)
ADD_LIBRARY( UsFidSegAlgo STATIC ${UsFidSegAlgo_SRCS} ${UsFidSegAlgo_HDRS} )

TARGET_LINK_LIBRARIES( UsFidSegAlgo ${UsFidSegAlgo_LIBS})

# --------------------------------------------------------------------------
# Set external MS project 
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} UsFidSegAlgo CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_UsFidSegAlgo UsFidSegAlgo "${PlusLib_BINARY_DIR}/src/UsFidSegAlgo/UsFidSegAlgo.vcproj" vtkPlusCommon CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Install
#

# INSTALL(TARGETS UsFidSegAlgo
  # DESTINATION bin
  # COMPONENT RuntimeLibraries)
  