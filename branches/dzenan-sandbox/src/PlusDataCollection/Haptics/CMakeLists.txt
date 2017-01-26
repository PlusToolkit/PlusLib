PROJECT(vtkPlusHaptics)

# Add entries to project details
SET(${PROJECT_NAME}_INCLUDE_DIRS ${${PROJECT_NAME}_SOURCE_DIR} ${${PROJECT_NAME}_BINARY_DIR} CACHE INTERNAL "" FORCE)

SET ( ${PROJECT_NAME}_SRCS
  vtkPlusForceFeedback.cxx
  vtkPlusHapticForce.cxx
  vtkPlusImplicitSplineForce.cxx
  vtkPlusPolydataForce.cxx
)

IF( MSVC )
  SET ( ${PROJECT_NAME}_HDRS 
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

SET(${PROJECT_NAME}_LIBS ${VTK_LIBRARIES} )

GENERATE_EXPORT_DIRECTIVE_FILE(vtkPlusHaptics)

# -----------------------------------------------------------------
# Build the library
include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})
add_library(${PROJECT_NAME} ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_HDRS})
target_link_libraries(${PROJECT_NAME} ${${PROJECT_NAME}_LIBS})