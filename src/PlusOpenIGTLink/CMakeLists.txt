# --------------------------------------------------------------------------
# Sources
SET(PlusOpenIGTLink_SRCS
  igtlPlusClientInfoMessage.cxx
  igtlPlusUsMessage.cxx
  igtlPlusTrackedFrameMessage.cxx
  PlusIgtlClientInfo.cxx
  vtkPlusIgtlMessageFactory.cxx
  vtkPlusIgtlMessageCommon.cxx
  vtkPlusIGTLMessageQueue.cxx
  )

IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  SET(PlusOpenIGTLink_HDRS
    igtlPlusClientInfoMessage.h
    igtlPlusUsMessage.h
    igtlPlusTrackedFrameMessage.h
    PlusIgtlClientInfo.h
    vtkPlusIgtlMessageFactory.h
    vtkPlusIgtlMessageCommon.h
    vtkPlusIGTLMessageQueue.h
    )
ENDIF ()

SET(PlusOpenIGTLink_INCLUDE_DIRS 
  ${CMAKE_CURRENT_SOURCE_DIR} 
  ${CMAKE_CURRENT_BINARY_DIR} 
  CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library
SET(PlusOpenIGTLink_LIBS
  vtkPlusCommon
  OpenIGTLink
  )

GENERATE_EXPORT_DIRECTIVE_FILE(vtkPlusOpenIGTLink)
ADD_LIBRARY(vtkPlusOpenIGTLink ${PlusOpenIGTLink_SRCS} ${PlusOpenIGTLink_HDRS})
FOREACH(p IN LISTS PlusOpenIGTLink_INCLUDE_DIRS)
  target_include_directories(vtkPlusOpenIGTLink PUBLIC $<BUILD_INTERFACE:${p}>)
ENDFOREACH()
target_include_directories(vtkPlusOpenIGTLink PUBLIC $<INSTALL_INTERFACE:${PLUSLIB_INCLUDE_INSTALL}>)
TARGET_LINK_LIBRARIES(vtkPlusOpenIGTLink PUBLIC ${PlusOpenIGTLink_LIBS})

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkPlusOpenIGTLink CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkPlusOpenIGTLink vtkPlusOpenIGTLink;${PlusLib_BINARY_DIR}/src/PlusOpenIGTLink/vtkPlusOpenIGTLink.vcxproj;vtkPlusCommon CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Install
#
INSTALL(TARGETS vtkPlusOpenIGTLink EXPORT PlusLib
  RUNTIME DESTINATION "${PLUSLIB_BINARY_INSTALL}" CONFIGURATIONS Release COMPONENT RuntimeLibraries
  LIBRARY DESTINATION "${PLUSLIB_LIBRARY_INSTALL}" CONFIGURATIONS Release COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION "${PLUSLIB_ARCHIVE_INSTALL}" CONFIGURATIONS Release COMPONENT Development
  )
INSTALL(FILES ${PlusOpenIGTLink_HDRS}
  DESTINATION "${PLUSLIB_INCLUDE_INSTALL}" CONFIGURATIONS Release COMPONENT Development
  )

# --------------------------------------------------------------------------
# Testing
#
IF(BUILD_TESTING)
  SUBDIRS (Testing)
ENDIF()