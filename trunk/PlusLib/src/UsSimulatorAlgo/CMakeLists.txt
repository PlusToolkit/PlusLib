# Sources
SET (UsSimulatorAlgo_SRCS
    vtkUsSimulatorAlgo.cxx
    )

SET (UsSimulatorAlgo_HDRS)
if (WIN32)    
  SET (UsSimulatorAlgo_HDRS
    vtkUsSimulatorAlgo.h
  )
endif (WIN32)

SET (UsSimulatorAlgo_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library

SET (UsSimulatorAlgo_LIBS
  vtkPlusCommon
  )     

SET (UsSimulatorAlgo_INCLUDE_DIRS ${UsSimulatorAlgo_INCLUDE_DIRS} ${UsSimulatorAlgo_INCLUDE_DIRS} CACHE INTERNAL "" FORCE    )
INCLUDE_DIRECTORIES( ${UsSimulatorAlgo_INCLUDE_DIRS} )
ADD_LIBRARY(UsSimulatorAlgo ${UsSimulatorAlgo_SRCS} ${UsSimulatorAlgo_HDRS})
TARGET_LINK_LIBRARIES(UsSimulatorAlgo ${UsSimulatorAlgo_LIBS})

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} UsSimulatorAlgo CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_UsSimulatorAlgo UsSimulatorAlgo ${PlusLib_BINARY_DIR}/src/UsSimulatorAlgo/UsSimulatorAlgo.vcproj vtkPlusCommon CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
IF (BUILD_TESTING)
  ADD_SUBDIRECTORY (Testing)
ENDIF (BUILD_TESTING)
# --------------------------------------------------------------------------

# Install
#

INSTALL(TARGETS UsSimulatorAlgo
  RUNTIME
  DESTINATION bin
  CONFIGURATIONS Release
  COMPONENT RuntimeLibraries
  )
