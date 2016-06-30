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
  IO/vtkPlusSequenceIOBase.cxx
  IO/vtkPlusSequenceIO.cxx
  vtkPlusRecursiveCriticalSection.cxx
  vtkPlusToolAxesActor.cxx
  )

IF(${VTK_MAJOR_VERSION} GREATER "5")
  SET( PlusCommon_SRCS ${PlusCommon_SRCS}
    IO/vtkPlusNrrdSequenceIO.cxx
  )
ENDIF()

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
    IO/vtkPlusSequenceIOBase.h
    vtkPlusRecursiveCriticalSection.h
    vtkPlusToolAxesActor.h
    PixelCodec.h
    )

  IF(${VTK_MAJOR_VERSION} GREATER "5")
    SET( PlusCommon_HDRS ${PlusCommon_HDRS}
      IO/vtkPlusNrrdSequenceIO.h
    )
  ENDIF()
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

GENERATE_EXPORT_DIRECTIVE_FILE(vtkPlusCommon)
ADD_LIBRARY(vtkPlusCommon ${PlusCommon_SRCS} ${PlusCommon_HDRS})
target_include_directories(vtkPlusCommon PUBLIC ${PlusCommon_INCLUDE_DIRS} )
TARGET_LINK_LIBRARIES(  vtkPlusCommon ${PlusCommon_LIBS} )

# --------------------------------------------------------------------------
# Set external MS project 
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkPlusCommon CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkPlusCommon vtkPlusCommon;"${PlusLib_BINARY_DIR}/src/PlusCommon/vtkPlusCommon.vcproj" CACHE INTERNAL "" FORCE)

#--------------------------------------------------------------------------------------------
IF(PLUSBUILD_BUILD_PlusLib_TOOLS)
  ADD_EXECUTABLE(EditSequenceFile Tools/EditSequenceFile.cxx )
  TARGET_LINK_LIBRARIES(EditSequenceFile vtkPlusCommon )
  GENERATE_HELP_DOC(EditSequenceFile)
  
  #--------------------------------------------------------------------------------------------
  ADD_EXECUTABLE(ModelRenderer Tools/ModelRenderer.cxx )
  TARGET_LINK_LIBRARIES(ModelRenderer vtkPlusCommon )

  INSTALL(TARGETS EditSequenceFile ModelRenderer
    RUNTIME DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeExecutables
    )
ENDIF()

# --------------------------------------------------------------------------
# Install
#

INSTALL(TARGETS vtkPlusCommon
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
