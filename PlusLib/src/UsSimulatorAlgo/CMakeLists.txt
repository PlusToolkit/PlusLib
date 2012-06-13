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

INCLUDE_DIRECTORIES( ${UsSimulatorAlgo_INCLUDE_DIRS} )
ADD_LIBRARY(vtkUsSimulatorAlgo ${UsSimulatorAlgo_SRCS} ${UsSimulatorAlgo_HDRS})
TARGET_LINK_LIBRARIES(vtkUsSimulatorAlgo ${UsSimulatorAlgo_LIBS})

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkUsSimulatorAlgo CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkUsSimulatorAlgo vtkUsSimulatorAlgo ${PlusLib_BINARY_DIR}/src/UsSimulatorAlgo/vtkUsSimulatorAlgo.vcproj vtkPlusCommon CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
IF (BUILD_TESTING)
  ADD_SUBDIRECTORY (Testing)
ENDIF (BUILD_TESTING)
# --------------------------------------------------------------------------

# Install
#

INSTALL(TARGETS vtkUsSimulatorAlgo
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION bin
  CONFIGURATIONS Release
  COMPONENT RuntimeLibraries
  )
