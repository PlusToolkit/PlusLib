# Sources
SET (PlusUsSimulator_SRCS
    vtkPlusUsSimulatorAlgo.cxx
    PlusSpatialModel.cxx
    )

SET (PlusUsSimulator_HDRS)
if (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode") 
  SET (PlusUsSimulator_HDRS
    vtkPlusUsSimulatorAlgo.h
    PlusSpatialModel.h
  )
endif ()

SET (PlusUsSimulator_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library
SET (PlusUsSimulator_LIBS
  vtkPlusCommon
  vtkPlusRendering
  vtkPlusImageProcessing
  vtkFiltersSources
  vtkFiltersFlowPaths
  vtkIOGeometry
  vtkIOXML
  )

GENERATE_EXPORT_DIRECTIVE_FILE(vtkPlusUsSimulator)
ADD_LIBRARY(vtkPlusUsSimulator ${PlusUsSimulator_SRCS} ${PlusUsSimulator_HDRS})
foreach(p IN LISTS PlusUsSimulator_INCLUDE_DIRS)
  target_include_directories(vtkPlusUsSimulator PUBLIC 
    $<BUILD_INTERFACE:${p}>)
endforeach()
target_include_directories(vtkPlusUsSimulator PUBLIC 
  $<INSTALL_INTERFACE:${PLUSLIB_INCLUDE_INSTALL}>)
TARGET_LINK_LIBRARIES(vtkPlusUsSimulator PUBLIC ${PlusUsSimulator_LIBS})

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkPlusUsSimulator CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkPlusUsSimulator vtkPlusUsSimulator;${PlusLib_BINARY_DIR}/src/PlusUsSimulator/vtkPlusUsSimulator.vcxproj;vtkPlusCommon CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
IF (BUILD_TESTING)
  ADD_SUBDIRECTORY (Testing)
ENDIF (BUILD_TESTING)

# --------------------------------------------------------------------------
# Install
#
INSTALL(TARGETS vtkPlusUsSimulator EXPORT PlusLib
  RUNTIME DESTINATION "${PLUSLIB_BINARY_INSTALL}" CONFIGURATIONS Release COMPONENT RuntimeLibraries
  LIBRARY DESTINATION "${PLUSLIB_LIBRARY_INSTALL}" CONFIGURATIONS Release COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION "${PLUSLIB_ARCHIVE_INSTALL}" CONFIGURATIONS Release COMPONENT Development
  )
INSTALL(FILES ${PlusUsSimulator_HDRS}
  DESTINATION "${PLUSLIB_INCLUDE_INSTALL}"
  )