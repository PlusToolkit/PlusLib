# --------------------------------------------------------------------------
# Options

# --------------------------------------------------------------------------
# Sources
SET (PlusVolumeReconstruction_SRCS
  vtkPlusPasteSliceIntoVolume.cxx
  vtkPlusVolumeReconstructor.cxx
  vtkPlusFillHolesInVolume.cxx
  vtkPlusFanAngleDetectorAlgo.cxx
  )

SET (PlusVolumeReconstruction_HDRS)
if (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  SET (PlusVolumeReconstruction_HDRS
    fixed.h
    vtkPlusPasteSliceIntoVolume.h
    vtkPlusPasteSliceIntoVolumeHelperCommon.h
    vtkPlusPasteSliceIntoVolumeHelperOptimized.h
    vtkPlusPasteSliceIntoVolumeHelperUnoptimized.h
    vtkPlusVolumeReconstructor.h
    vtkPlusFillHolesInVolume.h
    vtkPlusFanAngleDetectorAlgo.h
    )
endif ()  

SET (PlusVolumeReconstruction_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "" FORCE )

# --------------------------------------------------------------------------
# Build the library

SET (PlusVolumeReconstruction_LIBS
  ${VTK_LIBRARIES}
  vtkPlusCommon
  )

GENERATE_EXPORT_DIRECTIVE_FILE(vtkPlusVolumeReconstruction)
ADD_LIBRARY(vtkPlusVolumeReconstruction ${PlusVolumeReconstruction_SRCS} ${PlusVolumeReconstruction_HDRS})
target_include_directories(vtkPlusVolumeReconstruction PUBLIC ${PlusVolumeReconstruction_INCLUDE_DIRS} )
TARGET_LINK_LIBRARIES(  vtkPlusVolumeReconstruction  ${PlusVolumeReconstruction_LIBS} )

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkPlusVolumeReconstruction CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkPlusVolumeReconstruction vtkPlusVolumeReconstruction;"${PlusLib_BINARY_DIR}/src/PlusVolumeReconstruction/vtkPlusVolumeReconstruction.vcxproj";vtkPlusCommon;vtkPlusDataCollection CACHE INTERNAL "" FORCE)

IF(PLUSBUILD_BUILD_PlusLib_TOOLS)
  ADD_EXECUTABLE(VolumeReconstructor Tools/VolumeReconstructor.cxx )
  SET_TARGET_PROPERTIES(VolumeReconstructor PROPERTIES FOLDER Tools)
  TARGET_LINK_LIBRARIES(VolumeReconstructor vtkPlusVolumeReconstruction )
  GENERATE_HELP_DOC(VolumeReconstructor)

  ADD_EXECUTABLE(CreateSliceModels Tools/CreateSliceModels.cxx )
  SET_TARGET_PROPERTIES(CreateSliceModels PROPERTIES FOLDER Tools)
  TARGET_LINK_LIBRARIES( CreateSliceModels vtkPlusCommon vtkPlusVolumeReconstruction )
  GENERATE_HELP_DOC(CreateSliceModels)

  ADD_EXECUTABLE(CompareVolumes Tools/CompareVolumes.cxx Tools/vtkPlusCompareVolumes.cxx )
  SET_TARGET_PROPERTIES(CompareVolumes PROPERTIES FOLDER Tools)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Tools)
  TARGET_LINK_LIBRARIES( CompareVolumes vtkPlusCommon )

  ADD_EXECUTABLE(DrawClipRegion Tools/DrawClipRegion.cxx )
  SET_TARGET_PROPERTIES(DrawClipRegion PROPERTIES FOLDER Tools)
  TARGET_LINK_LIBRARIES( DrawClipRegion vtkPlusCommon vtkPlusVolumeReconstruction)
  GENERATE_HELP_DOC(DrawClipRegion)

  INSTALL(
    TARGETS
      VolumeReconstructor
      CreateSliceModels
      DrawClipRegion
    DESTINATION bin
    COMPONENT RuntimeExecutables
    )
ENDIF()

# --------------------------------------------------------------------------
# Install
#

INSTALL(TARGETS vtkPlusVolumeReconstruction
  RUNTIME DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  LIBRARY DESTINATION lib CONFIGURATIONS Release COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION lib CONFIGURATIONS Release COMPONENT Development
  )

# --------------------------------------------------------------------------
# Testing
#

IF (BUILD_TESTING)
  SUBDIRS (Testing)
ENDIF (BUILD_TESTING)
