PROJECT(PlusWidgets)

# --------------------------------------------------------------------------
# Sources
SET(${PROJECT_NAME}_SRCS
  QPlusDeviceSetSelectorWidget.cxx
  QPlusStatusIcon.cxx
  QPlusToolStateDisplayWidget.cxx
  QPlusConfigFileSaverDialog.cxx
  )

SET(${PROJECT_NAME}_HDRS
  QPlusDeviceSetSelectorWidget.h
  QPlusStatusIcon.h
  QPlusToolStateDisplayWidget.h
  QPlusConfigFileSaverDialog.h
  )

SET(${PROJECT_NAME}_UI_SRCS
  QPlusDeviceSetSelectorWidget.ui
  QPlusConfigFileSaverDialog.ui
  )

SET(_Qt_resource ${PROJECT_NAME}.qrc)
SET(PLUSLIB_WIDGETS_QRC ${CMAKE_CURRENT_SOURCE_DIR}/${_Qt_resource} CACHE STRING "Location of the PlusWidgets qrc file" FORCE)
MARK_AS_ADVANCED(PLUSLIB_WIDGETS_QRC)

# --------------------------------------------------------------------------
# Build the library
SET(${PROJECT_NAME}_LIBS 
  Qt5::Xml
  Qt5::Widgets
  vtkGUISupportQt
  vtkPlusCalibration
  vtkPlusCommon
  vtkIGSIOCommon
  vtkPlusDataCollection
  )

SET(${PROJECT_NAME}_INCLUDE_DIRS
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )
# If CMake 3.7 is the minimum version then the next line can be
# changed to: IF(CMAKE_VERSION VERSION_GREATER_EQUAL "3.8")
IF(NOT (CMAKE_VERSION VERSION_LESS "3.8"))
  # CMake 3.8 and later puts generated headers (such as ui_QPlusDeviceSetSelectorWidget.h) into
  # .../PlusLib-bin/src/PlusWidgets/PlusWidgets_autogen/include
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_autogen/include)
  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_autogen/include)

  # CMake 3.10 puts generated headers (such as ui_QPlusDeviceSetSelectorWidget.h) into
  # .../PlusLib-bin/src/PlusWidgets/PlusWidgets_autogen/include_<Config>
  FOREACH(CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES})
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_autogen/include_${CONFIG_TYPE})
    LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_autogen/include_${CONFIG_TYPE})
  ENDFOREACH()
ENDIF()

GENERATE_EXPORT_DIRECTIVE_FILE(${PROJECT_NAME})
ADD_LIBRARY(${PROJECT_NAME}
  ${${PROJECT_NAME}_SRCS}
  ${${PROJECT_NAME}_HDRS} 
  ${${PROJECT_NAME}_UI_SRCS} 
  ${_Qt_resource}
  )
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES 
  COMPILE_DEFINITIONS 
  ${Qt5Widgets_COMPILE_DEFINITIONS} 
  )
TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_LIBS})
FOREACH(p IN LISTS ${PROJECT_NAME}_INCLUDE_DIRS)
  target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${p}>)
ENDFOREACH()
target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${PLUSLIB_INCLUDE_INSTALL}>)
PlusLibAddVersionInfo(${PROJECT_NAME} "Library containing reusable Qt widgets that interact with Plus toolkit classes." ${PROJECT_NAME} ${PROJECT_NAME})

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} ${PROJECT_NAME} CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_${PROJECT_NAME} ${PROJECT_NAME};${PlusLib_BINARY_DIR}/src/PlusWidgets/${PROJECT_NAME}.vcxproj CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Install
#
PlusLibInstallLibrary(${PROJECT_NAME} ${PROJECT_NAME})

# --------------------------------------------------------------------------
# Testing
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY (Testing)
ENDIF()