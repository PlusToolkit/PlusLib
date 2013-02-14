# --------------------------------------------------------------------------
# Sources
SET (PlusServer_SRCS 
  vtkPlusOpenIGTLinkServer.cxx
  vtkPlusOpenIGTLinkClient.cxx
  vtkPlusCommand.cxx 
  vtkPlusCommandProcessor.cxx 
  vtkPlusReconstructVolumeCommand.cxx 
  vtkPlusStartStopRecordingCommand.cxx 
  vtkPlusRequestDeviceIDsCommand.cxx
	)
	
IF (WIN32)	
	SET (PlusServer_HDRS 
    vtkPlusOpenIGTLinkServer.h
    vtkPlusOpenIGTLinkClient.h
    vtkPlusCommand.h
    vtkPlusCommandProcessor.h
    vtkPlusReconstructVolumeCommand.h
    vtkPlusStartStopRecordingCommand.h
    vtkPlusRequestDeviceIDsCommand.h 
		)
ENDIF (WIN32)	

SET (PlusServer_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library

SET (PlusServer_LIBS   
  vtkPlusCommon 
  #vtkPlusOpenIGTLink
  vtkDataCollection
  vtkVolumeReconstruction
  )

INCLUDE_DIRECTORIES( ${PlusServer_INCLUDE_DIRS} )
ADD_LIBRARY(vtkPlusServer ${PlusServer_SRCS} ${PlusServer_HDRS})
TARGET_LINK_LIBRARIES(vtkPlusServer ${PlusServer_LIBS})

# --------------------------------------------------------------------------
# Set external MS project 
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkPlusServer CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkPlusServer vtkPlusServer;"${PlusLib_BINARY_DIR}/src/PlusServer/vtkPlusServer.vcproj";vtkPlusCommon CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Install
#
INSTALL(TARGETS vtkPlusServer
  RUNTIME DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  LIBRARY DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  )
  
# --------------------------------------------------------------------------
# Testing
#

IF (BUILD_TESTING)
	SUBDIRS (Testing)
ENDIF (BUILD_TESTING)
