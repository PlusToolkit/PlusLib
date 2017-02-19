# --------------------------------------------------------------------------
# Sources
SET(PlusRendering_SRCS
  PlusPlotter.cxx
  vtkPlusToolAxesActor.cxx
  )

IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  SET(PlusRendering_HDRS
    PlusPlotter.h
    vtkPlusToolAxesActor.h
    )
ENDIF()

SET(PlusRendering_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library
SET(PlusRendering_LIBS 
  vtkPlusCommon
  vtkCommonCore
  vtkFiltersSources
  vtkChartsCore
  vtkViewsContext2D
  vtkIOCore
  vtkInteractionStyle
  vtkRenderingFreeType
  vtkRenderingAnnotation
  vtkRendering${VTK_RENDERING_BACKEND}
  vtkRenderingContext${VTK_RENDERING_BACKEND}
  )

GENERATE_EXPORT_DIRECTIVE_FILE(vtkPlusRendering)
ADD_LIBRARY(vtkPlusRendering ${PlusRendering_SRCS} ${PlusRendering_HDRS})
FOREACH(p IN LISTS PlusRendering_INCLUDE_DIRS)
  target_include_directories(vtkPlusRendering PUBLIC $<BUILD_INTERFACE:${p}>)
ENDFOREACH()
target_include_directories(vtkPlusRendering PUBLIC $<INSTALL_INTERFACE:${PLUSLIB_INCLUDE_INSTALL}>)
TARGET_LINK_LIBRARIES(vtkPlusRendering PUBLIC ${PlusRendering_LIBS} )

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkPlusRendering CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkPlusRendering vtkPlusRendering;${PlusLib_BINARY_DIR}/src/PlusRendering/vtkPlusRendering.vcxproj CACHE INTERNAL "" FORCE)

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
INSTALL(TARGETS vtkPlusRendering EXPORT PlusLib
  RUNTIME DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  LIBRARY DESTINATION lib CONFIGURATIONS Release COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION lib CONFIGURATIONS Release COMPONENT Development
  )
INSTALL(FILES ${PlusRendering_HDRS}
  DESTINATION "${PLUSLIB_INCLUDE_INSTALL}" CONFIGURATIONS Release COMPONENT Development
  )

# --------------------------------------------------------------------------
# Testing
#
IF(BUILD_TESTING)
  SUBDIRS(Testing)
ENDIF()