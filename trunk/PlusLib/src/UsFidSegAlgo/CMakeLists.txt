# --------------------------------------------------------------------------
# Sources

SET (UsFidSegAlgo_SRCS
  FidSegmentation.cxx
  FidLineFinder.cxx
  FidLabeling.cxx
  FidPatternRecognitionCommon.cxx
  FidPatternRecognition.cxx
  UsFidSegResultFile.cxx
  )

SET (UsFidSegAlgo_HDRS)	
IF (WIN32)	
	SET (UsFidSegAlgo_HDRS
		FidSegmentation.h
		FidLineFinder.h
		FidLabeling.h
		FidPatternRecognitionCommon.h
		FidPatternRecognition.h
		UsFidSegResultFile.h
		)
ENDIF (WIN32)

SET (UsFidSegAlgo_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "" FORCE )

# --------------------------------------------------------------------------
# Build the library

SET(UsFidSegAlgo_LIBS
  itkvnl_algo
  ITKIO
	ITKCommon
	ITKBasicFilters
	ITKAlgorithms
	ITKNumerics
	ITKSpatialObject
	ITKStatistics
  vtkPlusCommon 
  vtkCommon )
  
INCLUDE_DIRECTORIES( ${UsFidSegAlgo_INCLUDE_DIRS} )
ADD_LIBRARY( UsFidSegAlgo STATIC ${UsFidSegAlgo_SRCS} ${UsFidSegAlgo_HDRS} )
TARGET_LINK_LIBRARIES( UsFidSegAlgo ${UsFidSegAlgo_LIBS})

# --------------------------------------------------------------------------
# Set external MS project 
SET (PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} UsFidSegAlgo CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_UsFidSegAlgo UsFidSegAlgo "${PlusLib_BINARY_DIR}/src/UsFidSegAlgo/UsFidSegAlgo.vcproj" vtkPlusCommon CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Install
#

# INSTALL(TARGETS UsFidSegAlgo
  # DESTINATION bin
  # COMPONENT RuntimeLibraries)
  
# --------------------------------------------------------------------------
# Testing
#

IF( BUILD_TESTING )
  SUBDIRS( Testing )
ENDIF( BUILD_TESTING )
