PROJECT(vtkPlusHaptics)

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

GENERATE_EXPORT_DIRECTIVE_FILE(vtkPlusHaptics)

# -----------------------------------------------------------------
# Build the library
ADD_LIBRARY(${PROJECT_NAME} ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_HDRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${${PROJECT_NAME}_LIBS})
FOREACH(p IN LISTS ${PROJECT_NAME}_INCLUDE_DIRS)
  target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${p}>)
ENDFOREACH()
target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${PLUSLIB_INCLUDE_INSTALL}>)

# --------------------------------------------------------------------------
# Install
#
INSTALL(TARGETS ${PROJECT_NAME} EXPORT PlusLib
  RUNTIME DESTINATION "${PLUSLIB_BINARY_INSTALL}" CONFIGURATIONS Release COMPONENT RuntimeLibraries
  LIBRARY DESTINATION "${PLUSLIB_LIBRARY_INSTALL}" CONFIGURATIONS Release COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION "${PLUSLIB_ARCHIVE_INSTALL}" CONFIGURATIONS Release COMPONENT Development
  )
INSTALL(FILES ${${PROJECT_NAME}_INSTALL_INCLUDE_FILES}
  DESTINATION "${${PROJECT_NAME}_HDRS}" CONFIGURATIONS Release COMPONENT Development)