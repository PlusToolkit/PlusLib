# --------------------------------------------------------------------------
# Sources

SET (PlusCommon_SRCS 
  PlusCommon.cxx
  vtkPlusAccurateTimer.cxx 
  vtkPlusLogger.cxx
  vtkPlusHTMLGenerator.cxx 
  vtkPlusConfig.cxx
  PlusMath.cxx 
  vtkPlusTransformRepository.cxx
  PlusVideoFrame.cxx
  vtkPlusTrackedFrameList.cxx
  PlusTrackedFrame.cxx
  IO/vtkPlusMetaImageSequenceIO.cxx
  IO/vtkPlusNrrdSequenceIO.cxx
  IO/vtkPlusSequenceIOBase.cxx
  IO/vtkPlusSequenceIO.cxx
  vtkPlusRecursiveCriticalSection.cxx
  vtkPlusToolAxesActor.cxx
  )

SET (PlusCommon_HDRS)
if (WIN32)  
  SET (PlusCommon_HDRS
    PlusCommon.h
    vtkPlusAccurateTimer.h
    WindowsAccurateTimer.h
    vtkPlusLogger.h
    vtkPlusHTMLGenerator.h
    vtkPlusConfig.h
    vtkPlusMacro.h
    PlusMath.h
    vtkPlusTransformRepository.h
    vtkPlusTrackedFrameList.h
    PlusTrackedFrame.h
    PlusVideoFrame.h
    PlusVideoFrame.txx
    IO/vtkPlusMetaImageSequenceIO.h
    IO/vtkPlusSequenceIO.h
    IO/vtkPlusNrrdSequenceIO.h
    IO/vtkPlusSequenceIOBase.h
    vtkPlusRecursiveCriticalSection.h
    vtkPlusToolAxesActor.h
    PixelCodec.h
    )
endif (WIN32)

SET (PlusCommon_INCLUDE_DIRS 
  ${CMAKE_CURRENT_SOURCE_DIR} 
  ${CMAKE_CURRENT_BINARY_DIR} 
  ${CMAKE_CURRENT_SOURCE_DIR}/IO
  CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library
SET (PlusCommon_LIBS ${ITK_LIBRARIES} ${VTK_LIBRARIES} ${PlusCommon_LIBS} )

if (WIN32)  
  SET (PlusCommon_LIBS ${PlusCommon_LIBS} Winmm )
endif (WIN32)

IF (PLUS_USE_OpenIGTLink)
   SET (PlusCommon_LIBS ${PlusCommon_LIBS} OpenIGTLink )
ENDIF(PLUS_USE_OpenIGTLink)   

INCLUDE_DIRECTORIES( ${PlusCommon_INCLUDE_DIRS} )
GENERATE_EXPORT_DIRECTIVE_FILE(vtkPlusCommon)
ADD_LIBRARY(vtkPlusCommon ${PlusCommon_SRCS} ${PlusCommon_HDRS})
TARGET_LINK_LIBRARIES(  vtkPlusCommon ${PlusCommon_LIBS} )

# --------------------------------------------------------------------------
# Set external MS project 
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkPlusCommon CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkPlusCommon vtkPlusCommon;"${PlusLib_BINARY_DIR}/src/PlusCommon/vtkPlusCommon.vcproj" CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Install
#
  
INSTALL(TARGETS vtkPlusCommon
  RUNTIME DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  LIBRARY DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION bin CONFIGURATIONS Release COMPONENT Development
  )

# --------------------------------------------------------------------------
# Testing
#

IF (BUILD_TESTING)
  SUBDIRS (Testing)
ENDIF (BUILD_TESTING)
