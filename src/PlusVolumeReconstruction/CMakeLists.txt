PROJECT(PlusVolumeReconstruction)

# --------------------------------------------------------------------------
# Sources
SET(${PROJECT_NAME}_SRCS
  vtkPlusVolumeReconstructor.cxx
  )

SET(${PROJECT_NAME}_HDRS
  vtkPlusVolumeReconstructor.h
  )

SET(${PROJECT_NAME}_INCLUDE_DIRS 
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${VTKVOLUMERECONSTRUCTION_INCLUDE_DIRS}
  CACHE INTERNAL "" FORCE )

# --------------------------------------------------------------------------
# Build the library
SET(${PROJECT_NAME}_LIBS
  vtkPlusCommon
  ${PLUSLIB_VTK_PREFIX}InteractionStyle
  ${PLUSLIB_VTK_PREFIX}RenderingFreeType
  vtkVolumeReconstruction
  )
IF(PLUS_RENDERING_ENABLED)
  LIST(APPEND ${PROJECT_NAME}_LIBS
    ${PLUSLIB_VTK_PREFIX}Rendering${VTK_RENDERING_BACKEND}
    ${PLUSLIB_VTK_PREFIX}RenderingVolume${VTK_RENDERING_BACKEND}
    )
ENDIF()

GENERATE_EXPORT_DIRECTIVE_FILE(vtk${PROJECT_NAME})
ADD_LIBRARY(vtk${PROJECT_NAME} ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_HDRS})
FOREACH(p IN LISTS ${PROJECT_NAME}_INCLUDE_DIRS)
  target_include_directories(vtk${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${p}>)
ENDFOREACH()
target_include_directories(vtk${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${PLUSLIB_INCLUDE_INSTALL}>)
TARGET_LINK_LIBRARIES(vtk${PROJECT_NAME} ${${PROJECT_NAME}_LIBS})
PlusLibAddVersionInfo(vtk${PROJECT_NAME} "Library for performing volume reconstruction of image series. Part of the Plus toolkit." vtk${PROJECT_NAME} vtk${PROJECT_NAME})

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtk${PROJECT_NAME} CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtk${PROJECT_NAME} vtk${PROJECT_NAME};${PlusLib_BINARY_DIR}/src/${PROJECT_NAME}/vtk${PROJECT_NAME}.vcxproj;vtkPlusCommon;vtkPlusDataCollection CACHE INTERNAL "" FORCE)

IF(PLUSBUILD_BUILD_PlusLib_TOOLS)
  ADD_EXECUTABLE(VolumeReconstructor Tools/VolumeReconstructor.cxx )
  SET_TARGET_PROPERTIES(VolumeReconstructor PROPERTIES FOLDER Tools)
  TARGET_LINK_LIBRARIES(VolumeReconstructor 
    vtkPlusVolumeReconstruction
    )
  GENERATE_HELP_DOC(VolumeReconstructor)

  ADD_EXECUTABLE(CreateSliceModels Tools/CreateSliceModels.cxx )
  SET_TARGET_PROPERTIES(CreateSliceModels PROPERTIES FOLDER Tools)
  TARGET_LINK_LIBRARIES(CreateSliceModels 
    vtkPlusCommon 
    vtkPlusVolumeReconstruction 
    ${PLUSLIB_VTK_PREFIX}FiltersSources 
    ${PLUSLIB_VTK_PREFIX}FiltersGeneral
    ${PLUSLIB_VTK_PREFIX}IOLegacy
    )
  GENERATE_HELP_DOC(CreateSliceModels)

  ADD_EXECUTABLE(CompareVolumes Tools/CompareVolumes.cxx Tools/vtkPlusCompareVolumes.cxx )
  SET_TARGET_PROPERTIES(CompareVolumes PROPERTIES FOLDER Tools)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Tools)
  TARGET_LINK_LIBRARIES(CompareVolumes 
    vtkPlusCommon 
    ${PLUSLIB_VTK_PREFIX}IOLegacy 
    ${PLUSLIB_VTK_PREFIX}ImagingMath 
    ${PLUSLIB_VTK_PREFIX}ImagingStatistics
    )

  ADD_EXECUTABLE(DrawClipRegion Tools/DrawClipRegion.cxx )
  SET_TARGET_PROPERTIES(DrawClipRegion PROPERTIES FOLDER Tools)
  TARGET_LINK_LIBRARIES(DrawClipRegion 
    vtkPlusCommon 
    vtkPlusVolumeReconstruction
    )
  GENERATE_HELP_DOC(DrawClipRegion)

  INSTALL(
    TARGETS
      VolumeReconstructor
      CreateSliceModels
      DrawClipRegion
    DESTINATION "${PLUSLIB_BINARY_INSTALL}"
    COMPONENT RuntimeExecutables
    )
ENDIF()

# --------------------------------------------------------------------------
# Install
#
PlusLibInstallLibrary(vtk${PROJECT_NAME} ${PROJECT_NAME})

# --------------------------------------------------------------------------
# Testing
#
IF(BUILD_TESTING)
  SUBDIRS(Testing)
ENDIF()