SET(CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}/Modules 
  ${CMAKE_MODULE_PATH}
  )

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
    RawImage.cpp 
    ImageGrabberThread.cpp 
    ImageGrabber.cpp 
    RawImage.cpp 
    MediaFoundationCaptureLibrary.cpp 
    MfVideoCaptureLogger.cpp 
  )
  
SET (MfVideoCaptureLibrary_HDRS ${MfVideoCaptureLibrary_HDRS}
    FormatReader.h 
    MediaFoundationVideoCaptureApi.h 
    MediaFoundationVideoDevice.h 
    MediaFoundationVideoDevices.h 
    RawImage.h 
    ImageGrabberThread.h 
    ImageGrabber.h 
    RawImage.h 
    MediaFoundationCaptureLibrary.h 
    MfVideoCaptureLogger.h 
    MfVideoCaptureLoggerMacros.h 
	)

INCLUDE_DIRECTORIES( ${MfVideoCaptureLibrary_INCLUDE_DIRS} ${WINDOWSSDK_LATEST_DIR}/Include)
SET (Utilities_INCLUDE_DIRS ${Utilities_INCLUDE_DIRS} ${MfVideoCaptureLibrary_INCLUDE_DIRS} ${WINDOWSSDK_LATEST_DIR}/Include CACHE INTERNAL "")

IF( ${CMAKE_GENERATOR} MATCHES Win64 )
  SET( MF_LIB_DIR ${WINDOWSSDK_LATEST_DIR}/Lib/x64 )
ELSE()
  SET( MF_LIB_DIR ${WINDOWSSDK_LATEST_DIR}/Lib )
ENDIF()

SET (MfVideoCaptureLibrary_LIBS
  ${MF_LIB_DIR}/Evr.lib
  ${MF_LIB_DIR}/Mf.lib 
  ${MF_LIB_DIR}/Mfplat.lib 
  ${MF_LIB_DIR}/Shlwapi.lib 
  ${MF_LIB_DIR}/Mfreadwrite.lib 
  ${MF_LIB_DIR}/mfuuid.lib 
  ${MF_LIB_DIR}/Strmiids.lib
  )

ADD_LIBRARY(MfVideoCaptureLibrary STATIC ${MfVideoCaptureLibrary_SRCS} ${MfVideoCaptureLibrary_HDRS})
TARGET_LINK_LIBRARIES( MfVideoCaptureLibrary ${MfVideoCaptureLibrary_LIBS})

# --------------------------------------------------------------------------
# Set external MS project 
SET (PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} MfVideoCaptureLibrary CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_MfVideoCaptureLibrary MfVideoCaptureLibrary;"${PlusLib_BINARY_DIR}/src/Utilities/MfVideoCaptureLibrary/MfVideoCaptureLibrary.vcproj" CACHE INTERNAL "" FORCE)

INSTALL_TARGETS(/lib MfVideoCaptureLibrary)
