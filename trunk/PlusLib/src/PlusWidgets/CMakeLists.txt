PROJECT(PlusWidgets)

# --------------------------------------------------------------------------
# Sources
SET (${PROJECT_NAME}_SRCS
  QPlusDeviceSetSelectorWidget.cxx
  QPlusStatusIcon.cxx
  QPlusToolStateDisplayWidget.cxx
  QPlusConfigFileSaverDialog.cxx
  )

SET (${PROJECT_NAME}_UI_HDRS
  QPlusDeviceSetSelectorWidget.h
  QPlusStatusIcon.h
  QPlusToolStateDisplayWidget.h
  QPlusConfigFileSaverDialog.h
  )

SET (${PROJECT_NAME}_UI_SRCS
  QPlusDeviceSetSelectorWidget.ui
  QPlusConfigFileSaverDialog.ui
  )

SET(_Qt_resource ${PROJECT_NAME}.qrc)
SET(PLUSLIB_WIDGETS_QRC ${CMAKE_CURRENT_SOURCE_DIR}/${_Qt_resource} CACHE STRING "Location of the PlusWidgets qrc file" FORCE)
MARK_AS_ADVANCED(PLUSLIB_WIDGETS_QRC)

# --------------------------------------------------------------------------
# Build the library
SET (${PROJECT_NAME}_LIBS 
  Qt5::Xml
  Qt5::Widgets
  vtkGUISupportQt
  vtkPlusCalibration
  vtkPlusCommon
  vtkPlusDataCollection
  )

GENERATE_EXPORT_DIRECTIVE_FILE(${PROJECT_NAME})
ADD_LIBRARY(${PROJECT_NAME}
  ${${PROJECT_NAME}_SRCS} 
  ${${PROJECT_NAME}_UI_HDRS} 
  ${${PROJECT_NAME}_UI_SRCS} 
  ${_Qt_resource}
  )
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES 
  COMPILE_DEFINITIONS 
  ${Qt5Widgets_COMPILE_DEFINITIONS} 
  )
TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_LIBS})
target_include_directories(${PROJECT_NAME} PUBLIC 
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} ${PROJECT_NAME} CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_${PROJECT_NAME} ${PROJECT_NAME};${PlusLib_BINARY_DIR}/src/PlusWidgets/${PROJECT_NAME}.vcxproj CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Testing
IF (BUILD_TESTING)
  ADD_SUBDIRECTORY (Testing)
ENDIF()