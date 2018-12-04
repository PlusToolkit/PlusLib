PROJECT(PlusVolumeReconstruction)

# --------------------------------------------------------------------------
# Sources
SET(${PROJECT_NAME}_SRCS
  vtkPlusPasteSliceIntoVolume.cxx
  vtkPlusVolumeReconstructor.cxx
  vtkPlusFillHolesInVolume.cxx
  vtkPlusFanAngleDetectorAlgo.cxx
  )

IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  SET(${PROJECT_NAME}_HDRS
    fixed.h
    vtkPlusPasteSliceIntoVolume.h
    vtkPlusPasteSliceIntoVolumeHelperCommon.h
    vtkPlusPasteSliceIntoVolumeHelperOptimized.h
    vtkPlusPasteSliceIntoVolumeHelperUnoptimized.h
    vtkPlusVolumeReconstructor.h
    vtkPlusFillHolesInVolume.h
    vtkPlusFanAngleDetectorAlgo.h
    )
ENDIF()

SET(${PROJECT_NAME}_INCLUDE_DIRS 
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  CACHE INTERNAL "" FORCE )

# --------------------------------------------------------------------------
# Build the library
SET(${PROJECT_NAME}_LIBS
  vtkPlusCommon
  vtkIGSIOCommon
  vtkInteractionStyle
  vtkRenderingFreeType
  vtkRendering${VTK_RENDERING_BACKEND}
  vtkRenderingVolume${VTK_RENDERING_BACKEND}
  )

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
    vtkFiltersSources 
    vtkFiltersGeneral
    vtkIOLegacy
    )
  GENERATE_HELP_DOC(CreateSliceModels)

  ADD_EXECUTABLE(CompareVolumes Tools/CompareVolumes.cxx Tools/vtkPlusCompareVolumes.cxx )
  SET_TARGET_PROPERTIES(CompareVolumes PROPERTIES FOLDER Tools)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Tools)
  TARGET_LINK_LIBRARIES(CompareVolumes 
    vtkPlusCommon 
    vtkIOLegacy 
    vtkImagingMath 
    vtkImagingStatistics
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