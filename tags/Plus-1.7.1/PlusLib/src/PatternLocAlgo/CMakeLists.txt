# --------------------------------------------------------------------------
# Sources

SET (PatternLocAlgo_SRCS
  FidSegmentation.cxx
  FidLineFinder.cxx
  FidLabeling.cxx
  FidPatternRecognitionCommon.cxx
  FidPatternRecognition.cxx
  PatternLocResultFile.cxx
  )

SET (PatternLocAlgo_HDRS)	
IF (WIN32)	
	SET (PatternLocAlgo_HDRS
		FidSegmentation.h
		FidLineFinder.h
		FidLabeling.h
		FidPatternRecognitionCommon.h
		FidPatternRecognition.h
		PatternLocResultFile.h
		)
ENDIF (WIN32)

SET (PatternLocAlgo_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "" FORCE )

# --------------------------------------------------------------------------
# Build the library

SET(PatternLocAlgo_LIBS
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
  
INCLUDE_DIRECTORIES( ${PatternLocAlgo_INCLUDE_DIRS} )
ADD_LIBRARY( PatternLocAlgo STATIC ${PatternLocAlgo_SRCS} ${PatternLocAlgo_HDRS} )
TARGET_LINK_LIBRARIES( PatternLocAlgo ${PatternLocAlgo_LIBS})

# --------------------------------------------------------------------------
# Set external MS project 
SET (PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} PatternLocAlgo CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_PatternLocAlgo PatternLocAlgo;"${PlusLib_BINARY_DIR}/src/PatternLocAlgo/PatternLocAlgo.vcproj";vtkPlusCommon CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Install
#

# INSTALL(TARGETS PatternLocAlgo
  # DESTINATION bin
  # COMPONENT RuntimeLibraries)
  
# --------------------------------------------------------------------------
# Testing
#

IF( BUILD_TESTING )
  SUBDIRS( Testing )
ENDIF( BUILD_TESTING )
