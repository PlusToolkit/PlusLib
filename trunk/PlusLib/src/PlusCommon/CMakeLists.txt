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
  )

SET (PlusCommon_HDRS)
if (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
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
    IO/vtkPlusNrrdSequenceIO.h
    IO/vtkPlusSequenceIO.h
    IO/vtkPlusSequenceIOBase.h
    vtkPlusRecursiveCriticalSection.h
    PixelCodec.h
    )
endif ()

SET (PlusCommon_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/IO
  CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library
SET (PlusCommon_LIBS 
  ${PlusCommon_LIBS}
  vtkCommonCore
  vtkCommonDataModel
  vtkImagingCore
  vtkIOXMLParser
  vtkIOImage
  vtkzlib
  vtksys
  itksys
  itkzlib
  ITKIONIFTI
  ITKIONRRD
  ITKIOGIPL
  ITKIOHDF5
  ITKIOJPEG
  ITKIOGDCM
  ITKIOBMP
  ITKIOLSM
  ITKIOTIFF
  ITKIOPNG
  ITKIOVTK
  ITKIOGE
  ITKIOMRC
  ITKIOBioRad
  ITKIOStimulate
  ITKIOMeta
  itkvnl
  itkvnl_algo
  )

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

IF(MSVC)
  # enable parallel in-target build
  # do not add to interface defines as downstream targets don't necessarily need MP
  target_compile_options(vtkPlusCommon PRIVATE /MP)
ENDIF()

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkPlusCommon CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkPlusCommon vtkPlusCommon;"${PlusLib_BINARY_DIR}/src/PlusCommon/vtkPlusCommon.vcxproj" CACHE INTERNAL "" FORCE)

#--------------------------------------------------------------------------------------------
IF(PLUSBUILD_BUILD_PlusLib_TOOLS)
  ADD_EXECUTABLE(EditSequenceFile Tools/EditSequenceFile.cxx )
  SET_TARGET_PROPERTIES(EditSequenceFile PROPERTIES FOLDER Tools)
  TARGET_LINK_LIBRARIES(EditSequenceFile vtkPlusCommon )
  GENERATE_HELP_DOC(EditSequenceFile)

  #--------------------------------------------------------------------------------------------
  ADD_EXECUTABLE(ModelRenderer Tools/ModelRenderer.cxx )
  SET_TARGET_PROPERTIES(ModelRenderer PROPERTIES FOLDER Tools)
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
