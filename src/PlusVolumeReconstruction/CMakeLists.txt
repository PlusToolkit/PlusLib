# --------------------------------------------------------------------------
# Sources
SET(PlusVolumeReconstruction_SRCS
  vtkPlusPasteSliceIntoVolume.cxx
  vtkPlusVolumeReconstructor.cxx
  vtkPlusFillHolesInVolume.cxx
  vtkPlusFanAngleDetectorAlgo.cxx
  )

IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  SET(PlusVolumeReconstruction_HDRS
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

SET(PlusVolumeReconstruction_INCLUDE_DIRS 
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  CACHE INTERNAL "" FORCE )

# --------------------------------------------------------------------------
# Build the library
SET(PlusVolumeReconstruction_LIBS
  vtkPlusCommon
  vtkInteractionStyle
  vtkRenderingFreeType
  vtkRendering${VTK_RENDERING_BACKEND}
  vtkRenderingVolume${VTK_RENDERING_BACKEND}
  )

GENERATE_EXPORT_DIRECTIVE_FILE(vtkPlusVolumeReconstruction)
ADD_LIBRARY(vtkPlusVolumeReconstruction ${PlusVolumeReconstruction_SRCS} ${PlusVolumeReconstruction_HDRS})
FOREACH(p IN LISTS PlusVolumeReconstruction_INCLUDE_DIRS)
  target_include_directories(vtkPlusVolumeReconstruction PUBLIC $<BUILD_INTERFACE:${p}>)
ENDFOREACH()
target_include_directories(vtkPlusVolumeReconstruction PUBLIC $<INSTALL_INTERFACE:${PLUSLIB_INCLUDE_INSTALL}>)
TARGET_LINK_LIBRARIES(vtkPlusVolumeReconstruction ${PlusVolumeReconstruction_LIBS})

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkPlusVolumeReconstruction CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkPlusVolumeReconstruction vtkPlusVolumeReconstruction;${PlusLib_BINARY_DIR}/src/PlusVolumeReconstruction/vtkPlusVolumeReconstruction.vcxproj;vtkPlusCommon;vtkPlusDataCollection CACHE INTERNAL "" FORCE)

IF(PLUSBUILD_BUILD_PlusLib_TOOLS)
  ADD_EXECUTABLE(VolumeReconstructor Tools/VolumeReconstructor.cxx )
  SET_TARGET_PROPERTIES(VolumeReconstructor PROPERTIES FOLDER Tools)
  TARGET_LINK_LIBRARIES(VolumeReconstructor vtkPlusVolumeReconstruction )
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
  TARGET_LINK_LIBRARIES(CompareVolumes vtkPlusCommon vtkIOLegacy vtkImagingMath vtkImagingStatistics)

  ADD_EXECUTABLE(DrawClipRegion Tools/DrawClipRegion.cxx )
  SET_TARGET_PROPERTIES(DrawClipRegion PROPERTIES FOLDER Tools)
  TARGET_LINK_LIBRARIES( DrawClipRegion vtkPlusCommon vtkPlusVolumeReconstruction)
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
INSTALL(TARGETS vtkPlusVolumeReconstruction EXPORT PlusLib
  RUNTIME DESTINATION "${PLUSLIB_BINARY_INSTALL}" CONFIGURATIONS Release COMPONENT RuntimeLibraries
  LIBRARY DESTINATION "${PLUSLIB_LIBRARY_INSTALL}" CONFIGURATIONS Release COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION "${PLUSLIB_ARCHIVE_INSTALL}" CONFIGURATIONS Release COMPONENT Development
  )
INSTALL(FILES ${PlusVolumeReconstruction_HDRS}
  DESTINATION "${PLUSLIB_INCLUDE_INSTALL}" CONFIGURATIONS Release COMPONENT Development
  )

# --------------------------------------------------------------------------
# Testing
#
IF(BUILD_TESTING)
  SUBDIRS(Testing)
ENDIF()