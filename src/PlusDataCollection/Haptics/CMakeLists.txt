PROJECT(PlusHaptics)

# Add entries to project details
SET(${PROJECT_NAME}_INCLUDE_DIRS 
  ${${PROJECT_NAME}_SOURCE_DIR}
  ${${PROJECT_NAME}_BINARY_DIR}
  CACHE INTERNAL "" FORCE)

SET(${PROJECT_NAME}_SRCS
  vtkPlusForceFeedback.cxx
  vtkPlusHapticForce.cxx
  vtkPlusImplicitSplineForce.cxx
  vtkPlusPolydataForce.cxx
  )

IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  SET(${PROJECT_NAME}_HDRS 
    vtkPlusForceFeedback.h 
    vtkPlusHapticForce.h 
    vtkPlusImplicitSplineForce.h 
    vtkPlusPolydataForce.h 
  )
ENDIF()

SET_SOURCE_FILES_PROPERTIES(
  vtkPlusForceFeedback
  ABSTRACT
)

SET(${PROJECT_NAME}_LIBS
  vtkCommonCore
  vtkCommonDataModel
  vtksys
  )

# -----------------------------------------------------------------
# Build the library
GENERATE_EXPORT_DIRECTIVE_FILE(vtk${PROJECT_NAME})
ADD_LIBRARY(vtk${PROJECT_NAME} ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_HDRS})
FOREACH(p IN LISTS ${PROJECT_NAME}_INCLUDE_DIRS)
  target_include_directories(vtk${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${p}>)
ENDFOREACH()
target_include_directories(vtk${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${PLUSLIB_INCLUDE_INSTALL}>)
TARGET_LINK_LIBRARIES(vtk${PROJECT_NAME} ${${PROJECT_NAME}_LIBS})
PlusLibAddVersionInfo(vtk${PROJECT_NAME} "Library containing code for interacting with haptic devices. Part of the Plus toolkit." vtk${PROJECT_NAME} vtk${PROJECT_NAME})

# --------------------------------------------------------------------------
# Install
#
PlusLibInstallLibrary(vtk${PROJECT_NAME} ${PROJECT_NAME})