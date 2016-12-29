# --------------------------------------------------------------------------
# Sources

SET (PlusRendering_SRCS
  vtkPlusToolAxesActor.cxx
  )

SET (PlusRendering_HDRS)
if (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  SET (PlusRendering_HDRS
    vtkPlusToolAxesActor.h
    )
endif ()

SET (PlusRendering_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library
SET (PlusRendering_LIBS 
  vtkPlusCommon
  vtkCommonCore
  vtkFiltersSources
  vtkInteractionStyle
  vtkRenderingFreeType
  vtkRenderingAnnotation
  vtkRendering${VTK_RENDERING_BACKEND}
  )

GENERATE_EXPORT_DIRECTIVE_FILE(vtkPlusRendering)
ADD_LIBRARY(vtkPlusRendering ${PlusRendering_SRCS} ${PlusRendering_HDRS})
target_include_directories(vtkPlusRendering PUBLIC ${PlusRendering_INCLUDE_DIRS} )
TARGET_LINK_LIBRARIES(vtkPlusRendering PUBLIC ${PlusRendering_LIBS} )

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkPlusRendering CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkPlusRendering vtkPlusRendering;${PlusLib_BINARY_DIR}/src/PlusRendering/vtkPlusRendering.vcxproj CACHE INTERNAL "" FORCE)

#--------------------------------------------------------------------------------------------
IF(PLUSBUILD_BUILD_PlusLib_TOOLS)
  #--------------------------------------------------------------------------------------------
  ADD_EXECUTABLE(ModelRenderer Tools/ModelRenderer.cxx )
  SET_TARGET_PROPERTIES(ModelRenderer PROPERTIES FOLDER Tools)
  TARGET_LINK_LIBRARIES(ModelRenderer vtkPlusCommon vtkPlusRendering vtkIOGeometry)

  INSTALL(TARGETS ModelRenderer
    RUNTIME DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeExecutables
    )
ENDIF()

# --------------------------------------------------------------------------
# Install
#
INSTALL(TARGETS vtkPlusRendering
  RUNTIME DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  LIBRARY DESTINATION lib CONFIGURATIONS Release COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION lib CONFIGURATIONS Release COMPONENT Development
  )
INSTALL(FILES ${PlusRendering_HDRS}
  DESTINATION include
  )

# --------------------------------------------------------------------------
# Testing
#
IF (BUILD_TESTING)
  SUBDIRS (Testing)
ENDIF (BUILD_TESTING)
