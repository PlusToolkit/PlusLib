PROJECT(PlusHaptics)

# Add entries to project details
SET(${PROJECT_NAME}_INCLUDE_DIRS ${${PROJECT_NAME}_SOURCE_DIR} ${${PROJECT_NAME}_BINARY_DIR} CACHE INTERNAL "" FORCE)

SET ( ${PROJECT_NAME}_SRCS
  vtkForceFeedback.cxx
  vtkHapticForce.cxx
  vtkImplicitSplineForce.cxx
  vtkPolydataForce.cxx
)

IF( MSVC )
  SET ( ${PROJECT_NAME}_HDRS 
    vtkForceFeedback.h 
    vtkHapticForce.h 
    vtkImplicitSplineForce.h 
    vtkPolydataForce.h 
  )
ENDIF()

SET_SOURCE_FILES_PROPERTIES(
  vtkForceFeedback
  ABSTRACT
)

SET(${PROJECT_NAME}_LIBS ${VTK_LIBRARIES} )

GENERATE_EXPORT_DIRECTIVE_FILE(PlusHaptics)

# -----------------------------------------------------------------
# Build the library
include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})
add_library(${PROJECT_NAME} ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_HDRS})
target_link_libraries(${PROJECT_NAME} ${${PROJECT_NAME}_LIBS})