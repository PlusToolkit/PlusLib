# --------------------------------------------------------------------------
# Sources
SET (PlusOpenIGTLink_SRCS 
	igtlStringMessage1.cxx
	igtlPlusClientInfoMessage.cxx
	igtlPlusUsMessage.cxx 
	igtlPlusTrackedFrameMessage.cxx 
	PlusIgtlClientInfo.cxx 
	vtkPlusIgtlMessageFactory.cxx 
	vtkPlusIgtlMessageCommon.cxx 
	vtkIGTLMessageQueue.cxx
	)
	
IF (WIN32)	
	SET (PlusOpenIGTLink_HDRS 
		igtlStringMessage1.h
		igtlPlusClientInfoMessage.h
		igtlPlusUsMessage.h
		igtlPlusTrackedFrameMessage.h
		PlusIgtlClientInfo.h 
		vtkPlusIgtlMessageFactory.h
		vtkPlusIgtlMessageCommon.h
		vtkIGTLMessageQueue.h
		)
ENDIF (WIN32)	

SET (PlusOpenIGTLink_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library

SET (PlusOpenIGTLink_LIBS 
  vtkCommon 
  vtkHybrid 
  vtkFiltering 
  vtkPlusCommon 
  OpenIGTLink
  )

INCLUDE_DIRECTORIES( ${PlusOpenIGTLink_INCLUDE_DIRS} )
ADD_LIBRARY(vtkPlusOpenIGTLink ${PlusOpenIGTLink_SRCS} ${PlusOpenIGTLink_HDRS})
TARGET_LINK_LIBRARIES(vtkPlusOpenIGTLink ${PlusOpenIGTLink_LIBS})

# --------------------------------------------------------------------------
# Set external MS project 
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkPlusOpenIGTLink CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkPlusOpenIGTLink vtkPlusOpenIGTLink;"${PlusLib_BINARY_DIR}/src/PlusOpenIGTLink/vtkPlusOpenIGTLink.vcproj";vtkPlusCommon CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Install
#
INSTALL(TARGETS vtkPlusOpenIGTLink
  RUNTIME DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  LIBRARY DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  )

# --------------------------------------------------------------------------
# Testing
#

IF (BUILD_TESTING)
	SUBDIRS (Testing)
ENDIF (BUILD_TESTING)
