# --------------------------------------------------------------------------
# Sources

SET(PlusCommon_SRCS
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

IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  SET(PlusCommon_HDRS
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
    PlusXmlUtils.h
    )
ENDIF()

SET(PlusCommon_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/IO
  CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library
SET(PlusCommon_LIBS 
  vtkCommonCore
  vtkCommonDataModel
  vtkImagingCore
  vtkIOXMLParser
  vtkIOImage
  vtkCommonSystem
  vtksys
  itksys
  ${PlusZLib}
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

IF(WIN32)
  LIST(APPEND PlusCommon_LIBS Winmm)
ENDIF(WIN32)

IF(PLUS_USE_OpenIGTLink)
  LIST(APPEND PlusCommon_LIBS OpenIGTLink)
ENDIF()

GENERATE_EXPORT_DIRECTIVE_FILE(vtkPlusCommon)
ADD_LIBRARY(vtkPlusCommon ${PlusCommon_SRCS} ${PlusCommon_HDRS})
FOREACH(p IN LISTS PlusCommon_INCLUDE_DIRS)
  target_include_directories(vtkPlusCommon PUBLIC $<BUILD_INTERFACE:${p}>)
ENDFOREACH()
target_include_directories(vtkPlusCommon PUBLIC $<INSTALL_INTERFACE:${PLUSLIB_INCLUDE_INSTALL}>)
TARGET_LINK_LIBRARIES(vtkPlusCommon ${PlusCommon_LIBS})

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
SET(vcProj_vtkPlusCommon vtkPlusCommon;${PlusLib_BINARY_DIR}/src/PlusCommon/vtkPlusCommon.vcxproj CACHE INTERNAL "" FORCE)

#--------------------------------------------------------------------------------------------
IF(PLUSBUILD_BUILD_PlusLib_TOOLS)
  ADD_EXECUTABLE(EditSequenceFile Tools/EditSequenceFile.cxx)
  SET_TARGET_PROPERTIES(EditSequenceFile PROPERTIES FOLDER Tools)
  TARGET_LINK_LIBRARIES(EditSequenceFile vtkPlusCommon)
  GENERATE_HELP_DOC(EditSequenceFile)

  INSTALL(TARGETS EditSequenceFile EXPORT PlusLib
    RUNTIME DESTINATION "${PLUSLIB_BINARY_INSTALL}" CONFIGURATIONS Release COMPONENT RuntimeExecutables
    )
ENDIF()

# --------------------------------------------------------------------------
# Install
#
INSTALL(TARGETS vtkPlusCommon EXPORT PlusLib
  RUNTIME DESTINATION "${PLUSLIB_BINARY_INSTALL}" CONFIGURATIONS Release COMPONENT RuntimeLibraries
  LIBRARY DESTINATION "${PLUSLIB_LIBRARY_INSTALL}" CONFIGURATIONS Release COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION "${PLUSLIB_ARCHIVE_INSTALL}" CONFIGURATIONS Release COMPONENT Development
  )
INSTALL(FILES ${PlusCommon_HDRS}
  DESTINATION "${PLUSLIB_INCLUDE_INSTALL}" CONFIGURATIONS Release COMPONENT Development)

# --------------------------------------------------------------------------
# Testing
#
IF(BUILD_TESTING)
  SUBDIRS(Testing)
ENDIF()