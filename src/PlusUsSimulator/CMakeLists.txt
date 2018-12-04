PROJECT(PlusUsSimulator)

# Sources
SET(${PROJECT_NAME}_SRCS
    vtk${PROJECT_NAME}Algo.cxx
    PlusSpatialModel.cxx
    )

IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode") 
  SET(${PROJECT_NAME}_HDRS
    vtk${PROJECT_NAME}Algo.h
    PlusSpatialModel.h
    )
ENDIF()

SET(${PROJECT_NAME}_INCLUDE_DIRS 
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library
SET(${PROJECT_NAME}_LIBS
  vtkPlusCommon
  vtkIGSIOCommon
  vtkPlusRendering
  vtkPlusImageProcessing
  vtkFiltersSources
  vtkFiltersFlowPaths
  vtkIOGeometry
  vtkIOXML
  )

GENERATE_EXPORT_DIRECTIVE_FILE(vtk${PROJECT_NAME})
ADD_LIBRARY(vtk${PROJECT_NAME} ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_HDRS})
FOREACH(p IN LISTS ${PROJECT_NAME}_INCLUDE_DIRS)
  target_include_directories(vtk${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${p}>)
ENDFOREACH()
target_include_directories(vtk${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${PLUSLIB_INCLUDE_INSTALL}>)
TARGET_LINK_LIBRARIES(vtk${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_LIBS})
PlusLibAddVersionInfo(vtk${PROJECT_NAME} "Library containing code for ultrasound simulation. Part of the Plus toolkit." vtk${PROJECT_NAME} vtk${PROJECT_NAME})

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtk${PROJECT_NAME} CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtk${PROJECT_NAME} vtk${PROJECT_NAME};${PlusLib_BINARY_DIR}/src/${PROJECT_NAME}/vtk${PROJECT_NAME}.vcxproj;vtkPlusCommon CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Install
#
PlusLibInstallLibrary(vtk${PROJECT_NAME} ${PROJECT_NAME})

# --------------------------------------------------------------------------
# Testing
#
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF()