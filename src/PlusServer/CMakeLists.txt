# --------------------------------------------------------------------------
# Sources
SET(PlusServer_SRCS
  vtkPlusOpenIGTLinkServer.cxx
  vtkPlusOpenIGTLinkClient.cxx
  vtkPlusCommandResponse.cxx
  vtkPlusCommandProcessor.cxx
  Commands/vtkPlusCommand.cxx
  Commands/vtkPlusVersionCommand.cxx
  Commands/vtkPlusReconstructVolumeCommand.cxx
  Commands/vtkPlusStartStopRecordingCommand.cxx
  Commands/vtkPlusRequestIdsCommand.cxx
  Commands/vtkPlusUpdateTransformCommand.cxx
  Commands/vtkPlusSaveConfigCommand.cxx
  Commands/vtkPlusSendTextCommand.cxx
  Commands/vtkPlusGetImageCommand.cxx
  Commands/vtkPlusGetTransformCommand.cxx
  )

IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  SET(PlusServer_HDRS
    vtkPlusOpenIGTLinkServer.h
    vtkPlusOpenIGTLinkClient.h
    vtkPlusCommandResponse.h
    vtkPlusCommandProcessor.h
    Commands/vtkPlusCommand.h
    Commands/vtkPlusVersionCommand.h
    Commands/vtkPlusReconstructVolumeCommand.h
    Commands/vtkPlusStartStopRecordingCommand.h
    Commands/vtkPlusRequestIdsCommand.h
    Commands/vtkPlusUpdateTransformCommand.h
    Commands/vtkPlusSaveConfigCommand.h
    Commands/vtkPlusSendTextCommand.h
    Commands/vtkPlusGetImageCommand.h
    Commands/vtkPlusGetTransformCommand.h
    )
ENDIF()

IF(PLUS_USE_STEALTHLINK)
  LIST(APPEND PlusServer_SRCS Commands/vtkPlusStealthLinkCommand.cxx)
  IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    LIST(APPEND PlusServer_HDRS Commands/vtkPlusStealthLinkCommand.h)
  ENDIF()
ENDIF()

IF(PLUS_USE_OPTIMET_CONOPROBE)
  LIST(APPEND PlusServer_SRCS Commands/vtkPlusConoProbeLinkCommand.cxx)
  IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    LIST(APPEND PlusServer_HDRS Commands/vtkPlusConoProbeLinkCommand.h)
  ENDIF()
ENDIF()

SET(PlusServer_INCLUDE_DIRS 
  ${CMAKE_CURRENT_SOURCE_DIR} 
  ${CMAKE_CURRENT_BINARY_DIR} 
  ${CMAKE_CURRENT_SOURCE_DIR}/Commands 
  CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library
SET(PlusServer_LIBS
  vtkPlusCommon
  vtkPlusDataCollection
  vtkPlusVolumeReconstruction
  )
SET(PlusServer_PRIVATE_LIBS
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

GENERATE_EXPORT_DIRECTIVE_FILE(vtkPlusServer)
ADD_LIBRARY(vtkPlusServer ${PlusServer_SRCS} ${PlusServer_HDRS})
FOREACH(p IN LISTS PlusServer_INCLUDE_DIRS)
  target_include_directories(vtkPlusServer PUBLIC $<BUILD_INTERFACE:${p}>)
ENDFOREACH()
target_include_directories(vtkPlusServer PUBLIC $<INSTALL_INTERFACE:${PLUSLIB_INCLUDE_INSTALL}>)
IF(NOT (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode"))
  target_compile_options(vtkPlusServer PRIVATE -ftemplate-depth=5000)
ENDIF()
TARGET_LINK_LIBRARIES(vtkPlusServer PUBLIC ${PlusServer_LIBS})
TARGET_LINK_LIBRARIES(vtkPlusServer PRIVATE ${PlusServer_PRIVATE_LIBS})

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkPlusServer CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkPlusServer vtkPlusServer;${PlusLib_BINARY_DIR}/src/PlusServer/vtkPlusServer.vcxproj;vtkPlusCommon CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Executables built by PlusServer project
IF(PLUSBUILD_BUILD_PlusLib_TOOLS)
  ADD_EXECUTABLE(PlusServer Tools/PlusServer.cxx)
  SET_TARGET_PROPERTIES(PlusServer PROPERTIES FOLDER Tools)
  TARGET_LINK_LIBRARIES(PlusServer vtkPlusServer vtkPlusDataCollection)

  ADD_EXECUTABLE(PlusServerRemoteControl Tools/PlusServerRemoteControl.cxx )
  SET_TARGET_PROPERTIES(PlusServerRemoteControl PROPERTIES FOLDER Tools)
  TARGET_LINK_LIBRARIES(PlusServerRemoteControl vtkPlusDataCollection vtkPlusServer)
ENDIF()

# --------------------------------------------------------------------------
# Install
#
INSTALL(TARGETS vtkPlusServer EXPORT PlusLib
  RUNTIME DESTINATION "${PLUSLIB_BINARY_INSTALL}" CONFIGURATIONS Release COMPONENT RuntimeLibraries
  LIBRARY DESTINATION "${PLUSLIB_LIBRARY_INSTALL}" CONFIGURATIONS Release COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION "${PLUSLIB_ARCHIVE_INSTALL}" CONFIGURATIONS Release COMPONENT Development
  )
INSTALL(FILES ${PlusServer_HDRS}
  DESTINATION "${PLUSLIB_INCLUDE_INSTALL}" CONFIGURATIONS Release COMPONENT Development
  )

IF(PLUSBUILD_BUILD_PlusLib_TOOLS)
  INSTALL(TARGETS 
      PlusServer 
      PlusServerRemoteControl 
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