PROJECT(PlusCommon)

# --------------------------------------------------------------------------
# Sources
SET(${PROJECT_NAME}_SRCS
  ${PROJECT_NAME}.cxx
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
  SET(${PROJECT_NAME}_HDRS
    ${PROJECT_NAME}.h
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

SET(${PROJECT_NAME}_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/IO
  CACHE INTERNAL "" FORCE)

IF(IGSIO_BUILD_VTKVIDEOIO)
  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS ${VTKVIDEOIO_INCLUDE_DIRS})
ENDIF()
IF(VTKVIDEOIO_ENABLE_MKV)
  LIST(APPEND ${PROJECT_NAME}_SRCS IO/vtkPlusMkvSequenceIO.cxx)
  LIST(APPEND ${PROJECT_NAME}_HDRS IO/vtkPlusMkvSequenceIO.h)
ENDIF()

# If igtlioConverter was compiled as a static library, we do not need igtlio in the install configuration
GET_PROPERTY(IGTLIO_LIB_TYPE TARGET igtlioConverter PROPERTY STATIC_LIBRARY_FLAGS)
IF(NOT IGTLIO_LIB_TYPE STREQUAL "STATIC_LIBRARY")
  SET(OPENIGTLINKIO_CONFIG_CODE [=[
    IF(NOT DEFINED OpenIGTLinkIO_DIR)
      SET(OpenIGTLinkIO_DIR "@OpenIGTLinkIO_DIR@")
    ENDIF()
    FIND_PACKAGE(OpenIGTLinkIO NO_MODULE REQUIRED)]=])
ENDIF()

# --------------------------------------------------------------------------
# Build the library
SET(${PROJECT_NAME}_LIBS
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
  igtlioConverter
  )

IF(WIN32)
  LIST(APPEND ${PROJECT_NAME}_LIBS Winmm)
ENDIF(WIN32)

IF(PLUS_USE_OpenIGTLink)
  LIST(APPEND ${PROJECT_NAME}_LIBS OpenIGTLink)
ENDIF()

GENERATE_EXPORT_DIRECTIVE_FILE(vtk${PROJECT_NAME})
ADD_LIBRARY(vtk${PROJECT_NAME} ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_HDRS})
FOREACH(p IN LISTS ${PROJECT_NAME}_INCLUDE_DIRS)
  target_include_directories(vtk${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${p}>)
ENDFOREACH()
target_include_directories(vtk${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${PLUSLIB_INCLUDE_INSTALL}>)
TARGET_LINK_LIBRARIES(vtk${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_LIBS})

IF(IGSIO_BUILD_VTKVIDEOIO)
  TARGET_LINK_LIBRARIES(vtk${PROJECT_NAME} PRIVATE ${VTKVIDEOIO_LIBRARIES})
ENDIF()

IF(MSVC)
  # enable parallel in-target build
  # do not add to interface defines as downstream targets don't necessarily need MP
  target_compile_options(vtk${PROJECT_NAME} PRIVATE /MP)
ENDIF()

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtk${PROJECT_NAME} CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtk${PROJECT_NAME} vtk${PROJECT_NAME};${PlusLib_BINARY_DIR}/src/${PROJECT_NAME}/vtk${PROJECT_NAME}.vcxproj CACHE INTERNAL "" FORCE)

#--------------------------------------------------------------------------------------------
IF(PLUSBUILD_BUILD_PlusLib_TOOLS)
  ADD_EXECUTABLE(EditSequenceFile Tools/EditSequenceFile.cxx)
  SET_TARGET_PROPERTIES(EditSequenceFile PROPERTIES FOLDER Tools)
  TARGET_LINK_LIBRARIES(EditSequenceFile vtk${PROJECT_NAME})
  GENERATE_HELP_DOC(EditSequenceFile)

  INSTALL(TARGETS EditSequenceFile EXPORT PlusLib
    RUNTIME DESTINATION "${PLUSLIB_BINARY_INSTALL}" COMPONENT RuntimeExecutables
    )
ENDIF()

# --------------------------------------------------------------------------
# Install
#
PlusLibInstallLibrary(vtk${PROJECT_NAME} ${PROJECT_NAME})

# --------------------------------------------------------------------------
# Testing
#
IF(BUILD_TESTING)
  SUBDIRS(Testing)
ENDIF()