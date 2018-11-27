PROJECT(PlusRendering)

# --------------------------------------------------------------------------
# Sources
SET(${PROJECT_NAME}_SRCS
  PlusPlotter.cxx
  vtkPlusToolAxesActor.cxx
  )

IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  SET(${PROJECT_NAME}_HDRS
    PlusPlotter.h
    vtkPlusToolAxesActor.h
    )
ENDIF()

SET(${PROJECT_NAME}_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library
SET(${PROJECT_NAME}_LIBS 
  vtkPlusCommon
  vtkIGSIOCommon
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

GENERATE_EXPORT_DIRECTIVE_FILE(vtk${PROJECT_NAME})
ADD_LIBRARY(vtk${PROJECT_NAME} ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_HDRS})
FOREACH(p IN LISTS ${PROJECT_NAME}_INCLUDE_DIRS)
  target_include_directories(vtk${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${p}>)
ENDFOREACH()
target_include_directories(vtk${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${PLUSLIB_INCLUDE_INSTALL}>)
TARGET_LINK_LIBRARIES(vtk${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_LIBS} )
PlusLibAddVersionInfo(vtk${PROJECT_NAME} "Library containing reusable rendering tools. Part of the Plus toolkit." vtk${PROJECT_NAME} vtk${PROJECT_NAME})

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtk${PROJECT_NAME} CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtk${PROJECT_NAME} vtk${PROJECT_NAME};${PlusLib_BINARY_DIR}/src/${PROJECT_NAME}/vtk${PROJECT_NAME}.vcxproj CACHE INTERNAL "" FORCE)

IF(PLUSBUILD_BUILD_PlusLib_TOOLS)
  #--------------------------------------------------------------------------------------------
  ADD_EXECUTABLE(ModelRenderer Tools/ModelRenderer.cxx )
  SET_TARGET_PROPERTIES(ModelRenderer PROPERTIES FOLDER Tools)
  TARGET_LINK_LIBRARIES(ModelRenderer vtkPlusCommon vtk${PROJECT_NAME} vtkIOGeometry)

  INSTALL(TARGETS ModelRenderer
    RUNTIME DESTINATION bin COMPONENT RuntimeExecutables
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