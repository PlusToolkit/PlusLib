# --------------------------------------------------------------------------
# Sources
SET (PlusOpenIGTLink_SRCS 
  igtlPlusClientInfoMessage.cxx
  igtlPlusUsMessage.cxx 
  igtlPlusTrackedFrameMessage.cxx 
  PlusIgtlClientInfo.cxx 
  vtkPlusIgtlMessageFactory.cxx 
  vtkPlusIgtlMessageCommon.cxx 
  vtkPlusIGTLMessageQueue.cxx
  )
  
IF (WIN32)
  SET (PlusOpenIGTLink_HDRS 
    igtlPlusClientInfoMessage.h
    igtlPlusUsMessage.h
    igtlPlusTrackedFrameMessage.h
    PlusIgtlClientInfo.h 
    vtkPlusIgtlMessageFactory.h
    vtkPlusIgtlMessageCommon.h
    vtkPlusIGTLMessageQueue.h
    )
ENDIF (WIN32)

SET (PlusOpenIGTLink_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library

SET (PlusOpenIGTLink_LIBS 
  ${VTK_LIBRARIES} 
  vtkPlusCommon
  OpenIGTLink
  )

GENERATE_EXPORT_DIRECTIVE_FILE(vtkPlusOpenIGTLink)
ADD_LIBRARY(vtkPlusOpenIGTLink ${PlusOpenIGTLink_SRCS} ${PlusOpenIGTLink_HDRS})
target_include_directories(vtkPlusOpenIGTLink PUBLIC ${PlusOpenIGTLink_INCLUDE_DIRS} )
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
  LIBRARY DESTINATION lib CONFIGURATIONS Release COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION lib CONFIGURATIONS Release COMPONENT Development
  )

# --------------------------------------------------------------------------
# Testing
#

IF (BUILD_TESTING)
  SUBDIRS (Testing)
ENDIF (BUILD_TESTING)
