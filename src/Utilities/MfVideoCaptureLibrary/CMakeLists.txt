PROJECT(MfVideoCaptureLibrary)

SET(${PROJECT_NAME}_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

SET(MfVideoCaptureLibrary_SRCS
  FormatReader.cpp
  MediaFoundationVideoCaptureApi.cpp
  MediaFoundationVideoDevice.cpp
  MediaFoundationVideoDevices.cpp
  MediaFoundationCaptureLibrary.cpp
  MfVideoCaptureLogger.cpp
  )

if (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  SET (MfVideoCaptureLibrary_HDRS
    FormatReader.h
    MediaFoundationVideoCaptureApi.h
    MediaFoundationVideoDevice.h
    MediaFoundationVideoDevices.h
    MediaFoundationCaptureLibrary.h
    MfVideoCaptureLogger.h
    MfVideoCaptureLoggerMacros.h
    )
endif()

SET (Utilities_INCLUDE_DIRS ${Utilities_INCLUDE_DIRS} ${MfVideoCaptureLibrary_INCLUDE_DIRS} ${WINDOWS_SDK_INCLUDE_DIRS} CACHE INTERNAL "")

SET (MfVideoCaptureLibrary_LIBS
  Evr
  Mf
  Mfplat
  Shlwapi
  Mfreadwrite
  mfuuid
  Strmiids
  )

ADD_LIBRARY(MfVideoCaptureLibrary STATIC ${MfVideoCaptureLibrary_SRCS} ${MfVideoCaptureLibrary_HDRS})
SET_TARGET_PROPERTIES(MfVideoCaptureLibrary PROPERTIES FOLDER Utilities)
TARGET_LINK_LIBRARIES(MfVideoCaptureLibrary PRIVATE ${MfVideoCaptureLibrary_LIBS})
FOREACH(p IN LISTS MfVideoCaptureLibrary_INCLUDE_DIRS)
  target_include_directories(MfVideoCaptureLibrary PUBLIC $<BUILD_INTERFACE:${p}>)
ENDFOREACH()
target_include_directories(MfVideoCaptureLibrary PUBLIC $<INSTALL_INTERFACE:${PLUSLIB_INCLUDE_INSTALL}> ${WINDOWS_SDK_INCLUDE_DIRS})

# --------------------------------------------------------------------------
# Set external MS project
SET (PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} MfVideoCaptureLibrary CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_MfVideoCaptureLibrary MfVideoCaptureLibrary;${PlusLib_BINARY_DIR}/src/Utilities/MfVideoCaptureLibrary/MfVideoCaptureLibrary.vcxproj CACHE INTERNAL "" FORCE)

INSTALL(TARGETS MfVideoCaptureLibrary EXPORT PlusLib
  RUNTIME DESTINATION "${PLUSLIB_BINARY_INSTALL}" CONFIGURATIONS Release COMPONENT RuntimeLibraries
  LIBRARY DESTINATION "${PLUSLIB_LIBRARY_INSTALL}" CONFIGURATIONS Release COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION "${PLUSLIB_ARCHIVE_INSTALL}" CONFIGURATIONS Release COMPONENT Development
  )
INSTALL(FILES ${MfVideoCaptureLibrary_HDRS}
  DESTINATION "${PLUSLIB_INCLUDE_INSTALL}" CONFIGURATIONS Release COMPONENT Development
  )