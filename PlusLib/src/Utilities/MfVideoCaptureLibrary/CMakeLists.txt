PROJECT(MfVideoCaptureLibrary)

SET (MfVideoCaptureLibrary_INCLUDE_DIRS
  ${MfVideoCaptureLibrary_SOURCE_DIR}
  ${MfVideoCaptureLibrary_BINARY_DIR}
  )

SET(MfVideoCaptureLibrary_SRCS
  FormatReader.cpp
  MediaFoundationVideoCaptureApi.cpp
  MediaFoundationVideoDevice.cpp
  MediaFoundationVideoDevices.cpp
  MediaFoundationCaptureLibrary.cpp
  MfVideoCaptureLogger.cpp
  )

SET (MfVideoCaptureLibrary_HDRS ${MfVideoCaptureLibrary_HDRS}
  FormatReader.h
  MediaFoundationVideoCaptureApi.h
  MediaFoundationVideoDevice.h
  MediaFoundationVideoDevices.h
  MediaFoundationCaptureLibrary.h
  MfVideoCaptureLogger.h
  MfVideoCaptureLoggerMacros.h
  )

SET (Utilities_INCLUDE_DIRS ${Utilities_INCLUDE_DIRS} ${MfVideoCaptureLibrary_INCLUDE_DIRS} ${WINDOWS_SDK_INCLUDE_DIRS} CACHE INTERNAL "")

SET (MfVideoCaptureLibraryLibNames
  Evr
  Mf
  Mfplat
  Shlwapi
  Mfreadwrite
  mfuuid
  Strmiids
  )
set(MfVideoCaptureLibrary_LIBS)
foreach(_lib ${MfVideoCaptureLibraryLibNames})
  set(_libVarName "${_lib}Lib")
  set("${_libVarName}" "${_lib}-NOTFOUND")
  FIND_LIBRARY ("${_libVarName}" NAMES "${_lib}" PATHS ${WINDOWS_SDK_LIBRARY_DIRS} NO_DEFAULT_PATH)
  if ("${${_libVarName}}" STREQUAL "${_libVarName}-NOTFOUND")
    MESSAGE(FATAL_ERROR "Library not found in Windows SDK: ${_lib}")
  endif()
  LIST(APPEND MfVideoCaptureLibrary_LIBS "${${_libVarName}}")
endforeach()

ADD_LIBRARY(MfVideoCaptureLibrary STATIC ${MfVideoCaptureLibrary_SRCS} ${MfVideoCaptureLibrary_HDRS})
SET_TARGET_PROPERTIES(MfVideoCaptureLibrary PROPERTIES FOLDER Utilities)
TARGET_LINK_LIBRARIES( MfVideoCaptureLibrary ${MfVideoCaptureLibrary_LIBS})
target_include_directories(MfVideoCaptureLibrary PUBLIC ${MfVideoCaptureLibrary_INCLUDE_DIRS} ${WINDOWS_SDK_INCLUDE_DIRS})

# --------------------------------------------------------------------------
# Set external MS project
SET (PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} MfVideoCaptureLibrary CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_MfVideoCaptureLibrary MfVideoCaptureLibrary;"${PlusLib_BINARY_DIR}/src/Utilities/MfVideoCaptureLibrary/MfVideoCaptureLibrary.vcproj" CACHE INTERNAL "" FORCE)

INSTALL_TARGETS(/lib MfVideoCaptureLibrary)
