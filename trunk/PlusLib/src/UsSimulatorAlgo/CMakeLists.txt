# Sources
SET (UsSimulatorAlgo_SRCS
    vtkPlusUsSimulatorAlgo.cxx
    PlusSpatialModel.cxx
    )

SET (UsSimulatorAlgo_HDRS)
if (WIN32)    
  SET (UsSimulatorAlgo_HDRS
    vtkPlusUsSimulatorAlgo.h
    PlusSpatialModel.h
  )
endif (WIN32)

SET (UsSimulatorAlgo_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library

SET (UsSimulatorAlgo_LIBS
  vtkPlusCommon
  vtkImageProcessingAlgo
  )     

INCLUDE_DIRECTORIES( ${UsSimulatorAlgo_INCLUDE_DIRS} )
GENERATE_EXPORT_DIRECTIVE_FILE(vtkPlusUsSimulatorAlgo)
ADD_LIBRARY(vtkPlusUsSimulatorAlgo ${UsSimulatorAlgo_SRCS} ${UsSimulatorAlgo_HDRS})
TARGET_LINK_LIBRARIES(vtkPlusUsSimulatorAlgo ${UsSimulatorAlgo_LIBS})

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkPlusUsSimulatorAlgo CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkPlusUsSimulatorAlgo vtkPlusUsSimulatorAlgo;"${PlusLib_BINARY_DIR}/src/UsSimulatorAlgo/vtkPlusUsSimulatorAlgo.vcproj";vtkPlusCommon CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
IF (BUILD_TESTING)
  ADD_SUBDIRECTORY (Testing)
ENDIF (BUILD_TESTING)
# --------------------------------------------------------------------------

# Install
#

INSTALL(TARGETS vtkPlusUsSimulatorAlgo
  RUNTIME DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  LIBRARY DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION bin CONFIGURATIONS Release COMPONENT Development
  )
