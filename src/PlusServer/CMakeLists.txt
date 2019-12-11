PROJECT(PlusServer)

# --------------------------------------------------------------------------
# Sources
SET(${PROJECT_NAME}_CMD_SRCS
  Commands/vtkPlusCommand.cxx
  Commands/vtkPlusVersionCommand.cxx
  Commands/vtkPlusReconstructVolumeCommand.cxx
  Commands/vtkPlusStartStopRecordingCommand.cxx
  Commands/vtkPlusRequestIdsCommand.cxx
  Commands/vtkPlusUpdateTransformCommand.cxx
  Commands/vtkPlusSaveConfigCommand.cxx
  Commands/vtkPlusSendTextCommand.cxx
  Commands/vtkPlusGetImageCommand.cxx
  Commands/vtkPlusGetPolydataCommand.cxx
  Commands/vtkPlusGetTransformCommand.cxx
  Commands/vtkPlusSetUsParameterCommand.cxx
  Commands/vtkPlusGetUsParameterCommand.cxx
  Commands/vtkPlusAddRecordingDeviceCommand.cxx
  Commands/vtkPlusAtracsysCommand.cxx
  )
SET(${PROJECT_NAME}_SRCS
  vtkPlusOpenIGTLinkServer.cxx
  vtkPlusOpenIGTLinkClient.cxx
  vtkPlusCommandResponse.cxx
  vtkPlusCommandProcessor.cxx
  ${${PROJECT_NAME}_CMD_SRCS}
  )

IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  SET(${PROJECT_NAME}_CMD_HDRS
    Commands/vtkPlusCommand.h
    Commands/vtkPlusVersionCommand.h
    Commands/vtkPlusReconstructVolumeCommand.h
    Commands/vtkPlusStartStopRecordingCommand.h
    Commands/vtkPlusRequestIdsCommand.h
    Commands/vtkPlusUpdateTransformCommand.h
    Commands/vtkPlusSaveConfigCommand.h
    Commands/vtkPlusSendTextCommand.h
    Commands/vtkPlusGetImageCommand.h
    Commands/vtkPlusGetPolydataCommand.h
    Commands/vtkPlusGetTransformCommand.h
    Commands/vtkPlusSetUsParameterCommand.h
    Commands/vtkPlusGetUsParameterCommand.h
    Commands/vtkPlusAddRecordingDeviceCommand.h
    Commands/vtkPlusAtracsysCommand.h
    )
  SET(${PROJECT_NAME}_HDRS
    vtkPlusOpenIGTLinkServer.h
    vtkPlusOpenIGTLinkClient.h
    vtkPlusCommandResponse.h
    vtkPlusCommandProcessor.h
    ${${PROJECT_NAME}_CMD_HDRS}
    )
ENDIF()

IF(PLUS_USE_STEALTHLINK)
  LIST(APPEND ${PROJECT_NAME}_SRCS Commands/vtkPlusStealthLinkCommand.cxx)
  IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    LIST(APPEND ${PROJECT_NAME}_HDRS Commands/vtkPlusStealthLinkCommand.h)
  ENDIF()
ENDIF()

IF (PLUS_USE_CLARIUS)
  LIST(APPEND ${PROJECT_NAME}_SRCS Commands/vtkPlusClariusCommand.cxx)
  IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    LIST(APPEND ${PROJECT_NAME}_HDRS Commands/vtkPlusClariusCommand.h)
  ENDIF()
ENDIF()

IF(PLUS_USE_OPTIMET_CONOPROBE)
  LIST(APPEND ${PROJECT_NAME}_SRCS Commands/vtkPlusConoProbeLinkCommand.cxx)
  IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    LIST(APPEND ${PROJECT_NAME}_HDRS Commands/vtkPlusConoProbeLinkCommand.h)
  ENDIF()
ENDIF()

