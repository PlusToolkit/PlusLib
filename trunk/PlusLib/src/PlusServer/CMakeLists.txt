# --------------------------------------------------------------------------
# Sources
SET (PlusServer_SRCS 
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

IF (WIN32)
  SET (PlusServer_HDRS 
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
ENDIF (WIN32)

IF (PLUS_USE_STEALTHLINK)
  SET (PlusServer_SRCS  ${PlusServer_SRCS} Commands/vtkPlusStealthLinkCommand.cxx)
  IF (WIN32)
    SET (PlusServer_HDRS ${PlusServer_HDRS} Commands/vtkPlusStealthLinkCommand.h)
  ENDIF (WIN32)
ENDIF(PLUS_USE_STEALTHLINK)

IF (PLUS_USE_OPTIMET_CONOPROBE)
  SET (PlusServer_SRCS  ${PlusServer_SRCS} Commands/vtkPlusConoProbeLinkCommand.cxx)
  IF (WIN32)
    SET (PlusServer_HDRS ${PlusServer_HDRS} Commands/vtkPlusConoProbeLinkCommand.h)
  ENDIF (WIN32)
ENDIF(PLUS_USE_OPTIMET_CONOPROBE)

SET (PlusServer_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Commands CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library

SET (PlusServer_LIBS   
  vtkPlusCommon 
  #vtkPlusOpenIGTLink
  vtkPlusDataCollection
  vtkPlusVolumeReconstruction
  )

INCLUDE_DIRECTORIES( ${PlusServer_INCLUDE_DIRS} )
GENERATE_EXPORT_DIRECTIVE_FILE(vtkPlusServer)
ADD_LIBRARY(vtkPlusServer ${PlusServer_SRCS} ${PlusServer_HDRS})
TARGET_LINK_LIBRARIES(vtkPlusServer ${PlusServer_LIBS})

# --------------------------------------------------------------------------
# Set external MS project 
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkPlusServer CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkPlusServer vtkPlusServer;"${PlusLib_BINARY_DIR}/src/PlusServer/vtkPlusServer.vcproj";vtkPlusCommon CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Executables built by PlusServer project
ADD_EXECUTABLE( PlusServer PlusServer.cxx )
TARGET_LINK_LIBRARIES( PlusServer vtkPlusServer vtkPlusDataCollection ${VTK_LIBRARIES} )

ADD_EXECUTABLE( PlusServerRemoteControl PlusServerRemoteControl.cxx )
TARGET_LINK_LIBRARIES( PlusServerRemoteControl vtkPlusDataCollection ${VTK_LIBRARIES} vtkPlusServer )
  
# --------------------------------------------------------------------------
# Install
#
INSTALL(TARGETS vtkPlusServer
  RUNTIME DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  LIBRARY DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION bin CONFIGURATIONS Release COMPONENT Development
  )

INSTALL(TARGETS PlusServer PlusServerRemoteControl
  DESTINATION bin COMPONENT RuntimeExecutables
  )
  
# --------------------------------------------------------------------------
# Testing
#

IF (BUILD_TESTING)
  SUBDIRS (Testing)
ENDIF (BUILD_TESTING)
