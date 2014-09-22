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

SET (PatternLocAlgo_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "" FORCE )

# --------------------------------------------------------------------------
# Build the library

SET(PatternLocAlgo_LIBS
  itkvnl_algo
  ${ITK_LIBRARIES}
  vtkPlusCommon 
  ${VTK_LIBRARIES}
  )
  
INCLUDE_DIRECTORIES( ${PatternLocAlgo_INCLUDE_DIRS} )
GENERATE_EXPORT_DIRECTIVE_FILE(PatternLocAlgo)
ADD_LIBRARY( PatternLocAlgo ${PatternLocAlgo_SRCS} ${PatternLocAlgo_HDRS} )
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

INSTALL(TARGETS PatternLocAlgo
  RUNTIME DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  LIBRARY DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  )

# --------------------------------------------------------------------------
# Testing
#

IF( BUILD_TESTING )
  SUBDIRS( Testing )
ENDIF( BUILD_TESTING )