SET(${PROJECT_NAME}_INCLUDE_DIRS 
  ${CMAKE_CURRENT_SOURCE_DIR} 
  ${CMAKE_CURRENT_BINARY_DIR} 
  ${CMAKE_CURRENT_SOURCE_DIR}/Commands 
  CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library
SET(${PROJECT_NAME}_LIBS
  vtkPlusCommon
  vtkPlusDataCollection
  vtkPlusVolumeReconstruction
  vtkIOPLY
  )
SET(${PROJECT_NAME}_PRIVATE_LIBS
  igtlioConverter
  )

# If igtlioConverter was compiled as a static library, we do not need igtlio in the install configuration
GET_PROPERTY(IGTLIO_LIB_TYPE TARGET igtlioConverter PROPERTY STATIC_LIBRARY_FLAGS)
IF(NOT IGTLIO_LIB_TYPE STREQUAL "STATIC_LIBRARY")
  SET(OPENIGTLINKIO_CONFIG_CODE [=[
    IF(NOT DEFINED OpenIGTLinkIO_DIR)
      SET(OpenIGTLinkIO_DIR "@OpenIGTLinkIO_DIR@")
    ENDIF()
    FIND_PACKAGE(OpenIGTLinkIO NO_MODULE REQUIRED)]=])
ENDIF()

GENERATE_EXPORT_DIRECTIVE_FILE(vtk${PROJECT_NAME})
ADD_LIBRARY(vtk${PROJECT_NAME} ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_HDRS})
FOREACH(p IN LISTS ${PROJECT_NAME}_INCLUDE_DIRS)
  target_include_directories(vtk${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${p}>)
ENDFOREACH()
target_include_directories(vtk${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${PLUSLIB_INCLUDE_INSTALL}>)
IF(NOT (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode"))
  target_compile_options(vtk${PROJECT_NAME} PRIVATE -ftemplate-depth=5000)
ENDIF()
TARGET_LINK_LIBRARIES(vtk${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_LIBS})
TARGET_LINK_LIBRARIES(vtk${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_PRIVATE_LIBS})
PlusLibAddVersionInfo(vtk${PROJECT_NAME} "Library containing classes and functionality necessary for providing OpenIGTLink servers and clients. Part of the Plus toolkit." vtk${PROJECT_NAME} vtk${PROJECT_NAME})

source_group("Commands" FILES ${${PROJECT_NAME}_CMD_HDRS} ${${PROJECT_NAME}_CMD_SRCS})

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtk${PROJECT_NAME} CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtk${PROJECT_NAME} vtk${PROJECT_NAME};${PlusLib_BINARY_DIR}/src/${PROJECT_NAME}/vtk${PROJECT_NAME}.vcxproj;vtkPlusCommon CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Executables built by ${PROJECT_NAME} project
IF(PLUSBUILD_BUILD_PlusLib_TOOLS)
  ADD_EXECUTABLE(${PROJECT_NAME} Tools/${PROJECT_NAME}.cxx)
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES FOLDER Tools)
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} vtk${PROJECT_NAME} vtkPlusDataCollection)

  ADD_EXECUTABLE(${PROJECT_NAME}RemoteControl Tools/${PROJECT_NAME}RemoteControl.cxx )
  SET_TARGET_PROPERTIES(${PROJECT_NAME}RemoteControl PROPERTIES FOLDER Tools)
  TARGET_LINK_LIBRARIES(${PROJECT_NAME}RemoteControl vtkPlusDataCollection vtk${PROJECT_NAME})
ENDIF()

# --------------------------------------------------------------------------
# Install
#
PlusLibInstallLibrary(vtk${PROJECT_NAME} ${PROJECT_NAME})

IF(PLUSBUILD_BUILD_PlusLib_TOOLS)
  INSTALL(TARGETS 
      ${PROJECT_NAME} 
      ${PROJECT_NAME}RemoteControl 
    EXPORT PlusLib
    DESTINATION "${PLUSLIB_BINARY_INSTALL}" 
    COMPONENT RuntimeExecutables
    )
ENDIF()

# --------------------------------------------------------------------------
# Testing
#
IF(BUILD_TESTING)
  SUBDIRS(Testing)
ENDIF()